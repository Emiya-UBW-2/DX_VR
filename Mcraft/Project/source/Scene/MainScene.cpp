#pragma warning(disable:4464)
#include "../MainScene/NetworkBrowser.hpp"
#include "MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MainGameScene::Load_Sub(void) noexcept {
			//ロード
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			//BG
			BackGround->Load();
			//
			GunAnimManager::Instance()->Load("data/CharaAnime/");
			//
			BattleResourceMngr->Load();
			PlayerMngr->Init(NetWork::Player_num, 0);

			BattleResourceMngr->LoadChara("Suit", GetMyPlayerID());
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
				Chara->SetPlayMode(false);
			}
			LoadGun("M16-4", GetMyPlayerID(), false, 0);
			//UI
			this->m_UIclass.Load();
			PauseMenuControl::LoadPause();
			//
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			//
			BattleResourceMngr->Set();

			SetShadowScale(2.f);
			//
			BackGround->Init();
			//
			Vector3DX LightVec = Vector3DX::vget(-0.3f, -0.3f, -0.15f); LightVec = LightVec.normalized();
			DrawParts->SetAmbientLight(LightVec, GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
			SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));																// デフォルトライトのディフューズカラーを設定する
			//Cam
			DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));
			//info
			constexpr float Max = std::min(std::min(BackGround::DrawMaxXPlus, BackGround::DrawMaxZPlus), BackGround::DrawMaxYPlus) * BackGround::CellScale;
			float SQRTMax = Max;
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale_Rate * 0.03f, SQRTMax);
			//DoF
			PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.3f, Scale_Rate * 5.f, Scale_Rate * 0.1f, SQRTMax * 2.f);
			//Fog
			SetFogMode(DX_FOGMODE_LINEAR);
			SetFogStartEnd(SQRTMax, SQRTMax * 20.f);
			SetFogColor(114, 120, 128);
			//
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				auto& p = PlayerMngr->GetPlayer(index);
				auto& c = (std::shared_ptr<Sceneclass::CharacterClass>&)p->GetChara();
				//人の座標設定
				{
					Vector3DX pos_t;
					pos_t = Vector3DX::vget(0.f, 1.f * Scale_Rate, 0.f);

					Vector3DX EndPos = pos_t - Vector3DX::up() * 2.f * Scale_Rate;
					if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 2.f * Scale_Rate, &EndPos)) {
						pos_t = EndPos;
					}
					c->ValueSet((PlayerID)index, true, CharaTypeID::Team);
					c->MovePoint(deg2rad(0.f), deg2rad(180.f * static_cast<float>(index)), pos_t, 0);
				}
				p->GetAI()->Init((PlayerID)index);
			}
			//UI
			this->m_UIclass.Set();
			//this->m_UILayer = UISystem::Instance()->AddUI("data/UI/MainLoop.json");
			//
			this->m_DamageEvents.clear();
			auto* NetBrowser = NetWorkBrowser::Instance();
			NetBrowser->Init();
			PauseMenuControl::SetPause();
			FadeControl::SetFade();
			this->m_IsEnd = false;
			EffectControl::Init();
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
#ifdef DEBUG
			/*
			{
				auto* DrawParts = DXDraw::Instance();
				if (CheckHitKeyWithCheck(KEY_INPUT_1) != 0) {
					m_D1 = std::clamp(m_D1 - 0.1f * 1.f / DrawParts->GetFps(), 0.f, 1.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_2) != 0) {
					m_D1 = std::clamp(m_D1 + 0.1f * 1.f / DrawParts->GetFps(), 0.f, 1.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_3) != 0) {
					m_D2 = std::clamp(m_D2 - 0.1f * 1.f / DrawParts->GetFps(), 0.f, 1.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_4) != 0) {
					m_D2 = std::clamp(m_D2 + 0.1f * 1.f / DrawParts->GetFps(), 0.f, 1.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_5) != 0) {
					m_D3 = std::clamp(m_D3 - 0.1f * 1.f / DrawParts->GetFps(), 1.f, 10.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_6) != 0) {
					m_D3 = std::clamp(m_D3 + 0.1f * 1.f / DrawParts->GetFps(), 1.f, 10.f);
				}
				printfDx("Dif[%5.2f]\n", m_D1*255.f);
				printfDx("Spc[%5.2f]\n", m_D2*255.f);
				printfDx("Amb[%5.2f]\n", m_D3);
				printfDx("\n");
			}
			PostPassParts->SetLevelFilter(m_D1*255.f, m_D2*255.f, m_D3);
			//*/
			PostPassParts->SetLevelFilter(38, 154, 1.f);
