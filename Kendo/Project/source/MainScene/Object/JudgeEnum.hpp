#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class JudgeAnimeID : int {
			Bottom_Stand,
			Bottom_Stand_Walk,
			Bottom_Stand_WalkBack,
			Bottom_Stand_LeftStep,
			Bottom_Stand_RightStep,
			Bottom_Stand_Turn,
			//
			Upper_Ready,
			//
			AnimeIDMax,
		};
		enum class JudgeFrame {
			Upper,
			Upper2,
			Head,
			Max,
		};
		static const char* JudgeFrameName[(int)JudgeFrame::Max] = {
			"è„îºêg",
			"è„îºêg2",
			"ì™",
		};
	};
};
