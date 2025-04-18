#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Objects {
		class HelicopterObj : public BaseObject {
			Vector3DX m_PosR{};
			Vector3DX m_Pos{};

			float m_Yrad = 0.f;
			float m_Zrad = 0.f;
			float m_ZradR = 0.f;

			float m_SideSpeedPer = 0.f;
			float m_flontSpeedPer = 0.f;
		public:
			HelicopterObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Helicopter); }
			virtual ~HelicopterObj(void) noexcept {}
		public:
			void			SetMat(const Vector3DX& pos, const Matrix3x3DX& mat) noexcept {
				SetMove().SetAll(pos, pos, pos, Vector3DX::zero(), mat, mat);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heli))->SetPosition(GetMove().GetPos());
			}
		public:
			void				Init_Sub(void) noexcept override {
				SetMinAABB(Vector3DX::vget(-10.f, -10.f, -10.f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(10.f, 10.f, 10.f) * Scale3DRate);
				SetActive(true);
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heli))->Play3D(GetMove().GetPos(), 50.f * Scale3DRate, DX_PLAYTYPE_LOOP);
				m_PosR = Vector3DX::vget(0.f, -20.f, 0.f) * Scale3DRate;
				m_Yrad = 0.f;
			}
			void				FirstExecute(void) noexcept override{
				SetAnimLoop(static_cast<int>(0), 1.f);
				SetObj().SetAnim(static_cast<int>(0)).SetPer(1.f);
				SetObj().UpdateAnimAll();

				Easing(&m_PosR, (Vector3DX::vget(0.f, -20.f, 0.f) + Vector3DX::vget(GetRandf(1.f), GetRandf(1.f), GetRandf(1.f))) * Scale3DRate, 0.95f, EasingType::OutExpo);
				Easing(&m_Pos, m_PosR, 0.95f, EasingType::OutExpo);

				Easing(&m_ZradR, GetRandf(30), 0.95f, EasingType::OutExpo);
				Easing(&m_Zrad, m_ZradR, 0.95f, EasingType::OutExpo);

				SetMat(m_Pos,
					Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(m_Zrad + 20.f * m_SideSpeedPer)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30.f * m_flontSpeedPer)) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(m_Yrad)));
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
