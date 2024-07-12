#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		// 
		class ButtonControl : public SingletonBase<ButtonControl> {
		private:
			friend class SingletonBase<ButtonControl>;
		private:
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
				FontHandle::FontXCenter LMR{ FontHandle::FontXCenter::LEFT };
				FontHandle::FontYCenter TMB{ FontHandle::FontYCenter::TOP };

				float SelYadd{0.f};

				ButtonStatus m_ButtonStatus{ButtonStatus::Ready};
				ButtonMode m_ButtonMode{ButtonMode::String};
				bool m_EnableSelect{false};
			private:
				char m_String[64]{};
				GraphHandle m_Icon;

			public:
				ButtonClass(void) {}
				ButtonClass(const ButtonClass&) = delete;
				ButtonClass(ButtonClass&& o) = delete;
				ButtonClass& operator=(const ButtonClass&) = delete;
				ButtonClass& operator=(ButtonClass&& o) = delete;
			public:
				void			LoadCommon(GraphHandle* BGPath) noexcept {
					this->m_SelectBackImage = BGPath;
				}
				void			Load_Icon(const char* IconPath, bool IsEnableSelect) noexcept {
					this->m_Icon = GraphHandle::Load(IconPath);
					this->m_Icon.GetSize(&xsize, &ysize);
					this->m_ButtonMode = ButtonMode::Icon;
					this->m_EnableSelect = IsEnableSelect;
				}
				void			Load_String(const char* String, int fontsize, bool IsEnableSelect) noexcept {
					auto* Fonts = FontPool::Instance();
					snprintfDx(this->m_String, 64, String);
					xsize = Fonts->Get(FontPool::FontType::Nomal_AA).GetStringWidth(fontsize, this->m_String);
					ysize = fontsize;
					this->m_ButtonMode = ButtonMode::String;
					this->m_EnableSelect = IsEnableSelect;
				}
				void			Set(int xp, int yp, FontHandle::FontXCenter FontX, FontHandle::FontYCenter FontY) noexcept {
					xp1 = xp;
					yp1 = yp;
					LMR = FontX;
					TMB = FontY;
					SelYadd = 0.f;
					this->m_ButtonStatus = ButtonStatus::Ready;
				}
				void			Update(void) noexcept {
					Easing(&SelYadd, 0.f, 0.93f, EasingType::OutExpo);
				}
				void			Dispose(void) noexcept {
					this->m_Icon.Dispose();
				}
			public:
				auto			IsFocus(void) const noexcept { return this->m_ButtonStatus == ButtonStatus::Focus; }
				void			SetNone(void) noexcept { this->m_ButtonStatus = ButtonStatus::None; }
				void			SetReady(void) noexcept { this->m_ButtonStatus = ButtonStatus::Ready; }
				void			SetFocus(void) noexcept {
					SelYadd = 10.f;
					this->m_ButtonStatus = ButtonStatus::Focus;
				}
			public:
				bool			GetInto(void) noexcept {
					int xp = y_UI(xp1);
					int yp = y_UI(yp1);
					switch (LMR) {
						case FontHandle::FontXCenter::LEFT:
							xp = y_UI(xp1);
							break;
						case FontHandle::FontXCenter::MIDDLE:
							xp = y_UI(xp1) - y_UI(xsize) / 2;
							break;
						case FontHandle::FontXCenter::RIGHT:
							xp = y_UI(xp1) - y_UI(xsize);
							break;
						default:
							break;
					}
					switch (TMB) {
						case FontHandle::FontYCenter::TOP:
							yp = y_UI(yp1);
							break;
						case FontHandle::FontYCenter::MIDDLE:
							yp = y_UI(yp1) - y_UI(ysize) / 2;
							break;
						case FontHandle::FontYCenter::BOTTOM:
							yp = y_UI(yp1) - y_UI(ysize);
							break;
						default:
							break;
					}
					return IntoMouse(xp, yp, xp + y_UI(xsize), yp + y_UI(ysize));
				}
				void			Draw(void) noexcept {
					switch (this->m_ButtonMode) {
						case ButtonMode::String:
							{
								auto* Fonts = FontPool::Instance();
								if (SelYadd > 0.f) {
									int xp = y_UI(xp1);
									int yp = y_UI(yp1);
									switch (LMR) {
										case FontHandle::FontXCenter::LEFT:
											xp = y_UI(xp1);
											break;
										case FontHandle::FontXCenter::MIDDLE:
											xp = y_UI(xp1) - y_UI(xsize) / 2;
											break;
										case FontHandle::FontXCenter::RIGHT:
											xp = y_UI(xp1) - y_UI(xsize);
											break;
										default:
											break;
									}
									switch (TMB) {
										case FontHandle::FontYCenter::TOP:
											yp = y_UI(yp1);
											break;
										case FontHandle::FontYCenter::MIDDLE:
											yp = y_UI(yp1) - y_UI(ysize) / 2;
											break;
										case FontHandle::FontYCenter::BOTTOM:
											yp = y_UI(yp1) - y_UI(ysize);
											break;
										default:
											break;
									}

									float per = std::clamp(SelYadd / 5.f, 0.f, 1.f);
									float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(128.f * per), 0, 255));
									this->m_SelectBackImage->DrawExtendGraph(
										xp + y_UI(xsize) / 2 - static_cast<int>(static_cast<float>(y_UI(xsize) / 2 + y_UI(300)) * per2), yp + y_UI(ysize) - y_UI(12) - static_cast<int>(static_cast<float>(y_UI(ysize) / 6) * per),
										xp + y_UI(xsize) / 2 + static_cast<int>(static_cast<float>(y_UI(xsize) / 2 + y_UI(300)) * per2), yp + y_UI(ysize) - y_UI(12) + static_cast<int>(static_cast<float>(y_UI(ysize) / 6) * per),
										true);
									SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
								}
								unsigned int Color = Black;
								if (y_UI(ysize) > y_UI(50)) {
									switch (this->m_ButtonStatus) {
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
									switch (this->m_ButtonStatus) {
										case ButtonStatus::None:
											Color = Gray75;
											if (!this->m_EnableSelect) {
												Color = GetColor(64, 48, 48);
											}
											break;
										case ButtonStatus::Ready:
											Color = Gray15;
											if (!this->m_EnableSelect) {
												Color = Gray65;
											}
											break;
										case ButtonStatus::Focus:
											Color = WhiteSel;
											if (!this->m_EnableSelect) {
												Color = GetColor(216, 143, 143);
											}
											break;
										default:
											break;
									}
								}
								Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_UI(ysize), LMR, TMB,
																					y_UI(xp1), y_UI(yp1 + static_cast<int>(SelYadd)), Color, Black, this->m_String);
							}
							break;
						case ButtonMode::Icon:
							{
								if (SelYadd > 0.f) {
									float per1 = std::clamp(SelYadd / 5.f, 0.f, 1.f);
									float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(128.f * per1), 0, 255));
									this->m_SelectBackImage->DrawExtendGraph(
										y_UI(xp1) - static_cast<int>(static_cast<float>(y_UI(xsize)) * per2), y_UI(yp1) - static_cast<int>(static_cast<float>(y_UI(ysize)) * per2),
										y_UI(xp1) + static_cast<int>(static_cast<float>(y_UI(xsize)) * per2), y_UI(yp1) + static_cast<int>(static_cast<float>(y_UI(ysize)) * per2),
										true);
									SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
								}
								switch (this->m_ButtonStatus) {
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
								this->m_Icon.DrawRotaGraph(y_UI(xp1), y_UI(yp1), static_cast<float>(y_UI(100)) / 100.f * (1.f + SelYadd / 50.f), 0.f, true);
								SetDrawBright(255, 255, 255);
							}
							break;
						default:
							break;
					}
				}
			};
		private:
			GraphHandle					m_SelectBackImage;
			std::vector<std::shared_ptr<ButtonClass>>	ButtonSel{};
			int							select{0};
			bool						m_MouseSelMode{false};
		public:
			const auto& GetSelect(void) const noexcept { return select; }
			bool		GetTriggerButton(void) const noexcept;
		private:
			ButtonControl(void) noexcept;
			ButtonControl(const ButtonControl&) = delete;
			ButtonControl(ButtonControl&& o) = delete;
			ButtonControl& operator=(const ButtonControl&) = delete;
			ButtonControl& operator=(ButtonControl&& o) = delete;
				
			virtual ~ButtonControl(void) noexcept;
		public:
			void ResetSel(void) noexcept {
				select = 0;
				this->m_MouseSelMode = false;
			}
			void UpdateInput(void) noexcept;
			void Update(void) noexcept;
			void Draw(void) noexcept;
			void Dispose(void) noexcept;
		public:
			void AddStringButton(
				const char* String, int fontsize, bool IsEnableSelect,
				int xp, int yp, FontHandle::FontXCenter FontX, FontHandle::FontYCenter FontY
			) noexcept {
				ButtonSel.emplace_back(std::make_shared<ButtonClass>());
				ButtonSel.back()->LoadCommon(&this->m_SelectBackImage);
				ButtonSel.back()->Load_String(String, fontsize, IsEnableSelect);
				ButtonSel.back()->Set(xp, yp, FontX, FontY);
			}
			void AddIconButton(
				const char* IconPath, bool IsEnableSelect,
				int xp, int yp, FontHandle::FontXCenter FontX, FontHandle::FontYCenter FontY
			) noexcept {
				ButtonSel.emplace_back(std::make_shared<ButtonClass>());
				ButtonSel.back()->LoadCommon(&this->m_SelectBackImage);
				ButtonSel.back()->Load_Icon(IconPath, IsEnableSelect);
				ButtonSel.back()->Set(xp, yp, FontX, FontY);
			}
		};
		// 
		class CreditControl {
			static const int			CharMax = 256;
			int							m_CreditCoulm{0};
			std::array<std::pair<char[CharMax], char[CharMax]>, 64> m_CreditStr{};
		public:
			CreditControl(void) {}
			CreditControl(const CreditControl&) = delete;
			CreditControl(CreditControl&& o) = delete;
			CreditControl& operator=(const CreditControl&) = delete;
			CreditControl& operator=(CreditControl&& o) = delete;
		public:
			void Init(void) noexcept;
			void Draw(int xmin, int ymin, int xmax) const noexcept;
			void Dispose(void) noexcept;
		};
		// 
	};
};
