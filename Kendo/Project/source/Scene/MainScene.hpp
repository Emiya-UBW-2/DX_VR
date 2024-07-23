#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"

#include "../MainScene/BackGround/BackGround.hpp"
#include "../MainScene/Object/Character.hpp"
#include "../MainScene/Player/Player.hpp"

#include "../MainScene/NetWork.hpp"

#include "../MainScene/UI/MainSceneUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE {
		private:
			//UIŠÖ˜A
			float										m_GuardStart{ 0.f };
			float										m_Concussion{ 0.f };
			float										m_ConcussionPer{ 0.f };
			UIClass										m_UIclass;
			int											m_UILayer{ 0 };
			//NetWork
			std::unique_ptr<NetWorkController>			m_NetWorkController{nullptr};
			std::vector<DamageEvent>					m_DamageEvents;
			//
#ifdef DEBUG
			float m_D1{ 38.f / 255.f }, m_D2{ 192.f / 255.f }, m_D3{ 1.f };
#endif
		private:
			auto		GetMyPlayerID(void) const noexcept {
				if (m_NetWorkController) {
					return m_NetWorkController->GetMyPlayerID();
				}
				return (PlayerID)0;
			}
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
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		};
	};
};
