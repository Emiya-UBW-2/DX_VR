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
			//UI
			this->hit_Graph.Load("data/UI/battle_hit.bmp");
			this->guard_Graph.Load("data/UI/battle_guard.bmp");
			this->m_UIclass.Load();
			m_PauseMenuControl.Load();
			//
			PlayerMngr->Init(NetWork::Player_num);
			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(loop)->GetChara();
				if (loop == GetViewPlayerID()) {
					CharacterClass::LoadChara("Main", (PlayerID)loop);
					int Rand = GetRand(100);
					if (Rand < 30) {
						c->LoadCharaGun("type89", 0, false);
					}
					else if (Rand < 30 + 40) {
						c->LoadCharaGun("type20E", 0, false);
					}
					else {
						c->LoadCharaGun("Mod870", 0, false);
					}
					c->LoadCharaGun("P226", 1, false);
					c->LoadCharaGun("RGD5", 2, false);
					c->SetCharaTypeID(CharaTypeID::Team);
				}
				else {
					CharacterClass::LoadChara("Soldier", (PlayerID)loop);
					c->LoadCharaGun("AKS-74", 0, false);
					c->LoadCharaGun("RGD5", 2, false);
					//ラグドール
					if (loop == 1) {
						MV1::Load((c->GetFilePath() + "model_Rag.mv1").c_str(), &c->GetRagDoll(), DX_LOADMODEL_PHYSICS_REALTIME);//身体ラグドール
					}
					else {
						auto& Base = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1)->GetChara();
						c->GetRagDoll().Duplicate(Base->GetRagDoll());
					}
					c->SetupRagDoll();
					c->SetCharaTypeID(CharaTypeID::Enemy);
				}
				c->SetPlayerID((PlayerID)loop);
			}
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* OptionParts = OptionManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* NetBrowser = NetWorkBrowser::Instance();
			//
			BattleResourceMngr->Set();

			PostPassParts->SetShadowScale(0.5f);
			//
			BackGround->Init();
			//
			Vector3DX LightVec = Vector3DX::vget(0.f, -0.3f, 0.15f); LightVec = LightVec.normalized();
			PostPassParts->SetAmbientLight(LightVec);

			SetLightEnable(FALSE);

			auto& FirstLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec);
			SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.0f, 0.96f, 0.94f, 1.0f));
			SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 0.96f, 0.94f, 1.0f));

			PostPassParts->SetGodRayPer(0.25f);
			//Cam
			CameraParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));
			//info
			constexpr float FarMax = std::min(std::min(BackGround::DrawMaxXPlus, BackGround::DrawMaxZPlus), BackGround::DrawMaxYPlus) * BackGround::CellScale;
			CameraParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale3DRate * 0.03f, FarMax);
			//Fog
			SetVerticalFogMode(DX_FOGMODE_LINEAR);
			SetVerticalFogStartEnd(-26.f * Scale3DRate, -10.f * Scale3DRate);
			SetVerticalFogColor(0, 0, 0);
			//Fog
			SetFogMode(DX_FOGMODE_LINEAR);
			SetFogStartEnd(FarMax, FarMax * 20.f);
			SetFogColor(114, 120, 128);
			//
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
				//人の座標設定
				Vector3DX pos_t;
				pos_t.Set(GetRandf(10.f), -20.f, GetRandf(10.f));
				pos_t *= Scale3DRate;

				Vector3DX EndPos = pos_t - Vector3DX::up() * 200.f * Scale3DRate;
				if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 0.f * Scale3DRate, &EndPos)) {
					pos_t = EndPos;
				}
				c->Spawn(deg2rad(0.f), deg2rad(GetRand(360)), pos_t, 0);
			}
			//UI
			this->m_UIclass.Set();
			//
			this->m_DamageEvents.clear();
			NetBrowser->Init();
			m_PauseMenuControl.Init();
			m_FadeControl.Init();
			this->m_IsEnd = false;
			m_EffectControl.Init();
			this->m_StartTimer = 5.f;

			m_EffectControl.SetLoop(static_cast<int>(Sceneclass::Effect::ef_dust), Vector3DX::zero());
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("Execute=Start");
#endif // DEBUG
			auto* CameraParts = Camera3D::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* Pad = PadControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* NetBrowser = NetWorkBrowser::Instance();
			auto* OptionParts = OptionManager::Instance();

			PostPassParts->SetLevelFilter(38, 154, 1.f);
			m_PauseMenuControl.Update();
			if (m_PauseMenuControl.IsRetire()) {
				if (!this->m_IsEnd) {
					m_FadeControl.SetBlackOut(true);
				}
				this->m_IsEnd = true;
			}
			if (this->m_IsEnd && m_FadeControl.IsAll()) {
				return false;
			}

			m_FadeControl.Update();

			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetViewPlayerID())->GetChara();

			Pad->SetMouseMoveEnable(true);
			KeyGuideParts->ChangeGuide(
				[]() {
					auto* SceneParts = SceneControl::Instance();
					auto* KeyGuideParts = KeyGuide::Instance();
					if (SceneParts->IsPause()) {
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizePool::Instance()->Get(9992));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD), LocalizePool::Instance()->Get(9991));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizePool::Instance()->Get(9993));
					}
					else {
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizePool::Instance()->Get(9900));

						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::LEAN_L), "");
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::LEAN_R), LocalizePool::Instance()->Get(9902));

						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::SHOT), LocalizePool::Instance()->Get(9906));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::AIM), LocalizePool::Instance()->Get(9908));
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::ULT), LocalizePool::Instance()->Get(9907));

						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::WALK), LocalizePool::Instance()->Get(9903));

						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD), LocalizePool::Instance()->Get(9904));

						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::THROW), LocalizePool::Instance()->Get(9904));
					}
				});
			if (SceneParts->IsPause()) {
				Pad->SetMouseMoveEnable(false);
				BackGround->SettingChange();
				if (!m_NetWorkController) {
					return true;
				}
			}
