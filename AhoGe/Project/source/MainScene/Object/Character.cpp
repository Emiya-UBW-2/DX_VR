#include	"Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		CharacterObject::CharacterObject() {
			m_InputVec.Set(0.f, 0.f, 0.f);
		}
		CharacterObject::~CharacterObject() {
		}
		//
		void		CharacterObject::ExecuteInput(InputControl* MyInput) noexcept {
			auto* DrawParts = DXDraw::Instance();

			if (MyInput->GetPADSPress(PADS::MOVE_D)) {
				m_InputVec.x = std::clamp(m_InputVec.x + 256.f / DrawParts->GetFps(), -128.f, 128.f);
			}
			else if (MyInput->GetPADSPress(PADS::MOVE_A)) {
				m_InputVec.x = std::clamp(m_InputVec.x - 256.f / DrawParts->GetFps(), -128.f, 128.f);
			}
			else {
				Easing(&m_InputVec.x, 0.f, 0.95f, EasingType::OutExpo);
			}

			if (MyInput->GetPADSPress(PADS::MOVE_W)) {
				m_InputVec.y = std::clamp(m_InputVec.y + 256.f / DrawParts->GetFps(), -128.f, 128.f);
			}
			else if (MyInput->GetPADSPress(PADS::MOVE_S)) {
				m_InputVec.y = std::clamp(m_InputVec.y - 256.f / DrawParts->GetFps(), -128.f, 128.f);
			}
			else {
				Easing(&m_InputVec.y, 0.f, 0.95f, EasingType::OutExpo);
			}
			SetVec(m_InputVec.normalized() * std::max(std::abs(m_InputVec.x), std::abs(m_InputVec.y)));
		}
		//
		void CharacterObject::Execute_Sub() noexcept {
		}
		void CharacterObject::Draw_Sub() noexcept {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			float Radius = y_r(64.f* 64.f / CamPos.z) / 2;
			auto DispPos = Convert2DtoDisp(GetPos());
			DrawCircle((int)DispPos.x, (int)DispPos.y, Radius, Red);
		}
		void CharacterObject::Dispose_Sub() noexcept {
		}
	};
};
