// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT {
	float4 dif         : COLOR0;		// �f�B�t���[�Y�J���[
	float2 texCoords0  : TEXCOORD0;	// �e�N�X�`�����W
	float4 pos         : SV_POSITION;   // ���W( �v���W�F�N�V������� )
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT {
	float4 color0           : SV_TARGET0;	// �F
};

//��ʃT�C�Y
static int2 dispsize = {0, 0};

// �萔�o�b�t�@�s�N�Z���V�F�[�_�[��{�p�����[�^
struct DX_D3D11_PS_CONST_BUFFER_BASE {
	float4		FactorColor;			// �A���t�@�l��

	float		MulAlphaColor;			// �J���[�ɃA���t�@�l����Z���邩�ǂ���( 0.0f:��Z���Ȃ�  1.0f:��Z���� )
	float		AlphaTestRef;			// �A���t�@�e�X�g�Ŏg�p�����r�l
	float2		Padding1;

	int		    AlphaTestCmpMode;		// �A���t�@�e�X�g��r���[�h( DX_CMP_NEVER �Ȃ� )
	int3		Padding2;

	float4		IgnoreTextureColor;	// �e�N�X�`���J���[���������p�J���[
};

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_PS_BASE				: register(b1) {
	DX_D3D11_PS_CONST_BUFFER_BASE		g_Base;
};

// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3) {
	float4 caminfo;
}

cbuffer cbLIGHTCAMERA_MATRIX : register(b4) {
	matrix		g_LightViewMatrix;			// ���C�g�̃��[���h�@���@�r���[�s��
	matrix		g_LightProjectionMatrix;	// ���C�g�̃r���[�@�@���@�ˉe�s��
};

// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^2
cbuffer cbMULTIPLYCOLOR_CBUFFER3 : register(b5) {
	float4	g_param;
}


//�e�N�X�`��
SamplerState g_Register0MapSampler : register(s0); // �f�B�t���[�Y�}�b�v�T���v��
Texture2D g_Register0MapTexture : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

SamplerState g_Register1MapSampler : register(s1); // �@���}�b�v�T���v��
Texture2D g_Register1MapTexture : register(t1); // �@���}�b�v�e�N�X�`��

SamplerState g_Register2MapSampler : register(s2); // �[�x�}�b�v�T���v��
Texture2D g_Register2MapTexture : register(t2); // �[�x�}�b�v�e�N�X�`��

SamplerState dynamicCubeMapSampler     : register(s3);
TextureCube  dynamicCubeMapTexture     : register(t3);

//�֐�
float4 GetTexColor0(float2 texCoord, int2 offset = int2(0, 0)) {
	return g_Register0MapTexture.Sample(g_Register0MapSampler, texCoord, offset);
}
float4 GetTexColor1(float2 texCoord, int2 offset = int2(0, 0)) {
	return g_Register1MapTexture.Sample(g_Register1MapSampler, texCoord, offset);
}
float4 GetTexColor2(float2 texCoord, int2 offset = int2(0, 0)) {
	return g_Register2MapTexture.Sample(g_Register2MapSampler, texCoord, offset);
}

