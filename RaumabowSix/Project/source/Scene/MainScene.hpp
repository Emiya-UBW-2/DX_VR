#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public EffectControl {
		private:
			static const int		Chara_num = Player_num;
			static const int		Vehicle_num = 0;
			static const int		gun_num = Chara_num;
		private:
			//リソース関連
			std::shared_ptr<BackGroundClass>			m_BackGround;				//BG
			SoundHandle				m_Env;
			//人
			std::vector<std::shared_ptr<CharacterClass>> character_Pool;	//ポインター別持ち
			//操作関連
			float					m_EyePosPer_Prone = 0.f;
			float					m_EyeRunPer{ 0.f };
			switchs					m_FPSActive;
			switchs					m_MouseActive;
			int						m_LookMode{ 0 };
			//UI関連
			UIClass					m_UIclass;
			float					m_HPBuf{ 0.f };
			float					m_ScoreBuf{ 0.f };
			GraphHandle				hit_Graph;
			GraphHandle				aim_Graph;
			//
			float					m_CamShake{ 0.f };
			VECTOR_ref				m_CamShake1;
			VECTOR_ref				m_CamShake2;
			//銃関連
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;
			//
			MATRIX_ref				m_FreeLookMat;
			float					m_TPS_Xrad{ 0.f };
			float					m_TPS_Yrad{ 0.f };
			float					m_TPS_XradR{ 0.f };
			float					m_TPS_YradR{ 0.f };
			float					m_TPS_Per{ 1.f };
			//
			std::vector<DamageEvent>	m_DamageEvents;

			NetWorkBrowser			m_NetWorkBrowser;
			//共通
			double					m_ClientFrame{ 0.0 };

			float					fov_base{ 0.f };

			float					m_AimRot{ 0.f };
			VECTOR_ref				m_Laserpos2D;
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			MAINLOOP(void) noexcept { }
			void			Set_Sub(void) noexcept override {
				GunAnimManager::Create();
				auto* AnimMngr = GunAnimManager::Instance();
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				SetAmbientShadow(
					VECTOR_ref::vget(Scale_Rate*-100.f, Scale_Rate*-10.f, Scale_Rate*-100.f),
					VECTOR_ref::vget(Scale_Rate*100.f, Scale_Rate*50.f, Scale_Rate*100.f),
					VECTOR_ref::vget(-0.8f, -0.5f, -0.1f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				//Load
				//BG
				this->m_BackGround = std::make_shared<BackGroundClass>();
				this->m_BackGround->Init();
				//
				ObjMngr->Init(this->m_BackGround);
				for (int i = 0; i < Chara_num; i++) {
					character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/Soldier1/")));
				}
				for (int i = 0; i < gun_num; i++) {
					ObjMngr->AddObject(ObjType::Gun, "data/gun/AR15/");
					ObjMngr->AddObject(ObjType::Gun, "data/gun/MEU1911/");
				}
				//ロード
				SetCreate3DSoundFlag(FALSE);
				this->m_Env = SoundHandle::Load("data/Sound/SE/envi.wav");
				SetCreate3DSoundFlag(FALSE);
				this->m_Env.vol(64);
				//UI
				this->m_UIclass.Set();

				hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
				//Set
				//人
				for (int loop = 0; loop < (int)EnumGunAnim::Max; loop++) {
					std::string Path = "data/CharaAnime/";
					Path += EnumGunAnimName[loop];
					Path += ".txt";
					AnimMngr->LoadAction(Path.c_str(), (EnumGunAnim)loop);
				}
				for (auto& c : this->character_Pool) {
					size_t index = &c - &this->character_Pool.front();
					VECTOR_ref pos_t = VECTOR_ref::vget(0.f + (float)(index)*20.f, 0.f, 0.f);
					auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
					if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), false, pos_t, (PlayerID)index);
					c->SetGunPtr(
						(std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, (int)(index * 2 + 0))),
						(std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, (int)(index * 2 + 1)))
					);
					if (index == 0) {
						//c->SetUseRealTimePhysics(true);
						c->SetUseRealTimePhysics(false);
						c->SetCharaType(CharaTypeID::Team);
					}
					else {
						c->SetUseRealTimePhysics(false);
						c->SetCharaType(CharaTypeID::Enemy);
					}
				}
				//player
				PlayerMngr->Init(Player_num);
				for (int i = 0; i < Player_num; i++) {
					PlayerMngr->GetPlayer(i).SetChara((std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i)));
					//PlayerMngr->GetPlayer(i).SetChara(nullptr);
				}
				this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetChara()->GetHP();
				this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();

				//Cam
				SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
				SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				//サウンド
				auto SE = SoundPool::Instance();
				SE->Add((int)SoundEnum::Shot_Gun, 3, "data/Sound/SE/gun/shot.wav");
				SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
				for (int i = 0; i < 4; i++) {
					SE->Add((int)SoundEnum::Cocking0 + i, 3, "data/Sound/SE/gun/slide/bolt/" + std::to_string(i) + ".wav");
				}
				SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
				SE->Add((int)SoundEnum::SlideFoot, 9, "data/Sound/SE/move/sliding.wav");
				SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
				SE->Add((int)SoundEnum::Heart, 9, "data/Sound/SE/move/heart.wav");

				SE->Get((int)SoundEnum::Shot_Gun).SetVol_Local(128);
				SE->Get((int)SoundEnum::Trigger).SetVol_Local(128);
				for (int i = 0; i < 4; i++) {
					SE->Get((int)SoundEnum::Cocking0 + i).SetVol_Local(128);
				}
				SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
				SE->Get((int)SoundEnum::Heart).SetVol_Local(92);


				//入力
				this->m_FPSActive.Set(true);
				this->m_MouseActive.Set(true);

				this->m_DamageEvents.clear();

				m_NetWorkBrowser.Init();
			}
			//
			bool			Update_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
