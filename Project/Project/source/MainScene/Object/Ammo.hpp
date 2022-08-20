#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class AmmoData {
		private:
			std::string		m_name;
			std::string		m_path;
			float			m_caliber{ 0.f };
			float			m_speed{ 100.f };				//弾速
			float			m_penetration{ 10.f };			//貫通
			HitPoint		m_damage{ 10 };					//ダメージ
		public://getter
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		GetPath(void) const noexcept { return this->m_path; }
			const auto&		GetCaliber(void) const noexcept { return this->m_caliber; }
			const auto&		GetSpeed(void) const noexcept { return this->m_speed; }
			const auto&		GetPenetration(void) const noexcept { return this->m_penetration; }
			const auto&		GetDamage(void) const noexcept { return this->m_damage; }
		public:
			void			Set(std::string path_, std::string named) {
				this->m_name = named;
				this->m_path = path_ + named;
				int mdata = FileRead_open((this->m_path + "/data.txt").c_str(), FALSE);
				this->m_caliber = getparams::_float(mdata) * 0.001f;	//口径
				this->m_speed = getparams::_float(mdata);				//弾速
				this->m_penetration = getparams::_float(mdata);			//貫通
				this->m_damage = (HitPoint)getparams::_int(mdata);		//ダメージ
				FileRead_close(mdata);
			}
		};
		class AmmoClass : public ObjectBaseClass {
		private:
			bool			m_IsHit{ false };
			float			m_HitTimer{ 0.f };
			int				m_RicochetCnt{ 0 };
			const AmmoData*	m_AmmoData{ nullptr };
			float			m_speed{ 0.f };
			float			m_penetration{ 0.f };
			float			Hit_alpha{ 0.f };
			int				Hit_window_x{ 0 };
			int				Hit_window_y{ 0 };
			int				m_ShootCheraID{ -1 };
		public://getter
			const auto&		GetShootedID(void) const noexcept { return this->m_ShootCheraID; }
			const auto&		GetDamage(void) const noexcept { return this->m_AmmoData->GetDamage(); }
			const auto&		GetCaliberSize(void) const noexcept { return this->m_AmmoData->GetCaliber(); }
			const auto		GetEffectSize(void) const noexcept { return ((this->m_AmmoData->GetCaliber() >= 0.020f) ? this->m_AmmoData->GetCaliber() : 0.025f) / 0.1f; }
			const auto		GetHitPicActive(void) const noexcept { return this->Hit_alpha > 0.f; }
		public:
			void			Put(const AmmoData* pAmmoData, const VECTOR_ref& pPos, const VECTOR_ref& pVec, int pMyID) noexcept {
				this->m_IsActive = true;
				this->m_IsDraw = true;
				this->m_RicochetCnt = 0;
				this->m_IsHit = false;
				this->m_HitTimer = 0.f;
				this->m_move.pos = pPos;
				this->m_move.repos = this->m_move.pos;
				this->m_move.vec = pVec;
				this->m_AmmoData = pAmmoData;
				this->m_speed = this->m_AmmoData->GetSpeed() * Scale_Rate;
				this->m_penetration = this->m_AmmoData->GetPenetration();
				this->m_ShootCheraID = pMyID;
			}
			void			Draw_Hit_UI(GraphHandle& Hit_Graph) noexcept {
				if (this->Hit_alpha >= 10.f / 255.f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f * this->Hit_alpha));
					Hit_Graph.DrawRotaGraph(this->Hit_window_x, this->Hit_window_y, (float)y_r(this->Hit_alpha * 0.5f * 100.0f) / 100.f, 0.f, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
			}
			const auto		ColCheckGround(void) noexcept {
				MV1_COLL_RESULT_POLY ColResGround; ColResGround.HitFlag = FALSE;
				if (this->m_IsActive) {
					ColResGround = this->m_MapCol->CollCheck_Line(this->m_move.repos, this->m_move.pos);
					if (ColResGround.HitFlag == TRUE) {
						this->m_move.pos = ColResGround.HitPosition;
					}
				}
				return ColResGround;
			}
			const auto		PenetrationCheck(float pArmer, const VECTOR_ref& normal) const noexcept { return (this->m_penetration > (pArmer * (1.0f / std::abs(this->m_move.vec.Norm().dot(normal))))); }
			void			Penetrate(void) noexcept {
				this->m_IsActive = false;
				this->m_IsHit = true;
			}
			void			Ricochet(const VECTOR_ref& position, const VECTOR_ref& normal) noexcept {
				this->m_IsHit = true;
				//this->m_penetration *= 0.8f;
				this->m_RicochetCnt++;
				this->m_move.vec = (this->m_move.vec + normal * ((this->m_move.vec.dot(normal)) * -2.0f)).Norm();
				this->m_move.pos = this->m_move.vec * (0.1f) + position;
			}
			void			HitGround(void) noexcept {
				this->m_IsActive = false;
			}
		public: //コンストラクタ、デストラクタ
			AmmoClass(void) noexcept { this->m_objType = ObjType::Ammo; }
			~AmmoClass(void) noexcept {}
		public: //継承
			void			Init() noexcept override {
				ObjectBaseClass::Init();
				this->m_Use_RealTimePhysics = false;
				this->m_IsActive = false;
			}
			//
			void			FirstExecute(void) noexcept override {
				if (this->m_IsHit) {
					this->m_IsHit = false;
					this->m_HitTimer = 0.25f;
					this->Hit_alpha = 1.f;
				}
				this->m_HitTimer = std::clamp(this->m_HitTimer - 1.f / FPS, 0.f, 0.25f);
				if (this->Hit_alpha > 0.f) {
					Easing(&this->Hit_alpha, (this->m_HitTimer > 0.f) ? 2.f : 0.f, 0.95f, EasingType::OutExpo);
					if (this->Hit_alpha <= 0.01f) {
						this->Hit_alpha = 0;
					}
				}
				if (this->m_IsActive) {
					//移動確定
					this->m_move.SetPos(this->m_move.pos + (this->m_move.vec * (this->m_speed / FPS)));
					//消す(スピードが0以下、貫通が0以下、5回反射する)
					if (this->m_speed <= 0.f || this->m_penetration <= 0.f || this->m_RicochetCnt > 5) {
						this->m_IsActive = false;
					}
					//this->m_speed -= 5.f / FPS;
					//this->m_penetration -= 5.f / FPS;
				}
			}
			//
			void			DrawShadow(void) noexcept  override {}
			void			CheckDraw(void) noexcept  override {
				if (this->m_IsHit) {
					auto p = ConvWorldPosToScreenPos(this->m_move.pos.get());
					if (p.z >= 0.f && p.z <= 1.f) {
						this->Hit_window_x = int(p.x);
						this->Hit_window_y = int(p.y);
					}
				}
			}
			void			Draw(void) noexcept  override {
				if (this->m_IsDraw) {
					if (!this->m_IsActive) {
						this->m_IsDraw = false;
					}
					SetUseLighting(FALSE);
					DrawCapsule_3D(this->m_move.pos, this->m_move.repos, ((this->m_AmmoData->GetCaliber() - 0.00762f) * 0.1f + 0.00762f)*Scale_Rate, GetColor(255, 255, 172), GetColor(255, 255, 255));
					SetUseLighting(TRUE);
				}
			}
		};
	};
};
