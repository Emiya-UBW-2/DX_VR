#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"
#include	"../sub.hpp"


namespace FPS_n2 {
	namespace NetWork {
#if DEBUG_NET
		static const int		Player_num = 2;
#else
		static const int		Player_num = 10;
#endif

		using NetTime = LONGLONG;

		class MoveInfo {
		public:
			Vector3DX repos;		// 反映用座標
			Vector3DX pos;		// 反映用座標
			Vector3DX vec;		// 加速
			Matrix3x3DX mat;	// 回転
			Vector3DX WatchRad;	// 加速
		public:
			auto LerpMove(const MoveInfo& o, float Per) const noexcept {
				MoveInfo tmp;
				tmp.repos = Lerp(this->repos, o.repos, Per);
				tmp.pos = Lerp(this->pos, o.pos, Per);
				tmp.vec = Lerp(this->vec, o.vec, Per);
				tmp.mat = Lerp(this->mat, o.mat, Per);
				tmp.WatchRad = Lerp(this->WatchRad, o.WatchRad, Per);
				return tmp;
			}
		};

		//クライアントが送信するデータ ローカル版
		class PlayerSendData {
		private:
			InputControl				m_Input;
			MoveInfo					m_move;
			DamageEventControl			m_DamageEvent;
			int32_t						m_FreeData[10]{};
		public:
			const auto&		GetInput(void) const noexcept { return this->m_Input; }
			const auto&		GetMove(void) const noexcept { return this->m_move; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto&		GetFreeData(void) const noexcept { return this->m_FreeData; }
		public:
			void			PopDamageEvent(std::vector<DamageEvent>* pRet) noexcept { this->m_DamageEvent.Pop(pRet); }
		public:
			void			SetMyPlayer(const InputControl& pInput, const MoveInfo& move_t, const DamageEventControl& Damage_t, int32_t* pFreeData) noexcept {
				this->m_Input = pInput;
				this->m_move = move_t;
				this->m_DamageEvent = Damage_t;
				for (int loop = 0; loop < 10; ++loop) {
					this->m_FreeData[loop] = pFreeData[loop];
				}
			}
			static PlayerSendData	GetLerpData(const PlayerSendData& PrevData, const PlayerSendData& NewData, float Per) noexcept {
				PlayerSendData tmp;
				//
				tmp.m_Input = Lerp(PrevData.m_Input, NewData.m_Input, Per);
				tmp.m_Input.SetKeyInputFlags(NewData.m_Input);
				tmp.m_move = PrevData.m_move.LerpMove(NewData.m_move, Per);
				tmp.m_DamageEvent = NewData.m_DamageEvent;
				for (int loop = 0; loop < 10; ++loop) {
					tmp.m_FreeData[loop] = NewData.GetFreeData()[loop];
				}
				return tmp;
			}
			int			CalcCheckSum(void) const noexcept {
				return (
					(static_cast<int>(GetMove().pos.x * 100.f) + static_cast<int>(GetMove().pos.y * 100.f) + static_cast<int>(GetMove().pos.z * 100.f)) +
					(static_cast<int>(GetMove().vec.x * 100.f) + static_cast<int>(GetMove().vec.y * 100.f) + static_cast<int>(GetMove().vec.z * 100.f))
					);
			}
		};
		//クライアントが送信するデータ　通信版
		class PlayerNetData {
		private:
			size_t						m_CheckSum{};			//チェックサム
			NetTime						m_ClientTime{ 0 };		//そのプレイヤーのインゲーム時間
			PlayerSendData				m_PlayerSendData{};
			PlayerID					m_ID{ 0 };
		private:
			int			CalcCheckSum(void) const noexcept {
				return (
					500 +
					static_cast<int>(this->m_ID) +
					static_cast<int>(this->m_ClientTime) +
					this->m_PlayerSendData.CalcCheckSum()
					);
			}
		public:
			auto			IsCheckSum(void) const noexcept { return this->m_CheckSum == static_cast<size_t>(CalcCheckSum()); }
			const auto& GetClientTime(void) const noexcept { return this->m_ClientTime; }
			const auto& GetID(void) const noexcept { return this->m_ID; }
			const auto& GetPlayerSendData(void) const noexcept { return this->m_PlayerSendData; }
		public:
			void			SetID(PlayerID ID) noexcept { this->m_ID = ID; }
			void			PopDamageEvent(std::vector<DamageEvent>* pRet) noexcept { this->m_PlayerSendData.PopDamageEvent(pRet); }
		public:
			void			SetData(const PlayerSendData& sendData, NetTime ClientTime) noexcept {
				this->m_PlayerSendData = sendData;
				this->m_ClientTime = ClientTime;
				this->m_CheckSum = static_cast<size_t>(CalcCheckSum());
			}
		public:
			static PlayerNetData	GetLerpData(const PlayerNetData& PrevData, const PlayerNetData& NewData, float Per) noexcept {
				PlayerNetData tmp;
				//
				tmp.m_PlayerSendData = PlayerSendData::GetLerpData(PrevData.GetPlayerSendData(), NewData.GetPlayerSendData(), Per);
				//
				tmp.m_ID = NewData.m_ID;
				tmp.m_ClientTime = NewData.m_ClientTime;
				return tmp;
			}
		};
		enum class ServerPhase : int {
			Empty,
			Matching,
			Ingame,
		};
		//サーバーが管理するデータ
		class ServerNetData {
		private:
		public:
			size_t				m_CheckSum{};			//チェックサム
			size_t				m_ServerFrame{ 0 };			//サーバーフレーム
			PlayerNetData		m_PlayerData[Player_num];
			size_t				m_PlayerFill[Player_num]{ 0 };
			ServerPhase			m_ServerPhase{ ServerPhase::Empty };
		private:
			int					CalcCheckSum(void) const noexcept {
				int Players = 0;
				for (int loop = 0; loop < Player_num; ++loop) {
					Players += static_cast<int>(this->m_PlayerData[loop].IsCheckSum() ? 100 : 0);
					Players += static_cast<int>(this->m_PlayerFill[loop] * 10);
				}
				return (
					500 +
					static_cast<int>(this->m_ServerFrame) +
					Players
					);
			}
		public:
			const auto			IsCheckSum(void) const noexcept { return this->m_CheckSum == static_cast<size_t>(CalcCheckSum()); }
			const auto			GetEmptyID(void) const noexcept {
				for (int loop = 0; loop < Player_num; ++loop) {
					if (this->m_PlayerFill[loop] == 1) {
						return loop;
					}
				}
				return InvalidID;
			}
		public:
			void				SetCheckSum(void) noexcept { this->m_CheckSum = static_cast<size_t>(CalcCheckSum()); }
		public:
			void Init(void) noexcept {
				for (int loop = 0; loop < Player_num; ++loop) {
					this->m_PlayerFill[loop] = 0;
				}
			}
			ServerNetData(void) noexcept {
				Init();
			}
		};
		//通信
		class PlayerNetWork {
		private:
			size_t											m_LastServerFrame{ 0 };//最後にサーバーから受信したサーバーフレーム
			size_t											m_LeapFrame{};
			size_t											m_LeapFrameMax{};
			ServerNetData									m_LastServerDataBuffer;	//それぞれのコントローラーからもらったデータ
			ServerNetData									m_LastServerData;	//最新の受信データ
			ServerNetData									m_PrevServerData;	//前回の受信データ
			PlayerNetData									m_LocalData;		//ローカルのプレイヤーデータ
			NetTime											m_TickCnt{ 0 };
			NetTime											m_TickRate{ 0 };
			NetTime											m_PrevFrame{ 0 };
			NetTime											m_ClientTime{ 0 };	//そのプレイヤーのインゲーム時間
			NetTime											m_PrevTime{ 0 };

