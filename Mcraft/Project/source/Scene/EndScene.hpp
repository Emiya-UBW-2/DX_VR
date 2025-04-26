#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"

#include	"../sub.hpp"

#include	"../MainScene/BackGround/BackGround.hpp"
#include	"../MainScene/Object/Character.hpp"
#include	"../MainScene/Player/Player.hpp"

#include	"../MainScene/NetWork.hpp"

#include	"../MainScene/UI/MainSceneUIControl.hpp"
#include	"../CommonScene/UI/CommonUIControl.hpp"
#include	"../MainScene/MainSceneSub.hpp"

#include	"../MainScene/EventScene/EventScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class EndScene : public TEMPSCENE {
		private:
			//イベント
			EventScene									m_EventScene;
			bool										m_IsEventSceneActive{ false };
			bool										m_IsEventSceneFlag{ false };
			std::string									m_EventSelect;
			//
			bool										m_IsEnd{ false };//シーン全体の終了フラグ
		private:
			void			SetupEvent(const char* Path) noexcept;
			void			UpdateEvent(void) noexcept;
		public:
			EndScene(void) noexcept {}
			EndScene(const EndScene&) = delete;
			EndScene(EndScene&& o) = delete;
			EndScene& operator=(const EndScene&) = delete;
			EndScene& operator=(EndScene&& o) = delete;

			virtual ~EndScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			void			Dispose_Load_Sub(void) noexcept override;
			//
			void			BG_Draw_Sub(void) const noexcept override;
			void			ShadowDraw_Sub(void) const noexcept override;
			void			SetShadowDraw_Sub(void) const noexcept override;
			void			MainDraw_Sub(void) const noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) const noexcept override;
			void			DrawUI_In_Sub(void) const noexcept override;
		};
	}
}
