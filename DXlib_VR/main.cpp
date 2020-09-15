#define FRAME_RATE 90.f
#include"DXLib_ref/DXLib_ref.h"

#include"DXLib_ref/VR.hpp"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	auto Drawparts = std::make_unique<DXDraw>("FPS_0", FRAME_RATE,true);	/*汎用クラス*/

	std::array<GraphHandle, 3> outScreen;	//スクリーンバッファ
	GraphHandle BufScreen;					//スクリーンバッファ
	float start_fl = 0.f;
	VECTOR_ref pos_HMD;
	MATRIX_ref mat_HMD;
	bool endp = false, startp = false;
	VECTOR_ref campos, camvec, camup;	//カメラ
	float fov = deg2rad(90);						//カメラ
	{
		BufScreen = GraphHandle::Make(Drawparts->disp_x,Drawparts->disp_y);		/*バッファスクリーン*/
		outScreen[0] = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y);	/*左目*/
		outScreen[1] = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y);	/*右目*/
		outScreen[2] = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y);	/*TPS用*/
		//
		Drawparts->Set_Device();									//VRセット

		auto draw_doing = [&] {
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			for (int x = -10; x < 10; x++) {
				for (int y = -10; y < 10; y++) {
					DrawCone3D(VGet(float(x)/10.f, 1.5f, float(y) / 10.f), VGet(float(x) / 10.f, 0.f, float(y) / 10.f), 0.05f, 8, GetColor(255, 255, 255), GetColor(255, 255, 255), TRUE);
				}
			}

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * start_fl / 3.f), 0, 255));
			DrawBox(0, 0, Drawparts->disp_x, Drawparts->disp_y, GetColor(255, 255, 255), TRUE);

		};
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
				if (Drawparts->use_vr) {
					campos = pos_HMD;
					camvec = mat_HMD.zvec()*-1.f;
					camup = mat_HMD.yvec();
				}
				else {
					campos = VGet(0, 0, 1.f);
					camvec = VGet(0, 0, 0);
					camup = VGet(0, 1.f, 0);
				}
			}
			else {
				start_fl += 1.f / fps;
				if (start_fl > 3.f) {
					endp = true;
				}
			}

			//VR空間に適用
			Drawparts->Move_Player();
			{
				if (Drawparts->use_vr) {
					for (char i = 0; i < 2; i++) {
						VECTOR_ref eyepos = campos + Drawparts->GetEyePosition_minVR(i);
						outScreen[i].SetDraw_Screen(0.1f, 10.f, fov, eyepos, eyepos + camvec, camup);
						draw_doing();
						GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						outScreen[i].DrawGraph(0, 0, false);
						Drawparts->PutEye((ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D(), i);
					}
				}
				else {
					outScreen[0].SetDraw_Screen(0.1f, 10.f, fov, campos, campos + camvec, camup);
					draw_doing();
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