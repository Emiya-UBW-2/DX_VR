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

#include	"../MainScene/Object/MovieCharacter.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		enum class TaskType {
			Obtain,
			KillEnemy,
		};

		class TaskInfo {
		public:
			TaskType	m_TaskType{};
			int		m_ItemID{ InvalidID };
			int		m_Count{ 1 };
		};

		class MainGameScene : public TEMPSCENE {
			PauseMenuControl							m_PauseMenuControl;
			MainSceneUI									m_UIclass;			//UI関連
			MainSceneResultUI							m_UIresult;
			std::unique_ptr<NetWork::NetWorkController>	m_NetWorkController{ nullptr };			//NetWork
			NetWork::PlayerSendData						m_LocalSend;
			bool										m_IsEnd{ false };//シーン全体の終了フラグ
			float										m_StartTimer{ 0.0f };
			float										m_BattleTimer{ 0.0f };
			float										m_ReturnPer{ 0.0f };
			bool										m_IsFindContainer{ false };
			float										m_FindContainerTimer{ 0.0f };
			float										m_AnnounceTimer{ 0.0f };

			bool										m_IsGameOver = false;
			bool										m_IsGameClear = false;

			float										m_GameClearCount2 = 0.f;
			float										m_FadeoutEndTimer = 0.f;
			GraphHandle									m_KillGraph;

			Vector3DX									m_EffectPos;
			InputControl								MyInput;
			MV1											m_MainRagDoll;
			MV1											m_RagDoll;
			float										AberrationPower{ 1.f };
			bool										m_IsAddScoreArea = false;

			int											m_LimitAlarmCount{};
			float										m_LimitAlarmTimer{ 0.0f };

			bool										m_IsGameReady = false;
			float										m_StartAnimTimer{ 0.0f };

			std::shared_ptr<Charas::MovieObject>		m_MovieHeli;


			std::vector<std::pair<TaskInfo, int>>		m_TaskInfoList;
			bool										m_TaskClearOnce{ false };

			bool										m_IsSkipMovie{ false };
			float										m_MovieEndTimer{ 0.0f };

			bool										m_IsTutorial{ false };
			int m_TextID = InvalidID;

			int m_PrevTextSoundID = InvalidID;
			float m_TextTimer{ 0.f };
		private:
			auto		GetViewPlayerID(void) const noexcept {
				if (this->m_NetWorkController) {
					return this->m_NetWorkController->GetMyLocalPlayerID();
				}
				return (PlayerID)0;
			}

			bool		IsStartedBattle(void) const noexcept { return this->m_StartTimer <= 0.0f; }
		public:
			void		SetTutorial(bool value) noexcept { this->m_IsTutorial = value; }
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
				if (!this->m_IsGameReady) {
					FillGraph(GetDrawScreen(), 0, 0, 0);
				}
				BackGround::BackGroundControl::Instance()->BG_Draw();
			}
			void			ShadowDraw_Far_Sub(void) const noexcept override {
				BackGround::BackGroundControl::Instance()->Shadow_Draw();
			}
			void			ShadowDraw_Sub(void) const noexcept override {
				ObjectManager::Instance()->Draw_Shadow();
			}
			void			DepthDraw_Sub(int layer) const noexcept override {
				ObjectManager::Instance()->Draw_Depth(layer);
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
