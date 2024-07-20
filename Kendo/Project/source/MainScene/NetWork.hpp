#pragma once
#pragma warning(disable:4464)
#include "../Header.hpp"
#include "../sub.hpp"

class UDPDX {
	int			m_Handle{-1};				// �l�b�g���[�N�n���h��
	IPDATA		m_SendIP{127,0,0,1};		// ���M�p�h�o�A�h���X�f�[�^
	int			m_SendPort{-1};				// �ʐM�p�|�[�g
	IPDATA		RecvIp{127,0,0,1};			// ��M�p�h�o�A�h���X�f�[�^
	int			RecvPort{0};				// ��M�p�|�[�g
public:
	auto			IsActive(void) const noexcept { return (m_Handle != -1); }
public:
	void			SetServerIP(const IPDATA& pIP) noexcept { m_SendIP = pIP; }//�N���C�A���g�͕K���s��
	void			Init(bool IsServer, int PORT = -1) noexcept {
		if (!IsActive()) {
			m_SendPort = PORT;
			m_Handle = MakeUDPSocket(IsServer ? m_SendPort : -1);
		}
	}
	void			Dispose(void) noexcept {
		if (IsActive()) {
			DeleteUDPSocket(m_Handle);	// �t�c�o�\�P�b�g�n���h���̍폜
			m_Handle = -1;
			m_SendPort = -1;
		}
	}
private:
	template<class T>
	void			SendData(IPDATA& Ip, int SendPort, const T& Data) noexcept {
		if (IsActive()) {
			NetWorkSendUDP(m_Handle, Ip, SendPort, &Data, sizeof(T));
			//printfDx("���M:[%d,%d,%d,%d][%d]\n", Ip.d1, Ip.d2, Ip.d3, Ip.d4, SendPort);
		}
	}
public:
	//���M
	template<class T>
	void			SendData(const T& Data) noexcept { SendData(m_SendIP, m_SendPort, Data); }
	//��M
	template<class T>
	bool			RecvData(T* Data, int* RecvReturn,bool IsPeek) noexcept {
		*RecvReturn = -1;
		switch (CheckNetWorkRecvUDP(m_Handle)) {
			case TRUE:
				*RecvReturn = NetWorkRecvUDP(m_Handle, &RecvIp, &RecvPort, Data, sizeof(T), IsPeek ? TRUE :  FALSE);		// ��M
				return true;
			case FALSE://�ҋ@
				break;
			default://error
				break;
		}
		return false;
	}
	//�ԑ�
	template<class T>
	void			ReturnData(const T& Data) noexcept { SendData(RecvIp, RecvPort, Data); }
};

namespace FPS_n2 {
	namespace Sceneclass {
		static const int		Player_num = 2;

		using NetTime = LONGLONG;
		//�N���C�A���g�����M����f�[�^ ���[�J����
		class PlayerSendData {
		private:
			size_t						m_CheckSum{};			//�`�F�b�N�T��
			PlayerID					m_ID{0};
			InputControl				m_Input;
			moves						m_move;
			std::array<DamageEvent,64>	m_DamageEvents;
			NetTime						m_ClientTime{ 0 };	//���̃v���C���[�̃C���Q�[������
			NetTime						m_PrevTime{ 0 };
		public:
			const auto&		GetID(void) const noexcept { return this->m_ID; }
			const auto&		GetInput(void) const noexcept { return this->m_Input; }
			const auto&		GetMove(void) const noexcept { return this->m_move; }
			const auto&		GetDamageEvents(void) const noexcept { return this->m_DamageEvents; }
			const auto&		GetClientTime(void) const noexcept { return this->m_ClientTime; }
		public:
			void			SetID(PlayerID value) noexcept { m_ID = value; }
			void			SetMove(const moves& value) noexcept { m_move = value; }
		public:
			void			SetMyPlayer(const InputControl& pInput, const moves& move_t, const std::array<DamageEvent, 64>& Damage_t) noexcept {
				this->m_Input = pInput;
				this->m_move = move_t;
				this->m_DamageEvents = Damage_t;
			}
		public:
			void			InitTime(void) noexcept {
				this->m_ClientTime = 0;
				this->m_PrevTime = GetNowHiPerformanceCount();
			}
			void			UpdateTime(void) noexcept {
				LONGLONG NowTime = GetNowHiPerformanceCount();
				this->m_ClientTime += NowTime - this->m_PrevTime;
				this->m_PrevTime = NowTime;
			}
		};
		//�N���C�A���g�����M����f�[�^�@�ʐM��
		class PlayerNetData {
		private:
			size_t						m_CheckSum{};			//�`�F�b�N�T��
			bool						m_IsActive{ false };
			NetTime						m_ClientTime{ 0 };		//���̃v���C���[�̃C���Q�[������
			PlayerID					m_ID{ 0 };
			InputControl				m_Input;
			moves						m_move;
			std::array<DamageEvent, 64>	m_DamageEvents;
		private:
			int			CalcCheckSum(void) const noexcept {
				return (
					(static_cast<int>(this->m_move.pos.x * 100.f) + static_cast<int>(this->m_move.pos.y * 100.f) + static_cast<int>(this->m_move.pos.z * 100.f)) +
					(static_cast<int>(this->m_move.vec.x * 100.f) + static_cast<int>(this->m_move.vec.y * 100.f) + static_cast<int>(this->m_move.vec.z * 100.f)) +
					static_cast<int>(rad2deg(this->m_move.rad.y)) +
					static_cast<int>(m_ID) +
					static_cast<int>(m_ClientTime)
					);
			}
		public:
			auto			IsCheckSum(void) const noexcept { return m_CheckSum == (size_t)this->CalcCheckSum(); }
			const auto& 	GetIsActive(void) const noexcept { return this->m_IsActive; }
			const auto&		GetClientTime(void) const noexcept { return this->m_ClientTime; }
			const auto&		GetID(void) const noexcept { return this->m_ID; }
			const auto&		GetInput(void) const noexcept { return this->m_Input; }
			const auto&		GetMove(void) const noexcept { return this->m_move; }
			//const auto&		GetDamageEvents(void) const noexcept { return this->m_DamageEvents; }
		public:
			void			SetActive(bool value) noexcept { m_IsActive = value; }
			void			SetID(PlayerID value) noexcept { m_ID = value; }
			void			SetMove(const moves& value) noexcept { m_move = value; }
		public:
			void			SetData(const PlayerSendData& o) noexcept {
				this->m_Input = o.GetInput();
				this->m_move = o.GetMove();
				this->m_DamageEvents = o.GetDamageEvents();
				this->m_ClientTime = o.GetClientTime();
				this->m_CheckSum = (size_t)this->CalcCheckSum();
			}
		public:
			static PlayerNetData	GetLerpData(const PlayerNetData& PrevData, const PlayerNetData& ServerData, float Per) noexcept;
		};
		//�T�[�o�[���Ǘ�����f�[�^
		class ServerNetData {
		private:
		public:
			int					m_PlayerID{0};			//
			size_t				ServerFrame{0};			//�T�[�o�[�t���[��
			PlayerNetData		PlayerData[Player_num];	//
		public:
			void			SetInGame(void) noexcept { m_PlayerID = -1; }
			auto			IsInGame(void) const noexcept { return m_PlayerID == -1; }//�C���Q�[�����ł�
		};
		//�ʐM
		class PlayerNetWork {
		private:
			size_t											m_LastServerFrame{0};//�Ō�ɃT�[�o�[�����M�����T�[�o�[�t���[��
			size_t											m_LeapFrame{};
			size_t											m_LeapFrameMax{};
			ServerNetData									m_ServerDataCommon;	//�ŐV�̎�M�f�[�^
			ServerNetData									m_PrevServerData;	//�O��̎�M�f�[�^
			PlayerNetData									m_PlayerData;		//���[�J���̃v���C���[�f�[�^
			NetTime											m_TickCnt{ 0 };
			NetTime											m_TickRate{ 0 };
			NetTime											m_PrevFrame{ 0 };
		public:
			PlayerNetWork(void) noexcept {}
			PlayerNetWork(const PlayerNetWork&) = delete;
			PlayerNetWork(PlayerNetWork&& o) = delete;
			PlayerNetWork& operator=(const PlayerNetWork&) = delete;
			PlayerNetWork& operator=(PlayerNetWork&& o) = delete;

