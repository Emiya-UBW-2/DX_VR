#pragma once
#pragma warning(disable:4464)
#include "../Header.hpp"
#include "../sub.hpp"


namespace FPS_n2 {
	namespace NetWork {
		static const int		Player_num = 10;

		using NetTime = LONGLONG;
		//�N���C�A���g�����M����f�[�^ ���[�J����
		class PlayerSendData {
		private:
			InputControl				m_Input;
			moves						m_move;
			DamageEventControl			m_DamageEvent;
			int32_t						m_FreeData[10]{};
		public:
			const auto& GetInput(void) const noexcept { return this->m_Input; }
			const auto& GetMove(void) const noexcept { return this->m_move; }
			const auto& GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto& GetFreeData(void) const noexcept { return this->m_FreeData; }
		public:
			void			SetMyPlayer(const InputControl& pInput, const moves& move_t, const DamageEventControl& Damage_t, int32_t* pFreeData) noexcept {
				this->m_Input = pInput;
				this->m_move = move_t;
				this->m_DamageEvent = Damage_t;
				for (int i = 0; i < 10; ++i) {
					this->m_FreeData[i] = pFreeData[i];
				}
			}
		};
		enum class NetAttribute : uint8_t {
			IsActive,
		};
		//�N���C�A���g�����M����f�[�^�@�ʐM��
		class PlayerNetData {
		private:
			size_t						m_CheckSum{};			//�`�F�b�N�T��
			NetTime						m_ClientTime{ 0 };		//���̃v���C���[�̃C���Q�[������
			InputControl				m_Input;
			moves						m_move;
			DamageEventControl			m_DamageEvent;
			int32_t						m_FreeData[10]{};
			PlayerID					m_ID{ 0 };
			uint8_t						m_Attribute{ 0 };	//�t���O
		private:
			int			CalcCheckSum(void) const noexcept {
				return (
					500 +
					static_cast<int>(this->m_ID) +
					static_cast<int>(this->m_Attribute) +
					static_cast<int>(this->m_ClientTime) +
					(static_cast<int>(this->m_move.GetPos().x * 100.f) + static_cast<int>(this->m_move.GetPos().y * 100.f) + static_cast<int>(this->m_move.GetPos().z * 100.f)) +
					(static_cast<int>(this->m_move.GetVec().x * 100.f) + static_cast<int>(this->m_move.GetVec().y * 100.f) + static_cast<int>(this->m_move.GetVec().z * 100.f))
					);
			}
		public:
			auto			IsCheckSum(void) const noexcept { return this->m_CheckSum == (size_t)this->CalcCheckSum(); }
			auto		 	GetFlag(NetAttribute flag) const noexcept { return (this->m_Attribute & (1 << static_cast<int>(flag))) != 0; }
			const auto& GetClientTime(void) const noexcept { return this->m_ClientTime; }
			const auto& GetID(void) const noexcept { return this->m_ID; }
			const auto& GetInput(void) const noexcept { return this->m_Input; }
			moves			GetMove(void) const noexcept { return this->m_move; }
			const auto& GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto& GetFreeData(void) const noexcept { return this->m_FreeData; }
		public:
			void			SetFlag(NetAttribute flag, bool value) noexcept {
				if (value) {
					this->m_Attribute |= (1 << static_cast<int>(flag));
				}
				else {
					this->m_Attribute &= ~(1 << static_cast<int>(flag));
				}
			}
			void			SetID(PlayerID value) noexcept { this->m_ID = value; }
			void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept { this->m_DamageEvent.AddDamageEvent(pRet); }
		public:
			void			SetData(const PlayerSendData& o, NetTime ClientTime) noexcept {
				this->m_Input = o.GetInput();
				this->m_move = o.GetMove();
				this->m_DamageEvent = o.GetDamageEvent();
				this->m_ClientTime = ClientTime;
				this->m_CheckSum = (size_t)this->CalcCheckSum();
				for (int i = 0; i < 10; ++i) {
					this->m_FreeData[i] = o.GetFreeData()[i];
				}
			}
		public:
			static PlayerNetData	GetLerpData(const PlayerNetData& PrevData, const PlayerNetData& ServerData, float Per) noexcept;
		};
		//�T�[�o�[���Ǘ�����f�[�^
		class ServerNetData {
		private:
		public:
			size_t				m_CheckSum{};			//�`�F�b�N�T��
			size_t				ServerFrame{ 0 };			//�T�[�o�[�t���[��
			PlayerNetData		PlayerData[Player_num];
			PlayerID			m_PlayerID{ 0 };
		private:
			int				CalcCheckSum(void) const noexcept {
				int Players = 0;
				for (int i = 0; i < Player_num; ++i) {
					Players += (PlayerData[i].IsCheckSum() ? 100 : 0);
				}
				return (
					500 +
					static_cast<int>(this->m_PlayerID) +
					static_cast<int>(ServerFrame) +
					Players
					);
			}
		public:
			auto			IsCheckSum(void) const noexcept { return this->m_CheckSum == (size_t)this->CalcCheckSum(); }
		public:
			void			CalcCheckSun(void) noexcept { this->m_CheckSum = (size_t)this->CalcCheckSum(); }
			void			SetInGame(void) noexcept { this->m_PlayerID = -1; }
			auto			IsInGame(void) const noexcept { return this->m_PlayerID == -1; }//�C���Q�[�����ł�
		};
		//�ʐM
		class PlayerNetWork {
		private:
			size_t											m_LastServerFrame{ 0 };//�Ō�ɃT�[�o�[�����M�����T�[�o�[�t���[��
			size_t											m_LeapFrame{};
			size_t											m_LeapFrameMax{};
			ServerNetData									m_GetServerData;	//���ꂼ��̃R���g���[���[�����������f�[�^
			ServerNetData									m_ServerDataCommon;	//�ŐV�̎�M�f�[�^
			ServerNetData									m_PrevServerData;	//�O��̎�M�f�[�^
			PlayerNetData									m_LocalData;		//���[�J���̃v���C���[�f�[�^
			NetTime											m_TickCnt{ 0 };
			NetTime											m_TickRate{ 0 };
			NetTime											m_PrevFrame{ 0 };

