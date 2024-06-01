// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
	float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
	float2 TexCoords0 : TEXCOORD0; // �e�N�X�`�����W
	float4 pos         : SV_POSITION;   // ���W( �v���W�F�N�V������� )
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT
{
	float4 color0 : SV_TARGET0; // �F
};

// �萔�o�b�t�@�s�N�Z���V�F�[�_�[��{�p�����[�^
struct DX_D3D11_PS_CONST_BUFFER_BASE
{
	float4 FactorColor; // �A���t�@�l��

	float MulAlphaColor; // �J���[�ɃA���t�@�l����Z���邩�ǂ���( 0.0f:��Z���Ȃ�  1.0f:��Z���� )
	float AlphaTestRef; // �A���t�@�e�X�g�Ŏg�p�����r�l
	float2 Padding1;

	int AlphaTestCmpMode; // �A���t�@�e�X�g��r���[�h( DX_CMP_NEVER �Ȃ� )
	int3 Padding2;

	float4 IgnoreTextureColor; // �e�N�X�`���J���[���������p�J���[
};

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1)
{
	DX_D3D11_PS_CONST_BUFFER_BASE g_Base;
};

// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^2
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3)
{
	float4 caminfo;
}


cbuffer cbLIGHTCAMERA_MATRIX						: register(b4) {
	matrix		g_LightViewMatrix;			// ���C�g�̃��[���h�@���@�r���[�s��
	matrix		g_LightProjectionMatrix;	// ���C�g�̃r���[�@�@���@�ˉe�s��
};

cbuffer cbLIGHTCAMERA_MATRIX2						: register(b5) {
	matrix		g_LightViewMatrix1;			// ���C�g�̃��[���h�@���@�r���[�s��
	matrix		g_LightProjectionMatrix1;	// ���C�g�̃r���[�@�@���@�ˉe�s��
};

cbuffer cbLIGHTCAMERA_MATRIX3						: register(b6) {
	matrix		g_LightViewMatrix2;			// ���C�g�̃��[���h�@���@�r���[�s��
	matrix		g_LightProjectionMatrix2;	// ���C�g�̃r���[�@�@���@�ˉe�s��
};

SamplerState g_DiffuseMapSampler : register(s0); // �f�B�t���[�Y�}�b�v�T���v��
Texture2D g_DiffuseMapTexture : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

SamplerState g_DepthMapSampler : register(s1); // �[�x�}�b�v�T���v��
Texture2D g_DepthMapTexture : register(t1); // �[�x�}�b�v�e�N�X�`��

SamplerState g_ShadowMapSampler : register(s2); // �[�x�o�b�t�@�e�N�X�`��
Texture2D g_ShadowMapTexture : register(t2); // �[�x�o�b�t�@�e�N�X�`��

SamplerState g_ShadowFarMapSampler : register(s3); // �[�x�o�b�t�@�e�N�X�`��
Texture2D g_ShadowFarMapTexture : register(t3); // �[�x�o�b�t�@�e�N�X�`��

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
	float4 LPPosition; // ���C�g����݂����W( x��y�̓��C�g�̎ˉe���W�Az�̓r���[���W )
	float2 DepthTexCoord;

	float Total = 0.f;
	float Div = 50.f;
	for (int i = 0;i < Div;i++) {
		lWorldPosition.xyz = ViewPositionOne * (Depth * i / Div);
		lWorldPosition.w = 0.f;
		lWorldPosition = mul(g_LightViewMatrix, lWorldPosition);

		// �[�x�e�p�̃��C�g���猩���ˉe���W���Z�o ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( �J�n )

		// ���C�g�̃r���[���W�����C�g�̎ˉe���W�ɕϊ�
		LPPosition = mul(g_LightProjectionMatrix1, mul(g_LightViewMatrix1, lWorldPosition));
		// �[�x�e�N�X�`���̍��W���Z�o
		DepthTexCoord.x = (LPPosition.x + 1.0f) / 2.0f; // LPPosition.xy �� -1.0f �` 1.0f �̒l�Ȃ̂ŁA����� 0.0f �` 1.0f �̒l�ɂ���
		DepthTexCoord.y = 1.0f - (LPPosition.y + 1.0f) / 2.0f; // y�͍X�ɏ㉺���]
		Total += g_ShadowMapTexture.Sample(g_ShadowMapSampler, DepthTexCoord).r;

		//------------------------------------------------------------------------------------------
		/*
		// ���C�g�̃r���[���W�����C�g�̎ˉe���W�ɕϊ�
		LPPosition = mul(g_LightProjectionMatrix2, mul(g_LightViewMatrix2, lWorldPosition));
		// �[�x�e�N�X�`���̍��W���Z�o
		DepthTexCoord.x = (LPPosition.x + 1.0f) / 2.0f; // LPPosition.xy �� -1.0f �` 1.0f �̒l�Ȃ̂ŁA����� 0.0f �` 1.0f �̒l�ɂ���
		DepthTexCoord.y = 1.0f - (LPPosition.y + 1.0f) / 2.0f; // y�͍X�ɏ㉺���]
		float ShadowFarMap = g_ShadowFarMapTexture.Sample(g_ShadowFarMapSampler, DepthTexCoord).r;
		//*/

		// �[�x�e�p�̃��C�g���猩���ˉe���W���Z�o ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( �I�� )
	}
	Total /= Div;
	PSOutput.color0 = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, PSInput.TexCoords0);

	PSOutput.color0.r = Total;
	//PSOutput.color0.g = 0.f;
	//PSOutput.color0.b = 0.f;
	PSOutput.color0.a = 1.f;
	return PSOutput;
}