#include	"MainSceneSub.hpp"

#include	"Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void PauseMenuControl::LoadPause(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->ResetSel();
			ButtonParts->AddStringButton("Retire", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Option", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 1, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Return Game", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 0, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
		}
		void PauseMenuControl::SetPause(void) noexcept {
			this->m_IsRetire = false;
		}
		void PauseMenuControl::UpdatePause(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* SceneParts = SceneControl::Instance();
			if (SceneParts->IsPause()) {
				if (!WindowSizeParts->IsExit() && !WindowSizeParts->IsRestart()) {
					Pad->SetMouseMoveEnable(false);
					auto* KeyGuideParts = KeyGuide::Instance();
					KeyGuideParts->ChangeGuide(
						[]() {
							auto* KeyGuideParts = KeyGuide::Instance();
							auto* Pad = PadControl::Instance();
							auto* LocalizeParts = LocalizePool::Instance();
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9992));
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD), LocalizeParts->Get(9991));
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
							KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizeParts->Get(9993));
						});
					if (!OptionPopup::Instance()->IsActive()) {
						ButtonParts->UpdateInput();
						// 選択時の挙動
						if (ButtonParts->GetTriggerButton()) {
							switch (ButtonParts->GetSelect()) {
							case 0:
								this->m_IsRetire = true;
								WindowSizeParts->SetPause(false);
								break;
							case 1:
								OptionPopup::Instance()->SetActive();
								break;
							case 2:
								WindowSizeParts->SetPause(false);
								break;
							default:
								WindowSizeParts->SetPause(false);
								break;
							}
							SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetPadsInfo(Controls::PADS::RELOAD).GetKey().trigger()) {
							SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_CANCEL))->Play(DX_PLAYTYPE_BACK, TRUE);
							WindowSizeParts->SetPause(false);
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
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* SceneParts = SceneControl::Instance();
			// ポーズ
			if (SceneParts->IsPause() && (!WindowSizeParts->IsExit() && !WindowSizeParts->IsRestart())) {
				ButtonParts->Draw();
			}
		}
		void PauseMenuControl::DisposePause(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->Dispose();
		}
		//
		void FadeControl::SetFade(void) noexcept {
			this->m_IsBlackOut = false;
			this->m_BlackOutAlpha = 1.f;
		}
		void FadeControl::UpdateFade(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			this->m_BlackOutAlpha = std::clamp(this->m_BlackOutAlpha + (this->m_IsBlackOut ? 1.f : -1.f) / DXLib_refParts->GetFps() / 0.5f, 0.f, 1.f);
		}
		void FadeControl::DrawFade(void) const noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * this->m_BlackOutAlpha), 0, 255));
			WindowSystem::DrawControl::Instance()->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, WindowSizeParts->GetUIXMax(), WindowSizeParts->GetUIYMax(), Black, TRUE);
			WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
		}
};
};
