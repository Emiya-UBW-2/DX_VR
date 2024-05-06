#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ItemType {
			AMMO,
			ARMER,
		};
		class ItemObjClass : public ObjectBaseClass {
			float		m_yAdd{0.f};
			ItemType	m_ItemType{ItemType::ARMER};
		private:
			std::shared_ptr<BackGroundClassBase>		m_BackGround;				//BG
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept { this->m_BackGround = backGround; }
		public:
			ItemObjClass(void) noexcept { this->m_objType = (int)ObjType::ItemObj; }
			~ItemObjClass(void) noexcept {}
		public:
			void			SetFall(const Vector3DX& pos, const Vector3DX& vec) noexcept;
			void			SetItemType(ItemType item) noexcept { this->m_ItemType = item; }
			const auto&		GetItemType(void) const noexcept { return this->m_ItemType; }
		public:
			void			Init_Sub(void) noexcept override {
				this->m_IsActive = false;
				this->m_ItemType = ItemType::ARMER;
			}
			void			FirstExecute(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-0.1f*Scale_Rate, -0.1f*Scale_Rate, -0.1f*Scale_Rate)).get(),
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(0.1f*Scale_Rate, 0.1f*Scale_Rate, 0.1f*Scale_Rate)).get()) == FALSE
						) {
						if (!isDrawSemiTrans) {
							this->m_obj.DrawModel();
						}
					}
				}
			}
			void			Dispose_Sub(void) noexcept override {
				this->m_BackGround.reset();
			}
		};
	};
};
