#pragma once
#pragma warning(disable:4464)
#include	"../Header.hpp"

#include	"../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TitleScene : public TEMPSCENE {
		private:
			FadeControl							m_FadeControl;
			bool								m_IsEnd{ false };//シーン全体の終了フラグ
			GraphHandle							m_TitleImage;
			std::unique_ptr<CreditControl>		m_CreditControl;
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
			void			MainDraw_Sub(int) const noexcept override {}
			void			DrawUI_Base_Sub(void) const noexcept override;
		};
	};
};
