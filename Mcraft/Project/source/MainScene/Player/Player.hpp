#pragma once
#pragma warning(disable:4464)
#include	"../../MainScene/NetWork.hpp"
#include	"CPU.hpp"

#include	"../Object/Character.hpp"
#include	"../Object/Vehicle.hpp"
#include	"../Object/Helicopter.hpp"
#include	"../Object/ItemObj.hpp"
#include	"../Object/Armer.hpp"

namespace FPS_n2 {
	namespace Player {
		class PlayerManager : public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		private:
			class PlayerControl {
				std::shared_ptr<Charas::CharacterObj>	m_Chara{ nullptr };
				std::shared_ptr<AIs::AIControl>			m_AI{ nullptr };
				std::array<std::pair<int, float>, 5>	m_Inventory{};
				int										m_Score{ 0 };							//スコア
				int										m_Kill{ 0 };							//スコア
				int										m_Hit{ 0 };							//スコア
				int										m_Shot{ 0 };							//スコア
			public:
				PlayerControl(void) noexcept {
					this->m_Score = 0;
					this->m_Kill = 0;
					this->m_Hit = 0;
					this->m_Shot = 0;
				}

				PlayerControl(const PlayerControl&) = delete;
				PlayerControl(PlayerControl&&) = delete;
				PlayerControl& operator=(const PlayerControl&) = delete;
				PlayerControl& operator=(PlayerControl&&) = delete;

				virtual ~PlayerControl(void) noexcept {
					Dispose();
				}
			public:
				void		SetChara(const std::shared_ptr<Charas::CharacterObj>& pChara) noexcept { this->m_Chara = pChara; }
				auto&		GetChara(void) noexcept { return this->m_Chara; }

				void		SetAI(const std::shared_ptr<AIs::AIControl>& pAI) noexcept { this->m_AI = pAI; }
				auto&		GetAI(void) noexcept { return this->m_AI; }

				auto& SetInventory(void) noexcept { return this->m_Inventory; }
				const auto& GetInventory(void) const noexcept { return this->m_Inventory; }
				const auto HasEmptyInventory(void) const noexcept {
					int count = 0;
					for (auto& i : this->m_Inventory) {
						if (i.first == InvalidID) {
							++count;
						}
					}
					return count;
				}
				bool AddInventory(int ID) noexcept {
					for (auto& i : this->m_Inventory) {
						if (i.first == InvalidID) {
							i.first = ID;
							i.second = 0.f;
							return true;
						}
					}
					return false;
				}
				bool SubInventory(int ID) noexcept {
					for (auto& i : this->m_Inventory) {
						if (i.first == ID) {
							i.first = InvalidID;
							return true;
						}
					}
					return false;
				}
				bool SubInventoryIndex(int index) noexcept {
					if (this->m_Inventory.at(index).first != InvalidID) {
						this->m_Inventory.at(index).first = InvalidID;
						return true;
					}
					return false;
				}

				void InitInventory() noexcept {
					for (auto& i : this->m_Inventory) {
						i.first = InvalidID;
					}
				}
				void		AddScore(int Score) noexcept { this->m_Score += Score; }
				void		SetScore(int Score) noexcept { this->m_Score = Score; }
				const auto& GetScore(void) const noexcept { return this->m_Score; }

				void		AddKill(int Kill) noexcept { this->m_Kill += Kill; }
				void		SetKill(int Kill) noexcept { this->m_Kill = Kill; }
				const auto& GetKill(void) const noexcept { return this->m_Kill; }

				void		AddHit(int Hit) noexcept { this->m_Hit += Hit; }
				void		SetHit(int Hit) noexcept { this->m_Hit = Hit; }
				const auto& GetHit(void) const noexcept { return this->m_Hit; }

				void		AddShot(int Shot) noexcept { this->m_Shot += Shot; }
				void		SetShot(int Shot) noexcept { this->m_Shot = Shot; }
				const auto& GetShot(void) const noexcept { return this->m_Shot; }
			public:
				void Init(void) noexcept {
					this->m_Chara = nullptr;
					this->m_AI = nullptr;
					InitInventory();
				}

				void Dispose(void) noexcept {
					if (this->m_Chara) {
						this->m_Chara.reset();
					}
					if (this->m_AI) {
						this->m_AI.reset();
					}
					this->m_Score = 0;
				}
			};
		private:
			std::vector<std::unique_ptr<PlayerControl>>	m_Player;
			int											m_PlayerNum{};

			PlayerID									m_WatchPlayer{};
	
			std::shared_ptr<Objects::VehicleObj>		m_VehicleObj;
			std::shared_ptr<Objects::HelicopterObj>		m_HelicopterObj;
			std::shared_ptr<Objects::ArmorObj>			m_ArmorObj;
		public:
			float										m_FindCount{};
		private:
			PlayerManager(void) noexcept {}
			PlayerManager(const PlayerManager&) = delete;
			PlayerManager(PlayerManager&&) = delete;
			PlayerManager& operator=(const PlayerManager&) = delete;
			PlayerManager& operator=(PlayerManager&&) = delete;

			virtual ~PlayerManager(void) noexcept {}
		public:
			const auto& GetWatchPlayerID(void) const noexcept { return this->m_WatchPlayer; }

			const auto& GetPlayerNum(void) const noexcept { return this->m_PlayerNum; }
			auto& GetPlayer(int ID) noexcept { return this->m_Player[ID]; }

			auto& GetWatchPlayer(void) noexcept { return this->m_Player[this->m_WatchPlayer]; }

			void SetWatchPlayerID(PlayerID playerID) noexcept { this->m_WatchPlayer = playerID; }

			void		SetVehicle(const std::shared_ptr<Objects::VehicleObj>& pObj) noexcept { this->m_VehicleObj = pObj; }
			auto&		GetVehicle(void) noexcept { return this->m_VehicleObj; }

			void		SetHelicopter(const std::shared_ptr<Objects::HelicopterObj>& pObj) noexcept { this->m_HelicopterObj = pObj; }
			auto&		GetHelicopter(void) noexcept { return this->m_HelicopterObj; }

			void		SetArmor(const std::shared_ptr<Objects::ArmorObj>& pObj) noexcept { this->m_ArmorObj = pObj; }
			auto&		GetArmor(void) noexcept { return this->m_ArmorObj; }
		public:
			void Init(int playerNum) noexcept {
				if (playerNum > 0) {
					this->m_PlayerNum = playerNum;
					this->m_Player.resize(static_cast<size_t>(this->m_PlayerNum));
					for (auto& player : this->m_Player) {
						player = std::make_unique<PlayerControl>();
					}
				}
			}
			void Dispose(void) noexcept {
				m_VehicleObj.reset();
				m_HelicopterObj.reset();
				m_ArmorObj.reset();
				for (auto& player : this->m_Player) {
					player.reset();
				}
				this->m_Player.clear();
			}
		};
	}
}
