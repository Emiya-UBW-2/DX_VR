#include	"Morphine.hpp"

#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void MorphineClass::FirstExecute(void) noexcept {
			if (this->m_IsFirstLoop) {
			}
			else {
			}
		}
		void MorphineClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-0.5f*Scale_Rate, -0.5f*Scale_Rate, -0.5f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(0.5f*Scale_Rate, 0.5f*Scale_Rate, 0.5f*Scale_Rate)).get()) == FALSE
					) {
					for (int i = 0; i < this->GetObj().GetMeshNum(); i++) {
						if (this->GetObj().GetMeshSemiTransState(i) == isDrawSemiTrans) {
							this->GetObj().DrawMesh(i);
						}
					}
				}
			}
		}
	};
};
