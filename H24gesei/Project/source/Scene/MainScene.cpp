#pragma warning(disable:4464)
#include "MainScene.hpp"
#include "../MainScene/NetworkBrowser.hpp"
#include "../MainScene/Object/Ammo.hpp"

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
			BattleResourceMngr->LoadShip("data/Ship/Allied/BB/", (PlayerID)0);
			PlayerMngr->DisposeNPC();
			for (int index = 0; index < 10; ++index) {
				auto& p = PlayerMngr->AddNPC();
				BattleResourceMngr->LoadShipNPC("data/Ship/Axis/CA/", index);
			}
			//UI
			this->m_RotateImage = GraphHandle::Load("data/UI/Rotate.png");
			this->m_EnemyImage = GraphHandle::Load("data/UI/enemyrad.png");
			this->m_UIclass.Load();
			PauseMenuControl::LoadPause();
			HitMark::Instance()->Load();
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			//
			BattleResourceMngr->Set();
			//
			BackGround->Init();
			//
			Vector3DX LightVec = Vector3DX::vget(0.9f, -0.8f, 0.05f); LightVec = LightVec.normalized();
			DrawParts->SetAmbientLight(LightVec, GetColorF(1.0f, 0.96f, 0.94f, 1.0f));
			SetLightDifColor(GetColorF(1.0f, 0.96f, 0.94f, 1.0f));																// デフォルトライトのディフューズカラーを設定する
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
					pos_t = Vector3DX::vget(0.f, 0.f, 0.f);

					Vector3DX EndPos = pos_t - Vector3DX::up() * 10.f * Scale_Rate;
					//if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 10.f * Scale_Rate, &EndPos, true)) {
					//	pos_t = EndPos;
					//}
					c->ValueSet((PlayerID)index, CharaTypeID::Team);
					c->MovePoint(deg2rad(0.f), deg2rad(180.f * static_cast<float>(index)), pos_t);
				}
				p->GetAI()->Init(c->GetMyPlayerID());
			}

			for (int index = 0; index < PlayerMngr->GetNPCNum(); ++index) {
				auto& p = PlayerMngr->GetNPC(index);
				auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
				c->SetViewID(0);
				//人の座標設定
				{
					Vector3DX pos_t;
					pos_t = Vector3DX::vget(0.f, 0.f, (1000.f * Scale_Rate));
					pos_t = Matrix3x3DX::Vtrans(pos_t, Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRand(360))));
					if (index > 0) {
						for (int i = 0; i < 100; i++) {
							bool IsHit = false;
							for (int index2 = 0; index2 < index; ++index2) {
								auto& p2 = PlayerMngr->GetNPC(index2);
								auto& c2 = (std::shared_ptr<CharacterObject::CharacterClass>&)p2->GetChara();
								if ((pos_t - c2->GetMove().GetPos()).magnitude() < (100.f * Scale_Rate)) {
									IsHit = true;
									break;
								}
							}
							if (IsHit) {
								pos_t = Matrix3x3DX::Vtrans(pos_t, Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRand(360))));
							}
							else {
								break;
							}
						}
					}

					Vector3DX EndPos = pos_t - Vector3DX::up() * 10.f * Scale_Rate;
					//if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 10.f * Scale_Rate, &EndPos, true)) {
					//	pos_t = EndPos;
					//}
					c->ValueSet((PlayerID)(index + PlayerMngr->GetPlayerNum()), CharaTypeID::Team);
					c->MovePoint(deg2rad(0.f), atan2f(pos_t.x, pos_t.z), pos_t);
				}
				p->GetAI()->Init(c->GetMyPlayerID());
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
				auto* SE = SoundPool::Instance();
				SE->Get(static_cast<int>(SoundEnum::Env)).Play(0, DX_PLAYTYPE_LOOP, TRUE);
			}
			//Input,AI
			{
				InputControl MyInput;
				if (DXDraw::Instance()->IsPause() || !FadeControl::IsFadeClear()) {
					MyInput.ResetAllInput();
				}
				else {
					MyInput.SetInputStart(Pad->GetLS_Y() / 600.f, Pad->GetLS_X() / 600.f);
					MyInput.SetInputPADS(PADS::MOVE_W, Pad->GetKey(PADS::MOVE_W).press());
					MyInput.SetInputPADS(PADS::MOVE_S, Pad->GetKey(PADS::MOVE_S).press());
					MyInput.SetInputPADS(PADS::MOVE_A, Pad->GetKey(PADS::MOVE_A).press());
					MyInput.SetInputPADS(PADS::MOVE_D, Pad->GetKey(PADS::MOVE_D).press());

					MyInput.SetInputPADS(PADS::SHOT, Pad->GetKey(PADS::SHOT).press());
					MyInput.SetInputPADS(PADS::AIM, Pad->GetKey(PADS::AIM).press());
					MyInput.SetInputPADS(PADS::ULT, Pad->GetKey(PADS::ULT).press());

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
					//NPC
					for (int index = 0; index < PlayerMngr->GetNPCNum(); ++index) {
						auto& p = PlayerMngr->GetNPC(index);
						auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
						c->SetViewID(GetMyPlayerID());
						{
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
				//ダメージイベント
				for (int index = 0; index < PlayerMngr->GetNPCNum(); ++index) {
					auto& p = PlayerMngr->GetNPC(index);
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
				m_Range += static_cast<float>(GetMouseWheelRotVolWithCheck()*10.f);

				m_Range = std::clamp(m_Range, 0.f, 240.f);

				auto& ViewChara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
				auto* OptionParts = OPTION::Instance();
				//カメラ
				Vector3DX CamPos = ViewChara->GetEyePosition() + ViewChara->GetEyeMatrix().zvec() * (m_Range * Scale_Rate);
				Vector3DX CamVec = CamPos - ViewChara->GetEyeMatrix().zvec() * (5.f * Scale_Rate);
				CamVec += CameraShake::Instance()->GetCamShake();
				//CamPos += CameraShake::Instance()->GetCamShake()*2.f;
				DrawParts->SetMainCamera().SetCamPos(CamPos, CamVec, ViewChara->GetEyeMatrix().yvec());
				//info
				DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale_Rate * 10.f, Scale_Rate * 1200.f);
				//DoF
				PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 15.f, Scale_Rate * 600.f, Scale_Rate * 10.f, Scale_Rate * 1200.f);
			}
			/*
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
			//*/
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
			//
			{
				int loop = 0;
				while (true) {
					auto ammo = ObjMngr->GetObj((int)ObjType::Ammo, loop);
					if (ammo != nullptr) {
						auto& a = (std::shared_ptr<CharacterObject::AmmoClass>&)(*ammo);

						if (a->IsActive()) {
							//AmmoClass
							Vector3DX repos_tmp = a->GetMove().GetRePos();
							Vector3DX pos_tmp = a->GetMove().GetPos();
							if (GetMyPlayerID() != a->GetShootedID()) {
								if (GetMinLenSegmentToPoint(repos_tmp, pos_tmp, DrawParts->GetMainCamera().GetCamPos()) < 5.f * Scale_Rate) {
									m_Concussion = 1.f;
								}
							}

							Vector3DX norm_tmp;
							bool is_HitAll = false;
							for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
								auto& p = PlayerMngr->GetPlayer(index);
								auto& tgt = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
								if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
								HitPoint Damage = a->GetDamage();
								if (tgt->CheckDamageRay(&Damage, a->GetShootedID(), repos_tmp, &pos_tmp)) {
									is_HitAll |= true;
									a->Penetrate();
								}
							}

							for (int index = 0; index < PlayerMngr->GetNPCNum(); ++index) {
								auto& p = PlayerMngr->GetNPC(index);
								auto& tgt = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
								if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
								HitPoint Damage = a->GetDamage();
								if (tgt->CheckDamageRay(&Damage, a->GetShootedID(), repos_tmp, &pos_tmp)) {
									is_HitAll |= true;
									a->Penetrate();
								}
							}
						}
					}
					else {
						break;
					}
					loop++;
				}
			}
			//UIパラメーター
			{
				if (GetIsFirstLoop()) {
				}
				//残り
				int AliveNum = 0;
				for (int index = 0; index < PlayerMngr->GetNPCNum(); ++index) {
					auto& p = PlayerMngr->GetNPC(index);
					auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
					if (c->GetHP() > 0) {
						AliveNum++;
					}
				}
				if (AliveNum == 0) {
					this->m_IsEnd = true;
				}
				//NvsN
				this->m_UIclass.SetIntParam(0, AliveNum);
			}
			m_CenterPos.z = -1.f;
			{
				auto& ViewChara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
				m_RotateRad += ViewChara->GetYRadChange()/DrawParts->GetFps();

				Easing(&m_WS, static_cast<float>(ViewChara->GetWSLevel()), 0.9f, EasingType::OutExpo);
				Easing(&m_AD, static_cast<float>(ViewChara->GetADLevel()), 0.9f, EasingType::OutExpo);
			}
			HitMark::Instance()->Update();
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
			m_RotateImage.Dispose();
			m_EnemyImage.Dispose();
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
			BackGround->Shadow_Draw();
			ObjectManager::Instance()->Draw_Shadow();
		}

		void			MainGameScene::MainDraw_Sub(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			{
				auto& ViewChara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
				Vector3DX TargetPos = (ViewChara->GetEyePosition() - ViewChara->GetGunMatrix().zvec() * (1000.f * Scale_Rate));
				auto tmp = ConvWorldPosToScreenPos(TargetPos.get());
				if (tmp.z >= 0.f && tmp.z <= 1.f) {
					m_CenterPos = tmp;
				}
			}
			SetFogStartEnd(DrawParts->GetMainCamera().GetCamNear(), DrawParts->GetMainCamera().GetCamFar() * 2.f);
			BackGround->Draw();
			ObjectManager::Instance()->Draw();
			//ObjectManager::Instance()->Draw_Depth();
			for (int i = 0; i < PlayerMngr->GetNPCNum(); ++i) {
				PlayerMngr->GetNPC(i)->GetAI()->Draw();
			}
			HitMark::Instance()->CheckDraw();
		}
		//UI表示
		void			MainGameScene::DrawUI_In_Sub(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();
			HitMark::Instance()->Draw();
			FadeControl::DrawFade();
			//UI
			if (!DXDraw::Instance()->IsPause()) {
				this->m_UIclass.Draw();
				if (m_CenterPos.z >= 0.f && m_CenterPos.z <= 1.f) {

					m_CenterPos *= static_cast<float>(DrawParts->GetUIY(1080)) / static_cast<float>(DrawParts->GetScreenY(1080));

					int x1{}, y1{}, x2{}, y2{};

					x1 = static_cast<int>(m_CenterPos.x);
					y1 = static_cast<int>(m_CenterPos.y);
					x2 = static_cast<int>(m_CenterPos.x);
					y2 = DrawParts->GetUIY(1080);
					DrawLine(x1, y1, x2, y2, GetColor(0, 255, 0), 2);

					x1 = static_cast<int>(m_CenterPos.x) - DrawParts->GetUIY(64);
					y1 = static_cast<int>(m_CenterPos.y);
					x2 = DrawParts->GetUIY(0);
					y2 = static_cast<int>(m_CenterPos.y);
					DrawLine(x1, y1, x2, y2, GetColor(0, 255, 0), 2);

					x1 = static_cast<int>(m_CenterPos.x) + DrawParts->GetUIY(64);
					y1 = static_cast<int>(m_CenterPos.y);
					x2 = DrawParts->GetUIY(1920);
					y2 = static_cast<int>(m_CenterPos.y);
					DrawLine(x1, y1, x2, y2, GetColor(0, 255, 0), 2);
				}
				{
					auto& ViewChara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();

					int xp = DrawParts->GetUIY(192);
					int yp = DrawParts->GetUIY(1080 - 256);
					m_RotateImage.DrawRotaGraph(xp, yp,
						static_cast<float>(DrawParts->GetUIY(100)) / 100.f,
						m_RotateRad, true
					);

					int x1{}, y1{}, x2{}, y2{};
					x1 = xp + DrawParts->GetUIY(24 * -3);
					y1 = yp - DrawParts->GetUIY(128 + 24);
					x2 = xp + DrawParts->GetUIY(24 * 3);
					y2 = yp - DrawParts->GetUIY(128 + 24);
					DrawLine(x1, y1, x2, y2, GetColor(64, 64, 64), 6);
					x1 = xp;
					y1 = yp - DrawParts->GetUIY(128 + 24);
					x2 = xp + DrawParts->GetUIY(static_cast<int>(24 * m_AD));
					y2 = yp - DrawParts->GetUIY(128 + 24);
					DrawLine(x1, y1, x2, y2, GetColor(0, 255, 0), 6);

					x1 = xp + DrawParts->GetUIY(128 + 24);
					y1 = yp - DrawParts->GetUIY(24 * 3);
					x2 = xp + DrawParts->GetUIY(128 + 24);
					y2 = yp - DrawParts->GetUIY(24 * -3);
					DrawLine(x1, y1, x2, y2, GetColor(64, 64, 64), 6);
					x1 = xp + DrawParts->GetUIY(128 + 24);
					y1 = yp - DrawParts->GetUIY(static_cast<int>(24 * m_WS));
					x2 = xp + DrawParts->GetUIY(128 + 24);
					y2 = yp - DrawParts->GetUIY(24 * 0);
					DrawLine(x1, y1, x2, y2, GetColor(0, 255, 0), 6);

					xp = DrawParts->GetUIY(24);
					yp = DrawParts->GetUIY(1080*1/3);
					std::string String;
					for (const auto& t : ViewChara->GetTurretData()) {
						Fonts->Get(FontPool::FontType::MS_Gothic, DrawParts->GetUIY(16), 3)->DrawString(INVALID_ID,
							FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp, yp,
							(t.NowLoadTime == 0) ? Green : Red, Black,
							"%3.1f cm Cannon : %5.2fs", t.AmmoSize, (t.NowLoadTime == 0) ? t.LoadTime : t.NowLoadTime);

						yp += DrawParts->GetUIY(16 +4);
					}
				}
				{
					//残り
					int AliveNum = 0;
					for (int index = 0; index < PlayerMngr->GetNPCNum(); ++index) {
						auto& p = PlayerMngr->GetNPC(index);
						auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
						if (c->GetHP() > 0) {
							AliveNum++;
						}
					}
					if (AliveNum == 0) {
						int xp = DrawParts->GetUIY(1920 / 2);
						int yp = DrawParts->GetUIY(1080 / 2);
						Fonts->Get(FontPool::FontType::MS_Gothic, DrawParts->GetUIY(72), 3)->DrawString(INVALID_ID, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE, xp, yp, White, Black, "クリア！");
					}
				}
				{
					int xp = DrawParts->GetUIY(1920 / 2);
					int yp = DrawParts->GetUIY(1080 / 2);
					auto& ViewChara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();

					
					for (int index = 0; index < PlayerMngr->GetNPCNum(); ++index) {
						auto& p = PlayerMngr->GetNPC(index);
						auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
						if (c->GetHP() > 0) {
							Vector3DX vec = c->GetMove().GetPos()-ViewChara->GetMove().GetPos();
							m_EnemyImage.DrawRotaGraph(xp,yp, static_cast<float>(DrawParts->GetUIY(100)) / 100.f,
								-ViewChara->GetEyeRad().y + atan2f(-vec.x, -vec.z)
								,true);
						}
					}
				}
			}
			else {
				PauseMenuControl::DrawPause();
			}
			//通信設定
			//auto* NetBrowser = NetWorkBrowser::Instance();
			//NetBrowser->Draw();
			if (m_NetWorkController) {
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
