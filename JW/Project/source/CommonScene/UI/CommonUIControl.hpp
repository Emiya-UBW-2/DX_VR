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
		private:
			char m_String[64];
			GraphHandle m_Icon;
		public:
			void			LoadCommon(GraphHandle* BGPath) noexcept {
				m_SelectBackImage = BGPath;
			}
			void			Load_Icon(const char* IconPath) noexcept {
				m_Icon = GraphHandle::Load(IconPath);
				m_Icon.GetSize(&xsize, &ysize);
				xsize = y_r(xsize);
				ysize = y_r(ysize);
				m_ButtonMode = ButtonMode::Icon;
			}
			void			Load_String(const char* String, int fontsize) noexcept {
				auto* Fonts = FontPool::Instance();
				snprintfDx(m_String, 64, String);
				xsize = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, m_String);
				ysize = fontsize;
				m_ButtonMode = ButtonMode::String;
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
			void			SetNone() noexcept { m_ButtonStatus = ButtonStatus::None; }
			void			SetReady() noexcept { m_ButtonStatus = ButtonStatus::Ready; }
			void			SetFocus() noexcept {
				SelYadd = 10.f;
				m_ButtonStatus = ButtonStatus::Focus;
			}
		public:
			bool			GetInto() {
				auto* Pad = PadControl::Instance();
				int xp = xp1;
				int yp = yp1;
				switch (LMR) {
					case DXLib_ref::FontHandle::FontXCenter::LEFT:
						xp = xp1;
						break;
					case DXLib_ref::FontHandle::FontXCenter::MIDDLE:
						xp = xp1 - xsize / 2;
						break;
					case DXLib_ref::FontHandle::FontXCenter::RIGHT:
						xp = xp1 - xsize;
						break;
					default:
						break;
				}
				switch (TMB) {
					case DXLib_ref::FontHandle::FontYCenter::TOP:
						yp = yp1;
						break;
					case DXLib_ref::FontHandle::FontYCenter::MIDDLE:
						yp = yp1 - ysize / 2;
						break;
					case DXLib_ref::FontHandle::FontYCenter::BOTTOM:
						yp = yp1 - ysize;
						break;
					default:
						break;
				}
				return in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp, yp, xp + xsize, yp + ysize);
			}
			void			Draw() {
				switch (m_ButtonMode) {
					case ButtonMode::String:
						{
							auto* Fonts = FontPool::Instance();

							auto WhiteSel = GetColor(216, 255, 216);
							auto Gray25 = GetColor(216, 216, 216);
							auto Gray = GetColor(64, 64, 64);
							auto Black = GetColor(0, 0, 0);

							if (SelYadd > 0.f) {
								int xp = xp1;
								int yp = yp1;
								switch (LMR) {
									case DXLib_ref::FontHandle::FontXCenter::LEFT:
										xp = xp1;
										break;
									case DXLib_ref::FontHandle::FontXCenter::MIDDLE:
										xp = xp1 - xsize / 2;
										break;
									case DXLib_ref::FontHandle::FontXCenter::RIGHT:
										xp = xp1 - xsize;
										break;
									default:
										break;
								}
								switch (TMB) {
									case DXLib_ref::FontHandle::FontYCenter::TOP:
										yp = yp1;
										break;
									case DXLib_ref::FontHandle::FontYCenter::MIDDLE:
										yp = yp1 - ysize / 2;
										break;
									case DXLib_ref::FontHandle::FontYCenter::BOTTOM:
										yp = yp1 - ysize;
										break;
									default:
										break;
								}

								float per = std::clamp(SelYadd / 5.f, 0.f, 1.f);
								float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(128.f*per), 0, 255));
								m_SelectBackImage->DrawExtendGraph(
									xp + xsize / 2 - (int)((float)(xsize / 2 + y_r(300))*per2), yp + ysize - y_r(12) - (int)((float)(ysize / 6) * per),
									xp + xsize / 2 + (int)((float)(xsize / 2 + y_r(300))*per2), yp + ysize - y_r(12) + (int)((float)(ysize / 6) * per),
									true);
								SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
							}
							unsigned int Color = Black;
							if (ysize > y_r(50)) {
								switch (m_ButtonStatus) {
									case ButtonStatus::None:
										Color = Gray;
										break;
									case ButtonStatus::Ready:
										Color = GetColor(148, 216, 132);
										break;
									case ButtonStatus::Focus:
										Color = GetColor(0, 255, 0);
										break;
									default:
										break;
								}
							}
							else {
								switch (m_ButtonStatus) {
									case ButtonStatus::None:
										Color = Gray;
										break;
									case ButtonStatus::Ready:
										Color = Gray25;
										break;
									case ButtonStatus::Focus:
										Color = WhiteSel;
										break;
									default:
										break;
								}
							}
							Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(ysize, LMR, TMB,
																				xp1, yp1 + y_r(SelYadd), Color, Black, m_String);
						}
						break;
					case ButtonMode::Icon:
						{
							if (SelYadd > 0.f) {
								float per1 = std::clamp(SelYadd / 5.f, 0.f, 1.f);
								float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(128.f*per1), 0, 255));
								m_SelectBackImage->DrawExtendGraph(
									xp1 - (int)((float)xsize*per2), yp1 - (int)((float)ysize*per2),
									xp1 + (int)((float)xsize*per2), yp1 + (int)((float)ysize*per2),
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
							m_Icon.DrawRotaGraph(xp1, yp1, (float)y_r(100) / 100.f*(1.f + SelYadd / 50.f), 0.f, true);
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
