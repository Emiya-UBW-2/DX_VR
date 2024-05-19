#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	namespace Sceneclass {
		enum class ObjType {
			Human,
			Weapon,
		};
		enum class SoundEnum {
			RunFoot = (int)SoundEnumCommon::Num,
			Sliding,
			StandupFoot,
			Heart,
			//
			Kendo_Swing,
			Kendo_Hit,
			Kendo_Foot,
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

			Num,
		};
		enum class CharaTypeID {
			Mine,
			Team,
			Enemy,
		};
	};

	typedef char PlayerID;
	typedef short HitPoint;
};