			bool											m_TickUpdateFlag{ false };
			LONGLONG										m_PingTime{ 0 };
		public:
			PlayerNetWork(void) noexcept {}
			PlayerNetWork(const PlayerNetWork&) = delete;
			PlayerNetWork(PlayerNetWork&&) = delete;
			PlayerNetWork& operator=(const PlayerNetWork&) = delete;
			PlayerNetWork& operator=(PlayerNetWork&&) = delete;

			virtual ~PlayerNetWork(void) noexcept {}
		public:
			auto& SetLastServerDataBuffer(void) noexcept { return this->m_LastServerDataBuffer; }
			const auto& GetLastServerDataBuffer(void) const noexcept { return this->m_LastServerDataBuffer; }
			const auto& GetMyLocalPlayerID(void) const noexcept { return this->m_LocalData.GetID(); }
			const auto& GetLocalPlayerData(void) const noexcept { return this->m_LocalData; }
			const auto& GetTickUpdateFlag(void) const noexcept { return this->m_TickUpdateFlag; }
			const auto& GetPingTime(void) const noexcept { return this->m_PingTime; }
		public:
			void			SetLocalPlayerID(PlayerID ID) noexcept { this->m_LocalData.SetID(ID); }
		public:
			PlayerNetData 	GetLerpServerPlayerData(PlayerID ID) const noexcept {
				return PlayerNetData::GetLerpData(
					this->m_PrevServerData.m_PlayerData[static_cast<size_t>(ID)], this->m_LastServerData.m_PlayerData[static_cast<size_t>(ID)],
					std::clamp(static_cast<float>(this->m_LeapFrame) / static_cast<float>(this->m_LeapFrameMax), 0.f, 1.f));
			}
		public:
			void			Int(NetTime Tick) noexcept {
				this->m_LastServerFrame = 0;
				this->m_LeapFrame = 0;
				this->m_LeapFrameMax = 20;
				this->m_LastServerData.m_ServerFrame = 0;
				this->m_PrevServerData.m_ServerFrame = 0;
				this->m_TickCnt = 0;
				this->m_TickRate = Tick;
				LONGLONG NowFrame = GetNowHiPerformanceCount();
				this->m_PrevFrame = NowFrame;
				this->m_LastServerDataBuffer.Init();
			}
			void			UpdateLocalData(const PlayerSendData& pdata, bool TimerStart) noexcept {
				//更新間隔
				if (!TimerStart) {
					this->m_ClientTime = 0;
					this->m_PrevTime = GetNowHiPerformanceCount();
				}
				else {
					LONGLONG NowTime = GetNowHiPerformanceCount();
					this->m_ClientTime += NowTime - this->m_PrevTime;
					this->m_PrevTime = NowTime;
				}
				this->m_LocalData.SetData(pdata, this->m_ClientTime);
			}
			void			UpdateTick(void) noexcept {
				LONGLONG NowFrame = GetNowHiPerformanceCount();
				this->m_TickCnt += NowFrame - this->m_PrevFrame;
				this->m_PrevFrame = NowFrame;
				if (this->m_TickCnt > this->m_TickRate) {
					this->m_TickCnt -= this->m_TickRate;
					this->m_TickUpdateFlag = true;
					return;
				}
				this->m_TickUpdateFlag = false;
			}
			void			Update(void) noexcept {
				if (this->m_TickUpdateFlag) {
					this->m_PingTime = MAXLONGLONG;
					if (this->m_LastServerFrame < this->m_LastServerDataBuffer.m_ServerFrame && this->m_LastServerDataBuffer.m_ServerFrame <= (this->m_LastServerFrame + 60)) {//入力されたデータが以前もらったものから0~60F後だったら
						this->m_LastServerFrame = this->m_LastServerDataBuffer.m_ServerFrame;
						this->m_LeapFrame = 0;
						this->m_PrevServerData = this->m_LastServerData;
						this->m_LastServerData = this->m_LastServerDataBuffer;
						auto Total = static_cast<int>(this->m_LastServerData.m_ServerFrame) - static_cast<int>(this->m_PrevServerData.m_ServerFrame);
						if (Total <= 0) { Total = 20; }
						this->m_LeapFrameMax = static_cast<size_t>(Total);
						this->m_PingTime = this->m_LocalData.GetClientTime() - this->m_LastServerData.m_PlayerData[this->m_LocalData.GetID()].GetClientTime();//もらったデータと自分のフレームとの差異がpingになる
					}
				}
				else {
					this->m_LeapFrame = std::min<size_t>(this->m_LeapFrame + 1, this->m_LeapFrameMax);
					this->m_PingTime = 0;
				}
			}
		};
		//
		enum class ClientPhase : int8_t {
			Error_CannotConnect = -1,
			Error_ServerInGame = -2,

