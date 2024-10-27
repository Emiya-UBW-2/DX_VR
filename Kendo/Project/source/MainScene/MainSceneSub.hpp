#pragma once
#pragma warning(disable:4464)

#include	"../sub.hpp"

#include	"../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PauseMenuControl {
			bool			m_IsRetire{ false };
		protected:
			const auto&		IsRetire(void) const noexcept { return this->m_IsRetire; }
		protected:
			void			LoadPause(void) noexcept;
			void			SetPause(void) noexcept;
			void			UpdatePause(void) noexcept;
			void			DrawPause(void) const noexcept;
			void			DisposePause(void) noexcept;
		};

		class FadeControl {
			bool						m_IsBlackOut{ false };//カットシーン中フラグ
			float						m_BlackOutAlpha{ 0.f };
		protected:
			auto		IsFadeClear(void) const noexcept { return this->m_BlackOutAlpha == 0.f; }
			auto		IsFadeAll(void) const noexcept { return this ->m_BlackOutAlpha >= 1.f; }
		protected:
			void			SetFadeIn(void) noexcept;
			void			SetFadeOut(void) noexcept;
			void			UpdateFade(void) noexcept;
			void			DrawFade(void) const noexcept;
		};
	};
};
