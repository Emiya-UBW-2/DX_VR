#include"Header.hpp"

#include "Scene/MainScene.hpp"
#include "Scene/MainSceneLoader.hpp"

#include "MainScene/NetworkBrowser.hpp"

#ifdef DEBUG
int DBG_CamSel = -1;
#endif

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	SetDoubleStartValidFlag(TRUE);
	SetEnableXAudioFlag(TRUE);//Xaudio(ロードが長いとロストするので必要に応じて)
	DXLib_ref::Create();
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (!DXLib_refParts->StartLogic()) { return 0; }
	//追加設定
	std::string Name = "AA"+std::to_string(GetNowHiPerformanceCount());
	SetMainWindowText(Name.c_str());						//タイトル
	//SetUseHalfLambertLighting(TRUE);
	MV1SetLoadModelReMakeNormal(TRUE);
	//
	FPS_n2::Sceneclass::WeaponAnimManager::Create();
	FPS_n2::Sceneclass::PlayerManager::Create();
	FPS_n2::Sceneclass::CommonBattleResource::Create();
	FPS_n2::Sceneclass::WeaponAnimManager::Instance()->Load("data/CharaAnime/");
	FPS_n2::Sceneclass::BackGroundClass::Create();
	FPS_n2::Sceneclass::NetWorkBrowser::Create();
	//シーン
	auto MAINLOOPLOADERscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOPLOADER>();
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	SceneControl::Instance()->AddList(MAINLOOPLOADERscene);
	SceneControl::Instance()->AddList(MAINLOOPscene);
	//遷移先指定
	MAINLOOPLOADERscene->SetNextSceneList(0, MAINLOOPscene);
	MAINLOOPscene->SetNextSceneList(0, MAINLOOPLOADERscene);
	//最初の読み込み
	if (!DXLib_refParts->MainLogic()) { return 0; }
	return 0;
}
