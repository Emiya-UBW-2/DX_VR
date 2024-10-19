#include	"MainScene.hpp"
#include "../sub.hpp"
#include "../MainScene/Player/Player.hpp"
#include "../MainScene/Player/CPU.hpp"
#include "../MainScene/Object/Character.hpp"
#include <ctime>

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			MAINLOOP::Load_Sub(void) noexcept {
			//ロード
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			auto* DrawParts = DXDraw::Instance();
			//BG
			GunAnimManager::Instance()->Load("data/CharaAnime/");
			//
			BattleResourceMngr->Load();
			//
			this->m_UIclass.Load();
			this->hit_Graph.Load("data/UI/battle_hit.bmp");
			this->guard_Graph.Load("data/UI/battle_guard.bmp");
			this->m_MiniMapScreen.Make(DrawParts->GetUIY(128) * 2, DrawParts->GetUIY(128) * 2, true);
			PlayerMngr->Init(11, 0);
			for (int i = 1; i < PlayerMngr->GetPlayerNum(); i++) {
				BattleResourceMngr->LoadChara("Soldier", (PlayerID)i);
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
				if (i == 1) {
					MV1::Load((c->GetFilePath() + "model_Rag.mv1").c_str(), &c->GetRagDoll(), DX_LOADMODEL_PHYSICS_REALTIME, -1.f);//身体ラグドール
					MV1::SetAnime(&c->GetRagDoll(), c->GetObj());
				}
				else {
					auto& Base = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1)->GetChara();
					c->GetRagDoll().Duplicate(Base->GetRagDoll());
					MV1::SetAnime(&c->GetRagDoll(), c->GetObj());
				}
				c->Init_RagDollControl(c->GetObj());
			}

			std::string GunName;
			for (int i = 1; i < PlayerMngr->GetPlayerNum(); i++) {
				if (!m_IsHardMode) {
					GunName = "PCC_4";
				}
				else {
					GunName = ULT_GUNName[GetRand((int)ULT_GUN::Max - 1)];
				}
				LoadGun(GunName.c_str(), (PlayerID)i, false, 0);
			}
		}
		void			MAINLOOP::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			//
			Vector3DX LightVec = Vector3DX::vget(0.f, -1.f, 0.f);
			DrawParts->SetAmbientLight(LightVec, GetColorF(0.92f, 0.91f, 0.90f, 0.0f));
			SetShadowScale(0.5f);
			//
			this->m_BackGround = std::make_shared<BackGroundClassMain>();
			this->m_BackGround->Init("", "");//1.59秒
			//ロード
			BattleResourceMngr->LoadChara("Suit", GetMyPlayerID());
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
				Chara->LoadExtends();
				Chara->SetPlayMode(m_IsHardMode);
			}
			GunsModify::LoadSlots("Save/gundata.svf");//プリセット読み込み
			if (!m_IsHardMode) {
				LoadGun("G17Gen3", GetMyPlayerID(), true, 0);
				std::string ULTName = ULT_GUNName[(int)GunsModify::GetULTSelect()];
				LoadGun(ULTName.c_str(), GetMyPlayerID(), false, 1);
			}
			else {
				LoadGun("M4A1", GetMyPlayerID(), false, 0);
				LoadGun("G17Gen3", GetMyPlayerID(), true, 1);
			}

			//BGをオブジェに登録
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
				c->SetMapCol(this->m_BackGround);
			}
			//人の座標設定
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
				auto& p = PlayerMngr->GetPlayer(index);
				auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
				Vector3DX pos_t;
				float rad_t = 0.f;
				if (index == 0) {
					pos_t = Vector3DX::vget(0.f, 0.f, 0.f);
					Vector3DX BGPos_XZ;
					for (auto& C : this->m_BackGround->GetBuildData()) {
						if (C.GetMeshSel() < 0) { continue; }
						BGPos_XZ = C.GetMatrix().pos(); BGPos_XZ.y = (0.f);
						if (BGPos_XZ.magnitude() < 5.f*Scale_Rate) {
							pos_t = BGPos_XZ;
							break;
						}
					}
					rad_t = deg2rad(GetRandf(180.f));
				}
				else {
					auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0)->GetChara();
					Vector3DX TgtPos_XZ = TargetChara->GetMove().GetPos(); TgtPos_XZ.y = (0.f);
					Vector3DX BGPos_XZ;
					while (true) {
						auto& C = this->m_BackGround->GetBuildData().at(static_cast<size_t>(GetRand(static_cast<int>(this->m_BackGround->GetBuildData().size()) - 1)));
						BGPos_XZ = C.GetMatrix().pos(); BGPos_XZ.y = (0.f);
						if ((BGPos_XZ - TgtPos_XZ).magnitude() > 10.f*Scale_Rate) {
							auto StartPos = TgtPos_XZ + Vector3DX::up()*(1.f*Scale_Rate);
							auto EndPos = BGPos_XZ + Vector3DX::up()*(1.f*Scale_Rate);
							bool CanLookTarget = true;
							for (auto& C2 : this->m_BackGround->GetBuildData()) {
								if (C2.GetMeshSel() < 0) { continue; }
								if (GetMinLenSegmentToPoint(StartPos, EndPos, C2.GetMatrix().pos()) >= 8.f*Scale_Rate) { continue; }
								auto ret = C2.GetColLine(StartPos, EndPos);
								if (ret.HitFlag == TRUE) {
									CanLookTarget = false;
									break;
								}
							}
							if (!CanLookTarget) { break; }
						}
					}


					pos_t = BGPos_XZ;
					rad_t = deg2rad(GetRandf(180.f));
				}

				/*
				Vector3DX EndPos = pos_t + Vector3DX::up() * 10.f*Scale_Rate;
				if (this->m_BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * -10.f*Scale_Rate, &EndPos, false)) {
					pos_t = EndPos;
				}
				*/

				c->ValueSet((PlayerID)index, true, (index == 0) ? CharaTypeID::Team : CharaTypeID::Enemy);
				c->MovePoint(deg2rad(0.f), rad_t, pos_t, (index == 0) ? 1 : 0);
				c->Heal(100, true);
				if (index != 0) {
					p->GetAI()->Init(m_BackGround, (PlayerID)index);
				}
			}
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));
			m_DeathCamYAdd = 0.f;
			m_DeathPer = 0.f;
			//サウンド
			BattleResourceMngr->Set();
			//UI
			if (!m_IsHardMode) {
				std::string ULTName = ULT_GUNName[(int)GunsModify::GetULTSelect()];
				this->m_UIclass.Set("G17Gen3", ULTName.c_str(), m_IsHardMode);
			}
			else {
				this->m_UIclass.Set("M4A1", "G17Gen3", m_IsHardMode);
			}
			prevScore = 0;
			prevLastMan = -1;
			//入力
			this->m_DamageEvents.clear();
			auto* NetBrowser = NetWorkBrowser::Instance();
			NetBrowser->Init();
			m_ReadyTimer = 6.f;
			m_Timer = 180.f;//61.f;
			m_RashStartLog = false;
			m_IsEnd = false;
			m_PreEndTimer = -1.f;
			m_EndTimer = -1.f;
			m_LastMan = -1;
			//
			m_MainLoopPauseControl.Init();
			m_ConcussionControl.Init();
			//
			StartUIParam();
			//
			for (auto& r : m_ResultFlip) {
				r.Init();
			}
			m_ResultRankingPer = 1.f;
			m_ResultRankDrawTime = 0.f;
			m_ResultRank = -1;
			m_ResultPhase = 0;

			m_SelectBackImage.Load("CommonData/UI/select.png");
			ButtonSel.LoadCommon(&m_SelectBackImage);

			ButtonSel.Load_Icon("CommonData/UI/Right.png", true);
			ButtonSel.Set((960), (840), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);

			ButtonSel.SetNone();
			//開始時刻
			m_StartTime = std::time(nullptr);
			EffectControl::Init();
		}
		bool			MAINLOOP::Update_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* BGM = BGMPool::Instance();
			auto* ItemLogParts = SideLog::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();

			auto* Pad = PadControl::Instance();
			Pad->SetMouseMoveEnable(!m_IsEnd);
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
						if (m_IsEnd) {
							KeyGuide->AddGuide(PADS::INTERACT, "スキップ");
						}
						else if (m_PreEndTimer == -1.f) {
							KeyGuide->AddGuide(PADS::MOVE_W, "");
							KeyGuide->AddGuide(PADS::MOVE_S, "");
							KeyGuide->AddGuide(PADS::MOVE_A, "");
							KeyGuide->AddGuide(PADS::MOVE_D, "");
							KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9900));
							KeyGuide->AddGuide(PADS::LEAN_L, "");
							KeyGuide->AddGuide(PADS::LEAN_R, LocalizePool::Instance()->Get(9901));
							KeyGuide->AddGuide(PADS::RUN, LocalizePool::Instance()->Get(9902));
							KeyGuide->AddGuide(PADS::WALK, LocalizePool::Instance()->Get(9903));
							KeyGuide->AddGuide(PADS::SQUAT, LocalizePool::Instance()->Get(9904));
							KeyGuide->AddGuide(PADS::JUMP, LocalizePool::Instance()->Get(9905));
							KeyGuide->AddGuide(PADS::SHOT, LocalizePool::Instance()->Get(9906));
							KeyGuide->AddGuide(PADS::ULT, LocalizePool::Instance()->Get(9907));
							KeyGuide->AddGuide(PADS::AIM, LocalizePool::Instance()->Get(9908));
							KeyGuide->AddGuide(PADS::MELEE, LocalizePool::Instance()->Get(9909));
							KeyGuide->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9910));
							KeyGuide->AddGuide(PADS::CHECK, LocalizePool::Instance()->Get(9911));
							KeyGuide->AddGuide(PADS::THROW, LocalizePool::Instance()->Get(9912));
						}
					}
				});

			if (DXDraw::Instance()->IsPause()) {
				if (!DrawParts->IsExit() && !DrawParts->IsRestart()) {
					m_MainLoopPauseControl.Execute();
				}
#if FALSE
				if (m_IsEnd && GetMovieStateToGraph(movie.get()) == 1) {
					PauseMovieToGraph(movie.get());
				}
#else
#endif
				m_CountDownBGMTimer = (int)m_ReadyTimer;
				if (!m_NetWorkController) {
					return true;
				}
			}
			else {
				m_MainLoopPauseControl.Reset();
				if (!m_IsEnd) {
					if (m_LastMan == 0 || (m_Timer <= 0.f)) {
						if (m_PreEndTimer < 0.f) {
							m_PreEndTimer = 5.f;
							ItemLogParts->Add(3.f, GetColor(25, 122, 75), "%s +%4d", LocalizePool::Instance()->Get(202), 100);
							PlayerMngr->GetPlayer(GetMyPlayerID())->AddScore(100);
							if (m_LastMan == 0) {
								ItemLogParts->Add(3.f, GetColor(25, 122, 75), "%s +%4d", LocalizePool::Instance()->Get(203), 100);
								PlayerMngr->GetPlayer(GetMyPlayerID())->AddScore(100);
							}
						}
						else {
							m_PreEndTimer = std::max(m_PreEndTimer - 1.f / DrawParts->GetFps(), 0.f);
						}
						if (m_PreEndTimer == 0.f) {
							StartResult();
						}
					}
					if (m_IsHardMode && (m_DeathPer >= 0.99f)) {
						StartResult();
					}
				}
				else {
					if (UpdateResult()) {
						return false;
					}
				}
				if (!Chara->IsAlive()) {
					Easing(&m_DeathPer, 1.f, 0.975f, EasingType::OutExpo);
				}
				if (!m_IsHardMode && (m_DeathPer >= 0.99f)) {
					return false;
				}
				if (m_MainLoopPauseControl.GetIsRetireSelected()) {
					return false;
				}
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("update start");
#endif // DEBUG
			//FirstDoingv
			if (GetIsFirstLoop()) {
				SetMousePoint(DXDraw::Instance()->GetScreenX(1920) / 2, DXDraw::Instance()->GetScreenY(1080) / 2);
				m_ReadyTimer = 6.f;
				SE->Get((int)SoundEnum::Env).Play(0, DX_PLAYTYPE_LOOP);
				SE->Get((int)SoundEnum::Env2).Play(0, DX_PLAYTYPE_LOOP);
				if (!m_IsHardMode) {
					ItemLogParts->Add(3.f, GetColor(251, 91, 1), LocalizePool::Instance()->Get(204));
				}
				else {
					ItemLogParts->Add(3.f, GetColor(251, 91, 1), LocalizePool::Instance()->Get(205));
					m_Timer = 0.001f;
				}
			}
			else {
				m_ReadyTimer = std::max(m_ReadyTimer - 1.f / DrawParts->GetFps(), 0.f);
				if (m_ReadyTimer == 0.f && Chara->IsAlive()) {
					if (!m_IsHardMode) {
						m_Timer = std::max(m_Timer - 1.f / DrawParts->GetFps(), 0.f);
					}
					else {
						m_Timer = m_Timer + 1.f / DrawParts->GetFps();
					}
				}
				if (m_ReadyTimer > 0.f) {
					BGM->Get(0)->SetVol_Local(static_cast<int>(255.f*m_ReadyTimer / 6.f));
				}
				else {
					BGM->Get(0)->Stop();
				}
				if (m_IsHardMode) {
					Chara->AddULT(100, false);
				}
			}
			//
			UpdateInput();							//Input,AI
			ObjMngr->ExecuteObject();				//Execute
			UpdateBullet();							//弾の更新
			UpdateItem();							//アイテム入手
			UpdateMelee();							//近接攻撃
			this->m_BackGround->FirstExecute();		//背景更新
			ObjMngr->LateExecuteObject();
			UpdateView();							//視点
			this->m_BackGround->Execute();			//
			UpdateLight();							//射撃光
			UpdateLaser();							//レーザーサイト
#ifdef DEBUG
			DebugParts->SetPoint("---");
#endif // DEBUG
			UpdateUIParam();						//UIパラメーター
			EffectControl::Execute();				//
			UpdateMiniMap();						//ミニマップ
#ifdef DEBUG
			DebugParts->SetPoint("update end");
#endif // DEBUG
			return true;
		}
		void			MAINLOOP::Dispose_Sub(void) noexcept {
			{
				//
				auto* SaveDataParts = SaveDataClass::Instance();
				auto* ItemLogParts = SideLog::Instance();

				//クリア
				if (m_IsEnd) {
					//ハードモードアンロック
					if (SaveDataParts->GetParam("UnlockHardMode") != 1) {
						SaveDataParts->SetParam("UnlockHardMode", 1);
						ItemLogParts->Add(10.f, GetColor(50, 255, 50), "%s が開放されました！", "Fire Party モード");
					}
				}
				//
				if (!m_MainLoopPauseControl.GetIsRetireSelected()) {
					int bases = std::clamp(static_cast<int>(SaveDataParts->GetParam("ULT Unlock")), 1, (int)ULT_GUN::Max);
					SaveDataParts->SetParam("ULT Unlock", std::clamp(bases + 1, 1, (int)ULT_GUN::Max));
					if (bases != static_cast<int>(SaveDataParts->GetParam("ULT Unlock"))) {
						ItemLogParts->Add(10.f, GetColor(50, 255, 50), "%s が開放されました！", ULT_GUNName[SaveDataParts->GetParam("ULT Unlock") - 1]);
					}
				}
				//
				if (!m_MainLoopPauseControl.GetIsRetireSelected()) {
					int bases = std::clamp(static_cast<int>(SaveDataParts->GetParam("Glock Unlock")), 1, 4);
					SaveDataParts->SetParam("Glock Unlock", std::clamp(bases + 1, 1, 4));
					int after = static_cast<int>(SaveDataParts->GetParam("Glock Unlock"));
					if (bases != after) {
						switch (after - 1) {
							case 1:
								{
									SaveDataParts->SetParam("G17 ThreadedBarrel", 1);
									SaveDataParts->SetParam("9x19 Suppressor", 1);
									ItemLogParts->Add(10.f, GetColor(50, 255, 50), "%s が開放されました！", "サプレッサーキット");
								}
								break;
							case 2:
								{
									SaveDataParts->SetParam("G-High IronSight", 1);
									SaveDataParts->SetParam("Glock17 Gen2 Frame", 1);
									SaveDataParts->SetParam("SBal PL LaserSight", 1);
									ItemLogParts->Add(10.f, GetColor(50, 255, 50), "%s が開放されました！", "スポーターキット");
								}
								break;
							case 3:
								{
									SaveDataParts->SetParam("FMG-9 Carbine Kit", 1);
									SaveDataParts->SetParam("Glock18 Slide (for 17)", 1);
									SaveDataParts->SetParam("ACOG TA31 Scope", 1);
									ItemLogParts->Add(10.f, GetColor(50, 255, 50), "%s が開放されました！", "FMG-9キット");
								}
								break;
							default:
								break;
						}
					}
				}
				SaveDataParts->Save();
			}
			auto* SE = SoundPool::Instance();
			auto* BGM = BGMPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			BGM->Get(0)->Stop();

			SE->Get((int)SoundEnum::Env).StopAll(0);
			SE->Get((int)SoundEnum::Env2).StopAll(0);
			//使い回しオブジェ系
			ObjectManager::Instance()->DelObj(&PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara());
			PlayerMngr->GetPlayer(GetMyPlayerID())->Dispose();
			this->m_BackGround->Dispose();
			this->m_BackGround.reset();
			GunsModify::DisposeSlots();
			//
			EffectControl::Dispose();

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
			m_SelectBackImage.Dispose();
			ButtonSel.Dispose();

			m_MainLoopPauseControl.Dispose();
#if FALSE
			movie.Dispose();
#else
#endif
		}
		//
		void			MAINLOOP::BG_Draw_Sub(void) noexcept {
			if (m_IsEnd) {
				return;
			}
			this->m_BackGround->BG_Draw();
		}
		void			MAINLOOP::ShadowDraw_Sub(void) noexcept {
			if (m_IsEnd) {
				return;
			}
			this->m_BackGround->Shadow_Draw();
			ObjectManager::Instance()->Draw_Shadow();
		}
		void MAINLOOP::SetShadowDraw_Sub(void) noexcept {
			ObjectManager::Instance()->Draw_Shadow();
		}
		void			MAINLOOP::MainDraw_Sub(void) noexcept {
			//*
			SetVerticalFogEnable(TRUE);
			SetVerticalFogMode(DX_FOGMODE_LINEAR);
			//SetVerticalFogColor(55, 150, 255);
			//SetVerticalFogStartEnd(1.5f*Scale_Rate, -10.f*Scale_Rate);
			SetVerticalFogColor(16, 16, 16);
			SetVerticalFogStartEnd(1.5f*Scale_Rate, -1.f*Scale_Rate);
			//SetVerticalFogDensity(0.3f*Scale_Rate, 0.2f);
			//*/
			if (m_IsEnd) {
				return;
			}
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			SetFogStartEnd(Scale_Rate * 1200.f, Scale_Rate * 1600.f);

			this->m_BackGround->Draw();
			ObjMngr->Draw();
			//レーザー
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
				c->DrawLaser();
			}

			//ObjMngr->Draw_Depth();
			//シェーダー描画用パラメーターセット
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
#ifdef DEBUG
			if (false) {
				for (int i = 1; i < PlayerMngr->GetPlayerNum(); i++) {
					PlayerMngr->GetPlayer(i)->GetAI()->Draw();
				}
			}
			//
