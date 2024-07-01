#include"Header.hpp"

#include "MainScene/Player/Player.hpp"
//
#include "Scene/TitleScene.hpp"
#include "Scene/CustomScene.hpp"
#include "Scene/TutorialScene.hpp"
#include "Scene/MainScene.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	SetEnableXAudioFlag(TRUE);//Xaudio(ロードが長いとロストするので必要に応じて)
	DXLib_ref::Create();
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (!DXLib_refParts->StartLogic()) { return 0; }
	//追加設定
	SetMainWindowText("Phantom of the Bunker");						//タイトル
	//MV1SetLoadModelUsePackDraw(TRUE);
	SetUseHalfLambertLighting(TRUE);	//ハーフランバート化
	//
	FPS_n2::Sceneclass::PlayerManager::Create();
	FPS_n2::Sceneclass::GunAnimManager::Create();
	FPS_n2::Sceneclass::ModDataManager::Create();
	FPS_n2::Sceneclass::AmmoDataManager::Create();
	FPS_n2::Sceneclass::CommonBattleResource::Create();
	//
	auto* SaveDataParts = SaveDataClass::Instance();
	auto* BGM = BGMPool::Instance();
	auto* OptionParts = OPTION::Instance();
	//初期開放
	SaveDataParts->SetParam("Glock17 Gen3 Barrel", 1);
	SaveDataParts->SetParam("Glock17 Gen3 Frame", 1);
	SaveDataParts->SetParam("Glock17 Gen3 Slide", 1);
	SaveDataParts->SetParam("GMag 17Round", 1);
	SaveDataParts->SetParam("G-Normal IronSight", 1);
	SaveDataParts->SetParam("X400 LaserSight", 1);
	SaveDataParts->SetParam("Universal Mount", 1);
	SaveDataParts->SetParam("VIPER RedDotSight", 1);
	SaveDataParts->SetParam("ULT Unlock", std::max(static_cast<int>(SaveDataParts->GetParam("ULT Unlock")), 1));
	SaveDataParts->SetParam("Glock Unlock", std::max(static_cast<int>(SaveDataParts->GetParam("Glock Unlock")), 1));
	//SaveDataParts->SetParam("UnlockHardMode", 1);
	bool IsFirstGame = (SaveDataParts->GetParam("FirstGame") != 1);
	SaveDataParts->Save();
	//BGM
	BGM->Add(0, "data/Sound/BGM/Title.wav");
	BGM->Add(1, "data/Sound/BGM/Vivaldi_Winter.wav", true);
	BGM->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
	//シーン
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto Customscene = std::make_shared<FPS_n2::Sceneclass::CustomScene>();
	auto Tutorialscene = std::make_shared<FPS_n2::Sceneclass::TutorialScene>();
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	auto MAINLOOPscene_HardMode = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	MAINLOOPscene_HardMode->SetPlayMode(true);
	//遷移先指定
	Titlescene->SetNextSceneList(0, MAINLOOPscene);
	Titlescene->SetNextSceneList(1, Customscene);
	Titlescene->SetNextSceneList(2, Tutorialscene);
	Titlescene->SetNextSceneList(3, MAINLOOPscene_HardMode);
	Customscene->SetNextSceneList(0, Titlescene);
	Tutorialscene->SetNextSceneList(0, Titlescene);
	MAINLOOPscene->SetNextSceneList(0, Titlescene);
	MAINLOOPscene_HardMode->SetNextSceneList(0, Titlescene);

	if (IsFirstGame) {
		SceneControl::Instance()->AddList(Tutorialscene);
		SceneControl::Instance()->AddList(Titlescene);
		SceneControl::Instance()->AddList(Customscene);
		SceneControl::Instance()->AddList(MAINLOOPscene);
	}
	else {
		SceneControl::Instance()->AddList(Titlescene);
		SceneControl::Instance()->AddList(Customscene);
		SceneControl::Instance()->AddList(Tutorialscene);
		SceneControl::Instance()->AddList(MAINLOOPscene_HardMode);
	}
	//最初の読み込み
	if (!DXLib_refParts->MainLogic()) { return 0; }
	return 0;
}
