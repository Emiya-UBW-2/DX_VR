#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Objects {
		enum class HeliFrame {
			Rappelling1,
			Rappelling2,
			Max,
		};
		static const char* HeliFrameName[static_cast<int>(HeliFrame::Max)] = {
			"Aキャビン下",
			"Bキャビン下",
		};


		enum class HelicopterMove {
			Random,
			Rappelling,
		};

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

			bool m_Open = false;
			float m_OpenPer = 0.f;

			bool m_Rope = false;
			float m_RopePer = 0.f;

			HelicopterMove m_HelicopterMove{ HelicopterMove::Random };
		private:
			int					GetFrameNum(void) noexcept override { return static_cast<int>(HeliFrame::Max); }
			const char*			GetFrameStr(int id) noexcept override { return HeliFrameName[id]; }
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
			const auto		GetIsActiveRappelling() const noexcept {
				return (this->m_HelicopterMove == HelicopterMove::Rappelling) && (13.f <= this->m_Timer && this->m_Timer <= 30.f);
			}
		private:
			void				SetAction(HelicopterMove Move) noexcept {
				m_HelicopterMove = Move;
				m_Timer = 0.f;
				switch (this->m_HelicopterMove) {
				case HelicopterMove::Random:
					m_PrevPos = m_NowPos;
					m_TargetPos = Vector3DX::vget(GetRandf(50.f), 0.f, GetRandf(50.f)) * Scale3DRate;
					break;
				case HelicopterMove::Rappelling:
					m_PrevPos = m_NowPos;
					m_TargetPos = Vector3DX::vget(GetRandf(10.f), 0.f, GetRandf(10.f)) * Scale3DRate;
					break;
				default:
					break;
				}
				auto Vec = (m_TargetPos - m_PrevPos);
				if (Vec.magnitude() > 0.f) {
					m_YradRT = rad2deg(std::atan2(Vec.x, Vec.z) + DX_PI_F);
				}
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
				SetAction(HelicopterMove::Random);
			}
			void				FirstUpdate(void) noexcept override {
				auto* DXLib_refParts = DXLib_ref::Instance();

				m_RopePer = std::clamp(m_RopePer + (m_Rope ? 10.f : -5.f) * DXLib_refParts->GetDeltaTime(), 0.f, 10.f);
				Easing(&m_OpenPer, (m_Open) ? 1.f : 0.f, 0.95f, EasingType::OutExpo);

				SetAnimLoop(static_cast<int>(0), 1.f);
				SetObj().SetAnim(static_cast<int>(0)).SetPer(1.f);
				SetAnimLoop(static_cast<int>(1), 1.f);
				SetObj().SetAnim(static_cast<int>(1)).SetPer(m_OpenPer);
				SetObj().UpdateAnimAll();

				m_Timer += DXLib_refParts->GetDeltaTime();

				switch (this->m_HelicopterMove) {
				case HelicopterMove::Random:
					if (m_Timer > 10.f) {
						SetAction(HelicopterMove::Random);

						SetAction(HelicopterMove::Rappelling);
					}
					m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.f, 0.f, 1.f));


					break;
				case HelicopterMove::Rappelling:
					if (m_Timer > 8.f) {
						m_Open = true;
					}
					if (m_Timer > 13.f) {
						m_Rope = true;
					}
					if (m_Timer > 33.f) {
						m_Rope = false;
					}
					if (m_Timer > 38.f) {
						SetAction(HelicopterMove::Random);
						m_Open = false;
					}
					m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.f, 0.f, 1.f));
					break;
				default:
					break;
				}



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
			void				Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!IsDraw(Range)) { return; }
				if (!GetObj().IsActive()) { return; }
				if (isDrawSemiTrans) { return; }
				GetObj().DrawModel();

				if (m_RopePer > 0.f) {
					Vector3DX Pos;

					Pos = GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(HeliFrame::Rappelling1))).pos();

					DxLib::DrawCapsule3D(Pos.get(), (Pos + Vector3DX::down() * (m_RopePer * Scale3DRate)).get(), 0.05f * Scale3DRate, 4, GetColor(32, 32, 32), GetColor(32, 32, 32), TRUE);

					Pos = GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(HeliFrame::Rappelling2))).pos();

					DxLib::DrawCapsule3D(Pos.get(), (Pos + Vector3DX::down() * (m_RopePer * Scale3DRate)).get(), 0.05f * Scale3DRate, 4, GetColor(32, 32, 32), GetColor(32, 32, 32), TRUE);
				}
			}
			void				Dispose_Sub(void) noexcept override {
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heli))->StopAll();
			}
		};
	};
};
