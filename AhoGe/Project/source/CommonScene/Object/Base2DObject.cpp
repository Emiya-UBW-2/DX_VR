#include	"Base2DObject.hpp"
#include "Object2DManager.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void Base2DObject::Execute() noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto Obj2DParts = Object2DManager::Instance();

			Execute_Sub();

			//Õ“Ëž‚Ý‚Ì‰‰ŽZ
			m_Pos += m_Vec / DrawParts->GetFps();
			float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));

			if ((m_ColTarget == ColTarget::All) || (m_ColTarget == ColTarget::Wall)) {
				BackGroundClassBase::Instance()->CheckLinetoMap(m_PrevPos, &m_Pos, HalfLen);
			}
			if ((m_ColTarget == ColTarget::All) || (m_ColTarget == ColTarget::None)) {
				Obj2DParts->CheckColObject(this);
			}
			m_PrevPos = m_Pos;
		}
	};
};
