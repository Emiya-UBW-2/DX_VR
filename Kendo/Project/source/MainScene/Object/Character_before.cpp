#include	"Character_before.hpp"
#include	"Character.hpp"
#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		void HitBoxControl::Update(const ObjectBaseClass* ptr, float SizeRate) noexcept {
			auto* Ptr = (CharacterClass*)ptr;
			size_t ID = 0;
			m_HitBox[ID].Update(Ptr->GetEyePosition(), 0.2f * Scale3DRate * SizeRate, HitType::Head); ID++;
			m_HitBox[ID].Update(Ptr->GetFramePosition(CharaFrame::Upper), 0.18f * Scale3DRate * SizeRate, HitType::Body); ID++;

			m_HitBox[ID].Update((Ptr->GetFramePosition(CharaFrame::RightWrist) + Ptr->GetFramePosition(CharaFrame::RightArm2)) / 2.f, 0.1f * Scale3DRate * SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Update(Ptr->GetFramePosition(CharaFrame::RightWrist), 0.1f * Scale3DRate * SizeRate, HitType::Arm); ID++;

			m_HitBox[ID].Update((Ptr->GetFramePosition(CharaFrame::LeftWrist) + Ptr->GetFramePosition(CharaFrame::LeftArm2)) / 2.f, 0.1f * Scale3DRate * SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Update(Ptr->GetFramePosition(CharaFrame::LeftWrist), 0.1f * Scale3DRate * SizeRate, HitType::Arm); ID++;
		}
	}
}