			NetTime						m_ClientTime{ 0 };	//���̃v���C���[�̃C���Q�[������
			NetTime						m_PrevTime{ 0 };
		public:
			PlayerNetWork(void) noexcept {}
			PlayerNetWork(const PlayerNetWork&) = delete;
			PlayerNetWork(PlayerNetWork&& o) = delete;
			PlayerNetWork& operator=(const PlayerNetWork&) = delete;
			PlayerNetWork& operator=(PlayerNetWork&& o) = delete;

			~PlayerNetWork(void) noexcept {}
		public:
			auto& SetGetServerData(void) noexcept { return this->m_GetServerData; }
			const auto& GetServerDataCommon(void) const noexcept { return this->m_ServerDataCommon; }
			const auto& GetMyLocalPlayerID(void) const noexcept { return this->m_LocalData.GetID(); }
			const auto& GetLocalPlayerData(void) const noexcept { return this->m_LocalData; }
		public:
			void			SetLocalPlayerID(PlayerID ID) noexcept { this->m_LocalData.SetID(ID); }
			void			SetLocalPlayerFlag(NetAttribute flag, bool value) noexcept { this->m_LocalData.SetFlag(flag, value); }
			void			UpdateLocalData(const PlayerSendData& pdata, bool TimerStart) noexcept {
				//�X�V�Ԋu
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
		public:
			PlayerNetData 	GetLerpServerPlayerData(PlayerID pPlayerID) const noexcept;
		public:
			void			Int(NetTime Tick) noexcept;
			bool			UpdateTick(void) noexcept;
			NetTime			Update(bool IsUpdateTick) noexcept;
		};
		//
		enum class ClientPhase : int8_t {
			Error_CannotConnect = -1,
			Error_ServerInGame = -2,
			NotConnect = 0,
			WaitConnect,
			GetNumber,
			Ready,
		};
		struct UDPS {
			UDPNetWorkDX			m_NetWork;
			ClientPhase				m_Phase{};
		public:
			bool IsReady(void) const noexcept { return this->m_Phase == ClientPhase::Ready; }
		};
		//�T�[�o�[��p
		class ServerControl {
			std::array<PlayerNetData, Player_num>	m_LastPlayerData;
			std::array<UDPS, Player_num>			m_Net;
			bool									m_IsServerPlay{ false };//�T�[�o�[���v���C���[�Ƃ��ĎQ�킷�邩
		public:
			ServerControl(void) noexcept {}
			ServerControl(const ServerControl&) = delete;
			ServerControl(ServerControl&& o) = delete;
			ServerControl& operator=(const ServerControl&) = delete;
			ServerControl& operator=(ServerControl&& o) = delete;

