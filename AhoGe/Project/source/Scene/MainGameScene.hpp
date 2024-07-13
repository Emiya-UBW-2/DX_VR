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
			bool						m_IsPrologue{false};//�V�[���S�̂̏I���t���O
			bool						m_IsGoodEnd{false};//�V�[���S�̂̏I���t���O

			bool						m_IsBadEnd{false};//�V�[���S�̂̏I���t���O

			bool						m_IsGoNext{ false };//���V�[���ɍs���t���O
			bool						m_IsEnd{false};//�V�[���S�̂̏I���t���O
			bool						m_IsPlayable{ false };//����\
			// 
			Vector3DX					m_CamAddPos;
			Vector3DX					m_CamAddPosR;
			//
			std::string					m_MapName;
			int							m_EntryID{};
			int							m_CutSceneID{};//�J�b�g�V�[��ID

			int							m_BossID{};
			int							m_WinCutSceneID{};//�J�b�g�V�[��ID
			//
			std::pair<int, int>			m_PrevXY{};
			//
			bool						m_IsNormalBGM{false};
			float						m_NormalBGM{0.f};
			bool						m_IsCautionBGM{false};
			float						m_CautionBGM{0.f};
			bool						m_IsAlertBGM{false};
			float						m_AlertBGM{0.f};

			float						m_StartTime{0.f};
		public:
			MainGameScene(void) noexcept { Set3DActive(false); }
			MainGameScene(const MainGameScene&) = delete;
			MainGameScene(MainGameScene&& o) = delete;
			MainGameScene& operator=(const MainGameScene&) = delete;
			MainGameScene& operator=(MainGameScene&& o) = delete;

			virtual ~MainGameScene(void) noexcept {}
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
