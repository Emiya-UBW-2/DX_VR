#pragma warning(disable:4464)
#include	"../MainScene/NetworkBrowser.hpp"
#include	"MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		constexpr float FarMax = std::min(std::min(BackGround::DrawMaxXPlus, BackGround::DrawMaxZPlus), BackGround::DrawMaxYPlus) * BackGround::CellScale;

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
					ObjectManager::Instance()->LoadModelBefore(ChildPath);
				}
			}
			//
			{
				std::string Path = "data/Item/";
				std::vector<WIN32_FIND_DATA> pData;
				GetFileNamesInDirectory((Path + "*").c_str(), &pData);
				for (auto& data : pData) {
					std::string ChildPath = Path;
					ChildPath += data.cFileName;
					ChildPath += "/";
					ObjectManager::Instance()->LoadModelBefore(ChildPath);
					Objects::ItemObjDataManager::Instance()->Add(ChildPath);
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
					ObjectManager::Instance()->LoadModelBefore(ChildPath);
					Guns::GunPartsDataManager::Instance()->Add(ChildPath);
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
					ObjectManager::Instance()->LoadModelBefore(ChildPath);
					Guns::GunPartsDataManager::Instance()->Add(ChildPath);
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
					ObjectManager::Instance()->LoadModelBefore(ChildPath);
					Objects::AmmoDataManager::Instance()->Add(ChildPath);
				}
			}

			MV1::Load("data/Charactor/Main/model_Rag.mv1", &m_MainRagDoll, DX_LOADMODEL_PHYSICS_REALTIME);//身体ラグドール
			MV1::Load("data/Charactor/Soldier/model_Rag.mv1", &m_RagDoll, DX_LOADMODEL_PHYSICS_REALTIME);//身体ラグドール

			ObjectManager::Instance()->LoadModelBefore("data/model/hindD/");
			ObjectManager::Instance()->LoadModelBefore("data/model/BMP3/");
		}
		void			MainGameScene::LoadEnd_Sub(void) noexcept {
			Objects::AmmoPool::Create();
			Objects::AmmoLinePool::Create();
			Objects::ItemObjPool::Create();
			Player::PlayerManager::Create();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			PlayerMngr->Init(NetWork::Player_num);
			PlayerMngr->SetWatchPlayerID(GetViewPlayerID());

			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
				//
				if (loop == PlayerMngr->GetWatchPlayerID()) {
					Charas::CharacterObj::LoadChara("Main", (PlayerID)loop);
#if DEBUG_NET
					chara->LoadCharaGun("type20E", 0);
#else
					//*
					{
						int Rand = GetRand(100);
						int rate = 100 / 5;
						if (Rand < rate) {
							chara->LoadCharaGun("type89", 0);
						}
						else if (Rand < rate * 2) {
							chara->LoadCharaGun("type20E", 0);
						}
						else if (Rand < rate * 3) {
							chara->LoadCharaGun("M700", 0);
						}
						else if (Rand < rate * 4) {
							chara->LoadCharaGun("AKS-74", 0);
						}
						else {
							chara->LoadCharaGun("Mod870", 0);
						}
					}
					//*/
					//chara->LoadCharaGun("M700", 0);
#endif
					{
						int Rand = GetRand(100);
						int rate = 100 / 2;
						if (Rand < rate) {
							chara->LoadCharaGun("P226", 1);
						}
						else {
							chara->LoadCharaGun("MP443", 1);
						}
					}
					chara->LoadCharaGun("RGD5", 2);
					//ラグドール
					chara->SetupRagDoll(m_MainRagDoll);
				}
				else {
					Charas::CharacterObj::LoadChara("Soldier", (PlayerID)loop);

					int Rand = GetRand(100);
					int rate = 100 / 2;
					if (Rand < rate) {
						chara->LoadCharaGun("type89", 0);
					}
					else {
						chara->LoadCharaGun("AKS-74", 0);
					}
					//chara->LoadCharaGun("MP443", 1);
					chara->LoadCharaGun("RGD5", 2);
					//ラグドール
					chara->SetupRagDoll(m_RagDoll);
				}
				//
				chara->SetPlayerID((PlayerID)loop);
				chara->SetCharaTypeID((loop == PlayerMngr->GetWatchPlayerID()) ? CharaTypeID::Team : CharaTypeID::Enemy);
				//
				for (int loop2 = 0; loop2 < 3; ++loop2) {
					auto& gun = chara->GetGunPtr(loop2);
					if (!gun) { continue; }
					gun->SetupGun();
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
			auto* CameraParts = Camera3D::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			//
			CommonBattleResource::Set();
			EffectSingleton::Create();
			NetWorkBrowser::Create();

			PostPassParts->SetShadowScale(0.5f);
			//
			BackGroundParts->Init();
			//
			Vector3DX LightVec = Vector3DX::vget(0.05f, -0.2f, -0.15f); LightVec = LightVec.normalized();
			PostPassParts->SetAmbientLight(LightVec);

			SetLightEnable(false);

			auto& FirstLight = LightPool::Instance()->Put(LightType::DIRECTIONAL, LightVec);
			SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.0f, 0.9f, 0.8f, 1.0f));
			SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 0.9f, 0.8f, 1.0f));

			PostPassParts->SetGodRayPer(0.5f);
			//Cam
			CameraParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));
			//info
			CameraParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale3DRate * 0.03f, Scale3DRate * 100.0f);
			//Fog
			SetVerticalFogEnable(true);
			SetVerticalFogMode(DX_FOGMODE_LINEAR);
			SetVerticalFogStartEnd(-26.0f * Scale3DRate, -10.0f * Scale3DRate);
			SetVerticalFogColor(0, 0, 0);
			//Fog
			SetFogEnable(true);
			SetFogMode(DX_FOGMODE_LINEAR);
			SetFogStartEnd(FarMax, FarMax * 20.0f);
			SetFogColor(128, 110, 110);
			//
			auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();

			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
				Vector3DX TargetPos;
				if (loop == PlayerMngr->GetWatchPlayerID()) {
					TargetPos = BackGroundParts->GetBuildData().at(static_cast<size_t>(GetRand(static_cast<int>(BackGroundParts->GetBuildData().size()) - 1))).GetPos();
				}
				else {
					while (true) {
						TargetPos = BackGroundParts->GetBuildData().at(static_cast<size_t>(GetRand(static_cast<int>(BackGroundParts->GetBuildData().size()) - 1))).GetPos();
						Vector3DX Vec = ViewChara->GetMove().GetPos() - TargetPos; Vec.y = 0.f;
						if (Vec.sqrMagnitude() > (5.f * Scale3DRate) * (5.f * Scale3DRate)) {
							//
							Vector3DX EndPos = TargetPos - Vector3DX::up() * 50.0f * Scale3DRate;
							if (BackGroundParts->CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
								TargetPos = EndPos;
							}
							EndPos = TargetPos;
							if (BackGroundParts->CheckLinetoMap(ViewChara->GetMove().GetPos() + Vector3DX::up() * 1.0f * Scale3DRate, &EndPos) != 0) {
								break;
							}
						}
					}
				}
				//人の座標設定
				if (loop == PlayerMngr->GetWatchPlayerID()) {
					chara->Spawn(deg2rad(0.0f), deg2rad(GetRand(360)), TargetPos, 0, true);
				}
				else{
					chara->Spawn(deg2rad(0.0f), deg2rad(GetRand(360)), TargetPos, 0, true);
				}
			}

			for (int loop = 0; loop < 10; ++loop) {
				Vector3DX TargetPos;
				while (true) {
					TargetPos = BackGroundParts->GetBuildData().at(static_cast<size_t>(GetRand(static_cast<int>(BackGroundParts->GetBuildData().size()) - 1))).GetPos();
					Vector3DX Vec = ViewChara->GetMove().GetPos() - TargetPos; Vec.y = 0.f;
					if (Vec.sqrMagnitude() > (5.f * Scale3DRate) * (5.f * Scale3DRate)) {
						break;
					}
				}
				{
					Vector3DX EndPos = TargetPos - Vector3DX::up() * 50.0f * Scale3DRate;
					if (BackGroundParts->CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
						TargetPos = EndPos;
					}
				}
				Objects::ItemObjPool::Instance()->Put(loop % Objects::ItemObjDataManager::Instance()->GetList().size(),
					TargetPos,
					Vector3DX::vget(GetRandf(1.f), 1.f, GetRandf(1.f)) * Scale3DRate * 0.01f
				);
			}
			//UI
			this->m_UIclass.Set();
			//
			this->m_PauseMenuControl.Init();
			FadeControl::Instance()->Init();
			this->m_IsEnd = false;
			this->m_StartTimer = 3.0f;

			auto* SE = SoundPool::Instance();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Envi))->Play(DX_PLAYTYPE_LOOP, true);

			//Vector3DX posBuf = Matrix3x3DX::Vtrans(Vector3DX::forward() * (15.0f * Scale3DRate), Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180))));
			//posBuf.y = -25.0f * Scale3DRate;
			//PlayerMngr->GetVehicle()->Spawn(std::atan2f(posBuf.x, posBuf.z), posBuf);
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
			auto* CameraParts = Camera3D::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* Pad = PadControl::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* NetBrowser = NetWorkBrowser::Instance();
			auto* OptionParts = OptionManager::Instance();

			PlayerMngr->SetWatchPlayerID(GetViewPlayerID());
			PostPassParts->SetLevelFilter(38, 154, 1.0f);
			this->m_PauseMenuControl.Update();
			if (this->m_PauseMenuControl.IsRetire()) {
				if (!this->m_IsEnd) {
					FadeControl::Instance()->SetBlackOut(true);
				}
				this->m_IsEnd = true;
			}
			if (this->m_IsEnd && FadeControl::Instance()->IsAll()) {
				return false;
			}

			FadeControl::Instance()->Update();

			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
			auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();

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
				BackGround::BackGroundControl::Instance()->SettingChange();
				if (!this->m_NetWorkController) {
					return true;
				}
			}
			//FirstDoingv
			if (GetIsFirstLoop()) {}
			//Input,AI
			{
				if (!GetIsFirstLoop()) {
					this->m_StartTimer = std::max(this->m_StartTimer - DXLib_refParts->GetDeltaTime(), 0.0f);
				}
				MyInput.ResetAllInput();
				if (!SceneParts->IsPause() && FadeControl::Instance()->IsClear() && (this->m_StartTimer <= 0.0f)) {
					float AimPer = 1.0f / std::max(1.0f, ViewChara->GetIsADS() ? ViewChara->GetGunPtrNow()->GetSightZoomSize() : 1.0f);
					MyInput.SetAddxRad(Pad->GetLS_Y() / 200.0f * AimPer);
					MyInput.SetAddyRad(Pad->GetLS_X() / 200.0f * AimPer);
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

					int loop = 0;
					for (auto& ID : ViewPlayer->GetInventory()) {
						if (ID != InvalidID) {
							bool IsPress = false;
							switch (loop) {
							case 0: IsPress = CheckHitKey(KEY_INPUT_1) != 0; break;
							case 1: IsPress = CheckHitKey(KEY_INPUT_2) != 0; break;
							case 2: IsPress = CheckHitKey(KEY_INPUT_3) != 0; break;
							case 3: IsPress = CheckHitKey(KEY_INPUT_4) != 0; break;
							case 4: IsPress = CheckHitKey(KEY_INPUT_5) != 0; break;
							default:
								break;
							}
							if (IsPress) {
								//auto& item = Objects::ItemObjDataManager::Instance()->GetList().at(ID);
								Vector3DX Vec = ViewChara->GetEyeRotationCache().zvec() * -1.f;
								Vec.y = std::clamp(Vec.y, 0.1f, 0.3f); Vec = Vec.normalized();

								Objects::ItemObjPool::Instance()->Put(ID,
									ViewChara->GetFrameWorldMat(Charas::CharaFrame::Upper2).pos(),
									Vec * (10.f * Scale3DRate)
								);
								ViewPlayer->SubInventoryIndex(loop);
							}
						}
						++loop;
					}
				}
				//ネットワーク
				if (NetBrowser->IsDataReady() && !this->m_NetWorkController) {
					this->m_NetWorkController = std::make_unique<NetWork::NetWorkController>(NetBrowser->IsServer(), NetBrowser->GetNetSetting().UsePort, NetBrowser->GetNetSetting().IP, NetBrowser->GetServerPlayer());
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

					m_LocalSend.SetMyPlayer(MyInput, MoveInfoData, ViewChara->GetDamageEvent(), FreeData);
					ViewChara->SetDamageEventReset();
					if (this->m_NetWorkController->IsServer()) {
						//ホストならBMP、ヘリの射撃判定もやる
						if (PlayerMngr->GetVehicle()) {
							//PlayerMngr->GetVehicle()->GetDamageEvent()//TODO
							PlayerMngr->GetVehicle()->SetDamageEventReset();
						}
						if (PlayerMngr->GetHelicopter()) {
							//PlayerMngr->GetHelicopter()->GetDamageEvent()//TODO
							PlayerMngr->GetHelicopter()->SetDamageEventReset();
						}
					}
					this->m_NetWorkController->Update(m_LocalSend);
				}
				std::vector<DamageEvent>	DamageEvents;
				if (this->m_NetWorkController && this->m_NetWorkController->IsInGame()) {//オンライン
					bool IsServerNotPlayer = this->m_NetWorkController->IsServer() && !this->m_NetWorkController->GetServerPlayer();//サーバーだけど遊ばないよ
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						NetWork::PlayerDataOnNetwork Ret = this->m_NetWorkController->GetServerPlayerData((PlayerID)loop);
						if (loop == PlayerMngr->GetWatchPlayerID() && !IsServerNotPlayer) {
							chara->Input(MyInput);//自身が動かすもの
						}
						else {//サーバーからのデータで動くもの
							if (this->m_NetWorkController->IsServer()) {
								//サーバーがCPUを動かす場合
							}
							//サーバーからのデータでキャラを動かす
							chara->OverrideAutoAimID(static_cast<PlayerID>(Ret.GetPlayerSendData().GetFreeData()[0]), Ret.GetPlayerSendData().GetFreeData()[1]);
							chara->Input(Ret.GetPlayerSendData().GetInput());
							chara->SetMoveOverRide(Ret.GetPlayerSendData().GetMove());
						}
						//このプレイヤーが出したダメージイベントをリストに追加
						Ret.PopDamageEvent(&DamageEvents);
					}
				}
				else {//オフライン
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						if (loop == PlayerMngr->GetWatchPlayerID()) {
							chara->Input(MyInput);
						}
						else {
							if (!SceneParts->IsPause() && FadeControl::Instance()->IsClear() && (this->m_StartTimer <= 0.0f)) {
								chara->Input(PlayerMngr->GetPlayer(loop)->GetAI()->Update());//AIに入力させる
							}
						}
						//このプレイヤーが出したダメージイベントをリストに追加
						chara->PopDamageEvent(&DamageEvents);
					}
					//PlayerMngr->GetVehicle()->PopDamageEvent(&DamageEvents);
					PlayerMngr->GetHelicopter()->PopDamageEvent(&DamageEvents);
				}
				//ダメージイベント
				for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
					auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
					for (size_t loop2 = 0, Max = DamageEvents.size(); loop2 < Max; ++loop2) {
						if (chara->SetDamageEvent(DamageEvents[loop2])) {
							std::swap(DamageEvents[Max - 1], DamageEvents[loop2]);
							//DamageEvents.pop_back();//ループ範囲外なのでやらなくてよい
							--Max;
							--loop2;
						}
					}
				}
				DamageEvents.clear();

				if (!ViewChara->IsAlive()) {
					if (!this->m_IsEnd) {
						FadeControl::Instance()->SetBlackOut(true, 3.f);
					}
					this->m_IsEnd = true;
				}
			}

			//PlayerMngr->GetVehicle()->SetInput(MyInput, true);

			PlayerMngr->m_FindCount = std::max(PlayerMngr->m_FindCount - DXLib_refParts->GetDeltaTime(), 0.f);
			//ほかプレイヤーとの判定
			if (this->m_NetWorkController && this->m_NetWorkController->IsInGame()) {//オンライン
				float Radius = 2.0f * 1.f * Scale3DRate;
				for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
					auto& player = PlayerMngr->GetPlayer(loop);
					auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
					if (!chara->IsAlive()) { continue; }
					if (chara->GetIsRappelling()) { continue; }
					auto Dir = chara->GetEyeRotationCache().zvec() * -1.f;
					auto Dir_XZ = Dir; Dir_XZ.y = (0.f);

					//自分が当たったら押し出す
					for (auto& g : Objects::ItemObjPool::Instance()->GetList()) {
						if (!g->IsActive()) { continue; }
						if (!g->CanPick()) { continue; }
						Vector3DX Vec = (chara->GetMove().GetPos() - g->GetMove().GetPos()); Vec.y = (0.0f);
						float Len = Vec.magnitude();
						if (Len < Radius) {
							Vector3DX EndPos = g->GetMove().GetPos() + Vector3DX::up() * (1.f * Scale3DRate);
							if (
								(player->HasEmptyInventory() != 0) &&
								((Vector3DX::Dot(Dir_XZ.normalized(), Vec.normalized() * -1.f)) > 0.f) && 
								(BackGround::BackGroundControl::Instance()->CheckLinetoMap(ViewChara->GetEyePositionCache(), &EndPos) == 0)
								) {
								player->AddInventory(g->GetUniqueID());
								g->SetActive(false);
								continue;
							}
							else {
								g->SetMove().SetPos(g->GetMove().GetPos() + Vec.normalized() * (Len - Radius));
								g->SetMove().Update(0.f, 0.f);
							}
						}
					}
				}
			}
			else {
				if (ViewChara->IsAlive()) {
					float Radius = 2.0f * 1.f * Scale3DRate;
					//自分が当たったら押し出す 取れるなら取る
					auto Dir = ViewChara->GetEyeRotationCache().zvec() * -1.f;
					auto Dir_XZ = Dir; Dir_XZ.y = (0.f);

					//自分が当たったら押し出す
					for (auto& g : Objects::ItemObjPool::Instance()->GetList()) {
						if (!g->IsActive()) { continue; }
						if (!g->CanPick()) { continue; }
						Vector3DX Vec = (ViewChara->GetMove().GetPos() - g->GetMove().GetPos()); Vec.y = (0.0f);
						float Len = Vec.magnitude();
						if (Len < Radius) {
							Vector3DX EndPos = g->GetMove().GetPos() + Vector3DX::up() * (1.f * Scale3DRate);
							if (
								(ViewPlayer->HasEmptyInventory() != 0) &&
								((Vector3DX::Dot(Dir_XZ.normalized(), Vec.normalized() * -1.f)) > 0.f) &&
								(BackGround::BackGroundControl::Instance()->CheckLinetoMap(ViewChara->GetEyePositionCache(), &EndPos) == 0)
								) {
								ViewPlayer->AddInventory(g->GetUniqueID());
								g->SetActive(false);
								continue;
							}
							else {
								g->SetMove().SetPos(g->GetMove().GetPos() + Vec.normalized() * (Len - Radius));
								g->SetMove().Update(0.f, 0.f);
							}
						}
					}
				}
			}
			//Update
			ObjMngr->UpdateObject();
			ObjMngr->LateUpdateObject();
			Objects::AmmoLinePool::Instance()->Update();
			//視点
			auto& CamChara = PlayerMngr->GetPlayer(0)->GetChara();//PlayerMngr->GetWatchPlayer()->GetChara();
			{
				//カメラ
				Vector3DX BaseCamPos = CamChara->GetCameraPosition();
				CameraParts->SetMainCamera().SetCamPos(
					BaseCamPos + Camera3D::Instance()->GetCamShake(),
					BaseCamPos + CamChara->GetEyeRotationCache().zvec2() + Camera3D::Instance()->GetCamShake() * 2.0f,
					CamChara->GetEyeRotationCache().yvec());
#if defined(DEBUG) && DEBUG_CAM
				if (CheckHitKey(KEY_INPUT_F1) != 0) {
					DBG_CamSelect = InvalidID;
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
						CamPos += Rot.xvec() * (3.0f * Scale3DRate);
						break;
					case 1:
						CamVec = CamPos;
						CamPos += Rot.xvec() * (-3.0f * Scale3DRate);
						break;
					case 2:
						CamVec = CamPos;
						CamPos += Rot.yvec() * (3.0f * Scale3DRate) + Rot.zvec2() * 0.1f;
						break;
					case 3:
						CamVec = CamPos;
						CamPos += Rot.zvec2() * (3.0f * Scale3DRate);
						break;
					default:
						break;
					}
					CameraParts->SetMainCamera().SetCamPos(CamPos, CamVec, CamUp);
				}
