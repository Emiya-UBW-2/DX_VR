#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//�萔
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
			Max,
		};
		static const char* ObjTypeName[(int)ObjType::Max] = {
			"Human",
			"Ammo",
			"FallObj",
			"Gun",
			"Magazine",
			"Lower",
			"Upper",
			"Barrel",
			"UnderRail",
			"Sight",
			"MuzzleAdapter",
		};
		enum class SoundEnum {
			RunFoot = (int)SoundSelectCommon::Num,
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
			Cocking4_0,
			Cocking4_1,
			Shot4,
			Unload4,
			Load4,
			Shot4_Suppressor,
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
			Man_breathing,
			Man_breathend,
			//
			Hit,
			HitMe,
			HitGuard,
			Near,
			//
			HitGround0,
			HitGround1,
			HitGround2,
			HitGround3,
			HitGround4,
			//
			CountDown,
			Second,
			OneMunute,
			TimeUp,

		};
		enum class CharaTypeID {
			Team,
			Enemy,
		};

		enum class Effect {
			ef_gndsmoke,		// �n�ʃq�b�g
			ef_fire2,			// ���C��
			ef_hitblood,		// �����Ԃ�
			ef_greexp,			// �O������
			ef_dust,			// �܂��̚�
		};
	};

	typedef char PlayerID;
	typedef short HitPoint;
	typedef short ArmerPoint;
};
