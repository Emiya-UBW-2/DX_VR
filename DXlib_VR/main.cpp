#define FRAME_RATE 90.f
#include"DXLib_ref/DXLib_ref.h"

#include"DXLib_ref/VR.hpp"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	auto Drawparts = std::make_unique<DXDraw>("FPS_0", FRAME_RATE,true);	/*汎用クラス*/

	std::array<GraphHandle, 3> outScreen;	//スクリーンバッファ
	GraphHandle BufScreen;					//スクリーンバッファ
	float start_fl = 0.f, sets = 0.f;
	VECTOR_ref pos_HMD;
	MATRIX_ref mat_HMD;
	bool endp = false, startp = false;
	unsigned char restart = 0;
	VECTOR_ref campos, campos_buf, camvec, camup, campos_TPS;	//カメラ
	float fov = deg2rad(45);								//カメラ
	{
		BufScreen = GraphHandle::Make(Drawparts->disp_x,Drawparts->disp_y);		/*バッファスクリーン*/
		outScreen[0] = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y);	/*左目*/
		outScreen[1] = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y);	/*右目*/
		outScreen[2] = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y);	/*TPS用*/
		//
		Drawparts->Set_Device();									//VRセット

		//
		while (ProcessMessage() == 0) {
			const auto fps = GetFPS();
			const auto waits = GetNowHiPerformanceCount();
			if (!startp) {
				//VR用
				{
					if (Drawparts->use_vr) {
						Drawparts->GetDevicePositionVR(Drawparts->get_hmd_num(), &pos_HMD, &mat_HMD);
						if (Drawparts->get_hand1_num() != -1) {
							auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand1_num()];
							if (ptr_.turn && ptr_.now) {
								if ((ptr_.on[0] & BUTTON_TRIGGER) != 0) { startp = true; }
							}
						}
					}
					{
						if (CheckHitKey(KEY_INPUT_SPACE) != 0) { startp = true; }
					}
				}
				//
				if (startp) {
				}
			}

			if (!startp) {
				campos = mat_HMD.zvec() * 0.6f;
			}
			else {
				easing_set(&campos, VGet(0.f, 0.f, 1.f), 0.95f, fps);
				start_fl += 1.f / fps;
				if (start_fl > 3.f) {
					endp = true;
				}
			}
			//VR空間に適用
			Drawparts->Move_Player();
			{
				outScreen[0].SetDraw_Screen(0.1f, 10.f, fov, campos, VGet(0, 0, 0), VGet(0.f, 1.f, 0.f));
				{
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					DrawCone3D(VGet(0, 0, 0.3f), VGet(0, 0, 0), 0.1f, 6, GetColor(255, 255, 255), GetColor(255, 255, 255), TRUE);

					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * start_fl / 3.f), 0, 255));
					DrawBox(0, 0, Drawparts->disp_x, Drawparts->disp_y, GetColor(255, 255, 255), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				if (Drawparts->use_vr) {
					for (char i = 0; i < 2; i++) {
						GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
						outScreen[0].DrawGraph(0, 0, false);
						Drawparts->PutEye((ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D(), i);
					}
				}
				GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
				{
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					outScreen[0].DrawExtendGraph(0, 0, Drawparts->out_disp_x, Drawparts->out_disp_y, true);
				}
			}
			DXDraw::Screen_Flip();
			Drawparts->Eye_Flip(waits, 90.f);
			if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
				break;
			}
			if (endp) {
				break;
			}
		}
	}


	return 0;
}