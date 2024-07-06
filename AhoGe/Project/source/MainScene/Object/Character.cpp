#include	"Character.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

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

			//ˆÚ“®‘¬“x
			float Speed = 48.f;
			if (MyInput->GetPADSPress(PADS::RUN)) {
				Speed = 96.f;
			}
			if (MyInput->GetPADSPress(PADS::WALK)) {
				Speed = 16.f;
			}
			Easing(&m_MoveSpeed, Speed, 0.9f, EasingType::OutExpo);
			//ˆÚ“®‰‰ŽZ
			if (MyInput->GetPADSPress(PADS::MOVE_D)) {
				m_InputVec.x = std::clamp(m_InputVec.x + 256.f / DrawParts->GetFps(), -m_MoveSpeed, m_MoveSpeed);
			}
			else if (MyInput->GetPADSPress(PADS::MOVE_A)) {
				m_InputVec.x = std::clamp(m_InputVec.x - 256.f / DrawParts->GetFps(), -m_MoveSpeed, m_MoveSpeed);
			}
			else {
				Easing(&m_InputVec.x, 0.f, 0.95f, EasingType::OutExpo);
			}
			if (MyInput->GetPADSPress(PADS::MOVE_W)) {
				m_InputVec.y = std::clamp(m_InputVec.y + 256.f / DrawParts->GetFps(), -m_MoveSpeed, m_MoveSpeed);
			}
			else if (MyInput->GetPADSPress(PADS::MOVE_S)) {
				m_InputVec.y = std::clamp(m_InputVec.y - 256.f / DrawParts->GetFps(), -m_MoveSpeed, m_MoveSpeed);
			}
			else {
				Easing(&m_InputVec.y, 0.f, 0.95f, EasingType::OutExpo);
			}
			SetVec(m_InputVec.normalized() * std::max(std::abs(m_InputVec.x), std::abs(m_InputVec.y)));
		}
		//
		void CharacterObject::Execute_Sub() noexcept {
			if (m_PlayerID != 0) {
				auto& CamPos = Cam2DControl::Instance()->GetCamPos();
				int Radius = y_r(64.f* 64.f / CamPos.z) / 2;
				auto DispPos = Convert2DtoDisp(GetPos());
				Easing(&m_Alpha, BackGroundClassBase::Instance()->CheckHideShadow(DispPos, Radius), 0.5f, EasingType::OutExpo);
			}
			else {
				m_Alpha = 1.f;
			}
		}
		void CharacterObject::Draw_Sub() noexcept {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			int Radius = y_r(64.f* 64.f / CamPos.z) / 2;
			auto DispPos = Convert2DtoDisp(GetPos());

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * m_Alpha), 0, 255));
			DrawCircle((int)DispPos.x, (int)DispPos.y, Radius, Red);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		void CharacterObject::Dispose_Sub() noexcept {
		}
	};
};
