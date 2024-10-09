#include "DxLib.h"
#include <array>
#include <functional>

// 画面のサイズ
#define SCREEN_W		1920/2
#define SCREEN_H		1080/2

//シェーダーを使用する際の補助クラス
class ShaderUseClass {
public:
	//2Dにシェーダーを適用する際に使用する画面サイズの頂点情報
	class ScreenVertex {
		VERTEX3DSHADER Screen_vertex[6] = {};
	public:
		// 頂点データの取得
		const auto* GetScreenVertex() noexcept { return Screen_vertex; }
		// 頂点データの準備
		void			SetScreenVertex(int dispx, int dispy) noexcept {
			int xp1 = 0;
			int yp1 = dispy;
			int xp2 = dispx;
			int yp2 = 0;
			Screen_vertex[0].pos = VGet((float)xp1, (float)yp1, 0.0f);
			Screen_vertex[1].pos = VGet((float)xp2, (float)yp1, 0.0f);
			Screen_vertex[2].pos = VGet((float)xp1, (float)yp2, 0.0f);
			Screen_vertex[3].pos = VGet((float)xp2, (float)yp2, 0.0f);
			Screen_vertex[0].dif = GetColorU8(255, 255, 255, 255);
			Screen_vertex[1].dif = GetColorU8(255, 255, 255, 255);
			Screen_vertex[2].dif = GetColorU8(255, 255, 255, 255);
			Screen_vertex[3].dif = GetColorU8(255, 255, 255, 255);
			Screen_vertex[0].u = 0.0f; Screen_vertex[0].v = 0.0f;
			Screen_vertex[1].u = 1.0f; Screen_vertex[1].v = 0.0f;
			Screen_vertex[2].u = 0.0f; Screen_vertex[3].v = 1.0f;
			Screen_vertex[3].u = 1.0f; Screen_vertex[2].v = 1.0f;
			Screen_vertex[4] = Screen_vertex[2];
			Screen_vertex[5] = Screen_vertex[1];
		}
	};
private:
	//シェーダーハンドル
	int m_VertexShaderhandle{ -1 };
	int m_PixelShaderhandle{ -1 };
	//シェーダーに渡す追加パラメーターを配するハンドル
	std::array<int, 4> m_VertexShadercbhandle{ -1 };
	int m_PixelShaderSendDispSizeHandle{ -1 };
	std::array<int, 4> m_PixelShadercbhandle{ -1 };
public:
	ShaderUseClass() {
		//シェーダーハンドル
		m_VertexShaderhandle = -1;
		m_PixelShaderhandle = -1;
		//シェーダーに渡す追加パラメーターを配するハンドル
		for (auto& h : m_VertexShadercbhandle) { h = -1; }
		m_PixelShaderSendDispSizeHandle = -1;
		for (auto& h : m_PixelShadercbhandle) { h = -1; }
	}
	~ShaderUseClass() {
		Dispose();
	}
public:
	//初期化
	void			Init(const char* VertexShader, const char* PixelShader) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		//頂点シェーダー周り
		for (auto& h : m_VertexShadercbhandle) {
			h = CreateShaderConstantBuffer(sizeof(float) * 4);
		}
		this->m_VertexShaderhandle = LoadVertexShader(VertexShader);		// 頂点シェーダーバイナリコードの読み込み
		//ピクセルシェーダ―周り
		this->m_PixelShaderSendDispSizeHandle = CreateShaderConstantBuffer(sizeof(float) * 4);
		for (auto& h : m_PixelShadercbhandle) {
			h = CreateShaderConstantBuffer(sizeof(float) * 4);
		}
		this->m_PixelShaderhandle = LoadPixelShader(PixelShader);			// ピクセルシェーダーバイナリコードの読み込み
	}
	//後始末
	void			Dispose() noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		//頂点シェーダー周り
		for (auto& h : m_VertexShadercbhandle) {
			DeleteShaderConstantBuffer(h);
		}
		DeleteShader(this->m_VertexShaderhandle);
		//ピクセルシェーダ―周り
		DeleteShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);
		for (auto& h : m_PixelShadercbhandle) {
			DeleteShaderConstantBuffer(h);
		}
		DeleteShader(this->m_PixelShaderhandle);
	}
