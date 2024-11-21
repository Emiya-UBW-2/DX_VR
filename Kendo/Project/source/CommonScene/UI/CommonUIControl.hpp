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
				GraphHandle*				m_SelectBackImage{ nullptr };
				int							xp1{ 0 };
				int							yp1{ 0 };
				int							xsize{ 0 };
				int							ysize{ 0 };
				FontSystem::FontXCenter		LMR{ FontSystem::FontXCenter::LEFT };
				FontSystem::FontYCenter		TMB{ FontSystem::FontYCenter::TOP };
				float						SelYadd{ 0.f };
				ButtonStatus				m_ButtonStatus{ ButtonStatus::Ready };
				ButtonMode					m_ButtonMode{ ButtonMode::String };
				bool						m_EnableSelect{ false };
				int							m_LocalizeID{0};
				GraphHandle					m_Icon;
			public:
				ButtonClass(GraphHandle* BGPath, int xp, int yp, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY, bool IsEnableSelect) noexcept {
					this->m_SelectBackImage = BGPath;
					this->xp1 = xp;
					this->yp1 = yp;
					this->LMR = FontX;
					this->TMB = FontY;
					this->SelYadd = 0.f;
					this->m_ButtonStatus = ButtonStatus::Ready;
					this->m_EnableSelect = IsEnableSelect;
				}
				ButtonClass(const ButtonClass&) = delete;
				ButtonClass(ButtonClass&& o) = delete;
				ButtonClass& operator=(const ButtonClass&) = delete;
				ButtonClass& operator=(ButtonClass&& o) = delete;
				~ButtonClass(void) noexcept {
					this->m_Icon.Dispose();
				}
			public:
				void			Load_Icon(const char* IconPath) noexcept {
					this->m_Icon.Load(IconPath);
					this->m_Icon.GetSize(&xsize, &ysize);
					this->m_ButtonMode = ButtonMode::Icon;
				}
				void			Load_String(int LocalizeID, int fontsize) noexcept {
					auto* LocalizeParts = LocalizePool::Instance();
					m_LocalizeID = LocalizeID;
					xsize = FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, fontsize, 3)->GetStringWidth(LocalizeParts->Get(m_LocalizeID));
					ysize = fontsize;
					this->m_ButtonMode = ButtonMode::String;
				}
			public:
				const auto&		IsEnableSelect(void) const noexcept { return m_EnableSelect; }
				auto			IsFocus(void) const noexcept { return this->m_ButtonStatus == ButtonStatus::Focus; }
				void			SetNone(void) noexcept { this->m_ButtonStatus = ButtonStatus::None; }
				void			SetReady(void) noexcept { this->m_ButtonStatus = ButtonStatus::Ready; }
				void			SetFocus(void) noexcept {
					SelYadd = 10.f;
					this->m_ButtonStatus = ButtonStatus::Focus;
				}
				bool			GetInto(void) const noexcept {
					int xp = xp1;
					int yp = yp1;
					switch (LMR) {
					case FontSystem::FontXCenter::LEFT:
						break;
					case FontSystem::FontXCenter::MIDDLE:
						xp = xp1 - xsize / 2;
						break;
					case FontSystem::FontXCenter::RIGHT:
						xp = xp1 - xsize;
						break;
					default:
						break;
					}
					switch (TMB) {
					case FontSystem::FontYCenter::TOP:
						break;
					case FontSystem::FontYCenter::MIDDLE:
						yp = yp1 - ysize / 2;
						break;
					case FontSystem::FontYCenter::BOTTOM:
						yp = yp1 - ysize;
						break;
					default:
						break;
					}
					return IntoMouse(xp, yp, xp + xsize, yp + ysize);
				}
			public:
				void			Update(void) noexcept {
					Easing(&SelYadd, 0.f, 0.93f, EasingType::OutExpo);
				}
				void			Draw(void) noexcept;
			};
		private:
			GraphHandle					m_SelectBackImage;
			std::vector<std::shared_ptr<ButtonClass>>	ButtonSel{};
			int							select{ 0 };
			bool						m_MouseSelMode{ false };
		public:
			const auto& IsMouseSelMode(void) const noexcept { return m_MouseSelMode; }
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
			void AddStringButton(int LocalizeID, int fontsize, bool IsEnableSelect, int xp, int yp, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY) noexcept {
				ButtonSel.emplace_back(std::make_shared<ButtonClass>(&this->m_SelectBackImage, xp, yp, FontX, FontY, IsEnableSelect));
				ButtonSel.back()->Load_String(LocalizeID, fontsize);
			}
			void AddIconButton(const char* IconPath, bool IsEnableSelect, int xp, int yp, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY) noexcept {
				ButtonSel.emplace_back(std::make_shared<ButtonClass>(&this->m_SelectBackImage, xp, yp, FontX, FontY, IsEnableSelect));
				ButtonSel.back()->Load_Icon(IconPath);
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
		class InfoControl {
			struct InfoPage {
				bool						m_IsMovie{ false };
				GraphHandle					m_Graph;
				std::vector<std::string>	m_String;

				void Dispose(void) noexcept {
					m_Graph.Dispose();
					m_String.clear();
				}
			};
		private:
			int						m_InfoNow{ 0 };
			std::vector<InfoPage>	m_InfoStr{};
		public:
			InfoControl(void) noexcept {}
			InfoControl(const InfoControl&) = delete;
			InfoControl(InfoControl&& o) = delete;
			InfoControl& operator=(const InfoControl&) = delete;
			InfoControl& operator=(InfoControl&& o) = delete;
		private:
			void DelPage() noexcept;
			void SetPage() noexcept;
		public:
			void Init(void) noexcept;
			void Start() noexcept;
			void Draw(int xmin, int ymin, int xmax, int ymax) noexcept;
			void End() noexcept;
			void Guide() noexcept;
			void Dispose(void) noexcept;
		};
		// 
		class FadeControl : public SingletonBase<FadeControl> {
		private:
			friend class SingletonBase<FadeControl>;
		private:
			bool						m_IsBlackOut{ false };// カットシーン中フラグ
			float						m_BlackOutAlpha{ 0.f };
			float						m_BlackOutPower{ 2.f };
		private:
			//コンストラクタ
			FadeControl(void) noexcept {}
			FadeControl(const FadeControl&) = delete;// コピーしてはいけないので通常のコンストラクタ以外をすべてdelete
			FadeControl(FadeControl&& o) = delete;
			FadeControl& operator=(const FadeControl&) = delete;
			FadeControl& operator=(FadeControl&& o) = delete;
			// デストラクタはシングルトンなので呼ばれません
		public:
			auto			IsFadeClear(void) const noexcept { return this->m_BlackOutAlpha == 0.f; }
			auto			IsFadeAll(void) const noexcept { return this->m_BlackOutAlpha >= 1.f; }
		public:
			// true=FadeOut
			void			SetFade(bool value) noexcept { this->m_IsBlackOut = value; }
			void			SetFadeIn(float Per) noexcept;
			void			SetFadeOut(float Per) noexcept;
		public:
			void			Update(void) noexcept;
			void			Draw(void) const noexcept;
		};
		// 
	};
};
