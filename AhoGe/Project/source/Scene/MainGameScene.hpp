#pragma once
#include	"../Header.hpp"
#include	"../CommonScene/UI/CommonUIControl.hpp"
#include	"../MainScene/MainSceneSub.hpp"
#include	"../MainScene/Player/CPU.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MainGameScene : public TEMPSCENE,
			public PauseMenuControl,
			public InGameUIControl,
			public CutSceneControl,
			public FadeControl
		{
			const PlayerID				m_MyPlayerID{0};
		private:
			bool						m_IsGoNext{ false };//次シーンに行くフラグ
			bool						m_IsEnd{false};//シーン全体の終了フラグ
			bool						m_IsPlayable{ false };//操作可能
			// 
			Vector3DX					m_CamAddPos;
			//
			std::string					m_MapName;
			int							m_EntryID{};
			//
			std::pair<int, int>			m_PrevXY{};
			//
			bool						m_IsCautionBGM{false};
			float						m_CautionBGM{0.f};
			bool						m_IsAlertBGM{false};
			float						m_AlertBGM{0.f};
		public:
			MainGameScene(void) noexcept { Set3DActive(false); }
			~MainGameScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			void			Dispose_Load_Sub(void) noexcept override;
			// 
			void			MainDraw_Sub(void) noexcept override;
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		};
	};
};
