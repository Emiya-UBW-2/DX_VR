#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class CharaAnimeID : int {
			RightHand,
			LeftHand,

			Upper_Aim,
			Upper_Shot,
			Upper_Cocking,
			Upper_RunningStart,
			Upper_Running,
			Upper_Sprint,

			Mid_Squat,

			Bottom_Stand,
			Bottom_Walk,
			Bottom_WalkBack,
			Bottom_LeftStep,
			Bottom_RightStep,
			Bottom_Turn,
			Bottom_Run,

			All_Prone,
			All_ProneShot,
			All_ProneCocking,

			All_ProneWalk,

			All_PronetoStand,

			Upper_ReloadStart,
			Upper_ReloadOne,
			Upper_ReloadEnd,

			All_Prone_ReloadStart,
			All_Prone_ReloadOne,
			All_Prone_ReloadEnd,

			Upper_Ready,
			Upper_Down,




			Upper_Reload2Start,
			Upper_Reload2One,
			Upper_Reload2End,

			All_Prone_Reload2Start,
			All_Prone_Reload2One,
			All_Prone_Reload2End,

			Upper_Aim2,
			Upper_Shot2,
			Upper_Cocking2,

			All_Prone2,
			All_ProneShot2,
			All_ProneCocking2,



			Upper_Reload3Start,
			Upper_Reload3One,
			Upper_Reload3End,

			All_Prone_Reload3Start,
			All_Prone_Reload3One,
			All_Prone_Reload3End,

			Upper_Aim3,
			Upper_Shot3,
			Upper_Cocking3,

			All_Prone3,
			All_ProneShot3,
			All_ProneCocking3,




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
		static const char* CharaFrameName[(int)CharaFrame::Max] = {
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
		static const char* CharaShapeName[(int)CharaShape::Max] = {
			"",
			"あ",
			"い",
			"う",
			"え",
			"お",
			"真面目",
			"まばたき",
		};

	};
};
