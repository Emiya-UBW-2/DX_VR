#define NOMINMAX

#include "DXLib_ref/DXLib_ref.h"
using namespace DXLibRef;

// 画面のサイズ
constexpr auto SCREEN_W = 1920;
constexpr auto SCREEN_H = 1080;

#include "source/CubeEditer.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	float Xrad = 0.f;
	float Yrad = 0.f;
	int XBase = 0;
	int YBase = 0;
	int Yofs = 0;

	// ウインドウモードで起動
	ChangeWindowMode(TRUE);

	// 画面解像度を設定
	SetGraphMode(SCREEN_W, SCREEN_H, 32);
	SetWaitVSyncFlag(TRUE);
	// ＤＸライブラリの初期化
	if (DxLib_Init() < 0) return -1;
	SetUseZBuffer3D(TRUE);										//zbufuse
	SetWriteZBuffer3D(TRUE);									//zbufwrite
	SetUsePixelLighting(TRUE);	// ピクセル単位のライティングを行う描画モードに設定する
	SetBackgroundColor(30, 65, 90);

	CubeEditer cubeEditer;

	cubeEditer.Init();
	cubeEditer.LoadCellsFile();

	bool OnClick = false;
	bool OnUndo = false;
	bool OnRedo = false;
	bool OnSave = false;
	float OnDeleteTimer = 0.f;
	float CamLength = 7.f;
	// メインループ
	Vector3DX LookPos = Vector3DX::zero();
	Vector3DX TargetPos;
	Vector3DX DragPos;

	bool CanPut = false;
	bool RectangleMode = false;
	bool IsDelete = false;
	bool Is3rdMode = false;
	bool On3rd = false;
	while (ProcessMessage() == 0) {
		if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) { break; }
		Matrix4x4DX Rot = Matrix4x4DX::RotAxis(Vector3DX::right(), Xrad) * Matrix4x4DX::RotAxis(Vector3DX::up(), Yrad);
		{
			int mx = 0, my = 0;
			GetMousePoint(&mx, &my);
			//カメラ回転
			IsDelete = false;
			if ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) {
				OnDeleteTimer += 1.f / GetFPS();
			}
			else {
				if (0 < OnDeleteTimer && OnDeleteTimer <= 0.5f) {
					IsDelete = true;
				}
				OnDeleteTimer = 0.f;
			}

			if (0.f < OnDeleteTimer) {
				Xrad = std::clamp(Xrad + (float)(my - YBase) * deg2rad(1.f), deg2rad(0), deg2rad(90));
				Yrad += (float)(mx - XBase) * deg2rad(1.f);
			}
			if ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0) {
				LookPos += Matrix4x4DX::Vtrans((Vector3DX::left() * (float)(mx - XBase) + Vector3DX::up() * (float)(my - YBase)) * 0.02f, Rot);
			}
			XBase = mx;
			YBase = my;
			if (CanPut) {
				RectangleMode = (CheckHitKey(KEY_INPUT_SPACE) != 0);
				if ((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0) {
					if (!OnClick) {
						OnClick = true;
						DragPos = TargetPos;
					}
				}
				else {
					if (OnClick) {
						OnClick = false;
						if (RectangleMode) {
							for (int x = std::min((int)DragPos.x, (int)TargetPos.x); x <= std::max((int)DragPos.x, (int)TargetPos.x); x++) {
								for (int y = std::min((int)DragPos.y, (int)TargetPos.y); y <= std::max((int)DragPos.y, (int)TargetPos.y); y++) {
									for (int z = std::min((int)DragPos.z, (int)TargetPos.z); z <= std::max((int)DragPos.z, (int)TargetPos.z); z++) {
										if (Is3rdMode) {
											for (int xm = 0; xm < 3; xm++) {
												for (int ym = 0; ym < 3; ym++) {
													for (int zm = 0; zm < 3; zm++) {
														cubeEditer.AddCube(x + xm, y + ym, z + zm, 0);
													}
												}
											}
										}
										else {
											cubeEditer.AddCube(x, y, z, 0);
										}
									}
								}
							}
							cubeEditer.CheckPoint();
						}
						else {
							CubeEditer::Bresenham3D((int)DragPos.x, (int)DragPos.y, (int)DragPos.z, (int)TargetPos.x, (int)TargetPos.y, (int)TargetPos.z,
								[&](int x, int y, int z) {
									if (Is3rdMode) {
										for (int xm = 0; xm < 3; xm++) {
											for (int ym = 0; ym < 3; ym++) {
												for (int zm = 0; zm < 3; zm++) {
													cubeEditer.AddCube(x + xm, y + ym, z + zm, 0);
												}
											}
										}
									}
									else {
										cubeEditer.AddCube(x, y, z, 0);
									}
									return false;
								}
							);
							cubeEditer.CheckPoint();
						}
					}
				}
				if (IsDelete) {
					int x = (int)TargetPos.x;
					int y = (int)TargetPos.y;
					int z = (int)TargetPos.z;
					if (Is3rdMode) {
						for (int xm = 0; xm < 3; xm++) {
							for (int ym = 0; ym < 3; ym++) {
								for (int zm = 0; zm < 3; zm++) {
									cubeEditer.DelCube(x + xm, y + ym, z + zm);
								}
							}
						}
					}
					else {
						cubeEditer.DelCube(x, y, z);
					}
					cubeEditer.CheckPoint();
				}
				if (!OnClick) {
					if (CheckHitKey(KEY_INPUT_T) != 0) {
						if (!On3rd) {
							On3rd = true;
							Is3rdMode ^= 1;
						}
					}
					else {
						On3rd = false;
					}
				}
			}
			int MouseWheelRot = GetMouseWheelRotVolWithCheck();
			if (MouseWheelRot != 0) {
				if (CheckHitKey(KEY_INPUT_TAB) != 0) {
					Yofs += MouseWheelRot * (Is3rdMode ? 3 : 1);
				}
				else {
					CamLength = std::clamp(CamLength - (float)MouseWheelRot * 0.5f, 1.0f, 60.f);
				}
			}
			if (CheckHitKey(KEY_INPUT_F1) != 0) {
				LookPos = Vector3DX::zero();
				Xrad = 0.f;
				Yrad = 0.f;
			}
			if (CheckHitKey(KEY_INPUT_F2) != 0) {
				LookPos = Vector3DX::zero();
				Xrad = 0.f;
				Yrad = deg2rad(90.0f);
			}
			if (CheckHitKey(KEY_INPUT_F3) != 0) {
				LookPos = Vector3DX::zero();
				Xrad = deg2rad(90.0f);
				Yrad = 0.f;
			}

			Vector3DX NearPos = ConvScreenPosToWorldPos(Vector3DX::vget((float)mx, (float)my, 0.f).get());
			Vector3DX FarPos = ConvScreenPosToWorldPos(Vector3DX::vget((float)mx, (float)my, 1.f).get());
			{
				Vector3DX prev = TargetPos;
				if (OnClick) {
					prev = DragPos;
				}
				TargetPos = NearPos + (FarPos - NearPos) * std::abs(NearPos.y / std::max(std::abs(FarPos.y - NearPos.y), 0.001f));
				TargetPos.Set((float)((int)TargetPos.x), (float)(Yofs), (float)((int)TargetPos.z));
				if (Is3rdMode) {
					TargetPos.Set(
						(float)(((std::abs((int)TargetPos.x) / 3) * 3) * (TargetPos.x > 0.f ? 1.f : -1.f)),
						(float)(((std::abs((int)Yofs) / 3) * 3) * (Yofs > 0.f ? 1.f : -1.f)),
						(float)(((std::abs((int)TargetPos.z) / 3) * 3) * (TargetPos.z > 0.f ? 1.f : -1.f)));
				}
				TargetPos.x = std::clamp(TargetPos.x, -50.f, 50.f);
				TargetPos.z = std::clamp(TargetPos.z, -50.f, 50.f);
				if (CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
					TargetPos.x = prev.x;
				}
				if (CheckHitKey(KEY_INPUT_LCONTROL) != 0) {
					TargetPos.z = prev.z;
				}
				if (CheckHitKey(KEY_INPUT_TAB) != 0) {
					TargetPos.x = prev.x;
					TargetPos.z = prev.z;
				}
			}
			CanPut = FarPos.y < 0.f;

			if (CheckHitKey(KEY_INPUT_LCONTROL) != 0) {
				if (CheckHitKey(KEY_INPUT_Z) != 0) {
					if (!OnUndo) {
						OnUndo = true;
						cubeEditer.UnDo();
					}
				}
				else {
					OnUndo = false;
				}
				if (CheckHitKey(KEY_INPUT_Y) != 0) {
					if (!OnRedo) {
						OnRedo = true;
						cubeEditer.ReDo();
					}
				}
				else {
					OnRedo = false;
				}
				if (CheckHitKey(KEY_INPUT_S) != 0) {
					if (!OnSave) {
						OnSave = true;
						cubeEditer.SaveCellsFile();
					}
				}
				else {
					OnSave = false;
				}
			}
		}
		//
		GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
		SetCameraPositionAndTargetAndUpVec(
			(Matrix4x4DX::Vtrans(Vector3DX::back() * CamLength, Rot) + LookPos).get(),
			LookPos.get(),
			Matrix4x4DX::Vtrans(Vector3DX::up(), Rot).get()
		);
		SetCameraNearFar(0.1f, 300.f);
		SetupCamera_Perspective(deg2rad(90.f));
		{
			cubeEditer.DrawBG();
			cubeEditer.Draw();
			if (CanPut) {
				DrawCube3D(TargetPos.get(), (TargetPos + Vector3DX::one() * (float)(Is3rdMode ? 3 : 1)).get(), White, White, FALSE);
				Vector3DX pos = ConvWorldPosToScreenPos((TargetPos + Vector3DX::one() * 0.5f).get());
				if (0.f <= pos.z && pos.z <= 1.f) {
					DrawFormatString((int)pos.x, (int)pos.y, White, "(%d,%d,%d)", (int)TargetPos.x, (int)TargetPos.y, (int)TargetPos.z);
				}
			}
			if (OnClick) {
				if (RectangleMode) {
					for (int x = std::min((int)DragPos.x, (int)TargetPos.x); x <= std::max((int)DragPos.x, (int)TargetPos.x); x++) {
						for (int y = std::min((int)DragPos.y, (int)TargetPos.y); y <= std::max((int)DragPos.y, (int)TargetPos.y); y++) {
							for (int z = std::min((int)DragPos.z, (int)TargetPos.z); z <= std::max((int)DragPos.z, (int)TargetPos.z); z++) {
								Vector3DX Target; Target.Set((float)x, (float)y, (float)z);
								DrawCube3D(Target.get(), (Target + Vector3DX::one()* (float)(Is3rdMode ? 3 : 1)).get(), Green, Green, TRUE);
							}
						}
					}
				}
				else {
					CubeEditer::Bresenham3D((int)DragPos.x, (int)DragPos.y, (int)DragPos.z, (int)TargetPos.x, (int)TargetPos.y, (int)TargetPos.z,
						[&](int x, int y, int z) {
							Vector3DX Target; Target.Set((float)x, (float)y, (float)z);
							DrawCube3D(Target.get(), (Target + Vector3DX::one() * (float)(Is3rdMode ? 3 : 1)).get(), Green, Green, TRUE);
							return false;
						}
					);
				}
			}
			// パラメータを画面に表示
			clsDx();
			printfDx("FPS           %6.2f : FPS\n", GetFPS());
			printfDx("LShift  X固定\n");
			printfDx("LCtrl   Z固定\n");
			printfDx("Tab     Y固定\n");
			printfDx("%d / %d\n", cubeEditer.GetSavePointDataNow(), cubeEditer.GetSavePointDataMax());
			printfDx("Esc  終了\n");
		}


		// キーによるパラメータの操作
		LONGLONG StartTime = GetNowHiPerformanceCount();		//ScreenFlip前に現在の時間を取る
		ScreenFlip();		// 裏画面の内容を表画面に反映
	}
	cubeEditer.SaveCellsFile();
	DxLib_End();
	return 0;
}
