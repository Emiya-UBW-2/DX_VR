#include	"Character_before.hpp"
#include	"Character.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void HitBoxControl::UpdataHitBox(const ObjectBaseClass* ptr, float SizeRate) noexcept {
			auto* Ptr = (CharacterClass*)ptr;
			int ID = 0;
			auto headpos = Ptr->GetEyeMatrix().pos();
			m_HitBox[ID].Execute(headpos, 0.13f*Scale_Rate*SizeRate, HitType::Head); ID++;
			m_HitBox[ID].Execute((headpos + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::Upper)).pos()) / 2.f, 0.16f*Scale_Rate*SizeRate, HitType::Body); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::Upper)).pos(), 0.13f*Scale_Rate*SizeRate, HitType::Body); ID++;

			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::Upper)).pos() + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot1)).pos()) / 2.f, 0.13f*Scale_Rate*SizeRate, HitType::Body); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::Upper)).pos() + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot1)).pos()) / 2.f, 0.13f*Scale_Rate*SizeRate, HitType::Body); ID++;

			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightArm)).pos() + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightArm2)).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightArm2)).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightWrist)).pos() + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightArm2)).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightWrist)).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;

			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftArm)).pos() + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftArm2)).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftArm2)).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftWrist)).pos() + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftArm2)).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftWrist)).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;

			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot1)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot1)).pos() + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot2)).pos()) / 2.f, 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot2)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot)).pos()*0.25f + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot2)).pos()*0.75f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot)).pos()*0.5f + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot2)).pos()*0.5f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot)).pos()*0.75f + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot2)).pos()*0.25f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::RightFoot)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;

			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot1)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot1)).pos() + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot2)).pos()) / 2.f, 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot2)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot)).pos()*0.25f + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot2)).pos()*0.75f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot)).pos()*0.5f + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot2)).pos()*0.5f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot)).pos()*0.75f + Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot2)).pos()*0.25f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(Ptr->GetFrame((int)CharaFrame::LeftFoot)).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
		}
	};
};
