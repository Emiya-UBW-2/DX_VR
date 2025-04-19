#pragma warning(disable:4464)
#include	"../MainScene/NetworkBrowser.hpp"
#include	"MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MainGameScene::Load_Sub(void) noexcept {
			BackGround::BackGroundControl::Create();
			Charas::GunAnimManager::Create();
			Charas::GunAnimManager::Instance()->Load("data/CharaAnime/");
			CommonBattleResource::Load();
			HitMarkerPool::Create();
			this->m_UIclass.Load();
			this->m_PauseMenuControl.Load();
			//
			{
				std::string Path = "data/Charactor/";
				std::vector<WIN32_FIND_DATA> pData;
				GetFileNamesInDirectory((Path + "*").c_str(), &pData);
				for (auto& data : pData) {
					std::string ChildPath = Path;
					ChildPath += data.cFileName;
					ChildPath += "/";
					ObjectManager::Instance()->LoadModelBefore(ChildPath.c_str());
				}
			}
			//
			{
				std::string Path = "data/gun/";
				std::vector<WIN32_FIND_DATA> pData;
				GetFileNamesInDirectory((Path + "*").c_str(), &pData);
				for (auto& data : pData) {
					std::string ChildPath = Path;
					ChildPath += data.cFileName;
					ChildPath += "/";
					ObjectManager::Instance()->LoadModelBefore(ChildPath.c_str());
					Guns::GunPartsDataManager::Instance()->Add(ChildPath.c_str());
				}
			}
			//
			{
				std::string Path = "data/Mods/";
				std::vector<WIN32_FIND_DATA> pData;
				GetFileNamesInDirectory((Path + "*").c_str(), &pData);
				for (auto& data : pData) {
					std::string ChildPath = Path;
					ChildPath += data.cFileName;
					ChildPath += "/";
					ObjectManager::Instance()->LoadModelBefore(ChildPath.c_str());
					Guns::GunPartsDataManager::Instance()->Add(ChildPath.c_str());
				}
			}
			//
			{
				std::string Path = "data/ammo/";
				std::vector<WIN32_FIND_DATA> pData;
				GetFileNamesInDirectory((Path + "*").c_str(), &pData);
				for (auto& data : pData) {
					std::string ChildPath = Path;
					ChildPath += data.cFileName;
					ChildPath += "/";
					ObjectManager::Instance()->LoadModelBefore(ChildPath.c_str());
					Objects::AmmoDataManager::Instance()->Add(ChildPath.c_str());
				}
			}

			MV1::Load("data/Charactor/Soldier/model_Rag.mv1", &m_RagDoll, DX_LOADMODEL_PHYSICS_REALTIME);//身体ラグドール

			ObjectManager::Instance()->LoadModelBefore("data/model/hindD/");
			ObjectManager::Instance()->LoadModelBefore("data/model/BMP3/");
		}
		void			MainGameScene::LoadEnd_Sub(void) noexcept {
			Player::PlayerManager::Create();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			PlayerMngr->Init(NetWork::Player_num);
			PlayerMngr->SetWatchPlayer(GetViewPlayerID());
			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
				//
				if (loop == PlayerMngr->GetWatchPlayer()) {
					Charas::CharacterObj::LoadChara("Main", (PlayerID)loop);
#if DEBUG_NET
					chara->LoadCharaGun("type20E", 0);
#else
					//*
					int Rand = GetRand(100);
					if (Rand < 30) {
						chara->LoadCharaGun("type89", 0);
					}
					else if (Rand < 30 + 40) {
						chara->LoadCharaGun("type20E", 0);
					}
					else {
						chara->LoadCharaGun("Mod870", 0);
					}
					//*/
					//chara->LoadCharaGun("Mod870", 0);
#endif
					chara->LoadCharaGun("P226", 1);
					chara->LoadCharaGun("RGD5", 2);
				}
				else {
					Charas::CharacterObj::LoadChara("Soldier", (PlayerID)loop);
					chara->LoadCharaGun("AKS-74", 0);
					chara->LoadCharaGun("RGD5", 2);
					//ラグドール
					chara->SetRagDoll().Duplicate(m_RagDoll);
					chara->SetupRagDoll();
				}
				//
				chara->SetPlayerID((PlayerID)loop);
				chara->SetCharaTypeID((loop == PlayerMngr->GetWatchPlayer()) ? CharaTypeID::Team : CharaTypeID::Enemy);
				//
				for (int loop2 = 0; loop2 < 3; ++loop2) {
					auto& g = chara->GetGunPtr(loop2);
					if (!g) { continue; }
					g->SetupGun();
				}
			}
			PlayerMngr->SetHelicopter(std::make_shared<Objects::HelicopterObj>());
			ObjectManager::Instance()->InitObject(PlayerMngr->GetHelicopter(), "data/model/hindD/");
			//PlayerMngr->SetVehicle(std::make_shared<Objects::VehicleObj>());
			//ObjectManager::Instance()->InitObject(PlayerMngr->GetVehicle(), "data/model/BMP3/");
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* OptionParts = OptionManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			//
			CommonBattleResource::Set();
			EffectSingleton::Create();
			NetWorkBrowser::Create();

			PostPassParts->SetShadowScale(0.5f);
			//
			BackGroundParts->Init();
			//
			Vector3DX LightVec = Vector3DX::vget(0.05f, -0.3f, 0.15f); LightVec = LightVec.normalized();
			PostPassParts->SetAmbientLight(LightVec);

			SetLightEnable(false);

			auto& FirstLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec);
			SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.0f, 0.96f, 0.94f, 1.0f));
			SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 0.96f, 0.94f, 1.0f));

			PostPassParts->SetGodRayPer(0.5f);
			//Cam
			CameraParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));
			//info
			constexpr float FarMax = std::min(std::min(BackGround::DrawMaxXPlus, BackGround::DrawMaxZPlus), BackGround::DrawMaxYPlus) * BackGround::CellScale;
			CameraParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale3DRate * 0.03f, FarMax);
			//Fog
			SetVerticalFogEnable(true);
			SetVerticalFogMode(DX_FOGMODE_LINEAR);
			SetVerticalFogStartEnd(-26.f * Scale3DRate, -10.f * Scale3DRate);
			SetVerticalFogColor(0, 0, 0);
			//Fog
			SetFogEnable(true);
			SetFogMode(DX_FOGMODE_LINEAR);
			SetFogStartEnd(FarMax, FarMax * 20.f);
			SetFogColor(114, 120, 128);
			//
			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
				//人の座標設定
				Vector3DX pos_t;
				pos_t.Set(GetRandf(10.f), -20.f, GetRandf(10.f));
				pos_t *= Scale3DRate;

				Vector3DX EndPos = pos_t - Vector3DX::up() * 200.f * Scale3DRate;
				if (BackGroundParts->CheckLinetoMap(pos_t + Vector3DX::up() * 0.f * Scale3DRate, &EndPos)) {
					pos_t = EndPos;
				}
				chara->Spawn(deg2rad(0.f), deg2rad(GetRand(360)), pos_t, 0);
			}
			//UI
			this->m_UIclass.Set();
			//
			this->m_DamageEvents.clear();
			this->m_PauseMenuControl.Init();
			this->m_FadeControl.Init();
			this->m_IsEnd = false;
			this->m_StartTimer = 3.f;

			auto* SE = SoundPool::Instance();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Envi))->Play(DX_PLAYTYPE_LOOP, true);

			//Vector3DX pos_t = Matrix3x3DX::Vtrans(Vector3DX::forward() * (15.f * Scale3DRate), Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180))));
			//pos_t.y = -25.f * Scale3DRate;

			//PlayerMngr->GetVehicle()->Spawn(std::atan2f(pos_t.x, pos_t.z), pos_t);
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
#if defined(DEBUG)
			DebugDraw::Instance()->SetPoint("Execute=Start");
