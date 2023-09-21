#pragma once
#include	"../../Header.hpp"
#include "ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CartClass : public ObjectBaseClass {
			float m_yAdd{ 0.f };
			float m_Timer{ 0.f };
			bool m_SoundSwitch{ false };
		public:
			CartClass(void) noexcept { this->m_objType = ObjType::Cart; }
			~CartClass(void) noexcept { }
		public:
			void Set(const VECTOR_ref& pos, const MATRIX_ref& mat, const VECTOR_ref& vec) noexcept {
				this->m_IsActive = true;
				this->m_move.pos = pos;
				this->m_move.vec = vec;
				this->m_yAdd = 0.f;
				this->m_move.repos = this->m_move.pos;
				this->m_move.mat = mat;
				this->m_Timer = 0.f;
				this->m_SoundSwitch = true;
			}
		public:
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				{
					this->m_Timer = 2.f;
					this->m_IsActive = false;
				}
			}
			void			FirstExecute(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-0.1f*Scale_Rate, -0.1f*Scale_Rate, -0.1f*Scale_Rate)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(0.1f*Scale_Rate, 0.1f*Scale_Rate, 0.1f*Scale_Rate)).get()) == FALSE
						) {
						if (!isDrawSemiTrans) {
							this->m_obj.DrawModel();
						}
					}
				}
			}
			void			Dispose(void) noexcept override {
			}
		};
	};
};
