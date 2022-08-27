#include"Header.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	OPTION::Create();
	auto* OptionParts = OPTION::Instance();
	OptionParts->Load();																										//設定読み込み
	DXDraw::Create(("Realy3D_" + std::to_string(GetNowHiPerformanceCount())).c_str(), FPS_n2::Frame_Rate);						//汎用
	FontPool::Create();
	LightPool::Create();
	//MV1SetLoadModelUsePackDraw(TRUE);
#ifdef DEBUG
	DebugClass::Create(FPS_n2::Frame_Rate);
#endif // DEBUG
	OptionParts->Set_useVR(DXDraw::Instance()->use_vr);
	auto scene = std::make_unique<FPS_n2::Sceneclass::SceneControl>();															//シーンコントロール
	PostPassEffect::Create();																									//シェーダー

	SoundPool::Create();
	FPS_n2::Sceneclass::ObjectManager::Create();
	FPS_n2::Sceneclass::PlayerManager::Create();

	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();														//シーン
	EffectControl::Create();

	auto* EffectUseControl = EffectControl::Instance();

	EffectUseControl->Init();																									//エフェクト
	MAINLOOPscene->Set_Next(MAINLOOPscene, FPS_n2::Sceneclass::scenes::MAIN_LOOP);												//遷移先指定
	scene->ChangeScene(FPS_n2::Sceneclass::scenes::MAIN_LOOP, (std::shared_ptr<FPS_n2::Sceneclass::TEMPSCENE>&)MAINLOOPscene);	//開始時遷移先
	do {
		scene->StartScene();																									//開始処理
		while (true) {
			if (scene->Execute()) { break; }																					//更新処理
			if (!scene->isPause()) {
				EffectUseControl->Calc();																						//エフェクシアのアプデを60FPS相当に変更
			}
			scene->Draw();																										//描画
			scene->Vsync();																										//FPS待機
		}
		scene->NextScene();																										//終了処理
	} while (!scene->isEnd());
	EffectUseControl->Dispose();
	return 0;
}
