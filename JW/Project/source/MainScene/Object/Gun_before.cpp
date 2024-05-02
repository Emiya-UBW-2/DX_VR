#include	"Gun_before.hpp"
#include "Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool	ModSlotControl::IsEffectParts(GunSlot Slot, GunFrame frame) const noexcept {
			if (this->m_Parts_Ptr[(int)Slot] == nullptr) {
				return false;
			}
			bool Ret = false;
			switch (Slot) {
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
						case GunFrame::Sight:
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
			if (Ret) {
				auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[(int)Slot]);
				if (Obj->HaveFrame(frame)) {
					return true;
				}
			}
			return false;
		}
		const bool ModSlotControl::HasFrameBySlot(GunFrame frame) const noexcept {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					return true;
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇóDêÊ
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->HasFrameBySlot(frame)) {
						return true;
					}
				}
			}
			return false;
		}
		const bool	ModSlotControl::GetPartsFrameLocalMatBySlot(GunFrame frame, Matrix4x4DX* pRet) const noexcept {
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
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetPartsFrameLocalMatBySlot(frame, pRet)) {
						Ret = true;
					}
				}
			}
			return Ret;
		}
		const bool	ModSlotControl::GetPartsFrameWorldMat(GunFrame frame, Matrix4x4DX* pRet) const noexcept {
			bool Ret = false;
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					auto& m = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					*pRet = m->GetFrameWorldMat(frame);
					if (frame == GunFrame::Sight) {
						if (m->GetChildFramesNum(frame) > 0) {
							Vector3DX vec = (m->GetChildFrameWorldMat(frame, 0).pos() - pRet->pos()).normalized();
							//Vector3DX::Cross(pRet->xvec(), vec)
							*pRet *= Matrix4x4DX::RotVec2(pRet->yvec(), vec);
						}
					}
					Ret = true;
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇóDêÊ
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetPartsFrameWorldMat(frame, pRet)) {
						Ret = true;
					}
				}
			}
			return Ret;
		}

		void		ModSlotControl::GetChildPartsList(std::vector<const SharedObj*>* Ret) const noexcept {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					Ret->emplace_back(&this->m_Parts_Ptr[loop]);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetChildPartsList(Ret);
				}
			}
		}
		void		ModSlotControl::ResetPartsFrameLocalMat(GunFrame frame) noexcept {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->ResetFrameLocalMat(frame);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->ResetPartsFrameLocalMat(frame);
				}
			}
		}
		void		ModSlotControl::SetPartsFrameLocalMat(GunFrame frame, const Matrix4x4DX&value) noexcept {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->SetFrameLocalMat(frame, value);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->SetPartsFrameLocalMat(frame, value);
				}
			}
		}
		void		ModSlotControl::SetActiveBySlot(bool value) noexcept {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->SetActive(value);
				}
			}
			//ë∑Ç™Ç†ÇÍÇŒÇªÇøÇÁÇ‡
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->SetActiveBySlot(value);
				}
			}
		}

		void		ModSlotControl::RemoveMod(GunSlot Slot) noexcept {
			if (this->m_Parts_Ptr[(int)Slot]) {
				auto* ObjMngr = ObjectManager::Instance();
				this->m_Parts_Ptr[(int)Slot]->Dispose();
				ObjMngr->DelObj(&this->m_Parts_Ptr[(int)Slot]);
				this->m_Parts_Ptr[(int)Slot].reset();
			}
		}

		void		ModSlotControl::UpdatePartsAnim(const MV1& pParent) {
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (this->m_Parts_Ptr[loop]) {
					auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					for (int i = 0; i < Obj->GetObj().get_anime().size(); i++) {
						Obj->GetAnime((GunAnimeID)i).per = pParent.getanime(i).per;
						Obj->GetAnime((GunAnimeID)i).time = pParent.getanime(i).time;
					}
					Obj->ResetFrameLocalMat(GunFrame::Center);
					Obj->GetObj().work_anime();
					Obj->SetFrameLocalMat(GunFrame::Center, Obj->GetFrameLocalMat(GunFrame::Center).rotation());//1ÇÃÉtÉåÅ[ÉÄà⁄ìÆó Çñ≥éãÇ∑ÇÈ
				}
			}
		}
		void		ModSlotControl::UpdatePartsMove(const Matrix4x4DX& pMat, GunSlot Slot) {
			if (this->m_Parts_Ptr[(int)Slot]) {
				auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[(int)Slot]);
				Obj->SetModMatrix(pMat);
			}
		}

		const SharedObj&		ModSlotControl::SetMod(GunSlot Slot, int ID, const MV1& BaseModel) noexcept {
			RemoveMod(Slot);
			auto* Slots = this->m_ModDataClass->GetPartsSlot(Slot);
			if (Slots) {
				if (this->m_Parts_Ptr[(int)Slot] == nullptr) {
					auto* ObjMngr = ObjectManager::Instance();

					ObjType Type = ObjType::Magazine;
					switch (Slot) {
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
					auto& Data = *ModDataManager::Instance()->GetData(Slots->m_ItemsUniqueID.at(ID));
					ObjMngr->LoadObjectModel((*Ptr).get(), Data->GetPath().c_str());
					MV1::SetAnime(&(*Ptr)->GetObj(), BaseModel);
					this->m_Parts_Ptr[(int)Slot] = (std::shared_ptr<ModClass>&)(*Ptr);
					(*Ptr)->Init();
				}
			}

			return this->m_Parts_Ptr[(int)Slot];
		}
	};
};
