#include	"Judge.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void JudgeClass::CheckDraw(void) noexcept {
			this->m_IsDraw = false;
			this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetScreenPosition()).magnitude();
			if (CheckCameraViewClip_Box(
				(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f * Scale3DRate, -0.f * Scale3DRate, -2.5f * Scale3DRate)).get(),
				(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f * Scale3DRate, 2.5f * Scale3DRate, 2.5f * Scale3DRate)).get()) == FALSE
				) {
				this->m_IsDraw |= true;
			}
		}
		void JudgeClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				for (int i = 0; i < this->GetObj().GetMeshNum(); i++) {
					if (this->GetObj().GetMeshSemiTransState(i) == isDrawSemiTrans) {
						this->GetObj().DrawMesh(i);
					}
				}
			}
		}
		void JudgeClass::DrawShadow(void) noexcept {
			if (this->m_IsActive) {
				this->GetObj().DrawModel();
			}
		}
	};
};