#ifdef DEBUG
			DebugParts->SetPoint("0.05");
#endif // DEBUG
			//FirstDoingv
			if (GetIsFirstLoop()) {}
			//Input,AI
			{
				if (!GetIsFirstLoop()) {
					this->m_StartTimer = std::max(this->m_StartTimer - DXLib_refParts->GetDeltaTime(), 0.f);
				}
				InputControl MyInput; MyInput.ResetAllInput();
				if (!SceneParts->IsPause() && m_FadeControl.IsClear() && (this->m_StartTimer <= 0.f)) {
					float AimPer = 1.f / std::max(1.f, Chara->GetSightZoomSize());
					auto RecoilRadAdd = Chara->GetRecoilRadAdd();
					MyInput.SetAddxRad(Pad->GetLS_Y() / 200.f * AimPer - RecoilRadAdd.y);
					MyInput.SetAddyRad(Pad->GetLS_X() / 200.f * AimPer + RecoilRadAdd.x);
					MyInput.SetInputPADS(Controls::PADS::MOVE_W, Pad->GetPadsInfo(Controls::PADS::MOVE_W).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::MOVE_S, Pad->GetPadsInfo(Controls::PADS::MOVE_S).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::MOVE_A, Pad->GetPadsInfo(Controls::PADS::MOVE_A).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::MOVE_D, Pad->GetPadsInfo(Controls::PADS::MOVE_D).GetKey().press());
					//MyInput.SetInputPADS(Controls::PADS::RUN, Pad->GetPadsInfo(Controls::PADS::RUN).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::LEAN_L, Pad->GetPadsInfo(Controls::PADS::LEAN_L).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::LEAN_R, Pad->GetPadsInfo(Controls::PADS::LEAN_R).GetKey().press());
					//MyInput.SetInputPADS(Controls::PADS::MELEE, Pad->GetPadsInfo(Controls::PADS::MELEE).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::RELOAD, Pad->GetPadsInfo(Controls::PADS::RELOAD).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::INTERACT, Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().press());
					//MyInput.SetInputPADS(Controls::PADS::SQUAT, Pad->GetPadsInfo(Controls::PADS::SQUAT).GetKey().press());
					//MyInput.SetInputPADS(Controls::PADS::PRONE, Pad->GetPadsInfo(Controls::PADS::PRONE).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::SHOT, Pad->GetPadsInfo(Controls::PADS::SHOT).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::AIM, Pad->GetPadsInfo(Controls::PADS::AIM).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::ULT, Pad->GetPadsInfo(Controls::PADS::ULT).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::THROW, Pad->GetPadsInfo(Controls::PADS::THROW).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::CHECK, Pad->GetPadsInfo(Controls::PADS::CHECK).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::WALK, Pad->GetPadsInfo(Controls::PADS::WALK).GetKey().press());
					//MyInput.SetInputPADS(Controls::PADS::JUMP, Pad->GetPadsInfo(Controls::PADS::JUMP).GetKey().press());
				}
				//ネットワーク
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
						auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
						NetWork::PlayerNetData Ret = this->m_NetWorkController->GetLerpServerPlayerData((PlayerID)index);
						if (index == GetViewPlayerID() && !IsServerNotPlayer) {
							c->Input(Ret.GetInput());//自身が動かすもの
						}
						else {//サーバーからのデータで動くもの
							//サーバーがCPUを動かす場合
							if (!m_NetWorkController->GetClient()) {
								//cpu
								//p->GetAI()->Execute(&MyInput);
							}
							c->Input(Ret.GetInput());
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
						auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
						if (index == GetViewPlayerID()) {
							c->Input(MyInput);
						}
						else {
							InputControl OtherInput;
							p->GetAI()->Execute(&OtherInput);
							c->Input(OtherInput);
						}
						//ダメージイベント処理
						c->AddDamageEvent(&this->m_DamageEvents);
					}
				}
				//ダメージイベント
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
					auto& p = PlayerMngr->GetPlayer(index);
					auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
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
#ifdef DEBUG
			DebugParts->SetPoint("1.72");
