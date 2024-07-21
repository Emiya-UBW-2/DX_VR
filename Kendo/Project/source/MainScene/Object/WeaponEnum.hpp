#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class WeaponFrame : uint8_t {
			Center,
			LeftHandPos,
			LeftHandYvec,
			LeftHandZvec,
			RightHandPos,
			RightHandYvec,
			RightHandZvec,
			Move,
			End,
			Max,
		};
		static const char* WeaponFrameName[static_cast<int>(WeaponFrame::Max)] = {
			"ÉZÉìÉ^Å[",
			"lefthand",
			"lefthand_yvec",
			"lefthand_zvec",
			"righthand",
			"righthand_yvec",
			"righthand_zvec",
			"à⁄ìÆ",
			"End",
		};
	};
};