#endif // DEBUG
			auto* CameraParts = Camera3D::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* Pad = PadControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* NetBrowser = NetWorkBrowser::Instance();
			auto* OptionParts = OptionManager::Instance();

			PlayerMngr->SetWatchPlayer(GetViewPlayerID());
			PostPassParts->SetLevelFilter(38, 154, 1.f);
			this->m_PauseMenuControl.Update();
			if (this->m_PauseMenuControl.IsRetire()) {
				if (!this->m_IsEnd) {
					this->m_FadeControl.SetBlackOut(true);
				}
				this->m_IsEnd = true;
			}
			if (this->m_IsEnd && this->m_FadeControl.IsAll()) {
				return false;
			}

			this->m_FadeControl.Update();

			auto& ViewChara = PlayerMngr->GetPlayer(PlayerMngr->GetWatchPlayer())->GetChara();

			Pad->SetMouseMoveEnable(true);
#if DEBUG_NET
			Pad->SetMouseMoveEnable(false);
#endif
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

						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::SQUAT), LocalizePool::Instance()->Get(9909));
						//KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::WALK), LocalizePool::Instance()->Get(9903));

						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD), LocalizePool::Instance()->Get(9904));

						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::THROW), LocalizePool::Instance()->Get(9905));
					}
				});
			if (SceneParts->IsPause()) {
				Pad->SetMouseMoveEnable(false);
				BackGroundParts->SettingChange();
				if (!this->m_NetWorkController) {
					return true;
				}
			}
			//FirstDoingv
			if (GetIsFirstLoop()) {}
			//Input,AI
			{
				if (!GetIsFirstLoop()) {
					this->m_StartTimer = std::max(this->m_StartTimer - DXLib_refParts->GetDeltaTime(), 0.f);
				}
				MyInput.ResetAllInput();
				if (!SceneParts->IsPause() && this->m_FadeControl.IsClear() && (this->m_StartTimer <= 0.f)) {
					float AimPer = 1.f / std::max(1.f, ViewChara->GetIsADS() ? ViewChara->GetGunPtrNow()->GetSightZoomSize() : 1.f);
					MyInput.SetAddxRad(Pad->GetLS_Y() / 200.f * AimPer);
					MyInput.SetAddyRad(Pad->GetLS_X() / 200.f * AimPer);
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
					MyInput.SetInputPADS(Controls::PADS::SQUAT, Pad->GetPadsInfo(Controls::PADS::SQUAT).GetKey().press());
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
				if (NetBrowser->IsDataReady() && !this->m_NetWorkController) {
					this->m_NetWorkController = std::make_unique<NetWork::NetWorkController>(NetBrowser->GetClient(), NetBrowser->GetNetSetting().UsePort, NetBrowser->GetNetSetting().IP, NetBrowser->GetServerPlayer());
				}
				if (this->m_NetWorkController) {
					NetWork::MoveInfo MoveInfoData;
					int32_t FreeData[10]{};

					FreeData[0] = ViewChara->GetAutoAimID();
					FreeData[1] = ViewChara->GetAutoAimPos();

					MoveInfoData.repos = ViewChara->GetMove().GetRePos();
					MoveInfoData.pos = ViewChara->GetMove().GetPos();
					MoveInfoData.vec = ViewChara->GetMove().GetVec();
					MoveInfoData.mat = ViewChara->GetMove().GetMat();
					MoveInfoData.WatchRad = ViewChara->GetRotateRad();
					this->m_NetWorkController->SetLocalData().SetMyPlayer(MyInput, MoveInfoData, ViewChara->GetDamageEvent(), FreeData);
					this->m_NetWorkController->Update();
					ViewChara->SetDamageEventReset();
					//ホストならBMPの判定もやる
					if (!this->m_NetWorkController->GetClient() && this->m_NetWorkController->GetServerPlayer()) {
						//PlayerMngr->GetVehicle()->GetDamageEvent()//TODO
						//PlayerMngr->GetVehicle()->SetDamageEventReset();
					}
				}
				if (this->m_NetWorkController && this->m_NetWorkController->IsInGame()) {//オンライン
					bool IsServerNotPlayer = !this->m_NetWorkController->GetClient() && !this->m_NetWorkController->GetServerPlayer();
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						NetWork::PlayerNetData Ret = this->m_NetWorkController->GetServerPlayerData((PlayerID)loop);
						if (loop == PlayerMngr->GetWatchPlayer() && !IsServerNotPlayer) {
							chara->Input(MyInput);//自身が動かすもの
						}
						else {//サーバーからのデータで動くもの
							//サーバーがCPUを動かす場合
							if (!this->m_NetWorkController->GetClient()) {
								//cpu
								//PlayerMngr->GetPlayer(loop)->GetAI()->Execute(&MyInput);//AIに入力させる
							}
							//サーバーからのデータでキャラを動かす
							chara->OverrideAutoAimID(static_cast<PlayerID>(Ret.GetPlayerSendData().GetFreeData()[0]), Ret.GetPlayerSendData().GetFreeData()[1]);
							chara->Input(Ret.GetPlayerSendData().GetInput());
							chara->SetMoveOverRide(Ret.GetPlayerSendData().GetMove());
						}
						//このプレイヤーが出したダメージイベントをリストに追加
						Ret.PopDamageEvent(&this->m_DamageEvents);
					}
				}
				else {//オフライン
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						if (loop == PlayerMngr->GetWatchPlayer()) {
							chara->Input(MyInput);
						}
						else {
							InputControl OtherInput;
							PlayerMngr->GetPlayer(loop)->GetAI()->Execute(&OtherInput);//AIに入力させる
							chara->Input(OtherInput);
						}
						//このプレイヤーが出したダメージイベントをリストに追加
						chara->PopDamageEvent(&this->m_DamageEvents);
					}
					//PlayerMngr->GetVehicle()->PopDamageEvent(&this->m_DamageEvents);
				}
				//ダメージイベント
				for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
					auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
					for (int loop2 = 0, Num = static_cast<int>(this->m_DamageEvents.size()); loop2 < Num; ++loop2) {
						if (chara->SetDamageEvent(this->m_DamageEvents[static_cast<size_t>(loop2)])) {
							std::swap(this->m_DamageEvents.back(), this->m_DamageEvents[static_cast<size_t>(loop2)]);
							this->m_DamageEvents.pop_back();
							--Num;
							--loop2;
						}
					}
				}
			}

			//PlayerMngr->GetVehicle()->SetInput(MyInput, true);

			//Execute
			ObjMngr->ExecuteObject();
			ObjMngr->LateExecuteObject();
			//視点
			{
				//カメラ
				Vector3DX BaseCamPos = ViewChara->GetCameraPosition();
				CameraParts->SetMainCamera().SetCamPos(
					BaseCamPos + Camera3D::Instance()->GetCamShake(),
					BaseCamPos + ViewChara->GetEyeRotationCache().zvec() * -1.f + Camera3D::Instance()->GetCamShake() * 2.f,
					ViewChara->GetEyeRotationCache().yvec());
#if defined(DEBUG) && DEBUG_CAM
				if (CheckHitKey(KEY_INPUT_F1) != 0) {
					DBG_CamSelect = -1;
				}
				if (CheckHitKey(KEY_INPUT_F2) != 0) {
					DBG_CamSelect = 0;
				}
				if (CheckHitKey(KEY_INPUT_F3) != 0) {
					DBG_CamSelect = 1;
				}
				if (CheckHitKey(KEY_INPUT_F4) != 0) {
					DBG_CamSelect = 2;
				}
				if (CheckHitKey(KEY_INPUT_F5) != 0) {
					DBG_CamSelect = 3;
				}
				if (0 <= DBG_CamSelect && DBG_CamSelect <= 3) {
					Vector3DX CamPos = CameraParts->GetMainCamera().GetCamPos();
					Vector3DX CamVec = CameraParts->GetMainCamera().GetCamVec();
					Vector3DX CamUp = CameraParts->GetMainCamera().GetCamUp();
					Matrix3x3DX Rot = Matrix3x3DX::Axis1(CamUp, (CamVec - CamPos).normalized());
					switch (DBG_CamSelect) {
					case 0:
						CamVec = CamPos;
						CamPos += Rot.xvec() * (3.f * Scale3DRate);
						break;
					case 1:
						CamVec = CamPos;
						CamPos += Rot.xvec() * (-3.f * Scale3DRate);
						break;
					case 2:
						CamVec = CamPos;
						CamPos += Rot.yvec() * (3.f * Scale3DRate) + Rot.zvec() * 0.1f;
						break;
					case 3:
						CamVec = CamPos;
						CamPos += Rot.zvec() * (-3.f * Scale3DRate);
						break;
					default:
						break;
					}
					CameraParts->SetMainCamera().SetCamPos(CamPos, CamVec, CamUp);
				}
#endif
				auto fov_t = CameraParts->GetMainCamera().GetCamFov();
				//fov
				{
					float fov = deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov));
					if (ViewChara->GetIsADS()) {
						fov -= deg2rad(15);
						fov /= std::max(1.f, ViewChara->GetGunPtrNow()->GetSightZoomSize() / 2.f);
					}
					if (ViewChara->GetGunPtrNow() && ViewChara->GetGunPtrNow()->GetShotSwitch()) {
						fov -= deg2rad(5);
						Easing(&fov_t, fov, 0.5f, EasingType::OutExpo);
					}
					else {
						Easing(&fov_t, fov, 0.8f, EasingType::OutExpo);
					}
