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
		class FallObjClass : public ObjectBaseClass {
			float m_yAdd{ 0.f };
			float m_Timer{ 0.f };
			bool m_SoundSwitch{ false };
			FallObjectType m_FallObjectType{};
			bool m_GrenadeBombFlag{ false };
		public:
			FallObjClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::FallObj); }
			~FallObjClass(void) noexcept {}
		public:
			bool			PopGrenadeBombSwitch() noexcept;
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
	};
};
