#define FRAME_RATE 90.f
#include"DXLib_ref/DXLib_ref.h"

#include"DXLib_ref/VR.hpp"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	auto Drawparts = std::make_unique<DXDraw>("FPS_0", FRAME_RATE, false);	/*汎用クラス*/
	//
	VECTOR_ref pos_HMD;
	MATRIX_ref mat_HMD;
	bool endp = false, startp = false;
	VECTOR_ref campos, camvec, camup;	//カメラ
	float fov = deg2rad(90);			//カメラ
	{
		//
		Drawparts->Set_Device();									//VRセット
		Drawparts->Set_Light_Shadow(VGet(10.f, 10.f, 10.f), VGet(-10.f, -10.f, -10.f), VGet(-0.5f, -0.5f, -0.5f), [&] {; });
		//
		while (ProcessMessage() == 0) {
			const auto waits = GetNowHiPerformanceCount();
			//
			Drawparts->GetDevicePositionVR(Drawparts->get_hmd_num(), &pos_HMD, &mat_HMD);
			//操作
			{
				//VR用
				if (Drawparts->get_hand1_num() != -1) {
					auto& ptr_ = *Drawparts->get_device_hand1();
					if (ptr_.turn && ptr_.now) {
						if ((ptr_.on[0] & BUTTON_TRIGGER) != 0) { startp = true; }
					}
				}
				//
				if (CheckHitKey(KEY_INPUT_SPACE) != 0) { startp = true; }
			}

			if (!startp) {
				if (Drawparts->use_vr) {
					campos = pos_HMD;
					camvec = mat_HMD.zvec()*-1.f;
					camup = mat_HMD.yvec();
				}
				else {
					campos = VGet(0, 1.f, 0);
					camvec = VGet(0, 0, -1.f);
					camup = VGet(0, 1.f, 0);
				}
			}
			//VR空間に適用
			Drawparts->Move_Player();
			//描画
			Drawparts->draw_VR(
				[&] {
					Drawparts->Draw_by_Shadow(
						[&] {
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						for (int x = -50; x < 50; x++) {
							for (int y = -50; y < 50; y++) {
								DrawCone3D(VGet(float(x) / 10.f, 0.75f, float(y) / 10.f), VGet(float(x) / 10.f, 0.f, float(y) / 10.f), 0.1f, 8, GetColor(255, 255, 255), GetColor(255, 255, 255), TRUE);
							}
						}
					});
				},
				campos, camvec, camup, fov, 0.1f, 10.f);
			//画面更新
			Drawparts->Screen_Flip(waits);
			if ((CheckHitKey(KEY_INPUT_ESCAPE) != 0) | startp) {
				break;
			}
		}
	}


	return 0;
}