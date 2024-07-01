#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	class BuildControl {
	private:
	public:
		void			Load(void) noexcept;
		void			Init() noexcept;
		void			Execute(void) noexcept;
		void			ShadowDraw() noexcept;
		void			Draw() noexcept;
		void			DrawFront() noexcept;
		void			Dispose(void) noexcept;
	};
};
