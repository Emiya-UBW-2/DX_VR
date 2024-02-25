#include"Header.hpp"

#include "MainScene/Player/Player.hpp"
//
#include "Scene/TitleScene.hpp"
#include "Scene/CustomScene.hpp"
#include "Scene/TutorialScene.hpp"
#include "Scene/MainScene.hpp"


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	DXDraw::Create();								//汎用
	SetMainWindowText("JW");						//タイトル
	//MV1SetLoadModelUsePackDraw(TRUE);
	PostPassEffect::Create();						//シェーダー
#ifdef DEBUG
	auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG
	auto* DrawParts = DXDraw::Instance();
	//
	FPS_n2::Sceneclass::ObjectManager::Create();
	FPS_n2::Sceneclass::PlayerManager::Create();
	FPS_n2::Sceneclass::GunAnimManager::Create();
	//
	FPS_n2::Sceneclass::ModDataManager::Create();
	FPS_n2::Sceneclass::AmmoDataManager::Create();
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
	//繰り返し
	while (true) {
		scene->StartScene();
		while (true) {
			if ((ProcessMessage() != 0) || (CheckHitKeyWithCheck(KEY_INPUT_ESCAPE) != 0)) { return 0; }
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
			//デバッグ
#ifdef DEBUG
			DebugParts->DebugWindow(1920 - 300, 50);
			//TestDrawShadowMap(DrawParts->m_Shadow[0].GetHandle(), 0, 0, 960, 540);
#else
			{
				auto* Fonts = FontPool::Instance();
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, y_r(1920 - 8), y_r(8), GetColor(255, 255, 255), GetColor(0, 0, 0), "%5.2f FPS", FPS);
			}
#endif // DEBUG
			DrawParts->Screen_Flip();				//画面の反映
		}
		SaveDataClass::Instance()->Save();
		bool isTitle = (scene->GetNowScene() == Titlescene);
		if (isTitle) {
			switch (Titlescene->SelMode()) {
				case 0:
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
				case 1:
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
				case 2:
					Titlescene->Set_Next(MAINLOOPscene);
					if (isTutorialLoop) {
						isTutorialLoop = false;
						Tutorialscene->Dispose_Load();
					}
					if (!isMainLoop) {
						isMainLoop = true;
						MAINLOOPscene->Load();
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
