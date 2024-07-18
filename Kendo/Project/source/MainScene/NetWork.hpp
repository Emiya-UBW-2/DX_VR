#pragma once
#pragma warning(disable:4464)
#include "../Header.hpp"
#include "../sub.hpp"

class UDPDX {
	int			m_Handle{-1};				// ネットワークハンドル
	IPDATA		m_SendIP{127,0,0,1};		// 送信用ＩＰアドレスデータ
	int			m_SendPort{-1};				// 通信用ポート
	IPDATA		RecvIp{127,0,0,1};			// 受信用ＩＰアドレスデータ
	int			RecvPort{0};				// 受信用ポート
public:
	auto			IsActive(void) const noexcept { return (m_Handle != -1); }
public:
	void			SetServerIP(const IPDATA& pIP) noexcept { m_SendIP = pIP; }//クライアントは必ず行う
	void			Init(bool IsServer, int PORT = -1) noexcept {
		if (!IsActive()) {
			m_SendPort = PORT;
			m_Handle = MakeUDPSocket(IsServer ? m_SendPort : -1);
		}
	}
	void			Dispose(void) noexcept {
		if (IsActive()) {
			DeleteUDPSocket(m_Handle);	// ＵＤＰソケットハンドルの削除
			m_Handle = -1;
			m_SendPort = -1;
		}
	}
private:
	template<class T>
	void			SendData(IPDATA& Ip, int SendPort, const T& Data) noexcept {
		if (IsActive()) {
			NetWorkSendUDP(m_Handle, Ip, SendPort, &Data, sizeof(T));
			//printfDx("送信:[%d,%d,%d,%d][%d]\n", Ip.d1, Ip.d2, Ip.d3, Ip.d4, SendPort);
		}
	}
public:
	//送信
	template<class T>
	void			SendData(const T& Data) noexcept { SendData(m_SendIP, m_SendPort, Data); }
	//受信
	template<class T>
	bool			RecvData(T* Data) noexcept {
		switch (CheckNetWorkRecvUDP(m_Handle)) {
			case TRUE:
				NetWorkRecvUDP(m_Handle, &RecvIp, &RecvPort, Data, sizeof(T), FALSE);		// 受信
				return true;
			case FALSE://待機
				break;
			default://error
				break;
		}
		return false;
	}
	//返送
	template<class T>
	void			ReturnData(const T& Data) noexcept { SendData(RecvIp, RecvPort, Data); }
};

namespace FPS_n2 {
	namespace Sceneclass {
		static const int		Player_num = 2;

