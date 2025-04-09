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
			class ButtonClass {
				GraphHandle*	m_SelectBackImage{nullptr};

				int xp1{0};
				int yp1{0};
				int xsize{0};
				int ysize{0};
				FontSystem::FontXCenter LMR{ FontSystem::FontXCenter::LEFT };
				FontSystem::FontYCenter TMB{ FontSystem::FontYCenter::TOP };

				float SelYadd{0.f};

				ButtonStatus	m_ButtonStatus{ButtonStatus::Ready};
				ButtonMode		m_ButtonMode{ButtonMode::String};
				bool			m_EnableSelect{false};
			private:
				char			m_String[64]{};
				GraphHandle		m_Icon;

			public:
				ButtonClass(GraphHandle* BGPath, int xp, int yp, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY) noexcept {
					LoadCommon(BGPath);
					Set(xp, yp, FontX, FontY);
				}
				ButtonClass(const ButtonClass&) = delete;
				ButtonClass(ButtonClass&&) = delete;
				ButtonClass& operator=(const ButtonClass&) = delete;
				ButtonClass& operator=(ButtonClass&&) = delete;
				~ButtonClass(void) noexcept { Dispose(); }
			public:
				void			LoadCommon(GraphHandle* BGPath) noexcept {
					this->m_SelectBackImage = BGPath;
				}
				void			Set(int xp, int yp, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY) noexcept {
					xp1 = xp;
					yp1 = yp;
					LMR = FontX;
					TMB = FontY;
					SelYadd = 0.f;
					this->m_ButtonStatus = ButtonStatus::Ready;
				}
				void			Load_Icon(const char* IconPath, bool IsEnableSelect) noexcept {
					this->m_Icon.Load(IconPath);
					this->m_Icon.GetSize(&xsize, &ysize);
					this->m_ButtonMode = ButtonMode::Icon;
					this->m_EnableSelect = IsEnableSelect;
				}
				void			Load_String(const char* String, int fontsize, bool IsEnableSelect) noexcept {
					snprintfDx(this->m_String, 64, String);
					xsize = FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, fontsize, 3)->GetStringWidth(this->m_String);
					ysize = fontsize;
					this->m_ButtonMode = ButtonMode::String;
					this->m_EnableSelect = IsEnableSelect;
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
				bool			GetInto(void) const noexcept {
					int xp = xp1;
					int yp = yp1;
					switch (LMR) {
						case FontSystem::FontXCenter::LEFT:
							xp = xp1;
							break;
						case FontSystem::FontXCenter::MIDDLE:
							xp = xp1 - xsize / 2;
							break;
						case FontSystem::FontXCenter::RIGHT:
							xp = xp1 - (xsize);
							break;
						default:
							break;
					}
					switch (TMB) {
						case FontSystem::FontYCenter::TOP:
							yp = (yp1);
							break;
						case FontSystem::FontYCenter::MIDDLE:
							yp = (yp1) - (ysize) / 2;
							break;
						case FontSystem::FontYCenter::BOTTOM:
							yp = (yp1) - (ysize);
							break;
						default:
							break;
					}
					return IntoMouse(xp, yp, xp + (xsize), yp + (ysize));
				}
				void			Draw(void) noexcept {
					auto* DrawCtrls = WindowSystem::DrawControl::Instance();
					switch (this->m_ButtonMode) {
						case ButtonMode::String:
							{
								if (SelYadd > 0.f) {
									int xp = (xp1);
									int yp = (yp1);
									switch (LMR) {
										case FontSystem::FontXCenter::LEFT:
											xp = (xp1);
											break;
										case FontSystem::FontXCenter::MIDDLE:
											xp = (xp1) - (xsize) / 2;
											break;
										case FontSystem::FontXCenter::RIGHT:
											xp = (xp1) - (xsize);
											break;
										default:
											break;
									}
									switch (TMB) {
										case FontSystem::FontYCenter::TOP:
											yp = (yp1);
											break;
										case FontSystem::FontYCenter::MIDDLE:
											yp = (yp1) - (ysize) / 2;
											break;
										case FontSystem::FontYCenter::BOTTOM:
											yp = (yp1) - (ysize);
											break;
										default:
											break;
									}

									float per = std::clamp(SelYadd / 5.f, 0.f, 1.f);
									float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
									DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(128.f * per), 0, 255));
									DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, 
										this->m_SelectBackImage,
										xp + (xsize) / 2 - static_cast<int>(static_cast<float>((xsize) / 2 + (300)) * per2), yp + (ysize) - (12) - static_cast<int>(static_cast<float>((ysize) / 6) * per),
										xp + (xsize) / 2 + static_cast<int>(static_cast<float>((xsize) / 2 + (300)) * per2), yp + (ysize) - (12) + static_cast<int>(static_cast<float>((ysize) / 6) * per),
										true);
									DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
								}
								unsigned int Color = Black;
								if ((ysize) > (50)) {
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
								DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, (ysize),
									LMR, TMB, (xp1), (yp1 + static_cast<int>(SelYadd)), Color, Black, this->m_String);
							}
							break;
						case ButtonMode::Icon:
							{
								if (SelYadd > 0.f) {
									float per1 = std::clamp(SelYadd / 5.f, 0.f, 1.f);
									float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
									DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(128.f * per1), 0, 255));
									DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
										this->m_SelectBackImage,
										(xp1) - static_cast<int>(static_cast<float>((xsize)) * per2), (yp1) - static_cast<int>(static_cast<float>((ysize)) * per2),
										(xp1) + static_cast<int>(static_cast<float>((xsize)) * per2), (yp1) + static_cast<int>(static_cast<float>((ysize)) * per2),
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
									(xp1), (yp1), static_cast<float>((100)) / 100.f * (1.f + SelYadd / 50.f), 0.f, true);
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
			std::vector<std::shared_ptr<ButtonClass>>	m_ButtonSel{};
			int							m_select{0};
			bool						m_MouseSelMode{false};
		public:
			const auto& GetSelect(void) const noexcept { return m_select; }
			bool		GetTriggerButton(void) const noexcept;
		private:
			ButtonControl(void) noexcept;
			ButtonControl(const ButtonControl&) = delete;
			ButtonControl(ButtonControl&&) = delete;
			ButtonControl& operator=(const ButtonControl&) = delete;
			ButtonControl& operator=(ButtonControl&&) = delete;
				
			virtual ~ButtonControl(void) noexcept;
		public:
			void ResetSel(void) noexcept {
				m_select = 0;
				this->m_MouseSelMode = false;
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
				m_ButtonSel.emplace_back(std::make_shared<ButtonClass>(&this->m_SelectBackImage, xp, yp, FontX, FontY));
				m_ButtonSel.back()->Load_String(String, fontsize, IsEnableSelect);
			}
			void AddIconButton(
				const char* IconPath, bool IsEnableSelect,
				int xp, int yp, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY
			) noexcept {
				m_ButtonSel.emplace_back(std::make_shared<ButtonClass>(&this->m_SelectBackImage, xp, yp, FontX, FontY));
				m_ButtonSel.back()->Load_Icon(IconPath, IsEnableSelect);
			}
		};
		// 
		class CreditControl {
			static const int			CharMax = 256;
			int							m_CreditCoulm{0};
			std::array<std::pair<char[CharMax], char[CharMax]>, 64>	m_CreditStr{};
		public:
			CreditControl(void) noexcept { Init(); }
			CreditControl(const CreditControl&) = delete;
			CreditControl(CreditControl&&) = delete;
			CreditControl& operator=(const CreditControl&) = delete;
			CreditControl& operator=(CreditControl&&) = delete;

			~CreditControl(void) noexcept { Dispose(); }
		public:
			void Draw(int xmin, int ymin, int xmax) const noexcept;
		private:
			void Init(void) noexcept;
			void Dispose(void) noexcept;
		};
		// 
	};
};