			~ServerControl(void) noexcept {}
		private:
			bool AllReady(void) const noexcept;
		public:
			const auto& GetIsServerPlayer(void) const noexcept { return this->m_IsServerPlay; }
		public:
			void			Init(int pPort, bool IsServerPlay) noexcept;
			bool			Update(ServerNetData* pServerCtrl, const PlayerNetData& MyLocalPlayerData, bool IsUpdateTick) noexcept;
			void			Dispose(void) noexcept;
		};
		//�N���C�A���g��p
		class ClientControl {
			float					m_CannotConnectTimer{ 0.f };
			int						m_Port{ 0 };
			IPDATA					m_IP{ 127,0,0,1 };
			int						m_NetWorkSel{ 0 };
			UDPS					m_Net;

			PlayerID				m_MyID{};
			ServerNetData			m_BufferDataOnce;
		public:
			ClientControl(void) noexcept {}
			ClientControl(const ClientControl&) = delete;
			ClientControl(ClientControl&& o) = delete;
			ClientControl& operator=(const ClientControl&) = delete;
			ClientControl& operator=(ClientControl&& o) = delete;

			~ClientControl(void) noexcept {}
		public:
			const auto&		GetMyID(void) const noexcept { return this->m_MyID; }
			auto			CanGetMyID(void) const noexcept { return (this->m_Net.m_Phase == ClientPhase::GetNumber) || (this->m_Net.m_Phase == ClientPhase::Ready); }
		public:
			void			Init(int pPort, const IPDATA& pIP) noexcept;
			bool			Update(ServerNetData* pServerCtrl, const PlayerNetData& MyLocalPlayerData, bool IsUpdateTick) noexcept;
			void			Dispose(void) noexcept;
		};


		//�ʐM
		enum class NetWorkSequence : uint8_t {
			Matching,
			MainGame,
		};
		class NetWorkController {
			bool					m_IsClient{ true };
			bool					m_IsServerPlayer{ true };
			float					m_Tick{ 30.f };//�e�B�b�N���[�g�@�v���C���[��FPS���Ⴂ�Ƌl�܂�
			PlayerSendData			m_LocalData;
			PlayerNetWork			m_PlayerNet;
			ServerControl			m_ServerCtrl;			//�T�[�o�[��p
			ClientControl			m_ClientCtrl;			//�N���C�A���g��p
			std::array<float, 60>	m_Pings{};
			float					m_Ping{ 0 };
			size_t					m_PingNow{ 0 };
			NetWorkSequence			m_Sequence{ NetWorkSequence::Matching };
		public:
			NetWorkController(void) noexcept {}
			NetWorkController(const NetWorkController&) = delete;
			NetWorkController(NetWorkController&& o) = delete;
			NetWorkController& operator=(const NetWorkController&) = delete;
			NetWorkController& operator=(NetWorkController&& o) = delete;

			~NetWorkController(void) noexcept {}
		private:
			void			CalcPing(LONGLONG microsec) noexcept {
				if (microsec == -1) {
					this->m_Ping = -1.f;
					return;
				}
				this->m_Pings.at(this->m_PingNow) = std::max(0.f, static_cast<float>(microsec) / 1000.f - (1000.f / this->m_Tick));//�e�B�b�N������
				++m_PingNow %= this->m_Pings.size();
				this->m_Ping = 0.f;
				for (auto& p : this->m_Pings) {
					this->m_Ping += p;
				}
				this->m_Ping /= static_cast<float>(this->m_Pings.size());
			}
		public:
			auto			IsInGame(void) const noexcept { return this->m_Sequence == NetWorkSequence::MainGame; }
			PlayerNetData 	GetLerpServerPlayerData(PlayerID ID) noexcept { return this->m_PlayerNet.GetLerpServerPlayerData(ID); }
			const auto& GetClient(void) const noexcept { return this->m_IsClient; }
			const auto& GetServerPlayer(void) const noexcept { return this->m_IsServerPlayer; }
			const auto& GetPing(void) const noexcept { return this->m_Ping; }
			const auto& GetMyLocalPlayerID(void) const noexcept { return this->m_PlayerNet.GetMyLocalPlayerID(); }

			auto& SetLocalData(void) noexcept { return this->m_LocalData; }
		public:
			void Init(bool IsClient, int Port, const IPDATA& ip, bool IsServerPlayer) noexcept;
			void Update(void) noexcept;
			void Dispose(void) noexcept {
				this->m_ServerCtrl.Dispose();
				this->m_ClientCtrl.Dispose();
			}
		};
	}
}
