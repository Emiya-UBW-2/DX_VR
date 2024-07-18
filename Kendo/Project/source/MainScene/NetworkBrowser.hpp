#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"
#include	"NetWork.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//通信
		enum class SequenceEnum {
			SelMode,
			CheckPreset,
			Set_Port,
			SetIP,
			SetTick,
			Matching,
			MainGame,
		};

		struct NewSetting {
			IPDATA					IP{ 127,0,0,1 };
			int						UsePort{ 10850 };
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
					m_NewWorkSetting.back().IP.d1 = (unsigned char)std::clamp<int>(getparams::_int(mdata), 0, 255);
					m_NewWorkSetting.back().IP.d2 = (unsigned char)std::clamp<int>(getparams::_int(mdata), 0, 255);
					m_NewWorkSetting.back().IP.d3 = (unsigned char)std::clamp<int>(getparams::_int(mdata), 0, 255);
					m_NewWorkSetting.back().IP.d4 = (unsigned char)std::clamp<int>(getparams::_int(mdata), 0, 255);
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
					int ID = static_cast<int>(&n - &this->m_NewWorkSetting.front());
					outputfile << "Setting" + std::to_string(ID) + "_Port=" + std::to_string(n.UsePort) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP1=" + std::to_string(n.IP.d1) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP2=" + std::to_string(n.IP.d2) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP3=" + std::to_string(n.IP.d3) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP4=" + std::to_string(n.IP.d4) + "\n";
				}
				outputfile.close();
			}

			auto		GetSize(void) const noexcept { return static_cast<int>(m_NewWorkSetting.size()); }
			const auto& Get(int ID) const noexcept { return this->m_NewWorkSetting[static_cast<size_t>(ID)]; }
			auto		Add(void) noexcept {
				m_NewWorkSetting.resize(this->m_NewWorkSetting.size() + 1);
				m_NewWorkSetting.back().UsePort = 10850;
				m_NewWorkSetting.back().IP.d1 = 127;
				m_NewWorkSetting.back().IP.d2 = 0;
				m_NewWorkSetting.back().IP.d3 = 0;
				m_NewWorkSetting.back().IP.d4 = 1;
				return static_cast<int>(m_NewWorkSetting.size()) - 1;;
			}
			void		Set(int ID, const NewSetting& per) noexcept { this->m_NewWorkSetting[static_cast<size_t>(ID)] = per; }
		};

		class NetWorkBrowser {
			PlayerNetWork			m_PlayerNet;
			//サーバー専用
			ServerControl			m_ServerCtrl;
			//クライアント専用
			ClientControl			m_ClientCtrl;
			//共通
			bool					m_IsClient{true};
			SequenceEnum			m_Sequence{SequenceEnum::SelMode};
			bool					m_SeqFirst{false};
			float					m_Tick{1.f};
			NewWorkSetting			m_NewWorkSetting;
			int						m_NewWorkSelect{0};
			NewSetting				m_NewSetting;
			NetTime					m_ClientFrame{0};
			NetTime					m_PrevFrame{0};
			float					m_Ping{0.f};
			switchs					m_LeftClick;
			float											m_TickCnt{0.f};
			float											m_TickRate{0.f};
		public:
			NetWorkBrowser(void) noexcept {}
			NetWorkBrowser(const NetWorkBrowser&) = delete;
			NetWorkBrowser(NetWorkBrowser&& o) = delete;
			NetWorkBrowser& operator=(const NetWorkBrowser&) = delete;
			NetWorkBrowser& operator=(NetWorkBrowser&& o) = delete;

			~NetWorkBrowser(void) noexcept {}
		public:
			const auto&		GetPing(void) const noexcept { return  this->m_Ping; }
			const auto&		GetClient(void) const noexcept { return this->m_IsClient; }
			const auto&		GetSequence(void) const noexcept { return this->m_Sequence; }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_PlayerNet.GetMyPlayerID(); }
		public:
			auto			GetNowServerPlayerData(PlayerID ID, bool isYradReset) noexcept {
				auto tmp = this->m_PlayerNet.GetNowServerPlayerData(ID, isYradReset);
				if ((ID == this->m_PlayerNet.GetMyPlayerID()) && this->m_PlayerNet.CanGetRecvData(ID)) {
					this->m_Ping = static_cast<float>(this->m_ClientFrame - tmp.GetFrame()) / 1000.f;
				}
				return tmp;
			}
		private:
			bool			UpdateTick() {
				auto* DrawParts = DXDraw::Instance();
				this->m_TickCnt += 60.f / DrawParts->GetFps();
				if (this->m_TickCnt > this->m_TickRate) {
					this->m_TickCnt -= this->m_TickRate;
					return true;
				}
				return false;
			}
		public:
			void Init(void) noexcept {
				m_NewWorkSetting.Load();
				m_NewWorkSelect = 0;
				this->m_ClientFrame = 0;
			}
			void FirstExecute(const PlayerNetData& pdata) noexcept {
				m_PlayerNet.SetData(pdata, this->m_ClientFrame);
				if (this->m_Sequence == SequenceEnum::Matching) {
					if (m_SeqFirst) {
						m_PlayerNet.Int();
						this->m_TickCnt = 0.f;
						this->m_TickRate = this->m_Tick;
						if (this->m_IsClient) {
							m_ClientCtrl.Init(this->m_NewSetting.UsePort, this->m_NewSetting.IP);
						}
						else {
							m_ServerCtrl.Init(this->m_NewSetting.UsePort);
						}
					}
					if (this->m_IsClient) {
						if (m_ClientCtrl.CanGetMyID()) {
							m_PlayerNet.SetPlayerID(m_ClientCtrl.GetMyID());
						}
					}
					else {
						m_PlayerNet.SetPlayerID((PlayerID)0);
					}
				}
				if (this->m_Sequence >= SequenceEnum::Matching) {
					bool IsUpdateTick = UpdateTick();
					if (this->m_IsClient) {
						if (this->m_ClientCtrl.Execute(&m_PlayerNet.SetPlayerData(), IsUpdateTick)) {
							this->m_Sequence = SequenceEnum::MainGame;
						}
						if (this->m_ClientCtrl.IsServerDataUpdated()) {
							m_PlayerNet.UpdateDataCommon(this->m_ClientCtrl.GetServerData());
						}
					}
					else {
						if (this->m_ServerCtrl.Execute(&m_PlayerNet.SetPlayerData(), IsUpdateTick)) {
							this->m_Sequence = SequenceEnum::MainGame;
						}
						if (IsUpdateTick) {
							m_PlayerNet.UpdateDataCommon(this->m_ServerCtrl.GetServerData());
						}
					}
				}
			}
			void LateExecute(void) noexcept {
				LONGLONG NowFrame = GetNowHiPerformanceCount();
				if (this->m_Sequence == SequenceEnum::MainGame) {
					this->m_ClientFrame += NowFrame - this->m_PrevFrame;
				}
				else {
					this->m_ClientFrame = 0;
				}
				this->m_PrevFrame = NowFrame;
			}
			void Draw(void) noexcept ;
			void Dispose(void) noexcept {
				m_ServerCtrl.Dispose();
				m_ClientCtrl.Dispose();
			}
		};
	};
};
