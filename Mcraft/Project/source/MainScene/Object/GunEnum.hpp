#pragma once
#include	"../../Header.hpp"
#include	"CharaAnimData.hpp"

namespace FPS_n2 {
	namespace Guns {
		enum class GunFrame {
			Center,
			Cart,
			CartVec,
			Muzzle,
			Eyepos,
			Lens,
			LensSize,
			Magpos,
			Mag2,
			LeftHandPos,
			LeftHandYvec,
			LeftHandZvec,
			RightHandPos,
			RightHandYvec,
			RightHandZvec,
			LeftHandPosCock,
			LeftHandYvecCock,
			LeftHandZvecCock,
			RightHandPosCock,
			RightHandYvecCock,
			RightHandZvecCock,
			MoveAxis,
			MovePoint,
			Move,
			Lower,
			Upper,
			Barrel,
			UnderRail,
			Sight,
			MuzzleAdapter,
			LaserSight,
			Light,
			smoke1,
			smoke2,
			smoke3,
			smoke4,
			smoke5,
			Max,
		};
		static const char* GunFrameName[static_cast<int>(GunFrame::Max)] = {
			"センター",
			"cart",
			"cartvec",
			"muzzle",
			"EyePos",
			"lens",
			"lenssize",
			"magpos",
			"mag2",
			"lefthand",
			"lefthand_yvec",
			"lefthand_zvec",
			"righthand",
			"righthand_yvec",
			"righthand_zvec",
			"lefthandcock",
			"lefthandcock_yvec",
			"lefthandcock_zvec",
			"righthandcock",
			"righthandcock_yvec",
			"righthandcock_zvec",
			"動作軸",
			"動作点",
			"移動",
			"Lower",
			"Upper",
			"Barrel",
			"UnderRail",
			"Sight",
			"MuzzleAdapter",
			"LaserSight",
			"Light",
			"smoke1",
			"smoke2",
			"smoke3",
			"smoke4",
			"smoke5",
		};

		enum class SHOTTYPE {
			PUMP,
			BOLT,
			SEMI,
			FULL,
			Max,
		};
		static const char* SHOTTYPEName[static_cast<int>(SHOTTYPE::Max)] = {
			"PUMP",
			"BOLT",
			"SEMI",
			"FULL",
		};
		enum class RELOADTYPE {
			AMMO,
			MAG,
		};

		enum class STOCKTYPE {
			none,
			curve,
			straight,
			Max,
		};
		static const char* STOCKTYPEName[static_cast<int>(STOCKTYPE::Max)] = {
			"none",
			"curve",
			"straight",
		};

		enum class EnumGunSound {
			CockingPull,
			CockingRelease,
			ShotNormal,
			ShotSuppressor,
			UnloadMag,
			LoadMag,
			Max,
		};
		static const char* EnumGunSoundName[static_cast<int>(EnumGunSound::Max)] = {
			"CockingPull",
			"CockingRelease",
			"Normal",
			"Suppressor",
			"UnloadMag",
			"LoadMag",
		};

		struct GunSoundSet {
			std::array<SoundEnum, static_cast<int>(EnumGunSound::Max)>	Sound{ };
		};
		const GunSoundSet GunSoundSets[] = {
			{
				//M4
				SoundEnum::Cocking2_0,
				SoundEnum::Cocking2_1,
				SoundEnum::Shot2,
				SoundEnum::Shot2_Suppressor,
				SoundEnum::Unload2,
				SoundEnum::Load2
			},
			{
				//ハンドガン
				SoundEnum::Cocking3_0,
				SoundEnum::Cocking3_1,
				SoundEnum::Shot3,
				SoundEnum::Shot3_Suppressor,
				SoundEnum::Unload3,
				SoundEnum::Load3
			},
			{
				//M870
				SoundEnum::Cocking1_0,
				SoundEnum::Cocking1_1,
				SoundEnum::Shot1,
				SoundEnum::Shot1_Suppressor,
				SoundEnum::Unload1,
				SoundEnum::Load1
			},
			{
				//AK
				SoundEnum::Cocking4_0,
				SoundEnum::Cocking4_1,
				SoundEnum::Shot4,
				SoundEnum::Shot4_Suppressor,
				SoundEnum::Unload4,
				SoundEnum::Load4
			},
		};
		enum class GunSlot {
			Gun,
			Magazine,
			Upper,
			Lower,
			Barrel,
			UnderRail,
			Sight,
			MuzzleAdapter,
			Max,
		};
		static const char* GunSlotName[static_cast<int>(GunSlot::Max)] = {
			"Gun",
			"Magazine",
			"Upper",
			"Lower",
			"Barrel",
			"UnderRail",
			"Sight",
			"MuzzleAdapter",
		};
	}
}
