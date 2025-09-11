
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

	int Chara3Handle{ -1 };
	int Chara2Handle{ -1 };
	int Chara1Handle{ -1 };

	int StartMX = -1;

	int MX = 0;
	int MY = 0;

	float StartTimer = 0.f;
	float EndTimer = 0.f;
	bool isEnd = false;

	bool isDrawHint = false;
	float DrawHint = 0.f;

	bool PrevF1 = false;

	bool PrevMouseLeftPress{};
public:
	void Init() noexcept {
		BufferHandle = MakeScreen(1920, 1080, false);
		Image3Handle = LoadGraph("data/title3.png");
		Image2Handle = LoadGraph("data/title2.png");
		Image1Handle = LoadGraph("data/title1.png");

		Chara3Handle = LoadGraph("data/TeamChara.bmp");
		Chara2Handle = LoadGraph("data/TeamCharaNPC.bmp");
		Chara1Handle = LoadGraph("data/EnemyChara.bmp");

		StartTimer = 0.f;
		EndTimer = 0.f;
		StartMX = -1;
	}
	bool Update() noexcept {
		auto* maincontrol = DXDraw::Instance();
		auto* rule = Rule::Instance();
		//マウス座標を取得しておく
		GetMousePoint(&MX, &MY);
		MX = MX * 1920 / maincontrol->GetDispX();
		MY = MY * 1080 / maincontrol->GetDispY();
		if (StartMX == -1) {
			StartMX = MX;
		}
		StartTimer = std::min(StartTimer + 1.f / BaseFrameRate, 1.f);

		DrawHint = std::clamp(DrawHint + (isDrawHint ? 1.f : -1.f) * 1.f / BaseFrameRate/0.1f, 0.f, 1.f);
		if (GetWindowActiveFlag()) {
			bool F1 = CheckHitKey(KEY_INPUT_F1) != 0;
			if (F1 && !PrevF1) {
				isDrawHint ^= 1;
			}
			PrevF1 = F1;
			if (!F1 && CheckHitKeyAll(DX_CHECKINPUT_KEY) != 0) {
				//start
				isEnd = true;
			}
			if (!isEnd) {
				bool LMP = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
				if (LMP && !PrevMouseLeftPress) {
					{
						{
							int X1 = 1920 / 2 + 700;
							int Y1 = 1080 * 3 / 4;
							bool InMouse = (X1 <= MX && MX <= X1 + 64 && Y1 <= MY && MY <= Y1 + 48);
							if (InMouse) {
								rule->SetPlayerAll(rule->GetPlayerAll() + 2);
								rule->SetPlayablePlayer(rule->GetPlayablePlayer());
							}
						}
						{
							int X1 = 1920 / 2 + 700;
							int Y1 = 1080 * 3 / 4 + 128;
							bool InMouse = (X1 <= MX && MX <= X1 + 64 && Y1 <= MY && MY <= Y1 + 48);
							if (InMouse) {
								rule->SetPlayerAll(rule->GetPlayerAll() - 2);
								rule->SetPlayablePlayer(rule->GetPlayablePlayer());
							}
						}

						{
							int X1 = 1920 / 2 + 800;
							int Y1 = 1080 * 3 / 4;
							bool InMouse = (X1 <= MX && MX <= X1 + 64 && Y1 <= MY && MY <= Y1 + 48);
							if (InMouse) {
								rule->SetPlayablePlayer(rule->GetPlayablePlayer() + 1);
							}
						}
						{
							int X1 = 1920 / 2 + 800;
							int Y1 = 1080 * 3 / 4 + 128;
							bool InMouse = (X1 <= MX && MX <= X1 + 64 && Y1 <= MY && MY <= Y1 + 48);
							if (InMouse) {
								rule->SetPlayablePlayer(rule->GetPlayablePlayer() - 1);
							}
						}
					}
				}
				PrevMouseLeftPress = LMP;
			}
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
		auto* rule = Rule::Instance();
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
				{
					char Text[64] = "Press Any Key";
					DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, 1080 * 3 / 4, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
				}
				{
					char Text[64] = "Press F1 Key Information";
					DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, 1080 * 3 / 4 + 24, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
				}
				{
					bool InPlayerAll = false;
					bool InPlayablePlayer = false;
					{
						int X1 = 1920 / 2 + 700;
						int Y1 = 1080 * 3 / 4;
						bool InMouse = (X1 <= MX && MX <= X1 + 64 && Y1 <= MY && MY <= Y1 + 48);
						InPlayerAll |= InMouse;
						DrawTriangle(X1, Y1 + 48, X1 + 64 / 2, Y1, X1 + 64, Y1 + 48, InMouse ? GetColor(255, 255, 255) : GetColor(0, 0, 0), TRUE);
					}
					{
						int X1 = 1920 / 2 + 700 + 64 / 2;
						int Y1 = 1080 * 3 / 4 + 128 / 2 - 18 / 2 + 48 / 2;
						char Text[64] = "";
						sprintfDx(Text, "%d", rule->GetPlayerAll());
						DrawString(X1 - GetDrawStringWidth(Text, sizeof(Text)) / 2, Y1, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
					}
					{
						int X1 = 1920 / 2 + 700;
						int Y1 = 1080 * 3 / 4 + 128;
						bool InMouse = (X1 <= MX && MX <= X1 + 64 && Y1 <= MY && MY <= Y1 + 48);
						InPlayerAll |= InMouse;
DrawTriangle(X1, Y1, X1 + 64 / 2, Y1 + 48, X1 + 64, Y1, InMouse ? GetColor(255, 255, 255) : GetColor(0, 0, 0), TRUE);
					}

					{
						int X1 = 1920 / 2 + 800;
						int Y1 = 1080 * 3 / 4;
						bool InMouse = (X1 <= MX && MX <= X1 + 64 && Y1 <= MY && MY <= Y1 + 48);
						InPlayablePlayer |= InMouse;
						DrawTriangle(X1, Y1 + 48, X1 + 64 / 2, Y1, X1 + 64, Y1 + 48, InMouse ? GetColor(255, 255, 255) : GetColor(0, 0, 0), TRUE);
					}
					{
						int X1 = 1920 / 2 + 800 + 64 / 2;
						int Y1 = 1080 * 3 / 4 + 128 / 2 - 18 / 2 + 48 / 2;
						char Text[64] = "";
						sprintfDx(Text, "%d", rule->GetPlayablePlayer());
						DrawString(X1 - GetDrawStringWidth(Text, sizeof(Text)) / 2, Y1, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
					}
					{
						int X1 = 1920 / 2 + 800;
						int Y1 = 1080 * 3 / 4 + 128;
						bool InMouse = (X1 <= MX && MX <= X1 + 64 && Y1 <= MY && MY <= Y1 + 48);
						InPlayablePlayer |= InMouse;
						DrawTriangle(X1, Y1, X1 + 64 / 2, Y1 + 48, X1 + 64, Y1, InMouse ? GetColor(255, 255, 255) : GetColor(0, 0, 0), TRUE);
					}

					if (InPlayerAll) {
						int X1 = 1920 - 64;
						int Y1 = 1080 * 3 / 4 + 192;
						char Text[64] = "総プレイヤー数を変更する";
						DrawString(X1 - GetDrawStringWidth(Text, sizeof(Text)), Y1, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
					}
					if (InPlayablePlayer) {
						int X1 = 1920 - 64;
						int Y1 = 1080 * 3 / 4 + 192;
						char Text[64] = "操作可能なプレイヤー数を変更する";
						DrawString(X1 - GetDrawStringWidth(Text, sizeof(Text)), Y1, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
					}
				}
			}
			if (DrawHint > 0.f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(64 * std::clamp(DrawHint, 0.f, 1.f)));
				DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				if (DrawHint >= 0.95f) {
					int YPos = 1080 * 1 / 3;
					{
						char Text[256] = "このゲームはエアソフトガンで撃ち合うサバイバルゲームをモチーフにしたゲームです。";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, YPos, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
						YPos += 24;
					}
					{
						char Text[256] = "4対4で敵チームをせん滅すれば勝利となります。";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, YPos, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
						YPos += 24;
					}
					YPos += 24;
					{
						char Text[256] = "プレイヤーはマウスクリックで味方の移動を指示し、勝利を目指します。";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, YPos, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
						YPos += 24;
					}
					{
						char Text[256] = "左クリックを押すとその地点に進みます。";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, YPos, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
						YPos += 24;
					}
					{
						char Text[256] = "左クリックを押し続けている間、移動中に向く方向を指定することができます。";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, YPos, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
						YPos += 24;
					}
					{
						char Text[256] = "方向を指定することで平行移動したり後方を警戒しながら進行することができます。";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, YPos, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
						YPos += 24;
					}
					{
						char Text[256] = "左クリック中に右クリックを押すと指示をキャンセルできます。";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, YPos, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
						YPos += 24;
					}
					YPos += 24;
					{
						char Text[256] = "操作できるキャラはこの色です。→";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, YPos, Text, GetColor(181, 230, 29), GetColor(0, 0, 0));

						DrawRotaGraph(1920 / 2 + GetDrawStringWidth(Text, sizeof(Text)) / 2 + 64, YPos + 18 / 2, 1.0, 0.0, Chara3Handle, true);
						YPos += 24;
					}
					YPos += 24;
					{
						char Text[256] = "自立移動する味方はこの色です。→";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, YPos, Text, GetColor(0, 162, 232), GetColor(0, 0, 0));

						DrawRotaGraph(1920 / 2 + GetDrawStringWidth(Text, sizeof(Text)) / 2 + 64, YPos + 18 / 2, 1.0, 0.0, Chara2Handle, true);
						YPos += 24;
					}
					YPos += 24;
					{
						char Text[256] = "敵対するキャラはこの色です。→";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, sizeof(Text)) / 2, YPos, Text, GetColor(237, 28, 36), GetColor(0, 0, 0));

						DrawRotaGraph(1920 / 2 + GetDrawStringWidth(Text, sizeof(Text)) / 2 + 64, YPos + 18 / 2, 1.0, 0.0, Chara1Handle, true);
						YPos += 24;
					}
					YPos += 24;
				}
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

		DeleteGraph(Chara3Handle);
		DeleteGraph(Chara2Handle);
		DeleteGraph(Chara1Handle);

		BufferHandle = -1;
		Image3Handle = -1;
		Image2Handle = -1;
		Image1Handle = -1;

		Chara3Handle = -1;
		Chara2Handle = -1;
		Chara1Handle = -1;
	}
};


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	EnumScene NowScene = EnumScene::Title;

	DXDraw::Create();
	SetDefaultFontState("", 18, -1, -1, -1, 3);
	SoundPool::Create();
	Rule::Create();

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
	Rule::Release();
	SoundPool::Release();
	DXDraw::Release();
	return 0;// ソフトの終了 
}
