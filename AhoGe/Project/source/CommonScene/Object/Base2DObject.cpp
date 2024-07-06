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
			float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));

			int Max = (int)std::max(1.f, 240.f / std::max(30.f, DrawParts->GetFps()));
			for (int i = 0;i < Max;i++) {
				m_PrevPos = m_Pos;
				m_Pos += m_Vec * ((1.f / (float)Max) / DrawParts->GetFps());
				if ((m_ColTarget == ColTarget::All) || (m_ColTarget == ColTarget::Wall)) {
					BackGroundClassBase::Instance()->CheckLinetoMap(m_PrevPos, &m_Pos, HalfLen);
				}
			}

			if ((m_ColTarget == ColTarget::All) || (m_ColTarget == ColTarget::Object)) {
				Obj2DParts->CheckColObject(this);
			}
		}
	};
};
