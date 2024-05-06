#include"Header.hpp"

#include "MainScene/Player/Player.hpp"
//
#include "Scene/TitleScene.hpp"
#include "Scene/CustomScene.hpp"
#include "Scene/TutorialScene.hpp"
#include "Scene/MainScene.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	DXDraw::Create();								//汎用
#ifdef DEBUG
	auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG
	auto* DrawParts = DXDraw::Instance();

	DrawParts->Init();
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
	//シーンコントロール
	auto scene = std::make_unique<SceneControl>(IsFirstGame ? (std::shared_ptr<TEMPSCENE>&)Tutorialscene : (std::shared_ptr<TEMPSCENE>&)Titlescene);
	//遷移先指定
	Titlescene->Set_Next(MAINLOOPscene);
	Customscene->Set_Next(Titlescene);
	Tutorialscene->Set_Next(Titlescene);
	MAINLOOPscene->Set_Next(Titlescene);
	//最初の読み込み
	scene->GetNowScene()->Load();
	//繰り返し
	while (true) {
		scene->StartScene();
		while (true) {
			if (!DrawParts->FirstExecute()) {
				SaveDataParts->Save();//セーブ
				return 0;
			}
			FPS = GetFrameRate();
#ifdef DEBUG
			clsDx();
			DebugParts->SetStartPoint();
			DebugParts->SetPoint("Execute start");
#endif // DEBUG
			if (scene->Execute()) { break; }
			//描画
			scene->Draw();
			//デバッグ
#ifdef DEBUG
			DebugParts->DebugWindow(y_r(1920 - 250), y_r(150));
			//TestDrawShadowMap(DrawParts->m_Shadow[0].GetHandle(), 0, 0, 960, 540);
#endif // DEBUG
			DrawParts->Screen_Flip();//画面の反映
		}
		SaveDataParts->Save();//セーブ
		bool IsGoTitle = (scene->GetNowScene() == Titlescene);
		bool IsReturnTitle = (scene->GetNowScene()->Get_Next() == Titlescene);
		if (IsGoTitle) {
			switch (Titlescene->SelMode()) {
				case 0:
					Titlescene->Set_Next(MAINLOOPscene);
					break;
				case 1:
					Titlescene->Set_Next(Customscene);
					break;
				case 2:
					Titlescene->Set_Next(Tutorialscene);
					break;
				case 3:
					Titlescene->Set_Next(MAINLOOPscene);
					break;
				default:
					Titlescene->Set_Next(MAINLOOPscene);//戻しとく
					break;
			}
		}
		scene->NextScene();
		if (IsGoTitle) {
			Titlescene->Dispose_Load();
			if (Titlescene->SelMode() != 1) {
				Customscene->Dispose_Load();
			}
			if (Titlescene->SelMode() != 2) {
				Tutorialscene->Dispose_Load();
			}
			switch (Titlescene->SelMode()) {
				case 0:
					if (MAINLOOPscene->SetPlayMode(false)) {
						scene->GetNowScene()->Dispose_Load();
					}
					break;
				case 1:
					MAINLOOPscene->Dispose_Load();
					break;
				case 2:
					MAINLOOPscene->Dispose_Load();
					break;
				case 3:
					if (MAINLOOPscene->SetPlayMode(true)) {
						scene->GetNowScene()->Dispose_Load();
					}
					break;
				default:
					break;
			}
		}
		else if (IsReturnTitle) {
			MAINLOOPscene->Dispose_Load();
			Customscene->Dispose_Load();
			Tutorialscene->Dispose_Load();
		}
		scene->GetNowScene()->Load();
	}
	return 0;
}
