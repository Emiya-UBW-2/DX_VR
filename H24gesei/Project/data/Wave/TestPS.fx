// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
    float2 texCoords0 : TEXCOORD0; // �e�N�X�`�����W
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT
{
    float4 color0 : SV_TARGET0; // �F
};


// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^1
cbuffer cbFree_CBUFFER1 : register(b2)
{
    float4 g_param;
}

//2�t���[���O�̕`�挋��
SamplerState g_Prev2Sampler : register(s0);
Texture2D g_Prev2Texture : register(t0);
//1�t���[���O�̕`�挋��
SamplerState g_Prev1Sampler : register(s1);
Texture2D g_Prev1Texture : register(t1);

float CalcWave(float2 TexCoords0)
{
    float s2 = 0.2;

    float Prev1Center = 2 * g_Prev1Texture.Sample(g_Prev1Sampler, TexCoords0).r;

    float p =
        Prev1Center - g_Prev2Texture.Sample(g_Prev2Sampler, TexCoords0).r +
        s2 * (
            g_Prev1Texture.Sample(g_Prev1Sampler, TexCoords0, float2(-1, 0)).r
            - Prev1Center
            + g_Prev1Texture.Sample(g_Prev1Sampler, TexCoords0, float2(1, 0)).r
            + g_Prev1Texture.Sample(g_Prev1Sampler, TexCoords0, float2(0, -1)).r
            - Prev1Center
            + g_Prev1Texture.Sample(g_Prev1Sampler, TexCoords0, float2(0, 1)).r
        );
    return p;
}
// main�֐�
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    PSOutput.color0.r = CalcWave(PSInput.texCoords0);
    PSOutput.color0.b = 0.f;
    PSOutput.color0.a = 0.f;
    PSOutput.color0.a = 1.f;
	// �o�̓p�����[�^��Ԃ�
    return PSOutput;
}

