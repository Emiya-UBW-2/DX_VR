#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"

#include "../MainScene/BackGround/BackGround.hpp"
#include "../MainScene/Object/Character.hpp"
#include "../MainScene/Player/Player.hpp"

#include "../MainScene/NetWork.hpp"

#include "../MainScene/UI/MainSceneUIControl.hpp"
#include	"../CommonScene/UI/CommonUIControl.hpp"
#include	"../MainScene/MainSceneSub.hpp"

#include "../MainScene/EventScene/EventScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MainGameScene : public TEMPSCENE ,
			public FadeControl
		{
		private:
			//UI関連
			float										m_Concussion{ 0.f };
			float										m_ConcussionPer{ 0.f };
			UIClass										m_UIclass;
			PauseMenuControl							m_PauseMenuControl;
			//NetWork
			std::unique_ptr<NetWork::NetWorkController>	m_NetWorkController{ nullptr };
			std::vector<DamageEvent>					m_DamageEvents;
			//
			bool										m_IsEnd{ false };//シーン全体の終了フラグ
			bool										m_isTraining{ false };
#ifdef DEBUG
			float m_D1{ 38.f / 255.f }, m_D2{ 192.f / 255.f }, m_D3{ 1.f };
#endif
			EventScene		m_EventScene;
			bool			m_IsEventSceneActive{ false };
			bool			m_IsEventSceneFlag{ false };
			std::string		m_EventSelect;

			GraphHandle		m_GameStart;
			GraphHandle		m_GameRestart;
			GraphHandle*	m_pStart{ nullptr };
			GraphHandle		m_Once;
			GraphHandle		m_GameEnd;
			GraphHandle		m_Result;

			std::vector<GraphHandle>		m_Teacher;

			float m_GameStartTimer{ 2.f };
			float m_GameStartAlpha{ 0.f };
			float m_GameStartScale{ 0.f };

			bool m_IsGameStart{ false };
			bool m_IsGameEnd{ false };
			float m_Timer{ 0.f };
			bool m_IsPlayable{ false };

			float m_WinOnceTimer{ 2.f };
			float m_WinOnceAlpha{ 0.f };
			float m_WinOnceScale{ 0.f };

			float m_GameEndTimer{ 2.f };
			float m_GameEndAlpha{ 0.f };
			float m_GameEndScale{ 0.f };

			bool m_IsDrawOneMinute{ false };
			bool m_IsTimeUp{ false };
			bool m_IsWinSound{ false };

			bool m_IsResult{ false };
			int m_TimerSE{ 0 };

			float m_DivideTimer{ 5.f };

			float m_TutorialTimer{ 0.f };

			float							m_ScoreUp0{ 0.f };
			float							m_ScoreUp1{ 0.f };
		private:
			auto		GetMyPlayerID(void) const noexcept {
				if (m_NetWorkController) {
					return m_NetWorkController->GetMyPlayerID();
				}
				return (PlayerID)0;
			}
		public:
			void		SetIsTraining(bool value)noexcept { m_isTraining = value; }
		public:
			MainGameScene(void) noexcept {}
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
			void			BG_Draw_Sub(void) const noexcept override;
			void			ShadowDraw_Far_Sub(void) const noexcept override;
			void			ShadowDraw_Sub(void) const noexcept override;
			void			CubeMap_Sub(void) const noexcept override;

			void			SetShadowDraw_Sub(void) const noexcept override;

			void			MainDraw_Sub(void) const noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) const noexcept override;
			void			DrawUI_In_Sub(void) const noexcept override;
		};
	}
}
