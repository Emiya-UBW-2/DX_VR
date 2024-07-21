#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class CharaAnimeID : uint8_t {
			Bottom_Stand,
			Bottom_Stand_Walk,
			Bottom_Stand_WalkBack,
			Bottom_Stand_LeftStep,
			Bottom_Stand_RightStep,
			Bottom_Stand_Turn,
			Bottom_Stand_Run,

			Bottom_Squat,
			Bottom_Squat_Walk,
			//
			Upper_Ready,
			Hand_Ready,
			//
			Bottom_Stand_Attack,
			//
			AnimeIDMax,
		};
		enum class CharaFrame : uint8_t {
			Center,
			Upper,
			Upper2,
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
			"�Z���^�[",
			"�㔼�g",
			"�㔼�g2",
			"��",
			"����",
			"�E��",
			"����",
			"���Ђ�",
			"������",
			"�E��",
			"�E�Ђ�",
			"�E����",
			"�E�r",
			"�E�Ђ�",
			"�E���",
			"�E�_�~�[",
			"�E�l�w�R",
			"�E�e�w�Q",
			"���r",
			"���Ђ�",
			"�����",
			"���_�~�[",
			"LEFT_mag",
			"LEFT_magzvec",
			"LEFT_magyvec",
			"Gun",
			"Holster",
		};
		enum class CharaShape : uint8_t {
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
			"��",
			"��",
			"��",
			"��",
			"��",
			"�^�ʖ�",
			"�܂΂���",
		};
	};
};
