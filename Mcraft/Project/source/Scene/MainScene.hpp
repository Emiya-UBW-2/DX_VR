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

		class MainGameScene : public TEMPSCENE {
		private:
			PauseMenuControl							m_PauseMenuControl;
			FadeControl									m_FadeControl;
			EffectControl								m_EffectControl;
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
			GraphHandle									hit_Graph;
			GraphHandle									guard_Graph;
			Vector3DX									m_EffectPos;
			InputControl								MyInput;
#ifdef DEBUG
			std::vector<LineDebug>						m_LineDebug;
#endif
		private:
			auto		GetViewPlayerID(void) const noexcept {
				if (m_NetWorkController) {
					return m_NetWorkController->GetMyLocalPlayerID();
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
			void			BG_Draw_Sub(void) const noexcept override {
				BackGround::BackGroundClass::Instance()->BG_Draw();
			}
			void			ShadowDraw_Far_Sub(void) const noexcept override {}
			void			ShadowDraw_Sub(void) const noexcept override {
				BackGround::BackGroundClass::Instance()->Shadow_Draw();
				ObjectManager::Instance()->Draw_Shadow();
			}
			void			CubeMap_Sub(void) const noexcept override {
				BackGround::BackGroundClass::Instance()->Draw();
			}

			void			SetShadowDraw_Rigid_Sub(void) const noexcept override {
				BackGround::BackGroundClass::Instance()->SetShadow_Draw_Rigid();
			}
			void			SetShadowDraw_Sub(void) const noexcept override {
				ObjectManager::Instance()->Draw(false);
			}
			void			MainDraw_Sub(void) const noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) const noexcept override;
			void			DrawUI_In_Sub(void) const noexcept override {
				auto* SceneParts = SceneControl::Instance();
				if (SceneParts->IsPause()) {
					m_PauseMenuControl.Draw();
				}
			}
		private:
			void			UpdateBullet(void) noexcept;
			void			DrawHitGraph(void) const noexcept;
		};
	}
}
