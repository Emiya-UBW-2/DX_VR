#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class FallObjClass : public ObjectBaseClass {
			float m_yAdd{0.f};
			float m_Timer{0.f};
			bool m_SoundSwitch{false};
			SoundEnum m_CallSound{SoundEnum::CartFall};
		private:
			std::shared_ptr<BackGroundClassBase>		m_BackGround;				//BG
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept { this->m_BackGround = backGround; }
		public:
			FallObjClass(void) noexcept { this->m_objType = (int)ObjType::FallObj; }
			~FallObjClass(void) noexcept {}
		public:
			void			SetFall(const Vector3DX& pos, const Matrix4x4DX& mat, const Vector3DX& vec, float timer, SoundEnum sound) noexcept;
		public:
			void			Init_Sub(void) noexcept override {
				this->m_IsActive = false;
			}
			void			FirstExecute(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-0.1f*Scale_Rate, -0.1f*Scale_Rate, -0.1f*Scale_Rate)).get(),
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(0.1f*Scale_Rate, 0.1f*Scale_Rate, 0.1f*Scale_Rate)).get()) == FALSE
						) {
						if (!isDrawSemiTrans) {
							this->m_obj.DrawModel();
						}
					}
				}
			}
			void			Dispose_Sub(void) noexcept override {
				this->m_BackGround.reset();
			}
		};
	};
};
