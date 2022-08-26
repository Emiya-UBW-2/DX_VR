#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//通信
		enum class SequenceEnum {
			SelMode,
			CheckPreset,
			Set_Port,
			SetIP,
			SetTick,
			Matching,
			MainGame,
		};


		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			static const int		Chara_num = 3;
			static const int		Vehicle_num = 3;

			static const int		gun_num = Chara_num;
			static const int		cart_num = 2;
		private:
			//リソース関連
			BackGroundClass			m_BackGround;				//BG
			SoundHandle				m_Env;
			MV1						hit_pic;					//弾痕  
			//戦車データ
			std::vector<VhehicleData>	vehc_data;
			std::vector<std::shared_ptr<VehicleClass>> vehicle_Pool;	//戦車のベクター
			//操作関連
			float					m_EyeRunPer{ 0.f };
			switchs					m_FPSActive;
			switchs					m_MouseActive;
			switchs					m_LeftClick;
			float					m_LeftPressTimer{ 0.f };
			int						m_LookMode{ 0 };
			//UI関連
			UIClass					m_UIclass;
			float					m_HPBuf{ 0.f };
			float					m_ScoreBuf{ 0.f };
			GraphHandle				hit_Graph;
			//
			float					m_CamShake{ 0.f };
			VECTOR_ref				m_CamShake1;
			VECTOR_ref				m_CamShake2;
			//
			MATRIX_ref				m_FreeLookMat;
			float					m_TPS_Xrad{ 0.f };
			float					m_TPS_Yrad{ 0.f };
			float					m_TPS_XradR{ 0.f };
			float					m_TPS_YradR{ 0.f };
			float					m_TPS_Per{ 1.f };
			//
			float					m_Rader{ 1.f };
			float					m_Rader_r{ 1.f };
			//
			std::vector<DamageEvent>	m_DamageEvents;
			bool IsRide = false;

			//サーバー専用
			ServerControl			m_ServerCtrl;
			//クライアント専用
			ClientControl			m_ClientCtrl;
			IPDATA					IPData{ 127,0,0,1 };				// 送信用ＩＰアドレスデータ
			//共通
			bool					m_IsClient{ true };
			SequenceEnum			m_Sequence{ SequenceEnum::SelMode };
			bool					SeqFirst{ false };
			float					m_Tick{ 10.f };
			NewWorkSetting			m_NewWorkSetting;
			int						m_NewWorkSelect{ 0 };
			NewSetting				m_NewSetting;
			double					m_ClientFrame{ 0.0 };
			float					m_Ping{ 0.f };
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return (this->m_IsClient) ? this->m_ClientCtrl.GetMyPlayer().ID : this->m_ServerCtrl.GetMyPlayer().ID; }
		public:
			using TEMPSCENE::TEMPSCENE;
			void			Set(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				Set_EnvLight(
					VECTOR_ref::vget(Scale_Rate*-300.f, Scale_Rate*-10.f, Scale_Rate*-300.f),
					VECTOR_ref::vget(Scale_Rate*300.f, Scale_Rate*50.f, Scale_Rate*300.f),
					VECTOR_ref::vget(-0.25f, -0.5f, 0.0f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				//Load
				//BG
				this->m_BackGround.Init();
				//
				auto data_t = GetFileNamesInDirectory("data/tank/");
				for (auto& d : data_t) {
					if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						this->vehc_data.resize(this->vehc_data.size() + 1);
						this->vehc_data.back().Set_Pre(d.cFileName);
					}
				}
				for (auto& t : this->vehc_data) { t.Set(); }										//戦車2
				//

				ObjMngr->Init(&this->m_BackGround.GetGroundCol());
				for (int i = 0; i < Chara_num; i++) {
					ObjMngr->AddObject(ObjType::Human,"data/Charactor/WinningTicket/");
				}
				for (int i = 0; i < Vehicle_num; i++) {
					ObjMngr->AddObject(ObjType::Vehicle);
				}

				for (int i = 0; i < gun_num; i++) {
					ObjMngr->AddObject(ObjType::Gun, "data/gun/Mosin/");
					ObjMngr->AddObject(ObjType::Magazine, "data/gun/Mosin/", "model_mag");

					auto& m = (std::shared_ptr<MagazineClass>&)(*ObjMngr->GetObj(ObjType::Magazine, i));
					m->GetAmmoAll();
					for (int j = 0; j < cart_num; j++) {
						ObjMngr->AddObject(ObjType::Cart, "data/gun/Mosin/", "ammo");
					}
				}

				//ロード
				SetCreate3DSoundFlag(FALSE);
				this->m_Env = SoundHandle::Load("data/Sound/SE/envi.wav");
				SetCreate3DSoundFlag(FALSE);
				this->m_Env.vol(64);
				//
				MV1::Load("data/model/hit/model.mv1", &this->hit_pic);						//弾痕モデル
				//UI
				this->m_UIclass.Set();

				hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				//
				TEMPSCENE::Set();
				//Set
				//人
				for (int i = 0; i < Chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
					VECTOR_ref pos_t = VECTOR_ref::vget(0.f + (float)(i)*20.f, 0.f, 0.f);
					auto HitResult = this->m_BackGround.GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
					if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
					c->SetGunPtr((std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, i)));
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), false, false, pos_t);
					if (i == 0) {
						c->SetUseRealTimePhysics(false);
						c->SetCharaType(CharaTypeID::Team);
					}
					else {
						c->SetUseRealTimePhysics(false);
						c->SetCharaType(CharaTypeID::Enemy);
					}
				}
				for (int i = 0; i < gun_num; i++) {
					auto& g = (std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, i));
					auto& m = (std::shared_ptr<MagazineClass>&)(*ObjMngr->GetObj(ObjType::Magazine, i));
					g->SetMagPtr(m);
					for (int j = 0; j < cart_num; j++) {
						m->SetCartPtr((std::shared_ptr<CartClass>&)(*ObjMngr->GetObj(ObjType::Cart, i*cart_num + j)));
					}
				}
				//登録
				for (int i = 0; i < Vehicle_num; i++) {
					auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
					vehicle_Pool.emplace_back(v);

					VECTOR_ref pos_t = VECTOR_ref::vget(0.f + (float)(i)*10.f*Scale_Rate, 0.f, 0.f);
					auto HitResult = this->m_BackGround.GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
					if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
					v->ValueInit(&vehc_data[0], hit_pic, this->m_BackGround.GetBox2Dworld(), (PlayerID)i);
					v->ValueSet(deg2rad(0), deg2rad(90), pos_t);
				}
				{
					auto& Chara = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, 0));//自分
					auto& Vehicle = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, 0));//自分
					if (IsRide) {
						this->m_HPBuf = (float)Vehicle->GetHP();
						//this->m_ScoreBuf = Vehicle->GetScore();
					}
				}
				//Cam
				camera_main.set_cam_info(deg2rad(65), 1.f, 100.f);
				camera_main.set_cam_pos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				Set_zoom_lens(3.5f);
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
				//SE->Add((int)SoundEnum::GateOpen, 1, "data/Sound/SE/GateOpen.wav");
				SE->Add((int)SoundEnum::Tank_Shot, 3, "data/Sound/SE/gun/fire/7.wav");
				for (int i = 0; i < 17; i++) {
					SE->Add((int)SoundEnum::Tank_Ricochet, 3, "data/Sound/SE/ricochet/" + std::to_string(i) + ".wav");
				}
				for (int i = 0; i < 2; i++) {
					SE->Add((int)SoundEnum::Tank_Damage, 3, "data/Sound/SE/damage/" + std::to_string(i) + ".wav");
				}
				SE->Add((int)SoundEnum::Tank_engine, 10, "data/Sound/SE/engine.wav");
				for (int i = 0; i < 7; i++) {
					SE->Add((int)SoundEnum::Tank_Eject, 3, "data/Sound/SE/gun/reload/eject/" + std::to_string(i) + ".wav", false);
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Tank_Reload, 3, "data/Sound/SE/gun/reload/hand/" + std::to_string(i) + ".wav", false);
				}

				SE->Get((int)SoundEnum::Shot_Gun).SetVol_Local(128);
				SE->Get((int)SoundEnum::Trigger).SetVol_Local(128);
				for (int i = 0; i < 4; i++) {
					SE->Get((int)SoundEnum::Cocking0 + i).SetVol_Local(128);
				}
				SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
				SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
				//SE->Get((int)SoundEnum::GateOpen).SetVol_Local(128);

				SE->Get((int)SoundEnum::Tank_Shot).SetVol(0.5f);
				SE->Get((int)SoundEnum::Tank_engine).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Ricochet).SetVol(0.65f);
				SE->Get((int)SoundEnum::Tank_Damage).SetVol(0.65f);
				SE->Get((int)SoundEnum::Tank_Eject).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Reload).SetVol(0.25f);


				//入力
				this->m_FPSActive.Init(false);
				this->m_MouseActive.Init(false);

				this->m_DamageEvents.clear();

				m_NewWorkSetting.Load();
				m_NewWorkSelect = 0;
			}
			//
			bool			Update(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				//FirstDoingv
				if (IsFirstLoop) {
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
					this->m_Env.play(DX_PLAYTYPE_LOOP, TRUE);
				}
				//Input,AI
				{
					auto& Chara = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, GetMyPlayerID()));//自分
					auto& Vehicle = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, GetMyPlayerID()));//自分
					float cam_per = ((camera_main.fov / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					float pp_x = 0.f, pp_y = 0.f;
					bool look_key = false;
					bool eyechange_key = false;
					bool Lockon_key = false;
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
								pp_x = std::clamp(-(float)(-input.Rz) / 100.f*1.f, -9.f, 9.f) * cam_per;
								pp_y = std::clamp((float)(input.Z) / 100.f*1.f, -9.f, 9.f) * cam_per;

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
								Lockon_key = (input.Buttons[0] != 0);/*△*/
								//_key = (input.Buttons[2] != 0);/*×*/
								MyInput.SetInput(
									pp_x*(1.f - this->m_TPS_Per), pp_y*(1.f - this->m_TPS_Per),
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
						this->m_MouseActive.GetInput(CheckHitKeyWithCheck(KEY_INPUT_TAB) != 0);
						int mx = DXDraw::Instance()->disp_x / 2, my = DXDraw::Instance()->disp_y / 2;
						if (this->m_MouseActive.on()) {
							if (this->m_MouseActive.trigger()) {
								SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
							}
							GetMousePoint(&mx, &my);
							SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
							SetMouseDispFlag(FALSE);
						}
						else {
							SetMouseDispFlag(TRUE);
						}
						pp_x = std::clamp(-(float)(my - DXDraw::Instance()->disp_y / 2)*1.f, -9.f, 9.f) * cam_per;
						pp_y = std::clamp((float)(mx - DXDraw::Instance()->disp_x / 2)*1.f, -9.f, 9.f) * cam_per;
						look_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) && this->m_MouseActive.on();
						eyechange_key = CheckHitKeyWithCheck(KEY_INPUT_V) != 0;
						Lockon_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0) && this->m_MouseActive.on();
						MyInput.SetInput(
							pp_x*(1.f - this->m_TPS_Per), pp_y*(1.f - this->m_TPS_Per),
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

					this->m_LeftClick.GetInput((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
					if (!this->m_LeftClick.press()) {
						this->m_LeftPressTimer = 0.f;
					}
					this->m_FPSActive.GetInput(eyechange_key);
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

					Easing(&this->m_TPS_XradR, this->m_TPS_Xrad, 0.5f, EasingType::OutExpo);

					this->m_TPS_YradR += (sin(this->m_TPS_Yrad)*cos(this->m_TPS_YradR) - cos(this->m_TPS_Yrad) * sin(this->m_TPS_YradR))*20.f / FPS;
					if (!IsRide) {
						Chara->SetEyeVec((camera_main.camvec - camera_main.campos).Norm());
						MyInput.SetRadBuf(Chara->GetRadBuf().x(), Chara->GetRadBuf().y());
					}
					else {
						MyInput.SetRadBuf(Vehicle->GetViewxRad(), Vehicle->GetViewyRad());
					}
					//クライアント
					if (this->m_IsClient) {
						if (!IsRide) {
							auto vec = Chara->GetMove().vec;
							vec.y(0);
							m_ClientCtrl.SetMyPlayer(MyInput, Chara->GetMove().pos, vec, Chara->GetRadBuf().y(), this->m_ClientFrame, nullptr, (0));
						}
						else {
							m_ClientCtrl.SetMyPlayer(MyInput, Vehicle->GetMove().pos, Vehicle->GetMove().vec, Vehicle->Get_body_yrad(), this->m_ClientFrame, &Vehicle->GetDamageEvent(), (Vehicle->GetDamageSwitch() ? 1 : 0));
						}
						if ((this->m_Sequence == SequenceEnum::Matching) && SeqFirst) {
							m_ClientCtrl.ClientInit(this->m_NewSetting.UsePort, this->m_Tick, IPData);
						}
						if ((this->m_Sequence >= SequenceEnum::Matching) && this->m_ClientCtrl.ClientExecute()) {
							this->m_Sequence = SequenceEnum::MainGame;
						}
						for (int i = 0; i < Vehicle_num; i++) {
							auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
							if (i == GetMyPlayerID()) {
								v->SetCharaType(CharaTypeID::Mine);
							}
							else {
								v->SetCharaType(CharaTypeID::Enemy);
							}
						}
					}
					//サーバー
					else {
						if (!IsRide) {
							auto vec = Chara->GetMove().vec;
							vec.y(0);
							m_ServerCtrl.SetMyPlayer(MyInput, Chara->GetMove().pos, vec, Chara->GetRadBuf().y(), this->m_ClientFrame, nullptr, (0));
						}
						else {
							m_ServerCtrl.SetMyPlayer(MyInput, Vehicle->GetMove().pos, Vehicle->GetMove().vec, Vehicle->Get_body_yrad(), this->m_ClientFrame, &Vehicle->GetDamageEvent(), (Vehicle->GetDamageSwitch() ? 1 : 0));
						}
						if ((this->m_Sequence == SequenceEnum::Matching) && SeqFirst) {
							m_ServerCtrl.ServerInit(this->m_NewSetting.UsePort, this->m_Tick);
						}
						if ((this->m_Sequence >= SequenceEnum::Matching) && this->m_ServerCtrl.ServerExecute()) {
							this->m_Sequence = SequenceEnum::MainGame;
						}
					}
					//
					bool isready = true;
					if (this->m_Sequence == SequenceEnum::MainGame) {
						for (int i = 0; i < Chara_num; i++) {
							auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
							auto tmp = (this->m_IsClient) ? this->m_ClientCtrl.GetNowServerPlayerData(i) : this->m_ServerCtrl.GetNowServerPlayerData(i);
							if (i == GetMyPlayerID()) {
								if (!IsRide) {
									MyInput.SetKeyInput(tmp.Input.GetKeyInput());//キーフレームだけサーバーに合わせる
									c->SetInput(MyInput, isready);
									if ((this->m_IsClient) ? this->m_ClientCtrl.GetRecvData(i) : this->m_ServerCtrl.GetRecvData(i)) {
										this->m_Ping = (float)(this->m_ClientFrame - tmp.Frame)*1000.f;
									}
									printfDx("ping %lf \n", this->m_Ping);
								}
							}
							else {
								if (!IsRide) {
									c->SetInput(tmp.Input, isready);
									bool override_true = (tmp.CalcCheckSum()!=0);
									if (override_true) {
										c->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf);
									}
								}
							}
						}
						for (int i = 0; i < Vehicle_num; i++) {
							auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
							auto tmp = (this->m_IsClient) ? this->m_ClientCtrl.GetNowServerPlayerData(i) : this->m_ServerCtrl.GetNowServerPlayerData(i);
							if (i == GetMyPlayerID()) {
								if (IsRide) {
									MyInput.SetKeyInput(tmp.Input.GetKeyInput());//キーフレームだけサーバーに合わせる
									v->SetInput(MyInput, isready, false);
									if ((this->m_IsClient) ? this->m_ClientCtrl.GetRecvData(i) : this->m_ServerCtrl.GetRecvData(i)) {
										this->m_Ping = (float)(this->m_ClientFrame - tmp.Frame)*1000.f;
									}
									printfDx("ping %lf \n", this->m_Ping);
								}
							}
							else {
								if (IsRide) {
									v->SetInput(tmp.Input, isready, true);
									bool override_true = true;
									for (int j = 0; j < Vehicle_num; j++) {
										auto& v2 = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, j));
										if (v != v2) {
											if ((v->GetMove().pos - v2->GetMove().pos).size() <= 10.f*Scale_Rate) {
												override_true = false;
												break;
											}
										}
									}
									if (override_true) {
										v->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.YradBuf);
									}
								}
							}
							//ダメージイベント処理
							{
								if (tmp.DamageSwitch != v->GetDamageSwitchRec()) {
									this->m_DamageEvents.emplace_back(tmp.Damage);
									v->SetDamageSwitchRec(tmp.DamageSwitch);
								}
							}
						}
						this->m_ClientFrame += 1.0 / (double)FPS;
					}
					else {
						for (int i = 0; i < Chara_num; i++) {
							auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
							if (i == GetMyPlayerID()) {
								if (!IsRide) {
									c->SetInput(MyInput, isready);
								}
							}
						}
						for (int i = 0; i < Vehicle_num; i++) {
							auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
							if (i == GetMyPlayerID()) {
								if (IsRide) {
									v->SetInput(MyInput, isready, false);
								}
							}
							//ダメージイベント処理
							{
								if (v->GetDamageSwitch() != v->GetDamageSwitchRec()) {
									this->m_DamageEvents.emplace_back(v->GetDamageEvent());
									v->SetDamageSwitchRec(v->GetDamageSwitch());
								}
							}
						}
						this->m_ClientFrame = 0.0;
					}
					//ダメージイベント
					for (int i = 0; i < Vehicle_num; i++) {
						auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
						for (int j = 0; j < this->m_DamageEvents.size(); j++) {
							if (v->SetDamageEvent(this->m_DamageEvents[j])) {
								std::swap(this->m_DamageEvents.back(), this->m_DamageEvents[j]);
								this->m_DamageEvents.pop_back();
								j--;
							}
						}
					}
				}
				//
				{
					auto& Vehicle = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, GetMyPlayerID()));//自分
					auto StartPos = Vehicle->GetGunMuzzlePos();
					auto EndPos = StartPos + Vehicle->GetGunMuzzleVec() * 500.f*Scale_Rate;
					Vehicle->GetMapColNearest(StartPos, &EndPos);
					while (true) {
						auto colres = this->m_BackGround.GetGroundCol().CollCheck_Line(StartPos, EndPos);
						if (colres.HitFlag == TRUE) {
							if (EndPos == colres.HitPosition) { break; }
							EndPos = colres.HitPosition;
						}
						else {
							break;
						}
					}
					for (int i = 0; i < Vehicle_num; i++) {
						auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
						if (i != GetMyPlayerID()) {
							if (v->RefreshCol(StartPos, EndPos, 10.f*Scale_Rate)) {
								v->GetColNearestInAllMesh(StartPos, &EndPos);
							}
							v->SetAimingDistance(-1.f);
						}
					}
					Vehicle->SetAimingDistance((StartPos - EndPos).size());
				}
				//Execute
				ObjMngr->ExecuteObject();
				//
				for (int j = 0; j < gun_num; j++) {
					auto& Gun = (std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, j));
					if (Gun->GetIsShot()) {
						//エフェクト
						auto mat = Gun->GetMuzzleMatrix();
						Effect_UseControl::SetOnce(Effect::ef_fire2, mat.pos(), mat.GetRot().zvec()*-1.f, 1.f);
					}
				}
				//弾の更新
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
							if (!a->IsActive() && !a->GetHitPicActive()) {
								ObjMngr->DelObj(ObjType::Ammo, loop);
								loop--;
							}
						}
						else {
							break;
						}
						loop++;
					}
				}
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
								bool is_HitAll = false;
								auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, a->GetShootedID()));
								for (int i = 0; i < Vehicle_num; i++) {
									auto& tgt = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
									if (i == a->GetShootedID()) { continue; }
									auto res = tgt->CheckAmmoHit(a.get(), v->GetMove().pos);
									is_HitAll |= res.first;
									if (res.second) { break; }
								}
								if (ColResGround.HitFlag == TRUE && !is_HitAll) {
									a->HitGround();
									v->HitGround(a->GetMove().pos, ColResGround.Normal, a->GetMove().vec);
								}
							}
						}
						else {
							break;
						}
						loop++;
					}
				}
				this->m_BackGround.GetBox2Dworld()->Step(1.f, 1, 1);//物理更新
				ObjMngr->LateExecuteObject();
				//視点
				if (!IsRide) {
					auto& Chara = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, GetMyPlayerID()));//自分
					if (Chara->GetSendCamShake()) {
						this->m_CamShake = 1.f;
					}
					Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(this->m_CamShake), GetRandf(this->m_CamShake), GetRandf(this->m_CamShake)), 0.8f, EasingType::OutExpo);
					Easing(&this->m_CamShake2, this->m_CamShake1, 0.8f, EasingType::OutExpo);
					this->m_CamShake = std::max(this->m_CamShake - 1.f / FPS, 0.f);

					if (this->m_FPSActive.on()) {
						camera_main.campos = Chara->GetEyePosition();
						camera_main.camvec = camera_main.campos + Chara->GetEyeVecMat().zvec() * -1.f;
						camera_main.camup = Chara->GetMatrix().GetRot().yvec();
					}
					else {
						MATRIX_ref FreeLook;
						FreeLook = MATRIX_ref::RotAxis(Chara->GetMatrix().xvec(), this->m_TPS_XradR) * MATRIX_ref::RotAxis(Chara->GetMatrix().yvec(), this->m_TPS_YradR);
						Easing_Matrix(&m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

						VECTOR_ref CamVec = Lerp(Chara->GetEyeVecMat().zvec() * -1.f, MATRIX_ref::Vtrans(Chara->GetEyeVecMat().zvec() * -1.f, m_FreeLookMat), this->m_TPS_Per);

						MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot();
						VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * 14.f;
						CamPos += Lerp((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), this->m_EyeRunPer);

						camera_main.campos = (CamPos + CamVec * Lerp(-20.f, -50.f, this->m_TPS_Per)) + this->m_CamShake2 * 5.f;
						camera_main.camvec = CamPos + CamVec * 100.f;
						camera_main.camup = Chara->GetEyeVecMat().yvec() + this->m_CamShake2 * 0.25f;
					}
					Easing(&this->m_EyeRunPer, Chara->GetIsRun() ? 1.f : 0.f, 0.95f, EasingType::OutExpo);

					if (Chara->GetIsRun()) {
						Easing(&camera_main.fov, deg2rad(90), 0.9f, EasingType::OutExpo);
						Easing(&camera_main.near_, 3.f, 0.9f, EasingType::OutExpo);
						Easing(&camera_main.far_, Scale_Rate * 150.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&camera_main.fov, deg2rad(75), 0.9f, EasingType::OutExpo);
						Easing(&camera_main.near_, 10.f, 0.9f, EasingType::OutExpo);
						Easing(&camera_main.far_, Scale_Rate * 300.f, 0.9f, EasingType::OutExpo);
					}
				}
				else {
					auto& Vehicle = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, GetMyPlayerID()));//自分
					Vehicle->Setcamera(camera_main, fov_base);
					{
						MATRIX_ref FreeLook;
						FreeLook = MATRIX_ref::RotAxis((camera_main.camvec - camera_main.campos).cross(camera_main.camup), this->m_TPS_XradR) * MATRIX_ref::RotAxis(camera_main.camup, this->m_TPS_YradR);
						Easing_Matrix(&this->m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

						VECTOR_ref CamVec = (camera_main.camvec - camera_main.campos).Norm();
						CamVec = Lerp(CamVec, MATRIX_ref::Vtrans(CamVec, this->m_FreeLookMat), this->m_TPS_Per);

						VECTOR_ref CamPos = Vehicle->Get_EyePos_Base();

						camera_main.campos = Lerp(camera_main.campos, (CamPos + CamVec * -100.f), this->m_TPS_Per);
						camera_main.camvec = CamPos + CamVec * 100.f;
					}
				}

				this->m_BackGround.Execute();
				//UIパラメーター
				{
					auto& Vehicle = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, GetMyPlayerID()));//自分
					this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
					//this->m_ScoreBuf += std::clamp((Vehicle->GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(2, 1);

					this->m_UIclass.SetIntParam(3, (int)Vehicle->GetHP());
					this->m_UIclass.SetIntParam(4, (int)Vehicle->GetHPMax());
					this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
					this->m_HPBuf += std::clamp((Vehicle->GetHP() - this->m_HPBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(6, (int)1.f);
					this->m_UIclass.SetIntParam(7, (int)1.f);
					this->m_UIclass.SetIntParam(8, (int)1.f);
				}
				TEMPSCENE::Update();
				Effect_UseControl::Execute_Effect();
				return true;
			}
			void			Dispose(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				m_ServerCtrl.NetWorkDispose();
				m_ClientCtrl.NetWorkDispose();
				Effect_UseControl::Dispose_Effect();
				ObjMngr->DisposeObject();
				this->vehicle_Pool.clear();
				this->m_BackGround.Dispose();
			}
			//
			void			Depth_Draw(void) noexcept override {
				//auto* ObjMngr = ObjectManager::Instance();
				this->m_BackGround.Draw();
				//ObjMngr->DrawDepthObject();
			}
			void			BG_Draw(void) noexcept override {
				this->m_BackGround.BG_Draw();
			}
			void			Shadow_Draw_NearFar(void) noexcept override {
				//auto* ObjMngr = ObjectManager::Instance();
				this->m_BackGround.Shadow_Draw_NearFar();
				//ObjMngr->DrawObject_Shadow();
			}
			void			Shadow_Draw(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				this->m_BackGround.Shadow_Draw();
				ObjMngr->DrawObject_Shadow();
			}
			void			Main_Draw(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				this->m_BackGround.Draw();
				ObjMngr->DrawObject();
				//ObjMngr->DrawDepthObject();
				//シェーダー描画用パラメーターセット
				{
					Set_is_Blackout(false);
					Set_is_lens(false);
				}
				for (int i = 0; i < Chara_num; i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
					auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						c->SetCameraPosition(campos);
						c->SetCameraSize(std::max(20.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
					}
				}
				for (int i = 0; i < Vehicle_num; i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
					auto pos = v->Set_MidPos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						v->SetCameraPosition(campos);
						v->SetCameraSize(std::max(80.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
					}
				}
				SetFogStartEnd(camera_main.near_, camera_main.far_*3.f);
			}
			void			Main_Draw2(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				ObjMngr->DrawDepthObject();
			}
			void			LAST_Draw(void) noexcept override {
			}
			//UI表示
			void			UI_Draw(void) noexcept  override {
				auto* ObjMngr = ObjectManager::Instance();
				auto& Chara = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, GetMyPlayerID()));//自分
				auto& Vehicle = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, GetMyPlayerID()));//自分

				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto Red = GetColor(255, 0, 0);
				auto Blue = GetColor(50, 50, 255);
				auto Green = GetColor(43, 163, 91);
				auto White = GetColor(212, 255, 239);
				auto Gray = GetColor(64, 64, 64);
				auto Black = GetColor(0, 0, 0);
				unsigned int color = Red;
				//キャラ
				for (int i = 0; i < Chara_num; i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
					auto campos = c->GetCameraPosition();
					if (0.f < campos.z() && campos.z() < 1.f) {
						switch (c->GetCharaType()) {
						case CharaTypeID::Mine:
							color = Blue;
							break;
						case CharaTypeID::Team:
							color = Blue;
							break;
						case CharaTypeID::Enemy:
							color = Red;
							break;
						default:
							break;
						}
						int xp = (int)(campos.x());
						int yp = (int)(campos.y());
						int xs = (int)(100.f*c->GetCameraSize());
						int ys = (int)(100.f*c->GetCameraSize());
						int siz = y_r(std::max((int)(20.f*c->GetCameraSize()), 10));
						DrawEdgeBox_2D(xp - xs, yp - ys, xp + xs, yp + ys, color, White);
						Fonts->Get(siz, FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat_MID(xp, yp - ys - siz, color, White, "%s", c->GetName().c_str());
						Fonts->Get(siz, FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat(xp + xs, yp + ys, color, White, "%dm", (int)((c->GetMatrix().pos() - Chara->GetMatrix().pos()).size() / Scale_Rate));
						//リセット
						campos.z(-1.f);
						c->SetCameraPosition(campos);
					}
				}
				for (int i = 0; i < Vehicle_num; i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
					auto campos = v->GetCameraPosition();
					if (0.f < campos.z() && campos.z() < 1.f) {
						switch (v->GetCharaType()) {
						case CharaTypeID::Mine:
							color = Blue;
							break;
						case CharaTypeID::Team:
							color = Blue;
							break;
						case CharaTypeID::Enemy:
							color = Red;
							break;
						default:
							break;
						}
						int xp = (int)(campos.x());
						int yp = (int)(campos.y());
						int xs = (int)(100.f*v->GetCameraSize());
						int ys = (int)(100.f*v->GetCameraSize());
						int siz = y_r(std::max((int)(20.f*v->GetCameraSize()), 10));
						DrawEdgeBox_2D(xp - xs, yp - ys, xp + xs, yp + ys, color, White);
						Fonts->Get(siz, FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat_MID(xp, yp - ys - siz, color, White, "%s", v->GetName().c_str());
						Fonts->Get(siz, FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat(xp + xs, yp + ys, color, White, "%dm", (int)((v->GetMatrix().pos() - Vehicle->GetMatrix().pos()).size() / Scale_Rate));
						//リセット
						campos.z(-1.f);
						v->SetCameraPosition(campos);
					}
				}
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
				//レーダー
				{
					Easing(&this->m_Rader_r, this->m_Rader, 0.8f, EasingType::OutExpo);

					int xp1, yp1;
					int xs1, ys1, xs2, ys2;

					xs1 = y_r((int)(256.f * 0.5f));
					ys1 = y_r((int)(256.f * 0.8f));
					xs2 = y_r((int)(256.f * 0.5f));
					ys2 = y_r((int)(256.f * 0.2f));
					xp1 = DrawParts->disp_x - y_r(80) - xs1;
					yp1 = DrawParts->disp_y - y_r(300) - ys1;

					Fonts->Get(y_r(20), FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat((int)(xp1 - xs1), (int)(yp1 - ys1) - y_r(20), Green, White, "x%4.2f", this->m_Rader_r);

					DrawLine_2D((int)(xp1 - xs1), (int)(yp1), (int)(xp1 + xs2), (int)(yp1), Green, 3);
					DrawLine_2D((int)(xp1), (int)(yp1 - ys1), (int)(xp1), (int)(yp1 + ys2), Green, 3);
					DrawLine_2D((int)(xp1 - xs1), (int)(yp1), (int)(xp1 + xs2), (int)(yp1), White);
					DrawLine_2D((int)(xp1), (int)(yp1 - ys1), (int)(xp1), (int)(yp1 + ys2), White);
					DrawEdgeBox_2D((int)(xp1 - xs1), (int)(yp1 - ys1), (int)(xp1 + xs2), (int)(yp1 + ys2), White, Green);

					xs1 = y_r((int)(256.f * 0.5f*std::min(1.f, this->m_Rader_r)));
					ys1 = y_r((int)(256.f * 0.8f*std::min(1.f, this->m_Rader_r)));
					xs2 = y_r((int)(256.f * 0.5f*std::min(1.f, this->m_Rader_r)));
					ys2 = y_r((int)(256.f * 0.2f*std::min(1.f, this->m_Rader_r)));
					DrawEdgeBox_2D((int)(xp1 - xs1), (int)(yp1 - ys1), (int)(xp1 + xs2), (int)(yp1 + ys2), White, Green);

					auto BaseBos = ((!IsRide) ? Chara->GetMatrix().pos() : Vehicle->GetMatrix().pos());

					xs1 = y_r((int)(256.f * 0.5f));
					ys1 = y_r((int)(256.f * 0.8f));
					xs2 = y_r((int)(256.f * 0.5f));
					ys2 = y_r((int)(256.f * 0.2f));
					auto base = Chara->GetCharaDir().zvec()*-1.f;
					if (IsRide) {
						base = Vehicle->GetLookVec().zvec()*-1.f;
					}
					base.y(0.f);
					base = base.Norm();

					auto vec = VECTOR_ref::front();
					auto rad = std::atan2f(base.cross(vec).y(), base.dot(vec));
					{
						float BaseVPer = 2.f;

						this->m_Rader = BaseVPer;
						bool isout = true;
						auto tmpRader = 1.f;
						int div = 5;
						for (int i = 0; i < Chara_num; i++) {
							if (i == GetMyPlayerID()) { continue; }
							auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
							tmpRader = BaseVPer;
							for (int j = 0; j < div; j++) {
								auto pos = MATRIX_ref::Vtrans(c->GetMatrix().pos() - BaseBos, MATRIX_ref::RotY(rad))*((1.f / Scale_Rate) * tmpRader);
								if (((-xs1 < pos.x() && pos.x() < xs2) && (-ys1 < -pos.z() && -pos.z() < ys2))) {
									if (this->m_Rader >= tmpRader) {
										this->m_Rader = tmpRader;
									}
									isout = false;
									break;
								}
								tmpRader -= BaseVPer / div;
							}
						}
						for (int i = 0; i < Vehicle_num; i++) {
							if (i == GetMyPlayerID()) { continue; }
							auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
							tmpRader = BaseVPer;
							for (int j = 0; j < div; j++) {
								auto pos = MATRIX_ref::Vtrans(v->GetMatrix().pos() - BaseBos, MATRIX_ref::RotY(rad))*((1.f / Scale_Rate) * tmpRader);
								if (((-xs1 < pos.x() && pos.x() < xs2) && (-ys1 < -pos.z() && -pos.z() < ys2))) {
									if (this->m_Rader >= tmpRader) {
										this->m_Rader = tmpRader;
									}
									isout = false;
									break;
								}
								tmpRader -= BaseVPer / div;
							}
						}
						if (isout) {
							this->m_Rader = tmpRader + BaseVPer / div;
						}
					}

					for (int i = 0; i < Chara_num; i++) {
						if (i == GetMyPlayerID()) { continue; }
						auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
						auto pos = MATRIX_ref::Vtrans(c->GetMatrix().pos() - BaseBos, MATRIX_ref::RotY(rad))*((1.f / Scale_Rate) * this->m_Rader_r);
						if ((-xs1 < pos.x() && pos.x() < xs2) && (-ys1 < -pos.z() && -pos.z() < ys2)) {
							switch (c->GetCharaType()) {
							case CharaTypeID::Team:
								color = Blue;
								break;
							case CharaTypeID::Enemy:
								color = Red;
								break;
							default:
								break;
							}
							int xp, yp;
							xp = xp1 + (int)(pos.x());
							yp = yp1 + (int)(-pos.z());
							DrawCircle(xp, yp, (int)(5.f * std::min(1.f, this->m_Rader_r)), color, TRUE);
						}
					}
					for (int i = 0; i < Vehicle_num; i++) {
						if (i == GetMyPlayerID()) { continue; }
						auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
						auto pos = MATRIX_ref::Vtrans(v->GetMatrix().pos() - BaseBos, MATRIX_ref::RotY(rad))*((1.f / Scale_Rate) * this->m_Rader_r);
						if ((-xs1 < pos.x() && pos.x() < xs2) && (-ys1 < -pos.z() && -pos.z() < ys2)) {
							switch (v->GetCharaType()) {
							case CharaTypeID::Mine:
								color = Blue;
								break;
							case CharaTypeID::Team:
								color = Blue;
								break;
							case CharaTypeID::Enemy:
								color = Red;
								break;
							default:
								break;
							}
							int xp, yp;
							xp = xp1 + (int)(pos.x());
							yp = yp1 + (int)(-pos.z());
							DrawCircle(xp, yp, (int)(5.f * std::min(1.f, this->m_Rader_r)), color, TRUE);
						}
					}
				}

				Vehicle->DrawModuleView(y_r(1500), y_r(200), y_r(200));
				//通信設定
				if (!this->m_MouseActive.on()) {
					int xp, yp, xs, ys;
					xp = y_r(100);
					yp = y_r(250);
					xs = y_r(500);
					ys = y_r(300);

					int y_h = y_r(30);
					//bool Mid_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0);
					//bool Right_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
					int mx = DXDraw::Instance()->disp_x / 2, my = DXDraw::Instance()->disp_y / 2;
					GetMousePoint(&mx, &my);
					//ラムダ
					auto MsgBox = [&](int xp1, int yp1, int xp2, int yp2, std::string String, auto&&... args) {
						color = Black;
						DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
						Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, String, args...);
					};
					auto ClickBox = [&](int xp1, int yp1, int xp2, int yp2, std::string String, auto&&... args) {
						bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
						color = (into) ? Black : Gray;
						DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
						Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, String, args...);
						return into && this->m_LeftClick.trigger();
					};
					auto AddSubBox = [&](int xp1, int yp1, std::function<void()> UpFunc, std::function<void()> DownFunc) {
						int xp2, yp2;
						{
							xp2 = xp1 + y_r(50);
							yp2 = yp1 + y_h;
							bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
							color = (into) ? Black : Gray;
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawString_MID(xp1 + (xp2 - xp1) / 2, yp1, "+", GetColor(255, 255, 255));
							if (into) {
								if (this->m_LeftClick.trigger()) {
									UpFunc();
								}
								if (this->m_LeftClick.press()) {
									m_LeftPressTimer += 1.f / FPS;
									if (this->m_LeftPressTimer > 0.5f) {
										UpFunc();
									}
								}
							}
						}
						{
							yp1 += y_r(50);
							yp2 += y_r(50);
							bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
							color = (into) ? Black : Gray;
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawString_MID(xp1 + (xp2 - xp1) / 2, yp1, "-", GetColor(255, 255, 255));
							if (into) {
								if (this->m_LeftClick.trigger()) {
									DownFunc();
								}
								if (this->m_LeftClick.press()) {
									m_LeftPressTimer += 1.f / FPS;
									if (this->m_LeftPressTimer > 0.5f) {
										DownFunc();
									}
								}
							}
						}
					};
					//
					{
						DrawBox(xp - y_r(10), yp - y_r(10), xp + xs + y_r(10), yp + ys + y_r(10), GetColor(164, 164, 164), TRUE);//背景
						MsgBox(xp, yp, xp + xs, yp + y_h, " %d/%d", (int)this->m_Sequence, (int)SequenceEnum::MainGame);
						//ログ
						{
							int xp1, yp1;
							xp1 = xp;
							yp1 = yp + ys + y_r(10) + y_r(10);
							if (this->m_Sequence > SequenceEnum::SelMode) {
								Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, "種別[%s]", this->m_IsClient ? "クライアント" : "サーバー"); yp1 += y_h;
							}
							if (this->m_Sequence > SequenceEnum::Set_Port) {
								Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, "使用ポート[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1); yp1 += y_h;
							}
							if (this->m_Sequence > SequenceEnum::SetTick) {
								Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, "ティックレート[%4.1f]", Frame_Rate / this->m_Tick); yp1 += y_h;
							}
						}
					}
					auto Prev = this->m_Sequence;
					switch (this->m_Sequence) {
					case SequenceEnum::SelMode:
						if (ClickBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "クライアントになる")) {
							this->m_IsClient = true;
							this->m_Tick = 5.f;
							this->m_Sequence = SequenceEnum::CheckPreset;
						}
						if (ClickBox(xp, yp + y_r(100), xp + xs, yp + y_r(100) + y_h, "サーバーになる")) {
							this->m_IsClient = false;
							this->m_Tick = 10.f;
							this->m_Sequence = SequenceEnum::CheckPreset;
						}
						break;
					case SequenceEnum::CheckPreset:
						MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "プリセット設定");
						for (int i = 0; i < this->m_NewWorkSetting.GetSize(); i++) {
							auto n = this->m_NewWorkSetting.Get(i);
							if (ClickBox(xp, yp + y_r(50)*(i + 2), xp + xs, yp + y_r(50)*(i + 2) + y_h, "[%d][%d,%d,%d,%d]", n.UsePort, n.IP.d1, n.IP.d2, n.IP.d3, n.IP.d4)) {
								this->m_NewSetting.UsePort = n.UsePort;
								this->m_NewSetting.IP = n.IP;
								this->m_Sequence = SequenceEnum::SetTick;
								m_NewWorkSelect = i;
								break;
							}
						}
						{
							int i = this->m_NewWorkSetting.GetSize();
							if (ClickBox(xp, yp + y_r(50)*(i + 2), xp + xs, yp + y_r(50)*(i + 2) + y_h, "設定を追加する")) {
								m_NewWorkSetting.Add();
								m_NewWorkSelect = i;
								this->m_Sequence = SequenceEnum::Set_Port;
							}
						}
						break;
					case SequenceEnum::Set_Port://ポート
						MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "ポート=[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1);
						AddSubBox(xp, yp + y_r(100), [&]() { this->m_NewSetting.UsePort++; }, [&]() { this->m_NewSetting.UsePort--; });
						if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
							this->m_Sequence = SequenceEnum::SetIP;//サーバ-は一応いらない
						}
						break;
					case SequenceEnum::SetIP://IP
						MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4);
						for (int i = 0; i < 4; i++) {
							auto* ip_tmp = &this->m_NewSetting.IP.d1;
							switch (i) {
							case 0:ip_tmp = &this->m_NewSetting.IP.d1; break;
							case 1:ip_tmp = &this->m_NewSetting.IP.d2; break;
							case 2:ip_tmp = &this->m_NewSetting.IP.d3; break;
							case 3:ip_tmp = &this->m_NewSetting.IP.d4; break;
							}
							AddSubBox(y_r(100 + 70 * i), yp + y_r(100),
								[&]() {
								if (*ip_tmp == 255) { *ip_tmp = 0; }
								else { (*ip_tmp)++; }
							}, [&]() {
								if (*ip_tmp == 0) { *ip_tmp = 255; }
								else { (*ip_tmp)--; }
							});
						}
						if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
							IPData = this->m_NewSetting.IP;
							this->m_Sequence = SequenceEnum::SetTick;
							m_NewWorkSetting.Set(this->m_NewWorkSelect, this->m_NewSetting);
							m_NewWorkSetting.Save();
						}
						break;
					case SequenceEnum::SetTick:
						MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "ティック=[%4.1f]", Frame_Rate / this->m_Tick);
						AddSubBox(xp, yp + y_r(100), [&]() { this->m_Tick = std::clamp(this->m_Tick - 1.f, 1.f, 20.f); }, [&]() { this->m_Tick = std::clamp(this->m_Tick + 1.f, 1.f, 20.f); });
						if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
							this->m_Sequence = SequenceEnum::Matching;
						}
						break;
					case SequenceEnum::Matching:
						MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "他プレイヤー待機中…");
						for (int i = 0; i < Player_num; i++) {
							bool isActive = (((this->m_IsClient) ? this->m_ClientCtrl.GetServerDataCommon() : this->m_ServerCtrl.GetServerData()).PlayerData[i].IsActive == 1);
							int yp1 = yp + y_r(50) + y_r(35)*(i + 1);
							color = isActive ? Black : Gray;
							DrawBox(y_r(200), yp1, y_r(200) + y_r(300), yp1 + y_h, color, TRUE);
							Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawString(y_r(200), yp1, "Player", GetColor(255, 255, 255));
							Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawString_RIGHT(y_r(200) + y_r(300), yp1, (isActive ? "〇" : ""), GetColor(255, 255, 255));
						}
						break;
					case SequenceEnum::MainGame:
						MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "通信中!");
						break;
					default:
						break;
					}
					SeqFirst = (Prev != this->m_Sequence);
				}
				//
			}
		};
	};
};
