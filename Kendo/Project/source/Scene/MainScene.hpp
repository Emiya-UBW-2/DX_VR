#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"
#include "../MainScene/Input.hpp"

#include "../MainScene/BackGround/BackGround.hpp"


#include "../MainScene/NetWork.hpp"
#include "../MainScene/NetworkBrowser.hpp"

#include "../MainScene/Object/Character.hpp"

#include "../MainScene/UI/MainSceneUIControl.hpp"

#include "../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE {
		private:
			static const int		Chara_num = 0;
			static const int		Vehicle_num = Player_num;
			static const int		gun_num = Chara_num;
		private:
			//ëÄçÏä÷òA
			float										m_Concussion{ 0.f };
			float										m_ConcussionPer{ 0.f };
			//UIä÷òA
			UIClass										m_UIclass;
			float m_GuardStart{0.f};
			int											m_UILayer{0};

			std::unique_ptr<NetWorkBrowser>				m_NetWorkBrowser;
			//
			std::vector<DamageEvent>					m_DamageEvents;
			float										m_fov_base{ deg2rad(45.f) };

			float m_D1{38.f / 255.f}, m_D2{192.f / 255.f}, m_D3{1.f};
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return m_NetWorkBrowser->GetMyPlayerID(); }
		public:
			MAINLOOP(void) noexcept {}
			MAINLOOP(const MAINLOOP&) = delete;
			MAINLOOP(MAINLOOP&& o) = delete;
			MAINLOOP& operator=(const MAINLOOP&) = delete;
			MAINLOOP& operator=(MAINLOOP&& o) = delete;

			virtual ~MAINLOOP(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			void			Dispose_Load_Sub(void) noexcept override;
			//
			void			BG_Draw_Sub(void) noexcept override;
			void			ShadowDraw_Far_Sub(void) noexcept override;
			void			ShadowDraw_Sub(void) noexcept override;
			void			CubeMap_Sub(void) noexcept override;
			
			void			MainDraw_Sub(void) noexcept override;
			//UIï\é¶
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		};
	};
};
