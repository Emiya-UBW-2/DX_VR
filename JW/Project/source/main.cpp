#include"Header.hpp"

#include "MainScene/Player/Player.hpp"
//
#include "Scene/TitleScene.hpp"
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
	FPS_n2::SaveDataClass::Create();
	FPS_n2::Sceneclass::ObjectManager::Create();
	FPS_n2::Sceneclass::PlayerManager::Create();
	FPS_n2::Sceneclass::GunAnimManager::Create();
	//
	FPS_n2::Sceneclass::GunDataManager::Create();
	FPS_n2::Sceneclass::ModDataManager::Create();
	FPS_n2::Sceneclass::AmmoDataManager::Create();
	//シーン
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	//シーンコントロール
	auto scene = std::make_unique<SceneControl>(Titlescene);
	//遷移先指定
	Titlescene->Set_Next(MAINLOOPscene);
	MAINLOOPscene->Set_Next(Titlescene);
	//繰り返し
	while (true) {
		scene->StartScene();
		while (true) {
			if ((ProcessMessage() != 0) || (CheckHitKeyWithCheck(KEY_INPUT_ESCAPE) != 0)) { return 0; }
			FPS = GetFPS();
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
#else
			{
				auto* Fonts = FontPool::Instance();
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, y_r(1920 - 8), y_r(8), GetColor(255, 255, 255), GetColor(0, 0, 0), "%5.2f FPS", FPS);
			}
#endif // DEBUG
			DrawParts->Screen_Flip();				//画面の反映
		}
		FPS_n2::SaveDataClass::Instance()->Save();
		scene->NextScene();							//次のシーンへ移行
	}
	return 0;
}
