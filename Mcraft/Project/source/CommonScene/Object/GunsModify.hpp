#pragma once
#include	"../../Header.hpp"

#include "../../MainScene/Object/Gun.hpp"
#include "../../MainScene/Object/ModData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunsModify : public SingletonBase<GunsModify> {
		private:
			friend class SingletonBase<GunsModify>;
		private:
			GunsModify(void) noexcept {}
			virtual ~GunsModify(void) noexcept {}
		public:
			struct Slot {
				GunSlot			SlotType{ GunSlot::Magazine };
				int				m_sel{ 0 };
				bool			m_selectSwitch{ 0 };
				const Slot*		ParentSlot{ nullptr };
				ModSlotControl*	m_Data{ nullptr };
			};
		private:
			struct SlotSaveData {
				GunSlot SlotType{ GunSlot::Magazine };
				int		m_sel{ 0 };
				GunSlot ParentSlotType{ GunSlot::Gun };
				int		m_Parentsel{ 0 };
			public:
				const auto IsParentNone(void) const noexcept { return (ParentSlotType == GunSlot::Gun); }
				const auto IsParent(GunSlot Slot_t, int sel_t) const noexcept { return (ParentSlotType == Slot_t) && (this->m_Parentsel == sel_t); }
			};
		private:
			std::vector<std::shared_ptr<Slot>>	SelData;
			std::vector<SlotSaveData>	SlotSave;
			SharedObj		 			m_BaseObj{ nullptr };
		public:
			const auto& GetSelData(void) const noexcept { return SelData; }
		private:
			void			SetMods(ModSlotControl* ModPtr, const Slot* SlotPtr);
			void			UpdateMods(ModSlotControl* ModPtr, const Slot* SlotPtr, bool isPreset) noexcept;
		public:
			void			CreateSelData(const std::shared_ptr<GunClass>& GunPtr, bool isPreset) noexcept;
			bool			ChangeSelData(const Slot* SlotPtr, int sel, bool isDeleteSlot) noexcept;
		public:
			void			LoadSlots(const char* path) noexcept;
			void			SaveSlots(const char* path) noexcept;
		public:
			void			DisposeSlots(void) noexcept {
				for (auto& y : SelData) {
					y.reset();
				}
				SelData.clear();
				this->m_BaseObj.reset();
			}
		};
	};
};
