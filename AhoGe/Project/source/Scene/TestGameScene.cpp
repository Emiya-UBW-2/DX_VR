#include	"TestGameScene.hpp"

#include "../sub.hpp"

#include "../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			TestGameScene::Load_Sub(void) noexcept {
			//ロード
		}
		void			TestGameScene::Set_Sub(void) noexcept {
			m_MainLoopPauseControl.Init();
			m_Range = 10.f;
			m_SelAlpha = 0.f;
			m_IsEnd = false;
			m_Alpha = 1.f;
		}
		bool			TestGameScene::Update_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			if (GetIsFirstLoop()) {
				m_Range = 10.f;
				m_SelAlpha = 2.f;
			}

			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					if (DXDraw::Instance()->IsPause()) {
						KeyGuide->AddGuide(PADS::INTERACT, LocalizePool::Instance()->Get(9992));
						KeyGuide->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9991));
						KeyGuide->AddGuide(PADS::MOVE_W, "");
						KeyGuide->AddGuide(PADS::MOVE_S, "");
						KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9993));
					}
					else {
						KeyGuide->AddGuide(PADS::MOVE_W, "");
						KeyGuide->AddGuide(PADS::MOVE_S, "");
						KeyGuide->AddGuide(PADS::MOVE_A, "");
						KeyGuide->AddGuide(PADS::MOVE_D, "");
						KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9900));
						KeyGuide->AddGuide(PADS::RUN, LocalizePool::Instance()->Get(9902));
						KeyGuide->AddGuide(PADS::JUMP, LocalizePool::Instance()->Get(9905));
						KeyGuide->AddGuide(PADS::SHOT, LocalizePool::Instance()->Get(9906));
					}
				});


			if (DXDraw::Instance()->IsPause()) {
				if (!DrawParts->IsExit() && !DrawParts->IsRestart()) {
					m_MainLoopPauseControl.Execute();
				}
				return true;
			}
			else {
				m_MainLoopPauseControl.Reset();
				if (m_MainLoopPauseControl.GetIsRetireSelected()) {
					return false;
				}
			}


			m_SelAlpha = std::max(m_SelAlpha - 1.f / DrawParts->GetFps(), 0.f);


			if (Pad->GetKey(PADS::RELOAD).trigger()) {
				m_IsEnd = true;
				SE->Get((int)SoundEnumCommon::UI_CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}

			Pad->SetMouseMoveEnable(false);


			m_Alpha = std::clamp(m_Alpha + (m_IsEnd ? 1.f : -1.f) / 60, 0.f, 1.f);//一旦DrawParts->GetFps()使わない
			if (m_IsEnd && m_Alpha == 1.f) {
				return false;
			}
			return true;
		}
		void			TestGameScene::Dispose_Sub(void) noexcept {
			m_MainLoopPauseControl.Dispose();
			auto* SaveDataParts = SaveDataClass::Instance();
			SaveDataParts->Save();//セーブ
		}
		void			TestGameScene::MainDraw_Sub(void) noexcept {
			DrawBox(0, 0, y_r(1920), y_r(1080), Gray75, TRUE);
		}

		void			TestGameScene::DrawUI_Base_Sub(void) noexcept {
			auto* Fonts = FontPool::Instance();
			//
			Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_UI(96),
																FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																y_UI(64), y_UI(64), White, Black, "TestScene");
			//
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*m_Alpha), 0, 255));
				DrawBox(0, 0, y_r(1920), y_r(1080), Black, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}

		void			TestGameScene::DrawUI_In_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//ポーズ
			if (DrawParts->IsPause() && (!DrawParts->IsExit() && !DrawParts->IsRestart())) {
				m_MainLoopPauseControl.Draw();
			}
		}
		//使い回しオブジェ系
		void			TestGameScene::Dispose_Load_Sub(void) noexcept {
		}
		//
	};
};
