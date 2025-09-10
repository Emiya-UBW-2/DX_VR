
#include "Sub.hpp"

enum class EnumScene {
	Title,
	Main,
};

class TitleScene {
	int BufferHandle{ -1 };

	int MX = 0;
	int MY = 0;
public:
	void Init() noexcept {
		BufferHandle = MakeScreen(1920, 1080, false);
	}
	bool Update() noexcept {
		auto* maincontrol = DXDraw::Instance();
		//マウス座標を取得しておく
		GetMousePoint(&MX, &MY);
		MX = MX * 1920 / maincontrol->GetDispX();
		MY = MY * 1080 / maincontrol->GetDispY();

		return true;
	}
	void Draw() noexcept {
		auto* maincontrol = DXDraw::Instance();
		int PrevScreen = GetDrawScreen();
		{
			//描画
			SetDrawScreen(BufferHandle);
			ClearDrawScreen();
			{
			}
		}
		SetDrawScreen(PrevScreen);
		{
			int OffsetX = 0;
			int OffsetY = 0;
			DrawExtendGraph(OffsetX, OffsetY, maincontrol->GetDispX() + OffsetX, maincontrol->GetDispY() + OffsetY, BufferHandle, FALSE);
		}
	}
	void Dispose() noexcept {
	}
};


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	EnumScene NowScene = EnumScene::Main;

	DXDraw::Create();
	SetDefaultFontState("", 18, -1, -1, -1, 3);
	SoundPool::Create();

	std::unique_ptr<TitleScene> m_TitleScene;
	std::unique_ptr<MainScene> m_MainScene;
	while (ProcessMessage() == 0) {
		switch (NowScene) {
		case EnumScene::Title:
			m_TitleScene = std::make_unique<TitleScene>();
			m_TitleScene->Init();
			break;
		case EnumScene::Main:
			m_MainScene = std::make_unique<MainScene>();
			m_MainScene->Init();
			break;
		default:
			break;
		}
		//メインループ開始
		LONGLONG NowTime = GetNowHiPerformanceCount();//現時点の経過秒数を得る
		bool IsEnd = false;
		while (ProcessMessage() == 0) {
			LONGLONG WaitTime = GetNowHiPerformanceCount();//現時点の経過秒数を得る
			if ((WaitTime - NowTime) > (1000 * 1000 / BaseFrameRate)) {
				NowTime = WaitTime - ((WaitTime - NowTime) - (1000LL * 1000 / static_cast<LONGLONG>(BaseFrameRate)));

				switch (NowScene) {
				case EnumScene::Title:
					IsEnd = !m_TitleScene->Update();
					break;
				case EnumScene::Main:
					IsEnd = !m_MainScene->Update();
					break;
				default:
					break;
				}
			}
			SetDrawScreen(DX_SCREEN_BACK);
			ClearDrawScreen();
			{
				switch (NowScene) {
				case EnumScene::Title:
					m_TitleScene->Draw();
					break;
				case EnumScene::Main:
					m_MainScene->Draw();
					break;
				default:
					break;
				}
				//デバッグ表示
#if _DEBUG
				printfDx("FPS:%4.1f\n", GetFPS());
				printfDx("処理時間:%5.2fms\n", (float)(GetNowHiPerformanceCount() - WaitTime) / 1000.f);
#endif
			}
			ScreenFlip();
#if _DEBUG
			clsDx();
#endif
			if (IsEnd) { break; }
		}
		switch (NowScene) {
		case EnumScene::Title:
			m_TitleScene->Dispose();
			m_TitleScene.reset();

			NowScene = EnumScene::Main;
			break;
		case EnumScene::Main:
			m_MainScene->Dispose();
			m_MainScene.reset();

			NowScene = EnumScene::Title;
			break;
		default:
			break;
		}
	}
	SoundPool::Release();
	DXDraw::Release();
	return 0;// ソフトの終了 
}
