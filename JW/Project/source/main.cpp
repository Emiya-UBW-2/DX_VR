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
	//SetUseHalfLambertLighting(TRUE);	//ハーフランバート化
	//
	FPS_n2::GetItemLog::Create();
	FPS_n2::Sceneclass::ObjectManager::Create();
	FPS_n2::Sceneclass::PlayerManager::Create();
	FPS_n2::Sceneclass::GunAnimManager::Create();
	FPS_n2::Sceneclass::ModDataManager::Create();
	FPS_n2::Sceneclass::AmmoDataManager::Create();
	FPS_n2::Sceneclass::CommonBattleResource::Create();
	//
	auto* ItemLogParts = FPS_n2::GetItemLog::Instance();
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
	SaveDataParts->Save();
	//BGM
	BGM->Add(0, "data/Sound/BGM/Title.wav");
	BGM->Add(1, "data/Sound/BGM/Vivaldi_Winter.wav");
	BGM->SetVol(OptionParts->Get_BGM());
	//シーン
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto Customscene = std::make_shared<FPS_n2::Sceneclass::CustomScene>();
	auto Tutorialscene = std::make_shared<FPS_n2::Sceneclass::TutorialScene>();
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	//シーンコントロール
	auto scene = std::make_unique<SceneControl>(Titlescene);
	//遷移先指定
	Titlescene->Set_Next(MAINLOOPscene);
	Customscene->Set_Next(Titlescene);
	Tutorialscene->Set_Next(Titlescene);
	MAINLOOPscene->Set_Next(Titlescene);
	bool isTitleLoop = false;
	auto TitleLoad = [&]() {
		if (!isTitleLoop) {
			isTitleLoop = true;
			Titlescene->Load();
		}
	};
	auto TitleUnLoad = [&]() {
		if (isTitleLoop) {
			isTitleLoop = false;
			Titlescene->Dispose_Load();
		}
	};
	bool isCustomLoop = false;
	auto CustomLoad = [&]() {
		if (!isCustomLoop) {
			isCustomLoop = true;
			Customscene->Load();
		}
	};
	auto CustomUnLoad = [&]() {
		if (isCustomLoop) {
			isCustomLoop = false;
			Customscene->Dispose_Load();
		}
	};
	bool isTutorialLoop = false;
	auto TutorialLoad = [&]() {
		if (!isTutorialLoop) {
			isTutorialLoop = true;
			Tutorialscene->Load();
		}
	};
	auto TutorialUnLoad = [&]() {
		if (isTutorialLoop) {
			isTutorialLoop = false;
			Tutorialscene->Dispose_Load();
		}
	};
	bool isMainLoop = false;
	auto MainLoad = [&](bool IsHardMode) {
		if (MAINLOOPscene->SetPlayMode(IsHardMode)) {
			isMainLoop = true;
			MAINLOOPscene->Dispose_Load();
			MAINLOOPscene->Load();
		}
		else {
			if (!isMainLoop) {
				isMainLoop = true;
				MAINLOOPscene->Load();
			}
		}
	};
	auto MainUnLoad = [&]() {
		if (isMainLoop) {
			isMainLoop = false;
			MAINLOOPscene->Dispose_Load();
		}
	};

	//最初の読み込み
	TitleLoad();
	//繰り返し
	while (true) {
		scene->StartScene();
		//SetUsePixelLighting(FALSE);									//ピクセルライティングの使用(DXLIBまち)

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
			ItemLogParts->Update();
			//描画
			scene->Draw();
			ItemLogParts->Draw();
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
			TitleUnLoad();
			switch (Titlescene->SelMode()) {
				case 0:
					CustomUnLoad();
					TutorialUnLoad();
					MainLoad(false);
					break;
				case 1:
					MainUnLoad();
					TutorialUnLoad();
					CustomLoad();
					break;
				case 2:
					MainUnLoad();
					CustomUnLoad();
					TutorialLoad();
					break;
				case 3:
					CustomUnLoad();
					TutorialUnLoad();
					MainLoad(true);
					break;
				default:
					break;
			}
		}
		else if (IsReturnTitle) {
			MainUnLoad();
			CustomUnLoad();
			TutorialUnLoad();
			TitleLoad();
		}
	}
	return 0;
}
