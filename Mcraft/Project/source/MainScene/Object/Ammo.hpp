#pragma once

#pragma once
#include	"../../Header.hpp"
#include	"AmmoData.hpp"

namespace FPS_n2 {
	namespace Objects {
		class AmmoObj : public BaseObject {
			int				m_RicochetCnt{ 0 };
			const std::unique_ptr<AmmoData>*	m_AmmoData{ nullptr };
			float			m_speed{ 0.f };
			float			m_penetration{ 0.f };
			int				m_ShootCheraID{ InvalidID };
			float			m_yAdd{ 0.f };
			float			m_Timer{ 0.f };

			Vector3DX		m_pos;
			Vector3DX		m_vec;
			Vector3DX		m_repos;

			bool			m_IsDrawLine{ false };
		public:
			//接地
			void			Put(const std::unique_ptr<AmmoData>* pAmmoData, const Vector3DX& pos_t, const Vector3DX& pVec, int pMyID) {
				this->m_RicochetCnt = 0;
				this->m_pos = pos_t;
				this->m_repos = pos_t;
				this->m_vec = pVec;
				this->m_AmmoData = pAmmoData;
				this->m_speed = (*this->m_AmmoData)->GetSpeed() * Scale3DRate;
				this->m_penetration = (*this->m_AmmoData)->GetPenetration();
				this->m_yAdd = 0.f;
				this->m_Timer = 0.f;
				this->m_ShootCheraID = pMyID;
				SetActive(true);
				this->m_IsDrawLine = true;
			}
			/*
			//跳ね返された
			const auto		PenetrationCheck(float pArmer, const Vector3DX& normal) const noexcept { return (this->m_penetration > (pArmer * (1.0f / std::abs(Vector3DX::Dot(this->m_vec.normalized(), normal))))); }
			void			Ricochet(const Vector3DX& position, const Vector3DX& normal) {
				++this->m_RicochetCnt;
				this->m_vec = ((this->m_vec + normal * ((Vector3DX::Dot(this->m_vec, normal)) * -2.0f)).normalized());
				this->m_pos = this->m_vec * (0.1f) + position;
				this->m_yAdd = 0.f;
			}
			//*/
		public: //コンストラクタ、デストラクタ
			AmmoObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Ammo); }
			virtual ~AmmoObj(void) noexcept { this->m_AmmoData = nullptr; }
		public: //継承
			void			Init_Sub(void) noexcept override {
				this->m_IsDrawLine = false;
				SetActive(false);
				SetMinAABB(Vector3DX::vget(-1.f, -1.f, -1.f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(1.f, 1.f, 1.f) * Scale3DRate);
			}
			void			FirstUpdate(void) noexcept override;
			void			DrawShadow(void) noexcept override {}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!this->m_IsDrawLine) { return; }
				if (!IsDraw(Range)) { return; }
				if (isDrawSemiTrans) { return; }
				SetUseLighting(false);
				SetUseHalfLambertLighting(false);
				DrawCapsule_3D(this->m_pos, this->m_repos, (((*this->m_AmmoData)->GetCaliber() - 0.00762f) * 0.1f + 0.00762f) * Scale3DRate, GetColor(255, 255, 172), Yellow);
				SetUseLighting(true);
				SetUseHalfLambertLighting(true);
			}
		};


		class AmmoPool : public SingletonBase<AmmoPool> {
		private:
			friend class SingletonBase<AmmoPool>;
		private:
			std::vector<std::shared_ptr<Objects::AmmoObj>> m_AmmoList;
		private:
			AmmoPool(void) noexcept {
				this->m_AmmoList.resize(64);
				for (auto& ammo : this->m_AmmoList) {
					ammo = std::make_shared<Objects::AmmoObj>();
					ObjectManager::Instance()->InitObject(ammo);
				}
			}
			virtual ~AmmoPool(void) noexcept {
				for (auto& ammo : this->m_AmmoList) {
					ObjectManager::Instance()->DelObj(ammo);
					ammo.reset();
				}
				this->m_AmmoList.clear();
			}
		public:
			void Put(const std::unique_ptr<AmmoData>* pAmmoData, const Vector3DX& pos_t, const Vector3DX& pVec, int pMyID) noexcept {
				for (auto& ammo : this->m_AmmoList) {
					if (!ammo->IsActive()) {
						ammo->Put(pAmmoData, pos_t, pVec, pMyID);
						return;
					}
				}
				this->m_AmmoList.emplace_back(std::make_shared<Objects::AmmoObj>());
				ObjectManager::Instance()->InitObject(this->m_AmmoList.back());
				this->m_AmmoList.back()->Put(pAmmoData, pos_t, pVec, pMyID);
			}
		};
	}
}
