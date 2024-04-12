#include	"TutorialScene.hpp"

#include "../sub.hpp"

#include "../MainScene/Player/Player.hpp"
#include "../MainScene/Object/Target.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const int PosNum = 9;
		const VECTOR_ref TargetPositions[PosNum] = {
			VECTOR_ref::vget(17.39f,0.0f,-7.65f),
			VECTOR_ref::vget(24.16f,0.0f,-7.65f),
			VECTOR_ref::vget(30.28f,0.0f,-7.65f),
			VECTOR_ref::vget(17.39f,0.0f,-3.51f),
			VECTOR_ref::vget(24.16f,0.0f,-3.51f),
			VECTOR_ref::vget(30.28f,0.0f,-3.51f),
			VECTOR_ref::vget(17.39f,0.0f, 0.85f),
			VECTOR_ref::vget(24.16f,0.0f, 0.85f),
			VECTOR_ref::vget(30.28f,0.0f, 0.85f),
		};
		//
		void			TutorialScene::Load_Sub(void) noexcept {
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
				this->m_UIclass.Load();

				PlayerMngr->Init(1);
			}
		}
		void			TutorialScene::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			//
			VECTOR_ref LightVec = VECTOR_ref::vget(-0.1f, -1.f, 0.f);
			DrawParts->SetAmbientLight(LightVec, GetColorF(1.f, 1.f, 1.f, 0.0f));
			DrawParts->SetShadow(LightVec, VECTOR_ref::vget(-10.f, -3.f, -10.f)*Scale_Rate, VECTOR_ref::vget(10.f, 0.5f, 10.f)*Scale_Rate, 0);
			DrawParts->SetShadow(LightVec, VECTOR_ref::vget(-10.f, -3.f, -10.f)*Scale_Rate, VECTOR_ref::vget(10.f, 0.f, 10.f)*Scale_Rate, 1);
			DrawParts->SetShadow(LightVec, VECTOR_ref::vget(-100.f, 0.f, -100.f)*Scale_Rate, VECTOR_ref::vget(100.f, 1.f, 100.f)*Scale_Rate, 2);
			//
			DeleteLightHandleAll();
			SetLightEnable(TRUE);
			ChangeLightTypeDir(LightVec.get());
			SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
			SetLightSpcColor(GetColorF(0.1f, 0.1f, 0.1f, 0.f));
			SetLightAmbColor(GetColorF(0.1f, 0.1f, 0.1f, 1.f));
			//
			this->m_BackGround = std::make_shared<BackGroundClassTutorial>();
			this->m_BackGround->Init("data/model/map/","");
			//ロード
			BattleResourceMngr->LoadChara("Suit", 0);
			GunsModify::LoadSlots("Save/gundata.svf");//プリセット読み込み
			LoadGun("G17Gen3", 0, true, 0);

			std::string ULTName = ULT_GUNName[(int)GunsModify::GetULTSelect()];

			LoadGun(ULTName.c_str(), 0, false, 1);
			//BGをオブジェに登録
			auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
			c->SetMapCol(this->m_BackGround);
			//人の座標設定
			{
				VECTOR_ref pos_t;
				pos_t = VECTOR_ref::vget(0.f, 0.f, Scale_Rate*9.f);

				VECTOR_ref EndPos = pos_t + VECTOR_ref::up() * 10.f*Scale_Rate;
				if (this->m_BackGround->CheckLinetoMap(pos_t + VECTOR_ref::up() * -10.f*Scale_Rate, &EndPos, false)) {
					pos_t = EndPos;
				}
				c->ValueSet((PlayerID)0, false, CharaTypeID::Team);
				c->MovePoint(deg2rad(0.f), deg2rad(90.f), pos_t, 0);
				c->Heal(100, true);
			}
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
			//サウンド
			BattleResourceMngr->Set();
			//UI
			this->m_UIclass.Set("G17Gen3", ULTName.c_str(), false);
			//
			m_MainLoopPauseControl.Init();

			m_PrevSSAO = OptionParts->Get_SSAO();
			OptionParts->Set_SSAO(false);

			this->m_TutorialLog.Set();
			this->m_Tutorial.clear();
			{
				int mdata = FileRead_open("data/Tutorial.txt", FALSE);
				while (true) {
					if (FileRead_eof(mdata) != 0) { break; }
					auto ALL = getparams::Getstr(mdata);
					//コメントアウト
					if (ALL.find("//") != std::string::npos) {
						ALL = ALL.substr(0, ALL.find("//"));
					}
					//
					if (ALL == "") { continue; }
					if (ALL.find('=') != std::string::npos) {
						auto LEFT = getparams::getleft(ALL);
						auto RIGHT = getparams::getright(ALL);
						if (LEFT == "Voice") {
							m_Tutorial.emplace_back(RIGHT + "\n");
						}
					}
					else {
						m_Tutorial.back() += ALL + "\n";
					}
				}
				FileRead_close(mdata);
			}

			auto* BGM = BGMPool::Instance();
			BGM->StopAll();

			ScoreBoard = GraphHandle::Load("data/UI/Score.png");
			ScoreBoard2 = GraphHandle::Load("data/UI/Score2.png");
			//
			//*
			for (int j = 0; j < 5; j++) {
				ObjMngr->MakeObject(ObjType::Target);
				auto& t = *ObjMngr->GetObj(ObjType::Target, j);
				ObjMngr->LoadObjectModel(t.get(), "data/model/Target/");
				MV1::SetAnime(&t->GetObj(), t->GetObj());
				t->SetMove(MATRIX_ref::RotY(deg2rad(-90)), VECTOR_ref::vget(Scale_Rate*-(5.f + 10.f*j), 0.f, Scale_Rate*(9.f - 2.f*j)));
			}
			//*/

			for (int j = 0; j < 9; j++) {
				ObjMngr->MakeObject(ObjType::Target);
				auto& t = *ObjMngr->GetObj(ObjType::Target, 5 + j);
				ObjMngr->LoadObjectModel(t.get(), "data/model/Target2/");
				MV1::SetAnime(&t->GetObj(), t->GetObj());

				float rad = deg2rad(GetRand(360));
				t->SetMove(MATRIX_ref::RotY(rad),
					(TargetPositions[j] * Scale_Rate) +
						   MATRIX_ref::RotY(rad).zvec() *(1.5f*Scale_Rate));
			}
			//UI
			tgtSel = -1;
			tgtTimer = 0.f;

			{
				this->m_UIclass.InitGaugeParam(5 + 3, 0, 20);
				this->m_UIclass.InitGaugeParam(5 + 3 + 1, 0, 20);
				//HP
				this->m_UIclass.InitGaugeParam(0, (int)c->GetHPMax(), (int)c->GetHPMax());
				//AP
				this->m_UIclass.InitGaugeParam(1, (int)c->GetAPMax(), (int)c->GetAPMax());
				//Ammo
				if (c->GetGunPtrNow()) {
					this->m_UIclass.InitGaugeParam(2, (int)c->GetGunPtrNow()->GetAmmoAll() + 1, (int)c->GetGunPtrNow()->GetAmmoAll() + 1);
				}
				//ULT
				this->m_UIclass.InitGaugeParam(3, (int)c->GetULTMax(), (int)c->GetULTMax());
				//ULT
				if (c->GetGunPtr(0)) {
					this->m_UIclass.InitGaugeParam(4, (int)c->GetGunPtr(0)->GetAmmoAll() + 1, (int)c->GetGunPtr(0)->GetAmmoAll() + 1);
				}
				//mag
				int mags = 0;
				for (const auto& M : c->GetMagDatas()) {
					this->m_UIclass.InitGaugeParam(5 + mags, M.AmmoAll, M.AmmoAll);
					mags++;
				}
			}
		}
		bool			TutorialScene::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
