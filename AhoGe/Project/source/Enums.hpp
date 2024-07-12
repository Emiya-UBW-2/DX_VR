#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	// íËêî
	constexpr int INVALID_ID = -1;
	constexpr float Frame_Rate{60.f};
	constexpr float Scale_Rate{12.5f};

	namespace Sceneclass {
		enum class SoundEnum {
			RunFoot = static_cast<int>(SoundEnumCommon::Num),
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
		constexpr float Tile_DispSize{ 48.f };
		constexpr float Base_CamScale{ 64.f };//ãÛä‘è„ÇÃê∏ìx
	};

	using PlayerID = char;
	using HitPoint = short;
	using ArmerPoint = short;
};
