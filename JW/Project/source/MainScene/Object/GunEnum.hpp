#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class GunAnimeID : int {
			Base,//0
			Shot,//1
			Cocking,//2
			ReloadStart_Empty,//3
			ReloadStart,//3
			ReloadOne,//4
			ReloadEnd,//5
			CheckStart,
			Checking,
			CheckEnd,
			Open,
			AnimeIDMax,
		};
		enum class GunFrame {
			Center,
			Cart,
			CartVec,
			Muzzle,
			Eyepos,
			Lens,
			LensSize,
			Magpos,
			LeftHandPos,
			LeftHandYvec,
			LeftHandZvec,
			RightHandPos,
			RightHandYvec,
			RightHandZvec,
			LeftHandRPos,
			LeftHandRYvec,
			LeftHandRZvec,
			MoveAxis,
			MovePoint,
			Move,
			Max,
		};
		static const char* GunFrameName[(int)GunFrame::Max] = {
			"センター",
			"cart",
			"cartvec",
			"muzzle",
			"EyePos",
			"lens",
			"lenssize",
			"magpos",
			"lefthand",
			"lefthand_yvec",
			"lefthand_zvec",
			"righthand",
			"righthand_yvec",
			"righthand_zvec",
			"lefthandR",
			"lefthandR_yvec",
			"lefthandR_zvec",
			"動作軸",
			"動作点",
			"移動",
		};
		enum class SHOTTYPE {
			BOLT,
			SEMI,
			FULL,
		};
		enum class RELOADTYPE {
			AMMO,
			MAG,
		};

		struct GunSoundSet {
			std::array<SoundEnum, 4>	m_Cock{ SoundEnum::Shot2 ,SoundEnum::Shot2 ,SoundEnum::Shot2 ,SoundEnum::Shot2 };
			SoundEnum					m_Shot{ SoundEnum::Shot2 };
			SoundEnum					m_Unload{ SoundEnum::Unload2 };
			SoundEnum					m_Load{ SoundEnum::Load2 };
		};
	};
};
