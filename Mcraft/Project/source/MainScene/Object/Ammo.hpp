#pragma once

#pragma once
#include	"../../Header.hpp"
#include "AmmoData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class AmmoClass : public ObjectBaseClass {
			int				m_RicochetCnt{ 0 };
			std::shared_ptr<AmmoDataClass>	m_AmmoData{ nullptr };
			float			m_speed{ 0.f };
			float			m_penetration{ 0.f };
			int				m_ShootCheraID{ -1 };
			float			m_yAdd{ 0.f };
			float			m_Timer{ 0.f };

			Vector3DX		m_pos;
			Vector3DX		m_vec;
			Vector3DX		m_repos;
		public:
			//接地
			void			Put(std::shared_ptr<AmmoDataClass> pAmmoData, const Vector3DX& pos_t, const Vector3DX& pVec, int pMyID) {
				this->m_RicochetCnt = 0;
				this->m_pos = pos_t;
				this->m_repos = pos_t;
				this->m_vec = pVec;
				this->m_AmmoData = pAmmoData;
				this->m_speed = this->m_AmmoData->GetSpeed() * Scale3DRate;
				this->m_penetration = this->m_AmmoData->GetPenetration();
				this->m_yAdd = 0.f;
				this->m_Timer = 0.f;
				this->m_ShootCheraID = pMyID;
			}
			/*
			//跳ね返された
			const auto		PenetrationCheck(float pArmer, const Vector3DX& normal) const noexcept { return (this->m_penetration > (pArmer * (1.0f / std::abs(Vector3DX::Dot(this->m_vec.normalized(), normal))))); }
			void			Ricochet(const Vector3DX& position, const Vector3DX& normal) {
				this->m_RicochetCnt++;
				this->m_vec = ((this->m_vec + normal * ((Vector3DX::Dot(this->m_vec, normal)) * -2.0f)).normalized());
				this->m_pos = this->m_vec * (0.1f) + position;
				this->m_yAdd = 0.f;
			}
			//*/
		public: //コンストラクタ、デストラクタ
			AmmoClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Ammo); }
			~AmmoClass(void) noexcept { m_AmmoData.reset(); }
		public: //継承
			void			Init_Sub() noexcept override {
				SetActive(true);
			}
			void			FirstExecute(void) noexcept override;
			void			DrawShadow(void) noexcept override {}
			void			CheckDraw(void) noexcept override {}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (!isDrawSemiTrans) {
					if (IsActive()) {
						if (CheckCameraViewClip_Box(this->m_repos.get(), this->m_pos.get()) == FALSE) {
							SetUseLighting(FALSE);
							SetUseHalfLambertLighting(FALSE);
							DrawCapsule_3D(this->m_pos, this->m_repos, 1.f * ((this->m_AmmoData->GetCaliber() - 0.00762f) * 0.1f + 0.00762f) * Scale3DRate, GetColor(255, 255, 172), Yellow);
							SetUseLighting(TRUE);
							SetUseHalfLambertLighting(TRUE);
						}
					}
				}
			}
		};
	};
};
