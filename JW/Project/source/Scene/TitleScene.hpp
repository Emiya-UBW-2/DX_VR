#pragma once
#include	"../Header.hpp"

#include "../CommonScene/Object/GunsModify.hpp"
#include "../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		static const char* SelectName[6] = {
			"Ready",
			"Customize",
			"Shooting Range",
			"Fire Party",
			"",
			"",
		};

		class TitleScene : public TEMPSCENE, public GunsModify {
		private:
			GraphHandle					m_TitleImage;
			GraphHandle					m_SelectBackImage;
			std::array<ButtonClass, 6>	ButtonSel{};
			int							select{0};

			float						GameFadeIn{0.f};
			float						GameStart{0.f};

			bool						m_MouseSelMode{false};

			static const int			CharMax = 256;
			int							m_CreditCoulm{0};
			std::array<std::pair<char[CharMax], char[CharMax]>, 64> m_CreditStr{};
			bool						m_HardModeActive{false};

			float						m_Yrad{0.f};
			float						m_Xrad{0.f};
			float						m_Yrad_R{0.f};
			float						m_Xrad_R{0.f};
		public:
			TitleScene(void) noexcept {}
			~TitleScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			BG_Draw_Sub(void) noexcept override;
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override;
			void			SetShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override {}
		public:
			void			Dispose_Load_Sub(void) noexcept override;
		};
	};
};
