#include	"FallObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			FallObjClass::SetFall(const Vector3DX& pos, const Matrix3x3DX& mat, const Vector3DX& vec, float timer, SoundEnum sound) noexcept {
			this->m_IsActive = true;
			this->m_yAdd = 0.001f;
			this->m_SoundSwitch = true;
			this->m_Timer = timer;

			this->SetMove().SetAll(pos, pos, pos, vec, mat, mat);
			this->m_CallSound = sound;
		}
		void			FallObjClass::FirstExecute(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			if (this->m_IsActive) {
				Vector3DX PosBuf = this->GetMove().GetPos() + this->GetMove().GetVec() * 60.f / DrawParts->GetFps() + Vector3DX::up() * this->m_yAdd;
				if (this->m_yAdd != 0.f) {
					this->m_yAdd += (GravityRate / (DrawParts->GetFps()*DrawParts->GetFps()));
				}
				if ((PosBuf - this->GetMove().GetRePos()).y < 0.f) {
					Vector3DX EndPos = PosBuf;
					Vector3DX Normal;
					if (this->m_BackGround->CheckLinetoMap(this->GetMove().GetRePos(), &EndPos, false, &Normal)) {
						PosBuf = EndPos;
						this->SetMove().SetVec(Vector3DX::Reflect(this->GetMove().GetVec(), Normal));
						if (Normal.y > 0.5f) {
							//this->m_yAdd = 0.f;
						}
						if (m_SoundSwitch) {
							m_SoundSwitch = false;
							SoundPool::Instance()->Get((int)this->m_CallSound).Play_3D(0, PosBuf, Scale_Rate * 3.f);
						}
					}
				}
				PosBuf.y = (std::max(PosBuf.y, 0.f));

				if (this->m_yAdd != 0.f) {
					auto BB = (PosBuf - this->GetMove().GetRePos()).normalized();
					if ((PosBuf - this->GetMove().GetRePos()).y <= 0.f) {
						BB *= -1.f;
					}
					this->SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::Cross(BB, this->GetMove().GetMat().zvec()), deg2rad(-50.f * 60.f / DrawParts->GetFps())) * this->GetMove().GetMat());
				}

				if (this->m_Timer < 0.f) {
					this->m_IsActive = false;
				}
				this->m_Timer -= 1.f / DrawParts->GetFps();
				//����
				this->SetMove().SetPos(PosBuf);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
		}
	};
};
