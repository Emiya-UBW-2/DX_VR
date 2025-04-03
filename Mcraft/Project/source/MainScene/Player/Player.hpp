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
					this->m_Chara = nullptr;
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
					this->m_Score = 0;
				}
			};
		private:
			std::vector<std::unique_ptr<PlayerControl>>	m_Player;
			int											m_PlayerNum{};

			PlayerID									m_WatchPlayer{};
		private:
			PlayerManager(void) noexcept {}
			PlayerManager(const PlayerManager&) = delete;
			PlayerManager(PlayerManager&& o) = delete;
			PlayerManager& operator=(const PlayerManager&) = delete;
			PlayerManager& operator=(PlayerManager&& o) = delete;

			virtual ~PlayerManager(void) noexcept {}
		public:
			const auto& GetWatchPlayer(void) const noexcept { return this->m_WatchPlayer; }

			const auto& GetPlayerNum(void) const noexcept { return this->m_PlayerNum; }
			auto& GetPlayer(int ID) noexcept { return this->m_Player[ID]; }

			void SetWatchPlayer(PlayerID playerID) noexcept { this->m_WatchPlayer = playerID; }
		public:
			void Init(int playerNum) noexcept {
				if (playerNum > 0) {
					this->m_PlayerNum = playerNum;
					this->m_Player.resize(static_cast<size_t>(this->m_PlayerNum));
					for (auto& p : this->m_Player) {
						p = std::make_unique<PlayerControl>();
					}
				}
			}
			void Dispose(void) noexcept {
				for (auto& p : this->m_Player) {
					p.reset();
				}
				this->m_Player.clear();
			}
		};
	}
};
