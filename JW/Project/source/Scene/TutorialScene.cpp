#include	"TutorialScene.hpp"

#include "../sub.hpp"

#include "../MainScene/Object/Target.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			TutorialScene::Load_Sub(void) noexcept {
			//ロード
			if (m_IsFirstLoad) {
				m_IsFirstLoad = false;
				//BG
				GunAnimManager::Instance()->Load("data/CharaAnime/");
				//
				LoadSE();
				//
				this->m_UIclass.Load();
			}
		}
		void			TutorialScene::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			//
			SetAmbientLight(VECTOR_ref::vget(0.f, -1.f, 0.f), GetColorF(1.f, 1.f, 1.f, 0.0f));
			SetFarShadow(VECTOR_ref::vget(Scale_Rate*-100.f, Scale_Rate*0.f, Scale_Rate*-100.f), VECTOR_ref::vget(Scale_Rate*100.f, Scale_Rate*1.f, Scale_Rate*100.f));
			SetMiddleShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.f, Scale_Rate*10.f));
			SetNearShadow(VECTOR_ref::vget(Scale_Rate*-10.f, Scale_Rate*-3.f, Scale_Rate*-10.f), VECTOR_ref::vget(Scale_Rate*10.f, Scale_Rate*0.5f, Scale_Rate*10.f));

			SetShadowDir(GetLightVec(), 0);
			SetShadowDir(GetLightVec(), 1);
			SetShadowDir(GetLightVec(), 2);
			//
			DeleteLightHandleAll();
			SetLightEnable(TRUE);
			ChangeLightTypeDir(GetLightVec().get());
			SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
			SetLightSpcColor(GetColorF(0.1f, 0.1f, 0.1f, 0.f));
			SetLightAmbColor(GetColorF(0.1f, 0.1f, 0.1f, 1.f));
			//
			this->m_BackGround = std::make_shared<BackGroundClassTutorial>();
			this->m_BackGround->Init("data/model/map/","");//1.59秒
			//ロード
			LoadChara("Suit");
			GunsModify::LoadSlots("Save/gundata.svf");//プリセット読み込み
			LoadGun("G17Gen3", true, 0);
			//LoadGun("AKS-74", false, 0);
			//LoadGun("M16-4", false, 0);

			std::string ULTName = ULT_GUNName[(int)GunsModify::GetULTSelect()];

			LoadGun(ULTName.c_str(), false, 1);
			//BGをオブジェに登録
			m_CharacterPtr->SetMapCol(this->m_BackGround, false);
			//
			{
				/*
				for (auto& M : m_CharacterPtr->GetMagDatas()) {
					m_CharacterPtr->SetMag((int)(&M - &m_CharacterPtr->GetMagDatas().front()), 0);
				}
				m_CharacterPtr->GetGunPtrNow()->UnloadChamber();
				m_CharacterPtr->GetGunPtrNow()->SetAmmo(0);
				//*/
			}
			//人の座標設定
			{
				VECTOR_ref pos_t;
				pos_t = VECTOR_ref::vget(0.f, 0.f, 0.f);

				VECTOR_ref EndPos = pos_t + VECTOR_ref::up() * 10.f*Scale_Rate;
				if (this->m_BackGround->CheckLinetoMap(pos_t + VECTOR_ref::up() * -10.f*Scale_Rate, &EndPos, false)) {
					pos_t = EndPos;
				}
				m_CharacterPtr->ValueSet(deg2rad(0.f), deg2rad(90.f), pos_t, (PlayerID)0, 0);
				m_CharacterPtr->SetCharaType(CharaTypeID::Team);
			}
			//Cam
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
			DrawParts->SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
			//サウンド
			SetSE();
			//UI
			this->m_UIclass.Set(ULTName.c_str(), false);
			//
			m_MainLoopPauseControl.Init();

			m_PrevSSAO = OptionParts->Get_SSAO();
			OptionParts->Set_SSAO(false);

			//m_CharacterPtr->AddAmmoStock(99);

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
			//
			for (int j = 0; j < 5; j++) {
				ObjMngr->MakeObject(ObjType::Target);
				auto& t = *ObjMngr->GetObj(ObjType::Target, j);
				ObjMngr->LoadObjectModel(t.get(), "data/model/Target/");
				MV1::SetAnime(&t->GetObj(), t->GetObj());
				t->SetMove(MATRIX_ref::RotY(deg2rad(-90)), VECTOR_ref::vget(Scale_Rate*-(5.f + 10.f*j), 0.f, Scale_Rate*((-2.f * 5 / 2) + 2.f*j)));
			}
			//UI
			tgtSel = -1;
			tgtTimer = 0.f;

			{
				this->m_UIclass.InitGaugeParam(4 + 3, 0, 20);
				this->m_UIclass.InitGaugeParam(4 + 3 + 1, 0, 20);
				//HP
				this->m_UIclass.InitGaugeParam(0, (int)m_CharacterPtr->GetHPMax(), (int)m_CharacterPtr->GetHPMax());
				//AP
				this->m_UIclass.InitGaugeParam(1, (int)m_CharacterPtr->GetAPMax(), (int)m_CharacterPtr->GetAPMax());
				//Ammo
				if (m_CharacterPtr->GetGunPtrNow()) {
					this->m_UIclass.InitGaugeParam(2, (int)m_CharacterPtr->GetGunPtrNow()->GetAmmoAll() + 1, (int)m_CharacterPtr->GetGunPtrNow()->GetAmmoAll() + 1);
				}
				//ULT
				this->m_UIclass.InitGaugeParam(3, (int)m_CharacterPtr->GetULTMax(), (int)m_CharacterPtr->GetULTMax());
				//mag
				int mags = 0;
				for (const auto& M : m_CharacterPtr->GetMagDatas()) {
					this->m_UIclass.InitGaugeParam(4 + mags, M.AmmoAll, M.AmmoAll);
					mags++;
				}
			}
		}
		bool			TutorialScene::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
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
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
#ifdef DEBUG
			//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
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
				m_GameEnd |= (!(m_CharacterPtr->IsAlive() && (Timer <= TotalTime)));
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
				if (m_CharacterPtr->GetGunPtrNow()) {
					pp_x -= m_CharacterPtr->GetRecoilRadAdd().y();
					pp_y -= m_CharacterPtr->GetRecoilRadAdd().x();
				}
				MyInput.SetInputStart(pp_x, pp_y, m_CharacterPtr->GetRadBuf());
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
				m_CharacterPtr->SetInput(MyInput, isready && m_CharacterPtr->IsAlive());
				//ダメージイベント処理
				for (auto& e : m_CharacterPtr->GetDamageEvent()) {
					this->m_DamageEvents.emplace_back(e);
				}
				m_CharacterPtr->GetDamageEvent().clear();
				//ダメージイベント
				for (int j = 0; j < this->m_DamageEvents.size(); j++) {
					if (m_CharacterPtr->SetDamageEvent(this->m_DamageEvents[j])) {
						std::swap(this->m_DamageEvents.back(), m_DamageEvents[j]);
						this->m_DamageEvents.pop_back();
						j--;
					}
				}
				m_CharacterPtr->AddULT(100, true);
				m_CharacterPtr->AddAmmoStock(99 - m_CharacterPtr->GetAmmoStock());
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

				Set_is_Blackout(false);
				Set_Per_Blackout(0.f);
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
					VECTOR_ref CamPos = m_CharacterPtr->GetEyeMatrix().pos() + DrawParts->GetCamShake();
					DrawParts->SetMainCamera().SetCamPos(CamPos, CamPos + m_CharacterPtr->GetEyeMatrix().zvec() * -1.f, m_CharacterPtr->GetEyeMatrix().yvec());

					//VECTOR_ref CamPos = VECTOR_ref::vget(0.f, 1.5f, 0.f)*Scale_Rate;
					//VECTOR_ref CamVec = VECTOR_ref::vget(1.f, 1.5f, 0.f)*Scale_Rate;
					//DrawParts->SetMainCamera().SetCamPos(CamPos, CamVec, m_CharacterPtr->GetEyeMatrix().yvec());

					//info
					auto fov_t = DrawParts->GetMainCamera().GetCamFov();
					auto near_t = DrawParts->GetMainCamera().GetCamNear();
					auto far_t = DrawParts->GetMainCamera().GetCamFar();
					if (m_CharacterPtr->GetIsADS()) {
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
						if (m_CharacterPtr->GetIsADS()) {
							fov -= deg2rad(15);
							fov /= std::max(1.f, m_CharacterPtr->GetSightZoomSize() / 2.f);
						}
						else if (m_CharacterPtr->GetRun()) {
							fov += deg2rad(5);
						}
						if (m_CharacterPtr->GetMeleeSwitch()) {
							fov += deg2rad(15);
							Easing(&fov_t, fov, 0.8f, EasingType::OutExpo);
						}
						else if (m_CharacterPtr->GetGunPtrNow() && m_CharacterPtr->GetGunPtrNow()->GetShotSwitch()) {
							fov -= deg2rad(5);
							Easing(&fov_t, fov, 0.5f, EasingType::OutExpo);
						}
						else {
							Easing(&fov_t, fov, 0.8f, EasingType::OutExpo);
						}
					}
					DrawParts->SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
					//DoF
					if (m_CharacterPtr->GetIsADS()) {
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
			if (m_CharacterPtr->GetGunPtrNow() && m_CharacterPtr->GetGunPtrNow()->GetShotSwitch()) {// 
				/*
				auto mat = m_CharacterPtr->GetGunPtrNow()->GetFrameWorldMat(GunFrame::Muzzle);
				SetLightEnable(TRUE);
				ChangeLightTypePoint(mat.pos().get(),
					4.0f*Scale_Rate,
					0.001f,
					0.012f,
					0.004f);
				//*/
			}
			
			//SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(0)).Norm(), 0);
			//SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(1)).Norm(), 1);
			//SetShadowDir((DrawParts->GetMainCamera().GetCamPos() - this->m_BackGround->GetNearestLight(2)).Norm(), 2);
			//SetisUpdateFarShadow(true);
			//レーザーサイト
			while (true) {
				if (!m_CharacterPtr->GetGunPtrNow()) { continue; }
				if (!m_CharacterPtr->GetGunPtrNow()->HasFrame(GunFrame::LaserSight) || !m_CharacterPtr->GetGunPtrNow()->IsActive()) {
					m_CharacterPtr->SetIsLaserActive(false);
					break;
				}
				//
				auto mat = m_CharacterPtr->GetGunPtrNow()->GetFrameWorldMat(GunFrame::LaserSight);
				VECTOR_ref StartPos = mat.pos();
				VECTOR_ref EndPos = StartPos + mat.zvec()*-1.f * 15.f*Scale_Rate;
				this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, true);
				auto Vec = (EndPos - StartPos);
				EndPos = StartPos + Vec.Norm()*std::max(Vec.Length() - 0.3f*Scale_Rate, 0.f);
				m_CharacterPtr->SetLaserStartPos(StartPos);
				m_CharacterPtr->SetLaserEndPos(EndPos);
				m_CharacterPtr->SetIsLaserActive(true);
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
				this->m_UIclass.SetIntParam(2, (int)(rad2deg(m_CharacterPtr->GetGunRadAdd())*5.f));
				//AmmoStock
				this->m_UIclass.SetIntParam(3, m_CharacterPtr->GetAmmoStock());
				//Time
				this->m_UIclass.SetfloatParam(0, 0);
				this->m_UIclass.SetfloatParam(1, 0);
				//
				this->m_UIclass.SetGaugeParam(4 + 3, (int)(m_CharacterPtr->GetAutoAimPer()*20.f), 20);
				this->m_UIclass.SetGaugeParam(4 + 3 + 1, (int)(m_CharacterPtr->GetAutoAimPer()*20.f), 20);
				this->m_UIclass.SetIntParam(4, m_CharacterPtr->GetAutoAimID());
				this->m_UIclass.SetfloatParam(2, m_CharacterPtr->GetAutoAimOn());
				//Score
				this->m_UIclass.SetIntParam(6, 0);
				//HP
				this->m_UIclass.SetGaugeParam(0, (int)m_CharacterPtr->GetHP(), (int)m_CharacterPtr->GetHPMax());
				//AP
				this->m_UIclass.SetGaugeParam(1, (int)m_CharacterPtr->GetAP(), (int)m_CharacterPtr->GetAPMax());
				//Ammo
				if (m_CharacterPtr->GetGunPtrNow()) {
					this->m_UIclass.SetGaugeParam(2, (int)m_CharacterPtr->GetGunPtrNow()->GetAmmoNumTotal(), (int)m_CharacterPtr->GetGunPtrNow()->GetAmmoAll() + 1);
				}
				//ULT
				this->m_UIclass.SetGaugeParam(3, (int)m_CharacterPtr->GetULT(), (int)m_CharacterPtr->GetULTMax());
				//mag
				int mags = 0;
				for (const auto& M : m_CharacterPtr->GetMagDatas()) {
					this->m_UIclass.SetGaugeParam(4 + mags, M.AmmoNum, M.AmmoAll);
					mags++;
				}
				this->m_TutorialLog.Update();
			}
			//
			EffectControl::Execute();
			//
			{
				VECTOR_ref campos; campos.z(-1.f);
				m_CharacterPtr->SetCameraPosition(campos);
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

			//使い回しオブジェ系
			ObjMngr->DelObj((SharedObj*)&m_CharacterPtr);
			m_CharacterPtr->Dispose();
			this->m_BackGround->Dispose();
			this->m_BackGround.reset();
			GunsModify::DisposeSlots();
			//
			EffectControl::Dispose();

			OptionParts->Set_SSAO(m_PrevSSAO);
			this->m_TutorialLog.Dispose();

			ScoreBoard.Dispose();
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
			auto* ObjMngr = ObjectManager::Instance();
			SetFogEnable(FALSE);
			this->m_BackGround->Draw();
			ObjMngr->DrawObject();
			//レーザー
			m_CharacterPtr->DrawLaser();

			//ObjMngr->DrawDepthObject();
			//シェーダー描画用パラメーターセット
			{
				//
				if (m_CharacterPtr->GetGunPtrNow()) {
					m_MyPlayerReticleControl.Update(
						m_CharacterPtr->GetGunPtrNow()->GetLensPos(),
						m_CharacterPtr->GetGunPtrNow()->GetFrameWorldMat(GunFrame::LensSize).pos(),
						m_CharacterPtr->GetGunPtrNow()->GetReticlePos()
					);
					//*
					if (m_MyPlayerReticleControl.IsActive() && m_CharacterPtr->GetSightZoomSize() > 1.f) {
						Set_is_lens(true);
						Set_xp_lens(m_MyPlayerReticleControl.GetLensXPos());
						Set_yp_lens(m_MyPlayerReticleControl.GetLensYPos());
						Set_size_lens(m_MyPlayerReticleControl.GetLensSize());
						Set_zoom_lens(std::max(1.f, m_CharacterPtr->GetSightZoomSize() / 2.f));
					}
					else {
						Set_is_lens(false);
						Set_zoom_lens(1.f);
					}
					//*/
				}
				else {
					m_MyPlayerReticleControl.SetActiveOff();
					Set_is_lens(false);
				}
			}
		}

		void			TutorialScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//着弾表示
			if (m_CharacterPtr->IsAlive()) {
				//レティクル表示
				if (m_MyPlayerReticleControl.IsActive() && m_CharacterPtr->IsSightPtrActive()
					
					&&
					!((m_CharacterPtr->GetADSPer() < 0.8f) && m_CharacterPtr->GetSightZoomSize() > 1.f)
					) {
					m_CharacterPtr->GetSightReitcleGraphPtr().DrawRotaGraph(
						(int)m_MyPlayerReticleControl.GetReticleXPos(),
						(int)m_MyPlayerReticleControl.GetReticleYPos(),
						1.f, m_CharacterPtr->GetGunRadAdd(), true);
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
						ScoreBoard.DrawExtendGraph(xp, yp, xp2, yp2, true);
						//命中箇所
						for (auto& r : t->GetHitPosRec()) {
							float cos_t, sin_t;
							t->GetHitPoint(r, &cos_t, &sin_t);
							DrawCircle(xp + xs + (int)((float)xs * cos_t), yp + ys + (int)((float)ys * sin_t), 2, GetColor(0, 255, 0));
						}
						//点数
						int ypAdd = 0;
						auto* Fonts = FontPool::Instance();
						for (auto& r : t->GetHitPosRec()) {
							Fonts->Get(FontPool::FontType::Gothic_AA, y_r(24)).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																						  xp, yp2 + ypAdd, GetColor(255, 255, 255), GetColor(255, 255, 255), "[%4.1f]", t->GetHitPoint(r));
							ypAdd += y_r(24);
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
				DisposeSE();
				this->m_UIclass.Dispose();
				m_GunPtr.reset();
				m_CharacterPtr.reset();
				ObjMngr->DisposeObject();
			}
		}
		//SE
		void			TutorialScene::LoadSE(void) noexcept {
			auto* SE = SoundPool::Instance();

			SE->Add((int)SoundEnum::CartFall, 6, "data/Sound/SE/gun/case.wav", false);
			SE->Add((int)SoundEnum::MagFall, 6, "data/Sound/SE/gun/ModFall.wav", false);
			SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
			SE->Add((int)SoundEnum::AmmoLoad, 1, "data/Sound/SE/gun/ammoload.wav");
			SE->Add((int)SoundEnum::Tinnitus, 2, "data/Sound/SE/Tinnitus.wav");
			
			for (int i = 0; i < 6; i++) {
				SE->Add((int)SoundEnum::Cocking1_0 + i, 4, "data/Sound/SE/gun/autoM870/" + std::to_string(i) + ".wav");
				SE->Add((int)SoundEnum::Cocking2_0 + i, 4, "data/Sound/SE/gun/autoM16/" + std::to_string(i) + ".wav");
				SE->Add((int)SoundEnum::Cocking3_0 + i, 4, "data/Sound/SE/gun/auto1911/" + std::to_string(i) + ".wav");
			}
			SE->Add((int)SoundEnum::StandUp, 1, "data/Sound/SE/move/sliding.wav", false);
			SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add((int)SoundEnum::SlideFoot, 3, "data/Sound/SE/move/sliding.wav");
			SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
			SE->Add((int)SoundEnum::GetAmmo, 3, "data/Sound/SE/move/getammo.wav");
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
			SE->Add((int)SoundEnum::Stim, 1, "data/Sound/SE/Stim.wav");
		}
		void			TutorialScene::SetSE(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();

			SE->Get((int)SoundEnum::CartFall).SetVol_Local(48);
			SE->Get((int)SoundEnum::MagFall).SetVol_Local(48);
			SE->Get((int)SoundEnum::Trigger).SetVol_Local(48);
			//SE->Get((int)SoundEnum::AmmoLoad).SetVol_Local(48);
			SE->Get((int)SoundEnum::Shot2).SetVol_Local(216);
			SE->Get((int)SoundEnum::Shot3).SetVol_Local(216);
			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
			SE->Get((int)SoundEnum::Hit).SetVol_Local(255);
			for (int i = 0; i < 5; i++) {
				SE->Get((int)SoundEnum::HitGround0 + i).SetVol_Local(92);
			}
			SE->SetVol(OptionParts->Get_SE());
		}
		void			TutorialScene::DisposeSE(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete((int)SoundEnum::CartFall);
			SE->Delete((int)SoundEnum::MagFall);
			SE->Delete((int)SoundEnum::Env);
			SE->Delete((int)SoundEnum::Env2);
			SE->Delete((int)SoundEnum::StandUp);
			SE->Delete((int)SoundEnum::Trigger);
			SE->Delete((int)SoundEnum::AmmoLoad);
			SE->Delete((int)SoundEnum::Tinnitus);
			for (int i = 0; i < 6; i++) {
				SE->Delete((int)SoundEnum::Cocking1_0 + i);
				SE->Delete((int)SoundEnum::Cocking2_0 + i);
				SE->Delete((int)SoundEnum::Cocking3_0 + i);
			}
			SE->Delete((int)SoundEnum::RunFoot);
			SE->Delete((int)SoundEnum::SlideFoot);
			SE->Delete((int)SoundEnum::StandupFoot);
			SE->Delete((int)SoundEnum::GetAmmo);
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
			SE->Delete((int)SoundEnum::Stim);
		}
		//
		void			TutorialScene::LoadChara(const std::string&FolderName) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* Ptr = ObjMngr->MakeObject(ObjType::Human);
			m_CharacterPtr = (std::shared_ptr<CharacterClass>&)(*Ptr);
			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";
			ObjMngr->LoadObjectModel(m_CharacterPtr.get(), Path.c_str());
			MV1::SetAnime(&m_CharacterPtr->GetObj(), m_CharacterPtr->GetObj());
			m_CharacterPtr->Init();
			//アーマー
			{
				auto* ArmerPtr = ObjMngr->MakeObject(ObjType::Armer);
				auto& a = (std::shared_ptr<ArmerClass>&)(*ArmerPtr);

				ObjMngr->LoadObjectModel((*ArmerPtr).get(), "data/model/PlateCarrler/");
				(*ArmerPtr)->Init();
				m_CharacterPtr->SetArmer(a);
			}
			{
				auto* MorphinePtr = ObjMngr->MakeObject(ObjType::Morphine);
				auto& a = (std::shared_ptr<MorphineClass>&)(*MorphinePtr);

				ObjMngr->LoadObjectModel((*MorphinePtr).get(), "data/model/Morphine/");
				(*MorphinePtr)->Init();
				m_CharacterPtr->SetMorphine(a);
			}
		}
		void			TutorialScene::LoadGun(const std::string&FolderName, bool IsPreset, int Sel) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* Ptr = ObjMngr->MakeObject(ObjType::Gun);
			m_GunPtr = ((std::shared_ptr<GunClass>&)(*Ptr));
			std::string Path = "data/gun/";
			Path += FolderName;
			Path += "/";
			ObjMngr->LoadObjectModel(m_GunPtr.get(), Path.c_str());
			MV1::SetAnime(&m_GunPtr->GetObj(), m_GunPtr->GetObj());
			m_CharacterPtr->SetGunPtr(Sel, m_GunPtr);
			m_GunPtr->Init();
			GunsModify::CreateSelData(m_GunPtr, IsPreset);
			m_GunPtr->Init_Gun();
		}
	};
};