#ifdef DEBUG
			//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG

			Pad->SetMouseMoveEnable(true);
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
						KeyGuide->AddGuide(PADS::LEAN_R, "覗き");
						KeyGuide->AddGuide(PADS::RUN, "走る");
						KeyGuide->AddGuide(PADS::WALK, "歩く");
						KeyGuide->AddGuide(PADS::SQUAT, "しゃがむ");
						KeyGuide->AddGuide(PADS::JUMP, "スタンス切替");

						KeyGuide->AddGuide(PADS::SHOT, "射撃");
						KeyGuide->AddGuide(PADS::ULT, "武器切替");
						KeyGuide->AddGuide(PADS::AIM, "エイム");
						KeyGuide->AddGuide(PADS::MELEE, "殴打");

						KeyGuide->AddGuide(PADS::RELOAD, "再装填");

						KeyGuide->AddGuide(PADS::CHECK, "装備確認");

						KeyGuide->AddGuide(PADS::THROW, "モルヒネ注射");

						//KeyGuide->AddGuide(PADS::INTERACT, "取得");
						KeyGuide->AddGuide(PADS::INVENTORY, "ポーズ");

						KeyGuide->AddGuide(PADS::INTERACT, "");
						KeyGuide->AddGuide(PADS::THROW, "チュートリアルを進める/戻る");
					}
				});

			if (DXDraw::Instance()->IsPause()) {
				m_MainLoopPauseControl.Execute();
				m_PrevSSAO |= OptionParts->Get_SSAO();
				OptionParts->Set_SSAO(m_PrevSSAO);
				return true;
			}
			else {
				m_MainLoopPauseControl.Reset();
				OptionParts->Set_SSAO(false);
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("update start");
#endif // DEBUG
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
			//FirstDoingv
			if (GetIsFirstLoop()) {
				SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
				m_TutorialNow = 0;
			}
			if (Pad->GetKey(PADS::INTERACT).trigger()) {
				if (m_TutorialNow < m_Tutorial.size()) {
					this->m_TutorialLog.AddLog(m_Tutorial.at(m_TutorialNow).c_str());
					SetCreate3DSoundFlag(FALSE);
					m_TutorialVoice.stop();
					m_TutorialVoice.Dispose();

					char Mes[256]{};
					snprintfDx(Mes, 256, "data/Sound/VOICE/voice-%d.wav", m_TutorialNow);

					m_TutorialVoice = SoundHandle::Load(Mes);
					m_TutorialVoice.play(DX_PLAYTYPE_BACK, TRUE);
					m_TutorialVoice.vol((int)(255 * OptionParts->Get_SE()));

					m_TutorialNow++;
				}
				else {
					this->m_TutorialLog.AddLog();
				}
			}
			if (Pad->GetKey(PADS::THROW).trigger()) {
				this->m_TutorialLog.SubLog();
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
				//
				bool isready = true;
				Chara->SetInput(MyInput, isready && Chara->IsAlive());
				//ダメージイベント処理
				for (auto& e : Chara->GetDamageEvent()) {
					this->m_DamageEvents.emplace_back(e);
				}
				Chara->GetDamageEvent().clear();
				//ダメージイベント
				for (int j = 0; j < this->m_DamageEvents.size(); j++) {
					if (Chara->SetDamageEvent(this->m_DamageEvents[j])) {
						std::swap(this->m_DamageEvents.back(), m_DamageEvents[j]);
						this->m_DamageEvents.pop_back();
						j--;
					}
				}
				Chara->AddULT(100, true);
				Chara->AddAmmoStock(99 - Chara->GetAmmoStock());
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

							int j = 0;
							while (true) {
								auto target = ObjMngr->GetObj(ObjType::Target, j);
								if (target != nullptr) {
									auto& t = (std::shared_ptr<TargetClass>&)(*target);
									auto Res = t->GetColLine(repos_tmp, pos_tmp, -1);
									if (Res.HitFlag == TRUE) {
										//エフェクト
										EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, Res.HitPosition, Res.Normal, a->GetCaliberSize() / 0.02f * Scale_Rate);
										//ヒット演算
										if (tgtSel != -1 && tgtSel != j) {
											auto& tOLD = (std::shared_ptr<TargetClass>&)(*ObjMngr->GetObj(ObjType::Target, tgtSel));
											tOLD->ResetHit();
										}
										tgtSel = j;
										tgtTimer = 2.f;

										t->SetHitPos(Res.HitPosition);
									}
								}
								else {
									break;
								}
								j++;
							}

							VECTOR_ref norm_tmp;
							auto ColResGround = this->m_BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, true, &norm_tmp);
							bool is_HitAll = false;
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
			}
			tgtTimer = std::max(tgtTimer - 1.f / FPS, 0.f);
			//近接攻撃
			{
			}
			//背景更新
			this->m_BackGround->FirstExecute();
			ObjMngr->LateExecuteObject();
			//視点
			{
				{
					//FPSカメラ
					VECTOR_ref CamPos = Chara->GetEyeMatrix().pos() + DrawParts->GetCamShake();
					DrawParts->SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeMatrix().zvec() * -1.f, Chara->GetEyeMatrix().yvec());

					//VECTOR_ref CamPos = VECTOR_ref::vget(0.f, 1.5f, 0.f)*Scale_Rate;
					//VECTOR_ref CamVec = VECTOR_ref::vget(1.f, 1.5f, 0.f)*Scale_Rate;
					//DrawParts->SetMainCamera().SetCamPos(CamPos, CamVec, Chara->GetEyeMatrix().yvec());

					//info
					auto fov_t = DrawParts->GetMainCamera().GetCamFov();
					auto near_t = DrawParts->GetMainCamera().GetCamNear();
					auto far_t = DrawParts->GetMainCamera().GetCamFar();
					if (Chara->GetIsADS()) {
						Easing(&near_t, Scale_Rate * 0.03f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 100.f, 0.5f, EasingType::OutExpo);
					}
					else {
						Easing(&near_t, Scale_Rate * 0.05f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 100.f, 0.5f, EasingType::OutExpo);
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
						PostPassEffect::Instance()->Set_DoFNearFar(Scale_Rate * 0.5f, far_t - 0.1f, near_t, far_t);
					}
				}
			}
			{
				PostPassEffect::Instance()->SetLevelFilter(0, 255, 1.f);
			}
			//
			this->m_BackGround->Execute();
			//射撃光
			if (Chara->GetGunPtrNow() && Chara->GetGunPtrNow()->GetShotSwitch()) {// 
				/*
				auto mat = Chara->GetGunPtrNow()->GetFrameWorldMat(GunFrame::Muzzle);
				SetLightEnable(TRUE);
				ChangeLightTypePoint(mat.pos().get(),
					4.0f*Scale_Rate,
					0.001f,
					0.012f,
					0.004f);
				//*/
			}
			//レーザーサイト
			while (true) {
				if (!Chara->GetGunPtrNow()) { continue; }
				if (!Chara->GetGunPtrNow()->HasFrame(GunFrame::LaserSight) || !Chara->GetGunPtrNow()->IsActive()) {
					Chara->SetIsLaserActive(false);
					break;
				}
				//
				auto mat = Chara->GetGunPtrNow()->GetFrameWorldMat(GunFrame::LaserSight);
				VECTOR_ref StartPos = mat.pos();
				VECTOR_ref EndPos = StartPos + mat.zvec()*-1.f * 15.f*Scale_Rate;
				this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, true);
				auto Vec = (EndPos - StartPos);
				EndPos = StartPos + Vec.Norm()*std::max(Vec.Length() - 0.3f*Scale_Rate, 0.f);
				Chara->SetLaserStartPos(StartPos);
				Chara->SetLaserEndPos(EndPos);
				Chara->SetIsLaserActive(true);
				break;
			}
