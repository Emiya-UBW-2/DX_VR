// ピクセルシェーダーの入力
struct PS_INPUT
{
    float4 Diffuse : COLOR0; // ディフューズカラー
    float2 TexCoords0 : TEXCOORD0; // テクスチャ座標
    float4 pos : SV_POSITION; // 座標( プロジェクション空間 )
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
    float4 color0 : SV_TARGET0; // 色
};

//画面サイズ
static int2 dispsize = { 0, 0 };


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


cbuffer cbLIGHTCAMERA_MATRIX : register(b4)
{
    matrix g_CamViewToWorldMatrix; // ライトのワールド　→　ビュー行列
    matrix g_LightProjectionMatrix; // ライトのビュー　　→　射影行列
};

cbuffer cbLIGHTCAMERA_MATRIX2 : register(b5)
{
    matrix g_LightViewMatrix1; // ライトのワールド　→　ビュー行列
    matrix g_LightProjectionMatrix1; // ライトのビュー　　→　射影行列
};

/*
cbuffer cbLIGHTCAMERA_MATRIX3 : register(b6)
{
    matrix g_LightViewMatrix2; // ライトのワールド　→　ビュー行列
    matrix g_LightProjectionMatrix2; // ライトのビュー　　→　射影行列
};
//*/

SamplerState g_DepthMapSampler : register(s0); // 深度マップサンプラ
Texture2D g_DepthMapTexture : register(t0); // 深度マップテクスチャ

SamplerState g_ShadowMapSampler : register(s1); // 深度バッファテクスチャ
Texture2D g_ShadowMapTexture : register(t1); // 深度バッファテクスチャ

/*
SamplerState g_ShadowFarMapSampler : register(s2); // 深度バッファテクスチャ
Texture2D g_ShadowFarMapTexture : register(t2); // 深度バッファテクスチャ
//*/

float3 DisptoProjNorm(float2 screenUV)
{
    float2 pos = screenUV;

    pos /= 0.5f;
    pos.x = pos.x - 1.f;
    pos.y = 1.f - pos.y;

    float3 position;
    position.x = pos.x * caminfo.z * dispsize.x / dispsize.y;
    position.y = pos.y * caminfo.z;
    position.z = 1.f;

    return position;
}

/*
// 行列の乗算を行う
float4 mul4x4(float4x4 m, float4 v)
{
    float4 result;
    result.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    result.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    result.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    result.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
    return result;
}
//*/

float3 GetLP(float4 lWorldPosition, float4x4 LV, float4x4 LP)
{
	// ライトのビュー座標をライトの射影座標に変換
    float4 lLViewPosition = mul(LV, lWorldPosition);
	// ライトからみた座標( xとyはライトの射影座標、zはビュー座標 )
    float2 LPPosition = mul(LP, lLViewPosition).xy *0.5f + 0.5f;
    // LPPosition.xy は -1.0f 〜 1.0f の値なので、これを 0.0f 〜 1.0f の値にする
    return float3(LPPosition.x, 1.0f - LPPosition.y, lLViewPosition.z);
}

PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
	//画面サイズを取得しておく
    g_DepthMapTexture.GetDimensions(dispsize.x, dispsize.y);

    float Depth = g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.TexCoords0).r;
    if (Depth <= 0.f)
    {
        Depth = 100000.f;
    }

    float3 ViewPositionOne = DisptoProjNorm(PSInput.TexCoords0);

    float4 lWorldPosition;
    float DepthS;

    float Total1 = 0.f;
    //float Total2 = 0.f;

    for (int i = 1; i <= 50; i++)
    {
        if (i <= caminfo.x)
        {
            lWorldPosition.xyz = ViewPositionOne * Depth * i / caminfo.x;
            lWorldPosition.w = 1.f;
            lWorldPosition = mul(g_CamViewToWorldMatrix, lWorldPosition);

            {
                float3 LPPosition = GetLP(lWorldPosition, g_LightViewMatrix1, g_LightProjectionMatrix1);
                DepthS = g_ShadowMapTexture.Sample(g_ShadowMapSampler, LPPosition.xy).r;
                if (((LPPosition.z - DepthS) > 0.f) && (DepthS > 0))
                {
                    Total1++;
                }
            }
            /*
            {
                float3 LPPosition = GetLP(lWorldPosition, g_LightViewMatrix2, g_LightProjectionMatrix2);
                DepthS = g_ShadowFarMapTexture.Sample(g_ShadowFarMapSampler, LPPosition.xy).r;
                if (((LPPosition.z - DepthS) > 0.f) && (DepthS > 0))
                {
                    Total2++;
                }
            }
            //*/
        }
    }
    Total1 /= caminfo.x;
    //Total2 /= caminfo.x;

    PSOutput.color0.rgb = 1.f - Total1;
    //PSOutput.color0.rgb = min(1.f - Total1, 1.f - Total2);
    PSOutput.color0.a = 1.f;

    return PSOutput;
}