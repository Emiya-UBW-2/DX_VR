#include	"Cart.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			CartClass::FirstExecute(void) noexcept {
			this->m_obj.SetMatrix(this->m_move.MatIn());
			if (this->m_IsActive) {
				this->m_move.repos = this->m_move.pos;
				this->m_move.pos += this->m_move.vec*60.f / FPS + VECTOR_ref::up()*this->m_yAdd;
				this->m_yAdd += (M_GR / (FPS*FPS));

				if ((this->GetMove().pos - this->GetMove().repos).y() < 0.f) {
					auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);
					for (auto& C : MainGB->GetBuildDatas()) {
						auto Vec = (C.GetMatrix().pos() - this->GetMove().pos); Vec.y(0.f);
						if (Vec.Length() <= 2.f*Scale_Rate) {
							auto HitResult = C.GetColLine(this->GetMove().repos, this->GetMove().pos);
							if (HitResult.HitFlag == TRUE) {
								this->m_move.pos = HitResult.HitPosition;
								VECTOR_ref Normal = HitResult.Normal;
								this->m_move.vec += Normal * ((Normal).dot(this->m_move.vec*-1.f))*2.f;
								if (Normal.y() > 0.5f) {
									this->m_yAdd = 0.f;
								}
								if (m_SoundSwitch) {
									m_SoundSwitch = false;
									auto SE = SoundPool::Instance();
									SE->Get((int)SoundEnum::CartFall).Play_3D(0, this->m_move.pos, Scale_Rate * 3.f);
								}
								break;
							}
						}
					}
				}
			}
			auto BB = (this->m_move.pos - this->m_move.repos).Norm();
			if ((this->m_move.pos - this->m_move.repos).y() <= 0.f) {
				BB *= -1.f;
			}
			this->m_move.mat = MATRIX_ref::RotAxis(BB.cross(this->m_move.mat.zvec()), deg2rad(-(20.f + GetRandf(30.f))*60.f / FPS))*this->m_move.mat;

			if (this->m_Timer > 2.f) {
				this->m_IsActive = false;
			}
			this->m_Timer += 1.f / FPS;
			//‹¤’Ê
			ObjectBaseClass::FirstExecute();
		}
	};
};
