#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class CharaAnimeID : int {
			Bottom_Stand,
			Bottom_Stand_Walk,
			Bottom_Stand_WalkBack,
			Bottom_Stand_LeftStep,
			Bottom_Stand_RightStep,
			Bottom_Stand_Turn,
			Bottom_Stand_Run,

			Upper_Ready,
			Hand_Ready,

			//AR15
			Upper_Down1,
			Upper_Ready1,
			Upper_ADS1,
			Upper_Cocking1,

			Upper_Slash,

			Bottom_JumpReady,
			Bottom_Jump,
			Bottom_JumpEnd,

			AnimeIDMax,
		};
		enum class CharaFrame {
			Center,
			Upper,
			Upper2,
			Bottom,
			Head,
			Neck,
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
			Weapon,
			Holster,
			Max,
		};
		static const char* CharaFrameName[(int)CharaFrame::Max] = {
			"�Z���^�[",
			"�㔼�g",
			"�㔼�g2",
			"�����g",
			"��",
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
			"Weapon",
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
		static const char* CharaShapeName[(int)CharaShape::Max] = {
			"",
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
