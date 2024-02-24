#include	"GunsModify.hpp"
#include "../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			GunsModify::SetMods(ModSlotControl* ModPtr, const Slot* SlotPtr) {
			//自分の下の世代のオブジェをセット
			for (auto& y : SelData) {
				if (y->ParentSlot == SlotPtr) {
					const auto& Data = ModPtr->GetModData()->GetPartsSlot(y->SlotType);
					if (Data && y->m_sel < (int)Data->m_ItemsUniqueID.size()) {
						if (y->m_selectSwitch) {
							y->m_selectSwitch = false;
							ModPtr->RemoveMod(y->SlotType);
							ModPtr->SetMod(y->SlotType, y->m_sel, *m_BaseObj);
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
			for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
				if (ModPtr->GetModData()->GetPartsSlot((GunSlot)loop)) {
					SelData.emplace_back(new Slot);
					auto& y = SelData.back();
					y->SlotType = (GunSlot)loop;
					y->ParentSlot = SlotPtr;
					y->m_Data = ModPtr;
					y->m_selectSwitch = true;
					const auto& Data = ModPtr->GetModData()->GetPartsSlot(y->SlotType);
					y->m_sel = Data->m_IsNeed ? 0 : (int)Data->m_ItemsUniqueID.size();
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
			for (int loop = 0; loop < (int)SelData.size(); loop++) {
				const Slot* y = SelData[loop];
				if (y->ParentSlot == SlotPtr) {
					UpdateMods(((std::shared_ptr<ModClass>&)(ModPtr->GetSlotControl()->GetPartsPtr(y->SlotType))).get(), y, isPreset);
				}
			}
		}
		//
		void			GunsModify::CreateSelData(const std::shared_ptr<GunClass>& GunPtr, bool isPreset) {
			m_BaseObj = &GunPtr->GetObj();
			UpdateMods(GunPtr.get(), nullptr, isPreset);
		}
		bool			GunsModify::ChangeSelData(const Slot* SlotPtr, int sel, bool isDeleteSlot) {
			for (int loop = 0; loop < (int)SelData.size(); loop++) {
				const auto& y = SelData[loop];
				if (y != SlotPtr) {
					if (y->ParentSlot == SlotPtr) {
						if (ChangeSelData(y, (int)y->m_Data->GetModData()->GetPartsSlot(y->SlotType)->m_ItemsUniqueID.size(), true)) { loop--; }
					}
				}
			}
			bool Ret = false;
			for (int loop = 0; loop < (int)SelData.size(); loop++) {
				const auto& y = SelData[loop];
				if (y == SlotPtr) {
					y->m_sel = sel;
					y->m_selectSwitch = true;
					SetMods(y->m_Data, y->ParentSlot);
					if (sel == (int)y->m_Data->GetModData()->GetPartsSlot(y->SlotType)->m_ItemsUniqueID.size()) {
						//削除
						if (isDeleteSlot) {
							delete SelData[loop];
							SelData.erase(SelData.begin() + loop);
						}
					}
					else {
						UpdateMods(((std::shared_ptr<ModClass>&)(y->m_Data->GetSlotControl()->GetPartsPtr(y->SlotType))).get(), y, false);
					}
					Ret = true;
					break;
				}
			}
			return Ret;
		}
		void			GunsModify::LoadSlots(const char* path) {
			SlotSaveData Tmp;
			SlotSave.clear();
			std::ifstream inputfile(path, std::ios::binary);
			while (!inputfile.eof()) {  //ファイルの最後まで続ける
				inputfile.read(reinterpret_cast<char *>(&Tmp), sizeof(Tmp));
				SlotSave.emplace_back(Tmp);
			}
			inputfile.close();
		}
		void			GunsModify::SaveSlots(const char* path) {
			SlotSaveData Tmp;
			std::ofstream outputfile(path, std::ios::binary);
			for (auto& y : SelData) {
				Tmp.SlotType = y->SlotType;
				Tmp.m_sel = y->m_sel;
				Tmp.ParentSlotType = (y->ParentSlot) ? y->ParentSlot->SlotType : GunSlot::Gun;
				Tmp.m_Parentsel = (y->ParentSlot) ? y->ParentSlot->m_sel : 0;
				outputfile.write(reinterpret_cast<char *>(&Tmp), sizeof(Tmp));
			}
			outputfile.close();
		}
	};
};
