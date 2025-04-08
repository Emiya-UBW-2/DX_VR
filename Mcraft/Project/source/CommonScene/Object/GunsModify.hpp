#pragma once
#include	"../../Header.hpp"

#include	"../../MainScene/Object/Gun.hpp"
#include	"../../MainScene/Object/ModData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunsModify : public SingletonBase<GunsModify> {
		private:
			friend class SingletonBase<GunsModify>;
		private:
			GunsModify(void) noexcept {}
			virtual ~GunsModify(void) noexcept {}
		private:
			struct Slot {
				GunSlot			m_SlotType{ GunSlot::Magazine };
				int				m_sel{ 0 };
				bool			m_selectSwitch{ 0 };
				const std::unique_ptr<Slot>*		m_ParentSlot{ nullptr };
				ModSlotControl*	m_Data{ nullptr };
			};
			struct SlotSaveData {
				GunSlot m_SlotType{ GunSlot::Magazine };
				int		m_sel{ 0 };
				GunSlot m_ParentSlotType{ GunSlot::Gun };
				int		m_Parentsel{ 0 };
			public:
				const auto IsParentNone(void) const noexcept { return (this->m_ParentSlotType == GunSlot::Gun); }
				const auto IsParent(GunSlot Slot_t, int sel_t) const noexcept { return (this->m_ParentSlotType == Slot_t) && (this->m_Parentsel == sel_t); }
			};
		private:
			std::vector<std::unique_ptr<Slot>>	m_SelectPartsData;
			std::vector<SlotSaveData>			m_SlotSave;
			SharedObj		 					m_BaseObj{ nullptr };
		private:
			void			SetMods(ModSlotControl* pMod, const std::unique_ptr<Slot>* pParentSlot);
			void			UpdateMods(ModSlotControl* pMod, const std::unique_ptr<Slot>* pParentSlot, bool isPreset) noexcept;
		public:
			void			CreateSelData(const std::shared_ptr<GunClass>& GunPtr, bool isPreset) noexcept;
			bool			ChangeSelData(const std::unique_ptr<Slot>* pParentSlot, int sel, bool isDeleteSlot) noexcept;
		public:
			void			LoadSlots(const char* path) noexcept;
			void			SaveSlots(const char* path) noexcept;
		public:
			void			DisposeSlots(void) noexcept {
				for (auto& data : this->m_SelectPartsData) {
					data.reset();
				}
				this->m_SelectPartsData.clear();
				this->m_BaseObj.reset();
			}
		};
	};
};
