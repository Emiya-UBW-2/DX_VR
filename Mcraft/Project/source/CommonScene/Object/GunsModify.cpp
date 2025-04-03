#include	"GunsModify.hpp"
#include "../../sub.hpp"

const FPS_n2::Sceneclass::GunsModify* SingletonBase<FPS_n2::Sceneclass::GunsModify>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			GunsModify::SetMods(ModSlotControl* ModPtr, const Slot* SlotPtr) {
			//自分の下の世代のオブジェをセット
			for (auto& y : SelData) {
				if (y->ParentSlot == SlotPtr) {
					const auto& Data = ModPtr->GetModData()->GetPartsSlot(y->SlotType);
					if (Data && y->m_sel < static_cast<int>(Data->m_ItemsUniqueID.size())) {
						if (y->m_selectSwitch) {
							y->m_selectSwitch = false;
							ModPtr->SetMod(y->SlotType, y->m_sel, this->m_BaseObj);
						}
					}
					else {
						ModPtr->RemoveMod(y->SlotType);
					}
				}
			}
		}
		void			GunsModify::UpdateMods(ModSlotControl* ModPtr, const Slot* SlotPtr, bool isPreset) noexcept {
			if (!ModPtr) { return; }
			//AddSlot
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
				if (ModPtr->GetModData()->GetPartsSlot((GunSlot)loop)) {
					SelData.emplace_back(std::make_shared<Slot>());
					auto& y = SelData.back();
					y->SlotType = (GunSlot)loop;
					y->ParentSlot = SlotPtr;
					y->m_Data = ModPtr;
					y->m_selectSwitch = true;
					const auto& Data = ModPtr->GetModData()->GetPartsSlot(y->SlotType);
					y->m_sel = Data->m_IsNeed ? 0 : static_cast<int>(Data->m_ItemsUniqueID.size());
					if (isPreset) {
						for (const auto& S : SlotSave) {
							if (y->ParentSlot) {
								if (!S.IsParent(y->ParentSlot->SlotType, y->ParentSlot->m_sel)) { continue; }
							}
							else {
								if (!S.IsParentNone()) { continue; }
							}
							if (y->SlotType == S.SlotType) {
								y->m_sel = S.m_sel;
								break;
							}
						}
					}
				}
			}
			//
			SetMods(ModPtr, SlotPtr);
			//
			for (int loop = 0; loop < static_cast<int>(SelData.size()); loop++) {
				auto& y = SelData[loop];
				if (y->ParentSlot == SlotPtr) {
					if (!ModPtr->GetPartsPtr(y->SlotType)) { return; }
					UpdateMods(&((std::shared_ptr<ModClass>&)(ModPtr->GetPartsPtr(y->SlotType)))->SetModSlot(), y.get(), isPreset);
				}
			}
		}
		//
		void			GunsModify::CreateSelData(const std::shared_ptr<GunClass>& GunPtr, bool isPreset) noexcept {
			this->m_BaseObj = GunPtr;
			UpdateMods(&GunPtr->SetModSlot(), nullptr, isPreset);
		}
		bool			GunsModify::ChangeSelData(const Slot* SlotPtr, int sel, bool isDeleteSlot) noexcept {
			for (int loop = 0; loop < static_cast<int>(SelData.size()); loop++) {
				const auto& y = SelData[loop];
				if (y.get() != SlotPtr) {
					if (y->ParentSlot == SlotPtr) {
						if (ChangeSelData(y.get(), static_cast<int>(y->m_Data->GetModData()->GetPartsSlot(y->SlotType)->m_ItemsUniqueID.size()), true)) { loop--; }
					}
				}
			}
			bool Ret = false;
			for (int loop = 0; loop < static_cast<int>(SelData.size()); loop++) {
				const auto& y = SelData[loop];
				if (y.get() == SlotPtr) {
					y->m_sel = sel;
					y->m_selectSwitch = true;
					SetMods(y->m_Data, y->ParentSlot);
					if (sel == static_cast<int>(y->m_Data->GetModData()->GetPartsSlot(y->SlotType)->m_ItemsUniqueID.size())) {
						//削除
						if (isDeleteSlot) {
							SelData[loop].reset();
							SelData.erase(SelData.begin() + loop);
						}
					}
					else {
						UpdateMods(&((std::shared_ptr<ModClass>&)(y->m_Data->GetPartsPtr(y->SlotType)))->SetModSlot(), y.get(), false);
					}
					Ret = true;
					break;
				}
			}
			return Ret;
		}
		void			GunsModify::LoadSlots(const char* path) noexcept {
			SlotSaveData Tmp;
			SlotSave.clear();
			std::ifstream inputfile(path, std::ios::binary);
			if (inputfile) {
				{
					int ULTSel = 0;
					inputfile.read(reinterpret_cast<char*>(&ULTSel), sizeof(ULTSel));
				}
				while (!inputfile.eof()) { //ファイルの最後まで続ける
					inputfile.read(reinterpret_cast<char*>(&Tmp), sizeof(Tmp));
					SlotSave.emplace_back(Tmp);
				}
			}
			inputfile.close();
		}
		void			GunsModify::SaveSlots(const char* path) noexcept {
			SlotSaveData Tmp;
			std::ofstream outputfile(path, std::ios::binary);
			{
				int ULTSel = 0;
				outputfile.write(reinterpret_cast<char*>(&ULTSel), sizeof(ULTSel));
			}
			for (auto& y : SelData) {
				Tmp.SlotType = y->SlotType;
				Tmp.m_sel = y->m_sel;
				Tmp.ParentSlotType = (y->ParentSlot) ? y->ParentSlot->SlotType : GunSlot::Gun;
				Tmp.m_Parentsel = (y->ParentSlot) ? y->ParentSlot->m_sel : 0;
				outputfile.write(reinterpret_cast<char*>(&Tmp), sizeof(Tmp));
			}
			outputfile.close();
		}
	};
};