#ifdef DEBUG
			DebugParts->SetPoint("---");
#endif // DEBUG
			//UIパラメーター
			{
				//シェイク
				this->m_UIclass.SetIntParam(0, (int)(DrawParts->GetCamShake().x()*100.f));
				this->m_UIclass.SetIntParam(1, (int)(DrawParts->GetCamShake().y()*100.f));
				this->m_UIclass.SetIntParam(2, (int)(rad2deg(Chara->GetLeanRad()*5.f)));
				//AmmoStock
				this->m_UIclass.SetIntParam(3, Chara->GetAmmoStock());
				//Time
				this->m_UIclass.SetfloatParam(0, 0);
				this->m_UIclass.SetfloatParam(1, 0);
				//
				this->m_UIclass.SetGaugeParam(5 + 3, (int)(Chara->GetAutoAimPer()*20.f), 20);
				this->m_UIclass.SetGaugeParam(5 + 3 + 1, (int)(Chara->GetAutoAimPer()*20.f), 20);
				this->m_UIclass.SetIntParam(4, Chara->GetAutoAimID());
				this->m_UIclass.SetfloatParam(2, Chara->GetAutoAimOn());
				//Score
				this->m_UIclass.SetIntParam(6, 0);
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
				if (Chara->GetGunPtr(0)) {
					this->m_UIclass.SetGaugeParam(4, (int)Chara->GetGunPtr(0)->GetAmmoNumTotal(), (int)Chara->GetGunPtr(0)->GetAmmoAll() + 1);
				}
				this->m_UIclass.SetfloatParam(3, Chara->GetGunSelPer());
				this->m_UIclass.SetfloatParam(4, 99.99f);
				//mag
				int mags = 0;
				for (const auto& M : Chara->GetMagDatas()) {
					this->m_UIclass.SetGaugeParam(5 + mags, M.AmmoNum, M.AmmoAll);
					mags++;
				}
				this->m_UIclass.SetIntParam(8, Chara->GetArmerStock());
				this->m_UIclass.SetIntParam(9, Chara->GetMorphineStock());

				this->m_TutorialLog.Update();
			}
			//
			EffectControl::Execute();
			//
			{
				VECTOR_ref campos; campos.z(-1.f);
				Chara->SetCameraPosition(campos);
			}
