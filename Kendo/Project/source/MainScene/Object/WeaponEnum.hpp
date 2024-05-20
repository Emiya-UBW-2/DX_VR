#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class WeaponFrame {
			Center,
			LeftHandPos,
			LeftHandYvec,
			LeftHandZvec,
			RightHandPos,
			RightHandYvec,
			RightHandZvec,
			Move,
			Start,
			End,
			Max,
		};
		static const char* WeaponFrameName[(int)WeaponFrame::Max] = {
			"ÉZÉìÉ^Å[",
			"lefthand",
			"lefthand_yvec",
			"lefthand_zvec",
			"righthand",
			"righthand_yvec",
			"righthand_zvec",
			"à⁄ìÆ",
			"Start",
			"End",
		};
	};
};
