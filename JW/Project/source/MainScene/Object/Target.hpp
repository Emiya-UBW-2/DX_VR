#pragma once
#include	"../../Header.hpp"

#include "ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TargetClass : public ObjectBaseClass {
			std::vector<Vector3DX> HitPosRec;
		public:
			TargetClass(void) noexcept { this->m_objType = ObjType::Target; }
			~TargetClass(void) noexcept {}
		public:
			const auto& GetHitPosRec() const noexcept { return this->HitPosRec; }
			const auto GetCenterPos() const noexcept { return this->m_col.frame(2); }
			const auto GetHitPoint(const Vector3DX& value, float* x = nullptr, float* y = nullptr) noexcept {
				auto vecx = this->m_col.frame(3) - GetCenterPos();
				auto vecy = this->m_col.frame(4) - GetCenterPos();
				auto vecsize = (vecx.magnitude() + vecy.magnitude()) / 2;
				auto vec2 = value - GetCenterPos();

				if (x != nullptr) {
					*x = (vec2.magnitude() / vecsize) * (-Vector3DX::Dot(vecx.normalized(), vec2.normalized()));
				}
				if (y != nullptr) {
					*y = (vec2.magnitude() / vecsize) * (-Vector3DX::Dot(vecy.normalized(), vec2.normalized()));
				}
				return ((1.f - (vec2.magnitude() / vecsize)) * 11.f);
			}
			const auto SetHitPos(const Vector3DX& value) noexcept {
				this->m_obj.get_anime(0).GoStart();
				HitPosRec.emplace_back(value);
				return GetHitPoint(HitPosRec.back());
			}
			void ResetHit(void) noexcept {
				HitPosRec.clear();
			}
		public:
			void FirstExecute(void) noexcept override {
				this->m_obj.get_anime(0).per = 1.f;
				SetAnimOnce(0, 1.f);
				this->m_obj.work_anime();
			}
			void DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					this->GetObj().DrawModel();
				}
			}
		};
	};
};
