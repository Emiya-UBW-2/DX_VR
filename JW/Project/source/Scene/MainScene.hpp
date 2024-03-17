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
			bool											m_IsFirstLoad{true};			//共通リソースをロードしたか
			std::shared_ptr<BackGroundClassMain>			m_BackGround;					//BG

			UIClass											m_UIclass;						//UI関連
			GetItemLog										m_GetItemLog;
			int												prevScore{0};
			int												prevLastMan{0};

			MyPlayerReticleControl							m_MyPlayerReticleControl;		//銃関連
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
			float											Min = 0.f;
			float											Gamma = 1.f;
			float											AberrationPower{1.f};

			float											m_DeathCamYAdd{0.f};
			float											m_DeathPer{0.f};

			GraphHandle movie;										//
			int m_movieTotalFrame{0};
		public:
			MAINLOOP(void) noexcept {}
			~MAINLOOP(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			void			BG_Draw_Sub(void) noexcept override;
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_NearFar_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			void			MainDrawFront_Sub(void) noexcept override;
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		public:
			void			Dispose_Load(void) noexcept;
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_NetWorkBrowser.GetMyPlayerID(); }
		private:
			void			SetDrawMiniMap(void) noexcept;
			void			DrawSoundGraph(void) noexcept;
			void			DrawHitGraph(void) noexcept;
		private:
			void			LoadSE(void) noexcept;
			void			SetSE(void) noexcept;
			void			DisposeSE(void) noexcept;
		private:
			void			LoadChara(const std::string&FolderName, PlayerID ID, bool IsRagDoll, bool IsRagDollBaseObj = false) noexcept;
			void			LoadGun(const std::string&FolderName, PlayerID ID, bool IsPreset, int Sel) noexcept;
		};
	};
};
