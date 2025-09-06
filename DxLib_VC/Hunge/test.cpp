
#include "Sub.hpp"

class CharacterBase {
private:
	int			m_PointLightID;
	VECTOR2D	m_Pos;
	VECTOR2D	m_DrawPos;
	VECTOR2D	m_TargetPos;
	bool		m_isTargetChanged = false;

	int										TargetPathPlanningIndex{ -1 };		// 次の中間地点となる経路上のポリゴンの経路探索情報が格納されているメモリアドレスを格納する変数
	PathChecker								m_PathChecker;
public:
	void SetTargetPos(VECTOR2D Pos) noexcept {
		m_TargetPos = Pos;
		m_isTargetChanged = true;
	}
public:
	void Init(VECTOR2D Pos) noexcept {
		auto* drawcontrol = DrawControl::Instance();
		m_PointLightID = drawcontrol->AddPointLight(VGet(Pos.x, Pos.y, 0.f));
		m_Pos = Pos;
		m_DrawPos = Pos;
		SetTargetPos(Pos);
	}
	void Update() noexcept {
		auto* drawcontrol = DrawControl::Instance();

		float speed = 0.1f;
		if (CheckHitKey(KEY_INPUT_W) != 0) {
			m_Pos.y += -speed * 60.f / GetFPS();
		}
		if (CheckHitKey(KEY_INPUT_S) != 0) {
			m_Pos.y += speed * 60.f / GetFPS();
		}
		if (CheckHitKey(KEY_INPUT_A) != 0) {
			m_Pos.x += -speed * 60.f / GetFPS();
		}
		if (CheckHitKey(KEY_INPUT_D) != 0) {
			m_Pos.x += speed * 60.f / GetFPS();
		}
		if(m_isTargetChanged){
			m_isTargetChanged = false;
			//経路探索しなおす
			m_PathChecker.Dispose();
			if (m_PathChecker.Init(m_Pos, m_TargetPos)) {	// 指定の２点の経路情報を探索する
				TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetPolyIndex();	// 移動開始時点の移動中間地点の経路探索情報もスタート地点にあるポリゴンの情報
			}
		}
		//経路に沿って移動
		{
			VECTOR2D Pos = m_PathChecker.GetNextPoint(m_Pos, &TargetPathPlanningIndex);
			VECTOR2D Vec(Pos.x - m_Pos.x, Pos.y - m_Pos.y);
			float Length = std::hypotf(Vec.x, Vec.y);
			if (Length > 0.5f) {
				float speed = 0.1f * 60.f / GetFPS();
				Vec.x *= speed / Length;
				Vec.y *= speed / Length;
				m_Pos.x += Vec.x;
				m_Pos.y += Vec.y;
			}
			else {
				m_Pos = Pos;
			}
		}
		//近似
		{
			float Per = (1.f - std::powf(0.9f, 60.f / GetFPS()));
			m_DrawPos.x = m_DrawPos.x + (m_Pos.x - m_DrawPos.x) * Per;
			m_DrawPos.y = m_DrawPos.y + (m_Pos.y - m_DrawPos.y) * Per;
		}
		//
		VECTOR LightPos = drawcontrol->GetPointLight(m_PointLightID);
		LightPos.x = m_DrawPos.x;
		LightPos.y = m_DrawPos.y;
		drawcontrol->SetPointLight(m_PointLightID, LightPos);
	}
	void Draw() const noexcept {
		auto* drawcontrol = DrawControl::Instance();
		VECTOR Pos = drawcontrol->Get2DPos(m_DrawPos.x, m_DrawPos.y, 0.f);
		DrawCircle((int)Pos.x, (int)Pos.y, 16, GetColor(255, 255, 0));
	}
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	auto maincontrol = std::make_unique<DXDraw>();
	DrawControl::Create(maincontrol->GetDispX(), maincontrol->GetDispY());
	auto* drawcontrol = DrawControl::Instance();
	//設定
	//マップデータ作成
	int Width = 24;
	int Height = 16;
	{
		int SoftImageHandle = -1;

		SoftImageHandle = LoadSoftImage("map.bmp");

		GetSoftImageSize(SoftImageHandle, &Width, &Height);

		drawcontrol->SetPicture("Tex3.bmp");
		drawcontrol->SetCameraPos(VGet(static_cast<float>(Width / 2), static_cast<float>(Height / 2), 12.f));					//視点の場所(x,y,高さ)
		drawcontrol->SetMapSize(Width, Height);
		for (int x = 0; x < Width; x++) {
			for (int y = 0; y < Height; y++) {
				int r, g, b;
				GetPixelSoftImage(SoftImageHandle, x, y, &r, &g, &b, NULL);
				if (r == 0 && g == 0 && b == 0) {
					drawcontrol->SetTileData(x, y)->Height = 8;
				}
				else {
					drawcontrol->SetTileData(x, y)->Height = 0;
				}
				drawcontrol->SetTileData(x, y)->FloorID = 0;
			}
		}
		DeleteSoftImage(SoftImageHandle);


		//経路探索
		{
			drawcontrol->StartCount();
			for (int x = 0; x < Width; x++) {
				for (int y = 0; y < Height; y++) {
					if (drawcontrol->SetTileData(x, y)->Height == 0) {
						auto& bu = drawcontrol->AddWayPoint(VECTOR2D((float)x - 0.5f, (float)y - 0.5f), VECTOR2D((float)(x + 0.5f), (float)(y + 0.5f)));
						if (drawcontrol->SetTileData(x + 1, y)->Height == 0) {
							bu.SetLinkBuffer(0, x + 1, y);
						}
						if (drawcontrol->SetTileData(x, y + 1)->Height == 0) {
							bu.SetLinkBuffer(1, x, y + 1);
						}
						if (drawcontrol->SetTileData(x - 1, y)->Height == 0) {
							bu.SetLinkBuffer(2, x - 1, y);
						}
						if (drawcontrol->SetTileData(x, y - 1)->Height == 0) {
							bu.SetLinkBuffer(3, x, y - 1);
						}
					}
				}
			}
			//リンクを確立
			drawcontrol->Setup();
		}
	}

