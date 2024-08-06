// インクルード ---------------------------------

// マクロ定義 -----------------------------------

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

// マクロ定義 -----------------------------------

#define DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM			3			// テクスチャ座標変換行列の転置行列の数
#define DX_D3D11_VS_CONST_WORLD_MAT_NUM					54			// トライアングルリスト一つで同時に使用するローカル→ワールド行列の最大数

// データ型定義 ---------------------------------

// 基本パラメータ
struct DX_D3D11_VS_CONST_BUFFER_BASE
{
	float4 AntiViewportMatrix[4]; // アンチビューポート行列
	float4 ProjectionMatrix[4]; // ビュー　→　プロジェクション行列
	float4 ViewMatrix[3]; // ワールド　→　ビュー行列
	float4 LocalWorldMatrix[3]; // ローカル　→　ワールド行列

	float4 ToonOutLineSize; // トゥーンの輪郭線の大きさ
	float DiffuseSource; // ディフューズカラー( 0.0f:マテリアル  1.0f:頂点 )
	float SpecularSource; // スペキュラカラー(   0.0f:マテリアル  1.0f:頂点 )
	float MulSpecularColor; // スペキュラカラー値に乗算する値( スペキュラ無効処理で使用 )
	float Padding;
};

// その他の行列
struct DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX
{
	float4 ShadowMapLightViewProjectionMatrix[3][4]; // シャドウマップ用のライトビュー行列とライト射影行列を乗算したもの
	float4 TextureMatrix[DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM][2]; // テクスチャ座標操作用行列
};

// スキニングメッシュ用の　ローカル　→　ワールド行列
struct DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX
{
	float4		lwMatrix[ DX_D3D11_VS_CONST_WORLD_MAT_NUM * 3 ] ;					// ローカル　→　ワールド行列
};

// 構造体定義 --------------------------------------------------------------------

// 定数定義 ----------------------------------------------------------------------

// 頂点シェーダー・ピクセルシェーダー共通パラメータ
cbuffer cbD3D11_CONST_BUFFER_COMMON					: register( b0 )
{
	DX_D3D11_CONST_BUFFER_COMMON				g_Common ;
} ;

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_VS_BASE				: register( b1 )
{
	DX_D3D11_VS_CONST_BUFFER_BASE				g_Base ;
} ;

// その他の行列
cbuffer cbD3D11_CONST_BUFFER_VS_OTHERMATRIX			: register( b2 )
{
	DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX		g_OtherMatrix ;
} ;

// スキニングメッシュ用の　ローカル　→　ワールド行列
cbuffer cbD3D11_CONST_BUFFER_VS_LOCALWORLDMATRIX	: register( b3 )
{
	DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX	g_LocalWorldMatrix ;
} ;


cbuffer cbCAMERAPOSW								: register(b4) {
	float4										g_CameraPosW;
};

struct Wave
{
    float2 dir;
    float amplitude;
    float waveLength;
};

cbuffer cbFree_CBUFFER2 : register(b5)
{
    Wave waves[20];
};


float3 CalcGerstnerWaveOffset(float3 v)
{
    static int numWaves = 20;
    static float steepness = 0.8;
    static float speed = 15;
    float3 sum = float3(0, 0, 0);
	[unroll]
    for (int i = 0; i < numWaves; i++)
    {
        Wave wave = waves[i];
        float wi = 2 / wave.waveLength;
        float Qi = steepness / (wave.amplitude * wi * numWaves);
        float phi = speed * wi;
        float rad = dot(wave.dir, v.xz) * wi + g_CameraPosW.w * phi; //g_CameraPosW.w==time
        sum.y += sin(rad) * wave.amplitude;
        sum.xz += cos(rad) * wave.amplitude * Qi * wave.dir;
    }
    return sum;
}

// 頂点シェーダーの入力
struct VS_INPUT
{
    float3 Position : POSITION; // 座標( ローカル空間 )
    float3 Normal : NORMAL0; // 法線( ローカル空間 )
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float4 TexCoords0 : TEXCOORD0; // テクスチャ座標
    float4 TexCoords1 : TEXCOORD1; // サブテクスチャ座標

