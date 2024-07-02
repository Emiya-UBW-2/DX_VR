#pragma once
#include	"../Header.hpp"

#include "../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		class TitleScene : public TEMPSCENE {
		private:
			GraphHandle					m_TitleImage;
			GraphHandle					m_SelectBackImage;
			std::array<ButtonClass, 4>	ButtonSel{};
			int							select{0};

			float						GameFadeIn{0.f};
			float						GameStart{0.f};

			bool						m_MouseSelMode{false};
			//
			static const int			CharMax = 256;
			int							m_CreditCoulm{0};
			std::array<std::pair<char[CharMax], char[CharMax]>, 64> m_CreditStr{};
		public:
			TitleScene(void) noexcept { Set3DActive(false); }
			~TitleScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			MainDraw_Sub(void) noexcept override;
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override {}
		public:
			void			Dispose_Load_Sub(void) noexcept override;
		};
	};
};
