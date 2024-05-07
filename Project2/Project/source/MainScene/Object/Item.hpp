#pragma once
#include	"../../Header.hpp"
#include "../../MainScene/Object/Vehicle.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ItemClass : public ObjectBaseClass {
		private:
			int							m_Count{ 1 };
			std::shared_ptr<ItemData>	m_ItemData{ nullptr };
			bool						m_CanMove{ true };
			std::shared_ptr<BackGroundClass>			m_BackGround;				//BG
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClass>& backGround) { this->m_BackGround = backGround; }
		public://getter
			const auto& GetItemData(void) const noexcept { return this->m_ItemData; }
		public: //コンストラクタ、デストラクタ
			ItemClass(void) noexcept { this->m_objType = (int)ObjType::Item; }
			~ItemClass(void) noexcept {}
		public: //継承
			void SetData(const std::shared_ptr<ItemData>& pItemData, int cap) noexcept {
				this->m_ItemData = pItemData;
				if (cap < 0) {
					this->m_Count = this->m_ItemData->GetCapacity();
				}
				else {
					this->m_Count = cap;
				}
			}
			const auto&		GetCount(void) const noexcept { return this->m_Count; }
			//
			void DrawItemData(int xp1, int yp1, int xp2, int yp2) noexcept {
				auto* Fonts = FontPool::Instance();
				if (this->m_ItemData) {
					DrawBox(xp1, yp1, xp2, yp2, GetColor(128, 128, 128), TRUE);
					Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
						xp1, yp1 + (yp2 - yp1) / 2, GetColor(255, 255, 255), GetColor(0, 0, 0), "%s", this->m_ItemData->GetName().c_str());
				}
			}
			//
			void			Init_Sub() noexcept override {
				SetActive(false);
				this->m_CanMove = true;
			}
			//
			void			FirstExecute(void) noexcept override;
			//
			void			DrawShadow(void) noexcept override {
				GetObj().DrawModel();
			}
			//
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (isDrawSemiTrans) { return; }
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-20, 0, -20)).get(),
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(20, 20, 20)).get()) == FALSE
						) {
						auto Length = (this->GetObj().GetMatrix().pos() - DxLib::GetCameraPosition()).magnitude();
						if (Length < 50.f*Scale_Rate) {
							this->GetObj().DrawModel();
						}
					}
				}
			}
			//
		};
	};
};
