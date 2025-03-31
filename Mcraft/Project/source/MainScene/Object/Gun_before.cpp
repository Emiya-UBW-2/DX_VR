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
		const bool ModSlotControl::IsEffectParts(const SharedObj& SlotParts, GunFrame frame) const noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (SlotParts == this->m_Parts_Ptr[loop]) {
					return IsEffectParts((GunSlot)loop, frame);
				}
			}
			return false;
		}
		const void ModSlotControl::CalcAnyBySlot(const std::function<void(const SharedObj&)>& Doing) const noexcept {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				Doing(this->m_Parts_Ptr[loop]);
			}
			//孫があればそちらも
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop]) {
					((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop])->SetModSlot().CalcAnyBySlot(Doing);
				}
			}
		}
		const SharedObj* ModSlotControl::HasFrameBySlot(GunFrame frame) const noexcept {
			const SharedObj* pRet = nullptr;
			CalcAnyBySlot([&](const SharedObj& ptr) { if (IsEffectParts(ptr, frame)) { pRet = &ptr; } });
			return pRet;
		}
		void		ModSlotControl::GetChildPartsList(std::vector<const SharedObj*>* Ret) const noexcept {
			CalcAnyBySlot([&](const SharedObj& ptr) { if (ptr) { Ret->emplace_back(&ptr); } });
		}
		void		ModSlotControl::SetActiveBySlot(bool value) const noexcept {
			CalcAnyBySlot([&](const SharedObj& ptr) { if (ptr) { ((std::shared_ptr<ModClass>&)ptr)->SetActive(value); } });
		}

		void		ModSlotControl::RemoveMod(GunSlot Slot) noexcept {
			if (this->m_Parts_Ptr[static_cast<int>(Slot)]) {
				auto* ObjMngr = ObjectManager::Instance();
				ObjMngr->DelObj(&this->m_Parts_Ptr[static_cast<int>(Slot)]);
				this->m_Parts_Ptr[static_cast<int>(Slot)].reset();
			}
		}

		void		ModSlotControl::UpdatePartsAnim(const MV1& pParent) {
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (this->m_Parts_Ptr[loop] && false) {//現状のカスタム範囲では不要
					//1のフレーム移動量を無視する
					auto& Obj = ((std::shared_ptr<ModClass>&)this->m_Parts_Ptr[loop]);
					for (int i = 0; i < Obj->GetObj_const().GetAnimNum(); i++) {
						Obj->GetObj().SetAnim(i).SetPer(pParent.GetAnim(i).GetPer());
						Obj->GetObj().SetAnim(i).SetTime(pParent.GetAnim(i).GetTime());
					}
					Obj->GetObj().UpdateAnimAll();
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
