#pragma once

#pragma once
#include	"../../Header.hpp"
#include "AmmoData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class AmmoClass : public ObjectBaseClass {
		public:
			bool			m_IsDrawHitUI{false};
			float			m_Hit_alpha{0.f};
			VECTOR_ref		m_Hit_DispPos;
			int				m_Hit_AddX{0};
			int				m_Hit_AddY{0};
			HitPoint		m_Damage{0};
			ArmerPoint		m_ArmerDamage{0};
		private:
			bool			m_IsHit{false};
			float			m_HitTimer{0.f};
			int				m_RicochetCnt{0};
			std::shared_ptr<AmmoDataClass>	m_AmmoData{nullptr};
			float			m_speed{0.f};
			float			m_penetration{0.f};
			int				m_ShootCheraID{-1};
			float			m_yAdd{0.f};
			float			m_Timer{0.f};
		public://getter
			const auto&		GetShootedID(void) const noexcept { return this->m_ShootCheraID; }
			const auto&		GetDamage(void) const noexcept { return this->m_AmmoData->GetDamage(); }
			const auto&		GetCaliberSize(void) const noexcept { return this->m_AmmoData->GetCaliber(); }//0.00762f
		public:
			void			Put(std::shared_ptr<AmmoDataClass> pAmmoData, const VECTOR_ref& pos_t, const VECTOR_ref& pVec, int pMyID) {
				SetActive(true);
				this->m_RicochetCnt = 0;
				this->m_IsHit = false;
				this->m_IsDrawHitUI = false;
				this->m_HitTimer = 0.f;
				this->m_move.pos = pos_t;
				this->m_move.repos = this->m_move.pos;
				this->m_move.vec = pVec;
				this->m_AmmoData = pAmmoData;
				this->m_speed = this->m_AmmoData->GetSpeed() * Scale_Rate;
				this->m_penetration = this->m_AmmoData->GetPenetration();
				this->m_yAdd = 0.f;
				this->m_Timer = 0.f;
				this->m_ShootCheraID = pMyID;
			}
			const auto		PenetrationCheck(float pArmer, const VECTOR_ref& normal) const noexcept { return (this->m_penetration > (pArmer * (1.0f / std::abs(this->m_move.vec.Norm().dot(normal))))); }
			void			Penetrate(HitPoint Damage, ArmerPoint ArmerDamage) noexcept {
				SetActive(false);
				this->m_IsHit = true;
				this->m_IsDrawHitUI = true;
				this->m_Damage = Damage;
				this->m_ArmerDamage = ArmerDamage;
			}
			void			Ricochet(const VECTOR_ref& position, const VECTOR_ref& normal) {
				this->m_IsHit = true;
				this->m_IsDrawHitUI = false;
				//this->m_penetration *= 0.8f;
				this->m_RicochetCnt++;
				this->m_move.vec = (this->m_move.vec + normal * ((this->m_move.vec.dot(normal)) * -2.0f)).Norm();
				this->m_move.pos = this->m_move.vec * (0.1f) + position;
				this->m_yAdd = 0.f;
			}
			void			HitGround(const VECTOR_ref& position) {
				SetActive(false);
				this->m_move.pos = position;
				this->m_IsHit = true;
				this->m_IsDrawHitUI = false;
			}
		public: //コンストラクタ、デストラクタ
			AmmoClass(void) noexcept { this->m_objType = ObjType::Ammo; }
			~AmmoClass(void) noexcept { m_AmmoData.reset(); }
		public: //継承
			void			Init() noexcept override {
				ObjectBaseClass::Init();
				SetActive(false);
			}
			//
			void			FirstExecute(void) noexcept override {
				if (this->m_IsHit) {
					this->m_IsHit = false;
					this->m_HitTimer = 0.25f;
					this->m_Hit_alpha = 1.f;
					this->m_Hit_AddX = GetRand(32);
					this->m_Hit_AddY = GetRand(32);
				}
				this->m_HitTimer = std::clamp(this->m_HitTimer - 1.f / FPS, 0.f, 0.25f);
				if (this->m_Hit_alpha > 0.f) {
					Easing(&this->m_Hit_alpha, (this->m_HitTimer > 0.f) ? 2.f : 0.f, 0.95f, EasingType::OutExpo);
					if (this->m_Hit_alpha <= 0.01f) {
						this->m_Hit_alpha = 0;
						SetIsDelete(true);
					}
				}
				if (IsActive()) {
					//移動確定
					this->m_move.UpdatePos(this->m_move.pos + (this->m_move.vec * (this->m_speed / FPS)) + VECTOR_ref::up()*this->m_yAdd);
					this->m_yAdd += (M_GR / (FPS*FPS));

					//消す(スピードが0以下、貫通が0以下、5回反射する)
					if (this->m_speed <= 0.f || this->m_penetration <= 0.f || this->m_RicochetCnt > 5 || this->m_Timer > 5.f) {
						SetActive(false);
					}
					//this->m_speed -= 5.f / FPS;
					//this->m_penetration -= 5.f / FPS;
				}
				this->m_Timer += 1.f / FPS;
			}
			//
			void			DrawShadow(void) noexcept override {}
			void			CheckDraw(void) noexcept override {
				auto tmp = ConvWorldPosToScreenPos(this->m_move.pos.get());
				if (tmp.z >= 0.f && tmp.z <= 1.f) {
					this->m_Hit_DispPos = tmp;
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (!isDrawSemiTrans) {
					if (IsActive()) {
						if (CheckCameraViewClip_Box(this->m_move.repos.get(), this->m_move.pos.get()) == FALSE) {
							SetUseLighting(FALSE);
							SetUseHalfLambertLighting(FALSE);
							DrawCapsule_3D(this->m_move.pos, this->m_move.repos, 1.f * ((this->m_AmmoData->GetCaliber() - 0.00762f) * 0.1f + 0.00762f)*Scale_Rate, GetColor(255, 255, 172), Yellow);
							SetUseLighting(TRUE);
							SetUseHalfLambertLighting(TRUE);
						}
					}
				}
			}
		};
	};
};
