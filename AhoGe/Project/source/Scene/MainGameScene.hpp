#pragma once
#include	"../Header.hpp"
#include "../CommonScene/UI/CommonUIControl.hpp"
#include "../MainScene/MainSceneSub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MainGameScene : public TEMPSCENE {
			const PlayerID				m_MyPlayerID{ 0 };
		private:
			bool						m_IsEnd{ false };
			float						m_Alpha{ 0.f };
			bool						m_IsRetire{ false };
		private:
		public:
			MainGameScene(void) noexcept { Set3DActive(false); }
			~MainGameScene(void) noexcept {}
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
			void			DrawUI_In_Sub(void) noexcept override;
		public:
			void			Dispose_Load_Sub(void) noexcept override;
		};
	};
};