	// バンプマップ
    float3 Tan : TANGENT0; // 接線( ローカル空間 )
    float3 Bin : BINORMAL0; // 従法線( ローカル空間 )

	// スキニングメッシュ
    int4 BlendIndices0 : BLENDINDICES0; // ボーン処理用 Float型定数配列インデックス０
    float4 BlendWeight0 : BLENDWEIGHT0; // ボーン処理用ウエイト値０
};

// 頂点シェーダーの出力
struct VS_OUTPUT
{
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float2 TexCoords0_1 : TEXCOORD0; // xy:テクスチャ座標
    float3 VPosition : TEXCOORD1; // 座標( ビュー空間 )
    float3 VNormal : TEXCOORD2; // 法線( ビュー空間 )
    float3 VTan : TEXCOORD3; // 接線( ビュー空間 )
    float3 VBin : TEXCOORD4; // 従法線( ビュー空間 )
	float3 normalW         : TEXCOORD5;
	float3 viewVecW        : TEXCOORD6;
	float4 Position : SV_POSITION; // 座標( プロジェクション空間 )
};


Texture2D g_NormalMapTexture : register(t1);

// main関数
VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT VSOutput;
    int4 lBoneFloatIndex;
    float4 lLocalWorldMatrix[3];
    float4 lLocalPosition;
    float4 lWorldPosition;
    float4 lViewPosition;
    float3 lWorldNrm;
    float3 lWorldTan;
    float3 lWorldBin;
    float3 lViewNrm;
    float3 lViewTan;
    float3 lViewBin;

	// テクスチャ座標のセット
    VSOutput.TexCoords0_1.x = dot(VSInput.TexCoords0, g_OtherMatrix.TextureMatrix[0][0]);
    VSOutput.TexCoords0_1.y = dot(VSInput.TexCoords0, g_OtherMatrix.TextureMatrix[0][1]);

	// スキンメッシュ

		// ブレンド行列の作成
		lBoneFloatIndex = VSInput.BlendIndices0 ;
		lLocalWorldMatrix[ 0 ]  = g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.x + 0 ] * VSInput.BlendWeight0.xxxx;
		lLocalWorldMatrix[ 1 ]  = g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.x + 1 ] * VSInput.BlendWeight0.xxxx;
		lLocalWorldMatrix[ 2 ]  = g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.x + 2 ] * VSInput.BlendWeight0.xxxx;

		lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.y + 0 ] * VSInput.BlendWeight0.yyyy;
		lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.y + 1 ] * VSInput.BlendWeight0.yyyy;
		lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.y + 2 ] * VSInput.BlendWeight0.yyyy;

		lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.z + 0 ] * VSInput.BlendWeight0.zzzz;
		lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.z + 1 ] * VSInput.BlendWeight0.zzzz;
		lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.z + 2 ] * VSInput.BlendWeight0.zzzz;

		lLocalWorldMatrix[ 0 ] += g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.w + 0 ] * VSInput.BlendWeight0.wwww;
		lLocalWorldMatrix[ 1 ] += g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.w + 1 ] * VSInput.BlendWeight0.wwww;
		lLocalWorldMatrix[ 2 ] += g_LocalWorldMatrix.lwMatrix[ lBoneFloatIndex.w + 2 ] * VSInput.BlendWeight0.wwww;

	// ローカル座標のセット
    lLocalPosition.xyz = VSInput.Position;
    lLocalPosition.w = 1.0f;

	// 座標計算( ローカル→ビュー→プロジェクション )
    lWorldPosition.x = dot(lLocalPosition, lLocalWorldMatrix[0]);
    lWorldPosition.y = dot(lLocalPosition, lLocalWorldMatrix[1]);
    lWorldPosition.z = dot(lLocalPosition, lLocalWorldMatrix[2]);
    lWorldPosition.w = 1.0f;

    float Per = pow(1.f - saturate(abs(g_CameraPosW.y - lWorldPosition.y)), 2.f);
    float PrevY = lWorldPosition.y;
    //ゲルストナー波
    lWorldPosition.y += CalcGerstnerWaveOffset(lWorldPosition.xyz).y;

    //高さを加味
    int2 dispsize;
    g_NormalMapTexture.GetDimensions(dispsize.x, dispsize.y);
    float3 TexPosition;
    TexPosition.xy = VSInput.TexCoords0.xy * dispsize;
    TexPosition.z = 0;
    lWorldPosition.y += (g_NormalMapTexture.Load(TexPosition).a-0.5f) * 0.25f;

    lWorldPosition.y = lWorldPosition.y + (PrevY - lWorldPosition.y) * Per;
    
    lViewPosition.x = dot(lWorldPosition, g_Base.ViewMatrix[0]);
    lViewPosition.y = dot(lWorldPosition, g_Base.ViewMatrix[1]);
    lViewPosition.z = dot(lWorldPosition, g_Base.ViewMatrix[2]);
    lViewPosition.w = 1.0f;

    VSOutput.Position.x = dot(lViewPosition, g_Base.ProjectionMatrix[0]);
    VSOutput.Position.y = dot(lViewPosition, g_Base.ProjectionMatrix[1]);
    VSOutput.Position.z = dot(lViewPosition, g_Base.ProjectionMatrix[2]);
    VSOutput.Position.w = dot(lViewPosition, g_Base.ProjectionMatrix[3]);
	
	// 座標( ビュー空間 )を保存
    VSOutput.VPosition = lViewPosition.xyz;
	
	// 法線を計算
    lWorldNrm.x = dot(VSInput.Normal, lLocalWorldMatrix[0].xyz);
    lWorldNrm.y = dot(VSInput.Normal, lLocalWorldMatrix[1].xyz);
    lWorldNrm.z = dot(VSInput.Normal, lLocalWorldMatrix[2].xyz);

    lViewNrm.x = dot(lWorldNrm, g_Base.ViewMatrix[0].xyz);
    lViewNrm.y = dot(lWorldNrm, g_Base.ViewMatrix[1].xyz);
    lViewNrm.z = dot(lWorldNrm, g_Base.ViewMatrix[2].xyz);

	// 法線( ビュー空間 )を保存
    VSOutput.VNormal = normalize(lViewNrm);

	// 従法線、接線をビュー空間に投影する
    lWorldTan.x = dot(VSInput.Tan, lLocalWorldMatrix[0].xyz);
    lWorldTan.y = dot(VSInput.Tan, lLocalWorldMatrix[1].xyz);
    lWorldTan.z = dot(VSInput.Tan, lLocalWorldMatrix[2].xyz);

    lWorldBin.x = dot(VSInput.Bin, lLocalWorldMatrix[0].xyz);
    lWorldBin.y = dot(VSInput.Bin, lLocalWorldMatrix[1].xyz);
    lWorldBin.z = dot(VSInput.Bin, lLocalWorldMatrix[2].xyz);

    lViewTan.x = dot(lWorldTan, g_Base.ViewMatrix[0].xyz);
    lViewTan.y = dot(lWorldTan, g_Base.ViewMatrix[1].xyz);
    lViewTan.z = dot(lWorldTan, g_Base.ViewMatrix[2].xyz);

    lViewBin.x = dot(lWorldBin, g_Base.ViewMatrix[0].xyz);
    lViewBin.y = dot(lWorldBin, g_Base.ViewMatrix[1].xyz);
    lViewBin.z = dot(lWorldBin, g_Base.ViewMatrix[2].xyz);
		
	// 従法線、接線( ビュー空間 )を保存
    VSOutput.VTan = normalize(lViewTan);
    VSOutput.VBin = normalize(lViewBin);

	// ディフューズカラーをセット
    VSOutput.Diffuse = g_Base.DiffuseSource > 0.5f ? VSInput.Diffuse : g_Common.Material.Diffuse;
	// スペキュラカラーをセット
    VSOutput.Specular = (g_Base.SpecularSource > 0.5f ? VSInput.Specular : g_Common.Material.Specular) * g_Base.MulSpecularColor;

	VSOutput.viewVecW = lWorldPosition.xyz - g_CameraPosW.xyz;
	VSOutput.normalW = lWorldNrm;

    return VSOutput;
}

