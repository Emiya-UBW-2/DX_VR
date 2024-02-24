#pragma once
#include	"Header.hpp"
#include "sub.hpp"

class NewWorkControl {
	//�ʐM�֘A
	int NetUDPHandle{-1};			// �l�b�g���[�N�n���h��
	IPDATA IP{127,0,0,1};			// ���M�p�h�o�A�h���X�f�[�^
	int UsePort{-1};				// �ʐM�p�|�[�g
	//�T�[�o�[��p
	IPDATA RecvIp{127,0,0,1};		// ��M�p�h�o�A�h���X�f�[�^
	int RecvPort{0};				// ��M�p�|�[�g
public:
	void			InitClient() {
		// ���M�p�\�P�b�g������ď����M
		if (NetUDPHandle == -1) {
			NetUDPHandle = MakeUDPSocket(-1);
		}
	}
	void			InitServer() {
		// �t�c�o�ʐM�p�̃\�P�b�g�n���h�����쐬
		if (NetUDPHandle == -1) {
			NetUDPHandle = MakeUDPSocket(UsePort);
		}
	}
	void			Dispose() {
		if (NetUDPHandle != -1) {
			DeleteUDPSocket(NetUDPHandle);	// �t�c�o�\�P�b�g�n���h���̍폜
			NetUDPHandle = -1;
			UsePort = -1;
		}
	}
public:
	void			SetIP(const IPDATA& pIP) { IP = pIP; }
	void			Set_Port(int PORT) { UsePort = PORT; }

	template<class T>
	void			SendtoServer(const T& Data) { Send(IP, UsePort, Data); }
	template<class T>
	void			SendtoClient(const T& Data) { Send(RecvIp, RecvPort, Data); }
	template<class T>
	void			Send(IPDATA& Ip, int SendPort, const T& Data) {
		if (NetUDPHandle != -1) {
			NetWorkSendUDP(NetUDPHandle, Ip, SendPort, &Data, sizeof(T));
			//printfDx("���M:[%d,%d,%d,%d][%d]\n", Ip.d1, Ip.d2, Ip.d3, Ip.d4, SendPort);
		}
	}
	template<class T>
	bool Recv(T* Data) {
		switch (CheckNetWorkRecvUDP(NetUDPHandle)) {
			case TRUE:
				NetWorkRecvUDP(NetUDPHandle, &RecvIp, &RecvPort, Data, sizeof(T), FALSE);		// ��M
				return true;
				break;
			case FALSE://�ҋ@
				break;
			default://error
				break;
		}
		return false;
	}
};

namespace FPS_n2 {
	namespace Sceneclass {
		//�ʐM
		static const int		Player_num = 12;
		struct NewSetting {
			IPDATA					IP{127,0,0,1};
			int						UsePort{10850};
		};
		class NewWorkSetting {
			std::vector<NewSetting>	m_NewWorkSetting;
		public:
			void Load(void) noexcept {
				SetOutApplicationLogValidFlag(FALSE);
				int mdata = FileRead_open("data/NetWorkSetting.txt", FALSE);
				while (true) {
					m_NewWorkSetting.resize(this->m_NewWorkSetting.size() + 1);
					m_NewWorkSetting.back().UsePort = std::clamp<int>(getparams::_int(mdata), 0, 50000);
					m_NewWorkSetting.back().IP.d1 = (unsigned char)std::clamp((int)getparams::_int(mdata), 0, 255);
					m_NewWorkSetting.back().IP.d2 = (unsigned char)std::clamp((int)getparams::_int(mdata), 0, 255);
					m_NewWorkSetting.back().IP.d3 = (unsigned char)std::clamp((int)getparams::_int(mdata), 0, 255);
					m_NewWorkSetting.back().IP.d4 = (unsigned char)std::clamp((int)getparams::_int(mdata), 0, 255);
					if (FileRead_eof(mdata) != 0) {
						break;
					}
				}
				FileRead_close(mdata);
				SetOutApplicationLogValidFlag(TRUE);
			}
			void Save(void) noexcept {
				std::ofstream outputfile("data/NetWorkSetting.txt");
				for (auto& n : this->m_NewWorkSetting) {
					int ID = (int)(&n - &this->m_NewWorkSetting.front());
					outputfile << "Setting" + std::to_string(ID) + "_Port=" + std::to_string(n.UsePort) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP1=" + std::to_string(n.IP.d1) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP2=" + std::to_string(n.IP.d2) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP3=" + std::to_string(n.IP.d3) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP4=" + std::to_string(n.IP.d4) + "\n";
				}
				outputfile.close();
			}

