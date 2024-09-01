#pragma once
#pragma warning(disable:4464)
#include "../../MainScene/NetWork.hpp"
#include	"CPU.hpp"

namespace FPS_n2 {
	namespace Player {
		class PlayerManager : public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		public:
			class PlayerControl {
			private:
				std::shared_ptr<ObjectBaseClass>				m_Chara{ nullptr };
				std::shared_ptr<AIControl>						m_AI{ nullptr };
				int												m_Score{ 0 };							//ƒXƒRƒA
			public:
				PlayerControl(void) noexcept {
					this->m_Score = 0;
				}

				PlayerControl(const PlayerControl&) = delete;
				PlayerControl(PlayerControl&& o) = delete;
				PlayerControl& operator=(const PlayerControl&) = delete;
				PlayerControl& operator=(PlayerControl&& o) = delete;

				~PlayerControl(void) noexcept {
					this->Dispose();
				}
			public:
				void		SetChara(const std::shared_ptr<ObjectBaseClass>& pChara) noexcept { this->m_Chara = pChara; }
				auto& GetChara(void) noexcept { return this->m_Chara; }

				void		SetAI(const std::shared_ptr<AIControl>& pAI) noexcept { this->m_AI = pAI; }
				auto& GetAI(void) noexcept { return this->m_AI; }

				void		AddScore(int value) noexcept { this->m_Score += value; }
				void		SetScore(int value) noexcept { this->m_Score = value; }
				const auto& GetScore(void) const noexcept { return this->m_Score; }
			public:
				void Init(void) noexcept {
					m_Chara = nullptr;
					this->m_AI = nullptr;
				}

				void Dispose(void) noexcept {
					if (this->m_Chara) {
						this->m_Chara.reset();
					}
					if (this->m_AI) {
						this->m_AI->Dispose();
						this->m_AI.reset();
					}
					m_Score = 0;
				}
			};
		private:
			std::vector<std::unique_ptr<PlayerControl>>	m_Player;
			int											m_PlayerNum{};

			std::vector<std::unique_ptr<PlayerControl>>	m_NPC;
		private:
			PlayerManager(void) noexcept {}
			PlayerManager(const PlayerManager&) = delete;
			PlayerManager(PlayerManager&& o) = delete;
			PlayerManager& operator=(const PlayerManager&) = delete;
			PlayerManager& operator=(PlayerManager&& o) = delete;

			virtual ~PlayerManager(void) noexcept {}
		public:
			const auto& GetPlayerNum(void) const noexcept { return this->m_PlayerNum; }
			auto& GetPlayer(int ID) noexcept { return this->m_Player[static_cast<size_t>(ID)]; }
		public:
			auto GetNPCNum(void) const noexcept { return this->m_NPC.size(); }
			auto& GetNPC(int ID) noexcept { return this->m_NPC[static_cast<size_t>(ID)]; }
		public:
			void Init(int playerNum) noexcept {
				m_PlayerNum = playerNum;
				m_Player.resize(static_cast<size_t>(playerNum));
				for (auto& p : m_Player) {
					p = std::make_unique<PlayerControl>();
				}
			}
			void Dispose(void) noexcept {
				for (auto& p : m_Player) {
					p.reset();
				}
				m_Player.clear();

				DisposeNPC();
			}

			const std::unique_ptr<PlayerControl>& AddNPC() noexcept {
				m_NPC.emplace_back(std::make_unique<PlayerControl>());
				return m_NPC.back();
			}
			void DisposeNPC(void) noexcept {
				for (auto& p : m_NPC) {
					p.reset();
				}
				m_NPC.clear();
			}
		};
	}
};
