#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//定数
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	enum class ObjType : uint8_t {
		Human,
		Weapon,
	};
	enum class SoundEnum : uint8_t {
		EV1_1 = static_cast<uint8_t>(SoundEnumCommon::Num),
		Taiko1,
		EV2_1,
		EV2_2,
		//
		RunFoot,
		StandupFoot,
		Heart,
		//
		Kendo_Swing,
		Kendo_Hit,
		Kendo_GetHit,
		Kendo_Foot,
		Kendo_Tsuba,
		//
		Voice_Ya,
		Voice_Men,
		Voice_Kote,
		Voice_Dou,
		Voice_Tsuki,
		//
		JudgeVoice_Men,
		JudgeVoice_Kote,
		JudgeVoice_Dou,
		JudgeVoice_Start,
		JudgeVoice_Stop,
		JudgeVoice_Divide,
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
		"EV1_1",
		"Taiko1",
		"EV2_1",
		"EV2_2",
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


	enum class WazaType {
		Men,		//メイン
		SuriageMen,	//ガード後にメイン
		Hikimen,	//引きながらメイン

		Kote,		//サブ
		Hikigote,	//引きながらサブ

		Dou,		//ガード後にサブ
	};
};
