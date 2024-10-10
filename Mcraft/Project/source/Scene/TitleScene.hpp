#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"

#include	"../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TitleScene : public TEMPSCENE {
		private:
			float						m_GameFadeIn{0.f};
			float						m_GameStart{0.f};
			GraphHandle					m_TitleImage;
			bool						m_CloseResetSave{false};
			// 
			std::unique_ptr<CreditControl>	m_CreditControl;
		public:
			TitleScene(void) noexcept {}
			TitleScene(const TitleScene&) = delete;
			TitleScene(TitleScene&& o) = delete;
			TitleScene& operator=(const TitleScene&) = delete;
			TitleScene& operator=(TitleScene&& o) = delete;

			virtual ~TitleScene(void) noexcept {}
		public:
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			// 
			void			MainDraw_Sub(void) noexcept override;
			// UI•\Ž¦
			void			DrawUI_Base_Sub(void) noexcept override;
		};
	};
};
