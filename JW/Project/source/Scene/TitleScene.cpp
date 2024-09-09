#include	"TitleScene.hpp"
#include "../sub.hpp"

#include"../Header.hpp"

#include "../MainScene/Player/Player.hpp"
#include "../MainScene/Object/Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Load_Sub(void) noexcept {
			//ロード
				auto* PlayerMngr = PlayerManager::Instance();
				//BG
				GunAnimManager::Instance()->Load("data/CharaAnime/");
				//
				PlayerMngr->Init(1, 0);
		}
		void			TitleScene::Set_Sub(void) noexcept {
			select = 0;
			m_MouseSelMode = false;
			auto* SaveDataParts = SaveDataClass::Instance();
			auto* DrawParts = DXDraw::Instance();
			m_HardModeActive = (SaveDataParts->GetParam("UnlockHardMode") == 1);
			GameFadeIn = 1.f;
			GameStart = 0.f;

			m_TitleImage = GraphHandle::Load("data/UI/Title.png");
			m_SelectBackImage = GraphHandle::Load("CommonData/UI/select.png");
			for (auto& y : ButtonSel) {
				y.LoadCommon(&m_SelectBackImage);
			}

			Vector3DX LightVec = Vector3DX::vget(0.f, -1.f, 0.5f);
			DrawParts->SetAmbientLight(LightVec, GetColorF(1.f, 1.f, 1.f, 0.0f));
			//
			DeleteLightHandleAll();
			SetLightEnable(TRUE);
			ChangeLightTypeDir(LightVec.get());
			SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
			SetLightSpcColor(GetColorF(0.1f, 0.1f, 0.1f, 0.f));
			SetLightAmbColor(GetColorF(0.1f, 0.1f, 0.1f, 1.f));

			ButtonSel.at(0).Load_String(SelectName[0], (64), true);
			ButtonSel.at(0).Set((1920 / 2), (1080 - 96), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM);
			ButtonSel.at(1).Load_String(SelectName[1], (48), true);
			ButtonSel.at(1).Set((1920 - 64), (1080 - 84 - 64 * 2), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonSel.at(2).Load_String(SelectName[2], (48), true);
			ButtonSel.at(2).Set((1920 - 64), (1080 - 84 - 64 * 1), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);

			ButtonSel.at(3).Load_String(SelectName[3], (48), m_HardModeActive);
			ButtonSel.at(3).Set((1920 - 64), (1080 - 84 - 64 * 0), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);

			ButtonSel.at(4).Load_Icon("CommonData/UI/setting.png", true);
			ButtonSel.at(4).Set((1920 - 96 - 64), (64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			ButtonSel.at(5).Load_Icon("CommonData/UI/credit.png", true);
			ButtonSel.at(5).Set((1920 - 64), (64), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);
			//クレジット
			int mdata = FileRead_open("data/Credit.txt", FALSE);
			m_CreditCoulm = 0;
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				auto ALL = getparams::Getstr(mdata);
				if (ALL.find('=') != std::string::npos) {
					auto LEFT = getparams::getleft(ALL);
					auto RIGHT = getparams::getright(ALL);
					sprintfDx(m_CreditStr.at(m_CreditCoulm).first, LEFT.c_str());
					sprintfDx(m_CreditStr.at(m_CreditCoulm).second, RIGHT.c_str());
				}
				else {
					sprintfDx(m_CreditStr.at(m_CreditCoulm).first, ALL.c_str());
					sprintfDx(m_CreditStr.at(m_CreditCoulm).second, "");
				}
				m_CreditCoulm++;
			}
			FileRead_close(mdata);
			//
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();

			BattleResourceMngr->LoadChara("Suit", (PlayerID)(0));
			//
			GunsModify::LoadSlots("Save/gundata.svf");
			{
				BattleResourceMngr->LoadCharaGun("G17Gen3", (PlayerID)(0), 0);
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer((PlayerID)(0))->GetChara();
				auto& g = c->GetGunPtr(0);
				c->SetGunPtr(0, g);
				GunsModify::CreateSelData(c->GetGunPtr(0), true);
				c->GetGunPtr(0)->Init_Gun();
			}
			{
				BattleResourceMngr->LoadCharaGun(ULT_GUNName[(int)GunsModify::GetULTSelect()], (PlayerID)(0), 1);
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer((PlayerID)(0))->GetChara();
				auto& g = c->GetGunPtr(1);
				c->SetGunPtr(1, g);
				GunsModify::CreateSelData(c->GetGunPtr(1), false);
				c->GetGunPtr(1)->Init_Gun();
			}
			{
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer((PlayerID)(0))->GetChara();
				c->ValueSet((PlayerID)(0), false, CharaTypeID::Team);

				Vector3DX pos_t = Vector3DX::vget(-0.6f, 0.0f, 0.2f)*Scale_Rate;
				float rad_t = 0.f;
				c->MovePoint(deg2rad(0.f), rad_t, pos_t, 0);
				c->Heal(100, true);
			}

			auto* BGM = BGMPool::Instance();
			if (!BGM->Get(0)->Check()) {
				BGM->Get(0)->Play(DX_PLAYTYPE_LOOP, TRUE);
			}
			BGM->Get(0)->SetVol_Local(255);

			m_Yrad = deg2rad(45);
			m_Xrad = 0.f;
			m_Yrad_R = m_Yrad;
			m_Xrad_R = m_Xrad;
		}
		bool			TitleScene::Update_Sub(void) noexcept {
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}
			auto* Pad = PadControl::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* PopUpParts = PopUp::Instance();

			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0)->GetChara();

			Pad->SetMouseMoveEnable(false);
			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					KeyGuide->AddGuide(PADS::MOVE_W, "");
					KeyGuide->AddGuide(PADS::MOVE_S, "");
					KeyGuide->AddGuide(PADS::MOVE_A, "");
					KeyGuide->AddGuide(PADS::MOVE_D, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9993));
					KeyGuide->AddGuide(PADS::INTERACT, LocalizePool::Instance()->Get(9992));
				}
			);
			if (!PopUpParts->IsActivePop()) {
				if ((GameFadeIn == 0.f) && (GameStart == 0.f)) {
					int preselect = select;
					bool preMouseSel = m_MouseSelMode;
					if (Pad->GetKey(PADS::MOVE_W).trigger() || Pad->GetKey(PADS::MOVE_A).trigger()) {
						if (select != -1) {
							--select;
							if (select < 0) { select = (int)ButtonSel.size() - 1; }
						}
						else {
							select = 0;
						}
						m_MouseSelMode = false;
					}
					if (Pad->GetKey(PADS::MOVE_S).trigger() || Pad->GetKey(PADS::MOVE_D).trigger()) {
						if (select != -1) {
							++select;
							if (select > (int)ButtonSel.size() - 1) { select = 0; }
						}
						else {
							select = 0;
						}
						m_MouseSelMode = false;
					}

					if (m_MouseSelMode) {
						select = -1;
					}
					//
					for (auto& y : ButtonSel) {
						if (y.GetInto()) {
							m_MouseSelMode = true;
							select = static_cast<int>(&y - &ButtonSel.front());
						}
					}
					if ((select != -1) && (m_MouseSelMode ? Pad->GetMouseClick().trigger() : Pad->GetKey(PADS::INTERACT).trigger())) {
						switch (select) {
						case 0:
						case 1:
						case 2:
							GameStart += 0.0001f;
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
							break;
						case 3:
							if (m_HardModeActive) {
								GameStart += 0.0001f;
								SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
							}
							else {
								SE->Get((int)SoundEnumCommon::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
							}
							break;
						case 4:
							OptionWindowClass::Instance()->SetActive();
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
							break;
						case 5:
							PopUpParts->Add(LocalizePool::Instance()->Get(120), DrawParts->GetUIY(720), DrawParts->GetUIY(840),
								[&](int xmin, int ymin, int xmax, int, bool) {
									auto* DrawParts = DXDraw::Instance();
									auto* WindowParts = WindowSystem::DrawControl::Instance();
									int xp1, yp1;
									xp1 = xmin + DrawParts->GetUIY(24);
									yp1 = ymin + LineHeight;
									int Height = DrawParts->GetUIY(12);
									for (int i = 0; i < m_CreditCoulm; i++) {
										int xpos = xp1 + DrawParts->GetUIY(6);
										int ypos = yp1 + (yp1 + Height - yp1) / 2;
										WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
											Height, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
											xpos, ypos, White, Black, m_CreditStr.at(i).first);

										xpos = xmax - DrawParts->GetUIY(24);
										WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
											Height, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
											xpos, ypos, White, Black, m_CreditStr.at(i).second);
										yp1 += Height;
									}
								},
								[&]() {},
								[&]() {},
								true
							);
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
							break;
						default:
							break;
						}
					}
					if (preselect != select || preMouseSel != m_MouseSelMode) {
						if (select != -1) {
							for (auto& y : ButtonSel) {
								y.SetNone();
							}
							ButtonSel.at(select).SetFocus();
							SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						else {
							for (auto& y : ButtonSel) {
								y.SetReady();
							}
						}

						//
						if (select == 0) {
							Chara->SetAimOrADS();
							Chara->SelectGun(0);
						}
						else if (select == 1 || select == 2) {
							Chara->SetADS();
							//Chara->GetGunPtrNow()->ChangeSightSel();
						}
						else if ((m_HardModeActive && (select == 3))) {
							Chara->SetAimOrADS();
							Chara->SelectGun(1);
						}
						else {
							if (Chara->IsGun0Select()) {
								Chara->SetAimOrADS();
							}
							else {
								Chara->SetReady();
							}
						}
					}
				}
			}
			if (Chara->IsGun0Select()) {
				Chara->SetAimOrADS();
			}
			//
			if (select == 1 || select == 2) {
				Chara->SetADS();
			}
			//
			ObjMngr->ExecuteObject();
			ObjMngr->LateExecuteObject();

			Easing(&m_Xrad, deg2rad(0.f + GetRandf(10.f)), 0.95f, EasingType::OutExpo);
			Easing(&m_Yrad, deg2rad(45.f + GetRandf(10.f)), 0.95f, EasingType::OutExpo);
			Easing(&m_Xrad_R, m_Xrad, 0.95f, EasingType::OutExpo);
			Easing(&m_Yrad_R, m_Yrad, 0.95f, EasingType::OutExpo);

			Vector3DX Pos = Vector3DX::vget(0.f, 1.25f, 0.0f)*Scale_Rate;
			DrawParts->SetMainCamera().SetCamPos(
				Pos + Matrix4x4DX::Vtrans(Vector3DX::forward()*(3.f*Scale_Rate), Matrix4x4DX::RotAxis(Vector3DX::right(), m_Xrad_R)*Matrix4x4DX::RotAxis(Vector3DX::up(), m_Yrad_R + DX_PI_F)),
				Pos,
				Vector3DX::vget(0.f, 1.f, 0.f));
			float far_t = 20.f*Scale_Rate;
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(45), 0.5f*Scale_Rate, far_t);
			PostPassEffect::Instance()->Set_DoFNearFar(1.f * Scale_Rate, far_t / 2, 0.5f*Scale_Rate, far_t);

			//
			for (auto& y : ButtonSel) {
				y.Update();
			}
			GameFadeIn = std::max(GameFadeIn - 1.f / DrawParts->GetFps() / 0.5f, 0.f);
			if (GameStart != 0.f) { GameStart += 1.f / DrawParts->GetFps() / 0.5f; }
			return  (GameStart < 1.f);
		}
		void			TitleScene::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			{
				auto* Ptr = &PlayerMngr->GetPlayer((PlayerID)(0))->GetChara();
				ObjMngr->DelObj(Ptr);
			}
			PlayerMngr->GetPlayer(0)->Dispose();
			GunsModify::DisposeSlots();
			//
			m_SelectBackImage.Dispose();
			for (auto& y : ButtonSel) {
				y.Dispose();
			}
			//
			m_TitleImage.Dispose();
			auto* SaveDataParts = SaveDataClass::Instance();
			SaveDataParts->Save();//セーブ
			//次シーン決定
			SetNextSelect(select);
		}
		void			TitleScene::BG_Draw_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			DrawBox(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), Gray65, TRUE);
		}

		void			TitleScene::ShadowDraw_Sub(void) noexcept {
			ObjectManager::Instance()->Draw_Shadow();
		}
		void			TitleScene::MainDraw_Sub(void) noexcept {
			ObjectManager::Instance()->Draw();
		}
		//
		void			TitleScene::DrawUI_Base_Sub(void) noexcept {
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* DrawParts = DXDraw::Instance();
			//
			WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_TitleImage, DrawParts->GetUIY(64), DrawParts->GetUIY(64), DrawParts->GetUIY(64 + 369), DrawParts->GetUIY(64 + 207), true);
			//
			{
				WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(18),
																	  FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
																	  DrawParts->GetUIY(64 + 369), DrawParts->GetUIY(64 + 207), White, Black, "Ver 1.1.1");
			}
			//
			for (auto& y : ButtonSel) {
				y.Draw();
			}
			//
			if ((select != -1) && !PopUpParts->IsActivePop()) {
				WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
					DrawParts->GetUIY(18),
																	  FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM,
																	  DrawParts->GetUIY(32), DrawParts->GetUIY(1080 - 32 - 32), White, Black, LocalizePool::Instance()->Get(9020 + select));
			}
			//
			{
				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f*std::clamp(GameFadeIn, 0.f, 1.f)), 0, 255));
				WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), Black, TRUE);

				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f*std::clamp(GameStart, 0.f, 1.f)), 0, 255));
				WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), White, TRUE);

				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
		//使い回しオブジェ系
		void			TitleScene::Dispose_Load_Sub(void) noexcept {
				PlayerManager::Instance()->Dispose();
				ObjectManager::Instance()->DeleteAll();
		}
		//
	};
};
