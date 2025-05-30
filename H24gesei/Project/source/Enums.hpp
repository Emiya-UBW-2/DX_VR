#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//�萔
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	enum class ObjType : uint8_t {
		Human,
		Ammo,
	};
	enum class SoundEnum : uint8_t {
		Shot = static_cast<uint8_t>(SoundEnumCommon::Num),
		ShotBig,
		load,
		//
		Hit,
		Near,
		Damage,
		//
		Gear,
		Rotate,
		Engine,
		//
		Env,
		//
		CountDown,
		Second,
		OneMunute,
		TimeUp,

		Num,
	};
	enum class CharaTypeID : uint8_t {
		Mine,
		Team,
		Enemy,
	};

	typedef int8_t	PlayerID;
	typedef short	HitPoint;

	enum class HitType {
		Head,
		Body,
		Arm,
		Leg,
	};
};
