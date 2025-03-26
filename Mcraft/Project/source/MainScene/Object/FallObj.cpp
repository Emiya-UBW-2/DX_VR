#include	"FallObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		bool FallObjClass::PopGrenadeBombSwitch() {
			if (m_GrenadeBombFlag) {
				m_GrenadeBombFlag = false;
				return true;
			}
			return false;
		}
		void			FallObjClass::SetFall(const Vector3DX& pos, const Matrix3x3DX& mat, const Vector3DX& vec, float timer, SoundEnum sound, bool IsGrenade) noexcept {
			this->m_IsActive = true;
			this->m_yAdd = 0.001f;
			this->m_SoundSwitch = true;
			this->m_Timer = timer;
			this->m_IsGrenade = IsGrenade;
			this->m_GrenadeBombFlag = false;
			this->m_BoundCount = 0;

			this->SetMove().SetAll(pos, pos, pos, vec, mat, mat);
			this->m_CallSound = sound;
			SetMove().Update(0.f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
		}
		void			FallObjClass::FirstExecute(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			if (this->m_IsActive) {
				Vector3DX PosBuf = this->GetMove().GetPos() + this->GetMove().GetVec() * 60.f * DXLib_refParts->GetDeltaTime() + Vector3DX::up() * this->m_yAdd;
				if (this->m_yAdd != 0.f) {
					this->m_yAdd += (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));
				}
				//if ((PosBuf - this->GetMove().GetRePos()).y < 0.f) 
				{
					Vector3DX EndPos = PosBuf;
					Vector3DX Normal;
					if (BackGround->CheckLinetoMap(this->GetMove().GetRePos(), &EndPos, &Normal)) {
						PosBuf = EndPos + Normal * (0.5f * Scale3DRate);
						if (std::abs(Normal.y) > 0.5f) {
							m_BoundCount++;
						}
						Vector3DX Vec = Vector3DX::Reflect(this->GetMove().GetVec(), Normal);
						Vec *= 0.5f;
						this->SetMove().SetVec(Vec);
						this->m_yAdd = 0.001f;
						if (m_SoundSwitch) {
							m_SoundSwitch = false;
							SoundPool::Instance()->Get(SoundType::SE, (int)this->m_CallSound)->Play3D(PosBuf, Scale3DRate * 3.f);
						}
					}
				}
				//PosBuf.y = (std::max(PosBuf.y, 0.f));

				if (this->m_yAdd != 0.f) {
					auto BB = (PosBuf - this->GetMove().GetRePos()).normalized();
					if ((PosBuf - this->GetMove().GetRePos()).y <= 0.f) {
						BB *= -1.f;
					}
					this->SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::Cross(BB, this->GetMove().GetMat().zvec()), deg2rad(-50.f * 60.f * DXLib_refParts->GetDeltaTime())) * this->GetMove().GetMat());
				}

				if (this->m_Timer < 0.f) {
					if (this->m_IsGrenade) {
						m_GrenadeBombFlag = true;
					}
					this->m_IsActive = false;
				}
				this->m_Timer -= DXLib_refParts->GetDeltaTime();
				//‹¤’Ê
				this->SetMove().SetPos(PosBuf);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
			if (this->m_IsGrenade) {
			}
		}
	};
};
