#include"Header.hpp"

#include "Scene/MainScene.hpp"
#include "Scene/MainSceneLoader.hpp"

#ifdef DEBUG
int DBG_CamSel = -1;
#endif

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	DXDraw::Create();						//汎用
	auto* DrawParts = DXDraw::Instance();

	DrawParts->Init();
	if (DrawParts->IsFirstBoot()) {
		return 0;
	}
#ifdef DEBUG
	auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG
	SetMainWindowText("Phantom of the Bunker");						//タイトル
	//
	FPS_n2::Sceneclass::WeaponAnimManager::Create();
	FPS_n2::Sceneclass::PlayerManager::Create();
	FPS_n2::Sceneclass::CommonBattleResource::Create();
	//BG
	FPS_n2::Sceneclass::WeaponAnimManager::Instance()->Load("data/CharaAnime/");
	//シーン
	auto MAINLOOPLOADERscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOPLOADER>();
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	//シーンコントロール
	auto scene = std::make_unique<SceneControl>(MAINLOOPLOADERscene);
	//遷移先指定
	MAINLOOPLOADERscene->Set_Next(MAINLOOPscene);
	MAINLOOPscene->Set_Next(MAINLOOPLOADERscene);
	//最初の読み込み
	scene->GetNowScene()->Load();
	//繰り返し
	while (true) {
		scene->StartScene();
		while (true) {
			if (!DrawParts->FirstExecute()) {
				return 0;
			}
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
		scene->NextScene();							//次のシーンへ移行
	}
	return 0;
}
