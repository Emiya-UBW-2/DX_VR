#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Objects {
		class HelicopterObj : public BaseObject {
			Vector3DX m_TargetPos{};
			Vector3DX m_PrevPos{};
			Vector3DX m_NowPos{};


			Vector3DX m_PosR{};
			Vector3DX m_Pos{};

			float m_Yrad = 0.f;
			float m_YradR = 0.f;
			float m_YradRT = 0.f;
			float m_Zrad = 0.f;
			float m_ZradR = 0.f;

			float m_SideSpeedPer = 0.f;
			float m_flontSpeedPer = 0.f;

			float m_Timer = 0.f;
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
				m_YradR = 0.f;
				m_NowPos = Vector3DX::vget(0.f, 0.f, 0.f) * Scale3DRate;
				m_PrevPos = m_NowPos;
				m_TargetPos = Vector3DX::vget(GetRandf(50.f), 0.f, GetRandf(50.f)) * Scale3DRate;
				auto Vec = (m_TargetPos - m_PrevPos);
				if (Vec.magnitude() > 0.f) {
					m_YradRT = rad2deg(std::atan2(Vec.x, Vec.z) + DX_PI_F);
				}
				m_Timer = 0.f;
			}
			void				FirstUpdate(void) noexcept override{
				auto* DXLib_refParts = DXLib_ref::Instance();

				SetAnimLoop(static_cast<int>(0), 1.f);
				SetObj().SetAnim(static_cast<int>(0)).SetPer(1.f);
				SetObj().UpdateAnimAll();

				m_Timer += DXLib_refParts->GetDeltaTime();

				if (m_Timer > 10.f) {
					m_Timer -= 10.f;
					m_PrevPos = m_TargetPos;
					m_TargetPos = Vector3DX::vget(GetRandf(50.f), 0.f, GetRandf(50.f)) * Scale3DRate;
					auto Vec = (m_TargetPos - m_PrevPos);
					if (Vec.magnitude() > 0.f) {
						m_YradRT = rad2deg(std::atan2(Vec.x, Vec.z) + DX_PI_F);
					}
				}
				m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.f, 0.f, 1.f));

				auto PrevPos = m_Pos;

				Easing(&m_PosR, m_NowPos + (Vector3DX::vget(0.f, -20.f, 0.f) + Vector3DX::vget(GetRandf(1.f), GetRandf(1.f), GetRandf(1.f))) * Scale3DRate, 0.95f, EasingType::OutExpo);
				Easing(&m_Pos, m_PosR, 0.95f, EasingType::OutExpo);

				auto Vec = Matrix3x3DX::Vtrans((m_Pos - PrevPos), GetMove().GetMat().inverse()) / (60.f * DXLib_refParts->GetDeltaTime());
				Easing(&m_flontSpeedPer, std::clamp(-Vec.z, -1.f, 1.f), 0.975f, EasingType::OutExpo);
				Easing(&m_SideSpeedPer, std::clamp(-Vec.x, -1.f, 1.f), 0.975f, EasingType::OutExpo);

				auto PrevYrad = m_Yrad;
				Easing(&m_YradR, m_YradRT, 0.985f, EasingType::OutExpo);
				Easing(&m_Yrad, m_YradR, 0.985f, EasingType::OutExpo);

				Easing(&m_ZradR, GetRandf(30) + (m_Yrad - PrevYrad) * 50.f, 0.95f, EasingType::OutExpo);
				Easing(&m_Zrad, m_ZradR, 0.95f, EasingType::OutExpo);

				SetMat(m_Pos,
					Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(m_Zrad + 20.f * m_SideSpeedPer)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30.f * m_flontSpeedPer)) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(m_Yrad)));
			}
			void				DrawShadow(void) noexcept override {
				SetObj().SetMaterialDrawAlphaTestAll(TRUE, DX_CMP_GREATER, 128);
				BaseObject::DrawShadow();
				SetObj().SetMaterialDrawAlphaTestAll(FALSE, DX_CMP_GREATER, 254);
			}
			void				Dispose_Sub(void) noexcept override {
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heli))->StopAll();
			}
		};
	};
};
