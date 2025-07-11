#include	"Header.hpp"

#include	"Scene/MainScene.hpp"
#include	"Scene/MainSceneLoader.hpp"
#include	"Scene/TitleScene.hpp"

#include	"MainScene/NetworkBrowser.hpp"

#if defined(DEBUG) && DEBUG_CAM
int DBG_CamSelect = InvalidID;
#endif

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	SetDoubleStartValidFlag(true);
	//SetEnableXAudioFlag(true);//Xaudio(ロードが長いとロストするので必要に応じて)
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
	Pad->SetIsUseButton(Controls::PADS::RELOAD, true);
	Pad->SetIsUseButton(Controls::PADS::INTERACT, true);//UIのみ
	Pad->SetIsUseButton(Controls::PADS::THROW, true);
	Pad->SetIsUseButton(Controls::PADS::MELEE, false);
	Pad->SetIsUseButton(Controls::PADS::JUMP, false);
	Pad->SetIsUseButton(Controls::PADS::INVENTORY, true);
	Pad->SetIsUseButton(Controls::PADS::RUN, false);
	Pad->SetIsUseButton(Controls::PADS::WALK, true);
	Pad->SetIsUseButton(Controls::PADS::SHOT, true);
	Pad->SetIsUseButton(Controls::PADS::AIM, true);
	Pad->SetIsUseButton(Controls::PADS::ULT, true);
	Pad->SetIsUseButton(Controls::PADS::SQUAT, false);
	Pad->SetIsUseButton(Controls::PADS::PRONE, false);
	Pad->SetIsUseButton(Controls::PADS::CHECK, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE1, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE2, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE3, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE4, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE5, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE, true);
	Pad->SetIsUseButton(Controls::PADS::HEALARMOR, true);
	//
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (DXLib_refParts->FirstBootSetting()) { return 0; }
	DXLib_refParts->StartLogic();
	//追加設定
	std::string Title = "Phantom of the Bunker";
	Title += std::to_string(GetRand(1000));
	SetMainWindowText(Title.c_str());						//タイトル
	SetUseHalfLambertLighting(true);
	MV1SetLoadModelReMakeNormal(true);
	//SetUsePixelLighting(true);
	SetUseBackCulling(true);
	SetTextureAddressMode(DX_TEXADDRESS_WRAP);
	//MV1SetLoadModelUsePackDraw(TRUE);
	//
	FPS_n2::FadeControl::Create();
	FPS_n2::UIs::ButtonControl::Create();
	//
	FPS_n2::Guns::GunPartsDataManager::Create();
	FPS_n2::Objects::AmmoDataManager::Create();
	FPS_n2::Objects::ItemObjDataManager::Create();
	//初期セーブ
	SaveData::Instance()->Save();
	//BGM
	/*
	auto* SE = SoundPool::Instance();
	SE->Add(SoundType::BGM, 0, 1, "data/Sound/BGM/Title.wav");
	//*/
	//シーン
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto LoadScenePtr = std::make_shared<FPS_n2::Sceneclass::LoadScene>();
	auto MainGameScenePtr = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	//遷移先指定
	Titlescene->SetNextSceneList(0, LoadScenePtr);
	LoadScenePtr->SetNextSceneList(0, MainGameScenePtr);
	MainGameScenePtr->SetNextSceneList(0, Titlescene);
	MainGameScenePtr->SetNextSceneList(1, LoadScenePtr);
	SceneControl::Instance()->SetFirstScene(Titlescene);
	//SceneControl::Instance()->SetFirstScene(LoadScenePtr);
	//メインロジック開始
	DXLib_refParts->MainLogic();
	return 0;
}