			const auto		GetSize(void) const noexcept { return (int)m_NewWorkSetting.size(); }
			const auto&		Get(int ID) const noexcept { return this->m_NewWorkSetting[ID]; }
			const auto		Add(void) noexcept {
				m_NewWorkSetting.resize(this->m_NewWorkSetting.size() + 1);
				m_NewWorkSetting.back().UsePort = 10850;
				m_NewWorkSetting.back().IP.d1 = 127;
				m_NewWorkSetting.back().IP.d2 = 0;
				m_NewWorkSetting.back().IP.d3 = 0;
				m_NewWorkSetting.back().IP.d4 = 1;
				return (int)m_NewWorkSetting.size() - 1;;
			}
			void Set(int ID, const NewSetting& per)noexcept { this->m_NewWorkSetting[ID] = per; }
		};

		class PlayerNetData {
		private:
			size_t						CheckSum{0};
			double						Frame{0.0};
		public:
			bool						IsActive{false};
			PlayerID					ID{0};
			InputControl				Input;
			moves						m_move;
			std::vector<DamageEvent>	m_DamageEvents;
		private:
			const auto			CalcCheckSum(void) noexcept {
				return (
					((int)(this->m_move.pos.x()*100.f) + (int)(this->m_move.pos.y()*100.f) + (int)(this->m_move.pos.z()*100.f)) +
					((int)(this->m_move.vec.x()*100.f) + (int)(this->m_move.vec.y()*100.f) + (int)(this->m_move.vec.z()*100.f)) +
					(int)(rad2deg(this->m_move.rad.y())) +
					(int)(ID)
					);
			}
		public:
			const auto			GetIsActive(void) noexcept { return this->CalcCheckSum() != 0; }
			const auto			IsCheckSum(void) noexcept { return CheckSum == (size_t)this->CalcCheckSum(); }

			const auto&			GetID(void) const noexcept { return this->ID; }
			const auto&			GetFrame(void) const noexcept { return this->Frame; }

			void				SetMyPlayer(const InputControl& pInput, const moves & move_t, const std::vector<DamageEvent>& Damage_t, double pFrame) noexcept {
				this->Input = pInput;
				this->m_move = move_t;
				this->m_DamageEvents = Damage_t;
				this->Frame = pFrame;
				this->CheckSum = (size_t)this->CalcCheckSum();
			}
		public:
			static const auto	GetLerpData(const PlayerNetData&PrevData, const PlayerNetData& ServerData, float Per, bool isYradReset) {
				PlayerNetData tmp;
				//
				tmp.Input = Lerp(PrevData.Input, ServerData.Input, Per);
				tmp.m_move = Lerp(PrevData.m_move, ServerData.m_move, Per);
				//
				tmp.ID = ServerData.ID;
				tmp.IsActive = ServerData.IsActive;
				tmp.Frame = ServerData.Frame;
				tmp.Input.SetKeyInputFlags(ServerData.Input);
				if (isYradReset) {
					auto radvec = Lerp(MATRIX_ref::RotY(PrevData.m_move.rad.y()).zvec(), MATRIX_ref::RotY(ServerData.m_move.rad.y()).zvec(), Per).Norm();
					tmp.m_move.rad.y(-atan2f(radvec.x(), radvec.z()));
				}
				tmp.m_DamageEvents = ServerData.m_DamageEvents;
				return tmp;
			}
		};
		struct ServerNetData {
			int					Tmp1{0};				//4
			int					StartFlag{0};			//4
			size_t				ServerFrame{0};		//8
			PlayerNetData		PlayerData[Player_num];	//37 * 3
		};
		class NetWorkControl {
		protected:
			size_t											m_ServerFrame{0};
			std::array<int, Player_num>						m_LeapFrame{0};
			ServerNetData									m_ServerDataCommon, m_PrevServerData;
			PlayerNetData									m_PlayerData;
			float											m_TickCnt{0.f};
			float											m_TickRate{10.f};
		public:
			const auto		GetRecvData(int pPlayerID) const noexcept { return this->m_LeapFrame[pPlayerID] <= 1; }
			const auto&		GetServerDataCommon(void) const noexcept { return this->m_ServerDataCommon; }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_PlayerData.GetID(); }
			void			SetMyPlayer(const InputControl& pInput, const moves & move_t, const std::vector<DamageEvent>& Damage_t, double pFrame) noexcept {
				this->m_PlayerData.SetMyPlayer(pInput, move_t, Damage_t, pFrame);
			}

