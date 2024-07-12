#pragma once
#include	"../../Header.hpp"
#include	"../Object/Character.hpp"
#include	"CPU.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PlayerManager : public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		private:
			class PlayerControl {
			private:
				std::shared_ptr<CharacterObject>				m_Chara{nullptr};
				std::shared_ptr<AIControl>						m_AI{ nullptr };
			public:
				PlayerControl(void) {}
				PlayerControl(const PlayerControl&) = delete;
				PlayerControl(PlayerControl&& o) = delete;
				PlayerControl& operator=(const PlayerControl&) = delete;
				PlayerControl& operator=(PlayerControl&& o) = delete;

				~PlayerControl(void) noexcept {
					this->Dispose();
				}
			public:
				void		SetChara(const std::shared_ptr<CharacterObject>& pChara) noexcept { this->m_Chara = pChara; }
				auto&		GetChara(void) noexcept { return this->m_Chara; }

				void		SetAI(const std::shared_ptr<AIControl>& pAI) noexcept { this->m_AI = pAI; }
				auto&		GetAI(void) noexcept { return this->m_AI; }
			public:
				void Init(void) noexcept {
					this->m_Chara = nullptr;
					this->m_AI = nullptr;
				}
				void Dispose(void) noexcept {
					if (this->m_Chara) {
						this->m_Chara.reset();
					}
					if (this->m_AI) {
						this->m_AI.reset();
					}
				}
			};
		private:
			std::vector<std::unique_ptr<PlayerControl>>	m_Player;
			int											m_PlayerNum{ 0 };
		public:
			auto&		GetPlayer(PlayerID ID) noexcept { return this->m_Player.at(static_cast<size_t>(ID)); }
			const auto&	GetPlayerNum(void) const noexcept { return this->m_PlayerNum; }
		private:
			PlayerManager(void) {}
			PlayerManager(const PlayerManager&) = delete;
			PlayerManager(PlayerManager&& o) = delete;
			PlayerManager& operator=(const PlayerManager&) = delete;
			PlayerManager& operator=(PlayerManager&& o) = delete;
		public:
			void Init(int playerNum) noexcept {
				this->m_Player.clear();
				for (int i = 0; i < playerNum; i++) {
					this->m_Player.emplace_back(std::make_unique<PlayerControl>());
				}
				this->m_PlayerNum = playerNum;
			}
			void Dispose(void) noexcept {
				for (auto& p : this->m_Player) {
					p->Dispose();
					p.reset();
				}
				this->m_Player.clear();
			}
		};

	};
};
