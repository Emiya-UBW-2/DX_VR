// �C���N���[�h ---------------------------------

// �}�N����` -----------------------------------

#define DX_D3D11_COMMON_CONST_LIGHT_NUM			(6)		// ���ʃp�����[�^�̃��C�g�̍ő吔

// �\���̒�` --------------------------------------------------------------------

// �}�e���A���p�����[�^
struct DX_D3D11_CONST_MATERIAL
{
	float4 Diffuse; // �f�B�t���[�Y�J���[
	float4 Specular; // �X�y�L�����J���[
	float4 Ambient_Emissive; // �}�e���A���G�~�b�V�u�J���[ + �}�e���A���A���r�G���g�J���[ * �O���[�o���A���r�G���g�J���[

	float Power; // �X�y�L�����̋���
	float TypeParam0; // �}�e���A���^�C�v�p�����[�^0
	float TypeParam1; // �}�e���A���^�C�v�p�����[�^1
	float TypeParam2; // �}�e���A���^�C�v�p�����[�^2
};

// �t�H�O�p�����[�^
struct DX_D3D11_VS_CONST_FOG
{
	float LinearAdd; // �t�H�O�p�p�����[�^ end / ( end - start )
	float LinearDiv; // �t�H�O�p�p�����[�^ -1  / ( end - start )
	float Density; // �t�H�O�p�p�����[�^ density
	float E; // �t�H�O�p�p�����[�^ ���R�ΐ��̒�

	float4 Color; // �J���[
};

// ���C�g�p�����[�^
struct DX_D3D11_CONST_LIGHT
{
	int Type; // ���C�g�^�C�v( DX_LIGHTTYPE_POINT �Ȃ� )
	int3 Padding1; // �p�f�B���O�P

	float3 Position; // ���W( �r���[��� )
	float RangePow2; // �L�������̂Q��

	float3 Direction; // ����( �r���[��� )
	float FallOff; // �X�|�b�g���C�g�pFallOff

	float3 Diffuse; // �f�B�t���[�Y�J���[
	float SpotParam0; // �X�|�b�g���C�g�p�p�����[�^�O( cos( Phi / 2.0f ) )

	float3 Specular; // �X�y�L�����J���[
	float SpotParam1; // �X�|�b�g���C�g�p�p�����[�^�P( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

	float4 Ambient; // �A���r�G���g�J���[�ƃ}�e���A���̃A���r�G���g�J���[����Z��������

	float Attenuation0; // �����ɂ�錸�������p�p�����[�^�O
	float Attenuation1; // �����ɂ�錸�������p�p�����[�^�P
	float Attenuation2; // �����ɂ�錸�������p�p�����[�^�Q
	float Padding2; // �p�f�B���O�Q
};

// �s�N�Z���V�F�[�_�[�E���_�V�F�[�_�[���ʃp�����[�^
struct DX_D3D11_CONST_BUFFER_COMMON
{
    DX_D3D11_CONST_LIGHT Light[DX_D3D11_COMMON_CONST_LIGHT_NUM]; // ���C�g�p�����[�^
    DX_D3D11_CONST_MATERIAL Material; // �}�e���A���p�����[�^
    DX_D3D11_VS_CONST_FOG Fog; // �t�H�O�p�����[�^
};

// �}�N����` -----------------------------------

#define DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM			3			// �e�N�X�`�����W�ϊ��s��̓]�u�s��̐�
#define DX_D3D11_VS_CONST_WORLD_MAT_NUM					54			// �g���C�A���O�����X�g��œ����Ɏg�p���郍�[�J�������[���h�s��̍ő吔

// �f�[�^�^��` ---------------------------------

// ��{�p�����[�^
struct DX_D3D11_VS_CONST_BUFFER_BASE
{
	float4 AntiViewportMatrix[4]; // �A���`�r���[�|�[�g�s��
	float4 ProjectionMatrix[4]; // �r���[�@���@�v���W�F�N�V�����s��
	float4 ViewMatrix[3]; // ���[���h�@���@�r���[�s��
	float4 LocalWorldMatrix[3]; // ���[�J���@���@���[���h�s��

	float4 ToonOutLineSize; // �g�D�[���̗֊s���̑傫��
	float DiffuseSource; // �f�B�t���[�Y�J���[( 0.0f:�}�e���A��  1.0f:���_ )
	float SpecularSource; // �X�y�L�����J���[(   0.0f:�}�e���A��  1.0f:���_ )
	float MulSpecularColor; // �X�y�L�����J���[�l�ɏ�Z����l( �X�y�L�������������Ŏg�p )
	float Padding;
};

// ���̑��̍s��
struct DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX
{
	float4 ShadowMapLightViewProjectionMatrix[3][4]; // �V���h�E�}�b�v�p�̃��C�g�r���[�s��ƃ��C�g�ˉe�s�����Z��������
	float4 TextureMatrix[DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM][2]; // �e�N�X�`�����W����p�s��
};

// �X�L�j���O���b�V���p�́@���[�J���@���@���[���h�s��
struct DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX
{
	float4		lwMatrix[ DX_D3D11_VS_CONST_WORLD_MAT_NUM * 3 ] ;					// ���[�J���@���@���[���h�s��
};

// �\���̒�` --------------------------------------------------------------------

// �萔��` ----------------------------------------------------------------------

