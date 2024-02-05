#include	"MainScene.hpp"
#include "../sub.hpp"
#include "../ObjectManager.hpp"
#include "../MainScene/Player/Player.hpp"
#include "../MainScene/Player/CPU.hpp"
#include "../MainScene/Object/Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			MAINLOOP::Load_Sub(void) noexcept {
			//ロード
			if (m_IsFirstLoad) {
				m_IsFirstLoad = false;
				auto* PlayerMngr = PlayerManager::Instance();
				//BG
				GunAnimManager::Instance()->Load("data/CharaAnime/");
				//
				LoadSE();
				//
				this->m_AICtrl.resize(Chara_num);
				for (int i = 1; i < Chara_num; i++) {
					this->m_AICtrl[i] = std::make_shared<AIControl>();
				}
				//
				this->m_UIclass.Load();
				this->hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				this->m_MiniMapScreen = GraphHandle::Make(y_r(128) * 2, y_r(128) * 2, true);
				PlayerMngr->Init(Chara_num);
				for (int i = 1; i < Chara_num; i++) {
					LoadChara("Soldier", (PlayerID)i, true, (i == 1));
				}

				for (int i = 1; i < gun_num; i++) {
					LoadGun("AR15_90", (PlayerID)i, false, 0);
				}
			}
		}
		void			MAINLOOP::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			//
			SetAmbientLight(VECTOR_ref::vget(-0.8f, -0.5f, -0.1f), GetColorF(0.92f, 0.91f, 0.90f, 0.0f));
			SetFarShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f));
			SetMiddleShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f));
			SetNearShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f));

			this->m_BackGround = std::make_shared<BackGroundClassMain>();
			this->m_BackGround->Init("", "");//1.59秒
			//ロード
			LoadChara("Suit", GetMyPlayerID(), false);
			GunsModify::LoadSlots("data/bokuzyo.ok");//プリセット読み込み
			LoadGun("G17Gen3", GetMyPlayerID(), true, 0);
			LoadGun("Mod870", GetMyPlayerID(), false, 1);
			//BGをオブジェに登録
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				c->SetMapCol(this->m_BackGround);
				c->GetGunPtr(0)->SetMapCol(this->m_BackGround);
				if (c->GetGunPtr(1)) {
					c->GetGunPtr(1)->SetMapCol(this->m_BackGround);
				}
			}
			//人の座標設定
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				VECTOR_ref pos_t;
				float rad_t = 0.f;
				if (index == 0) {
					pos_t = VECTOR_ref::vget(0.f, 0.f, 0.f);
					rad_t = deg2rad(GetRandf(180.f));
				}
				else {
					auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
					auto TgtPos_XZ = TargetChara->GetMove().pos; TgtPos_XZ.y(0.f);
					VECTOR_ref BGPos_XZ;
					while (true) {
						auto& C = this->m_BackGround->GetBuildDatas().at(GetRand((int)(this->m_BackGround->GetBuildDatas().size()) - 1));
						BGPos_XZ = C.GetMatrix().pos(); BGPos_XZ.y(0.f);
						if ((BGPos_XZ - TgtPos_XZ).Length() > 10.f*Scale_Rate) {
							break;
						}
					}
					pos_t = BGPos_XZ;
					rad_t = deg2rad(GetRandf(180.f));
				}

				auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
				if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
				c->ValueSet(deg2rad(0.f), rad_t, pos_t, (PlayerID)index);
				if (index == 0) {
					c->SetCharaType(CharaTypeID::Team);
				}
				else {
					c->SetCharaType(CharaTypeID::Enemy);
					this->m_AICtrl[index]->Init(this->m_BackGround, (PlayerID)index);
				}
			}
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
			//サウンド
			SetSE();
			//UI
			this->m_UIclass.Set();
			//入力
			this->m_DamageEvents.clear();
			m_NetWorkBrowser.Init();
			m_ReadyTimer = 6.f;
			m_Timer = 180.f;
			//
			m_MainLoopPauseControl.Init();
			m_ConcussionControl.Init();
			/*
			auto P = GetNowHiPerformanceCount();
			LogFileFmtAdd("End2[%f]", (float)(GetNowHiPerformanceCount() - P) / 1000.f / 1000.f);
			P = GetNowHiPerformanceCount();
			//*/
		}
		bool			MAINLOOP::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			Pad->ChangeGuide(
				[&]() {
				auto* KeyGuide = PadControl::Instance();
				if (DXDraw::Instance()->IsPause()) {
					KeyGuide->AddGuide(PADS::INTERACT, "決定");
					KeyGuide->AddGuide(PADS::RELOAD, "戻る");
					KeyGuide->AddGuide(PADS::MOVE_W, "");
					KeyGuide->AddGuide(PADS::MOVE_S, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, "選択");
				}
				else {
					KeyGuide->AddGuide(PADS::MOVE_W, "");
					KeyGuide->AddGuide(PADS::MOVE_S, "");
					KeyGuide->AddGuide(PADS::MOVE_A, "");
					KeyGuide->AddGuide(PADS::MOVE_D, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, "移動");

					KeyGuide->AddGuide(PADS::LEAN_L, "");
					KeyGuide->AddGuide(PADS::LEAN_R, "左右傾け(リーン)");
					KeyGuide->AddGuide(PADS::RUN, "走る");
					KeyGuide->AddGuide(PADS::SQUAT, "しゃがみ");
					KeyGuide->AddGuide(PADS::RELOAD, "リロード");
					KeyGuide->AddGuide(PADS::SHOT, "射撃");
					KeyGuide->AddGuide(PADS::INTERACT, "武器切替");
					KeyGuide->AddGuide(PADS::AIM, "エイム");
					KeyGuide->AddGuide(PADS::INVENTORY, "ポーズ");
					KeyGuide->AddGuide(PADS::MELEE, "近接攻撃");
				}	
			});

			if (DXDraw::Instance()->IsPause()) {
				m_MainLoopPauseControl.Execute();
				return true;
			}
			else {
				m_MainLoopPauseControl.Reset();
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("update start");
#endif // DEBUG
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
#ifdef DEBUG
			//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			//FirstDoingv
			if (GetIsFirstLoop()) {
				SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
				m_ReadyTimer = 6.f;
				SE->Get((int)SoundEnum::Env).Play(0, DX_PLAYTYPE_LOOP);
				SE->Get((int)SoundEnum::Env2).Play(0, DX_PLAYTYPE_LOOP);
			}
			else {
				m_ReadyTimer = std::max(m_ReadyTimer - 1.f / FPS, 0.f);
				if (m_ReadyTimer == 0.f) {
					m_Timer = std::max(m_Timer - 1.f / FPS, 0.f);
				}
			}
			{
				/*
				m_GameEnd |= (!(Chara->IsAlive() && (Timer <= TotalTime)));
				if (m_GameEnd) {
					EndTimer = std::max(EndTimer - 1.f / FPS, 0.f);
				}
				//*/
			}
			//Input,AI
			{
				float cam_per = ((DrawParts->GetMainCamera().GetCamFov() / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
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
					pp_x -= Chara->GetGunPtrNow()->GetRecoilRadAdd().y();
					pp_y -= Chara->GetGunPtrNow()->GetRecoilRadAdd().x();
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
				//ネットワーク
				auto& CharaPtr = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();

				moves tmpmove;
				tmpmove.pos = CharaPtr->GetMove().pos;
				tmpmove.vec = CharaPtr->GetMove().vec;
				tmpmove.vec.y(0);
				tmpmove.rad = CharaPtr->GetRadBuf();

				m_NetWorkBrowser.FirstExecute(MyInput, tmpmove, CharaPtr->GetDamageEvent());
				//クライアント
				if (m_NetWorkBrowser.GetClient()) {
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
				}
				//
				bool isready = (m_ReadyTimer == 0.f);
				for (int index = 0; index < Player_num; index++) {
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
					if (m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
						auto tmp = this->m_NetWorkBrowser.GetNowServerPlayerData(index, false);
						if (index == GetMyPlayerID()) {
							MyInput.SetKeyInputFlags(tmp.Input);//キーフレームだけサーバーに合わせる
							c->SetInput(MyInput, isready);
							m_NetWorkBrowser.GetRecvData(index, tmp.GetFrame());
						}
						else {
							if (!m_NetWorkBrowser.GetClient()) {
								m_AICtrl[index]->Execute(&tmp.Input);
							}
							c->SetInput(tmp.Input, isready);
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
							c->SetInput(MyInput, isready);
						}
						else {
							InputControl OtherInput;
							m_AICtrl[index]->Execute(&OtherInput);//めっちゃ重い
							c->SetInput(OtherInput, isready);
						}
						//ダメージイベント処理
						for (auto& e : c->GetDamageEvent()) {
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
			//Execute
			ObjMngr->ExecuteObject();
			//弾の更新
			{
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
								is_HitAll |= tgt->CheckAmmoHit(a.get(), repos_tmp, &pos_tmp);
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
					SE->Get((int)SoundEnum::Tank_near).Play_3D(0, DrawParts->GetMainCamera().GetCamPos(), 10.f*Scale_Rate, 255);//, DX_PLAYTYPE_LOOP
				}
				Set_is_Blackout(m_ConcussionControl.IsActive());
				Set_Per_Blackout(m_ConcussionControl.GetPer());
				m_ConcussionControl.Update();
			}
			//近接攻撃
			{
				for (int index = 0; index < Player_num; index++) {
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
					if (c->GetShotPhase() == GunAnimeID::Melee) {
						VECTOR_ref StartPos = c->GetEyePosition();
						VECTOR_ref EndPos = StartPos + c->GetEyeVector() * (1.f*Scale_Rate);
						for (int index2 = 0; index2 < Player_num; index2++) {
							if (index == index2) { continue; }
							auto& tgt = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index2).GetChara();
							if (tgt->GetHP() <= 0) { continue; }
							if (tgt->IsDamaging()) { continue; }
							tgt->CheckMeleeHit(c->GetMyPlayerID(), StartPos, &EndPos);
						}
					}
				}
			}
			//背景更新
			this->m_BackGround->FirstExecute();
			ObjMngr->LateExecuteObject();
			//視点
			{
				{
					//FPSカメラ
					VECTOR_ref CamPos = Chara->GetEyePosition() + DrawParts->GetCamShake();
					DrawParts->SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeVector(), Chara->GetEyeVecY());
					//info
					auto fov_t = DrawParts->GetMainCamera().GetCamFov();
					auto near_t = DrawParts->GetMainCamera().GetCamNear();
					auto far_t = DrawParts->GetMainCamera().GetCamFar();
					if (Chara->GetIsADS()) {
						Easing(&near_t, Scale_Rate * 0.01f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 60.f, 0.5f, EasingType::OutExpo);
					}
					else {
						Easing(&near_t, Scale_Rate * 0.01f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 30.f, 0.9f, EasingType::OutExpo);
					}
					//fov
					{
						float fov = deg2rad(OptionParts->Get_Fov());
						if (Chara->GetIsADS()) {
							fov -= deg2rad(15);
						}
						else if (Chara->GetRun()) {
							fov += deg2rad(5);
						}
						if (Chara->GetMeleeSwitch()) {
							fov += deg2rad(25);
							Easing(&fov_t, fov, 0.8f, EasingType::OutExpo);
						}
						else if (Chara->GetGunPtrNow() && Chara->GetGunPtrNow()->GetShotSwitch()) {
							fov -= deg2rad(15);
							Easing(&fov_t, fov, 0.5f, EasingType::OutExpo);
						}
						else {
							Easing(&fov_t, fov, 0.8f, EasingType::OutExpo);
						}
					}
					DrawParts->SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
					//DoF
					if (Chara->GetIsADS()) {
						PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t - 0.1f, Scale_Rate * 0.3f, far_t);
					}
					else {
						PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t / 2, near_t, far_t);
					}
				}
			}
			{
				auto Len = (DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(0)).Length();
				auto LenPer = std::clamp(Len / (5.f*Scale_Rate), 0.f, 1.f);
				bool HPLow = Chara->GetHP() < (Chara->GetHPMax() * 3 / 10);
				Easing(&Min, 
					(HPLow) ? Lerp(20.f, 35.f, LenPer) : Lerp(0.f, 25.f, LenPer),
					0.95f, EasingType::OutExpo);
				Easing(&Gamma, 
					(HPLow) ? Lerp(1.15f, 1.35f, LenPer) : Lerp(1.f, 1.15f, LenPer),
					0.95f, EasingType::OutExpo);

				PostPassEffect::Instance()->SetLevelFilter((int)Min, (HPLow) ? 128 : 192, Gamma);
			}
			//
			this->m_BackGround->Execute();
			//射撃光
			if (Chara->GetGunPtrNow() && Chara->GetGunPtrNow()->GetShotSwitch()) {// 
				auto mat = Chara->GetGunPtrNow()->GetFrameWorldMat(GunFrame::Muzzle);
				SetLightEnable(TRUE);
				ChangeLightTypePoint(mat.pos().get(),
					4.0f*Scale_Rate,
					0.001f,
					0.012f,
					0.004f);
			}
			SetShadowDir(VECTOR_ref::vget(0.f, -1.f, 0.f), 0);
			//SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(0)).Norm(), 0);
			//SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(1)).Norm(), 1);
			//SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(2)).Norm(), 2);
			//SetisUpdateFarShadow(true);
			//レーザーサイト
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				if (!c->GetGunPtrNow()) { continue; }
				if (!c->GetGunPtrNow()->HasFrame(GunFrame::LaserSight)) {
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
#ifdef DEBUG
			DebugParts->SetPoint("---");
#endif // DEBUG
			//UIパラメーター
			{
				VECTOR_ref MyPos = Chara->GetMove().pos;
				for (int index = 0; index < Chara_num; index++) {
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
					if (index == 0) { continue; }
					c->CanLookTarget = true;
					{
						VECTOR_ref TgtPos = c->GetMove().pos;
						//
						c->CanLookTarget = true;
						auto Dir_t = TgtPos - MyPos;
						if (Dir_t.Length() < 15.f*Scale_Rate) {
							for (auto& C : this->m_BackGround->GetBuildDatas()) {
								if (C.GetMeshSel() < 0) { continue; }
								auto StartPos = MyPos;
								auto EndPos = TgtPos;
								if (GetMinLenSegmentToPoint(StartPos, EndPos, C.GetMatrix().pos()) >= 5.f*Scale_Rate) { continue; }
								auto ret = C.GetColLine(StartPos, EndPos);
								if (ret.HitFlag == TRUE) {
									c->CanLookTarget = false;
									break;
								}
							}
						}
						else {
							c->CanLookTarget = false;
						}
					}
				}
			}
			{
				//シェイク
				this->m_UIclass.SetIntParam(0, (int)(DrawParts->GetCamShake().x()*100.f));
				this->m_UIclass.SetIntParam(1, (int)(DrawParts->GetCamShake().y()*100.f));
				this->m_UIclass.SetIntParam(2, (int)(rad2deg(Chara->GetGunRadAdd())*5.f));
				//Time
				this->m_UIclass.SetfloatParam(0, m_Timer);
				this->m_UIclass.SetfloatParam(1, m_ReadyTimer);
				//Score
				this->m_UIclass.SetIntParam(6, PlayerMngr->GetPlayer(GetMyPlayerID()).GetScore());
				//HP
				this->m_UIclass.SetGaugeParam(0, (int)Chara->GetHP(), (int)Chara->GetHPMax());
				//SPeed,ALT
				if (Chara->GetGunPtrNow()) {
					this->m_UIclass.SetGaugeParam(1, (int)100, (int)100);
					this->m_UIclass.SetGaugeParam(2, (int)Chara->GetGunPtrNow()->GetAmmoNum(), (int)Chara->GetGunPtrNow()->GetAmmoAll() + 1);
				}
			}
			//
			EffectControl::Execute();
			//
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				VECTOR_ref campos; campos.z(-1.f);
				c->SetCameraPosition(campos);
			}
			//ミニマップ
			SetDrawMiniMap();
#ifdef DEBUG
			DebugParts->SetPoint("update end");
#endif // DEBUG
			if (m_MainLoopPauseControl.GetIsRetireSelected()) {
				return false;
			}
			return true;
		}
		void			MAINLOOP::Dispose_Sub(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

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
		}
		//
		void			MAINLOOP::BG_Draw_Sub(void) noexcept {
			this->m_BackGround->BG_Draw();
		}
		void			MAINLOOP::ShadowDraw_Sub(void) noexcept {
			this->m_BackGround->Shadow_Draw();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			MAINLOOP::MainDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			SetFogStartEnd(Scale_Rate * 1200.f, Scale_Rate * 1600.f);

			ObjMngr->DrawObject();
			this->m_BackGround->Draw();
			//レーザー
			for (int index = 0; index < Chara_num; index++) {
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index).GetChara();
				c->DrawLaser();
			}

			//ObjMngr->DrawDepthObject();
			//シェーダー描画用パラメーターセット
			{
				//
				if (Chara->GetGunPtrNow() && Chara->GetGunPtrNow()->GetReticlePtr()) {
					m_MyPlayerReticleControl.Update(
						Chara->GetGunPtrNow()->GetLensPos(),
						Chara->GetGunPtrNow()->GetFrameWorldMat(GunFrame::LensSize).pos(),
						Chara->GetGunPtrNow()->GetReticlePos()
					);
					/*
					if (m_MyPlayerReticleControl.IsActive()) {
						Set_is_lens(false);
						Set_xp_lens(m_MyPlayerReticleControl.GetLensXPos());
						Set_yp_lens(m_MyPlayerReticleControl.GetLensYPos());
						Set_size_lens(m_MyPlayerReticleControl.GetLensSize());
					}
					//*/
				}
				else {
					m_MyPlayerReticleControl.SetActiveOff();
					Set_is_lens(false);
				}
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
			if (false)
			{
				for (int i = 1; i < Chara_num; i++) {
					m_AICtrl[i]->Draw();
				}
			}
			//
#endif
		}
		//UI表示
		void			MAINLOOP::DrawUI_Base_Sub(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			//着弾表示
			DrawHitGraph();
			if (!DXDraw::Instance()->IsPause()) {
				DrawSoundGraph();
			}
			//レティクル表示
			if (m_MyPlayerReticleControl.IsActive()) {
				Chara->GetGunPtrNow()->GetReticlePtr()->DrawRotaGraph(
					(int)m_MyPlayerReticleControl.GetReticleXPos(),
					(int)m_MyPlayerReticleControl.GetReticleYPos(),
					1.f, Chara->GetGunRadAdd(), true);
			}
			//UI
			this->m_UIclass.Draw();
			//通信設定
			/*
			if (DXDraw::Instance()->IsPause()) {
				m_NetWorkBrowser.Draw();
			}
			//*/
		}
		void			MAINLOOP::DrawUI_In_Sub(void) noexcept {
			//ポーズ
			if (DXDraw::Instance()->IsPause()) {
				m_MainLoopPauseControl.Draw();
			}
			else {
				//ミニマップ
				m_MiniMapScreen.DrawRotaGraph(y_r(960), y_r(840), 1.f, 0.f, true);
			}
		}
		//使い回しオブジェ系
		void			MAINLOOP::Dispose_Load(void) noexcept {
			if (!m_IsFirstLoad) {
				m_IsFirstLoad = true;
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				DisposeSE();
				m_AICtrl.clear();
				this->m_UIclass.Dispose();
				this->hit_Graph.Dispose();
				this->m_MiniMapScreen.Dispose();
				PlayerMngr->Dispose();
				ObjMngr->DisposeObject();
			}
		}
		//ミニマップ
		void			MAINLOOP::SetDrawMiniMap(void) noexcept {
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
					Chara->GetEyeVector().cross(VECTOR_ref::front()).y(),
					Chara->GetEyeVector().dot(VECTOR_ref::front())
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
					if (!c->CanLookTarget) { continue; }
					unsigned int Color = (c->GetHP() > 0) ? GetColor(255, 0, 0) : GetColor(128, 0, 0);
					int xp2 = xp1 + (int)(15.f + c->GetMove().pos.x());
					int yp2 = yp1 - (int)(15.f + c->GetMove().pos.z());
					DrawCircle(
						xp + (int)(((float)xp2 * std::cos(rad) - (float)yp2 * std::sin(rad))*size),
						yp + (int)(((float)yp2 * std::cos(rad) + (float)xp2 * std::sin(rad))*size),
						5, Color, TRUE);
				}
				{
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
					unsigned int Color = (c->GetHP() > 0) ? GetColor(0, 255, 0) : GetColor(0, 128, 0);
					int xp2 = xp1 + (int)(15.f + c->GetMove().pos.x());
					int yp2 = yp1 - (int)(15.f + c->GetMove().pos.z());
					DrawCircle(
						xp + (int)(((float)xp2 * std::cos(rad) - (float)yp2 * std::sin(rad))*size),
						yp + (int)(((float)yp2 * std::cos(rad) + (float)xp2 * std::sin(rad))*size),
						5, Color, TRUE);
				}
			}
		}
		void			MAINLOOP::DrawSoundGraph(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();

			int xp = y_r(960);
			int yp = y_r(540);

			int xp1 = -(int)(Chara->GetMove().pos.x());
			int yp1 = (int)(Chara->GetMove().pos.z());

			float rad = std::atan2f(
				Chara->GetEyeVector().cross(VECTOR_ref::front()).y(),
				Chara->GetEyeVector().dot(VECTOR_ref::front())
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
				//if (!c->CanLookTarget) { continue; }
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
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
			for (auto& d : DegPers) {
				if (d.second > 0) {
					d.first = 1.f + d.first / d.second + GetRandf(0.01f);
				}
				else {
					d.first = 1.f + GetRandf(0.01f);
				}
			}
			for (int index = 0; index < DegDiv; index++) {
				int next = (index + 1) % DegDiv;
				DrawLine(
					xp + (int)(cos(deg2rad(index * 360 / DegDiv))*(DegPers[index].first*200.f)),
					yp + (int)(sin(deg2rad(index * 360 / DegDiv))*(DegPers[index].first*200.f)),
					xp + (int)(cos(deg2rad(next * 360 / DegDiv))*(DegPers[next].first*200.f)),
					yp + (int)(sin(deg2rad(next * 360 / DegDiv))*(DegPers[next].first*200.f)),
					GetColor(255, 255, 255),
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
						int			Damage = (int)a->m_Damage;
						VECTOR_ref	DispPos = a->m_Hit_DispPos;
						if ((Alpha >= 10) && (DispPos.z() >= 0.f && DispPos.z() <= 1.f)) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
							//
							int r = (int)(255 * std::clamp((float)Damage / 100.f*2.f, 0.f, 1.f));
							int g = (int)(255 * std::clamp(1.f - (float)Damage / 100.f*2.f, 0.f, 1.f));
							SetDrawBright(r, g, 0);
							hit_Graph.DrawRotaGraph((int)DispPos.x(), (int)DispPos.y(), (float)y_r((float)Alpha / 255.f * 0.5f * 100.0f) / 100.f, 0.f, true);
							SetDrawBright(255, 255, 255);
							//
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
								(int)DispPos.x() + a->m_Hit_AddX, (int)DispPos.y() + a->m_Hit_AddY, GetColor(r, g, 0), GetColor(0, 0, 0), "%d", Damage);
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
		//SE
		void			MAINLOOP::LoadSE(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::Env, 1, "data/Sound/SE/envi.wav", false);
			SE->Add((int)SoundEnum::Env2, 1, "data/Sound/SE/envi2.wav", false);

			SE->Add((int)SoundEnum::CartFall, 6, "data/Sound/SE/gun/case.wav", false);
			SE->Add((int)SoundEnum::MagFall, 6, "data/Sound/SE/gun/ModFall.wav", false);
			SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
			for (int i = 0; i < 6; i++) {
				SE->Add((int)SoundEnum::Cocking1_0 + i, 4, "data/Sound/SE/gun/autoM870/" + std::to_string(i) + ".wav");
				SE->Add((int)SoundEnum::Cocking2_0 + i, 4, "data/Sound/SE/gun/autoM16/" + std::to_string(i) + ".wav");
				SE->Add((int)SoundEnum::Cocking3_0 + i, 4, "data/Sound/SE/gun/auto1911/" + std::to_string(i) + ".wav");
			}
			SE->Add((int)SoundEnum::StandUp, 1, "data/Sound/SE/move/sliding.wav", false);
			SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add((int)SoundEnum::SlideFoot, 3, "data/Sound/SE/move/sliding.wav");
			SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
			SE->Add((int)SoundEnum::Heart, 3, "data/Sound/SE/move/heart.wav");
			SE->Add((int)SoundEnum::Hit, 3, "data/Sound/SE/hit.wav");
			for (int i = 0; i < 5; i++) {
				SE->Add((int)SoundEnum::HitGround0 + i, 2, "data/Sound/SE/gun/HitGround/" + std::to_string(i + 1) + ".wav");
			}
			
			for (int i = 0; i < 6; i++) {
				SE->Add((int)SoundEnum::Man_Hurt1 + i, 2, "data/Sound/SE/voice/hurt_0" + std::to_string(i + 1) + ".wav");
			}
			for (int i = 0; i < 8; i++) {
				SE->Add((int)SoundEnum::Man_Death1 + i, 2, "data/Sound/SE/voice/death_0" + std::to_string(i + 1) + ".wav");
			}
			SE->Add((int)SoundEnum::Man_contact, 10, "data/Sound/SE/voice/contact.wav");
			SE->Add((int)SoundEnum::Man_openfire, 10, "data/Sound/SE/voice/openfire.wav");
			SE->Add((int)SoundEnum::Man_reload, 10, "data/Sound/SE/voice/reload.wav");
			SE->Add((int)SoundEnum::Man_takecover, 10, "data/Sound/SE/voice/takecover.wav");
			SE->Add((int)SoundEnum::Man_teamdown, 10, "data/Sound/SE/voice/teamdown.wav");


			SE->Add((int)SoundEnum::Tank_near, 5, "data/Sound/SE/near.wav");
		}
		void			MAINLOOP::SetSE(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Get((int)SoundEnum::CartFall).SetVol_Local(48);
			SE->Get((int)SoundEnum::MagFall).SetVol_Local(48);
			SE->Get((int)SoundEnum::Trigger).SetVol_Local(48);
			SE->Get((int)SoundEnum::Shot2).SetVol_Local(216);
			SE->Get((int)SoundEnum::Shot3).SetVol_Local(216);
			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
			SE->Get((int)SoundEnum::Hit).SetVol_Local(255);
			for (int i = 0; i < 5; i++) {
				SE->Get((int)SoundEnum::HitGround0 + i).SetVol_Local(92);
			}
		}
		void			MAINLOOP::DisposeSE(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete((int)SoundEnum::CartFall);
			SE->Delete((int)SoundEnum::MagFall);
			SE->Delete((int)SoundEnum::Env);
			SE->Delete((int)SoundEnum::Env2);
			SE->Delete((int)SoundEnum::StandUp);
			SE->Delete((int)SoundEnum::Trigger);
			for (int i = 0; i < 6; i++) {
				SE->Delete((int)SoundEnum::Cocking1_0 + i);
				SE->Delete((int)SoundEnum::Cocking2_0 + i);
				SE->Delete((int)SoundEnum::Cocking3_0 + i);
			}
			SE->Delete((int)SoundEnum::RunFoot);
			SE->Delete((int)SoundEnum::SlideFoot);
			SE->Delete((int)SoundEnum::StandupFoot);
			SE->Delete((int)SoundEnum::Heart);
			SE->Delete((int)SoundEnum::Hit);
			for (int i = 0; i < 5; i++) {
				SE->Delete((int)SoundEnum::HitGround0 + i);
			}
			for (int i = 0; i < 6; i++) {
				SE->Delete((int)SoundEnum::Man_Hurt1 + i);
			}
			for (int i = 0; i < 8; i++) {
				SE->Delete((int)SoundEnum::Man_Death1 + i);
			}
			SE->Delete((int)SoundEnum::Man_contact);
			SE->Delete((int)SoundEnum::Man_openfire);
			SE->Delete((int)SoundEnum::Man_reload);
			SE->Delete((int)SoundEnum::Man_takecover);
			SE->Delete((int)SoundEnum::Man_teamdown);

			SE->Delete((int)SoundEnum::Tank_near);
		}
		//
		void			MAINLOOP::LoadChara(const std::string&FolderName, PlayerID ID, bool IsRagDoll, bool IsRagDollBaseObj) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* Ptr = ObjMngr->MakeObject(ObjType::Human);
			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";
			ObjMngr->LoadObjectModel((*Ptr).get(), Path.c_str());
			MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());
			if (IsRagDoll) {
				auto& c = (std::shared_ptr<CharacterClass>&)(*Ptr);
				if (IsRagDollBaseObj) {
					MV1::Load((c->GetFilePath() + "model_Rag.mv1").c_str(), &c->GetRagDoll(), DX_LOADMODEL_PHYSICS_REALTIME, -1.f);//身体ラグドール
					MV1::SetAnime(&c->GetRagDoll(), c->GetObj());
				}
				else {
					auto& Base = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1).GetChara();
					c->GetRagDoll() = Base->GetRagDoll().Duplicate();
					MV1::SetAnime(&c->GetRagDoll(), c->GetObj());
				}
			}
			(*Ptr)->Init();
			PlayerMngr->GetPlayer(ID).SetChara(*Ptr);
		}
		void			MAINLOOP::LoadGun(const std::string&FolderName, PlayerID ID, bool IsPreset, int Sel) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			auto* Ptr = ObjMngr->MakeObject(ObjType::Gun);
			std::string Path = "data/gun/";
			Path += FolderName;
			Path += "/";
			ObjMngr->LoadObjectModel((*Ptr).get(), Path.c_str());
			MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());
			auto& Gun = ((std::shared_ptr<GunClass>&)(*Ptr));
			auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(ID).GetChara();
			c->SetGunPtr(Sel, Gun);
			(*Ptr)->Init();
			GunsModify::CreateSelData(Gun, IsPreset);
			Gun->Init_Gun();
		}
	};
};
