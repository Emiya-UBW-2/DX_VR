#pragma once

#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ConcussionControl {
			float										m_Timer{0.f};
			float										m_Per{0.f};
		public:
			ConcussionControl() {}
			~ConcussionControl() {}
		public:
			void Init() noexcept {
				m_Timer = 0.f;
				m_Per = 0.f;
			}
			void Update() noexcept {
				if (m_Timer > 0.9f) {
					Easing(&m_Per, 1.f, 0.1f, EasingType::OutExpo);
				}
				else if (m_Timer > 0.25f) {
					if (m_Per > 0.25f) {
						Easing(&m_Per, 0.f, 0.8f, EasingType::OutExpo);
					}
					else {
						m_Per = 0.25f;
					}
				}
				else {
					Easing(&m_Per, 0.f, 0.8f, EasingType::OutExpo);
				}
				m_Timer = std::max(m_Timer - 1.f / FPS, 0.f);
			}
		public:
			void SetActive() noexcept { m_Timer = 1.f; }
			const auto		GetActiveSwitch(void) const noexcept { return this->m_Timer == 1.f; }
			const auto		IsActive(void) const noexcept { return this->m_Timer > 0.f; }
			const auto		GetPer(void) const noexcept { return this->m_Per  * 1.5f; }
		private:

		};
		class MainLoopPauseControl {
			int select{0};
			std::array<float, 3> SelYadd{};
			bool	m_IsRetire{false};
		public:
			MainLoopPauseControl() {}
			~MainLoopPauseControl() {}
		public:
			void Init() noexcept {
				Reset();
				m_IsRetire = false;
			}
			void Execute() noexcept {
				auto* Pad = PadControl::Instance();
				auto* SE = SoundPool::Instance();
				if (!OptionWindowClass::Instance()->IsActive()) {
					if (Pad->GetKey(PADS::MOVE_W).trigger()) {
						--select;
						if (select < 0) { select = 2; }
						SelYadd[select] = 10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetKey(PADS::MOVE_S).trigger()) {
						++select;
						if (select > 2) { select = 0; }
						SelYadd[select] = -10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					for (int i = 0; i < 3; i++) {
						Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
					}
					if (Pad->GetKey(PADS::INTERACT).trigger()) {
						SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						switch (select) {
							case 0:
								m_IsRetire = true;
								DXDraw::Instance()->PauseExit();
								break;
							case 1:
								OptionWindowClass::Instance()->SetActive();
								break;
							case 2:
								DXDraw::Instance()->PauseExit();
								break;
							default:
								DXDraw::Instance()->PauseExit();
								break;
						}
					}
					if (Pad->GetKey(PADS::RELOAD).trigger()) {
						SE->Get((int)SoundEnumCommon::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
						DXDraw::Instance()->PauseExit();
					}
				}
			}
			void Draw() noexcept {
				int xp1, yp1;
				//auto* Fonts = FontPool::Instance();
				//auto Red = GetColor(255, 0, 0);
				//auto Red75 = GetColor(192, 0, 0);
				//auto White = GetColor(255, 255, 255);
				//auto Gray75 = GetColor(128, 128, 128);
				//auto Gray = GetColor(64, 64, 64);

				xp1 = y_r(1920 - 256 - 54 * 2);
				yp1 = y_r(1080 - 108 - 108 * 2 + (int)SelYadd[0]);
				DrawFetteString(xp1, yp1, 1.f, (select == 0), "Retire");

				xp1 = y_r(1920 - 256 - 54 * 1);
				yp1 = y_r(1080 - 108 - 108 * 1 + (int)SelYadd[1]);
				DrawFetteString(xp1, yp1, 1.f, (select == 1), "Option");

				xp1 = y_r(1920 - 256);
				yp1 = y_r(1080 - 108 + (int)SelYadd[2]);
				DrawFetteString(xp1, yp1, 1.f, (select == 2), "Return Game");
			}
		public:
			void Reset() noexcept {
				for (auto& y : SelYadd) {
					y = 0.f;
				}
				select = 0;
			}
			const auto&		GetIsRetireSelected(void) const noexcept { return this->m_IsRetire; }
		private:
		};
		class MyPlayerReticleControl {
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;
			float Lens_xpos = 0;
			float Lens_ypos = 0;
			float LensSize = 10000;
		public:
			MyPlayerReticleControl() {}
			~MyPlayerReticleControl() {}
		public:
			void Update(const VECTOR_ref& LensPos, const VECTOR_ref& LensPos2, const VECTOR_ref& ReticlePos) noexcept {
				VECTOR_ref LensPosBuf = ConvWorldPosToScreenPos(LensPos.get());
				if (!(0.f < LensPosBuf.z() && LensPosBuf.z() < 1.f)) {
					return;
				}
				Lens_xpos = LensPosBuf.x();
				Lens_ypos = LensPosBuf.y();
				VECTOR_ref LensSizeBuf = ConvWorldPosToScreenPos(LensPos2.get());
				if (0.f < LensSizeBuf.z() && LensSizeBuf.z() < 1.f) {
					LensSize = std::hypotf(Lens_xpos - LensSizeBuf.x(), Lens_ypos - LensSizeBuf.y());
				}
				VECTOR_ref ReticlePosBuf = ConvWorldPosToScreenPos(ReticlePos.get());
				if (0.f < ReticlePosBuf.z() && ReticlePosBuf.z() < 1.f) {
					Reticle_xpos = ReticlePosBuf.x();
					Reticle_ypos = ReticlePosBuf.y();
					Reticle_on = (LensSize > std::hypotf(Lens_xpos - Reticle_xpos, Lens_ypos - Reticle_ypos));
				}
			}
		public:
			void SetActiveOff() noexcept {
				Reticle_on = false;
			}
			const auto&		IsActive(void) const noexcept { return this->Reticle_on; }
			const auto&		GetReticleXPos(void) const noexcept { return this->Reticle_xpos; }
			const auto&		GetReticleYPos(void) const noexcept { return this->Reticle_ypos; }
			const auto&		GetLensXPos(void) const noexcept { return this->Lens_xpos; }
			const auto&		GetLensYPos(void) const noexcept { return this->Lens_ypos; }
			const auto&		GetLensSize(void) const noexcept { return this->LensSize; }
		private:
		};
	};
};
