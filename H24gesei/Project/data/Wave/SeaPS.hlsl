#define DX_D3D11_COMMON_CONST_LIGHT_NUM			(6)		// 共通パラメータのライトの最大数

// 構造体定義 --------------------------------------------------------------------

// マテリアルパラメータ
struct DX_D3D11_CONST_MATERIAL
{
    float4 Diffuse; // ディフューズカラー
    float4 Specular; // スペキュラカラー
    float4 Ambient_Emissive; // マテリアルエミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー

    float Power; // スペキュラの強さ
    float TypeParam0; // マテリアルタイプパラメータ0
    float TypeParam1; // マテリアルタイプパラメータ1
    float TypeParam2; // マテリアルタイプパラメータ2
};

// フォグパラメータ
struct DX_D3D11_VS_CONST_FOG
{
    float LinearAdd; // フォグ用パラメータ end / ( end - start )
    float LinearDiv; // フォグ用パラメータ -1  / ( end - start )
    float Density; // フォグ用パラメータ density
    float E; // フォグ用パラメータ 自然対数の低

    float4 Color; // カラー
};

// ライトパラメータ
struct DX_D3D11_CONST_LIGHT
{
    int Type; // ライトタイプ( DX_LIGHTTYPE_POINT など )
    int3 Padding1; // パディング１

    float3 Position; // 座標( ビュー空間 )
    float RangePow2; // 有効距離の２乗

    float3 Direction; // 方向( ビュー空間 )
    float FallOff; // スポットライト用FallOff

    float3 Diffuse; // ディフューズカラー
    float SpotParam0; // スポットライト用パラメータ０( cos( Phi / 2.0f ) )

    float3 Specular; // スペキュラカラー
    float SpotParam1; // スポットライト用パラメータ１( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

    float4 Ambient; // アンビエントカラーとマテリアルのアンビエントカラーを乗算したもの

    float Attenuation0; // 距離による減衰処理用パラメータ０
    float Attenuation1; // 距離による減衰処理用パラメータ１
    float Attenuation2; // 距離による減衰処理用パラメータ２
    float Padding2; // パディング２
};

// ピクセルシェーダー・頂点シェーダー共通パラメータ
struct DX_D3D11_CONST_BUFFER_COMMON
{
    DX_D3D11_CONST_LIGHT Light[DX_D3D11_COMMON_CONST_LIGHT_NUM]; // ライトパラメータ
    DX_D3D11_CONST_MATERIAL Material; // マテリアルパラメータ
    DX_D3D11_VS_CONST_FOG Fog; // フォグパラメータ
};

// 定数定義 ----------------------------------------------------------------------

// 頂点シェーダー・ピクセルシェーダー共通パラメータ
cbuffer cbD3D11_CONST_BUFFER_COMMON : register(b0)
{
    DX_D3D11_CONST_BUFFER_COMMON g_Common;
};

// 関数定義 ----------------------------------------------------------------------

// ピクセルシェーダーの入力
struct PS_INPUT
{
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float4 TexCoords0_1 : TEXCOORD0; // xy:テクスチャ座標 zw:サブテクスチャ座標
    float3 VPosition : TEXCOORD1; // 座標( ビュー空間 )
    float3 VNormal : TEXCOORD2; // 法線( ビュー空間 )
    float3 VTan : TEXCOORD3; // 接線( ビュー空間 )
    float3 VBin : TEXCOORD4; // 従法線( ビュー空間 )
	float3 normalW         : TEXCOORD5;
	float3 viewVecW        : TEXCOORD6;
	float4 Position : SV_POSITION; // 座標( プロジェクション空間 )
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
    float4 Color0 : SV_TARGET0; // 色
    float4 Normal : SV_TARGET1; // 法線( ビュー空間 )
    float4 Depth : SV_TARGET2; // 深度
};



SamplerState g_DiffuseMapSampler : register(s0); // ディフューズマップテクスチャ
Texture2D g_DiffuseMapTexture : register(t0); // ディフューズマップテクスチャ

