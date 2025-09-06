
#include "Sub.hpp"

constexpr float BaseFrameRate = 60.f;

enum class CharaType {
	Team,
	TeamNPC,
	Enemy,
};

class CharacterBase {
private:
	CharaType	m_CharaType{};
	int			m_GraphHandle{};
	int			m_WatchHandle{};

	bool		m_CanAttack{ true };
	bool		m_isDraw{ true };

	bool		m_isMoving{ false };
	float		m_DrawAlpha{ 1.f };

	size_t		m_PointLightID{ SIZE_MAX };
	VECTOR2D	m_Pos{};
	VECTOR2D	m_DrawPos{};
	VECTOR2D	m_AimPos{};
	VECTOR2D	m_TargetPos{};
	float		m_Rad{};
	float		m_DrawRad{};
	bool		m_isTargetChanged = false;

	int										TargetPathPlanningIndex{ -1 };		// 次の中間地点となる経路上のポリゴンの経路探索情報が格納されているメモリアドレスを格納する変数
	PathChecker								m_PathChecker{};

	VECTOR2D	m_FogMoveRandom{};
	float		m_MoveTimer = 0.f;
public:
	std::array<int, 5>	m_WayPointList{};
public:
	const auto GetCharaType() const noexcept { return m_CharaType; }
	const auto GetPos() const noexcept { return m_DrawPos; }
	const auto GetRad() const noexcept { return m_DrawRad; }
	const auto IsMoving() const noexcept { return m_isMoving; }

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
	void Init(VECTOR2D Pos, float Yrad, CharaType Type) noexcept {
		auto* drawcontrol = DrawControl::Instance();
		m_Pos = Pos;
		m_DrawPos = Pos;

		m_Rad = Yrad;
		m_DrawRad = Yrad;
		SetAimPos(Pos);
		SetTargetPos(Pos);
		m_CharaType = Type;
		switch (m_CharaType) {
		case CharaType::Team:
			m_PointLightID = drawcontrol->AddPointLight(VGet(Pos.x, Pos.y, 0.f));
			m_GraphHandle = LoadGraph("data/TeamChara.bmp");
			break;
		case CharaType::TeamNPC:
			m_GraphHandle = LoadGraph("data/TeamCharaNPC.bmp");
			break;
		case CharaType::Enemy:
			m_GraphHandle = LoadGraph("data/EnemyChara.bmp");
			break;
		default:
			break;
		}
		m_WatchHandle = LoadGraph("data/Watch.png");

		for (auto& w : m_WayPointList) {
			w = -1;
		}
		m_isMoving = false;
	}
	void Update() noexcept {
		auto* drawcontrol = DrawControl::Instance();

		m_DrawAlpha = std::clamp(m_DrawAlpha + (m_isDraw ? 1.f / 0.1f : -1.f) / BaseFrameRate, 0.f, 1.f);

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
				float speed = 0.1f;
				Vec.x *= speed / Length;
				Vec.y *= speed / Length;
				m_Pos.x += Vec.x;
				m_Pos.y += Vec.y;

				m_Rad = std::atan2f(Vec.x, -Vec.y);

				m_isMoving = true;
			}
			else {
				m_Pos = Pos;
				m_isMoving = false;
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
			float Per = (1.f - 0.9f);
			m_DrawPos.x = m_DrawPos.x + (m_Pos.x - m_DrawPos.x) * Per;
			m_DrawPos.y = m_DrawPos.y + (m_Pos.y - m_DrawPos.y) * Per;

			//m_isMoving = std::hypotf(m_Pos.x - m_DrawPos.x, m_Pos.y - m_DrawPos.y) >= 0.1f;

			float Diff = m_DrawRad - m_Rad;
			if (Diff > DX_PI_F) {
				Diff -= DX_PI_F * 2;
			}
			if (Diff < -DX_PI_F) {
				Diff += DX_PI_F * 2;
			}
			if (Diff > 1.f * DX_PI_F / 180.f) {
				m_DrawRad -= 3.f * DX_PI_F / 180.f;
			}
			else if (Diff < -1.f * DX_PI_F / 180.f) {
				m_DrawRad += 3.f * DX_PI_F / 180.f;
			}

			if (m_DrawRad > DX_PI_F) {
				m_DrawRad -= DX_PI_F * 2;
			}
			if (m_DrawRad < -DX_PI_F) {
				m_DrawRad += DX_PI_F * 2;
			}
		}
		//
		if (m_PointLightID != SIZE_MAX) {
			VECTOR LightPos = drawcontrol->GetPointLight(m_PointLightID);
			LightPos.x = m_DrawPos.x;
			LightPos.y = m_DrawPos.y;
			drawcontrol->SetPointLight(m_PointLightID, LightPos);
		}
		//ランダム形成
		if (m_MoveTimer == 0.f) {
			if (!m_isDraw && IsMoving()) {
				m_MoveTimer = 0.5f;
				m_FogMoveRandom.x = m_DrawPos.x + static_cast<float>(GetRand(200) - 100) / 100.f * 3.f;
				m_FogMoveRandom.y = m_DrawPos.y + static_cast<float>(GetRand(200) - 100) / 100.f * 3.f;

				m_FogMoveRandom.x = std::clamp(m_FogMoveRandom.x, 0.f, static_cast<float>(drawcontrol->GetMapXsize()));
				m_FogMoveRandom.y = std::clamp(m_FogMoveRandom.y, 0.f, static_cast<float>(drawcontrol->GetMapYsize()));
			}
		}
		else {
			m_MoveTimer = std::max(m_MoveTimer - 1.f / BaseFrameRate, 0.f);
		}
	}
	void Draw() const noexcept {
		auto* drawcontrol = DrawControl::Instance();
		VECTOR EyeTargetPos = drawcontrol->GetCameraPos();
		VECTOR Pos = drawcontrol->Get2DPos(m_DrawPos.x, m_DrawPos.y, 0.f);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(128 * m_DrawAlpha));
		SetDrawBright(150, 200, 255);
		DrawCircleGauge((int)Pos.x, (int)Pos.y,
			static_cast<double>(m_DrawRad * 100.f / (DX_PI_F * 2.f) + 45.f * 100.f / 360.f),
			m_WatchHandle,
			static_cast<double>(m_DrawRad * 100.f / (DX_PI_F * 2.f) - 45.f * 100.f / 360.f),
			static_cast<double>(EyeTargetPos.z * 100.f / 128.f)
		);
		SetDrawBright(255, 255, 255);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * m_DrawAlpha));
		DrawRotaGraph((int)Pos.x, (int)Pos.y, static_cast<double>(EyeTargetPos.z), static_cast<double>(m_DrawRad), m_GraphHandle, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		if (m_MoveTimer > 0.f) {
			VECTOR Pos2 = drawcontrol->Get2DPos(m_FogMoveRandom.x, m_FogMoveRandom.y, 0.f);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(128 * m_MoveTimer / 0.5f));
			DrawCircle((int)Pos2.x, (int)Pos2.y, (int)(32.f * (0.5f - m_MoveTimer) / 0.5f), GetColor(192, 192, 192), FALSE, 2);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
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
		Width /= 2;
		Height /= 2;

		drawcontrol->SetPicture("data/Tex3.bmp");
		drawcontrol->SetMapSize(Width, Height);
		for (int x = 0; x < Width; x++) {
			for (int y = 0; y < Height; y++) {
				int r, g, b;
				GetPixelSoftImage(SoftImageHandle, x * 2, y * 2, &r, &g, &b, NULL);
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
					drawcontrol->SetTileData(x, y)->FloorID = (GetRand(100) > 50) ? 3 : 0;
				}
				drawcontrol->SetTileData(x, y)->WayPointRad = -1;
				if ((r == 128 && g == 128 && b == 128) || (r == 192 && g == 192 && b == 192)) {
					drawcontrol->SetTileData(x, y)->WayPointRad = 0;
					int Color[2][2] = {};
					for (int xl = 0; xl < 2; xl++) {
						for (int yl = 0; yl < 2; yl++) {
							GetPixelSoftImage(SoftImageHandle, x * 2 + xl, y * 2 + yl, &r, &g, &b, NULL);
							if (r == 192 && g == 192 && b == 192) {
								Color[xl][yl] = 0;
							}
							if (r == 128 && g == 128 && b == 128) {
								Color[xl][yl] = 1;
							}
						}
					}
					if (Color[0][0] == 0 && Color[1][0] == 0) {
						drawcontrol->SetTileData(x, y)->WayPointRad = 180;
					}
					if (Color[1][0] == 0 && Color[1][1] == 0) {
						drawcontrol->SetTileData(x, y)->WayPointRad = 90;
					}
					if (Color[1][1] == 0 && Color[0][1] == 0) {
						drawcontrol->SetTileData(x, y)->WayPointRad = 0;
					}
					if (Color[0][1] == 0 && Color[0][0] == 0) {
						drawcontrol->SetTileData(x, y)->WayPointRad = -90;
					}
				}
			}
		}
		DeleteSoftImage(SoftImageHandle);


		//経路探索
		drawcontrol->SetupLink();
	}

	std::array<CharacterBase, 8> Chara;
	Chara.at(0).Init(VECTOR2D(0.f, 0.f), DX_PI_F, CharaType::Team);
	Chara.at(1).Init(VECTOR2D(1.f, 0.f), DX_PI_F, CharaType::Team);
	Chara.at(2).Init(VECTOR2D(2.f, 0.f), DX_PI_F, CharaType::TeamNPC);
	Chara.at(3).Init(VECTOR2D(3.f, 0.f), DX_PI_F, CharaType::TeamNPC);
	Chara.at(4).Init(VECTOR2D(static_cast<float>(Width - 1 - 0), static_cast<float>(Height - 1)), 0.f, CharaType::Enemy);
	Chara.at(5).Init(VECTOR2D(static_cast<float>(Width - 1 - 1), static_cast<float>(Height - 1)), 0.f, CharaType::Enemy);
	Chara.at(6).Init(VECTOR2D(static_cast<float>(Width - 1 - 2), static_cast<float>(Height - 1)), 0.f, CharaType::Enemy);
	Chara.at(7).Init(VECTOR2D(static_cast<float>(Width - 1 - 3), static_cast<float>(Height - 1)), 0.f, CharaType::Enemy);

	int m_NowMoveCharacter = 0;
	bool m_MovingCharacter = false;

	drawcontrol->SetCameraPos(VGet(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y, 1.f));					//視点の場所(x,y,高さ)
	VECTOR EyeTargetPos = drawcontrol->GetCameraPos();
	VECTOR EyePos = EyeTargetPos;

	bool PrevMouseLeftPress{};
	VECTOR2D MouseLeftHoldPos{};
	bool LeftPressCancel{};
	//メインループ開始
	LONGLONG NowTime = GetNowHiPerformanceCount();//現時点の経過秒数を得る
	while (ProcessMessage() == 0) {
		LONGLONG WaitTime = GetNowHiPerformanceCount();//現時点の経過秒数を得る
		if ((WaitTime - NowTime) > (1000 * 1000 / BaseFrameRate)) {
			NowTime = WaitTime - ((WaitTime - NowTime) - (1000 * 1000 / BaseFrameRate));
			//カメラの位置を設定
			{
				float speed = 0.08f;
				if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_W) != 0) {
					EyeTargetPos.y += -speed;
				}
				if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_S) != 0) {
					EyeTargetPos.y += speed;
				}
				if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_A) != 0) {
					EyeTargetPos.x += -speed;
				}
				if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_D) != 0) {
					EyeTargetPos.x += speed;
				}
				if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
					if (m_NowMoveCharacter == 0 || m_NowMoveCharacter == 1 || m_NowMoveCharacter == 2 || m_NowMoveCharacter == 3) {
						EyeTargetPos.x = Chara.at(m_NowMoveCharacter).GetPos().x;
						EyeTargetPos.y = Chara.at(m_NowMoveCharacter).GetPos().y;
					}
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
				EyeTargetPos.z = std::clamp(EyeTargetPos.z + static_cast<float>(MouseWheelRot) * 0.1f, 0.5f, 1.f);


				float Per = (1.f - 0.9f);
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
					if (tm.GetCharaType() == CharaType::Enemy) { continue; }
					bool ThroughWay = drawcontrol->CheckPolyMoveWidth(en.GetPos(), tm.GetPos(), 0.1f);
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
				//en.SetDraw(true);
			}
			//
			if (m_MovingCharacter) {
				if (!Chara.at(m_NowMoveCharacter).IsMoving()) {
					++m_NowMoveCharacter %= static_cast<int>(Chara.size());
					m_MovingCharacter = false;
				}
			}
			//敵行動
			for (auto& en : Chara) {
				if (en.GetCharaType() == CharaType::Team) { continue; }
				bool CheckTarget = false;
				/*
				if (en.m_WayPointList.at(0) != -1) {
					auto& Target = drawcontrol->GetTileData(en.m_WayPointList.at(0)).GetPos();
					float Length = std::hypotf(Target.x - en.GetPos().x, Target.y - en.GetPos().y);
					if (Length < 0.5f) {
						CheckTarget = true;
					}
				}
				else {
					CheckTarget = true;
				}
				//*/
				if (!m_MovingCharacter) {
					if (m_NowMoveCharacter == static_cast<int>(&en - &Chara.front())) {
						m_MovingCharacter = true;
						CheckTarget = true;
					}
				}
				if (!CheckTarget) { continue; }
				int X{}, Y{};
				float LengthReq = 12.f;
				for (int x = 0; x < Width; x++) {
					for (int y = 0; y < Height; y++) {
						if ((drawcontrol->SetTileData(x, y)->Height == 0) && (drawcontrol->SetTileData(x, y)->WayPointRad != -1)) {
							{
								bool isHitWay = false;
								for (auto& w : en.m_WayPointList) {
									if (drawcontrol->SetTileData(x, y)->GetIndex() == w) {
										isHitWay = true;
										break;
									}
								}
								//違うルートを通る
								for (auto& en2 : Chara) {
									if (en2.GetCharaType() == CharaType::Team) { continue; }
									if (&en == &en2) { continue; }
									if (drawcontrol->SetTileData(x, y)->GetIndex() == en2.m_WayPointList.at(0)) {
										isHitWay = true;
										break;
									}
								}
								if (isHitWay) { continue; }
							}
							{
								auto& Target = drawcontrol->SetTileData(x, y)->GetPos();
								VECTOR2D AimVec(Target.x - en.GetPos().x, Target.y - en.GetPos().y);
								float Diff = en.GetRad() - std::atan2f(AimVec.x, -AimVec.y);
								if (Diff > DX_PI_F) {
									Diff -= DX_PI_F * 2;
								}
								if (Diff < -DX_PI_F) {
									Diff += DX_PI_F * 2;
								}
								if (!(std::fabsf(Diff) <= 60.f * DX_PI_F / 180.f)) {
									continue;
								}
							}
							/*
							{
								float rad = static_cast<float>(drawcontrol->SetTileData(x, y)->WayPointRad) * DX_PI_F / 180.f;
								float Diff = en.GetRad() - rad;
								if (Diff > DX_PI_F) {
									Diff -= DX_PI_F * 2;
								}
								if (Diff < -DX_PI_F) {
									Diff += DX_PI_F * 2;
								}
								if (!(std::fabsf(Diff) <= 120.f * DX_PI_F / 180.f)) {
									continue;
								}
							}
							//*/
							float Length = std::hypotf(static_cast<float>(x) - en.GetPos().x, static_cast<float>(y) - en.GetPos().y);
							if (LengthReq > Length) {
								LengthReq = Length;
								X = x;
								Y = y;
							}
						}
					}
				}
				if (LengthReq != 12.f) {
					for (int loop = static_cast<int>(en.m_WayPointList.size()) - 1; loop >= 1; --loop) {
						en.m_WayPointList.at(loop) = en.m_WayPointList.at(loop - 1);
					}

					en.m_WayPointList.at(0) = drawcontrol->SetTileData(X, Y)->GetIndex();
					en.SetTargetPos(VECTOR2D(static_cast<float>(X), static_cast<float>(Y)));
					float rad = static_cast<float>(drawcontrol->SetTileData(X, Y)->WayPointRad) * DX_PI_F / 180.f;
					VECTOR2D Vec(std::sin(rad), std::cos(rad));
					en.SetAimPos(VECTOR2D(static_cast<float>(X) + Vec.x, static_cast<float>(Y) + Vec.y));
				}
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
			if (!m_MovingCharacter) {
				if ((m_NowMoveCharacter == 0 || m_NowMoveCharacter == 1)) {
					int NearCharaID = -1;
					for (auto& c : Chara) {
						if (std::fabsf(MousePos.x - c.GetPos().x) < 0.5f && std::fabsf(MousePos.y - c.GetPos().y) < 0.5f) {
							NearCharaID = static_cast<int>(&c - &Chara.front());
							break;
						}
					}
					float Len = std::hypotf(MousePos.x - Chara.at(m_NowMoveCharacter).GetPos().x, MousePos.y - Chara.at(m_NowMoveCharacter).GetPos().y);
					bool IsActivePlace = (drawcontrol->SetTileData(MX, MY)->Height == 0 && (NearCharaID == -1 || NearCharaID == m_NowMoveCharacter));

					if (GetWindowActiveFlag() && (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) {
						LeftPressCancel = true;
					}
					bool LMP = (GetWindowActiveFlag() && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0);
					if (LMP && !LeftPressCancel) {
						if (!PrevMouseLeftPress) {
							if (!IsActivePlace || Len > 12.f) {
								LeftPressCancel = true;
							}
							MouseLeftHoldPos = MousePos;
						}
					}
					else {
						if (PrevMouseLeftPress) {
							if (!LeftPressCancel) {
								if (IsActivePlace) {
									Chara.at(m_NowMoveCharacter).SetAimPos(MousePos);
									Chara.at(m_NowMoveCharacter).SetTargetPos(MouseLeftHoldPos);
									m_MovingCharacter = true;
								}
							}
						}
						if (!LMP) {
							LeftPressCancel = false;
						}
					}
					PrevMouseLeftPress = LMP;
				}
				else {
					PrevMouseLeftPress = false;
				}
			}
			printfDx("[m_NowMoveCharacter]%d\n", m_NowMoveCharacter);
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
				if (!m_MovingCharacter) {
					if ((m_NowMoveCharacter == 0 || m_NowMoveCharacter == 1)) {
						int NearCharaID = -1;
						for (auto& c : Chara) {
							if (std::fabsf(MousePos.x - c.GetPos().x) < 0.5f && std::fabsf(MousePos.y - c.GetPos().y) < 0.5f) {
								NearCharaID = static_cast<int>(&c - &Chara.front());
								break;
							}
						}
						float Len = std::hypotf(MousePos.x - Chara.at(m_NowMoveCharacter).GetPos().x, MousePos.y - Chara.at(m_NowMoveCharacter).GetPos().y);
						bool IsActivePlace = (drawcontrol->SetTileData(MX, MY)->Height == 0 && (NearCharaID == -1 || NearCharaID == m_NowMoveCharacter));


						if (PrevMouseLeftPress && !LeftPressCancel) {
							VECTOR MP = drawcontrol->Get2DPos(MouseLeftHoldPos.x, MouseLeftHoldPos.y, 0.f);
							DrawCircle((int)MP.x, (int)MP.y, 16, IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), false, 3);

							VECTOR MP2 = drawcontrol->Get2DPos(MousePos.x, MousePos.y, 0.f);

							if (IsActivePlace) {
								DrawCircle((int)MP2.x, (int)MP2.y, 8, IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), false, 3);
							}

							VECTOR2D Vec(MP2.x - MP.x, MP2.y - MP.y);
							float Length = std::hypotf(Vec.x, Vec.y);
							if (Length > 0.5f) {
								MP2.x = MP.x + Vec.x / Length * 64.f;
								MP2.y = MP.y + Vec.y / Length * 64.f;
							}
							DrawLine((int)MP.x, (int)MP.y, (int)MP2.x, (int)MP2.y, IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), 3);
						}
						else {
							if (IsActivePlace && Len < 12.f) {
								VECTOR MP = drawcontrol->Get2DPos(MousePos.x, MousePos.y, 0.f);
								DrawCircle((int)MP.x, (int)MP.y, 16, IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), false, 3);
							}
							{
								VECTOR MP = drawcontrol->Get2DPos(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y, 0.f);
								VECTOR MP2 = drawcontrol->Get2DPos(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y + 12.f, 0.f);
								DrawCircle((int)MP.x, (int)MP.y, (MP2.y- MP.y), IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), false, 3);
							}
						}
						/*
						for (int x = 0; x < Width; x++) {
							for (int y = 0; y < Height; y++) {
								if (drawcontrol->SetTileData(x, y)->WayPointRad != -1) {
									VECTOR MP = drawcontrol->Get2DPos(x, y, 0.f);
									DrawCircle((int)MP.x, (int)MP.y, 16, GetColor(0, 255, 0), false, 3);

									float rad = static_cast<float>(drawcontrol->SetTileData(x, y)->WayPointRad) * DX_PI_F / 180.f;
									VECTOR MP2{};
									VECTOR2D Vec(std::sin(rad), std::cos(rad));
									float Length = std::hypotf(Vec.x, Vec.y);
									if (Length > 0.5f) {
										MP2.x = MP.x + Vec.x / Length * 64.f;
										MP2.y = MP.y + Vec.y / Length * 64.f;
									}
									DrawLine((int)MP.x, (int)MP.y, (int)MP2.x, (int)MP2.y, GetColor(0, 255, 0), 3);
								}
							}
						}
						//*/
					}
				}
				//デバッグ表示
				printfDx("FPS:%4.1f\n", GetFPS());
				printfDx("処理時間:%5.2fms\n", (float)(GetNowHiPerformanceCount() - WaitTime) / 1000.f);
			}
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