#endif // DEBUG
			ObjMngr->ExecuteObject();
#ifdef DEBUG
			DebugParts->SetPoint("0.00");
#endif // DEBUG
			ObjMngr->LateExecuteObject();
			UpdateBullet();							//弾の更新
#ifdef DEBUG
			DebugParts->SetPoint("0.52");
#endif // DEBUG
			//視点
			{
				auto& ViewChara = (std::shared_ptr<Sceneclass::CharacterClass>&)PlayerMngr->GetPlayer(GetViewPlayerID())->GetChara();
				//カメラ
				Vector3DX CamPos = ViewChara->GetEyePositionCache() + Camera3D::Instance()->GetCamShake();
				Vector3DX CamVec = CamPos + ViewChara->GetEyeRotationCache().zvec() * -1.f;
				CamVec += Camera3D::Instance()->GetCamShake();
				CamPos += Camera3D::Instance()->GetCamShake() * 2.f;
#ifdef DEBUG_CAM
				if (CheckHitKey(KEY_INPUT_F1) != 0) {
					DBG_CamSel = -1;
				}
				if (CheckHitKey(KEY_INPUT_F2) != 0) {
					DBG_CamSel = 0;
				}
				if (CheckHitKey(KEY_INPUT_F3) != 0) {
					DBG_CamSel = 1;
				}
				if (CheckHitKey(KEY_INPUT_F4) != 0) {
					DBG_CamSel = 2;
				}
				if (CheckHitKey(KEY_INPUT_F5) != 0) {
					DBG_CamSel = 3;
				}
				if (CheckHitKey(KEY_INPUT_F6) != 0) {
					DBG_CamSel = 4;
				}
				if (CheckHitKey(KEY_INPUT_F7) != 0) {
					DBG_CamSel = 5;
				}
				switch (DBG_CamSel) {
				case 0:
					CamVec = CamPos;
					CamPos += ViewChara->GetEyeRotationCache().xvec() * (3.f * Scale3DRate);
					break;
				case 1:
					CamVec = CamPos;
					CamPos -= ViewChara->GetEyeRotationCache().xvec() * (3.f * Scale3DRate);
					break;
				case 2:
					CamVec = CamPos;
					CamPos += ViewChara->GetEyeRotationCache().yvec() * (3.f * Scale3DRate) + ViewChara->GetEyeRotationCache().zvec() * 0.1f;
					break;
				case 3:
					CamVec = CamPos;
					CamPos += ViewChara->GetEyeRotationCache().zvec() * (-3.f * Scale3DRate);
					break;
				default:
					break;
				}
#endif
				Easing(&m_EffectPos, CamPos, 0.95f, EasingType::OutExpo);
				m_EffectControl.Update_LoopEffect(static_cast<int>(Sceneclass::Effect::ef_dust), m_EffectPos, Vector3DX::forward(), 0.5f);
				m_EffectControl.SetEffectColor(static_cast<int>(Sceneclass::Effect::ef_dust), 255, 255, 255, 64);

				CameraParts->SetMainCamera().SetCamPos(CamPos, CamVec, ViewChara->GetEyeRotationCache().yvec());
				auto fov_t = CameraParts->GetMainCamera().GetCamFov();
				//fov
				{
					float fov = deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov));
					if (Chara->GetIsADS()) {
						fov -= deg2rad(15);
						fov /= std::max(1.f, Chara->GetSightZoomSize() / 2.f);
					}
					if (Chara->IsGunShotSwitch()) {
						fov -= deg2rad(5);
						Easing(&fov_t, fov, 0.5f, EasingType::OutExpo);
					}
					else {
						Easing(&fov_t, fov, 0.8f, EasingType::OutExpo);
					}
