#include"Header.hpp"

#include "MainScene/Player/Player.hpp"
//
#include "Scene/TitleScene.hpp"
#include "Scene/CustomScene.hpp"
#include "Scene/TutorialScene.hpp"
#include "Scene/MainScene.hpp"

#define LineHeight	y_r(18)

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	DXDraw::Create();								//汎用
#ifdef DEBUG
	auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG
	auto* DrawParts = DXDraw::Instance();

	DrawParts->Init();
	SetMainWindowText("JW");						//タイトル
	//MV1SetLoadModelUsePackDraw(TRUE);
	//SetUseHalfLambertLighting(TRUE);	//ハーフランバート化

	PostPassEffect::Create();						//シェーダー

	FPS_n2::GetItemLog::Create();
	auto* ItemLogParts = FPS_n2::GetItemLog::Instance();

	auto* SaveDataParts = SaveDataClass::Instance();
	//初回データ作成を兼ねてセーブ
	SaveDataParts->Load();
	SaveDataParts->Save();

	//初期開放品
	SaveDataParts->SetParam("Glock17 Gen3 Barrel", 1);
	SaveDataParts->SetParam("Glock17 Gen3 Frame", 1);
	SaveDataParts->SetParam("Glock17 Gen3 Slide", 1);
	SaveDataParts->SetParam("GMag 17Round", 1);
	SaveDataParts->SetParam("G-Normal IronSight", 1);
	SaveDataParts->SetParam("X400 LaserSight", 1);
	SaveDataParts->SetParam("Universal Mount", 1);
	SaveDataParts->SetParam("VIPER RedDotSight", 1);
	SaveDataParts->SetParam("ULT Unlock", std::max(SaveDataParts->GetParam("ULT Unlock"), 1));
	SaveDataParts->SetParam("Glock Unlock", std::max(SaveDataParts->GetParam("Glock Unlock"), 1));

	SaveDataParts->Save();

	SaveDataParts->SetParam("UnlockHardMode", 1);
	//SaveDataParts->Save();

	//
	FPS_n2::Sceneclass::ObjectManager::Create();
	FPS_n2::Sceneclass::PlayerManager::Create();
	FPS_n2::Sceneclass::GunAnimManager::Create();
	//
	FPS_n2::Sceneclass::ModDataManager::Create();
	FPS_n2::Sceneclass::AmmoDataManager::Create();

	auto* BGM = BGMPool::Instance();
	auto* OptionParts = OPTION::Instance();
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
	bool isTutorialLoop = false;
	bool isMainLoop = false;

#ifdef DEBUG
#else
	std::array<float, 60> FPSAvgs;
	int m_FPSAvg = 0;
	for (auto& f : FPSAvgs) {
		f = 60.f;
	}
#endif

	//繰り返し
	while (true) {
		scene->StartScene();
		while (true) {
			DrawParts->SetStartTime();
			if ((ProcessMessage() != 0) || DrawParts->UpdateEscWindow()) { return 0; }
			FPS = std::max(GetFPS(), 30.f);
#ifdef DEBUG
			clsDx();
			DebugParts->SetStartPoint();
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("Execute start");
#endif // DEBUG
			if (scene->Execute()) { break; }		//更新
			scene->Draw();							//描画

			ItemLogParts->Update();
			ItemLogParts->Draw();
			//デバッグ
#ifdef DEBUG
			DebugParts->DebugWindow(y_r(1920 - 250), y_r(150));
			//TestDrawShadowMap(DrawParts->m_Shadow[0].GetHandle(), 0, 0, 960, 540);
#else
			{
				FPSAvgs.at(m_FPSAvg) = FPS;
				++m_FPSAvg %= ((int)FPSAvgs.size());

				float Avg = 0.f;
				for (auto& f : FPSAvgs) {
					Avg += f;
				}
				Avg = Avg / ((float)FPSAvgs.size());

				auto* Fonts = FontPool::Instance();
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, y_r(1920 - 8), y_r(8), GetColor(255, 255, 255), GetColor(0, 0, 0), "%5.2f FPS", Avg);
			}
#endif // DEBUG
			DrawParts->Screen_Flip();				//画面の反映
		}
		SaveDataClass::Instance()->Save();
		bool isTitle = (scene->GetNowScene() == Titlescene);
		if (isTitle) {
			switch (Titlescene->SelMode()) {
				case 0:
					Titlescene->Set_Next(MAINLOOPscene);
					if (MAINLOOPscene->SetPlayMode(false)) {
						isMainLoop = true;
						MAINLOOPscene->Dispose_Load();
						MAINLOOPscene->Load();
					}
					else {
						if (isTutorialLoop) {
							isTutorialLoop = false;
							Tutorialscene->Dispose_Load();
						}
						if (!isMainLoop) {
							isMainLoop = true;
							MAINLOOPscene->Load();
						}
					}
					break;
				case 1:
					Titlescene->Set_Next(Customscene);
					if (isMainLoop) {
						isMainLoop = false;
						MAINLOOPscene->Dispose_Load();
					}
					if (isTutorialLoop) {
						isTutorialLoop = false;
						Tutorialscene->Dispose_Load();
					}
					break;
				case 2:
					Titlescene->Set_Next(Tutorialscene);
					if (isMainLoop) {
						isMainLoop = false;
						MAINLOOPscene->Dispose_Load();
					}
					if (!isTutorialLoop) {
						isTutorialLoop = true;
						Tutorialscene->Load();
					}
					break;
				case 3:
					Titlescene->Set_Next(MAINLOOPscene);
					if (MAINLOOPscene->SetPlayMode(true)) {
						isMainLoop = true;
						MAINLOOPscene->Dispose_Load();
						MAINLOOPscene->Load();
					}
					else {
						if (isTutorialLoop) {
							isTutorialLoop = false;
							Tutorialscene->Dispose_Load();
						}
						if (!isMainLoop) {
							isMainLoop = true;
							MAINLOOPscene->Load();
						}
					}
					break;
				default:
					break;
			}
		}
		scene->NextScene();							//次のシーンへ移行
		if (isTitle) {
			Titlescene->Set_Next(MAINLOOPscene);//戻しとく
		}
	}
	return 0;
}
