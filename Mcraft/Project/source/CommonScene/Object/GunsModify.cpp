#include	"GunsModify.hpp"
#include	"../../sub.hpp"

const FPS_n2::Sceneclass::GunsModify* SingletonBase<FPS_n2::Sceneclass::GunsModify>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			GunsModify::SetMods(ModSlotControl* pMod, const std::unique_ptr<Slot>* pParentSlot) {
			//自分の下の世代のオブジェをセット
			for (auto& data : this->m_SelectPartsData) {
				if (data->m_ParentSlot == pParentSlot) {
					const auto& Data = pMod->GetModData()->GetPartsSlot(data->m_SlotType);
					if (Data && data->m_sel < static_cast<int>(Data->m_ItemsUniqueID.size())) {
						if (data->m_selectSwitch) {
							data->m_selectSwitch = false;
							pMod->SetMod(data->m_SlotType, data->m_sel, this->m_BaseObj);
						}
					}
					else {
						pMod->RemoveMod(data->m_SlotType);
					}
				}
			}
		}
		void			GunsModify::UpdateMods(ModSlotControl* pMod, const std::unique_ptr<Slot>* pParentSlot, bool isPreset) noexcept {
			if (!pMod) { return; }
			//AddSlot
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				if (pMod->GetModData()->GetPartsSlot((GunSlot)loop)) {
					this->m_SelectPartsData.emplace_back(std::make_unique<Slot>());
					auto& data = this->m_SelectPartsData.back();
					data->m_SlotType = (GunSlot)loop;
					data->m_ParentSlot = pParentSlot;
					data->m_Data = pMod;
					data->m_selectSwitch = true;
					const auto& Data = pMod->GetModData()->GetPartsSlot(data->m_SlotType);
					data->m_sel = Data->m_IsNeed ? 0 : static_cast<int>(Data->m_ItemsUniqueID.size());
					if (isPreset) {
						for (const auto& S : m_SlotSave) {
							if (data->m_ParentSlot) {
								if (!S.IsParent((*data->m_ParentSlot)->m_SlotType, (*data->m_ParentSlot)->m_sel)) { continue; }
							}
							else {
								if (!S.IsParentNone()) { continue; }
							}
							if (data->m_SlotType == S.m_SlotType) {
								data->m_sel = S.m_sel;
								break;
							}
						}
					}
				}
			}
			//
			SetMods(pMod, pParentSlot);
			//
			for (int loop = 0; loop < static_cast<int>(this->m_SelectPartsData.size()); ++loop) {
				auto& data = this->m_SelectPartsData[loop];
				if (data->m_ParentSlot == pParentSlot) {
					if (!pMod->GetPartsPtr(data->m_SlotType)) { return; }
					UpdateMods(&((std::unique_ptr<ModClass>&)(pMod->GetPartsPtr(data->m_SlotType)))->SetModSlot(), &data, isPreset);
				}
			}
		}
		//
		void			GunsModify::CreateSelData(const std::shared_ptr<GunClass>& GunPtr, bool isPreset) noexcept {
			this->m_BaseObj = GunPtr;
			UpdateMods(&GunPtr->SetModSlot(), nullptr, isPreset);
		}
		bool			GunsModify::ChangeSelData(const std::unique_ptr<Slot>* pParentSlot, int sel, bool isDeleteSlot) noexcept {
			for (int loop = 0; loop < static_cast<int>(this->m_SelectPartsData.size()); ++loop) {
				const auto& data = this->m_SelectPartsData[loop];
				if (&data != pParentSlot) {
					if (data->m_ParentSlot == pParentSlot) {
						if (ChangeSelData(&data, static_cast<int>(data->m_Data->GetModData()->GetPartsSlot(data->m_SlotType)->m_ItemsUniqueID.size()), true)) { --loop; }
					}
				}
			}
			bool Ret = false;
			for (int loop = 0; loop < static_cast<int>(this->m_SelectPartsData.size()); ++loop) {
				const auto& data = this->m_SelectPartsData[loop];
				if (&data == pParentSlot) {
					data->m_sel = sel;
					data->m_selectSwitch = true;
					SetMods(data->m_Data, data->m_ParentSlot);
					if (sel == static_cast<int>(data->m_Data->GetModData()->GetPartsSlot(data->m_SlotType)->m_ItemsUniqueID.size())) {
						//削除
						if (isDeleteSlot) {
							this->m_SelectPartsData[loop].reset();
							this->m_SelectPartsData.erase(this->m_SelectPartsData.begin() + loop);
						}
					}
					else {
						UpdateMods(&((std::shared_ptr<ModClass>&)(data->m_Data->GetPartsPtr(data->m_SlotType)))->SetModSlot(), &data, false);
					}
					Ret = true;
					break;
				}
			}
			return Ret;
		}
		void			GunsModify::LoadSlots(const char* path) noexcept {
			SlotSaveData Tmp;
			m_SlotSave.clear();
			std::ifstream inputfile(path, std::ios::binary);
			if (inputfile) {
				{
					int ULTSel = 0;
					inputfile.read(reinterpret_cast<char*>(&ULTSel), sizeof(ULTSel));
				}
				while (!inputfile.eof()) { //ファイルの最後まで続ける
					inputfile.read(reinterpret_cast<char*>(&Tmp), sizeof(Tmp));
					m_SlotSave.emplace_back(Tmp);
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
			for (auto& data : this->m_SelectPartsData) {
				Tmp.m_SlotType = data->m_SlotType;
				Tmp.m_sel = data->m_sel;
				Tmp.m_ParentSlotType = (data->m_ParentSlot) ? (*data->m_ParentSlot)->m_SlotType : GunSlot::Gun;
				Tmp.m_Parentsel = (data->m_ParentSlot) ? (*data->m_ParentSlot)->m_sel : 0;
				outputfile.write(reinterpret_cast<char*>(&Tmp), sizeof(Tmp));
			}
			outputfile.close();
		}
	};
};
