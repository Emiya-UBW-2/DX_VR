#include "DxLib.h"
#include<functional>
#include<array>
#include<algorithm>
//シェーダーを使用する際の補助クラス
class ShaderUseClass {
public:
	class ScreenVertex {
		VERTEX3DSHADER Screen_vertex[6] = {};
	public:
		const auto* GetScreenVertex() noexcept { return Screen_vertex; }
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
	typedef float DX_D3D11_SHADER_FLOAT2[2];

	struct Wave {
		DX_D3D11_SHADER_FLOAT2 dir;
		float amplitude;
		float waveLength;
	};
	struct ImmutableCB {
		Wave waves[20];
	};
private:
	//シェーダーハンドル
	int m_VertexShaderhandle{ -1 };
	int m_PixelShaderhandle{ -1 };

	std::array<int, 4> m_VertexShadercbhandle{-1};
	ImmutableCB WaveData;
	int m_VertexShadercbWaveDataHandle{ -1 };
public:
	ShaderUseClass() {
		//シェーダーハンドル
		m_VertexShaderhandle = -1;
		m_PixelShaderhandle = -1;
	}
	~ShaderUseClass() {
		Dispose();
	}
public:
	//初期化
	void			Init(const char* VertexShader, const char* PixelShader) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		this->m_VertexShaderhandle = LoadVertexShader(VertexShader);		// 頂点シェーダーバイナリコードの読み込み
		this->m_PixelShaderhandle = LoadPixelShader(PixelShader);			// ピクセルシェーダーバイナリコードの読み込み
		//頂点シェーダー周り
		for (auto& h : m_VertexShadercbhandle) {
			h = CreateShaderConstantBuffer(sizeof(float) * 4);
		}
		m_VertexShadercbWaveDataHandle = CreateShaderConstantBuffer(sizeof(ImmutableCB));
	}
	//後始末
	void			Dispose() noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		DeleteShader(this->m_VertexShaderhandle);
		DeleteShader(this->m_PixelShaderhandle);
		for (auto& h : m_VertexShadercbhandle) {
			DeleteShaderConstantBuffer(h);
		}
		DeleteShaderConstantBuffer(m_VertexShadercbWaveDataHandle);
	}
public:
	//頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
	void			SetVertexParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		FLOAT4* f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(this->m_VertexShadercbhandle[0]);		// 頂点シェーダー用の定数バッファのアドレスを取得
		f4->x = param1;
		f4->y = param2;
		f4->z = param3;
		f4->w = param4;
		UpdateShaderConstantBuffer(this->m_VertexShadercbhandle[0]);								// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
		SetShaderConstantBuffer(this->m_VertexShadercbhandle[0], DX_SHADERTYPE_VERTEX, Slot);		// 頂点シェーダーの定数バッファを定数バッファレジスタ４にセット
	}
	//
	void CalcGWave() {
		for (int i = 0; i < 20; i++)
		{
			Wave& w = WaveData.waves[i];
			float randomRad = (float)(GetRand(30) * DX_PI_F * 2 * 0.3f);
			w.dir[0] = sinf(randomRad);
			w.dir[1] = cosf(randomRad);
			w.amplitude = (0.03f + powf(2.0f, (float)GetRand(3) * 2.0f) * 0.05f)*0.05f;
			w.waveLength = 1.0f + powf(2.f, 1.f + (float)GetRand(3)) * 10.f;
		}
	}
	//頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
	void			SetVertexWave() noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		ImmutableCB* f4 = (ImmutableCB*)GetBufferShaderConstantBuffer(this->m_VertexShadercbWaveDataHandle);		// 頂点シェーダー用の定数バッファのアドレスを取得
		*f4 = WaveData;
		UpdateShaderConstantBuffer(this->m_VertexShadercbWaveDataHandle);								// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
		SetShaderConstantBuffer(this->m_VertexShadercbWaveDataHandle, DX_SHADERTYPE_VERTEX, 5);		// 頂点シェーダーの定数バッファを定数バッファレジスタ４にセット
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
	void			Draw(ScreenVertex& Screenvertex) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		Draw_lamda([&] {DrawPolygon3DToShader(Screenvertex.GetScreenVertex(), 2); });
	}
};

