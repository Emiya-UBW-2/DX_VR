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
		enum class SkillType {
			Runner,			//・1ゲージぶんだけ走れるようにする（次プレイまで回復しない）
			Guts,			//・即死しそうな時に1度だけ踏みとどまれる
			AP_AMMO,		//・弾がアーマーを貫通する代わりにダメージが落ちるAP弾に
			ItemEater,		//・Fキーを押すと3m以内のアイテムを吸い寄せる
			PeaceMaker,		//・対面している時だけ敵が撃ってこなくなる
			TeiziTaisha,	//・帰還要請と援護射撃までのタイマーを早める
			Adrenaline,		//・物を拾って5秒だけ重さデバフを無視できる
			DamageCut,		//・アーマーを含め被ダメージを5% カット
			Max,
		};

		class SkillInfo {
		public:
			SkillType	m_SkillType{};
			int			m_Level{ 1 };
		};

		class SkillList : public SingletonBase<SkillList> {
		private:
			friend class SingletonBase<SkillList>;
		private:
			std::vector<SkillInfo>						m_SkillInfo;
		public:
			const auto	GetSkilLevel(SkillType type) const noexcept {
				for (auto& s : m_SkillInfo) {
					if (s.m_SkillType == type) {
						return s.m_Level;
					}
				}
				return 0;
			}

			const auto GetSkillValue(SkillType type, int Level) const noexcept {
				float Num = 0.f;
				switch (type) {
				case Player::SkillType::Runner:
					switch (Level) {
					case 1:
						Num = 5.f;
						break;
					case 2:
						Num = 10.f;
						break;
					case 3:
						Num = 15.f;
						break;
					default:
						break;
					}
					break;
				case Player::SkillType::Guts:
					switch (Level) {
					case 1:
						Num = 30.f;
						break;
					case 2:
						Num = 20.f;
						break;
					case 3:
						Num = 10.f;
						break;
					default:
						break;
					}
					break;
				case Player::SkillType::AP_AMMO:
					switch (Level) {
					case 1:
						Num = 80.f;
						break;
					case 2:
						Num = 60.f;
						break;
					case 3:
						Num = 40.f;
						break;
					default:
						break;
					}
					break;
				case Player::SkillType::ItemEater:
					switch (Level) {
					case 1:
						Num = 4.f;
						break;
					case 2:
						Num = 5.f;
						break;
					case 3:
						Num = 6.f;
						break;
					default:
						break;
					}
					break;
				case Player::SkillType::PeaceMaker:
					switch (Level) {
					case 1:
						Num = 2.f;
						break;
					case 2:
						Num = 5.f;
						break;
					case 3:
						Num = 8.f;
						break;
					default:
						break;
					}
					break;
				case Player::SkillType::TeiziTaisha:
					switch (Level) {
					case 1:
						Num = 10.f;
						break;
					case 2:
						Num = 25.f;
						break;
					case 3:
						Num = 50.f;
						break;
					default:
						break;
					}
					break;
				case Player::SkillType::Adrenaline:
					switch (Level) {
					case 1:
						Num = 1.f;
						break;
					case 2:
						Num = 2.f;
						break;
					case 3:
						Num = 3.f;
						break;
					default:
						break;
					}
					break;
				case Player::SkillType::DamageCut:
					switch (Level) {
					case 1:
						Num = 5.f;
						break;
					case 2:
						Num = 15.f;
						break;
					case 3:
						Num = 20.f;
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
				return Num;
			}

			const auto GetSkillValueNow(SkillType type) const noexcept { return GetSkillValue(type, GetSkilLevel(type)); }

			const auto GetSkillValueNext(SkillType type) const noexcept { return GetSkillValue(type, std::clamp(GetSkilLevel(type) + 1, 1, 3)); }
		private:
			SkillList() noexcept {
				m_SkillInfo.clear();
				for (int loop = 0; loop < static_cast<int>(Player::SkillType::Max); ++loop) {
					int LV = static_cast<int>(SaveData::Instance()->GetParam("skill" + std::to_string(loop)));
					if (LV > 0) {
						m_SkillInfo.emplace_back();
						m_SkillInfo.back().m_SkillType = static_cast<Player::SkillType>(loop);
						m_SkillInfo.back().m_Level = LV;
					}
				}
			}
			virtual ~SkillList() noexcept {

			}
		};

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
			std::shared_ptr<Objects::TeamHelicopterObj>	m_TeamHelicopterObj;
			std::shared_ptr<Objects::ArmorObj>			m_ArmorObj;
			std::shared_ptr<Objects::ArmorObj>			m_HelmetObj;
			std::shared_ptr<Objects::ItemContainerObj>	m_ItemContainerObj;
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
			auto&		GetHelicopter() noexcept { return this->m_HelicopterObj; }

			void		SetTeamHelicopter(const std::shared_ptr<Objects::TeamHelicopterObj>& pObj) noexcept { this->m_TeamHelicopterObj = pObj; }
			auto&		GetTeamHelicopter() noexcept { return this->m_TeamHelicopterObj; }

			void		SetArmor(const std::shared_ptr<Objects::ArmorObj>& pObj) noexcept { this->m_ArmorObj = pObj; }
			auto&		GetArmor(void) noexcept { return this->m_ArmorObj; }

			void		SetHelmet(const std::shared_ptr<Objects::ArmorObj>& pObj) noexcept { this->m_HelmetObj = pObj; }
			auto&		GetHelmet(void) noexcept { return this->m_HelmetObj; }

			void		SetItemContainerObj(const std::shared_ptr<Objects::ItemContainerObj>& pObj) noexcept { this->m_ItemContainerObj = pObj; }
			auto&		GetItemContainerObj(void) noexcept { return this->m_ItemContainerObj; }
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
				m_TeamHelicopterObj.reset();
				m_ArmorObj.reset();
				m_HelmetObj.reset();
				m_ItemContainerObj.reset();
				for (auto& player : this->m_Player) {
					player.reset();
				}
				this->m_Player.clear();
			}
		};
	}
}
