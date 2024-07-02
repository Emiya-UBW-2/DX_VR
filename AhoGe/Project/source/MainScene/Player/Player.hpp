#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PlayerManager : public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		private:
			class PlayerControl {
			private:
				std::shared_ptr<ObjectBaseClass>				m_Chara{nullptr};
				int												m_Score{0};							//ƒXƒRƒA
			public:
				PlayerControl(void) noexcept {
					this->m_Score = 0;
				}
				~PlayerControl(void) noexcept {
					this->Dispose();
				}
			public:
				void		SetChara(const std::shared_ptr<ObjectBaseClass>& pChara) noexcept { this->m_Chara = pChara; }
				auto&		GetChara(void) noexcept { return this->m_Chara; }

				void		AddScore(int value) noexcept { this->m_Score += value; }
				void		SetScore(int value) noexcept { this->m_Score = value; }
				const auto&	GetScore(void) const noexcept { return this->m_Score; }
			public:
				void Init(void) noexcept {
					m_Chara = nullptr;
				}

				void Dispose(void) noexcept {
					m_Chara.reset();
					m_Score = 0;
				}
			};
		private:
			std::vector<PlayerControl>	m_Player;
		public:
			auto&		GetPlayer(int ID) noexcept { return this->m_Player[ID]; }
		public:
			void Init(int playerNum) noexcept {
				m_Player.resize(playerNum);
			}
			void Dispose(void) noexcept {
				m_Player.clear();
			}
		};

	};
};