#ifdef DEBUG
				//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
				//FirstDoingv
				if (GetIsFirstLoop()) {
					SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
					this->m_Env.play(DX_PLAYTYPE_LOOP, TRUE);
					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					Chara->LoadReticle();//プレイヤー変更時注意
					fov_base = GetMainCamera().GetCamFov();
				}
				//Input,AI
				{
					float cam_per = ((GetMainCamera().GetCamFov() / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					float pp_x = 0.f, pp_y = 0.f;
					bool look_key = false;
					bool eyechange_key = false;
					InputControl MyInput;

					if (GetJoypadNum() > 0) {
						DINPUT_JOYSTATE input;
						int padID = DX_INPUT_PAD1;
						GetJoypadInputState(padID);
						switch (GetJoypadType(padID)) {
						case DX_PADTYPE_OTHER:
						case DX_PADTYPE_DUAL_SHOCK_4:
						case DX_PADTYPE_DUAL_SENSE:
						case DX_PADTYPE_SWITCH_JOY_CON_L:
						case DX_PADTYPE_SWITCH_JOY_CON_R:
						case DX_PADTYPE_SWITCH_PRO_CTRL:
						case DX_PADTYPE_SWITCH_HORI_PAD:
							GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
							{
								pp_x = std::clamp(-(float)(-input.Rz) / 100.f*0.5f, -9.f, 9.f) * cam_per;
								pp_y = std::clamp((float)(input.Z) / 100.f*0.5f, -9.f, 9.f) * cam_per;

								float deg = rad2deg(atan2f((float)input.X, -(float)input.Y));
								bool w_key = false;
								bool s_key = false;
								bool a_key = false;
								bool d_key = false;
								if (!(input.X == 0 && input.Y == 0)) {
									w_key = (-50.f <= deg && deg <= 50.f);
									a_key = (-140.f <= deg && deg <= -40.f);
									s_key = (130.f <= deg || deg <= -130.f);
									d_key = (40.f <= deg && deg <= 140.f);
								}
								//視点切り替え
								look_key = (input.Buttons[11] != 0);
								//eyechange_key = (input.Buttons[11]!=0);
								//十字
								deg = (float)(input.POV[0]) / 100.f;
								bool right_key = (40.f <= deg && deg <= 140.f);
								bool left_key = (220.f <= deg && deg <= 320.f);
								bool up_key = (310.f <= deg || deg <= 50.f);
								bool down_key = (130.f <= deg && deg <= 230.f);
								//ボタン
								//_key = (input.Buttons[0] != 0);/*△*/
								//_key = (input.Buttons[2] != 0);/*×*/
								auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
								MyInput.SetInput(
									pp_x*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().y(),
									pp_y*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().x(),
									w_key, s_key, a_key, d_key,
									(input.Buttons[10] != 0),
									(input.Buttons[6] != 0), (input.Buttons[7] != 0),
									right_key, left_key, up_key, down_key,
									(input.Buttons[10] != 0), (input.Buttons[5] != 0), (input.Buttons[4] != 0), (input.Buttons[3] != 0)/*□*/, (input.Buttons[1] != 0)/*〇*/,
									false
								);
							}
							break;
						case DX_PADTYPE_XBOX_360:
						case DX_PADTYPE_XBOX_ONE:
							break;
						default:
							break;
						}
					}
					else {//キーボード
						this->m_MouseActive.Execute(CheckHitKeyWithCheck(KEY_INPUT_TAB) != 0);
						int mx = DXDraw::Instance()->m_DispXSize / 2, my = DXDraw::Instance()->m_DispYSize / 2;
						if (this->m_MouseActive.on()) {
							if (this->m_MouseActive.trigger()) {
								SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
							}
							GetMousePoint(&mx, &my);
							SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
							SetMouseDispFlag(FALSE);
						}
						else {
							SetMouseDispFlag(TRUE);
						}
						pp_x = std::clamp((-(float)(my - DXDraw::Instance()->m_DispYSize / 2))*0.5f, -9.f, 9.f) * cam_per;
						pp_y = std::clamp(((float)(mx - DXDraw::Instance()->m_DispXSize / 2))*0.5f, -9.f, 9.f) * cam_per;
						look_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0) && this->m_MouseActive.on();
						eyechange_key = CheckHitKeyWithCheck(KEY_INPUT_V) != 0;
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						MyInput.SetInput(
							pp_x*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().y(),
							pp_y*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().x(),
							(CheckHitKeyWithCheck(KEY_INPUT_W) != 0), (CheckHitKeyWithCheck(KEY_INPUT_S) != 0), (CheckHitKeyWithCheck(KEY_INPUT_A) != 0), (CheckHitKeyWithCheck(KEY_INPUT_D) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_Q) != 0), (CheckHitKeyWithCheck(KEY_INPUT_E) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0), (CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0), (CheckHitKeyWithCheck(KEY_INPUT_UP) != 0), (CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0),

							(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_R) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_X) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_C) != 0),
							(((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0) && this->m_MouseActive.on()),
							(((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) && this->m_MouseActive.on())
						);
					}

					this->m_FPSActive.Execute(eyechange_key);
					if (look_key) {
						this->m_LookMode = 1;
					}
					else {
						this->m_LookMode = 0;
					}
					Easing(&this->m_TPS_Per, (!this->m_FPSActive.on() && (this->m_LookMode == 1)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

					this->m_TPS_Xrad += pp_x;
					this->m_TPS_Yrad += pp_y;
					this->m_TPS_Xrad = std::clamp(this->m_TPS_Xrad, deg2rad(-60), deg2rad(60));
					if (this->m_TPS_Yrad >= deg2rad(180)) { this->m_TPS_Yrad -= deg2rad(360); }
					if (this->m_TPS_Yrad <= deg2rad(-180)) { this->m_TPS_Yrad += deg2rad(360); }

					this->m_TPS_Xrad *= this->m_TPS_Per;
					this->m_TPS_Yrad *= this->m_TPS_Per;

					Easing(&this->m_TPS_XradR, m_TPS_Xrad, 0.5f, EasingType::OutExpo);

					this->m_TPS_YradR += (sin(this->m_TPS_Yrad)*cos(this->m_TPS_YradR) - cos(this->m_TPS_Yrad) * sin(this->m_TPS_YradR))*20.f / FPS;
					MyInput.SetRadBuf(PlayerMngr->GetPlayer(GetMyPlayerID()).GetRadBuf());
					//ネットワーク
					m_NetWorkBrowser.FirstExecute(MyInput, PlayerMngr->GetPlayer(GetMyPlayerID()).GetNetSendMove());
					//クライアント
					if (m_NetWorkBrowser.GetClient()) {
						for (auto& c : this->character_Pool) {
							if (c->GetMyPlayerID() == GetMyPlayerID()) {
								c->SetUseRealTimePhysics(false);
								//c->SetUseRealTimePhysics(true);
								c->SetCharaType(CharaTypeID::Mine);
							}
							else {
								c->SetUseRealTimePhysics(false);
								c->SetCharaType(CharaTypeID::Enemy);
							}
						}
					}
					//
					bool isready = true;
					for (int i = 0; i < Player_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
						if (m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
							auto tmp = this->m_NetWorkBrowser.GetNowServerPlayerData(i, false);
							if (i == GetMyPlayerID()) {
								MyInput.SetKeyInput(tmp.Input.GetKeyInput());//キーフレームだけサーバーに合わせる
								c->SetInput(MyInput, isready);
								m_NetWorkBrowser.GetRecvData(i, tmp.Frame);
							}
							else {
								c->SetInput(tmp.Input, isready);
								bool override_true = true;
								override_true = (tmp.CalcCheckSum() != 0);
								if (override_true) {
									c->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.radBuf);
								}

							}
							//ダメージイベント処理
							if (ObjMngr->GetObj(ObjType::Human, i) != nullptr) {
								if (tmp.DamageSwitch != c->GetDamageSwitchRec()) {
									this->m_DamageEvents.emplace_back(tmp.Damage);
									c->SetDamageSwitchRec(tmp.DamageSwitch);
								}
							}
						}
						else {
							if (i == GetMyPlayerID()) {
								c->SetInput(MyInput, isready);
							}
							//ダメージイベント処理
							if (ObjMngr->GetObj(ObjType::Human, i) != nullptr) {
								if (c->GetDamageSwitch() != c->GetDamageSwitchRec()) {
									this->m_DamageEvents.emplace_back(c->GetDamageEvent());
									c->SetDamageSwitchRec(c->GetDamageSwitch());
								}
							}
						}
					}
					m_NetWorkBrowser.LateExecute();
					//ダメージイベント
					for (auto& c : this->character_Pool) {
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
				//
				for (int j = 0; j < gun_num; j++) {
					auto& Gun = (std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, j));
					if (Gun->GetIsShot()) {
						//エフェクト
						auto mat = Gun->GetMuzzleMatrix();
						//EffectControl::SetOnce(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 1.f);
					}
				}
				//いらないオブジェクトの除去
				ObjMngr->DeleteCheck();
				//弾の更新
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
							if (a->IsActive()) {
								//AmmoClass
								MV1_COLL_RESULT_POLY ColResGround = a->ColCheckGround();
								VECTOR_ref pos_tmp = a->GetMove().pos;
								VECTOR_ref norm_tmp;
								bool is_HitAll = false;
								auto& c = *ObjMngr->GetObj(ObjType::Human, a->GetShootedID());//(std::shared_ptr<CharacterClass>&)
								for (auto& tgt : this->character_Pool) {
									if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
									auto res = tgt->CheckAmmoHit(a.get(), c->GetMove().pos);
									is_HitAll |= res.first;
									if (res.second) { break; }
								}
								if ((ColResGround.HitFlag == TRUE) && !is_HitAll) {
									a->HitGround(pos_tmp);
									if (ColResGround.HitFlag == TRUE) {
										EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, ColResGround.HitPosition, ColResGround.Normal, a->GetCaliberSize() / 0.1f * Scale_Rate);
									}
								}
							}
						}
						else {
							break;
						}
						loop++;
					}
				}
				this->m_BackGround->FirstExecute();
				ObjMngr->LateExecuteObject();
				//視点
				{
					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					if (Chara->GetSendCamShake()) {
						this->m_CamShake = 1.f;
					}
					Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(this->m_CamShake), GetRandf(this->m_CamShake), GetRandf(this->m_CamShake)), 0.8f, EasingType::OutExpo);
					Easing(&this->m_CamShake2, m_CamShake1, 0.8f, EasingType::OutExpo);
					this->m_CamShake = std::max(this->m_CamShake - 1.f / FPS, 0.f);

					if (this->m_FPSActive.on()) {
						VECTOR_ref CamPos = Lerp(Chara->GetEyePosition(), Chara->GetScopePos(), Chara->GetADSPer());
						SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeVector(), Chara->GetMatrix().yvec());
					}
					else {
						MATRIX_ref FreeLook;
						FreeLook = MATRIX_ref::RotAxis(Chara->GetMatrix().xvec(), m_TPS_XradR) * MATRIX_ref::RotAxis(Chara->GetMatrix().yvec(), m_TPS_YradR);
						Easing_Matrix(&m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

						VECTOR_ref CamVec = Lerp(Chara->GetEyeVector(), MATRIX_ref::Vtrans(Chara->GetEyeVector(), m_FreeLookMat), m_TPS_Per);

						MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot();
						VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * 14.f;
						CamPos += Lerp((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), m_EyeRunPer);

						SetMainCamera().SetCamPos(
							CamPos + CamVec * Lerp(Lerp(-20.f, -50.f, m_TPS_Per), 2.f, m_EyePosPer_Prone),
							CamPos + CamVec * 100.f,
							Chara->GetEyeVecMat().yvec() + this->m_CamShake2 * 0.25f);
					}
					Easing(&this->m_EyeRunPer, Chara->GetIsRun() ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
					auto fov_t = GetMainCamera().GetCamFov();
					auto near_t = GetMainCamera().GetCamNear();
					auto far_t = GetMainCamera().GetCamFar();
					if (this->m_FPSActive.on()) {
						if (Chara->GetIsADS()) {
							Easing(&near_t, Scale_Rate * 1.f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 40.f, 0.9f, EasingType::OutExpo);
						}
						else {
							Easing(&near_t, Scale_Rate * 0.5f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 20.f, 0.9f, EasingType::OutExpo);
						}
					}
					else {
						if (Chara->GetIsADS()) {
							Easing(&near_t, 10.f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 30.f, 0.9f, EasingType::OutExpo);
						}
						else if (Chara->GetIsRun()) {
							Easing(&near_t, 3.f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 15.f, 0.9f, EasingType::OutExpo);
						}
						else {
							Easing(&near_t, 10.f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 30.f, 0.9f, EasingType::OutExpo);
						}
					}
					//fov
					if (this->m_FPSActive.on()) {
						fov_t = Chara->GetFov();
					}
					else {
						float fov = 0;
						if (Chara->GetIsADS()) {
							fov = deg2rad(50);
						}
						else if (Chara->GetIsRun()) {
							fov = deg2rad(70);
						}
						else {
							fov = deg2rad(55);
						}
						Easing(&fov_t, fov, 0.9f, EasingType::OutExpo);
					}
					SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
				}
				this->m_BackGround->Execute();
				//レーザーサイト
				for (auto& c : this->character_Pool) {
					c->SetLaser(&character_Pool);
					VECTOR_ref CamPos = Lerp(c->GetEyePosition(), c->GetScopePos(), c->GetADSPer());
					VECTOR_ref Laserpos = GetScreenPos(CamPos, CamPos + c->GetEyeVector(), c->GetMatrix().yvec(), c->GetFov(), c->GetLaser());
					if (0.f < Laserpos.z() && Laserpos.z() < 1.f) {
						c->SetLaser2D(Laserpos);
					}
				}

				//UIパラメーター
				{
					this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
					this->m_ScoreBuf += std::clamp((PlayerMngr->GetPlayer(0).GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(2, 1);

					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					this->m_UIclass.SetIntParam(3, (int)Chara->GetHP());
					this->m_UIclass.SetIntParam(4, (int)Chara->GetHPMax());
					this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
					this->m_HPBuf += std::clamp((Chara->GetHP() - this->m_HPBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(6, (int)1.f);
					this->m_UIclass.SetIntParam(7, (int)1.f);
					this->m_UIclass.SetIntParam(8, (int)1.f);


					this->m_UIclass.SetIntParam(12, (int)0);//現在選択
					this->m_UIclass.SetIntParam(13, (int)1);//銃の総数
					{
						this->m_UIclass.SetStrParam(1, Chara->GetGunPtrNow()->GetName().c_str());
						this->m_UIclass.SetIntParam(14, (int)(Chara->GetAmmoNum()));
						this->m_UIclass.SetIntParam(15, (int)(Chara->GetAmmoAll()));
						this->m_UIclass.SetItemGraph(0, &Chara->GetGunPtrNow()->GetGunPic());
					}
					{
						auto Next = (int)((Chara->GetGunPtrNowID() + 1) % Chara->GetGunPtrNum());
						this->m_UIclass.SetStrParam(2, Chara->GetGunPtr(Next)->GetName().c_str());
						this->m_UIclass.SetItemGraph(1, &Chara->GetGunPtr(Next)->GetGunPic());
					}
					/*
					{
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						this->m_UIclass.SetStrParam(3, Chara->GetGunPtrNow()->GetName().c_str());
						this->m_UIclass.SetItemGraph(2, &Chara->GetGunPtrNow()->GetGunPic());
					}
					//*/
				}
				EffectControl::Execute();
				//オートエイム
				for (auto& Chara : this->character_Pool) {
					Chara->SetAutoAim();
					if (!Chara->GetIsADS()) {
						bool CanAutoAim = false;
						VECTOR_ref AimPos;
						{
							auto MyPos = Chara->GetGunPtrNow()->GetMuzzleMatrix().pos();
							float lenBuf = 1000.f*Scale_Rate;
							for (const auto& c : this->character_Pool) {
								if (Chara->GetMyPlayerID() == c->GetMyPlayerID()) { continue; }
								auto TargetPos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
								VECTOR_ref CamPos = Lerp(Chara->GetEyePosition(), Chara->GetScopePos(), Chara->GetADSPer());
								VECTOR_ref campos = GetScreenPos(CamPos,CamPos + Chara->GetEyeVector(), Chara->GetMatrix().yvec(), Chara->GetFov(), TargetPos);
								if (0.f < campos.z() && campos.z() < 1.f) {
									campos = campos - Chara->GetAimPoint();
									campos.z(0.f);
									if (campos.Length() <= y_r(128) * Chara->GetActiveAutoScale()) {
										if (!this->m_BackGround->CheckLinetoMap(MyPos, &TargetPos, false)) {
											auto vec = (TargetPos - MyPos);
											if (lenBuf >= vec.Length()) {
												lenBuf = vec.Length();
												AimPos = TargetPos;
												CanAutoAim = true;
											}
										}

									}
								}
							}
						}
						if (CanAutoAim) {
							Chara->SetAutoAim(&AimPos);
						}
					}
				}
				//
				for (auto& c : this->character_Pool) {
					VECTOR_ref campos; campos.z(-1.f);
					c->SetCameraPosition(campos);
				}
				return true;
			}
			void			Dispose_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();

				m_NetWorkBrowser.Dispose();
				EffectControl::Dispose();
				PlayerMngr->Dispose();
				ObjMngr->DisposeObject();
				this->m_BackGround->Dispose();
			}
			//
			void			Depth_Draw_Sub(void) noexcept override {
				//auto* ObjMngr = ObjectManager::Instance();
				//auto* PlayerMngr = PlayerManager::Instance();

				this->m_BackGround->Draw();
				//ObjMngr->DrawDepthObject();
			}
			void			BG_Draw_Sub(void) noexcept override {
				this->m_BackGround->BG_Draw();
			}
			void			ShadowDraw_NearFar_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_NearFar();
			}
			void			ShadowDraw_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				//this->m_BackGround->Shadow_Draw();
				ObjMngr->DrawObject_Shadow();
			}
			void			MainDraw_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				SetFogStartEnd(Scale_Rate * 10.f, Scale_Rate * 50.f);
				this->m_BackGround->Draw();
				ObjMngr->DrawObject();
				//ObjMngr->DrawDepthObject();
				//シェーダー描画用パラメーターセット
				{
					//
					Set_is_Blackout(true);
					Set_Per_Blackout(0.5f + (1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));
					//
					Set_is_lens(Chara->GetIsADS() && Chara->GetReticleSize()>1.f);
					if (Chara->GetIsADS()) {
						VECTOR_ref LensPos = ConvWorldPosToScreenPos(Chara->GetLensPos().get());
						if (0.f < LensPos.z() && LensPos.z() < 1.f) {
							Set_xp_lens(LensPos.x());
							Set_yp_lens(LensPos.y());
							LensPos = ConvWorldPosToScreenPos(Chara->GetLensPosSize().get());
							if (0.f < LensPos.z() && LensPos.z() < 1.f) {
								Set_size_lens(std::hypotf(xp_lens() - LensPos.x(), yp_lens() - LensPos.y()));
							}
						}
						LensPos = ConvWorldPosToScreenPos(Chara->GetReticlePos().get());
						if (0.f < LensPos.z() && LensPos.z() < 1.f) {
							Reticle_xpos = LensPos.x();
							Reticle_ypos = LensPos.y();
							Reticle_on = (size_lens() > std::hypotf(xp_lens() - Reticle_xpos, yp_lens() - Reticle_ypos));
						}
					}
					else {
						Reticle_on = false;
					}
					Set_zoom_lens(Chara->GetReticleSize());
				}
				for (auto& c : this->character_Pool) {
					if (c->GetMyPlayerID() == GetMyPlayerID()) { continue; }
					auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						c->SetCameraPosition(campos);
						c->SetCameraSize(std::max(20.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
					}

				}
				{
					VECTOR_ref Laserpos2D = ConvWorldPosToScreenPos(Chara->GetLaser().get());
					if (0.f < Laserpos2D.z() && Laserpos2D.z() < 1.f) {
						m_Laserpos2D = Laserpos2D;
					}
				}
				for (auto& c : this->character_Pool) {
					c->DrawLaser();
				}
			}
			void			MainDrawbyDepth_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				ObjMngr->DrawDepthObject();
			}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept  override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//auto* Fonts = FontPool::Instance();
				//auto* DrawParts = DXDraw::Instance();
				//auto Red = GetColor(255, 0, 0);
				//auto Blue = GetColor(50, 50, 255);
				//auto Green = GetColor(43, 163, 91);
				//auto White = GetColor(212, 255, 239);
				//auto Gray = GetColor(64, 64, 64);
				//auto Black = GetColor(0, 0, 0);
				//unsigned int color = Red;
				//
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
							a->Draw_Hit_UI(hit_Graph);
						}
						else {
							break;
						}
						loop++;
					}
				}
				//UI
				this->m_UIclass.Draw();
				if (!Chara->GetIsADS()) {
					if (0.f < m_Laserpos2D.z() && m_Laserpos2D.z() < 1.f) {
						aim_Graph.DrawRotaGraph((int)m_Laserpos2D.x(), (int)m_Laserpos2D.y(), (float)(y_r(100)) / 100.f*Chara->GetActiveAutoScale(), m_AimRot, true);
					}
				}
				m_AimRot += 60.f / FPS;
				//通信設定
				if (!this->m_MouseActive.on()) {
					m_NetWorkBrowser.Draw();
				}
				//
			}
			void			DrawUI_In_Sub(void) noexcept override {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//レティクル表示
				if (Reticle_on) {
					int x, y;
					Chara->GetGunPtrNow()->GetReticlePic().GetSize(&x, &y);
					Chara->GetGunPtrNow()->GetReticlePic().DrawRotaGraph((int)Reticle_xpos, (int)Reticle_ypos, size_lens() / (y / 2.f), Chara->GetReticleRad(), true);
				}
			}
		};
	};
};
