#pragma warning(disable:4464)
#include	"NetWork.hpp"

namespace FPS_n2 {
	namespace NetWork {
		//�T�[�o�[��p
		void ServerControl::Update(PlayerNetWork* pPlayerNetwork) noexcept {
			pPlayerNetwork->UpdateTick();
			bool AllReady = true;
			for (int loop = 0; loop < Player_num; ++loop) {
				if (GetIsServerPlayer(static_cast<PlayerID>(loop))) {
					// �T�[�o�[�v���C���[�̃f�[�^���A�b�v�f�[�g
					pPlayerNetwork->SetLastServerDataBuffer().m_PlayerData[loop] = pPlayerNetwork->GetLocalPlayerData();
					// �Ȃ͖��܂��Ă܂�
					pPlayerNetwork->SetLastServerDataBuffer().m_PlayerFill[loop] = 2;
					continue;
				}
				auto& net = this->m_PlayerUDPPhase.at(loop);

				PlayerNetData tmpData;
				int recvRet = -1;
				auto IsDataUpdated = net.m_NetWork.RecvData(&tmpData, &recvRet, false) && tmpData.IsCheckSum();
				if (IsDataUpdated) {
					this->m_LastPlayerData.at(loop) = tmpData;
				}
				const auto& player = this->m_LastPlayerData.at(loop);
				switch (net.m_Phase) {
				case ClientPhase::Server_WaitConnect:// �����ʎ�t
					if (IsDataUpdated) {// �Y���\�P�b�g�ɃN���C�A���g����Ȃɂ���M������
						net.m_Phase = ClientPhase::Ready;
						// �Ȃ͖��܂��Ă܂�
						pPlayerNetwork->SetLastServerDataBuffer().m_PlayerFill[loop] = 1;
						//�}�b�`���O���Ƃ���
						if (pPlayerNetwork->SetLastServerDataBuffer().m_ServerPhase != ServerPhase::Matching) {
							pPlayerNetwork->SetLastServerDataBuffer().m_ServerPhase = ServerPhase::Matching;
						}
					}
					break;
				case ClientPhase::Ready:
					//�����v���C���[�̃f�[�^���A�b�v�f�[�g
					if (IsDataUpdated) {
						if (player.GetID() == loop) {
							pPlayerNetwork->SetLastServerDataBuffer().m_PlayerFill[loop] = 2;
						}
						pPlayerNetwork->SetLastServerDataBuffer().m_PlayerData[loop] = player;
					}
					// �N���C�A���g�Ƀf�[�^�𑗂�
					if (pPlayerNetwork->GetTickUpdateFlag()) {
						pPlayerNetwork->SetLastServerDataBuffer().SetCheckSum();
						net.m_NetWork.ReturnData(pPlayerNetwork->GetLastServerDataBuffer());
					}
					break;
				default:
					break;
				}
				if (net.m_Phase != ClientPhase::Ready) {
					AllReady = false;
				}
			}
			//
			if (!AllReady) {
				pPlayerNetwork->SetLastServerDataBuffer().m_ServerFrame = 0;
			}
			else {
				++pPlayerNetwork->SetLastServerDataBuffer().m_ServerFrame;	// �T�[�o�[�t���[���X�V
				pPlayerNetwork->SetLastServerDataBuffer().m_ServerPhase = ServerPhase::Ingame; // �C���Q�[���ł�
			}
			//
			pPlayerNetwork->Update();
		}
		//�N���C�A���g��p
		void ClientControl::Update(PlayerNetWork* pPlayerNetwork) noexcept {
			pPlayerNetwork->UpdateTick();

			ServerNetData			tmpData;
			int recvRet = -1;
			bool IsDataUpdated = this->m_PlayerUDPPhase.m_NetWork.RecvData(&tmpData, &recvRet, false) && tmpData.IsCheckSum();
			if (IsDataUpdated) {
				pPlayerNetwork->SetLastServerDataBuffer() = tmpData;
			}
			switch (this->m_PlayerUDPPhase.m_Phase) {
			case ClientPhase::Client_WaitConnect:
				if (IsDataUpdated) {
					this->m_CannotConnectTimer = 0.f;
					int EmptyID = pPlayerNetwork->SetLastServerDataBuffer().GetEmptyID();
					if (EmptyID != InvalidID) {
						//�Ȃ��󂢂Ă��āA������ID�����炦���̂Ŏ���
						pPlayerNetwork->SetLocalPlayerID(static_cast<PlayerID>(EmptyID));
						this->m_PlayerUDPPhase.m_Phase = ClientPhase::Client_GotPlayerID;
					}
				}
				//���炦�ĂȂ��̂ŏ��莞�Ԃ��o������\�P�b�g��ς��čĐڑ�
				else {
					auto* DXLib_refParts = DXLib_ref::Instance();
					this->m_CannotConnectTimer += DXLib_refParts->GetDeltaTime();
					if (this->m_CannotConnectTimer > 1.f) {
						this->m_CannotConnectTimer -= 1.f;
						++this->m_NetWorkSelect;
						if (this->m_NetWorkSelect < Player_num) {
							this->m_PlayerUDPPhase.Dispose();
							this->m_PlayerUDPPhase.m_NetWork.SetServerIP(this->m_IP);
							this->m_PlayerUDPPhase.Init(false, this->m_Port + this->m_NetWorkSelect);
						}
						else {
							this->m_PlayerUDPPhase.SendError(ClientPhase::Error_CannotConnect);	//���^���Ȃ̂Őڑ����s
						}
					}
				}
				break;
			case ClientPhase::Client_GotPlayerID:
				if (pPlayerNetwork->GetLastServerDataBuffer().m_ServerPhase == ServerPhase::Ingame) {
					//�}�b�`�ςȂ̂Ŏ���
					this->m_PlayerUDPPhase.m_Phase = ClientPhase::Ready;
				}
				break;
			default:
				break;
			}
			// ���g�̃f�[�^�𑗐M
			switch (this->m_PlayerUDPPhase.m_Phase) {
			case ClientPhase::Client_WaitConnect:
			case ClientPhase::Client_GotPlayerID:
			case ClientPhase::Ready:
				if (pPlayerNetwork->GetTickUpdateFlag()) {
					this->m_PlayerUDPPhase.m_NetWork.SendData(pPlayerNetwork->GetLocalPlayerData());
				}
				break;
			default:
				break;
			}
			pPlayerNetwork->Update();
		}
		//�ʐM
		void NetWorkController::Update(void) noexcept {
			//���[�J���f�[�^�X�V
			this->m_PlayerNet.UpdateLocalData(this->m_LocalData, IsInGame());
			if (this->m_IsClient) {
				this->m_ClientCtrl.Update(&this->m_PlayerNet);
			}
			else {
				if (this->m_ServerCtrl.GetIsServerPlayer(0)) {
					//������PlayerID����
					this->m_PlayerNet.SetLocalPlayerID((PlayerID)0);
				}
				else {
					//�T�[�o�[���ϐ��
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
					for (int loop = 0; loop < Player_num; ++loop) {
						if (CheckHitKey(KeyInputs[loop]) != 0) {
							this->m_PlayerNet.SetLocalPlayerID((PlayerID)loop);
							break;
						}
					}
				}
				this->m_ServerCtrl.Update(&this->m_PlayerNet);
			}
			//PING�v��
			if (this->m_PlayerNet.GetTickUpdateFlag()) {
				if (this->m_PlayerNet.GetPingTime() == MAXLONGLONG) {
					CalcPing(InvalidID); //���X�g
				}
				else if (this->m_PlayerNet.GetPingTime() != 0) {
					CalcPing(this->m_PlayerNet.GetPingTime());
				}
			}
		}
	}
}
