#pragma once
#include	"../Header.hpp"
#include "../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		static const char* SelectName[5] = {
			"Ready",
			"Customize Handgun",
			"Shooting Range",
			"",
			"",
		};
		static const char* SelectInfoName[5] = {
			"戦闘を行います",
			"使用する拳銃のカスタマイズをすることができます",
			"射撃場で装備を試します",
			"オプションを開きます",
			"クレジットを開きます",
		};

		class TitleScene : public TEMPSCENE {
		private:
			GraphHandle					m_SelectBackImage;
			std::array<ButtonClass, 5>	ButtonSel{};
			int							select{0};

			float						GameFadeIn{0.f};
			float						GameStart{0.f};

			bool						m_MouseSelMode{false};

			static const int			CharMax = 256;
			int							m_CreditCoulm{0};
			std::array<char[CharMax], 64> m_CreditStr{};
			bool						m_CreditActive{false};
			PopUpDrawClass				m_PopUpDrawClass;
			SoundHandle										m_BGM;
		public:
			const auto SelMode() const noexcept { return select; }
		public:
			TitleScene(void) noexcept {}
			~TitleScene(void) noexcept {}
		public:
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			void			BG_Draw_Sub(void) noexcept override {}
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_NearFar_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override {}
			void			MainDraw_Sub(void) noexcept override {}
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override {}
		};
	};
};
