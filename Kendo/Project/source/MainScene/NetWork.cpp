#pragma warning(disable:4464)
#include "NetWork.hpp"

namespace FPS_n2 {
	namespace NetWork {
		//�N���C�A���g�����M����f�[�^
		PlayerNetData PlayerNetData::GetLerpData(const PlayerNetData& PrevData, const PlayerNetData& ServerData, float Per) noexcept {
			PlayerNetData tmp;
			//
			tmp.m_Input = Lerp(PrevData.m_Input, ServerData.m_Input, Per);
			tmp.m_Input.SetKeyInputFlags(ServerData.m_Input);
			tmp.m_move = PrevData.m_move.LerpMove(ServerData.m_move, Per);
			tmp.m_DamageEvent = ServerData.m_DamageEvent;
			//
			tmp.m_ID = ServerData.m_ID;
			tmp.m_Attribute = ServerData.m_Attribute;
			tmp.m_ClientTime = ServerData.m_ClientTime;
			for (int i = 0; i < 10; ++i) {
				tmp.m_FreeData[i] = ServerData.GetFreeData()[i];
			}
			return tmp;
		}
		//�ʐM
		PlayerNetData PlayerNetWork::GetLerpServerPlayerData(PlayerID pPlayerID) const noexcept {
			return PlayerNetData::GetLerpData(
				this->m_PrevServerData.PlayerData[static_cast<size_t>(pPlayerID)], this->m_ServerDataCommon.PlayerData[static_cast<size_t>(pPlayerID)],
				std::clamp(static_cast<float>(this->m_LeapFrame) / static_cast<float>(this->m_LeapFrameMax), 0.f, 1.f));
		}
		void PlayerNetWork::Int(NetTime Tick) noexcept {
			this->m_LastServerFrame = 0;
			this->m_LeapFrame = 0;
			this->m_LeapFrameMax = 20;
			this->m_ServerDataCommon.ServerFrame = 0;
			this->m_PrevServerData.ServerFrame = 0;
			this->m_TickCnt = 0;
			this->m_TickRate = Tick;
			LONGLONG NowFrame = GetNowHiPerformanceCount();
			this->m_PrevFrame = NowFrame;
		}
		bool PlayerNetWork::UpdateTick(void) noexcept {
			LONGLONG NowFrame = GetNowHiPerformanceCount();
			this->m_TickCnt += NowFrame - this->m_PrevFrame;
			this->m_PrevFrame = NowFrame;
			if (this->m_TickCnt > this->m_TickRate) {
				this->m_TickCnt -= this->m_TickRate;
				return true;
			}
			return false;
		}
		NetTime PlayerNetWork::Update(bool IsUpdateTick) noexcept {
			if (IsUpdateTick) {
				if (this->m_LastServerFrame < this->m_GetServerData.ServerFrame && this->m_GetServerData.ServerFrame <= (this->m_LastServerFrame + 60)) {//���͂��ꂽ�f�[�^���ȑO����������̂���0~60F�ゾ������
					this->m_LastServerFrame = this->m_GetServerData.ServerFrame;
					this->m_LeapFrame = 0;
					this->m_PrevServerData = this->m_ServerDataCommon;
					this->m_ServerDataCommon = this->m_GetServerData;
					auto Total = static_cast<int>(this->m_ServerDataCommon.ServerFrame) - static_cast<int>(this->m_PrevServerData.ServerFrame);
					if (Total <= 0) { Total = 20; }
					this->m_LeapFrameMax = static_cast<size_t>(Total);
					return this->m_LocalData.GetClientTime() - this->m_ServerDataCommon.PlayerData[this->m_LocalData.GetID()].GetClientTime();//��������f�[�^�Ǝ����̃t���[���Ƃ̍��ق�ping�ɂȂ�
				}
				return MAXLONGLONG;
			}
			else {
				this->m_LeapFrame = std::min<size_t>(this->m_LeapFrame + 1, this->m_LeapFrameMax);
				return 0;
			}
		}
		//�T�[�o�[��p
		bool ServerControl::AllReady(void) const noexcept {
			for (auto& n : this->m_Net) {
				int index = static_cast<int>(&n - &this->m_Net.front());
				if (this->m_IsServerPlay) {
					if (index == 0) { continue; }//�T�[�o�[�v���C���[�͐��0���g���̂�
				}
				if (!n.IsReady()) {
					return false;
				}
			}
			return true;
		}
		void ServerControl::Init(int pPort, bool IsServerPlay) noexcept {
			this->m_IsServerPlay = IsServerPlay;
			int i = 0;
			for (auto& n : this->m_Net) {
				int index = static_cast<int>(&n - &this->m_Net.front());
				if (this->m_IsServerPlay) {
					if (index == 0) { continue; }//�T�[�o�[�v���C���[�͐��0���g���̂�
				}
				n.m_NetWork.Init(true, pPort + i); ++i;
				n.m_Phase = ClientPhase::WaitConnect;
			}
		}
		bool ServerControl::Update(ServerNetData* pServerCtrl, const PlayerNetData& MyLocalPlayerData, bool IsUpdateTick) noexcept {
			//
			auto FlipPlayerData = [&](const PlayerNetData& value) { pServerCtrl->PlayerData[value.GetID()] = value; };
			FlipPlayerData(MyLocalPlayerData);// �T�[�o�[�v���C���[�̂̃v���C���[�f�[�^
			bool IsAllReady = AllReady();
			if (!IsAllReady) {
				pServerCtrl->ServerFrame = 0;
			}
			else {
				++pServerCtrl->ServerFrame;	// �T�[�o�[�t���[���X�V
				pServerCtrl->SetInGame();		// �C���Q�[���ł�
			}
			for (auto& n : this->m_Net) {
				int index = static_cast<int>(&n - &this->m_Net.front());
				if (this->m_IsServerPlay) {
					if (index == 0) { continue; }//�T�[�o�[�v���C���[�͐��0���g���̂�
				}
				PlayerNetData tmpData;
				int recvRet = -1;
				auto IsDataUpdated = n.m_NetWork.RecvData(&tmpData, &recvRet, false);
				if (IsDataUpdated) {
					if (tmpData.IsCheckSum()) {
						m_LastPlayerData[static_cast<size_t>(index)] = tmpData;
					}
				}
				bool IsSendData = false;
				switch (n.m_Phase) {
				case ClientPhase::WaitConnect:						// �����ʎ�t
					pServerCtrl->m_PlayerID = 0;
					if (IsDataUpdated) {							// �Y���\�P�b�g�ɃN���C�A���g����Ȃɂ���M������
						n.m_Phase = ClientPhase::GetNumber;
					}
					break;
				case ClientPhase::GetNumber://�v���C���[��t
					pServerCtrl->m_PlayerID = static_cast<PlayerID>(index);//ID��t���ł���
					IsSendData = true;
					if (IsDataUpdated) {
						if (m_LastPlayerData[static_cast<size_t>(index)].GetFlag(NetAttribute::IsActive)) {				// ID��ꂽ�Ǝ��ʏo������
							n.m_Phase = ClientPhase::Ready;
						}
					}
					break;
				case ClientPhase::Ready:						//���v���C���[�̑����҂�
					if (IsDataUpdated) {						// �N���C�A���g�����M������
						if (m_LastPlayerData[static_cast<size_t>(index)].IsCheckSum()) {				// �`�F�b�N�T���̂̂�
							FlipPlayerData(m_LastPlayerData[static_cast<size_t>(index)]);			// �X�V
						}
					}
					pServerCtrl->SetInGame();								//ID��t�I���Ɏw���ύX
					IsSendData = true;
					break;
				case ClientPhase::NotConnect:
				case ClientPhase::Error_CannotConnect:
				case ClientPhase::Error_ServerInGame:
				default:
					n.m_Phase = ClientPhase::WaitConnect;
					break;
				}
				if (IsSendData && IsUpdateTick) {
					pServerCtrl->CalcCheckSun();
					n.m_NetWork.ReturnData(*pServerCtrl);		// �N���C�A���g��ID�𑗂�
				}
			}
			return IsAllReady;
		}
		void ServerControl::Dispose(void) noexcept {
			for (auto& n : this->m_Net) {
				n.m_NetWork.Dispose();
			}
		}
		//�N���C�A���g��p
		void ClientControl::Init(int pPort, const IPDATA& pIP) noexcept {
			this->m_Port = pPort;
			this->m_IP = pIP;
			this->m_NetWorkSel = 0;
			this->m_Net.m_Phase = ClientPhase::NotConnect;
		}
		bool ClientControl::Update(ServerNetData* pServerCtrl, const PlayerNetData& MyLocalPlayerData, bool IsUpdateTick) noexcept {
			auto* DrawParts = DXDraw::Instance();
			int recvRet = -1;
			bool IsDataUpdated = this->m_Net.m_NetWork.RecvData(&m_BufferDataOnce, &recvRet, false);
			if (IsDataUpdated) {
				if (m_BufferDataOnce.IsCheckSum()) {
					*pServerCtrl = m_BufferDataOnce;
				}
			}
			bool IsSendData = false;
			switch (this->m_Net.m_Phase) {
			case ClientPhase::NotConnect:
				this->m_CannotConnectTimer = 0.f;
				this->m_Net.m_NetWork.SetServerIP(this->m_IP);
				this->m_Net.m_NetWork.Init(false, this->m_Port + this->m_NetWorkSel);
				this->m_Net.m_Phase = ClientPhase::WaitConnect;
				break;
			case ClientPhase::WaitConnect:
				IsSendData = true;
				if (IsDataUpdated) {
					this->m_CannotConnectTimer = 0.f;
					if (pServerCtrl->m_PlayerID > 0) {
						//�Ȃ��󂢂Ă��āA������ID�����炦���̂Ŏ���
						m_MyID = pServerCtrl->m_PlayerID;
						this->m_Net.m_Phase = ClientPhase::GetNumber;
					}
					else if (pServerCtrl->IsInGame()) {
						//�v�������T�[�o�[���C���Q�[�����ł���
						this->m_Net.m_NetWork.Dispose();
						this->m_Net.m_Phase = ClientPhase::Error_ServerInGame;	//�}�b�`�ςȂ̂Őڑ����s
					}
				}
				//���炦�ĂȂ�
				else {
					m_CannotConnectTimer += DrawParts->GetDeltaTime();
					if (this->m_CannotConnectTimer > 1.f) {
						m_CannotConnectTimer -= 1.f;
						++this->m_NetWorkSel;
						if (this->m_NetWorkSel < Player_num) {
							this->m_Net.m_NetWork.Dispose();
							this->m_Net.m_NetWork.SetServerIP(this->m_IP);
							this->m_Net.m_NetWork.Init(false, this->m_Port + this->m_NetWorkSel);
							this->m_Net.m_Phase = ClientPhase::WaitConnect;
						}
						else {
							this->m_Net.m_NetWork.Dispose();
							this->m_Net.m_Phase = ClientPhase::Error_CannotConnect;	//���^���Ȃ̂Őڑ����s
						}
					}
				}
				break;
			case ClientPhase::GetNumber:
				IsSendData = true;
				if (pServerCtrl->IsInGame()) {
					//�}�b�`�ςȂ̂Ŏ���
					this->m_Net.m_Phase = ClientPhase::Ready;
				}
				break;
			case ClientPhase::Ready:
				IsSendData = true;
				break;
			case ClientPhase::Error_CannotConnect:
			case ClientPhase::Error_ServerInGame:
			default:
				//�G���[
				break;
			}
			if (IsSendData && IsUpdateTick) {
				this->m_Net.m_NetWork.SendData(MyLocalPlayerData);		// ���g�̃f�[�^�𑗐M
			}
			return this->m_Net.IsReady();
		}
		void ClientControl::Dispose(void) noexcept {
			this->m_Net.m_NetWork.Dispose();
		}
		//�ʐM
		void NetWorkController::Init(bool IsClient, int Port, const IPDATA& ip, bool IsServerPlayer) noexcept {
			m_PlayerNet.Int(static_cast<NetTime>((1000.f / this->m_Tick) * 1000.f));
			this->m_IsClient = IsClient;
			this->m_IsServerPlayer = IsServerPlayer;
			if (this->m_IsClient) {
				m_ClientCtrl.Init(Port, ip);
			}
			else {
				m_ServerCtrl.Init(Port, this->m_IsServerPlayer);
			}
			this->m_Sequence = NetWorkSequence::Matching;
		}
		void NetWorkController::Update(void) noexcept {
			//���[�J���f�[�^�X�V
			this->m_PlayerNet.UpdateLocalData(m_LocalData, (this->m_Sequence == NetWorkSequence::MainGame));
			if (this->m_IsClient) {
				if (this->m_Sequence == NetWorkSequence::Matching) {
					//������PlayerID����
					if (m_ClientCtrl.CanGetMyID()) {
						this->m_PlayerNet.SetLocalPlayerID(m_ClientCtrl.GetMyID());
					}
				}
				this->m_PlayerNet.SetLocalPlayerFlag(NetAttribute::IsActive, m_ClientCtrl.CanGetMyID());
			}
			else if (this->m_ServerCtrl.GetIsServerPlayer()) {
				if (this->m_Sequence == NetWorkSequence::Matching) {
					//������PlayerID����
					this->m_PlayerNet.SetLocalPlayerID((PlayerID)0);
				}
			}
			else {
				static const int KeyInputs[] = {
					KEY_INPUT_NUMPAD0,
					KEY_INPUT_NUMPAD1,
					KEY_INPUT_NUMPAD2,
					KEY_INPUT_NUMPAD3,
					KEY_INPUT_NUMPAD4,
					KEY_INPUT_NUMPAD5,
					KEY_INPUT_NUMPAD6,
					KEY_INPUT_NUMPAD7,
					KEY_INPUT_NUMPAD8,
					KEY_INPUT_NUMPAD9,
				};
				//�T�[�o�[���ϐ��
				for (int index = 0; index < Player_num; ++index) {
					if (CheckHitKeyWithCheck(KeyInputs[index]) != 0) {
						this->m_PlayerNet.SetLocalPlayerID((PlayerID)index);
						break;
					}
				}
			}
			//�ʐM����
			bool IsUpdateTick = this->m_PlayerNet.UpdateTick();
			LONGLONG Ping = MAXLONGLONG;
			if (this->m_IsClient) {
				if (this->m_ClientCtrl.Update(&this->m_PlayerNet.SetGetServerData(), this->m_PlayerNet.GetLocalPlayerData(), IsUpdateTick)) {
					this->m_Sequence = NetWorkSequence::MainGame;
				}
				Ping = this->m_PlayerNet.Update(IsUpdateTick);
			}
			else {
				if (this->m_ServerCtrl.Update(&this->m_PlayerNet.SetGetServerData(), this->m_PlayerNet.GetLocalPlayerData(), IsUpdateTick)) {
					this->m_Sequence = NetWorkSequence::MainGame;
				}
				Ping = this->m_PlayerNet.Update(IsUpdateTick);
			}
			if (IsUpdateTick) {
				if (Ping != MAXLONGLONG) {
					if (Ping != 0) {
						CalcPing(Ping);
					}
				}
				else {
					CalcPing(-1); //���X�g
				}
			}
		}
	}
}
