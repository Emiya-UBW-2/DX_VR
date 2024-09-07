#include	"FallObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			FallObjClass::SetFall(const Vector3DX& pos, const Matrix3x3DX& mat, const Vector3DX& vec, float timer, SoundEnum sound) noexcept {
			this->m_IsActive = true;
			this->m_yAdd = 0.001f;
			this->m_SoundSwitch = true;
			this->m_Timer = timer;

			this->m_move.SetAll(pos, pos, pos, vec, mat, mat);
			this->m_CallSound = sound;
		}
		void			FallObjClass::FirstExecute(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			if (this->m_IsActive) {
				this->m_move.SetPos(this->m_move.GetPos() + this->GetMove().GetVec() *60.f / DrawParts->GetFps() + Vector3DX::up()*this->m_yAdd);
				if (this->m_yAdd != 0.f) {
					this->m_yAdd += (M_GR / (DrawParts->GetFps()*DrawParts->GetFps()));
				}
				if ((this->GetMove().GetPos() - this->GetMove().GetRePos()).y < 0.f) {
					Vector3DX EndPos = this->GetMove().GetPos();
					Vector3DX Normal;
					if (this->m_BackGround->CheckLinetoMap(this->GetMove().GetRePos(), &EndPos, false, &Normal)) {
						this->m_move.SetPos(EndPos);
						this->m_move.SetVec(Vector3DX::Reflect(this->GetMove().GetVec(), Normal));
						if (Normal.y > 0.5f) {
							//this->m_yAdd = 0.f;
						}
						if (m_SoundSwitch) {
							m_SoundSwitch = false;
							SoundPool::Instance()->Get((int)this->m_CallSound).Play_3D(0, this->m_move.GetPos(), Scale_Rate * 3.f);
						}
					}
				}
				Vector3DX pos = this->m_move.GetPos();
				pos.y = (std::max(this->m_move.GetPos().y, 0.f));
				this->m_move.SetPos(pos);

				if (this->m_yAdd != 0.f) {
					auto BB = (this->m_move.GetPos() - this->m_move.GetRePos()).normalized();
					if ((this->m_move.GetPos() - this->m_move.GetRePos()).y <= 0.f) {
						BB *= -1.f;
					}
					this->m_move.SetMat(Matrix3x3DX::RotAxis(Vector3DX::Cross(BB, this->m_move.GetMat().zvec()), deg2rad(-50.f * 60.f / DrawParts->GetFps())) * this->m_move.GetMat());
				}

				SetMove().Update(0.f, 0.f);
				if (this->m_Timer < 0.f) {
					this->m_IsActive = false;
				}
				this->m_Timer -= 1.f / DrawParts->GetFps();
				//‹¤’Ê
				SetMove().Update(0.f, 0.f);
			}
		}
	};
};
