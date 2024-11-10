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
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* OptionWindowParts = OptionWindowClass::Instance();
			auto* SceneParts = SceneControl::Instance();
			if (SceneParts->IsPause()) {
				if (!SceneParts->IsExit() && !SceneParts->IsRestart()) {
					Pad->SetMouseMoveEnable(false);
					KeyGuideParts->ChangeGuide(
						[]() {
							auto* KeyGuideParts = KeyGuide::Instance();
							auto* Pad = PadControl::Instance();
							auto* LocalizeParts = LocalizePool::Instance();
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::INTERACT).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9992));
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::RELOAD).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9991));
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_W).GetAssign(), Pad->GetControlType()), "");
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_S).GetAssign(), Pad->GetControlType()), "");
							KeyGuideParts->AddGuide(KeyGuideParts->GetIDtoOffset(Pad->GetPadsInfo(PADS::MOVE_STICK).GetAssign(), Pad->GetControlType()), LocalizeParts->Get(9993));
						});
					if (!OptionWindowParts->IsActive()) {
						ButtonParts->UpdateInput();
						// 選択時の挙動
						if (ButtonParts->GetTriggerButton()) {
							switch (ButtonParts->GetSelect()) {
							case 0:
								this->m_IsRetire = true;
								SceneParts->ChangePause(false);
								break;
							case 1:
								OptionWindowParts->SetActive();
								break;
							case 2:
								SceneParts->ChangePause(false);
								break;
							default:
								SceneParts->ChangePause(false);
								break;
							}
							SE->Get(static_cast<int>(SoundEnumCommon::UI_OK)).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetPadsInfo(PADS::RELOAD).GetKey().trigger()) {
							SE->Get(static_cast<int>(SoundEnumCommon::UI_CANCEL)).Play(0, DX_PLAYTYPE_BACK, TRUE);
							SceneParts->ChangePause(false);
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
			auto* ButtonParts = ButtonControl::Instance();
			auto* SceneParts = SceneControl::Instance();
			// ポーズ
			if (SceneParts->IsPause() && (!SceneParts->IsExit() && !SceneParts->IsRestart())) {
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
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			if (this->m_BlackOutAlpha > 0.f) {
				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * this->m_BlackOutAlpha), 0, 255));
				WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, BaseScreenWidth, BaseScreenHeight, Black, TRUE);
				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
	};
};
