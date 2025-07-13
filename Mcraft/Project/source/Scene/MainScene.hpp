#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"

#include	"../MainScene/BackGround/BackGround.hpp"
#include	"../MainScene/Object/Character.hpp"
#include	"../MainScene/Player/Player.hpp"

#include	"../MainScene/NetWork.hpp"

#include	"../CommonScene/UI/CommonUIControl.hpp"
#include	"../MainScene/MainSceneSub.hpp"

#include	"../MainScene/Object/Helicopter.hpp"

#include	"../MainScene/Object/ItemObj.hpp"
#include	"../MainScene/Object/Armer.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MainGameScene : public TEMPSCENE {
			PauseMenuControl							m_PauseMenuControl;
			MainSceneUI									m_UIclass;			//UI関連
			std::unique_ptr<NetWork::NetWorkController>	m_NetWorkController{ nullptr };			//NetWork
			NetWork::PlayerSendData						m_LocalSend;
			bool										m_IsEnd{ false };//シーン全体の終了フラグ
			float										m_StartTimer{ 0.0f };
			float										m_Timer{ 0.0f };
			Vector3DX									m_EffectPos;
			InputControl								MyInput;
			MV1											m_MainRagDoll;
			MV1											m_RagDoll;
			float										AberrationPower{ 1.f };
		private:
			auto		GetViewPlayerID(void) const noexcept {
				if (this->m_NetWorkController) {
					return this->m_NetWorkController->GetMyLocalPlayerID();
				}
				return (PlayerID)0;
			}

			bool		IsStartedBattle(void) const noexcept { return this->m_StartTimer <= 0.0f; }
		public:
			MainGameScene(void) noexcept {}
			MainGameScene(const MainGameScene&) = delete;
			MainGameScene(MainGameScene&&) = delete;
			MainGameScene& operator=(const MainGameScene&) = delete;
			MainGameScene& operator=(MainGameScene&&) = delete;

			virtual ~MainGameScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			LoadEnd_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			void			Dispose_Load_Sub(void) noexcept override;
			//
			void			BG_Draw_Sub(void) const noexcept override {
				BackGround::BackGroundControl::Instance()->BG_Draw();
			}
			void			ShadowDraw_Far_Sub(void) const noexcept override {
				BackGround::BackGroundControl::Instance()->Shadow_Draw();
			}
			void			ShadowDraw_Sub(void) const noexcept override {
				ObjectManager::Instance()->Draw_Shadow();
			}
			void			CubeMap_Sub(void) const noexcept override {
				BackGround::BackGroundControl::Instance()->Draw();
			}

			void			SetShadowDraw_Rigid_Sub(void) const noexcept override {
				BackGround::BackGroundControl::Instance()->SetShadow_Draw_Rigid();
			}
			void			SetShadowDraw_Sub(void) const noexcept override {
				ObjectManager::Instance()->Draw(false, InvalidID);
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
