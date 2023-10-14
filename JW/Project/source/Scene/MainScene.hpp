#pragma once
#include	"../Header.hpp"
#include "../NetWork.hpp"
#include "../MainScene/UI/MainSceneUIControl.hpp"
#include "../MainScene/BackGround/BackGround.hpp"
#include "../MainScene/NetworkBrowser.hpp"
#include "../MainScene/Object/Character.hpp"
#include "../MainScene/Player/CPU.hpp"
#include "CustomScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public EffectControl, public GunsModify {
		private:
			static const int		Chara_num = Player_num;
			static const int		gun_num = Chara_num;
		private:
			bool											m_IsFirstLoad{ true };
		private:
			//リソース関連
			std::shared_ptr<BackGroundClassMain>			m_BackGround;				//BG
			GraphHandle										m_MiniMapScreen;
			GraphHandle										Gauge_Graph;
			GraphHandle										hit_Graph;
			//人
			std::vector<std::shared_ptr<AIControl>>			m_AICtrl;					//AI
			//UI関連
			UIClass											m_UIclass;
			int												m_HPBuf{ 0 };
			int												m_ScoreBuf{ 0 };
			//
			float											m_CamShake{ 0.f };
			VECTOR_ref										m_CamShake1;
			VECTOR_ref										m_CamShake2;
			//銃関連
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;
			std::vector<DamageEvent>						m_DamageEvents;
			//ネットワーク
			NetWorkBrowser									m_NetWorkBrowser;
			double											m_ClientFrame{ 0.0 };
			//共通
			float											Timer{ 5.f };

			float	Min = 0.f;
			float	Gamma = 1.f;

			bool	m_Retire{ false };
		private:
			int select{ 0 };
			std::array<float, 3> SelYadd{};
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			MAINLOOP(void) noexcept { }
			~MAINLOOP(void) noexcept { }
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			void			BG_Draw_Sub(void) noexcept override;
			void			ShadowDraw_Far_Sub(void) noexcept override;
			void			ShadowDraw_NearFar_Sub(void) noexcept override;
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			void			MainDrawbyDepth_Sub(void) noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		public:
			void			Dispose_Load(void) noexcept;
		};
	};
};
