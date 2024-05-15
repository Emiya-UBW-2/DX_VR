#pragma once
#include	"../../Header.hpp"

#include "../../MainScene/Object/ItemData.hpp"
#include "../../MainScene/Object/Vehicle.hpp"

#include "../../MainScene/NetWork.hpp"
#include "../../MainScene/NetworkBrowser.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		class PlayerControl {
		private:
			std::shared_ptr<VehicleClass>		m_Vehicle{ nullptr };
			float								m_Score{ 0.f };							//ƒXƒRƒA
		public:
			PlayerControl(void) noexcept {
				this->m_Score = 0.f;
			}
			~PlayerControl(void) noexcept {
				this->Dispose();
			}
		public:
			void		SetVehicle(const std::shared_ptr<VehicleClass>& pVehicle) { m_Vehicle = pVehicle; }
			void		AddScore(float value) { this->m_Score += value; }
			void		SubScore(float value) { this->m_Score -= value; }
			void		SetScore(float value) { this->m_Score = value; }
			auto&		GetVehicle(void) noexcept { return m_Vehicle; }
			const auto&	GetScore(void) const noexcept { return this->m_Score; }
		public:
			void Init(void) noexcept {
				this->m_Vehicle = nullptr;
			}

			void Dispose(void) noexcept {
				this->m_Vehicle = nullptr;
			}
		};

		class PlayerManager :public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		private:
			std::vector<PlayerControl> m_Player;
		public:
			auto&		GetPlayer(int ID) { return m_Player[ID]; }
		public:
			void Init(int playerNum) {
				this->m_Player.resize(playerNum);
				for (auto& p : this->m_Player) {
					p.Init();
				}
			}
			void Dispose(void) noexcept {
				for (auto& p : this->m_Player) {
					p.Dispose();
				}
				this->m_Player.clear();
			}
		};

	};
};
