#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class FallObjectType {
			Cart,
			Magazine,
			Grenade,
		};

		class FallObjChildBase {
		public:
			FallObjChildBase() {}
			virtual ~FallObjChildBase() {}
		public:
			virtual SoundEnum GetFallSound(void) const noexcept = 0;
		public:
			virtual void RotateOnAir(moves* objMove) noexcept = 0;
			virtual void RotateOnGround(moves* objMove) noexcept = 0;
			virtual void OnTimeEnd(const moves& objMove) noexcept = 0;
		};

		class FallObjClass : public ObjectBaseClass {
			float m_yAdd{ 0.f };
			float m_Timer{ 0.f };
			bool m_SoundSwitch{ false };
			std::unique_ptr<FallObjChildBase> m_FallObject{};
			bool m_IsEndFall{ false };
		public:
			FallObjClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::FallObj); }
			~FallObjClass(void) noexcept {}
		public:
			bool			PopIsEndFall() noexcept {
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
			}
			void				FirstExecute(void) noexcept override;
			void				DrawShadow(void) noexcept override {
				if (!IsActive()) { return; }

				auto* CameraParts = Camera3D::Instance();
				if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }

				GetObj().DrawModel();
			}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!IsActive()) { return; }
				if (!IsDraw(Range)) { return; }
				if (isDrawSemiTrans) { return; }

				auto* CameraParts = Camera3D::Instance();
				if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }

				GetObj().DrawModel();
			}
			void			Dispose_Sub(void) noexcept override {}
		};

		class AmmoInChamberClass : public ObjectBaseClass {
		public:
			AmmoInChamberClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::AmmoInChamber); }
			~AmmoInChamberClass(void) noexcept {}
		public:
			void			SetMat(const Vector3DX& pos, const Matrix3x3DX& mat) noexcept {
				SetMove().SetAll(pos, pos, pos, Vector3DX::zero(), mat, mat);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
		public:
			void				Init_Sub(void) noexcept override {
				ObjectBaseClass::SetMinAABB(Vector3DX::vget(-1.f, -1.f, -1.f) * Scale3DRate);
				ObjectBaseClass::SetMaxAABB(Vector3DX::vget(1.f, 1.f, 1.f) * Scale3DRate);
				SetActive(true);
			}
			void				FirstExecute(void) noexcept override{}
			void				DrawShadow(void) noexcept override {
				if (!IsActive()) { return; }
				auto* CameraParts = Camera3D::Instance();
				if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }
				GetObj().DrawModel();
			}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!IsActive()) { return; }
				if (!IsDraw(Range)) { return; }
				if (isDrawSemiTrans) { return; }
				auto* CameraParts = Camera3D::Instance();
				if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }
				GetObj().DrawModel();
			}
			void			Dispose_Sub(void) noexcept override {}
		};
	};
};
