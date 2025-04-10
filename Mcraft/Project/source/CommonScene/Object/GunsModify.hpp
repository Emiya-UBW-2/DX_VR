#pragma once
#include	"../../Header.hpp"

#include	"../../MainScene/Object/Gun.hpp"
#include	"../../MainScene/Object/ModData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass;

		class GunsModify {
		private:
			struct SlotSaveData {
				GunSlot m_SlotType{ GunSlot::Magazine };
				int		m_sel{ 0 };
				GunSlot m_ParentSlotType{ GunSlot::Gun };
				int		m_Parentsel{ 0 };
			};
			class SlotData {
			private:
				GunSlot										m_SlotType{ GunSlot::Magazine };
				const std::unique_ptr<SlotData>*			m_ParentSlot{ nullptr };
				const std::unique_ptr<GunPartsSlotControl>*	m_MySlot{ nullptr };
				int											m_sel{ 0 };//�Z�[�u�ւ̕ێ��ɂ����g���
			public:
				bool IsAttachedParts(void) const noexcept { return (*this->m_MySlot)->GetPartsPtr(this->m_SlotType) != nullptr; }
				const auto& GetAttachedPartsSlot(void) const noexcept { return (*this->m_MySlot)->GetPartsPtr(this->m_SlotType)->GetGunPartsSlot(); }
				const auto& GetParentSlot(void) const noexcept { return this->m_ParentSlot; }
			public:
				SlotData(GunSlot SlotSelect, const std::unique_ptr<SlotData>*pParent, const std::unique_ptr<GunPartsSlotControl>* pParts) noexcept {
					m_SlotType = SlotSelect;
					m_ParentSlot = pParent;
					m_MySlot = pParts;
				}
				~SlotData() noexcept {
					(*this->m_MySlot)->RemoveGunParts(this->m_SlotType);
				}
			public:
				void Set(const SharedObj& BaseModel, int sel) noexcept {
					this->m_sel = sel;
					(*this->m_MySlot)->RemoveGunParts(this->m_SlotType);
					if (sel != InvalidID) {
						(*this->m_MySlot)->AttachGunParts(this->m_SlotType, sel, BaseModel);
					}
				}
			public:
				//�����Ă���f�[�^�̃Z�[�u�f�[�^�ւ̕ϊ�
				SlotSaveData GetSaveData(void) const noexcept {
					SlotSaveData Tmp;
					Tmp.m_SlotType = this->m_SlotType;
					Tmp.m_sel = this->m_sel;
					Tmp.m_ParentSlotType = (this->m_ParentSlot) ? (*this->m_ParentSlot)->m_SlotType : GunSlot::Gun;
					Tmp.m_Parentsel = (this->m_ParentSlot) ? (*this->m_ParentSlot)->m_sel : 0;
					return Tmp;
				}
				//���g���Y���̃X���b�g�Ɠ��ꂩ�ǂ����m�F
				bool IsSavedSlot(const SlotSaveData& S) {
					if (this->m_ParentSlot) {
						if (!(S.m_ParentSlotType == (*this->m_ParentSlot)->m_SlotType) && (S.m_Parentsel == (*this->m_ParentSlot)->m_sel)) { return false; }
					}
					else {
						if (S.m_ParentSlotType != GunSlot::Gun) { return false; }
					}
					return (this->m_SlotType == S.m_SlotType);
				}
			};
		private:
			std::vector<std::unique_ptr<SlotData>>	m_SlotDataPool;
			std::vector<SlotSaveData>				m_SlotSave;
			std::shared_ptr<GunClass>				m_BaseGun{ nullptr };
		public:
			GunsModify(const std::shared_ptr<GunClass>& GunPtr, bool isPreset) noexcept {
				CreateSelData(GunPtr, isPreset);
			}
			virtual ~GunsModify(void) noexcept {
				for (auto& data : this->m_SlotDataPool) {
					data.reset();
				}
				this->m_SlotDataPool.clear();
				this->m_BaseGun.reset();
			}
		private:
			//pBaseParts�̎q���ȍ~�̃X���b�g�Ƀf�t�H���g�X���b�g��ݒ�
			void			SetupDefaultGunParts(const std::unique_ptr<GunPartsSlotControl>* pBaseParts, const std::unique_ptr<SlotData>* pParentSlot, bool isPreset) noexcept;
			//�Y���X���b�g�̃p�[�c�����ׂĊO��
			bool			DeleteSlotsChildParts(const std::unique_ptr<SlotData>* pSlot) noexcept;
		public:
			void			CreateSelData(const std::shared_ptr<GunClass>& GunPtr, bool isPreset) noexcept;
			//�Y���X���b�g�̃p�[�c����芷����
			void			ChangeSelData(const std::unique_ptr<SlotData>* pSlot, int sel) noexcept;
		public:
			void			LoadSlots(const char* path) noexcept;
			void			SaveSlots(const char* path) noexcept;
		};
	};
};
