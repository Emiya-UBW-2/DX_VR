#pragma once
#include	"../../Header.hpp"
#include "ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ItemType {
			AMMO,
			ARMER,
		};
		class ItemObjClass : public ObjectBaseClass {
			float m_yAdd{ 0.f };
			ItemType m_ItemType;
		private:
			std::shared_ptr<BackGroundClassBase>		m_BackGround;				//BG
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept { this->m_BackGround = backGround; }
		public:
			ItemObjClass(void) noexcept { this->m_objType = ObjType::ItemObj; }
			~ItemObjClass(void) noexcept { }
		public:
			void			SetFall(const VECTOR_ref& pos, const VECTOR_ref& vec) noexcept;
			void			SetItemType(ItemType item) noexcept { this->m_ItemType = item; }
			const auto&		GetItemType(void) const noexcept { return this->m_ItemType; }
		public:
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_IsActive = false;
				this->m_ItemType = ItemType::ARMER;
			}
			void			FirstExecute(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-0.1f*Scale_Rate, -0.1f*Scale_Rate, -0.1f*Scale_Rate)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(0.1f*Scale_Rate, 0.1f*Scale_Rate, 0.1f*Scale_Rate)).get()) == FALSE
						) {
						if (!isDrawSemiTrans) {
							this->m_obj.DrawModel();
						}
					}
				}
			}
			void			Dispose(void) noexcept override {
				this->m_BackGround.reset();
				this->GetObj().Dispose();
				this->m_col.Dispose();
			}
		};
	};
};
