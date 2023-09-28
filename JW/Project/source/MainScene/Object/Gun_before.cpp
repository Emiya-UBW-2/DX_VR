#include	"Gun_before.hpp"
#include "Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool	SlotPartsControl::IsEffectParts(ObjType objType, GunFrame frame) const noexcept {
			bool Ret = false;
			if (HasParts(objType)) {
				switch (objType) {
				case ObjType::Magazine:
					break;
				case ObjType::Lower:
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
				case ObjType::Upper:
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
				case ObjType::Barrel:
					switch (frame) {
					case GunFrame::Muzzle:
						Ret = true;
						break;
					default:
						break;
					}
					break;
				case ObjType::UnderRail:
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
				case ObjType::Sight:
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
				default:
					break;
				}
			}
			if (Ret) {
				auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[(int)objType]);
				if (Obj->GetFrame(frame) != 0) {
					return true;
				}
			}
			return false;
		}
		const bool	SlotPartsControl::GetPartsFrameLocalMat(GunFrame frame, MATRIX_ref* pRet) const noexcept {
			bool Ret = false;
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
				if (IsEffectParts((ObjType)loop, frame)) {
					*pRet = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetFrameLocalMat(frame);
					Ret = true;
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇóDêÊ
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModControl().GetPartsFrameLocalMat(frame, pRet)) {
						Ret = true;
					}
				}
			}
			return Ret;
		}
		const bool	SlotPartsControl::GetPartsFrameWorldMat(GunFrame frame, MATRIX_ref* pRet) const noexcept {
			bool Ret = false;
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
				if (IsEffectParts((ObjType)loop, frame)) {
					*pRet = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetFrameWorldMat(frame);
					Ret = true;
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇóDêÊ
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModControl().GetPartsFrameWorldMat(frame, pRet)) {
						Ret = true;
					}
				}
			}
			return Ret;
		}

		void		SlotPartsControl::GetChildPartsList(std::vector<const std::shared_ptr<ObjectBaseClass>*>* Ret) const noexcept {
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					Ret->emplace_back(&this->m_Parts_Ptr[loop]);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModControl().GetChildPartsList(Ret);
				}
			}
		}
		void		SlotPartsControl::ResetPartsFrameLocalMat(GunFrame frame) noexcept {
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
				if (IsEffectParts((ObjType)loop, frame)) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->ResetFrameLocalMat(frame);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModControl().ResetPartsFrameLocalMat(frame);
				}
			}
		}
		void		SlotPartsControl::SetPartsFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept {
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
				if (IsEffectParts((ObjType)loop, frame)) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->SetFrameLocalMat(frame, value);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModControl().SetPartsFrameLocalMat(frame, value);
				}
			}
		}
		void		SlotPartsControl::SetParts(std::string pPath, ObjType objType, const MV1& BaseModel) {
			if (!this->m_Parts_Ptr[(int)objType]) {
				auto* ObjMngr = ObjectManager::Instance();
				auto* Ptr = ObjMngr->MakeObject(objType);
				switch (objType) {
				case ObjType::Magazine:
					pPath = "mag/" + pPath;
					break;
				case ObjType::Lower:
					pPath = "Lower/" + pPath;
					break;
				case ObjType::Upper:
					pPath = "Upper/" + pPath;
					break;
				case ObjType::Barrel:
					pPath = "Barrel/" + pPath;
					break;
				case ObjType::UnderRail:
					pPath = "UnderRail/" + pPath;
					break;
				case ObjType::Sight:
					pPath = "Sight/" + pPath;
					break;
				default:
					break;
				}
				pPath = "data/Mods/" + pPath + "/";
				ObjMngr->LoadObjectModel((*Ptr).get(), pPath.c_str());
				MV1::SetAnime(&(*Ptr)->GetObj(), BaseModel);
				this->m_Parts_Ptr[(int)objType] = (std::shared_ptr<ModClass>&)(*Ptr);
				(*Ptr)->Init();
			}
		}
		void		SlotPartsControl::UpdatePartsAnim(const MV1& pParent) {
			for (int loop = 0; loop < (int)ObjType::Max; loop++) {
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
		void		SlotPartsControl::UpdatePartsMove(const MATRIX_ref& pMat, ObjType objType) {
			if (this->m_Parts_Ptr[(int)objType]) {
				auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[(int)objType]);
				Obj->SetGunMatrix(pMat);
			}
		}
		void		SlotPartsControl::DisposeSlotPartsControl() {
			for (auto& p : this->m_Parts_Ptr) {
				p.reset();
			}
		}
	};
};
