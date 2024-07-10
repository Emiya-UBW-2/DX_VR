#include	"Header.hpp"

#include	"sub.hpp"
#include	"MainScene/Player/Player.hpp"
#include	"MainScene/BackGround/BackGround.hpp"
// 
#include	"CommonScene/Object/Object2DManager.hpp"
#include	"Scene/TitleScene.hpp"
#include	"Scene/MainGameScene.hpp"
#include	"Scene/TestGameScene.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// SetEnableXAudioFlag(TRUE);// Xaudio(ロードが長いとロストするので必要に応じて)
	DXLib_ref::Create();
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (!DXLib_refParts->StartLogic()) { return 0; }
	// 追加設定
	SetMainWindowText("Title");					// タイトル
	// MV1SetLoadModelUsePackDraw(TRUE);
	// SetUseHalfLambertLighting(TRUE);	// ハーフランバート化
	// 
	FPS_n2::Sceneclass::Object2DManager::Create();
	FPS_n2::Sceneclass::PlayerManager::Create();
	FPS_n2::Sceneclass::CommonBattleResource::Create();
	FPS_n2::Sceneclass::Cam2DControl::Create();
	FPS_n2::Sceneclass::BackGroundClassBase::Create();
	FPS_n2::Sceneclass::ButtonControl::Create();

	// 
	auto* SaveDataParts = SaveDataClass::Instance();
	auto* BGM = BGMPool::Instance();
	auto* OptionParts = OPTION::Instance();
	// 初期開放
	// SaveDataParts->SetParam("UnlockHardMode", 1);
	// bool IsFirstGame = (SaveDataParts->GetParam("FirstGame") != 1);
	SaveDataParts->Save();
	// BGM
	// BGM->Add(0, "data/Sound/BGM/Title.wav");
	BGM->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
	// シーン
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto MainGamescene = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	auto TestGamescene = std::make_shared<FPS_n2::Sceneclass::TestGameScene>();
	// 遷移先指定
	Titlescene->SetNextSceneList(0, MainGamescene);
	Titlescene->SetNextSceneList(1, TestGamescene);
	MainGamescene->SetNextSceneList(0, Titlescene);
	MainGamescene->SetNextSceneList(1, MainGamescene);
	TestGamescene->SetNextSceneList(0, Titlescene);

	auto* SceneParts = SceneControl::Instance();
	/*
	SceneParts->AddList(Titlescene);
	SceneParts->AddList(MainGamescene);
	SceneParts->AddList(TestGamescene);
	// */
	// *
	SceneParts->AddList(MainGamescene);
	SceneParts->AddList(Titlescene);
	SceneParts->AddList(TestGamescene);
	// */
	// 最初の読み込み
	if (!DXLib_refParts->MainLogic()) { return 0; }
	return 0;
}
