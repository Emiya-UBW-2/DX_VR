﻿#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Charas {
		enum class CharaAnimeID : int {
			Bottom_Stand,
			Bottom_Stand_Walk,
			Bottom_Stand_WalkBack,
			Bottom_Stand_LeftStep,
			Bottom_Stand_RightStep,
			Bottom_Stand_Turn,
			Bottom_Stand_Run,

			Bottom_Squat,
			Bottom_Squat_Walk,
			Bottom_Squat_WalkBack,
			Bottom_Squat_LeftStep,
			Bottom_Squat_RightStep,
			Bottom_Squat_Turn,//12
			//
			Upper_Ready,
			//
			Right_Thumb,
			Right_Point,
			Right_middle,
			Right_Ring,
			Right_Pinky,
			//
			Left_Thumb,
			Left_Point,
			Left_middle,
			Left_Ring,
			Left_Pinky,
			//
			NotUse,
			Rappelling,
			//
			AnimeIDMax,
		};
		enum class CharaFrame {
			Center,
			Upper,
			Upper2,
			Neck,
			Head,
			LeftEye,
			RightEye,
			LeftFoot1,
			LeftFoot2,
			LeftFoot,
			RightFoot1,
			RightFoot2,
			RightFoot,
			RightArm,
			RightArm2,
			RightWrist,
			RightHandJoint,
			RightThumb,
			RightPointer,
			LeftArm,
			LeftArm2,
			LeftWrist,
			LeftHandJoint,
			LeftMag,
			LeftMagzvec,
			LeftMagyvec,
			Gun,
			Holster,
			Max,
		};
		static const char* CharaFrameName[static_cast<int>(CharaFrame::Max)] = {
			"センター",
			"上半身",
			"上半身2",
			"首",
			"頭",
			"左目",
			"右目",
			"左足",
			"左ひざ",
			"左足首",
			"右足",
			"右ひざ",
			"右足首",
			"右腕",
			"右ひじ",
			"右手首",
			"右ダミー",
			"右人指３",
			"右親指２",
			"左腕",
			"左ひじ",
			"左手首",
			"左ダミー",
			"LEFT_mag",
			"LEFT_magzvec",
			"LEFT_magyvec",
			"Gun",
			"Holster",
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
		static const char* CharaShapeName[static_cast<int>(CharaShape::Max)] = {
			"None",
			"あ",
			"い",
			"う",
			"え",
			"お",
			"真面目",
			"まばたき",
		};
	}
}
