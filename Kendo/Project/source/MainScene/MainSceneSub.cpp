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
			auto* OptionWindowParts = OptionWindowClass::Instance();
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
					if (!OptionWindowParts->IsActive()) {
						ButtonParts->UpdateInput();
						// 選択時の挙動
						if (ButtonParts->GetTriggerButton()) {
							switch (ButtonParts->GetSelect()) {
							case 0:
								this->m_IsRetire = true;
								DrawParts->SetPause(false);
								break;
							case 1:
								OptionWindowParts->SetActive();
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
		void FadeControl::SetFadeIn(float Per) noexcept {
			this->m_IsBlackOut = false;
			this->m_BlackOutAlpha = 1.f;
			this->m_BlackOutPower = Per;
		}
		void FadeControl::SetFadeOut(float Per) noexcept {
			this->m_IsBlackOut = true;
			this->m_BlackOutAlpha = 0.f;
			this->m_BlackOutPower = Per;
		}
		void FadeControl::UpdateFade(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			this->m_BlackOutAlpha = std::clamp(this->m_BlackOutAlpha + (this->m_IsBlackOut ? 1.f : -1.f) * DXLib_refParts->GetDeltaTime() * this->m_BlackOutPower, 0.f, 1.f);
		}
		void FadeControl::DrawFade(void) const noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			if (this->m_BlackOutAlpha > 0.f) {
				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * this->m_BlackOutAlpha), 0, 255));
				WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIXMax(), DrawParts->GetUIYMax(), Black, TRUE);
				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
	};
};
