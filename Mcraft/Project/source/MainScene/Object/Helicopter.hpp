#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Objects {
		class HelicopterObj : public BaseObject {
		public:
			HelicopterObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Helicopter); }
			virtual ~HelicopterObj(void) noexcept {}
		public:
			void			SetMat(const Vector3DX& pos, const Matrix3x3DX& mat) noexcept {
				SetMove().SetAll(pos, pos, pos, Vector3DX::zero(), mat, mat);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
		public:
			void				Init_Sub(void) noexcept override {
				SetMinAABB(Vector3DX::vget(-10.f, -10.f, -10.f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(10.f, 10.f, 10.f) * Scale3DRate);
				SetActive(true);
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heli))->Play3D(GetMove().GetPos(), 50.f * Scale3DRate, DX_PLAYTYPE_LOOP);
			}
			void				FirstExecute(void) noexcept override{
				SetAnimLoop(static_cast<int>(0), 1.f);
				SetObj().SetAnim(static_cast<int>(0)).SetPer(1.f);
				SetObj().UpdateAnimAll();
			}
			void				DrawShadow(void) noexcept override {
				SetObj().SetMaterialDrawAlphaTestAll(TRUE, DX_CMP_GREATER, 128);
				BaseObject::DrawShadow();
			}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				SetObj().SetMaterialDrawAlphaTestAll(FALSE, DX_CMP_GREATER, 254);
				BaseObject::Draw(isDrawSemiTrans, Range);
			}
			void				Dispose_Sub(void) noexcept override {
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heli))->StopAll();
			}
		};
	};
};
