// ピクセルシェーダーの入力
struct PS_INPUT
{
	float4 Diffuse         : COLOR0;
	float4 Specular        : COLOR1;
	float2 TexCoords0      : TEXCOORD0;
	float4 VPosition	   : TEXCOORD1;	// 座標( 射影空間 )ピクセルシェーダーで参照する為の物
	float4 Position        : SV_POSITION;	// 座標( プロジェクション空間 )
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
	float4 Color0           : SV_TARGET0;	// 色
};


// 定数バッファピクセルシェーダー基本パラメータ
struct DX_D3D11_PS_CONST_BUFFER_BASE
{
	float4		FactorColor;			// アルファ値等

	float		MulAlphaColor;			// カラーにアルファ値を乗算するかどうか( 0.0f:乗算しない  1.0f:乗算する )
	float		AlphaTestRef;			// アルファテストで使用する比較値
	float2		Padding1;

	int			AlphaTestCmpMode;		// アルファテスト比較モード( DX_CMP_NEVER など )
	int3		Padding2;

	float4		IgnoreTextureColor;	// テクスチャカラー無視処理用カラー
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_PS_BASE				: register(b1)
{
	DX_D3D11_PS_CONST_BUFFER_BASE		g_Base;
};

// main関数
PS_OUTPUT main(PS_INPUT PSInput)
{
	PS_OUTPUT PSOutput;

	// 出力カラー = ディフューズカラー
	PSOutput.Color0.r = PSInput.VPosition.z;

	// 出力アルファ = 1.f
	PSOutput.Color0.a = 1.f;

	// 出力パラメータを返す
	return PSOutput;
}

/*
// ピクセルシェーダーの入力
struct PS_INPUT
{
	float4 ViewPosition   : TEXCOORD0;
};
// ピクセルシェーダーの出力
struct PS_OUTPUT
{
	float4 Color0         : COLOR0;
};

// main関数
PS_OUTPUT main(PS_INPUT PSInput)
{
	PS_OUTPUT PSOutput;

	// 奥行き値を書き込み
	PSOutput.Color0.r = PSInput.ViewPosition.z;
	PSOutput.Color0.g = 0.0f;
	PSOutput.Color0.b = 0.0f;
	PSOutput.Color0.a = 1.0;

	return PSOutput;
}
//*/