			~PlayerNetWork(void) noexcept {}
		public:
			const auto&		GetServerDataCommon(void) const noexcept { return this->m_ServerDataCommon; }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_PlayerData.GetID(); }
		public:
			void			SetPlayerID(PlayerID ID) noexcept { this->m_PlayerData.SetID(ID); }
			void			SetData(const PlayerSendData& pdata) noexcept { this->m_PlayerData.SetData(pdata); }
			auto&			SetPlayerData(void) noexcept { return this->m_PlayerData; }
		public:
			PlayerNetData 	GetLerpServerPlayerData(PlayerID pPlayerID) const noexcept;
		public:
		public:
			void			Int(NetTime Tick) noexcept;
			bool			UpdateTick(void) noexcept;
			NetTime			Update(const ServerNetData& UpdateData) noexcept;
		};
		//
		enum class ClientPhase {
			Error_CannotConnect = -1,
			Error_ServerInGame = -2,
			NotConnect = 0,
			WaitConnect,
			GetNumber,
			Ready,
		};
		struct UDPS {
			UDPDX					m_NetWork;
			ClientPhase				m_Phase{};
		public:
			bool IsReady() const noexcept { return m_Phase == ClientPhase::Ready; }
		};
		//�T�[�o�[��p
		class ServerControl {
			ServerNetData			m_ServerData;
			std::array<UDPS, Player_num - 1>			m_Net;
		public:
			ServerControl(void) noexcept {}
			ServerControl(const ServerControl&) = delete;
			ServerControl(ServerControl&& o) = delete;
			ServerControl& operator=(const ServerControl&) = delete;
			ServerControl& operator=(ServerControl&& o) = delete;

			~ServerControl(void) noexcept {}
		private:
			bool AllReady() const noexcept;
		public:
			const auto&		GetServerData(void) const noexcept { return this->m_ServerData; }
		public:
			void			Init(int pPort) noexcept;
			bool			Execute(PlayerNetData* MyPlayerData, bool IsUpdateTick) noexcept;
			void			Dispose(void) noexcept;
		};
		//�N���C�A���g��p
		class ClientControl {
			float					m_CannotConnectTimer{0.f};
			int						m_Port{0};
			IPDATA					m_IP{127,0,0,1};
			int						m_NetWorkSel{ 0 };
			UDPS					m_Net;

			PlayerID				m_MyID{};
			bool					m_IsServerDataUpdated{};
			ServerNetData			m_ServerData;
			ServerNetData			m_BufferData[30];
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
			auto			CanGetMyID(void) const noexcept { return (this->m_Net.m_Phase == ClientPhase::GetNumber) || (this->m_Net.m_Phase == ClientPhase::Ready); }
		public:
			void			Init(int pPort, const IPDATA& pIP) noexcept;
			bool			Execute(PlayerNetData* MyPlayerData, bool IsUpdateTick) noexcept;
			void			Dispose(void) noexcept;
		};
	};
};
