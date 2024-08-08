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

//1フレーム前の描画結果
SamplerState g_Sampler : register(s0);
Texture2D g_Texture : register(t0);

//法線マップ作成
float4 BumpMap(float2 TexCoords0)
{
    float4 Out;

    //上下左右のテクセル位置の高さを取得する
    float H1 = g_Texture.Sample(g_Sampler, TexCoords0, float2(1, 0)).r;
    float H2 = g_Texture.Sample(g_Sampler, TexCoords0, float2(0, 1)).r;
    float H3 = g_Texture.Sample(g_Sampler, TexCoords0, float2(-1, 0)).r;
    float H4 = g_Texture.Sample(g_Sampler, TexCoords0, float2(0, -1)).r;
 
    //X 方向の高さの変化量を計算する
    //波の高さ情報は -1.0f ～ 1.0f の範囲で格納されているので 0.0f ～ 1.0f に変換する
    float tu = 0.5f * (H3 - H1) + 0.5f;

    //Y 方向の高さの変化量を計算する
    //波の高さ情報は -1.0f ～ 1.0f の範囲で格納されているので 0.0f ～ 1.0f に変換する
    float tv = 0.5f * (H4 - H2) + 0.5f;
 
    //視差マッピングは高さ情報も使用する。そのためα成分に高さ情報を格納する
    Out = float4(tu, tv, 1.0f, g_Texture.Sample(g_Sampler, TexCoords0).r);

    return Out;
}
// main関数
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    PSOutput.color0 = BumpMap(PSInput.texCoords0);
	// 出力パラメータを返す
    return PSOutput;
}

