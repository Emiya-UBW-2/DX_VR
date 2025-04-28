#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"
#include	"../sub.hpp"
#include	"../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TitleScene : public TEMPSCENE {
			bool								m_IsEnd{ false };//シーン全体の終了フラグ
			GraphHandle							m_TitleImage;
			std::unique_ptr<UIs::CreditControl>		m_CreditControl;
		public:
			TitleScene(void) noexcept {}
			TitleScene(const TitleScene&) = delete;
			TitleScene(TitleScene&&) = delete;
			TitleScene& operator=(const TitleScene&) = delete;
			TitleScene& operator=(TitleScene&&) = delete;

			virtual ~TitleScene(void) noexcept {}
		public:
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			void			MainDraw_Sub(int) const noexcept override {}
			void			DrawUI_Base_Sub(void) const noexcept override;
		};
	}
}
