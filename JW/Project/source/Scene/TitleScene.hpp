#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TitleScene : public TEMPSCENE {
		private:
			int select{0};

			std::array<float, 5> GameStart{};
			std::array<float, 5> SelYadd{};

			float GameFadeIn{0.f};
			GraphHandle	m_SettingImage;
			GraphHandle	m_CreditImage;
			bool m_MouseSelMode{false};
		public:
			const auto SelMode() const noexcept { return select; }
		public:
			TitleScene(void) noexcept {}
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			void			BG_Draw_Sub(void) noexcept override {}
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_NearFar_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override {}
			void			MainDraw_Sub(void) noexcept override {}
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override {}
		private:
			const auto GetGameStartMax() const noexcept { return std::max(std::max(GameStart[0], GameStart[1]), GameStart[2]); }
		};
	};
};
