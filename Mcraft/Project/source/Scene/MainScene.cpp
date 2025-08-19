#pragma warning(disable:4464)
#include	"../MainScene/NetworkBrowser.hpp"
#include	"MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		constexpr float FarMax = std::min(std::min(BackGround::DrawMaxXPlus, BackGround::DrawMaxZPlus), BackGround::DrawMaxYPlus) * BackGround::CellScale;

		void			MainGameScene::Load_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			BackGround::BackGroundControl::Create();
			CommonBattleResource::Load();
			HitMarkerPool::Create();
			Player::PlayerManager::Create();

			this->m_PauseMenuControl = std::make_unique<PauseMenuControl>();
			this->m_UIclass = std::make_unique<MainSceneUI>();
			this->m_UIresult = std::make_unique<MainSceneResultUI>();
			this->m_TransceiverUI = std::make_unique<TransceiverUI>();
			this->m_TaskOperator = std::make_unique<TaskOperator>();
			this->m_StartMovie = std::make_unique<StartMovie >();
			//
			for (const auto& Path : Objects::ItemObjDataManager::Instance()->GetPathList()) {
				ObjMngr->LoadModelBefore(Path);
			}
			for (const auto& Path : Guns::GunPartsDataManager::Instance()->GetGunPathList()) {
				ObjMngr->LoadModelBefore(Path);
			}
			for (const auto& Path : Guns::GunPartsDataManager::Instance()->GetModPathList()) {
				ObjMngr->LoadModelBefore(Path);
			}
			for (const auto& Path : Objects::AmmoDataManager::Instance()->GetPathList()) {
				ObjMngr->LoadModelBefore(Path);
			}
			//
			{
				std::string Path = "data/Charactor/";
				std::vector<WIN32_FIND_DATA> pData;
				GetFileNamesInDirectory((Path + "*").c_str(), &pData);
				for (auto& data : pData) {
					std::string ChildPath = Path;
					ChildPath += data.cFileName;
					ChildPath += "/";
					ObjMngr->LoadModelBefore(ChildPath);
				}
			}
			//
			ObjMngr->LoadModelBefore("data/model/hindD/");
			ObjMngr->LoadModelBefore("data/model/UH60/");
			ObjMngr->LoadModelBefore("data/model/BMP3/");

			ObjMngr->LoadModelBefore("data/model/PlateCarrler/");
			ObjMngr->LoadModelBefore("data/model/Helmet/");
			ObjMngr->LoadModelBefore("data/model/container/");
			ObjMngr->LoadModelBefore("data/model/circle/");
		}
		void			MainGameScene::LoadEnd_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			Objects::AmmoPool::Create();
			Objects::AmmoLinePool::Create();
			Objects::ItemObjPool::Create();

			//スキルをセーブから読み取り
			Player::SkillList::Create();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			PlayerMngr->Init(NetWork::Player_num);
			PlayerMngr->SetWatchPlayerID(GetViewPlayerID());

			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
				//
				if (loop == PlayerMngr->GetWatchPlayerID()) {
					Charas::CharacterObj::LoadChara("Main", (PlayerID)loop);
					//GUN
					for (const auto& guns : Guns::GunPartsDataManager::Instance()->GetGunNameList()) {
						int ID = static_cast<int>(SaveData::Instance()->GetParam(guns));
						if (ID >= 1) {
							chara->LoadCharaGun(guns, ID - 1);
						}
					}
					//ラグドール
					chara->SetupRagDoll(PlayerMngr->GetTeamRagDoll());
				}
				else {
					Charas::CharacterObj::LoadChara("Soldier", (PlayerID)loop);
					//GUN
					chara->LoadCharaGun("AK12E", 0);
					//chara->LoadCharaGun("MP443", 1);
					//chara->LoadCharaGun("RGD5", 2);
					//ラグドール
					chara->SetupRagDoll(PlayerMngr->GetEnemyRagDoll());
				}
				//
				chara->SetPlayerID((PlayerID)loop);
				chara->SetCharaTypeID((loop == PlayerMngr->GetWatchPlayerID()) ? CharaTypeID::Team : CharaTypeID::Enemy);
				//
				for (int loop2 = 0; loop2 < 3; ++loop2) {
					auto& gun = chara->GetGunPtr(loop2);
					if (!gun) { continue; }
					gun->SetupGun();

					if (loop == PlayerMngr->GetWatchPlayerID()) {
						auto& mod = gun->GetModifySlot();
						std::string PresetPath = "Save/";
						PresetPath += mod->GetMyData()->GetName();
						PresetPath += ".dat";
						if (IsFileExist(PresetPath.c_str())) {
							gun->GetGunsModify()->LoadSlots(PresetPath.c_str());
							gun->SetupGun();
						}
					}
				}
			}
			if (!this->m_IsTutorial) {
				PlayerMngr->SetHelicopter(std::make_shared<Objects::HelicopterObj>());
				ObjMngr->InitObject(PlayerMngr->GetHelicopter(), "data/model/hindD/");
			}

			PlayerMngr->SetTeamHelicopter(std::make_shared<Objects::TeamHelicopterObj>());
			ObjMngr->InitObject(PlayerMngr->GetTeamHelicopter(), "data/model/UH60/");
			PlayerMngr->GetTeamHelicopter()->SetIsIntercept(!this->m_IsTutorial);

			//PlayerMngr->SetVehicle(std::make_shared<Objects::VehicleObj>());
			//ObjMngr->InitObject(PlayerMngr->GetVehicle(), "data/model/BMP3/");

			PlayerMngr->SetArmor(std::make_shared<Objects::ArmorObj>());
			ObjMngr->InitObject(PlayerMngr->GetArmor(), "data/model/PlateCarrler/");

			PlayerMngr->SetHelmet(std::make_shared<Objects::ArmorObj>());
			ObjMngr->InitObject(PlayerMngr->GetHelmet(), "data/model/Helmet/");

			//ItemContainerObj
			PlayerMngr->SetItemContainerObj(std::make_shared<Objects::ItemContainerObj>());
			ObjMngr->InitObject(PlayerMngr->GetItemContainerObj(), "data/model/container/");

			this->m_StartMovie->Set();
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* OptionParts = OptionManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			auto* SE = SoundPool::Instance();
			//
			CommonBattleResource::Set();
			NetWorkBrowser::Create();

			PostPassParts->SetShadowScale(0.5f);
			//
			BackGroundParts->Init(this->m_IsTutorial);
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
				if (this->m_IsTutorial) {
					if (loop == PlayerMngr->GetWatchPlayerID()) {
						TargetPos = Vector3DX::vget(0.f, 0.f, 16.f) * Scale3DRate;
						Vector3DX EndPos = TargetPos - Vector3DX::up() * 40.0f * Scale3DRate;
						if (BackGroundParts->CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
							TargetPos = EndPos;
						}
					}
					else if (loop == 1) {
						TargetPos = Vector3DX::vget(0.f, 0.f, 10.f) * Scale3DRate;
						Vector3DX EndPos = TargetPos - Vector3DX::up() * 40.0f * Scale3DRate;
						if (BackGroundParts->CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
							TargetPos = EndPos;
						}
					}
					else {
						TargetPos = Vector3DX::vget((loop - 2) * -0.5f, 0.f, -8.f + -(loop-2) * 2.f) * Scale3DRate;
						Vector3DX EndPos = TargetPos - Vector3DX::up() * 40.0f * Scale3DRate;
						if (BackGroundParts->CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
							TargetPos = EndPos;
						}
					}
					//人の座標設定
					if (loop == PlayerMngr->GetWatchPlayerID()) {
						float RunTime = 0.f;
						float value = Player::SkillList::Instance()->GetSkillValueNow(Player::SkillType::Runner);
						if (value > 0.f) {
							RunTime = value;
						}
						chara->Spawn(deg2rad(0.0f), deg2rad(0), TargetPos, 0, true, RunTime, false);
					}
					else {
						chara->Spawn(deg2rad(0.0f), deg2rad(180), TargetPos, 0, true, 0.f);
					}
				}
				else {
					if (loop == PlayerMngr->GetWatchPlayerID()) {
						while (true) {
							TargetPos = BackGroundParts->GetBuildData().at(static_cast<size_t>(GetRand(static_cast<int>(BackGroundParts->GetBuildData().size()) - 1))).GetPos();
							Vector3DX Vec = TargetPos; Vec.y = 0.f;
							if (Vec.sqrMagnitude() <= (5.f * Scale3DRate) * (5.f * Scale3DRate)) {
								Vector3DX EndPos = TargetPos - Vector3DX::up() * 50.0f * Scale3DRate;
								if (BackGroundParts->CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
									TargetPos = EndPos;
								}
								break;
							}
						}
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
						float RunTime = 0.f;
						float value = Player::SkillList::Instance()->GetSkillValueNow(Player::SkillType::Runner);
						if (value > 0.f) {
							RunTime = value;
						}
						chara->Spawn(deg2rad(0.0f), deg2rad(GetRand(360)), TargetPos, 0, true, RunTime);
					}
					else {
						chara->Spawn(deg2rad(0.0f), deg2rad(GetRand(360)), TargetPos, 0, true, 0.f);
					}
				}
			}

			if (this->m_IsTutorial) {
				for (int loop = 0; loop < Objects::ItemObjDataManager::Instance()->GetList().size(); ++loop) {
					Vector3DX TargetPos;
					TargetPos = Vector3DX::vget(1.5f, 0.f, -8.f-loop * 1.f) * Scale3DRate;
					{
						Vector3DX EndPos = TargetPos - Vector3DX::up() * 40.0f * Scale3DRate;
						if (BackGroundParts->CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
							TargetPos = EndPos;
						}
					}
					Objects::ItemObjPool::Instance()->Put(loop,
						TargetPos,
						Vector3DX::vget(GetRandf(1.f), 1.f, GetRandf(1.f)) * Scale3DRate * 0.01f
					);
				}
			}
			else {
				for (int loop = 0; loop < 20; ++loop) {
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
			}
			this->m_TaskOperator->Set();
			if (this->m_IsTutorial) {
				this->m_TaskOperator->AddKillEnemy(1);
				this->m_TaskOperator->AddKillEnemy(3);
				for (int loop = 0; loop < Objects::ItemObjDataManager::Instance()->GetList().size(); ++loop) {
					if (Objects::ItemObjDataManager::Instance()->Get(loop)->EnableSpawnBySoldier()) {
						this->m_TaskOperator->AddObtain(loop, 1);
						break;
					}
				}
			}
			else{
				this->m_TaskOperator->AddKillEnemy(2 + GetRand(2));
				for (int loop = 0; loop < Objects::ItemObjDataManager::Instance()->GetList().size(); ++loop) {
					this->m_TaskOperator->AddObtain(loop, 1 + GetRand(2));
				}
				this->m_TaskOperator->Shuffle();
			}
			{
				Vector3DX TargetPos = Vector3DX::zero();
				if (this->m_IsTutorial) {
					TargetPos = Vector3DX::vget(0.f, 0.f, -16.f) * Scale3DRate;
				}
				else {
					TargetPos = Matrix3x3DX::Vtrans(Vector3DX::vget(0.f, 0.f, 16.f * Scale3DRate), Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180))));
				}
				{
					Vector3DX EndPos = TargetPos - Vector3DX::up() * 50.0f * Scale3DRate;
					if (BackGroundParts->CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
						TargetPos = EndPos- Vector3DX::up() * 0.5f * Scale3DRate;;
					}
				}

				//周りの壁を破壊する
				//壁破壊
				{
					int								xput = 25;
					int								yput = 20;
					int								zput = 25;
					int8_t							damage = 100;
					auto Put = BackGroundParts->GetPoint(TargetPos);
					for (int xp = -xput / 2; xp <= xput / 2; ++xp) {
						for (int zp = -zput / 2; zp <= zput / 2; ++zp) {
							for (int yp = -yput / 2; yp <= yput / 2; ++yp) {
								BackGroundParts->DamageCell(Put.x + xp, Put.y + yp, Put.z + zp, damage);
							}
							for (int yp = -yput / 2; yp <= 0; ++yp) {
								//BackGroundParts->SetBlick(Put.x + xp, Put.y + yp, Put.z + zp, 1);
							}
						}

					}
				}

				{
					Vector3DX EndPos = TargetPos - Vector3DX::up() * 50.0f * Scale3DRate;
					if (BackGroundParts->CheckLinetoMap(TargetPos + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
						TargetPos = EndPos - Vector3DX::up() * 0.5f * Scale3DRate;;
					}
				}

				PlayerMngr->GetItemContainerObj()->Put(TargetPos, Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180))));
				{
					auto obj = std::make_shared<Objects::CircleEffect>();
					ObjectManager::Instance()->InitObject(obj, "data/model/circle/");
					obj->Put(TargetPos);
				}
			}
			//UI
			this->m_UIclass->Set();
			this->m_UIresult->Set();
			this->m_TransceiverUI->Set();
			//
			this->m_PauseMenuControl->Init();
			FadeControl::Instance()->Init();
			this->m_IsEnd = false;
			this->m_StartTimer = 3.0f;
			this->m_BattleTimer = 180.f;
			this->m_ReturnPer = 0.f;
			this->m_IsFindContainer = false;
			this->m_FindContainerTimer = 0.f;
			this->m_AnnounceTimer = 0.f;
			this->m_IsGameOver = false;
			this->m_FadeoutEndTimer = 0.f;
			this->AberrationPower = 1.f;
			this->m_IsAddScoreArea = false;
			this->m_LimitAlarmCount = 0;
			this->m_LimitAlarmTimer = 0.f;
			this->m_TaskClearOnce = false;
			this->m_IsGameClear = false;

			this->m_IsStartMoviePlay = !this->m_IsTutorial;

			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Envi))->Play(DX_PLAYTYPE_LOOP, true);
			//Vector3DX posBuf = Matrix3x3DX::Vtrans(Vector3DX::forward() * (15.0f * Scale3DRate), Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180))));
			//posBuf.y = -25.0f * Scale3DRate;
			//PlayerMngr->GetVehicle()->Spawn(std::atan2f(posBuf.x, posBuf.z), posBuf);

			if (!this->m_IsTutorial) {
				if (SaveData::Instance()->GetParam("BuffNextRound") > 0) {
					PlayerMngr->GetPlayer(0)->AddScore(1000);
					SaveData::Instance()->SetParam("BuffNextRound", 0);
				}
			}
			if (this->m_IsTutorial) {
				this->m_TransceiverUI->Put(100, 180.f);
			}
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* SideLogParts = SideLog::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* KeyGuideParts = KeyGuide::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* NetBrowser = NetWorkBrowser::Instance();
			auto* OptionParts = OptionManager::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();

			PlayerMngr->SetWatchPlayerID(GetViewPlayerID());
			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
			auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();

			PostPassParts->SetLevelFilter(38, 154, 1.0f);

			this->m_PauseMenuControl->Update();
			if (this->m_PauseMenuControl->IsRetire()) {
				if (!this->m_IsEnd) {
					FadeControl::Instance()->SetBlackOut(true);
				}
				this->m_IsEnd = true;
			}
			if (this->m_IsEnd) {
				SceneParts->SetPauseEnable(false);
			}
			if (this->m_IsEnd && FadeControl::Instance()->IsAll()) {
				m_FadeoutEndTimer += DXLib_refParts->GetDeltaTime();
				if (this->m_FadeoutEndTimer > 2.f) {
					return false;
				}
			}
			else {
				m_FadeoutEndTimer = 0.f;
			}

			FadeControl::Instance()->Update();


			KeyGuideParts->ChangeGuide(
				[this]() {
					auto* SceneParts = SceneControl::Instance();
					auto* KeyGuideParts = KeyGuide::Instance();
					if (this->m_IsGameClear) {}
					else if (SceneParts->IsPause()) {
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
						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::WALK), LocalizePool::Instance()->Get(9903));

						//KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD), LocalizePool::Instance()->Get(9904));

						KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::THROW), LocalizePool::Instance()->Get(9905));

						//KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::ITEMDELETE1), "");
						//KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::ITEMDELETE2), "");
						//KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::ITEMDELETE3), "");
						//KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::ITEMDELETE4), "");
						//KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::ITEMDELETE5), "");
						//KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::ITEMDELETE), LocalizePool::Instance()->Get(9910));
					}
				});
			if (SceneParts->IsPause()) {
				Pad->SetMouseMoveEnable(false);
				BackGroundParts->SettingChange();
				if (!this->m_NetWorkController) {
					return true;
				}
			}

			this->m_TransceiverUI->Update();

			if (this->m_IsStartMoviePlay) {
				Pad->SetMouseMoveEnable(false);
				SceneParts->SetPauseEnable(true);
				//OPムービー制御
				if (!this->m_PauseMenuControl->IsRetire()) {
					if (GetIsFirstLoop()) {
						if (!this->m_IsTutorial) {
							this->m_TransceiverUI->Put(0);
						}
					}
					if (this->m_StartMovie->Update()) {
						this->m_IsStartMoviePlay = false;
					}
				}
				return true;
			}
			else {
				if (!m_IsGameClear) {
					Pad->SetMouseMoveEnable(true);
#if DEBUG_NET
					Pad->SetMouseMoveEnable(false);
#endif
				}
				else {
					Pad->SetMouseMoveEnable(false);
				}
			}

			if (this->m_IsGameClear && !this->m_PauseMenuControl->IsRetire()) {
				SceneParts->SetPauseEnable(false);
				int EndCode = this->m_UIresult->Update();
				if (EndCode != InvalidID) {
					if (!this->m_IsEnd) {
						this->m_IsEnd = true;

						FadeControl::Instance()->SetBlackOut(true);
						//セーブデータにIDを追加
						int ID = this->m_UIresult->GetSkillSelect();

						if (!this->m_IsTutorial) {
							if (ID == static_cast<int>(Player::SkillType::ADDSCORE)) {
								SaveData::Instance()->SetParam("BuffNextRound", 1);
							}
							else {
								if (SaveData::Instance()->GetParam("skill" + std::to_string(ID)) > 0) {
									SaveData::Instance()->SetParam("skill" + std::to_string(ID), SaveData::Instance()->GetParam("skill" + std::to_string(ID)) + 1);
								}
								else {
									SaveData::Instance()->SetParam("skill" + std::to_string(ID), 1);
								}
							}
							if (SaveData::Instance()->GetParam("score") > 0) {
								SaveData::Instance()->SetParam("score", SaveData::Instance()->GetParam("score") + ViewPlayer->GetScore());
							}
							else {
								SaveData::Instance()->SetParam("score", ViewPlayer->GetScore());
							}
							SaveData::Instance()->SetParam("HighScore", std::max(SaveData::Instance()->GetParam("HighScore"), SaveData::Instance()->GetParam("score")));
							if (SaveData::Instance()->GetParam("round") > 0) {
								SaveData::Instance()->SetParam("round", SaveData::Instance()->GetParam("round") + 1);
							}
							else {
								SaveData::Instance()->SetParam("round", 1);
							}
						}
						else {
							if (SaveData::Instance()->GetParam("round") <= 0) {
								if (ID == static_cast<int>(Player::SkillType::ADDSCORE)) {
									SaveData::Instance()->SetParam("BuffNextRound", 1);
								}
								else {
									if (SaveData::Instance()->GetParam("skill" + std::to_string(ID)) > 0) {
										SaveData::Instance()->SetParam("skill" + std::to_string(ID), SaveData::Instance()->GetParam("skill" + std::to_string(ID)) + 1);
									}
									else {
										SaveData::Instance()->SetParam("skill" + std::to_string(ID), 1);
									}
								}
							}
						}
						if (EndCode == 1) {
							SetNextSelect(0);
							SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_CANCEL))->Play(DX_PLAYTYPE_BACK, true);
						}
						else {
							SetNextSelect(1);
							SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, true);
						}
					}
				}
				float FadeTimer = this->m_UIresult->GetTimer();
				if (FadeTimer >= 2.f) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heli))->SetLocalVolume(static_cast<int>(Lerp(255, 0, std::clamp(FadeTimer / 2.f, 0.f, 1.f))));
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heli2))->SetLocalVolume(static_cast<int>(Lerp(255, 0, std::clamp(FadeTimer / 2.f, 0.f, 1.f))));
				}

				return true;
			}

			//FirstDoingv
			if (GetIsFirstLoop()) {
				m_IsFindContainer = false;
			}
			//Input,AI
			{
				if (!GetIsFirstLoop()) {
					bool Prev = IsStartedBattle();
					this->m_StartTimer = std::max(this->m_StartTimer - DXLib_refParts->GetDeltaTime(), 0.0f);
					if (IsStartedBattle()) {
						if (!Prev) {
							KeyGuideParts->SetGuideFlip();
							SceneParts->SetPauseEnable(true);
							if (this->m_IsTutorial) {
								if (this->m_TransceiverUI->GetID() == 100) {
									this->m_TransceiverUI->Put(101, 180.f);
								}
							}
						}
						float prevBattleTimer = this->m_BattleTimer;
						this->m_BattleTimer = std::max(this->m_BattleTimer - DXLib_refParts->GetDeltaTime(), 0.0f);

						bool PrevFindContainer = m_IsFindContainer;
						if (!m_IsFindContainer) {
							if (PlayerMngr->GetItemContainerObj()) {
								Vector3DX StartPos = PlayerMngr->GetItemContainerObj()->GetMove().GetPos() + Vector3DX::up() * 1.0f * Scale3DRate;
								Vector3DX EndPos = ViewChara->GetEyePositionCache();

								StartPos = StartPos - ((StartPos - EndPos).normalized()*(3.f*Scale3DRate));
								if (BackGroundParts->CheckLinetoMap(StartPos, &EndPos) == 0) {
									m_IsFindContainer = true;
								}
							}
						}
						if (!this->m_IsEnd) {
							if (this->m_IsFindContainer) {
								m_FindContainerTimer += DXLib_refParts->GetDeltaTime();
								if (!PrevFindContainer) {
									if (static_cast<int>(this->m_BattleTimer) > 60) {
										SideLogParts->Add(5.0f, 0.0f, Red, LocalizeParts->Get(4001));
									}
									else if (static_cast<int>(this->m_BattleTimer) < 60) {
										SideLogParts->Add(5.0f, 0.0f, Red, LocalizeParts->Get(4002));
									}
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::announce))->Play(DX_PLAYTYPE_BACK, true);
									m_AnnounceTimer = 1.f;
									if (!this->m_IsTutorial) {
										this->m_TransceiverUI->Put(2);
									}
								}
								else {
									if ((static_cast<int>(this->m_BattleTimer) != static_cast<int>(prevBattleTimer))) {
										if (static_cast<int>(this->m_BattleTimer) == 60) {
											SideLogParts->Add(5.0f, 0.0f, Red, LocalizeParts->Get(4002));
											SE->Get(SoundType::SE, static_cast<int>(SoundEnum::announce))->Play(DX_PLAYTYPE_BACK, true);
											m_AnnounceTimer = 1.f;
											if (!this->m_IsTutorial) {
												this->m_TransceiverUI->Put(3);
											}
										}
									}
								}
							}
							else {
								m_FindContainerTimer = 0.f;
								if ((static_cast<int>(this->m_BattleTimer) != static_cast<int>(prevBattleTimer))) {
									if (static_cast<int>(this->m_BattleTimer) == 179) {
										SideLogParts->Add(5.0f, 0.0f, Red, LocalizeParts->Get(4000));
										SE->Get(SoundType::SE, static_cast<int>(SoundEnum::announce))->Play(DX_PLAYTYPE_BACK, true);
										m_AnnounceTimer = 1.f;
										if (!this->m_IsTutorial) {
											this->m_TransceiverUI->Put(1);
										}
									}
								}
							}
							if ((static_cast<int>(this->m_BattleTimer) != static_cast<int>(prevBattleTimer))) {
								if (static_cast<int>(this->m_BattleTimer) == 174) {
									this->m_TaskOperator->StartNextTask();
								}
							}
							if ((static_cast<int>(this->m_BattleTimer) != static_cast<int>(prevBattleTimer))) {
								if (static_cast<int>(this->m_BattleTimer) == 30) {
									if (!this->m_IsTutorial) {
										this->m_TransceiverUI->Put(5);
									}
								}
							}
						}
						if (!m_IsGameClear) {
							if (this->m_BattleTimer < 4.f * 5.f + 10.f) {
								this->m_LimitAlarmTimer += DXLib_refParts->GetDeltaTime();
								//
								int Count = 0;
								if (this->m_BattleTimer < 4.f * 4.f + 5.f) {
									Count = 1;
								}
								if (this->m_BattleTimer < 4.f * 3.f + 5.f) {
									Count = 2;
								}
								if (this->m_BattleTimer < 4.f * 2.f + 5.f) {
									Count = 3;
								}
								if (this->m_BattleTimer < 4.f * 1.f + 5.f) {
									Count = 4;
								}

								if (this->m_LimitAlarmTimer > 0.125f) {
									this->m_LimitAlarmTimer -= 0.125f;
									bool value = false;
									switch (Count) {
									case 0:
										value = ((((this->m_LimitAlarmCount) % 4) == 0)) && ((this->m_LimitAlarmCount) % 8 < 4);
										break;
									case 1:
										value = ((((this->m_LimitAlarmCount) % 4) == 0) || (((this->m_LimitAlarmCount) % 4) == 2)) && ((this->m_LimitAlarmCount) % 8 < 4);
										break;
									case 2:
										value = ((((this->m_LimitAlarmCount) % 4) == 0) || (((this->m_LimitAlarmCount) % 4) == 1) || (((this->m_LimitAlarmCount) % 4) == 2)) && ((this->m_LimitAlarmCount) % 8 < 4);
										break;
									case 3:
										value = true && ((this->m_LimitAlarmCount) % 8 < 4);
										break;
									default:
										value = true;
										break;
									}
									if (value) {
										SE->Get(SoundType::SE, static_cast<int>(SoundEnum::alarm))->Play(DX_PLAYTYPE_BACK, true);
									}
									++this->m_LimitAlarmCount;
								}
							}
							else {
								this->m_LimitAlarmCount = 0;
								this->m_LimitAlarmTimer = 0.f;
							}
						}
					}
				}
				MyInput.ResetAllInput();
				if (!SceneParts->IsPause() && FadeControl::Instance()->IsClear() && IsStartedBattle()) {
					float AimPer = 1.0f / std::max(1.0f, ViewChara->GetIsADS() ? ViewChara->GetGunPtrNow()->GetSightZoomSize() : 1.0f);
					MyInput.SetAddxRad(Pad->GetLS_Y() / 200.0f * AimPer);
					MyInput.SetAddyRad(Pad->GetLS_X() / 200.0f * AimPer);
					MyInput.SetInputPADS(Controls::PADS::MOVE_W, Pad->GetPadsInfo(Controls::PADS::MOVE_W).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::MOVE_S, Pad->GetPadsInfo(Controls::PADS::MOVE_S).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::MOVE_A, Pad->GetPadsInfo(Controls::PADS::MOVE_A).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::MOVE_D, Pad->GetPadsInfo(Controls::PADS::MOVE_D).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::RUN, Pad->GetPadsInfo(Controls::PADS::RUN).GetKey().press());
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

					MyInput.SetInputPADS(Controls::PADS::HEALARMOR, Pad->GetPadsInfo(Controls::PADS::HEALARMOR).GetKey().press());

					bool IsPressArmor = false;
					if (
						(ViewChara->GetBodyAP().GetPoint() != ViewChara->GetBodyAP().GetMax()) ||
						(ViewChara->GetHeadAP().GetPoint() != ViewChara->GetHeadAP().GetMax())
						) {
						IsPressArmor = MyInput.GetPADSPress(Controls::PADS::HEALARMOR);
					}

					m_IsAddScoreArea = false;
					if (PlayerMngr->GetItemContainerObj()) {
						auto Vec = ViewChara->GetMove().GetPos() - PlayerMngr->GetItemContainerObj()->GetMove().GetPos(); Vec.y = 0.f;
						if (Vec.magnitude() < 3.f * Scale3DRate) {
							//捨てると納品できるエリア
							m_IsAddScoreArea = true;
						}
					}


					int loop = 0;
					bool isFirst = true;
					bool isArmerHealing = true;
					bool isPressed = false;
					int TaskItemID = InvalidID;
					if (this->m_TaskOperator->IsActiveTask(TaskType::Obtain)) {
						for (auto& ID : ViewPlayer->GetInventory()) {
							if (!ID.HasItem()) { continue; }
							if (this->m_TaskOperator->GetNowTaskItemID() == ID.m_ItemID) {
								TaskItemID = ID.m_ItemID;
								break;
							}
						}
					}
					for (auto& ID : ViewPlayer->SetInventory()) {
						ID.m_timer += DXLib_refParts->GetDeltaTime();
						if (ID.HasItem()) {
							bool IsPress = false;
							if (
								isFirst && (
									(TaskItemID == InvalidID) ||								//タスクがないときか
									(this->m_IsAddScoreArea && TaskItemID == ID.m_ItemID) ||	//タスクがあってエリア中では納品アイテムから先に捨てる
									!(this->m_IsAddScoreArea || TaskItemID == ID.m_ItemID)		//タスクがあってエリア外では納品アイテム以外から先に捨てる
									)
								) {
								isFirst = false;
								IsPress |= Pad->GetPadsInfo(Controls::PADS::ITEMDELETE).GetKey().press();
							}
							if (isArmerHealing && IsPressArmor) {
								switch (ID.GetItem()->GetItemType()) {
								case Objects::ItemType::Helmet:
									if (ViewChara->GetHeadAP().GetPoint() != ViewChara->GetHeadAP().GetMax()) {
										IsPress |= true;
										isArmerHealing = false;
									}
									break;
								case Objects::ItemType::Armor:
									if (ViewChara->GetBodyAP().GetPoint() != ViewChara->GetBodyAP().GetMax()) {
										IsPress |= true;
										isArmerHealing = false;
									}
									break;
								default:
									break;
								}
							}
							switch (loop) {
							case 0: IsPress |= Pad->GetPadsInfo(Controls::PADS::ITEMDELETE1).GetKey().press(); break;
							case 1: IsPress |= Pad->GetPadsInfo(Controls::PADS::ITEMDELETE2).GetKey().press(); break;
							case 2: IsPress |= Pad->GetPadsInfo(Controls::PADS::ITEMDELETE3).GetKey().press(); break;
							case 3: IsPress |= Pad->GetPadsInfo(Controls::PADS::ITEMDELETE4).GetKey().press(); break;
							case 4: IsPress |= Pad->GetPadsInfo(Controls::PADS::ITEMDELETE5).GetKey().press(); break;
							default:
								break;
							}
							//重複防止
							if (IsPress) {
								if (isPressed) {
									IsPress = false;
								}
								isPressed = true;
							}

							if (IsPress) {
								if (ID.m_timer > 0.5f) {
									ID.m_timer = -0.5f;
									//アーマーヒール
									if (IsPressArmor) {
										switch (ID.GetItem()->GetItemType()) {
										case Objects::ItemType::Helmet:
											ViewChara->HealHelmet();
											break;
										case Objects::ItemType::Armor:
											ViewChara->HealArmor();
											if (this->m_IsTutorial) {
												if (this->m_TransceiverUI->GetID() == 102) {
													this->m_TransceiverUI->Put(103, 180.f);
												}
											}
											break;
										default:
											break;
										}
									}
									else {
										if (this->m_IsAddScoreArea) {
											auto Score = ID.GetItem()->GetScore();
											//納品
											PlayerMngr->GetPlayer(0)->AddScore(Score);
											SideLogParts->Add(5.0f, 0.0f, Green, ((std::string)(LocalizeParts->Get(205)) + " +" + std::to_string(Score)).c_str());
											SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Delivery))->Play(DX_PLAYTYPE_BACK, true);
											//タスク

											if (this->m_TaskOperator->CheckItem(ID.m_ItemID)) {
												this->m_TaskClearOnce = true;
												if (!this->m_IsTutorial) {
													this->m_TransceiverUI->Put(11);
												}
												if (this->m_IsTutorial) {
													if (this->m_TransceiverUI->GetID() == 105) {
														this->m_TransceiverUI->Put(106, 180.f);
													}
												}
											}
											else {
												if (this->m_IsTutorial) {
													if (this->m_TransceiverUI->GetID() == 106) {
														this->m_TransceiverUI->Put(107, 180.f);
													}
												}
											}
										}
										else {
											Vector3DX Vec = ViewChara->GetEyeRotationCache().zvec() * -1.f;
											Vec.y = std::clamp(Vec.y, 0.1f, 0.3f); Vec = Vec.normalized();
											Objects::ItemObjPool::Instance()->Put(ID.m_ItemID,
												ViewChara->GetFrameWorldMat(Charas::CharaFrame::Upper2).pos(),
												Vec * (10.f * Scale3DRate)
											);
										}
									}
									ViewPlayer->SubInventoryIndex(loop);
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::PutItem))->Play(DX_PLAYTYPE_BACK, true);
								}
							}
							else {
								if (ID.m_timer >= 0.f) {
									ID.m_timer = 0.f;
								}
							}
						}
						else {
							if (ID.m_timer >= 0.f) {
								ID.m_timer = 0.f;
							}
						}
						ID.UpdateAnim();
						++loop;
					}
				}
				{
					//タイムオーバー
					bool isEnd = false;
					if (this->m_BattleTimer <= 0.f && ViewChara->IsAlive()) {
						if (!this->m_IsAddScoreArea) {
							//デス
							ViewChara->SetDamage(ViewChara->GetMyPlayerID(), 1000, static_cast<int>(Charas::HitType::Head), ViewChara->GetMove().GetPos(), ViewChara->GetMove().GetPos());
						}
						else {
							//帰還する
							isEnd = true;
						}
					}
					//
					else if (this->m_ReturnPer >= 1.f) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::returnButton))->StopAll();
						//帰還する
						isEnd = true;
					}
					if (isEnd) {
						if (this->m_IsTutorial) {
							this->m_TransceiverUI->Put(108, 180.f);
						}
						m_IsGameClear = true;
						KeyGuideParts->SetGuideFlip();
						this->m_UIresult->Start(180.f - m_BattleTimer);
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::resultEnv))->Play(DX_PLAYTYPE_BACK, true);
						SE->Get(SoundType::BGM, 2)->Play(DX_PLAYTYPE_BACK, true);
						if (!this->m_IsTutorial) {
							this->m_TransceiverUI->Put(4);
						}
						//納品
						for (auto& ID : ViewPlayer->GetInventory()) {
							if (!ID.HasItem()) { continue; }
							auto Score = ID.GetItem()->GetScore();
							PlayerMngr->GetPlayer(0)->AddScore(Score);
							SideLogParts->Add(5.0f, 0.0f, Green, ((std::string)(LocalizeParts->Get(205)) + " +" + std::to_string(Score)).c_str());
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Delivery))->Play(DX_PLAYTYPE_BACK, true);
						}
					}
				}

				//ネットワーク
				if (NetBrowser->IsDataReady() && !this->m_NetWorkController) {
					this->m_NetWorkController = std::make_unique<NetWork::NetWorkController>(NetBrowser->IsServer(), NetBrowser->GetNetSetting().UsePort, NetBrowser->GetNetSetting().IP, NetBrowser->GetServerPlayer());
				}
				int PrevKill = PlayerMngr->GetWatchPlayer()->GetKill();
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
						if (PlayerMngr->GetTeamHelicopter()) {
							//PlayerMngr->GetTeamHelicopter()->GetDamageEvent()//TODO
							PlayerMngr->GetTeamHelicopter()->SetDamageEventReset();
						}
					}
					this->m_NetWorkController->Update(this->m_LocalSend);
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
							if (
								!SceneParts->IsPause() &&
								FadeControl::Instance()->IsClear() &&
								IsStartedBattle() &&
								!this->m_IsTutorial) {
								chara->Input(PlayerMngr->GetPlayer(loop)->GetAI()->Update());//AIに入力させる
							}
						}
						//このプレイヤーが出したダメージイベントをリストに追加
						chara->PopDamageEvent(&DamageEvents);
					}
					//PlayerMngr->GetVehicle()->PopDamageEvent(&DamageEvents);
					if (PlayerMngr->GetHelicopter()) {
						PlayerMngr->GetHelicopter()->PopDamageEvent(&DamageEvents);
					}
					PlayerMngr->GetTeamHelicopter()->PopDamageEvent(&DamageEvents);
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
				//
				if ((PrevKill != PlayerMngr->GetWatchPlayer()->GetKill()) && this->m_TaskOperator->CheckKill()) {
					this->m_TaskClearOnce = true;
					if (!this->m_IsTutorial) {
						this->m_TransceiverUI->Put(10);
					}
					if (this->m_IsTutorial) {
						if (this->m_TransceiverUI->GetID() == 101) {
							this->m_TransceiverUI->Put(102, 180.f);
						}
						else if (this->m_TransceiverUI->GetID() != InvalidID && this->m_TransceiverUI->GetID() <= 104) {
							this->m_TransceiverUI->Put(105, 180.f);
						}
					}
				}
				if (!ViewChara->IsAlive()) {
					if (!this->m_IsEnd) {
						FadeControl::Instance()->SetBlackOut(true, 3.f);
						if (!this->m_IsTutorial) {
							Player::ResultData::Instance()->SetData();
							for (int loop = 0; loop < static_cast<int>(Player::SkillType::Max); ++loop) {
								SaveData::Instance()->SetParam("skill" + std::to_string(loop), 0);
							}
							SaveData::Instance()->SetParam("score", 0);
							SaveData::Instance()->SetParam("round", 0);
						}
					}
					this->m_IsEnd = true;
				}
			}

			//PlayerMngr->GetVehicle()->SetInput(MyInput, true);

			PlayerMngr->m_FindCount = std::max(PlayerMngr->m_FindCount - DXLib_refParts->GetDeltaTime(), 0.f);
			//ほかプレイヤーとの判定
			{
				float EatLength = 0.f;
				if (Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().press()) {
					float value = Player::SkillList::Instance()->GetSkillValueNow(Player::SkillType::ItemEater);
					if (value > 0.f) {
						EatLength = value * Scale3DRate;
					}
				}

				float Radius = 2.0f * 1.f * Scale3DRate;
				if (this->m_NetWorkController && this->m_NetWorkController->IsInGame()) {//オンライン
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						auto& player = PlayerMngr->GetPlayer(loop);
						auto& chara = player->GetChara();
						if (!chara->IsAlive()) { continue; }
						if (chara->GetIsRappelling()) { continue; }
						//TODO
					}
				}
				else {
					if (ViewChara->IsAlive()) {
						//自分が当たったら押し出す 取れるなら取る
						auto Dir = ViewChara->GetEyeRotationCache().zvec() * -1.f;
						auto Dir_XZ = Dir; Dir_XZ.y = (0.f);

						//自分が当たったら押し出す
						for (auto& g : Objects::ItemObjPool::Instance()->GetList()) {
							if (!g->IsActive()) { continue; }
							Vector3DX Vec = (ViewChara->GetMove().GetPos() - g->GetMove().GetPos()); Vec.y = (0.0f);
							float Len = Vec.magnitude();
							if (Len < EatLength) {
								Vector3DX EndPos = g->GetMove().GetPos() + Vector3DX::up() * (1.f * Scale3DRate);
								if (
									(ViewPlayer->HasEmptyInventory() != 0) &&
									((Vector3DX::Dot(Dir_XZ.normalized(), Vec.normalized() * -1.f)) > 0.f) &&
									(BackGroundParts->CheckLinetoMap(ViewChara->GetEyePositionCache(), &EndPos) == 0)
									) {
									g->SetMove().SetPos(g->GetMove().GetPos() + Vec.normalized() * (1.f * Scale3DRate * DXLib_refParts->GetDeltaTime()));
									g->SetMove().Update(0.f, 0.f);
								}
							}
							if (Len < Radius) {
								Vector3DX EndPos = g->GetMove().GetPos() + Vector3DX::up() * (1.f * Scale3DRate);
								if (
									g->CanPick() &&
									(ViewPlayer->HasEmptyInventory() != 0) &&
									((Vector3DX::Dot(Dir_XZ.normalized(), Vec.normalized() * -1.f)) > 0.f) &&
									(BackGroundParts->CheckLinetoMap(ViewChara->GetEyePositionCache(), &EndPos) == 0)
									) {
									ViewPlayer->AddInventory(g->GetUniqueID());
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::GetItem))->Play(DX_PLAYTYPE_BACK, true);
									g->SetActive(false);

									float value = Player::SkillList::Instance()->GetSkillValueNow(Player::SkillType::Adrenaline);
									if (value > 0.f) {
										ViewChara->SetAdrenalineTime(value);
									}
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
			}
			//Update
			ObjMngr->UpdateObject();
			ObjMngr->LateUpdateObject();
			Objects::AmmoLinePool::Instance()->Update();
			//
			if (BackGroundParts->PopGrenadeBomb()) {
				if (this->m_IsTutorial) {
					if (this->m_TransceiverUI->GetID() != InvalidID && this->m_TransceiverUI->GetID() <= 103) {
						this->m_TransceiverUI->Put(104, 180.f);
					}
				}
			}
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
			BackGroundParts->Update();
			//UIパラメーター
			{
				bool CanReturn = ((ViewPlayer->GetScore() >= 1000) || (this->m_BattleTimer < 60.f) || this->m_TaskClearOnce);

				if (CanReturn && this->m_IsAddScoreArea &&  Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().press()) {
					if (Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger()) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::returnButton))->Play(DX_PLAYTYPE_BACK, true);
					}
					float SpeedUP = 1.f;
					float value = Player::SkillList::Instance()->GetSkillValueNow(Player::SkillType::TeiziTaisha);
					if (value > 0.f) {
						SpeedUP += value / 100.f;
					}
					this->m_ReturnPer = std::clamp(this->m_ReturnPer + DXLib_refParts->GetDeltaTime() / 5.f* SpeedUP, 0.f, 1.f);
				}
				else {
					if (this->m_ReturnPer != 0.f) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::returnButton))->StopAll();
					}
					this->m_ReturnPer = 0.f;
				}
				this->m_AnnounceTimer = std::clamp(this->m_AnnounceTimer - DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				//タイマー
				this->m_UIclass->SetfloatParam(0, this->m_BattleTimer);
				this->m_UIclass->SetfloatParam(1, this->m_StartTimer);
				this->m_UIclass->SetfloatParam(2, this->m_ReturnPer);
				this->m_UIclass->SetfloatParam(3, this->m_AnnounceTimer);
				this->m_UIclass->SetIntParam(0, this->m_IsAddScoreArea);
				{
					int select = 1;
					if (this->m_BattleTimer < 60.f) {
						select = 2;
					}
					if (!this->m_IsFindContainer) {
						select = 0;
					}
					this->m_UIclass->SetIntParam(1, select);
				}
				this->m_UIclass->SetIntParam(2, CanReturn);
				this->m_UIclass->Update();
			}
			HitMarkerPool::Instance()->Update();
			EffectSingleton::Instance()->Update();
			return true;
		}
		void			MainGameScene::Dispose_Sub(void) noexcept {
			if (!(this->m_IsGameClear && !this->m_PauseMenuControl->IsRetire())) {
				SetNextSelect(0);
			}
			auto* SceneParts = SceneControl::Instance();
			auto* SE = SoundPool::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			SceneParts->SetPauseEnable(true);
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Envi))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::resultEnv))->StopAll();
			SE->Get(SoundType::BGM, 2)->StopAll();
			EffectSingleton::Instance()->StopEffect(Effect::ef_dust);
			//使い回しオブジェ系
			BackGround::BackGroundControl::Instance()->Dispose();
			//
			if (this->m_NetWorkController) {
				this->m_NetWorkController.reset();
			}
			{
				PostPassParts->SetLevelFilter(0, 255, 1.0f);
				PostPassParts->SetAberrationPower(1.0f);
				PostPassParts->Set_is_Blackout(false);
				PostPassParts->Set_Per_Blackout(0.0f);
				PostPassParts->Set_is_lens(false);
				PostPassParts->Set_zoom_lens(1.0f);
			}
			NetWorkBrowser::Release();
		}
		void			MainGameScene::Dispose_Load_Sub(void) noexcept {
			this->m_StartMovie.reset();
			this->m_PauseMenuControl.reset();
			this->m_UIclass.reset();
			this->m_UIresult.reset();
			this->m_TransceiverUI.reset();
			this->m_TaskOperator.reset();
			Objects::AmmoPool::Release();
			Objects::AmmoLinePool::Release();
			Objects::ItemObjPool::Release();
			BackGround::BackGroundControl::Release();
			CommonBattleResource::Dispose();
			Player::PlayerManager::Release();
			Player::SkillList::Release();
			ObjectManager::Instance()->DeleteAll();
			HitMarkerPool::Release();
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

			if (this->m_IsStartMoviePlay) {
				this->m_TransceiverUI->Draw();
			}
			else {
				if (ViewChara->IsAlive() && !m_IsGameClear) {
					//レティクル表示
					if (ViewChara->GetGunPtrNow()) {
						ViewChara->GetGunPtrNow()->DrawReticle(ViewChara->GetLeanRad());
					}
					HitMarkerPool::Instance()->Draw();
					if (!SceneParts->IsPause()) {
						//コンテナ表示
						if (this->m_IsFindContainer) {
							if (PlayerMngr->GetItemContainerObj()) {
								if (PlayerMngr->GetItemContainerObj()->IsDrawUI()) {
									auto DispPos = PlayerMngr->GetItemContainerObj()->GetDispPos();

									float Per = std::clamp(this->m_FindContainerTimer / 0.5f, 0.f, 1.f);

									float LocalPer = 0.f;
									if (Per < 0.8f) {
										LocalPer = Lerp(0.f, 1.2f, (Per - 0.f) / (0.8f - 0.f));
									}
									else {
										LocalPer = Lerp(1.2f, 1.f, (Per - 0.8f) / (1.f - 0.8f));
									}
									bool IsDraw = true;
									if (Per >= 1.f) {
										if ((this->m_FindContainerTimer / 2.f - static_cast<int>(this->m_FindContainerTimer / 2.f)) > 0.5f) {
											IsDraw = false;
										}
									}
									if (IsDraw) {
										int xp1 = static_cast<int>(DispPos.XScreenPos() - 10.f * LocalPer);
										int yp1 = static_cast<int>(DispPos.YScreenPos() - 20.f * LocalPer);
										int xp2 = static_cast<int>(DispPos.XScreenPos());
										int yp2 = static_cast<int>(DispPos.YScreenPos());
										int xp3 = static_cast<int>(DispPos.XScreenPos() + 10.f * LocalPer);
										int yp3 = static_cast<int>(DispPos.YScreenPos() - 20.f * LocalPer);
										if (Per >= 0.8f) {
											DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
												18, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::BOTTOM,
												(xp1 + xp3) / 2, (yp1 + yp3) / 2 - 30, Yellow, Black, "CONTAINER");
										}
										DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Yellow, 2);
										DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp2, yp2, xp3, yp3, Yellow, 2);
										DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp3, yp3, xp1, yp1, Yellow, 2);
									}
								}
							}
						}
						//UI
						this->m_UIclass->Draw();
						//タスク
						this->m_TaskOperator->Draw();
						//
						this->m_TransceiverUI->Draw();
					}
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
				//クリア時表示
				if (this->m_IsGameClear) {
					this->m_UIresult->Draw();
					this->m_TransceiverUI->Draw();
				}
			}
			//
			FadeControl::Instance()->Draw();
		}
	}
}
