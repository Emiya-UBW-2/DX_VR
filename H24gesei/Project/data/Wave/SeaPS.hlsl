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

// �萔��` ----------------------------------------------------------------------

// ���_�V�F�[�_�[�E�s�N�Z���V�F�[�_�[���ʃp�����[�^
cbuffer cbD3D11_CONST_BUFFER_COMMON : register(b0)
{
    DX_D3D11_CONST_BUFFER_COMMON g_Common;
};

// �֐���` ----------------------------------------------------------------------

// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    float4 TexCoords0_1 : TEXCOORD0; // xy:�e�N�X�`�����W zw:�T�u�e�N�X�`�����W
    float3 VPosition : TEXCOORD1; // ���W( �r���[��� )
    float3 VNormal : TEXCOORD2; // �@��( �r���[��� )
    float3 VTan : TEXCOORD3; // �ڐ�( �r���[��� )
    float3 VBin : TEXCOORD4; // �]�@��( �r���[��� )
	float3 normalW         : TEXCOORD5;
	float3 viewVecW        : TEXCOORD6;
	float4 Position : SV_POSITION; // ���W( �v���W�F�N�V������� )
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT
{
    float4 Color0 : SV_TARGET0; // �F
    float4 Normal : SV_TARGET1; // �@��( �r���[��� )
    float4 Depth : SV_TARGET2; // �[�x
};



SamplerState g_DiffuseMapSampler : register(s0); // �f�B�t���[�Y�}�b�v�e�N�X�`��
Texture2D g_DiffuseMapTexture : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

SamplerState g_NormalMapSampler : register(s1); // �@���}�b�v�e�N�X�`��
Texture2D g_NormalMapTexture : register(t1); // �@���}�b�v�e�N�X�`��

SamplerState g_SpecularMapSampler : register(s2); // �X�y�L�����}�b�v�e�N�X�`��
Texture2D g_SpecularMapTexture : register(t2); // �X�y�L�����}�b�v�e�N�X�`��

//�����������`�挋��
SamplerState g_ScreenColorSampler : register(s3);
Texture2D g_ScreenColorTexture : register(t3);

//�����������[�x
SamplerState g_ScreenDepthSampler : register(s4);
Texture2D g_ScreenDepthTexture : register(t4);


SamplerState dynamicCubeMapSampler     : register(s5);
TextureCube  dynamicCubeMapTexture     : register(t5);

// main�֐�
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

	// ���_���W���王�_�ւ̃x�N�g����ڒ��Ԃɓ��e�����㐳�K�����ĕۑ�
    TempF3.x = dot(PSInput.VTan, -PSInput.VPosition.xyz);
    TempF3.y = dot(PSInput.VBin, -PSInput.VPosition.xyz);
    TempF3.z = dot(PSInput.VNormal, -PSInput.VPosition.xyz);
    V_to_Eye = normalize(TempF3);

		// �@���� 0�`1 �̒l�� -1.0�`1.0 �ɕϊ�����
    Normal = (g_NormalMapTexture.Sample(g_NormalMapSampler, PSInput.TexCoords0_1.xy).rgb - 0.5f) * 2.0f;

	// �f�B�t���[�Y�e�N�X�`���J���[���擾
    //TextureDiffuseColor = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, PSInput.TexCoords0_1.xy);
	
    
	// ���C�g���g���ꍇ *******************************************************************( �J�n )

	// �f�B�t���[�Y�J���[�ƃX�y�L�����J���[�̒~�ϒl��������
    TotalDiffuse = 0.0f;
    TotalSpecular = 0.0f;
	
    // ���C�g0�̏��� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( �J�n )
        lLightDir = g_Common.Light[0].Direction;
	    // �f�B�t���[�Y�F�v�Z

        lLightTemp.x = dot(PSInput.VTan, -lLightDir);
        lLightTemp.y = dot(PSInput.VBin, -lLightDir);
        lLightTemp.z = dot(PSInput.VNormal, -lLightDir);
        DiffuseAngleGen = saturate(dot(Normal, normalize(lLightTemp.xyz)));
	
		    // �f�B�t���[�Y�J���[�~�ϒl += ( ���C�g�̃f�B�t���[�Y�J���[ * �f�B�t���[�Y�J���[ * �f�B�t���[�Y�J���[�p�x������ + ���C�g�̃A���r�G���g�J���[�ƃ}�e���A���̃A���r�G���g�J���[����Z��������  ) * �����E�X�|�b�g���C�g�̊p�x������
        TotalDiffuse += (g_Common.Light[0].Diffuse * PSInput.Diffuse.xyz * DiffuseAngleGen + g_Common.Light[0].Ambient.xyz);


	    // �X�y�L�����J���[�v�Z
		    // �n�[�t�x�N�g���̌v�Z
        TempF3 = normalize(V_to_Eye - lLightDir);
		
		    // Temp = pow( max( 0.0f, N * H ), g_Material.Power.x )
        Temp = pow(max(0.0f, dot(Normal, TempF3)), g_Common.Material.Power);

		    // �X�y�L�����J���[�~�ϒl += Temp * �����E�X�|�b�g���C�g�̊p�x������ * ���C�g�̃X�y�L�����J���[
        //TotalSpecular += Temp * g_Common.Light[0].Specular;
    // ���C�g0�̏��� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( �I�� )

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
		//Z�e�X�g
        if (PrevDepth < PSInput.VPosition.z)
        {
            discard;
        }
		//�O���f�[�V����
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

	// TotalDiffuse = ���C�g�f�B�t���[�Y�J���[�~�ϒl + ( �}�e���A���̃A���r�G���g�J���[�ƃO���[�o���A���r�G���g�J���[����Z�������̂ƃ}�e���A���G�~�b�V�u�J���[�����Z�������� )
    TotalDiffuse += g_Common.Material.Ambient_Emissive.rgb;
	// �o�̓J���[ = TotalDiffuse * �e�N�X�`���J���[
    PSOutput.Color0.rgb = TotalDiffuse * TextureDiffuseColor.rgb;
	
	//�A���t�@�l
    PSOutput.Color0.a = 1.f;



	// �t���l������
    float alpha = saturate(abs(dot(-normalize(PSInput.VPosition.xyz), PSInput.VNormal)) * 3.f);

    float3 RefColor = dynamicCubeMapTexture.Sample(dynamicCubeMapSampler, reflect(PSInput.viewVecW, PSInput.normalW)).rgb;
	if (PSInput.normalW.y < -0.5f) {
		RefColor = _SeaBaseColor;

		//���ܕ\��
		PSOutput.Color0.rgb = lerp(g_ScreenColorTexture.Sample(g_ScreenColorSampler, TexCoords + Normal.xy * 0.6f).rgb, PSOutput.Color0.rgb, Per);

		PSOutput.Color0.rgb = lerp(RefColor, PSOutput.Color0.rgb, alpha);
	}
	else {
		PSOutput.Color0.rgb = lerp(RefColor, PSOutput.Color0.rgb, alpha);

		//���ܕ\��
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
