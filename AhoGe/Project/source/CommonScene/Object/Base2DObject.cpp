#include	"Base2DObject.hpp"
#include	"Object2DManager.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void Base2DObject::Execute() noexcept {
			auto* Obj2DParts = Object2DManager::Instance();
			//
			Execute_Sub();
			// オブジェクト判定
			if ((m_ColTarget == ColTarget::All) || (m_ColTarget == ColTarget::Object)) {
				Obj2DParts->CheckColObject(this);
			}
		}
		void Base2DObject::ExecuteAfter() noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			// 衝突込みの演算
			// 壁判定
			if ((m_ColTarget == ColTarget::All) || (m_ColTarget == ColTarget::Wall)) {
				int Max = (int)std::max(1.f, 240.f / std::max(30.f, DrawParts->GetFps()));
				for (int i = 0; i < Max; i++) {
					m_Pos += m_Vec * ((1.f / (float)Max) * Tile_DispSize / DrawParts->GetFps());
					BackGround->CheckLinetoMap(m_PrevPos, &m_Pos, Get2DSize(0.5f));
					m_PrevPos = m_Pos;
				}
			}
			else {
				m_Pos += m_Vec * (Tile_DispSize / DrawParts->GetFps());
				m_PrevPos = m_Pos;
			}
		}
	};
};
