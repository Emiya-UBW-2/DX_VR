#include	"MainScene.hpp"
#include "../sub.hpp"
#include "../ObjectManager.hpp"
#include "../MainScene/Player/Player.hpp"
#include "../MainScene/Player/CPU.hpp"
#include "../MainScene/Object/Character.hpp"
#include <ctime>

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			MAINLOOP::Load_Sub(void) noexcept {
			//ロード
			if (m_IsFirstLoad) {
				m_IsFirstLoad = false;
				auto* PlayerMngr = PlayerManager::Instance();
				auto* BattleResourceMngr = CommonBattleResource::Instance();
				//BG
				GunAnimManager::Instance()->Load("data/CharaAnime/");
				//
				BattleResourceMngr->Load();
				//
				this->m_AICtrl.resize(Chara_num);
				for (int i = 1; i < Chara_num; i++) {
					this->m_AICtrl[i] = std::make_shared<AIControl>();
				}
				//
				this->m_UIclass.Load();
				this->hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				this->guard_Graph = GraphHandle::Load("data/UI/battle_guard.bmp");
				this->m_MiniMapScreen = GraphHandle::Make(y_r(128) * 2, y_r(128) * 2, true);
				PlayerMngr->Init(Chara_num);
				for (int i = 1; i < Chara_num; i++) {
					BattleResourceMngr->LoadChara("Soldier", (PlayerID)i);
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i).GetChara();
					if (i == 1) {
						MV1::Load((c->GetFilePath() + "model_Rag.mv1").c_str(), &c->GetRagDoll(), DX_LOADMODEL_PHYSICS_REALTIME, -1.f);//身体ラグドール
						MV1::SetAnime(&c->GetRagDoll(), c->GetObj());
					}
					else {
						auto& Base = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1).GetChara();
						c->GetRagDoll() = Base->GetRagDoll().Duplicate();
						MV1::SetAnime(&c->GetRagDoll(), c->GetObj());
					}
					c->Init_RagDollControl(c->GetObj());
				}

				std::string GunName;
				for (int i = 1; i < gun_num; i++) {
					if (!m_IsHardMode) {
						GunName = "PCC_4";
					}
					else {
						GunName = ULT_GUNName[GetRand((int)ULT_GUN::Max - 1)];
					}
					LoadGun(GunName.c_str(), (PlayerID)i, false, 0);
				}
			}
		}
		void			MAINLOOP::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			//
			VECTOR_ref LightVec = VECTOR_ref::vget(0.f, -1.f, 0.f);
			DrawParts->SetAmbientLight(LightVec, GetColorF(0.92f, 0.91f, 0.90f, 0.0f));
			DrawParts->SetIsUpdateShadow(0, false);
			DrawParts->SetShadow(LightVec, VECTOR_ref::vget(-5.f, -3.f, -5.f)*Scale_Rate, VECTOR_ref::vget(5.f, 0.5f, 5.f)*Scale_Rate, 0);
			DrawParts->SetShadow(LightVec, VECTOR_ref::vget(-5.f, -3.f, -5.f)*Scale_Rate, VECTOR_ref::vget(5.f, 0.5f, 5.f)*Scale_Rate, 1);
			DrawParts->SetShadow(LightVec, VECTOR_ref::vget(-10.f, -3.f, -10.f)*Scale_Rate, VECTOR_ref::vget(10.f, 0.f, 10.f)*Scale_Rate, 2);

			this->m_BackGround = std::make_shared<BackGroundClassMain>();
			this->m_BackGround->Init("", "");//1.59秒
			//ロード
			BattleResourceMngr->LoadChara("Suit", GetMyPlayerID());
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
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
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				c->SetMapCol(this->m_BackGround);
			}
			//人の座標設定
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				VECTOR_ref pos_t;
				float rad_t = 0.f;
				if (index == 0) {
					pos_t = VECTOR_ref::vget(0.f, 0.f, 0.f);
					VECTOR_ref BGPos_XZ;
					for (auto& C : this->m_BackGround->GetBuildData()) {
						if (C.GetMeshSel() < 0) { continue; }
						BGPos_XZ = C.GetMatrix().pos(); BGPos_XZ.y(0.f);
						if (BGPos_XZ.Length() < 5.f*Scale_Rate) {
							pos_t = BGPos_XZ;
							break;
						}
					}
					rad_t = deg2rad(GetRandf(180.f));
				}
				else {
					auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
					auto TgtPos_XZ = TargetChara->GetMove().pos; TgtPos_XZ.y(0.f);
					VECTOR_ref BGPos_XZ;
					while (true) {
						auto& C = this->m_BackGround->GetBuildData().at(GetRand((int)(this->m_BackGround->GetBuildData().size()) - 1));
						BGPos_XZ = C.GetMatrix().pos(); BGPos_XZ.y(0.f);
						if ((BGPos_XZ - TgtPos_XZ).Length() > 10.f*Scale_Rate) {
							auto StartPos = TgtPos_XZ + VECTOR_ref::up()*(1.f*Scale_Rate);
							auto EndPos = BGPos_XZ + VECTOR_ref::up()*(1.f*Scale_Rate);
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
				VECTOR_ref EndPos = pos_t + VECTOR_ref::up() * 10.f*Scale_Rate;
				if (this->m_BackGround->CheckLinetoMap(pos_t + VECTOR_ref::up() * -10.f*Scale_Rate, &EndPos, false)) {
					pos_t = EndPos;
				}
				*/

				c->ValueSet((PlayerID)index, true, (index == 0) ? CharaTypeID::Team : CharaTypeID::Enemy);
				c->MovePoint(deg2rad(0.f), rad_t, pos_t, (index == 0) ? 1 : 0);
				c->Heal(100, true);
				if (index != 0) {
					this->m_AICtrl[index]->Init(this->m_BackGround, (PlayerID)index);
				}
			}
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
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
			m_NetWorkBrowser.Init();
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

			m_SelectBackImage = GraphHandle::Load("data/UI/select.png");
			ButtonSel.LoadCommon(&m_SelectBackImage);

			ButtonSel.Load_Icon("data/UI/Right.png", true);
			ButtonSel.Set(y_r(960), y_r(840), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE);

			ButtonSel.SetNone();
			//開始時刻
			m_StartTime = std::time(nullptr);
		}
		bool			MAINLOOP::Update_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* BGM = BGMPool::Instance();
			auto* ItemLogParts = GetItemLog::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();

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
							KeyGuide->AddGuide(PADS::INVENTORY, LocalizePool::Instance()->Get(9913));
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
				return true;
			}
			else {
				m_MainLoopPauseControl.Reset();
				if (!m_IsEnd) {
					if (m_LastMan == 0 || (m_Timer <= 0.f)) {
						if (m_PreEndTimer < 0.f) {
							m_PreEndTimer = 5.f;
							ItemLogParts->AddLog(3.f, GetColor(25, 122, 75), "%s +%4d", LocalizePool::Instance()->Get(202), 100);
							PlayerMngr->GetPlayer(GetMyPlayerID()).AddScore(100);
							if (m_LastMan == 0) {
								ItemLogParts->AddLog(3.f, GetColor(25, 122, 75), "%s +%4d", LocalizePool::Instance()->Get(203), 100);
								PlayerMngr->GetPlayer(GetMyPlayerID()).AddScore(100);
							}
						}
						else {
							m_PreEndTimer = std::max(m_PreEndTimer - 1.f / FPS, 0.f);
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
				SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
				m_ReadyTimer = 6.f;
				SE->Get((int)SoundEnum::Env).Play(0, DX_PLAYTYPE_LOOP);
				SE->Get((int)SoundEnum::Env2).Play(0, DX_PLAYTYPE_LOOP);
				if (!m_IsHardMode) {
					ItemLogParts->AddLog(3.f, GetColor(251, 91, 1), LocalizePool::Instance()->Get(204));
				}
				else {
					ItemLogParts->AddLog(3.f, GetColor(251, 91, 1), LocalizePool::Instance()->Get(205));
					m_Timer = 0.001f;
				}
			}
			else {
				m_ReadyTimer = std::max(m_ReadyTimer - 1.f / FPS, 0.f);
				if (m_ReadyTimer == 0.f && Chara->IsAlive()) {
					if (!m_IsHardMode) {
						m_Timer = std::max(m_Timer - 1.f / FPS, 0.f);
					}
					else {
						m_Timer = m_Timer + 1.f / FPS;
					}
				}
				if (m_ReadyTimer>0.f) {
					BGM->Get(0).SetVol_Local((int)(255.f*m_ReadyTimer / 6.f));
				}
				else {
					BGM->Get(0).Stop();
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
				auto* ItemLogParts = FPS_n2::GetItemLog::Instance();

				//クリア
				if (m_IsEnd) {
					//ハードモードアンロック
					if (SaveDataParts->GetParam("UnlockHardMode") != 1) {
						SaveDataParts->SetParam("UnlockHardMode", 1);
						ItemLogParts->AddLog(10.f, GetColor(50, 255, 50), "%s が開放されました！", "Fire Party モード");
					}
				}
				//
				if (!m_MainLoopPauseControl.GetIsRetireSelected()) {
					int bases = std::clamp(static_cast<int>(SaveDataParts->GetParam("ULT Unlock")), 1, (int)ULT_GUN::Max);
					SaveDataParts->SetParam("ULT Unlock", std::clamp(bases + 1, 1, (int)ULT_GUN::Max));
					if (bases != static_cast<int>(SaveDataParts->GetParam("ULT Unlock"))) {
						ItemLogParts->AddLog(10.f, GetColor(50, 255, 50), "%s が開放されました！", ULT_GUNName[SaveDataParts->GetParam("ULT Unlock") - 1]);
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
									ItemLogParts->AddLog(10.f, GetColor(50, 255, 50), "%s が開放されました！", "サプレッサーキット");
								}
								break;
							case 2:
								{
									SaveDataParts->SetParam("G-High IronSight", 1);
									SaveDataParts->SetParam("Glock17 Gen2 Frame", 1);
									SaveDataParts->SetParam("SBal PL LaserSight", 1);
									ItemLogParts->AddLog(10.f, GetColor(50, 255, 50), "%s が開放されました！", "スポーターキット");
								}
								break;
							case 3:
								{
									SaveDataParts->SetParam("FMG-9 Carbine Kit", 1);
									SaveDataParts->SetParam("Glock18 Slide (for 17)", 1);
									SaveDataParts->SetParam("ACOG TA31 Scope", 1);
									ItemLogParts->AddLog(10.f, GetColor(50, 255, 50), "%s が開放されました！", "FMG-9キット");
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
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			BGM->Get(0).Stop();

			SE->Get((int)SoundEnum::Env).StopAll(0);
			SE->Get((int)SoundEnum::Env2).StopAll(0);
			//使い回しオブジェ系
			auto* Ptr = &PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			ObjMngr->DelObj(Ptr);
			PlayerMngr->GetPlayer(GetMyPlayerID()).Dispose();
			this->m_BackGround->Dispose();
			this->m_BackGround.reset();
			GunsModify::DisposeSlots();
			//
			m_NetWorkBrowser.Dispose();
			EffectControl::Dispose();

			{
				auto* DrawParts = DXDraw::Instance();
				PostPassEffect::Instance()->SetLevelFilter(0, 255, 1.f);
				DrawParts->SetAberrationPower(1.f);
				DrawParts->SetIsUpdateShadow(0, true);
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
		void			MAINLOOP::ShadowDraw_NearFar_Sub(void) noexcept {
			if (m_IsEnd) {
				return;
			}
			this->m_BackGround->Shadow_Draw();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			MAINLOOP::ShadowDraw_Sub(void) noexcept {
			if (m_IsEnd) {
				return;
			}
			this->m_BackGround->Shadow_Draw();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
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
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			SetFogStartEnd(Scale_Rate * 1200.f, Scale_Rate * 1600.f);

			this->m_BackGround->Draw();
			ObjMngr->DrawObject();
			//レーザー
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				c->DrawLaser();
			}

			//ObjMngr->DrawDepthObject();
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
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				if (c->GetMyPlayerID() == GetMyPlayerID()) { continue; }
				//auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
				auto pos = c->GetMove().pos + c->GetMove().mat.zvec()*-1.f * 5.f*Scale_Rate;
				VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
				if (0.f < campos.z() && campos.z() < 1.f) {
					c->SetCameraPosition(campos);
					c->SetCameraSize(std::max(20.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
				}
			}
#ifdef DEBUG
			if (false) {
				for (int i = 1; i < Chara_num; i++) {
					m_AICtrl[i]->Draw();
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
			if (m_PreEndTimer != -1.f) {				return;			}
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			if (!Chara->IsAlive()) { return; }
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->Get_EX_UI()) {
				DrawHitGraph();			//着弾表示
			}
			if (OptionParts->Get_EX_UI2()) {
				DrawSoundGraph();		//サウンド表示
			}
			//レティクル表示
			if (Chara->GetGunPtrNow()) {
				if (Chara->GetGunPtrNow()->IsActiveReticle() && Chara->GetGunPtrNow()->GetSightPtr() &&
					!((Chara->GetADSPer() < 0.8f) && Chara->GetSightZoomSize() > 1.f)) {
					(*Chara->GetGunPtrNow()->GetSightPtr())->GetModData()->GetReitcleGraph().DrawRotaGraph(
						(int)Chara->GetGunPtrNow()->GetReticleXPos(),
						(int)Chara->GetGunPtrNow()->GetReticleYPos(),
						1.f, Chara->GetLeanRad(), true);
				}
			}
			//UI
			if (!DrawParts->IsPause()) {
				this->m_UIclass.Draw();
			}
			//通信設定
			/*
			if (DrawParts->IsPause()) {
				m_NetWorkBrowser.Draw();
			}
			//*/

		}
		void			MAINLOOP::DrawUI_In_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();

			if (m_PreEndTimer > 0) {
				DrawBlackOut((1.f - std::max(m_PreEndTimer - 2.f, 0.f)) * 2.f);
			}
			DrawBlackOut(m_DeathPer * 1.3f);

			if (m_IsEnd) {
				DrawResult();
			}
			else if (m_PreEndTimer == -1.f) {
				if (!DrawParts->IsPause()) {
					auto* PlayerMngr = PlayerManager::Instance();
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					if (Chara->IsAlive()) {
						//ミニマップ
						m_MiniMapScreen.DrawRotaGraph(y_r(960), y_r(840), 1.f, 0.f, true);
					}
				}
			}
			//ポーズ
			if (DrawParts->IsPause() && (!DrawParts->IsExit() && !DrawParts->IsRestart())) {
				m_MainLoopPauseControl.Draw();
			}
		}
		//使い回しオブジェ系
		void			MAINLOOP::Dispose_Load_Sub(void) noexcept {
			if (!m_IsFirstLoad) {
				m_IsFirstLoad = true;
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto* BattleResourceMngr = CommonBattleResource::Instance();
				BattleResourceMngr->Dispose();
				m_AICtrl.clear();
				this->m_UIclass.Dispose();
				this->hit_Graph.Dispose();
				this->guard_Graph.Dispose();
				this->m_MiniMapScreen.Dispose();
				PlayerMngr->Dispose();
				ObjMngr->DisposeObject();
			}
		}
		//load
		void			MAINLOOP::LoadGun(const std::string&FolderName, PlayerID ID, bool IsPreset, int Sel) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->LoadGun(FolderName, ID, Sel);
			auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(ID).GetChara();
			auto& g = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(ID).GetGun(Sel);
			c->SetGunPtr(Sel, g);
			GunsModify::CreateSelData(c->GetGunPtr(Sel), IsPreset);
			c->GetGunPtr(Sel)->Init_Gun();
		}
		//start
		void			MAINLOOP::StartUIParam(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
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
			movie = GraphHandle::Load("data/Movie/end0.mp4");
			PlayMovieToGraph(movie.get());
			ChangeMovieVolumeToGraph(std::clamp((int)(255.f*OptionParts->Get_SE()), 0, 255), movie.get());
			m_movieTotalFrame = GetMovieTotalFrameToGraph(movie.get());
#endif
			PadControl::Instance()->SetGuideUpdate();

			auto* SaveDataParts = SaveDataClass::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			{
				//自記録を追加
				int ScoreNum = std::max(static_cast<int>(SaveDataParts->GetParam("ScoreNum")), 0);
				SaveDataParts->SetParam("ScoreNum", ScoreNum + 1);
				SaveDataParts->SetParam("Score" + std::to_string(ScoreNum), PlayerMngr->GetPlayer(GetMyPlayerID()).GetScore());
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
					if (r.first == PlayerMngr->GetPlayer(GetMyPlayerID()).GetScore() && r.second == m_StartTime) {
						m_ResultRank = (int)(&r - &m_Ranking.front());
						break;
					}
				}
				//スコアの記録を並べ替え
				{
					for (auto& r : m_Ranking) {
						int index = (int)(&r - &m_Ranking.front());
						SaveDataParts->SetParam("Score" + std::to_string(index), r.first);
						SaveDataParts->SetParam("Time_" + std::to_string(index), r.second);
					}
				}
				SaveDataParts->SetParam("ScoreNum", std::min(ScoreNum, 20));
			}
		}
		//update
		bool			MAINLOOP::UpdateResult(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
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
				m_EndTimer = std::max(m_EndTimer - 1.f / FPS, 0.f);
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
					(PlayerMngr->GetPlayer(GetMyPlayerID()).GetShot() > 0) ? 100 * 100 * PlayerMngr->GetPlayer(GetMyPlayerID()).GetHit() / PlayerMngr->GetPlayer(GetMyPlayerID()).GetShot() : 0,
					m_ResultPhase != 0);
				m_ResultFlip.at(1).Update(PlayerMngr->GetPlayer(GetMyPlayerID()).GetKill(), m_ResultPhase != 0);
				if (m_ResultFlip.at(2).Update(PlayerMngr->GetPlayer(GetMyPlayerID()).GetScore(), m_ResultPhase != 0)) {
					if (m_ResultRankingPer > 0.5f) {
						m_ResultRankingPer = std::max(m_ResultRankingPer - 1.f / FPS, 0.f);
					}
					else {
						Easing(&m_ResultRankingPer, 0.f, 0.95f, EasingType::OutExpo);
					}
					if (m_ResultRankingPer < 0.1f) {
						m_ResultPhase = 1;
					}

					m_ResultRankDrawTime += 1.f / FPS;
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
				m_EndTimer = std::max(m_EndTimer - 1.f / FPS, 0.f);
			}
#endif
			ButtonSel.Update();
			return (m_EndTimer == 0.f);
		}
		void			MAINLOOP::UpdateInput(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* ItemLogParts = GetItemLog::Instance();
			auto* Pad = PadControl::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();

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
				pp_x -= Chara->GetRecoilRadAdd().y();
				pp_y -= Chara->GetRecoilRadAdd().x();
			}
			MyInput.SetInputStart(pp_x, pp_y, Chara->GetRadBuf());
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
			auto& CharaPtr = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();

			moves tmpmove;
			tmpmove.pos = CharaPtr->GetMove().pos;
			tmpmove.vec = CharaPtr->GetMove().vec;
			tmpmove.vec.y(0);
			tmpmove.rad = CharaPtr->GetRadBuf();

			//m_NetWorkBrowser.FirstExecute(MyInput, tmpmove, CharaPtr->GetDamageEvent());
			//クライアント
			if (m_NetWorkBrowser.GetClient()) {
				/*
				for (int index = 0; index < Chara_num; index++) {
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
					if (c->GetMyPlayerID() == GetMyPlayerID()) {
						c->SetCharaType(CharaTypeID::Team);
					}
					else {
						if (c->GetMyPlayerID() < 1) {
							c->SetCharaType(CharaTypeID::Team);
						}
						else {
							c->SetCharaType(CharaTypeID::Enemy);
						}
					}
				}
				//*/
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
							ItemLogParts->AddLog(3.f, GetColor(251, 91, 1), LocalizePool::Instance()->Get(206));
						}
						m_LastMan = 0;
						for (int index = 0; index < Player_num; index++) {
							if (index != GetMyPlayerID()) {
								if (!m_AICtrl[index]->CannotRepop()) {
									m_LastMan++;
								}
							}
						}
						if (m_LastMan != prevLastMan) {
							ItemLogParts->AddLog(3.f, GetColor(251, 91, 1), LocalizePool::Instance()->Get(207), m_LastMan);
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
			bool isready = (m_ReadyTimer == 0.f) && (m_PreEndTimer == -1.f);
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
							m_AICtrl[index]->Execute(&tmp.Input, (!m_IsHardMode && (m_Timer > 60.f)) || m_IsHardMode);
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
						m_AICtrl[index]->Execute(&OtherInput, (!m_IsHardMode && (m_Timer > 60.f)) || m_IsHardMode);
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
			for (int index = 0; index < Chara_num; index++) {
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
		void			MAINLOOP::UpdateBullet(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();

			int loop = 0;
			while (true) {
				auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
				if (ammo != nullptr) {
					auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
					if (a->IsActive()) {
						//AmmoClass
						VECTOR_ref repos_tmp = a->GetMove().repos;
						VECTOR_ref pos_tmp = a->GetMove().pos;

						if (GetMyPlayerID() != a->GetShootedID()) {
							if (GetMinLenSegmentToPoint(repos_tmp, pos_tmp, DrawParts->GetMainCamera().GetCamPos()) < 1.f*Scale_Rate) {
								m_ConcussionControl.SetActive();
							}
						}

						VECTOR_ref norm_tmp;
						auto ColResGround = this->m_BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, true, &norm_tmp);
						bool is_HitAll = false;
						for (int index = 0; index < Chara_num; index++) {
							auto& tgt = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
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
		void			MAINLOOP::UpdateItem(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* ItemLogParts = GetItemLog::Instance();

			int loop = 0;
			while (true) {
				auto ammo = ObjMngr->GetObj(ObjType::ItemObj, loop);
				if (ammo != nullptr) {
					auto& a = (std::shared_ptr<ItemObjClass>&)(*ammo);
					if (a->IsActive()) {
						VECTOR_ref pos_tmp = a->GetMove().pos;
						bool isHit = false;
						for (int index = 0; index < Chara_num; index++) {
							if (index != 0) { break; }
							auto& tgt = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
							float Len = (tgt->GetMove().pos - pos_tmp).Length();
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
											ItemLogParts->AddLog(3.f, GetColor(183, 143, 0), "%s +%d", LocalizePool::Instance()->Get(208), Add);
										}
										break;
									case ItemType::ARMER:
										if (tgt->GetArmer()) {
											isHit = true;
											SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, tgt->GetEyeMatrix().pos(), Scale_Rate * 10.f);
											ItemLogParts->AddLog(3.f, GetColor(183, 143, 0), LocalizePool::Instance()->Get(209));
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
							a->SetIsDelete(true);
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
			for (int index = 0; index < Player_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				if (c->GetCharaAction() == CharaActionID::Melee) {
					VECTOR_ref StartPos = c->GetEyeMatrix().pos();
					VECTOR_ref EndPos = StartPos + c->GetEyeMatrix().zvec() * (-1.f*Scale_Rate);
					for (int index2 = 0; index2 < Player_num; index2++) {
						if (index == index2) { continue; }
						auto& tgt = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index2).GetChara();
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
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			{
				{
					//FPSカメラ
					if (Chara->IsAlive()) {
						VECTOR_ref CamPos = Chara->GetEyeMatrix().pos() + DrawParts->GetCamShake();
						DrawParts->SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeMatrix().zvec() * -1.f, Chara->GetEyeMatrix().yvec());
						m_DeathCamYAdd = -0.2f;
					}
					else {
						VECTOR_ref CamPos = DrawParts->SetMainCamera().GetCamPos();
						VECTOR_ref CamVec = DrawParts->SetMainCamera().GetCamVec() - CamPos;
						VECTOR_ref CamUp = DrawParts->SetMainCamera().GetCamUp();
						CamPos.yadd(m_DeathCamYAdd);
						if (std::abs(m_DeathCamYAdd) > 0.01f) {
							m_DeathCamYAdd += (M_GR / (FPS * FPS)) / 2.f;
						}
						else {
							m_DeathCamYAdd = 0.f;
						}
						if (CamPos.y() < 0.1f) {
							CamPos.y(0.1f);
							m_DeathCamYAdd *= -0.2f;
						}
						float y = CamVec.y();
						Easing(&y, 0.9f, 0.95f, EasingType::OutExpo);
						CamVec.y(y);

						float x = CamUp.y();
						Easing(&y, 0.2f, 0.95f, EasingType::OutExpo);
						CamUp.x(x);
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
						float fov = deg2rad(OptionParts->Get_Fov());
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
				auto Len = (DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(0)).Length();
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
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			if (Chara->GetGunPtrNow() && Chara->GetGunPtrNow()->GetShotSwitch()) {// 
#if HIGH_FPS_ROM
#else
				auto mat = Chara->GetGunPtrNow()->GetFrameWorldMat(GunFrame::Muzzle);
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
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				if (!c->GetGunPtrNow()) { continue; }
				if (!c->GetGunPtrNow()->HasFrame(GunFrame::LaserSight) || !c->GetGunPtrNow()->IsActive()) {
					c->SetIsLaserActive(false);
					continue;
				}
				//
				auto mat = c->GetGunPtrNow()->GetFrameWorldMat(GunFrame::LaserSight);
				VECTOR_ref StartPos = mat.pos();
				VECTOR_ref EndPos = StartPos + mat.zvec()*-1.f * 15.f*Scale_Rate;
				this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, true);
				for (int index2 = 0; index2 < Chara_num; index2++) {
					auto& c2 = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index2).GetChara();
					if (c2->GetMyPlayerID() == c->GetMyPlayerID()) { continue; }
					if (!c2->IsAlive()) { continue; }
					if (!(GetMinLenSegmentToPoint(StartPos, EndPos, c2->GetMove().pos) <= 2.0f*Scale_Rate)) { continue; }
					c2->CheckLineHitNearest(StartPos, &EndPos);
				}
				auto Vec = (EndPos - StartPos);
				EndPos = StartPos + Vec.Norm()*std::max(Vec.Length() - 0.3f*Scale_Rate, 0.f);
				c->SetLaserStartPos(StartPos);
				c->SetLaserEndPos(EndPos);
				c->SetIsLaserActive(true);
			}
		}
		void			MAINLOOP::UpdateUIParam(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* ItemLogParts = GetItemLog::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			{
				VECTOR_ref StartPos = Chara->GetEyeMatrix().pos();
				for (int index = 0; index < Chara_num; index++) {
					if (index == 0) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
					VECTOR_ref TgtPos = c->GetEyeMatrix().pos();
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
				this->m_UIclass.SetIntParam(0, (int)(DrawParts->GetCamShake().x()*100.f));
				this->m_UIclass.SetIntParam(1, (int)(DrawParts->GetCamShake().y()*100.f));
				this->m_UIclass.SetIntParam(2, (int)(rad2deg(Chara->GetLeanRad()*5.f)));
				//AmmoStock
				this->m_UIclass.SetIntParam(3, Chara->GetAmmoStock());
				//Time
				this->m_UIclass.SetfloatParam(0, m_Timer);
				this->m_UIclass.SetfloatParam(1, m_ReadyTimer);
				//
				this->m_UIclass.SetGaugeParam(5 + 3, (int)(Chara->GetAutoAimPer()*20.f), 20);
				this->m_UIclass.SetGaugeParam(5 + 3 + 1, (int)(Chara->GetAutoAimPer()*20.f), 20);
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
				auto ScoreBuf = PlayerMngr->GetPlayer(GetMyPlayerID()).GetScore();
				if (ScoreBuf > prevScore && (ScoreBuf - prevScore) >= 100) {
					this->m_UIclass.SetIntParam(5, (ScoreBuf - prevScore));
					ItemLogParts->AddLog(3.f, GetColor(206, 0, 0), "%s +%4d", LocalizePool::Instance()->Get(210), (ScoreBuf - prevScore));
				}
				else {
					this->m_UIclass.SetIntParam(5, 0);
				}
				if (Chara->ULTActiveSwitch()) {
					ItemLogParts->AddLog(3.f, GetColor(251, 91, 1), LocalizePool::Instance()->Get(211));
				}
				prevScore = PlayerMngr->GetPlayer(GetMyPlayerID()).GetScore();
				//
				this->m_UIclass.SetIntParam(6, PlayerMngr->GetPlayer(GetMyPlayerID()).GetScore());

				this->m_UIclass.SetIntParam(8, Chara->GetArmerStock());
				this->m_UIclass.SetIntParam(9, Chara->GetMorphineStock());
			}
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				VECTOR_ref campos; campos.z(-1.f);
				c->SetCameraPosition(campos);
			}
		}
		void			MAINLOOP::UpdateMiniMap(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			m_MiniMapScreen.SetDraw_Screen();
			{
				float size = 0.7f;
				int xp = y_r(128);
				int yp = y_r(128);

				int xp1 = -(int)(15.f + Chara->GetMove().pos.x());
				int yp1 = (int)(15.f + Chara->GetMove().pos.z());

				float rad = std::atan2f(
					(Chara->GetEyeMatrix().zvec() * -1.f).cross(VECTOR_ref::front()).y(),
					(Chara->GetEyeMatrix().zvec() * -1.f).dot(VECTOR_ref::front())
				);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 216);
				this->m_BackGround->GetMapGraph().DrawRotaGraph3(
					xp + (int)(((float)xp1*std::cos(rad) - (float)yp1 * std::sin(rad))*size),
					yp + (int)(((float)yp1*std::cos(rad) + (float)xp1 * std::sin(rad))*size),
					this->m_BackGround->GetMapGraphXSize() / 2,
					this->m_BackGround->GetMapGraphYSize() / 2,
					size, -size,
					rad, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

				VECTOR_ref MyPos = Chara->GetMove().pos;
				for (int index = 0; index < Chara_num; index++) {
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
					if (index == 0) { continue; }
					if (!c->IsAlive()) { continue; }
					if (!c->CanLookTarget) { continue; }
					int xp2 = xp1 + (int)(15.f + c->GetMove().pos.x());
					int yp2 = yp1 - (int)(15.f + c->GetMove().pos.z());
					DrawCircle(
						xp + (int)(((float)xp2 * std::cos(rad) - (float)yp2 * std::sin(rad))*size),
						yp + (int)(((float)yp2 * std::cos(rad) + (float)xp2 * std::sin(rad))*size),
						5, Red, TRUE);
				}
				//アイテム入手
				{
					auto* ObjMngr = ObjectManager::Instance();
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::ItemObj, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<ItemObjClass>&)(*ammo);
							if (a->IsActive()) {
								int xp2 = xp1 + (int)(15.f + a->GetMove().pos.x());
								int yp2 = yp1 - (int)(15.f + a->GetMove().pos.z());
								DrawCircle(
									xp + (int)(((float)xp2 * std::cos(rad) - (float)yp2 * std::sin(rad))*size),
									yp + (int)(((float)yp2 * std::cos(rad) + (float)xp2 * std::sin(rad))*size),
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
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
					int xp2 = xp1 + (int)(15.f + c->GetMove().pos.x());
					int yp2 = yp1 - (int)(15.f + c->GetMove().pos.z());
					DrawCircle(
						xp + (int)(((float)xp2 * std::cos(rad) - (float)yp2 * std::sin(rad))*size),
						yp + (int)(((float)yp2 * std::cos(rad) + (float)xp2 * std::sin(rad))*size),
						5, Green, TRUE);
				}
			}
		}
		//UI
		void			MAINLOOP::DrawSoundGraph(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			if (DrawParts->IsPause()) { return; }
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();

			int xp = y_r(960);
			int yp = y_r(540);

			int xp1 = -(int)(Chara->GetMove().pos.x());
			int yp1 = (int)(Chara->GetMove().pos.z());

			float rad = std::atan2f(
				(Chara->GetEyeMatrix().zvec() * -1.f).cross(VECTOR_ref::front()).y(),
				(Chara->GetEyeMatrix().zvec() * -1.f).dot(VECTOR_ref::front())
			);

			VECTOR_ref MyPos = Chara->GetMove().pos;
			const int DegDiv = 360 / 5;
			std::array<std::pair<float, int>, DegDiv> DegPers;
			for (auto& d : DegPers) {
				d.first = 0.f;
				d.second = 0;
			}
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				if (index == 0) { continue; }
				if (!c->IsAlive()) { continue; }
				float length = (MyPos - c->GetMove().pos).Length() / (50.f*Scale_Rate);
				float xp2 = (float)(xp1 + (int)(c->GetMove().pos.x()));
				float yp2 = (float)(yp1 - (int)(c->GetMove().pos.z()));
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
					DegPers.at(rad_2).first += GetRandf(c->GetSoundPower() * 0.1f / length) * (float)(5 - std::abs(loop)) / 5.f;
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
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				if (index == 0) { continue; }
				if (!c->IsAlive()) { continue; }
				if (c->CanLookTarget) {
					float length = std::max(1.f, 0.5f / std::max((MyPos - c->GetMove().pos).Length() / (100.f*Scale_Rate), 0.1f));

					float xp2 = (float)(xp1 + (int)(c->GetMove().pos.x()));
					float yp2 = (float)(yp1 - (int)(c->GetMove().pos.z()));
					float xp3 = (xp2 * std::cos(rad) - yp2 * std::sin(rad));
					float yp3 = (yp2 * std::cos(rad) + xp2 * std::sin(rad));
					int rad_3 = (((int)rad2deg(std::atan2f(yp3, xp3)) + 360) % 360);

					int rad_2 = rad_3 - 10;
					int rad_4 = rad_3 + 10;

					float Len = y_r(180) - (float)y_r(10) *length;
					//(int)(std::max(1.f, 3.f / length))
					DrawTriangle(
						xp + (int)(cos(deg2rad(rad_2))*(Len)),
						yp + (int)(sin(deg2rad(rad_2))*(Len)),
						xp + (int)(cos(deg2rad(rad_3))*(y_r(180))),
						yp + (int)(sin(deg2rad(rad_3))*(y_r(180))),
						xp + (int)(cos(deg2rad(rad_3))*(Len)),
						yp + (int)(sin(deg2rad(rad_3))*(Len)),
						GetColor(255, std::clamp(255 - (int)(25.f*length), 0, 255), 0), TRUE);
					DrawTriangle(
						xp + (int)(cos(deg2rad(rad_3))*(Len)),
						yp + (int)(sin(deg2rad(rad_3))*(Len)),
						xp + (int)(cos(deg2rad(rad_3))*(y_r(180))),
						yp + (int)(sin(deg2rad(rad_3))*(y_r(180))),
						xp + (int)(cos(deg2rad(rad_4))*(Len)),
						yp + (int)(sin(deg2rad(rad_4))*(Len)),
						GetColor(255, std::clamp(255 - (int)(25.f*length), 0, 255), 0), TRUE);
				}
			}
			for (int index = 0; index < DegDiv; index++) {
				int next = (index + 1) % DegDiv;
				DrawLine_2D(
					xp + y_r(cos(deg2rad(index * 360 / DegDiv))*(DegPers[index].first*200.f)),
					yp + y_r(sin(deg2rad(index * 360 / DegDiv))*(DegPers[index].first*200.f)),
					xp + y_r(cos(deg2rad(next * 360 / DegDiv))*(DegPers[next].first*200.f)),
					yp + y_r(sin(deg2rad(next * 360 / DegDiv))*(DegPers[next].first*200.f)),
					White,
					3);
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		void			MAINLOOP::DrawHitGraph(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* Fonts = FontPool::Instance();
			int loop = 0;
			while (true) {
				auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
				if (ammo != nullptr) {
					auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
					if (a->m_IsDrawHitUI && a->GetShootedID() == 0) {
						int			Alpha = (int)(a->m_Hit_alpha * 255.f);
						VECTOR_ref	DispPos = a->m_Hit_DispPos;
						if ((Alpha >= 10) && (DispPos.z() >= 0.f && DispPos.z() <= 1.f)) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
							//
							int r = (int)(255 * std::clamp((float)a->m_Damage / 100.f*2.f, 0.f, 1.f));
							int g = 255 - r;
							if (a->m_Damage > 0) {
								SetDrawBright(r, g, 0);
								hit_Graph.DrawRotaGraph((int)DispPos.x(), (int)DispPos.y(), (float)y_r((float)Alpha / 255.f * 0.5f * 100.0f) / 100.f, 0.f, true);
							}
							if (a->m_ArmerDamage > 0) {
								SetDrawBright(128, 128, 128);
								guard_Graph.DrawRotaGraph((int)DispPos.x(), (int)DispPos.y(), (float)y_r((float)Alpha / 255.f * 0.5f * 100.0f) / 100.f, 0.f, true);
							}
							SetDrawBright(255, 255, 255);
							//
							if (a->m_Damage > 0) {
								Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
									(int)DispPos.x() + a->m_Hit_AddX, (int)DispPos.y() + a->m_Hit_AddY, GetColor(r, g, 0), Black, "%d", a->m_Damage);
							}
							//防いだダメージ
							if (a->m_ArmerDamage > 0) {
								Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(20), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
									(int)DispPos.x() + a->m_Hit_AddX - y_r(10), (int)DispPos.y() + a->m_Hit_AddY, Gray50, Black, "%d", a->m_ArmerDamage);
							}
						}
					}
				}
				else {
					break;
				}
				loop++;
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		void			MAINLOOP::DrawResult(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
#if FALSE
			movie.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, FALSE);
#else
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Gray75, TRUE);
#endif
			auto DrawScore = [&](int xp, int yp, bool isFront) {
				unsigned int Color = isFront ? White : Gray50;
				{
					int sel = 0;
					auto& r = m_ResultFlip[sel];
					if (r.m_Flip > 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*r.m_Flip), 0, 255));
						Fonts->Get(FontPool::FontType::Nomal_ItalicAA).DrawString(y_r(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM,
																				  xp + y_r(8.f * sel), yp + y_r(64.f * sel - r.m_Flip * 128.f), Color, Black, "Hits : ");

						Fonts->Get(FontPool::FontType::Nomal_ItalicAA).DrawString(y_r(48.f + 4.f* r.m_Up), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM,
																				  xp + y_r(112 + 8.f * sel), yp + y_r(64.f * sel - r.m_Flip * 128.f - r.m_Up * 15.f), Color, Black, "%5.2f %%", r.m_Point / 100.f);
					}
				}
				{
					int sel = 1;
					auto& r = m_ResultFlip[sel];
					if (r.m_Flip > 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*r.m_Flip), 0, 255));
						Fonts->Get(FontPool::FontType::Nomal_ItalicAA).DrawString(y_r(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM,
																				  xp + y_r(8.f * sel), yp + y_r(64.f * sel - r.m_Flip * 128.f), Color, Black, "Kill : ");

						Fonts->Get(FontPool::FontType::Nomal_ItalicAA).DrawString(y_r(48.f + 4.f* r.m_Up), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM,
																				  xp + y_r(112 + 8.f * sel), yp + y_r(64.f * sel - r.m_Flip * 128.f - r.m_Up * 15.f), Color, Black, "%4d", (int)r.m_Point);
					}
				}
				{
					int sel = 2;
					auto& r = m_ResultFlip[sel];
					if (r.m_Flip > 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*r.m_Flip), 0, 255));
						Fonts->Get(FontPool::FontType::Nomal_ItalicAA).DrawString(y_r(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM,
																				  xp + y_r(8.f * sel), yp + y_r(64.f * sel - r.m_Flip * 128.f), Color, Black, "Score : ");

						Fonts->Get(FontPool::FontType::Nomal_ItalicAA).DrawString(y_r(48.f + 4.f* r.m_Up), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM,
																				  xp + y_r(112 + 8.f * sel), yp + y_r(64.f * sel - r.m_Flip * 128.f - r.m_Up * 15.f), Color, Black, "%4d pts", (int)r.m_Point);
					}
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			};

			DrawScore(y_r(400 + 8), y_r(512 + 8), false);
			DrawScore(y_r(400), y_r(512), true);

			int xp1, yp1;
			{
				int size = 20;
				xp1 = y_r(1440.f + 1920.f * m_ResultRankingPer);
				yp1 = y_r(540) - y_r(36) * (size / 2);

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

					Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1 - y_r(300), yp1, Color, Black, "%02d", i + 1);
					if (i < m_Ranking.size()) {
						auto& Now = m_Ranking.at(i);

						std::time_t t = Now.second;
						std::tm now;
						_localtime64_s(&now, &t);
						Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "%04d/%02d/%02d %02d:%02d : ", 1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min);
						Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "%4d pts", Now.first);
					}
					else {
						Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "----/--/-- --:-- : ");
						Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "---- pts");
					}
					if (i == m_ResultRank && m_ResultRank == 0) {
						if (m_ResultRankingPer < 1.f) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*(1.f - m_ResultRankingPer)*2.f), 0, 255));
							Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(y_r(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
																				   y_r(1440) - y_r(340), yp1, Color, Black, "HIGH SCORE!");
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
						}
					}
					yp1 += y_r(36);
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
						Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "%04d/%02d/%02d %02d:%02d : ", 1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min);
						Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "%4d pts", PlayerMngr->GetPlayer(GetMyPlayerID()).GetScore());
					}
					else {
						Color = White;
						Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "----/--/-- --:-- : ");
						Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, Black, "---- pts");
					}
				}
			}

			ButtonSel.Draw();

			float per = (1.f - (16.f / 9.f) / 2.35f) / 2.f;
			DrawBox(0, 0, DrawParts->m_DispXSize, (int)(DrawParts->m_DispYSize * per), Black, TRUE);
			DrawBox(0, DrawParts->m_DispYSize - (int)(DrawParts->m_DispYSize * per), DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
			if (m_EndTimer > 0) {
				DrawBlackOut((1.f - m_EndTimer) * 2.f);
			}
		}
		void			MAINLOOP::DrawBlackOut(float per) noexcept {
			if (per > 0.f) {
				auto* DrawParts = DXDraw::Instance();
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	};
};
