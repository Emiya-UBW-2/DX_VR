#pragma once
#pragma warning(disable:4464)
#include "../Header.hpp"
#include "../sub.hpp"


namespace FPS_n2 {
	namespace Sceneclass {
		static const int		Player_num = 2;

		using NetTime = LONGLONG;
		//�N���C�A���g�����M����f�[�^ ���[�J����
		class PlayerSendData {
		private:
			PlayerID					m_ID{ 0 };
			InputControl				m_Input;
			moves						m_move;
			DamageEventControl			m_DamageEvent;
			NetTime						m_ClientTime{ 0 };	//���̃v���C���[�̃C���Q�[������
			NetTime						m_PrevTime{ 0 };
		public:
			const auto&		GetID(void) const noexcept { return this->m_ID; }
			const auto&		GetInput(void) const noexcept { return this->m_Input; }
			const auto&		GetMove(void) const noexcept { return this->m_move; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto&		GetClientTime(void) const noexcept { return this->m_ClientTime; }
		public:
			void			SetID(PlayerID value) noexcept { m_ID = value; }
			void			SetMove(const moves& value) noexcept { m_move = value; }
		public:
			void			SetMyPlayer(const InputControl& pInput, const moves& move_t, const DamageEventControl& Damage_t) noexcept {
				this->m_Input = pInput;
				this->m_move = move_t;
				this->m_DamageEvent = Damage_t;
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
			PlayerID					m_ID{ 0 };
			uint8_t						m_Attribute{ 0 };	//�t���O
		private:
			int			CalcCheckSum(void) const noexcept {
				return (
					static_cast<int>(m_ID) +
					static_cast<int>(m_Attribute) +
					static_cast<int>(m_ClientTime) +
					(static_cast<int>(this->m_move.GetPos().x * 100.f) + static_cast<int>(this->m_move.GetPos().y * 100.f) + static_cast<int>(this->m_move.GetPos().z * 100.f)) +
					(static_cast<int>(this->m_move.GetVec().x * 100.f) + static_cast<int>(this->m_move.GetVec().y * 100.f) + static_cast<int>(this->m_move.GetVec().z * 100.f))
					);
			}
		public:
			auto			IsCheckSum(void) const noexcept { return m_CheckSum == (size_t)this->CalcCheckSum(); }
			auto		 	GetFlag(NetAttribute flag) const noexcept { return this->m_Attribute & (1 << static_cast<int>(flag)); }
			const auto&		GetClientTime(void) const noexcept { return this->m_ClientTime; }
			const auto&		GetID(void) const noexcept { return this->m_ID; }
			const auto&		GetInput(void) const noexcept { return this->m_Input; }
			const auto&		GetMove(void) const noexcept { return this->m_move; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
		public:
			void			SetFlag(NetAttribute flag, bool value) noexcept {
				if (value) {
					this->m_Attribute |= (1 << static_cast<int>(flag));
				}
				else {
					this->m_Attribute &= ~(1 << static_cast<int>(flag));
				}
			}
			void			SetID(PlayerID value) noexcept { m_ID = value; }
			void			SetMove(const moves& value) noexcept { m_move = value; }
		public:
			void			SetData(const PlayerSendData& o) noexcept {
				this->m_Input = o.GetInput();
				this->m_move = o.GetMove();
				this->m_DamageEvent = o.GetDamageEvent();
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
			size_t				m_CheckSum{};			//�`�F�b�N�T��
			size_t				ServerFrame{ 0 };			//�T�[�o�[�t���[��
			PlayerNetData		PlayerData[Player_num];
			PlayerID			m_PlayerID{ 0 };
		private:
			int				CalcCheckSum(void) const noexcept {
				int Players = 0;
				for (int i = 0; i < Player_num; i++) {
					Players += (PlayerData[i].IsCheckSum() ? 100 : 0);
				}
				return (
					static_cast<int>(m_PlayerID) +
					static_cast<int>(ServerFrame) +
					Players
					);
			}
		public:
			auto			IsCheckSum(void) const noexcept { return m_CheckSum == (size_t)this->CalcCheckSum(); }
		public:
			void			SetInGame(void) noexcept { m_PlayerID = -1; }
			auto			IsInGame(void) const noexcept { return m_PlayerID == -1; }//�C���Q�[�����ł�
		};
		//�ʐM
		class PlayerNetWork {
		private:
			size_t											m_LastServerFrame{ 0 };//�Ō�ɃT�[�o�[�����M�����T�[�o�[�t���[��
			size_t											m_LeapFrame{};
			size_t											m_LeapFrameMax{};
			ServerNetData									m_ServerDataCommon;	//�ŐV�̎�M�f�[�^
			ServerNetData									m_PrevServerData;	//�O��̎�M�f�[�^
			PlayerNetData									m_LocalData;		//���[�J���̃v���C���[�f�[�^
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
			const auto&		GetMyLocalPlayerID(void) const noexcept { return this->m_LocalData.GetID(); }
			const auto&		GetLocalPlayerData(void) const noexcept { return this->m_LocalData; }
		public:
			void			SetLocalPlayerID(PlayerID ID) noexcept { this->m_LocalData.SetID(ID); }
			void			SetLocalPlayerFlag(NetAttribute flag, bool value) noexcept { this->m_LocalData.SetFlag(flag, value); }
			void			SetLocalData(const PlayerSendData& pdata) noexcept { this->m_LocalData.SetData(pdata); }
		public:
			PlayerNetData 	GetLerpServerPlayerData(PlayerID pPlayerID) const noexcept;
		public:
		public:
			void			Int(NetTime Tick) noexcept;
			bool			UpdateTick(void) noexcept;
			NetTime			Update(const ServerNetData& UpdateData) noexcept;
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
			bool IsReady() const noexcept { return m_Phase == ClientPhase::Ready; }
		};
		//�T�[�o�[��p
		class ServerControl {
			ServerNetData							m_ServerData;
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
			bool AllReady() const noexcept;
		public:
			const auto&		GetServerData(void) const noexcept { return this->m_ServerData; }
			const auto&		GetIsServerPlayer(void) const noexcept { return this->m_IsServerPlay; }
		public:
			void			Init(int pPort,bool IsServerPlay) noexcept;
			bool			Execute(const PlayerNetData& MyLocalPlayerData, bool IsUpdateTick) noexcept;
			void			Dispose(void) noexcept;
		};
		//�N���C�A���g��p
		class ClientControl {
			float					m_CannotConnectTimer{0.f};
			int						m_Port{ 0 };
			IPDATA					m_IP{127,0,0,1};
			int						m_NetWorkSel{ 0 };
			UDPS					m_Net;

			PlayerID				m_MyID{};
			ServerNetData			m_ServerData;
			ServerNetData			m_BufferDataOnce;
			ServerNetData			m_BufferData[10];
		public:
			ClientControl(void) noexcept {}
			ClientControl(const ClientControl&) = delete;
			ClientControl(ClientControl&& o) = delete;
			ClientControl& operator=(const ClientControl&) = delete;
			ClientControl& operator=(ClientControl&& o) = delete;

			~ClientControl(void) noexcept {}
		private:
			auto&	SetServerData(void) noexcept { return this->m_ServerData; }
		public:
			const auto&		GetServerData(void) const noexcept { return this->m_ServerData; }
			const auto&		GetMyID(void) const noexcept { return this->m_MyID; }
			auto			CanGetMyID(void) const noexcept { return (this->m_Net.m_Phase == ClientPhase::GetNumber) || (this->m_Net.m_Phase == ClientPhase::Ready); }
		public:
			void			Init(int pPort, const IPDATA& pIP) noexcept;
			bool			Execute(const PlayerNetData& MyLocalPlayerData, bool IsUpdateTick) noexcept;
			void			Dispose(void) noexcept;
		};
	};
};