int DispX = 1920;
int DispY = 1080;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ShaderUseClass::ScreenVertex m_ScreenVertex;
	ShaderUseClass m_Shader;
	ShaderUseClass m_Shader2;
	ShaderUseClass m_Shader3;
	//
	std::array<int, 3> m_PrevBufferHandle{};
	int m_WaveNormalHandle;
	int m_WaveWallHandle;
	int m_WaveObjHandle;
	int cubeTex;
	//
	int m_ScreenBufferHandle;
	int m_ScreenDepthBufferHandle;
	//
	int m_SeaBufferHandle;
	int m_CausticBufferHandle;
	//
	int Model3Handle = -1;
	int Model2Handle = -1;
	int ModelHandle = -1;
	int Model0Handle = -1;
	//
	float RotateAngle = 0.f;// Y軸回転の角度指定
	float XAngle = DX_PI_F*10/180;// X軸回転の角度指定
	float CanLen = 20.f;// X軸回転の角度指定
	//
	float timer = 0.f;
	//
	int xsize = 2048;
	int ysize = 2048;
	//
	SetAlwaysRunFlag(TRUE);
	ChangeWindowMode(TRUE);
	SetWaitVSyncFlag(FALSE);
	SetGraphMode(DispX, DispY, 32);		//解像度
	if (DxLib_Init() < 0) { return -1; }
	SetUsePixelLighting(TRUE);
	//シェーダー
	m_ScreenVertex.SetScreenVertex(xsize, ysize);
	m_Shader.Init("Wave/TestVS.vso", "Wave/TestPS.pso");
	m_Shader2.Init("Wave/TestVS.vso", "Wave/NormalMapPS.pso");
	m_Shader3.Init("Wave/SeaVS.vso", "Wave/SeaPS.pso");
	m_Shader3.CalcGWave();
	cubeTex = LoadGraph("Wave/SkyCube.dds");
	//
	for (auto& g : m_PrevBufferHandle) {
		g = MakeScreen(xsize, ysize, TRUE);
	}
	{
		int PresetHandle = LoadGraph("Wave/Preset.bmp");
		SetDrawScreen(m_PrevBufferHandle[0]);
		ClearDrawScreen();
		{
			SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			DrawGraph(0, 0, PresetHandle, FALSE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		SetDrawScreen(m_PrevBufferHandle[1]);
		ClearDrawScreen();
		{
			SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			DrawGraph(0, 0, PresetHandle, FALSE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		SetDrawScreen(m_PrevBufferHandle[2]);
		ClearDrawScreen();
		{
			SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			DrawGraph(0, 0, PresetHandle, FALSE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		DeleteGraph(PresetHandle);
	}
	m_WaveNormalHandle = MakeScreen(xsize, ysize, TRUE);

	m_WaveWallHandle = MakeScreen(xsize, ysize, TRUE);
	m_WaveObjHandle = MakeScreen(xsize, ysize, TRUE);
	//
	m_ScreenBufferHandle = MakeScreen(DispX, DispY, TRUE);
	m_SeaBufferHandle = MakeScreen(DispX, DispY, TRUE);
	m_CausticBufferHandle = MakeScreen(DispX, DispY, TRUE);
	// 深度を描画するテクスチャの作成( 2チャンネル浮動小数点32ビットテクスチャ )
	{
		auto prevMip = GetCreateDrawValidGraphChannelNum();
		auto prevFloatType = GetDrawValidFloatTypeGraphCreateFlag();
		auto prevBit = GetCreateGraphChannelBitDepth();
		SetCreateDrawValidGraphChannelNum(2);
		SetDrawValidFloatTypeGraphCreateFlag(TRUE);
		SetCreateGraphChannelBitDepth(32);
		m_ScreenDepthBufferHandle = MakeScreen(DispX, DispY, FALSE);
		SetCreateDrawValidGraphChannelNum(prevMip);
		SetDrawValidFloatTypeGraphCreateFlag(prevFloatType);
		SetCreateGraphChannelBitDepth(prevBit);
	}
	//
	Model3Handle = MV1LoadModel("Wave/sky/model.mv1");
	Model2Handle = MV1LoadModel("Wave/Sand/model.mv1");
	ModelHandle = MV1LoadModel("Wave/Sea/model.mv1");
	Model0Handle = MV1LoadModel("Wave/Sea/model_back.mv1");

	MV1SetPosition(Model2Handle, VGet(0, -5, 0));
	//
	SetDrawScreen(m_WaveWallHandle);
	ClearDrawScreen();
	SetupCamera_Ortho(400.f);
	SetCameraNearFar(1.f, 50.f);
	SetCameraPositionAndTargetAndUpVec(VGet(0.0f, 50.0f, 0.0f), VGet(0.0f, 0.0f, 0.0f), VGet(0.0f, 0.0f, 1.0f));
	{
		SetUseLighting(FALSE);
		MV1DrawModel(Model2Handle);
		SetUseLighting(TRUE);
	}
	//
	VECTOR MovePos;
	VECTOR CameraPos, CameraVec;
	VECTOR MousPoint;
	VECTOR PrevPoint; PrevPoint.y = -1.f;
	// ESCキーが押されるまでループ
	LONGLONG Time = GetNowHiPerformanceCount();
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
			RotateAngle += DX_PI_F * 60 / 180 / GetFPS();
		}
		if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
			RotateAngle -= DX_PI_F * 60 / 180 / GetFPS();
		}

		if (CheckHitKey(KEY_INPUT_UP) != 0) {
			XAngle += DX_PI_F * 60 / 180 / GetFPS();
		}
		if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
			XAngle -= DX_PI_F * 60 / 180 / GetFPS();
		}
		XAngle = std::clamp(XAngle, DX_PI_F * -60 / 180, DX_PI_F * 60 / 180);
		CameraPos = VTransform(VTransform(VGet(0.0f, 0.0f, -CanLen), MGetRotX(XAngle)), MGetRotY(RotateAngle));
		if (XAngle < 0) {
			CameraVec = VGet(0.0f, CameraPos.y, 0.0f);
		}
		else {
			CameraVec = VGet(0.0f, 0.0f, 0.0f);
		}
		auto MovVec = VNorm(VSub(CameraPos, CameraVec)); MovVec.y = 0.f; MovVec = VNorm(MovVec);
		auto MovSide = VNorm(VCross(MovVec, VGet(0.f, 1.f, 0.f)));

		if (CheckHitKey(KEY_INPUT_A) != 0) {
			MovePos = VAdd(MovePos, VScale(MovSide, -10.f / GetFPS()));
		}
		if (CheckHitKey(KEY_INPUT_D) != 0) {
			MovePos = VAdd(MovePos, VScale(MovSide, 10.f / GetFPS()));
		}

		if (CheckHitKey(KEY_INPUT_W) != 0) {
			MovePos = VAdd(MovePos, VScale(MovVec, -10.f / GetFPS()));
		}
		if (CheckHitKey(KEY_INPUT_S) != 0) {
			MovePos = VAdd(MovePos, VScale(MovVec, 10.f / GetFPS()));
		}

		if (CheckHitKey(KEY_INPUT_R) != 0) {
			CanLen += 10.f / GetFPS();
		}
		if (CheckHitKey(KEY_INPUT_F) != 0) {
			CanLen -= 10.f / GetFPS();
		}
		CanLen = std::clamp(CanLen, 1.f, 200.f);

		CameraPos = VAdd(CameraPos, MovePos);
		CameraVec = VAdd(CameraVec, MovePos);

		int mx = 0, my = 0;
		GetMousePoint(&mx, &my);
		//波シム
		LONGLONG NowTime = GetNowHiPerformanceCount();
		if (NowTime > Time + 1000 / 60 * 1000) {
			Time = NowTime;
			{
				//前までの画面を保持
				for (int i = 0; i < 2; i++) {
					SetDrawScreen(m_PrevBufferHandle[i]);
					ClearDrawScreen();
					{
						DrawGraph(0, 0, m_PrevBufferHandle[i + 1], TRUE);
					}
				}
				SetDrawScreen(m_WaveObjHandle);
				ClearDrawScreen();
				SetupCamera_Ortho(400.f);
				SetCameraNearFar(1.f, 50.f);
				SetCameraPositionAndTargetAndUpVec(VGet(0.0f, 50.0f, 0.0f), VGet(0.0f, 0.0f, 0.0f), VGet(0.0f, 0.0f, 1.0f));
				{
					SetUseLighting(FALSE);

					float Per = std::clamp(VSize(VSub(PrevPoint, MousPoint))/3.f,0.f,1.f);
					Per = 1.f;
					DrawCapsule3D(MousPoint, PrevPoint, 0.25f, 8,
								  GetColor((int)(255.f*Per), 0, 0), GetColor((int)(255.f*Per), 0, 0), TRUE);
					SetUseLighting(TRUE);
				}
				SetDrawScreen(m_PrevBufferHandle[1]);
				{
					//オブジェクト書き込み
					SetDrawBright(255, 0, 0);
					DrawGraph(0, 0, m_WaveObjHandle, TRUE);

					DrawBox(3, 3, xsize - 3, ysize - 3, GetColor(255, 0, 0), FALSE);
					//地形を書き込み
					SetDrawBright(0, 0, 0);
					DrawGraph(0, 0, m_WaveWallHandle, TRUE);
					SetDrawBright(255, 255, 255);
				}
				SetDrawScreen(m_PrevBufferHandle[2]);
				ClearDrawScreen();
				{
					SetUseTextureToShader(0, m_PrevBufferHandle[0]);
					SetUseTextureToShader(1, m_PrevBufferHandle[1]);
					m_Shader.Draw(m_ScreenVertex);
					SetUseTextureToShader(0, -1);
					SetUseTextureToShader(1, -1);
				}
				PrevPoint = MousPoint;
			}
			GraphFilter(m_PrevBufferHandle[2], DX_GRAPH_FILTER_GAUSS, 8, 16);
			//法線マップへの変換
			SetDrawScreen(m_WaveNormalHandle);
			ClearDrawScreen();
			{
				SetUseTextureToShader(0, m_PrevBufferHandle[2]);
				m_Shader2.Draw(m_ScreenVertex);
				SetUseTextureToShader(0, -1);
			}
			//現段階の波形の保存
			/*
			if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
				SetDrawScreen(m_PrevBufferHandle[2]);
				SaveDrawScreenToBMP(0, 0, xsize, ysize, "Wave/Preset.bmp");
			}
			//*/
			timer += 1.f / 60.f*0.1f;
		}
		//海面以外の描画
		SetDrawScreen(m_ScreenBufferHandle);
		SetRenderTargetToShader(0, m_ScreenBufferHandle);
		SetRenderTargetToShader(2, m_ScreenDepthBufferHandle);
		ClearDrawScreen();
		SetupCamera_Perspective(DX_PI_F * 60 / 180);
		SetCameraPositionAndTargetAndUpVec(CameraPos, CameraVec, VGet(0.0f, 1.0f, 0.0f));

		SetCameraNearFar(1000.f, 5000.f);
		SetUseLighting(FALSE);
		MV1DrawModel(Model3Handle);
		SetUseLighting(TRUE);
		SetCameraNearFar(1.f, 500.f);
		ClearDrawScreenZBuffer();
		{
			VECTOR Start = ConvScreenPosToWorldPos(VGet((float)mx, (float)my, 0.f));
			VECTOR End = ConvScreenPosToWorldPos(VGet((float)mx, (float)my, 1.f));
			float per = (0.f - Start.y) / (End.y - Start.y);
			MousPoint.x = Start.x + (End.x - Start.x)*per;
			MousPoint.y = Start.y + (End.y - Start.y)*per;
			MousPoint.z = Start.z + (End.z - Start.z)*per;
			if (PrevPoint.y < 0.f) {
				PrevPoint = MousPoint;
			}


			if (CameraPos.y < 0.f) {
				SetFogEnable(TRUE);
				SetFogMode(DX_FOGMODE_LINEAR);
				SetFogColor(27, 57, 77);
				SetFogStartEnd(10.f, 50.f);
			}
			SetVerticalFogEnable(TRUE);
			SetVerticalFogMode(DX_FOGMODE_LINEAR);
			SetVerticalFogColor(27, 57, 77);
			SetVerticalFogStartEnd(-0.5f, -25.f);
			MV1DrawModel(Model2Handle);
			SetVerticalFogEnable(FALSE);

			SetFogEnable(FALSE);
		}
		SetRenderTargetToShader(0, -1);
		SetRenderTargetToShader(2, -1);
		//海面の描画
		SetDrawScreen(m_SeaBufferHandle);
		SetRenderTargetToShader(0, m_SeaBufferHandle);
		SetRenderTargetToShader(1, m_CausticBufferHandle);
		ClearDrawScreen();
		SetupCamera_Perspective(DX_PI_F * 60 / 180);
		SetCameraNearFar(1.f, 500.f);
		SetCameraPositionAndTargetAndUpVec(CameraPos, CameraVec, VGet(0.0f, 1.0f, 0.0f));
		{
			SetUseTextureToShader(1, m_WaveNormalHandle);
			SetUseTextureToShader(2, m_ScreenBufferHandle);
			SetUseTextureToShader(3, m_ScreenDepthBufferHandle);
			SetUseTextureToShader(4, cubeTex);
			m_Shader3.SetVertexParam(4, CameraPos.x, CameraPos.y, CameraPos.z, timer);
			m_Shader3.SetVertexWave();
			m_Shader3.Draw_lamda([&]() {
				MV1DrawModel(Model0Handle);
				MV1DrawModel(ModelHandle);
								 });
			SetUseTextureToShader(1, -1);
			SetUseTextureToShader(2, -1);
			SetUseTextureToShader(3, -1);
			SetUseTextureToShader(4, -1);
		}
		SetRenderTargetToShader(0, -1);
		SetRenderTargetToShader(1, -1);

		GraphFilter(m_CausticBufferHandle, DX_GRAPH_FILTER_GAUSS, 16, 1600);
		//
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			DrawGraph(0, 0, m_ScreenBufferHandle, TRUE);
			DrawGraph(0, 0, m_SeaBufferHandle, TRUE);

			SetDrawBlendMode(DX_BLENDMODE_ADD, 128);
			DrawGraph(0, 0, m_CausticBufferHandle, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			//DrawExtendGraph(0, 0, DispY, DispY, m_PrevBufferHandle[2], FALSE);
			
			DrawFormatString(0, 0, GetColor(255, 0, 0), "%5.2f FPS", GetFPS());
		}
		ScreenFlip();
	}
	m_Shader.Dispose();
	m_Shader2.Dispose();
	DxLib_End();
	return 0;
}
