#pragma once

#pragma once
#include	"../../Header.hpp"
#include	"AmmoData.hpp"

namespace FPS_n2 {
	namespace Objects {
		class AmmoObj : public BaseObject {
			int				m_RicochetCnt{ 0 };
			const std::unique_ptr<AmmoData>*	m_AmmoData{ nullptr };
			float			m_speed{ 0.0f };
			float			m_penetration{ 0.0f };
			int				m_ShootCheraID{ InvalidID };
			float			m_yAdd{ 0.0f };
			float			m_Timer{ 0.0f };

			Vector3DX		m_pos;
			Vector3DX		m_vec;
			Vector3DX		m_repos;

			int				m_HP{};

			bool			m_IsDrawLine{ false };

			int				m_EffectUniqueID{ InvalidID };
			float			m_EffectTimer{ 0.0f };
		public:
			//接地
			void			Put(const std::unique_ptr<AmmoData>* pAmmoData, const Vector3DX& pos, const Vector3DX& vec, int myID) noexcept;
			/*
			//跳ね返された
			const auto		PenetrationCheck(float pArmer, const Vector3DX& normal) const noexcept { return (this->m_penetration > (pArmer * (1.0f / std::abs(Vector3DX::Dot(this->m_vec.normalized(), normal))))); }
			void			Ricochet(const Vector3DX& position, const Vector3DX& normal) {
				++this->m_RicochetCnt;
				this->m_vec = ((this->m_vec + normal * ((Vector3DX::Dot(this->m_vec, normal)) * -2.0f)).normalized());
				this->m_pos = this->m_vec * (0.1f) + position;
				this->m_yAdd = 0.0f;
			}
			//*/
		public: //コンストラクタ、デストラクタ
			AmmoObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Ammo); }
			virtual ~AmmoObj(void) noexcept { this->m_AmmoData = nullptr; }
		public: //継承
			void			Init_Sub(void) noexcept override {
				this->m_IsDrawLine = false;
				SetActive(false);
				SetMinAABB(Vector3DX::vget(-1.0f, -1.0f, -1.0f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(1.0f, 1.0f, 1.0f) * Scale3DRate);
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
			void FirstUpdate() noexcept {
				for (auto& ammo : this->m_AmmoList) {
					ammo->FirstUpdate();
				}
			}
		public:
			void Put(const std::unique_ptr<AmmoData>* pAmmoData, const Vector3DX& pos, const Vector3DX& vec, int myID) noexcept {
				for (auto& ammo : this->m_AmmoList) {
					if (!ammo->IsActive()) {
						ammo->Put(pAmmoData, pos, vec, myID);
						return;
					}
				}
				this->m_AmmoList.emplace_back(std::make_shared<Objects::AmmoObj>());
				ObjectManager::Instance()->InitObject(this->m_AmmoList.back());
				this->m_AmmoList.back()->Put(pAmmoData, pos, vec, myID);
			}
		};

		class AmmoLine {
			float m_Time{ 0.f };
			Vector3DX m_Pos;
			Vector3DX m_Vec;
			float m_Per{ 0.f };
		public:
			bool IsActive() const noexcept {
				return m_Time > 0.f;
			}
			void Put(const Vector3DX& pos, const Vector3DX& vec, const Vector3DX&) noexcept {
				m_Time = 2.f;


				m_Vec = vec.normalized();
				m_Pos = pos;// GetMinPosSegmentToPoint(pos - m_Vec * (1000.f * Scale3DRate), pos + m_Vec * (1000.f * Scale3DRate), MyPos);
			}
		public:
			void Update() noexcept;
			void Draw() noexcept {
				if (!IsActive()) { return; }
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(128.0f * m_Per));
				DrawCapsule_3D(
					this->m_Pos,
					this->m_Pos + this->m_Vec * (10.f * Scale3DRate),
					0.001f * Scale3DRate, GetColor(255, 255, 255), Yellow);
			}
		};

		class AmmoLinePool : public SingletonBase<AmmoLinePool> {
		private:
			friend class SingletonBase<AmmoLinePool>;
		private:
			std::vector<std::shared_ptr<AmmoLine>> m_AmmoList;
		private:
			AmmoLinePool(void) noexcept {
				this->m_AmmoList.resize(64);
				for (auto& ammo : this->m_AmmoList) {
					ammo = std::make_shared<AmmoLine>();
				}
			}
			virtual ~AmmoLinePool(void) noexcept {
				for (auto& ammo : this->m_AmmoList) {
					ammo.reset();
				}
				this->m_AmmoList.clear();
			}
		public:
			void Put(const Vector3DX& pos, const Vector3DX& vec, const Vector3DX& MyPos) noexcept {
				for (auto& ammo : this->m_AmmoList) {
					if (!ammo->IsActive()) {
						ammo->Put(pos, vec, MyPos);
						return;
					}
				}
				this->m_AmmoList.emplace_back(std::make_shared<AmmoLine>());
				this->m_AmmoList.back()->Put(pos, vec, MyPos);
			}
		public:
			void Update() noexcept {
				for (auto& ammo : this->m_AmmoList) {
					ammo->Update();
				}
			}
			void Draw() noexcept {
				SetUseLighting(false);
				SetUseHalfLambertLighting(false);
				for (auto& ammo : this->m_AmmoList) {
					ammo->Draw();
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				SetUseLighting(true);
				SetUseHalfLambertLighting(true);
			}
		};
	}
}