#else
			PostPassParts->SetLevelFilter(38, 154, 1.f);
#endif
			PauseMenuControl::UpdatePause();
			if (PauseMenuControl::IsRetire()) {
				this->m_IsEnd = true;
			}
			if (this->m_IsEnd && FadeControl::IsFadeAll()) {
				return false;
			}
			FadeControl::SetBlackOut(this->m_IsEnd);

			FadeControl::UpdateFade();
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			auto& Chara = (std::shared_ptr<Sceneclass::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();

			auto* Pad = PadControl::Instance();
			Pad->SetMouseMoveEnable(true);
			Pad->ChangeGuide(
				[]() {
					auto* Pad = PadControl::Instance();
					if (DXDraw::Instance()->IsPause()) {
						Pad->AddGuide(PADS::INTERACT, LocalizePool::Instance()->Get(9992));
						Pad->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9991));
						Pad->AddGuide(PADS::MOVE_W, "");
						Pad->AddGuide(PADS::MOVE_S, "");
						Pad->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9993));
					}
					else {
						Pad->AddGuide(PADS::MOVE_W, "");
						Pad->AddGuide(PADS::MOVE_S, "");
						Pad->AddGuide(PADS::MOVE_A, "");
						Pad->AddGuide(PADS::MOVE_D, "");
						Pad->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9900));

						Pad->AddGuide(PADS::SHOT, LocalizePool::Instance()->Get(9906));
						Pad->AddGuide(PADS::AIM, LocalizePool::Instance()->Get(9908));
						Pad->AddGuide(PADS::ULT, LocalizePool::Instance()->Get(9907));

						Pad->AddGuide(PADS::WALK, LocalizePool::Instance()->Get(9903));
						Pad->AddGuide(PADS::JUMP, LocalizePool::Instance()->Get(9905));
					}
				});
			if (DXDraw::Instance()->IsPause()) {
				Pad->SetMouseMoveEnable(false);
				BackGround->SettingChange();
				if (!m_NetWorkController) {
					return true;
				}
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("Execute=Start");
#endif // DEBUG
			//FirstDoingv
			if (GetIsFirstLoop()) {
				//SE->Get(static_cast<int>(SoundEnum::Environment)).Play(0, DX_PLAYTYPE_LOOP, TRUE);
			}
			//Input,AI
			{
				InputControl MyInput;
				if (DXDraw::Instance()->IsPause() || !FadeControl::IsFadeClear()) {
					MyInput.ResetAllInput();
				}
				else {
					MyInput.SetInputStart(Pad->GetLS_Y() / 100.f, Pad->GetLS_X() / 100.f);
					MyInput.SetInputPADS(PADS::MOVE_W, Pad->GetKey(PADS::MOVE_W).press());
					MyInput.SetInputPADS(PADS::MOVE_S, Pad->GetKey(PADS::MOVE_S).press());
					MyInput.SetInputPADS(PADS::MOVE_A, Pad->GetKey(PADS::MOVE_A).press());
					MyInput.SetInputPADS(PADS::MOVE_D, Pad->GetKey(PADS::MOVE_D).press());
					MyInput.SetInputPADS(PADS::RUN, Pad->GetKey(PADS::RUN).press());
					MyInput.SetInputPADS(PADS::LEAN_L, Pad->GetKey(PADS::LEAN_L).press());
					MyInput.SetInputPADS(PADS::LEAN_R, Pad->GetKey(PADS::LEAN_R).press());
					MyInput.SetInputPADS(PADS::MELEE, Pad->GetKey(PADS::MELEE).press());
					MyInput.SetInputPADS(PADS::RELOAD, Pad->GetKey(PADS::RELOAD).press());
					MyInput.SetInputPADS(PADS::INTERACT, Pad->GetKey(PADS::INTERACT).press());
					MyInput.SetInputPADS(PADS::SQUAT, Pad->GetKey(PADS::SQUAT).press());
					MyInput.SetInputPADS(PADS::SHOT, Pad->GetKey(PADS::SHOT).press() && !DXDraw::Instance()->IsPause());
					MyInput.SetInputPADS(PADS::AIM, Pad->GetKey(PADS::AIM).press() && !DXDraw::Instance()->IsPause());
					MyInput.SetInputPADS(PADS::ULT, Pad->GetKey(PADS::ULT).press());
					MyInput.SetInputPADS(PADS::THROW, Pad->GetKey(PADS::THROW).press());
					MyInput.SetInputPADS(PADS::CHECK, Pad->GetKey(PADS::CHECK).press());
					MyInput.SetInputPADS(PADS::WALK, Pad->GetKey(PADS::WALK).press());
					MyInput.SetInputPADS(PADS::JUMP, Pad->GetKey(PADS::JUMP).press());
				}
				//ネットワーク
				auto* NetBrowser = NetWorkBrowser::Instance();
				if (NetBrowser->IsDataReady() && !m_NetWorkController) {
					m_NetWorkController = std::make_unique<NetWork::NetWorkController>();
					this->m_NetWorkController->Init(NetBrowser->GetClient(), NetBrowser->GetNetSetting().UsePort, NetBrowser->GetNetSetting().IP, NetBrowser->GetServerPlayer());
				}
				if (m_NetWorkController) {
					int32_t FreeData[10]{};
					this->m_NetWorkController->SetLocalData().SetMyPlayer(MyInput, Chara->GetMove(), Chara->GetDamageEvent(), FreeData);
					this->m_NetWorkController->Update();
				}
				//
				if (m_NetWorkController && m_NetWorkController->IsInGame()) {
					bool IsServerNotPlayer = !m_NetWorkController->GetClient() && !m_NetWorkController->GetServerPlayer();
					for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
						auto& p = PlayerMngr->GetPlayer(index);
						auto& c = (std::shared_ptr<Sceneclass::CharacterClass>&)p->GetChara();
						NetWork::PlayerNetData Ret = this->m_NetWorkController->GetLerpServerPlayerData((PlayerID)index);
						if (index == GetMyPlayerID() && !IsServerNotPlayer) {
							c->SetInput(Ret.GetInput(), true);//自身が動かすもの
						}
						else {//サーバーからのデータで動くもの
							//サーバーがCPUを動かす場合
							if (!m_NetWorkController->GetClient()) {
								//cpu
								//p->GetAI()->Execute(&MyInput);
							}
							c->SetInput(Ret.GetInput(), true);
							bool override_true = true;
							//override_true = Ret.GetIsActive();
							if (override_true) {
								c->SetMoveOverRide(Ret.GetMove());
							}

						}
						//ダメージイベント処理
						Ret.AddDamageEvent(&this->m_DamageEvents);
					}
				}
				else {//オフライン
					for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
						auto& p = PlayerMngr->GetPlayer(index);
						auto& c = (std::shared_ptr<Sceneclass::CharacterClass>&)p->GetChara();
						if (index == GetMyPlayerID()) {
							c->SetInput(MyInput, true);
						}
						else {
							InputControl OtherInput;
							p->GetAI()->Execute(&OtherInput, false);
							c->SetInput(OtherInput, true);
						}
						//ダメージイベント処理
						c->AddDamageEvent(&this->m_DamageEvents);
					}
				}
				//ダメージイベント
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
					auto& p = PlayerMngr->GetPlayer(index);
					auto& c = (std::shared_ptr<Sceneclass::CharacterClass>&)p->GetChara();
					for (int j = 0, Num = static_cast<int>(this->m_DamageEvents.size()); j < Num; ++j) {
						if (c->SetDamageEvent(this->m_DamageEvents[static_cast<size_t>(j)])) {
							std::swap(this->m_DamageEvents.back(), m_DamageEvents[static_cast<size_t>(j)]);
							this->m_DamageEvents.pop_back();
							--Num;
							--j;
						}
					}
				}
			}
			//Execute
			ObjMngr->ExecuteObject();
			ObjMngr->LateExecuteObject();
			UpdateBullet();							//弾の更新
			//視点
			{
				auto& ViewChara = (std::shared_ptr<Sceneclass::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
				//カメラ
				Vector3DX CamPos = ViewChara->GetEyeMatrix().pos() + CameraShake::Instance()->GetCamShake();
				Vector3DX CamVec = CamPos + ViewChara->GetEyeMatrix().zvec() * -1.f;
				CamVec += CameraShake::Instance()->GetCamShake();
				CamPos += CameraShake::Instance()->GetCamShake() * 2.f;
#ifdef DEBUG
				if (CheckHitKeyWithCheck(KEY_INPUT_F1) != 0) {
					DBG_CamSel = -1;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_F2) != 0) {
					DBG_CamSel = 0;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_F3) != 0) {
					DBG_CamSel = 1;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_F4) != 0) {
					DBG_CamSel = 2;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_F5) != 0) {
					DBG_CamSel = 3;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_F6) != 0) {
					DBG_CamSel = 4;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_F7) != 0) {
					DBG_CamSel = 5;
				}
				switch (DBG_CamSel) {
				case 0:
				case 3:
					CamVec = CamPos;
					CamPos += ViewChara->GetEyeMatrix().xvec() * (2.f * Scale_Rate);
					break;
				case 1:
				case 4:
					CamVec = CamPos;
					CamPos += ViewChara->GetEyeMatrix().yvec() * (2.f * Scale_Rate) + ViewChara->GetEyeMatrix().zvec() * 0.1f;
					break;
				case 2:
				case 5:
					CamVec = CamPos;
					CamPos += ViewChara->GetEyeMatrix().zvec() * (-2.f * Scale_Rate);
					break;
				default:
					break;
				}
