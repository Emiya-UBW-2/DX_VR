#include	"MainSceneSub.hpp"

#include	"Player/Player.hpp"

#include	"../MainScene/Object/ItemObj.hpp"

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

			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
			m_PrevItemID.resize(ViewPlayer->GetInventory().size());
			m_ItemAnimTimer.resize(ViewPlayer->GetInventory().size());
			for (int loop = 0; loop < m_PrevItemID.size(); ++loop) {
				m_PrevItemID.at(loop) = ViewPlayer->GetInventory().at(loop).first;
			}
			for (int loop = 0; loop < m_ItemAnimTimer.size(); ++loop) {
				m_ItemAnimTimer.at(loop) = 0.f;
			}
		}
		void			MainSceneUI::Update(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();

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
				if (m_AmmoNumTotal != ViewChara->GetGunPtrNow()->GetAmmoNumTotal()) {
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
				if (m_RadPrev != Now) {
					if (
						(m_RadPrev == 0 && Now == 1) ||
						(m_RadPrev == 1 && Now == 2) ||
						(m_RadPrev == 2 && Now == 0)
						) {
						m_RadR = -1.f;
					}
					else if (
						(m_RadPrev == 0 && Now == 2) ||
						(m_RadPrev == 2 && Now == 1) ||
						(m_RadPrev == 1 && Now == 0)
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
				auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
				int loop = 0;
				for (auto& ID : ViewPlayer->GetInventory()) {
					bool Prev = m_PrevItemID.at(loop) != InvalidID;
					bool Now = ID.first != InvalidID;
					if (Prev != Now) {
						//開始アニメ
						//終了アニメ
						m_ItemAnimTimer.at(loop) += DXLib_refParts->GetDeltaTime();
						if (m_ItemAnimTimer.at(loop) >= 0.3f) {
							m_ItemAnimTimer.at(loop) = 0.f;
							m_PrevItemID.at(loop) = ID.first;
						}
					}
					else {
						m_ItemAnimTimer.at(loop) = 0.f;
					}
					++loop;
				}
			}

			{
				float kirogram = static_cast<float>(ViewChara->GetWeight_gram()) / 1000.f;
				if (std::abs(kirogram - m_Gram) > 5.f) {
					Easing(&m_Gram, kirogram, 0.95f, EasingType::OutExpo);
				}
				else if(std::abs(kirogram - m_Gram) > 0.02f){
					m_Gram += DXLib_refParts->GetDeltaTime() / 0.1f * ((kirogram - m_Gram) > 0.f ? 1.f : -1.f);
				}
				else {
					m_Gram = kirogram;
				}
			}

		}
		void			MainSceneUI::Draw(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
			auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();

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
				for (auto& ID : ViewPlayer->GetInventory()) {
					float Yadd = 0.f;
					float Scale = 1.f;
					float Alpha = 1.f;
					int DrawID = ID.first;
					bool Prev = m_PrevItemID.at(loop) != InvalidID;
					bool Now = ID.first != InvalidID;
					{
						float Seek = m_ItemAnimTimer.at(loop) / 0.3f;
						if (!Prev && Now) {
							//開始アニメ
							DrawID = ID.first;
							Yadd = 0.f;
							if (Seek < 0.8f) {
								Scale = Lerp(0.f, 1.2f, Seek / 0.8f);
							}
							else {
								Scale = Lerp(1.2f, 1.f, (Seek - 0.8f) / (1.f - 0.8f));
							}
							Alpha = Lerp(0.f, 1.f, Seek);
						}
						else if (Prev && !Now) {
							//終了アニメ
							DrawID = m_PrevItemID.at(loop);
							if (Seek < 0.8f) {
								Yadd = Lerp(0.f, 128.f, Seek / 0.8f);
							}
							else {
								Yadd = 128.f;
							}
							Scale = 1.f;
							Alpha = Lerp(1.f, 0.f, Seek);
						}
						else {
							Yadd = 0.f;
							Scale = 1.f;
							Alpha = 1.f;
						}
					}


					xp1 = 400 - Height * (static_cast<int>(ViewPlayer->GetInventory().size()) / 2) + Height * loop;

					DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 215);
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &this->m_ItembackGraph, xp1, yp1, 96.f / 512.f, 0.f, true);
					if (DrawID != InvalidID) {
						auto& item = Objects::ItemObjDataManager::Instance()->GetList().at(DrawID);
						DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, static_cast<int>(255.f * Alpha));
						DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &item->GetIconGraph(), xp1, yp1 - static_cast<int>(Yadd), (128.f / 512.f) * Scale, 0.f, true);

						if (ID.second > 0.f) {
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, 255, 0);
							DrawCtrls->SetDrawCircleGauge(WindowSystem::DrawLayer::Normal, &this->DeleteItemGraph, xp1, yp1, 100.f * (ID.second / 0.5f), 0.f, 64.f / 128.f);
							DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
						}
					}
					++loop;
				}
				{
					xp1 = 400;
					yp1 = (1080 * 4 / 5) + 64;

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
						Color, Black, "Weight : %05.2f kg", m_Gram);
				}
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
			//タイム,スコア
			{
				xp1 = (30);
				yp1 = (10);
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, "TIME");
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (32),
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xp1 + (240), yp1, White, Black, "%d:%05.2f",
					static_cast<int>(floatParam[0] / 60.0f), static_cast<float>(static_cast<int>(floatParam[0]) % 60) + (floatParam[0] - static_cast<float>(static_cast<int>(floatParam[0]))));



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
								unsigned int Color = (loop == ViewChara->GetGunPtrNow()->GetAutoAimID()) ? Red : Green;
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
							for (auto& ID : ViewPlayer->SetInventory()) {
								if (ID.first != InvalidID) {
									auto& item = Objects::ItemObjDataManager::Instance()->GetList().at(ID.first);
									switch (item->GetItemType()) {
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
							}

							if (HasSpare) {
								xp1 = xp1 + 64;
								yp1 = yp1 + 64;

								KeyGuideParts->DrawButton(xp1 - 32, yp1 - 32, KeyGuide::GetPADStoOffset(Controls::PADS::HEALARMOR));

								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (16),
									FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, Red, Black, LocalizeParts->Get(3000));
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
								int Bright = Lerp(255, 128, std::abs(m_RadR));
								DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 0, Bright, 0);
								float S = Lerp(0.3f, 0.2f, std::abs(m_RadR)) * scale;
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
									xp1 - 100 - static_cast<int>(m_AmmoRandR * m_AmmoInPer * 10.f),
									yp1 - 3 - static_cast<int>(m_AmmoInPer * 3.f),
									Color, Black, "%d", ViewChara->GetGunPtrNow()->GetAmmoNumTotal());

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
					if ((1.f - IsDrawAimUIPer) > 0.01f) {
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

			}
			//方位磁針
			{
				Vector3DX Vec = ViewChara->GetEyeRotationCache().zvec2(); Vec.y = 0.0f; Vec = Vec.normalized();
				float radian = std::atan2f(Vec.x, Vec.z);
				float degreeBase = rad2deg(radian);


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
			}
			//
		}
	}
}