SamplerState g_NormalMapSampler : register(s1); // 法線マップテクスチャ
Texture2D g_NormalMapTexture : register(t1); // 法線マップテクスチャ

SamplerState g_SpecularMapSampler : register(s2); // スペキュラマップテクスチャ
Texture2D g_SpecularMapTexture : register(t2); // スペキュラマップテクスチャ

//水を除いた描画結果
SamplerState g_ScreenColorSampler : register(s3);
Texture2D g_ScreenColorTexture : register(t3);

//水を除いた深度
SamplerState g_ScreenDepthSampler : register(s4);
Texture2D g_ScreenDepthTexture : register(t4);


SamplerState dynamicCubeMapSampler     : register(s5);
TextureCube  dynamicCubeMapTexture     : register(t5);

// main関数
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    float4 TextureDiffuseColor;
	
    float DiffuseAngleGen;
    float3 TotalDiffuse;
    float3 TotalSpecular;
    float Temp;
    float3 TempF3;
    float3 Normal;
    float3 V_to_Eye;
    float3 lLightTemp;
    float3 lLightDir;

	// 頂点座標から視点へのベクトルを接底空間に投影した後正規化して保存
    TempF3.x = dot(PSInput.VTan, -PSInput.VPosition.xyz);
    TempF3.y = dot(PSInput.VBin, -PSInput.VPosition.xyz);
    TempF3.z = dot(PSInput.VNormal, -PSInput.VPosition.xyz);
    V_to_Eye = normalize(TempF3);

		// 法線の 0～1 の値を -1.0～1.0 に変換する
    Normal = (g_NormalMapTexture.Sample(g_NormalMapSampler, PSInput.TexCoords0_1.xy).rgb - 0.5f) * 2.0f;

	// ディフューズテクスチャカラーを取得
    //TextureDiffuseColor = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, PSInput.TexCoords0_1.xy);
	
    
	// ライトを使う場合 *******************************************************************( 開始 )

	// ディフューズカラーとスペキュラカラーの蓄積値を初期化
    TotalDiffuse = 0.0f;
    TotalSpecular = 0.0f;
	
    // ライト0の処理 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )
        lLightDir = g_Common.Light[0].Direction;
	    // ディフューズ色計算

        lLightTemp.x = dot(PSInput.VTan, -lLightDir);
        lLightTemp.y = dot(PSInput.VBin, -lLightDir);
        lLightTemp.z = dot(PSInput.VNormal, -lLightDir);
        DiffuseAngleGen = saturate(dot(Normal, normalize(lLightTemp.xyz)));
	
		    // ディフューズカラー蓄積値 += ( ライトのディフューズカラー * ディフューズカラー * ディフューズカラー角度減衰率 + ライトのアンビエントカラーとマテリアルのアンビエントカラーを乗算したもの  ) * 距離・スポットライトの角度減衰率
        TotalDiffuse += (g_Common.Light[0].Diffuse * PSInput.Diffuse.xyz * DiffuseAngleGen + g_Common.Light[0].Ambient.xyz);


	    // スペキュラカラー計算
		    // ハーフベクトルの計算
        TempF3 = normalize(V_to_Eye - lLightDir);
		
		    // Temp = pow( max( 0.0f, N * H ), g_Material.Power.x )
        Temp = pow(max(0.0f, dot(Normal, TempF3)), g_Common.Material.Power);

		    // スペキュラカラー蓄積値 += Temp * 距離・スポットライトの角度減衰率 * ライトのスペキュラカラー
        //TotalSpecular += Temp * g_Common.Light[0].Specular;
    // ライト0の処理 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )

    float3 _SeaBaseColor = float3(27.f / 255.f, 57.f / 255.f, 77.f / 255.f);
    float3 _SeaShallowColor = float3(75.f / 255.f, 89.f / 255.f, 35.f / 255.f);
    float _BaseColorStrength = (0.9);
    float _ColorHightOffset = (0.15);
    float _ShallowColorStrength = (0.35);

    float3 sea_base_color = _SeaBaseColor * DiffuseAngleGen * _BaseColorStrength + lerp(_SeaBaseColor, _SeaShallowColor * _ShallowColorStrength, DiffuseAngleGen);
    
    float wave_height = (g_NormalMapTexture.Sample(g_NormalMapSampler, PSInput.TexCoords0_1.xy).a - 0.5f) * 0.25f;
    
    TextureDiffuseColor.rgb = _SeaBaseColor * DiffuseAngleGen * _BaseColorStrength + _SeaShallowColor * (wave_height * 0.5 + 0.2) * _ColorHightOffset;
    TextureDiffuseColor.a = 1.f;


    int2 dispsize;
    g_ScreenDepthTexture.GetDimensions(dispsize.x, dispsize.y);
    float2 TexCoords;
    TexCoords = (PSInput.Position.xy / dispsize);
	
    float Per = 1.f;

    float PrevDepth = g_ScreenDepthTexture.Sample(g_ScreenDepthSampler, TexCoords).r;

    if (PrevDepth > 0)
    {
		//Zテスト
        if (PrevDepth < PSInput.VPosition.z)
        {
            discard;
        }
		//グラデーション
        else
        {
            Per = saturate(abs(PrevDepth - PSInput.VPosition.z) * 0.05f);
        }
	}
	else {
		if (PSInput.normalW.y < -0.5f) {
			Per = 0.f;
		}

	}

	// TotalDiffuse = ライトディフューズカラー蓄積値 + ( マテリアルのアンビエントカラーとグローバルアンビエントカラーを乗算したものとマテリアルエミッシブカラーを加算したもの )
    TotalDiffuse += g_Common.Material.Ambient_Emissive.rgb;
	// 出力カラー = TotalDiffuse * テクスチャカラー
    PSOutput.Color0.rgb = TotalDiffuse * TextureDiffuseColor.rgb;
	
	//アルファ値
    PSOutput.Color0.a = 1.f;



	// フレネル反射
    float alpha = saturate(abs(dot(-normalize(PSInput.VPosition.xyz), PSInput.VNormal)) * 3.f);

    float3 RefColor = dynamicCubeMapTexture.Sample(dynamicCubeMapSampler, reflect(PSInput.viewVecW, PSInput.normalW)).rgb;
	if (PSInput.normalW.y < -0.5f) {
		RefColor = _SeaBaseColor;

		//屈折表現
		PSOutput.Color0.rgb = lerp(g_ScreenColorTexture.Sample(g_ScreenColorSampler, TexCoords + Normal.xy * 0.6f).rgb, PSOutput.Color0.rgb, Per);

		PSOutput.Color0.rgb = lerp(RefColor, PSOutput.Color0.rgb, alpha);
	}
	else {
		PSOutput.Color0.rgb = lerp(RefColor, PSOutput.Color0.rgb, alpha);

		//屈折表現
		PSOutput.Color0.rgb = lerp(g_ScreenColorTexture.Sample(g_ScreenColorSampler, TexCoords + Normal.xy * 0.6f).rgb, PSOutput.Color0.rgb, Per);
	}


    PSOutput.Color0.rgb += TotalSpecular;


    PSOutput.Normal.x = (Normal.x + 1.0) / 2.0;
    PSOutput.Normal.y = (Normal.y + 1.0) / 2.0;
    PSOutput.Normal.z = (Normal.z + 1.0) / 2.0;
    PSOutput.Normal.w = 1.0;

    float4 TextureSpecularColor;
    TextureSpecularColor.xyzw = 0.0;

    TextureSpecularColor.xyz = g_SpecularMapTexture.Sample(g_SpecularMapSampler, TexCoords).xyz * PSInput.Specular.xyz;

    PSOutput.Depth.x = PSInput.VPosition.z;
    PSOutput.Depth.y = 0.5f;
    PSOutput.Depth.z = 0.0;
    PSOutput.Depth.w = 1.0;


    return PSOutput;
}