			NotConnect = 0,

			Client_WaitConnect,
			Client_GotPlayerID,

			Server_WaitConnect,

			Ready,
		};
		struct UDPS {
			UDPNetWorkDX			m_NetWork;
			ClientPhase				m_Phase{};
		public:
			bool IsReady(void) const noexcept { return this->m_Phase == ClientPhase::Ready; }
			void SendError(ClientPhase ErrorMessage) noexcept {
				this->Dispose();
				this->m_Phase = ErrorMessage;
			}
		public:
			void Init(bool IsServer, int PORT) noexcept {
				this->m_NetWork.Init(IsServer, PORT);
			}
			void Dispose(void) noexcept {
				this->m_NetWork.Dispose();
			}
		};
		//サーバー専用
		class ServerControl {
			const PlayerID							m_ServerPlayerID = 0;
		private:
			std::array<PlayerNetData, Player_num>	m_LastPlayerData;
			std::array<UDPS, Player_num>			m_PlayerUDPPhase;
			bool									m_IsServerPlay{ false };//サーバーもプレイヤーとして参戦するか
		public:
			ServerControl(void) noexcept {}
			ServerControl(const ServerControl&) = delete;
			ServerControl(ServerControl&&) = delete;
			ServerControl& operator=(const ServerControl&) = delete;
			ServerControl& operator=(ServerControl&&) = delete;

