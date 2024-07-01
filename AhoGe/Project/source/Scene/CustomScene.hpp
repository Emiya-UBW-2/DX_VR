#pragma once
#include	"../Header.hpp"
#include "../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class LookSelect {
			ModSet,
			ULTSet,
			FreeLook,
			Max,
		};

		class CustomScene : public TEMPSCENE {
			class SlotMove {
			public:
				float				Xadd{0.f};
				float				Yadd{0.f};
				int					index{0};
			};
		private:
			int							select{0};
			float						m_SelAlpha{0.f};
			float						m_Yrad{0.f};
			float						m_Xrad{0.f};
			float						m_Yrad_R{0.f};
			float						m_Xrad_R{0.f};
			float						m_Range{1.f};
			std::vector<SlotMove>		SelMoveClass;
			bool						m_IsEnd{false};
			float						m_Alpha{0.f};
			LookSelect					m_LookSel{LookSelect::ModSet};
			GraphHandle					m_SelectBackImage;
			std::array<float, 3>		ButtonAlpha{};
			std::array<ButtonClass, 8>	ButtonSel{};
			int							bselect{0};
			bool						m_MouseSelMode{false};
		private:
		public:
			CustomScene(void) noexcept {}
			~CustomScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			//void			BG_Draw_Sub(void) noexcept override {}
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override {}
		public:
			void			Dispose_Load_Sub(void) noexcept override;
		};
	};
};
