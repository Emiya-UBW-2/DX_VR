#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//’è”
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };
	
	enum class Effect {
		ef_fire,			//”­–C‰Š
		ef_reco,			//¬ŒûŒa’µ’e
		ef_smoke,			//e‚Ì‹OÕ
		ef_gndsmoke,		//’n–Ê‚Ì‹OÕ
		ef_fire2,			//”­–C‰Š
		ef_hitblood,		//ŒŒ‚µ‚Ô‚«
		ef_greexp,			//ƒOƒŒ”š”­
		ef_greexp2,			//ƒOƒŒ”š”­2

		effects,			//“Ç‚İ‚Ş‘—Ê
	};
	namespace Sceneclass {
		enum class scenes
		{
			NONE_SCENE,
			LOAD_SCENE,
			MAIN_LOOP
		};
		enum class ObjType {
			Vehicle,
			Ammo,
		};
		enum class SoundEnum {
			Tank_Shot,
			Tank_Ricochet,
			Tank_Damage,
			Tank_engine,
			Tank_Eject,
			Tank_Reload,

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


};