			virtual ~ServerControl(void) noexcept {}
		public:
			const bool		GetIsServerPlayer(PlayerID ID) const noexcept { return this->m_IsServerPlay && (ID == this->m_ServerPlayerID); }
		public:
			void			Init(int pPort, bool IsServerPlay) noexcept {
				this->m_IsServerPlay = IsServerPlay;
				int PortOffset = 0;
				for (int loop = 0; loop < Player_num; ++loop) {
					if (GetIsServerPlayer(static_cast<PlayerID>(loop))) { continue; }
					this->m_PlayerUDPPhase[loop].Init(true, pPort + PortOffset); ++PortOffset;
					this->m_PlayerUDPPhase[loop].m_Phase = ClientPhase::Server_WaitConnect;
				}
			}
			void			Update(PlayerNetWork* pPlayerNetwork) noexcept;
			void			Dispose(void) noexcept {
				for (int loop = 0; loop < Player_num; ++loop) {
					this->m_PlayerUDPPhase[loop].Dispose();
				}
			}
		};
		//クライアント専用
		class ClientControl {
			float					m_CannotConnectTimer{ 0.f };
			int						m_Port{ 0 };
			IPDATA					m_IP{ 127,0,0,1 };
			int						m_NetWorkSelect{ 0 };
			UDPS					m_PlayerUDPPhase;
		public:
			ClientControl(void) noexcept {}
			ClientControl(const ClientControl&) = delete;
			ClientControl(ClientControl&&) = delete;
			ClientControl& operator=(const ClientControl&) = delete;
			ClientControl& operator=(ClientControl&&) = delete;