#endif
			SetVerticalFogEnable(FALSE);
		}
		void			MAINLOOP::MainDrawFront_Sub(void) noexcept {
			if (m_IsEnd) {
				return;
			}
			this->m_BackGround->DrawFront();
		}
		//UI表示
		void			MAINLOOP::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			if (m_PreEndTimer == -1.f) {
				if (Chara->IsAlive()) {
					//auto* OptionParts = OPTION::Instance();
					DrawHitGraph();			//着弾表示
					DrawSoundGraph();		//サウンド表示
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
					//UI
					if (!DrawParts->IsPause()) {
						this->m_UIclass.Draw();
					}
				}
			}
			//通信設定
			/*
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
			//*/

			if (m_PreEndTimer > 0) {
				DrawBlackOut((1.f - std::max(m_PreEndTimer - 2.f, 0.f)) * 2.f);
			}
			DrawBlackOut(m_DeathPer * 1.3f);

			if (m_IsEnd) {
				DrawResult();
			}
			else if (m_PreEndTimer == -1.f) {
				if (!DrawParts->IsPause()) {
					if (Chara->IsAlive()) {
						//ミニマップ
						auto* WindowParts = WindowSystem::DrawControl::Instance();
						WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &m_MiniMapScreen, DrawParts->GetUIY(960), DrawParts->GetUIY(840), 1.f, 0.f, true);
					}
				}
			}
		}
		void MAINLOOP::DrawUI_In_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//ポーズ
			if (DrawParts->IsPause() && (!DrawParts->IsExit() && !DrawParts->IsRestart())) {
				m_MainLoopPauseControl.Draw();
			}
		}
		//使い回しオブジェ系
		void			MAINLOOP::Dispose_Load_Sub(void) noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto* BattleResourceMngr = CommonBattleResource::Instance();
				BattleResourceMngr->Dispose();
				this->m_UIclass.Dispose();
				this->hit_Graph.Dispose();
				this->guard_Graph.Dispose();
				this->m_MiniMapScreen.Dispose();
				PlayerMngr->Dispose();
				ObjectManager::Instance()->DeleteAll();
		}
		//load
		void			MAINLOOP::LoadGun(const std::string&FolderName, PlayerID ID, bool IsPreset, int Sel) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->LoadCharaGun(FolderName, ID, Sel);
			auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(ID)->GetChara();
			GunsModify::CreateSelData(c->GetGunPtr(Sel), IsPreset);
			c->GetGunPtr(Sel)->Init_Gun();
		}
		//start
		void			MAINLOOP::StartUIParam(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			this->m_UIclass.InitGaugeParam(4 + 4, 0, 20);
			this->m_UIclass.InitGaugeParam(4 + 4 + 1, 0, 20);
			//HP
			this->m_UIclass.InitGaugeParam(0, (int)Chara->GetHPMax(), (int)Chara->GetHPMax());
			//AP
			this->m_UIclass.InitGaugeParam(1, (int)Chara->GetAPMax(), (int)Chara->GetAPMax());
			//Ammo
			if (Chara->GetGunPtrNow()) {
				this->m_UIclass.InitGaugeParam(2, (int)Chara->GetGunPtrNow()->GetAmmoAll() + 1, (int)Chara->GetGunPtrNow()->GetAmmoAll() + 1);
			}
			//ULT
			this->m_UIclass.InitGaugeParam(3, 0, (int)Chara->GetULTMax());
			//GUN
			if (Chara->GetGunPtr(0)) {
				this->m_UIclass.InitGaugeParam(4, (int)Chara->GetGunPtr(0)->GetAmmoAll() + 1, (int)Chara->GetGunPtr(0)->GetAmmoAll() + 1);
			}
			//mag
			int mags = 0;
			for (const auto& M : Chara->GetMagDatas()) {
				this->m_UIclass.InitGaugeParam(5 + mags, M.AmmoAll, M.AmmoAll);
				mags++;
			}
		}
		void			MAINLOOP::StartResult(void) noexcept {
			m_IsEnd = true;
#if FALSE
			movie.Load("data/Movie/end0.mp4");
			PlayMovieToGraph(movie.get());
			ChangeMovieVolumeToGraph(std::clamp(static_cast<int>(255.f*OptionParts->GetParamFloat(EnumSaveParam::SE)), 0, 255), movie.get());
			m_movieTotalFrame = GetMovieTotalFrameToGraph(movie.get());
#endif
			PadControl::Instance()->SetGuideUpdate();

			auto* SaveDataParts = SaveDataClass::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			{
				//自記録を追加
				int ScoreNum = std::max(static_cast<int>(SaveDataParts->GetParam("ScoreNum")), 0);
				SaveDataParts->SetParam("ScoreNum", (int64_t)(ScoreNum + 1));
				SaveDataParts->SetParam("Score" + std::to_string(ScoreNum), PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore());
				SaveDataParts->SetParam("Time_" + std::to_string(ScoreNum), m_StartTime);

				m_Ranking.clear();
				for (int i = 0;i <= ScoreNum;i++) {
					int Score = static_cast<int>(SaveDataParts->GetParam("Score" + std::to_string(i)));
					int64_t Time = SaveDataParts->GetParam("Time_" + std::to_string(i));
					m_Ranking.emplace_back(std::make_pair(Score, Time));
				}
				std::sort(m_Ranking.begin(), m_Ranking.end(), std::greater<>{});

				m_ResultRank = (int)m_Ranking.size();
				for (auto& r : m_Ranking) {
					if (r.first == PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore() && r.second == m_StartTime) {
						m_ResultRank = static_cast<int>(&r - &m_Ranking.front());
						break;
					}
				}
				//スコアの記録を並べ替え
				{
					for (auto& r : m_Ranking) {
						int index = static_cast<int>(&r - &m_Ranking.front());
						SaveDataParts->SetParam("Score" + std::to_string(index), r.first);
						SaveDataParts->SetParam("Time_" + std::to_string(index), r.second);
					}
				}
				SaveDataParts->SetParam("ScoreNum", std::min(ScoreNum + 1, 20));
			}
		}
		//update
		bool			MAINLOOP::UpdateResult(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* DrawParts = DXDraw::Instance();
#if FALSE
			if (GetMovieStateToGraph(movie.get()) == 0) {
				PlayMovieToGraph(movie.get());
			}
			if (m_EndTimer < 0.f) {
				if (Pad->GetKey(PADS::INTERACT).trigger()) {
					m_EndTimer = 2.f;
				}
				if (TellMovieToGraphToFrame(movie.get()) >= m_movieTotalFrame - 5) {
					m_EndTimer = 2.f;
				}
			}
			else {
				m_EndTimer = std::max(m_EndTimer - 1.f / DrawParts->GetFps(), 0.f);
			}
#else
			{
				for (auto& r : m_ResultFlip) {
					Easing(&r.m_Flip, 1.f, 0.8f, EasingType::OutExpo);
					if (r.m_Flip < 0.99f) {
						break;
					}
				}
				m_ResultFlip.at(0).Update(
					(PlayerMngr->GetPlayer(GetMyPlayerID())->GetShot() > 0) ? 100 * 100 * PlayerMngr->GetPlayer(GetMyPlayerID())->GetHit() / PlayerMngr->GetPlayer(GetMyPlayerID())->GetShot() : 0,
					m_ResultPhase != 0);
				m_ResultFlip.at(1).Update(PlayerMngr->GetPlayer(GetMyPlayerID())->GetKill(), m_ResultPhase != 0);
				if (m_ResultFlip.at(2).Update(PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore(), m_ResultPhase != 0)) {
					if (m_ResultRankingPer > 0.5f) {
						m_ResultRankingPer = std::max(m_ResultRankingPer - 1.f / DrawParts->GetFps(), 0.f);
					}
					else {
						Easing(&m_ResultRankingPer, 0.f, 0.95f, EasingType::OutExpo);
					}
					if (m_ResultRankingPer < 0.1f) {
						m_ResultPhase = 1;
					}

					m_ResultRankDrawTime += 1.f / DrawParts->GetFps();
					if (m_ResultRankDrawTime > 1.f) {
						m_ResultRankDrawTime -= 1.f;
					}
				}
			}
			if (m_EndTimer < 0.f) {
				if (Pad->GetKey(PADS::INTERACT).trigger()) {
					if (m_ResultPhase == 1) {
						m_EndTimer = 1.f;
					}
					m_ResultPhase++;
				}
				if (ButtonSel.GetInto()) {
					if (!ButtonSel.IsFocus()) {
						ButtonSel.SetFocus();
					}
					if (Pad->GetMouseClick().trigger()) {
						m_EndTimer = 1.f;
						m_ResultPhase++;
					}
				}
				else {
					ButtonSel.SetNone();
				}
			}
			else {
				m_EndTimer = std::max(m_EndTimer - 1.f / DrawParts->GetFps(), 0.f);
			}
#endif
			ButtonSel.Update();
			return (m_EndTimer == 0.f);
		}
		void			MAINLOOP::UpdateInput(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* ItemLogParts = SideLog::Instance();
			auto* Pad = PadControl::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();

			float cam_per = ((DrawParts->GetMainCamera().GetCamFov() / deg2rad(75)) / (DrawParts->is_lens() ? DrawParts->zoom_lens() : 1.f)) / 100.f;
			cam_per *= 0.6f;
			float pp_x = 0.f, pp_y = 0.f;
			InputControl MyInput;
			//
			//cam_per /= std::max(1.f, std::hypotf(Pad->GetLS_Y(), Pad->GetLS_X()));

			pp_x = std::clamp(Pad->GetLS_Y() * cam_per*0.5f, -0.2f, 0.2f);
			pp_y = std::clamp(Pad->GetLS_X() * cam_per*0.5f, -0.2f, 0.2f);

			if (Pad->GetKey(PADS::AIM).press()) {
				pp_x /= 2.f;
				pp_y /= 2.f;
			}
			if (Chara->GetGunPtrNow()) {
				pp_x -= Chara->GetRecoilRadAdd().y;
				pp_y -= Chara->GetRecoilRadAdd().x;
			}
			MyInput.SetInputStart(pp_x, pp_y);
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
			//ネットワーク
			auto* NetBrowser = NetWorkBrowser::Instance();
			//*
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
			bool isready = (m_ReadyTimer == 0.f) && (m_PreEndTimer == -1.f);
			if (m_NetWorkController && m_NetWorkController->IsInGame()) {
				bool IsServerNotPlayer = !m_NetWorkController->GetClient() && !m_NetWorkController->GetServerPlayer();
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); ++index) {
					auto& p = PlayerMngr->GetPlayer(index);
					auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
					NetWork::PlayerNetData Ret = this->m_NetWorkController->GetLerpServerPlayerData((PlayerID)index);
					if (index == GetMyPlayerID() && !IsServerNotPlayer) {
						c->SetInput(Ret.GetInput(), isready);//自身が動かすもの
					}
					else {//サーバーからのデータで動くもの
						//サーバーがCPUを動かす場合
						if (!m_NetWorkController->GetClient()) {
							//cpu
							//p->GetAI()->Execute(&MyInput);
						}
						c->SetInput(Ret.GetInput(), isready);
						bool override_true = true;
						//override_true = Ret.GetIsActive();
						if (override_true) {
							c->SetMoveOverRide(Ret.GetMove());
							//アクションが違う場合には上書き
							if (Ret.GetFreeData()[0] != static_cast<int>(c->GetCharaAction())) {
								//c->SetActionOverRide(static_cast<EnumArmAnimType>(Ret.GetFreeData()[0]));
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
					auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
					if (index == GetMyPlayerID()) {
						c->SetInput(MyInput, isready && c->IsAlive());
					}
					else {
						InputControl OtherInput;
						p->GetAI()->Execute(&OtherInput, (!m_IsHardMode && (m_Timer > 60.f)) || m_IsHardMode);
						c->SetInput(OtherInput, isready && c->IsAlive());
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
			//
			if (Chara->IsAlive()) {
				if (!m_IsHardMode) {
					if (m_Timer > 60.f) {
						m_LastMan = -1;
					}
					else {
						if (!m_RashStartLog) {
							m_RashStartLog = true;
							ItemLogParts->Add(3.f, GetColor(251, 91, 1), LocalizePool::Instance()->Get(206));
						}
						m_LastMan = 0;
						for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
							if (index != GetMyPlayerID()) {
								auto& p = PlayerMngr->GetPlayer(index);
								if (!p->GetAI()->CannotRepop()) {
									m_LastMan++;
								}
							}
						}
						if (m_LastMan != prevLastMan) {
							ItemLogParts->Add(3.f, GetColor(251, 91, 1), "%s %d", LocalizePool::Instance()->Get(207), m_LastMan);
						}
					}
					prevLastMan = m_LastMan;

					if (m_Timer <= 6.f) {
						if (!m_CountDownBGM1Flag) {
							m_CountDownBGM1Flag = true;
							SE->Get((int)SoundEnum::Second).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					else {
						m_CountDownBGM1Flag = false;
					}
					if (m_Timer < 61.f) {
						if (!m_CountDownBGM2Flag) {
							m_CountDownBGM2Flag = true;
							SE->Get((int)SoundEnum::OneMunute).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					else {
						m_CountDownBGM2Flag = false;
					}
					if (m_Timer < 0.5f) {
						if (!m_CountDownBGM3Flag) {
							m_CountDownBGM3Flag = true;
							SE->Get((int)SoundEnum::TimeUp).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					else {
						m_CountDownBGM3Flag = false;
					}
					if (m_Timer < 60.f) {
						if (m_LastMan != 0) {
							if (m_CountDownBGMTimer != (int)m_Timer) {
								m_CountDownBGMTimer = (int)m_Timer;
								SE->Get((int)SoundEnum::CountDown).Play(0, DX_PLAYTYPE_BACK, TRUE);
								SE->Get((int)SoundEnum::CountDown).SetVol_Local((int)Lerp(64.f, 255.f, std::clamp(1.f - ((m_Timer - 15.f) / (60.f - 15.f)), 0.f, 1.f)));
							}
						}
					}
					else if (m_ReadyTimer > 0 && m_ReadyTimer <= 5) {
						if (m_CountDownBGMTimer != (int)m_ReadyTimer) {
							m_CountDownBGMTimer = (int)m_ReadyTimer;
							SE->Get((int)SoundEnum::CountDown).Play(0, DX_PLAYTYPE_BACK, TRUE);
							SE->Get((int)SoundEnum::CountDown).SetVol_Local(128);
						}
					}
					else {
						m_CountDownBGMTimer = -1;
					}
				}
			}
		}
		void			MAINLOOP::UpdateBullet(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
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

						if (GetMyPlayerID() != a->GetShootedID()) {
							if (GetMinLenSegmentToPoint(repos_tmp, pos_tmp, DrawParts->GetMainCamera().GetCamPos()) < 1.f*Scale_Rate) {
								m_ConcussionControl.SetActive();
							}
						}

						Vector3DX norm_tmp;
						auto ColResGround = this->m_BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, true, &norm_tmp);
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
						if (this->m_BackGround->HitLightCheck(repos_tmp, &pos_tmp)) {
							EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_tmp, norm_tmp, a->GetCaliberSize() / 0.02f * Scale_Rate);
							SE->Get((int)SoundEnum::HitGround0 + GetRand(5 - 1)).Play_3D(0, pos_tmp, Scale_Rate * 10.f);
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

			//コンカッション
			if (m_ConcussionControl.GetActiveSwitch()) {
				SE->Get((int)SoundEnum::Near).Play_3D(0, DrawParts->GetMainCamera().GetCamPos(), 10.f*Scale_Rate, 255);//, DX_PLAYTYPE_LOOP
			}
			DrawParts->Set_is_Blackout(m_ConcussionControl.IsActive());
			DrawParts->Set_Per_Blackout(m_ConcussionControl.GetPer());
			m_ConcussionControl.Update();
		}
		void			MAINLOOP::UpdateItem(void) const noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* ItemLogParts = SideLog::Instance();

			int loop = 0;
			while (true) {
				auto ammo = ObjMngr->GetObj((int)ObjType::ItemObj, loop);
				if (ammo != nullptr) {
					auto& a = (std::shared_ptr<ItemObjClass>&)(*ammo);
					if (a->IsActive()) {
						Vector3DX pos_tmp = a->GetMove().GetPos();
						bool isHit = false;
						for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
							if (index != 0) { break; }
							auto& tgt = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
							float Len = (tgt->GetMove().GetPos() - pos_tmp).magnitude();
							if (Len < 1.f*Scale_Rate) {
								switch (a->GetItemType()) {
									case ItemType::AMMO:
										if (tgt->GetCharaAction() != CharaActionID::AmmoLoad) {
											int Add = 0;
											if (!m_IsHardMode) {
												Add = 2 + GetRand(8);
											}
											else {
												Add = 12 + GetRand(18);
											}
											tgt->AddAmmoStock(Add);
											isHit = true;
											SE->Get((int)SoundEnum::GetAmmo).Play_3D(0, tgt->GetEyeMatrix().pos(), Scale_Rate * 10.f);
											ItemLogParts->Add(3.f, GetColor(183, 143, 0), "%s +%d", LocalizePool::Instance()->Get(208), Add);
										}
										break;
									case ItemType::ARMER:
										if (tgt->GetArmer()) {
											isHit = true;
											SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, tgt->GetEyeMatrix().pos(), Scale_Rate * 10.f);
											ItemLogParts->Add(3.f, GetColor(183, 143, 0), LocalizePool::Instance()->Get(209));
										}
										break;
									default:
										break;
								}
								if (isHit) {
									break;
								}
							}
						}
						if (isHit) {
							a->SetDelete();
						}
					}
				}
				else {
					break;
				}
				loop++;
			}
		}
		void			MAINLOOP::UpdateMelee(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
				if (c->GetCharaAction() == CharaActionID::Melee) {
					Vector3DX StartPos = c->GetEyeMatrix().pos();
					Vector3DX EndPos = StartPos + c->GetEyeMatrix().zvec() * (-1.f*Scale_Rate);
					for (int index2 = 0; index2 < PlayerMngr->GetPlayerNum(); index2++) {
						if (index == index2) { continue; }
						auto& tgt = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index2)->GetChara();
						if (!tgt->IsAlive()) { continue; }
						if (tgt->IsDamaging()) { continue; }
						HitPoint Damage = 100;
						ArmerPoint ArmerDamage = 0;
						tgt->CheckDamageRay(&Damage, &ArmerDamage, false, c->GetMyPlayerID(), StartPos, &EndPos);
					}
				}
			}
		}
		void			MAINLOOP::UpdateView(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			{
				{
					//DrawParts->GetFps()カメラ
					if (Chara->IsAlive()) {
						Vector3DX CamPos = Chara->GetEyeMatrix().pos() + CameraShake::Instance()->GetCamShake();
						DrawParts->SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeMatrix().zvec() * -1.f, Chara->GetEyeMatrix().yvec());
						m_DeathCamYAdd = -0.2f;
					}
					else {
						Vector3DX CamPos = DrawParts->SetMainCamera().GetCamPos();
						Vector3DX CamVec = DrawParts->SetMainCamera().GetCamVec() - CamPos;
						Vector3DX CamUp = DrawParts->SetMainCamera().GetCamUp();
						CamPos.y += (m_DeathCamYAdd);
						if (std::abs(m_DeathCamYAdd) > 0.01f) {
							m_DeathCamYAdd += (GravityRate / (DrawParts->GetFps() * DrawParts->GetFps())) / 2.f;
						}
						else {
							m_DeathCamYAdd = 0.f;
						}
						if (CamPos.y < 0.1f) {
							CamPos.y = (0.1f);
							m_DeathCamYAdd *= -0.2f;
						}
						float y = CamVec.y;
						Easing(&y, 0.9f, 0.95f, EasingType::OutExpo);
						CamVec.y = (y);

						float x = CamUp.y;
						Easing(&y, 0.2f, 0.95f, EasingType::OutExpo);
						CamUp.x = (x);
						DrawParts->SetMainCamera().SetCamPos(CamPos, CamPos + CamVec, CamUp);
					}
					//info
					auto fov_t = DrawParts->GetMainCamera().GetCamFov();
					auto near_t = DrawParts->GetMainCamera().GetCamNear();
					auto far_t = DrawParts->GetMainCamera().GetCamFar();
					if (Chara->GetIsADS()) {
						Easing(&near_t, Scale_Rate * 0.03f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 50.f, 0.5f, EasingType::OutExpo);
					}
					else {
						Easing(&near_t, Scale_Rate * 0.05f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 50.f, 0.5f, EasingType::OutExpo);
					}
					//fov
					{
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
					//DoF
					if (Chara->GetIsADS()) {
						PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t - 0.1f, Scale_Rate * 0.1f, far_t);
					}
					else {
						PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t / 2, near_t, far_t);
					}
				}
			}
			{
				auto Len = (DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(0)).magnitude();
				auto LenPer = std::clamp(Len / (5.f*Scale_Rate), 0.f, 1.f);
				bool HPLow = Chara->IsLowHP();
				Easing(&Min,
					(HPLow) ? Lerp(20.f, 35.f, LenPer) : Lerp(0.f, 25.f, LenPer),
					   0.95f, EasingType::OutExpo);
				Easing(&Gamma,
					(HPLow) ? Lerp(1.15f, 1.35f, LenPer) : Lerp(1.f, 1.15f, LenPer),
					   0.95f, EasingType::OutExpo);

				PostPassEffect::Instance()->SetLevelFilter((int)Min, (HPLow) ? 128 : 192, Gamma);

				Easing(&AberrationPower,
					(HPLow) ? (10.f + GetRandf(30.f)) : 1.f,
					   0.95f, EasingType::OutExpo);
				if (Chara->PopHeadShotSwitch()) {
					SE->Get((int)SoundEnum::Tinnitus).Play_3D(0, Chara->GetEyeMatrix().pos(), Scale_Rate*50.f);
					Chara->SetHeadShot();
					AberrationPower = 30.f;
				}
				DrawParts->SetAberrationPower(AberrationPower);
			}
		}
		void			MAINLOOP::UpdateLight(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			if (Chara->GetGunPtrNow() && Chara->GetGunPtrNow()->GetShotSwitch()) {// 
#if HIGH_FPS_ROM
#else
				auto mat = Chara->GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Muzzle);
				SetLightEnable(TRUE);
				ChangeLightTypePoint(mat.pos().get(),
									 4.0f*Scale_Rate,
									 0.001f,
									 0.012f,
									 0.004f);
#endif
			}
		}
		void			MAINLOOP::UpdateLaser(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
				if (!c->GetGunPtrNow()) { continue; }
				if (!c->GetGunPtrNow()->HasFrame(GunFrame::LaserSight) || !c->GetGunPtrNow()->IsActive()) {
					c->SetIsLaserActive(false);
					continue;
				}
				//
				auto mat = c->GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::LaserSight);
				Vector3DX StartPos = mat.pos();
				Vector3DX EndPos = StartPos + mat.zvec()*-1.f * 15.f*Scale_Rate;
				this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, true);
				for (int index2 = 0; index2 < PlayerMngr->GetPlayerNum(); index2++) {
					auto& c2 = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index2)->GetChara();
					if (c2->GetMyPlayerID() == c->GetMyPlayerID()) { continue; }
					if (!c2->IsAlive()) { continue; }
					if (!(GetMinLenSegmentToPoint(StartPos, EndPos, c2->GetMove().GetPos()) <= 2.0f*Scale_Rate)) { continue; }
					c2->CheckLineHitNearest(StartPos, &EndPos);
				}
				auto Vec = (EndPos - StartPos);
				EndPos = StartPos + Vec.normalized()*std::max(Vec.magnitude() - 0.3f*Scale_Rate, 0.f);
				c->SetLaserStartPos(StartPos);
				c->SetLaserEndPos(EndPos);
				c->SetIsLaserActive(true);
			}
			//ライト
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
				if (!c->GetGunPtrNow()) { continue; }
				if (!c->GetGunPtrNow()->HasFrame(GunFrame::Light) || !c->GetGunPtrNow()->IsActive()) {
					continue;
				}
				//
				auto mat = c->GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Light);
				Vector3DX StartPos = mat.pos();
				SetLightEnable(TRUE);
				ChangeLightTypeSpot(mat.pos().get(),
					(mat.zvec()*-1.f).get(),
									DX_PI_F / 8.0f,
									DX_PI_F / 12.0f,
									35.0f*Scale_Rate,
									0.0f,
									0.006f,
									0.0f);
				SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
				SetLightSpcColor(GetColorF(0.01f, 0.01f, 0.01f, 0.f));
				SetLightAmbColor(GetColorF(0.1f, 0.1f, 0.1f, 1.f));
			}
		}
		void			MAINLOOP::UpdateUIParam(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* ItemLogParts = SideLog::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			{
				Vector3DX StartPos = Chara->GetEyeMatrix().pos();
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
					if (index == 0) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
					Vector3DX TgtPos = c->GetEyeMatrix().pos();
					c->CanLookTarget = true;
					for (auto& C : this->m_BackGround->GetBuildData()) {
						if (C.GetMeshSel() < 0) { continue; }
						auto EndPos = TgtPos;
						if (GetMinLenSegmentToPoint(StartPos, EndPos, C.GetMatrix().pos()) >= 8.f*Scale_Rate) { continue; }
						auto ret = C.GetColLine(StartPos, EndPos);
						if (ret.HitFlag == TRUE) {
							c->CanLookTarget = false;
							break;
						}
					}
				}
			}
			{
				//シェイク
				this->m_UIclass.SetIntParam(0, static_cast<int>(CameraShake::Instance()->GetCamShake().x*100.f));
				this->m_UIclass.SetIntParam(1, static_cast<int>(CameraShake::Instance()->GetCamShake().y*100.f));
				this->m_UIclass.SetIntParam(2, static_cast<int>(rad2deg(Chara->GetLeanRad()*5.f)));
				//AmmoStock
				this->m_UIclass.SetIntParam(3, Chara->GetAmmoStock());
				//Time
				this->m_UIclass.SetfloatParam(0, m_Timer);
				this->m_UIclass.SetfloatParam(1, m_ReadyTimer);
				//
				this->m_UIclass.SetGaugeParam(5 + 3, static_cast<int>(Chara->GetAutoAimPer()*20.f), 20);
				this->m_UIclass.SetGaugeParam(5 + 3 + 1, static_cast<int>(Chara->GetAutoAimPer()*20.f), 20);
				this->m_UIclass.SetIntParam(4, Chara->GetAutoAimID());
				this->m_UIclass.SetfloatParam(2, Chara->GetAutoAimOn());
				//HP
				this->m_UIclass.SetGaugeParam(0, (int)Chara->GetHP(), (int)Chara->GetHPMax());
				//AP
				this->m_UIclass.SetGaugeParam(1, (int)Chara->GetAP(), (int)Chara->GetAPMax());
				//Ammo
				if (Chara->GetGunPtrNow()) {
					this->m_UIclass.SetGaugeParam(2, (int)Chara->GetGunPtrNow()->GetAmmoNumTotal(), (int)Chara->GetGunPtrNow()->GetAmmoAll() + 1);
				}
				//ULT
				this->m_UIclass.SetGaugeParam(3, (int)Chara->GetULT(), (int)Chara->GetULTMax());
				if (m_IsHardMode) {
					this->m_UIclass.SetIntParam(7, -1);
				}
				else {
					this->m_UIclass.SetIntParam(7, ((m_ReadyTimer == 0.f) && (m_PreEndTimer == -1.f) && (Chara->IsULTSelect() || Chara->IsULTActive())) ? 1 : 0);
				}
				if (Chara->GetGunPtr(0)) {
					this->m_UIclass.SetGaugeParam(4, (int)Chara->GetGunPtr(0)->GetAmmoNumTotal(), (int)Chara->GetGunPtr(0)->GetAmmoAll() + 1);
				}
				this->m_UIclass.SetfloatParam(3, Chara->GetGunSelPer());
				this->m_UIclass.SetfloatParam(4, m_ULTCounter);
				Easing(&m_ULTCounter, (100.f / (1.f / 0.25f)) * Chara->GetULT() / Chara->GetULTMax(), 0.95f, EasingType::OutExpo);
				//mag
				int mags = 0;
				for (const auto& M : Chara->GetMagDatas()) {
					this->m_UIclass.SetGaugeParam(5 + mags, M.AmmoNum, M.AmmoAll);
					mags++;
				}
				//
				auto ScoreBuf = PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore();
				if (ScoreBuf > prevScore && (ScoreBuf - prevScore) >= 100) {
					this->m_UIclass.SetIntParam(5, (ScoreBuf - prevScore));
					ItemLogParts->Add(3.f, GetColor(206, 0, 0), "%s +%4d", LocalizePool::Instance()->Get(210), (ScoreBuf - prevScore));
				}
				else {
					this->m_UIclass.SetIntParam(5, 0);
				}
				if (Chara->ULTActiveSwitch()) {
					ItemLogParts->Add(3.f, GetColor(251, 91, 1), LocalizePool::Instance()->Get(211));
				}
				prevScore = PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore();
				//
				this->m_UIclass.SetIntParam(6, PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore());

				this->m_UIclass.SetIntParam(8, Chara->GetArmerStock());
				this->m_UIclass.SetIntParam(9, Chara->GetMorphineStock());
			}
		}
		void			MAINLOOP::UpdateMiniMap(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();
			m_MiniMapScreen.SetDraw_Screen();//WindowParts->Set つかうな
			{
				float size = 0.7f;
				int xp = DrawParts->GetUIY(128);
				int yp = DrawParts->GetUIY(128);

				int xp1 = -static_cast<int>(15.f + Chara->GetMove().GetPos().x);
				int yp1 = static_cast<int>(15.f + Chara->GetMove().GetPos().z);

				float rad = std::atan2f(
					Vector3DX::Cross((Chara->GetEyeMatrix().zvec() * -1.f), Vector3DX::forward()).y,
					Vector3DX::Dot((Chara->GetEyeMatrix().zvec() * -1.f), Vector3DX::forward())
				);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 216);
				this->m_BackGround->GetMapGraph().DrawRotaGraph3(
					xp + static_cast<int>(((float)xp1*std::cos(rad) - (float)yp1 * std::sin(rad))*size),
					yp + static_cast<int>(((float)yp1*std::cos(rad) + (float)xp1 * std::sin(rad))*size),
					this->m_BackGround->GetMapGraphXSize() / 2,
					this->m_BackGround->GetMapGraphYSize() / 2,
					size, -size,
					rad, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

				Vector3DX MyPos = Chara->GetMove().GetPos();
				for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
					if (index == 0) { continue; }
					if (!c->IsAlive()) { continue; }
					if (!c->CanLookTarget) { continue; }
					int xp2 = xp1 + static_cast<int>(15.f + c->GetMove().GetPos().x);
					int yp2 = yp1 - static_cast<int>(15.f + c->GetMove().GetPos().z);
					DrawCircle(
						xp + static_cast<int>(((float)xp2 * std::cos(rad) - (float)yp2 * std::sin(rad))*size),
						yp + static_cast<int>(((float)yp2 * std::cos(rad) + (float)xp2 * std::sin(rad))*size),
						5, Red, TRUE);
				}
				//アイテム入手
				{
					auto* ObjMngr = ObjectManager::Instance();
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj((int)ObjType::ItemObj, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<ItemObjClass>&)(*ammo);
							if (a->IsActive()) {
								int xp2 = xp1 + static_cast<int>(15.f + a->GetMove().GetPos().x);
								int yp2 = yp1 - static_cast<int>(15.f + a->GetMove().GetPos().z);
								DrawCircle(
									xp + static_cast<int>(((float)xp2 * std::cos(rad) - (float)yp2 * std::sin(rad))*size),
									yp + static_cast<int>(((float)yp2 * std::cos(rad) + (float)xp2 * std::sin(rad))*size),
									2, Yellow, TRUE);

							}
						}
						else {
							break;
						}
						loop++;
					}
				}
				{
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0)->GetChara();
					int xp2 = xp1 + static_cast<int>(15.f + c->GetMove().GetPos().x);
					int yp2 = yp1 - static_cast<int>(15.f + c->GetMove().GetPos().z);
					DrawCircle(
						xp + static_cast<int>(((float)xp2 * std::cos(rad) - (float)yp2 * std::sin(rad))*size),
						yp + static_cast<int>(((float)yp2 * std::cos(rad) + (float)xp2 * std::sin(rad))*size),
						5, Green, TRUE);
				}
			}
		}
		//UI
		void			MAINLOOP::DrawSoundGraph(void) noexcept {
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			if (DrawParts->IsPause()) { return; }
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID())->GetChara();

			int xp = DrawParts->GetUIY(960);
			int yp = DrawParts->GetUIY(540);

			int xp1 = -static_cast<int>(Chara->GetMove().GetPos().x);
			int yp1 = static_cast<int>(Chara->GetMove().GetPos().z);

			float rad = std::atan2f(
				Vector3DX::Cross((Chara->GetEyeMatrix().zvec() * -1.f), Vector3DX::forward()).y,
				Vector3DX::Dot((Chara->GetEyeMatrix().zvec() * -1.f), Vector3DX::forward())
			);

			Vector3DX MyPos = Chara->GetMove().GetPos();
			const int DegDiv = 360 / 5;
			std::array<std::pair<float, int>, DegDiv> DegPers;
			for (auto& d : DegPers) {
				d.first = 0.f;
				d.second = 0;
			}
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
				if (index == 0) { continue; }
				if (!c->IsAlive()) { continue; }
				float length = (MyPos - c->GetMove().GetPos()).magnitude() / (50.f*Scale_Rate);
				float xp2 = static_cast<float>(xp1 + static_cast<int>(c->GetMove().GetPos().x));
				float yp2 = static_cast<float>(yp1 - static_cast<int>(c->GetMove().GetPos().z));
				float len = std::hypotf(xp2, yp2);
				if (len > 0.f) {
					xp2 = (xp2 / len);
					yp2 = (yp2 / len);
				}
				float xp3 = (xp2 * std::cos(rad) - yp2 * std::sin(rad));
				float yp3 = (yp2 * std::cos(rad) + xp2 * std::sin(rad));
				int rad_3 = (((int)rad2deg(std::atan2f(yp3, xp3)) + 360) % 360) * DegDiv / 360;
				for (int loop = -5; loop <= 5; loop++) {
					int rad_2 = rad_3 + loop;
					if (rad_2 < 0) {
						rad_2 += DegDiv;
					}
					if (rad_2 > DegDiv - 1) {
						rad_2 -= DegDiv;
					}
					DegPers.at(rad_2).first += GetRandf(c->GetSoundPower() * 0.1f / length) * static_cast<float>(5 - std::abs(loop)) / 5.f;
					DegPers.at(rad_2).second++;
				}
			}
			for (auto& d : DegPers) {
				if (d.second > 0) {
					d.first = 1.f + d.first / d.second + GetRandf(0.01f);
				}
				else {
					d.first = 1.f + GetRandf(0.01f);
				}
			}
			//
			WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 128);
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
				if (index == 0) { continue; }
				if (!c->IsAlive()) { continue; }
				if (c->CanLookTarget) {
					float length = std::max(1.f, 0.5f / std::max((MyPos - c->GetMove().GetPos()).magnitude() / (100.f*Scale_Rate), 0.1f));

					float xp2 = static_cast<float>(xp1 + static_cast<int>(c->GetMove().GetPos().x));
					float yp2 = static_cast<float>(yp1 - static_cast<int>(c->GetMove().GetPos().z));
					float xp3 = (xp2 * std::cos(rad) - yp2 * std::sin(rad));
					float yp3 = (yp2 * std::cos(rad) + xp2 * std::sin(rad));
					int rad_3 = (((int)rad2deg(std::atan2f(yp3, xp3)) + 360) % 360);

					int rad_2 = rad_3 - 10;
					int rad_4 = rad_3 + 10;

					float Len = DrawParts->GetUIY(180) - (float)DrawParts->GetUIY(10) *length;
					//static_cast<int>(std::max(1.f, 3.f / length))
					DrawTriangle(
						xp + static_cast<int>(cos(deg2rad(rad_2))*(Len)),
						yp + static_cast<int>(sin(deg2rad(rad_2))*(Len)),
						xp + static_cast<int>(cos(deg2rad(rad_3))*(DrawParts->GetUIY(180))),
						yp + static_cast<int>(sin(deg2rad(rad_3))*(DrawParts->GetUIY(180))),
						xp + static_cast<int>(cos(deg2rad(rad_3))*(Len)),
						yp + static_cast<int>(sin(deg2rad(rad_3))*(Len)),
						GetColor(255, std::clamp(255 - static_cast<int>(25.f*length), 0, 255), 0), TRUE);
					DrawTriangle(
						xp + static_cast<int>(cos(deg2rad(rad_3))*(Len)),
						yp + static_cast<int>(sin(deg2rad(rad_3))*(Len)),
						xp + static_cast<int>(cos(deg2rad(rad_3))*(DrawParts->GetUIY(180))),
						yp + static_cast<int>(sin(deg2rad(rad_3))*(DrawParts->GetUIY(180))),
						xp + static_cast<int>(cos(deg2rad(rad_4))*(Len)),
						yp + static_cast<int>(sin(deg2rad(rad_4))*(Len)),
						GetColor(255, std::clamp(255 - static_cast<int>(25.f*length), 0, 255), 0), TRUE);
				}
			}
			for (int index = 0; index < DegDiv; index++) {
				int next = (index + 1) % DegDiv;
				DrawLine(
					xp + DrawParts->GetUIY(static_cast<int>(cos(deg2rad(index * 360 / DegDiv))*(DegPers[index].first*200.f))),
					yp + DrawParts->GetUIY(static_cast<int>(sin(deg2rad(index * 360 / DegDiv))*(DegPers[index].first*200.f))),
					xp + DrawParts->GetUIY(static_cast<int>(cos(deg2rad(next * 360 / DegDiv))*(DegPers[next].first*200.f))),
					yp + DrawParts->GetUIY(static_cast<int>(sin(deg2rad(next * 360 / DegDiv))*(DegPers[next].first*200.f))),
					White,
					3);
			}
			WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
		}
		void			MAINLOOP::DrawHitGraph(void) const noexcept {
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			int loop = 0;
			while (true) {
				auto ammo = ObjMngr->GetObj((int)ObjType::Ammo, loop);
				if (ammo != nullptr) {
					auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
					if (a->m_IsDrawHitUI && a->GetShootedID() == 0) {
						int			Alpha = static_cast<int>(a->m_Hit_alpha * 255.f);
						Vector3DX	DispPos = a->m_Hit_DispPos;
						if ((Alpha >= 10) && (DispPos.z >= 0.f && DispPos.z <= 1.f)) {
							DispPos = DispPos *((float)DrawParts->GetUIY(1080) / (float)DrawParts->GetScreenY(1080));
							WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, Alpha);
							//
							int r = static_cast<int>(255 * std::clamp((float)a->m_Damage / 100.f*2.f, 0.f, 1.f));
							int g = 255 - r;
							if (a->m_Damage > 0) {
								WindowParts->SetBright(WindowSystem::DrawLayer::Normal, r, g, 0);
								WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &hit_Graph, (int)DispPos.x, (int)DispPos.y, (float)DrawParts->GetUIY(static_cast<int>((float)Alpha / 255.f * 0.5f * 100.0f)) / 100.f, 0.f, true);
							}
							if (a->m_ArmerDamage > 0) {
								WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 128, 128, 128);
								WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &guard_Graph, (int)DispPos.x, (int)DispPos.y, (float)DrawParts->GetUIY(static_cast<int>((float)Alpha / 255.f * 0.5f * 100.0f)) / 100.f, 0.f, true);
							}
							WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							//
							if (a->m_Damage > 0) {
								WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
									DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
									(int)DispPos.x + a->m_Hit_AddX, (int)DispPos.y + a->m_Hit_AddY, GetColor(r, g, 0), Black, "%d", a->m_Damage);
							}
							//防いだダメージ
							if (a->m_ArmerDamage > 0) {
								WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
									DrawParts->GetUIY(20), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
									(int)DispPos.x + a->m_Hit_AddX - DrawParts->GetUIY(10), (int)DispPos.y + a->m_Hit_AddY, Gray50, Black, "%d", a->m_ArmerDamage);
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
		void			MAINLOOP::DrawResult(void) noexcept {
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
#if FALSE
			WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &movie, 0, 0, DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), FALSE);