#if defined(DEBUG) && DEBUG_CAM
					if (0 <= DBG_CamSelect && DBG_CamSelect <= 3) {
						fov_t = deg2rad(15);
					}
#endif
				}
				CameraParts->SetMainCamera().SetCamInfo(fov_t, CameraParts->GetMainCamera().GetCamNear(), CameraParts->GetMainCamera().GetCamFar());
			}

			//PlayerMngr->GetVehicle()->SetCam(CameraParts->SetMainCamera());


#if defined(DEBUG)
			auto Put = CameraParts->GetMainCamera().GetCamPos() / Scale3DRate;
			printfDx("%f,%f,%f\n", Put.x, Put.y, Put.z);
#endif
			//DoF
			PostPassEffect::Instance()->Set_DoFNearFar(
				ViewChara->GetIsADS() ? (Scale3DRate * 0.3f) : (Scale3DRate * 0.15f), ViewChara->GetIsADS() ? (CameraParts->GetMainCamera().GetCamFar() * 0.8f) : Scale3DRate * 5.f,
				ViewChara->GetIsADS() ? (Scale3DRate * 0.1f) : (Scale3DRate * 0.05f), CameraParts->GetMainCamera().GetCamFar());
			//埃エフェクト
			if (GetIsFirstLoop()) {
				this->m_EffectPos = CameraParts->GetMainCamera().GetCamPos();
				EffectSingleton::Instance()->SetLoop(Effect::ef_dust, this->m_EffectPos);
			}
			else {
				Easing(&this->m_EffectPos, CameraParts->GetMainCamera().GetCamPos(), 0.95f, EasingType::OutExpo);
				EffectSingleton::Instance()->Update_LoopEffect(Effect::ef_dust, this->m_EffectPos, Vector3DX::forward(), 0.5f);
				EffectSingleton::Instance()->SetEffectColor(Effect::ef_dust, 255, 255, 255, 64);
			}
			//背景
			BackGroundParts->Execute();
			//UIパラメーター
			{
				//timer
				this->m_UIclass.SetfloatParam(0, 0.f);
				this->m_UIclass.SetfloatParam(1, this->m_StartTimer);

				this->m_UIclass.Update();
			}
			HitMarkerPool::Instance()->Update();
			EffectSingleton::Instance()->Update();
