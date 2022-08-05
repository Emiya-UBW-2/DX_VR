#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class CharaAnimeID : int {
			Upper_Down,
			Upper_RunningStart,
			Upper_Running,
			Upper_Sprint,

			Bottom_Stand,
			Bottom_Walk,
			Bottom_WalkBack,
			Bottom_LeftStep,
			Bottom_RightStep,
			Bottom_Turn,
			Bottom_Run,

			Upper_FlightNormal,
			Bottom_FlightNormal,

			Bottom_FlightKickLeft,
			Bottom_FlightKickRight,

			Upper_UseItem,
			Upper_FlightUseItem,

			Upper_SetMagic,
			Upper_FlightSetMagic,

			Upper_UseMagic1,
			Upper_FlightUseMagic1,

			AnimeIDMax,
		};
		enum class CharaFrame {
			Center,
			Upper,
			Upper2,
			Head,
			LeftEye,
			RightEye,
			LeftFoot,
			RightFoot,
			RightWrist,
			RightHandJoint,
			RightThumb,
			RightPointer,
			LeftArm,
			LeftArm2,
			LeftWrist,
			LeftHandJoint,
			Max,
		};
		const char* CharaFrameName[(int)CharaFrame::Max] = {
			"センター",
			"上半身",
			"上半身2",
			"頭",
			"左目",
			"右目",
			"左足首",
			"右足首",
			"右手首",
			"右ダミー",
			"右人指３",
			"右親指２",
			"左腕",
			"左ひじ",
			"左手首",
			"左ダミー",
		};
		enum class CharaShape {
			None,
			A,
			I,
			U,
			E,
			O,
			MAZIME,
			EYECLOSE,
			Max,
		};
		const char* CharaShapeName[(int)CharaShape::Max] = {
			"",
			"あ",
			"い",
			"う",
			"え",
			"お",
			"真面目",
			"まばたき",
		};

		enum class CharaTypeID {
			Team,
			Enemy,
		};

		enum class MagicType {
			FireBall,
			Thunder,
			SearchSonic,
			Max,
		};
		const char* MagicTypeName[(int)MagicType::Max] = {
			"FireBall",
			"Thunder",
			"SearchSonic",
		};

	};
};
