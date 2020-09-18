#define FRAME_RATE 90.f
#include"DXLib_ref/DXLib_ref.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	auto Drawparts = std::make_unique<DXDraw>("FPS_0", FRAME_RATE, true);	/*汎用クラス*/
	auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);	/*デバッグクラス*/
	//
	VECTOR_ref pos_HMD;
	MATRIX_ref mat_HMD;
	VECTOR_ref pos_HAND1;
	MATRIX_ref mat_HAND1;
	VECTOR_ref pos_HAND2;
	MATRIX_ref mat_HAND2;
	DXDraw::cam_info cams;
	//
	bool startp = false;
	cams.set_cam_info(VGet(0, 1.f, 0), VGet(0, 0, -1.f), VGet(0, 1.f, 0), deg2rad(90), 0.1f, 10.f);//カメラ
	{
		Drawparts->Set_Light_Shadow(VGet(10.f, 10.f, 10.f), VGet(-10.f, -10.f, -10.f), VGet(-0.5f, -0.5f, -0.5f), [&] {; });	//ライト、影
		//main_loop
		while (ProcessMessage() == 0) {
			const auto waits = GetNowHiPerformanceCount();
			Debugparts->put_way();
			//座標取得
			Drawparts->GetDevicePositionVR(Drawparts->get_hmd_num(), &pos_HMD, &mat_HMD);
			Drawparts->GetDevicePositionVR(Drawparts->get_hand1_num(), &pos_HAND1, &mat_HAND1);
			Drawparts->GetDevicePositionVR(Drawparts->get_hand2_num(), &pos_HAND2, &mat_HAND2);
			//操作
			{
				//VR用
				if (Drawparts->get_hand1_num() != -1) {
					auto& ptr_ = *Drawparts->get_device_hand1();
					if (ptr_.turn && ptr_.now) {
						if ((ptr_.on[0] & BUTTON_TRIGGER) != 0) {
							startp = true;
						}
					}
				}
				//共通
				if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
					startp = true;
				}
			}
			//VR空間に適用
			Drawparts->Move_Player();
			//カメラ指定
			if (Drawparts->use_vr) {
				cams.campos = pos_HMD;
				cams.camvec = mat_HMD.zvec()*-1.f;
				cams.camup = mat_HMD.yvec();
			}
			else {
				cams.campos = VGet(0, 1.f, 0);
				cams.camvec = VGet(0, 0, -1.f);
				cams.camup = VGet(0, 1.f, 0);
			}
			//描画
			Drawparts->draw_VR(
				[&] {
					Drawparts->Draw_by_Shadow(
						[&] {
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						for (int x = -10; x < 10; x++) {
							for (int y = -10; y < 0; y++) {
								DrawCone3D(VGet(float(x) / 10.f, 0.75f, float(y) / 10.f), VGet(float(x) / 10.f, 0.f, float(y) / 10.f), 0.1f, 8, GetColor(255, 255, 255), GetColor(255, 255, 255), TRUE);
							}
						}
						//hand1
						DrawCone3D((pos_HAND1 + mat_HAND1.zvec()*0.1f).get(), (pos_HAND1 - mat_HAND1.zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
						//hand2
						DrawCone3D((pos_HAND2 + mat_HAND2.zvec()*0.1f).get(), (pos_HAND2 - mat_HAND2.zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
					});
				},
				cams);
			//debug
			GraphHandle::SetDraw_Screen(int(DX_SCREEN_BACK), false);
			Debugparts->end_way();
			Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
			//画面更新
			Drawparts->Screen_Flip(waits);
			if ((CheckHitKey(KEY_INPUT_ESCAPE) != 0) | startp) {
				break;
			}
		}
	}


	return 0;
}