#endif
				DrawParts->SetMainCamera().SetCamPos(CamPos, CamVec, ViewChara->GetEyeMatrix().yvec());
				auto fov_t = DrawParts->GetMainCamera().GetCamFov();
				auto near_t = DrawParts->GetMainCamera().GetCamNear();
				auto far_t = DrawParts->GetMainCamera().GetCamFar();
				//fov
				{
					auto* OptionParts = OPTION::Instance();
					float fov = deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov));
					if (Chara->GetIsADS()) {
						fov -= deg2rad(15);
						fov /= std::max(1.f, Chara->GetSightZoomSize() / 2.f);
					}
					else if (Chara->GetRun()) {
						fov += deg2rad(5);
					}
					if (Chara->GetMeleeSwitch()) {
						fov += deg2rad(15);
						Easing(&fov_t, fov, 0.8f, EasingType::OutExpo);
					}
					else if (Chara->GetGunPtrNow() && Chara->GetGunPtrNow()->GetShotSwitch()) {
						fov -= deg2rad(5);
						Easing(&fov_t, fov, 0.5f, EasingType::OutExpo);
					}
					else {
						Easing(&fov_t, fov, 0.8f, EasingType::OutExpo);
					}
				}
				DrawParts->SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
			}
			//コンカッション
			{
				//if (Chara->PopConcussionSwitch()) {
				//	m_Concussion = 1.f;
				//}
				DrawParts->Set_is_Blackout(m_Concussion > 0.f);
				if (m_Concussion == 1.f) {
					CameraShake::Instance()->SetCamShake(0.5f, 0.01f * Scale_Rate);
				}
				if (m_Concussion > 0.9f) {
					Easing(&m_ConcussionPer, 1.f, 0.1f, EasingType::OutExpo);
				}
				else if (m_Concussion > 0.25f) {
					if (m_ConcussionPer > 0.25f) {
						Easing(&m_ConcussionPer, 0.f, 0.8f, EasingType::OutExpo);
					}
					else {
						m_ConcussionPer = 0.25f;
					}
				}
				else {
					Easing(&m_ConcussionPer, 0.f, 0.8f, EasingType::OutExpo);
				}
				DrawParts->Set_Per_Blackout(m_ConcussionPer * 2.f);
				m_Concussion = std::max(m_Concussion - 1.f / DrawParts->GetFps(), 0.f);
			}
			BackGround->Execute();
			//UIパラメーター
			{
				//NvsN
				this->m_UIclass.SetIntParam(0, 0);
				this->m_UIclass.SetIntParam(1, 0);
				//timer
				this->m_UIclass.SetfloatParam(0, 0.f);
			}
