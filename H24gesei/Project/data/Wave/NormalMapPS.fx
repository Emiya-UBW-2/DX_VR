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

//1�t���[���O�̕`�挋��
SamplerState g_Sampler : register(s0);
Texture2D g_Texture : register(t0);

//�@���}�b�v�쐬
float4 BumpMap(float2 TexCoords0)
{
    float4 Out;

    //�㉺���E�̃e�N�Z���ʒu�̍������擾����
    float H1 = g_Texture.Sample(g_Sampler, TexCoords0, float2(1, 0)).r;
    float H2 = g_Texture.Sample(g_Sampler, TexCoords0, float2(0, 1)).r;
    float H3 = g_Texture.Sample(g_Sampler, TexCoords0, float2(-1, 0)).r;
    float H4 = g_Texture.Sample(g_Sampler, TexCoords0, float2(0, -1)).r;
 
    //X �����̍����̕ω��ʂ��v�Z����
    //�g�̍������� -1.0f �` 1.0f �͈̔͂Ŋi�[����Ă���̂� 0.0f �` 1.0f �ɕϊ�����
    float tu = 0.5f * (H3 - H1) + 0.5f;

    //Y �����̍����̕ω��ʂ��v�Z����
    //�g�̍������� -1.0f �` 1.0f �͈̔͂Ŋi�[����Ă���̂� 0.0f �` 1.0f �ɕϊ�����
    float tv = 0.5f * (H4 - H2) + 0.5f;
 
    //�����}�b�s���O�͍��������g�p����B���̂��߃������ɍ��������i�[����
    Out = float4(tu, tv, 1.0f, g_Texture.Sample(g_Sampler, TexCoords0).r);

    return Out;
}
// main�֐�
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    PSOutput.color0 = BumpMap(PSInput.texCoords0);
	// �o�̓p�����[�^��Ԃ�
    return PSOutput;
}

