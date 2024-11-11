#include	"Character_before.hpp"
#include	"Character.hpp"
#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		void HitBoxControl::UpdataHitBox(const ObjectBaseClass* ptr, float SizeRate) noexcept {
			auto* Ptr = (CharacterClass*)ptr;
			size_t ID = 0;
			m_HitBox[ID].Execute(Ptr->GetEyePosition(), 0.2f * Scale3DRate * SizeRate, HitType::Head); ID++;
			m_HitBox[ID].Execute(Ptr->GetObj().GetFrameLocalWorldMatrix(Ptr->GetFrame(static_cast<int>(CharaFrame::Upper))).pos(), 0.18f * Scale3DRate * SizeRate, HitType::Body); ID++;

			m_HitBox[ID].Execute((Ptr->GetObj().GetFrameLocalWorldMatrix(Ptr->GetFrame(static_cast<int>(CharaFrame::RightWrist))).pos() + Ptr->GetObj().GetFrameLocalWorldMatrix(Ptr->GetFrame(static_cast<int>(CharaFrame::RightArm2))).pos()) / 2.f, 0.1f * Scale3DRate * SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetObj().GetFrameLocalWorldMatrix(Ptr->GetFrame(static_cast<int>(CharaFrame::RightWrist))).pos(), 0.1f * Scale3DRate * SizeRate, HitType::Arm); ID++;

			m_HitBox[ID].Execute((Ptr->GetObj().GetFrameLocalWorldMatrix(Ptr->GetFrame(static_cast<int>(CharaFrame::LeftWrist))).pos() + Ptr->GetObj().GetFrameLocalWorldMatrix(Ptr->GetFrame(static_cast<int>(CharaFrame::LeftArm2))).pos()) / 2.f, 0.1f * Scale3DRate * SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetObj().GetFrameLocalWorldMatrix(Ptr->GetFrame(static_cast<int>(CharaFrame::LeftWrist))).pos(), 0.1f * Scale3DRate * SizeRate, HitType::Arm); ID++;
		}
	}
}
