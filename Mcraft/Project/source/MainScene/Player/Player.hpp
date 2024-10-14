#pragma once
#pragma warning(disable:4464)
#include "../../MainScene/NetWork.hpp"
#include	"CPU.hpp"

namespace FPS_n2 {
	namespace Player {
		class PlayerManager : public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		private:
			class PlayerControl {
			private:
				std::shared_ptr<ObjectBaseClass>				m_Chara{ nullptr };
				std::shared_ptr<Sceneclass::AIControl>			m_AI{ nullptr };
				int												m_Score{ 0 };							//スコア
				int												m_Kill{ 0 };							//スコア
				int												m_Hit{ 0 };							//スコア
				int												m_Shot{ 0 };							//スコア
			public:
				PlayerControl(void) noexcept {
					this->m_Score = 0;
					this->m_Kill = 0;
					this->m_Hit = 0;
					this->m_Shot = 0;
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

				void		SetAI(const std::shared_ptr<Sceneclass::AIControl>& pAI) noexcept { this->m_AI = pAI; }
				auto& GetAI(void) noexcept { return this->m_AI; }

				void		AddScore(int value) noexcept { this->m_Score += value; }
				void		SetScore(int value) noexcept { this->m_Score = value; }
				const auto& GetScore(void) const noexcept { return this->m_Score; }

				void		AddKill(int value) noexcept { this->m_Kill += value; }
				void		SetKill(int value) noexcept { this->m_Kill = value; }
				const auto& GetKill(void) const noexcept { return this->m_Kill; }

				void		AddHit(int value) noexcept { this->m_Hit += value; }
				void		SetHit(int value) noexcept { this->m_Hit = value; }
				const auto& GetHit(void) const noexcept { return this->m_Hit; }

				void		AddShot(int value) noexcept { this->m_Shot += value; }
				void		SetShot(int value) noexcept { this->m_Shot = value; }
				const auto& GetShot(void) const noexcept { return this->m_Shot; }
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

			std::vector<std::shared_ptr<ObjectBaseClass>>	m_Gun{};
			int												m_GunNum{};
		private:
			PlayerManager(void) noexcept {}
			PlayerManager(const PlayerManager&) = delete;
			PlayerManager(PlayerManager&& o) = delete;
			PlayerManager& operator=(const PlayerManager&) = delete;
			PlayerManager& operator=(PlayerManager&& o) = delete;

			virtual ~PlayerManager(void) noexcept {}
		public:
			const auto& GetPlayerNum(void) const noexcept { return this->m_PlayerNum; }
			auto& GetPlayer(int ID) noexcept { return this->m_Player[ID]; }

			const auto& GetGunNum(void) const noexcept { return this->m_GunNum; }
			void		SetGun(int ID, const std::shared_ptr<ObjectBaseClass>& pGunPtr0) noexcept { this->m_Gun[ID] = pGunPtr0; }
			auto& GetGun(int ID) noexcept { return this->m_Gun[ID]; }
		public:
			void Init(int playerNum, int gunNum) noexcept {
				if (playerNum > 0) {
					m_PlayerNum = playerNum;
					m_Player.resize(static_cast<size_t>(m_PlayerNum));
					for (auto& p : m_Player) {
						p = std::make_unique<PlayerControl>();
					}
				}
				if (gunNum > 0) {
					m_GunNum = gunNum;
					m_Gun.resize(static_cast<size_t>(m_GunNum));
				}
			}
			void Dispose(void) noexcept {
				for (auto& p : m_Player) {
					p.reset();
				}
				m_Player.clear();
				for (auto& p : m_Gun) {
					p.reset();
				}
				m_Gun.clear();
			}
		};
	}
};
