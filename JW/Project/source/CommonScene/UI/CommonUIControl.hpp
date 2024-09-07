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
			FontHandle::FontXCenter LMR;
			FontHandle::FontYCenter TMB;

			float SelYadd{0.f};

			ButtonStatus m_ButtonStatus{ButtonStatus::Ready};
			ButtonMode m_ButtonMode{ButtonMode::String};
			bool m_EnableSelect{false};
		private:
			char m_String[64];
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
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(128.f*per), 0, 255));
								m_SelectBackImage->DrawExtendGraph(
									xp + DrawParts->GetUIY(xsize) / 2 - (int)((float)(DrawParts->GetUIY(xsize) / 2 + DrawParts->GetUIY(300))*per2), yp + DrawParts->GetUIY(ysize) - DrawParts->GetUIY(12) - (int)((float)(DrawParts->GetUIY(ysize) / 6) * per),
									xp + DrawParts->GetUIY(xsize) / 2 + (int)((float)(DrawParts->GetUIY(xsize) / 2 + DrawParts->GetUIY(300))*per2), yp + DrawParts->GetUIY(ysize) - DrawParts->GetUIY(12) + (int)((float)(DrawParts->GetUIY(ysize) / 6) * per),
									true);
								SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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
							WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, 
								DrawParts->GetUIY(ysize), LMR, TMB,
																				DrawParts->GetUIY(xp1), DrawParts->GetUIY(yp1 + (int)(SelYadd)), Color, Black, m_String);
						}
						break;
					case ButtonMode::Icon:
						{
							if (SelYadd > 0.f) {
								float per1 = std::clamp(SelYadd / 5.f, 0.f, 1.f);
								float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(128.f*per1), 0, 255));
								m_SelectBackImage->DrawExtendGraph(
									DrawParts->GetUIY(xp1) - (int)((float)DrawParts->GetUIY(xsize)*per2), DrawParts->GetUIY(yp1) - (int)((float)DrawParts->GetUIY(ysize)*per2),
									DrawParts->GetUIY(xp1) + (int)((float)DrawParts->GetUIY(xsize)*per2), DrawParts->GetUIY(yp1) + (int)((float)DrawParts->GetUIY(ysize)*per2),
									true);
								SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
							}
							switch (m_ButtonStatus) {
								case ButtonStatus::None:
									SetDrawBright(128, 128, 128);
									break;
								case ButtonStatus::Ready:
									SetDrawBright(216, 216, 216);
									break;
								case ButtonStatus::Focus:
									SetDrawBright(216, 255, 216);
									break;
								default:
									break;
							}
							m_Icon.DrawRotaGraph(DrawParts->GetUIY(xp1), DrawParts->GetUIY(yp1), (float)DrawParts->GetUIY(100) / 100.f*(1.f + SelYadd / 50.f), 0.f, true);
							SetDrawBright(255, 255, 255);
						}
						break;
					default:
						break;
				}
			}
		};
	};
};
