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
			int m_SpawnPoint{};
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
				return (this->m_HelicopterMove == HelicopterMove::Rappelling) && (13.f <= this->m_Timer && this->m_Timer <= 30.f) && (this->m_SpawnPoint > 0);
			}
			const auto		PopSpawnPoint() noexcept {
				auto Answer = this->m_SpawnPoint;
				--this->m_SpawnPoint;
				return Answer;
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
				SetMinAABB(Vector3DX::vget(-6.5f, -6.5f, -6.5f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(6.5f, 6.5f, 6.5f) * Scale3DRate);
				SetActive(true);
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heli))->Play3D(GetMove().GetPos(), 50.f * Scale3DRate, DX_PLAYTYPE_LOOP);
				m_PosR = Vector3DX::vget(0.f, -20.f, 0.f) * Scale3DRate;
				m_Yrad = 0.f;
				m_YradR = 0.f;
				m_NowPos = Vector3DX::vget(0.f, 0.f, 0.f) * Scale3DRate;
				SetAction(HelicopterMove::Random);
			}
			void				FirstUpdate(void) noexcept override;
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

					DxLib::DrawCapsule3D(Pos.get(), (Pos - GetMove().GetMat().yvec() * (m_RopePer * Scale3DRate)).get(), 0.05f * Scale3DRate, 4, GetColor(32, 32, 32), GetColor(32, 32, 32), TRUE);

					Pos = GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(HeliFrame::Rappelling2))).pos();

					DxLib::DrawCapsule3D(Pos.get(), (Pos - GetMove().GetMat().yvec() * (m_RopePer * Scale3DRate)).get(), 0.05f * Scale3DRate, 4, GetColor(32, 32, 32), GetColor(32, 32, 32), TRUE);
				}
			}
			void				Dispose_Sub(void) noexcept override {
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heli))->StopAll();
			}
		};
	};
};
