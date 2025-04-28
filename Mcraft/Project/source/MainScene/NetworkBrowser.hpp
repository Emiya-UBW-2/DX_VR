#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"
#include	"NetWork.hpp"

namespace FPS_n2 {
	enum class BrowserSequence : uint8_t {
		SelectMode,
		CheckPreset,
		SetNewData,
		Ready,
	};
	class NetWorkBrowser : public SingletonBase<NetWorkBrowser> {
	private:
		friend class SingletonBase<NetWorkBrowser>;
	private:
		struct NewSetting {
			IPDATA					IP{ 127,0,0,1 };
			int						UsePort{ 10850 };
		};
		class NewWorkSetting {
			std::vector<NewSetting>	m_NewWorkSetting;
		public:
			void Load(void) noexcept {
				SetOutApplicationLogValidFlag(false);
				FileStreamDX FileStream("data/NetWorkSetting.txt");
				while (true) {
					this->m_NewWorkSetting.emplace_back();
					this->m_NewWorkSetting.back().UsePort = std::clamp<int>(std::stoi(FileStreamDX::getright(FileStream.SeekLineAndGetStr())), 0, 50000);
					this->m_NewWorkSetting.back().IP.d1 = (unsigned char)std::clamp<int>(std::stoi(FileStreamDX::getright(FileStream.SeekLineAndGetStr())), 0, 255);
					this->m_NewWorkSetting.back().IP.d2 = (unsigned char)std::clamp<int>(std::stoi(FileStreamDX::getright(FileStream.SeekLineAndGetStr())), 0, 255);
					this->m_NewWorkSetting.back().IP.d3 = (unsigned char)std::clamp<int>(std::stoi(FileStreamDX::getright(FileStream.SeekLineAndGetStr())), 0, 255);
					this->m_NewWorkSetting.back().IP.d4 = (unsigned char)std::clamp<int>(std::stoi(FileStreamDX::getright(FileStream.SeekLineAndGetStr())), 0, 255);
					if (FileStream.ComeEof()) {
						break;
					}
				}
				SetOutApplicationLogValidFlag(true);
			}
			void Save(void) noexcept {
				std::ofstream outputfile("data/NetWorkSetting.txt");
				for (auto& netset : this->m_NewWorkSetting) {
					int ID = static_cast<int>(&netset - &this->m_NewWorkSetting.front());
					outputfile << "Setting" + std::to_string(ID) + "_Port=" + std::to_string(netset.UsePort) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP1=" + std::to_string(netset.IP.d1) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP2=" + std::to_string(netset.IP.d2) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP3=" + std::to_string(netset.IP.d3) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP4=" + std::to_string(netset.IP.d4) + "\n";
				}
				outputfile.close();
			}

			auto		GetSize(void) const noexcept { return static_cast<int>(this->m_NewWorkSetting.size()); }
			const auto& Get(int ID) const noexcept { return this->m_NewWorkSetting[static_cast<size_t>(ID)]; }
			void		AddBack(void) noexcept {
				this->m_NewWorkSetting.emplace_back();
				this->m_NewWorkSetting.back().UsePort = 10850;
				this->m_NewWorkSetting.back().IP.d1 = 127;
				this->m_NewWorkSetting.back().IP.d2 = 0;
				this->m_NewWorkSetting.back().IP.d3 = 0;
				this->m_NewWorkSetting.back().IP.d4 = 1;
			}
			void		SetBack(const NewSetting& per) noexcept { this->m_NewWorkSetting.back() = per; }
		};
	private:
		bool					m_IsServer{ true };
		bool					m_IsServerPlayer{ true };
		NewSetting				m_NetSetting;
		//共通
		BrowserSequence			m_Sequence{ BrowserSequence::SelectMode };
		bool					m_SeqFirst{ false };
		NewWorkSetting			m_NewWorkSettings;
	private:
		NetWorkBrowser(void) noexcept {
			this->m_Sequence = BrowserSequence::SelectMode;
		}
		NetWorkBrowser(const NetWorkBrowser&) = delete;
		NetWorkBrowser(NetWorkBrowser&&) = delete;
		NetWorkBrowser& operator=(const NetWorkBrowser&) = delete;
		NetWorkBrowser& operator=(NetWorkBrowser&&) = delete;

		virtual ~NetWorkBrowser(void) noexcept {}
	public:
		auto			IsDataReady(void) const noexcept { return this->m_Sequence == BrowserSequence::Ready; }
		const auto& IsServer(void) const noexcept { return this->m_IsServer; }
		const auto& GetServerPlayer(void) const noexcept { return this->m_IsServerPlayer; }
		const auto& GetNetSetting(void) const noexcept { return this->m_NetSetting; }
	public:
		void			BeClient(void) noexcept {
			this->m_IsServer = false;
		}
		void			BeServerPlayer(void) noexcept {
			this->m_IsServer = true;
			this->m_IsServerPlayer = true;
		}
		void			BeServer(void) noexcept {
			this->m_IsServer = true;
			this->m_IsServerPlayer = false;
		}
		void			ReadyConnect(const NewSetting& netset) noexcept {
			this->m_NetSetting = netset;
			this->m_Sequence = BrowserSequence::Ready;
		}
	public:
		void Draw(void) noexcept;
	};
}
