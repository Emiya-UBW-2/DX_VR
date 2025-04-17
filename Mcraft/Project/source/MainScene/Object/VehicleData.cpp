#include "VehicleData.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Objects {
		void FootWorld::cat_frame::FrameExecute(MV1* pTargetObj) noexcept {
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			if (this->m_frame.GetFrameID() >= 0) {
				auto y_vec = pTargetObj->GetMatrix().yvec();
				pTargetObj->ResetFrameUserLocalMatrix(this->m_frame.GetFrameID());
				auto startpos = pTargetObj->GetFramePosition(this->m_frame.GetFrameID());
				auto pos_t1 = startpos + y_vec * ((-this->m_frame.GetFrameWorldPosition().pos().y) + 2.f * Scale3DRate);
				auto pos_t2 = startpos + y_vec * ((-this->m_frame.GetFrameWorldPosition().pos().y) - 0.3f * Scale3DRate);
				auto ColRes = BackGroundParts->CheckLinetoMap(pos_t1, &pos_t2);
				this->m_Res_y = (ColRes) ? pos_t2.y : (std::numeric_limits<float>::max)();
				pTargetObj->SetFrameLocalMatrix(this->m_frame.GetFrameID(),
					Matrix4x4DX::Mtrans(Vector3DX::up() * ((ColRes) ? (this->m_Res_y + y_vec.y * this->m_frame.GetFrameWorldPosition().pos().y - startpos.y) : -0.4f * Scale3DRate)) *
					Matrix4x4DX::Mtrans(this->m_frame.GetFrameWorldPosition().pos())
				);
			}
		}
	}
}