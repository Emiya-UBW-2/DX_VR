#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//定数
	enum class ObjType : uint8_t {
		Human,
		Weapon,
		Judge,
	};
	enum class SoundEnum : uint8_t {
		EV1_1 = static_cast<uint8_t>(SoundSelectCommon::Num),
		Taiko1,
		EV2_1,
		EV2_2,
		ThankYou,
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
		JudgeVoice_Win,
		JudgeVoice_Draw,
		//
		CountDown,
		TimeUp,

		Audience_Base,
		Audience_Near,
		Audience_Good,

		Breath,

		Envi,

		Max,
	};
	static const char* SoundID[static_cast<int>(SoundEnum::Max)] = {
		"",
		"",
		"",
		"",
		"EV1_1",
		"Taiko1",
		"EV2_1",
		"EV2_2",
		"ThankYou",
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
		"",
		"",
		"",
	};

	typedef int8_t	PlayerID;
	typedef short	HitPoint;

	enum class HitType {
		Head,
		Body,
		Arm,
		Leg,
		None,
	};


	enum class WazaType {
		Men,		//メイン
		SuriageMen,	//ガード後にメイン
		Hikimen,	//引きながらメイン

		Kote,		//サブ
		Hikigote,	//引きながらサブ

		Dou,		//ガード後にサブ
	};

	enum class GameMode {
		Main,
		Training,
		Replay,
	};

	enum class GameControlType {
		InGame,
		Network,
		Replay,
		Result,
	};
}
