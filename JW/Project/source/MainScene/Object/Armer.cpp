#include	"Armer.hpp"

#include "../../ObjectManager.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void ArmerClass::Init(void) noexcept {
			ObjectBaseClass::Init();
		}
		void ArmerClass::FirstExecute(void) noexcept {
			auto SE = SoundPool::Instance();
			if (this->m_IsFirstLoop) {
			}
			else {
			}
		}
		void ArmerClass::Draw(bool isDrawSemiTrans) noexcept {
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
		void ArmerClass::Dispose(void) noexcept {
			ObjectBaseClass::Dispose();
		}
	};
};
