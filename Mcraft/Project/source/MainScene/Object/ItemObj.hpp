#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Objects {
		class ItemObj : public BaseObject {
			float		m_yAdd{ 0.0f };
			Pendulum2D	m_Zrotate;
			float		m_Yrad{};
			Matrix3x3DX	m_Rot;
			Vector3DX	m_Pos;
			Vector3DX	m_Repos;
			bool		m_IsLR{};
		public:
			ItemObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::ItemObj); }
			virtual ~ItemObj(void) noexcept {}
		public:
			void				Spawn(const Vector3DX& pos) noexcept {
				SetMove().SetPos(pos);
				SetMove().Update(0.0f, 0.0f);
				m_Zrotate.Init(0.08f * Scale3DRate, 3.0f, deg2rad(50));
				this->m_Yrad = deg2rad(GetRandf(360.f));
			}
		public:
			void				Init_Sub(void) noexcept override {
				SetActive(true);
				SetMinAABB(Vector3DX::vget(-1.f, 0.f, -1.f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(1.f, 1.f, 1.f) * Scale3DRate);
			}
			void				FirstUpdate(void) noexcept override;
			void				DrawShadow(void) noexcept override {
				if (!IsActive()) { return; }
				if ((GetMove().GetPos() - Camera3D::Instance()->GetMainCamera().GetCamPos()).sqrMagnitude() > (10.0f * Scale3DRate * 10.0f * Scale3DRate)) { return; }
				GetObj().DrawModel();
			}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!IsActive()) { return; }
				if (!IsDraw(Range)) { return; }
				if (isDrawSemiTrans) { return; }
				if ((GetMove().GetPos() - Camera3D::Instance()->GetMainCamera().GetCamPos()).sqrMagnitude() > (10.0f * Scale3DRate * 10.0f * Scale3DRate)) { return; }
				GetObj().DrawModel();
			}
			void			Dispose_Sub(void) noexcept override {}
		};
	}
}