#ifdef DEBUG
			DebugParts->SetPoint("Execute=0.7ms");
#endif // DEBUG
			EffectControl::Execute();
			return true;
		}
		void			MainGameScene::Dispose_Sub(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			//使い回しオブジェ系
			BackGround->Dispose();
			GunsModify::DisposeSlots();
			//
			if (m_NetWorkController) {
				m_NetWorkController->Dispose();
				m_NetWorkController.reset();
			}
			{
				auto* DrawParts = DXDraw::Instance();
				PostPassEffect::Instance()->SetLevelFilter(0, 255, 1.f);
				DrawParts->SetAberrationPower(1.f);
				DrawParts->Set_is_Blackout(false);
				DrawParts->Set_Per_Blackout(0.f);
				DrawParts->Set_is_lens(false);
				DrawParts->Set_zoom_lens(1.f);
			}
			UISystem::Instance()->DelUI(m_UILayer);
			EffectControl::Dispose();
			if (this->m_IsEnd) {//タイトルに戻る
				SetNextSelect(0);
			}
			else {//次のシーンへ
				SetNextSelect(1);
			}
		}
		void			MainGameScene::Dispose_Load_Sub(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->Dispose();
			this->m_UIclass.Dispose();
			PlayerMngr->Dispose();
			ObjectManager::Instance()->DeleteAll();
			PauseMenuControl::DisposePause();
		}

		//
		void			MainGameScene::BG_Draw_Sub(void) noexcept {
			BackGround::BackGroundClass::Instance()->BG_Draw();
		}
		void			MainGameScene::ShadowDraw_Far_Sub(void) noexcept {
		}
		void			MainGameScene::ShadowDraw_Sub(void) noexcept {
			BackGround::BackGroundClass::Instance()->Shadow_Draw();
			ObjectManager::Instance()->Draw_Shadow();
		}
		void			MainGameScene::CubeMap_Sub(void) noexcept {
			BackGround::BackGroundClass::Instance()->Draw();
		}

		void MainGameScene::SetShadowDraw_Rigid_Sub(void) noexcept {
			BackGround::BackGroundClass::Instance()->SetShadow_Draw_Rigid();
		}

		void MainGameScene::SetShadowDraw_Sub(void) noexcept {
			ObjectManager::Instance()->Draw();
		}

		void			MainGameScene::MainDraw_Sub(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			SetFogEnable(TRUE);
			BackGround::BackGroundClass::Instance()->Draw();
			ObjectManager::Instance()->Draw();
			//ObjectManager::Instance()->Draw_Depth();
			for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
				PlayerMngr->GetPlayer(i)->GetAI()->Draw();
			}
			SetFogEnable(FALSE);


			//シェーダー描画用パラメーターセット
			auto* DrawParts = DXDraw::Instance();

			auto& Chara = (std::shared_ptr<Sceneclass::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			if (Chara->GetGunPtrNow()) {
				Chara->GetGunPtrNow()->UpdateReticle();
				float Zoom = Chara->GetSightZoomSize();
				bool IsActive = Chara->GetGunPtrNow()->IsActiveReticle() && Zoom > 1.f;
				DrawParts->Set_is_lens(IsActive);
				DrawParts->Set_zoom_lens(std::max(1.f, Zoom / 2.f));
				if (IsActive) {
					DrawParts->Set_xp_lens(Chara->GetGunPtrNow()->GetLensXPos());
					DrawParts->Set_yp_lens(Chara->GetGunPtrNow()->GetLensYPos());
					DrawParts->Set_size_lens(Chara->GetGunPtrNow()->GetLensSize());
				}
			}
			else {
				DrawParts->Set_is_lens(false);
				DrawParts->Set_zoom_lens(1.f);
			}
		}
		//UI表示
		void			MainGameScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			if (!Chara->IsAlive()) { return; }
			//レティクル表示
			if (Chara->GetGunPtrNow()) {
				if (Chara->GetGunPtrNow()->IsActiveReticle() && Chara->GetGunPtrNow()->GetSightPtr() &&
					!((Chara->GetADSPer() < 0.8f) && Chara->GetSightZoomSize() > 1.f)) {
					auto* WindowParts = WindowSystem::DrawControl::Instance();
					WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &(*Chara->GetGunPtrNow()->GetSightPtr())->GetModData()->GetReitcleGraph(),
						static_cast<int>(Chara->GetGunPtrNow()->GetReticleXPos() * DrawParts->GetUIY(1980) / DrawParts->GetScreenY(1980)),
						static_cast<int>(Chara->GetGunPtrNow()->GetReticleYPos() * DrawParts->GetUIY(1080) / DrawParts->GetScreenY(1080)),
						1.f, Chara->GetLeanRad(), true);
				}
			}


			FadeControl::DrawFade();
			//UI
			if (!DXDraw::Instance()->IsPause()) {
				this->m_UIclass.Draw();
			}
			//通信設定
			//auto* NetBrowser = NetWorkBrowser::Instance();
			//NetBrowser->Draw();
			if (m_NetWorkController) {
				if (m_NetWorkController->GetPing() >= 0.f) {
					WindowSystem::SetMsg(UIWidth, DrawParts->GetUIY(32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "Ping:%3dms", static_cast<int>(m_NetWorkController->GetPing()));
				}
				else {
					if (m_NetWorkController->GetClient()) {
						WindowSystem::SetMsg(UIWidth, DrawParts->GetUIY(32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, Red, Black, "Lost Connection");
					}
					else {
						WindowSystem::SetMsg(UIWidth, DrawParts->GetUIY(32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "Ping:---ms");
					}
				}
			}
		}
		void MainGameScene::DrawUI_In_Sub(void) noexcept {
			if (DXDraw::Instance()->IsPause()) {
				PauseMenuControl::DrawPause();
			}
		}
		//load
		void MainGameScene::LoadGun(const std::string& FolderName, PlayerID ID, bool IsPreset, int Sel) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->LoadCharaGun(FolderName, ID, Sel);
			auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(ID)->GetChara();
			GunsModify::CreateSelData(c->GetGunPtr(Sel), IsPreset);
			c->GetGunPtr(Sel)->Init_Gun();
		}
		void MainGameScene::UpdateBullet(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* SE = SoundPool::Instance();

			int loop = 0;
			while (true) {
				auto ammo = ObjMngr->GetObj((int)ObjType::Ammo, loop);
				if (ammo != nullptr) {
					auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
					if (a->IsActive()) {
						//AmmoClass
						Vector3DX repos_tmp = a->GetRePos();
						Vector3DX pos_tmp = a->GetMove().GetPos();

						Vector3DX norm_tmp;
						auto ColResGround = BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, &norm_tmp);
						bool is_HitAll = false;
						for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
							auto& tgt = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
							if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
							HitPoint Damage = a->GetDamage();
							ArmerPoint ArmerDamage = 0;
							if (tgt->CheckDamageRay(&Damage, &ArmerDamage, true, (PlayerID)a->GetShootedID(), repos_tmp, &pos_tmp)) {
								a->Penetrate(Damage, ArmerDamage);
								is_HitAll = true;
							}
						}
						if (ColResGround && !is_HitAll) {
							a->HitGround(pos_tmp);
							EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_tmp, norm_tmp, a->GetCaliberSize() / 0.02f * Scale_Rate);
							SE->Get((int)SoundEnum::HitGround0 + GetRand(5 - 1)).Play_3D(0, pos_tmp, Scale_Rate * 10.f);
						}
					}
				}
				else {
					break;
				}
				loop++;
			}
		}
	}
}