public:
	//頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
	void			SetVertexParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_VertexShadercbhandle[0]);		// 頂点シェーダー用の定数バッファのアドレスを取得
		f4->x = param1;
		f4->y = param2;
		f4->z = param3;
		f4->w = param4;
		UpdateShaderConstantBuffer(this->m_VertexShadercbhandle[0]);								// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
		SetShaderConstantBuffer(this->m_VertexShadercbhandle[0], DX_SHADERTYPE_VERTEX, Slot);		// 頂点シェーダーの定数バッファを定数バッファレジスタ４にセット
	}
	//ピクセルシェーダ―の2番目のレジスタに画面サイズの情報をセット
	void			SetPixelDispSize(int dispx, int dispy) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);	// ピクセルシェーダー用の定数バッファのアドレスを取得
		dispsize->u = (float)dispx;
		dispsize->v = (float)dispy;
		UpdateShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);									// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
		SetShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle, DX_SHADERTYPE_PIXEL, 2);				// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
	}
	//ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=3)
	void			SetPixelParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_PixelShadercbhandle[0]);				// ピクセルシェーダー用の定数バッファのアドレスを取得
		f4->x = param1;
		f4->y = param2;
		f4->z = param3;
		f4->w = param4;
		UpdateShaderConstantBuffer(this->m_PixelShadercbhandle[0]);											// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
		SetShaderConstantBuffer(this->m_PixelShadercbhandle[0], DX_SHADERTYPE_PIXEL, Slot);					// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
	}
	//3D空間に適用する場合の関数(引数に3D描画のラムダ式を代入)
	void			Draw_lamda(std::function<void()> doing) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
			doing();
			return;
		}
		SetUseVertexShader(this->m_VertexShaderhandle);											// 使用する頂点シェーダーをセット
		SetUsePixelShader(this->m_PixelShaderhandle);											// 使用するピクセルシェーダーをセット
		MV1SetUseOrigShader(TRUE);
		doing();
		MV1SetUseOrigShader(FALSE);
		SetUseVertexShader(-1);
		SetUsePixelShader(-1);
	}
	//2D画像に適用する場合の関数
	void			Draw(ScreenVertex& Screenvertex) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		Draw_lamda([&] {DrawPolygon3DToShader(Screenvertex.GetScreenVertex(), 2); });
	}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	static const int EXTEND = 1;


	ShaderUseClass::ScreenVertex	m_ScreenVertex;								// 頂点データ
	ShaderUseClass					m_Shader2D;									// 使用するシェーダー
	ShaderUseClass					m_Shader2D2;									// 使用するシェーダー

	float XPos = 0.f;
	float ZPos = -1.92f;

	float SSRScale = 12.5f;
	float INTENSITY = 1.0f;
	float SCALE = 0.1f/12.5f;
	float BIAS = 0.93f;
	float SAMPLE_RAD = 16.5f;
	float MAX_DISTANCE = 1500.0f;

	int ModelHandle;
	int ModelHandleA;
	int NormalScreenHandle;
	int DepthScreenHandle;
	int ColorScreenHandle;
	int AberrationScreen;
	int OutputScreenHandle;

	// ウインドウモードで起動
	ChangeWindowMode(TRUE);

	// 画面解像度を設定
	SetGraphMode(SCREEN_W, SCREEN_H, 32);
	SetWaitVSyncFlag(FALSE);
	// ＤＸライブラリの初期化
	if (DxLib_Init() < 0) return -1;

	m_ScreenVertex.SetScreenVertex(SCREEN_W, SCREEN_H);							// 頂点データの準備
	m_Shader2D.Init("shader/VS_SSAO.vso", "shader/PS_SSAO.pso");					// レンズ
	m_Shader2D2.Init("shader/VS_SSAO.vso", "shader/PS_BilateralBlur.pso");					// レンズ

	ModelHandle = MV1LoadModel("SSAO_Stage.mqo");								// ３Ｄモデルの読み込み
	ModelHandleA = MV1LoadModel("SSAO_Stagea.mqo");								// ３Ｄモデルの読み込み
	{
		SetCreateGraphChannelBitDepth(24);
		ColorScreenHandle = MakeScreen(SCREEN_W, SCREEN_H, FALSE);				// カラーバッファの作成

		NormalScreenHandle = MakeScreen(SCREEN_W, SCREEN_H, TRUE);				// 法線バッファの作成

		SetDrawValidFloatTypeGraphCreateFlag(TRUE);
		SetCreateDrawValidGraphChannelNum(2);
		SetCreateGraphChannelBitDepth(32);
		DepthScreenHandle = MakeScreen(SCREEN_W, SCREEN_H, TRUE);				// 法線バッファの作成
		SetCreateGraphChannelBitDepth(24);
		SetCreateDrawValidGraphChannelNum(4);
		SetDrawValidFloatTypeGraphCreateFlag(FALSE);
	}
	AberrationScreen = MakeScreen(SCREEN_W / EXTEND, SCREEN_H / EXTEND, TRUE);	// 描画スクリーン

	OutputScreenHandle = MakeScreen(SCREEN_W, SCREEN_H, TRUE);					// 出力先バッファの作成

	SetUsePixelLighting(TRUE);	// ピクセル単位のライティングを行う描画モードに設定する

	SetUseZBuffer3D(TRUE);
	SetDrawZBuffer(TRUE);

	// メインループ
	float DrawTimer = 0.f;
	while (ProcessMessage() == 0) {
		if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) { break; }
		//
		float fov_cam = 60.f * DX_PI_F / 180.0f;
		//もとになる画面とGバッファを描画
		{
			// カラーバッファを描画対象0に、法線バッファを描画対象1、深度＋光沢グレースケールバッファを描画対象2、光沢RGBバッファを描画対象3に設定
			SetRenderTargetToShader(0, ColorScreenHandle);
			SetRenderTargetToShader(1, NormalScreenHandle);
			SetRenderTargetToShader(2, DepthScreenHandle);
			ClearDrawScreen();			// 画面のクリア
			// カメラを設定
			SetCameraPositionAndTarget_UpVecY(VGet(XPos, 0.f, ZPos), VGet(0.f, 0.f, 0.f));
			SetCameraNearFar(0.1f, 300.f);
			SetupCamera_Perspective(fov_cam);
			{
				MV1DrawModel(ModelHandle);				// モデルの描画
				MV1DrawModel(ModelHandleA);				// モデルの描画
			}
			// 描画対象をすべて解除
			SetRenderTargetToShader(0, -1);
			SetRenderTargetToShader(1, -1);
			SetRenderTargetToShader(2, -1);
		}
		//SSRシェーダーを適用
		SetDrawScreen(OutputScreenHandle);
		{
			SetUseTextureToShader(0, ColorScreenHandle);	//使用するテクスチャをセット
			SetUseTextureToShader(1, NormalScreenHandle);
			SetUseTextureToShader(2, DepthScreenHandle);
			m_Shader2D.SetPixelDispSize(SCREEN_W, SCREEN_H);
			m_Shader2D.SetPixelParam(3, 0.0f, SSRScale, std::tan(fov_cam / 2.f), INTENSITY);
			m_Shader2D.SetPixelParam(4, SCALE, BIAS, SAMPLE_RAD, MAX_DISTANCE);

			m_Shader2D.Draw(m_ScreenVertex);

			SetUseTextureToShader(0, -1);
			SetUseTextureToShader(1, -1);
			SetUseTextureToShader(2, -1);
		}
		//SSRシェーダーにぼかしを入れる
		SetDrawScreen(AberrationScreen);
		{
			SetUseTextureToShader(0, OutputScreenHandle);	//使用するテクスチャをセット
			SetUseTextureToShader(1, ColorScreenHandle);	//使用するテクスチャをセット

			m_Shader2D2.SetPixelDispSize(SCREEN_W, SCREEN_H);
			m_Shader2D2.Draw(m_ScreenVertex);

			SetUseTextureToShader(0, -1);
			SetUseTextureToShader(1, -1);
		}
		//
		SetDrawScreen(DX_SCREEN_BACK);	//使用するテクスチャをセット
		{
			DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), TRUE);
			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, ColorScreenHandle, TRUE);

			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, DepthScreenHandle, TRUE);
			//DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(255, 255, 255), TRUE);
			//SetDrawBlendMode(DX_BLENDMODE_MULA, 255);
			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, OutputScreenHandle, TRUE);
			//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			// パラメータを画面に表示
			/*
			DrawFormatString(0, 32 * 0, GetColor(255, 255, 255), "W/S/A/D key  (%6.2f,%6.2f,%6.2f) : カメラ座標", XPos,0.f,ZPos);
			DrawFormatString(0, 32 * 1, GetColor(255, 255, 255), "FPS           %6.2f : FPS", GetFPS());
			DrawFormatString(0, 32 * 2, GetColor(255, 255, 255), "DrawTime(ms)  %6.2f : 描画時間", DrawTimer);
			DrawFormatString(0, 32 * 3, GetColor(255, 255, 255), "P    SSR画面のみ表示");
			DrawFormatString(0, 32 * 4, GetColor(255, 255, 255), "Esc  終了");
			//*/
			clsDx();
			printfDx("INTENSITY    %f\n", INTENSITY);
			printfDx("SCALE        %f\n", SCALE);
			printfDx("BIAS         %f\n", BIAS);
			printfDx("SAMPLE_RAD   %f\n", SAMPLE_RAD);
			printfDx("MAX_DISTANCE %f\n", MAX_DISTANCE);
			printfDx("FPS          %6.2f : FPS", GetFPS());
			printfDx("DrawTime(ms) %6.2f : 描画時間", DrawTimer);
		}

		// キーによるパラメータの操作
		switch (GetInputChar(TRUE)) {
		case 'a': XPos -= 0.3f; break;
		case 'd': XPos += 0.3f; break;
		case 'w': ZPos += 0.3f; break;
		case 's': ZPos -= 0.3f; break;
		case 'r': INTENSITY += 1.f; break;
		case 'f': INTENSITY -= 1.f; break;
		case 't': SCALE += 0.1f; break;
		case 'g': SCALE -= 0.1f; break;
		case 'y': BIAS += 0.1f; break;
		case 'h': BIAS -= 0.1f; break;
		case 'u': SAMPLE_RAD += 1.f; break;
		case 'j': SAMPLE_RAD -= 1.f; break;
		case 'i': MAX_DISTANCE += 100.f; break;
		case 'k': MAX_DISTANCE -= 100.f; break;
		default:break;
		}
		if (SSRScale <= 0.f) { SSRScale = 0.1f; }

		LONGLONG StartTime = GetNowHiPerformanceCount();		//ScreenFlip前に現在の時間を取る

		ScreenFlip();		// 裏画面の内容を表画面に反映

		DrawTimer = (float)(GetNowHiPerformanceCount() - StartTime) / 1000.f;//ScreenFlip出かかった時間をミリ秒単位で取得
	}

	// ＤＸライブラリの後始末
	DxLib_End();

	// ソフトの終了
	return 0;
}