#if defined(DEBUG)
			DebugDraw::Instance()->SetPoint("Execute=End");
#endif // DEBUG
			return true;
		}
		void			MainGameScene::Dispose_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Envi))->StopAll();
			EffectSingleton::Instance()->StopEffect(Effect::ef_dust);
			//使い回しオブジェ系
			BackGround::BackGroundControl::Instance()->Dispose();
			//
			if (this->m_NetWorkController) {
				this->m_NetWorkController.reset();
			}
			{
				auto* PostPassParts = PostPassEffect::Instance();
				PostPassParts->SetLevelFilter(0, 255, 1.f);
				PostPassParts->SetAberrationPower(1.f);
				PostPassParts->Set_is_Blackout(false);
				PostPassParts->Set_Per_Blackout(0.f);
				PostPassParts->Set_is_lens(false);
				PostPassParts->Set_zoom_lens(1.f);
			}
			EffectSingleton::Release();
			NetWorkBrowser::Release();
			if (this->m_IsEnd) {//タイトルに戻る
				SetNextSelect(0);
			}
			else {//次のシーンへ
				SetNextSelect(1);
			}
		}
		void			MainGameScene::Dispose_Load_Sub(void) noexcept {
			BackGround::BackGroundControl::Release();
			CommonBattleResource::Dispose();
			this->m_UIclass.Dispose();
			Player::PlayerManager::Release();
			ObjectManager::Instance()->DeleteAll();
			this->m_PauseMenuControl.Dispose();
			HitMarkerPool::Release();
			Charas::GunAnimManager::Release();
			m_RagDoll.Dispose();
		}
		//
		void			MainGameScene::MainDraw_Sub(int Range) const noexcept {
			BackGround::BackGroundControl::Instance()->Draw();
			ObjectManager::Instance()->Draw(true, Range);
			//ObjectManager::Instance()->Draw_Depth();
			HitMarkerPool::Instance()->Check();
		}
		//UI表示
		void			MainGameScene::DrawUI_Base_Sub(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto& ViewChara = PlayerMngr->GetPlayer(PlayerMngr->GetWatchPlayer())->GetChara();
			if (!ViewChara->IsAlive()) { return; }
			//レティクル表示
			if (ViewChara->GetGunPtrNow()) {
				ViewChara->GetGunPtrNow()->DrawReticle(ViewChara->GetLeanRad());
			}
			HitMarkerPool::Instance()->Draw();
			if (!SceneParts->IsPause()) { this->m_UIclass.Draw(); }		//UI
#if DEBUG_NET
			NetWorkBrowser::Instance()->Draw();						//通信設定
#endif
			if (this->m_NetWorkController) {
				std::string PingMes;
				if (this->m_NetWorkController->GetPing() >= 0.f) {
					char Mes[260];
					sprintfDx(Mes, "Ping:%3dms", static_cast<int>(this->m_NetWorkController->GetPing()));

					PingMes = Mes;
				}
				else {
					if (this->m_NetWorkController->GetClient()) {
						PingMes = "Lost Connection";
					}
					else {
						PingMes = "Ping:---ms";
					}
				}
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, (1920), (64), White, Black,
					PingMes.c_str());
			}
			this->m_FadeControl.Draw();
		}
	}
}
