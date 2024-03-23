#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{60.f};

	static const float Scale_Rate{12.5f};

	namespace Sceneclass {
		enum class ObjType {
			Human,
			Ammo,
			FallObj,
			Armer,
			Morphine,
			Gun,
			Magazine,
			Lower,
			Upper,
			Barrel,
			UnderRail,
			Sight,
			MuzzleAdapter,
			MovieObj,
			ItemObj,
			Max,
		};
		static const char* ObjTypeName[(int)ObjType::Max] = {
			"Human",
			"Ammo",
			"FallObj",
			"Armer",
			"Morphine",
			"Gun",
			"Magazine",
			"Lower",
			"Upper",
			"Barrel",
			"UnderRail",
			"Sight",
			"MuzzleAdapter",
			"MovieObj",
			"ItemObj",
		};
		enum class SoundEnum {
			RunFoot = (int)SoundEnumCommon::Num,
			StandUp,
			SlideFoot,
			StandupFoot,
			GetAmmo,
			Heart,
			//
			Env,
			Env2,
			//
			Trigger,
			AmmoLoad,
			Tinnitus,
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
			Stim,
			//
			Man_Hurt1,
			Man_Hurt2,
			Man_Hurt3,
			Man_Hurt4,
			Man_Hurt5,
			Man_Hurt6,
			//
			Man_Death1,
			Man_Death2,
			Man_Death3,
			Man_Death4,
			Man_Death5,
			Man_Death6,
			Man_Death7,
			Man_Death8,
			//
			Man_contact,
			Man_openfire,
			Man_reload,
			Man_takecover,
			Man_teamdown,
			Man_breathing,
			Man_breathend,
			//
			Hit,
			HitMe,
			HitGuard,

			Tank_near,
			HitGround0,
			HitGround1,
			HitGround2,
			HitGround3,
			HitGround4,

			CountDown,
		};
		enum class CharaTypeID {
			Mine,
			Team,
			Enemy,
		};

		enum class ULT_GUN {
			PCC_4,
			AKS_74,
			Mod870,
			M16_4,
			Max,
		};
		static const char* ULT_GUNName[(int)ULT_GUN::Max] = {
			"PCC_4",
			"AKS-74",
			"Mod870",
			"M16-4",
		};
	};

	typedef char PlayerID;
	typedef short HitPoint;
	typedef short ArmerPoint;
};
