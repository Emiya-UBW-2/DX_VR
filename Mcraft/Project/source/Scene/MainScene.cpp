#pragma warning(disable:4464)
#include "../MainScene/NetworkBrowser.hpp"
#include "MainScene.hpp"

#include "../MainScene/Object/Target.hpp"

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

			BattleResourceMngr->LoadChara("Main", GetMyPlayerID());
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
				Chara->SetPlayMode(false);
			}
			if (GetRand(100) < 50) {
				LoadGun("type89", GetMyPlayerID(), false, 0);
			}
			else {
				LoadGun("Mod870", GetMyPlayerID(), false, 0);
			}
			LoadGun("P226", GetMyPlayerID(), false, 1);
			//UI
			this->m_UIclass.Load();
			PauseMenuControl::LoadPause();
			//
		}
		void			MainGameScene::Set_Sub(void) noexcept {
			auto* OptionParts = OptionManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
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

			PostPassParts->SetGodRayPer(0.5f);
			//Cam
			CameraParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));
			//info
			constexpr float Max = std::min(std::min(BackGround::DrawMaxXPlus, BackGround::DrawMaxZPlus), BackGround::DrawMaxYPlus) * BackGround::CellScale;
			float SQRTMax = Max;
			CameraParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), Scale3DRate * 0.03f, SQRTMax);
			//Fog
			SetVerticalFogMode(DX_FOGMODE_LINEAR);
			SetVerticalFogStartEnd(-26.f * Scale3DRate, -10.f * Scale3DRate);
			SetVerticalFogColor(0, 0, 0);
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
					pos_t = Vector3DX::vget(0.f, -20.f, 0.f);
					pos_t *= Scale3DRate;

					Vector3DX EndPos = pos_t - Vector3DX::up() * 200.f * Scale3DRate;
					if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 0.f * Scale3DRate, &EndPos)) {
						pos_t = EndPos;
					}
					c->ValueSet((PlayerID)index, true, CharaTypeID::Team);
					c->MovePoint(deg2rad(0.f), deg2rad(0.f), pos_t, 0);
				}
				p->GetAI()->Init((PlayerID)index);
			}
			auto* ObjMngr = ObjectManager::Instance();
			for (int j = 0; j < 10; j++) {
				auto Obj = std::make_shared<TargetClass>();
				ObjMngr->AddObject(Obj);
				ObjMngr->LoadModel(Obj, Obj, "data/model/Target2/");

				Vector3DX pos_t;
				pos_t.Set(GetRandf(10.f), -20.f, GetRandf(10.f));
				pos_t *= Scale3DRate;

				Vector3DX EndPos = pos_t - Vector3DX::up() * 200.f * Scale3DRate;
				if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 0.f * Scale3DRate, &EndPos)) {
					pos_t = EndPos;
				}
				Obj->SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRand(360))));
				Obj->SetMove().SetPos(pos_t);
				Obj->SetMove().Update(0.f, 0.f);
				Obj->UpdateObjMatrix(Obj->GetMove().GetMat(), Obj->GetMove().GetPos());
			}
			ScoreBoard2.Load("data/UI/Score2.png");
			//UI
			tgtSel = -1;
			tgtTimer = 0.f;
			//UI
			this->m_UIclass.Set();
			//
			this->m_DamageEvents.clear();
			auto* NetBrowser = NetWorkBrowser::Instance();
			NetBrowser->Init();
			PauseMenuControl::SetPause();
			FadeControl::SetFade();
			this->m_IsEnd = false;
			EffectControl::Init();
			this->m_StartTimer = 5.f;
		}
		bool			MainGameScene::Update_Sub(void) noexcept {
			auto* CameraParts = Camera3D::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
#ifdef DEBUG
			/*
			{
				auto* WindowSizeParts = WindowSizeControl::Instance();
				if (CheckHitKey(KEY_INPUT_1) != 0) {
					m_D1 = std::clamp(m_D1 - 0.1f * 1.f / DXLib_refParts->GetFps(), 0.f, 1.f);
				}
				if (CheckHitKey(KEY_INPUT_2) != 0) {
					m_D1 = std::clamp(m_D1 + 0.1f * 1.f / DXLib_refParts->GetFps(), 0.f, 1.f);
				}
				if (CheckHitKey(KEY_INPUT_3) != 0) {
					m_D2 = std::clamp(m_D2 - 0.1f * 1.f / DXLib_refParts->GetFps(), 0.f, 1.f);
				}
				if (CheckHitKey(KEY_INPUT_4) != 0) {
					m_D2 = std::clamp(m_D2 + 0.1f * 1.f / DXLib_refParts->GetFps(), 0.f, 1.f);
				}
				if (CheckHitKey(KEY_INPUT_5) != 0) {
					m_D3 = std::clamp(m_D3 - 0.1f * 1.f / DXLib_refParts->GetFps(), 1.f, 10.f);
				}
				if (CheckHitKey(KEY_INPUT_6) != 0) {
					m_D3 = std::clamp(m_D3 + 0.1f * 1.f / DXLib_refParts->GetFps(), 1.f, 10.f);
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
				if (!this->m_IsEnd) {
					FadeControl::SetBlackOut(true);
				}
				this->m_IsEnd = true;
			}
			if (this->m_IsEnd && FadeControl::IsFadeAll()) {
				return false;
			}

			FadeControl::UpdateFade();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			auto& Chara = (std::shared_ptr<Sceneclass::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();

			auto* SceneParts = SceneControl::Instance();
			auto* Pad = PadControl::Instance();
			Pad->SetMouseMoveEnable(true);
			auto* KeyGuideParts = KeyGuide::Instance();
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
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("Execute=Start");
#endif // DEBUG
			//FirstDoingv
			if (GetIsFirstLoop()) {
				//SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Environment))->Play(DX_PLAYTYPE_LOOP, TRUE);
			}
			//Input,AI
			{
				if (!GetIsFirstLoop()) {
					this->m_StartTimer = std::max(this->m_StartTimer - DXLib_refParts->GetDeltaTime(), 0.f);
				}
				if (2.f > this->m_StartTimer && this->m_StartTimer > 1.f) {
					Chara->SetAim();
				}
				InputControl MyInput;
				if (SceneParts->IsPause() || !FadeControl::IsFadeClear() || (this->m_StartTimer > 0.f)) {
					MyInput.ResetAllInput();
				}
				else {
					MyInput.ResetAllInput();
					MyInput.SetAddxRad(Pad->GetLS_Y() / 200.f);
					MyInput.SetAddyRad(Pad->GetLS_X() / 200.f);
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
					MyInput.SetInputPADS(Controls::PADS::SHOT, Pad->GetPadsInfo(Controls::PADS::SHOT).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::AIM, Pad->GetPadsInfo(Controls::PADS::AIM).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::ULT, Pad->GetPadsInfo(Controls::PADS::ULT).GetKey().press());
					//MyInput.SetInputPADS(Controls::PADS::THROW, Pad->GetPadsInfo(Controls::PADS::THROW).GetKey().press());
					//MyInput.SetInputPADS(Controls::PADS::CHECK, Pad->GetPadsInfo(Controls::PADS::CHECK).GetKey().press());
					MyInput.SetInputPADS(Controls::PADS::WALK, Pad->GetPadsInfo(Controls::PADS::WALK).GetKey().press());
					//MyInput.SetInputPADS(Controls::PADS::JUMP, Pad->GetPadsInfo(Controls::PADS::JUMP).GetKey().press());
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
							p->GetAI()->Execute(&OtherInput);
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
			tgtTimer = std::max(tgtTimer - DXLib_refParts->GetDeltaTime(), 0.f);
			//視点
			{
				auto& ViewChara = (std::shared_ptr<Sceneclass::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
				//カメラ
				Vector3DX CamPos = ViewChara->GetEyeMatrix().pos() + Camera3D::Instance()->GetCamShake();
				Vector3DX CamVec = CamPos + ViewChara->GetEyeMatrix().zvec() * -1.f;
				CamVec += Camera3D::Instance()->GetCamShake();
				CamPos += Camera3D::Instance()->GetCamShake() * 2.f;
#ifdef DEBUG
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
					CamPos += ViewChara->GetEyeMatrix().xvec() * (10.f * Scale3DRate);
					break;
				case 1:
					CamVec = CamPos;
					CamPos -= ViewChara->GetEyeMatrix().xvec() * (10.f * Scale3DRate);
					break;
				case 2:
					CamVec = CamPos;
					CamPos += ViewChara->GetEyeMatrix().yvec() * (10.f * Scale3DRate) + ViewChara->GetEyeMatrix().zvec() * 0.1f;
					break;
				case 3:
					CamVec = CamPos;
					CamPos += ViewChara->GetEyeMatrix().zvec() * (-10.f * Scale3DRate);
					break;
				default:
					break;
				}
#endif
				CameraParts->SetMainCamera().SetCamPos(CamPos, CamVec, ViewChara->GetEyeMatrix().yvec());
				auto fov_t = CameraParts->GetMainCamera().GetCamFov();
				auto near_t = CameraParts->GetMainCamera().GetCamNear();
				auto far_t = CameraParts->GetMainCamera().GetCamFar();
				//fov
				{
					auto* OptionParts = OptionManager::Instance();
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
#ifdef DEBUG
				if (0 <= DBG_CamSel && DBG_CamSel <= 3) {
					fov_t = deg2rad(15);
				}
#endif

				CameraParts->SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
				//DoF
				if (Chara->GetIsADS()) {
					PostPassEffect::Instance()->Set_DoFNearFar(Scale3DRate * 0.3f, Scale3DRate * 5.f, Scale3DRate * 0.1f, far_t * 3.f);
				}
				else {
					PostPassEffect::Instance()->Set_DoFNearFar(Scale3DRate * 0.3f, Scale3DRate * 5.f, Scale3DRate * 0.1f, far_t * 2.f);
				}
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
				m_Concussion = std::max(m_Concussion - 1.f / DXLib_refParts->GetFps(), 0.f);
			}
			BackGround->Execute();
			//UIパラメーター
			{
				//NvsN
				this->m_UIclass.SetIntParam(0, 0);
				this->m_UIclass.SetIntParam(1, 0);
				//timer
				this->m_UIclass.SetfloatParam(0, 0.f);
				this->m_UIclass.SetfloatParam(1, m_StartTimer);
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
			ScoreBoard2.Dispose();
			//
			if (m_NetWorkController) {
				m_NetWorkController->Dispose();
				m_NetWorkController.reset();
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
		void			MainGameScene::BG_Draw_Sub(void) const noexcept {
			BackGround::BackGroundClass::Instance()->BG_Draw();
		}
		void			MainGameScene::ShadowDraw_Far_Sub(void) const noexcept {
		}
		void			MainGameScene::ShadowDraw_Sub(void) const noexcept {
			BackGround::BackGroundClass::Instance()->Shadow_Draw();
			ObjectManager::Instance()->Draw_Shadow();
		}
		void			MainGameScene::CubeMap_Sub(void) const noexcept {
			BackGround::BackGroundClass::Instance()->Draw();
		}

		void MainGameScene::SetShadowDraw_Rigid_Sub(void) const noexcept {
			BackGround::BackGroundClass::Instance()->SetShadow_Draw_Rigid();
		}

		void MainGameScene::SetShadowDraw_Sub(void) const noexcept {
			ObjectManager::Instance()->Draw();
		}

		void			MainGameScene::MainDraw_Sub(void) const noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			SetFogEnable(TRUE);
			SetVerticalFogEnable(TRUE);
			BackGround::BackGroundClass::Instance()->Draw();
			ObjectManager::Instance()->Draw();
			//ObjectManager::Instance()->Draw_Depth();
			for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
				PlayerMngr->GetPlayer(i)->GetAI()->Draw();
			}
#ifdef DEBUG
			for (auto& s : m_LineDebug) {
				int index = &s - &m_LineDebug.front();
				DrawCapsule3D(s.PosA.get(), s.PosB.get(), 0.02f * Scale3DRate, 8, GetColor(0, 0, (32 * index) % 256), GetColor(0, 0, 0), TRUE);
			}
#endif // DEBUG
			SetVerticalFogEnable(FALSE);
			SetFogEnable(FALSE);


			//シェーダー描画用パラメーターセット
			auto* PostPassParts = PostPassEffect::Instance();

			auto& Chara = (std::shared_ptr<Sceneclass::CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			if (Chara->GetGunPtrNow()) {
				Chara->GetGunPtrNow()->UpdateReticle();
				float Zoom = Chara->GetSightZoomSize();
				bool IsActive = Chara->GetGunPtrNow()->IsActiveReticle() && Zoom > 1.f;
				PostPassParts->Set_is_lens(IsActive);
				PostPassParts->Set_zoom_lens(std::max(1.f, Zoom / 2.f));
				if (IsActive) {
					PostPassParts->Set_xp_lens(Chara->GetGunPtrNow()->GetLensXPos());
					PostPassParts->Set_yp_lens(Chara->GetGunPtrNow()->GetLensYPos());
					PostPassParts->Set_size_lens(Chara->GetGunPtrNow()->GetLensSize());
				}
			}
			else {
				PostPassParts->Set_is_lens(false);
				PostPassParts->Set_zoom_lens(1.f);
			}
		}
		//UI表示
		void			MainGameScene::DrawUI_Base_Sub(void) const noexcept {
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			if (!Chara->IsAlive()) { return; }
			//レティクル表示
			if (Chara->GetGunPtrNow()) {
				if (Chara->GetGunPtrNow()->IsActiveReticle() && Chara->GetGunPtrNow()->GetSightPtr() &&
					!((Chara->GetADSPer() < 0.8f) && Chara->GetSightZoomSize() > 1.f)) {
					auto* WindowParts = WindowSystem::DrawControl::Instance();
					WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &(*Chara->GetGunPtrNow()->GetSightPtr())->GetModData()->GetReitcleGraph(),
						static_cast<int>(Chara->GetGunPtrNow()->GetReticleXPos() * 1980 / WindowSizeParts->GetScreenY(1980)),
						static_cast<int>(Chara->GetGunPtrNow()->GetReticleYPos() * 1080 / WindowSizeParts->GetScreenY(1080)),
						1.f, Chara->GetLeanRad(), true);
				}
			}


			FadeControl::DrawFade();
			//UI
			auto* SceneParts = SceneControl::Instance();
			if (!SceneParts->IsPause()) {
				this->m_UIclass.Draw();
			}
			if (!SceneParts->IsPause()) {
				//的ヒット状況表示
				if (tgtSel >= 0) {
					auto* WindowParts = WindowSystem::DrawControl::Instance();
					auto& t = (std::shared_ptr<TargetClass>&)(*ObjectManager::Instance()->GetObj((int)ObjType::Target, tgtSel));

					int xp = 1920 / 2 - 300;
					int yp = 1080 / 2 + 100;
					int size = 100;
					int xs = size / 2;
					int ys = size / 2;
					int xp2 = xp + ys * 2;
					int yp2 = yp + ys * 2;
					float AlphaPer = std::clamp(tgtTimer, 0.f, 1.f);
					if (AlphaPer > 0.01f) {
						WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, static_cast<int>(255.f * AlphaPer));
						//背景
						WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &ScoreBoard2, xp, yp, xp2, yp2, true);
						//命中箇所
						for (auto& r : t->GetHitPosRec()) {
							float cos_t, sin_t;
							t->GetHitPoint(r, &cos_t, &sin_t);
							WindowParts->SetDrawCircle(WindowSystem::DrawLayer::Normal, xp + xs + static_cast<int>((float)xs * cos_t), yp + ys + static_cast<int>((float)ys * sin_t), 2, Green);
						}
						WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
					}
				}
			}
			//通信設定
			//auto* NetBrowser = NetWorkBrowser::Instance();
			//NetBrowser->Draw();
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
		}
		void MainGameScene::DrawUI_In_Sub(void) const noexcept {
			auto* SceneParts = SceneControl::Instance();
			if (SceneParts->IsPause()) {
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
						Vector3DX pos_tmp = a->GetPos();

						bool is_HitAll = false;

						Vector3DX norm_tmp;
						auto ColResGround = BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, &norm_tmp);


						int j = 0;
						while (true) {
							auto target = ObjMngr->GetObj((int)ObjType::Target, j);
							if (target != nullptr) {
								auto& t = (std::shared_ptr<TargetClass>&)(*target);
								auto Res = t->GetColLine(repos_tmp, pos_tmp, -1);
								if (Res.HitFlag == TRUE) {
									pos_tmp = Res.HitPosition;
									norm_tmp = Res.Normal;
									//エフェクト
									EffectControl::SetOnce_Any((int)EffectResource::Effect::ef_gndsmoke, pos_tmp, (pos_tmp - repos_tmp).normalized(), a->GetCaliberSize() / 0.02f * Scale3DRate);
									SE->Get(SoundType::SE, (int)SoundEnum::HitGround0 + GetRand(5 - 1))->Play3D(pos_tmp, Scale3DRate * 10.f);
									//ヒット演算
									if (tgtSel != -1 && tgtSel != j) {
										auto& tOLD = (std::shared_ptr<TargetClass>&)(*ObjMngr->GetObj((int)ObjType::Target, tgtSel));
										tOLD->ResetHit();
									}
									tgtSel = j;
									tgtTimer = 2.f;

									t->SetHitPos(pos_tmp);

									a->Penetrate(0, 0);
									is_HitAll = true;
								}
							}
							else {
								break;
							}
							j++;
						}


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
#ifdef DEBUG
						//m_LineDebug.emplace_back();
						//m_LineDebug.back().PosA = repos_tmp;
						//m_LineDebug.back().PosB = pos_tmp;
						//m_LineDebug.back().Time = 10.f;
#endif // DEBUG
						if (ColResGround && !is_HitAll) {
							a->HitGround(pos_tmp);
							EffectControl::SetOnce_Any((int)EffectResource::Effect::ef_gndsmoke, pos_tmp, (pos_tmp - repos_tmp).normalized(), a->GetCaliberSize() / 0.02f * Scale3DRate);
							SE->Get(SoundType::SE, (int)SoundEnum::HitGround0 + GetRand(5 - 1))->Play3D(pos_tmp, Scale3DRate * 10.f);
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
