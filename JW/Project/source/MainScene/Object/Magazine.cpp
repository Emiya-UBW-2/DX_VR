#include	"Magazine.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MagazineClass::FirstExecute(void) noexcept {
			if (m_IsHand) {
				switch (m_ReloadTypeBuf) {
				case RELOADTYPE::MAG:
				{
					{
						auto tmp = GetMove();
						SetMove(
							MATRIX_ref::RotX(deg2rad(-30.f*this->HandPer))*tmp.mat.GetRot(),
							Lerp(tmp.pos, this->HandMatrix.pos(), this->HandPer));
					}
				}
				break;
				case RELOADTYPE::AMMO:
				{
				}
				break;
				default:
					break;
				}
			}
			else {
				if (IsActive()) {
					this->m_move.repos = this->m_move.pos;
					this->m_move.pos += this->m_move.vec*60.f / FPS + VECTOR_ref::up()*this->m_yAdd;
					this->m_yAdd += (M_GR / (FPS*FPS));

					if ((this->GetMove().pos - this->GetMove().repos).y() < 0.f) {
						auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);
						for (auto& C : MainGB->GetBuildCol()) {
							auto Vec = (C.GetMatrix().pos() - this->GetMove().pos); Vec.y(0.f);
							if (Vec.Length() <= 2.f*Scale_Rate) {
								auto HitResult = C.GetColLine(this->GetMove().repos, this->GetMove().pos);
								if (HitResult.HitFlag == TRUE) {
									this->m_move.pos = HitResult.HitPosition;
									VECTOR_ref Normal = HitResult.Normal;
									this->m_move.vec = Normal * ((Normal).dot(this->m_move.vec*-1.f))*1.5f;

									//this->m_move.mat = MATRIX_ref::RotVec2(VECTOR_ref::right(), Normal);
									if (Normal.y() > 0.5f) {
										this->m_yAdd = 0.f;
										this->m_move.vec.clear();
									}
									if (m_SoundSwitch) {
										m_SoundSwitch = false;
										auto SE = SoundPool::Instance();
										SE->Get((int)SoundEnum::CateFall).Play_3D(0, this->m_move.pos, Scale_Rate * 3.f);
									}
									break;
								}
							}
						}
					}
					this->m_move.pos.y(std::max(this->m_move.pos.y(), 0.f));


					auto BB = (this->m_move.pos - this->m_move.repos).Norm();
					if ((this->m_move.pos - this->m_move.repos).y() <= 0.f) {
						BB *= -1.f;
					}

					UpdateMove();
					if (this->m_Timer > 12.f) {
						this->m_IsActive = false;
					}
					this->m_Timer += 1.f / FPS;
				}
			}
			//‹¤’Ê
			ObjectBaseClass::FirstExecute();
		}
	};
};