#endif
				auto fovBuf = CameraParts->GetMainCamera().GetCamFov();
				//fov
				{
					float fovTarget = deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov));
					if (CamChara->IsAlive()) {
						if (CamChara->GetIsADS()) {
							fovTarget -= deg2rad(15);
							fovTarget /= std::max(1.0f, CamChara->GetGunPtrNow()->GetSightZoomSize() / 2.0f);
						}
						if (CamChara->GetGunPtrNow() && CamChara->GetGunPtrNow()->GetShotSwitch()) {
							fovTarget -= deg2rad(8) * CamChara->GetGunPtrNow()->GetRecoilRandViewScale();
							Easing(&fovBuf, fovTarget, 0.5f, EasingType::OutExpo);
						}
						else {
							Easing(&fovBuf, fovTarget, 0.8f, EasingType::OutExpo);
						}
					}
					else {
						Easing(&fovBuf, fovTarget, 0.8f, EasingType::OutExpo);
					}
#if defined(DEBUG) && DEBUG_CAM
					if (0 <= DBG_CamSelect && DBG_CamSelect <= 3) {
						fovBuf = deg2rad(65);
					}
#endif
				}
				CameraParts->SetMainCamera().SetCamInfo(fovBuf, CameraParts->GetMainCamera().GetCamNear(), CameraParts->GetMainCamera().GetCamFar());
			}

			{
				Easing(&AberrationPower,
					(CamChara->IsLowHP()) ? (10.f + GetRandf(30.f)) : 1.f,
					0.95f, EasingType::OutExpo);
				if (CamChara->PopHeadShotSwitch()) {
					auto* SE = SoundPool::Instance();
					SE->Get(SoundType::SE, (int)SoundEnum::Tinnitus)->Play3D(CamChara->GetEyePositionCache(), Scale3DRate * 50.f);
					CamChara->SetHeadShot();
					AberrationPower = 30.f;
				}
				PostPassParts->SetAberrationPower(AberrationPower);
			}
			//PlayerMngr->GetVehicle()->SetCam(CameraParts->SetMainCamera());


