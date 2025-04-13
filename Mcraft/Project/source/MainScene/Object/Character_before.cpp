#include	"Character_before.hpp"
#include	"Character.hpp"
#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Charas {
		void HitBoxControl::Update(const BaseObject* ptr, float SizeRate) noexcept {
			SizeRate *= Scale3DRate;
			auto* Ptr = (CharacterObj*)ptr;
			int ID = 0;
			this->m_HitBox[ID].Update(Ptr->GetEyePositionCache(), 0.13f * SizeRate, HitType::Head); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetEyePositionCache() + Ptr->GetFrameWorldMat(CharaFrame::Upper).pos()) / 2.f, 0.16f * SizeRate, HitType::Body); ++ID;
			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat(CharaFrame::Upper).pos(), 0.13f * SizeRate, HitType::Body); ++ID;

			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat(CharaFrame::Upper).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightFoot1)).pos()) / 2.f, 0.13f * SizeRate, HitType::Body); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat(CharaFrame::Upper).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot1)).pos()) / 2.f, 0.13f * SizeRate, HitType::Body); ++ID;

			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightArm)).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightArm2)).pos()) / 2.f, 0.06f * SizeRate, HitType::Arm); ++ID;
			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::RightArm2)).pos(), 0.06f * SizeRate, HitType::Arm); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightWrist)).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightArm2)).pos()) / 2.f, 0.06f * SizeRate, HitType::Arm); ++ID;
			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::RightWrist)).pos(), 0.06f * SizeRate, HitType::Arm); ++ID;

			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftArm)).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftArm2)).pos()) / 2.f, 0.06f * SizeRate, HitType::Arm); ++ID;
			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::LeftArm2)).pos(), 0.06f * SizeRate, HitType::Arm); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftWrist)).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftArm2)).pos()) / 2.f, 0.06f * SizeRate, HitType::Arm); ++ID;
			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::LeftWrist)).pos(), 0.06f * SizeRate, HitType::Arm); ++ID;

			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::RightFoot1)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightFoot1)).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos()) / 2.f, 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos() * 0.25f + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos() * 0.75f), 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos() * 0.5f + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos() * 0.5f), 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos() * 0.75f + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos() * 0.25f), 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;

			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::LeftFoot1)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot1)).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos()) / 2.f, 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos() * 0.25f + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos() * 0.75f), 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos() * 0.5f + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos() * 0.5f), 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos() * 0.75f + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos() * 0.25f), 0.095f * SizeRate, HitType::Leg); ++ID;
			this->m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;
		}
	};
};
