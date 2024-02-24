#include	"Gun_before.hpp"
#include "Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool	SlotPartsControl::IsEffectParts(GunSlot objType, GunFrame frame) const noexcept {
			bool Ret = false;
			if (HasParts(objType)) {
				switch (objType) {
					case GunSlot::Magazine:
						break;
					case GunSlot::Lower:
						switch (frame) {
							case GunFrame::Magpos:
							case GunFrame::LeftHandPos:
							case GunFrame::LeftHandYvec:
							case GunFrame::LeftHandZvec:
							case GunFrame::RightHandPos:
							case GunFrame::RightHandYvec:
							case GunFrame::RightHandZvec:
								Ret = true;
								break;
							default:
								break;
						}
						break;
					case GunSlot::Upper:
						switch (frame) {
							case GunFrame::Cart:
							case GunFrame::CartVec:
							case GunFrame::Eyepos:
							case GunFrame::Lens:
							case GunFrame::LensSize:
								Ret = true;
								break;
							default:
								break;
						}
						break;
					case GunSlot::Barrel:
						switch (frame) {
							case GunFrame::Muzzle:
								Ret = true;
								break;
							default:
								break;
						}
						break;
					case GunSlot::UnderRail:
						switch (frame) {
							case GunFrame::Eyepos:
							case GunFrame::Lens:
							case GunFrame::LensSize:
							case GunFrame::LaserSight:
								Ret = true;
								break;
							default:
								break;
						}
						break;
					case GunSlot::Sight:
						switch (frame) {
							case GunFrame::Eyepos:
							case GunFrame::Lens:
							case GunFrame::LensSize:
								Ret = true;
								break;
							default:
								break;
						}
						break;
					case GunSlot::MuzzleAdapter:
						switch (frame) {
							case GunFrame::Muzzle:
								Ret = true;
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}
			}
			if (Ret) {
				auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[(int)objType]);
				if (Obj->HaveFrame(frame)) {
					return true;
				}
			}
			return false;
		}
		const bool  SlotPartsControl::HasFrame(GunFrame frame) const noexcept {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					return true;
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇóDêÊ
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetSlotControl()->HasFrame(frame)) {
						return true;
					}
				}
			}
			return false;
		}
		const bool	SlotPartsControl::GetPartsFrameLocalMat(GunFrame frame, MATRIX_ref* pRet) const noexcept {
			bool Ret = false;
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					*pRet = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetFrameLocalMat(frame);
					Ret = true;
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇóDêÊ
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetSlotControl()->GetPartsFrameLocalMat(frame, pRet)) {
						Ret = true;
					}
				}
			}
			return Ret;
		}
		const bool	SlotPartsControl::GetPartsFrameWorldMat(GunFrame frame, MATRIX_ref* pRet) const noexcept {
			bool Ret = false;
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					*pRet = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetFrameWorldMat(frame);
					Ret = true;
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇóDêÊ
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetSlotControl()->GetPartsFrameWorldMat(frame, pRet)) {
						Ret = true;
					}
				}
			}
			return Ret;
		}

		void		SlotPartsControl::GetChildPartsList(std::vector<const SharedObj*>* Ret) const noexcept {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					Ret->emplace_back(&this->m_Parts_Ptr[loop]);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetSlotControl()->GetChildPartsList(Ret);
				}
			}
		}
		void		SlotPartsControl::ResetPartsFrameLocalMat(GunFrame frame) noexcept {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->ResetFrameLocalMat(frame);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetSlotControl()->ResetPartsFrameLocalMat(frame);
				}
			}
		}
		void		SlotPartsControl::SetPartsFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->SetFrameLocalMat(frame, value);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetSlotControl()->SetPartsFrameLocalMat(frame, value);
				}
			}
		}
		void		SlotPartsControl::SetActive(bool value) noexcept {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (HasParts((GunSlot)loop)) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->SetActive(value);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetSlotControl()->SetActive(value);
				}
			}
		}

		const SharedObj*	SlotPartsControl::SetParts(int uniqueID, GunSlot objType, const MV1& BaseModel) {
			if (!this->m_Parts_Ptr[(int)objType]) {
				auto* ObjMngr = ObjectManager::Instance();

				ObjType Type = ObjType::Magazine;
				switch (objType) {
					case GunSlot::Magazine:				Type = ObjType::Magazine;			break;
					case GunSlot::Upper:				Type = ObjType::Upper;				break;
					case GunSlot::Lower:				Type = ObjType::Lower;				break;
					case GunSlot::Barrel:				Type = ObjType::Barrel;				break;
					case GunSlot::UnderRail:			Type = ObjType::UnderRail;			break;
					case GunSlot::Sight:				Type = ObjType::Sight;				break;
					case GunSlot::MuzzleAdapter:		Type = ObjType::MuzzleAdapter;		break;
					default: break;
				}

				auto* Ptr = ObjMngr->MakeObject(Type);
				auto& Data = *ModDataManager::Instance()->GetData(uniqueID);
				ObjMngr->LoadObjectModel((*Ptr).get(), Data->GetPath().c_str());
				MV1::SetAnime(&(*Ptr)->GetObj(), BaseModel);
				this->m_Parts_Ptr[(int)objType] = (std::shared_ptr<ModClass>&)(*Ptr);
				return Ptr;
			}
			return nullptr;
		}
		void		SlotPartsControl::RemoveParts(GunSlot objType) {
			if (this->m_Parts_Ptr[(int)objType]) {
				auto* ObjMngr = ObjectManager::Instance();
				auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[(int)objType]);
				Obj->Dispose();
				ObjMngr->DelObj(&this->m_Parts_Ptr[(int)objType]);
				this->m_Parts_Ptr[(int)objType].reset();
			}
		}

		void		SlotPartsControl::UpdatePartsAnim(const MV1& pParent) {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					for (int i = 0; i < this->m_Parts_Ptr[loop]->GetObj().get_anime().size(); i++) {
						Obj->GetAnime((GunAnimeID)i).per = pParent.getanime(i).per;
						Obj->GetAnime((GunAnimeID)i).time = pParent.getanime(i).time;
					}
					Obj->ResetFrameLocalMat(GunFrame::Center);
					this->m_Parts_Ptr[loop]->GetObj().work_anime();
					Obj->SetFrameLocalMat(GunFrame::Center, Obj->GetFrameLocalMat(GunFrame::Center).GetRot());//1ÇÃÉtÉåÅ[ÉÄà⁄ìÆó Çñ≥éãÇ∑ÇÈ
				}
			}
		}
		void		SlotPartsControl::UpdatePartsMove(const MATRIX_ref& pMat, GunSlot objType) {
			if (this->m_Parts_Ptr[(int)objType]) {
				auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[(int)objType]);
				Obj->SetGunMatrix(pMat);
			}
		}
		void		SlotPartsControl::DisposeSlotPartsControl() {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				RemoveParts((GunSlot)loop);
			}
		}
	};
};
