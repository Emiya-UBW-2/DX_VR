#pragma once
#include	"../Header.hpp"
#include "../CommonScene/UI/CommonUIControl.hpp"
#include "../MainScene/MainSceneSub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TestGameScene : public TEMPSCENE {
		private:
			MainLoopPauseControl							m_MainLoopPauseControl;			//ポーズメニュー

			float						m_SelAlpha{0.f};
			float						m_Range{1.f};
			bool						m_IsEnd{false};
			float						m_Alpha{0.f};
		private:
		public:
			TestGameScene(void) noexcept { Set3DActive(false); }
			~TestGameScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			MainDraw_Sub(void) noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		public:
			void			Dispose_Load_Sub(void) noexcept override;
		};
	};
};
