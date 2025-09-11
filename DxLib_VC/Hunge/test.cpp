
#include "Sub.hpp"

enum class EnumScene {
	Title,
	Main,
};

class TitleScene {
	int BufferHandle{ -1 };

	int Image3Handle{ -1 };
	int Image2Handle{ -1 };
	int Image1Handle{ -1 };

	int StartMX = -1;

	int MX = 0;
	int MY = 0;

	float StartTimer = 0.f;
	float EndTimer = 0.f;
	bool isEnd = false;
public:
	void Init() noexcept {
		BufferHandle = MakeScreen(1920, 1080, false);
		Image3Handle = LoadGraph("data/title3.png");
		Image2Handle = LoadGraph("data/title2.png");
		Image1Handle = LoadGraph("data/title1.png");
		StartTimer = 0.f;
		EndTimer = 0.f;
		StartMX = -1;
	}
	bool Update() noexcept {
		auto* maincontrol = DXDraw::Instance();
		//マウス座標を取得しておく
		GetMousePoint(&MX, &MY);
		MX = MX * 1920 / maincontrol->GetDispX();
		MY = MY * 1080 / maincontrol->GetDispY();
		if (StartMX == -1) {
			StartMX = MX;
		}
		StartTimer = std::min(StartTimer + 1.f / BaseFrameRate, 1.f);
		if (GetWindowActiveFlag() && CheckHitKeyAll()!=0) {
			//start
			isEnd = true;
		}
		if (isEnd) {
			EndTimer = std::min(EndTimer + 1.f / BaseFrameRate, 2.f);
			if (EndTimer >= 2.f) {
				return false;
			}
		}

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
				int OffsetX = (MX- StartMX) * 100 / 1920;
				auto prev = GetDrawMode();
				DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
				DrawExtendGraph(OffsetX * 0, 0, 1920 + OffsetX * 0, 1080 + 0, Image3Handle, TRUE);
				DrawExtendGraph(OffsetX * 1, 0, 1920 + OffsetX * 1, 1080 + 0, Image2Handle, TRUE);
				DrawExtendGraph(OffsetX * 2, 0, 1920 + OffsetX * 2, 1080 + 0, Image1Handle, TRUE);
				DxLib::SetDrawMode(prev);

				DrawBox(0, 0, OffsetX * 2, 1080, GetColor(0, 0, 0), TRUE);
				DrawBox(1920 + OffsetX * 2, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);

				char Text[64] = "Press Any Key";
				DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 1080 * 3 / 4, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
			}

			if (StartTimer < 1.f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * std::clamp(1.f - StartTimer, 0.f, 1.f)));
				DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			if (EndTimer > 0.f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * std::clamp(EndTimer, 0.f, 1.f)));
				DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
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
		DeleteGraph(BufferHandle);
		DeleteGraph(Image3Handle);
		DeleteGraph(Image2Handle);
		DeleteGraph(Image1Handle);
	}
};


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	EnumScene NowScene = EnumScene::Title;

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
