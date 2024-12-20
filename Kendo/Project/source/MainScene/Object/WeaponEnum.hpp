#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace WeaponObject {
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
			"センター",
			"lefthand",
			"lefthand_yvec",
			"lefthand_zvec",
			"righthand",
			"righthand_yvec",
			"righthand_zvec",
			"移動",
			"End",
		};
	}
}
