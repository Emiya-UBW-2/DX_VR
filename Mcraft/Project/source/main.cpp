#include"Header.hpp"

#include "Scene/MainScene.hpp"
#include "Scene/MainSceneLoader.hpp"
#include "Scene/TitleScene.hpp"

#include "MainScene/NetworkBrowser.hpp"

#ifdef DEBUG
int DBG_CamSel = -1;
#endif

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	SetDoubleStartValidFlag(TRUE);
	//SetEnableXAudioFlag(TRUE);//Xaudio(ロードが長いとロストするので必要に応じて)
	DXLib_ref::Create();
	//使用するボタンの指定
	auto* Pad = PadControl::Instance();
	Pad->SetIsUseButton(Controls::PADS::MOVE_W, true);
	Pad->SetIsUseButton(Controls::PADS::MOVE_A, true);
	Pad->SetIsUseButton(Controls::PADS::MOVE_S, true);
	Pad->SetIsUseButton(Controls::PADS::MOVE_D, true);
	Pad->SetIsUseButton(Controls::PADS::MOVE_STICK, true);
	Pad->SetIsUseButton(Controls::PADS::DIR_UP, false);
	Pad->SetIsUseButton(Controls::PADS::DIR_DOWN, false);
	Pad->SetIsUseButton(Controls::PADS::DIR_LEFT, false);
	Pad->SetIsUseButton(Controls::PADS::DIR_RIGHT, false);
	Pad->SetIsUseButton(Controls::PADS::DIR_STICK, true);
	Pad->SetIsUseButton(Controls::PADS::LEAN_L, true);
	Pad->SetIsUseButton(Controls::PADS::LEAN_R, true);
	Pad->SetIsUseButton(Controls::PADS::RELOAD, true);//UIのみ
	Pad->SetIsUseButton(Controls::PADS::INTERACT, true);//UIのみ
	//Pad->SetIsUseButton(Controls::PADS::THROW, true);
	//Pad->SetIsUseButton(Controls::PADS::MELEE, true);
	//Pad->SetIsUseButton(Controls::PADS::JUMP, true);
	Pad->SetIsUseButton(Controls::PADS::INVENTORY, true);
	//Pad->SetIsUseButton(Controls::PADS::RUN, true);
	Pad->SetIsUseButton(Controls::PADS::WALK, true);
	Pad->SetIsUseButton(Controls::PADS::SHOT, true);
	Pad->SetIsUseButton(Controls::PADS::AIM, true);
	Pad->SetIsUseButton(Controls::PADS::ULT, true);
	//Pad->SetIsUseButton(Controls::PADS::SQUAT, true);
	//Pad->SetIsUseButton(Controls::PADS::PRONE, true);
	//Pad->SetIsUseButton(Controls::PADS::CHECK, true);
	//
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (DXLib_refParts->FirstBootSetting()) { return 0; }
	DXLib_refParts->StartLogic();
	//追加設定
	SetMainWindowText("Phantom of the Bunker");						//タイトル
	//SetUseHalfLambertLighting(TRUE);
	MV1SetLoadModelReMakeNormal(TRUE);
	//SetUsePixelLighting(TRUE);
	SetUseBackCulling(TRUE);
	SetTextureAddressMode(DX_TEXADDRESS_WRAP);
	//
	FPS_n2::Player::PlayerManager::Create();
	FPS_n2::Sceneclass::CommonBattleResource::Create();
	FPS_n2::BackGround::BackGroundClass::Create();
	FPS_n2::NetWorkBrowser::Create();
	FPS_n2::Sceneclass::ButtonControl::Create();
	FPS_n2::Sceneclass::GunAnimManager::Create();
	FPS_n2::Sceneclass::ModDataManager::Create();
	FPS_n2::Sceneclass::AmmoDataManager::Create();
	//
	auto* SaveDataParts = SaveDataClass::Instance();
	auto* SE = SoundPool::Instance();
	//初期開放
	SaveDataParts->Save();
	//BGM
	SE->Add(SoundType::BGM, 0, 1, "data/Sound/BGM/Title.wav");
	SE->Add(SoundType::BGM, 1, 1, "data/Sound/BGM/Vivaldi_Winter.wav", true);
	//シーン
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto LoadScenePtr = std::make_shared<FPS_n2::Sceneclass::LoadScene>();
	auto MainGameScenePtr = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();


	//遷移先指定
	Titlescene->SetNextSceneList(0, MainGameScenePtr);
	LoadScenePtr->SetNextSceneList(0, MainGameScenePtr);
	MainGameScenePtr->SetNextSceneList(0, Titlescene);
	MainGameScenePtr->SetNextSceneList(1, MainGameScenePtr);

	auto* SceneParts = SceneControl::Instance();
	//SceneParts->AddList(Titlescene);
	SceneParts->SetFirstScene(LoadScenePtr);
	//最初の読み込み
	DXLib_refParts->MainLogic();
	return 0;
}
