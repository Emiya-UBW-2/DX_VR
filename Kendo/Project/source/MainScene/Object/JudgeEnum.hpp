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

			Upper_RedWin,
			Upper_WhiteWin,
			//
			AnimeIDMax,
		};
		enum class JudgeFrame {
			Upper,
			Max,
		};
		static const char* JudgeFrameName[(int)JudgeFrame::Max] = {
			"è„îºêg",
		};
	};
};
