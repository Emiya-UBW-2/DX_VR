#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//定数
	enum class ObjType {
		Human,
		Ammo,
		FallObj,
		AmmoInChamber,
		Helicopter,
		Vehicle,
		Gun,
		Magazine,
		Lower,
		Upper,
		Barrel,
		UnderRail,
		Sight,
		MuzzleAdapter,
		ItemObj,
		Armor,
		ItemContainerObj,
		CircleEffect,
		MovieObj,
		Max,
	};
	static const char* ObjTypeName[static_cast<int>(ObjType::Max)] = {
		"Human",
		"Ammo",
		"FallObj",
		"AmmoInChamber",
		"Helicopter",
		"Vehicle",
		"Gun",
		"Magazine",
		"Lower",
		"Upper",
		"Barrel",
		"UnderRail",
		"Sight",
		"MuzzleAdapter",
		"ItemObj",
		"Armor",
		"ItemContainerObj",
		"CircleEffect",
		"MovieObj",
	};
	enum class SoundEnum {
		RunFoot = static_cast<int>(SoundSelectCommon::Num),
		StandupFoot,
		Heli,
		Heli2,
		Envi,
		//
		Pin,
		Throw,
		FallGrenade,
		Explosion,
		GetItem,
		PutItem,
		Delivery,
		announce,
		//
		Trigger,
		rolling_rocket,
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
		//
		HitGround0,
		HitGround1,
		HitGround2,
		HitGround3,
		HitGround4,
		//
		Tank_Shot,
		Tank_Ricochet,
		Tank_engine,
		Tank_move,
		//
		Tinnitus,
		//
		resultEnv,
		resultbutton,
		resultEnd,
		taskstart,

		alarm,
	};
	enum class VoiceEnum {
		GameStart000 = 1,//テスト分があるので
		Mission000,
		Mission001,
		Mission002,
		GameEnd000,
		Last30,
		Mumble000,
		Mumble001,
	};

	enum class CharaTypeID {
		Team,
		Enemy,
	};

	enum class Effect {
		ef_gndsmoke,		// 地面ヒット
		ef_fire2,			// 発砲炎
		ef_hitblood,		// 血しぶき
		ef_greexp,			// グレ爆発
		ef_dust,			// まわりの埃
		ef_break,			// ブロック破壊
		ef_shotdust,		// 発砲時の煙
		ef_rocket,
	};

	typedef char PlayerID;
	typedef short HitPoint;
	typedef short ArmerPoint;
}
