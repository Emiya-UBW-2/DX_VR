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
		class MainGameScene : public TEMPSCENE {
		private:
			PauseMenuControl							m_PauseMenuControl;
			FadeControl									m_FadeControl;
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
			Vector3DX									m_EffectPos;
			InputControl								MyInput;
		private:
			auto		GetViewPlayerID(void) const noexcept {
				if (this->m_NetWorkController) {
					return this->m_NetWorkController->GetMyLocalPlayerID();
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
				ObjectManager::Instance()->Draw(false, -1);
			}
			void			MainDraw_Sub(int Range) const noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) const noexcept override;
			void			DrawUI_In_Sub(void) const noexcept override {
				auto* SceneParts = SceneControl::Instance();
				if (SceneParts->IsPause()) {
					this->m_PauseMenuControl.Draw();
				}
			}
		};
	}
}
