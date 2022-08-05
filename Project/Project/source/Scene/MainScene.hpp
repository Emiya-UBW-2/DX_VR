#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			static const int		team_num = 1;
			static const int		enemy_num = 1;
		private:
			//リソース関連
			ObjectManager			m_Obj;						//モデル
			BackGroundClass			m_BackGround;				//BG
			SoundHandle				m_Env;
			//操作関連
			float					m_EyeRunPer{ 0.f };
			switchs					m_FPSActive;
			switchs					m_MouseActive;
			switchs					m_Lockon;
			switchs					m_LookModeChange;
			int						m_LookMode{ 0 };
			float					m_LookReturnTime{ 0.f };
			//UI関連
			UIClass					m_UIclass;
			float					m_HPBuf{ 0.f };
			float					m_ScoreBuf{ 0.f };
			float					m_LockonBuf{ 0.f };
			float					m_LockoffBuf{ 0.f };
			int						m_LockoffXposBuf{ 0 };
			int						m_LockoffYposBuf{ 0 };
			int						m_LockoffXsizeBuf{ 0 };
			int						m_LockoffYsizeBuf{ 0 };
			//
			float					m_CamShake{ 0.f };
			VECTOR_ref				m_CamShake1;
			VECTOR_ref				m_CamShake2;
			//
			MATRIX_ref				m_FreeLookMat;
			//
			float					m_Rader{ 1.f };
			float					m_Rader_r{ 1.f };
			//
			float					m_TPS_Xrad{ 0.f };
			float					m_TPS_Yrad{ 0.f };
			float					m_TPS_XradR{ 0.f };
			float					m_TPS_YradR{ 0.f };
			float					m_TPS_Per{ 1.f };


			//通信


			struct PlayerNetData {
			public:
				char	ID;			//1	* 1	= 1byte
				InputControl Input;	//4 * 3	= 12byte
									//		  13byte
			};
			struct ServerNetData {
				unsigned long long Frame;
				PlayerNetData PlayerData[team_num + enemy_num];
			};

			NewWorkControl			NetWork;
			ServerNetData			m_ServerData;		// サーバーデータ
			PlayerNetData			m_PlayerData;		// 送信用ＩＰアドレスデータ
			int						Sequence = 0;
			switchs					m_LeftClick;
			float					m_LeftPressTimer{ 0.f };
			bool					Client = true;
			int						UsePort = 10800;
			int						IP[4]{ 255,255,255,255 };
			int						SeqCnt = 0;
		public:
			using TEMPSCENE::TEMPSCENE;
			void Set(void) noexcept override {
				SoundPool::Create();
				Set_EnvLight(
					VECTOR_ref::vget(Scale_Rate*-300.f, Scale_Rate*-10.f, Scale_Rate*-300.f),
					VECTOR_ref::vget(Scale_Rate*300.f, Scale_Rate*50.f, Scale_Rate*300.f),
					VECTOR_ref::vget(-0.25f, -0.5f, 0.0f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				//Load
				//BG
				this->m_BackGround.Load();
				//
				this->m_Obj.Init(&this->m_BackGround.GetGroundCol());
				for (int i = 0; i < team_num; i++) {
					this->m_Obj.AddObject(ObjType::Human, "data/Charactor/Marisa/");
				}
				for (int i = 0; i < enemy_num; i++) {
					this->m_Obj.AddObject(ObjType::Human, "data/Charactor/Marisa/");
				}
				//ロード
				SetCreate3DSoundFlag(FALSE);
				this->m_Env = SoundHandle::Load("data/Sound/SE/envi.wav");
				SetCreate3DSoundFlag(FALSE);
				this->m_Env.vol(64);
				//UI
				this->m_UIclass.Set();
				//
				TEMPSCENE::Set();
				//Set
				//人
				for (int i = 0; i < team_num + enemy_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), VECTOR_ref::vget(0.f + (float)(i - 1)*100.f, 0.f, -52.5f));
					if (i == 0) {
						c->SetUseRealTimePhysics(true);
					}
					else {
						c->SetUseRealTimePhysics(false);
					}
					if (i < team_num) {
						c->SetCharaType(CharaTypeID::Team);
					}
					else {
						c->SetCharaType(CharaTypeID::Enemy);
					}
				}
				{
					auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, 0));//自分
					this->m_HPBuf = Chara->GetHP();
					this->m_ScoreBuf = Chara->GetScore();
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
				SE->Add((int)SoundEnum::GateOpen, 1, "data/Sound/SE/GateOpen.wav");

				SE->Get((int)SoundEnum::Shot_Gun).SetVol_Local(128);
				SE->Get((int)SoundEnum::Trigger).SetVol_Local(128);
				for (int i = 0; i < 4; i++) {
					SE->Get((int)SoundEnum::Cocking0 + i).SetVol_Local(128);
				}
				SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
				SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
				SE->Get((int)SoundEnum::GateOpen).SetVol_Local(128);
				//入力
				this->m_FPSActive.Init(false);
				this->m_MouseActive.Init(false);
				this->m_LookMode = 0;
				this->m_LookReturnTime = 0.f;
			}
			//
			bool Update(void) noexcept override {



				auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, m_PlayerData.ID));//自分
				//FirstDoing
				if (IsFirstLoop) {
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
					this->m_Env.play(DX_PLAYTYPE_LOOP, TRUE);
				}
				//Input,AI
				{
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

					float cam_per = ((camera_main.fov / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					float pp_x = std::clamp(-(float)(my - DXDraw::Instance()->disp_y / 2)*1.f, -9.f, 9.f) * cam_per;
					float pp_y = std::clamp((float)(mx - DXDraw::Instance()->disp_x / 2)*1.f, -9.f, 9.f) * cam_per;
					bool w_key = (CheckHitKeyWithCheck(KEY_INPUT_W) != 0);
					bool s_key = (CheckHitKeyWithCheck(KEY_INPUT_S) != 0);
					bool a_key = (CheckHitKeyWithCheck(KEY_INPUT_A) != 0);
					bool d_key = (CheckHitKeyWithCheck(KEY_INPUT_D) != 0);
					bool Lshift_key = (CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0);
					bool q_key = (CheckHitKeyWithCheck(KEY_INPUT_Q) != 0);
					bool e_key = (CheckHitKeyWithCheck(KEY_INPUT_E) != 0);

					bool right_key = (CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
					bool left_key = (CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
					bool up_key = (CheckHitKeyWithCheck(KEY_INPUT_UP) != 0);
					bool down_key = (CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0);

					bool a1_key = (CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
					bool a2_key = (CheckHitKeyWithCheck(KEY_INPUT_R) != 0);
					bool a3_key = (CheckHitKeyWithCheck(KEY_INPUT_F) != 0);
					bool a4_key = (CheckHitKeyWithCheck(KEY_INPUT_J) != 0);
					bool a5_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0) && this->m_MouseActive.on();

					bool look_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) && this->m_MouseActive.on();
					bool eyechange_key = CheckHitKeyWithCheck(KEY_INPUT_V) != 0;

					bool Lockon_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0) && this->m_MouseActive.on();

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
								if (!(input.X == 0 && input.Y == 0)) {
									w_key = (-50.f <= deg && deg <= 50.f);
									a_key = (-140.f <= deg && deg <= -40.f);
									s_key = (130.f <= deg || deg <= -130.f);
									d_key = (40.f <= deg && deg <= 140.f);
								}
								else {
									w_key = false;
									a_key = false;
									s_key = false;
									d_key = false;
								}
								//走り(未定)
								Lshift_key = (input.Buttons[10] != 0);
								a1_key = (input.Buttons[10] != 0);
								//視点切り替え
								look_key = (input.Buttons[11] != 0);
								//eyechange_key = (input.Buttons[11]!=0);
								//ヨー
								q_key = (input.Buttons[6] != 0);
								e_key = (input.Buttons[7] != 0);
								//加減速
								a3_key = (input.Buttons[4] != 0);
								a2_key = (input.Buttons[5] != 0);
								//十字
								deg = (float)(input.POV[0]) / 100.f;
								up_key = (310.f <= deg || deg <= 50.f);
								left_key = (220.f <= deg && deg <= 320.f);
								down_key = (130.f <= deg && deg <= 230.f);
								right_key = (40.f <= deg && deg <= 140.f);
								//ボタン
								a4_key = (input.Buttons[3] != 0);//□
								Lockon_key = (input.Buttons[0] != 0);//△
								a5_key = (input.Buttons[1] != 0);//〇
								//_key = (input.Buttons[2] != 0);//×
							}
							break;
						case DX_PADTYPE_XBOX_360:
						case DX_PADTYPE_XBOX_ONE:
							break;
						default:
							break;
						}
					}

					this->m_FPSActive.GetInput(eyechange_key);
					this->m_LookModeChange.GetInput(look_key);
					this->m_Lockon.GetInput(Lockon_key);

					if (this->m_LookModeChange.trigger()) {
						++this->m_LookMode %= 2;
					}
					if (this->m_LookMode == 1) {
						if (this->m_LookModeChange.press()) {
							this->m_LookReturnTime = std::min(this->m_LookReturnTime + 1.f / FPS, 1.f);
						}
						else {
							if (this->m_LookReturnTime >= 1.f) {
								this->m_LookMode = 0;
							}
							this->m_LookReturnTime = 0.f;
						}
					}
					if (this->m_Lockon.trigger()) {
						if (Chara->GetLockOn() == nullptr) {
							for (int i = 0; i < team_num + enemy_num; i++) {
								if (i == m_PlayerData.ID) { continue; }
								auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));

								if (Chara->GetCharaType() == c->GetCharaType()) { continue; }
								auto cPos = c->GetFrameWorldMat(CharaFrame::Upper).pos();

								auto minLength = GetMinLenSegmentToPoint(
									Chara->GetEyePosition(),
									Chara->GetEyePosition() + Lerp(Chara->GetEyeVecMat().zvec() * -1.f, MATRIX_ref::Vtrans(Chara->GetEyeVecMat().zvec() * -1.f, m_FreeLookMat), this->m_TPS_Per) * Scale_Rate * 3000.f,
									cPos);
								if (minLength <= (100.f / (50 * Scale_Rate)*(cPos - Chara->GetEyePosition()).size())) {
									Chara->SetLockOn(c);
									this->m_LockonBuf = 1.f;
									break;
								}
							}
						}
						else {
							Chara->SetLockOn(nullptr);
							this->m_LockoffBuf = 1.f;
						}
					}

					Easing(&this->m_TPS_Per, (!this->m_FPSActive.on() && (this->m_LookMode == 1)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

					this->m_TPS_Xrad += pp_x;
					this->m_TPS_Yrad += pp_y;
					this->m_TPS_Xrad = std::clamp(this->m_TPS_Xrad, deg2rad(-80), deg2rad(80));
					if (this->m_TPS_Yrad >= deg2rad(180)) { this->m_TPS_Yrad -= deg2rad(360); }
					if (this->m_TPS_Yrad <= deg2rad(-180)) { this->m_TPS_Yrad += deg2rad(360); }

					this->m_TPS_Xrad *= this->m_TPS_Per;
					this->m_TPS_Yrad *= this->m_TPS_Per;

					Easing(&this->m_TPS_XradR, this->m_TPS_Xrad, 0.5f, EasingType::OutExpo);

					this->m_TPS_YradR += (sin(this->m_TPS_Yrad)*cos(this->m_TPS_YradR) - cos(this->m_TPS_Yrad) * sin(this->m_TPS_YradR))*20.f / FPS;

					Chara->SetEyeVec((camera_main.camvec - camera_main.campos).Norm());

					InputControl Input;
					bool isready = true;
					/*
					for (int i = 0; i < team_num + enemy_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
						//操作
						if (i == 0
							//&& false
							) {
							Input.SetInput(
								pp_x*(1.f - this->m_TPS_Per),
								pp_y*(1.f - this->m_TPS_Per),
								w_key && isready,
								s_key && isready,
								a_key && isready,
								d_key && isready,
								Lshift_key && isready,
								q_key && isready,
								e_key && isready,

								right_key && isready,
								left_key && isready,
								up_key && isready,
								down_key && isready,

								a1_key && isready,
								a2_key && isready,
								a3_key && isready,
								a4_key && isready,
								a5_key && isready
							);
							Chara->SetInput(Input, isready);
							continue;
						}
						Input.SetInput(
							0.f,
							0.f,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready,
							(false) && isready
						);
						c->SetInput(Input, isready);
						c->SetLockOn((std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, 0)));
					}
					//*/
					m_PlayerData.Input.SetInput(
						pp_x*(1.f - this->m_TPS_Per),
						pp_y*(1.f - this->m_TPS_Per),
						w_key && isready,
						s_key && isready,
						a_key && isready,
						d_key && isready,
						Lshift_key && isready,
						q_key && isready,
						e_key && isready,

						right_key && isready,
						left_key && isready,
						up_key && isready,
						down_key && isready,

						a1_key && isready,
						a2_key && isready,
						a3_key && isready,
						a4_key && isready,
						a5_key && isready
					);


					if (Sequence == 5) {
						//サーバー
						if (!Client) {
							m_ServerData.PlayerData[m_PlayerData.ID] = m_PlayerData;	// サーバーのプレイヤーデータ
							m_ServerData.Frame++;
						}
						{
							//クライアント
							if (Client) {
								//サーバーからのデータを受信
								if (NetWork.Recv(&m_ServerData)) {
								}
								//自身のデータを送信
								NetWork.SendtoServer(m_PlayerData);
							}
							//サーバー
							else {
								PlayerNetData tmpData;										// 送信用ＩＰアドレスデータ
								if (NetWork.Recv(&tmpData)) {								// 該当ソケットにクライアントから受信したら
									m_ServerData.PlayerData[tmpData.ID] = tmpData;
									//サーバーからクライアントに返信
									NetWork.SendtoClient(m_ServerData);
								}
							}
						}
					}
					for (int i = 0; i < team_num + enemy_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
						c->SetInput(m_ServerData.PlayerData[i].Input, isready);
					}

				}
				//Execute
				this->m_Obj.ExecuteObject();
				//col

				//視点
				{
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
						auto charalock = Chara->GetLockOn();
						if (charalock != nullptr && (this->m_LookMode == 1)) {
							FreeLook = MATRIX_ref::RotVec2(Chara->GetMatrix().zvec()*-1.f, charalock->GetFrameWorldMat(CharaFrame::Upper).pos() - Chara->GetEyePosition());
						}
						else {
							FreeLook = MATRIX_ref::RotAxis(Chara->GetMatrix().xvec(), this->m_TPS_XradR) * MATRIX_ref::RotAxis(Chara->GetMatrix().yvec(), this->m_TPS_YradR);
						}
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

				this->m_BackGround.Execute();
				//UIパラメーター
				{
					this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
					this->m_ScoreBuf += std::clamp((Chara->GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(2, 1);

					this->m_UIclass.SetIntParam(3, (int)Chara->GetHP());
					this->m_UIclass.SetIntParam(4, (int)Chara->GetHPMax());
					this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
					this->m_HPBuf += std::clamp((Chara->GetHP() - this->m_HPBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(6, (int)1.f);
					this->m_UIclass.SetIntParam(7, (int)1.f);
					this->m_UIclass.SetIntParam(8, (int)1.f);
				}
				TEMPSCENE::Update();
				Effect_UseControl::Update_Effect();
				return true;
			}
			void Dispose(void) noexcept override {
				NetWork.Dispose();

				Effect_UseControl::Dispose_Effect();
				this->m_Obj.DisposeObject();
			}
			//
			void Depth_Draw(void) noexcept override {
				this->m_BackGround.Draw();
				//this->m_Obj.DrawDepthObject();
			}
			void BG_Draw(void) noexcept override {
				this->m_BackGround.BG_Draw();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				this->m_BackGround.Shadow_Draw_NearFar();
				//this->m_Obj.DrawObject_Shadow();
			}
			void Shadow_Draw(void) noexcept override {
				this->m_BackGround.Shadow_Draw();
				this->m_Obj.DrawObject_Shadow();
			}

			void Main_Draw(void) noexcept override {
				this->m_BackGround.Draw();
				this->m_Obj.DrawObject();
				//this->m_Obj.DrawDepthObject();
				//シェーダー描画用パラメーターセット
				{
					Set_is_Blackout(false);
					Set_is_lens(false);
				}
				for (int i = 0; i < team_num + enemy_num; i++) {
					if (i == m_PlayerData.ID) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
					auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						c->SetCameraPosition(campos);
						c->SetCameraSize(
							std::max(
								20.f / ((pos - GetCameraPosition()).size() / 2.f),
								0.2f
							)
						);
					}
				}
			}
			void Main_Draw2(void) noexcept override {
				this->m_Obj.DrawDepthObject();
			}
			void LAST_Draw(void) noexcept override {
			}
			//UI表示
			void UI_Draw(void) noexcept  override {
				auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, m_PlayerData.ID));//自分

				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto Red = GetColor(255, 0, 0);
				auto Blue = GetColor(50, 50, 255);
				auto Green = GetColor(43, 163, 91);
				auto White = GetColor(212, 255, 239);
				unsigned int color = Red;
				//キャラ
				for (int i = 0; i < team_num + enemy_num; i++) {
					if (i == m_PlayerData.ID) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
					auto campos = c->GetCameraPosition();
					if (0.f < campos.z() && campos.z() < 1.f) {
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
						int xp, yp, xs, ys;
						xp = (int)(campos.x());
						yp = (int)(campos.y());
						xs = (int)(100.f*c->GetCameraSize());
						ys = (int)(100.f*c->GetCameraSize());
						int siz = y_r(std::max((int)(20.f*c->GetCameraSize()), 10));
						int p = 0;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);
						p = 1;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, color, FALSE);
						p = 2;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);

						Fonts->Get(siz, FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat_MID(xp, yp - ys - siz, color, White, "%s", c->GetName().c_str());

						Fonts->Get(siz, FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat(xp + xs, yp + ys, color, White, "%dm", (int)((c->GetMatrix().pos() - Chara->GetMatrix().pos()).size() / Scale_Rate));

						if (Chara->GetLockOn() == c) {
							Fonts->Get(siz, FontPool::FontType::HUD_Edge).Get_handle().DrawStringFormat_MID(xp, yp - siz / 2, color, White, "%s", (c->GetCameraSize() > 0.25f) ? "TARGET" : "TGT");

							if (this->m_LockonBuf > 0.f) {
								xs = (int)((100.f + 100.f * this->m_LockonBuf)*c->GetCameraSize());
								ys = (int)((100.f + 100.f * this->m_LockonBuf)*c->GetCameraSize());
								p = 0;
								DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);
								p = 1;
								DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, color, FALSE);
								p = 2;
								DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);

								this->m_LockonBuf -= 1.f / FPS / 0.5f;

							}
							this->m_LockoffXposBuf = xp;
							this->m_LockoffYposBuf = yp;
							this->m_LockoffXsizeBuf = xs;
							this->m_LockoffYsizeBuf = ys;
						}
						//リセット
						campos.z(-1.f);
						c->SetCameraPosition(campos);
					}
					if (this->m_LockoffBuf > 0.f) {
						int xp, yp, xs, ys;
						xp = this->m_LockoffXposBuf;
						yp = this->m_LockoffYposBuf;
						xs = this->m_LockoffXsizeBuf;
						ys = this->m_LockoffYsizeBuf;

						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*this->m_LockoffBuf));
						int p = 0;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);
						p = 1;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, Red, FALSE);
						p = 2;
						DrawBox(xp - xs + p, yp - ys + p, xp + xs - p, yp + ys - p, White, FALSE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

						this->m_LockoffBuf -= 1.f / FPS / 2.f;
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

					int p = 0;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, Green, FALSE);
					p = 1;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, White, FALSE);
					p = 2;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, Green, FALSE);

					xs1 = y_r((int)(256.f * 0.5f*std::min(1.f, this->m_Rader_r)));
					ys1 = y_r((int)(256.f * 0.8f*std::min(1.f, this->m_Rader_r)));
					xs2 = y_r((int)(256.f * 0.5f*std::min(1.f, this->m_Rader_r)));
					ys2 = y_r((int)(256.f * 0.2f*std::min(1.f, this->m_Rader_r)));
					p = 0;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, Green, FALSE);
					p = 1;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, White, FALSE);
					p = 2;
					DrawBox((int)(xp1 - xs1) + p, (int)(yp1 - ys1) + p, (int)(xp1 + xs2) - p, (int)(yp1 + ys2) - p, Green, FALSE);

					auto BaseBos = Chara->GetMatrix().pos();

					xs1 = y_r((int)(256.f * 0.5f));
					ys1 = y_r((int)(256.f * 0.8f));
					xs2 = y_r((int)(256.f * 0.5f));
					ys2 = y_r((int)(256.f * 0.2f));
					auto base = Chara->GetCharaDir().zvec()*-1.f;
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
						for (int i = 0; i < team_num + enemy_num; i++) {
							if (i == m_PlayerData.ID) { continue; }
							auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
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
						if (isout) {
							this->m_Rader = tmpRader + BaseVPer / div;
						}
					}

					for (int i = 0; i < team_num + enemy_num; i++) {
						if (i == m_PlayerData.ID) { continue; }
						auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
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
				}
				//
				//通信設定
				m_LeftClick.GetInput((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
				if (!m_LeftClick.press()) {
					m_LeftPressTimer = 0.f;
				}
				if (!this->m_MouseActive.on()) {
					bool Mid_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0);
					bool Right_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
					int mx = DXDraw::Instance()->disp_x / 2, my = DXDraw::Instance()->disp_y / 2;
					GetMousePoint(&mx, &my);
					int xp1, yp1, xp2, yp2;
					unsigned int color;
					{
						xp1 = y_r(90);
						yp1 = y_r(340);
						xp2 = xp1 + y_r(420);
						yp2 = yp1 + y_r(200);
						DrawBox(xp1, yp1, xp2, yp2, GetColor(164, 164, 164), TRUE);

						xp1 = y_r(100);
						yp1 = y_r(350);
						xp2 = xp1 + y_r(400);
						yp2 = yp1 + y_r(30);
						DrawBox(xp1, yp1, xp2, yp2, GetColor(0, 0, 0), TRUE);
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), " %d/%d", Sequence, 0);
					}
					xp1 = y_r(100);
					yp1 = y_r(550);
					if (Sequence > 0) {
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), "種別　[%s]", Client ? "クライアント" : "サーバー"); yp1 += y_r(30);
					}
					if (Sequence > 1) {
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), "ポート[%d]", UsePort); yp1 += y_r(30);
					}
					switch (Sequence) {
					case 0://選択
					{
						{
							xp1 = y_r(100);
							yp1 = y_r(400);
							xp2 = xp1 + y_r(400);
							yp2 = yp1 + y_r(30);
							bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
							color = (into) ? GetColor(0, 0, 0) : GetColor(64, 64, 64);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString(xp1, yp1, "クライアントになる", GetColor(255, 255, 255));
							if (into && m_LeftClick.trigger()) {
								Client = true;
								Sequence++;
							}
						}
						{
							xp1 = y_r(100);
							yp1 = y_r(450);
							xp2 = xp1 + y_r(400);
							yp2 = yp1 + y_r(30);
							bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
							color = (into) ? GetColor(0, 0, 0) : GetColor(64, 64, 64);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString(xp1, yp1, "サーバーになる", GetColor(255, 255, 255));
							if (into && m_LeftClick.trigger()) {
								Client = false;
								Sequence++;
							}
						}
						if (Sequence == 1) {
							if (Client) {
								m_PlayerData.ID = 1;					//クライアント
							}
							else {
								m_PlayerData.ID = 0;					//サーバー
							}
						}
					}
						break;
					case 1://ポート
					{
						{
							xp1 = y_r(100);
							yp1 = y_r(400);
							xp2 = xp1 + y_r(400);
							yp2 = yp1 + y_r(30);
							color = GetColor(0, 0, 0);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), "ポート=[%d]", UsePort);
						}
						{
							xp1 = y_r(100);
							yp1 = y_r(450);
							xp2 = xp1 + y_r(120);
							yp2 = yp1 + y_r(30);
							bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
							color = (into) ? GetColor(0, 0, 0) : GetColor(64, 64, 64);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString_MID(xp1 + (xp2 - xp1) / 2, yp1, "+", GetColor(255, 255, 255));
							if (into){
								if (m_LeftClick.trigger()) {
									UsePort++;
								}
								if (m_LeftClick.press()) {
									m_LeftPressTimer += 1.f / FPS;
									if (m_LeftPressTimer > 0.5f) {
										UsePort++;
									}
								}
							}
						}
						{
							xp1 = y_r(240);
							yp1 = y_r(450);
							xp2 = xp1 + y_r(120);
							yp2 = yp1 + y_r(30);
							bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
							color = (into) ? GetColor(0, 0, 0) : GetColor(64, 64, 64);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString_MID(xp1 + (xp2 - xp1) / 2, yp1, "-", GetColor(255, 255, 255));
							if (into) {
								if (m_LeftClick.trigger()) {
									UsePort--;
								}
								if (m_LeftClick.press()) {
									m_LeftPressTimer += 1.f / FPS;
									if (m_LeftPressTimer > 0.5f) {
										UsePort--;
									}
								}
							}
						}
						{
							xp1 = y_r(380);
							yp1 = y_r(450);
							xp2 = xp1 + y_r(120);
							yp2 = yp1 + y_r(30);
							bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
							color = (into) ? GetColor(0, 0, 0) : GetColor(64, 64, 64);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString_MID(xp1 + (xp2 - xp1) / 2, yp1, "Set", GetColor(255, 255, 255));
							if (into && m_LeftClick.trigger()) {
								NetWork.Set_Port(UsePort);
								if (Client) {
									Sequence+=1;
								}
								else {
									Sequence+=2;
								}
							}
						}
					}
						break;
					case 2://IP
					{
						{
							xp1 = y_r(100);
							yp1 = y_r(400);
							xp2 = xp1 + y_r(400);
							yp2 = yp1 + y_r(30);
							color = GetColor(0, 0, 0);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), "IP=[%d,%d,%d,%d]", IP[0], IP[1], IP[2], IP[3]);
						}
						for (int i = 0; i < 4; i++) {
							{
								xp1 = y_r(100 + 70 * i);
								yp1 = y_r(450);
								xp2 = xp1 + y_r(50);
								yp2 = yp1 + y_r(30);
								bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
								color = (into) ? GetColor(0, 0, 0) : GetColor(64, 64, 64);
								DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
								Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString_MID(xp1 + (xp2 - xp1) / 2, yp1, "+", GetColor(255, 255, 255));
								if (into) {
									if (m_LeftClick.trigger()) {
										IP[i]++;
									}
									if (m_LeftClick.press()) {
										m_LeftPressTimer += 1.f / FPS;
										if (m_LeftPressTimer > 0.5f) {
											IP[i]++;
										}
									}
								}
							}
							{
								xp1 = y_r(100 + 70 * i);
								yp1 = y_r(500);
								xp2 = xp1 + y_r(50);
								yp2 = yp1 + y_r(30);
								bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
								color = (into) ? GetColor(0, 0, 0) : GetColor(64, 64, 64);
								DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
								Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString_MID(xp1 + (xp2 - xp1) / 2, yp1, "-", GetColor(255, 255, 255));
								if (into) {
									if (m_LeftClick.trigger()) {
										IP[i]--;
									}
									if (m_LeftClick.press()) {
										m_LeftPressTimer += 1.f / FPS;
										if (m_LeftPressTimer > 0.5f) {
											IP[i]--;
										}
									}
								}
							}
							if (IP[i] > 255) { IP[i] = 0; }
							if (IP[i] < 0) { IP[i] = 255; }
						}
						{
							xp1 = y_r(380);
							yp1 = y_r(450);
							xp2 = xp1 + y_r(120);
							yp2 = yp1 + y_r(30);
							bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
							color = (into) ? GetColor(0, 0, 0) : GetColor(64, 64, 64);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString_MID(xp1 + (xp2 - xp1) / 2, yp1, "Set", GetColor(255, 255, 255));
							if (into && m_LeftClick.trigger()) {
								IPDATA IPt;					// 送信用ＩＰアドレスデータ
								IPt.d1 = (unsigned char)(IP[0]);
								IPt.d2 = (unsigned char)(IP[1]);
								IPt.d3 = (unsigned char)(IP[2]);
								IPt.d4 = (unsigned char)(IP[3]);
								NetWork.SetIP(IPt);
								Sequence++;
							}
						}
					}
						break;
					case 3://init
					{
						if (Client) {
							NetWork.InitClient();
						}
						else {
							NetWork.InitServer();
							m_ServerData.Frame = 0;
						}
						Sequence++;
					}
						break;
					case 4://通信待機
					{
						{
							xp1 = y_r(100);
							yp1 = y_r(400);
							xp2 = xp1 + y_r(400);
							yp2 = yp1 + y_r(30);
							color = GetColor(0, 0, 0);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString(xp1, yp1, "通信待機中…", GetColor(255, 255, 255));
						}
						int tmpData = -1;
						//クライアント
						if (Client) {
							switch (SeqCnt) {
							case 0:
								NetWork.SendtoServer(0);			// 通信リクエスト
								//サーバーからの自分のIDを受信
								if (NetWork.Recv(&tmpData)) {
									if (tmpData > 0) {
										m_PlayerData.ID = tmpData;
										SeqCnt++;
									}
								}
								break;
							case 1:
								NetWork.SendtoServer(1);			// ID取れたよ
								//サーバーからのデータを受信したら次へ
								if (NetWork.Recv(&m_ServerData)) {
									Sequence++;
								}
								break;
							default:
								break;
							}
						}
						//サーバー
						else {
							switch (SeqCnt) {
							case 0:									//無差別受付
								if (NetWork.Recv(&tmpData)) {			// 該当ソケットにクライアントから受信したら
									SeqCnt++;
								}
								break;
							case 1:
								if (NetWork.Recv(&tmpData)) {
									NetWork.SendtoClient(1);			// IDを送る
									if (tmpData == 1) {					// ID取れたと識別出来たら
										Sequence++;						// マッチ開始
									}
								}
								break;
							default:
								break;
							}
						}
					}
						break;
					case 5://通信
					{
						{
							xp1 = y_r(100);
							yp1 = y_r(400);
							xp2 = xp1 + y_r(400);
							yp2 = yp1 + y_r(30);
							color = GetColor(0, 0, 0);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString(xp1, yp1, "通信!", GetColor(255, 255, 255));
						}
					}
						break;
					default:
						break;
					}
				}
				//
			}
		};
	};
};
