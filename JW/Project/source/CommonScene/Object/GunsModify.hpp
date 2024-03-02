#pragma once
#include	"../../Header.hpp"

#include "../../MainScene/Object/Gun.hpp"
#include "../../MainScene/Object/ModData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunsModify {
		public:
			struct Slot {
				GunSlot			SlotType{GunSlot::Magazine};
				int				m_sel{0};
				bool			m_selectSwitch{0};
				const Slot*		ParentSlot{nullptr};
				ModSlotControl*	m_Data{nullptr};
			};
		private:
			struct SlotSaveData {
				GunSlot SlotType{GunSlot::Magazine};
				int		m_sel{0};
				GunSlot ParentSlotType{GunSlot::Gun};
				int		m_Parentsel{0};
			public:
				const auto IsParentNone() const noexcept { return (ParentSlotType == GunSlot::Gun); }
				const auto IsParent(GunSlot Slot_t, int sel_t) const noexcept { return (ParentSlotType == Slot_t) && (m_Parentsel == sel_t); }
			};
		private:
			std::vector<std::shared_ptr<Slot>>	SelData;
			ULT_GUN						m_ULTSelect{ULT_GUN::PCC_4};
			std::vector<SlotSaveData>	SlotSave;
			MV1*						m_BaseObj{nullptr};
		public:
			const auto&		GetSelData() const noexcept { return SelData; }
			const auto&		GetULTSelect() const noexcept { return m_ULTSelect; }
		private:
			void			SetMods(ModSlotControl* ModPtr, const Slot* SlotPtr);
			void			UpdateMods(ModSlotControl* ModPtr, const Slot* SlotPtr, bool isPreset) noexcept;
		public:
			void			CreateSelData(const std::shared_ptr<GunClass>& GunPtr, bool isPreset);
			bool			ChangeSelData(const Slot* SlotPtr, int sel, bool isDeleteSlot);

			void			SetULTSelect(ULT_GUN value) noexcept { m_ULTSelect = value; }
		public:
			void			LoadSlots(const char* path);
			void			SaveSlots(const char* path);
		public:
			void			DisposeSlots() {
				for (auto& y : SelData) {
					y.reset();
				}
				SelData.clear();
			}
		};
	};
};
