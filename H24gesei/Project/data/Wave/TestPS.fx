// ピクセルシェーダーの入力
struct PS_INPUT
{
    float2 texCoords0 : TEXCOORD0; // テクスチャ座標
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
    float4 color0 : SV_TARGET0; // 色
};


// プログラムとのやり取りのために使うレジスタ1
cbuffer cbFree_CBUFFER1 : register(b2)
{
    float4 g_param;
}

//2フレーム前の描画結果
SamplerState g_Prev2Sampler : register(s0);
Texture2D g_Prev2Texture : register(t0);
//1フレーム前の描画結果
SamplerState g_Prev1Sampler : register(s1);
Texture2D g_Prev1Texture : register(t1);

float CalcWave(float2 TexCoords0)
{
    float s2 = 0.2;

    float Prev1Center = 2 * g_Prev1Texture.Sample(g_Prev1Sampler, TexCoords0).r;

    float p =
        Prev1Center - g_Prev2Texture.Sample(g_Prev2Sampler, TexCoords0).r +
        s2 * (
            g_Prev1Texture.Sample(g_Prev1Sampler, TexCoords0, float2(-1, 0)).r
            - Prev1Center
            + g_Prev1Texture.Sample(g_Prev1Sampler, TexCoords0, float2(1, 0)).r
            + g_Prev1Texture.Sample(g_Prev1Sampler, TexCoords0, float2(0, -1)).r
            - Prev1Center
            + g_Prev1Texture.Sample(g_Prev1Sampler, TexCoords0, float2(0, 1)).r
        );
    return p;
}
// main関数
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    PSOutput.color0.r = CalcWave(PSInput.texCoords0);
    PSOutput.color0.b = 0.f;
    PSOutput.color0.a = 0.f;
    PSOutput.color0.a = 1.f;
	// 出力パラメータを返す
    return PSOutput;
}