		using NetTime = LONGLONG;
		//クライアントが送信するデータ
		class PlayerNetData {
		private:
			bool						m_IsActive{ false };
			size_t						m_CheckSum{};			//チェックサム
			NetTime						m_Frame{};			//そのプレイヤーのインゲーム時間
			PlayerID					m_ID{0};
			InputControl				m_Input;
			moves						m_move;
			std::array<DamageEvent,64>	m_DamageEvents;
		private:
			auto			CalcCheckSum(void) const noexcept {
				return (
					(static_cast<int>(this->m_move.pos.x*100.f) + static_cast<int>(this->m_move.pos.y*100.f) + static_cast<int>(this->m_move.pos.z*100.f)) +
					(static_cast<int>(this->m_move.vec.x*100.f) + static_cast<int>(this->m_move.vec.y*100.f) + static_cast<int>(this->m_move.vec.z*100.f)) +
					static_cast<int>(rad2deg(this->m_move.rad.y)) +
					static_cast<int>(m_ID)
					);
			}
		public:
			auto			GetIsActive(void) const noexcept { return this->CalcCheckSum() != 0; }
			auto			IsCheckSum(void) const noexcept { return m_CheckSum == (size_t)this->CalcCheckSum(); }
			const auto&		GetID(void) const noexcept { return this->m_ID; }
			const auto&		GetFrame(void) const noexcept { return this->m_Frame; }
			const auto&		GetInput(void) const noexcept { return this->m_Input; }
			const auto&		GetMove(void) const noexcept { return this->m_move; }
			const auto&		GetDamageEvents(void) const noexcept { return this->m_DamageEvents; }
		public:
			void			SetActive(bool value) noexcept { m_IsActive = value; }
			void			SetID(PlayerID value) noexcept { m_ID = value; }
			void			SetMove(const moves& value) noexcept { m_move = value; }
		public:
			void			SetMyPlayer(const InputControl& pInput, const moves & move_t, const std::array<DamageEvent, 64>& Damage_t) noexcept {
				this->m_Input = pInput;
				this->m_move = move_t;
				this->m_DamageEvents = Damage_t;
			}
			void			SetData(const PlayerNetData& o, NetTime pFrame) noexcept {
				this->m_Input = o.m_Input;
				this->m_move = o.m_move;
				this->m_DamageEvents = o.m_DamageEvents;
				this->m_Frame = pFrame;
				this->m_CheckSum = (size_t)this->CalcCheckSum();
			}
		public:
			static auto		GetLerpData(const PlayerNetData&PrevData, const PlayerNetData& ServerData, float Per, bool isYradReset) noexcept {
				PlayerNetData tmp;
				//
				tmp.m_Input = Lerp(PrevData.m_Input, ServerData.m_Input, Per);
				tmp.m_move = PrevData.m_move.LerpMove(ServerData.m_move, Per);
				//
				tmp.m_ID = ServerData.m_ID;
				tmp.m_IsActive = ServerData.m_IsActive;
				tmp.m_Frame = ServerData.m_Frame;
				tmp.m_Input.SetKeyInputFlags(ServerData.m_Input);
				if (isYradReset) {
					auto radvec = Lerp(Matrix4x4DX::RotAxis(Vector3DX::up(), PrevData.m_move.rad.y).zvec(), Matrix4x4DX::RotAxis(Vector3DX::up(), ServerData.m_move.rad.y).zvec(), Per).normalized();
					tmp.m_move.rad.y = (-atan2f(radvec.x, radvec.z));
				}
				tmp.m_DamageEvents = ServerData.m_DamageEvents;
				return tmp;
			}
		};
		//サーバーが管理するデータ
		class ServerNetData {
		private:
		public:
			int					PlayerID{0};			//4
			int					StartFlag{0};			//4
			size_t				ServerFrame{0};			//8
			PlayerNetData		PlayerData[Player_num];	//37 * 3
		};
		//通信
		class PlayerNetWork {
		private:
		//protected:
			size_t											m_ServerFrame{0};
			std::array<int, Player_num>						m_LeapFrame{};
			ServerNetData									m_ServerDataCommon;	//最新の受信データ
			ServerNetData									m_PrevServerData;	//前回の受信データ
			PlayerNetData									m_PlayerData;		//ローカルのプレイヤーデータ
		public:
			PlayerNetWork(void) noexcept {}
			PlayerNetWork(const PlayerNetWork&) = delete;
			PlayerNetWork(PlayerNetWork&& o) = delete;
			PlayerNetWork& operator=(const PlayerNetWork&) = delete;
			PlayerNetWork& operator=(PlayerNetWork&& o) = delete;

			~PlayerNetWork(void) noexcept {}
		public:
			auto			CanGetRecvData(int pPlayerID) const noexcept { return this->m_LeapFrame[static_cast<size_t>(pPlayerID)] <= 1; }
			const auto&		GetServerDataCommon(void) const noexcept { return this->m_ServerDataCommon; }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_PlayerData.GetID(); }

