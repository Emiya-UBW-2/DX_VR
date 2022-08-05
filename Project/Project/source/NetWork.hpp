
#pragma once

#include"Header.hpp"

class NewWorkControl {
	//通信関連
	int NetUDPHandle = -1;			// ネットワークハンドル
	IPDATA SendIp;					// 送信用ＩＰアドレスデータ
	int UsePort = -1;				// 通信用ポート
	//サーバー専用
	IPDATA RecvIp;					// 受信用ＩＰアドレスデータ
	int RecvPort;					// 受信用ポート
public:
	void InitClient() {
		// 送信用ソケットを作って初送信
		if (NetUDPHandle == -1) {
			NetUDPHandle = MakeUDPSocket(-1);
		}
	}
	void InitServer() {
		// ＵＤＰ通信用のソケットハンドルを作成
		if (NetUDPHandle == -1) {
			NetUDPHandle = MakeUDPSocket(UsePort);
		}
	}
	void Dispose() {
		if (NetUDPHandle != -1) {
			DeleteUDPSocket(NetUDPHandle);	// ＵＤＰソケットハンドルの削除
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
			NetWorkSendUDP(NetUDPHandle, Ip, SendPort, &Data, sizeof(T));	// 文字列の送信
			printfDx("送信:[%d,%d,%d,%d][%d]\n", Ip.d1, Ip.d2, Ip.d3, Ip.d4, SendPort);
		}
	}
	template<class T>
	bool Recv(T* Data) {
		bool ans = false;
		switch (CheckNetWorkRecvUDP(NetUDPHandle)) {
		case TRUE:
			NetWorkRecvUDP(NetUDPHandle, &RecvIp, &RecvPort, Data, sizeof(T), FALSE);		// 受信
			ans = true;
			printfDx("受信\n");
			break;
		case FALSE:
			printfDx("受信待ち\n");			//待ち
			break;
		default:
			printfDx("受信エラー\n");
			break;
		}
		return ans;
	}
};
