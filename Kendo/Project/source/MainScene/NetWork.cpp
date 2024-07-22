#pragma warning(disable:4464)
#include "NetWork.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
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
			return tmp;
		}
		//�ʐM
		PlayerNetData  PlayerNetWork::GetLerpServerPlayerData(PlayerID pPlayerID) const noexcept {
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
		NetTime PlayerNetWork::Update(const ServerNetData& UpdateData) noexcept {
			auto& Data = UpdateData;

			if (this->m_LastServerFrame <= Data.ServerFrame && Data.ServerFrame <= (this->m_LastServerFrame + 60)) {//���͂��ꂽ�f�[�^���ȑO����������̂���0~60F�ゾ������
				this->m_LastServerFrame = Data.ServerFrame;
				this->m_LeapFrame = 0;
				this->m_PrevServerData = this->m_ServerDataCommon;
				this->m_ServerDataCommon = Data;
				auto Total = static_cast<int>(this->m_ServerDataCommon.ServerFrame) - static_cast<int>(this->m_PrevServerData.ServerFrame);
				if (Total <= 0) { Total = 20; }
				this->m_LeapFrameMax = static_cast<size_t>(Total);
				return this->m_LocalData.GetClientTime() - this->m_ServerDataCommon.PlayerData[this->m_LocalData.GetID()].GetClientTime();//��������f�[�^�Ǝ����̃t���[���Ƃ̍��ق�ping�ɂȂ�
			}
			else {
				this->m_LeapFrame = std::min<size_t>(this->m_LeapFrame + 1, this->m_LeapFrameMax);
			}
			return MAXLONGLONG;
		}
		//�T�[�o�[��p
		bool ServerControl::AllReady() const noexcept {
			for (auto& n : this->m_Net) {
				if (!n.IsReady()) {
					return false;
				}
			}
			return true;
		}
		void ServerControl::Init(int pPort, bool IsServerPlay) noexcept {
			this->m_ServerData.m_PlayerID = -1;
			this->m_IsServerPlay = IsServerPlay;
			int i = 0;
			for (auto& n : this->m_Net) {
				int index = static_cast<int>(&n - &this->m_Net.front());
				if (this->m_IsServerPlay) {
					if (index == 0) { continue; }//�T�[�o�[�v���C���[�͐��0���g���̂�
				}
				n.m_NetWork.Init(true, pPort + i); i++;
				n.m_Phase = ClientPhase::WaitConnect;
			}
		}
		bool ServerControl::Execute(const PlayerNetData& MyLocalPlayerData, bool IsUpdateTick) noexcept {
			//
			auto FlipPlayerData = [&](const PlayerNetData& value) { this->m_ServerData.PlayerData[value.GetID()] = value; };
			FlipPlayerData(MyLocalPlayerData);// �T�[�o�[�v���C���[�̂̃v���C���[�f�[�^
			bool IsAllReady = AllReady();
			if (!IsAllReady) {
				this->m_ServerData.ServerFrame = 0;
			}
			else {
				this->m_ServerData.ServerFrame++;	// �T�[�o�[�t���[���X�V
				this->m_ServerData.SetInGame();		// �C���Q�[���ł�
			}
			for (auto& n : this->m_Net) {
				int index = static_cast<int>(&n - &this->m_Net.front());
				if (this->m_IsServerPlay) {
					if (index == 0) { continue; }//�T�[�o�[�v���C���[�͐��0���g���̂�
				}
				PlayerNetData tmpData;
				int recvRet = -1;
				auto IsDataUpdated = n.m_NetWork.RecvData(&tmpData, &recvRet, true);
				if (IsDataUpdated) {
					if (recvRet == -2) {
						PlayerNetData tmpDatas[30];
						//�傫�߂̃o�b�t�@�[��p�ӂ��Ă��̒�����T��
						n.m_NetWork.RecvData(&tmpDatas, &recvRet, false);
						NetTime serverframe = 0;
						int watch = -1;
						for (int i = 0; i < 30; i++) {
							if (tmpDatas[i].IsCheckSum()) {//todo:�I�ł̃`�F�b�N�T��
								if (serverframe < tmpDatas[i].GetClientTime()) {
									serverframe = tmpDatas[i].GetClientTime();							//��ԃT�o�t���̑傫�ȓz��T��
									watch = i;
								}
							}
						}
						if (watch != -1) {
							tmpData = tmpDatas[watch];
						}
					}
					else {
						n.m_NetWork.RecvData(&tmpData, &recvRet, false);
					}
				}
				bool IsSendData = false;
				switch (n.m_Phase) {
				case ClientPhase::WaitConnect:						// �����ʎ�t
					if (IsDataUpdated) {							// �Y���\�P�b�g�ɃN���C�A���g����Ȃɂ���M������
						n.m_Phase = ClientPhase::GetNumber;
					}
					break;
				case ClientPhase::GetNumber://�v���C���[��t
					if (IsDataUpdated) {
						if (tmpData.GetFlag(NetAttribute::IsActive)) {				// ID��ꂽ�Ǝ��ʏo������
							n.m_Phase = ClientPhase::Ready;
						}
					}
					this->m_ServerData.m_PlayerID = static_cast<PlayerID>(index);//ID��t���ł���
					IsSendData = true;
					break;
				case ClientPhase::Ready:						//���v���C���[�̑����҂�
					if (IsDataUpdated) {						// �N���C�A���g�����M������
						if (tmpData.IsCheckSum()) {				// �`�F�b�N�T���̂̂�
							FlipPlayerData(tmpData);			// �X�V
						}
					}
					this->m_ServerData.m_PlayerID = -1;								//ID��t�I���Ɏw���ύX
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
					n.m_NetWork.ReturnData(this->m_ServerData);		// �N���C�A���g��ID�𑗂�
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
		bool ClientControl::Execute(const PlayerNetData& MyLocalPlayerData, bool IsUpdateTick) noexcept {
			auto* DrawParts = DXDraw::Instance();
			int recvRet = -1;
			bool IsDataUpdated = this->m_Net.m_NetWork.RecvData(&m_BufferDataOnce, &recvRet, true);
			if (IsDataUpdated) {
				if (recvRet == -2) {//����Ȃ��ꍇ
					//�傫�߂̃o�b�t�@�[��p�ӂ��Ă��̒�����T��
					this->m_Net.m_NetWork.RecvData(&m_BufferData, &recvRet, false);
					size_t serverframe = 0;
					int watch = -1;
					for (int i = 0; i < 10; i++) {
						if (m_BufferData[i].IsCheckSum()) {
							if (serverframe < m_BufferData[i].ServerFrame) {
								serverframe = m_BufferData[i].ServerFrame;							//��ԃT�o�t���̑傫�ȓz��T��
								watch = i;
							}
						}
					}
					if (watch != -1) {
						m_ServerData = m_BufferData[watch];
					}
				}
				else {
					this->m_Net.m_NetWork.RecvData(&m_ServerData, &recvRet, false);
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
					if (this->m_ServerData.m_PlayerID >= 0) {
						//�Ȃ��󂢂Ă��āA������ID�����炦���̂Ŏ���
						m_MyID = this->m_ServerData.m_PlayerID;
						this->m_Net.m_Phase = ClientPhase::GetNumber;
					}
					else if (this->m_ServerData.IsInGame()) {
						//�v�������T�[�o�[���C���Q�[�����ł���
						this->m_Net.m_NetWork.Dispose();
						this->m_Net.m_Phase = ClientPhase::Error_ServerInGame;	//�}�b�`�ςȂ̂Őڑ����s
					}
				}
				//���炦�ĂȂ�
				else {
					m_CannotConnectTimer += 1.f / DrawParts->GetFps();
					if (this->m_CannotConnectTimer > 1.f) {
						m_CannotConnectTimer -= 1.f;
						this->m_NetWorkSel++;
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
				if (this->m_ServerData.IsInGame()) {
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
	};
};
