#pragma once
#include	"../Header.hpp"
#include	"../CommonScene/UI/CommonUIControl.hpp"
#include	"../MainScene/MainSceneSub.hpp"
#include	"../MainScene/Player/CPU.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MainGameScene : public TEMPSCENE {
			const PlayerID				m_MyPlayerID{0};
		private:
			bool						m_IsEnd{false};//�V�[���S�̂̏I���t���O
			float						m_Alpha{0.f};
			// 
			Vector3DX					m_CamAddPos;
			// 
			GraphHandle					m_ViewHandle;
			GraphHandle					m_Watch;
			GraphHandle					m_Caution;
			GraphHandle					m_Alert;
		private:
			void			LoadPause(void) noexcept;
			void			UpdatePause(void) noexcept;
			void			DrawPause(void) noexcept;
			void			DisposePause(void) noexcept;
		private:
			void			LoadUI(void) noexcept;
			void			DrawCharaUI_Back(PlayerID value) noexcept;
			void			DrawCharaUI_Front(PlayerID value) noexcept;
			void			Dispose_LoadUI(void) noexcept;
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
			// UI�\��
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
			// 
			void			Dispose_Load_Sub(void) noexcept override;
		};
	};
};