			virtual ~ClientControl(void) noexcept {}
		public:
			void			Init(int pPort, const IPDATA& pIP) noexcept {
				this->m_Port = pPort;
				this->m_IP = pIP;
				this->m_CannotConnectTimer = 0.f;
				this->m_NetWorkSelect = 0;
				this->m_PlayerUDPPhase.m_NetWork.SetServerIP(this->m_IP);
				this->m_PlayerUDPPhase.Init(false, this->m_Port + this->m_NetWorkSelect);
				this->m_PlayerUDPPhase.m_Phase = ClientPhase::Client_WaitConnect;
			}
			void			Update(PlayerNetWork* pPlayerNetwork) noexcept;
			void			Dispose(void) noexcept {
				this->m_PlayerUDPPhase.Dispose();
			}
		};


		//通信
		class NetWorkController {
			static const int		m_PingTotal{ 60 };
			float					m_Tick{ 15.f };//ティックレート　プレイヤーのFPSより低いと詰まる

			bool					m_IsClient{ true };
			bool					m_IsServerPlayer{ true };
			PlayerSendData			m_LocalData;
			PlayerNetWork			m_PlayerNet;
			ServerControl			m_ServerCtrl;			//サーバー専用
			ClientControl			m_ClientCtrl;			//クライアント専用
			std::array<float, m_PingTotal>	m_Pings{};
			float					m_Ping{ 0 };
			size_t					m_PingNow{ 0 };
		public:
			NetWorkController(bool IsClient, int Port, const IPDATA& ip, bool IsServerPlayer) noexcept {
				Init(IsClient, Port, ip, IsServerPlayer);
			}
			NetWorkController(const NetWorkController&) = delete;
			NetWorkController(NetWorkController&&) = delete;
			NetWorkController& operator=(const NetWorkController&) = delete;
			NetWorkController& operator=(NetWorkController&&) = delete;

			virtual ~NetWorkController(void) noexcept { Dispose(); }
		private:
			void			CalcPing(LONGLONG microsec) noexcept {
				if (microsec == InvalidID) {
					this->m_Ping = -1.f;
					return;
				}
				this->m_Pings[this->m_PingNow] = std::max(0.f, static_cast<float>(microsec) / 1000.f - (1000.f / this->m_Tick));//ティック分引く
				++this->m_PingNow %= this->m_PingTotal;
				this->m_Ping = 0.f;
				for (auto& ping : this->m_Pings) {
					this->m_Ping += ping;
				}
				this->m_Ping /= static_cast<float>(this->m_PingTotal);
			}
		public:
			auto			IsInGame(void) const noexcept { return this->m_PlayerNet.GetLastServerDataBuffer().m_ServerPhase == ServerPhase::Ingame; }
			PlayerNetData 	GetServerPlayerData(PlayerID ID) noexcept { return this->m_PlayerNet.GetLerpServerPlayerData(ID); }
			const auto& GetClient(void) const noexcept { return this->m_IsClient; }
			const auto& GetServerPlayer(void) const noexcept { return this->m_IsServerPlayer; }
			const auto& GetPing(void) const noexcept { return this->m_Ping; }
			const auto& GetMyLocalPlayerID(void) const noexcept { return this->m_PlayerNet.GetMyLocalPlayerID(); }

			const auto& GetPlayerData(void) const noexcept { return this->m_PlayerNet; }

			auto& SetLocalData(void) noexcept { return this->m_LocalData; }
		private:
			void Init(bool IsClient, int Port, const IPDATA& ip, bool IsServerPlayer) noexcept {
				this->m_PlayerNet.Int(static_cast<NetTime>((1000.f / this->m_Tick) * 1000.f));
				this->m_IsClient = IsClient;
				this->m_IsServerPlayer = IsServerPlayer;
				if (this->m_IsClient) {
					this->m_ClientCtrl.Init(Port, ip);
				}
				else {
					this->m_ServerCtrl.Init(Port, this->m_IsServerPlayer);
				}
			}
			void Dispose(void) noexcept {
				this->m_ServerCtrl.Dispose();
				this->m_ClientCtrl.Dispose();
			}
		public:
			void Update(void) noexcept;
		};
	}
}
