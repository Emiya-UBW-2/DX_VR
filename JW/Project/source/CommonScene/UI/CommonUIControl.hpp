#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ButtonStatus {
			None,
			Ready,
			Focus,
		};
		enum class ButtonMode {
			String,
			Icon,
		};
		class ButtonClass {
			GraphHandle* m_SelectBackImage{nullptr};

			int xp1{0};
			int yp1{0};
			int xsize{0};
			int ysize{0};
			FontHandle::FontXCenter LMR{};
			FontHandle::FontYCenter TMB{};

			float SelYadd{0.f};

			ButtonStatus m_ButtonStatus{ButtonStatus::Ready};
			ButtonMode m_ButtonMode{ButtonMode::String};
			bool m_EnableSelect{false};
		private:
			char m_String[64]{};
			GraphHandle m_Icon;
		public:
			void			LoadCommon(GraphHandle* BGPath) noexcept {
				m_SelectBackImage = BGPath;
			}
			void			Load_Icon(const char* IconPath, bool IsEnableSelect) noexcept {
				m_Icon = GraphHandle::Load(IconPath);
				m_Icon.GetSize(&xsize, &ysize);
				m_ButtonMode = ButtonMode::Icon;
				m_EnableSelect = IsEnableSelect;
			}
			void			Load_String(const char* String, int fontsize, bool IsEnableSelect) noexcept {
				auto* Fonts = FontPool::Instance();
				snprintfDx(m_String, 64, String);
				xsize = Fonts->Get(FontPool::FontType::MS_Gothic, fontsize, 3)->GetStringWidth(fontsize, m_String);
				ysize = fontsize;
				m_ButtonMode = ButtonMode::String;
				m_EnableSelect = IsEnableSelect;
			}
			void			Set(int xp, int yp, FontHandle::FontXCenter FontX, FontHandle::FontYCenter FontY) noexcept {
				xp1 = xp;
				yp1 = yp;
				LMR = FontX;
				TMB = FontY;
				SelYadd = 0.f;
				m_ButtonStatus = ButtonStatus::Ready;
			}
			void			Update() noexcept {
				Easing(&SelYadd, 0.f, 0.93f, EasingType::OutExpo);
			}
			void			Dispose(void) noexcept {
				m_Icon.Dispose();
			}
		public:
			const auto		IsFocus() const noexcept { return m_ButtonStatus == ButtonStatus::Focus; }
			void			SetNone() noexcept { m_ButtonStatus = ButtonStatus::None; }
			void			SetReady() noexcept { m_ButtonStatus = ButtonStatus::Ready; }
			void			SetFocus() noexcept {
				SelYadd = 10.f;
				m_ButtonStatus = ButtonStatus::Focus;
			}
		public:
			bool			GetInto() const noexcept {
				auto* DrawParts = DXDraw::Instance();
				int xp = DrawParts->GetUIY(xp1);
				int yp = DrawParts->GetUIY(yp1);
				switch (LMR) {
					case DXLibRef::FontHandle::FontXCenter::LEFT:
						xp = DrawParts->GetUIY(xp1);
						break;
					case DXLibRef::FontHandle::FontXCenter::MIDDLE:
						xp = DrawParts->GetUIY(xp1) - DrawParts->GetUIY(xsize) / 2;
						break;
					case DXLibRef::FontHandle::FontXCenter::RIGHT:
						xp = DrawParts->GetUIY(xp1) - DrawParts->GetUIY(xsize);
						break;
					default:
						break;
				}
				switch (TMB) {
					case DXLibRef::FontHandle::FontYCenter::TOP:
						yp = DrawParts->GetUIY(yp1);
						break;
					case DXLibRef::FontHandle::FontYCenter::MIDDLE:
						yp = DrawParts->GetUIY(yp1) - DrawParts->GetUIY(ysize) / 2;
						break;
					case DXLibRef::FontHandle::FontYCenter::BOTTOM:
						yp = DrawParts->GetUIY(yp1) - DrawParts->GetUIY(ysize);
						break;
					default:
						break;
				}
				return IntoMouse(xp, yp, xp + DrawParts->GetUIY(xsize), yp + DrawParts->GetUIY(ysize));
			}
			void			Draw() {
				auto* WindowParts = WindowSystem::DrawControl::Instance();
				auto* DrawParts = DXDraw::Instance();
				switch (m_ButtonMode) {
					case ButtonMode::String:
						{
							if (SelYadd > 0.f) {
								int xp = DrawParts->GetUIY(xp1);
								int yp = DrawParts->GetUIY(yp1);
								switch (LMR) {
									case DXLibRef::FontHandle::FontXCenter::LEFT:
										xp = DrawParts->GetUIY(xp1);
										break;
									case DXLibRef::FontHandle::FontXCenter::MIDDLE:
										xp = DrawParts->GetUIY(xp1) - DrawParts->GetUIY(xsize) / 2;
										break;
									case DXLibRef::FontHandle::FontXCenter::RIGHT:
										xp = DrawParts->GetUIY(xp1) - DrawParts->GetUIY(xsize);
										break;
									default:
										break;
								}
								switch (TMB) {
									case DXLibRef::FontHandle::FontYCenter::TOP:
										yp = DrawParts->GetUIY(yp1);
										break;
									case DXLibRef::FontHandle::FontYCenter::MIDDLE:
										yp = DrawParts->GetUIY(yp1) - DrawParts->GetUIY(ysize) / 2;
										break;
									case DXLibRef::FontHandle::FontYCenter::BOTTOM:
										yp = DrawParts->GetUIY(yp1) - DrawParts->GetUIY(ysize);
										break;
									default:
										break;
								}

								float per = std::clamp(SelYadd / 5.f, 0.f, 1.f);
								float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
								WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(128.f*per), 0, 255));
								WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, m_SelectBackImage,
									xp + DrawParts->GetUIY(xsize) / 2 - static_cast<int>(static_cast<float>(DrawParts->GetUIY(xsize) / 2 + DrawParts->GetUIY(300))*per2), yp + DrawParts->GetUIY(ysize) - DrawParts->GetUIY(12) - static_cast<int>(static_cast<float>(DrawParts->GetUIY(ysize) / 6) * per),
									xp + DrawParts->GetUIY(xsize) / 2 + static_cast<int>(static_cast<float>(DrawParts->GetUIY(xsize) / 2 + DrawParts->GetUIY(300))*per2), yp + DrawParts->GetUIY(ysize) - DrawParts->GetUIY(12) + static_cast<int>(static_cast<float>(DrawParts->GetUIY(ysize) / 6) * per),
									true);
								WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
							}
							unsigned int Color = Black;
							if (DrawParts->GetUIY(ysize) > DrawParts->GetUIY(50)) {
								switch (m_ButtonStatus) {
									case ButtonStatus::None:
										Color = Gray75;
										break;
									case ButtonStatus::Ready:
										Color = GetColor(148, 216, 132);
										break;
									case ButtonStatus::Focus:
										Color = Green;
										break;
									default:
										break;
								}
							}
							else {
								switch (m_ButtonStatus) {
									case ButtonStatus::None:
										Color = Gray75;
										if (!m_EnableSelect) {
											Color = GetColor(64, 48, 48);
										}
										break;
									case ButtonStatus::Ready:
										Color = Gray15;
										if (!m_EnableSelect) {
											Color = Gray65;
										}
										break;
									case ButtonStatus::Focus:
										Color = WhiteSel;
										if (!m_EnableSelect) {
											Color = GetColor(216, 143, 143);
										}
										break;
									default:
										break;
								}
							}
							WindowParts->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, 
								DrawParts->GetUIY(ysize), LMR, TMB,
																				DrawParts->GetUIY(xp1), DrawParts->GetUIY(yp1 + static_cast<int>(SelYadd)), Color, Black, m_String);
						}
						break;
					case ButtonMode::Icon:
						{
							if (SelYadd > 0.f) {
								float per1 = std::clamp(SelYadd / 5.f, 0.f, 1.f);
								float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
								WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(128.f*per1), 0, 255));
								WindowParts->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, m_SelectBackImage,
									DrawParts->GetUIY(xp1) - static_cast<int>((float)DrawParts->GetUIY(xsize)*per2), DrawParts->GetUIY(yp1) - static_cast<int>((float)DrawParts->GetUIY(ysize)*per2),
									DrawParts->GetUIY(xp1) + static_cast<int>((float)DrawParts->GetUIY(xsize)*per2), DrawParts->GetUIY(yp1) + static_cast<int>((float)DrawParts->GetUIY(ysize)*per2),
									true);
								WindowParts->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
							}
							switch (m_ButtonStatus) {
								case ButtonStatus::None:
									WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 128, 128, 128);
									break;
								case ButtonStatus::Ready:
									WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 216, 216, 216);
									break;
								case ButtonStatus::Focus:
									WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 216, 255, 216);
									break;
								default:
									break;
							}
							WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &m_Icon, DrawParts->GetUIY(xp1), DrawParts->GetUIY(yp1), (float)DrawParts->GetUIY(100) / 100.f*(1.f + SelYadd / 50.f), 0.f, true);
							WindowParts->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
						}
						break;
					default:
						break;
				}
			}
		};
	};
};
