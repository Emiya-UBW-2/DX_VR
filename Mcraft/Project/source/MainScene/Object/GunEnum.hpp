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
			std::array<Sceneclass::SoundEnum, static_cast<int>(EnumGunSound::Max)>	m_Sound{ };
		};
		const GunSoundSet GunSoundSets[] = {
			{
				//M4
				Sceneclass::SoundEnum::Cocking2_0,
				Sceneclass::SoundEnum::Cocking2_1,
				Sceneclass::SoundEnum::Shot2,
				Sceneclass::SoundEnum::Shot2_Suppressor,
				Sceneclass::SoundEnum::Unload2,
				Sceneclass::SoundEnum::Load2
			},
			{
				//ハンドガン
				Sceneclass::SoundEnum::Cocking3_0,
				Sceneclass::SoundEnum::Cocking3_1,
				Sceneclass::SoundEnum::Shot3,
				Sceneclass::SoundEnum::Shot3_Suppressor,
				Sceneclass::SoundEnum::Unload3,
				Sceneclass::SoundEnum::Load3
			},
			{
				//M870
				Sceneclass::SoundEnum::Cocking1_0,
				Sceneclass::SoundEnum::Cocking1_1,
				Sceneclass::SoundEnum::Shot1,
				Sceneclass::SoundEnum::Shot1_Suppressor,
				Sceneclass::SoundEnum::Unload1,
				Sceneclass::SoundEnum::Load1
			},
			{
				//AK
				Sceneclass::SoundEnum::Cocking4_0,
				Sceneclass::SoundEnum::Cocking4_1,
				Sceneclass::SoundEnum::Shot4,
				Sceneclass::SoundEnum::Shot4_Suppressor,
				Sceneclass::SoundEnum::Unload4,
				Sceneclass::SoundEnum::Load4
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
	};
};
