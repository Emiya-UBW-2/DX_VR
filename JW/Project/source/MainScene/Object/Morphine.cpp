#include	"Morphine.hpp"

#include "../../ObjectManager.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void MorphineClass::Init(void) noexcept {
			ObjectBaseClass::Init();
		}
		void MorphineClass::FirstExecute(void) noexcept {
			if (this->m_IsFirstLoop) {
			}
			else {
			}
		}
		void MorphineClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-0.5f*Scale_Rate, -0.5f*Scale_Rate, -0.5f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(0.5f*Scale_Rate, 0.5f*Scale_Rate, 0.5f*Scale_Rate)).get()) == FALSE
					) {
					for (int i = 0; i < this->GetObj().mesh_num(); i++) {
						if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
							this->GetObj().DrawMesh(i);
						}
					}
				}
			}
		}
		void MorphineClass::Dispose(void) noexcept {
			ObjectBaseClass::Dispose();
		}
	};
};
