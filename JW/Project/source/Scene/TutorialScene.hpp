#pragma once
#include	"../Header.hpp"

#include "../MainScene/MainSceneSub.hpp"
#include "../MainScene/Object/Character.hpp"
#include "../MainScene/UI/MainSceneUIControl.hpp"

#include "../MainScene/Object/GunsModify.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TutorialScene : public TEMPSCENE, public EffectControl, public GunsModify {
		private:
			bool											m_PrevSSAO{ true };
			bool											m_IsFirstLoad{ true };			//共通リソースをロードしたか
			std::shared_ptr<BackGroundClassTutorial>		m_BackGround;					//BG
			MyPlayerReticleControl							m_MyPlayerReticleControl;		//銃関連
			MainLoopPauseControl							m_MainLoopPauseControl;			//ポーズメニュー
			UIClass											m_UIclass;						//UI関連
			GraphHandle										hit_Graph;
			GraphHandle										guard_Graph;

			std::vector<DamageEvent>						m_DamageEvents;				//ダメージ

			std::shared_ptr<GunClass>		m_GunPtr;				//ポインター別持ち
			std::shared_ptr<CharacterClass>	m_CharacterPtr;			//ポインター別持ち
		public:
			TutorialScene(void) noexcept { }
			~TutorialScene(void) noexcept { }
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
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		public:
			void			Dispose_Load(void) noexcept;
		private:
			void			LoadSE(void) noexcept;
			void			SetSE(void) noexcept;
			void			DisposeSE(void) noexcept;
		private:
			void			LoadChara(const std::string&FolderName) noexcept;
			void			LoadGun(const std::string&FolderName, bool IsPreset, int Sel) noexcept;
		};
	};
};
