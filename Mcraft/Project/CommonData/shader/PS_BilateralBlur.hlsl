// ピクセルシェーダーの入力
struct PS_INPUT
{
    float4 dif : COLOR0; // ディフューズカラー
    float2 texCoords0 : TEXCOORD0; // テクスチャ座標
    float4 pos : SV_POSITION; // 座標( プロジェクション空間 )
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

// プログラムとのやり取りのために使うレジスタ1
cbuffer cbMULTIPLYCOLOR_CBUFFER1 : register(b2)
{
    float2 dispsize;
}

SamplerState g_DiffuseMapSampler : register(s0); // ディフューズマップサンプラ
Texture2D g_DiffuseMapTexture : register(t0); // ディフューズマップテクスチャ

float Gaussian2(float sigma, float2 x)
{
    return exp(-dot(x, x) / (2.0 * sigma * sigma));
}
float Gaussian3(float sigma, float3 x)
{
    return exp(-dot(x, x) / (2.0 * sigma * sigma));
}

float3 JoinedBilateralGaussianBlur(float2 uv)
{
    float2 g_pixelSize = float2(1.0 / dispsize.x, 1.0 / dispsize.y);
    float g_sigmaV1 = 32.;
    float g_sigmaV2 = 0.25;

    
    const float c_halfSamplesX = 4.;
    const float c_halfSamplesY = 4.;

    float3 total = 0.0;
    float ret = 0.0;

    float3 pivot = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, uv).rgb;

    for (float iy = -c_halfSamplesY; iy <= c_halfSamplesY; iy++)
    {
        for (float ix = -c_halfSamplesX; ix <= c_halfSamplesX; ix++)
        {
            float3 value = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, uv + float2(ix * g_pixelSize.x, iy * g_pixelSize.y)).rgb;

            float weight = Gaussian2(g_sigmaV1, float2(ix, iy)) * Gaussian3(g_sigmaV2, value - pivot);

            total += value * weight;
            ret += weight;

        }
    }
    
    return total / ret;
}


PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    
    PSOutput.color0.rgb = JoinedBilateralGaussianBlur(PSInput.texCoords0);
    PSOutput.color0.a = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, PSInput.texCoords0).a;
    return PSOutput;
}
