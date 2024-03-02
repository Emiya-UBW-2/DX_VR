#include	"Character_before.hpp"
#include	"Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void		KeyControl::InitKey(float pxRad, float pyRad) {
			for (int i = 0; i < 4; i++) {
				this->m_Vec[i] = 0.f;
			}
			this->m_Input.SetInputStart(0.f, 0.f, VECTOR_ref::zero());
			this->m_radAdd.clear();
			this->m_rad_Buf.x(pxRad);
			this->m_rad_Buf.y(pyRad);
			this->m_rad = this->m_rad_Buf;

			SetIsSquat(false);
		}
		void		KeyControl::InputKey(const InputControl& pInput, bool pReady, const VECTOR_ref& pAddRadvec) {
			this->m_Input = pInput;
			if (!pReady) {
				this->m_Input.ResetKeyInput();
			}
			//“ü—Í
			this->m_ULTKey.Execute(this->m_Input.GetPADSPress(PADS::ULT));
			this->m_Squat.Execute(this->m_Input.GetPADSPress(PADS::SQUAT));
			if (this->GetRun()) { SetIsSquat(false); }
			//‰ñ“]
			{
				Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);

				this->m_rad_Buf.x(
					std::clamp(
						this->m_rad_Buf.x() + (this->m_Input.GetAddxRad()*(this->m_Input.GetPADSPress(PADS::RUN) ? 0.5f : 1.f)),
						deg2rad(-70.f), deg2rad(24.f))
					+ this->m_radAdd.x()
				);
				this->m_rad_Buf.y(
					this->m_rad_Buf.y() + (this->m_Input.GetAddyRad()*(this->m_Input.GetPADSPress(PADS::RUN) ? 0.5f : 1.f))
					+ this->m_radAdd.y()
				);
				this->m_rad.SetEasingX(m_rad_Buf.x(), 0.5f);
				this->m_rad.SetEasingY(m_rad_Buf.y(), 0.8f);
				this->m_rad.SetEasingZ(m_rad_Buf.z(), 0.5f);
			}
			//ˆÚ“®
			SetVec(
				this->m_Input.GetPADSPress(PADS::MOVE_W),
				this->m_Input.GetPADSPress(PADS::MOVE_A),
				this->m_Input.GetPADSPress(PADS::MOVE_S),
				this->m_Input.GetPADSPress(PADS::MOVE_D));
			//ƒŠ[ƒ“
			if (true) {//ƒgƒOƒ‹Ž®
				this->m_QKey.Execute(this->m_Input.GetPADSPress(PADS::LEAN_L));
				if (this->m_QKey.trigger()) {
					if (this->m_LeanRate == -1) {
						this->m_LeanRate = 1;
					}
					else {
						if (this->m_LeanRate < 1) {
							this->m_LeanRate++;
						}
						else {
							this->m_LeanRate--;
						}
					}
				}
				this->m_EKey.Execute(this->m_Input.GetPADSPress(PADS::LEAN_R));
				if (this->m_EKey.trigger()) {
					if (this->m_LeanRate == 1) {
						this->m_LeanRate = -1;
					}
					else {
						if (this->m_LeanRate > -1) {
							this->m_LeanRate--;
						}
						else {
							this->m_LeanRate++;
						}
					}
				}
			}
			else {
				this->m_LeanRate = 0;
				if (this->m_Input.GetPADSPress(PADS::LEAN_L)) {
					this->m_LeanRate = 1;
				}
				if (this->m_Input.GetPADSPress(PADS::LEAN_R)) {
					this->m_LeanRate = -1;
				}
			}
			this->m_LeanRate = std::clamp(this->m_LeanRate, -1, 1);
		}

		void HitBoxControl::UpdataHitBox(const ObjectBaseClass* ptr, float SizeRate) noexcept {
			auto* Ptr = (CharacterClass*)ptr;
			int ID = 0;
			auto headpos = (Ptr->GetFrameWorldMat(CharaFrame::LeftEye).pos() + Ptr->GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f - Ptr->GetCharaDir().zvec() * -1.f;
			m_HitBox[ID].Execute(headpos, 0.13f*Scale_Rate*SizeRate, HitType::Head); ID++;
			m_HitBox[ID].Execute((headpos + Ptr->GetFrameWorldMat(CharaFrame::Upper).pos()) / 2.f, 0.16f*Scale_Rate*SizeRate, HitType::Body); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::Upper).pos(), 0.13f*Scale_Rate*SizeRate, HitType::Body); ID++;

			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::Upper).pos() + Ptr->GetFrameWorldMat(CharaFrame::RightFoot1).pos()) / 2.f, 0.13f*Scale_Rate*SizeRate, HitType::Body); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::Upper).pos() + Ptr->GetFrameWorldMat(CharaFrame::LeftFoot1).pos()) / 2.f, 0.13f*Scale_Rate*SizeRate, HitType::Body); ID++;

			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::RightArm).pos() + Ptr->GetFrameWorldMat(CharaFrame::RightArm2).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::RightArm2).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::RightWrist).pos() + Ptr->GetFrameWorldMat(CharaFrame::RightArm2).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::RightWrist).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;

			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::LeftArm).pos() + Ptr->GetFrameWorldMat(CharaFrame::LeftArm2).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::LeftArm2).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::LeftWrist).pos() + Ptr->GetFrameWorldMat(CharaFrame::LeftArm2).pos()) / 2.f, 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::LeftWrist).pos(), 0.06f*Scale_Rate*SizeRate, HitType::Arm); ID++;

			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::RightFoot1).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::RightFoot1).pos() + Ptr->GetFrameWorldMat(CharaFrame::RightFoot2).pos()) / 2.f, 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::RightFoot2).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::RightFoot).pos()*0.25f + Ptr->GetFrameWorldMat(CharaFrame::RightFoot2).pos()*0.75f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::RightFoot).pos()*0.5f + Ptr->GetFrameWorldMat(CharaFrame::RightFoot2).pos()*0.5f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::RightFoot).pos()*0.75f + Ptr->GetFrameWorldMat(CharaFrame::RightFoot2).pos()*0.25f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::RightFoot).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;

			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::LeftFoot1).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::LeftFoot1).pos() + Ptr->GetFrameWorldMat(CharaFrame::LeftFoot2).pos()) / 2.f, 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::LeftFoot2).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::LeftFoot).pos()*0.25f + Ptr->GetFrameWorldMat(CharaFrame::LeftFoot2).pos()*0.75f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::LeftFoot).pos()*0.5f + Ptr->GetFrameWorldMat(CharaFrame::LeftFoot2).pos()*0.5f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute((Ptr->GetFrameWorldMat(CharaFrame::LeftFoot).pos()*0.75f + Ptr->GetFrameWorldMat(CharaFrame::LeftFoot2).pos()*0.25f), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Execute(Ptr->GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 0.095f*Scale_Rate*SizeRate, HitType::Leg); ID++;
		}

		void ItemFallControl::Init(const std::shared_ptr<BackGroundClassBase>& backGround, const std::string& pPath, ItemType type) {
			auto* ObjMngr = ObjectManager::Instance();
			for (auto& c : m_Ptr) {
				auto* Ptr = ObjMngr->MakeObject(ObjType::ItemObj);
				ObjMngr->LoadObjectModel((*Ptr).get(), pPath.c_str());
				MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());
				c = (std::shared_ptr<ItemObjClass>&)(*Ptr);
				c->SetMapCol(backGround);
				c->Init();
				c->SetItemType(type);
			}
		}
		void ItemFallControl::SetFall(const VECTOR_ref& pPos, const VECTOR_ref& pVec) {
			this->m_Ptr[this->m_Now]->SetFall(pPos, pVec);
			++this->m_Now %= this->m_Ptr.size();
		}
		void ItemFallControl::Dispose() noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			for (auto& c : m_Ptr) {
				ObjMngr->DelObj((SharedObj*)&c);
				c.reset();
			}
		}
	};
};
