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
			if (!pReady) { return; }
			//“ü—Í
			this->m_Input = pInput;
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
			SetVec(0, this->m_Input.GetPADSPress(PADS::MOVE_W));
			SetVec(1, this->m_Input.GetPADSPress(PADS::MOVE_A));
			SetVec(2, this->m_Input.GetPADSPress(PADS::MOVE_S));
			SetVec(3, this->m_Input.GetPADSPress(PADS::MOVE_D));
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

		void MagStockControl::Init_MagStockControl(int AmmoNum, int ModUniqueID) noexcept {
			size_t Total = m_MagazineStock.size();
			for (size_t i = 0; i < Total; i++) {
				m_MagazineStock[i].AmmoNum = AmmoNum;
				m_MagazineStock[i].ModUniqueID = ModUniqueID;
			}
			m_UseMagazineID = 0;

		}
		void MagStockControl::SetNextMag(int OLDAmmoNum, int OLDModUniqueID) noexcept {
			m_MagazineStock[m_UseMagazineID].AmmoNum = OLDAmmoNum;
			m_MagazineStock[m_UseMagazineID].ModUniqueID = OLDModUniqueID;
			m_UseMagazineID = GetNextMagID();
		}
		void MagStockControl::SortMag() noexcept {
			m_UseMagazineID = 0;
			std::sort(m_MagazineStock.begin(), m_MagazineStock.end(), [&](const MagStock&A, const MagStock&B) {return A.AmmoNum > B.AmmoNum; });
		}

	};
};
