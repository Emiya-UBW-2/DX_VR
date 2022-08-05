
#pragma once

#include"Header.hpp"

class NewWorkControl {
	//�ʐM�֘A
	int NetUDPHandle = -1;			// �l�b�g���[�N�n���h��
	IPDATA SendIp;					// ���M�p�h�o�A�h���X�f�[�^
	int UsePort = -1;				// �ʐM�p�|�[�g
	//�T�[�o�[��p
	IPDATA RecvIp;					// ��M�p�h�o�A�h���X�f�[�^
	int RecvPort;					// ��M�p�|�[�g
public:
	void InitClient() {
		// ���M�p�\�P�b�g������ď����M
		if (NetUDPHandle == -1) {
			NetUDPHandle = MakeUDPSocket(-1);
		}
	}
	void InitServer() {
		// �t�c�o�ʐM�p�̃\�P�b�g�n���h�����쐬
		if (NetUDPHandle == -1) {
			NetUDPHandle = MakeUDPSocket(UsePort);
		}
	}
	void Dispose() {
		if (NetUDPHandle != -1) {
			DeleteUDPSocket(NetUDPHandle);	// �t�c�o�\�P�b�g�n���h���̍폜
			NetUDPHandle = -1;
			UsePort = -1;
		}
	}
public:
	void SetIP(const IPDATA& IP) {
		SendIp = IP;
	}
	void Set_Port(int PORT) {
		UsePort = PORT;
	}

	template<class T>
	void SendtoServer(const T& Data) {
		Send(SendIp, UsePort, Data);
	}
	template<class T>
	void SendtoClient(const T& Data) {
		Send(RecvIp, RecvPort, Data);
	}
	template<class T>
	void Send(IPDATA& Ip, int SendPort, const T& Data) {
		if (NetUDPHandle != -1) {
			NetWorkSendUDP(NetUDPHandle, Ip, SendPort, &Data, sizeof(T));	// ������̑��M
			printfDx("���M:[%d,%d,%d,%d][%d]\n", Ip.d1, Ip.d2, Ip.d3, Ip.d4, SendPort);
		}
	}
	template<class T>
	bool Recv(T* Data) {
		bool ans = false;
		switch (CheckNetWorkRecvUDP(NetUDPHandle)) {
		case TRUE:
			NetWorkRecvUDP(NetUDPHandle, &RecvIp, &RecvPort, Data, sizeof(T), FALSE);		// ��M
			ans = true;
			printfDx("��M\n");
			break;
		case FALSE:
			printfDx("��M�҂�\n");			//�҂�
			break;
		default:
			printfDx("��M�G���[\n");
			break;
		}
		return ans;
	}
};
