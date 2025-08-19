#include	"MainSceneSub.hpp"

#include	"Player/Player.hpp"

#include	"../MainScene/Object/ItemObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			PauseMenuControl::Load(void) noexcept {
			auto* ButtonParts = UIs::ButtonControl::Instance();
			ButtonParts->ResetSelect();
			ButtonParts->AddStringButton("Return Title", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Option", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 1, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
			ButtonParts->AddStringButton("Return Game", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 0, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
		}
		void			PauseMenuControl::Init(void) noexcept {
			this->m_IsRetire = false;
		}
		void			PauseMenuControl::Update(void) noexcept {
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
		void			PauseMenuControl::Draw(void) const noexcept {
			if (SceneControl::Instance()->IsPause()) {
				UIs::ButtonControl::Instance()->Draw();
			}
		}
		void			PauseMenuControl::Dispose(void) noexcept {
			UIs::ButtonControl::Instance()->Dispose();
		}

		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();

			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Heli), 1, "data/Sound/SE/Heli.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Heli2), 1, "data/Sound/SE/Heli.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Envi), 1, "data/Sound/SE/Envi.wav", false);

			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::CartFall), 6, "data/Sound/SE/gun/case.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::MagFall), 6, "data/Sound/SE/gun/ModFall.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Trigger), 1, "data/Sound/SE/gun/trigger.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::rolling_rocket), 1, "data/Sound/SE/rolling_rocket.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Tinnitus), 2, "data/Sound/SE/Tinnitus.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Pin), 4, "data/Sound/SE/pin.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Throw), 4, "data/Sound/SE/throw.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::FallGrenade), 4, "data/Sound/SE/fallgrenade.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Explosion), 4, "data/Sound/SE/explosion.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::GetItem), 4, "data/Sound/SE/getitem.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::PutItem), 4, "data/Sound/SE/putitem.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Delivery), 2, "data/Sound/SE/Delivery.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::announce), 2, "data/Sound/SE/announce.wav", false);
			//
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::resultEnv), 2, "data/Sound/SE/resultEnv.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::resultbutton), 2, "data/Sound/SE/resultbutton.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::resultEnd), 2, "data/Sound/SE/resultEnd.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::taskstart), 2, "data/Sound/SE/taskstart.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::alarm), 2, "data/Sound/SE/alarm.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::returnButton), 1, "data/Sound/SE/returnButton.wav", false);

			for (int loop = 0; loop < 6; ++loop) {
				SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Cocking1_0) + loop, 4, "data/Sound/SE/gun/autoM870/" + std::to_string(loop) + ".wav");
				SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Cocking2_0) + loop, 4, "data/Sound/SE/gun/autoM16/" + std::to_string(loop) + ".wav");
				SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Cocking3_0) + loop, 4, "data/Sound/SE/gun/auto1911/" + std::to_string(loop) + ".wav");
				SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Cocking4_0) + loop, 4, "data/Sound/SE/gun/autoAK/" + std::to_string(loop) + ".wav");
			}
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::RunFoot), 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot), 3, "data/Sound/SE/move/standup.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Hit), 3, "data/Sound/SE/hit.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::HitMe), 3, "data/Sound/SE/HitMe.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::HitGuard), 3, "data/Sound/SE/Guard.wav");
			for (int loop = 0; loop < 5; ++loop) {
				SE->Add(SoundType::SE, static_cast<int>(SoundEnum::HitGround0) + loop, 2, "data/Sound/SE/gun/HitGround/" + std::to_string(loop + 1) + ".wav");
			}

			for (int loop = 0; loop < 6; ++loop) {
				SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Man_Hurt1) + loop, 2, "data/Sound/SE/voice/hurt_0" + std::to_string(loop + 1) + ".wav");
			}
			for (int loop = 0; loop < 8; ++loop) {
				SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Man_Death1) + loop, 2, "data/Sound/SE/voice/death_0" + std::to_string(loop + 1) + ".wav");
			}
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Man_contact), 10, "data/Sound/SE/voice/contact.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Man_openfire), 10, "data/Sound/SE/voice/openfire.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Man_reload), 10, "data/Sound/SE/voice/reload.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Man_takecover), 10, "data/Sound/SE/voice/takecover.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing), 5, "data/Sound/SE/voice/breathing.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Man_breathend), 5, "data/Sound/SE/voice/breathend.wav", false);

			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V000), 1, "data/Sound/VOICE/vo000.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V001), 1, "data/Sound/VOICE/vo001.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V002), 1, "data/Sound/VOICE/vo002.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V003), 1, "data/Sound/VOICE/vo003.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V004), 1, "data/Sound/VOICE/vo004.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V005), 1, "data/Sound/VOICE/vo005.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V010), 1, "data/Sound/VOICE/vo010.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V011), 1, "data/Sound/VOICE/vo011.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V100), 1, "data/Sound/VOICE/vo100.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V101), 1, "data/Sound/VOICE/vo101.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V102), 1, "data/Sound/VOICE/vo102.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V103), 1, "data/Sound/VOICE/vo103.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V104), 1, "data/Sound/VOICE/vo104.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V105), 1, "data/Sound/VOICE/vo105.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V106), 1, "data/Sound/VOICE/vo106.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V107), 1, "data/Sound/VOICE/vo107.wav", false);
			SE->Add(SoundType::VOICE, static_cast<int>(VoiceEnum::V108), 1, "data/Sound/VOICE/vo108.wav", false);
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();

			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::CartFall))->SetLocalVolume(48);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::MagFall))->SetLocalVolume(48);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Trigger))->SetLocalVolume(48);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::rolling_rocket))->SetLocalVolume(128);
			//SE->Get(SoundType::SE, static_cast<int>(SoundEnum::AmmoLoad))->SetLocalVolume(48);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Shot2))->SetLocalVolume(216);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Shot3))->SetLocalVolume(216);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->SetLocalVolume(128);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Hit))->SetLocalVolume(255);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitMe))->SetLocalVolume(255);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->SetLocalVolume(255);

			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing))->SetLocalVolume(192);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathend))->SetLocalVolume(192);
			for (int loop = 0; loop < 5; ++loop) {
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGround0) + loop)->SetLocalVolume(92);
			}
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Heli));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Heli2));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Envi));

			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::CartFall));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::MagFall));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Trigger));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::rolling_rocket));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Tinnitus));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Pin));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Throw));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::FallGrenade));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Explosion));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::GetItem));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::PutItem));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Delivery));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::announce));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::resultEnv));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::resultbutton));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::resultEnd));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::taskstart));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::alarm));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::returnButton));

			for (int loop = 0; loop < 6; ++loop) {
				SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Cocking1_0) + loop);
				SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Cocking2_0) + loop);
				SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Cocking3_0) + loop);
				SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Cocking4_0) + loop);
			}
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::RunFoot));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Hit));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::HitMe));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::HitGuard));
			for (int loop = 0; loop < 5; ++loop) {
				SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::HitGround0) + loop);
			}
			for (int loop = 0; loop < 6; ++loop) {
				SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Man_Hurt1) + loop);
			}
			for (int loop = 0; loop < 8; ++loop) {
				SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Man_Death1) + loop);
			}
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Man_contact));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Man_openfire));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Man_reload));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Man_takecover));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Man_breathend));

			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V000));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V001));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V002));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V003));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V004));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V005));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V010));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V011));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V100));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V101));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V102));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V103));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V104));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V105));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V106));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V107));
			SE->Delete(SoundType::VOICE, static_cast<int>(VoiceEnum::V108));
		}
		//
		void			MainSceneUI::Set(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();

			this->m_BodyGraph.Load("data/UI/Body.bmp");
			this->m_ArmorGraph.Load("data/UI/Armor.bmp");
			this->m_HeadGraph.Load("data/UI/Head.bmp");
			this->m_BodyCGraph.Load("data/UI/BodyC.bmp");
			this->m_ArmorCGraph.Load("data/UI/ArmorC.bmp");
			this->m_HeadCGraph.Load("data/UI/HeadC.bmp");
			this->m_ItembackGraph.Load("data/UI/itemback.png");
			this->OIL_Graph.Load("data/UI/back.png");
			this->DeleteItemGraph.Load("data/UI/Delete.png");
			this->ReturnGraph.Load("data/UI/Return.png");
			this->GaugeGraph.Load("data/UI/Gauge.bmp");
			this->RunGraph.Load("data/UI/Run.png");
			this->AdrenalineGraph.Load("data/UI/Adrenaline.png");

			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
			m_Score = static_cast<float>(ViewPlayer->GetScore());
		}
		void			MainSceneUI::Update(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();

			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
			auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();

			CanLookTarget = false;
			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				if (loop == PlayerMngr->GetWatchPlayerID()) { continue; }
				auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
				CanLookTarget |= chara->IsAlive() && chara->GetCanLookByPlayer();
			}
			Easing(&LookPer, CanLookTarget ? 1.0f : 0.0f, 0.9f, EasingType::OutExpo);
			bool IsDrawAimUI = false;
			if (ViewChara->GetGunPtrNow()) {
				IsDrawAimUI = ViewChara->GetGunPtrNow()->GetCanShot() && (ViewChara->GetGunPtrNow()->GetGunAnime() != Charas::GunAnimeID::Watch);
			}

			IsDrawAimUIPer = std::clamp(IsDrawAimUIPer + (IsDrawAimUI ? 1.0f : -1.0f) * DXLib_refParts->GetDeltaTime() / 0.1f, 0.f, 1.f);

			Easing(&IsDownUIPer, !IsDrawAimUI ? 1.0f : 0.0f, 0.9f, EasingType::OutExpo);

			Easing(&m_AmmoInPer, 0.f, 0.9f, EasingType::OutExpo);
			if (ViewChara->GetGunPtrNow()) {
				if (this->m_AmmoNumTotal != ViewChara->GetGunPtrNow()->GetAmmoNumTotal()) {
					m_AmmoNumTotal = ViewChara->GetGunPtrNow()->GetAmmoNumTotal();
					m_AmmoInPer = 1.f;
					m_AmmoRand = (GetRand(100) < 50) ? 1.f : -1.f;
				}
			}
			Easing(&m_AmmoRandR, m_AmmoRand, 0.9f, EasingType::OutExpo);

			if (this->m_ReHP > ViewChara->GetHP().GetPoint()) {
				this->m_DamagePer = 2.f;
			}
			else {
				this->m_DamagePer = std::max(this->m_DamagePer - DXLib_refParts->GetDeltaTime(), 0.f);
			}
			Easing(&this->m_DamagePerR, std::clamp(this->m_DamagePer, 0.f, 1.f), 0.8f, EasingType::OutExpo);
			this->m_ReHP = ViewChara->GetHP().GetPoint();

			{
				int Count = 0;
				int Now = 0;
				for (int gunNum = 0; gunNum < 3; ++gunNum) {
					if (ViewChara->GetGunPtrNowSel() == gunNum) {
						Now = Count;
					}
					if (ViewChara->GetGunPtr(gunNum)) {
						++Count;
					}
				}
				if (this->m_RadPrev != Now) {
					if (
						(this->m_RadPrev == 0 && Now == 1) ||
						(this->m_RadPrev == 1 && Now == 2) ||
						(this->m_RadPrev == 2 && Now == 0)
						) {
						m_RadR = -1.f;
					}
					else if (
						(this->m_RadPrev == 0 && Now == 2) ||
						(this->m_RadPrev == 2 && Now == 1) ||
						(this->m_RadPrev == 1 && Now == 0)
						)
					{
						m_RadR = 1.f;
					}
					else {
						m_RadR = 1.f;
					}
					m_RadPrev = Now;
				}
				else {
					Easing(&m_RadR, 0.f, 0.9f, EasingType::OutExpo);
				}
			}
			{
				float kirogram = static_cast<float>(ViewChara->GetWeight_gram()) / 1000.f;
				if (std::abs(kirogram - m_Gram) > 5.f) {
					Easing(&m_Gram, kirogram, 0.95f, EasingType::OutExpo);
				}
				else if (std::abs(kirogram - m_Gram) > 0.02f) {
					m_Gram += DXLib_refParts->GetDeltaTime() / 0.1f * ((kirogram - m_Gram) > 0.f ? 1.f : -1.f);
				}
				else {
					m_Gram = kirogram;
				}
			}

			{
				float Score = static_cast<float>(ViewPlayer->GetScore());
				if (std::abs(Score - m_Score) > 5.f) {
					Easing(&m_Score, Score, 0.95f, EasingType::OutExpo);
				}
				else if (std::abs(Score - m_Score) > 0.02f) {
					m_Score += DXLib_refParts->GetDeltaTime() / 0.1f * ((Score - m_Score) > 0.f ? 1.f : -1.f);
				}
				else {
					m_Score = Score;
				}
			}

		}
		void			MainSceneUI::Draw(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
			auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* LocalizeParts = LocalizePool::Instance();

			int xp1{}, yp1{};
			int xp2{}, yp2{};
			int xp3{}, yp3{};
			//ダメージ表示
			if (this->m_DamagePerR > 0.f) {
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, static_cast<int>(216.f * this->m_DamagePerR));
				DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &OIL_Graph, 0, 0, 1920, 1080, true);
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
			//アイテムスロット
			{

				xp1 = (400);
				yp1 = (1080 * 4 / 5);

				int Height = 100;

				int loop = 0;
				bool hasItem = false;
				for (auto& ID : ViewPlayer->GetInventory()) {
					xp1 = 400 - Height * (static_cast<int>(ViewPlayer->GetInventory().size()) / 2) + Height * loop;

					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 215);
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &this->m_ItembackGraph, xp1, yp1, 96.f / 512.f, 0.f, true);
					if (ID.m_DrawID != InvalidID) {
						hasItem = true;
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, static_cast<int>(255.f * ID.m_Alpha));
						DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
							&Objects::ItemObjDataManager::Instance()->GetList().at(ID.m_DrawID)->GetIconGraph(),
							xp1, yp1 - static_cast<int>(ID.m_Yadd), (128.f / 512.f) * ID.m_Scale, 0.f, true);

						if (ID.m_timer > 0.f) {
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, 255, 0);
							DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, &this->DeleteItemGraph, xp1, yp1, 100.f * (ID.m_timer / 0.5f), 0.f, 64.f / 128.f);
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
						}
						Controls::PADS Guide{};
						switch (loop) {
						case 0: Guide = Controls::PADS::ITEMDELETE1; break;
						case 1: Guide = Controls::PADS::ITEMDELETE2; break;
						case 2: Guide = Controls::PADS::ITEMDELETE3; break;
						case 3: Guide = Controls::PADS::ITEMDELETE4; break;
						case 4: Guide = Controls::PADS::ITEMDELETE5; break;
						default:
							break;
						}

						KeyGuideParts->DrawButton(xp1 + 64 - 32, yp1 + 64 - 32, KeyGuide::GetPADStoOffset(Guide));
					}
					++loop;
				}
				{
					xp1 = 400;
					yp1 = (1080 * 4 / 5) + 64 + 16;

					int gram = ViewChara->GetWeight_gram();
					unsigned int Color = Green;
					if (gram == 0) {
						Color = Blue;
					}
					else if (gram < 20000) {
						float Per = static_cast<float>(gram) / 20000;
						if (Per < 0.5f) {
							Color = GetColor(
								0,
								Lerp(0, 255, Per / 0.5f),
								255);
						}
						else {
							Color = GetColor(
								0,
								255,
								Lerp(255, 0, (Per - 0.5f) / 0.5f));
						}
					}
					else if (gram < 80000) {
						float Per = static_cast<float>(gram - 20000) / (80000 - 20000);
						if (Per < 0.5f) {
							Color = GetColor(
								Lerp(0, 255, Per / 0.5f),
								255,
								0);
						}
						else {
							Color = GetColor(
								255,
								Lerp(255, 0, (Per - 0.5f) / 0.5f),
								0);
						}
					}
					else {
						Color = Red;
					}

					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::BOTTOM,
						xp1,
						yp1,
						Color, Black, "Weight %05.2f kg", m_Gram);

					xp1 = 400 - Height * 3;
					yp1 += 24;
					KeyGuideParts->DrawButton(xp1 - 32, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::ITEMDELETE));

					xp1 -= 32;
					yp1 += 0;
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3002));

					xp1 = 1920 / 2;
					yp1 = 720 - 32;
					if (hasItem && this->intParam[0]) {
						KeyGuideParts->DrawButton(xp1 - 32 / 2, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::ITEMDELETE));
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
							FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3004));
						yp1 += 64;
					}
					if (this->intParam[0]) {
						if (!this->intParam[2]) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
								FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3007));
						}
						else {
							KeyGuideParts->DrawButton(xp1 - 32 / 2, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT));

							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
								FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3005));
							if (this->floatParam[2] > 0.f) {
								//xp1 = 1920 / 2;
								yp1 += 64;
								DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, 255, 0);
								DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, &this->ReturnGraph, xp1, yp1, 100.f * this->floatParam[2], 0.f, 64.f / 128.f);
								DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							}
						}
					}
				}
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
			//タイム,スコア
			{
				xp1 = (30);
				yp1 = (10);
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, (this->floatParam[0] < 60.f) ? Red : White, Black, "TIME");
				if ((this->floatParam[0] >= 60.f) || ((this->floatParam[0] < 60.f) && ((this->floatParam[0] * 2.f - static_cast<int>(this->floatParam[0] * 2.f)) > 0.5f))) {
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 + (280), yp1, (this->floatParam[0] < 60.f) ? Red : White, Black, "%d:%05.2f",
						static_cast<int>(floatParam[0] / 60.0f), static_cast<float>(static_cast<int>(floatParam[0]) % 60) + (floatParam[0] - static_cast<float>(static_cast<int>(floatParam[0]))));
				}
				yp1 = (10 + 32 + 10);
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, "SCORE");
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 + (280), yp1, White, Black, "%04d",
					static_cast<int>(this->m_Score));


				for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
					if (loop == PlayerMngr->GetWatchPlayerID()) { continue; }
					auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
					if (!chara->IsAlive() || !chara->GetCanLookByPlayer()) { continue; }
					if (chara->GetIsActiveCameraPosToPlayer()) {
						if (ViewChara->GetGunPtrNow() && ViewChara->GetGunPtrNow()->GetAutoAimActive() && loop == ViewChara->GetGunPtrNow()->GetAutoAimID()) {
							DrawCtrls->SetDrawCircle(WindowSystem::DrawLayer::Normal, static_cast<int>(chara->GetCameraPosToPlayer().XScreenPos()), static_cast<int>(chara->GetCameraPosToPlayer().YScreenPos()), static_cast<int>(2400 / std::max(0.1f, chara->GetLengthToPlayer()) + Lerp(100, 0, LookPer)), Red, false, 5);
						}
						else {
							DrawCtrls->SetDrawCircle(WindowSystem::DrawLayer::Normal, static_cast<int>(chara->GetCameraPosToPlayer().XScreenPos()), static_cast<int>(chara->GetCameraPosToPlayer().YScreenPos()), static_cast<int>(2400 / std::max(0.1f, chara->GetLengthToPlayer()) + Lerp(100, 0, LookPer)), Red50, false, 2);
						}
					}
				}

				if (ViewChara->GetGunPtrNow()) {
					float BaseRad = ViewChara->GetLeanRad();
					auto DrawCircle = [&](int X, int Y, float scale, float Per) {
						auto* LocalizeParts = LocalizePool::Instance();
						auto* KeyGuideParts = KeyGuide::Instance();

						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, Per));
						int Scale = static_cast<int>(static_cast<float>(1080 * ViewChara->GetGunPtrNow()->GetAutoAimRadian() / CameraParts->GetMainCamera().GetCamFov()) * scale);
						DrawCtrls->SetDrawCircle(WindowSystem::DrawLayer::Normal, X, Y, Scale, Green, false, 2);
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, LookPer * (Per)));
						DrawCtrls->SetDrawCircle(WindowSystem::DrawLayer::Normal, X, Y, Scale + 4 + Lerp(100, 0, LookPer), Green, false, 2);
						//DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, Per));

						int Scale2 = Scale + 4 + Lerp(100, 0, LookPer);

						Vector3DX StartPos = CameraParts->GetMainCamera().GetCamPos();
						Vector3DX Vec1 = CameraParts->GetMainCamera().GetCamVec() - StartPos; Vec1.y = 0.0f; Vec1 = Vec1.normalized();
						{
							for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
								if (loop == PlayerMngr->GetWatchPlayerID()) { continue; }
								auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
								if (!chara->IsAlive() || !chara->GetCanLookByPlayer()) { continue; }
								Vector3DX Vec2 = (chara->GetEyePositionCache() - StartPos); Vec2.y = 0.0f; Vec2 = Vec2.normalized();
								float Angle = DX_PI_F + Vector3DX::Angle(Vec1, Vec2) * (Vector3DX::Cross(Vec1, Vec2).y > 0.0f ? 1.0f : -1.0f);

								xp1 = X - static_cast<int>(Scale2 * 1.5f * sin(Angle + deg2rad(5)));
								yp1 = Y + static_cast<int>(Scale2 * 1.5f * cos(Angle + deg2rad(5)));
								xp2 = X - static_cast<int>(Scale2 * sin(Angle));
								yp2 = Y + static_cast<int>(Scale2 * cos(Angle));
								xp3 = X - static_cast<int>(Scale2 * 1.5f * sin(Angle - deg2rad(5)));
								yp3 = Y + static_cast<int>(Scale2 * 1.5f * cos(Angle - deg2rad(5)));
								unsigned int Color = (loop == ViewChara->GetGunPtrNow()->GetAutoAimID()) ? Red : Yellow;
								DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Color, 2);
								DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp2, yp2, xp3, yp3, Color, 2);
								DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp3, yp3, xp1, yp1, Color, 2);
							}
						}
						//
						{
							xp1 = X - static_cast<int>((Scale + 64.f) * sin(deg2rad(-90) + BaseRad));
							yp1 = Y + static_cast<int>((Scale + 64.f) * cos(deg2rad(-90) + BaseRad));

							int hitpointRateG = std::clamp(2 * 255 * ViewChara->GetHP().GetPoint() / ViewChara->GetHP().GetMax(), 0, 255);
							int hitpointRateR = std::clamp(2 * (255 - 255 * ViewChara->GetHP().GetPoint() / ViewChara->GetHP().GetMax()), 0, 255);
							DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 192, Per));
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, hitpointRateR, hitpointRateG, 0);
							DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, (ViewChara->GetIsSquat()) ? &this->m_BodyCGraph : &this->m_BodyGraph, xp1, yp1, 128.f / 500.f, BaseRad, true);

							int BodyAPRate = 255 * ViewChara->GetBodyAP().GetPoint() / ViewChara->GetBodyAP().GetMax();
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, BodyAPRate, BodyAPRate, BodyAPRate);
							DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, (ViewChara->GetIsSquat()) ? &this->m_ArmorCGraph : &this->m_ArmorGraph, xp1, yp1, 128.f / 500.f, BaseRad, true);

							int HeadAPRate = 255 * ViewChara->GetHeadAP().GetPoint() / ViewChara->GetHeadAP().GetMax();
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, HeadAPRate, HeadAPRate, HeadAPRate);
							DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, (ViewChara->GetIsSquat()) ? &this->m_HeadCGraph : &this->m_HeadGraph, xp1, yp1, 128.f / 500.f, BaseRad, true);

							DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);


							bool HasSpare = false;
							for (auto& ID : ViewPlayer->GetInventory()) {
								if (!ID.HasItem()) { continue; }
								switch (ID.GetItem()->GetItemType()) {
								case Objects::ItemType::Helmet:
									if (ViewChara->GetHeadAP().GetPoint() != ViewChara->GetHeadAP().GetMax()) {
										HasSpare = true;
									}
									break;
								case Objects::ItemType::Armor:
									if (ViewChara->GetBodyAP().GetPoint() != ViewChara->GetBodyAP().GetMax()) {
										HasSpare = true;
									}
									break;
								default:
									break;
								}
							}

							xp1 = xp1 + 64;
							if (ViewChara->GetRunGauge() > 0.f) {
								yp1 = yp1 + 64;

								DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &this->RunGraph, xp1 - 16, yp1 + 16, 32.f / 64.f, 0.f, true);
								DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, 255, 0);
								DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, &this->GaugeGraph, xp1 - 16, yp1 + 16, 100.f * ViewChara->GetRunGauge(), 0.f, 32.f / 64.f);
								DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);

								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
									FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xp1, yp1 + 16, (ViewChara->GetRunGauge() * 0.3f) ? Green : Red, Black, "%05.2f s", ViewChara->GetRunGaugeTimer());
							}
							if (ViewChara->GetAdrenalineTime() > 0.f) {
								yp1 = yp1 + 64;

								DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &this->AdrenalineGraph, xp1 - 16, yp1 + 16, 32.f / 64.f, 0.f, true);
								DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, 255, 0);
								DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, &this->GaugeGraph, xp1 - 16, yp1 + 16, 100.f * ViewChara->GetAdrenalineTime(), 0.f, 32.f / 64.f);
								DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);

								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
									FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xp1, yp1 + 16, (ViewChara->GetAdrenalineTime() * 0.3f) ? Green : Red, Black, "%05.2f s", ViewChara->GetAdrenalineTimeTimer());
							}
							if (HasSpare) {
								yp1 = yp1 + 64;

								KeyGuideParts->DrawButton(xp1 - 32, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::HEALARMOR));

								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
									FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3000));
							}
							if (ViewChara->GetIsSquat()) {
								yp1 = yp1 + 64;
								KeyGuideParts->DrawButton(xp1 - 32, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::SQUAT));

								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
									FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3003));
							}
						}
						//
						{
							DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, Per));
							{
								int Bright = 128;
								DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, Bright, 0);
								for (int gunNum = -1; gunNum <= 1; gunNum += 2) {
									int Now = ViewChara->GetGunPtrNowSel() + gunNum;
									Now = (Now + 3) % 3;
									{
										float S = 0.2f * scale;
										if (ViewChara->GetGunPtr(Now)) {
											float Rad = deg2rad(30) * m_RadR - deg2rad(30) * gunNum + BaseRad;
											xp1 = X - static_cast<int>((768.f / 2.f * S + Scale + 32.f) * sin(deg2rad(90) + Rad));
											yp1 = Y + static_cast<int>((768.f / 2.f * S + Scale + 32.f) * cos(deg2rad(90) + Rad));
											const GraphHandle* Ptr = nullptr;
											if (ViewChara->GetGunPtr(Now)->GetModifySlot()->GetMyData()->GetIconGraph().IsActive()) {
												Ptr = &ViewChara->GetGunPtr(Now)->GetModifySlot()->GetMyData()->GetIconGraph();
											}
											if (Ptr) {
												DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
													Ptr, xp1, yp1, S, Rad, true);
											}
										}
									}
								}
							}
							{
								int Bright = Lerp(255, 128, std::abs(this->m_RadR));
								DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, Bright, 0);
								float S = Lerp(0.3f, 0.2f, std::abs(this->m_RadR)) * scale;
								if (ViewChara->GetGunPtrNow()) {
									float Rad = deg2rad(30) * m_RadR + BaseRad;
									xp1 = X - static_cast<int>((768.f / 2.f * S + Scale + 32.f) * sin(deg2rad(90) + Rad));
									yp1 = Y + static_cast<int>((768.f / 2.f * S + Scale + 32.f) * cos(deg2rad(90) + Rad));
									const GraphHandle* Ptr = nullptr;
									if (ViewChara->GetGunPtrNow()->GetModifySlot()->GetMyData()->GetIconGraph().IsActive()) {
										Ptr = &ViewChara->GetGunPtrNow()->GetModifySlot()->GetMyData()->GetIconGraph();
									}
									if (Ptr) {
										DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
											Ptr, xp1, yp1, S, Rad, true);
									}
								}
							}
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}
						//
						{
							if (ViewChara->GetGunPtrNow()->GetAmmoAll() > 0) {
								xp1 = X - static_cast<int>(Scale * 2.f * sin(deg2rad(30) + BaseRad));
								yp1 = Y + static_cast<int>(Scale * 2.f * cos(deg2rad(30) + BaseRad));
								xp2 = X - static_cast<int>(Scale * sin(deg2rad(30) + BaseRad));
								yp2 = Y + static_cast<int>(Scale * cos(deg2rad(30) + BaseRad));
								DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);
								DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1 - 100, yp1, xp1, yp1, Green, 2);
								bool isLow = (100 * ViewChara->GetGunPtrNow()->GetAmmoNumTotal() / ViewChara->GetGunPtrNow()->GetAmmoAll() < 30);
								unsigned int Color = GetColor(
									Lerp(isLow ? 255 : 0, 255, m_AmmoInPer),
									Lerp(!isLow ? 255 : 0, 255, m_AmmoInPer),
									Lerp(0, 255, m_AmmoInPer));

								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
									FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, xp1 - 65, yp1 - 3, Color, Black, "/%d", ViewChara->GetGunPtrNow()->GetAmmoAll());
								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
									FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM,
									xp1 - 100 - static_cast<int>(this->m_AmmoRandR * m_AmmoInPer * 10.f),
									yp1 - 3 - static_cast<int>(this->m_AmmoInPer * 3.f),
									Color, Black, "%d", ViewChara->GetGunPtrNow()->GetAmmoNumTotal());

								if (isLow) {
									xp1 = xp1 - 64;
									yp1 = yp1 + 64;

									KeyGuideParts->DrawButton(xp1 - 32, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD));

									DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
										FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3001));
								}
							}
						}
						//
						{
							xp1 = X - static_cast<int>(Scale * 1.0f * sin(deg2rad(90) + BaseRad));
							yp1 = Y + static_cast<int>(Scale * 1.0f * cos(deg2rad(90) + BaseRad));
							xp2 = X - static_cast<int>(Scale * 0.85f * sin(deg2rad(90) + BaseRad));
							yp2 = Y + static_cast<int>(Scale * 0.85f * cos(deg2rad(90) + BaseRad));
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);

							xp1 = X - static_cast<int>(Scale * 1.0f * sin(deg2rad(-90) + BaseRad));
							yp1 = Y + static_cast<int>(Scale * 1.0f * cos(deg2rad(-90) + BaseRad));
							xp2 = X - static_cast<int>(Scale * 0.85f * sin(deg2rad(-90) + BaseRad));
							yp2 = Y + static_cast<int>(Scale * 0.85f * cos(deg2rad(-90) + BaseRad));
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);

							xp1 = X - static_cast<int>(Scale * 1.0f * sin(deg2rad(0) + BaseRad));
							yp1 = Y + static_cast<int>(Scale * 1.0f * cos(deg2rad(0) + BaseRad));
							xp2 = X - static_cast<int>(Scale * 0.5f * sin(deg2rad(0) + BaseRad));
							yp2 = Y + static_cast<int>(Scale * 0.5f * cos(deg2rad(0) + BaseRad));
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);

							DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}
						};

					if (IsDrawAimUIPer > 0.01f) {
						int RetX = ViewChara->GetGunPtrNow()->GetAimXPos() + static_cast<int>(ViewChara->GetMoveEyePos().x * 100.0f);
						int RetY = ViewChara->GetGunPtrNow()->GetAimYPos() + static_cast<int>(ViewChara->GetMoveEyePos().y * 100.0f);

						DrawCircle(RetX, RetY, 1.f * (1.f - IsDownUIPer), IsDrawAimUIPer);
					}
					if ((1.f - IsDrawAimUIPer) > 0.01f &&
						(ViewChara->GetGunPtrNow() && (ViewChara->GetGunPtrNow()->GetGunAnime() != Charas::GunAnimeID::Watch))
						) {
						int RetX = 1920 / 2;
						int RetY = 1080 / 2;
						DrawCircle(RetX, RetY, 2.f * IsDownUIPer, 1.f - IsDrawAimUIPer);
					}
				}

				//スタートタイマー
				if (floatParam[1] > 0.0f) {
					xp1 = 1920 / 2;
					yp1 = (340);
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, "%d:%05.2f",
						static_cast<int>(floatParam[1] / 60.0f), static_cast<float>(static_cast<int>(floatParam[1]) % 60) + (floatParam[1] - static_cast<float>(static_cast<int>(floatParam[1]))));
				}

				if ((this->floatParam[3] == 0) || ((this->floatParam[3] * 4.f - static_cast<int>(this->floatParam[3] * 4.f)) > 0.5f)) {
					xp1 = 1920 / 2;
					yp1 = 400;
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(4000 + this->intParam[1]));
				}
			}
			//方位磁針
			{
				Vector3DX Vec = ViewChara->GetEyeRotationCache().zvec2(); Vec.y = 0.0f; Vec = Vec.normalized();
				float degreeBase = rad2deg(std::atan2f(Vec.x, Vec.z));
				float degreeBase2 = 0.f;
				if (PlayerMngr->GetItemContainerObj()) {
					Vector3DX Vec2 = PlayerMngr->GetItemContainerObj()->GetMove().GetPos() - ViewChara->GetEyePositionCache(); Vec2.y = 0.f; Vec2 = Vec2.normalized();
					degreeBase2 = rad2deg(Vector3DX::SignedAngle(Vec, Vec2, Vector3DX::up()));
				}

				for (int loop = -25; loop <= 25; ++loop) {
					int degree = (360 + static_cast<int>(degreeBase) + loop) % 360;
					xp1 = 960 + static_cast<int>((static_cast<int>(degreeBase) - degreeBase + loop) * 10) + static_cast<int>(ViewChara->GetMoveEyePos().x * 100.0f);
					yp1 = 1080 * 2 / 10 + static_cast<int>(ViewChara->GetMoveEyePos().y * 100.0f);
					if (degree % 90 == 0) {
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1 - 10, xp1, yp1 + 20, Green, 4);
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
							FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1 + 24, Green, Black, "%03d", degree);
					}
					else if (degree % 10 == 0) {
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1 - 10, xp1, yp1 + 10, Green, 4);
					}
					else {
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1 - 6, xp1, yp1 + 6, Green, 2);
					}
				}
				if (-25 < degreeBase2 && degreeBase2 < 25) {
					xp1 = 960 + static_cast<int>(degreeBase2 * 10) + static_cast<int>(ViewChara->GetMoveEyePos().x * 100.0f);
					yp1 = 1080 * 2 / 10 + static_cast<int>(ViewChara->GetMoveEyePos().y * 100.0f);
					DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1 - 10, xp1, yp1 + 16, Yellow, 8);
				}
				else {
					if (degreeBase2 < 0) {
						xp1 = 960 + static_cast<int>(-28 * 10) + static_cast<int>(ViewChara->GetMoveEyePos().x * 100.0f);
						yp1 = 1080 * 2 / 10 + static_cast<int>(ViewChara->GetMoveEyePos().y * 100.0f);
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1 - 10, xp1, yp1 + 10, Yellow, 2);
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1 - 5, yp1, xp1, yp1 + 10, Yellow, 2);
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1 - 5, yp1, xp1, yp1 - 10, Yellow, 2);
					}
					else {
						xp1 = 960 + static_cast<int>(28 * 10) + static_cast<int>(ViewChara->GetMoveEyePos().x * 100.0f);
						yp1 = 1080 * 2 / 10 + static_cast<int>(ViewChara->GetMoveEyePos().y * 100.0f);
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1 - 10, xp1, yp1 + 10, Yellow, 2);
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1 + 5, yp1, xp1, yp1 + 10, Yellow, 2);
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1 + 5, yp1, xp1, yp1 - 10, Yellow, 2);
					}
				}
			}
			//
		}
		//
		void			MainSceneResultUI::Load(void) noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			m_GameEndScreen.Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), false);
			m_ResultGraph.Load("data/UI/result.png");
		}
		void			MainSceneResultUI::Start(float Time) noexcept {
			m_GameEndScreen.GraphFilterBlt(PostPassEffect::Instance()->GetBufferScreen(), DX_GRAPH_FILTER_DOWN_SCALE, 1);
			m_BattleTimer = Time;
		}
		int				MainSceneResultUI::Update(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
			//
			int Answer = InvalidID;
			this->m_GameClearCount += DXLib_refParts->GetDeltaTime();
			this->m_GameClearTimer += DXLib_refParts->GetDeltaTime();
			bool Prev = m_IsSkillSelect;
			if (this->m_GameClearCount > 0.05f && !m_IsGameClearEnd) {
				m_GameClearCount -= 0.05f;
				m_GameEndScreen.GraphFilter(DX_GRAPH_FILTER_GAUSS, 32, 100);
				//
				if (this->m_GameClearTimer > 1.5f) {
					float Per = (this->m_GameClearTimer - 1.5f) / 2.f;
					float LocalPer = 0.f;
					const float LerpPer = 0.5f;

					for (int loop = 0; loop < 4; ++loop) {
						LocalPer = std::clamp((Per - (LerpPer + 0.1f) * loop) / LerpPer, 0.f, 1.f);
						if (0.f < LocalPer && LocalPer < 1.f) {
							if ((loop == 3) && (ViewPlayer->GetShot() == 0)) {
								continue;
							}
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::resultbutton))->Play(DX_PLAYTYPE_BACK, true);
							break;
						}
					}

					if (this->m_GameClearTimer > 1.5f + (0.5f + 0.1f) * 5.f * 2.f) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::resultEnd))->Play(DX_PLAYTYPE_BACK, true);
						m_IsGameClearEnd = true;
					}
				}
			}
			if (this->m_GameClearTimer > 1.5f + (0.5f + 0.1f) * 5.f * 2.f + 1.f) {
				m_IsSkillSelect = true;
			}
			if (this->m_GameClearTimer > 1.5f) {
				if (Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger()) {
					if (!m_IsGameClearEnd) {
						this->m_GameClearTimer = 1.5f + (0.5f + 0.1f) * 5.f * 2.f;
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::resultEnd))->Play(DX_PLAYTYPE_BACK, true);
						m_IsGameClearEnd = true;
					}
					else if (!m_IsSkillSelect) {
						m_IsSkillSelect = true;
					}
					else {
						Answer = 0;
						this->m_IsSkillSelectEnd = true;
					}
				}
				if (Pad->GetPadsInfo(Controls::PADS::RELOAD).GetKey().trigger()) {
					if (m_IsGameClearEnd && m_IsSkillSelect) {
						Answer = 1;
						this->m_IsSkillSelectEnd = true;
					}
				}
			}
			if (Prev != m_IsSkillSelect && m_IsSkillSelect) {
				//選べるスキル総数
				int CanPicSkill = 0;
				for (int loop = 0; loop < static_cast<int>(Player::SkillType::Max); ++loop) {
					if (!(Player::SkillList::Instance()->GetSkilLevel(static_cast<Player::SkillType>(loop)) >= 3)) {
						++CanPicSkill;
					}
				}
				//
				for (int loop = 0; loop < 3; ++loop) {
					if (loop >= CanPicSkill) {
						m_SkillSelect.at(loop) = Player::SkillType::ADDSCORE;//次遊ぶ際のスコアを+500
						continue;
					}
					m_SkillSelect.at(loop) = static_cast<Player::SkillType>(GetRand(static_cast<int>(Player::SkillType::Max) - 1));
					bool isContinue = false;
					//スキルラインナップに被りがある
					if (!isContinue) {
						for (int loop2 = 0; loop2 < loop; ++loop2) {
							if (this->m_SkillSelect.at(loop2) == m_SkillSelect.at(loop)) {
								isContinue = true;
								break;
							}
						}
					}
					//もう選べないスキル
					if (!isContinue) {
						if (Player::SkillList::Instance()->GetSkilLevel(this->m_SkillSelect.at(loop)) >= 3) {
							isContinue = true;
						}
					}
					if (isContinue) {
						--loop;
						continue;
					}
				}
			}
			if (this->m_IsSkillSelect) {
				m_SkillSelectTimer = std::clamp(this->m_SkillSelectTimer + DXLib_refParts->GetDeltaTime() / 0.25f, 0.f, 1.f);
			}
			if (!this->m_IsSkillSelectEnd) {
				//スキル選択
				if (this->m_IsGameClearEnd && m_IsSkillSelect && m_SkillSelectTimer >= 1.f) {
					int xp1 = 1920 / 2;
					int yp1 = 1080 / 2;
					float wide = 400.f;
					bool isSelect = false;
					for (int loop = 0; loop < 3; ++loop) {
						xp1 = 1920 / 2 + static_cast<int>(wide * (-(3.f - 1.f) / 2.f + loop));
						yp1 = 1080 / 2;
						if (HitPointToRectangle(Pad->GetMS_X(), Pad->GetMS_Y(), xp1 - (static_cast<int>(wide) / 2 - 8), yp1 - 540 / 2, xp1 + (static_cast<int>(wide) / 2 - 8), yp1 + 540 / 2)) {
							if (this->m_SkillSelectNow != loop) {
								SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
							}
							this->m_SkillSelectNow = loop;
							isSelect = true;
							if (Pad->GetPadsInfo(Controls::PADS::SHOT).GetKey().press() || Pad->GetPadsInfo(Controls::PADS::AIM).GetKey().press()) {
								Answer = (Pad->GetPadsInfo(Controls::PADS::AIM).GetKey().press()) ? 1 : 0;
								this->m_IsSkillSelectEnd = true;
							}
							break;
						}
					}
					if (!isSelect) {
						if (Pad->GetPadsInfo(Controls::PADS::MOVE_A).GetKey().trigger()) {
							SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
							--m_SkillSelectNow;
							if (this->m_SkillSelectNow < 0) {
								m_SkillSelectNow = 3 - 1;
							}
						}
						if (Pad->GetPadsInfo(Controls::PADS::MOVE_D).GetKey().trigger()) {
							SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, true);
							++m_SkillSelectNow;
							if (this->m_SkillSelectNow > 3 - 1) {
								m_SkillSelectNow = 0;
							}
						}
					}
				}
			}
			return Answer;
		}
		void			MainSceneResultUI::Draw(void) const noexcept {
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();

			DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_GameEndScreen, 0, 0, 1920, 1080, false);
			if (this->m_GameClearTimer > 1.5f) {
				float Per = (this->m_GameClearTimer - 1.5f) / 2.f;
				float LocalPer = 0.f;
				float Value = 0.f;
				const float LerpPer = 0.5f;
				//リザルト描画
				int xp1 = 960;
				int yp1 = 930;

				if (!m_IsGameClearEnd || !m_IsSkillSelect) {
					KeyGuideParts->DrawButton(xp1 - 32 / 2, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT));

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3008));
				}


				xp1 = (960);
				yp1 = (540);

				LocalPer = std::clamp((Per - 0.f) / LerpPer, 0.f, 1.f);
				Value = Lerp(0.f, m_BattleTimer, LocalPer);
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, static_cast<int>(255.f * LocalPer));
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 - static_cast<int>(32.f * LocalPer), yp1, White, Black, "TIME");
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1 + static_cast<int>(32.f * LocalPer), yp1, White, Black, "%d:%05.2f",
					static_cast<int>(Value / 60.0f), static_cast<float>(static_cast<int>(Value) % 60) + (Value - static_cast<float>(static_cast<int>(Value))));

				yp1 += (32 + 10);
				LocalPer = std::clamp((Per - (LerpPer + 0.1f) * 1.f) / LerpPer, 0.f, 1.f);
				Value = Lerp(0.f, static_cast<float>(ViewPlayer->GetScore()), LocalPer);
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, static_cast<int>(255.f * LocalPer));
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 - static_cast<int>(32.f * LocalPer), yp1, White, Black, "SCORE");
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1 + static_cast<int>(32.f * LocalPer), yp1, White, Black, "%04d",
					static_cast<int>(Value));

				yp1 += (32 + 10);
				LocalPer = std::clamp((Per - (LerpPer + 0.1f) * 2.f) / LerpPer, 0.f, 1.f);
				Value = Lerp(0.f, static_cast<float> (ViewPlayer->GetKill()), LocalPer);
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, static_cast<int>(255.f * LocalPer));
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 - static_cast<int>(32.f * LocalPer), yp1, White, Black, "KILL");
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1 + static_cast<int>(32.f * LocalPer), yp1, White, Black, "%d",
					static_cast<int>(Value));

				if (ViewPlayer->GetShot() > 0) {
					yp1 += (32 + 10);
					LocalPer = std::clamp((Per - (LerpPer + 0.1f) * 3.f) / LerpPer, 0.f, 1.f);
					Value = Lerp(0.f, 100.f * static_cast<float>(ViewPlayer->GetHit()) / ViewPlayer->GetShot(), LocalPer);
					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, static_cast<int>(255.f * LocalPer));
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 - static_cast<int>(32.f * LocalPer), yp1, White, Black, "HIT RATE");
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1 + static_cast<int>(32.f * LocalPer), yp1, White, Black, "%05.2f ％",
						Value);
				}

				xp1 = (960);
				yp1 = (270);
				LocalPer = std::clamp((Per - (LerpPer + 0.1f) * 5.f) / 0.1f, 0.f, 1.f);
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, static_cast<int>(255.f * LocalPer));

				if (LocalPer < 0.8f) {
					LocalPer = Lerp(5.f, 0.9f, (LocalPer - 0.f) / (0.8f - 0.0f));
				}
				else {
					LocalPer = Lerp(0.9f, 1.f, (LocalPer - 0.8f) / (1.f - 0.8f));
				}

				int ShadowOfs = 10 + static_cast<int>((LocalPer - 1.f) * 10.f);

				xp1 += ShadowOfs;
				yp1 += ShadowOfs;
				DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, 0, 0);
				DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_ResultGraph, xp1 - static_cast<int>(256.f * LocalPer), yp1 - static_cast<int>(128.f * LocalPer), xp1 + static_cast<int>(256.f * LocalPer), yp1 + static_cast<int>(128.f * LocalPer), true);
				xp1 -= ShadowOfs;
				yp1 -= ShadowOfs;

				DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);

				DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &m_ResultGraph, xp1 - static_cast<int>(256.f * LocalPer), yp1 - static_cast<int>(128.f * LocalPer), xp1 + static_cast<int>(256.f * LocalPer), yp1 + static_cast<int>(128.f * LocalPer), true);
				DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);


				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}

			//スキル選択
			if (this->m_IsGameClearEnd && m_IsSkillSelect && m_SkillSelectTimer > 0.f) {
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, static_cast<int>(Lerp(0.f, 128.f, m_SkillSelectTimer)));
				DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, 1920, 1080, Black, true);
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);

				int xp1 = 1920 / 2;
				int yp1 = 1080 / 2;

				if (this->m_SkillSelectTimer < 0.8f) {
					yp1 += static_cast<int>(Lerp(-1080.f, 64.f, (this->m_SkillSelectTimer - 0.f) / (0.8f - 0.f)));
				}
				else if (this->m_SkillSelectTimer < 1.f) {
					yp1 += static_cast<int>(Lerp(64.f, 0.f, (this->m_SkillSelectTimer - 0.8f) / (1.f - 0.8f)));
				}

				float wide = 400.f;
				for (int loop = 0; loop < 3; ++loop) {
					xp1 = 1920 / 2 + static_cast<int>(wide * (-(3.f - 1.f) / 2.f + loop));
					bool IsADDSCORE = (this->m_SkillSelect.at(loop) == Player::SkillType::ADDSCORE);
					int ID = static_cast<int>(this->m_SkillSelect.at(loop));
					if (IsADDSCORE) {
						ID = static_cast<int>(Player::SkillType::Max);
					}
					DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 - (static_cast<int>(wide) / 2 - 8), yp1 - 540 / 2, xp1 + (static_cast<int>(wide) / 2 - 8), yp1 + 540 / 2, DarkGreen, true);

					std::string Title = LocalizeParts->Get(5000 + ID);
					if (!IsADDSCORE) {
						Title += " Lv.";
						Title += std::to_string(Player::SkillList::Instance()->GetSkilLevel(this->m_SkillSelect.at(loop)) + 1);
					}

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1 - 200, White, Black, Title.c_str());

					std::string Str = LocalizeParts->Get(5100 + ID);
					{
						std::string t = "[d]";  // 検索文字列
						auto pos = Str.find(t);
						auto len = t.length();
						if (pos != std::string::npos) {
							int ADD = 1000;
							if (!IsADDSCORE) {
								ADD = static_cast<int>(Player::SkillList::Instance()->GetSkillValueNext(this->m_SkillSelect.at(loop)));
							}
							Str.replace(pos, len, std::to_string(ADD));
						}
					}
					DrawCtrls->SetStringAutoFit(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
						xp1 - (static_cast<int>(wide) / 2 - 8 - 64), yp1 + 32, xp1 + (static_cast<int>(wide) / 2 - 8 - 64), yp1 + 540 / 2,
						White, Black, Str);
					if (loop == m_SkillSelectNow) {
						DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
							xp1 - (static_cast<int>(wide) / 2 - 8), yp1 - 540 / 2, xp1 + (static_cast<int>(wide) / 2 - 8), yp1 + 540 / 2,
							Green, false, 5);
					}
				}

				{
					xp1 = 960;
					yp1 = 70;

					if (this->m_SkillSelectTimer < 0.9f) {
						yp1 += static_cast<int>(Lerp(-1080.f, 64.f, (this->m_SkillSelectTimer - 0.f) / (0.9f - 0.f)));
					}
					else if (this->m_SkillSelectTimer < 1.f) {
						yp1 += static_cast<int>(Lerp(64.f, 0.f, (this->m_SkillSelectTimer - 0.9f) / (1.f - 0.9f)));
					}

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (64),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, Green, Black, "Select Skill!");
				}

				if (this->m_SkillSelectTimer == 1.f) {
					xp1 = 960;
					yp1 = 930;
					KeyGuideParts->DrawButton(xp1 - 32 / 2, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT));
					KeyGuideParts->DrawButton(xp1 + 32 / 2, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::SHOT));
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3009));

					yp1 -= 64;
					KeyGuideParts->DrawButton(xp1 - 32 / 2, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD));
					KeyGuideParts->DrawButton(xp1 + 32 / 2, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::AIM));
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3006));
				}
			}
		}
		//
		void			TransceiverUI::Put(int ID, float Sec) noexcept {
			this->m_TextID = ID;
			this->m_TextTimerMax = Sec;
		}
		void			TransceiverUI::Set(void) noexcept {
			this->m_TextID = InvalidID;
			this->m_PrevTextSoundID = InvalidID;
			this->m_TextTimer =0.f;
		}
		void			TransceiverUI::Update(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();

			int SoundID = InvalidID;
			if (this->m_TextID == InvalidID) {
			}
			else if (this->m_TextID < 10) {
				SoundID = static_cast<int>(VoiceEnum::V000) + this->m_TextID;
			}
			else if (this->m_TextID < 20) {
				SoundID = static_cast<int>(VoiceEnum::V010) + (this->m_TextID - 10);
			}
			else if (this->m_TextID < 100) {
			}
			else if (this->m_TextID < 110) {
				SoundID = static_cast<int>(VoiceEnum::V100) + (this->m_TextID - 100);
			}
			if (this->m_PrevTextSoundID != SoundID) {
				if (SoundID != InvalidID) {
					if (SE->Get(SoundType::VOICE, this->m_PrevTextSoundID)->CheckPlay()) {
						SE->Get(SoundType::VOICE, this->m_PrevTextSoundID)->StopAll();
					}
					SE->Get(SoundType::VOICE, SoundID)->Play(DX_PLAYTYPE_BACK, true);
					this->m_TextTimer = static_cast<float>(SE->Get(SoundType::VOICE, SoundID)->GetTotalTIme()) / 1000.f;
				}
				else {
					this->m_TextTimer = 3.f;
				}
				if (this->m_TextTimer <= this->m_TextTimerMax) {
					this->m_TextTimer = this->m_TextTimerMax;
				}
				this->m_PrevTextSoundID = SoundID;
			}
			if (this->m_TextTimer > 0.f) {
				this->m_TextTimer = std::max(this->m_TextTimer - DXLib_refParts->GetDeltaTime(), 0.f);
				if (this->m_TextTimer == 0.f) {
					this->m_TextID = InvalidID;
				}
			}
		}
		void			TransceiverUI::Draw(void) const noexcept{
			if (this->m_TextID == InvalidID) { return; }
			auto* LocalizeParts = LocalizePool::Instance();
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();

			std::string Str = "";
			Str += "[ ";
			Str += LocalizeParts->Get(6000 + this->m_TextID);
			Str += " ]";
			DrawCtrls->SetStringAutoFit(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, 18,
				(1920 / 2 - 520), (1080 - 128),
				(1920 / 2 + 520), (1080 - 128 + 18 * 3),
				White, Black, Str);
		}
		//
		void			TaskOperator::StartNextTask() noexcept {
			auto* LocalizeParts = LocalizePool::Instance();
			auto* SE = SoundPool::Instance();
			auto* SideLogParts = SideLog::Instance();
			if (HaveTask()) {
				SideLogParts->Add(5.0f, 0.0f, Yellow, LocalizeParts->Get(250 + static_cast<int>(this->m_TaskInfoList.begin()->first.m_TaskType)));
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::taskstart))->Play(DX_PLAYTYPE_BACK, true);
			}
		}
		void			TaskOperator::Load(void) noexcept {
			m_KillGraph.Load("data/UI/kill.png");
		}
		void			TaskOperator::Set(void) noexcept {
			m_TaskInfoList.clear();
		}
		bool			TaskOperator::CheckItem(int ItemID) noexcept {
			auto* SideLogParts = SideLog::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (IsActiveTask(TaskType::Obtain) && m_TaskInfoList.begin()->first.m_ItemID == ItemID) {
				++m_TaskInfoList.begin()->second;
				if (this->m_TaskInfoList.begin()->second >= m_TaskInfoList.begin()->first.m_Count) {
					m_TaskInfoList.erase(this->m_TaskInfoList.begin());
					PlayerMngr->GetPlayer(0)->AddScore(200);
					SideLogParts->Add(5.0f, 0.0f, Green, ((std::string)(LocalizeParts->Get(206)) + " +" + std::to_string(200)).c_str());
					StartNextTask();
					return true;
				}
			}
			return false;
		}
		bool			TaskOperator::CheckKill(void) noexcept {
			auto* SideLogParts = SideLog::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (IsActiveTask(TaskType::KillEnemy)) {
				++m_TaskInfoList.begin()->second;
				if (this->m_TaskInfoList.begin()->second >= m_TaskInfoList.begin()->first.m_Count) {
					m_TaskInfoList.erase(this->m_TaskInfoList.begin());
					PlayerMngr->GetPlayer(0)->AddScore(200);
					SideLogParts->Add(5.0f, 0.0f, Green, ((std::string)(LocalizeParts->Get(206)) + " +" + std::to_string(200)).c_str());
					StartNextTask();
					return true;
				}
			}
			return false;
		}
		void			TaskOperator::Draw(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			if (HaveTask()) {
				int xp1 = 400;
				int yp1 = 1080 * 3 / 4 - 64;
				DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 - 64, yp1 - 64, xp1 + 64, yp1 + 64, Black, true);
				DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 - 64, yp1 - 64, xp1 + 64, yp1 + 64, Green, false, 3);
				switch (this->m_TaskInfoList.begin()->first.m_TaskType) {
				case TaskType::Obtain:
				{
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
						&Objects::ItemObjDataManager::Instance()->GetList().at(this->m_TaskInfoList.begin()->first.m_ItemID)->GetIconGraph(),
						xp1, yp1, (128.f / 512.f) * 1.f, 0.f, true);
				}
				break;
				case TaskType::KillEnemy:
				{
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &m_KillGraph, xp1, yp1, (128.f / 512.f) * 1.f, 0.f, true);
				}
				break;
				default:
					break;
				}
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::BOTTOM, xp1, yp1 + 64 - 10, Green, Black, "%d / %d",
					m_TaskInfoList.begin()->second, m_TaskInfoList.begin()->first.m_Count);
			}
			else {
				int xp1 = 400;
				int yp1 = 1080 * 3 / 4 - 64;
				DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 - 64, yp1 - 64, xp1 + 64, yp1 + 64, Black, true);
				DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1 - 64, yp1 - 64, xp1 + 64, yp1 + 64, Green, false, 3);
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::BOTTOM, xp1, yp1 + 64 - 10, Green, Black, "ALL CLEAR");
			}
		}
		void			TaskOperator::Dispose(void) noexcept {
			m_KillGraph.Dispose();
		}
		//
		void			StartMovie::Load(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->LoadModelBefore("data/model/UH60_Movie/");
		}
		void			StartMovie::Set(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			m_MovieHeli = std::make_shared<Charas::MovieObject>();
			ObjMngr->InitObject(this->m_MovieHeli, "data/model/UH60_Movie/");
			this->m_StartAnimTimer = 0.f;
			this->m_MovieEndTimer = 1.5f;
			this->m_IsSkipMovie = false;
		}
		bool			StartMovie::Update(void) noexcept {
			bool										IsEnd = false;
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* OptionParts = OptionManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();

			SetFogEnable(false);
			if (this->m_StartAnimTimer == 0.f) {
				SE->Get(SoundType::BGM, 1)->Play(DX_PLAYTYPE_BACK, true);
			}
			m_StartAnimTimer += DXLib_refParts->GetDeltaTime();

			if (PlayerMngr->GetHelicopter()) {
				PlayerMngr->GetHelicopter()->SetActive(false);
			}
			if (PlayerMngr->GetTeamHelicopter()) {
				PlayerMngr->GetTeamHelicopter()->SetActive(false);
			}

			Vector3DX Pos = PlayerMngr->GetItemContainerObj()->GetMove().GetPos(); Pos.y = -20.f * Scale3DRate;
			m_MovieHeli->SetMove().SetPos(Pos);

			m_MovieHeli->UpdateLocal();

			//カメラ
			Vector3DX CamPos = CameraParts->GetMainCamera().GetCamPos();
			Vector3DX CamVec = CameraParts->GetMainCamera().GetCamVec();
			auto fovBuf = CameraParts->GetMainCamera().GetCamFov();
			float fovTarget = deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov));

			if (this->m_StartAnimTimer < 2.f) {
				CamPos = Vector3DX::vget(3.f, -26.5f, 10.f) * Scale3DRate;
				CamVec = m_MovieHeli->GetObj().GetFrameLocalWorldMatrix(this->m_MovieHeli->GetFrame(static_cast<int>(Charas::MovieHeliFrame::Center))).pos();
				Easing(&fovBuf, deg2rad(70), 0.9f, EasingType::OutExpo);
			}
			else if (this->m_StartAnimTimer < 4.5f) {
				CamPos = m_MovieHeli->GetObj().GetFrameLocalWorldMatrix(this->m_MovieHeli->GetFrame(static_cast<int>(Charas::MovieHeliFrame::Center))).pos() + Vector3DX::vget(0.f, 2.5f, 0.1f) * Scale3DRate;
				CamVec = m_MovieHeli->GetObj().GetFrameLocalWorldMatrix(this->m_MovieHeli->GetFrame(static_cast<int>(Charas::MovieHeliFrame::Center))).pos();
				Camera3D::Instance()->SetCamShake(0.5f, 0.1f * Scale3DRate);
				Easing(&fovBuf, deg2rad(140), 0.9f, EasingType::OutExpo);
			}
			else {
				CamPos = Vector3DX::vget(0.f, -24.0f, 0.f) * Scale3DRate;
				if (this->m_StartAnimTimer < 4.5f + 1.5f) {
					CamVec = Pos;
					Camera3D::Instance()->SetCamShake(0.5f, 0.5f * Scale3DRate);
					fovBuf = deg2rad(70);
				}
				else {
					Easing(&CamVec, PlayerMngr->GetItemContainerObj()->GetMove().GetPos(), 0.95f, EasingType::OutExpo);
					Easing(&fovBuf, deg2rad(35), 0.9f, EasingType::OutExpo);
				}

				if (this->m_StartAnimTimer >= 4.5f + 3.f) {
					if (FadeControl::Instance()->IsClear() && !m_IsSkipMovie) {
						FadeControl::Instance()->SetBlackOut(true);
					}
					else {
						m_MovieEndTimer = std::max(this->m_MovieEndTimer - DXLib_refParts->GetDeltaTime(), 0.f);
						if (this->m_MovieEndTimer == 0.f) {
							m_MovieHeli->SetActive(false);
							FadeControl::Instance()->SetBlackOut(false);
							IsEnd = true;
							fovBuf = fovTarget;
							SceneParts->SetPauseEnable(false);
							SetFogEnable(true);
							if (PlayerMngr->GetHelicopter()) {
								PlayerMngr->GetHelicopter()->SetActive(true);
							}
							if (PlayerMngr->GetTeamHelicopter()) {
								PlayerMngr->GetTeamHelicopter()->SetActive(true);
							}
						}
					}
				}
			}
			if (this->m_StartAnimTimer < 4.5f + 3.f) {
				if (!m_IsSkipMovie) {
					if (Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger()) {
						m_IsSkipMovie = true;
						FadeControl::Instance()->SetBlackOut(true);
					}
				}
				else {
					m_MovieEndTimer = std::max(this->m_MovieEndTimer - DXLib_refParts->GetDeltaTime(), 0.f);
					if (this->m_MovieEndTimer == 0.f) {
						m_MovieHeli->SetActive(false);
						FadeControl::Instance()->SetBlackOut(false);
						IsEnd = true;
						fovBuf = fovTarget;
						SceneParts->SetPauseEnable(false);
						SetFogEnable(true);
						if (PlayerMngr->GetHelicopter()) {
							PlayerMngr->GetHelicopter()->SetActive(true);
						}
						if (PlayerMngr->GetTeamHelicopter()) {
							PlayerMngr->GetTeamHelicopter()->SetActive(true);
						}
					}
				}
			}

			CameraParts->SetMainCamera().SetCamPos(
				CamPos + Camera3D::Instance()->GetCamShake(),
				CamVec + Camera3D::Instance()->GetCamShake(),
				Vector3DX::up());
			CameraParts->SetMainCamera().SetCamInfo(fovBuf, CameraParts->GetMainCamera().GetCamNear(), CameraParts->GetMainCamera().GetCamFar());


			//背景
			BackGroundParts->Update();
			return IsEnd;
		}
	}
}
