#pragma once

#include	"../sub.hpp"

#include	"../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PauseMenuControl {
			bool			m_IsRetire{ false };
		protected:
			const auto&		IsRetire(void) const noexcept { return this->m_IsRetire; }
		protected:
			void			LoadPause(void) noexcept;
			void			SetPause(void) noexcept;
			void			UpdatePause(void) noexcept;
			void			DrawPause(void) noexcept;
			void			DisposePause(void) noexcept;
		};

		class InGameUIControl {
			GraphHandle					m_ViewHandle;
			GraphHandle					m_Watch;
			GraphHandle					m_Caution;
			GraphHandle					m_Alert;
			GraphHandle					m_Goal;

			float						m_MapDrawTime{ 0.f };
			float						m_MapDrawPer{ 0.f };
			int							m_MapTextID{ 0 };

			Vector3DX					m_GoalPos;
			float						m_StartTime{0.f};
		private:
			void			DrawCharaUI_Back(PlayerID value) noexcept;
			void			DrawCharaUI_Front(PlayerID value) noexcept;
		protected:
			void			SetMap(int value, const Vector3DX& GoalPos) noexcept {
				this->m_MapTextID = value;
				this->m_GoalPos = GoalPos;
			}
			void			SetStartTime(float value) noexcept { this->m_StartTime = value; }
		public:
			InGameUIControl(void) {}
			InGameUIControl(const InGameUIControl&) = delete;
			InGameUIControl(InGameUIControl&& o) = delete;
			InGameUIControl& operator=(const InGameUIControl&) = delete;
			InGameUIControl& operator=(InGameUIControl&& o) = delete;
		protected:
			void			LoadUI(void) noexcept;
			void			SetUI(void) noexcept;
			void			UpdateUI(void) noexcept;
			void			Dispose_LoadUI(void) noexcept;
			void			SetupWatchScreen(void) noexcept;
			void			DrawUI_Back(void) noexcept;
			void			DrawUI_Front(void) noexcept;
			void			DrawUI_MapName(void) noexcept;
		};

		enum class CutSceneType {
			MsgBox,
			ViewPoint,
			WaitMilSec,
			CG,
			End,
			Max,
		};
		static const char* g_CutSceneStr[static_cast<int>(CutSceneType::Max)] = {
			"MsgBox",
			"ViewPoint",
			"WaitMilSec",
			"CG",
			"End",
		};

		class CutSceneControl {
			struct CutSceneData {
				CutSceneType m_Type{};
				int m_NameID{};
				int m_MsgID{};
				int m_PointX{};
				int m_PointY{};
				int m_WatiMS{};
				int m_CGSel{};
			};
		private:
			bool						m_IsCutScene{ false };//カットシーン中フラグ
			float						m_CutSceneAlpha{ 0.f };

			std::vector<CutSceneData>	m_CutSceneData{};
			int							m_CutSceneSeek{ 0 };

			std::array<std::string,10>	m_MsgString{};
			bool						m_IsMsgBox{ false };//カットシーン中フラグ
			float						m_MsgBoxAlpha{ 0.f };
			float						m_MsgBoxSeek{ 0.f };
			int							m_PrevMsgBoxSeek{};
			int							m_PointX{};
			int							m_PointY{};
			float						m_WaitMS{ 0.f };
			int							m_CGSel{INVALID_ID};
			float						m_CGFade{0.f};
			GraphHandle					m_CGGraph;
		protected:
			const auto& IsCutScene(void) const noexcept { return this->m_IsCutScene; }
			const auto& GetAddViewPointX(void) const noexcept { return this->m_PointX; }
			const auto& GetAddViewPointY(void) const noexcept { return this->m_PointY; }
		protected:
			void			StartCutScene(int ID) noexcept;
		protected:
			void			SetCut(void) noexcept;
			void			UpdateCut(void) noexcept;
			void			DrawCut(void) noexcept;
		};

		class FadeControl {
			bool						m_IsBlackOut{ false };//カットシーン中フラグ
			float						m_BlackOutAlpha{ 0.f };
		protected:
			auto		IsFadeClear(void) const noexcept { return this->m_BlackOutAlpha == 0.f; }
			auto		IsFadeAll(void) const noexcept { return this ->m_BlackOutAlpha >= 1.f; }
		protected:
			void			SetBlackOut(bool value) noexcept { this->m_IsBlackOut = value; }
		protected:
			void			SetFade(void) noexcept;
			void			UpdateFade(void) noexcept;
			void			DrawFade(void) noexcept;
		};
	};
};
