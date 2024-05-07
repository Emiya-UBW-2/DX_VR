#pragma once

#include "../sub.hpp"

#include "../CommonScene/UI/CommonUIControl.hpp"

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
				auto* DrawParts = DXDraw::Instance();
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
				m_Timer = std::max(m_Timer - 1.f / DrawParts->GetFps(), 0.f);
			}
		public:
			void SetActive() noexcept { m_Timer = 1.f; }
			const auto		GetActiveSwitch(void) const noexcept { return this->m_Timer == 1.f; }
			const auto		IsActive(void) const noexcept { return this->m_Timer > 0.f; }
			const auto		GetPer(void) const noexcept { return this->m_Per * 1.5f; }
		private:

		};
		class MainLoopPauseControl {
			GraphHandle					m_SelectBackImage;
			std::array<ButtonClass, 3>	ButtonSel{};
			int							select{0};

			bool						m_MouseSelMode{false};

			bool						m_IsRetire{false};
		public:
			MainLoopPauseControl() {}
			~MainLoopPauseControl() {}
		public:
			void Init() noexcept {
				Reset();
				m_IsRetire = false;

				m_SelectBackImage = GraphHandle::Load("data/UI/select.png");
				for (auto& y : ButtonSel) {
					y.LoadCommon(&m_SelectBackImage);
				}

				ButtonSel.at(0).Load_String("Retire", y_r(48), true);
				ButtonSel.at(0).Set(y_r(1920 - 64), y_r(1080 - 84 - 64 * 2), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
				ButtonSel.at(1).Load_String("Option", y_r(48), true);
				ButtonSel.at(1).Set(y_r(1920 - 64), y_r(1080 - 84 - 64 * 1), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
				ButtonSel.at(2).Load_String("Return Game", y_r(48), true);
				ButtonSel.at(2).Set(y_r(1920 - 64), y_r(1080 - 84 - 64 * 0), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM);
			}
			void Execute() noexcept {
				auto* Pad = PadControl::Instance();
				auto* SE = SoundPool::Instance();
				if (!OptionWindowClass::Instance()->IsActive()) {
					int preselect = select;
					bool preMouseSel = m_MouseSelMode;

					if (Pad->GetKey(PADS::MOVE_W).trigger()) {
						if (select != -1) {
							--select;
							if (select < 0) { select = (int)ButtonSel.size() - 1; }
						}
						else {
							select = 0;
						}
						m_MouseSelMode = false;
					}
					if (Pad->GetKey(PADS::MOVE_S).trigger()) {
						if (select != -1) {
							++select;
							if (select > (int)ButtonSel.size() - 1) { select = 0; }
						}
						else {
							select = 0;
						}
						m_MouseSelMode = false;
					}

					if (m_MouseSelMode) {
						select = -1;
					}
					//
					for (auto& y : ButtonSel) {
						if (y.GetInto()) {
							m_MouseSelMode = true;
							select = (int)(&y - &ButtonSel.front());
						}
					}
					if ((select != -1) && (m_MouseSelMode ? Pad->GetMouseClick().trigger() : Pad->GetKey(PADS::INTERACT).trigger())) {
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
						SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (preselect != select || preMouseSel != m_MouseSelMode) {
						if (select != -1) {
							for (auto& y : ButtonSel) {
								y.SetNone();
							}
							ButtonSel.at(select).SetFocus();
							SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						else {
							for (auto& y : ButtonSel) {
								y.SetReady();
							}
						}
					}

					if (Pad->GetKey(PADS::RELOAD).trigger()) {
						SE->Get((int)SoundEnumCommon::UI_CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
						DXDraw::Instance()->PauseExit();
					}

					for (auto& y : ButtonSel) {
						y.Update();
					}
				}
			}
			void Draw() noexcept {
				for (auto& y : ButtonSel) {
					y.Draw();
				}
			}
			void Dispose() noexcept {
				m_SelectBackImage.Dispose();
				for (auto& y : ButtonSel) {
					y.Dispose();
				}
			}

		public:
			void Reset() noexcept {
				select = 0;
				m_MouseSelMode = false;
			}
			const auto&		GetIsRetireSelected(void) const noexcept { return this->m_IsRetire; }
		private:
		};
	};
};
