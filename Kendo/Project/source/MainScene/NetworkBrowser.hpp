#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"
#include	"NetWork.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//通信
		enum class NetWorkSequence : uint8_t {
			Matching,
			MainGame,
		};
		class NetWorkController {
			bool					m_IsClient{true};
			float					m_Tick{2.f};
			PlayerSendData			m_LocalData;
			PlayerNetWork			m_PlayerNet;
			ServerControl			m_ServerCtrl;			//サーバー専用
			ClientControl			m_ClientCtrl;			//クライアント専用
			std::array<float, 60>	m_Pings{};
			float					m_Ping{ 0 };
			int						m_PingNow{ 0 };
			NetWorkSequence			m_Sequence{NetWorkSequence::Matching};
		public:
			NetWorkController(void) noexcept {}
			NetWorkController(const NetWorkController&) = delete;
			NetWorkController(NetWorkController&& o) = delete;
			NetWorkController& operator=(const NetWorkController&) = delete;
			NetWorkController& operator=(NetWorkController&& o) = delete;

			~NetWorkController(void) noexcept {}
		private:
			void			CalcPing(LONGLONG microsec) noexcept {
				this->m_Pings.at(m_PingNow) = std::max(0.f, static_cast<float>(microsec) / 1000.f - (this->m_Tick * 1000.f / Frame_Rate));//ティック分引く
				++m_PingNow %= static_cast<int>(this->m_Pings.size());
				m_Ping = 0.f;
				for (auto& p : this->m_Pings) {
					m_Ping += p;
				}
				m_Ping /= static_cast<float>(this->m_Pings.size());
			}
		public:
			auto			IsInGame(void) const noexcept { return this->m_Sequence == NetWorkSequence::MainGame; }
			PlayerNetData 	GetLerpServerPlayerData(PlayerID ID) noexcept { return this->m_PlayerNet.GetLerpServerPlayerData(ID); }
			const auto&		GetClient(void) const noexcept { return this->m_IsClient; }
			const auto&		GetPing(void) const noexcept { return this->m_Ping; }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_PlayerNet.GetMyLocalPlayerID(); }

			auto&			SetLocalData(void) noexcept { return this->m_LocalData; }
		public:
			void Init(bool IsClient, int Port, const IPDATA& ip) noexcept;
			void Update(void) noexcept;
			void Dispose(void) noexcept {
				m_ServerCtrl.Dispose();
				m_ClientCtrl.Dispose();
			}
		};

		enum class BrowserSequence : uint8_t {
			SelMode,
			CheckPreset,
			SetNewData,
			Ready,
		};
		class NetWorkBrowser {
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
				void		AddBack(void) noexcept {
					m_NewWorkSetting.resize(this->m_NewWorkSetting.size() + 1);
					m_NewWorkSetting.back().UsePort = 10850;
					m_NewWorkSetting.back().IP.d1 = 127;
					m_NewWorkSetting.back().IP.d2 = 0;
					m_NewWorkSetting.back().IP.d3 = 0;
					m_NewWorkSetting.back().IP.d4 = 1;
				}
				void		SetBack(const NewSetting& per) noexcept { this->m_NewWorkSetting.back() = per; }
			};
		private:
			bool					m_IsClient{true};
			NewSetting				m_NetSetting;
			//共通
			BrowserSequence			m_Sequence{BrowserSequence::SelMode};
			bool					m_SeqFirst{false};
			NewWorkSetting			m_NewWorkSettings;
		public:
			NetWorkBrowser(void) noexcept {}
			NetWorkBrowser(const NetWorkBrowser&) = delete;
			NetWorkBrowser(NetWorkBrowser&& o) = delete;
			NetWorkBrowser& operator=(const NetWorkBrowser&) = delete;
			NetWorkBrowser& operator=(NetWorkBrowser&& o) = delete;

			~NetWorkBrowser(void) noexcept {}
		public:
			auto			IsDataReady(void) const noexcept { return this->m_Sequence == BrowserSequence::Ready; }
			const auto&		GetClient(void) const noexcept { return this->m_IsClient; }
			const auto&		GetNetSetting(void) const noexcept { return this->m_NetSetting; }
		public:
			void Init(void) noexcept;
			void Draw(void) noexcept;
		};
	};
};
