#pragma warning(disable:4464)
#include "MainScene.hpp"
#include "../MainScene/NetworkBrowser.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MainGameScene::Load_Sub(void) noexcept {
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			//BG
			BackGround->Load();
			//
			BattleResourceMngr->Load();
			PlayerMngr->Init(NetWork::Player_num);
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				BattleResourceMngr->LoadChara("Chara", (PlayerID)index);
				BattleResourceMngr->LoadWeapon("Bamboo", (PlayerID)index);
			}
			//UI
			this->m_UIclass.Load();
			PauseMenuControl::LoadPause();
			HitMark::Instance()->Load();
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

			SetShadowScale(1.f);
			//
			BackGround->Init();
			//
			Vector3DX LightVec = Vector3DX::vget(1.3f, -0.5f, 0.05f); LightVec = LightVec.normalized();
			DrawParts->SetAmbientLight(LightVec, GetColorF(1.0f / 3.f, 0.96f / 3.f, 0.94f / 3.f, 1.0f));
			SetLightDifColor(GetColorF(1.0f, 0.96f, 0.94f, 1.0f));																// デフォルトライトのディフューズカラーを設定する

			auto& SecondLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec * -1.f);
			SetLightDifColorHandle(SecondLight.get(), GetColorF(0.5f, 0.5f, 0.3f, 0.1f));
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));

			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				auto& p = PlayerMngr->GetPlayer(index);
				auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
				c->SetViewID(0);
				//人の座標設定
				{
					Vector3DX pos_t;
					pos_t = Vector3DX::vget(0.f, 0.f, (-1.5f * Scale_Rate) * static_cast<float>(index * 2 - 1));

					Vector3DX EndPos = pos_t - Vector3DX::up() * 10.f * Scale_Rate;
					if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 10.f * Scale_Rate, &EndPos, true)) {
						pos_t = EndPos;
					}
					c->ValueSet((PlayerID)index, CharaTypeID::Team);
					c->MovePoint(deg2rad(0.f), deg2rad(180.f * static_cast<float>(index)), pos_t);
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
			HitMark::Instance()->Set();
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

			auto& Chara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();

			auto* Pad = PadControl::Instance();
			Pad->SetMouseMoveEnable(true);
			Pad->ChangeGuide(
				[]() {
					auto* KeyGuide = PadControl::Instance();
					if (DXDraw::Instance()->IsPause()) {
						KeyGuide->AddGuide(PADS::INTERACT, LocalizePool::Instance()->Get(9992));
						KeyGuide->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9991));
						KeyGuide->AddGuide(PADS::MOVE_W, "");
						KeyGuide->AddGuide(PADS::MOVE_S, "");
						KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9993));
					}
					else {
						KeyGuide->AddGuide(PADS::MOVE_W, "");
						KeyGuide->AddGuide(PADS::MOVE_S, "");
						KeyGuide->AddGuide(PADS::MOVE_A, "");
						KeyGuide->AddGuide(PADS::MOVE_D, "");
						KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9900));

						KeyGuide->AddGuide(PADS::SHOT, LocalizePool::Instance()->Get(9906));
						KeyGuide->AddGuide(PADS::AIM, LocalizePool::Instance()->Get(9908));
						KeyGuide->AddGuide(PADS::ULT, LocalizePool::Instance()->Get(9907));

						KeyGuide->AddGuide(PADS::WALK, LocalizePool::Instance()->Get(9903));
						KeyGuide->AddGuide(PADS::JUMP, LocalizePool::Instance()->Get(9905));
					}
				});
			if (DXDraw::Instance()->IsPause()) {
				Pad->SetMouseMoveEnable(false);
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
					MyInput.SetInputStart(Pad->GetLS_Y(), Pad->GetLS_X());
					MyInput.SetInputPADS(PADS::MOVE_W, Pad->GetKey(PADS::MOVE_W).press());
					MyInput.SetInputPADS(PADS::MOVE_S, Pad->GetKey(PADS::MOVE_S).press());
					MyInput.SetInputPADS(PADS::MOVE_A, Pad->GetKey(PADS::MOVE_A).press());
					MyInput.SetInputPADS(PADS::MOVE_D, Pad->GetKey(PADS::MOVE_D).press());

					MyInput.SetInputPADS(PADS::SHOT, Pad->GetKey(PADS::SHOT).press());
					MyInput.SetInputPADS(PADS::AIM, Pad->GetKey(PADS::AIM).press());
					MyInput.SetInputPADS(PADS::ULT, Pad->GetKey(PADS::ULT).press());

					MyInput.SetInputPADS(PADS::WALK, Pad->GetKey(PADS::WALK).press());
					MyInput.SetInputPADS(PADS::JUMP, Pad->GetKey(PADS::JUMP).press());
					{
						Vector2DX MSVec = Chara->GetBambooVec();
						MSVec.Set(
							std::clamp(MSVec.x + Pad->GetLS_Y() * deg2rad(0.1f), deg2rad(-10), deg2rad(10)),
							std::clamp(MSVec.y + Pad->GetLS_X() * deg2rad(0.1f), deg2rad(-30), deg2rad(30))
						);
						MyInput.SetxRad(MSVec.x);
						MyInput.SetyRad(MSVec.y);
					}
				}
				//ネットワーク
				auto* NetBrowser = NetWorkBrowser::Instance();
				if (NetBrowser->IsDataReady() && !m_NetWorkController) {
					m_NetWorkController = std::make_unique<NetWork::NetWorkController>();
					this->m_NetWorkController->Init(NetBrowser->GetClient(), NetBrowser->GetNetSetting().UsePort, NetBrowser->GetNetSetting().IP, NetBrowser->GetServerPlayer());
				}
				if (m_NetWorkController) {
					int32_t FreeData[10]{};
					FreeData[0] = static_cast<int>(Chara->GetCharaAction());
					this->m_NetWorkController->SetLocalData().SetMyPlayer(MyInput, Chara->GetMove(), Chara->GetDamageEvent(), FreeData);
					this->m_NetWorkController->Update();
				}
				//
				if (m_NetWorkController && m_NetWorkController->IsInGame()) {
					bool IsServerNotPlayer = !m_NetWorkController->GetClient() && !m_NetWorkController->GetServerPlayer();
					for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
						auto& p = PlayerMngr->GetPlayer(index);
						auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
						NetWork::PlayerNetData Ret = this->m_NetWorkController->GetLerpServerPlayerData((PlayerID)index);
						c->SetViewID(GetMyPlayerID());
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
								//アクションが違う場合には上書き
								if (Ret.GetFreeData()[0] != static_cast<int>(c->GetCharaAction())) {
									c->SetActionOverRide(static_cast<EnumArmAnimType>(Ret.GetFreeData()[0]));
								}
							}

						}
						//ダメージイベント処理
						Ret.AddDamageEvent(&this->m_DamageEvents);
					}
				}
				else {//オフライン
					for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
						auto& p = PlayerMngr->GetPlayer(index);
						auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
						c->SetViewID(GetMyPlayerID());
						if (index == GetMyPlayerID()) {
							c->SetInput(MyInput, true);
						}
						else {
							InputControl OtherInput;
							p->GetAI()->Execute(&OtherInput);
							{
								Vector2DX MSVec;
								MSVec.Set(
									std::clamp(c->GetBambooVec().x + Pad->GetLS_Y() * deg2rad(0.1f), deg2rad(-10), deg2rad(10)),
									std::clamp(c->GetBambooVec().y + Pad->GetLS_X() * deg2rad(0.1f), deg2rad(-30), deg2rad(30))
								);
								MyInput.SetxRad(MSVec.x);
								MyInput.SetyRad(MSVec.y);
							}
							c->SetInput(OtherInput, true);
						}
						//ダメージイベント処理
						c->AddDamageEvent(&this->m_DamageEvents);
					}
				}
				//ダメージイベント
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
					auto& p = PlayerMngr->GetPlayer(index);
					auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
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
			BackGround->FirstExecute();
			ObjMngr->LateExecuteObject();
			//視点
			{
				auto& ViewChara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
				auto* OptionParts = OPTION::Instance();
				//カメラ
				Vector3DX CamPos = ViewChara->GetEyePosition();
				Vector3DX CamVec = CamPos + ViewChara->GetEyeMatrix().zvec() * -1.f;
				CamVec += CameraShake::Instance()->GetCamShake();
				CamPos += CameraShake::Instance()->GetCamShake()*2.f;
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
				//info
				DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale_Rate * 0.3f, Scale_Rate * 20.f);
				//DoF
				PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.3f, Scale_Rate * 5.f, Scale_Rate * 0.1f, Scale_Rate * 20.f);
			}
			//竹刀判定
			{
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
					auto& p = PlayerMngr->GetPlayer(index);
					auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
					if (!c->IsAttacking()) { continue; }
					if (c->GetGuardOn()) { continue; }
#ifdef DEBUG
					printfDx("[%f]\n", c->GetWeaponPtr()->GetMoveSpeed());
#endif
					Vector3DX StartPos = c->GetWeaponPtr()->GetMove().GetPos();
					Vector3DX EndPos = c->GetWeaponPtr()->GetFramePosition(WeaponObject::WeaponFrame::End);
					StartPos = StartPos + (EndPos - StartPos) * 0.5f;

					for (int index2 = 0; index2 < PlayerMngr->GetPlayerNum(); ++index2) {
						if (index == index2) { continue; }
						auto& p2 = PlayerMngr->GetPlayer(index2);
						auto& tgt = (std::shared_ptr<CharacterObject::CharacterClass>&)p2->GetChara();
						HitPoint Damage = static_cast<HitPoint>(100.f * c->GetWeaponPtr()->GetMoveSpeed() / 5.f);
						tgt->CheckDamageRay(&Damage, c->GetMyPlayerID(), StartPos, &EndPos);
					}
				}
			}
			//コンカッション
			{
				if (Chara->PopConcussionSwitch()) {
					m_Concussion = 1.f;
				}
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
				if (GetIsFirstLoop()) {
					this->m_UIclass.InitGaugeParam(0, static_cast<int>(0.f), static_cast<int>(Chara->GetYaTimerMax()));
					this->m_UIclass.InitGaugeParam(1, static_cast<int>(Chara->GetStaminaMax()), static_cast<int>(Chara->GetStaminaMax()));
					this->m_UIclass.InitGaugeParam(0, static_cast<int>(0.f), static_cast<int>(Chara->GetGuardCoolDownTimerMax()));
				}
				//NvsN
				this->m_UIclass.SetIntParam(0, 0);
				this->m_UIclass.SetIntParam(1, 0);
				//timer
				this->m_UIclass.SetfloatParam(0, 0.f);
				//心拍数
				this->m_UIclass.SetIntParam(2, static_cast<int>(Chara->GetHeartRate()));
				this->m_UIclass.SetfloatParam(1, Chara->GetHeartRatePow());
				//ゲージ
				this->m_UIclass.SetGaugeParam(0, static_cast<int>((Chara->GetYaTimerMax() - Chara->GetYaTimer()) * 10000.f), static_cast<int>(Chara->GetYaTimerMax() * 10000.f), 15);
				this->m_UIclass.SetGaugeParam(1, static_cast<int>(Chara->GetStamina() * 10000.f), static_cast<int>(Chara->GetStaminaMax() * 10000.f), 15);
				this->m_UIclass.SetGaugeParam(2, static_cast<int>((Chara->GetGuardCoolDownTimer()) * 100.f), static_cast<int>(Chara->GetGuardCoolDownTimerMax() * 100.f), 0);
			}
