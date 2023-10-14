#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	namespace Sceneclass {
		enum class ObjType {
			Human,
			Ammo,
			FallObj,
			Gun,
			Magazine,
			Lower,
			Upper,
			Barrel,
			UnderRail,
			Sight,
			MuzzleAdapter,
			MovieObj,
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
			Cocking1_0,
			Cocking1_1,
			Shot1,
			Unload1,
			Load1,
			Shot1_Suppressor,
			//
			Cocking2_0,
			Cocking2_1,
			Shot2,
			Unload2,
			Load2,
			Shot2_Suppressor,
			//
			Cocking3_0,
			Cocking3_1,
			Shot3,
			Unload3,
			Load3,
			Shot3_Suppressor,
			//
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
