// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
    float4 dif : COLOR0; // �f�B�t���[�Y�J���[
    float2 texCoords0 : TEXCOORD0; // �e�N�X�`�����W
    float4 pos : SV_POSITION; // ���W( �v���W�F�N�V������� )
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

// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^1
cbuffer cbMULTIPLYCOLOR_CBUFFER1 : register(b2)
{
    float2 dispsize;
}

SamplerState g_DiffuseMapSampler : register(s0); // �f�B�t���[�Y�}�b�v�T���v��
Texture2D g_DiffuseMapTexture : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

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
