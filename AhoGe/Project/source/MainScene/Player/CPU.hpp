#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class AIControl {
		private:
			class Impl;
			Impl* m_Param;
			const Impl* GetParam() const noexcept { return m_Param; }
			Impl* GetParam() noexcept { return m_Param; }
		public:
			AIControl();
			~AIControl();
		public:
			const float& GetViewRad() const noexcept;
			const float& GetGraphTimer() const noexcept;

			const bool IsCaution() const noexcept;
			const bool IsAlert() const noexcept;
		public:
			void Init(PlayerID MyCharaID) noexcept;
			void Execute(InputControl* MyInput) noexcept;
			void Draw() noexcept {}
			void Dispose() noexcept {}
		};
	};
};
