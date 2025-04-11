#include	"MainSceneSub.hpp"

#include	"Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void PauseMenuControl::Load(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->ResetSelect();
			ButtonParts->AddStringButton("Retire", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Option", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 1, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Return Game", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 0, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
		}
		void PauseMenuControl::Init(void) noexcept {
			this->m_IsRetire = false;
		}
		void PauseMenuControl::Update(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* ButtonParts = ButtonControl::Instance();
			auto* SceneParts = SceneControl::Instance();
			if (SceneParts->IsPause()) {
				auto* PopUpParts = PopUp::Instance();
				if (!PopUpParts->IsActivePop()) {
					Pad->SetMouseMoveEnable(false);
					auto* KeyGuideParts = KeyGuide::Instance();
					KeyGuideParts->ChangeGuide(
						[]() {
							auto* KeyGuideParts = KeyGuide::Instance();
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
								SceneParts->ChangePause(false);
								break;
							case 1:
								OptionPopup::Instance()->SetActive();
								break;
							case 2:
								SceneParts->ChangePause(false);
								break;
							default:
								SceneParts->ChangePause(false);
								break;
							}
							SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, true);
						}
						if (Pad->GetPadsInfo(Controls::PADS::RELOAD).GetKey().trigger()) {
							SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_CANCEL))->Play(DX_PLAYTYPE_BACK, true);
							SceneParts->ChangePause(false);
						}
						// 
						ButtonParts->Update();
					}
				}
			}
			else {
				ButtonParts->ResetSelect();
			}
		}
		void PauseMenuControl::Draw(void) const noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			auto* SceneParts = SceneControl::Instance();
			// ポーズ
			if (SceneParts->IsPause()) {
				ButtonParts->Draw();
			}
		}
		void PauseMenuControl::Dispose(void) noexcept {
			auto* ButtonParts = ButtonControl::Instance();
			ButtonParts->Dispose();
		}
};
};
