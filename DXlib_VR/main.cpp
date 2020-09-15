#include"DXLib_ref/DXLib_ref.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	DxLib_Init();

	DrawPixel(320, 240, GetColor(255, 255, 255));

	WaitKey();

	DxLib_End();
	return 0;
}