#include	"ItemObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			ItemObjClass::SetFall(const Vector3DX& pos, const Vector3DX& vec) noexcept {
			this->m_IsActive = true;
			this->m_yAdd = 0.001f;

			this->m_move.pos = pos;
			this->m_move.vec = vec;
			this->m_move.repos = this->m_move.pos;
			this->m_move.mat = Matrix4x4DX::identity();
		}
		void			ItemObjClass::FirstExecute(void) noexcept {
			if (this->m_IsActive) {
				if (this->m_yAdd != 0.f) {
					this->m_move.repos = this->m_move.pos;
					this->m_move.pos += this->m_move.vec*60.f / FPS + Vector3DX::up()*this->m_yAdd;
					auto Y = this->m_move.vec.y;
					Easing(&this->m_move.vec, Vector3DX::zero(), 0.95f, EasingType::OutExpo);
					this->m_move.vec.y = (Y);

					this->m_yAdd += (M_GR / (FPS*FPS));

					Vector3DX EndPos = this->GetMove().pos;
					Vector3DX Normal;
					if (this->m_BackGround->CheckLinetoMap(this->GetMove().repos, &EndPos, false, &Normal)) {
						this->m_move.pos = EndPos;
						this->m_move.vec += Normal * (Vector3DX::Dot(Normal, this->m_move.vec*-1.f))*2.f;
						if (Normal.y > 0.5f) {
							this->m_yAdd *= -0.5f;
						}
					}
					this->m_move.pos.y = (std::max(this->m_move.pos.y, 0.f));
					if (this->m_move.pos.y == 0.f) {
						this->m_yAdd = 0.f;
					}
				}

				this->m_move.mat = Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(1.f*60.f / FPS))*this->m_move.mat;
				UpdateMove();
			}
		}
	};
};
