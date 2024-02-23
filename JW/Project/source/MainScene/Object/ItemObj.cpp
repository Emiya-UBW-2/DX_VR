#include	"ItemObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			ItemObjClass::SetFall(const VECTOR_ref& pos, const VECTOR_ref& vec) noexcept {
			this->m_IsActive = true;
			this->m_yAdd = 0.001f;

			this->m_move.pos = pos;
			this->m_move.vec = vec;
			this->m_move.repos = this->m_move.pos;
			this->m_move.mat=MATRIX_ref::zero();
		}
		void			ItemObjClass::FirstExecute(void) noexcept {
			if (this->m_IsActive) {
				this->m_move.repos = this->m_move.pos;
				this->m_move.pos += this->m_move.vec*60.f / FPS + VECTOR_ref::up()*this->m_yAdd;
				auto Y = this->m_move.vec.y();
				Easing(&this->m_move.vec, VECTOR_ref::zero(), 0.95f, EasingType::OutExpo);
				this->m_move.vec.y(Y);

				if (this->m_yAdd != 0.f) {
					this->m_yAdd += (M_GR / (FPS*FPS));
				}
				if ((this->GetMove().pos - this->GetMove().repos).y() < 0.f) {
					VECTOR_ref EndPos = this->GetMove().pos;
					VECTOR_ref Normal;
					if (this->m_BackGround->CheckLinetoMap(this->GetMove().repos, &EndPos, false, &Normal)) {
						this->m_move.pos = EndPos;
						this->m_move.vec += Normal * ((Normal).dot(this->m_move.vec*-1.f))*2.f;
						if (Normal.y() > 0.5f) {
							this->m_yAdd *= -0.5f;
						}
					}
				}
				this->m_move.pos.y(std::max(this->m_move.pos.y(), 0.f));

				this->m_move.mat = MATRIX_ref::RotY(deg2rad(1.f*60.f / FPS))*this->m_move.mat;

				UpdateMove();
				//‹¤’Ê
				ObjectBaseClass::FirstExecute();
				SetMove(GetMove().mat.GetRot(), GetMove().pos);
			}
		}
	};
};
