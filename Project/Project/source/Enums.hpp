#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//定数
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };
	

	enum class Effect {
		ef_fire,			//発砲炎
		ef_reco,			//小口径跳弾
		ef_smoke,			//銃の軌跡
		ef_gndsmoke,		//地面の軌跡
		ef_fire2,			//発砲炎
		ef_hitblood,		//血しぶき
		ef_greexp,			//グレ爆発
		ef_greexp2,			//グレ爆発2
		ef_FireBallStart,	//火はじめ
		ef_FireBallLoop,	//火ループ
		ef_FireBallHit,		//火ヒット

		ef_ThunderStart,	//雷はじめ
		ef_ThunderLoop,		//雷ループ

		ef_Sonic,			//衝撃波

		ef_ThunderHit,		//雷ヒット
		effects,			//読み込む総量
	};
	namespace Sceneclass {
		enum class scenes
		{
			NONE_SCENE,
			LOAD_SCENE,
			MAIN_LOOP
		};
		enum class ObjType {
			Human,
			Houki,
			Item,
		};
		enum class SoundEnum {
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
	};

	namespace Sceneclass {
		enum class GunAnimeID : int {
			Cocking,
			ReloadStart,
			ReloadOne,
			ReloadEnd,
			AnimeIDMax,
		};
		enum class GunFrame {
			Center,
			Cart,
			CartVec,
			Muzzle,
			Eyepos,
			Lens,
			LensSize,
			Magpos,
			LeftHandPos,
			LeftHandYvec,
			LeftHandZvec,
			Max,
		};
		const char* GunFrameName[(int)GunFrame::Max] = {
			"センター",
			"cart",
			"cartvec",
			"muzzle",
			"EyePos",
			"lens",
			"lenssize",
			"magpos",
			"lefthand",
			"lefthand_yvec",
			"lefthand_zvec",
		};
	};
	namespace Sceneclass {
		enum class TargetAnimeID : int {
			Cocking,
			ReloadStart,
			ReloadOne,
			ReloadEnd,
			AnimeIDMax,
		};
		enum class TargetFrame {
			Center,
			Cart,
			CartVec,
			Muzzle,
			Eyepos,
			Lens,
			LensSize,
			Magpos,
			Max,
		};
		const char* TargetFrameName[(int)TargetFrame::Max] = {
			"センター",
			"cart",
			"cartvec",
			"muzzle",
			"EyePos",
			"lens",
			"lenssize",
			"magpos",
		};
	};

};
