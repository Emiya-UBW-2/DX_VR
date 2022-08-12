#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			static const int		Vehicle_num = 3;
			static const int		Player_num = 3;
		private:
			//リソース関連
			ObjectManager			m_Obj;						//モデル
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
				size_t			CheckSum{ 0 };		//8 * 1	=  8byte
				InputControl	Input;				//4 * 5	= 20byte
				VECTOR_ref		PosBuf;				//4 * 3	= 12byte
				VECTOR_ref		VecBuf;				//4 * 3	= 12byte
				float			YradBuf;			//4 * 1	=  4byte
				char			ID{ 0 };			//1	* 1	=  1byte
				char			IsActive{ 0 };		//1	* 1	=  1byte
				double			Frame{ 0.0 };		//8 * 1 =  8byte
				unsigned char	DamageSwitch{ 0 };	//1 * 1 =  1byte
				DamageEvent		Damage{ 100 };		//9 * 1 =  9byte
													//		  76byte
			public:
				const auto	CalcCheckSum(void) noexcept {
					return (size_t)(
							((int)(PosBuf.x()) + (int)(PosBuf.y()) + (int)(PosBuf.z())) + 
							((int)(VecBuf.x()*100.f) + (int)(VecBuf.y()*100.f) + (int)(VecBuf.z())*100.f) + 
							(int)(rad2deg(YradBuf)) + 
							(int)(ID)
						);
				}

				const PlayerNetData operator+(const PlayerNetData& o) const noexcept {
					PlayerNetData tmp;

					tmp.ID = o.ID;
					tmp.Input = this->Input + o.Input;
					tmp.PosBuf = this->PosBuf + o.PosBuf;
					tmp.VecBuf = this->VecBuf + o.VecBuf;
					tmp.YradBuf = this->YradBuf + o.YradBuf;

					return tmp;
				}
				const PlayerNetData operator-(const PlayerNetData& o) const noexcept {
					PlayerNetData tmp;

					tmp.ID = o.ID;
					tmp.Input = this->Input - o.Input;
					tmp.PosBuf = this->PosBuf - o.PosBuf;
					tmp.VecBuf = this->VecBuf - o.VecBuf;
					tmp.YradBuf = this->YradBuf - o.YradBuf;

					return tmp;
				}
				const PlayerNetData operator*(float per) const noexcept {
					PlayerNetData tmp;

					tmp.ID = this->ID;
					tmp.Input = this->Input*per;
					tmp.PosBuf = this->PosBuf*per;
					tmp.VecBuf = this->VecBuf*per;
					tmp.YradBuf = this->YradBuf*per;
					return tmp;
				}
			};
			struct ServerNetData {
				int					Tmp1{ 0 };				//4
				int					StartFlag{ 0 };			//4
				size_t				ServerFrame{ 0 };		//8
				PlayerNetData		PlayerData[Player_num];	//37 * 3
			};
			class NetWorkControl {
			protected:
			protected:
				std::vector<std::pair<NewWorkControl, int>> m_NetWork;
				size_t					m_ServerFrame{ 0 };
				std::array<int, Player_num>		m_LeapFrame;
				ServerNetData			m_ServerDataCommon, m_PrevServerData;
				PlayerNetData			m_PlayerData;
				float					m_TickCnt{ 0.f };
				float					m_TickRate{ 10.f };
			public:
				const auto	GetRecvData(int PlayerID) const noexcept { return this->m_LeapFrame[PlayerID] <= 1; }
				const auto& GetServerDataCommon() const noexcept { return this->m_ServerDataCommon; }
				const auto& GetMyPlayer() const noexcept { return this->m_PlayerData; }
				void			SetMyPlayer(const InputControl& pInput, const VECTOR_ref& pPos, const VECTOR_ref& pVec, float pYrad, double pFrame, const DamageEvent& pDamage, char pDamageSwitch) noexcept {
					this->m_PlayerData.Input = pInput;
					this->m_PlayerData.PosBuf = pPos;
					this->m_PlayerData.VecBuf = pVec;
					this->m_PlayerData.YradBuf = pYrad;
					this->m_PlayerData.Frame = pFrame;
					this->m_PlayerData.Damage = pDamage;
					this->m_PlayerData.DamageSwitch = pDamageSwitch;
					this->m_PlayerData.CheckSum = this->m_PlayerData.CalcCheckSum();
				}

				const auto	GetNowServerPlayerData(int PlayerID) {
					auto Total = (int)this->m_ServerDataCommon.ServerFrame - (int)this->m_PrevServerData.ServerFrame;
					if (Total <= 0) { Total = 20; }
					auto Per = (float)this->m_LeapFrame[PlayerID] / (float)Total;
					auto tmp = Lerp(this->m_PrevServerData.PlayerData[PlayerID], this->m_ServerDataCommon.PlayerData[PlayerID], Per);

					auto radvec = Lerp(MATRIX_ref::RotY(this->m_PrevServerData.PlayerData[PlayerID].YradBuf).zvec(), MATRIX_ref::RotY(this->m_ServerDataCommon.PlayerData[PlayerID].YradBuf).zvec(), Per).Norm();
					tmp.YradBuf = -atan2f(radvec.x(), radvec.z());
					tmp.Frame = this->m_ServerDataCommon.PlayerData[PlayerID].Frame;
					tmp.Damage = this->m_ServerDataCommon.PlayerData[PlayerID].Damage;
					tmp.DamageSwitch = this->m_ServerDataCommon.PlayerData[PlayerID].DamageSwitch;
					this->m_LeapFrame[PlayerID] = std::clamp<int>(this->m_LeapFrame[PlayerID] + 1, 0, Total);
					return tmp;
				}
				virtual void			SetParam(int PlayerID, const VECTOR_ref& pPos) noexcept {
					this->m_ServerDataCommon.PlayerData[PlayerID].PosBuf = pPos;
					this->m_ServerDataCommon.ServerFrame = 0;
					this->m_PrevServerData.PlayerData[PlayerID].PosBuf = this->m_ServerDataCommon.PlayerData[PlayerID].PosBuf;	// サーバーデータ
					this->m_PrevServerData.ServerFrame = 0;
				}
				void			NetWorkDispose(void) noexcept {
					for (auto & n : m_NetWork) {
						n.first.Dispose();
					}
					m_NetWork.clear();
				}
			protected:
				void			NetWorkInit(void) noexcept {
					this->m_ServerFrame = 0;
				}
				void			NetCommonExecute(const ServerNetData& pData) {
					auto& tmpData = pData;
					if (this->m_ServerFrame <= tmpData.ServerFrame && ((tmpData.ServerFrame - this->m_ServerFrame) < 60)) {
						for (int i = 0; i < Player_num; i++) {
							this->m_LeapFrame[i] = 0;
						}
						this->m_PrevServerData = this->m_ServerDataCommon;
						this->m_ServerFrame = tmpData.ServerFrame;
						this->m_ServerDataCommon = tmpData;
					}
				}
			};
			class ServerControl : public NetWorkControl {
				ServerNetData			m_ServerData;
			public:
				const auto& GetServerData() const noexcept { return this->m_ServerData; }
				void			SetParam(int PlayerID, const VECTOR_ref& pPos) noexcept override {
					NetWorkControl::SetParam(PlayerID, pPos);
					this->m_ServerData.PlayerData[PlayerID].PosBuf = this->m_ServerDataCommon.PlayerData[PlayerID].PosBuf;
					this->m_ServerData.PlayerData[PlayerID].IsActive = 0;
				}
			public:
				void			ServerInit(int pPort, float pTick) {
					NetWorkInit();
					m_NetWork.resize(Player_num - 1);
					int i = 0;
					for (auto & n : m_NetWork) {
						n.first.Set_Port(pPort + i); i++;
						n.first.InitServer();
						n.second = 0;
					}
					this->m_ServerDataCommon.ServerFrame = 0;

					this->m_ServerData.Tmp1 = 0;
					this->m_ServerData.StartFlag = 0;
					this->m_ServerData.PlayerData[0].IsActive = 1;
					this->m_ServerData.ServerFrame = 0;

					this->m_PlayerData.ID = 0;
					this->m_TickRate = pTick;
				}
				bool ServerExecute(void) noexcept {
					bool canMatch = true;
					bool canSend = false;
					for (auto & n : m_NetWork) {
						if (!(n.second >= 2)) {
							canMatch = false;
						}
					}
					if (canMatch) {
						// ティックレート用演算
						this->m_TickCnt += 60.f / FPS;
						if (this->m_TickCnt > this->m_TickRate) {
							this->m_TickCnt -= this->m_TickRate;
							canSend = true;
						}
						//サーバーデータの更新
						this->m_ServerData.StartFlag = 1;
						this->m_ServerData.PlayerData[GetMyPlayer().ID] = this->m_PlayerData;		// サーバープレイヤーののプレイヤーデータ
						this->m_ServerData.ServerFrame++;											// サーバーフレーム更新
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
							this->m_ServerData.Tmp1 = 1 + i;
							this->m_ServerData.StartFlag = 0;
							this->m_ServerData.PlayerData[1 + i].IsActive = 1;

							n.first.SendtoClient(this->m_ServerData);					//クライアント全員に送る

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
									this->m_ServerData.PlayerData[tmpPData.ID] = tmpPData;	// 更新
								}
							}
						}
						if (canSend) {
							n.first.SendtoClient(this->m_ServerData);						//クライアント全員に送る
						}
						break;
						default:
							break;
						}
						i++;
					}
					if (canSend) {
						NetCommonExecute(this->m_ServerData);			// 更新
					}
					return canMatch;
				}
			};
			class ClientControl : public NetWorkControl {
				int						m_NetWorkSel{ 0 };
				float					m_CannotConnectTimer{ 0.f };
			public:
				void			ClientInit(int pPort, float pTick, const IPDATA& pIP) {
					NetWorkInit();
					m_NetWorkSel = 0;
					m_CannotConnectTimer = 0.f;
					m_NetWork.resize(Player_num - 1);
					int i = 0;
					for (auto & n : m_NetWork) {
						n.first.Set_Port(pPort + i); i++;
						n.first.SetIP(pIP);
						n.first.InitClient();
						n.second = 0;
					}

					this->m_PlayerData.ID = 1;
					this->m_TickRate = pTick;
				}
				bool ClientExecute(void) noexcept {
					ServerNetData tmpData;
					bool canMatch = true;
					bool canSend = false;
					canMatch = (m_NetWork[m_NetWorkSel].second >= 2);
					if (canMatch) {
						// ティックレート用演算
						this->m_TickCnt += 60.f / FPS;
						if (this->m_TickCnt > this->m_TickRate) {
							this->m_TickCnt -= this->m_TickRate;
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
								this->m_PlayerData.ID = (char)tmpData.Tmp1;
								m_NetWork[m_NetWorkSel].second++;
							}
						}
						else {
							m_CannotConnectTimer += 1.f / FPS;
							if (m_CannotConnectTimer > 1.f) {
								m_CannotConnectTimer = 0.f;
								m_NetWork[m_NetWorkSel].first.Dispose();
								m_NetWorkSel++;
								if (m_NetWorkSel >= Player_num) {
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
							m_NetWork[m_NetWorkSel].first.SendtoServer(this->m_PlayerData);				//自身のデータを送信
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
			};
			enum class SequenceEnum {
				SelMode,
				Set_Port,
				SetTick,
				SetIP,
				Matching,
				MainGame,
			};
			//サーバー専用
			ServerControl			m_ServerCtrl;
			//クライアント専用
			ClientControl			m_ClientCtrl;
			//int						IP[4]{ 127,0,0,1 };
			int						IP[4]{ 58,188,85,163 };
			IPDATA					IPData;				// 送信用ＩＰアドレスデータ
			//共通
			bool					m_IsClient{ true };
			SequenceEnum			m_Sequence{ SequenceEnum::SelMode };
			bool					SeqFirst{ false };
			int						UsePort{ 10850 };
			float					m_Tick{ 10.f };

			double					m_ClientFrame{ 0.0 };
			float					m_Ping{ 0.f };

			std::vector<DamageEvent>	m_DamageEvents;

			bool IsRide = true;

		private:
			const auto& GetMyPlayerID() const noexcept { return (this->m_IsClient) ? m_ClientCtrl.GetMyPlayer().ID : m_ServerCtrl.GetMyPlayer().ID; }
		public:
			using TEMPSCENE::TEMPSCENE;
			void			Set(void) noexcept override {
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
				VhehicleData::Set_Pre(&this->vehc_data, "data/tank/");								//戦車
				for (auto& t : this->vehc_data) { t.Set(); }										//戦車2
				//
				this->m_Obj.Init(&this->m_BackGround.GetGroundCol());
				for (int i = 0; i < Vehicle_num; i++) {
					this->m_Obj.AddObject(ObjType::Vehicle, ("data/tank/" + this->vehc_data[0].GetName() + "/").c_str());
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
				//
				TEMPSCENE::Set();
				//Set
				//登録
				for (int i = 0; i < Vehicle_num; i++) {
					auto& v = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, i));
					vehicle_Pool.emplace_back(v);

					VECTOR_ref pos_t = VECTOR_ref::vget(0.f + (float)(i)*10.f*Scale_Rate, 0.f, 0.f);
					auto HitResult = this->m_BackGround.GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
					if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
					v->ValueSet(deg2rad(0), deg2rad(90), pos_t, &vehc_data[0], hit_pic, this->m_BackGround.GetBox2Dworld(), &this->vehicle_Pool, &v);
				}
				{
					auto& Vehicle = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, 0));//自分
					if (IsRide) {
						this->m_HPBuf = (float)Vehicle->GetHP();
						this->m_ScoreBuf = Vehicle->GetScore();
					}
				}
				//Cam
				camera_main.set_cam_info(deg2rad(65), 1.f, 100.f);
				camera_main.set_cam_pos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				Set_zoom_lens(3.5f);
				//サウンド
				auto SE = SoundPool::Instance();
				//SE->Add((int)SoundEnum::Shot_Gun, 3, "data/Sound/SE/gun/shot.wav");
				//SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
				for (int i = 0; i < 4; i++) {
					//SE->Add((int)SoundEnum::Cocking0 + i, 3, "data/Sound/SE/gun/slide/bolt/" + std::to_string(i) + ".wav");
				}
				//SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
				//SE->Add((int)SoundEnum::SlideFoot, 9, "data/Sound/SE/move/sliding.wav");
				//SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
				//SE->Add((int)SoundEnum::Heart, 9, "data/Sound/SE/move/heart.wav");
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

				//SE->Get((int)SoundEnum::Shot_Gun).SetVol_Local(128);
				//SE->Get((int)SoundEnum::Trigger).SetVol_Local(128);
				for (int i = 0; i < 4; i++) {
					//SE->Get((int)SoundEnum::Cocking0 + i).SetVol_Local(128);
				}
				//SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
				//SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
				//SE->Get((int)SoundEnum::GateOpen).SetVol_Local(128);

				SE->Get((int)SoundEnum::Tank_Shot).SetVol(0.5f);
				SE->Get((int)SoundEnum::Tank_engine).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Ricochet).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Damage).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Eject).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Reload).SetVol(0.25f);


				//入力
				this->m_FPSActive.Init(false);
				this->m_MouseActive.Init(false);

				this->m_DamageEvents.clear();
			}
			//
			bool Update(void) noexcept override {
				auto& Vehicle = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, GetMyPlayerID()));//自分
				//FirstDoing
				if (IsFirstLoop) {
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
					this->m_Env.play(DX_PLAYTYPE_LOOP, TRUE);
				}
				//Input,AI
				{

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
									(input.Buttons[10] != 0), (input.Buttons[5] != 0), (input.Buttons[4] != 0), (input.Buttons[3] != 0)/*□*/, (input.Buttons[1] != 0)/*〇*/
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
							(CheckHitKeyWithCheck(KEY_INPUT_F) != 0),
							(CheckHitKeyWithCheck(KEY_INPUT_J) != 0),
							(((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0) && this->m_MouseActive.on())
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
					{
						MyInput.SetRadBuf(Vehicle->GetViewxRad(), Vehicle->GetViewyRad());
					}
					//クライアント
					if (this->m_IsClient) {
						m_ClientCtrl.SetMyPlayer(MyInput, Vehicle->GetMove().pos, Vehicle->GetMove().vec, Vehicle->Get_body_yrad(), this->m_ClientFrame, Vehicle->GetDamageEvent(), (Vehicle->GetDamageSwitch() ? 1 : 0));
						if ((this->m_Sequence == SequenceEnum::Matching) && SeqFirst) {
							m_ClientCtrl.ClientInit(UsePort, this->m_Tick, IPData);
						}
						if ((this->m_Sequence >= SequenceEnum::Matching) && m_ClientCtrl.ClientExecute()) {
							this->m_Sequence = SequenceEnum::MainGame;
						}
						for (int i = 0; i < Vehicle_num; i++) {
							auto& v = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, i));
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
						m_ServerCtrl.SetMyPlayer(MyInput, Vehicle->GetMove().pos, Vehicle->GetMove().vec, Vehicle->Get_body_yrad(), this->m_ClientFrame, Vehicle->GetDamageEvent(), (Vehicle->GetDamageSwitch() ? 1 : 0));
						if ((this->m_Sequence == SequenceEnum::Matching) && SeqFirst) {
							m_ServerCtrl.ServerInit(UsePort, this->m_Tick);
						}
						if ((this->m_Sequence >= SequenceEnum::Matching) && m_ServerCtrl.ServerExecute()) {
							this->m_Sequence = SequenceEnum::MainGame;
						}
					}
					//
					bool isready = true;
					if (this->m_Sequence == SequenceEnum::MainGame) {
						for (int i = 0; i < Vehicle_num; i++) {
							auto& v = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, i));
							auto tmp = (this->m_IsClient) ? m_ClientCtrl.GetNowServerPlayerData(i) : m_ServerCtrl.GetNowServerPlayerData(i);
							if (i == GetMyPlayerID()) {
								if (IsRide) {
									MyInput.SetKeyInput(tmp.Input.GetKeyInput());//キーフレームだけサーバーに合わせる
									v->SetInput(MyInput, isready, false);
									//v->SetInput(tmp.Input, isready, false);

									if ((this->m_IsClient) ? m_ClientCtrl.GetRecvData(i) : m_ServerCtrl.GetRecvData(i)) {
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
										auto& v2 = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, j));
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
						for (int i = 0; i < Vehicle_num; i++) {
							auto& v = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, i));
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
						auto& v = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, i));
						for (int i = 0; i < this->m_DamageEvents.size(); i++) {
							if (v->SetDamageEvent(this->m_DamageEvents[i])) {
								std::swap(this->m_DamageEvents.back(), this->m_DamageEvents[i]);
								this->m_DamageEvents.pop_back();
								i--;
							}
						}
					}
				}
				//
				{
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
						auto& v = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, i));
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
				this->m_Obj.ExecuteObject();
				this->m_BackGround.GetBox2Dworld()->Step(1.f, 1, 1);//物理更新
				this->m_Obj.LateExecuteObject();
				//視点
				{
					Vehicle->Setcamera(camera_main, fov_base);
					{
						MATRIX_ref FreeLook;
						FreeLook = MATRIX_ref::RotAxis((camera_main.camvec - camera_main.campos).cross(camera_main.camup), this->m_TPS_XradR) * MATRIX_ref::RotAxis(camera_main.camup, this->m_TPS_YradR);
						Easing_Matrix(&m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

						VECTOR_ref CamVec = (camera_main.camvec - camera_main.campos).Norm();
						CamVec = Lerp(CamVec, MATRIX_ref::Vtrans(CamVec, m_FreeLookMat), this->m_TPS_Per);

						VECTOR_ref CamPos = Vehicle->Get_EyePos_Base();

						camera_main.campos = Lerp(camera_main.campos, (CamPos + CamVec * -100.f), this->m_TPS_Per);
						camera_main.camvec = CamPos + CamVec * 100.f;
					}
				}

				this->m_BackGround.Execute();
				//UIパラメーター
				{
					this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
					this->m_ScoreBuf += std::clamp((Vehicle->GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / FPS;

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
				Effect_UseControl::Update_Effect();
				return true;
			}
			void			Dispose(void) noexcept override {
				m_ServerCtrl.NetWorkDispose();
				m_ClientCtrl.NetWorkDispose();
				Effect_UseControl::Dispose_Effect();
				this->m_Obj.DisposeObject();
				this->vehicle_Pool.clear();
			}
			//
			void			Depth_Draw(void) noexcept override {
				this->m_BackGround.Draw();
				//this->m_Obj.DrawDepthObject();
			}
			void			BG_Draw(void) noexcept override {
				this->m_BackGround.BG_Draw();
			}
			void			Shadow_Draw_NearFar(void) noexcept override {
				this->m_BackGround.Shadow_Draw_NearFar();
				//this->m_Obj.DrawObject_Shadow();
			}
			void			Shadow_Draw(void) noexcept override {
				this->m_BackGround.Shadow_Draw();
				this->m_Obj.DrawObject_Shadow();
			}

			void			Main_Draw(void) noexcept override {
				this->m_BackGround.Draw();
				this->m_Obj.DrawObject();
				//this->m_Obj.DrawDepthObject();
				//シェーダー描画用パラメーターセット
				{
					Set_is_Blackout(false);
					Set_is_lens(false);
				}
				for (int i = 0; i < Vehicle_num; i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& v = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, i));
					auto pos = v->Set_MidPos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						v->SetCameraPosition(campos);
						v->SetCameraSize(std::max(80.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
					}
				}
			}
			void			Main_Draw2(void) noexcept override {
				this->m_Obj.DrawDepthObject();
			}
			void			LAST_Draw(void) noexcept override {
			}
			//UI表示
			void			UI_Draw(void) noexcept  override {
				auto& Vehicle = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, GetMyPlayerID()));//自分

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
				for (int i = 0; i < Vehicle_num; i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& v = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, i));
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

					auto BaseBos = Vehicle->GetMatrix().pos();

					xs1 = y_r((int)(256.f * 0.5f));
					ys1 = y_r((int)(256.f * 0.8f));
					xs2 = y_r((int)(256.f * 0.5f));
					ys2 = y_r((int)(256.f * 0.2f));
					auto base = Vehicle->GetLookVec().zvec()*-1.f;
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
						for (int i = 0; i < Vehicle_num; i++) {
							if (i == GetMyPlayerID()) { continue; }
							auto& v = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, i));
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

					for (int i = 0; i < Vehicle_num; i++) {
						if (i == GetMyPlayerID()) { continue; }
						auto& v = (std::shared_ptr<VehicleClass>&)(*this->m_Obj.GetObj(ObjType::Vehicle, i));
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

				//通信設定
				if (!this->m_MouseActive.on()) {
					int xp, yp, xs, ys;
					xp = y_r(100);
					yp = y_r(250);
					xs = y_r(400);
					ys = y_r(200);

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
						return into && m_LeftClick.trigger();
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
								if (m_LeftClick.trigger()) {
									UpFunc();
								}
								if (m_LeftClick.press()) {
									m_LeftPressTimer += 1.f / FPS;
									if (m_LeftPressTimer > 0.5f) {
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
								if (m_LeftClick.trigger()) {
									DownFunc();
								}
								if (m_LeftClick.press()) {
									m_LeftPressTimer += 1.f / FPS;
									if (m_LeftPressTimer > 0.5f) {
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
								Fonts->Get(y_h, FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xp1, yp1, GetColor(255, 255, 255), Black, "使用ポート[%d-%d]", UsePort, UsePort + Player_num - 1); yp1 += y_h;
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
							this->m_Sequence = SequenceEnum::Set_Port;
						}
						if (ClickBox(xp, yp + y_r(100), xp + xs, yp + y_r(100) + y_h, "サーバーになる")) {
							this->m_IsClient = false;
							this->m_Tick = 10.f;
							this->m_Sequence = SequenceEnum::Set_Port;
						}
						break;
					case SequenceEnum::Set_Port://ポート
						MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "ポート=[%d-%d]", UsePort, UsePort + Player_num - 1);
						AddSubBox(xp, yp + y_r(100), [&]() { UsePort++; }, [&]() { UsePort--; });
						if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
							this->m_Sequence = SequenceEnum::SetTick;
						}
						break;
					case SequenceEnum::SetTick:
						MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "ティック=[%4.1f]", Frame_Rate / this->m_Tick);
						AddSubBox(xp, yp + y_r(100), [&]() { this->m_Tick = std::clamp(this->m_Tick - 1.f, 1.f, 20.f); }, [&]() { this->m_Tick = std::clamp(this->m_Tick + 1.f, 1.f, 20.f); });
						if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
							this->m_Sequence = (this->m_IsClient) ? SequenceEnum::SetIP : SequenceEnum::Matching;
						}
						break;
					case SequenceEnum::SetIP://IP
						MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "IP=[%d,%d,%d,%d]", IP[0], IP[1], IP[2], IP[3]);
						for (int i = 0; i < 4; i++) {
							AddSubBox(y_r(100 + 70 * i), yp + y_r(100),
								[&]() {
								IP[i]++;
								if (IP[i] > 255) { IP[i] = 0; }
							}, [&]() {
								IP[i]--;
								if (IP[i] < 0) { IP[i] = 255; }
							});
						}
						if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
							IPData.d1 = (unsigned char)(IP[0]);
							IPData.d2 = (unsigned char)(IP[1]);
							IPData.d3 = (unsigned char)(IP[2]);
							IPData.d4 = (unsigned char)(IP[3]);
							this->m_Sequence = SequenceEnum::Matching;
						}
						break;
					case SequenceEnum::Matching:
						MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "他プレイヤー待機中…");
						for (int i = 0; i < Player_num; i++) {
							bool isActive = (((this->m_IsClient) ? m_ClientCtrl.GetServerDataCommon() : m_ServerCtrl.GetServerData()).PlayerData[i].IsActive == 1);
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
