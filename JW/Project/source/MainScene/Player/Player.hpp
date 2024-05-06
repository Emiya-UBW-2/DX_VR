#pragma once
#include	"../../Header.hpp"
#include "../../CommonScene/NetWork.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PlayerManager : public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		private:
			class PlayerControl {
			private:
				std::shared_ptr<ObjectBaseClass>				m_Chara{nullptr};
				std::array<std::shared_ptr<ObjectBaseClass>, 2>	m_Gun{};
				int												m_Score{0};							//スコア
				int												m_Kill{0};							//スコア
				int												m_Hit{0};							//スコア
				int												m_Shot{0};							//スコア
			public:
				PlayerControl(void) noexcept {
					this->m_Score = 0;
					this->m_Kill = 0;
					this->m_Hit = 0;
					this->m_Shot = 0;
				}
				~PlayerControl(void) noexcept {
					this->Dispose();
				}
			public:
				void		SetChara(const std::shared_ptr<ObjectBaseClass>& pChara) noexcept { this->m_Chara = pChara; }
				void		SetGun(int ID, const std::shared_ptr<ObjectBaseClass>& pGunPtr0) noexcept { this->m_Gun[ID] = pGunPtr0; }
				auto&		GetChara(void) noexcept { return this->m_Chara; }
				auto&		GetGun(int ID) noexcept { return this->m_Gun[ID]; }

				void		AddScore(int value) noexcept { this->m_Score += value; }
				void		SetScore(int value) noexcept { this->m_Score = value; }
				const auto&	GetScore(void) const noexcept { return this->m_Score; }

				void		AddKill(int value) noexcept { this->m_Kill += value; }
				void		SetKill(int value) noexcept { this->m_Kill = value; }
				const auto&	GetKill(void) const noexcept { return this->m_Kill; }

				void		AddHit(int value) noexcept { this->m_Hit += value; }
				void		SetHit(int value) noexcept { this->m_Hit = value; }
				const auto&	GetHit(void) const noexcept { return this->m_Hit; }

				void		AddShot(int value) noexcept { this->m_Shot += value; }
				void		SetShot(int value) noexcept { this->m_Shot = value; }
				const auto&	GetShot(void) const noexcept { return this->m_Shot; }
			public:
				void Init(void) noexcept {
					m_Chara = nullptr;
					for (auto& g : m_Gun) {
						g = nullptr;
					}
				}

				void Dispose(void) noexcept {
					m_Chara.reset();
					for (auto& g : m_Gun) {
						g.reset();
					}
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
