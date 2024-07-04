#pragma once
#include	"../Header.hpp"

#include "../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TitleScene : public TEMPSCENE {
		private:
			float						GameFadeIn{ 0.f };
			float						GameStart{ 0.f };
			GraphHandle					m_TitleImage;
			//
			CreditControl				m_CreditControl;
		public:
			TitleScene(void) noexcept { Set3DActive(false); }
			~TitleScene(void) noexcept {}
		public:
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			MainDraw_Sub(void) noexcept override;
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override {}
		};
	};
};
