#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class WeaponAnimeID : int {
			Cocking,
			Shot,
			Base,
			AnimeIDMax,
		};
		enum class WeaponFrame {
			Center,
			LeftHandPos,
			LeftHandYvec,
			LeftHandZvec,
			RightHandPos,
			RightHandYvec,
			RightHandZvec,
			Move,
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
		};
		struct WeaponSoundSet {
			std::array<SoundEnum, 4>	m_Cock{ SoundEnum::Shot2 ,SoundEnum::Shot2 ,SoundEnum::Shot2 ,SoundEnum::Shot2 };
			SoundEnum					m_Shot{ SoundEnum::Shot2 };
			SoundEnum					m_Unload{ SoundEnum::Unload2 };
			SoundEnum					m_Load{ SoundEnum::Load2 };
		};
	};
};