	CharacterBase Chara1;
	Chara1.Init(VECTOR2D(5.f, 4.f));

	CharacterBase Chara2;
	Chara2.Init(VECTOR2D(6.f, 4.f));


	//メインループ開始
	while (ProcessMessage() == 0) {
		LONGLONG WaitTime = GetNowHiPerformanceCount();//現時点の経過秒数を得る
		//カメラの位置を設定
		{
			VECTOR EyePos = drawcontrol->GetCameraPos();
			float speed = 0.08f;
			if (CheckHitKey(KEY_INPUT_UP) != 0) {
				EyePos.y += -speed * 60.f / GetFPS();
			}
			if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
				EyePos.y += speed * 60.f / GetFPS();
			}
			if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
				EyePos.x += -speed * 60.f / GetFPS();
			}
			if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
				EyePos.x += speed * 60.f / GetFPS();
			}
			EyePos.x = std::clamp(EyePos.x, 0.f, static_cast<float>(Width));
			EyePos.y = std::clamp(EyePos.y, 0.f, static_cast<float>(Height));
			drawcontrol->SetCameraPos(EyePos);
		}
		Chara1.Update();
		Chara2.Update();
		//マウス座標を取得しておく
		int MX = 0;
		int MY = 0;
		GetMousePoint(&MX, &MY);
		VECTOR MPos = drawcontrol->GetPos2D(static_cast<float>(MX), static_cast<float>(MY));
		MX = std::clamp(static_cast<int>(MPos.x + 0.5f), 0, Width);
		MY = std::clamp(static_cast<int>(MPos.y + 0.5f), 0, Height);
		VECTOR2D MousePos(static_cast<float>(MX), static_cast<float>(MY));
		//操作
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			Chara1.SetTargetPos(MousePos);
		}
		if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) {
			Chara2.SetTargetPos(MousePos);
		}
		//
		drawcontrol->Execute();
		//描画
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			//背景
			drawcontrol->Draw(maincontrol->GetDispX(), maincontrol->GetDispY());
			//キャラ
			Chara1.Draw();
			Chara2.Draw();
			//カーソル
			if (drawcontrol->SetTileData(MX, MY)->Height == 0) {
				VECTOR MP = drawcontrol->Get2DPos(MousePos.x, MousePos.y, 0.f);
				DrawCircle((int)MP.x, (int)MP.y, 16, GetColor(255, 0, 0), false, 3);
			}
			//デバッグ表示
			clsDx();
			printfDx("FPS:%4.1f\n", GetFPS());
			printfDx("処理時間:%5.2fms\n", (float)(GetNowHiPerformanceCount() - WaitTime) / 1000.f);
		}
		ScreenFlip();
	}
	DrawControl::Release();

	return 0;// ソフトの終了 
}