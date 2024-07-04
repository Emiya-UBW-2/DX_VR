#include	"Base2DObject.hpp"
#include "Object2DManager.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void Base2DObject::Execute() noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto Obj2DParts = Object2DManager::Instance();

			Execute_Sub();
			auto Prev = m_Pos;
			//Õ“Ëž‚Ý‚Ì‰‰ŽZ
			m_Pos += m_Vec * (60.f / DrawParts->GetFps());
			if ((m_ColTarget == ColTarget::All) || (m_ColTarget == ColTarget::Wall)) {
				//
			}
			if ((m_ColTarget == ColTarget::All) || (m_ColTarget == ColTarget::None)) {
				//
			}
		}
	};
};
