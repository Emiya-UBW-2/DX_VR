#pragma once
#pragma warning(disable:4464)

#include	"../sub.hpp"

#include	"../CommonScene/UI/CommonUIControl.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PauseMenuControl {
			bool			m_IsRetire{ false };
		public:
			const auto&		IsRetire(void) const noexcept { return this->m_IsRetire; }
		public:
			void			Load(void) noexcept;
			void			Init(void) noexcept;
			void			Update(void) noexcept;
			void			Draw(void) const noexcept;
			void			Dispose(void) noexcept;
		};

		class FadeControl {
			bool						m_IsBlackOut{ false };//カットシーン中フラグ
			float						m_BlackOutAlpha{ 0.f };
		public:
			auto		IsClear(void) const noexcept { return this->m_BlackOutAlpha == 0.f; }
			auto		IsAll(void) const noexcept { return this ->m_BlackOutAlpha >= 1.f; }
		public:
			void			SetBlackOut(bool value) noexcept { this->m_IsBlackOut = value; }
		public:
			void			Init(void) noexcept;
			void			Update(void) noexcept;
			void			Draw(void) const noexcept;
		};
	};
};
