#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	enum class ObjType : uint8_t {
		Human,
		Weapon,
	};
	enum class SoundEnum : uint8_t {
		RunFoot = static_cast<uint8_t>(SoundEnumCommon::Num),
		KendoKun,
		Taiko1,
		StandupFoot,
		Heart,
		//
		Kendo_Swing,
		Kendo_Hit,
		Kendo_Foot,
		Kendo_Tsuba,
		//
		Voice_Ya,
		Voice_Men,
		Voice_Kote,
		Voice_Dou,
		Voice_Tsuki,
		//
		CountDown,
		Second,
		OneMunute,
		TimeUp,

		Max,
	};
	static const char* SoundID[(int)SoundEnum::Max] = {
		"",
		"",
		"",
		"",
		"RunFoot",
		"KendoKun",
		"Taiko1",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
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