			auto&			SetPlayerData(void) noexcept { return this->m_PlayerData; }
		public:
			void			SetPlayerID(PlayerID ID) noexcept { this->m_PlayerData.SetID(ID); }
			void			SetData(const PlayerNetData& pdata, NetTime pFrame) noexcept { this->m_PlayerData.SetData(pdata, pFrame); }
		public:
			auto			GetNowServerPlayerData(PlayerID pPlayerID, bool isyradReset) noexcept {
				size_t ID = static_cast<size_t>(pPlayerID);
				auto Total = static_cast<int>(this->m_ServerDataCommon.ServerFrame) - static_cast<int>(this->m_PrevServerData.ServerFrame);
				if (Total <= 0) { Total = 20; }
				PlayerNetData tmp;
				tmp = PlayerNetData::GetLerpData(
					this->m_PrevServerData.PlayerData[ID], this->m_ServerDataCommon.PlayerData[ID],
					static_cast<float>(this->m_LeapFrame[ID]) / static_cast<float>(Total), isyradReset);
				this->m_LeapFrame[ID] = std::clamp<int>(this->m_LeapFrame[ID] + 1, 0, Total);
				return tmp;
			}
		public:
			void			Int() noexcept {
				this->m_ServerFrame = 0;
				this->m_ServerDataCommon.ServerFrame = 0;
			}
			void			UpdateDataCommon(const ServerNetData& pData) noexcept {
				auto& tmpData = pData;
				if (this->m_ServerFrame <= tmpData.ServerFrame && ((tmpData.ServerFrame - this->m_ServerFrame) < 60)) {
					for (size_t i = 0; i < static_cast<size_t>(Player_num); i++) {
						this->m_LeapFrame[i] = 0;
					}
					this->m_PrevServerData = this->m_ServerDataCommon;
					this->m_ServerFrame = tmpData.ServerFrame;
					this->m_ServerDataCommon = tmpData;
				}
			}
		};
		//
		enum class ClientPhase {
			WaitConnect = 0,
			GetNumber,
			Ready,
		};
		struct UDPS {
			UDPDX					m_NetWork;
			ClientPhase				m_Phase{};

		public:
			bool IsReady() { return m_Phase == ClientPhase::Ready; }
		};
		//サーバー専用
		class ServerControl {
			ServerNetData			m_ServerData;
			std::array<UDPS, Player_num - 1>		m_Net;
		public:
			ServerControl(void) noexcept {}
			ServerControl(const ServerControl&) = delete;
			ServerControl(ServerControl&& o) = delete;
			ServerControl& operator=(const ServerControl&) = delete;
			ServerControl& operator=(ServerControl&& o) = delete;

			~ServerControl(void) noexcept {}
		private:
			bool AllReady() {
				bool canMatch = true;
				for (auto& n : this->m_Net) {
					if (!n.IsReady()) {
						canMatch = false;
						break;
					}
				}
				return canMatch;
			}
		public:
			const auto&		GetServerData(void) const noexcept { return this->m_ServerData; }
		public:
			void SetPlayerData(const PlayerNetData& value) { this->m_ServerData.PlayerData[value.GetID()] = value; }
		public:
			void			Init(int pPort) noexcept {
				int i = 0;
				for (auto & n : this->m_Net) {
					n.m_NetWork.Init(true, pPort + i); i++;
					n.m_Phase = ClientPhase::WaitConnect;
				}
				this->m_ServerData.PlayerID = 0;
				this->m_ServerData.StartFlag = FALSE;
				this->m_ServerData.ServerFrame = 0;
			}
			bool			Execute(PlayerNetData* MyPlayerData, bool IsUpdateTick) noexcept {
				PlayerNetData tmpData;
				for (auto & n : this->m_Net) {
					size_t index = static_cast<size_t>(&n - &this->m_Net.front());
					switch (n.m_Phase) {
						case ClientPhase::WaitConnect:						//無差別受付
							if (n.m_NetWork.RecvData(&tmpData)) {			// 該当ソケットにクライアントからなにか受信したら
								n.m_Phase = ClientPhase::GetNumber;
							}
							break;
						case ClientPhase::GetNumber:
							this->m_ServerData.PlayerID = static_cast<int>(index) + 1;
							n.m_NetWork.ReturnData(this->m_ServerData);					//クライアント全員に送る

							if (n.m_NetWork.RecvData(&tmpData)) {
								if (tmpData.GetIsActive()) {					// ID取れたと識別出来たら
									n.m_Phase = ClientPhase::Ready;
								}
							}
							break;
						case ClientPhase::Ready://揃い待ち
							break;
						default:
							break;
					}
				}
				if (AllReady()) {
					//サーバーデータの更新
					this->m_ServerData.StartFlag = TRUE;
					SetPlayerData(*MyPlayerData);// サーバープレイヤーののプレイヤーデータ
					this->m_ServerData.ServerFrame++; // サーバーフレーム更新
					for (auto & n : this->m_Net) {
						if (n.m_NetWork.RecvData(&tmpData)) {	// クライアントから受信したら
							if (tmpData.IsCheckSum()) {		// チェックサムののち
								SetPlayerData(tmpData);		// 更新
							}
						}
						if (IsUpdateTick) {
							n.m_NetWork.ReturnData(this->m_ServerData);						//クライアント全員に送る
						}
					}
					return true;
				}
				return false;
			}
			void			Dispose(void) noexcept {
				for (auto & n : this->m_Net) {
					n.m_NetWork.Dispose();
				}
			}
		};
		class ClientControl {
			int						m_NetWorkSel{0};
			float					m_CannotConnectTimer{0.f};
			int						m_Port{0};
			IPDATA					m_IP{127,0,0,1};
			UDPS					m_Net;
			PlayerID				m_MyID{};
			bool					m_IsServerDataUpdated{};
			ServerNetData			m_ServerData;
		public:
			ClientControl(void) noexcept {}
			ClientControl(const ClientControl&) = delete;
			ClientControl(ClientControl&& o) = delete;
			ClientControl& operator=(const ClientControl&) = delete;
			ClientControl& operator=(ClientControl&& o) = delete;

