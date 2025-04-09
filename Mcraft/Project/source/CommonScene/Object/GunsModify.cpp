#include	"GunsModify.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			GunsModify::SetupDefaultGunParts(const std::unique_ptr<GunPartsSlotControl>* pBaseParts, const std::unique_ptr<SlotData>* pParentSlot, bool isPreset) noexcept {
			if (!pBaseParts) { return; }
			//子供にデフォルトの設定を追加
			for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
				GunSlot NowSlot = (GunSlot)loop;
				const auto& Data = (*pBaseParts)->GetGunPartsData()->GetPartsSlot(NowSlot);
				if (!Data) { continue; }// スロットがないとスルー
				//追加
				this->m_SlotDataPool.emplace_back(std::make_unique<SlotData>(NowSlot, pParentSlot, pBaseParts));
				auto& data = this->m_SlotDataPool.back();
				//スロットに設定するオブジェクトの選択
				int Select = Data->m_IsNeed ? 0 : InvalidID;
				if (isPreset) {
					for (const auto& S : m_SlotSave) {
						if (data->IsSavedSlot(S)) {
							Select = S.m_sel;
							break;
						}
					}
				}
				//選択したパーツをセット
				data->Set(this->m_BaseGun, Select);
				//パーツがある場合
				if (data->IsAttachedParts()) {
					//スロットに設定したものに沿ってデフォルトの子パーツを設定
					SetupDefaultGunParts(&data->GetAttachedPartsSlot(), &data, isPreset);
				}
			}
		}
		bool			GunsModify::DeleteSlotsChildParts(const std::unique_ptr<SlotData>* pSlot) noexcept {
			//設定したいパーツが親であるパーツも子をすべて殺す
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
				//そのデータ自体を削除
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
			//一旦クリーンにする
			auto& data = *pSlot;
			data->Set(this->m_BaseGun, sel);
			//パーツがある場合
			if (data->IsAttachedParts()) {
				//スロットに設定したものに沿ってデフォルトのパーツを設定
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
				while (!inputfile.eof()) { //ファイルの最後まで続ける
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
