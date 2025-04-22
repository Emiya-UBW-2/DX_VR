#include	"MainSceneSub.hpp"

#include	"Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			PauseMenuControl::Load(void) noexcept {
			auto* ButtonParts = UIs::ButtonControl::Instance();
			ButtonParts->ResetSelect();
			ButtonParts->AddStringButton("Retire", 48, true, BaseScreenWidth - 64, BaseScreenHeight - 84 - 64 * 2, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM);
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
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Envi), 1, "data/Sound/SE/Envi.wav", false);

			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Tank_Shot), 1, "data/Sound/SE/Tank/Shot.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Tank_Ricochet), 1, "data/Sound/SE/Tank/Ricochet.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Tank_engine), 1, "data/Sound/SE/Tank/Engine.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Tank_move), 1, "data/Sound/SE/Tank/Move.wav");


			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::CartFall), 6, "data/Sound/SE/gun/case.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::MagFall), 6, "data/Sound/SE/gun/ModFall.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Trigger), 1, "data/Sound/SE/gun/trigger.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Pin), 4, "data/Sound/SE/pin.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Throw), 4, "data/Sound/SE/throw.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::FallGrenade), 4, "data/Sound/SE/fallgrenade.wav");
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Explosion), 4, "data/Sound/SE/explosion.wav");
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
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing), 10, "data/Sound/SE/voice/breathing.wav", false);
			SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Man_breathend), 10, "data/Sound/SE/voice/breathend.wav", false);
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();

			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_engine))->SetLocalVolume(64);

			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::CartFall))->SetLocalVolume(48);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::MagFall))->SetLocalVolume(48);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Trigger))->SetLocalVolume(48);
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
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Envi));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Tank_Shot));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Tank_Ricochet));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Tank_engine));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Tank_move));

			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::CartFall));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::MagFall));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Trigger));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Pin));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Throw));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::FallGrenade));
			SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Explosion));
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
		}
		//
		void			MainSceneUI::Update(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();

			CanLookTarget = false;
			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				if (loop == PlayerMngr->GetWatchPlayerID()) { continue; }
				auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
				CanLookTarget |= chara->IsAlive() && chara->GetCanLookByPlayer();
			}
			Easing(&LookPer, CanLookTarget ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			bool IsDrawAimUI = false;
			if (ViewChara->GetGunPtrNow()) {
				IsDrawAimUI = ViewChara->GetGunPtrNow()->GetCanShot() && (ViewChara->GetGunPtrNow()->GetGunAnime() != Charas::GunAnimeID::Watch);
			}
			Easing(&IsDrawAimUIPer, IsDrawAimUI ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			if (IsDrawAimUIPer < 0.1f && !IsDrawAimUI) { IsDrawAimUIPer = 0.f; }
		}
		void			MainSceneUI::Draw(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();

			int xp1{}, yp1{};
			int xp2{}, yp2{};
			int xp3{}, yp3{};
			//タイム,スコア
			{
				xp1 = (30);
				yp1 = (10);
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, "TIME");
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 + (240), yp1, White, Black, "%d:%05.2f",
					static_cast<int>(floatParam[0] / 60.f), static_cast<float>(static_cast<int>(floatParam[0]) % 60) + (floatParam[0] - static_cast<float>(static_cast<int>(floatParam[0]))));



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

				if (IsDrawAimUIPer > 0.1f && ViewChara->GetGunPtrNow()) {
					int RetX = ViewChara->GetGunPtrNow()->GetAimXPos() + static_cast<int>(ViewChara->GetMoveEyePos().x * 100.f);
					int RetY = ViewChara->GetGunPtrNow()->GetAimYPos() + static_cast<int>(ViewChara->GetMoveEyePos().y * 100.f);
					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, IsDrawAimUIPer));
					auto fov_t = CameraParts->GetMainCamera().GetCamFov();
					int Scale = static_cast<int>(1080 * ViewChara->GetGunPtrNow()->GetAutoAimRadian() / fov_t);
					DrawCtrls->SetDrawCircle(WindowSystem::DrawLayer::Normal, RetX, RetY, Scale, Green, false, 2);
					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, LookPer * IsDrawAimUIPer));
					DrawCtrls->SetDrawCircle(WindowSystem::DrawLayer::Normal, RetX, RetY, Scale + 4 + Lerp(100, 0, LookPer), Green, false, 2);
					//DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Lerp(0, 255, IsDrawAimUIPer));

					int Scale2 = Scale + 4 + Lerp(100, 0, LookPer);

					Vector3DX StartPos = CameraParts->GetMainCamera().GetCamPos();
					Vector3DX Vec1 = CameraParts->GetMainCamera().GetCamVec() - StartPos; Vec1.y = 0.f; Vec1 = Vec1.normalized();
					{
						for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
							if (loop == PlayerMngr->GetWatchPlayerID()) { continue; }
							auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
							if (!chara->IsAlive() || !chara->GetCanLookByPlayer()) { continue; }
							Vector3DX Vec2 = (chara->GetEyePositionCache() - StartPos); Vec2.y = 0.f; Vec2 = Vec2.normalized();
							float Angle = DX_PI_F + Vector3DX::Angle(Vec1, Vec2) * (Vector3DX::Cross(Vec1, Vec2).y > 0.f ? 1.f : -1.f);

							xp1 = RetX - static_cast<int>(Scale2 * 1.5f * sin(Angle + deg2rad(5)));
							yp1 = RetY + static_cast<int>(Scale2 * 1.5f * cos(Angle + deg2rad(5)));
							xp2 = RetX - static_cast<int>(Scale2 * sin(Angle));
							yp2 = RetY + static_cast<int>(Scale2 * cos(Angle));
							xp3 = RetX - static_cast<int>(Scale2 * 1.5f * sin(Angle - deg2rad(5)));
							yp3 = RetY + static_cast<int>(Scale2 * 1.5f * cos(Angle - deg2rad(5)));
							unsigned int Color = (loop == ViewChara->GetGunPtrNow()->GetAutoAimID()) ? Green : DarkGreen;
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Color, 2);
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp2, yp2, xp3, yp3, Color, 2);
							DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp3, yp3, xp1, yp1, Color, 2);
						}
					}
					float BaseRad = ViewChara->GetLeanRad();
					xp1 = RetX - static_cast<int>(Scale * 1.0f * sin(deg2rad(90) + BaseRad));
					yp1 = RetY + static_cast<int>(Scale * 1.0f * cos(deg2rad(90) + BaseRad));
					xp2 = RetX - static_cast<int>(Scale * 0.85f * sin(deg2rad(90) + BaseRad));
					yp2 = RetY + static_cast<int>(Scale * 0.85f * cos(deg2rad(90) + BaseRad));
					DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);

					xp1 = RetX - static_cast<int>(Scale * 1.0f * sin(deg2rad(-90) + BaseRad));
					yp1 = RetY + static_cast<int>(Scale * 1.0f * cos(deg2rad(-90) + BaseRad));
					xp2 = RetX - static_cast<int>(Scale * 0.85f * sin(deg2rad(-90) + BaseRad));
					yp2 = RetY + static_cast<int>(Scale * 0.85f * cos(deg2rad(-90) + BaseRad));
					DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);

					xp1 = RetX - static_cast<int>(Scale * 1.0f * sin(deg2rad(0) + BaseRad));
					yp1 = RetY + static_cast<int>(Scale * 1.0f * cos(deg2rad(0) + BaseRad));
					xp2 = RetX - static_cast<int>(Scale * 0.5f * sin(deg2rad(0) + BaseRad));
					yp2 = RetY + static_cast<int>(Scale * 0.5f * cos(deg2rad(0) + BaseRad));
					DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);

					if (ViewChara->GetGunPtrNow()->GetAmmoAll() > 0) {
						xp1 = RetX - static_cast<int>(Scale * 1.5f * sin(deg2rad(60) + BaseRad));
						yp1 = RetY + static_cast<int>(Scale * 1.5f * cos(deg2rad(60) + BaseRad));
						xp2 = RetX - static_cast<int>(Scale * sin(deg2rad(60) + BaseRad));
						yp2 = RetY + static_cast<int>(Scale * cos(deg2rad(60) + BaseRad));
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Green, 2);
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1 - 100, yp1, xp1, yp1, Green, 2);

						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (18),
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, xp1 - 65, yp1 - 3, Green, Black, "/%d", ViewChara->GetGunPtrNow()->GetAmmoAll());
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (24),
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::BOTTOM, xp1 - 100, yp1 - 3, Green, Black, "%d", ViewChara->GetGunPtrNow()->GetAmmoNumTotal());

					}

					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
				}

				//スタートタイマー
				if (floatParam[1] > 0.f) {
					xp1 = 1920 / 2;
					yp1 = (340);
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, "%d:%05.2f",
						static_cast<int>(floatParam[1] / 60.f), static_cast<float>(static_cast<int>(floatParam[1]) % 60) + (floatParam[1] - static_cast<float>(static_cast<int>(floatParam[1]))));
				}

			}
			//方位磁針
			{
				Vector3DX Vec = ViewChara->GetEyeRotationCache().zvec2(); Vec.y = 0.f; Vec = Vec.normalized();
				float radian = std::atan2f(Vec.x, Vec.z);
				float degreeBase = rad2deg(radian);


				for (int loop = -25; loop <= 25; ++loop) {
					int degree = (360 + static_cast<int>(degreeBase) + loop) % 360;
					xp1 = 960 + static_cast<int>((static_cast<int>(degreeBase) - degreeBase + loop) * 10) + static_cast<int>(ViewChara->GetMoveEyePos().x * 100.f);
					yp1 = 1080 * 2 / 10 + static_cast<int>(ViewChara->GetMoveEyePos().y * 100.f);
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
			}
		}
	}
}