#ifdef DEBUG_CAM
					if (0 <= DBG_CamSel && DBG_CamSel <= 3) {
						fov_t = deg2rad(15);
					}
#endif
				}
				auto far_t = CameraParts->GetMainCamera().GetCamFar();
				CameraParts->SetMainCamera().SetCamInfo(fov_t, CameraParts->GetMainCamera().GetCamNear(), far_t);
				//DoF
				PostPassEffect::Instance()->Set_DoFNearFar(Scale3DRate * 0.3f, Scale3DRate * 5.f, Scale3DRate * 0.1f, Chara->GetIsADS() ? (far_t * 3.f) : (far_t * 2.f));
			}
			//コンカッション
			{
				//if (Chara->PopConcussionSwitch()) {
				//	m_Concussion = 1.f;
				//}
				PostPassParts->Set_is_Blackout(m_Concussion > 0.f);
				if (m_Concussion == 1.f) {
					Camera3D::Instance()->SetCamShake(0.5f, 0.01f * Scale3DRate);
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
				PostPassParts->Set_Per_Blackout(m_ConcussionPer * 2.f);
				m_Concussion = std::max(m_Concussion - DXLib_refParts->GetDeltaTime(), 0.f);
			}
			BackGround->Execute();
			//
			{
				Vector3DX StartPos = Chara->GetEyePositionCache();
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
					if (index == GetViewPlayerID()) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
					Vector3DX TgtPos = c->GetEyePositionCache();
					auto EndPos = TgtPos;
					c->m_CanLookTarget = !BackGround->CheckLinetoMap(StartPos, &EndPos);
					c->m_Length = (TgtPos - StartPos).magnitude();
				}
			}
			//UIパラメーター
			{
				//timer
				this->m_UIclass.SetfloatParam(0, 0.f);
				this->m_UIclass.SetfloatParam(1, m_StartTimer);

				this->m_UIclass.Update();
			}
			m_EffectControl.Execute();
			//
			{
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
					c->m_CameraPos.z = -1.f;
				}
			}
#ifdef DEBUG
			DebugParts->SetPoint("Execute=End");
