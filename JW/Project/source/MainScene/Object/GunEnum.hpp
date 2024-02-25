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
			Melee,
			AmmoLoadStart,
			AmmoLoading,
			AmmoLoadEnd,
			OffsetAnim,
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
			UnderRail,
			Sight,
			MuzzleAdapter,
			LaserSight,
			EyeOffsetPos,
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
			"UnderRail",
			"Sight",
			"MuzzleAdapter",
			"LaserSight",
			"EyeOffsetPos",
		};
		enum class SHOTTYPE {
			BOLT,
			SEMI,
			FULL,
			Max,
		};
		static const char* SHOTTYPEName[(int)SHOTTYPE::Max] = {
			"BOLT",
			"SEMI",
			"FULL",
		};
		enum class RELOADTYPE {
			AMMO,
			MAG,
		};

		struct GunSoundSet {
			SoundEnum					m_Cock1{SoundEnum::Shot2};
			SoundEnum					m_Cock2{SoundEnum::Shot2};
			SoundEnum					m_ShotNormal{SoundEnum::Shot2};
			SoundEnum					m_ShotSuppressor{SoundEnum::Shot2};
			SoundEnum					m_Unload{SoundEnum::Unload2};
			SoundEnum					m_Load{SoundEnum::Load2};
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
				//M4
				SoundEnum::Cocking1_0,
				SoundEnum::Cocking1_1,
				SoundEnum::Shot1,
				SoundEnum::Shot1_Suppressor,
				SoundEnum::Unload1,
				SoundEnum::Load1
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
		static const char* GunSlotName[(int)GunSlot::Max] = {
			"Gun",
			"Magazine",
			"Upper",
			"Lower",
			"Barrel",
			"UnderRail",
			"Sight",
			"MuzzleAdapter",
		};

		enum class GunShootSound {
			Normal,
			Suppressor,
			Max,
		};
		static const char* GunShootSoundName[(int)GunShootSound::Max] = {
			"Normal",
			"Suppressor",
		};
	};
};
