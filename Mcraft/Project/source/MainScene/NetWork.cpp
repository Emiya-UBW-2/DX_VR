#pragma warning(disable:4464)
#include	"NetWork.hpp"

namespace FPS_n2 {
	namespace NetWork {
		//サーバー専用
		void ServerControl::Update(PlayerNetWork* pPlayerNetwork) noexcept {
			bool AllReady = true;
			for (int loop = 0; loop < Player_num; ++loop) {
				// サーバープレイヤーのデータをアップデート
				if (GetIsServerPlayer(static_cast<PlayerID>(loop))) {
					pPlayerNetwork->SetLastServerDataBuffer().UpdatePlayerData(pPlayerNetwork->GetLocalPlayerData());
					continue;
				}
				//それ以外のプレイヤーデータの受信と反映
				auto& net = this->m_PlayerUDPPhase[loop];
				auto IsDataUpdated = net.RecvData(&this->m_LastPlayerData[loop]);
				switch (net.GetPhase()) {
				case ClientPhase::WaitConnect:// 無差別受付
					if (IsDataUpdated) {// 該当ソケットにクライアントからなにか受信したら
						net.SetPhase(ClientPhase::Ready);
						pPlayerNetwork->SetLastServerDataBuffer().SetReady(static_cast<PlayerID>(loop));
					}
					break;
				case ClientPhase::Ready:
					//得たプレイヤーのデータをアップデート
					if (IsDataUpdated) {
						if (this->m_LastPlayerData[loop].GetID() == loop) {
							pPlayerNetwork->SetLastServerDataBuffer().UpdatePlayerData(this->m_LastPlayerData[loop]);
						}
					}
					// クライアントにデータを送る
					if (pPlayerNetwork->GetTickUpdateFlag()) {
						pPlayerNetwork->SetLastServerDataBuffer().SetCheckSum();
						net.SendData(pPlayerNetwork->GetLastServerDataBuffer());
					}
					break;
				default:
					break;
				}
				if (net.GetPhase() != ClientPhase::Ready) {
					AllReady = false;
				}
			}
			pPlayerNetwork->SetLastServerDataBuffer().Update(AllReady);
		}
		//クライアント専用
		void ClientControl::Update(PlayerNetWork* pPlayerNetwork) noexcept {
			auto& net = this->m_PlayerUDPPhase;
			bool IsDataUpdated = net.RecvData(&pPlayerNetwork->SetLastServerDataBuffer());
			switch (net.GetPhase()) {
			case ClientPhase::WaitConnect:
				if (IsDataUpdated) {
					this->m_CannotConnectTimer = 0.0f;
					int EmptyID = pPlayerNetwork->SetLastServerDataBuffer().GetEmptyID();
					if (EmptyID != InvalidID) {
						//席が空いていて、自分のIDをもらえたので次へ
						pPlayerNetwork->SetLocalPlayerID(static_cast<PlayerID>(EmptyID));
						net.SetPhase(ClientPhase::Client_WaitReady);
					}
				}
				else {
					auto* DXLib_refParts = DXLib_ref::Instance();
					this->m_CannotConnectTimer += DXLib_refParts->GetDeltaTime();
					if (this->m_CannotConnectTimer > 1.0f) {
						this->m_CannotConnectTimer -= 1.0f;
						++this->m_NetWorkSelect;
						net.Dispose();
						if (this->m_NetWorkSelect < Player_num) {
							net.Init(false, this->m_Port + this->m_NetWorkSelect, this->m_IP);//ポートをずらして再試行
						}
						else {
							net.SetPhase(ClientPhase::NotConnect);	//満タンなので接続失敗
						}
					}
				}
				// 自身のデータを送信
				if (pPlayerNetwork->GetTickUpdateFlag()) {
					net.SendData(pPlayerNetwork->GetLocalPlayerData());
				}
				break;
			case ClientPhase::Client_WaitReady:
				if (pPlayerNetwork->GetLastServerDataBuffer().GetServerPhase() == ServerPhase::Ingame) {
					//マッチ済なので次へ
					net.SetPhase(ClientPhase::Ready);
				}
				// 自身のデータを送信
				if (pPlayerNetwork->GetTickUpdateFlag()) {
					net.SendData(pPlayerNetwork->GetLocalPlayerData());
				}
				break;
			case ClientPhase::Ready:
				// 自身のデータを送信
				if (pPlayerNetwork->GetTickUpdateFlag()) {
					net.SendData(pPlayerNetwork->GetLocalPlayerData());
				}
				break;
			default:
				break;
			}
		}
		//通信
		void NetWorkController::Update(const PlayerSendData& Players) noexcept {
			//ローカルデータ更新
			this->m_PlayerNet.UpdateLocalData(Players, IsInGame());
			if (this->m_IsServer) {
				//閲覧するプレイヤーIDを確定
				auto GetViewID = this->m_ServerCtrl.GetViewPlayerID();
				if (GetViewID != InvalidID) {
					this->m_PlayerNet.SetLocalPlayerID((PlayerID)GetViewID);
				}
				this->m_ServerCtrl.Update(&this->m_PlayerNet);
			}
			else {
				this->m_ClientCtrl.Update(&this->m_PlayerNet);
			}
			this->m_PlayerNet.Update();
			//PING計測
			if (this->m_PlayerNet.IsNeedUpdatePing()) {
				CalcPing(this->m_PlayerNet.GetPingTime());
			}
		}
	}
}
