#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	// íËêî
	constexpr float Frame_Rate{60.f};
	constexpr float Scale_Rate{12.5f};

	namespace Sceneclass {
		enum class SoundEnum {
			RunFoot = static_cast<int>(SoundEnumCommon::Num),
			Shot1,
			Shot2,
			Shot3,
			Hit,
			Guard,
			Bomb,
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
			Metal,
		};
		constexpr float Tile_DispSize{ 48.f };
		constexpr float Base_CamScale{ 64.f };//ãÛä‘è„ÇÃê∏ìx
	};

	using PlayerID = char;
	using HitPoint = short;
	using ArmerPoint = short;
};
