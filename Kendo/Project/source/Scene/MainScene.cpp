#include "MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MAINLOOP::Load_Sub(void) noexcept {
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			//BG
			this->m_BackGround->Load();
			//
			this->m_hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
			this->m_aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
			this->m_scope_Graph = GraphHandle::Load("data/UI/battle_scope.png");
			//
			BattleResourceMngr->Load();
			PlayerMngr->Init(Vehicle_num);

			BattleResourceMngr->LoadChara("Chara", (PlayerID)1);
		}
		void			MAINLOOP::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			//
			BattleResourceMngr->Set();
			//
			this->m_BackGround->Init();
			//
			Vector3DX LightVec = Vector3DX::vget(-0.8f, -0.5f, -0.1f);
			DrawParts->SetAmbientLight(LightVec, GetColorF(0.92f, 0.91f, 0.90f, 1.0f));
			DrawParts->SetupShadowDir(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 0);
			DrawParts->SetupShadowDir(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 1);
			DrawParts->SetupShadowDir(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 2);
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));

			BattleResourceMngr->LoadChara("Chara", (PlayerID)0);

			for (int index = 0; index < Player_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				//BGをオブジェに登録
				c->SetMapCol(this->m_BackGround);
				//人の座標設定
				{
					Vector3DX pos_t;
					pos_t = Vector3DX::vget(0.f, 0.f, (-1.5f*Scale_Rate)*(float)(index*2-1));

					Vector3DX EndPos = pos_t - Vector3DX::up() * 10.f*Scale_Rate;
					if (this->m_BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 10.f*Scale_Rate, &EndPos, true)) {
						pos_t = EndPos;
					}
					c->ValueSet((PlayerID)index, false, CharaTypeID::Team);
					c->MovePoint(deg2rad(0.f), deg2rad(180.f*(float)index), pos_t);
					c->Heal(100, true);
				}
			}
			//UI
			this->m_UIclass.Set();
			//
			this->m_DamageEvents.clear();
			this->m_NetWorkBrowser.Init();
		}
		bool			MAINLOOP::Update_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			auto* Pad = PadControl::Instance();
			Pad->SetMouseMoveEnable(false);
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

						KeyGuide->AddGuide(PADS::RUN, LocalizePool::Instance()->Get(9902));
						KeyGuide->AddGuide(PADS::SQUAT, LocalizePool::Instance()->Get(9904));
					}
				});
			if (DXDraw::Instance()->IsPause()) {
				if (Pad->GetKey(PADS::JUMP).trigger()) {
					OptionWindowClass::Instance()->SetActive();
				}
				return true;
			}
			else {
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("Execute=Start");
#endif // DEBUG
			//FirstDoingv
			if (GetIsFirstLoop()) {
				SetMousePoint(DXDraw::Instance()->GetDispXSize() / 2, DXDraw::Instance()->GetDispYSize() / 2);
				//SE->Get((int)SoundEnum::Environment).Play(0, DX_PLAYTYPE_LOOP, TRUE);
				this->m_fov_base = DrawParts->GetMainCamera().GetCamFov();
			}
			//Input,AI
			{
				float cam_per = ((DrawParts->GetMainCamera().GetCamFov() / deg2rad(75)) / (DrawParts->is_lens() ? DrawParts->zoom_lens() : 1.f)) / 100.f;
				cam_per *= 0.6f;
				float pp_x = 0.f, pp_y = 0.f;
				InputControl MyInput;
				//
				//cam_per /= std::max(1.f, std::hypotf(Pad->GetLS_Y(), Pad->GetLS_X()));
				/*
				pp_x = std::clamp(Pad->GetLS_Y() * cam_per*0.5f, -0.2f, 0.2f);
				pp_y = std::clamp(Pad->GetLS_X() * cam_per*0.5f, -0.2f, 0.2f);
				MyInput.SetInputStart(pp_x, pp_y);
				//*/
				//
				MyInput.SetInputStart(0.f, 0.f);
				MyInput.SetInputPADS(PADS::MOVE_W, Pad->GetKey(PADS::MOVE_W).press());
				MyInput.SetInputPADS(PADS::MOVE_S, Pad->GetKey(PADS::MOVE_S).press());
				MyInput.SetInputPADS(PADS::MOVE_A, Pad->GetKey(PADS::MOVE_A).press());
				MyInput.SetInputPADS(PADS::MOVE_D, Pad->GetKey(PADS::MOVE_D).press());

				MyInput.SetInputPADS(PADS::SHOT, Pad->GetKey(PADS::SHOT).press());
				MyInput.SetInputPADS(PADS::AIM, Pad->GetKey(PADS::AIM).press());
				MyInput.SetInputPADS(PADS::ULT, Pad->GetKey(PADS::ULT).press());

				MyInput.SetInputPADS(PADS::WALK, Pad->GetKey(PADS::WALK).press());
				MyInput.SetInputPADS(PADS::JUMP, Pad->GetKey(PADS::JUMP).press());

				MyInput.SetInputPADS(PADS::RUN, Pad->GetKey(PADS::RUN).press());
				MyInput.SetInputPADS(PADS::SQUAT, Pad->GetKey(PADS::SQUAT).press());
				//スコープ
				{
					auto OLD = this->m_TPSLen;
					this->m_zoom = 2.0f;
					this->m_TPSLen = 8;
					this->m_changeview = ((this->m_TPSLen != OLD) && (this->m_TPSLen == 0));
					Easing(&this->m_range_r, (float)this->m_TPSLen *8.f / 6.f, 0.8f, EasingType::OutExpo);

					bool IsADS = (this->m_TPSLen == 0);
					{
						if (m_IsChangeView != IsADS) {
							this->m_ChangeViewPer = 1.f;
						}
						else {
							Easing(&this->m_ChangeViewPer, 0.f, 0.95f, EasingType::OutExpo);
						}
						m_IsChangeView = IsADS;
					}
				}
				/*
				//ネットワーク
				SendInfo ans;
				ans.m_Pos = Vehicle->GetMove().pos;
				ans.m_Vec = Vehicle->GetMove().vec;
				ans.m_rad.y = (Vehicle->Get_body_yrad());
				ans.m_Damage = &Vehicle->GetDamageEvent();
				ans.m_DamageSwitch = (Vehicle->GetDamageSwitch() ? 1 : 0);
				this->m_NetWorkBrowser.FirstExecute(MyInput, ans);
				//*/
				//クライアント
				if (m_NetWorkBrowser.GetClient()) {
					/*
					for (int i = 0; i < Vehicle_num; i++) {
						auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
						if (v->GetMyPlayerID() == GetMyPlayerID()) {
							v->SetCharaType(CharaTypeID::Mine);
						}
						else {
							v->SetCharaType(CharaTypeID::Enemy);
						}
					}
					//*/
				}
				//
				bool isready = true;
				for (int index = 0; index < Player_num; index++) {
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
					if (m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
						auto tmp = this->m_NetWorkBrowser.GetNowServerPlayerData(index, false);
						if (index == GetMyPlayerID()) {
							MyInput.SetKeyInputFlags(tmp.Input);//キーフレームだけサーバーに合わせる
							c->SetInput(MyInput, isready && c->IsAlive());
							m_NetWorkBrowser.GetRecvData(index, tmp.GetFrame());
						}
						else {
							if (!m_NetWorkBrowser.GetClient()) {
								//m_AICtrl[index]->Execute(&tmp.Input, (!m_IsHardMode && (m_Timer > 60.f)) || m_IsHardMode);
							}
							c->SetInput(tmp.Input, isready && c->IsAlive());
							bool override_true = true;
							override_true = tmp.GetIsActive();
							if (override_true) {
								c->SetPosBufOverRide(tmp.m_move);
							}

						}
						//ダメージイベント処理
						for (auto& e : tmp.m_DamageEvents) {
							this->m_DamageEvents.emplace_back(e);
						}
					}
					else {
						if (index == GetMyPlayerID()) {
							c->SetInput(MyInput, isready && c->IsAlive());
						}
						else {
							InputControl OtherInput;
							//m_AICtrl[index]->Execute(&OtherInput, (!m_IsHardMode && (m_Timer > 60.f)) || m_IsHardMode);
							c->SetInput(OtherInput, isready && c->IsAlive());
						}
						//ダメージイベント処理
						for (const auto& e : c->GetDamageEvent()) {
							this->m_DamageEvents.emplace_back(e);
						}
						c->GetDamageEvent().clear();
					}
				}
				m_NetWorkBrowser.LateExecute();
				//ダメージイベント
				for (int index = 0; index < Player_num; index++) {
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
					for (int j = 0; j < this->m_DamageEvents.size(); j++) {
						if (c->SetDamageEvent(this->m_DamageEvents[j])) {
							std::swap(this->m_DamageEvents.back(), m_DamageEvents[j]);
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
					DrawParts->SetCamShake(0.5f, 0.05f*Scale_Rate);
					//SE->Get((int)SoundEnum::Tank_near).Play_3D(0, DrawParts->GetMainCamera().GetCamPos(), 10.f*Scale_Rate, 128);//, DX_PLAYTYPE_LOOP
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
			this->m_BackGround->FirstExecute();
			ObjMngr->LateExecuteObject();
			//視点
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
			{
				auto* OptionParts = OPTION::Instance();

				//DrawParts->GetFps()カメラ
				Vector3DX CamPos = Chara->GetEyeMatrix().pos() + DrawParts->GetCamShake();
				DrawParts->SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeMatrix().zvec() * -1.f, Chara->GetEyeMatrix().yvec());

				//Vector3DX CamPos = Vector3DX::vget(0.f, 1.5f, 0.f)*Scale_Rate;
				//Vector3DX CamVec = Vector3DX::vget(1.f, 1.5f, 0.f)*Scale_Rate;
				//DrawParts->SetMainCamera().SetCamPos(CamPos, CamVec, Chara->GetEyeMatrix().yvec());

				//info
				auto fov_t = DrawParts->GetMainCamera().GetCamFov();
				auto near_t = DrawParts->GetMainCamera().GetCamNear();
				auto far_t = DrawParts->GetMainCamera().GetCamFar();
				Easing(&near_t, Scale_Rate * 0.03f, 0.9f, EasingType::OutExpo);
				Easing(&far_t, Scale_Rate * 10.f, 0.5f, EasingType::OutExpo);
				//fov
				{
					float fov = deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov));
					Easing(&fov_t, fov, 0.8f, EasingType::OutExpo);
				}
				DrawParts->SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
				//DoF
				PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, Scale_Rate * 5.f, near_t, far_t);
			}
			this->m_BackGround->Execute();
			//UIパラメーター
			{
				this->m_UIclass.SetItemGraph(0, &m_aim_Graph);
			}
			EffectControl::Execute();
#ifdef DEBUG
			DebugParts->SetPoint("Execute=0.7ms");
#endif // DEBUG
			return true;
		}
		void			MAINLOOP::Dispose_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			SE->Get((int)SoundEnum::Env).StopAll(0);
			SE->Get((int)SoundEnum::Env2).StopAll(0);
			//使い回しオブジェ系
			ObjectManager::Instance()->DelObj(&PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara());
			PlayerMngr->GetPlayer(GetMyPlayerID()).Dispose();
			this->m_BackGround->Dispose();
			this->m_BackGround.reset();
			//
			m_NetWorkBrowser.Dispose();
			EffectControl::Dispose();

			{
				auto* DrawParts = DXDraw::Instance();
				PostPassEffect::Instance()->SetLevelFilter(0, 255, 1.f);
				DrawParts->SetAberrationPower(1.f);
				DrawParts->Set_is_Blackout(false);
				DrawParts->Set_Per_Blackout(0.f);
				DrawParts->Set_is_lens(false);
				DrawParts->Set_zoom_lens(1.f);
			}
		}
		void			MAINLOOP::Dispose_Load_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->Dispose();
			m_AICtrl.clear();
			this->m_UIclass.Dispose();
			PlayerMngr->Dispose();
			ObjectManager::Instance()->DeleteAll();
		}
	};
};