#include	"Item.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			 ItemClass::FirstExecute(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			if (this->m_CanMove) {
				this->m_move.repos = this->m_move.pos;
				this->m_move.Update_Physics(0.f, 1.f);
				//AmmoClass
				Vector3DX repos_tmp = this->m_move.repos + Vector3DX::up()*Scale_Rate*2.f;
				Vector3DX pos_tmp = this->m_move.pos;
				Vector3DX norm_tmp;
				bool ColRes = this->m_BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, true, false, &norm_tmp);
				if (ColRes) {//�Œ艻
					this->m_move.mat *= Matrix4x4DX::RotVec2(this->m_move.mat.yvec(), Lerp(this->m_move.mat.yvec(), norm_tmp, std::powf(0.95f, 60.f / DrawParts->GetFps())));
					this->m_move.pos = pos_tmp;
					this->m_move.vec.y = (0.f);
					this->m_CanMove = false;
				}
				else {
					bool ColVeh = false;
					for (int i = 0; i < Player_num; i++) {
						auto& tgt = PlayerMngr->GetPlayer(i).GetVehicle();
						if (tgt->CheckLine(repos_tmp, &pos_tmp, &norm_tmp)) {
							ColVeh |= true;
							this->m_move.vec.Set(0.f, 0.f, 0.f);
							this->m_move.vec.x += (GetRandf(2.5f*Scale_Rate / DrawParts->GetFps()));
							this->m_move.vec.z += (GetRandf(2.5f*Scale_Rate / DrawParts->GetFps()));

							this->m_move.mat *= Matrix4x4DX::RotAxis(Vector3DX::right(), GetRandf(deg2rad(90)));
							this->m_move.mat *= Matrix4x4DX::RotAxis(Vector3DX::up(), GetRandf(deg2rad(90)));
							this->m_move.mat *= Matrix4x4DX::RotAxis(Vector3DX::forward(), GetRandf(deg2rad(90)));
						}
					}
					if (ColVeh) {
						this->m_move.mat *= Matrix4x4DX::RotVec2(this->m_move.mat.yvec(), Lerp(this->m_move.mat.yvec(), norm_tmp, std::powf(0.95f, 60.f / DrawParts->GetFps())));
						this->m_move.pos = pos_tmp;
						this->m_move.vec.y = (0.f);
					}
					if (!ColVeh) {
						auto recY = this->m_move.vec.y;
						Easing(&this->m_move.vec, Vector3DX::zero(), 0.95f, EasingType::OutExpo);
						this->m_move.vec.y = (recY);
					}
				}
				UpdateMove();
			}
		}
	};
};
