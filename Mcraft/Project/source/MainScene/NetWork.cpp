#pragma warning(disable:4464)
#include	"NetWork.hpp"

namespace FPS_n2 {
	namespace NetWork {
		//サーバー専用
		void ServerControl::Update(PlayerNetWork* pPlayerNetwork) noexcept {
			pPlayerNetwork->UpdateTick();
			bool AllReady = true;
			for (int loop = 0; loop < Player_num; ++loop) {
				if (GetIsServerPlayer(static_cast<PlayerID>(loop))) {
					// サーバープレイヤーのデータをアップデート
					pPlayerNetwork->SetLastServerDataBuffer().m_PlayerData[loop] = pPlayerNetwork->GetLocalPlayerData();
					// 席は埋まってます
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
				case ClientPhase::Server_WaitConnect:// 無差別受付
					if (IsDataUpdated) {// 該当ソケットにクライアントからなにか受信したら
						net.m_Phase = ClientPhase::Ready;
						// 席は埋まってます
						pPlayerNetwork->SetLastServerDataBuffer().m_PlayerFill[loop] = 1;
						//マッチング中とする
						if (pPlayerNetwork->SetLastServerDataBuffer().m_ServerPhase != ServerPhase::Matching) {
							pPlayerNetwork->SetLastServerDataBuffer().m_ServerPhase = ServerPhase::Matching;
						}
					}
					break;
				case ClientPhase::Ready:
					//得たプレイヤーのデータをアップデート
					if (IsDataUpdated) {
						if (player.GetID() == loop) {
							pPlayerNetwork->SetLastServerDataBuffer().m_PlayerFill[loop] = 2;
						}
						pPlayerNetwork->SetLastServerDataBuffer().m_PlayerData[loop] = player;
					}
					// クライアントにデータを送る
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
				++pPlayerNetwork->SetLastServerDataBuffer().m_ServerFrame;	// サーバーフレーム更新
				pPlayerNetwork->SetLastServerDataBuffer().m_ServerPhase = ServerPhase::Ingame; // インゲームです
			}
			//
			pPlayerNetwork->Update();
		}
		//クライアント専用
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
						//席が空いていて、自分のIDをもらえたので次へ
						pPlayerNetwork->SetLocalPlayerID(static_cast<PlayerID>(EmptyID));
						this->m_PlayerUDPPhase.m_Phase = ClientPhase::Client_GotPlayerID;
					}
				}
				//もらえてないので所定時間が経ったらソケットを変えて再接続
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
							this->m_PlayerUDPPhase.SendError(ClientPhase::Error_CannotConnect);	//満タンなので接続失敗
						}
					}
				}
				break;
			case ClientPhase::Client_GotPlayerID:
				if (pPlayerNetwork->GetLastServerDataBuffer().m_ServerPhase == ServerPhase::Ingame) {
					//マッチ済なので次へ
					this->m_PlayerUDPPhase.m_Phase = ClientPhase::Ready;
				}
				break;
			default:
				break;
			}
			// 自身のデータを送信
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
		//通信
		void NetWorkController::Update(void) noexcept {
			//ローカルデータ更新
			this->m_PlayerNet.UpdateLocalData(this->m_LocalData, IsInGame());
			if (this->m_IsClient) {
				this->m_ClientCtrl.Update(&this->m_PlayerNet);
			}
			else {
				if (this->m_ServerCtrl.GetIsServerPlayer(0)) {
					//自分のPlayerID決定
					this->m_PlayerNet.SetLocalPlayerID((PlayerID)0);
				}
				else {
					//サーバー兼観戦者
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
			//PING計測
			if (this->m_PlayerNet.GetTickUpdateFlag()) {
				if (this->m_PlayerNet.GetPingTime() == MAXLONGLONG) {
					CalcPing(InvalidID); //ロスト
				}
				else if (this->m_PlayerNet.GetPingTime() != 0) {
					CalcPing(this->m_PlayerNet.GetPingTime());
				}
			}
		}
	}
}
