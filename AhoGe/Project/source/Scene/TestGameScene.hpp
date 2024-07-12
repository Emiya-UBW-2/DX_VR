#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TestGameScene : public TEMPSCENE {
		public:
			TestGameScene(void) noexcept {}
			virtual ~TestGameScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			// 
			void			MainDraw_Sub(void) noexcept override;
			// UI•\Ž¦
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
			// 
			void			Dispose_Load_Sub(void) noexcept override;
		};
	};
};
