#pragma once
#include	"../../Header.hpp"
#include "ObjectBase_before.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class AmmoDataClass : public ItemData {
		private:
			float			m_caliber{ 0.f };
			float			m_speed{ 100.f };				//弾速
			float			m_penetration{ 10.f };			//貫通
			HitPoint		m_damage{ 10 };					//ダメージ
			int				m_Pellet{ 1 };
			float			m_Accuracy{ 0.f };
			int				m_FallSound{ 0 };
		public://getter
			const auto&		GetCaliber(void) const noexcept { return this->m_caliber; }
			const auto&		GetSpeed(void) const noexcept { return this->m_speed; }
			const auto&		GetPenetration(void) const noexcept { return this->m_penetration; }
			const auto&		GetDamage(void) const noexcept { return this->m_damage; }
			const auto&		GetPellet(void) const noexcept { return this->m_Pellet; }
			const auto&		GetAccuracy(void) const noexcept { return this->m_Accuracy; }
			const auto&		GetFallSound(void) const noexcept { return this->m_FallSound; }
		protected:
			void		Set_Sub(const std::string& LEFT, const std::string&RIGHT) noexcept override {
				if (LEFT == "ammo_cal(mm)") {
					this->m_caliber = std::stof(RIGHT)* 0.001f;		//口径
				}
				if (LEFT == "ammo_speed(m/s)") {
					this->m_speed = std::stof(RIGHT);				//弾速
				}
				if (LEFT == "ammo_pene(mm)") {
					this->m_penetration = std::stof(RIGHT);			//貫通
				}
				if (LEFT == "ammo_damage") {
					this->m_damage = (HitPoint)std::stoi(RIGHT);		//ダメージ
				}
				if (LEFT == "Pellets") {
					this->m_Pellet = std::stoi(RIGHT);				//ペレットの数
				}
				if (LEFT == "Accuracy") {
					this->m_Accuracy = std::stof(RIGHT);				//ペレットの数
				}
				if (LEFT == "FallSound") {
					this->m_FallSound = std::stoi(RIGHT);				//ペレットの数
				}
				
			}
		public:
		};

		class AmmoDataManager : public SingletonBase<AmmoDataManager> {
		private:
			friend class SingletonBase<AmmoDataManager>;
		private:
			std::vector<std::shared_ptr<AmmoDataClass>>	m_Object;
		public:
			const auto&	LoadAction(const std::string& filepath) noexcept {
				for (auto& o : m_Object) {
					if (o->GetPath() == filepath) {
						return o;
					}
				}
				m_Object.resize(m_Object.size() + 1);
				m_Object.back() = std::make_shared<AmmoDataClass>();
				m_Object.back()->Set(filepath);
				return m_Object.back();
			}
		};
	};
};
