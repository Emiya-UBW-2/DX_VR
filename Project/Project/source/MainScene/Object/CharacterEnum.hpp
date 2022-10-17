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

			Upper_Ready,
			Upper_Down,

			Upper_ReloadStart,
			Upper_ReloadOne,
			Upper_ReloadEnd,

			Upper_Reload2Start,
			Upper_Reload2One,
			Upper_Reload2End,

			Upper_Aim2,
			Upper_Shot2,
			Upper_Cocking2,

			Upper_Reload3Start,
			Upper_Reload3One,
			Upper_Reload3End,

			Upper_Aim3,
			Upper_Shot3,
			Upper_Cocking3,


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
			"�Z���^�[",
			"�㔼�g",
			"�㔼�g2",
			"��",
			"����",
			"�E��",
			"������",
			"�E����",
			"�E���",
			"�E�_�~�[",
			"�E�l�w�R",
			"�E�e�w�Q",
			"���r",
			"���Ђ�",
			"�����",
			"���_�~�[",
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