			const auto		GetNowServerPlayerData(int pPlayerID, bool isyradReset) noexcept {
				auto Total = (int)this->m_ServerDataCommon.ServerFrame - (int)this->m_PrevServerData.ServerFrame;
				if (Total <= 0) { Total = 20; }
				PlayerNetData tmp;
				tmp = PlayerNetData::GetLerpData(
					this->m_PrevServerData.PlayerData[pPlayerID], this->m_ServerDataCommon.PlayerData[pPlayerID],
					(float)this->m_LeapFrame[pPlayerID] / (float)Total, isyradReset);
				this->m_LeapFrame[pPlayerID] = std::clamp<int>(this->m_LeapFrame[pPlayerID] + 1, 0, Total);
				return tmp;
			}
			virtual void	SetParam(int pPlayerID, const VECTOR_ref& pPos) noexcept {
				this->m_ServerDataCommon.PlayerData[pPlayerID].m_move.pos = pPos;
				this->m_ServerDataCommon.ServerFrame = 0;
				this->m_PrevServerData.PlayerData[pPlayerID].m_move.pos = pPos;	// �T�[�o�[�f�[�^
				this->m_PrevServerData.ServerFrame = 0;
			}
		protected:
			void			CommonInit(void) noexcept {
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
		public:
			virtual void	Init(int, float, const IPDATA&) noexcept {}
			virtual bool	Execute(void) noexcept { return false; }
			virtual void	Dispose(void) noexcept {}
		};
		class ServerControl : public NetWorkControl {
			ServerNetData			m_ServerData;
			std::array<std::pair<NewWorkControl, int>, Player_num - 1>		m_NetWork;
		public:
			const auto&		GetServerData(void) const noexcept { return this->m_ServerData; }
			void			SetParam(int pPlayerID, const VECTOR_ref& pPos) noexcept override {
				NetWorkControl::SetParam(pPlayerID, pPos);
				this->m_ServerData.PlayerData[pPlayerID].m_move.pos = this->m_ServerDataCommon.PlayerData[pPlayerID].m_move.pos;
				this->m_ServerData.PlayerData[pPlayerID].IsActive = false;
			}
		public:
			void			Init(int pPort, float pTick, const IPDATA&) noexcept override {
				CommonInit();
				int i = 0;
				for (auto & n : this->m_NetWork) {
					n.first.Set_Port(pPort + i); i++;
					n.first.InitServer();
					n.second = 0;
				}
				this->m_ServerDataCommon.ServerFrame = 0;

				this->m_ServerData.Tmp1 = 0;
				this->m_ServerData.StartFlag = 0;
				this->m_ServerData.PlayerData[0].IsActive = true;
				this->m_ServerData.ServerFrame = 0;

				this->m_PlayerData.ID = 0;
				this->m_TickRate = pTick;
			}
			bool			Execute(void) noexcept override {
				bool canMatch = true;
				bool canSend = false;
				for (auto & n : this->m_NetWork) {
					if (!(n.second >= 2)) {
						canMatch = false;
						break;
					}
				}
				if (canMatch) {
					// �e�B�b�N���[�g�p���Z
					this->m_TickCnt += 60.f / FPS;
					if (this->m_TickCnt > this->m_TickRate) {
						this->m_TickCnt -= this->m_TickRate;
						canSend = true;
					}
					//�T�[�o�[�f�[�^�̍X�V
					this->m_ServerData.StartFlag = 1;
					this->m_ServerData.PlayerData[this->m_PlayerData.ID] = this->m_PlayerData;		// �T�[�o�[�v���C���[�̂̃v���C���[�f�[�^
					this->m_ServerData.ServerFrame++;											// �T�[�o�[�t���[���X�V
				}
				for (auto & n : this->m_NetWork) {
					size_t index = (&n - &this->m_NetWork.front()) + 1;
					int tmpData = -1;
					switch (n.second) {
						case 0:										//�����ʎ�t
							if (n.first.Recv(&tmpData)) {			// �Y���\�P�b�g�ɃN���C�A���g����Ȃɂ���M������
								n.second++;
							}
							break;
						case 1:
							this->m_ServerData.Tmp1 = (int)index;
							this->m_ServerData.StartFlag = 0;
							this->m_ServerData.PlayerData[index].IsActive = true;

							n.first.SendtoClient(this->m_ServerData);					//�N���C�A���g�S���ɑ���

							if (n.first.Recv(&tmpData)) {
								if (tmpData == 1) {					// ID��ꂽ�Ǝ��ʏo������
									n.second++;
								}
							}
							break;
						case 2://�����҂�
							if (canMatch) { n.second++; }
							break;
						case 3:
							{
								PlayerNetData tmpPData;
								if (n.first.Recv(&tmpPData)) {							// �N���C�A���g�����M������
									if (tmpPData.IsCheckSum()) {
										this->m_ServerData.PlayerData[tmpPData.ID] = tmpPData;	// �X�V
									}
								}
							}
							if (canSend) {
								n.first.SendtoClient(this->m_ServerData);						//�N���C�A���g�S���ɑ���
							}
							break;
						default:
							break;
					}
				}
				if (canSend) {
					NetCommonExecute(this->m_ServerData);			// �X�V
				}
				return canMatch;
			}
			void			Dispose(void) noexcept override {
				for (auto & n : this->m_NetWork) {
					n.first.Dispose();
				}
			}
		};
		class ClientControl : public NetWorkControl {
			int						m_NetWorkSel{0};
			float					m_CannotConnectTimer{0.f};
			int						m_Port{0};
			IPDATA					m_IP{127,0,0,1};
			std::pair<NewWorkControl, int>	m_NetWork;
		public:
			void			Init(int pPort, float pTick, const IPDATA& pIP) noexcept override {
				CommonInit();

				this->m_CannotConnectTimer = 0.f;
				this->m_Port = pPort;
				this->m_TickRate = pTick;
				this->m_IP = pIP;

				this->m_NetWorkSel = 0;
				this->m_NetWork.first.Set_Port(this->m_Port + this->m_NetWorkSel);
				this->m_NetWork.first.SetIP(pIP);
				this->m_NetWork.first.InitClient();
				this->m_NetWork.second = 0;

				this->m_PlayerData.ID = 1;
			}
			bool			Execute(void) noexcept override {
				ServerNetData tmpData;
				bool canMatch = true;
				bool canSend = false;
				canMatch = (this->m_NetWork.second >= 2);
				if (canMatch) {
					// �e�B�b�N���[�g�p���Z
					this->m_TickCnt += 60.f / FPS;
					if (this->m_TickCnt > this->m_TickRate) {
						this->m_TickCnt -= this->m_TickRate;
						canSend = true;
					}
				}
				switch (this->m_NetWork.second) {
					case 0:
						this->m_NetWork.first.SendtoServer(0);			// �ʐM���N�G�X�g
						//�T�[�o�[����̎�����ID����M
						if (this->m_NetWork.first.Recv(&tmpData)) {
							NetCommonExecute(tmpData);								//�X�V
							m_CannotConnectTimer = 0.f;
							if (tmpData.Tmp1 > 0) {
								this->m_PlayerData.ID = (PlayerID)tmpData.Tmp1;
								this->m_NetWork.second++;
							}
						}
						else {
							m_CannotConnectTimer += 1.f / FPS;
							if (this->m_CannotConnectTimer > 1.f) {
								m_CannotConnectTimer = 0.f;
								this->m_NetWork.first.Dispose();
								this->m_NetWorkSel++;
								this->m_NetWork.first.Set_Port(this->m_Port + this->m_NetWorkSel);
								this->m_NetWork.first.SetIP(this->m_IP);
								this->m_NetWork.first.InitClient();
								this->m_NetWork.second = 0;
								if (this->m_NetWorkSel >= Player_num) {
									//���^��
								}
							}
						}
						break;
					case 1:
						this->m_NetWork.first.SendtoServer(1);			// ID��ꂽ��
						//�T�[�o�[����̃f�[�^����M�����玟��
						if (this->m_NetWork.first.Recv(&tmpData)) {
							NetCommonExecute(tmpData);								//�X�V
							if (tmpData.StartFlag == 1) {
								this->m_NetWork.second++;
							}
						}
						break;
					case 2:
						if (canSend) {
							this->m_NetWork.first.SendtoServer(this->m_PlayerData);				//���g�̃f�[�^�𑗐M
						}
						if (this->m_NetWork.first.Recv(&tmpData)) {					//�T�[�o�[����̃f�[�^����M������
							NetCommonExecute(tmpData);								//�X�V
						}
						break;
					default:
						break;
				}
				return canMatch;
			}
			void			Dispose(void) noexcept override {
				this->m_NetWork.first.Dispose();
			}
		};
	};
};
