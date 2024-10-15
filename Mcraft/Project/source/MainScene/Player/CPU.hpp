#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ENUM_AI_PHASE {
			Normal,
			Shot,
			Dead,
		};

		class AIControl {
		public:
			AIControl();
			~AIControl();
		public:
			void Init(PlayerID) noexcept;
			void Execute(InputControl*) noexcept;
			void Draw() noexcept {}
			void Dispose() noexcept {}
		};
	};
};