#if defined(DEBUG) && FALSE
			auto Put = CameraParts->GetMainCamera().GetCamPos() / Scale3DRate;
			printfDx("%f,%f,%f\n", Put.x, Put.y, Put.z);
#endif
			//DoF
			PostPassEffect::Instance()->Set_DoFNearFar(
				CamChara->GetIsADS() ? (Scale3DRate * 0.3f) : (Scale3DRate * 0.15f), CamChara->GetIsADS() ? (FarMax * 0.8f) : Scale3DRate * 5.0f,
				CamChara->GetIsADS() ? (Scale3DRate * 0.1f) : (Scale3DRate * 0.05f), FarMax);
			//埃エフェクト
			if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 2) {
				if (!EffectSingleton::Instance()->IsPlayLoopEffect(Effect::ef_dust)) {
					this->m_EffectPos = CameraParts->GetMainCamera().GetCamPos();
					EffectSingleton::Instance()->SetLoop(Effect::ef_dust, this->m_EffectPos);
				}
				else {
					Easing(&this->m_EffectPos, CameraParts->GetMainCamera().GetCamPos(), 0.95f, EasingType::OutExpo);
					EffectSingleton::Instance()->Update_LoopEffect(Effect::ef_dust, this->m_EffectPos, Vector3DX::forward(), 0.5f);
					EffectSingleton::Instance()->SetEffectColor(Effect::ef_dust, 255, 255, 255, 64);
				}
			}
			else {
				if (EffectSingleton::Instance()->IsPlayLoopEffect(Effect::ef_dust)) {
					EffectSingleton::Instance()->StopEffect(Effect::ef_dust);
				}
			}
			//背景
			BackGround::BackGroundControl::Instance()->Update();
			//UIパラメーター
			{
				//timer
				this->m_UIclass.SetfloatParam(0, 0.0f);
				this->m_UIclass.SetfloatParam(1, this->m_StartTimer);

				this->m_UIclass.Update();
			}
			HitMarkerPool::Instance()->Update();
			EffectSingleton::Instance()->Update();
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
				PostPassParts->SetLevelFilter(0, 255, 1.0f);
				PostPassParts->SetAberrationPower(1.0f);
				PostPassParts->Set_is_Blackout(false);
				PostPassParts->Set_Per_Blackout(0.0f);
				PostPassParts->Set_is_lens(false);
				PostPassParts->Set_zoom_lens(1.0f);
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
			Objects::AmmoPool::Release();
			Objects::AmmoLinePool::Release();
			Objects::ItemObjPool::Release();
			BackGround::BackGroundControl::Release();
			CommonBattleResource::Dispose();
			this->m_UIclass.Dispose();
			Player::PlayerManager::Release();
			ObjectManager::Instance()->DeleteAll();
			this->m_PauseMenuControl.Dispose();
			HitMarkerPool::Release();
			Charas::GunAnimManager::Release();
			m_MainRagDoll.Dispose();
			m_RagDoll.Dispose();
		}
		//
		void			MainGameScene::MainDraw_Sub(int Range) const noexcept {
			BackGround::BackGroundControl::Instance()->Draw();
			ObjectManager::Instance()->Draw(true, Range);
			//ObjectManager::Instance()->Draw_Depth();
			HitMarkerPool::Instance()->Check();

			auto* PlayerMngr = Player::PlayerManager::Instance();
			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				PlayerMngr->GetPlayer(loop)->GetAI()->Draw();
			}
			Objects::AmmoLinePool::Instance()->Draw();
		}
		//UI表示
		void			MainGameScene::DrawUI_Base_Sub(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();
			if (ViewChara->IsAlive()) {
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
					if (this->m_NetWorkController->GetPing() >= 0.0f) {
						char Mes[260];
						sprintfDx(Mes, "Ping:%3dms", static_cast<int>(this->m_NetWorkController->GetPing()));

						PingMes = Mes;
					}
					else {
						if (!this->m_NetWorkController->IsServer()) {
							PingMes = "Lost Connection";
						}
						else {
							PingMes = "Ping:---ms";
						}
					}
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
						FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, (1920), (64), White, Black,
						PingMes);
				}
			}
			FadeControl::Instance()->Draw();
		}
	}
}
