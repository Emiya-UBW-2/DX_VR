#pragma once
#pragma warning(disable:4464)
#include	"../../MainScene/NetWork.hpp"
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
				std::shared_ptr<AIControl>						m_AI{ nullptr };
				int												m_Score{ 0 };							//スコア
				int												m_MaxScore{ 0 };							//スコア
			public:
				PlayerControl(void) noexcept {
					Init();
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

				void		AddScore(int value) noexcept {
					this->m_Score += value;
					this->m_MaxScore = std::max(this->m_MaxScore, this->m_Score);
				}
				void		SetScore(int value) noexcept {
					this->m_Score = value;
					this->m_MaxScore = std::max(this->m_MaxScore, this->m_Score);
				}
				const auto& GetScore(void) const noexcept { return this->m_Score; }
				const auto& GetMaxScore(void) const noexcept { return this->m_MaxScore; }
			public:
				void Init(void) noexcept {
					this->m_Chara = nullptr;
					this->m_AI = nullptr;
					this->m_Score = 0;
					this->m_MaxScore = 0;
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
			bool										m_ScoreChangeFlag{ false };
			PlayerID									m_LastAddScore{ InvalidID };
			HitType										m_LastHitType{ HitType::Head };
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
			}
		public:
			void AddScore(PlayerID ID, HitType hitPos) {
				m_ScoreChangeFlag = true;
				GetPlayer(ID)->AddScore(1);
				m_LastAddScore = ID;
				m_LastHitType = hitPos;
			}
			void ResetScore() {
				m_ScoreChangeFlag = false;
				for (auto& p : m_Player) {
					p->SetScore(0);
				}
				m_LastAddScore = InvalidID;
				m_LastHitType = HitType::Head;
			}
			const auto PutAddScoreFlag() {
				auto ret = m_ScoreChangeFlag;
				m_ScoreChangeFlag = false;
				return ret;
			}
			const auto& GetWinPlayer() const noexcept { return this->m_LastAddScore; }
			const auto& GetWinHitType() const noexcept { return this->m_LastHitType; }
		};
	}
};
