#pragma once
#include	"../Header.hpp"
#include "../MainScene/Object/Gun.hpp"
#include "../MainScene/Object/ModData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunsModify {
		public:
			struct Slot {
				GunSlot SlotType{ GunSlot::Magazine };
				int m_sel{ 0 };
				bool m_selectSwitch{ 0 };
				const Slot* ParentSlot{ nullptr };
				std::shared_ptr<ObjectBaseClass> m_Data{ nullptr };
			public:
				const auto& GetDataObj() const noexcept {
					if (this->ParentSlot) {
						return (std::shared_ptr<ObjectBaseClass>&)((std::shared_ptr<ModClass>&)this->m_Data);
					}
					else {
						return (std::shared_ptr<ObjectBaseClass>&)((std::shared_ptr<GunClass>&)this->m_Data);
					}
				}
				const auto* GetData() const noexcept {
					if (this->ParentSlot) {
						return (ModSlotControl*)((std::shared_ptr<ModClass>&)this->m_Data).get();
					}
					else {
						return (ModSlotControl*)((std::shared_ptr<GunClass>&)this->m_Data).get();
					}
				}
			public:
				void Init() noexcept {
					this->m_selectSwitch = true;
				}
			};
		private:
			struct SlotSaveData {
				GunSlot SlotType{ GunSlot::Magazine };
				int		m_sel{ 0 };
				GunSlot ParentSlotType{ GunSlot::Gun };
				int		m_Parentsel{ 0 };
			public:
				const auto IsParentNone() const noexcept { return (ParentSlotType == GunSlot::Gun); }
				const auto IsParent(GunSlot Slot_t, int sel_t) const noexcept { return (ParentSlotType == Slot_t) && (m_Parentsel == sel_t); }
			};
		private:
			std::vector<Slot*>			SelData;
			std::vector<SlotSaveData>	SlotSave;
			MV1*						m_BaseObj{ nullptr };
		public:
			const auto&		GetSelData() const noexcept { return SelData; }
		private:
			void			AddSelData(const std::shared_ptr<ObjectBaseClass>& ModPtr, const Slot* SlotPtr, bool isPreset);
			bool			DelSelData(const Slot* SlotPtr);
			void			SetMods(ModSlotControl* ModPtr, const Slot* SlotPtr);
			void			UpdateMods(ModSlotControl* ModPtr, const Slot* SlotPtr, bool isPreset) noexcept;
		public:
			void			CreateSelData(const std::shared_ptr<GunClass>& GunPtr, bool isPreset);
			void			ChangeSelData(const Slot* SlotPtr, int sel);
			void			LoadSlots(const char* path);
			void			SaveSlots(const char* path);
		public:
			void			DisposeSlots() {
				for (auto& y : SelData) {
					delete y;
				}
				SlotSave.clear();
				SelData.clear();
			}
		};

		class CustomScene : public TEMPSCENE, public GunsModify {
			class SlotMove {
			public:
				float				Xadd{ 0.f };
				float				Yadd{ 0.f };
				GunsModify::Slot*	Ptr;
				int					index{ 0 };
			};
		private:
			int select{ 0 };
			float m_SelAlpha{ 0.f };

			float m_Yrad{ 0.f };
			float m_Xrad{ 0.f };
			float m_Range{ 1.f };
			std::vector<SlotMove>			SelMoveClass;
		private:
			std::shared_ptr<GunClass>		m_GunPtr;				//ポインター別持ち

			void			UpdateSlotMove() {
				//もう存在せんものを削除
				for (int loop = 0; loop < SelMoveClass.size(); loop++) {
					bool isHit = false;
					for (const auto& S : GetSelData()) {
						if (S == SelMoveClass[loop].Ptr) {
							isHit = true;
							break;
						}
					}
					if (!isHit) {
						SelMoveClass.erase(SelMoveClass.begin() + loop);
						loop--;
					}
				}
				//存在するものを追加+順番を指定
				for (const auto& S : GetSelData()) {
					int index = (int)(&S - &GetSelData().front());
					bool isHit = false;
					for (int loop = 0; loop < SelMoveClass.size(); loop++) {
						if (S == SelMoveClass[loop].Ptr) {
							SelMoveClass[loop].index = index;
							isHit = true;
							break;
						}
					}
					if (!isHit) {
						SelMoveClass.resize(SelMoveClass.size() + 1);
						SelMoveClass.back().Ptr = S;
						SelMoveClass.back().index = index;
						SelMoveClass.back().Xadd = 0.f;
						SelMoveClass.back().Yadd = 0.f;
					}
				}
				//SelMoveClassをGetSelData()に合わせてソート
				std::sort(SelMoveClass.begin(), SelMoveClass.end(),
					[](const SlotMove& a, const SlotMove& b) {
					return a.index < b.index;
				}
				);
			}
		public:
			CustomScene(void) noexcept { }
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			//void			BG_Draw_Sub(void) noexcept override {}
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_NearFar_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept  override;
			void			DrawUI_In_Sub(void) noexcept override {}
		};
	};
};
