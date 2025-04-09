#include	"GunsModify.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			GunsModify::SetupDefaultGunParts(const std::unique_ptr<GunPartsSlotControl>* pBaseParts, const std::unique_ptr<SlotData>* pParentSlot, bool isPreset) noexcept {
			if (!pBaseParts) { return; }
			//�q���Ƀf�t�H���g�̐ݒ��ǉ�
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				GunSlot NowSlot = (GunSlot)loop;
				const auto& Data = (*pBaseParts)->GetGunPartsData()->GetPartsSlot(NowSlot);
				if (!Data) { continue; }// �X���b�g���Ȃ��ƃX���[
				//�ǉ�
				this->m_SlotDataPool.emplace_back(std::make_unique<SlotData>(NowSlot, pParentSlot, pBaseParts));
				auto& data = this->m_SlotDataPool.back();
				//�X���b�g�ɐݒ肷��I�u�W�F�N�g�̑I��
				int Select = Data->m_IsNeed ? 0 : InvalidID;
				if (isPreset) {
					for (const auto& S : m_SlotSave) {
						if (data->IsSavedSlot(S)) {
							Select = S.m_sel;
							break;
						}
					}
				}
				//�I�������p�[�c���Z�b�g
				data->Set(this->m_BaseGun, Select);
				//�p�[�c������ꍇ
				if (data->IsAttachedParts()) {
					//�X���b�g�ɐݒ肵�����̂ɉ����ăf�t�H���g�̎q�p�[�c��ݒ�
					SetupDefaultGunParts(&data->GetAttachedPartsSlot(), &data, isPreset);
				}
			}
		}
		bool			GunsModify::DeleteSlotsChildParts(const std::unique_ptr<SlotData>* pSlot) noexcept {
			//�ݒ肵�����p�[�c���e�ł���p�[�c���q�����ׂĎE��
			for (int loop = 0; loop < static_cast<int>(this->m_SlotDataPool.size()); ++loop) {
				const auto& data = this->m_SlotDataPool[loop];
				if (&data == pSlot) { continue; }
				if (data->GetParentSlot() != pSlot) { continue; }
				if (DeleteSlotsChildParts(&data)) {
					--loop;
				}
			}
			for (auto& data : this->m_SlotDataPool) {
				if (&data == pSlot) { continue; }
				int index = static_cast<int>(&data - &this->m_SlotDataPool.front());
				//���̃f�[�^���̂��폜
				data.reset();
				this->m_SlotDataPool.erase(this->m_SlotDataPool.begin() + index);
				return true;
			}
			return false;
		}
		//
		void			GunsModify::CreateSelData(const std::shared_ptr<GunClass>& pBaseGun, bool isPreset) noexcept {
			this->m_BaseGun = pBaseGun;
			SetupDefaultGunParts(&pBaseGun->GetGunPartsSlot(), nullptr, isPreset);
		}
		void			GunsModify::ChangeSelData(const std::unique_ptr<SlotData>* pSlot, int sel) noexcept {
			DeleteSlotsChildParts(pSlot);
			//
			//��U�N���[���ɂ���
			auto& data = *pSlot;
			data->Set(this->m_BaseGun, sel);
			//�p�[�c������ꍇ
			if (data->IsAttachedParts()) {
				//�X���b�g�ɐݒ肵�����̂ɉ����ăf�t�H���g�̃p�[�c��ݒ�
				SetupDefaultGunParts(&data->GetAttachedPartsSlot(), &data, false);
			}
		}
		void			GunsModify::LoadSlots(const char* path) noexcept {
			m_SlotSave.clear();
			std::ifstream inputfile(path, std::ios::binary);
			if (inputfile) {
				{
					int ULTSel = 0;
					inputfile.read(reinterpret_cast<char*>(&ULTSel), sizeof(ULTSel));
				}
				while (!inputfile.eof()) { //�t�@�C���̍Ō�܂ő�����
					SlotSaveData Tmp;
					inputfile.read(reinterpret_cast<char*>(&Tmp), sizeof(Tmp));
					m_SlotSave.emplace_back(Tmp);
				}
			}
			inputfile.close();
		}
		void			GunsModify::SaveSlots(const char* path) noexcept {
			std::ofstream outputfile(path, std::ios::binary);
			{
				int ULTSel = 0;
				outputfile.write(reinterpret_cast<char*>(&ULTSel), sizeof(ULTSel));
			}
			for (auto& data : this->m_SlotDataPool) {
				SlotSaveData Tmp = data->GetSaveData();
				outputfile.write(reinterpret_cast<char*>(&Tmp), sizeof(Tmp));
			}
			outputfile.close();
		}
};
};
