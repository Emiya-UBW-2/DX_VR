
#include "Sub.hpp"

constexpr float BaseFrameRate = 60.f;

enum class CharaType {
	Team,
	Enemy,
};

class CharacterBase {
private:
	CharaType	m_CharaType{};
	int			m_GraphHandle{};
	int			m_WatchHandle{};

	bool		m_CanAttack{ true };
	bool		m_isDraw{ true };
	float		m_DrawAlpha{ 1.f };

	int			m_PointLightID{ -1 };
	VECTOR2D	m_Pos{};
	VECTOR2D	m_DrawPos{};
	VECTOR2D	m_AimPos{};
	VECTOR2D	m_TargetPos{};
	float		m_Rad{};
	bool		m_isTargetChanged = false;

	int										TargetPathPlanningIndex{ -1 };		// 次の中間地点となる経路上のポリゴンの経路探索情報が格納されているメモリアドレスを格納する変数
	PathChecker								m_PathChecker{};
public:
	const auto GetCharaType() const noexcept { return m_CharaType; }
	const auto GetPos() const noexcept { return m_DrawPos; }
	const auto GetRad() const noexcept { return m_Rad; }

	void SetCanAttack(bool CanAttack) noexcept {
		m_CanAttack = CanAttack;
	}
	void SetDraw(bool isDraw) noexcept {
		m_isDraw = isDraw;
	}
	void SetAimPos(VECTOR2D Pos) noexcept {
		m_AimPos = Pos;
	}
	void SetTargetPos(VECTOR2D Pos) noexcept {
		m_TargetPos = Pos;
		m_isTargetChanged = true;
	}
public:
	void Init(VECTOR2D Pos, CharaType Type) noexcept {
		auto* drawcontrol = DrawControl::Instance();
		m_Pos = Pos;
		m_DrawPos = Pos;
		SetAimPos(Pos);
		SetTargetPos(Pos);
		m_CharaType = Type;
		switch (m_CharaType) {
		case CharaType::Team:
			m_PointLightID = drawcontrol->AddPointLight(VGet(Pos.x, Pos.y, 0.f));
			m_GraphHandle = LoadGraph("data/TeamChara.bmp");
			break;
		case CharaType::Enemy:
			m_GraphHandle = LoadGraph("data/EnemyChara.bmp");
			break;
		default:
			break;
		}
		m_WatchHandle = LoadGraph("data/Watch.png");
	}
	void Update() noexcept {
		auto* drawcontrol = DrawControl::Instance();

		m_DrawAlpha = std::clamp(m_DrawAlpha + (m_isDraw ? 1.f / 0.1f : -1.f) / GetFPS(), 0.f, 1.f);

		if (m_isTargetChanged) {
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
				float speed = 0.1f * BaseFrameRate / GetFPS();
				Vec.x *= speed / Length;
				Vec.y *= speed / Length;
				m_Pos.x += Vec.x;
				m_Pos.y += Vec.y;

				m_Rad = std::atan2f(Vec.x, -Vec.y);
			}
			else {
				m_Pos = Pos;
			}
			//
			{
				VECTOR2D AimVec(m_AimPos.x - m_Pos.x, m_AimPos.y - m_Pos.y);
				float AimLength = std::hypotf(AimVec.x, AimVec.y);
				if (AimLength > 0.5f) {
					m_Rad = std::atan2f(AimVec.x, -AimVec.y);
				}
			}
		}
		//近似
		{
			float Per = (1.f - std::powf(0.9f, BaseFrameRate / GetFPS()));
			m_DrawPos.x = m_DrawPos.x + (m_Pos.x - m_DrawPos.x) * Per;
			m_DrawPos.y = m_DrawPos.y + (m_Pos.y - m_DrawPos.y) * Per;
		}
		//
		if (m_PointLightID != -1) {
			VECTOR LightPos = drawcontrol->GetPointLight(m_PointLightID);
			LightPos.x = m_DrawPos.x;
			LightPos.y = m_DrawPos.y;
			drawcontrol->SetPointLight(m_PointLightID, LightPos);
		}
	}
	void Draw() const noexcept {
		auto* drawcontrol = DrawControl::Instance();
		VECTOR EyeTargetPos = drawcontrol->GetCameraPos();
		VECTOR Pos = drawcontrol->Get2DPos(m_DrawPos.x, m_DrawPos.y, 0.f);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(128 * m_DrawAlpha));
		DrawCircleGauge((int)Pos.x, (int)Pos.y,
			static_cast<double>(m_Rad * 100.f / (DX_PI_F * 2.f) + 45.f * 100.f / 360.f),
			m_WatchHandle,
			static_cast<double>(m_Rad * 100.f / (DX_PI_F * 2.f) - 45.f * 100.f / 360.f),
			static_cast<double>(EyeTargetPos.z * 100.f / 128.f)
		);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * m_DrawAlpha));
		DrawRotaGraph((int)Pos.x, (int)Pos.y, static_cast<double>(EyeTargetPos.z), static_cast<double>(m_Rad), m_GraphHandle, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	void Dispose() noexcept {
		DeleteGraph(m_GraphHandle);
		DeleteGraph(m_WatchHandle);
		m_PathChecker.Dispose();
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

		SoftImageHandle = LoadSoftImage("data/map.bmp");

		GetSoftImageSize(SoftImageHandle, &Width, &Height);

		drawcontrol->SetPicture("data/Tex3.bmp");
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
				if (r == 0 && g == 255 && b == 0) {
					drawcontrol->SetTileData(x, y)->FloorID = 1;
				}
				else if (r == 255 && g == 0 && b == 0) {
					drawcontrol->SetTileData(x, y)->FloorID = 2;
				}
				else {
					drawcontrol->SetTileData(x, y)->FloorID = 0;
				}
			}
		}
		DeleteSoftImage(SoftImageHandle);


		//経路探索
		drawcontrol->SetupLink();
	}

	std::array<CharacterBase,4> Chara;
	Chara.at(0).Init(VECTOR2D(1.f, 1.f), CharaType::Team);
	Chara.at(1).Init(VECTOR2D(2.f, 1.f), CharaType::Team);
	Chara.at(2).Init(VECTOR2D(static_cast<float>(Width - 1), static_cast<float>(Height - 1)), CharaType::Enemy);
	Chara.at(3).Init(VECTOR2D(static_cast<float>(Width - 2), static_cast<float>(Height - 1)), CharaType::Enemy);

	drawcontrol->SetCameraPos(VGet(Chara.at(0).GetPos().x, Chara.at(0).GetPos().y, 1.f));					//視点の場所(x,y,高さ)
	VECTOR EyeTargetPos = drawcontrol->GetCameraPos();
	VECTOR EyePos = EyeTargetPos;

	bool PrevMouseLeftPress{};
	bool PrevMouseRightPress{};
	VECTOR2D MouseLeftHoldPos{};
	VECTOR2D MouseRightHoldPos{};
	//メインループ開始
	while (ProcessMessage() == 0) {
		LONGLONG WaitTime = GetNowHiPerformanceCount();//現時点の経過秒数を得る
		//カメラの位置を設定
		{
			float speed = 0.08f;
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_W) != 0) {
				EyeTargetPos.y += -speed * BaseFrameRate / GetFPS();
			}
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_S) != 0) {
				EyeTargetPos.y += speed * BaseFrameRate / GetFPS();
			}
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_A) != 0) {
				EyeTargetPos.x += -speed * BaseFrameRate / GetFPS();
			}
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_D) != 0) {
				EyeTargetPos.x += speed * BaseFrameRate / GetFPS();
			}
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_Q) != 0) {
				EyeTargetPos.x = Chara.at(0).GetPos().x;
				EyeTargetPos.y = Chara.at(0).GetPos().y;
			}
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_E) != 0) {
				EyeTargetPos.x = Chara.at(1).GetPos().x;
				EyeTargetPos.y = Chara.at(1).GetPos().y;
			}

			EyeTargetPos.x = std::clamp(EyeTargetPos.x, 0.f, static_cast<float>(Width));
			EyeTargetPos.y = std::clamp(EyeTargetPos.y, 0.f, static_cast<float>(Height));

			//カメラ拡大縮小
			int MouseWheelRot = 0;
			if (GetWindowActiveFlag()) {
				MouseWheelRot = GetMouseWheelRotVol();
			}
			else {
				GetMouseWheelRotVol();//前回との差分が入る仕組みなのでアクティブでない場合も通してはおくように
			}
			EyeTargetPos.z = std::clamp(EyeTargetPos.z + static_cast<float>(MouseWheelRot)*0.1f, 0.5f, 1.f);


			float Per = (1.f - std::powf(0.9f, BaseFrameRate / GetFPS()));
			EyePos.x = EyePos.x + (EyeTargetPos.x - EyePos.x) * Per;
			EyePos.y = EyePos.y + (EyeTargetPos.y - EyePos.y) * Per;
			EyePos.z = EyePos.z + (EyeTargetPos.z - EyePos.z) * Per;
			drawcontrol->SetCameraPos(EyePos);
		}
		for (auto& c : Chara) {
			c.Update();
		}

		for (auto& en : Chara) {
			if (en.GetCharaType() != CharaType::Enemy) { continue; }
			bool CanAttack = false;
			bool CanSee = false;
			for (auto& tm : Chara) {
				if (tm.GetCharaType() != CharaType::Team) { continue; }
				bool ThroughWay = drawcontrol->CheckPolyMoveWidth(en.GetPos(), tm.GetPos(), 0.5f);
				CanAttack |= ThroughWay;

				VECTOR2D AimVec(en.GetPos().x - tm.GetPos().x, en.GetPos().y - tm.GetPos().y);
				float AimLength = std::hypotf(AimVec.x, AimVec.y);
				if (AimLength > 1.f) {
					float Diff = tm.GetRad() - std::atan2f(AimVec.x, -AimVec.y);
					if (Diff > DX_PI_F) {
						Diff -= DX_PI_F * 2;
					}
					if (Diff < -DX_PI_F) {
						Diff += DX_PI_F * 2;
					}
					CanSee |= (std::fabsf(Diff) <= 45.f * DX_PI_F / 180.f) && ThroughWay;
				}
				else {
					CanSee |= ThroughWay;
				}

			}
			en.SetCanAttack(CanAttack);
			en.SetDraw(CanSee);
		}

		//マウス座標を取得しておく
		int MX = 0;
		int MY = 0;
		GetMousePoint(&MX, &MY);
		VECTOR MPos = drawcontrol->GetPos2D(static_cast<float>(MX), static_cast<float>(MY));
		MX = std::clamp(static_cast<int>(MPos.x + 0.5f), 0, Width);
		MY = std::clamp(static_cast<int>(MPos.y + 0.5f), 0, Height);
		VECTOR2D MousePos(static_cast<float>(MX), static_cast<float>(MY));
		//操作
		if (drawcontrol->SetTileData(MX, MY)->Height == 0) {
			int NearCharaID = -1;
			for (auto& c : Chara) {
				if (std::fabsf(MousePos.x - c.GetPos().x) < 0.5f && std::fabsf(MousePos.y - c.GetPos().y) < 0.5f) {
					NearCharaID = static_cast<int>(&c - &Chara.front());
					break;
				}
			}

			if (NearCharaID == -1 || NearCharaID == 0) {
				bool LMP = (GetWindowActiveFlag() && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0);
				if (LMP) {
					if (!PrevMouseLeftPress) {
						MouseLeftHoldPos = MousePos;
					}
					Chara.at(0).SetAimPos(MousePos);
				}
				else {
					if (PrevMouseLeftPress) {
						Chara.at(0).SetTargetPos(MouseLeftHoldPos);
					}
				}
				PrevMouseLeftPress = LMP;
			}
			if (NearCharaID == -1 || NearCharaID == 1) {
				bool RMP = (GetWindowActiveFlag() && (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0);
				if (RMP) {
					if (!PrevMouseRightPress) {
						MouseRightHoldPos = MousePos;
					}
					Chara.at(1).SetAimPos(MousePos);
				}
				else {
					if (PrevMouseRightPress) {
						Chara.at(1).SetTargetPos(MouseRightHoldPos);
					}
				}
				PrevMouseRightPress = RMP;
			}
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
			for (auto& c : Chara) {
				c.Draw();
			}
			//カーソル
			{
				if (PrevMouseLeftPress) {
					VECTOR MP = drawcontrol->Get2DPos(MouseLeftHoldPos.x, MouseLeftHoldPos.y, 0.f);
					DrawCircle((int)MP.x, (int)MP.y, 16, GetColor(0, 255, 0), false, 3);

					VECTOR MP2 = drawcontrol->Get2DPos(MousePos.x, MousePos.y, 0.f);

					if (drawcontrol->SetTileData(MX, MY)->Height == 0) {
						DrawCircle((int)MP2.x, (int)MP2.y, 8, GetColor(0, 255, 0), false, 3);
					}

					VECTOR Vec(MP2.x - MP.x, MP2.y - MP.y);
					float Length = std::hypotf(Vec.x, Vec.y);
					if (Length > 0.5f) {
						MP2.x = MP.x + Vec.x / Length * 64.f;
						MP2.y = MP.y + Vec.y / Length * 64.f;
					}
					DrawLine((int)MP.x, (int)MP.y, (int)MP2.x, (int)MP2.y, GetColor(0, 255, 0), 3);
				}
				if (PrevMouseRightPress) {
					VECTOR MP = drawcontrol->Get2DPos(MouseRightHoldPos.x, MouseRightHoldPos.y, 0.f);
					DrawCircle((int)MP.x, (int)MP.y, 16, GetColor(0, 255, 0), false, 3);

					VECTOR MP2 = drawcontrol->Get2DPos(MousePos.x, MousePos.y, 0.f);

					if (drawcontrol->SetTileData(MX, MY)->Height == 0) {
						DrawCircle((int)MP2.x, (int)MP2.y, 8, GetColor(0, 255, 0), false, 3);
					}

					VECTOR Vec(MP2.x - MP.x, MP2.y - MP.y);
					float Length = std::hypotf(Vec.x, Vec.y);
					if (Length > 0.5f) {
						MP2.x = MP.x + Vec.x / Length * 64.f;
						MP2.y = MP.y + Vec.y / Length * 64.f;
					}
					DrawLine((int)MP.x, (int)MP.y, (int)MP2.x, (int)MP2.y, GetColor(0, 255, 0), 3);
				}
				if (drawcontrol->SetTileData(MX, MY)->Height == 0) {
					if (!PrevMouseLeftPress && !PrevMouseRightPress) {
						VECTOR MP = drawcontrol->Get2DPos(MousePos.x, MousePos.y, 0.f);
						DrawCircle((int)MP.x, (int)MP.y, 16, GetColor(0, 255, 0), false, 3);
					}
				}
			}
			//デバッグ表示
			printfDx("FPS:%4.1f\n", GetFPS());
			printfDx("処理時間:%5.2fms\n", (float)(GetNowHiPerformanceCount() - WaitTime) / 1000.f);
		}
		ScreenFlip();
		clsDx();
	}
	for (auto& c : Chara) {
		c.Dispose();
	}
	DrawControl::Release();

	return 0;// ソフトの終了 
}