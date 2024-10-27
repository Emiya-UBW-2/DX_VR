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
				GraphHandle* m_SelectBackImage{ nullptr };

				int xp1{ 0 };
				int yp1{ 0 };
				int xsize{ 0 };
				int ysize{ 0 };
				FontHandle::FontXCenter LMR{ FontHandle::FontXCenter::LEFT };
				FontHandle::FontYCenter TMB{ FontHandle::FontYCenter::TOP };

				float SelYadd{ 0.f };

				ButtonStatus m_ButtonStatus{ ButtonStatus::Ready };
				ButtonMode m_ButtonMode{ ButtonMode::String };
				bool m_EnableSelect{ false };
			private:
				char m_String[64]{};
				GraphHandle m_Icon;

			public:
				ButtonClass(void) noexcept {}
				ButtonClass(const ButtonClass&) = delete;
				ButtonClass(ButtonClass&& o) = delete;
				ButtonClass& operator=(const ButtonClass&) = delete;
				ButtonClass& operator=(ButtonClass&& o) = delete;
			public:
				void			LoadCommon(GraphHandle* BGPath) noexcept {
					this->m_SelectBackImage = BGPath;
				}
				void			Load_Icon(const char* IconPath, bool IsEnableSelect) noexcept {
					this->m_Icon.Load(IconPath);
					this->m_Icon.GetSize(&xsize, &ysize);
					this->m_ButtonMode = ButtonMode::Icon;
					this->m_EnableSelect = IsEnableSelect;
				}
				void			Load_String(const char* String, int fontsize, bool IsEnableSelect) noexcept {
					snprintfDx(this->m_String, 64, String);
					xsize = WindowSystem::GetMsgLen(fontsize, this->m_String);
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
				bool			GetInto(void) const noexcept {
					auto* DrawParts = DXDraw::Instance();
					int xp = DrawParts->GetUIY(xp1);
					int yp = DrawParts->GetUIY(yp1);
					switch (LMR) {
					case FontHandle::FontXCenter::LEFT:
						xp = DrawParts->GetUIY(xp1);
						break;
					case FontHandle::FontXCenter::MIDDLE:
						xp = DrawParts->GetUIY(xp1) - DrawParts->GetUIY(xsize) / 2;
						break;
					case FontHandle::FontXCenter::RIGHT:
						xp = DrawParts->GetUIY(xp1) - DrawParts->GetUIY(xsize);
						break;
					default:
						break;
					}
					switch (TMB) {
					case FontHandle::FontYCenter::TOP:
						yp = DrawParts->GetUIY(yp1);
						break;
					case FontHandle::FontYCenter::MIDDLE:
						yp = DrawParts->GetUIY(yp1) - DrawParts->GetUIY(ysize) / 2;
						break;
					case FontHandle::FontYCenter::BOTTOM:
						yp = DrawParts->GetUIY(yp1) - DrawParts->GetUIY(ysize);
						break;
					default:
						break;
					}
					return IntoMouse(xp, yp, xp + DrawParts->GetUIY(xsize), yp + DrawParts->GetUIY(ysize));
				}
				void			Draw(void) noexcept {
					auto* DrawParts = DXDraw::Instance();
					switch (this->m_ButtonMode) {
					case ButtonMode::String:
					{
						if (SelYadd > 0.f) {
							int xp = DrawParts->GetUIY(xp1);
							int yp = DrawParts->GetUIY(yp1);
							switch (LMR) {
							case FontHandle::FontXCenter::LEFT:
								xp = DrawParts->GetUIY(xp1);
								break;
							case FontHandle::FontXCenter::MIDDLE:
								xp = DrawParts->GetUIY(xp1) - DrawParts->GetUIY(xsize) / 2;
								break;
							case FontHandle::FontXCenter::RIGHT:
								xp = DrawParts->GetUIY(xp1) - DrawParts->GetUIY(xsize);
								break;
							default:
								break;
							}
							switch (TMB) {
							case FontHandle::FontYCenter::TOP:
								yp = DrawParts->GetUIY(yp1);
								break;
							case FontHandle::FontYCenter::MIDDLE:
								yp = DrawParts->GetUIY(yp1) - DrawParts->GetUIY(ysize) / 2;
								break;
							case FontHandle::FontYCenter::BOTTOM:
								yp = DrawParts->GetUIY(yp1) - DrawParts->GetUIY(ysize);
								break;
							default:
								break;
							}

							float per = std::clamp(SelYadd / 5.f, 0.f, 1.f);
							float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
							WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(128.f * per), 0, 255));
							WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
								this->m_SelectBackImage,
								xp + DrawParts->GetUIY(xsize) / 2 - static_cast<int>(static_cast<float>(DrawParts->GetUIY(xsize) / 2 + DrawParts->GetUIY(300)) * per2), yp + DrawParts->GetUIY(ysize) - DrawParts->GetUIY(12) - static_cast<int>(static_cast<float>(DrawParts->GetUIY(ysize) / 6) * per),
								xp + DrawParts->GetUIY(xsize) / 2 + static_cast<int>(static_cast<float>(DrawParts->GetUIY(xsize) / 2 + DrawParts->GetUIY(300)) * per2), yp + DrawParts->GetUIY(ysize) - DrawParts->GetUIY(12) + static_cast<int>(static_cast<float>(DrawParts->GetUIY(ysize) / 6) * per),
								true);
							WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}
						unsigned int Color = Black;
						if (DrawParts->GetUIY(ysize) > DrawParts->GetUIY(50)) {
							switch (this->m_ButtonStatus) {
							case ButtonStatus::None:
								Color = Gray75;
								break;
							case ButtonStatus::Ready:
								Color = GetColor(64, 192, 64);
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
								Color = Green;
								if (!this->m_EnableSelect) {
									Color = GetColor(216, 143, 143);
								}
								break;
							default:
								break;
							}
						}
						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic, DrawParts->GetUIY(ysize),
							LMR, TMB, DrawParts->GetUIY(xp1), DrawParts->GetUIY(yp1 + static_cast<int>(SelYadd)), Color, Black, this->m_String);
					}
					break;
					case ButtonMode::Icon:
					{
						if (SelYadd > 0.f) {
							float per1 = std::clamp(SelYadd / 5.f, 0.f, 1.f);
							float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
							WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(128.f * per1), 0, 255));
							WindowSystem::DrawControl::Instance()->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
								this->m_SelectBackImage,
								DrawParts->GetUIY(xp1) - static_cast<int>(static_cast<float>(DrawParts->GetUIY(xsize)) * per2), DrawParts->GetUIY(yp1) - static_cast<int>(static_cast<float>(DrawParts->GetUIY(ysize)) * per2),
								DrawParts->GetUIY(xp1) + static_cast<int>(static_cast<float>(DrawParts->GetUIY(xsize)) * per2), DrawParts->GetUIY(yp1) + static_cast<int>(static_cast<float>(DrawParts->GetUIY(ysize)) * per2),
								true);
							WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
						}
						switch (this->m_ButtonStatus) {
						case ButtonStatus::None:
							WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 128, 128, 128);
							break;
						case ButtonStatus::Ready:
							WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 216, 216, 216);
							break;
						case ButtonStatus::Focus:
							WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 0, 255, 0);
							break;
						default:
							break;
						}
						WindowSystem::DrawControl::Instance()->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal,
							&this->m_Icon,
							DrawParts->GetUIY(xp1), DrawParts->GetUIY(yp1), static_cast<float>(DrawParts->GetUIY(100)) / 100.f * (1.f + SelYadd / 50.f), 0.f, true);
						WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
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
			int							select{ 0 };
			bool						m_MouseSelMode{ false };
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
			CreditControl(void) noexcept {}
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
