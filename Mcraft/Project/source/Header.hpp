#pragma once

#include	"DXLib_ref/Souece/DXLib_ref.h"
using namespace DXLibRef;

#include	"Enums.hpp"

//周りから見まわしができる機能
#define DEBUG_CAM (false)
//DEBUG_CAMが有効なうえでアニメーションの編集ができる機能
#define DEBUG_ANIM (false)
//ネット対戦のデバッグ
#define DEBUG_NET (false)
//スレッドの実行時間表示
#define CHECKTHREADTIME (false)
//ヒットボックス表示
#define DRAW_HITBOX (false)

#if defined(DEBUG) && DEBUG_CAM
extern int DBG_CamSelect;
#endif