// ���_�V�F�[�_�[�E�s�N�Z���V�F�[�_�[���ʃp�����[�^
cbuffer cbD3D11_CONST_BUFFER_COMMON					: register( b0 )
{
	DX_D3D11_CONST_BUFFER_COMMON				g_Common ;
} ;

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_VS_BASE				: register( b1 )
{
	DX_D3D11_VS_CONST_BUFFER_BASE				g_Base ;
} ;

// ���̑��̍s��
cbuffer cbD3D11_CONST_BUFFER_VS_OTHERMATRIX			: register( b2 )
{
	DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX		g_OtherMatrix ;
} ;

// �X�L�j���O���b�V���p�́@���[�J���@���@���[���h�s��
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

// ���_�V�F�[�_�[�̓���
struct VS_INPUT
{
    float3 Position : POSITION; // ���W( ���[�J����� )
    float3 Normal : NORMAL0; // �@��( ���[�J����� )
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    float4 TexCoords0 : TEXCOORD0; // �e�N�X�`�����W
    float4 TexCoords1 : TEXCOORD1; // �T�u�e�N�X�`�����W

	// �o���v�}�b�v
    float3 Tan : TANGENT0; // �ڐ�( ���[�J����� )
    float3 Bin : BINORMAL0; // �]�@��( ���[�J����� )

	// �X�L�j���O���b�V��
    int4 BlendIndices0 : BLENDINDICES0; // �{�[�������p Float�^�萔�z��C���f�b�N�X�O
    float4 BlendWeight0 : BLENDWEIGHT0; // �{�[�������p�E�G�C�g�l�O
};

// ���_�V�F�[�_�[�̏o��
struct VS_OUTPUT
{
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    float2 TexCoords0_1 : TEXCOORD0; // xy:�e�N�X�`�����W
    float3 VPosition : TEXCOORD1; // ���W( �r���[��� )
    float3 VNormal : TEXCOORD2; // �@��( �r���[��� )
    float3 VTan : TEXCOORD3; // �ڐ�( �r���[��� )
    float3 VBin : TEXCOORD4; // �]�@��( �r���[��� )
	float3 normalW         : TEXCOORD5;
	float3 viewVecW        : TEXCOORD6;
	float4 Position : SV_POSITION; // ���W( �v���W�F�N�V������� )
};


Texture2D g_NormalMapTexture : register(t1);

// main�֐�
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

	// �e�N�X�`�����W�̃Z�b�g
    VSOutput.TexCoords0_1.x = dot(VSInput.TexCoords0, g_OtherMatrix.TextureMatrix[0][0]);
    VSOutput.TexCoords0_1.y = dot(VSInput.TexCoords0, g_OtherMatrix.TextureMatrix[0][1]);

	// �X�L�����b�V��

		// �u�����h�s��̍쐬
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

	// ���[�J�����W�̃Z�b�g
    lLocalPosition.xyz = VSInput.Position;
    lLocalPosition.w = 1.0f;

	// ���W�v�Z( ���[�J�����r���[���v���W�F�N�V���� )
    lWorldPosition.x = dot(lLocalPosition, lLocalWorldMatrix[0]);
    lWorldPosition.y = dot(lLocalPosition, lLocalWorldMatrix[1]);
    lWorldPosition.z = dot(lLocalPosition, lLocalWorldMatrix[2]);
    lWorldPosition.w = 1.0f;

    float Per = pow(1.f - saturate(abs(g_CameraPosW.y - lWorldPosition.y)), 2.f);
    float PrevY = lWorldPosition.y;
    //�Q���X�g�i�[�g
    lWorldPosition.y += CalcGerstnerWaveOffset(lWorldPosition.xyz).y;

    //����������
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
	
	// ���W( �r���[��� )��ۑ�
    VSOutput.VPosition = lViewPosition.xyz;
	
	// �@�����v�Z
    lWorldNrm.x = dot(VSInput.Normal, lLocalWorldMatrix[0].xyz);
    lWorldNrm.y = dot(VSInput.Normal, lLocalWorldMatrix[1].xyz);
    lWorldNrm.z = dot(VSInput.Normal, lLocalWorldMatrix[2].xyz);

    lViewNrm.x = dot(lWorldNrm, g_Base.ViewMatrix[0].xyz);
    lViewNrm.y = dot(lWorldNrm, g_Base.ViewMatrix[1].xyz);
    lViewNrm.z = dot(lWorldNrm, g_Base.ViewMatrix[2].xyz);

	// �@��( �r���[��� )��ۑ�
    VSOutput.VNormal = normalize(lViewNrm);

	// �]�@���A�ڐ����r���[��Ԃɓ��e����
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
		
	// �]�@���A�ڐ�( �r���[��� )��ۑ�
    VSOutput.VTan = normalize(lViewTan);
    VSOutput.VBin = normalize(lViewBin);

	// �f�B�t���[�Y�J���[���Z�b�g
    VSOutput.Diffuse = g_Base.DiffuseSource > 0.5f ? VSInput.Diffuse : g_Common.Material.Diffuse;
	// �X�y�L�����J���[���Z�b�g
    VSOutput.Specular = (g_Base.SpecularSource > 0.5f ? VSInput.Specular : g_Common.Material.Specular) * g_Base.MulSpecularColor;

	VSOutput.viewVecW = lWorldPosition.xyz - g_CameraPosW.xyz;
	VSOutput.normalW = lWorldNrm;

    return VSOutput;
}

