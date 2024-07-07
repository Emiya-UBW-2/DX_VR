#pragma once
#include	"../../Header.hpp"
#include	"../Object/Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PlayerManager : public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		private:
			class PlayerControl {
			private:
				std::shared_ptr<CharacterObject>				m_Chara{nullptr};
			public:
				PlayerControl(void) noexcept {
				}
				~PlayerControl(void) noexcept {
					this->Dispose();
				}
			public:
				void		SetChara(const std::shared_ptr<CharacterObject>& pChara) noexcept { this->m_Chara = pChara; }
				auto&		GetChara(void) noexcept { return this->m_Chara; }
			public:
				void Init(void) noexcept {
					m_Chara = nullptr;
				}
				void Dispose(void) noexcept {
					m_Chara.reset();
				}
			};
		private:
			std::vector<PlayerControl>	m_Player;
		public:
			auto&		GetPlayer(PlayerID ID) noexcept { return this->m_Player[(int)ID]; }
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