#ifdef DEBUG
			DebugParts->SetPoint("Execute=0.7ms");
#endif // DEBUG
			return true;
		}
		void			MainGameScene::Dispose_Sub(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			//使い回しオブジェ系
			BackGround->Dispose();
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
			HitMark::Instance()->Dispose();
		}

		//
		void			MainGameScene::BG_Draw_Sub(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			BackGround->BG_Draw();
		}
		void			MainGameScene::ShadowDraw_Far_Sub(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			BackGround->Shadow_Draw_Far();
		}
		void			MainGameScene::ShadowDraw_Sub(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			BackGround->Shadow_Draw();
			ObjectManager::Instance()->Draw_Shadow();
		}
		void			MainGameScene::CubeMap_Sub(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			BackGround->Draw();
		}

		void MainGameScene::SetShadowDraw_Sub(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			BackGround->Draw();
			ObjectManager::Instance()->Draw();
		}

		void			MainGameScene::MainDraw_Sub(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			SetFogStartEnd(DrawParts->GetMainCamera().GetCamNear(), DrawParts->GetMainCamera().GetCamFar() * 2.f);
			BackGround->Draw();
			ObjectManager::Instance()->Draw();
			//ObjectManager::Instance()->Draw_Depth();
			for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
				PlayerMngr->GetPlayer(i)->GetAI()->Draw();
			}
			HitMark::Instance()->Update();
		}
		//UI表示
		void			MainGameScene::DrawUI_Base_Sub(void) noexcept {
			HitMark::Instance()->Draw();
			FadeControl::DrawFade();
			//UI
			if (!DXDraw::Instance()->IsPause()) {
				this->m_UIclass.Draw();
			}
			else {
				PauseMenuControl::DrawPause();
			}
			//通信設定
			auto* NetBrowser = NetWorkBrowser::Instance();
			NetBrowser->Draw();
			if (m_NetWorkController) {
				auto* DrawParts = DXDraw::Instance();
				if (m_NetWorkController->GetPing() >= 0.f) {
					WindowSystem::SetMsg(DrawParts->GetUIY(1920), DrawParts->GetUIY(32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "Ping:%3dms", static_cast<int>(m_NetWorkController->GetPing()));
				}
				else {
					if (m_NetWorkController->GetClient()) {
						WindowSystem::SetMsg(DrawParts->GetUIY(1920), DrawParts->GetUIY(32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, Red, Black, "Lost Connection");
					}
					else {
						WindowSystem::SetMsg(DrawParts->GetUIY(1920), DrawParts->GetUIY(32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "Ping:---ms");
					}
				}
			}
		}
	}
}
