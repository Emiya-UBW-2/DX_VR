#pragma once

#pragma once
#include	"../../Header.hpp"
#include "AmmoData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class AmmoClass : public ObjectBaseClass {
		public:
			bool			m_IsDrawHitUI{ false };
			float			m_Hit_alpha{ 0.f };
			Vector3DX		m_Hit_DispPos;
			int				m_Hit_AddX{ 0 };
			int				m_Hit_AddY{ 0 };
			HitPoint		m_Damage{ 0 };
			ArmerPoint		m_ArmerDamage{ 0 };
		private:
			bool			m_IsHit{ false };
			float			m_HitTimer{ 0.f };
			int				m_RicochetCnt{ 0 };
			std::shared_ptr<AmmoDataClass>	m_AmmoData{ nullptr };
			float			m_speed{ 0.f };
			float			m_penetration{ 0.f };
			int				m_ShootCheraID{ -1 };
			float			m_yAdd{ 0.f };
			float			m_Timer{ 0.f };

			Vector3DX		pos;
			Vector3DX		repos;

		public://getter
			const auto& GetShootedID(void) const noexcept { return this->m_ShootCheraID; }
			const auto& GetDamage(void) const noexcept { return this->m_AmmoData->GetDamage(); }
			const auto& GetCaliberSize(void) const noexcept { return this->m_AmmoData->GetCaliber(); }//0.00762f

			const auto& GetRePos(void) const noexcept { return repos; }
			const auto& GetPos(void) const noexcept { return pos; }
		public:
			void			Put(std::shared_ptr<AmmoDataClass> pAmmoData, const Vector3DX& pos_t, const Vector3DX& pVec, int pMyID) {
				SetActive(true);
				this->m_RicochetCnt = 0;
				this->m_IsHit = false;
				this->m_IsDrawHitUI = false;
				this->m_HitTimer = 0.f;
				this->pos = pos_t;
				this->repos = pos_t;
				this->SetMove().SetAll(pos_t, pos_t, pos_t, pVec, Matrix3x3DX::identity(), Matrix3x3DX::identity());
				this->m_AmmoData = pAmmoData;
				this->m_speed = this->m_AmmoData->GetSpeed() * Scale3DRate;
				this->m_penetration = this->m_AmmoData->GetPenetration();
				this->m_yAdd = 0.f;
				this->m_Timer = 0.f;
				this->m_ShootCheraID = pMyID;
			}
			const auto		PenetrationCheck(float pArmer, const Vector3DX& normal) const noexcept { return (this->m_penetration > (pArmer * (1.0f / std::abs(Vector3DX::Dot(this->GetMove().GetVec().normalized(), normal))))); }
			void			Penetrate(HitPoint Damage, ArmerPoint ArmerDamage) noexcept {
				SetActive(false);
				this->m_IsHit = true;
				this->m_IsDrawHitUI = true;
				this->m_Damage = Damage;
				this->m_ArmerDamage = ArmerDamage;
			}
			void			Ricochet(const Vector3DX& position, const Vector3DX& normal) {
				this->m_IsHit = true;
				this->m_IsDrawHitUI = false;
				//this->m_penetration *= 0.8f;
				this->m_RicochetCnt++;
				this->SetMove().SetVec((this->GetMove().GetVec() + normal * ((Vector3DX::Dot(this->GetMove().GetVec(), normal)) * -2.0f)).normalized());
				this->pos = this->GetMove().GetVec() * (0.1f) + position;
				this->SetMove().Update(0.f, 0.f);
				//UpdateObjMatrix(GetMove().GetMat(), pos);
				this->m_yAdd = 0.f;
			}
			void			HitGround(const Vector3DX& position) {
				SetActive(false);
				this->pos = position;
				this->SetMove().Update(0.f, 0.f);
				//UpdateObjMatrix(GetMove().GetMat(), pos);
				this->m_IsHit = true;
				this->m_IsDrawHitUI = false;
			}
		public: //コンストラクタ、デストラクタ
			AmmoClass(void) noexcept { this->m_objType = (int)ObjType::Ammo; }
			~AmmoClass(void) noexcept { m_AmmoData.reset(); }
		public: //継承
			void			Init_Sub() noexcept override {
				SetActive(false);
			}
			//
			void			FirstExecute(void) noexcept override {
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (this->m_IsHit) {
					this->m_IsHit = false;
					this->m_HitTimer = 0.25f;
					this->m_Hit_alpha = 1.f;
					this->m_Hit_AddX = GetRand(32);
					this->m_Hit_AddY = GetRand(32);
				}
				this->m_HitTimer = std::clamp(this->m_HitTimer - DXLib_refParts->GetDeltaTime(), 0.f, 0.25f);
				if (this->m_Hit_alpha > 0.f) {
					Easing(&this->m_Hit_alpha, (this->m_HitTimer > 0.f) ? 2.f : 0.f, 0.95f, EasingType::OutExpo);
					if (this->m_Hit_alpha <= 0.01f) {
						this->m_Hit_alpha = 0;
						SetDelete();
					}
				}
				repos = this->pos;//弾だけ外で判定してるので
				if (IsActive()) {
					//移動確定
					this->pos = this->pos + (this->GetMove().GetVec() * (this->m_speed * DXLib_refParts->GetDeltaTime())) + Vector3DX::up() * this->m_yAdd;
					this->SetMove().Update(0.f, 0.f);
					this->m_yAdd += (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));

					//消す(スピードが0以下、貫通が0以下、5回反射する)
					if (this->m_speed <= 0.f || this->m_penetration <= 0.f || this->m_RicochetCnt > 5 || this->m_Timer > 5.f) {
						SetActive(false);
					}
					//this->m_speed -= 5.f  * DXLib_refParts->GetDeltaTime();
					//this->m_penetration -= 5.f * DXLib_refParts->GetDeltaTime();
				}

				this->m_Timer += DXLib_refParts->GetDeltaTime();
			}
			//
			void			DrawShadow(void) noexcept override {}
			void			CheckDraw(void) noexcept override {
				auto tmp = ConvWorldPosToScreenPos(this->pos.get());
				if (tmp.z >= 0.f && tmp.z <= 1.f) {
					this->m_Hit_DispPos = tmp;
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (!isDrawSemiTrans) {
					if (IsActive()) {
						if (CheckCameraViewClip_Box(this->repos.get(), this->pos.get()) == FALSE) {
							SetUseLighting(FALSE);
							SetUseHalfLambertLighting(FALSE);
							DrawCapsule_3D(this->pos, this->repos, 1.f * ((this->m_AmmoData->GetCaliber() - 0.00762f) * 0.1f + 0.00762f) * Scale3DRate, GetColor(255, 255, 172), Yellow);
							SetUseLighting(TRUE);
							SetUseHalfLambertLighting(TRUE);
						}
					}
				}
			}
		};
	};
};
