
#include "Sub.hpp"

enum class Match {
	Ready,
	Game,
	TeamWin,
	EnemyWin,
	Draw,
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	DXDraw::Create();
	auto* maincontrol = DXDraw::Instance();
	DrawControl::Create(1920, 1080);
	SetDefaultFontState("", 18, -1, -1, -1, 3);
	SoundPool::Create();
	SideLog::Create();
	Rule::Create();
	auto* drawcontrol = DrawControl::Instance();
	auto* soundPool = SoundPool::Instance();
	auto* sideLog = SideLog::Instance();
	auto* rule = Rule::Instance();
	AmmoEffectPool AmmoEffect;

	AmmoEffect.Init();
	//�ݒ�
	//�}�b�v�f�[�^�쐬
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


		//�o�H�T��
		drawcontrol->SetupLink();
	}

	int BGGraphHandle = LoadGraph("data/bg.png");

	int BufferHandle = MakeScreen(1920, 1080, false);

	std::vector<CharacterBase> Chara;
	Chara.resize(rule->GetPlayerAll());
	for (int loop = 0; loop < rule->GetPlayerAll(); ++loop) {
		if (loop < rule->GetPlayerAll() / 2 - rule->GetPlayablePlayer()) {
			int Index = loop;
			Chara.at(loop).Init(VECTOR2D(static_cast<float>(Index), 0.f), DX_PI_F, CharaType::TeamNPC);
		}
		else if (loop < rule->GetPlayerAll() / 2) {
			int Index = loop;
			Chara.at(loop).Init(VECTOR2D(static_cast<float>(Index), 0.f), DX_PI_F, CharaType::Team);
		}
		else {
			int Index = loop - (rule->GetPlayerAll() / 2);
			Chara.at(loop).Init(VECTOR2D(static_cast<float>(Width - 1 - Index), static_cast<float>(Height - 1)), 0.f, CharaType::Enemy);
		}
	}


	int m_NowMoveCharacter = 0;
	bool m_MovingCharacter = false;
	float m_MovingTimer = 0.1f;

	int SpeedRate = 1;
	bool PrevPressUp = false;
	bool PrevPressDown = false;

	drawcontrol->SetCameraPos(VGet(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y, 1.f));					//���_�̏ꏊ(x,y,����)
	VECTOR EyeTargetPos = drawcontrol->GetCameraPos();
	VECTOR EyePos = EyeTargetPos;

	bool PrevMouseLeftPress{};
	VECTOR2D MouseLeftHoldPos{};
	bool LeftPressCancel{};

	int TeamAlive = 0;
	int EnemyAlive = 0;

	float TeamAliveR = 0.f;
	float EnemyAliveR = 0.f;


	Match m_MatchNow = Match::Ready;



	float ReadyTimer = 4.f;
	float EndTimer = 0.f;

	float TotalTimer = 90.f;
	float Timer = TotalTimer;

	soundPool->PlayLoop(SoundEnum::Env);
	//���C�����[�v�J�n
	LONGLONG NowTime = GetNowHiPerformanceCount();//�����_�̌o�ߕb���𓾂�
	while (ProcessMessage() == 0) {
		LONGLONG WaitTime = GetNowHiPerformanceCount();//�����_�̌o�ߕb���𓾂�
		if ((WaitTime - NowTime) > (1000 * 1000 / BaseFrameRate)) {
			NowTime = WaitTime - ((WaitTime - NowTime) - (1000LL * 1000 / static_cast<LONGLONG>(BaseFrameRate)));
			//�J�����̈ʒu��ݒ�
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
					if (!Chara.at(m_NowMoveCharacter).IsEnemy()) {
						EyeTargetPos.x = Chara.at(m_NowMoveCharacter).GetPos().x;
						EyeTargetPos.y = Chara.at(m_NowMoveCharacter).GetPos().y;
					}
				}

				EyeTargetPos.x = std::clamp(EyeTargetPos.x, 0.f, static_cast<float>(Width));
				EyeTargetPos.y = std::clamp(EyeTargetPos.y, 0.f, static_cast<float>(Height));

				//�J�����g��k��
				int MouseWheelRot = 0;
				if (GetWindowActiveFlag()) {
					MouseWheelRot = GetMouseWheelRotVol();
				}
				else {
					GetMouseWheelRotVol();//�O��Ƃ̍���������d�g�݂Ȃ̂ŃA�N�e�B�u�łȂ��ꍇ���ʂ��Ă͂����悤��
				}
				EyeTargetPos.z = std::clamp(EyeTargetPos.z + static_cast<float>(MouseWheelRot) * 0.1f, 0.5f, 1.f);


				float Per = (1.f - 0.9f);
				EyePos.x = EyePos.x + (EyeTargetPos.x - EyePos.x) * Per;
				EyePos.y = EyePos.y + (EyeTargetPos.y - EyePos.y) * Per;
				EyePos.z = EyePos.z + (EyeTargetPos.z - EyePos.z) * Per;
				drawcontrol->SetCameraPos(EyePos);
			}
			int MX = 0;
			int MY = 0;
			VECTOR2D MousePos{};
			//�}�E�X���W���擾���Ă���
			GetMousePoint(&MX, &MY);
			MX = MX * 1920 / maincontrol->GetDispX();
			MY = MY * 1080 / maincontrol->GetDispY();

			VECTOR MPos = drawcontrol->GetPos2D(static_cast<float>(MX), static_cast<float>(MY));
			MX = std::clamp(static_cast<int>(MPos.x + 0.5f), 0, Width);
			MY = std::clamp(static_cast<int>(MPos.y + 0.5f), 0, Height);
			MousePos = VECTOR2D(static_cast<float>(MX), static_cast<float>(MY));
			for (int speedScale = 0; speedScale < SpeedRate; ++speedScale) {
				{
					TeamAlive = 0;
					EnemyAlive = 0;
					for (auto& c : Chara) {
						if (!c.IsAlive()) { continue; }
						if (!c.IsEnemy()) {
							TeamAlive++;
						}
						else {
							EnemyAlive++;
						}
					}
				}
				if (m_MatchNow != Match::Ready) {
					//
					for (auto& c : Chara) {
						c.Update();
					}
					if (m_MatchNow == Match::Game) {
						if (m_MovingCharacter) {
							Timer = std::max(Timer - 1.f / BaseFrameRate, 0.f);
						}
						{
							if (Timer == 0.f) {
								m_MatchNow = Match::Draw;
								soundPool->Play(SoundEnum::End);
							}
							if (TeamAlive != 0 && EnemyAlive == 0) {
								m_MatchNow = Match::TeamWin;
								soundPool->Play(SoundEnum::End);
							}
							if (TeamAlive == 0 && EnemyAlive != 0) {
								m_MatchNow = Match::EnemyWin;
								soundPool->Play(SoundEnum::End);
							}
						}
						for (auto& c : Chara) {
							c.SetIntercept(false);
						}
						//
						if (!Chara.at(m_NowMoveCharacter).IsAlive()) {
							for (int loop = 0; loop < static_cast<int>(Chara.size()); ++loop) {
								++m_NowMoveCharacter %= static_cast<int>(Chara.size());
								if (Chara.at(m_NowMoveCharacter).IsAlive()) {
									break;
								}
							}
						}
						if (m_MovingCharacter) {
							m_MovingTimer = std::max(m_MovingTimer - 1.f / BaseFrameRate, 0.f);
							if (m_MovingTimer == 0.f && !Chara.at(m_NowMoveCharacter).IsMoving()) {
								for (int loop = 0; loop < static_cast<int>(Chara.size()); ++loop) {
									++m_NowMoveCharacter %= static_cast<int>(Chara.size());
									if (Chara.at(m_NowMoveCharacter).IsAlive()) {
										break;
									}
								}
								m_MovingCharacter = false;
							}
						}
						//�ˌ�������łɎ��F����
						if (m_MovingCharacter) {
							for (auto& c : Chara) {
								if (!c.IsAlive()) { continue; }
								if (!c.IsEnemy()) { continue; }
								c.SetDraw(false);
								//c.SetDraw(true);
							}
							for (auto& c : Chara) {
								if (!c.IsAlive()) { continue; }
								int index = static_cast<int>(&c - &Chara.front());
								bool IsPeeker = (m_NowMoveCharacter == static_cast<int>(&c - &Chara.front()));//�ړ����̃L�����ł���
								float Rad = 45.f * DX_PI_F / 180.f;
								int TargetID = -1;
								//�����Ă���G�̒��Ŏ��g��������ʂ��Ă��Ĉ�Ԋp�x�����������G��T��
								for (auto& tgt : Chara) {
									if (!tgt.IsAlive()) { continue; }
									if (c.IsEnemy() == tgt.IsEnemy()) { continue; }
									int indextgt = static_cast<int>(&tgt - &Chara.front());
									if (!drawcontrol->CheckPolyMoveWidth(tgt.GetPos(), c.GetPos(), 0.1f)) { continue; }
									VECTOR2D AimVec(tgt.GetPos().x - c.GetPos().x, tgt.GetPos().y - c.GetPos().y);
									float AimLength = std::hypotf(AimVec.x, AimVec.y);
									if (AimLength > 3.f) {
										float Diff = c.GetRad() - std::atan2f(AimVec.x, -AimVec.y);
										if (Diff > DX_PI_F) {
											Diff -= DX_PI_F * 2;
										}
										if (Diff < -DX_PI_F) {
											Diff += DX_PI_F * 2;
										}
										if (!(std::fabsf(Diff) <= 45.f * DX_PI_F / 180.f)) { continue; }
										if (Rad > Diff) {
											Rad = Diff;
											TargetID = indextgt;
										}
										tgt.SetDraw(true);
									}
									else {
										Rad = 0.f;
										TargetID = indextgt;
										tgt.SetDraw(true);
									}
								}
								if (TargetID == -1) {
									c.SetShotReset();
									continue;
								}
								//���̓G�Ɍ���
								auto& tgt = Chara.at(TargetID);
								c.SetMoveAimPos(tgt.GetPos());
								c.SetIntercept(true);
								//shot
								if (c.IsShot()) {
									//�ˌ��G�t�F�N�g���o��
									VECTOR2D Vec(tgt.GetPos().x - c.GetPos().x, tgt.GetPos().y - c.GetPos().y);
									AmmoEffect.Add(c.GetPos(), Vec);
									VECTOR MP = drawcontrol->Get2DPos(c.GetPos().x, c.GetPos().y, 0.f);
									int Pan = 255 * (static_cast<int>(MP.x) - (1920 / 2)) / (1920 / 2);
									int Scale = 255 - std::abs(255 * (static_cast<int>(std::hypotf(MP.x - static_cast<float>(1920 / 2), MP.y - static_cast<float> (1080 / 2)))) / (1920 / 2)) / 2;
									soundPool->Play(SoundEnum::Shot, Pan, Scale);
									//���肷��
									if (GetRand(100) < (IsPeeker ? 55 : 30)) {
										//
										tgt.Death(index);
										soundPool->Play(SoundEnum::Hit);

										sideLog->Add(index, TargetID);
									}
								}
							}
						}
						//�����͋����\��
						for (auto& c : Chara) {
							if (!c.IsAlive()) { continue; }
							if (c.IsEnemy()) { continue; }
							c.SetDraw(true);
						}
						//�G�s��
						for (auto& en : Chara) {
							if (!en.IsAlive()) { continue; }
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
									m_MovingTimer = 0.4f;
								}
							}
							if (!CheckTarget) { continue; }
							float LengthBase = 5.f;
							while (true) {
								int X{}, Y{};
								float LengthReq = LengthBase;
								for (int x = 0; x < Width; x++) {
									for (int y = 0; y < Height; y++) {
										auto* pNow = drawcontrol->SetTileData(x, y);
										if ((pNow->Height == 0) && (pNow->WayPointRad != -1)) {
											{
												bool isHitWay = false;
												//�Ⴄ���[�g��ʂ�
												if (LengthReq <= 7.5f) {
													for (auto& w : en.m_WayPointList) {
														if (pNow->GetIndex() == w) {
															isHitWay = true;
															break;
														}
													}
												}
												if (isHitWay) { continue; }
												//�G�ƈႤ���[�g��ʂ�
												for (auto& en2 : Chara) {
													if (!en2.IsAlive()) { continue; }
													if (&en == &en2) { continue; }
													if (pNow->GetIndex() == en2.m_WayPointList.at(0)) {
														isHitWay = true;
														break;
													}
												}
												if (isHitWay) { continue; }
											}

											float Rad = en.GetRad();
											if ((en.IsEnemy() && TeamAlive <= 2) ||
												(!en.IsEnemy() && EnemyAlive <= 2)
												) {
												//��ԋ߂��G�̕���
												for (auto& tgt : Chara) {
													if (!tgt.IsAlive()) { continue; }
													if (tgt.IsEnemy() != en.IsEnemy()) { continue; }

													VECTOR2D Vec(tgt.GetPos().x - en.GetPos().x, tgt.GetPos().y - en.GetPos().y);
													float Length = std::hypotf(Vec.x, Vec.y);
													if (Length > 0.f) {
														Rad = std::atan2f(Vec.x, -Vec.y);
													}
													break;
												}
											}
											{
												auto& Target = pNow->GetPos();
												VECTOR2D AimVec(Target.x - en.GetPos().x, Target.y - en.GetPos().y);
												float Diff = Rad - std::atan2f(AimVec.x, -AimVec.y);
												if (Diff > DX_PI_F) {
													Diff -= DX_PI_F * 2;
												}
												if (Diff < -DX_PI_F) {
													Diff += DX_PI_F * 2;
												}
												if (!(std::fabsf(Diff) <= 70.f * DX_PI_F / 180.f)) {
													continue;
												}
											}
											//*
											{
												float NowRad =
													(en.m_WayPointList.at(0) == -1) ?
													Rad :
													static_cast<float>(drawcontrol->GetTileData(en.m_WayPointList.at(0)).WayPointRad) * DX_PI_F / 180.f;

												float rad = static_cast<float>(pNow->WayPointRad) * DX_PI_F / 180.f;
												float Diff = NowRad - rad;
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
								if (LengthReq != LengthBase) {
									for (int loop = static_cast<int>(en.m_WayPointList.size()) - 1; loop >= 1; --loop) {
										en.m_WayPointList.at(loop) = en.m_WayPointList.at(static_cast<size_t>(loop - 1));
									}

									en.m_WayPointList.at(0) = drawcontrol->SetTileData(X, Y)->GetIndex();
									en.SetTargetPos(VECTOR2D(static_cast<float>(X), static_cast<float>(Y)));
									float rad = static_cast<float>(drawcontrol->SetTileData(X, Y)->WayPointRad) * DX_PI_F / 180.f;
									VECTOR2D Vec(std::sin(rad), std::cos(rad));
									en.SetAimPos(VECTOR2D(static_cast<float>(X) + Vec.x, static_cast<float>(Y) + Vec.y));
									en.SetMoveAimPos(VECTOR2D(static_cast<float>(X) + Vec.x, static_cast<float>(Y) + Vec.y));
									break;
								}
								LengthBase += 0.5f;
								if (LengthBase > 10.f) {
									break;
								}
							}
						}
						//����
						if (!m_MovingCharacter) {
							if (Chara.at(m_NowMoveCharacter).GetCharaType() == CharaType::Team) {
								float Len = std::hypotf(MousePos.x - Chara.at(m_NowMoveCharacter).GetPos().x, MousePos.y - Chara.at(m_NowMoveCharacter).GetPos().y);
								bool IsActivePlace = (drawcontrol->SetTileData(MX, MY)->Height == 0);

								if (GetWindowActiveFlag() && (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) {
									LeftPressCancel = true;
								}
								bool LMP = (GetWindowActiveFlag() && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0);
								if (LMP && !LeftPressCancel) {
									if (!PrevMouseLeftPress) {
										if (!IsActivePlace || Len > 9.f) {
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
												Chara.at(m_NowMoveCharacter).SetMoveAimPos(MousePos);
												Chara.at(m_NowMoveCharacter).SetTargetPos(MouseLeftHoldPos);
												m_MovingCharacter = true;
												m_MovingTimer = 0.4f;
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
					}
					//
					AmmoEffect.Update();
				}
				{
					TeamAliveR = TeamAliveR + (static_cast<float>(TeamAlive) - TeamAliveR) * 0.1f;
					EnemyAliveR = EnemyAliveR + (static_cast<float>(EnemyAlive) - EnemyAliveR) * 0.1f;
				}
				ReadyTimer = std::max(ReadyTimer - 1.f / BaseFrameRate, 0.f);

				switch (m_MatchNow) {
				case Match::Ready:
					if (ReadyTimer <= 1.f) {
						m_MatchNow = Match::Game;
						soundPool->Play(SoundEnum::Start);
					}
					break;
				case Match::Draw:
				case Match::TeamWin:
				case Match::EnemyWin:
					EndTimer = std::min(EndTimer + 1.f / BaseFrameRate, 2.f);
					break;
				default:
					break;
				}
				sideLog->Update();
			}
			{
				bool PressUp = GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_UP) != 0;
				bool PressDown = GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_DOWN) != 0;

				if (PressUp && !PrevPressUp) {
					switch (SpeedRate) {
					case 0: SpeedRate = 1; break;
					case 1: SpeedRate = 2; break;
					case 2: SpeedRate = 4; break;
					case 4: SpeedRate = 4; break;
					default: break;
					}
				}
				if (PressDown && !PrevPressDown) {
					switch (SpeedRate) {
					case 0: SpeedRate = 0; break;
					case 1: SpeedRate = 0; break;
					case 2: SpeedRate = 1; break;
					case 4: SpeedRate = 2; break;
					default: break;
					}
				}
				PrevPressUp = PressUp;
				PrevPressDown = PressDown;
			}
			drawcontrol->Execute();
			//�`��
			SetDrawScreen(BufferHandle);
			ClearDrawScreen();
			{
				{
					//�w�i
					drawcontrol->Draw(1920, 1080);
					//�e
					AmmoEffect.Draw();
					//�L����
					for (auto& c : Chara) {
						if (c.IsAlive()) { continue; }
						c.Draw();
					}
					for (auto& c : Chara) {
						if (!c.IsAlive()) { continue; }
						int index = static_cast<int>(&c - &Chara.front());
						if (m_NowMoveCharacter == index) { continue; }
						c.Draw();
					}
					if (Chara.at(m_NowMoveCharacter).IsAlive()) {
						Chara.at(m_NowMoveCharacter).Draw();
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
					if (ReadyTimer > 1.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * (ReadyTimer - 1.f) / 3.f));
						DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					if (EndTimer > 1.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * (EndTimer - 1.f)));
						DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					if (m_MatchNow == Match::Game) {
						//�J�[�\��
						if (!m_MovingCharacter) {
							if (Chara.at(m_NowMoveCharacter).GetCharaType() == CharaType::Team) {
								float Len = std::hypotf(MousePos.x - Chara.at(m_NowMoveCharacter).GetPos().x, MousePos.y - Chara.at(m_NowMoveCharacter).GetPos().y);


								if (PrevMouseLeftPress && !LeftPressCancel) {
									bool IsActivePlace = (drawcontrol->SetTileData(MX, MY)->Height == 0);
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
									bool IsActivePlace = (drawcontrol->SetTileData(MX, MY)->Height == 0 && Len < 9.f);
									if (IsActivePlace) {
										VECTOR MP = drawcontrol->Get2DPos(MousePos.x, MousePos.y, 0.f);
										DrawCircle((int)MP.x, (int)MP.y, 16, IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), false, 3);
									}
									{
										VECTOR MP = drawcontrol->Get2DPos(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y, 0.f);
										VECTOR MP2 = drawcontrol->Get2DPos(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y + 9.f, 0.f);
										DrawCircle((int)MP.x, (int)MP.y, (int)(MP2.y - MP.y), IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), false, 3);
									}
								}
							}
						}
						//����L����
						if (Chara.at(m_NowMoveCharacter).IsDraw()) {
							VECTOR MP = drawcontrol->Get2DPos(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y, 0.f);
							DrawCircle((int)MP.x, (int)MP.y, 32, GetColor(255, 2550, 0), false, 3);
						}
					}
					//
					//Timer
					{
						unsigned int Color = 0;
						bool IsDraw = true;

						Color = GetColor(0, 255, 0);
						if (Timer < TotalTimer * 1.f / 3.f || Timer == 0.f) {
							Color = GetColor(255, 0, 0);
						}
						if (m_MovingCharacter) {
							IsDraw = (static_cast<int>(Timer * 10.f) % 10 > 5);
							Color = GetColor(255, 255, 0);
						}
						if (IsDraw) {
							char Text[64] = "";
							sprintfDx(Text, "%d:%05.2f", static_cast<int>(Timer / 60.f), Timer - static_cast<int>(Timer / 60.f) * 60.f);
							DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 92 + 32, Text, Color, GetColor(0, 0, 0));
						}
						{
							char Text[64] = "Stop";
							if (SpeedRate > 0) {
								sprintfDx(Text, "X %d", SpeedRate);
							}
							DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 92 + 32 + 24, Text, GetColor(0, 255, 0), GetColor(0, 0, 0));
						}
					}
					//turn
					switch (Chara.at(m_NowMoveCharacter).GetCharaType()) {
					case CharaType::Team:
						if (!m_MovingCharacter) {
							DrawString(128, 128, "���Ȃ��̑���t�F�[�Y�ł�", GetColor(128, 255, 0), GetColor(0, 0, 0));
						}
						break;
					case CharaType::TeamNPC:
						DrawString(128, 128, "�����̑���t�F�[�Y�ł�", GetColor(100, 100, 255), GetColor(0, 0, 0));
						break;
					case CharaType::Enemy:
						DrawString(128, 128, "�G�̑���t�F�[�Y�ł�", GetColor(255, 0, 0), GetColor(0, 0, 0));
						break;
					default:
						break;
					}
					//
					{
						{
							int X = static_cast<int>((TeamAliveR / 4.f) * static_cast<float>(1920 / 4));
							DrawBox(1920 / 2 - X - 1, 92 - 1, 1920 / 2, 92 + 18 + 1, GetColor(255, 255, 255), TRUE);
							DrawBox(1920 / 2 - X, 92, 1920 / 2, 92 + 18, GetColor(0, 255, 0), TRUE);
						}
						{
							int X = static_cast<int>((EnemyAliveR / 4.f) * static_cast<float>(1920 / 4));
							DrawBox(1920 / 2, 92 - 1, 1920 / 2 + X + 1, 92 + 18 + 1, GetColor(255, 255, 255), TRUE);
							DrawBox(1920 / 2, 92, 1920 / 2 + X, 92 + 18, GetColor(255, 0, 0), TRUE);
						}
					}
					//
					sideLog->Draw(128, 1080 / 2 - 64);

					DrawExtendGraph(0, 0, 1920, 1080, BGGraphHandle, true);
				}
				//�������
				DrawString(32, 1080 - 32 - 64, "�ړ����� ���N���b�N:�ړ��w�� ���N���b�N���ɉE�N���b�N:�L�����Z��", GetColor(255, 255, 0), GetColor(0, 0, 0));
				DrawString(32, 1080 - 32 - 32, "�J�������� ��Shift:����L�������t�H�[�J�X�@WASD:�ړ� ����:���x�ؑ�", GetColor(255, 255, 0), GetColor(0, 0, 0));
				//
				if (ReadyTimer > 0.f) {
					if (ReadyTimer - 1.f > 0.f) {
						int X = static_cast<int>(((ReadyTimer - 1.f) / 3.f) * static_cast<float>(1920 / 2));
						DrawBox(1920 / 2 - X, 1080 / 2 - 32 - 1, 1920 / 2 + X, 1080 / 2 + 32 + 1, GetColor(255, 255, 255), TRUE);
						DrawBox(1920 / 2 - X, 1080 / 2 - 32, 1920 / 2 + X, 1080 / 2 + 32, GetColor(64, 64, 64), TRUE);
					}
					if ((static_cast<int>(ReadyTimer * 20.f) % 10 > 5)) {
						char Text[64] = "Go !";
						if (ReadyTimer - 1.f > 0.f) {
							sprintfDx(Text, "Ready...");
						}
						DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 1080 / 2 - 18 / 2, Text, GetColor(0, 255, 0), GetColor(0, 0, 0));
					}
				}
				//
				if (m_MatchNow == Match::Draw || m_MatchNow == Match::TeamWin || m_MatchNow == Match::EnemyWin) {
					int X = static_cast<int>((1.f - std::powf(1.f - std::clamp(EndTimer / 0.5f, 0.f, 1.f), 2.f)) * static_cast<float>(1920 / 2));
					DrawBox(1920 / 2 - X, 1080 / 2 - 32 - 1, 1920 / 2 + X, 1080 / 2 + 32 + 1, GetColor(255, 255, 255), TRUE);
					DrawBox(1920 / 2 - X, 1080 / 2 - 32, 1920 / 2 + X, 1080 / 2 + 32, GetColor(64, 64, 64), TRUE);
					switch (m_MatchNow) {
					case Match::Draw:
					{
						const char Text[] = "Draw";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 1080 / 2 - 18 / 2, Text, GetColor(0, 255, 0), GetColor(0, 0, 0));
					}
					break;
					case Match::TeamWin:
					{
						const char Text[] = "Team Win";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 1080 / 2 - 18 / 2, Text, GetColor(0, 255, 0), GetColor(0, 0, 0));
					}
					break;
					case Match::EnemyWin:
					{
						const char Text[] = "Enemy Win";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 1080 / 2 - 18 / 2, Text, GetColor(255, 0, 0), GetColor(0, 0, 0));
					}
					break;
					default:
						break;
					}
				}
				//�f�o�b�O�\��
#if _DEBUG
				printfDx("FPS:%4.1f\n", GetFPS());
				printfDx("��������:%5.2fms\n", (float)(GetNowHiPerformanceCount() - WaitTime) / 1000.f);
#endif
			}
		}
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			DrawExtendGraph(0, 0, maincontrol->GetDispX(), maincontrol->GetDispY(), BufferHandle, FALSE);
		}
		ScreenFlip();
		clsDx();
	}
	AmmoEffect.Dispose();
	for (auto& c : Chara) {
		c.Dispose();
	}
	Rule::Release();
	SideLog::Release();
	SoundPool::Release();
	DrawControl::Release();
	DXDraw::Release();
	return 0;// �\�t�g�̏I�� 
}