float3 DisptoProjNorm(float2 screenUV) {
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
float3 DisptoProj(float2 screenUV) {
	float depth = GetTexColor2(screenUV).r;
	return DisptoProjNorm(screenUV) * (depth / (caminfo.y * 0.005f)); //����
}

float2 ProjtoDisp(float3 position) {
	position = position / position.z;

	float2 screenUV;
	screenUV.x = position.x / caminfo.z * dispsize.y / dispsize.x;
	screenUV.y = position.y / caminfo.z;

	screenUV.x = screenUV.x + 1.f;
	screenUV.y = 1.f - screenUV.y;
	screenUV *= 0.5f;
	return screenUV;

	/*
	float4 projectPosition = mul(gProjectionMatrix, float4(position, 1.0));
	float2 screenUV = projectPosition.xy / projectPosition.w * 0.5f + 0.5f;
	screenUV.y = 1.0f - screenUV.y;
	return screenUV;
	//*/
}

bool Hitcheck(float3 position) {
	float2 screenUV = ProjtoDisp(position);

	float depth = GetTexColor2(screenUV).r;

	if (
		(-1.f <= screenUV.x && screenUV.x <= 1.f) &&
		(-1.f <= screenUV.y && screenUV.y <= 1.f)
		) {
		float z = depth / (caminfo.y * 0.005f);
		return (position.z < z && z < position.z + caminfo.w);
	}
	else {
		return false;
	}
}

float4 applySSR(float3 normal, float2 screenUV) {
	float3 position = DisptoProj(screenUV);

	float4 color;

	color.r = 0.f;
	color.g = 0.f;
	color.b = 0.f;
	color.a = 0.f;
	float3 reflectVec = normalize(reflect(normalize(position), normal)); // ���˃x�N�g��
	int iteration = (int)(caminfo.x); // �J��Ԃ���
	float maxLength = 5000.f; // ���ˍő勗��
	int BinarySearchIterations = 4; //2���T���ő吔

	float pixelStride = maxLength / (float)iteration;
	float3 delta = reflectVec * pixelStride; // �P��Ői�ދ���
	int isend = 0;
	if (position.z <= 0.f) {
		isend = 1;
	}
	for (int i = 0; i < iteration; i++) {
		if (isend == 0) {
			position += delta;
			if (Hitcheck(position)) {
				//���������̂œ񕪒T��
				position -= delta; //���ɖ߂�
				delta /= BinarySearchIterations; //�i�ޗʂ�������
				for (int j = 0; j < BinarySearchIterations; j++) {
					if (isend == 0) {
						pixelStride *= 0.5f;
						position += delta * pixelStride;
						if (Hitcheck(position)) {
							pixelStride = -pixelStride;
						}
						if (length(pixelStride) < 10.f) {
							isend = 1;
						}
					}
				}
				// ���������̂ŐF���u�����h����
				color = GetTexColor0(ProjtoDisp(position));
				isend = 1;
			}
		}
	}
	return color;
}

PS_OUTPUT main(PS_INPUT PSInput) {
	//�߂�l
	PS_OUTPUT PSOutput;
	//��ʃT�C�Y���擾���Ă���
	g_Register0MapTexture.GetDimensions(dispsize.x, dispsize.y);

	//����
	float3 normal = GetTexColor1(PSInput.texCoords0).xyz;
	normal.x = normal.x * 2.f - 1.f;
	normal.y = normal.y * 2.f - 1.f;
	normal.z = normal.z * 2.f - 1.f;

	//�L���[�u�}�b�v����̔���
	float4 lWorldPosition;
	lWorldPosition.xyz = DisptoProjNorm(PSInput.texCoords0);
	lWorldPosition.w = 0.f;

	lWorldPosition.z *= -1.f;
	// ���[���h���W���ˉe���W�ɕϊ�
	float4 LPPosition1 = mul(g_LightProjectionMatrix, mul(g_LightViewMatrix, lWorldPosition));
	LPPosition1.x *= -1.f;

	lWorldPosition.xyz = normal;
	lWorldPosition.w = 0.f;

	lWorldPosition.z *= -1.f;
	// ���[���h���W���ˉe���W�ɕϊ�
	float4 LPPosition2 = mul(g_LightProjectionMatrix, mul(g_LightViewMatrix, lWorldPosition));
	LPPosition2.x *= -1.f;

	PSOutput.color0 = dynamicCubeMapTexture.Sample(dynamicCubeMapSampler, reflect(LPPosition1.xyz, LPPosition2.xyz));

	if (g_param.x >= 2) {
		float4 color = applySSR(normal, PSInput.texCoords0);
		if (color.a > 0.f) {
			PSOutput.color0 = color;
		}
		else {
			PSOutput.color0 = float4(0.f, 0.f, 0.f, 0.f);
		}
	}
	PSOutput.color0 = lerp(float4(0.f, 0.f, 0.f, 0.f), PSOutput.color0, GetTexColor2(PSInput.texCoords0).g);

	//�߂�l
	//return PSOutput;
	//�����������o�͂���ꍇ�͂�����
	float3 Color = GetTexColor0(PSInput.texCoords0).xyz;
	if (
		PSOutput.color0.r == Color.r &&
		PSOutput.color0.g == Color.g &&
		PSOutput.color0.b == Color.b
		) {
		PSOutput.color0.a = 0.0;
	}
	return PSOutput;
}
