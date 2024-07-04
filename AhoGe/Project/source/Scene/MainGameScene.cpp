#include	"MainGameScene.hpp"

#include "../sub.hpp"

#include "../MainScene/Player/Player.hpp"
#include "../MainScene/BackGround/BackGround.hpp"
#include "../CommonScene/Object/Object2DManager.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			MainGameScene::Load_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto Obj2DParts = Object2DManager::Instance();
			//ロード
			PlayerMngr->Init(Player_Num);
			for (int i = 1; i < Player_Num; i++) {
				auto& p = PlayerMngr->GetPlayer((PlayerID)i);
				p.SetChara(std::make_shared<CharacterObject>());
				Obj2DParts->AddObject(p.GetChara());
				p.GetChara()->SetObjType((int)Object2DType::Human);
			}
			//
			CommonBattleResource::Instance()->Load();
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto Obj2DParts = Object2DManager::Instance();
			CommonBattleResource::Instance()->Set();
			Cam2DControl::Instance()->Set();
			Vector3DX CamPos;CamPos.Set(0.f, 0.f, 64.f);
			Cam2DControl::Instance()->SetCamPos(CamPos);
			BackGroundClassBase::Instance()->Init("");
			//ポーズ
			m_IsRetire = false;
			ButtonParts->ResetSel();
			ButtonParts->AddStringButton(
				"Retire", 48, true,
				1920 - 64, 1080 - 84 - 64 * 2, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton(
				"Option", 48, true,
				1920 - 64, 1080 - 84 - 64 * 1, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton(
				"Return Game", 48, true,
				1920 - 64, 1080 - 84 - 64 * 0, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			//自機
			{
				auto& p = PlayerMngr->GetPlayer(m_MyPlayerID);
				p.SetChara(std::make_shared<CharacterObject>());
				Obj2DParts->AddObject(p.GetChara());
				p.GetChara()->SetObjType((int)Object2DType::Human);
			}
			for (int i = 1; i < Player_Num; i++) {
				Vector3DX Pos;Pos.Set((float)i*10.f, (float)i*10.f, 0.f);
				auto& p = PlayerMngr->GetPlayer((PlayerID)i);
				p.GetChara()->SetPos(Pos);
			}
			//
			m_IsEnd = false;
			m_Alpha = 1.f;
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto Obj2DParts = Object2DManager::Instance();

			auto& Chara = PlayerMngr->GetPlayer(0).GetChara();

			if (DrawParts->IsPause()) {
				if (!DrawParts->IsExit() && !DrawParts->IsRestart()) {
					Pad->SetMouseMoveEnable(false);
					Pad->ChangeGuide(
						[&]() {
							auto* KeyGuide = PadControl::Instance();
							auto* LocalizeParts = LocalizePool::Instance();
							KeyGuide->AddGuide(PADS::INTERACT, LocalizeParts->Get(9992));
							KeyGuide->AddGuide(PADS::RELOAD, LocalizeParts->Get(9991));
							KeyGuide->AddGuide(PADS::MOVE_W, "");
							KeyGuide->AddGuide(PADS::MOVE_S, "");
							KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizeParts->Get(9993));
						});
					if (!OptionWindowClass::Instance()->IsActive()) {
						ButtonParts->UpdateInput();
						//選択時の挙動
						if (ButtonParts->GetTriggerButton()) {
							switch (ButtonParts->GetSelect()) {
								case 0:
									m_IsRetire = true;
									DXDraw::Instance()->SetPause(false);
									break;
								case 1:
									OptionWindowClass::Instance()->SetActive();
									break;
								case 2:
									DXDraw::Instance()->SetPause(false);
									break;
								default:
									DXDraw::Instance()->SetPause(false);
									break;
							}
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetKey(PADS::RELOAD).trigger()) {
							SE->Get((int)SoundEnumCommon::UI_CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
							DXDraw::Instance()->SetPause(false);
						}
						//
						ButtonParts->Update();
					}
				}
				return true;
			}
			else {
				ButtonParts->ResetSel();
			}
			Pad->SetMouseMoveEnable(false);
			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					KeyGuide->AddGuide(PADS::MOVE_W, "");
					KeyGuide->AddGuide(PADS::MOVE_S, "");
					KeyGuide->AddGuide(PADS::MOVE_A, "");
					KeyGuide->AddGuide(PADS::MOVE_D, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizeParts->Get(9900));
					KeyGuide->AddGuide(PADS::RUN, LocalizeParts->Get(9902));
					KeyGuide->AddGuide(PADS::JUMP, LocalizeParts->Get(9905));
					KeyGuide->AddGuide(PADS::SHOT, LocalizeParts->Get(9906));
				});
			if (this->m_IsRetire) {
				m_IsEnd = true;
			}
			m_Alpha = std::clamp(m_Alpha + (m_IsEnd ? 1.f : -1.f) / DrawParts->GetFps(), 0.f, 1.f);
			if (m_IsEnd && m_Alpha >= 1.f) {
				return false;
			}
			//
			BackGroundClassBase::Instance()->Execute();
			//
			for (int i = 0;i < Player_Num;i++) {
				auto& p = PlayerMngr->GetPlayer(i);
				if (p.GetChara()) {
					if (i == 0) {
						InputControl MyInput;
						MyInput.SetInputStart(0.f, 0.f);
						MyInput.SetInputPADS(PADS::MOVE_W, Pad->GetKey(PADS::MOVE_W).press());
						MyInput.SetInputPADS(PADS::MOVE_S, Pad->GetKey(PADS::MOVE_S).press());
						MyInput.SetInputPADS(PADS::MOVE_A, Pad->GetKey(PADS::MOVE_A).press());
						MyInput.SetInputPADS(PADS::MOVE_D, Pad->GetKey(PADS::MOVE_D).press());
						MyInput.SetInputPADS(PADS::RUN, Pad->GetKey(PADS::RUN).press());
						MyInput.SetInputPADS(PADS::JUMP, Pad->GetKey(PADS::JUMP).press());
						MyInput.SetInputPADS(PADS::SHOT, Pad->GetKey(PADS::SHOT).press());
						//
						p.GetChara()->ExecuteInput(&MyInput);
					}
					else {

					}
				}
			}
			Obj2DParts->ExecuteObject();
			//
			Cam2DControl::Instance()->SetCamAim(Chara->GetPos());
			return true;
		}
		void			MainGameScene::Dispose_Sub(void) noexcept {
			auto* SaveDataParts = SaveDataClass::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto Obj2DParts = Object2DManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			//自機
			auto& p = PlayerMngr->GetPlayer(m_MyPlayerID);
			Obj2DParts->DelObj(p.GetChara());
			p.Dispose();
			//
			ButtonParts->Dispose();
			SaveDataParts->Save();//セーブ
			BackGroundClassBase::Instance()->Dispose();
		}
		void			MainGameScene::MainDraw_Sub(void) noexcept {
			auto Obj2DParts = Object2DManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			DrawBox(0, 0, y_r(1920), y_r(1080), Black, TRUE);

			BackGroundClassBase::Instance()->BG_Draw();

			BackGroundClassBase::Instance()->Draw();

			Obj2DParts->Draw();
			BackGroundClassBase::Instance()->DrawFront();
		}
		void			MainGameScene::DrawUI_Base_Sub(void) noexcept {
			auto* Fonts = FontPool::Instance();
			//
			Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_UI(96),
																FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																y_UI(64), y_UI(64), White, Black, "MainScene");
			//
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * m_Alpha), 0, 255));
				DrawBox(0, 0, y_r(1920), y_r(1080), Black, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}

		void			MainGameScene::DrawUI_In_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			//ポーズ
			if (DrawParts->IsPause() && (!DrawParts->IsExit() && !DrawParts->IsRestart())) {
				ButtonParts->Draw();
			}
		}
		//使い回しオブジェ系
		void			MainGameScene::Dispose_Load_Sub(void) noexcept {
			auto Obj2DParts = Object2DManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			PlayerMngr->Dispose();
			Obj2DParts->DeleteAll();
			CommonBattleResource::Instance()->Dispose();
		}
		//
	};
};
