#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	// íËêî
	static const float Frame_Rate{60.f};

	static const float Scale_Rate{12.5f};

	namespace Sceneclass {
		enum class SoundEnum {
			RunFoot = (int)SoundEnumCommon::Num,
			// 
			CountDown,
			Second,
			OneMunute,
			TimeUp,

			Normal,
			// 
		};
		enum class CharaTypeID {
			Team,
			Enemy,
		};

		enum class Object2DType {
			Human,
			Bullet,
		};
		static const float Tile_DispSize{ 48.f };
		static const float Base_CamScale{ 64.f };//ãÛä‘è„ÇÃê∏ìx
	};

	typedef char PlayerID;
	typedef short HitPoint;
	typedef short ArmerPoint;
};
