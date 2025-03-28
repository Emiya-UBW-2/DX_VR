#include	"Gun_before.hpp"
#include "Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool	ModSlotControl::IsEffectParts(GunSlot Slot, GunFrame frame) const noexcept {
			if (this->m_Parts_Ptr[static_cast<int>(Slot)] == nullptr) {
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
				case GunFrame::Light:
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
				auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[static_cast<int>(Slot)]);
				if (Obj->HaveFrame(static_cast<int>(frame))) {
					return true;
				}
			}
			return false;
		}
		const bool ModSlotControl::HasFrameBySlot(GunFrame frame) const noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					return true;
				}
			}
			//孫があればそちらを優先
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModSlot().HasFrameBySlot(frame)) {
						return true;
					}
				}
			}
			return false;
		}
		const bool	ModSlotControl::GetPartsFrameLocalMatBySlot(GunFrame frame, Matrix4x4DX* pRet) const noexcept {
			bool Ret = false;
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					auto& Ptr = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					*pRet = Ptr->GetFrameLocalMat(frame);
					Ret = true;
				}
			}
			//孫があればそちらを優先
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModSlot().GetPartsFrameLocalMatBySlot(frame, pRet)) {
						Ret = true;
					}
				}
			}
			return Ret;
		}
		const bool	ModSlotControl::GetPartsFrameWorldMat(GunFrame frame, Matrix4x4DX* pRet) const noexcept {
			bool Ret = false;
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					auto& m = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					*pRet = m->GetFrameWorldMat_P(frame);
					if (frame == GunFrame::Sight) {
						if (m->GetObj_const().GetFrameChildNum(m->GetFrame(static_cast<int>(frame))) > 0) {
							Vector3DX vec = (m->GetObj_const().GetChildFrameWorldMatrix(m->GetFrame(static_cast<int>(frame)), 0).pos() - pRet->pos()).normalized();
							//Vector3DX::Cross(pRet->xvec(), vec)
							*pRet *= Matrix4x4DX::RotVec2(pRet->yvec(), vec);
						}
					}
					Ret = true;
				}
			}
			//孫があればそちらを優先
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					if (((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModSlot().GetPartsFrameWorldMat(frame, pRet)) {
						Ret = true;
					}
				}
			}
			return Ret;
		}

		void		ModSlotControl::GetChildPartsList(std::vector<const SharedObj*>* Ret) const noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					Ret->emplace_back(&this->m_Parts_Ptr[loop]);
				}
			}
			//孫があればそちらも
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModSlot().GetChildPartsList(Ret);
				}
			}
		}
		void		ModSlotControl::ResetPartsFrameLocalMat(GunFrame frame) noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					auto& Ptr = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					Ptr->GetObj().ResetFrameUserLocalMatrix(Ptr->GetFrame(static_cast<int>(frame)));
				}
			}
			//孫があればそちらも
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModSlot().ResetPartsFrameLocalMat(frame);
				}
			}
		}
		void		ModSlotControl::SetPartsFrameLocalMat(GunFrame frame, const Matrix4x4DX& value) noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (IsEffectParts((GunSlot)loop, frame)) {
					auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					Obj->GetObj().SetFrameLocalMatrix(Obj->GetFrame(static_cast<int>(frame)), value * Obj->GetFrameBaseLocalMat(static_cast<int>(frame)));
				}
			}
			//孫があればそちらも
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModSlot().SetPartsFrameLocalMat(frame, value);
				}
			}
		}
		void		ModSlotControl::SetActiveBySlot(bool value) noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->SetActive(value);
				}
			}
			//孫があればそちらも
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->GetModSlot().SetActiveBySlot(value);
				}
			}
		}

		void		ModSlotControl::RemoveMod(GunSlot Slot) noexcept {
			if (this->m_Parts_Ptr[static_cast<int>(Slot)]) {
				auto* ObjMngr = ObjectManager::Instance();
				ObjMngr->DelObj(&this->m_Parts_Ptr[static_cast<int>(Slot)]);
				this->m_Parts_Ptr[static_cast<int>(Slot)].reset();
			}
		}

		void		ModSlotControl::UpdatePartsAnim(MV1& pParent) {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					//1のフレーム移動量を無視する
					auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					for (int i = 0; i < Obj->GetObj().GetAnimNum(); i++) {
						Obj->GetObj().SetAnim(i).SetPer(pParent.SetAnim(i).GetPer());
						Obj->GetObj().SetAnim(i).SetTime(pParent.SetAnim(i).GetTime());
					}
					Obj->GetObj().ResetFrameUserLocalMatrix(Obj->GetFrame(static_cast<int>(GunFrame::Center)));
					Obj->GetObj().UpdateAnimAll();
					Obj->GetObj().SetFrameLocalMatrix(Obj->GetFrame(static_cast<int>(GunFrame::Center)), Obj->GetFrameLocalMat(GunFrame::Center).rotation() * Obj->GetFrameBaseLocalMat(static_cast<int>(GunFrame::Center)));
				}
			}
		}
		void		ModSlotControl::UpdatePartsMove(const Matrix4x4DX& pMat, GunSlot Slot) {
			if (this->m_Parts_Ptr[static_cast<int>(Slot)]) {
				auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[static_cast<int>(Slot)]);
				Obj->SetModMatrix(Matrix3x3DX::Get33DX(pMat), pMat.pos());
			}
		}

		const SharedObj& ModSlotControl::SetMod(GunSlot Slot, int ID, const SharedObj& BaseModel) noexcept {
			RemoveMod(Slot);
			auto* Slots = this->m_ModDataClass->GetPartsSlot(Slot);
			if (Slots) {
				if (this->m_Parts_Ptr[static_cast<int>(Slot)] == nullptr) {
					auto AddObj = [&](const SharedObj& NewObj) {
						this->m_Parts_Ptr[static_cast<int>(Slot)] = NewObj;
						auto& Data = *ModDataManager::Instance()->GetData(Slots->m_ItemsUniqueID.at(ID));

						auto* ObjMngr = ObjectManager::Instance();
						ObjMngr->AddObject(NewObj);
						ObjMngr->LoadModel(NewObj, BaseModel, Data->GetPath().c_str());
						NewObj->Init();
						};
					switch (Slot) {
					case GunSlot::Magazine:
						AddObj(std::make_shared<MagazineClass>());
						break;
					case GunSlot::Lower:
						AddObj(std::make_shared<LowerClass>());
						break;
					case GunSlot::Upper:
						AddObj(std::make_shared<UpperClass>());
						break;
					case GunSlot::Barrel:
						AddObj(std::make_shared<BarrelClass>());
						break;
					case GunSlot::UnderRail:
						AddObj(std::make_shared<UnderRailClass>());
						break;
					case GunSlot::Sight:
						AddObj(std::make_shared<SightClass>());
						break;
					case GunSlot::MuzzleAdapter:
						AddObj(std::make_shared<MuzzleClass>());
						break;
					default:
						break;
					}
				}
			}

			return this->m_Parts_Ptr[static_cast<int>(Slot)];
		}
	};
};