#endif // DEBUG
			return true;
		}
		void			MainGameScene::Dispose_Sub(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			//使い回しオブジェ系
			BackGround->Dispose();
			GunsModify::Instance()->DisposeSlots();
			//
			if (m_NetWorkController) {
				m_NetWorkController->Dispose();
				m_NetWorkController.reset();
			}
			{
				PostPassParts->SetLevelFilter(0, 255, 1.f);
				PostPassParts->SetAberrationPower(1.f);
				PostPassParts->Set_is_Blackout(false);
				PostPassParts->Set_Per_Blackout(0.f);
				PostPassParts->Set_is_lens(false);
				PostPassParts->Set_zoom_lens(1.f);
			}
			m_EffectControl.Dispose();
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
			this->hit_Graph.Dispose();
			this->guard_Graph.Dispose();
			this->m_UIclass.Dispose();
			PlayerMngr->Dispose();
			ObjectManager::Instance()->DeleteAll();
			m_PauseMenuControl.Dispose();
		}
		//
		void			MainGameScene::DrawHitGraph(void) const noexcept {
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			int loop = 0;
			while (true) {
				auto ammo = ObjMngr->GetObj(static_cast<int>(ObjType::Ammo), loop);
				if (ammo != nullptr) {
					auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
					if (a->m_IsDrawHitUI && a->GetShootedID() == GetViewPlayerID()) {
						int			Alpha = static_cast<int>(a->m_Hit_alpha * 255.f);
						if ((Alpha >= 10) && (a->m_Hit_DispPos.z >= 0.f && a->m_Hit_DispPos.z <= 1.f)) {
							WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, Alpha);
							//
							int r = static_cast<int>(255 * std::clamp((float)a->m_Damage / 100.f * 2.f, 0.f, 1.f));
							int g = 255 - r;
							if (a->m_Damage > 0) {
								WindowParts->SetBright(WindowSystem::DrawLayer::Normal, r, g, 0);
								WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &hit_Graph, static_cast<int>(a->m_Hit_DispPos.x), static_cast<int>(a->m_Hit_DispPos.y), (float)static_cast<int>((float)Alpha / 255.f * 0.5f * 100.0f) / 100.f, 0.f, true);
							}
							if (a->m_ArmerDamage > 0) {
								WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 128, 128, 128);
								WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &guard_Graph, static_cast<int>(a->m_Hit_DispPos.x), static_cast<int>(a->m_Hit_DispPos.y), (float)static_cast<int>((float)Alpha / 255.f * 0.5f * 100.0f) / 100.f, 0.f, true);
							}
							WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							//
							if (a->m_Damage > 0) {
								WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
									24, FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
									static_cast<int>(a->m_Hit_DispPos.x + a->m_Hit_AddX), static_cast<int>(a->m_Hit_DispPos.y + a->m_Hit_AddY), GetColor(r, g, 0), Black, "%d", a->m_Damage);
							}
							//防いだダメージ
							if (a->m_ArmerDamage > 0) {
								WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
									20, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
									static_cast<int>(a->m_Hit_DispPos.x + a->m_Hit_AddX) - 10, static_cast<int>(a->m_Hit_DispPos.y + a->m_Hit_AddY), Gray50, Black, "%d", a->m_ArmerDamage);
							}
						}
					}
				}
				else {
					break;
				}
				loop++;
			}
			WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
		}
		//
		void			MainGameScene::MainDraw_Sub(void) const noexcept {
			SetFogEnable(TRUE);
			SetVerticalFogEnable(TRUE);
			BackGround::BackGroundClass::Instance()->Draw();
			ObjectManager::Instance()->Draw();
			//ObjectManager::Instance()->Draw_Depth();
			SetVerticalFogEnable(FALSE);
			SetFogEnable(FALSE);
#ifdef DEBUG
			auto* PlayerMngr = Player::PlayerManager::Instance();
			for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
				PlayerMngr->GetPlayer(i)->GetAI()->Draw();
			}
			for (auto& s : m_LineDebug) {
				int index = static_cast<int>(&s - &m_LineDebug.front());
				DrawCapsule3D(s.PosA.get(), s.PosB.get(), 0.02f * Scale3DRate, 8, GetColor(0, 0, (32 * index) % 256), GetColor(0, 0, 0), TRUE);
			}
