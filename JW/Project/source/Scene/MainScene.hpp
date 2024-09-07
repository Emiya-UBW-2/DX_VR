#pragma once
#include	"../Header.hpp"
#include "../MainScene/MainSceneSub.hpp"
#include "../MainScene/UI/MainSceneUIControl.hpp"
#include "../MainScene/BackGround/BackGround.hpp"
#include "../MainScene/Object/Character.hpp"
#include "../MainScene/Player/CPU.hpp"

#include "../CommonScene/NetWork.hpp"
#include "../CommonScene/NetworkBrowser.hpp"

#include "../CommonScene/Object/GunsModify.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public EffectControl, public GunsModify {
		private:
			static const int		Chara_num = Player_num;
			static const int		gun_num = Chara_num;
		private:
			bool											m_IsHardMode{false};

			std::shared_ptr<BackGroundClassMain>			m_BackGround;					//BG

			UIClass											m_UIclass;						//UI関連
			int												prevScore{0};
			int												prevLastMan{0};

			NetWorkBrowser									m_NetWorkBrowser;				//ネットワーク
			ConcussionControl								m_ConcussionControl;			//コンカッション
			MainLoopPauseControl							m_MainLoopPauseControl;			//ポーズメニュー
			//共通
			GraphHandle										m_MiniMapScreen;
			GraphHandle										hit_Graph;
			GraphHandle										guard_Graph;
			std::vector<std::shared_ptr<AIControl>>			m_AICtrl;					//AI
			std::vector<DamageEvent>						m_DamageEvents;				//ダメージ
			float											m_ReadyTimer{0.f};
			int												m_LastMan{0};
			float											m_Timer{0.f};
			bool											m_RashStartLog{false};
			bool											m_IsEnd{false};
			float											m_PreEndTimer{0.f};
			float											m_EndTimer{0.f};
			int												m_ResultPhase{0};

			struct ResultFlips {
				float m_Flip{0.f};
				float m_Point{0.f};
				float m_Timer{0.f};
				float m_Up{0.f};
			public:
				void Init() {
					m_Flip = 0.f;
					m_Point = 0.f;
					m_Timer = 0.f;
					m_Up = 0.f;
				}

				bool Update(int Aim, bool skip) {
					auto* DrawParts = DXDraw::Instance();
					float diff = ((float)Aim - m_Point);
					if (skip) { diff = 0.f; }
					if (diff <= 0.f) {
						m_Point = (float)Aim;
					}
					else {
						m_Point += 1.f / DrawParts->GetFps() * ((float)Aim / 4.f);
					}

					Easing(&m_Up, 0.f, 0.9f, EasingType::OutExpo);
					if (abs((float)Aim - m_Point) > 1.f) {
						m_Timer += 1.f / DrawParts->GetFps();
						if (m_Timer > 0.4f) {
							m_Timer -= 0.4f;
							m_Up = 1.f;
						}
					}
					else {
						return true;
					}
					return false;
				}
			};
			std::array<ResultFlips, 3>						m_ResultFlip{};
			float											m_ResultRankingPer{0.f};
			float											m_ResultRankDrawTime{0.f};
			int												m_ResultRank{0};
			std::vector<std::pair<int, int64_t>>			m_Ranking{};
			int64_t											m_StartTime{0};

			GraphHandle										m_SelectBackImage;
			ButtonClass										ButtonSel{};

			float											Min = 0.f;
			float											Gamma = 1.f;
			float											AberrationPower{1.f};
			float											m_DeathCamYAdd{0.f};
			float											m_DeathPer{0.f};
			float											m_ULTCounter{0.f};
			int												m_CountDownBGMTimer{0};
			bool											m_CountDownBGM1Flag{true};
			bool											m_CountDownBGM2Flag{true};
			bool											m_CountDownBGM3Flag{true};
			//GraphHandle movie;										//
			//int m_movieTotalFrame{0};
		public:
			MAINLOOP(void) noexcept {}
			~MAINLOOP(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			BG_Draw_Sub(void) noexcept override;
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			void			MainDrawFront_Sub(void) noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		public:
			void			Dispose_Load_Sub(void) noexcept override;
		public:
			void			SetPlayMode(bool value) noexcept { m_IsHardMode = value; }
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_NetWorkBrowser.GetMyPlayerID(); }
		private:
			void			LoadGun(const std::string&FolderName, PlayerID ID, bool IsPreset, int Sel) noexcept;
		private:
			void			StartUIParam(void) noexcept;
			void			StartResult(void) noexcept;
		private:
			bool			UpdateResult(void) noexcept;
			void			UpdateInput(void) noexcept;
			void			UpdateBullet(void) noexcept;
			void			UpdateItem(void) const noexcept;
			void			UpdateMelee(void) noexcept;
			void			UpdateView(void) noexcept;
			void			UpdateLight(void) noexcept;
			void			UpdateLaser(void) noexcept;
			void			UpdateUIParam(void) noexcept;
			void			UpdateMiniMap(void) noexcept;
		private:
			void			DrawSoundGraph(void) noexcept;
			void			DrawHitGraph(void) const noexcept;

			void			DrawResult(void) noexcept;
			void			DrawBlackOut(float per) noexcept;
		};
	};
};
