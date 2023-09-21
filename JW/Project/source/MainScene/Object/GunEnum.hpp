#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class GunAnimeID : int {
			Base,//0
			Shot,//1
			Cocking,//2
			ReloadStart_Empty,//3
			ReloadStart,//4
			ReloadOne,//5
			ReloadEnd,//6
			CheckStart,
			Checking,
			CheckEnd,
			Open,
			Watch,
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
			MoveAxis,
			MovePoint,
			Move,
			Lower,
			Upper,
			Barrel,
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
			"動作軸",
			"動作点",
			"移動",
			"Lower",
			"Upper",
			"Barrel",
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
			SoundEnum					m_Cock1{ SoundEnum::Shot2 };
			SoundEnum					m_Cock2{ SoundEnum::Shot2 };
			SoundEnum					m_Shot{ SoundEnum::Shot2 };
			SoundEnum					m_Unload{ SoundEnum::Unload2 };
			SoundEnum					m_Load{ SoundEnum::Load2 };
		};
		const GunSoundSet GunSoundSets[] = {
			{
				//M4
				SoundEnum::Cocking2_0,
				SoundEnum::Cocking2_1,
				SoundEnum::Shot2,
				SoundEnum::Unload2,
				SoundEnum::Load2
			},
			{
				//ハンドガン
				SoundEnum::Cocking3_0,
				SoundEnum::Cocking3_1,
				SoundEnum::Shot3,
				SoundEnum::Unload3,
				SoundEnum::Load3
			},
		};
		
		enum class GunSlot {
			Magazine,
			Upper,
			Lower,
			Barrel,
			Max,
		};
		static const char* GunSlotName[(int)GunSlot::Max] = {
			"Magazine",
			"Upper",
			"Lower",
			"Barrel",
		};
	};
};