#ifdef DEBUG
			DebugParts->SetPoint("update end");
#endif // DEBUG
			if (m_MainLoopPauseControl.GetIsRetireSelected()) {
				return false;
			}
			return true;
		}
		void			TutorialScene::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			//使い回しオブジェ系
			auto* Ptr = &PlayerMngr->GetPlayer(0).GetChara();
			ObjMngr->DelObj(Ptr);
			PlayerMngr->GetPlayer(0).Dispose();
			this->m_BackGround->Dispose();
			this->m_BackGround.reset();
			GunsModify::DisposeSlots();
			//
			EffectControl::Dispose();

			OptionParts->Set_SSAO(m_PrevSSAO);
			this->m_TutorialLog.Dispose();

			ScoreBoard.Dispose();
			ScoreBoard2.Dispose();
		}
		//
		void			TutorialScene::BG_Draw_Sub(void) noexcept {
			this->m_BackGround->BG_Draw();
		}
		void			TutorialScene::ShadowDraw_Far_Sub(void) noexcept {
			//this->m_BackGround->Shadow_Draw();
		}

		void			TutorialScene::ShadowDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			TutorialScene::MainDraw_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();

			SetFogEnable(FALSE);
			this->m_BackGround->Draw();
			ObjMngr->DrawObject();
			//レーザー
			Chara->DrawLaser();

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
		}

		void			TutorialScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
			//着弾表示
			if (Chara->IsAlive()) {
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
				this->m_TutorialLog.Draw();
				//通信設定
				/*
				if (DrawParts->IsPause()) {
					m_NetWorkBrowser.Draw();
				}
				//*/
			}
		}
		void			TutorialScene::DrawUI_In_Sub(void) noexcept {
			//ポーズ
			if (DXDraw::Instance()->IsPause()) {
				m_MainLoopPauseControl.Draw();
			}
			else {
				//的ヒット状況表示
				if (tgtSel >= 0) {
					auto* ObjMngr = ObjectManager::Instance();
					auto* DrawParts = DXDraw::Instance();
					auto& t = (std::shared_ptr<TargetClass>&)(*ObjMngr->GetObj(ObjType::Target, tgtSel));

					int xp = DrawParts->m_DispXSize / 2 - y_r(300);
					int yp = DrawParts->m_DispYSize / 2 + y_r(100);
					int size = y_r(100);
					int xs = size / 2;
					int ys = size / 2;
					int xp2 = xp + ys * 2;
					int yp2 = yp + ys * 2;
					float AlphaPer = std::clamp(tgtTimer, 0.f, 1.f);
					if (AlphaPer > 0.01f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*AlphaPer));
						//背景
						if (tgtSel < 5) {
							ScoreBoard.DrawExtendGraph(xp, yp, xp2, yp2, true);
						}
						else {
							ScoreBoard2.DrawExtendGraph(xp, yp, xp2, yp2, true);
						}
						//命中箇所
						for (auto& r : t->GetHitPosRec()) {
							float cos_t, sin_t;
							t->GetHitPoint(r, &cos_t, &sin_t);
							DrawCircle(xp + xs + (int)((float)xs * cos_t), yp + ys + (int)((float)ys * sin_t), 2, Green);
						}
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}
				}
			}
		}
		//使い回しオブジェ系
		void			TutorialScene::Dispose_Load(void) noexcept {
			if (!m_IsFirstLoad) {
				m_IsFirstLoad = true;
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto* BattleResourceMngr = CommonBattleResource::Instance();
				BattleResourceMngr->Dispose();
				this->m_UIclass.Dispose();
				PlayerMngr->Dispose();
				ObjMngr->DisposeObject();
			}
		}
		//
		void			TutorialScene::LoadGun(const std::string&FolderName, PlayerID ID, bool IsPreset, int Sel) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->LoadGun(FolderName, ID, Sel);
			auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(ID).GetChara();
			auto& g = (std::shared_ptr<GunClass>&)PlayerMngr->GetPlayer(ID).GetGun(Sel);
			c->SetGunPtr(Sel, g);
			GunsModify::CreateSelData(c->GetGunPtr(Sel), IsPreset);
			c->GetGunPtr(Sel)->Init_Gun();
		}
	};
};
