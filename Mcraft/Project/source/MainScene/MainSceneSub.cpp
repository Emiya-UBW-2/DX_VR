#include	"MainSceneSub.hpp"

#include	"Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void PauseMenuControl::Load(void) noexcept {
			auto* ButtonParts = UIs::ButtonControl::Instance();
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
			auto* ButtonParts = UIs::ButtonControl::Instance();
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
			auto* ButtonParts = UIs::ButtonControl::Instance();
			auto* SceneParts = SceneControl::Instance();
			// ポーズ
			if (SceneParts->IsPause()) {
				ButtonParts->Draw();
			}
		}
		void PauseMenuControl::Dispose(void) noexcept {
			auto* ButtonParts = UIs::ButtonControl::Instance();
			ButtonParts->Dispose();
		}

		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();

			SE->Add(SoundType::SE, (int)SoundEnum::CartFall, 6, "data/Sound/SE/gun/case.wav", false);
			SE->Add(SoundType::SE, (int)SoundEnum::MagFall, 6, "data/Sound/SE/gun/ModFall.wav", false);
			SE->Add(SoundType::SE, (int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Pin, 4, "data/Sound/SE/pin.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Throw, 4, "data/Sound/SE/throw.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::FallGrenade, 4, "data/Sound/SE/fallgrenade.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Explosion, 4, "data/Sound/SE/explosion.wav");
			for (int loop = 0; loop < 6; ++loop) {
				SE->Add(SoundType::SE, (int)SoundEnum::Cocking1_0 + loop, 4, "data/Sound/SE/gun/autoM870/" + std::to_string(loop) + ".wav");
				SE->Add(SoundType::SE, (int)SoundEnum::Cocking2_0 + loop, 4, "data/Sound/SE/gun/autoM16/" + std::to_string(loop) + ".wav");
				SE->Add(SoundType::SE, (int)SoundEnum::Cocking3_0 + loop, 4, "data/Sound/SE/gun/auto1911/" + std::to_string(loop) + ".wav");
				SE->Add(SoundType::SE, (int)SoundEnum::Cocking4_0 + loop, 4, "data/Sound/SE/gun/autoAK/" + std::to_string(loop) + ".wav");
			}
			SE->Add(SoundType::SE, (int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Hit, 3, "data/Sound/SE/hit.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::HitMe, 3, "data/Sound/SE/HitMe.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::HitGuard, 3, "data/Sound/SE/Guard.wav");
			for (int loop = 0; loop < 5; ++loop) {
				SE->Add(SoundType::SE, (int)SoundEnum::HitGround0 + loop, 2, "data/Sound/SE/gun/HitGround/" + std::to_string(loop + 1) + ".wav");
			}

			for (int loop = 0; loop < 6; ++loop) {
				SE->Add(SoundType::SE, (int)SoundEnum::Man_Hurt1 + loop, 2, "data/Sound/SE/voice/hurt_0" + std::to_string(loop + 1) + ".wav");
			}
			for (int loop = 0; loop < 8; ++loop) {
				SE->Add(SoundType::SE, (int)SoundEnum::Man_Death1 + loop, 2, "data/Sound/SE/voice/death_0" + std::to_string(loop + 1) + ".wav");
			}
			SE->Add(SoundType::SE, (int)SoundEnum::Man_contact, 10, "data/Sound/SE/voice/contact.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Man_openfire, 10, "data/Sound/SE/voice/openfire.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Man_reload, 10, "data/Sound/SE/voice/reload.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Man_takecover, 10, "data/Sound/SE/voice/takecover.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Man_breathing, 10, "data/Sound/SE/voice/breathing.wav", false);
			SE->Add(SoundType::SE, (int)SoundEnum::Man_breathend, 10, "data/Sound/SE/voice/breathend.wav", false);
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();

			SE->Get(SoundType::SE, (int)SoundEnum::CartFall)->SetLocalVolume(48);
			SE->Get(SoundType::SE, (int)SoundEnum::MagFall)->SetLocalVolume(48);
			SE->Get(SoundType::SE, (int)SoundEnum::Trigger)->SetLocalVolume(48);
			//SE->Get(SoundType::SE, (int)SoundEnum::AmmoLoad)->SetLocalVolume(48);
			SE->Get(SoundType::SE, (int)SoundEnum::Shot2)->SetLocalVolume(216);
			SE->Get(SoundType::SE, (int)SoundEnum::Shot3)->SetLocalVolume(216);
			SE->Get(SoundType::SE, (int)SoundEnum::RunFoot)->SetLocalVolume(128);
			SE->Get(SoundType::SE, (int)SoundEnum::Hit)->SetLocalVolume(255);
			SE->Get(SoundType::SE, (int)SoundEnum::HitMe)->SetLocalVolume(255);
			SE->Get(SoundType::SE, (int)SoundEnum::HitGuard)->SetLocalVolume(255);

			SE->Get(SoundType::SE, (int)SoundEnum::Man_breathing)->SetLocalVolume(192);
			SE->Get(SoundType::SE, (int)SoundEnum::Man_breathend)->SetLocalVolume(192);
			for (int loop = 0; loop < 5; ++loop) {
				SE->Get(SoundType::SE, (int)SoundEnum::HitGround0 + loop)->SetLocalVolume(92);
			}
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete(SoundType::SE, (int)SoundEnum::CartFall);
			SE->Delete(SoundType::SE, (int)SoundEnum::MagFall);
			SE->Delete(SoundType::SE, (int)SoundEnum::Trigger);
			SE->Delete(SoundType::SE, (int)SoundEnum::Pin);
			SE->Delete(SoundType::SE, (int)SoundEnum::Throw);
			SE->Delete(SoundType::SE, (int)SoundEnum::FallGrenade);
			SE->Delete(SoundType::SE, (int)SoundEnum::Explosion);
			for (int loop = 0; loop < 6; ++loop) {
				SE->Delete(SoundType::SE, (int)SoundEnum::Cocking1_0 + loop);
				SE->Delete(SoundType::SE, (int)SoundEnum::Cocking2_0 + loop);
				SE->Delete(SoundType::SE, (int)SoundEnum::Cocking3_0 + loop);
				SE->Delete(SoundType::SE, (int)SoundEnum::Cocking4_0 + loop);
			}
			SE->Delete(SoundType::SE, (int)SoundEnum::RunFoot);
			SE->Delete(SoundType::SE, (int)SoundEnum::StandupFoot);
			SE->Delete(SoundType::SE, (int)SoundEnum::Hit);
			SE->Delete(SoundType::SE, (int)SoundEnum::HitMe);
			SE->Delete(SoundType::SE, (int)SoundEnum::HitGuard);
			for (int loop = 0; loop < 5; ++loop) {
				SE->Delete(SoundType::SE, (int)SoundEnum::HitGround0 + loop);
			}
			for (int loop = 0; loop < 6; ++loop) {
				SE->Delete(SoundType::SE, (int)SoundEnum::Man_Hurt1 + loop);
			}
			for (int loop = 0; loop < 8; ++loop) {
				SE->Delete(SoundType::SE, (int)SoundEnum::Man_Death1 + loop);
			}
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_contact);
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_openfire);
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_reload);
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_takecover);
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_breathing);
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_breathend);
		}
	};
};
