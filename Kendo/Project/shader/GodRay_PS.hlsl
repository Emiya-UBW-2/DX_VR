// ピクセルシェーダーの入力
struct PS_INPUT
{
	float4 Diffuse : COLOR0; // ディフューズカラー
	float2 TexCoords0 : TEXCOORD0; // テクスチャ座標
	float4 pos         : SV_POSITION;   // 座標( プロジェクション空間 )
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
	float4 color0 : SV_TARGET0; // 色
};

// 定数バッファピクセルシェーダー基本パラメータ
struct DX_D3D11_PS_CONST_BUFFER_BASE
{
	float4 FactorColor; // アルファ値等

	float MulAlphaColor; // カラーにアルファ値を乗算するかどうか( 0.0f:乗算しない  1.0f:乗算する )
	float AlphaTestRef; // アルファテストで使用する比較値
	float2 Padding1;

	int AlphaTestCmpMode; // アルファテスト比較モード( DX_CMP_NEVER など )
	int3 Padding2;

	float4 IgnoreTextureColor; // テクスチャカラー無視処理用カラー
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1)
{
	DX_D3D11_PS_CONST_BUFFER_BASE g_Base;
};

// プログラムとのやり取りのために使うレジスタ2
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3)
{
	float4 caminfo;
}


cbuffer cbLIGHTCAMERA_MATRIX						: register(b4) {
	matrix		g_LightViewMatrix;			// ライトのワールド　→　ビュー行列
	matrix		g_LightProjectionMatrix;	// ライトのビュー　　→　射影行列
};

cbuffer cbLIGHTCAMERA_MATRIX2						: register(b5) {
	matrix		g_LightViewMatrix1;			// ライトのワールド　→　ビュー行列
	matrix		g_LightProjectionMatrix1;	// ライトのビュー　　→　射影行列
};

cbuffer cbLIGHTCAMERA_MATRIX3						: register(b6) {
	matrix		g_LightViewMatrix2;			// ライトのワールド　→　ビュー行列
	matrix		g_LightProjectionMatrix2;	// ライトのビュー　　→　射影行列
};

SamplerState g_DiffuseMapSampler : register(s0); // ディフューズマップサンプラ
Texture2D g_DiffuseMapTexture : register(t0); // ディフューズマップテクスチャ

SamplerState g_DepthMapSampler : register(s1); // 深度マップサンプラ
Texture2D g_DepthMapTexture : register(t1); // 深度マップテクスチャ

SamplerState g_ShadowMapSampler : register(s2); // 深度バッファテクスチャ
Texture2D g_ShadowMapTexture : register(t2); // 深度バッファテクスチャ

SamplerState g_ShadowFarMapSampler : register(s3); // 深度バッファテクスチャ
Texture2D g_ShadowFarMapTexture : register(t3); // 深度バッファテクスチャ

float3 DisptoProjNorm(float2 screenUV) {
	float2 pos = screenUV;
	pos /= 0.5f;

	float4 position;

	position.x = pos.x - 1.f;
	position.y = 1.f - pos.y;
	position.z = 0.f;
	position.w = 0.f;
	position = mul(g_LightProjectionMatrix, position);

	return position.xyz;
}


PS_OUTPUT main(PS_INPUT PSInput)
{
	PS_OUTPUT PSOutput;
	PSOutput.color0 = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, PSInput.TexCoords0);
	return PSOutput;

	float Depth = g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.TexCoords0).r;

	float3 ViewPositionOne = DisptoProjNorm(PSInput.TexCoords0);

	float4 lWorldPosition;
	float4 LPPosition; // ライトからみた座標( xとyはライトの射影座標、zはビュー座標 )
	float2 DepthTexCoord;

	float Total = 0.f;
	float Div = 50.f;
	for (int i = 0;i < Div;i++) {
		lWorldPosition.xyz = ViewPositionOne * (Depth * i / Div);
		lWorldPosition.w = 0.f;
		lWorldPosition = mul(g_LightViewMatrix, lWorldPosition);

		// 深度影用のライトから見た射影座標を算出 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

		// ライトのビュー座標をライトの射影座標に変換
		LPPosition = mul(g_LightProjectionMatrix1, mul(g_LightViewMatrix1, lWorldPosition));
		// 深度テクスチャの座標を算出
		DepthTexCoord.x = (LPPosition.x + 1.0f) / 2.0f; // LPPosition.xy は -1.0f ～ 1.0f の値なので、これを 0.0f ～ 1.0f の値にする
		DepthTexCoord.y = 1.0f - (LPPosition.y + 1.0f) / 2.0f; // yは更に上下反転
		Total += g_ShadowMapTexture.Sample(g_ShadowMapSampler, DepthTexCoord).r;

		//------------------------------------------------------------------------------------------
		/*
		// ライトのビュー座標をライトの射影座標に変換
		LPPosition = mul(g_LightProjectionMatrix2, mul(g_LightViewMatrix2, lWorldPosition));
		// 深度テクスチャの座標を算出
		DepthTexCoord.x = (LPPosition.x + 1.0f) / 2.0f; // LPPosition.xy は -1.0f ～ 1.0f の値なので、これを 0.0f ～ 1.0f の値にする
		DepthTexCoord.y = 1.0f - (LPPosition.y + 1.0f) / 2.0f; // yは更に上下反転
		float ShadowFarMap = g_ShadowFarMapTexture.Sample(g_ShadowFarMapSampler, DepthTexCoord).r;
		//*/

		// 深度影用のライトから見た射影座標を算出 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )
	}
	Total /= Div;
	PSOutput.color0 = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, PSInput.TexCoords0);

	PSOutput.color0.r = Total;
	//PSOutput.color0.g = 0.f;
	//PSOutput.color0.b = 0.f;
	PSOutput.color0.a = 1.f;
	return PSOutput;
}