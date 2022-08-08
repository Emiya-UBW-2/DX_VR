#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			static const int		chara_num = 3;
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
			float					m_TPS_Xrad{ 0.f };
			float					m_TPS_Yrad{ 0.f };
			float					m_TPS_XradR{ 0.f };
			float					m_TPS_YradR{ 0.f };
			float					m_TPS_Per{ 1.f };
			//
			float					m_Rader{ 1.f };
			float					m_Rader_r{ 1.f };
			//通信

			struct PlayerNetData {
			public:
				InputControl	Input;			//4 * 5	= 20byte
				VECTOR_ref		PosBuf;			//4 * 3	= 12byte
				size_t			CheckSum{ 0 };	//4 * 1	=  4byte
				char			ID{ 0 };		//1	* 1	=  1byte
				char			IsActive{ 0 };	//1	* 1	=  1byte
												//		  38byte
			public:
				const auto CalcCheckSum() { return (size_t)(((int)(PosBuf.x()) + (int)(PosBuf.y()) + (int)(PosBuf.z())) + (int)ID); }

				const PlayerNetData operator+(const PlayerNetData& o) const noexcept {
					PlayerNetData tmp;

					tmp.ID = o.ID;
					tmp.Input = this->Input + o.Input;
					tmp.PosBuf = this->PosBuf + o.PosBuf;

					return tmp;
				}
				const PlayerNetData operator-(const PlayerNetData& o) const noexcept {
					PlayerNetData tmp;

					tmp.ID = o.ID;
					tmp.Input = this->Input - o.Input;
					tmp.PosBuf = this->PosBuf - o.PosBuf;

					return tmp;
				}
				const PlayerNetData operator*(float per) const noexcept {
					PlayerNetData tmp;

					tmp.ID = this->ID;
					tmp.Input = this->Input*per;
					tmp.PosBuf = this->PosBuf*per;

					return tmp;
				}
			};
			struct ServerNetData {
				int					Tmp1{ 0 };				//4
				int					StartFlag{ 0 };			//4
				size_t				Frame{ 0 };				//8
				PlayerNetData		PlayerData[chara_num];	//37 * 3
			};

			std::array<ServerNetData, 1> InputRec;	//遅延再現用
			ServerNetData			m_ServerData;	// サーバーデータ
			int InputRecCnt = 0;

			std::vector<std::pair<NewWorkControl,int>> m_NetWork;

			int						m_NetWorkSel{ 0 };
			float					m_CannotConnectTimer{ 0.f };

			size_t					ServerFrame{ 0 };
			int						LeapFrame{ 0 };
			ServerNetData			m_ServerDataCommon;	// サーバーデータ
			ServerNetData			m_PrevServerData;	// サーバーデータ
			PlayerNetData			m_PlayerData;		// 送信用ＩＰアドレスデータ

			enum class SequenceEnum {
				SelMode,
				Set_Port,
				SetTick,
				SetIP,
				Matching,
				MainGame,
			};

			SequenceEnum			Sequence{ SequenceEnum::SelMode };
			bool					SeqFirst{ false };
			switchs					m_LeftClick;
			float					m_LeftPressTimer{ 0.f };
			bool					Client{ true };
			float					TickRate{ 10.f };
			int						UsePort{ 10850 };
			int						IP[4]{ 127,0,0,1 };
			//int						IP[4]{ 58,188,85,163 };
			IPDATA					IPData;				// 送信用ＩＰアドレスデータ
			float					serverCnt{ 0.f };
			//
			const auto& GetMyPlayerID() const noexcept { return m_PlayerData.ID; }
			//共通の更新
			void NetCommonExecute(const ServerNetData& pData) {
				//疑似遅延
				InputRec[0] = pData;
				for (int i = ((int)(InputRec.size()) - 1); i >= 1; i--) {
					InputRec[i] = InputRec[i - 1];
				}
				auto& tmpData = InputRec[InputRecCnt];
				InputRecCnt = std::clamp<int>(InputRecCnt + 1, 0, (int)(InputRec.size()) - 1);

				if (ServerFrame <= tmpData.Frame && ((tmpData.Frame - ServerFrame) < 60)) {
					LeapFrame = 0;
					m_PrevServerData = m_ServerDataCommon;
					ServerFrame = tmpData.Frame;
					m_ServerDataCommon = tmpData;
				}
			}
			void NetWorkDispose() {
				for (auto & n : m_NetWork) {
					n.first.Dispose();
				}
				m_NetWork.clear();
			}
			//クライアント処理
			void ClientInit(int pPort, const IPDATA& pIP) {
				m_NetWorkSel = 0;
				m_CannotConnectTimer = 0.f;
				m_NetWork.resize(chara_num - 1);
				int i = 0;
				for (auto & n : m_NetWork) {
					n.first.Set_Port(pPort + i); i++;
					n.first.SetIP(pIP);
					n.first.InitClient();
					n.second = 0;
				}
			}
			bool ClientExecute() {
				printfDx("\nSelect = %d\n\n", m_NetWorkSel);
				for (auto & n : m_NetWork) {
					printfDx("second = %d\n", n.second);
				}

				ServerNetData tmpData;
				bool canMatch = true;
				bool canSend = false;
				canMatch = (m_NetWork[m_NetWorkSel].second >= 2);
				if (canMatch) {
					// ティックレート用演算
					serverCnt += 60.f / FPS;
					if (serverCnt > TickRate) {
						serverCnt -= TickRate;
						canSend = true;
					}
				}
				switch (m_NetWork[m_NetWorkSel].second) {
				case 0:
					m_NetWork[m_NetWorkSel].first.SendtoServer(0);			// 通信リクエスト
					//サーバーからの自分のIDを受信
					if (m_NetWork[m_NetWorkSel].first.Recv(&tmpData)) {
						NetCommonExecute(tmpData);								//更新
						m_CannotConnectTimer = 0.f;
						if (tmpData.Tmp1 > 0) {
							m_PlayerData.ID = (char)tmpData.Tmp1;
							for (int i = 0; i < chara_num; i++) {
								auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
								if (i == GetMyPlayerID()) {
									//c->SetUseRealTimePhysics(true);
									c->SetCharaType(CharaTypeID::Team);
								}
								else {
									//c->SetUseRealTimePhysics(false);
									c->SetCharaType(CharaTypeID::Enemy);
								}
							}
							m_NetWork[m_NetWorkSel].second++;
						}
					}
					else {
						m_CannotConnectTimer += 1.f / FPS;
						if (m_CannotConnectTimer > 1.f) {
							m_CannotConnectTimer = 0.f;
							m_NetWork[m_NetWorkSel].first.Dispose();
							m_NetWorkSel++;
							if (m_NetWorkSel >= chara_num) {
								//満タン
							}
						}
					}
					break;
				case 1:
					m_NetWork[m_NetWorkSel].first.SendtoServer(1);			// ID取れたよ
					//サーバーからのデータを受信したら次へ
					if (m_NetWork[m_NetWorkSel].first.Recv(&tmpData)) {
						NetCommonExecute(tmpData);								//更新
						if (tmpData.StartFlag == 1) {
							m_NetWork[m_NetWorkSel].second++;
						}
					}
					break;
				case 2:
					if (canSend) {
						m_NetWork[m_NetWorkSel].first.SendtoServer(m_PlayerData);				//自身のデータを送信
					}
					if (m_NetWork[m_NetWorkSel].first.Recv(&tmpData)) {					//サーバーからのデータを受信したら
						NetCommonExecute(tmpData);								//更新
					}
					break;
				default:
					break;
				}
				return canMatch;
			}
			//サーバー処理
			void ServerInit(int pPort) {
				m_NetWork.resize(chara_num-1);
				int i = 0;
				for (auto & n : m_NetWork) {
					n.first.Set_Port(pPort + i); i++;
					n.first.InitServer();
					n.second = 0;
				}
				m_ServerDataCommon.Frame = 0;

				m_ServerData.Tmp1 = 0;
				m_ServerData.StartFlag = 0;
				m_ServerData.PlayerData[0].IsActive = 1;
				m_ServerData.Frame = 0;
			}
			bool ServerExecute() {
				bool canMatch = true;
				bool canSend = false;
				for (auto & n : m_NetWork) {
					if (!(n.second >= 2)) {
						canMatch = false;
					}
				}
				if (canMatch) {
					// ティックレート用演算
					serverCnt += 60.f / FPS;
					if (serverCnt > TickRate) {
						serverCnt -= TickRate;
						canSend = true;
					}
					//サーバーデータの更新
					m_ServerData.StartFlag = 1;
					m_ServerData.PlayerData[GetMyPlayerID()] = m_PlayerData;		// サーバープレイヤーののプレイヤーデータ
					m_ServerData.Frame++;											// サーバーフレーム更新
				}
				int i = 0;
				for (auto & n : m_NetWork) {
					int tmpData = -1;
					switch (n.second) {
					case 0:										//無差別受付
						if (n.first.Recv(&tmpData)) {			// 該当ソケットにクライアントからなにか受信したら
							n.second++;
						}
						break;
					case 1:
						m_ServerData.Tmp1 = 1 + i;
						m_ServerData.StartFlag = 0;
						m_ServerData.PlayerData[1 + i].IsActive = 1;

						n.first.SendtoClient(m_ServerData);					//クライアント全員に送る

						if (n.first.Recv(&tmpData)) {
							if (tmpData == 1) {					// ID取れたと識別出来たら
								n.second++;
							}
						}
						break;
					case 2://揃い待ち
						if (canMatch) { n.second++; }
						break;
					case 3:
						{
							PlayerNetData tmpPData;
							if (n.first.Recv(&tmpPData)) {							// クライアントから受信したら
								if (tmpPData.CheckSum == tmpPData.CalcCheckSum()) {
									m_ServerData.PlayerData[tmpPData.ID] = tmpPData;	// 更新
								}
							}
						}
						if (canSend) {
							n.first.SendtoClient(m_ServerData);						//クライアント全員に送る
						}
						break;
					default:
						break;
					}
					i++;
				}
				if (canSend) {
					NetCommonExecute(m_ServerData);			// 更新
				}
				return canMatch;
			}
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
				for (int i = 0; i < chara_num; i++) {
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
				for (int i = 0; i < chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), VECTOR_ref::vget(0.f + (float)(i - 1)*20.f, 0.f, -52.5f));
					if (i == 0) {
						c->SetUseRealTimePhysics(false);
						c->SetCharaType(CharaTypeID::Team);
					}
					else {
						c->SetUseRealTimePhysics(false);
						c->SetCharaType(CharaTypeID::Enemy);
					}
					m_ServerDataCommon.PlayerData[i].PosBuf = c->GetPosBuf();
					m_PrevServerData.PlayerData[i].PosBuf = c->GetPosBuf();	// サーバーデータ
					m_ServerData.PlayerData[i].PosBuf = c->GetPosBuf();
					m_ServerData.PlayerData[i].IsActive = 0;
				}

				m_ServerDataCommon.Frame = 0;
				m_PrevServerData.Frame = 0;
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

				this->ServerFrame = 0;
			}
			//
			bool Update(void) noexcept override {



				auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, GetMyPlayerID()));//自分
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
							for (int i = 0; i < chara_num; i++) {
								if (i == GetMyPlayerID()) { continue; }
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
					for (int i = 0; i < chara_num; i++) {
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
					Chara->ExecuteRadBuf(m_PlayerData.Input);
					m_PlayerData.Input.SetRadBuf(Chara->GetRadBuf().x(), Chara->GetRadBuf().y());

					m_PlayerData.PosBuf = Chara->GetPosBuf();


					m_PlayerData.CheckSum = m_PlayerData.CalcCheckSum();
					//クライアント
					if (Client) {
						if ((Sequence == SequenceEnum::Matching) && SeqFirst) {
							ClientInit(UsePort, IPData);
						}
						if ((Sequence >= SequenceEnum::Matching) && ClientExecute()) {
							Sequence = SequenceEnum::MainGame;
						}
					}
					//サーバー
					else {
						if ((Sequence == SequenceEnum::Matching) && SeqFirst) {
							ServerInit(UsePort);
						}
						if ((Sequence >= SequenceEnum::Matching) && ServerExecute()) {
							Sequence = SequenceEnum::MainGame;
						}
					}
					//
					if (Sequence == SequenceEnum::MainGame) {
						for (int i = 0; i < chara_num; i++) {
							auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
							if (i == GetMyPlayerID()) {
								c->SetInput(m_PlayerData.Input, isready);
							}
							else {
								auto Total = (int)m_ServerDataCommon.Frame - (int)m_PrevServerData.Frame;
								if (Total <= 0) { Total = 20; }
								auto tmp = Lerp(m_PrevServerData.PlayerData[i], m_ServerDataCommon.PlayerData[i], (float)LeapFrame / (float)Total);
								LeapFrame = std::clamp<int>(LeapFrame + 1, 0, Total);
								c->SetInput(tmp.Input, isready);
								//差があるときだけ移動量変更
								auto siz = (tmp.PosBuf - c->GetPosBuf()).size();
								if ((siz > (1.0f*Scale_Rate)) && (siz < (2.0f*Scale_Rate))) {
									c->SetPosBufOverRide(tmp.PosBuf);
								}
							}
						}
					}
					else {
						for (int i = 0; i < chara_num; i++) {
							auto& c = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, i));
							if (i == GetMyPlayerID()) {
								c->SetInput(m_PlayerData.Input, isready);
							}
						}

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
				NetWorkDispose();
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
				for (int i = 0; i < chara_num; i++) {
					if (i == GetMyPlayerID()) { continue; }
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
				auto& Chara = (std::shared_ptr<CharacterClass>&)(*this->m_Obj.GetObj(ObjType::Human, GetMyPlayerID()));//自分

				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto Red = GetColor(255, 0, 0);
				auto Blue = GetColor(50, 50, 255);
				auto Green = GetColor(43, 163, 91);
				auto White = GetColor(212, 255, 239);
				unsigned int color = Red;
				//キャラ
				for (int i = 0; i < chara_num; i++) {
					if (i == GetMyPlayerID()) { continue; }
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
						for (int i = 0; i < chara_num; i++) {
							if (i == GetMyPlayerID()) { continue; }
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

					for (int i = 0; i < chara_num; i++) {
						if (i == GetMyPlayerID()) { continue; }
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
					//bool Mid_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0);
					//bool Right_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
					int mx = DXDraw::Instance()->disp_x / 2, my = DXDraw::Instance()->disp_y / 2;
					GetMousePoint(&mx, &my);
					int xp1, yp1, xp2, yp2;
					//unsigned int color;
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
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), " %d/%d", (int)Sequence, (int)SequenceEnum::MainGame);
					}
					xp1 = y_r(100);
					yp1 = y_r(550);
					if (Sequence > SequenceEnum::SelMode) {
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), "種別[%s]", Client ? "クライアント" : "サーバー"); yp1 += y_r(30);
					}
					if (Sequence > SequenceEnum::Set_Port) {
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), "使用ポート[%d-%d]", UsePort, UsePort + chara_num - 1); yp1 += y_r(30);
					}
					if (Sequence > SequenceEnum::SetTick) {
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), "ティックレート[%4.1f]", Frame_Rate / TickRate); yp1 += y_r(30);
					}
					if (Sequence == SequenceEnum::MainGame) {
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), "サーバー"); yp1 += y_r(30);
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), " Frame = [%d]", m_ServerDataCommon.Frame); yp1 += y_r(30);
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), " StartFlag = [%d]", m_ServerDataCommon.StartFlag); yp1 += y_r(30);
						Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), " Tmp1 = [%d]", m_ServerDataCommon.Tmp1); yp1 += y_r(30);
					}
					auto Prev = Sequence;
					switch (Sequence) {
					case SequenceEnum::SelMode:
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
								m_PlayerData.ID = 1;					//クライアント
								Sequence = SequenceEnum::Set_Port;
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
								m_PlayerData.ID = 0;					//サーバー
								Sequence = SequenceEnum::Set_Port;
							}
						}
					}
					break;
					case SequenceEnum::Set_Port://ポート
					{
						{
							xp1 = y_r(100);
							yp1 = y_r(400);
							xp2 = xp1 + y_r(400);
							yp2 = yp1 + y_r(30);
							color = GetColor(0, 0, 0);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), "ポート=[%d-%d]", UsePort, UsePort + chara_num - 1);
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
							if (into) {
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
								Sequence = SequenceEnum::SetTick;
							}
						}
					}
					break;
					case SequenceEnum::SetTick:
					{
						{
							xp1 = y_r(100);
							yp1 = y_r(400);
							xp2 = xp1 + y_r(400);
							yp2 = yp1 + y_r(30);
							color = GetColor(0, 0, 0);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), GetColor(0, 0, 0), "ティック=[%4.1f]", Frame_Rate / TickRate);
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
							if (into) {
								if (m_LeftClick.trigger()) {
									TickRate -= 1.f;
									TickRate = std::clamp(TickRate, 1.f, 20.f);
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
									TickRate += 1.f;
									TickRate = std::clamp(TickRate, 1.f, 20.f);
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
								if (Client) {
									Sequence = SequenceEnum::SetIP;
								}
								else {
									Sequence = SequenceEnum::Matching;
								}
							}
						}

					}
					break;
					case SequenceEnum::SetIP://IP
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
								IPData.d1 = (unsigned char)(IP[0]);
								IPData.d2 = (unsigned char)(IP[1]);
								IPData.d3 = (unsigned char)(IP[2]);
								IPData.d4 = (unsigned char)(IP[3]);
								Sequence = SequenceEnum::Matching;
							}
						}
					}
					break;
					case SequenceEnum::Matching:
					{
						{
							xp1 = y_r(100);
							yp1 = y_r(400);
							xp2 = xp1 + y_r(400);
							yp2 = yp1 + y_r(30);
							color = GetColor(0, 0, 0);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString(xp1, yp1, "他プレイヤー待機中…", GetColor(255, 255, 255));
						}
						for (int i = 0; i < chara_num; i++) {
							bool isActive = (((Client) ? m_ServerDataCommon : m_ServerData).PlayerData[i].IsActive == 1);
							{
								xp1 = y_r(200);
								yp1 = y_r(400) + y_r(35)*(i + 1);
								xp2 = xp1 + y_r(300);
								yp2 = yp1 + y_r(30);
								color = isActive ? GetColor(0, 0, 0) : GetColor(64, 64, 64);
								DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
								Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString(xp1, yp1, "Player", GetColor(255, 255, 255));

								Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString_RIGHT(xp2, yp1, (isActive ? "〇" : ""), GetColor(255, 255, 255));
							}
						}

					}
					break;
					case SequenceEnum::MainGame:
					{
						{
							xp1 = y_r(100);
							yp1 = y_r(400);
							xp2 = xp1 + y_r(400);
							yp2 = yp1 + y_r(30);
							color = GetColor(0, 0, 0);
							DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
							Fonts->Get(y_r(30), FontPool::FontType::Nomal_Edge).Get_handle().DrawString(xp1, yp1, "通信中!", GetColor(255, 255, 255));
						}
					}
					break;
					default:
						break;
					}
					SeqFirst = (Prev != Sequence);
				}
				//
			}
		};
	};
};
