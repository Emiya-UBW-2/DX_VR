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

#include "../CommonScene/Object/GunsModify.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
#ifdef DEBUG
		struct LineDebug {
			Vector3DX PosA;
			Vector3DX PosB;
			float Time = 0;
		};
#endif

		class MainGameScene : public TEMPSCENE ,
			public PauseMenuControl,
			public FadeControl
			, public GunsModify
			, public EffectControl
		{
		private:
			//UI関連
			float										m_Concussion{ 0.f };
			float										m_ConcussionPer{ 0.f };
			UIClass										m_UIclass;
			//NetWork
			std::unique_ptr<NetWork::NetWorkController>	m_NetWorkController{ nullptr };
			std::vector<DamageEvent>					m_DamageEvents;
			//
			bool										m_IsEnd{ false };//シーン全体の終了フラグ

			float										m_StartTimer{ 0.f };


			int tgtSel = 0;
			float tgtTimer = 0.f;
			GraphHandle ScoreBoard2;
			GraphHandle										hit_Graph;
			GraphHandle										guard_Graph;

#ifdef DEBUG
			float m_D1{ 38.f / 255.f }, m_D2{ 192.f / 255.f }, m_D3{ 1.f };
			std::vector<LineDebug> m_LineDebug;
#endif
		private:
			auto		GetMyPlayerID(void) const noexcept {
				if (m_NetWorkController) {
					return m_NetWorkController->GetMyPlayerID();
				}
				return (PlayerID)0;
			}
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

			void			SetShadowDraw_Rigid_Sub(void) const noexcept override;
			void			SetShadowDraw_Sub(void) const noexcept override;

			void			MainDraw_Sub(void) const noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) const noexcept override;
			void			DrawUI_In_Sub(void) const noexcept override;
		private:
			void			LoadGun(const std::string& FolderName, PlayerID ID, bool IsPreset, int Sel) noexcept;
			void			UpdateBullet(void) noexcept;

			void			DrawHitGraph(void) const noexcept;
		};
	}
}