			~ClientControl(void) noexcept {}
		public:
			const auto&		IsServerDataUpdated(void) const noexcept { return this->m_IsServerDataUpdated; }
			const auto&		GetServerData(void) const noexcept { return this->m_ServerData; }
			const auto&		GetMyID(void) const noexcept { return this->m_MyID; }
			auto			CanGetMyID(void) const noexcept { return this->m_Net.m_Phase != ClientPhase::WaitConnect; }
		public:
			void			Init(int pPort, const IPDATA& pIP) noexcept {
				this->m_CannotConnectTimer = 0.f;
				this->m_Port = pPort;
				this->m_IP = pIP;
				this->m_NetWorkSel = 0;
				this->m_Net.m_NetWork.SetServerIP(pIP);
				this->m_Net.m_NetWork.Init(false, this->m_Port + this->m_NetWorkSel);
				this->m_Net.m_Phase = ClientPhase::WaitConnect;
			}
			bool			Execute(PlayerNetData* MyPlayerData, bool IsUpdateTick) noexcept {
				auto* DrawParts = DXDraw::Instance();
				m_IsServerDataUpdated = this->m_Net.m_NetWork.RecvData(&m_ServerData);
				switch (this->m_Net.m_Phase) {
					case ClientPhase::WaitConnect:
						MyPlayerData->SetActive(false);
						this->m_Net.m_NetWork.SendData(*MyPlayerData);			// 通信リクエスト
						//サーバーからの自分のIDを受信
						if (m_IsServerDataUpdated) {
							m_CannotConnectTimer = 0.f;
							if (m_ServerData.PlayerID > 0) {
								m_MyID = (PlayerID)m_ServerData.PlayerID;
								this->m_Net.m_Phase = ClientPhase::GetNumber;
							}
						}
						else {
							m_CannotConnectTimer += 1.f / DrawParts->GetFps();
							if (this->m_CannotConnectTimer > 1.f) {
								m_CannotConnectTimer -= 1.f;
								this->m_NetWorkSel++;
								this->m_Net.m_NetWork.Dispose();
								this->m_Net.m_NetWork.SetServerIP(this->m_IP);
								this->m_Net.m_NetWork.Init(false, this->m_Port + this->m_NetWorkSel);
								this->m_Net.m_Phase = ClientPhase::WaitConnect;
								if (this->m_NetWorkSel >= Player_num) {
									//満タン
								}
							}
						}
						break;
					case ClientPhase::GetNumber:
						MyPlayerData->SetActive(true);
						this->m_Net.m_NetWork.SendData(*MyPlayerData);								// ID取れたよ
						//サーバーからのデータを受信したら次へ
						if (m_IsServerDataUpdated) {
							if (m_ServerData.StartFlag == 1) {
								this->m_Net.m_Phase = ClientPhase::Ready;
							}
						}
						break;
					case ClientPhase::Ready:
						if (IsUpdateTick) {
							this->m_Net.m_NetWork.SendData(*MyPlayerData);			// 自身のデータを送信
						}
						break;
					default:
						break;
				}
				return this->m_Net.IsReady();
			}
			void			Dispose(void) noexcept {
				this->m_Net.m_NetWork.Dispose();
			}
		};
	};
};
