#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	namespace Sceneclass {
		enum class SoundEnum {
			RunFoot = (int)SoundEnumCommon::Num,
			//
			CountDown,
			Second,
			OneMunute,
			TimeUp,
			//
		};
		enum class CharaTypeID {
			Team,
			Enemy,
		};

		enum class Object2DType {
			Human,
			Stone,
		};
		static const int Player_Num{ 16 };
	};

	typedef char PlayerID;
	typedef short HitPoint;
	typedef short ArmerPoint;
};