#endif // DEBUG
		}
		//UI表示
		void			MainGameScene::DrawUI_Base_Sub(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SceneParts = SceneControl::Instance();
			//auto* NetBrowser = NetWorkBrowser::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetViewPlayerID())->GetChara();
			if (!Chara->IsAlive()) { return; }
			Chara->DrawReticle();										//レティクル表示
			DrawHitGraph();												//着弾表示
			if (!SceneParts->IsPause()) { this->m_UIclass.Draw(); }		//UI
			//NetBrowser->Draw();										//通信設定
			if (m_NetWorkController) {
				if (m_NetWorkController->GetPing() >= 0.f) {
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, (1920), (32), White, Black,
						"Ping:%3dms", static_cast<int>(m_NetWorkController->GetPing()));
				}
				else {
					if (m_NetWorkController->GetClient()) {
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, (1920), (32), White, Black,
							"Lost Connection");
					}
					else {
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, (1920), (32), White, Black,
							"Ping:---ms");
					}
				}
			}
			m_FadeControl.Draw();
		}
		//
		void			MainGameScene::UpdateBullet(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* SE = SoundPool::Instance();

			int loop = 0;
			while (true) {
				auto ammo = ObjMngr->GetObj(static_cast<int>(ObjType::Ammo), loop);
				if (ammo != nullptr) {
					auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
					if (a->IsActive()) {
						//AmmoClass
						Vector3DX repos_tmp = a->GetRePos();
						Vector3DX pos_tmp = a->GetPos();

						bool is_HitAll = false;

						Vector3DX norm_tmp = Vector3DX::zero();
						auto ColResGround = BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, &norm_tmp);

						for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
							auto& tgt = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
							HitPoint Damage = a->GetDamage();
							ArmerPoint ArmerDamage = 0;
							if (tgt->CheckDamageRay(&Damage, &ArmerDamage, true, (PlayerID)a->GetShootedID(), repos_tmp, &pos_tmp)) {
								a->Penetrate(pos_tmp, Damage, ArmerDamage);
								is_HitAll = true;
							}
						}
#ifdef DEBUG
						/*
						if (norm_tmp != Vector3DX::zero()) {
							m_LineDebug.emplace_back();
							m_LineDebug.back().PosA = pos_tmp;
							m_LineDebug.back().PosB = pos_tmp + norm_tmp * (1.f * Scale3DRate);
							m_LineDebug.back().Time = 10.f;
						}
						//*/
#endif // DEBUG
						if (ColResGround && !is_HitAll) {
							a->HitGround(pos_tmp);
							//破壊
							int								xput = 4;
							int								yput = 1;
							int								zput = 4;
							Vector3DX						PutPos = pos_tmp;
							auto Put = BackGround->GetPoint(PutPos);
							for (int xp = 0; xp < xput; xp++) {
								for (int yp = 0; yp < yput; yp++) {
									for (int zp = 0; zp < zput; zp++) {
										int xx = (Put.x + xp - xput / 2);
										int yy = (Put.y + yp - yput / 2);
										int zz = (Put.z + zp - zput / 2);
										auto& cell = BackGround->GetCellBuf(xx, yy, zz);
										switch (cell.m_Cell) {
										case 2:
											BackGround->SetBlick(xx, yy, zz, 3);
											break;
										case 3:
											BackGround->SetBlick(xx, yy, zz, FPS_n2::BackGround::s_EmptyBlick);
											break;
										default:
											break;
										}
									}
								}
							}
							m_EffectControl.SetOnce_Any(static_cast<int>(Sceneclass::Effect::ef_gndsmoke), pos_tmp, norm_tmp, a->GetCaliberSize() / 0.02f * Scale3DRate);
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGround0) + GetRand(5 - 1))->Play3D(pos_tmp, Scale3DRate * 10.f);
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
