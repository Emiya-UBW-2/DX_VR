#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ENUM_AI_PHASE {
			Normal,
			Shot,
			Dead,
		};

		class AIControl {
		private:
			class Impl;
			Impl *m_Param;
			Impl* GetParam() noexcept { return m_Param; }
		public:
			AIControl();
			~AIControl();
		public:
			void Init(PlayerID MyCharaID) noexcept;
			void Execute(InputControl* MyInput) noexcept;
			void Draw() noexcept {}
			void Dispose() noexcept {}
		};
	};
};