#else
			WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), Gray75, TRUE);
#endif
			auto DrawScore = [&](int xp, int yp, bool isFront) {
				unsigned int Color = isFront ? White : Gray50;
				{
					int sel = 0;
					auto& r = m_ResultFlip[sel];
					if (r.m_Flip > 0.f) {
						WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f*r.m_Flip), 0, 255));
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM,
																				  xp + DrawParts->GetUIY(static_cast<int>(8.f * sel)), yp + DrawParts->GetUIY(static_cast<int>(64.f * sel - r.m_Flip * 128.f)), Color, Black, "Hits : ");

						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(static_cast<int>(48.f + 4.f* r.m_Up)), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM,
																				  xp + DrawParts->GetUIY(static_cast<int>(112 + 8.f * sel)), yp + DrawParts->GetUIY(static_cast<int>(64.f * sel - r.m_Flip * 128.f - r.m_Up * 15.f)), Color, Black, "%5.2f %%", r.m_Point / 100.f);
					}
				}
				{
					int sel = 1;
					auto& r = m_ResultFlip[sel];
					if (r.m_Flip > 0.f) {
						WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f*r.m_Flip), 0, 255));
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM,
																				  xp + DrawParts->GetUIY(static_cast<int>(8.f * sel)), yp + DrawParts->GetUIY(static_cast<int>(64.f * sel - r.m_Flip * 128.f)), Color, Black, "Kill : ");

						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(static_cast<int>(48.f + 4.f* r.m_Up)), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM,
																				  xp + DrawParts->GetUIY(static_cast<int>(112 + 8.f * sel)), yp + DrawParts->GetUIY(static_cast<int>(64.f * sel - r.m_Flip * 128.f - r.m_Up * 15.f)), Color, Black, "%4d", (int)r.m_Point);
					}
				}
				{
					int sel = 2;
					auto& r = m_ResultFlip[sel];
					if (r.m_Flip > 0.f) {
						WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f*r.m_Flip), 0, 255));
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM,
																				  xp + DrawParts->GetUIY(static_cast<int>(8.f * sel)), yp + DrawParts->GetUIY(static_cast<int>(64.f * sel - r.m_Flip * 128.f)), Color, Black, "Score : ");

						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(static_cast<int>(48.f + 4.f* r.m_Up)), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM,
																				  xp + DrawParts->GetUIY(static_cast<int>(112 + 8.f * sel)), yp + DrawParts->GetUIY(static_cast<int>(64.f * sel - r.m_Flip * 128.f - r.m_Up * 15.f)), Color, Black, "%4d pts", (int)r.m_Point);
					}
				}
				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			};

			DrawScore(DrawParts->GetUIY(400 + 8), DrawParts->GetUIY(512 + 8), false);
			DrawScore(DrawParts->GetUIY(400), DrawParts->GetUIY(512), true);

			int xp1, yp1;
			{
				int size = 20;
				xp1 = DrawParts->GetUIY(static_cast<int>(1440.f + 1920.f * m_ResultRankingPer));
				yp1 = DrawParts->GetUIY(540) - DrawParts->GetUIY(36) * (size / 2);

				unsigned int Color = White;
				for (int i = 0;i < size;i++) {
					Color = White;
					if (i == m_ResultRank) {
						if (m_ResultRankDrawTime > 0.5f) {
							Color = Red;
						}
						else {
							Color = Yellow;
						}
					}

					WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
						DrawParts->GetUIY(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1 - DrawParts->GetUIY(300), yp1, Color, Black, "%02d", i + 1);
					if (i < m_Ranking.size()) {
						auto& Now = m_Ranking.at(i);

						std::time_t t = Now.second;
						std::tm now;
						_localtime64_s(&now, &t);
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "%04d/%02d/%02d %02d:%02d : ", 1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min);
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "%4d pts", Now.first);
					}
					else {
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "----/--/-- --:-- : ");
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "---- pts");
					}
					if (i == m_ResultRank && m_ResultRank == 0) {
						if (m_ResultRankingPer < 1.f) {
							WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f*(1.f - m_ResultRankingPer)*2.f), 0, 255));
							WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
								DrawParts->GetUIY(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
																				   DrawParts->GetUIY(1440) - DrawParts->GetUIY(340), yp1, Color, Black, "HIGH SCORE!");
							WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}
					}
					yp1 += DrawParts->GetUIY(36);
				}
				{
					if (m_ResultRank >= size) {
						if (m_ResultRankDrawTime > 0.5f) {
							Color = Red;
						}
						else {
							Color = Yellow;
						}
						std::time_t t = m_StartTime;
						std::tm now;
						_localtime64_s(&now, &t);
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "%04d/%02d/%02d %02d:%02d : ", 1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min);
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "%4d pts", PlayerMngr->GetPlayer(GetMyPlayerID())->GetScore());
					}
					else {
						Color = White;
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "----/--/-- --:-- : ");
						WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
							DrawParts->GetUIY(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "---- pts");
					}
				}
			}

			ButtonSel.Draw();

			float per = (1.f - (16.f / 9.f) / 2.35f) / 2.f;
			WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIY(1920), static_cast<int>(DrawParts->GetUIY(1080) * per), Black, TRUE);
			WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, DrawParts->GetUIY(1080) - static_cast<int>(DrawParts->GetUIY(1080) * per), DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), Black, TRUE);
			if (m_EndTimer >= 0) {
				DrawBlackOut((1.f - m_EndTimer) * 2.f);
			}
		}
		void			MAINLOOP::DrawBlackOut(float per) noexcept {
			auto* WindowParts = WindowSystem::DrawControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			if (per > 0.f) {
				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f*per), 0, 255));
				WindowParts->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, DrawParts->GetUIY(1920), DrawParts->GetUIY(1080), Black, TRUE);
				WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
	};
};
