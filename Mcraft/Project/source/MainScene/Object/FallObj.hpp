#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Objects {
		enum class FallObjectType {
			Cart,
			Magazine,
			Grenade,
		};

		class FallObjChildBase {
		public:
			FallObjChildBase(void) noexcept {}
			virtual ~FallObjChildBase(void) noexcept {}
		public:
			virtual SoundEnum GetFallSound(void) const noexcept = 0;
		public:
			virtual void RotateOnAir(moves* objMove) noexcept = 0;
			virtual void RotateOnGround(moves* objMove) noexcept = 0;
			virtual void OnTimeEnd(const moves& objMove) noexcept = 0;
		public:
			virtual bool IsDrawFar(void) const noexcept = 0;
		};

		class FallObj : public BaseObject {
			float	m_yAdd{ 0.f };
			float	m_Timer{ 0.f };
			bool	m_SoundSwitch{ false };
			std::unique_ptr<FallObjChildBase>	m_FallObject{};
			bool	m_IsEndFall{ false };
		public:
			FallObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::FallObj); }
			virtual ~FallObj(void) noexcept {}
		public:
			bool			PopIsEndFall(void) noexcept {
				if (this->m_IsEndFall) {
					this->m_IsEndFall = false;
					return true;
				}
				return false;
			}
			void			SetFall(const Vector3DX& pos, const Matrix3x3DX& mat, const Vector3DX& vec, float timer, FallObjectType Type) noexcept;
		public:
			void				Init_Sub(void) noexcept override {
				SetActive(false);
				SetMinAABB(Vector3DX::vget(-0.1f, -0.1f, -0.1f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(0.1f, 0.1f, 0.1f) * Scale3DRate);
			}
			void				FirstUpdate(void) noexcept override;
			void				DrawShadow(void) noexcept override {
				if (!IsActive()) { return; }
				if ((GetMove().GetPos() - Camera3D::Instance()->GetMainCamera().GetCamPos()).sqrMagnitude() > (10.f * Scale3DRate * 10.f * Scale3DRate)) { return; }
				GetObj().DrawModel();
			}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!IsActive()) { return; }
				if (!IsDraw(Range)) { return; }
				if (isDrawSemiTrans) { return; }
				if (!this->m_FallObject->IsDrawFar()) {
					if ((GetMove().GetPos() - Camera3D::Instance()->GetMainCamera().GetCamPos()).sqrMagnitude() > (10.f * Scale3DRate * 10.f * Scale3DRate)) { return; }
				}
				GetObj().DrawModel();
			}
			void			Dispose_Sub(void) noexcept override {}
		};

		class AmmoInChamberObj : public BaseObject {
		public:
			AmmoInChamberObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::AmmoInChamber); }
			virtual ~AmmoInChamberObj(void) noexcept {}
		public:
			void			SetMat(const Vector3DX& pos, const Matrix3x3DX& mat) noexcept {
				SetMove().SetAll(pos, pos, pos, Vector3DX::zero(), mat, mat);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
		public:
			void				Init_Sub(void) noexcept override {
				SetMinAABB(Vector3DX::vget(-0.1f, -0.1f, -0.1f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(0.1f, 0.1f, 0.1f) * Scale3DRate);
				SetActive(true);
			}
			void				FirstUpdate(void) noexcept override{}
			void				DrawShadow(void) noexcept override {
				if (!IsActive()) { return; }
				if ((GetMove().GetPos() - Camera3D::Instance()->GetMainCamera().GetCamPos()).sqrMagnitude() > (10.f * Scale3DRate * 10.f * Scale3DRate)) { return; }
				GetObj().DrawModel();
			}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!IsActive()) { return; }
				if (!IsDraw(Range)) { return; }
				if (isDrawSemiTrans) { return; }
				if ((GetMove().GetPos() - Camera3D::Instance()->GetMainCamera().GetCamPos()).sqrMagnitude() > (10.f * Scale3DRate * 10.f * Scale3DRate)) { return; }
				GetObj().DrawModel();
			}
			void			Dispose_Sub(void) noexcept override {}
		};
	}
}
