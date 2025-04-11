#pragma once
#pragma warning(disable:4464)
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
			class ButtonOnce {
				GraphHandle*	m_SelectBackImage{nullptr};

				int				m_xp1{0};
				int				m_yp1{0};
				int				m_xsize{0};
				int				m_ysize{0};
				FontSystem::FontXCenter	m_LMR{ FontSystem::FontXCenter::LEFT };
				FontSystem::FontYCenter	m_TMB{ FontSystem::FontYCenter::TOP };

				float			m_SelectYadd{0.f};

				ButtonStatus	m_ButtonStatus{ButtonStatus::Ready};
				ButtonMode		m_ButtonMode{ButtonMode::String};
				bool			m_EnableSelect{false};
			private:
				char			m_String[64]{};
				GraphHandle		m_Icon;

			public:
				ButtonOnce(GraphHandle* BGPath, int xp, int yp, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY) noexcept {
					LoadCommon(BGPath);
					Set(xp, yp, FontX, FontY);
				}
				ButtonOnce(const ButtonOnce&) = delete;
				ButtonOnce(ButtonOnce&&) = delete;
				ButtonOnce& operator=(const ButtonOnce&) = delete;
				ButtonOnce& operator=(ButtonOnce&&) = delete;
				virtual ~ButtonOnce(void) noexcept { Dispose(); }
			public:
				void			LoadCommon(GraphHandle* BGPath) noexcept {
					this->m_SelectBackImage = BGPath;
				}
				void			Set(int xp, int yp, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY) noexcept {
					this->m_xp1 = xp;
					this->m_yp1 = yp;
					this->m_LMR = FontX;
					this->m_TMB = FontY;
					this->m_SelectYadd = 0.f;
					this->m_ButtonStatus = ButtonStatus::Ready;
				}
				void			Load_Icon(const char* IconPath, bool IsEnableSelect) noexcept {
					this->m_Icon.Load(IconPath);
					this->m_Icon.GetSize(&this->m_xsize, &this->m_ysize);
					this->m_ButtonMode = ButtonMode::Icon;
					this->m_EnableSelect = IsEnableSelect;
				}
				void			Load_String(const char* String, int fontsize, bool IsEnableSelect) noexcept {
					snprintfDx(this->m_String, 64, String);
					this->m_xsize = FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, fontsize, 3)->GetStringWidth(this->m_String);
					this->m_ysize = fontsize;
					this->m_ButtonMode = ButtonMode::String;
					this->m_EnableSelect = IsEnableSelect;
				}
				void			Update(void) noexcept {
					Easing(&this->m_SelectYadd, 0.f, 0.93f, EasingType::OutExpo);
				}
				void			Dispose(void) noexcept {
					this->m_Icon.Dispose();
				}
			public:
				auto			IsFocus(void) const noexcept { return this->m_ButtonStatus == ButtonStatus::Focus; }
				void			SetNone(void) noexcept { this->m_ButtonStatus = ButtonStatus::None; }
				void			SetReady(void) noexcept { this->m_ButtonStatus = ButtonStatus::Ready; }
				void			SetFocus(void) noexcept {
					this->m_SelectYadd = 10.f;
					this->m_ButtonStatus = ButtonStatus::Focus;
				}
			public:
				bool			GetInto(void) const noexcept {
					int xp = this->m_xp1;
					int yp = this->m_yp1;
					switch (this->m_LMR) {
						case FontSystem::FontXCenter::LEFT:
							xp = this->m_xp1;
							break;
						case FontSystem::FontXCenter::MIDDLE:
							xp = this->m_xp1 - this->m_xsize / 2;
							break;
						case FontSystem::FontXCenter::RIGHT:
							xp = this->m_xp1 - this->m_xsize;
							break;
						default:
							break;
					}
					switch (this->m_TMB) {
						case FontSystem::FontYCenter::TOP:
							yp = this->m_yp1;
							break;
						case FontSystem::FontYCenter::MIDDLE:
							yp = this->m_yp1 -this->m_ysize / 2;
							break;
						case FontSystem::FontYCenter::BOTTOM:
							yp = this->m_yp1 - this->m_ysize;
							break;
						default:
							break;
					}
					return IntoMouse(xp, yp, xp + this->m_xsize, yp + this->m_ysize);
				}
				void			Draw(void) noexcept {
					auto* DrawCtrls = WindowSystem::DrawControl::Instance();
					switch (this->m_ButtonMode) {
						case ButtonMode::String:
							{
								if (this->m_SelectYadd > 0.f) {
									int xp = this->m_xp1;
									int yp = this->m_yp1;
									switch (this->m_LMR) {
										case FontSystem::FontXCenter::LEFT:
											xp = this->m_xp1;
											break;
										case FontSystem::FontXCenter::MIDDLE:
											xp = this->m_xp1 -this->m_xsize / 2;
											break;
										case FontSystem::FontXCenter::RIGHT:
											xp = this->m_xp1 - this->m_xsize;
											break;
										default:
											break;
									}
									switch (this->m_TMB) {
										case FontSystem::FontYCenter::TOP:
											yp = this->m_yp1;
											break;
										case FontSystem::FontYCenter::MIDDLE:
											yp = this->m_yp1 - this->m_ysize / 2;
											break;
										case FontSystem::FontYCenter::BOTTOM:
											yp = this->m_yp1 - this->m_ysize;
											break;
										default:
											break;
									}

									float per = std::clamp(this->m_SelectYadd / 5.f, 0.f, 1.f);
									float per2 = 1.f - std::clamp(this->m_SelectYadd / 10.f, 0.f, 1.f);
									DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(128.f * per), 0, 255));
									DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, 
										this->m_SelectBackImage,
										xp + this->m_xsize / 2 - static_cast<int>(static_cast<float>(this->m_xsize / 2 + (300)) * per2), yp + this->m_ysize - (12) - static_cast<int>(static_cast<float>(this->m_ysize / 6) * per),
										xp + this->m_xsize / 2 + static_cast<int>(static_cast<float>(this->m_xsize / 2 + (300)) * per2), yp + this->m_ysize - (12) + static_cast<int>(static_cast<float>(this->m_ysize / 6) * per),
										true);
									DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
								}
								unsigned int Color = Black;
								if (this->m_ysize > (50)) {
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
											Color = White;
											if (!this->m_EnableSelect) {
												Color = GetColor(216, 143, 143);
											}
											break;
										default:
											break;
									}
								}
								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, this->m_ysize,
									this->m_LMR, this->m_TMB, this->m_xp1, (this->m_yp1 + static_cast<int>(this->m_SelectYadd)), Color, Black, this->m_String);
							}
							break;
						case ButtonMode::Icon:
							{
								if (this->m_SelectYadd > 0.f) {
									float per1 = std::clamp(this->m_SelectYadd / 5.f, 0.f, 1.f);
									float per2 = 1.f - std::clamp(this->m_SelectYadd / 10.f, 0.f, 1.f);
									DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(128.f * per1), 0, 255));
									DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
										this->m_SelectBackImage,
										this->m_xp1 - static_cast<int>(static_cast<float>(this->m_xsize) * per2), this->m_yp1 - static_cast<int>(static_cast<float>(this->m_ysize) * per2),
										this->m_xp1 + static_cast<int>(static_cast<float>(this->m_xsize) * per2), this->m_yp1 + static_cast<int>(static_cast<float>(this->m_ysize) * per2),
										true);
									DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
								}
								switch (this->m_ButtonStatus) {
									case ButtonStatus::None:
										DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 128, 128, 128);
										break;
									case ButtonStatus::Ready:
										DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 216, 216, 216);
										break;
									case ButtonStatus::Focus:
										DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 216, 255, 216);
										break;
									default:
										break;
								}
								DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
									&this->m_Icon,
									this->m_xp1, this->m_yp1, static_cast<float>((100)) / 100.f * (1.f + this->m_SelectYadd / 50.f), 0.f, true);
								DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
							}
							break;
						default:
							break;
					}
				}
			};
		private:
			GraphHandle					m_SelectBackImage;
			std::vector<std::unique_ptr<ButtonOnce>>	m_ButtonSelect{};
			int							m_select{0};
			bool						m_MouseSelectMode{false};
		public:
			const auto& GetSelect(void) const noexcept { return this->m_select; }
			bool		GetTriggerButton(void) const noexcept;
		private:
			ButtonControl(void) noexcept;
			ButtonControl(const ButtonControl&) = delete;
			ButtonControl(ButtonControl&&) = delete;
			ButtonControl& operator=(const ButtonControl&) = delete;
			ButtonControl& operator=(ButtonControl&&) = delete;
				
			virtual ~ButtonControl(void) noexcept;
		public:
			void ResetSelect(void) noexcept {
				this->m_select = 0;
				this->m_MouseSelectMode = false;
			}
			void UpdateInput(void) noexcept;
			void Update(void) noexcept;
			void Draw(void) noexcept;
			void Dispose(void) noexcept;
		public:
			void AddStringButton(
				const char* String, int fontsize, bool IsEnableSelect,
				int xp, int yp, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY
			) noexcept {
				this->m_ButtonSelect.emplace_back(std::make_unique<ButtonOnce>(&this->m_SelectBackImage, xp, yp, FontX, FontY));
				this->m_ButtonSelect.back()->Load_String(String, fontsize, IsEnableSelect);
			}
			void AddIconButton(
				const char* IconPath, bool IsEnableSelect,
				int xp, int yp, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY
			) noexcept {
				this->m_ButtonSelect.emplace_back(std::make_unique<ButtonOnce>(&this->m_SelectBackImage, xp, yp, FontX, FontY));
				this->m_ButtonSelect.back()->Load_Icon(IconPath, IsEnableSelect);
			}
		};
		// 
		class CreditControl {
			static const int			CharMax = 256;
			int							m_CreditCoulm{0};
			std::array<std::pair<char[CharMax], char[CharMax]>, 64>	m_CreditStr{};
		public:
			CreditControl(void) noexcept;
			CreditControl(const CreditControl&) = delete;
			CreditControl(CreditControl&&) = delete;
			CreditControl& operator=(const CreditControl&) = delete;
			CreditControl& operator=(CreditControl&&) = delete;

			virtual ~CreditControl(void) noexcept;
		public:
			void Draw(int xmin, int ymin, int xmax) const noexcept;
		};
		// 
	};
};
