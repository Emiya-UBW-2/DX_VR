#pragma warning(disable:4464)
#include "MainScene.hpp"
#include "../MainScene/NetworkBrowser.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MAINLOOP::Load_Sub(void) noexcept {
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BackGround = BackGroundClass::Instance();
			//BG
			BackGround->Load();
			//
			BattleResourceMngr->Load();
			PlayerMngr->Init(Player_num);
			for (int index = 0; index < Player_num; index++) {
				BattleResourceMngr->LoadChara("Chara", (PlayerID)index);
				BattleResourceMngr->LoadGun("Bamboo", (PlayerID)index);
			}
			//UI
			this->m_UIclass.Load();
		}
		void			MAINLOOP::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BackGround = BackGroundClass::Instance();
			//
			BattleResourceMngr->Set();
			//
			BackGround->Init();
			//
			Vector3DX LightVec = Vector3DX::vget(1.3f, -0.5f, 0.05f);LightVec = LightVec.normalized();
			DrawParts->SetAmbientLight(LightVec, GetColorF(1.0f, 0.96f, 0.94f, 1.0f));
			SetLightDifColor(GetColorF(1.0f, 0.96f, 0.94f, 1.0f));																// デフォルトライトのディフューズカラーを設定する

			auto& SecondLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec*-1.f);
			SetLightDifColorHandle(SecondLight.get(), GetColorF(0.5f, 0.5f, 0.3f, 0.1f));
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));

			for (int index = 0; index < Player_num; index++) {
				auto& p = PlayerMngr->GetPlayer(index);
				auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
				c->SetViewID(0);
				//人の座標設定
				{
					Vector3DX pos_t;
					pos_t = Vector3DX::vget(0.f, 0.f, (-1.5f*Scale_Rate)*static_cast<float>(index*2-1));

					Vector3DX EndPos = pos_t - Vector3DX::up() * 10.f*Scale_Rate;
					if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 10.f*Scale_Rate, &EndPos, true)) {
						pos_t = EndPos;
					}
					c->ValueSet((PlayerID)index, false, CharaTypeID::Team);
					c->MovePoint(deg2rad(0.f), deg2rad(180.f* static_cast<float>(index)), pos_t);
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
		}
		bool			MAINLOOP::Update_Sub(void) noexcept {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* BackGround = BackGroundClass::Instance();
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

			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();

			auto* Pad = PadControl::Instance();
			Pad->SetMouseMoveEnable(true);
			Pad->ChangeGuide(
				[&]() {
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
				if (Pad->GetKey(PADS::JUMP).trigger()) {
					OptionWindowClass::Instance()->SetActive();
				}
				//if (!m_NetWorkController) {
					//return true;
				//}
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
				if (DXDraw::Instance()->IsPause()) {
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
				clsDx();
				//ネットワーク
				auto* NetBrowser = NetWorkBrowser::Instance();
				if (NetBrowser->IsDataReady() && !m_NetWorkController) {
					m_NetWorkController = std::make_unique<NetWorkController>();
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
					for (int index = 0; index < Player_num; index++) {
						auto& p = PlayerMngr->GetPlayer(index);
						auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
						PlayerNetData Ret = this->m_NetWorkController->GetLerpServerPlayerData((PlayerID)index);
						c->SetViewID(GetMyPlayerID());
						if (index == GetMyPlayerID()) {
							c->SetInput(Ret.GetInput(), true);
						}
						else {
							//サーバーがCPUを動かす
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
						Ret.GetDamageEvent().AddDamageEvent(&this->m_DamageEvents);
					}
				}
				else {//オフライン
					for (int index = 0; index < Player_num; index++) {
						auto& p = PlayerMngr->GetPlayer(index);
						auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
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
						c->GetDamageEvent().AddDamageEvent(&this->m_DamageEvents);
					}
				}
				//ダメージイベント
				for (int index = 0; index < Player_num; index++) {
					auto& p = PlayerMngr->GetPlayer(index);
					auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
					for (int j = 0; j < static_cast<int>(this->m_DamageEvents.size()); j++) {
						if (c->SetDamageEvent(this->m_DamageEvents[static_cast<size_t>(j)])) {
							std::swap(this->m_DamageEvents.back(), m_DamageEvents[static_cast<size_t>(j)]);
							this->m_DamageEvents.pop_back();
							j--;
						}
					}
				}
			}
			//Execute
			ObjMngr->ExecuteObject();
			//コンカッション
			{
				DrawParts->Set_is_Blackout(m_Concussion > 0.f);
				if (m_Concussion == 1.f) {
					CameraShake::Instance()->SetCamShake(0.5f, 0.05f*Scale_Rate);
					//SE->Get(static_cast<int>(SoundEnum::Tank_near)).Play_3D(0, DrawParts->GetMainCamera().GetCamPos(), 10.f*Scale_Rate, 128);//, DX_PLAYTYPE_LOOP
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
				DrawParts->Set_Per_Blackout(m_ConcussionPer * 1.5f);
				m_Concussion = std::max(m_Concussion - 1.f / DrawParts->GetFps(), 0.f);
			}
			BackGround->FirstExecute();
			ObjMngr->LateExecuteObject();
			//視点
			{
				auto* OptionParts = OPTION::Instance();

				//DrawParts->GetFps()カメラ
				Vector3DX CamPos = Chara->GetEyePosition() + CameraShake::Instance()->GetCamShake();
				Vector3DX CamVec = CamPos + Chara->GetEyeMatrix().zvec() * -1.f;
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
						CamPos += Chara->GetEyeMatrix().xvec()*(2.f*Scale_Rate);
						break;
					case 1:
					case 4:
						CamVec = CamPos;
						CamPos += Chara->GetEyeMatrix().yvec()*(2.f*Scale_Rate) + Chara->GetEyeMatrix().zvec() * 0.1f;
						break;
					case 2:
					case 5:
						CamVec = CamPos;
						CamPos += Chara->GetEyeMatrix().zvec()*(-2.f*Scale_Rate);
						break;
					default:
						break;
				}
#endif
				DrawParts->SetMainCamera().SetCamPos(CamPos, CamVec, Chara->GetEyeMatrix().yvec());
				//info
				DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale_Rate * 0.3f, Scale_Rate * 20.f);
				//DoF
				PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.3f, Scale_Rate * 5.f, Scale_Rate * 0.1f, Scale_Rate * 20.f);
			}
			BackGround->Execute();
			//UIパラメーター
			{
				if (GetIsFirstLoop()) {
					this->m_UIclass.InitGaugeParam(0, static_cast<int>(Chara->GetStamina()), static_cast<int>(Chara->GetStaminaMax()));
					this->m_UIclass.InitGaugeParam(1, static_cast<int>(Chara->GetStamina()), static_cast<int>(Chara->GetStaminaMax()));
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
				this->m_UIclass.SetGaugeParam(0, 100,100, 15);
				this->m_UIclass.SetGaugeParam(1, static_cast<int>(Chara->GetStamina()), static_cast<int>(Chara->GetStaminaMax()), 15);
				//ガード円
				Easing(&m_GuardStart, Chara->IsGuardStarting() ? 1.f : 0.f, Chara->IsGuardStarting() ? 0.8f : 0.5f, EasingType::OutExpo);
				this->m_UIclass.SetfloatParam(2, m_GuardStart);
				this->m_UIclass.SetfloatParam(3, Chara->GetGuardVec().y);
				this->m_UIclass.SetfloatParam(4, -Chara->GetGuardVec().x);
				this->m_UIclass.SetfloatParam(5, Chara->GetGuardStartPer());
			}
#ifdef DEBUG
			DebugParts->SetPoint("Execute=0.7ms");
#endif // DEBUG
			return true;
		}
		void			MAINLOOP::Dispose_Sub(void) noexcept {
			auto* BackGround = BackGroundClass::Instance();
			//使い回しオブジェ系
			BackGround->Dispose();
			//
			m_NetWorkController->Dispose();
			m_NetWorkController.reset();
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
		}
		void			MAINLOOP::Dispose_Load_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->Dispose();
			this->m_UIclass.Dispose();
			PlayerMngr->Dispose();
			ObjectManager::Instance()->DeleteAll();
		}

		//
		void			MAINLOOP::BG_Draw_Sub(void) noexcept {
			auto* BackGround = BackGroundClass::Instance();
			BackGround->BG_Draw();
		}
		void			MAINLOOP::ShadowDraw_Far_Sub(void) noexcept {
			auto* BackGround = BackGroundClass::Instance();
			BackGround->Shadow_Draw_Far();
		}
		void			MAINLOOP::ShadowDraw_Sub(void) noexcept {
			auto* BackGround = BackGroundClass::Instance();
			BackGround->Shadow_Draw();
			ObjectManager::Instance()->Draw_Shadow();
		}
		void			MAINLOOP::CubeMap_Sub(void) noexcept {
			auto* BackGround = BackGroundClass::Instance();
			BackGround->Draw();
		}

		void			MAINLOOP::MainDraw_Sub(void) noexcept {
			auto* BackGround = BackGroundClass::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			SetFogStartEnd(DrawParts->GetMainCamera().GetCamNear(), DrawParts->GetMainCamera().GetCamFar() * 2.f);
			BackGround->Draw();
			ObjectManager::Instance()->Draw();
			//ObjectManager::Instance()->Draw_Depth();
			for (int i = 0; i < Player_num; i++) {
				PlayerMngr->GetPlayer(i)->GetAI()->Draw();
			}

		}
		//UI表示
		void			MAINLOOP::DrawUI_Base_Sub(void) noexcept {
		}
		void			MAINLOOP::DrawUI_In_Sub(void) noexcept {
			//UI
			if (!DXDraw::Instance()->IsPause()) {
				this->m_UIclass.Draw();
			}
			//通信設定
			auto* NetBrowser = NetWorkBrowser::Instance();
			NetBrowser->Draw();
			if (m_NetWorkController) {
				auto* DrawParts = DXDraw::Instance();
				WindowSystem::SetMsg(DrawParts->GetUIY(1920), DrawParts->GetUIY(32) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "Ping:%2dms", static_cast<int>(m_NetWorkController->GetPing()));
			}
		}
	};
};