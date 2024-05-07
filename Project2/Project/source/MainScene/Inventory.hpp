#pragma once
#include	"../Header.hpp"

#include "../MainScene/Object/ItemData.hpp"
#include "../MainScene/Object/Item.hpp"
#include "../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class InventoryClass {
			int						m_mousex{0};
			int						m_mousey{0};
			switchs					m_Rkey;
			switchs					m_Deletekey;
			switchs					m_LCtrlkey;
			switchs					m_LAltkey;

			float					m_Alpha{0.f};

			const std::shared_ptr<CellItem>* m_DragIn{nullptr};
			const std::shared_ptr<ItemClass>* m_DragOut{nullptr};
			const std::shared_ptr<ItemData>* m_Drag{nullptr};
			bool Is90{false};
		public:
			//const auto& GetIsClient(void) const noexcept { return this->m_IsClient; }
		public:
		public:
			void Init(void) noexcept {
				m_Alpha = 0.f;
			}
			void FirstExecute(void) {
				this->m_Rkey.Execute(CheckHitKeyWithCheck(KEY_INPUT_R) != 0);
				this->m_Deletekey.Execute(CheckHitKeyWithCheck(KEY_INPUT_DELETE) != 0);
				this->m_LCtrlkey.Execute(CheckHitKeyWithCheck(KEY_INPUT_LCONTROL) != 0);
				this->m_LAltkey.Execute(CheckHitKeyWithCheck(KEY_INPUT_LALT) != 0);
			}
			void LateExecute(void) noexcept {}
			void Draw(const Vector3DX& MyPos) noexcept;
			void Dispose(void) noexcept {}
		};
	};
};
