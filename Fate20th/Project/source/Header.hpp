#pragma once

#define NOMINMAX

#ifdef _DEBUG

#define DEBUG

#endif // _DEBUG

//ここをコメントアウト
#define DEBUG


#include "DXLib_ref.h"

using namespace DXLib_ref;

//FPS(共通)
inline float FPS{ 60.f };

#include "Enums.hpp"
#include "MainScene/Object/ObjectBaseEnum.hpp"
#include "MainScene/Object/WeaponEnum.hpp"
#include "MainScene/Object/CharacterEnum.hpp"
//
#include "sub.hpp"
#include "NetWork.hpp"
//
#include "MainScene/BackGround/BackGroundSub.hpp"
#include "MainScene/BackGround/BackGround.hpp"

#include "MainScene/Object/ObjectBase.hpp"
#include "MainScene/Object/Weapon.hpp"
#include "MainScene/Object/Character.hpp"

#include "ObjectManager.hpp"
#include "MainScene/Player/Player.hpp"
#include "MainScene/Player/CPU.hpp"

#include "MainScene/UI/MainSceneUIControl.hpp"
//
#include "MainScene/NetworkBrowser.hpp"
#include "Scene/MainScene.hpp"
