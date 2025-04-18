#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//�萔
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	namespace Sceneclass {
		enum class ObjType {
			Vehicle,
			Ammo,
			Gun,
		};
		enum class SoundEnum {
			Environment = (int)SoundEnumCommon::Num,

			Tank_Shot,
			Tank_Ricochet,
			Tank_Damage,
			Tank_engine,
			Tank_Eject,
			Tank_Reload,
			Tank_Start,
			Tank_move,
			Tank_near,
			
			Shot_Gun,
			Trigger,
			Cocking0,
			Cocking1,
			Cocking2,
			Cocking3,
			RunFoot,
			SlideFoot,
			StandupFoot,
			Heart,
			GateOpen,
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
