#include	"FallObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			FallObjClass::SetFall(const Vector3DX& pos, const Matrix4x4DX& mat, const Vector3DX& vec, float timer, SoundEnum sound) noexcept {
			this->m_IsActive = true;
			this->m_yAdd = 0.001f;
			this->m_SoundSwitch = true;
			this->m_Timer = timer;

			this->m_move.pos = pos;
			this->m_move.vec = vec;
			this->m_move.repos = this->m_move.pos;
			this->m_move.mat = mat;
			this->m_CallSound = sound;
		}
		void			FallObjClass::FirstExecute(void) noexcept {
			if (this->m_IsActive) {
				this->m_move.repos = this->m_move.pos;
				this->m_move.pos += this->m_move.vec*60.f / FPS + Vector3DX::up()*this->m_yAdd;
				if (this->m_yAdd != 0.f) {
					this->m_yAdd += (M_GR / (FPS*FPS));
				}
				if ((this->GetMove().pos - this->GetMove().repos).y < 0.f) {
					Vector3DX EndPos = this->GetMove().pos;
					Vector3DX Normal;
					if (this->m_BackGround->CheckLinetoMap(this->GetMove().repos, &EndPos, false, &Normal)) {
						this->m_move.pos = EndPos;
						this->m_move.vec = Vector3DX::Reflect(this->m_move.vec, Normal);
						if (Normal.y > 0.5f) {
							//this->m_yAdd = 0.f;
						}
						if (m_SoundSwitch) {
							m_SoundSwitch = false;
							SoundPool::Instance()->Get((int)this->m_CallSound).Play_3D(0, this->m_move.pos, Scale_Rate * 3.f);
						}
					}
				}
				this->m_move.pos.y = (std::max(this->m_move.pos.y, 0.f));

				if (this->m_yAdd != 0.f) {
					auto BB = (this->m_move.pos - this->m_move.repos).normalized();
					if ((this->m_move.pos - this->m_move.repos).y <= 0.f) {
						BB *= -1.f;
					}
					this->m_move.mat = Matrix4x4DX::RotAxis(Vector3DX::Cross(BB, this->m_move.mat.zvec()), deg2rad(-50.f*60.f / FPS))*this->m_move.mat;
				}

				UpdateMove();
				if (this->m_Timer < 0.f) {
					this->m_IsActive = false;
				}
				this->m_Timer -= 1.f / FPS;
				//‹¤’Ê
				UpdateMove();
			}
		}
	};
};
