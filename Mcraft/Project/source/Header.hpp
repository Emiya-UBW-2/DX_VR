#pragma once

#include	"DXLib_ref/Souece/DXLib_ref.h"
using namespace DXLibRef;

#include	"Enums.hpp"

//���肩�猩�܂킵���ł���@�\
#define DEBUG_CAM (FALSE)
//DEBUG_CAM���L���Ȃ����ŃA�j���[�V�����̕ҏW���ł���@�\
#define DEBUG_ANIM (FALSE)
//�l�b�g�ΐ�̃f�o�b�O
#define DEBUG_NET (FALSE)
//�X���b�h�̎��s���ԕ\��
#define CHECKTHREADTIME (false)
//�q�b�g�{�b�N�X�\��
#define DRAW_HITBOX (false)

#if defined(DEBUG) && DEBUG_CAM
extern int DBG_CamSelect;
#endif
