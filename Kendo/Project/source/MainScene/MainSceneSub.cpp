#include	"MainSceneSub.hpp"

#include	"Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void PauseMenuControl::LoadPause(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->ResetSel();
			ButtonParts->AddStringButton("Retire", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 2, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Option", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 1, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Return Game", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 0, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
		}
		void PauseMenuControl::SetPause(void) noexcept {
			this->m_IsRetire = false;
		}
		void PauseMenuControl::UpdatePause(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			if (DrawParts->IsPause()) {
				if (!DrawParts->IsExit() && !DrawParts->IsRestart()) {
					Pad->SetMouseMoveEnable(false);
					Pad->ChangeGuide(
						[]() {
							auto* Pad = PadControl::Instance();
							auto* LocalizeParts = LocalizePool::Instance();
							Pad->AddGuide(PADS::INTERACT, LocalizeParts->Get(9992));
							Pad->AddGuide(PADS::RELOAD, LocalizeParts->Get(9991));
							Pad->AddGuide(PADS::MOVE_W, "");
							Pad->AddGuide(PADS::MOVE_S, "");
							Pad->AddGuide(PADS::MOVE_STICK, LocalizeParts->Get(9993));
						});
					if (!OptionWindowClass::Instance()->IsActive()) {
						ButtonParts->UpdateInput();
						// 選択時の挙動
						if (ButtonParts->GetTriggerButton()) {
							switch (ButtonParts->GetSelect()) {
							case 0:
								this->m_IsRetire = true;
								DrawParts->SetPause(false);
								break;
							case 1:
								OptionWindowClass::Instance()->SetActive();
								break;
							case 2:
								DrawParts->SetPause(false);
								break;
							default:
								DrawParts->SetPause(false);
								break;
							}
							SE->Get(static_cast<int>(SoundEnumCommon::UI_OK)).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetKey(PADS::RELOAD).trigger()) {
							SE->Get(static_cast<int>(SoundEnumCommon::UI_CANCEL)).Play(0, DX_PLAYTYPE_BACK, TRUE);
							DrawParts->SetPause(false);
						}
						// 
						ButtonParts->Update();
					}
				}
			}
			else {
				ButtonParts->ResetSel();
			}
		}
		void PauseMenuControl::DrawPause(void) const noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			// ポーズ
			if (DrawParts->IsPause() && (!DrawParts->IsExit() && !DrawParts->IsRestart())) {
				ButtonParts->Draw();
			}
		}
		void PauseMenuControl::DisposePause(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->Dispose();
		}
		//
		void FadeControl::SetFadeIn(void) noexcept {
			this->m_IsBlackOut = false;
			this->m_BlackOutAlpha = 1.f;
		}
		void FadeControl::SetFadeOut(void) noexcept {
			this->m_IsBlackOut = true;
			this->m_BlackOutAlpha = 0.f;
		}
		void FadeControl::UpdateFade(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			this->m_BlackOutAlpha = std::clamp(this->m_BlackOutAlpha + (this->m_IsBlackOut ? 1.f : -1.f) / DrawParts->GetFps() / 0.5f, 0.f, 1.f);
		}
		void FadeControl::DrawFade(void) const noexcept {
			auto* DrawParts = DXDraw::Instance();
			if (this->m_BlackOutAlpha > 0.f) {
				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * this->m_BlackOutAlpha), 0, 255));
				WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIXMax(), DrawParts->GetUIYMax(), Black, TRUE);
				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
	};
};
