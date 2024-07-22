#pragma warning(disable:4464)
#include "NetWork.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//クライアントが送信するデータ
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
		//通信
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

			if (this->m_LastServerFrame <= Data.ServerFrame && Data.ServerFrame <= (this->m_LastServerFrame + 60)) {//入力されたデータが以前もらったものから0~60F後だったら
				this->m_LastServerFrame = Data.ServerFrame;
				this->m_LeapFrame = 0;
				this->m_PrevServerData = this->m_ServerDataCommon;
				this->m_ServerDataCommon = Data;
				auto Total = static_cast<int>(this->m_ServerDataCommon.ServerFrame) - static_cast<int>(this->m_PrevServerData.ServerFrame);
				if (Total <= 0) { Total = 20; }
				this->m_LeapFrameMax = static_cast<size_t>(Total);
				return this->m_LocalData.GetClientTime() - this->m_ServerDataCommon.PlayerData[this->m_LocalData.GetID()].GetClientTime();//もらったデータと自分のフレームとの差異がpingになる
			}
			else {
				this->m_LeapFrame = std::min<size_t>(this->m_LeapFrame + 1, this->m_LeapFrameMax);
			}
			return MAXLONGLONG;
		}
		//サーバー専用
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
					if (index == 0) { continue; }//サーバープレイヤーは絶対0を使うので
				}
				n.m_NetWork.Init(true, pPort + i); i++;
				n.m_Phase = ClientPhase::WaitConnect;
			}
		}
		bool ServerControl::Execute(const PlayerNetData& MyLocalPlayerData, bool IsUpdateTick) noexcept {
			//
			auto FlipPlayerData = [&](const PlayerNetData& value) { this->m_ServerData.PlayerData[value.GetID()] = value; };
			FlipPlayerData(MyLocalPlayerData);// サーバープレイヤーののプレイヤーデータ
			bool IsAllReady = AllReady();
			if (!IsAllReady) {
				this->m_ServerData.ServerFrame = 0;
			}
			else {
				this->m_ServerData.ServerFrame++;	// サーバーフレーム更新
				this->m_ServerData.SetInGame();		// インゲームです
			}
			for (auto& n : this->m_Net) {
				int index = static_cast<int>(&n - &this->m_Net.front());
				if (this->m_IsServerPlay) {
					if (index == 0) { continue; }//サーバープレイヤーは絶対0を使うので
				}
				PlayerNetData tmpData;
				int recvRet = -1;
				auto IsDataUpdated = n.m_NetWork.RecvData(&tmpData, &recvRet, true);
				if (IsDataUpdated) {
					if (recvRet == -2) {
						PlayerNetData tmpDatas[30];
						//大きめのバッファーを用意してその中から探す
						n.m_NetWork.RecvData(&tmpDatas, &recvRet, false);
						NetTime serverframe = 0;
						int watch = -1;
						for (int i = 0; i < 30; i++) {
							if (tmpDatas[i].IsCheckSum()) {//todo:鯖版のチェックサム
								if (serverframe < tmpDatas[i].GetClientTime()) {
									serverframe = tmpDatas[i].GetClientTime();							//一番サバフレの大きな奴を探せ
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
				case ClientPhase::WaitConnect:						// 無差別受付
					if (IsDataUpdated) {							// 該当ソケットにクライアントからなにか受信したら
						n.m_Phase = ClientPhase::GetNumber;
					}
					break;
				case ClientPhase::GetNumber://プレイヤー受付
					if (IsDataUpdated) {
						if (tmpData.GetFlag(NetAttribute::IsActive)) {				// ID取れたと識別出来たら
							n.m_Phase = ClientPhase::Ready;
						}
					}
					this->m_ServerData.m_PlayerID = static_cast<PlayerID>(index);//ID受付中ですわ
					IsSendData = true;
					break;
				case ClientPhase::Ready:						//他プレイヤーの揃い待ち
					if (IsDataUpdated) {						// クライアントから受信したら
						if (tmpData.IsCheckSum()) {				// チェックサムののち
							FlipPlayerData(tmpData);			// 更新
						}
					}
					this->m_ServerData.m_PlayerID = -1;								//ID受付終了に指定を変更
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
					n.m_NetWork.ReturnData(this->m_ServerData);		// クライアントにIDを送る
				}
			}
			return IsAllReady;
		}
		void ServerControl::Dispose(void) noexcept {
			for (auto& n : this->m_Net) {
				n.m_NetWork.Dispose();
			}
		}
		//クライアント専用
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
				if (recvRet == -2) {//足りない場合
					//大きめのバッファーを用意してその中から探す
					this->m_Net.m_NetWork.RecvData(&m_BufferData, &recvRet, false);
					size_t serverframe = 0;
					int watch = -1;
					for (int i = 0; i < 10; i++) {
						if (m_BufferData[i].IsCheckSum()) {
							if (serverframe < m_BufferData[i].ServerFrame) {
								serverframe = m_BufferData[i].ServerFrame;							//一番サバフレの大きな奴を探せ
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
						//席が空いていて、自分のIDをもらえたので次へ
						m_MyID = this->m_ServerData.m_PlayerID;
						this->m_Net.m_Phase = ClientPhase::GetNumber;
					}
					else if (this->m_ServerData.IsInGame()) {
						//要求したサーバーがインゲーム中でした
						this->m_Net.m_NetWork.Dispose();
						this->m_Net.m_Phase = ClientPhase::Error_ServerInGame;	//マッチ済なので接続失敗
					}
				}
				//もらえてない
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
							this->m_Net.m_Phase = ClientPhase::Error_CannotConnect;	//満タンなので接続失敗
						}
					}
				}
				break;
			case ClientPhase::GetNumber:
				IsSendData = true;
				if (this->m_ServerData.IsInGame()) {
					//マッチ済なので次へ
					this->m_Net.m_Phase = ClientPhase::Ready;
				}
				break;
			case ClientPhase::Ready:
				IsSendData = true;
				break;
			case ClientPhase::Error_CannotConnect:
			case ClientPhase::Error_ServerInGame:
			default:
				//エラー
				break;
			}
			if (IsSendData && IsUpdateTick) {
				this->m_Net.m_NetWork.SendData(MyLocalPlayerData);		// 自身のデータを送信
			}
			return this->m_Net.IsReady();
		}
		void ClientControl::Dispose(void) noexcept {
			this->m_Net.m_NetWork.Dispose();
		}
	};
};
