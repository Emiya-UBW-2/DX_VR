#include	"Character.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//AI—p
		class CharacterObject::Impl {
		private:
			PlayerID m_MyCharaID{0};
			Vector3DX m_Pos{};
			Vector3DX m_Vec{};

			Vector3DX m_InputVec{};

			bool isHit = false;
		public:
			Impl(void) noexcept {}
			~Impl(void) noexcept {}
		public:
			const auto& GetPos() const noexcept { return m_Pos; }
		public:
			void		Init(PlayerID MyCharaID) noexcept {
				m_MyCharaID = MyCharaID;
				m_InputVec.Set(0.f, 0.f, 0.f);
			}
			//
			void		ExecuteInput(InputControl* MyInput) noexcept {
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
				m_Vec = m_InputVec.normalized() * std::max(std::abs(m_InputVec.x), std::abs(m_InputVec.y));

				auto Prev = m_Pos;

				m_Pos += m_Vec / DrawParts->GetFps();

				float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));

				isHit = BackGroundClassBase::Instance()->CheckLinetoMap(Prev, &m_Pos, HalfLen);
			}
			void		Execute() noexcept {}
			void		Draw() noexcept {
				auto& CamPos = Cam2DControl::Instance()->GetCamPos();
				float Radius = y_r(64.f* 64.f / CamPos.z) / 2;
				auto DispPos = Convert2DtoDisp(m_Pos);
				DrawCircle((int)DispPos.x, (int)DispPos.y, Radius, isHit ? Blue : Red);
			}
			void		Dispose() noexcept {}
		};
		//
		CharacterObject::CharacterObject() {
			m_Param = new Impl;
		}
		CharacterObject::~CharacterObject() {
			delete m_Param;
		}
		const Vector3DX & CharacterObject::GetPos() const noexcept { return this->GetParam()->GetPos(); }
		//
		void CharacterObject::Init(PlayerID MyCharaID) noexcept {
			this->GetParam()->Init(MyCharaID);
		}
		void CharacterObject::ExecuteInput(InputControl * MyInput) noexcept {
			this->GetParam()->ExecuteInput(MyInput);
		}
		void CharacterObject::Execute() noexcept {
			this->GetParam()->Execute();
		}
		void CharacterObject::Draw() noexcept {
			this->GetParam()->Draw();
		}
		void CharacterObject::Dispose() noexcept {
			this->GetParam()->Dispose();
		}
	};
};
