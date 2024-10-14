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
	SetEnableXAudioFlag(TRUE);//Xaudio(ロードが長いとロストするので必要に応じて)
	DXLib_ref::Create();
	//使用するボタンの指定
	auto* Pad = PadControl::Instance();
	Pad->SetIsUseButton(PADS::MOVE_W, true);
	Pad->SetIsUseButton(PADS::MOVE_A, true);
	Pad->SetIsUseButton(PADS::MOVE_S, true);
	Pad->SetIsUseButton(PADS::MOVE_D, true);
	Pad->SetIsUseButton(PADS::MOVE_STICK, true);
	Pad->SetIsUseButton(PADS::DIR_UP, false);
	Pad->SetIsUseButton(PADS::DIR_DOWN, false);
	Pad->SetIsUseButton(PADS::DIR_LEFT, false);
	Pad->SetIsUseButton(PADS::DIR_RIGHT, false);
	Pad->SetIsUseButton(PADS::DIR_STICK, true);
	Pad->SetIsUseButton(PADS::LEAN_L, true);//UIのみ
	Pad->SetIsUseButton(PADS::LEAN_R, true);//UIのみ
	Pad->SetIsUseButton(PADS::RELOAD, true);//UIのみ
	Pad->SetIsUseButton(PADS::INTERACT, true);//UIのみ
	Pad->SetIsUseButton(PADS::THROW, true);
	Pad->SetIsUseButton(PADS::MELEE, true);
	Pad->SetIsUseButton(PADS::JUMP, true);
	Pad->SetIsUseButton(PADS::INVENTORY, true);
	Pad->SetIsUseButton(PADS::RUN, true);
	Pad->SetIsUseButton(PADS::WALK, true);
	Pad->SetIsUseButton(PADS::SHOT, true);
	Pad->SetIsUseButton(PADS::AIM, true);
	Pad->SetIsUseButton(PADS::ULT, true);
	Pad->SetIsUseButton(PADS::SQUAT, true);
	Pad->SetIsUseButton(PADS::PRONE, true);
	Pad->SetIsUseButton(PADS::CHECK, true);
	//
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (!DXLib_refParts->StartLogic()) { return 0; }
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
	auto* BGM = BGMPool::Instance();
	auto* OptionParts = OPTION::Instance();
	//初期開放
	SaveDataParts->Save();
	//BGM
	BGM->Add(0, "data/Sound/BGM/Title.wav");
	BGM->Add(1, "data/Sound/BGM/Vivaldi_Winter.wav", true);
	BGM->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
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
	SceneParts->AddList(LoadScenePtr);
	SceneParts->AddList(MainGameScenePtr);
	//最初の読み込み
	if (!DXLib_refParts->MainLogic()) { return 0; }
	return 0;
}
