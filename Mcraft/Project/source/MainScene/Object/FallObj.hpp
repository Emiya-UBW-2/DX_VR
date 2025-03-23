#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class FallObjClass : public ObjectBaseClass {
			float m_yAdd{ 0.f };
			float m_Timer{ 0.f };
			bool m_SoundSwitch{ false };
			SoundEnum m_CallSound{ SoundEnum::CartFall };
			bool m_IsGrenade{ false };
			int m_BoundCount{ 0 };

			EffectControl m_EffectControl;
		public:
			FallObjClass(void) noexcept { this->m_objType = (int)ObjType::FallObj; }
			~FallObjClass(void) noexcept {}
		public:
			void			SetFall(const Vector3DX& pos, const Matrix3x3DX& mat, const Vector3DX& vec, float timer, SoundEnum sound, bool IsGrenade) noexcept;
		public:
			void			Init_Sub(void) noexcept override {
				this->m_IsActive = false;
				m_EffectControl.Init();
			}
			void			FirstExecute(void) noexcept override;
			void			CheckDraw(void) noexcept override {
				m_IsDraw = true;
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (!isDrawSemiTrans) {
						this->m_obj.DrawModel();
					}
				}
			}
			void			Dispose_Sub(void) noexcept override {
				m_EffectControl.Dispose();
			}
		};
	};
};
