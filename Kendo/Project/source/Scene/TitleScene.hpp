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
			std::unique_ptr<InfoControl>	m_InfoControl;
			//
			MV1							m_TitleBGSky;
			MV1							m_TitleBGMap;
			MV1							m_TitleChara;
			MV1							m_TitleBamboo;

			int							m_Phase{ 0 };
			float						m_PhaseTime{ 0.f };
		public:
			TitleScene(void) noexcept {}
			TitleScene(const TitleScene&) = delete;
			TitleScene(TitleScene&& o) = delete;
			TitleScene& operator=(const TitleScene&) = delete;
			TitleScene& operator=(TitleScene&& o) = delete;

			virtual ~TitleScene(void) noexcept {}
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
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) const noexcept override;
		};
	};
};
