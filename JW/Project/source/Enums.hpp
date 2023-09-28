#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	namespace Sceneclass {
		enum class scenes
		{
			NONE_SCENE,
			LOAD_SCENE,
			MAIN_LOOP
		};
		enum class ObjType {
			Human,
			Ammo,
			FallObj,
			Gun,
			Parts_Start,
			Magazine = Parts_Start,
			Lower,
			Upper,
			Barrel,
			UnderRail,
			Sight,
			Parts_End,
			Max,
		};
		enum class SoundEnum {
			RunFoot = (int)SoundEnumCommon::Num,
			StandUp,
			SlideFoot,
			StandupFoot,
			Heart,
			//
			Env,
			Env2,
			//
			Trigger,
			//
			Cocking2_0,
			Cocking2_1,
			Shot2,
			Unload2,
			Load2,
			//
			Cocking3_0,
			Cocking3_1,
			Shot3,
			Unload3,
			Load3,
			//
			LaserSwitch,
			CartFall,
			MagFall,
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
