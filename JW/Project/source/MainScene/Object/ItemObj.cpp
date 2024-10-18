#include	"ItemObj.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			ItemObjClass::SetFall(const Vector3DX& pos, const Vector3DX& vec) noexcept {
			this->m_IsActive = true;
			this->m_yAdd = 0.001f;

			this->SetMove().SetAll(pos, pos, pos, vec, Matrix3x3DX::identity(), Matrix3x3DX::identity());
		}
		void			ItemObjClass::FirstExecute(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			if (this->m_IsActive) {
				if (this->m_yAdd != 0.f) {
					this->SetMove().SetPos(this->GetMove().GetPos() + this->GetMove().GetVec() * 60.f / DrawParts->GetFps() + Vector3DX::up() * this->m_yAdd);

					auto Y = this->GetMove().GetVec().y;
					Vector3DX vec = this->GetMove().GetVec();
					Easing(&vec, Vector3DX::vget(0.f, Y, 0.f), 0.95f, EasingType::OutExpo);
					this->SetMove().SetVec(vec);

					this->m_yAdd += (GravityRate / (DrawParts->GetFps()*DrawParts->GetFps()));

					Vector3DX EndPos = this->GetMove().GetPos();
					Vector3DX Normal;
					if (this->m_BackGround->CheckLinetoMap(this->GetMove().GetRePos(), &EndPos, false, &Normal)) {
						this->SetMove().SetPos(EndPos);
						this->SetMove().SetVec(this->GetMove().GetVec() + Normal * (Vector3DX::Dot(Normal, this->GetMove().GetVec() * -1.f)) * 2.f);
						if (Normal.y > 0.5f) {
							this->m_yAdd *= -0.5f;
						}
					}
					Vector3DX pos = this->GetMove().GetPos();
					pos.y = (std::max(this->GetMove().GetPos().y, 0.f));
					this->SetMove().SetPos(pos);
					if (this->GetMove().GetPos().y == 0.f) {
						this->m_yAdd = 0.f;
					}
				}

				this->SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(1.f * 60.f / DrawParts->GetFps())) * this->GetMove().GetMat());
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
		}
	};
};
