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
				PlayerControl(void) noexcept {
				}
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
					m_Chara = nullptr;
					m_AI = nullptr;
				}
				void Dispose(void) noexcept {
					m_Chara.reset();
					m_AI.reset();
				}
			};
		private:
			std::vector<PlayerControl>	m_Player;
		public:
			auto&		GetPlayer(PlayerID ID) noexcept { return this->m_Player.at((int)ID); }
		public:
			void Init(int playerNum) noexcept {
				m_Player.resize(playerNum);
			}
			void Dispose(void) noexcept {
				for (auto& p : m_Player) {
					p.Dispose();
				}
				m_Player.clear();
			}
		};

	};
};
