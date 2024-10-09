#include "DxLib.h"
#include <array>
#include <functional>

// ��ʂ̃T�C�Y
#define SCREEN_W		1920/2
#define SCREEN_H		1080/2

//�V�F�[�_�[���g�p����ۂ̕⏕�N���X
class ShaderUseClass {
public:
	//2D�ɃV�F�[�_�[��K�p����ۂɎg�p�����ʃT�C�Y�̒��_���
	class ScreenVertex {
		VERTEX3DSHADER Screen_vertex[6] = {};
	public:
		// ���_�f�[�^�̎擾
		const auto* GetScreenVertex() noexcept { return Screen_vertex; }
		// ���_�f�[�^�̏���
		void			SetScreenVertex(int dispx, int dispy) noexcept {
			int xp1 = 0;
			int yp1 = dispy;
			int xp2 = dispx;
			int yp2 = 0;
			Screen_vertex[0].pos = VGet((float)xp1, (float)yp1, 0.0f);
			Screen_vertex[1].pos = VGet((float)xp2, (float)yp1, 0.0f);
			Screen_vertex[2].pos = VGet((float)xp1, (float)yp2, 0.0f);
			Screen_vertex[3].pos = VGet((float)xp2, (float)yp2, 0.0f);
			Screen_vertex[0].dif = GetColorU8(255, 255, 255, 255);
			Screen_vertex[1].dif = GetColorU8(255, 255, 255, 255);
			Screen_vertex[2].dif = GetColorU8(255, 255, 255, 255);
			Screen_vertex[3].dif = GetColorU8(255, 255, 255, 255);
			Screen_vertex[0].u = 0.0f; Screen_vertex[0].v = 0.0f;
			Screen_vertex[1].u = 1.0f; Screen_vertex[1].v = 0.0f;
			Screen_vertex[2].u = 0.0f; Screen_vertex[3].v = 1.0f;
			Screen_vertex[3].u = 1.0f; Screen_vertex[2].v = 1.0f;
			Screen_vertex[4] = Screen_vertex[2];
			Screen_vertex[5] = Screen_vertex[1];
		}
	};
private:
	//�V�F�[�_�[�n���h��
	int m_VertexShaderhandle{ -1 };
	int m_PixelShaderhandle{ -1 };
	//�V�F�[�_�[�ɓn���ǉ��p�����[�^�[��z����n���h��
	std::array<int, 4> m_VertexShadercbhandle{ -1 };
	int m_PixelShaderSendDispSizeHandle{ -1 };
	std::array<int, 4> m_PixelShadercbhandle{ -1 };
public:
	ShaderUseClass() {
		//�V�F�[�_�[�n���h��
		m_VertexShaderhandle = -1;
		m_PixelShaderhandle = -1;
		//�V�F�[�_�[�ɓn���ǉ��p�����[�^�[��z����n���h��
		for (auto& h : m_VertexShadercbhandle) { h = -1; }
		m_PixelShaderSendDispSizeHandle = -1;
		for (auto& h : m_PixelShadercbhandle) { h = -1; }
	}
	~ShaderUseClass() {
		Dispose();
	}
public:
	//������
	void			Init(const char* VertexShader, const char* PixelShader) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		//���_�V�F�[�_�[����
		for (auto& h : m_VertexShadercbhandle) {
			h = CreateShaderConstantBuffer(sizeof(float) * 4);
		}
		this->m_VertexShaderhandle = LoadVertexShader(VertexShader);		// ���_�V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
		//�s�N�Z���V�F�[�_�\����
		this->m_PixelShaderSendDispSizeHandle = CreateShaderConstantBuffer(sizeof(float) * 4);
		for (auto& h : m_PixelShadercbhandle) {
			h = CreateShaderConstantBuffer(sizeof(float) * 4);
		}
		this->m_PixelShaderhandle = LoadPixelShader(PixelShader);			// �s�N�Z���V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
	}
	//��n��
	void			Dispose() noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		//���_�V�F�[�_�[����
		for (auto& h : m_VertexShadercbhandle) {
			DeleteShaderConstantBuffer(h);
		}
		DeleteShader(this->m_VertexShaderhandle);
		//�s�N�Z���V�F�[�_�\����
		DeleteShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);
		for (auto& h : m_PixelShadercbhandle) {
			DeleteShaderConstantBuffer(h);
		}
		DeleteShader(this->m_PixelShaderhandle);
	}
public:
	//���_�V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
	void			SetVertexParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_VertexShadercbhandle[0]);		// ���_�V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
		f4->x = param1;
		f4->y = param2;
		f4->z = param3;
		f4->w = param4;
		UpdateShaderConstantBuffer(this->m_VertexShadercbhandle[0]);								// ���_�V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
		SetShaderConstantBuffer(this->m_VertexShadercbhandle[0], DX_SHADERTYPE_VERTEX, Slot);		// ���_�V�F�[�_�[�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^�S�ɃZ�b�g
	}
	//�s�N�Z���V�F�[�_�\��2�Ԗڂ̃��W�X�^�ɉ�ʃT�C�Y�̏����Z�b�g
	void			SetPixelDispSize(int dispx, int dispy) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);	// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
		dispsize->u = (float)dispx;
		dispsize->v = (float)dispy;
		UpdateShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);									// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
		SetShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle, DX_SHADERTYPE_PIXEL, 2);				// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^2�ɃZ�b�g
	}
	//�s�N�Z���V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=3)
	void			SetPixelParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_PixelShadercbhandle[0]);				// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
		f4->x = param1;
		f4->y = param2;
		f4->z = param3;
		f4->w = param4;
		UpdateShaderConstantBuffer(this->m_PixelShadercbhandle[0]);											// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
		SetShaderConstantBuffer(this->m_PixelShadercbhandle[0], DX_SHADERTYPE_PIXEL, Slot);					// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^3�ɃZ�b�g
	}
	//3D��ԂɓK�p����ꍇ�̊֐�(������3D�`��̃����_������)
	void			Draw_lamda(std::function<void()> doing) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
			doing();
			return;
		}
		SetUseVertexShader(this->m_VertexShaderhandle);											// �g�p���钸�_�V�F�[�_�[���Z�b�g
		SetUsePixelShader(this->m_PixelShaderhandle);											// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
		MV1SetUseOrigShader(TRUE);
		doing();
		MV1SetUseOrigShader(FALSE);
		SetUseVertexShader(-1);
		SetUsePixelShader(-1);
	}
	//2D�摜�ɓK�p����ꍇ�̊֐�
	void			Draw(ScreenVertex& Screenvertex) noexcept {
		if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
		Draw_lamda([&] {DrawPolygon3DToShader(Screenvertex.GetScreenVertex(), 2); });
	}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	static const int EXTEND = 1;


	ShaderUseClass::ScreenVertex	m_ScreenVertex;								// ���_�f�[�^
	ShaderUseClass					m_Shader2D;									// �g�p����V�F�[�_�[
	ShaderUseClass					m_Shader2D2;									// �g�p����V�F�[�_�[

	float XPos = 0.f;
	float ZPos = -1.92f;

	float SSRScale = 12.5f;
	float INTENSITY = 1.0f;
	float SCALE = 0.1f/12.5f;
	float BIAS = 0.93f;
	float SAMPLE_RAD = 16.5f;
	float MAX_DISTANCE = 1500.0f;

	int ModelHandle;
	int ModelHandleA;
	int NormalScreenHandle;
	int DepthScreenHandle;
	int ColorScreenHandle;
	int AberrationScreen;
	int OutputScreenHandle;

	// �E�C���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);

	// ��ʉ𑜓x��ݒ�
	SetGraphMode(SCREEN_W, SCREEN_H, 32);
	SetWaitVSyncFlag(FALSE);
	// �c�w���C�u�����̏�����
	if (DxLib_Init() < 0) return -1;

	m_ScreenVertex.SetScreenVertex(SCREEN_W, SCREEN_H);							// ���_�f�[�^�̏���
	m_Shader2D.Init("shader/VS_SSAO.vso", "shader/PS_SSAO.pso");					// �����Y
	m_Shader2D2.Init("shader/VS_SSAO.vso", "shader/PS_BilateralBlur.pso");					// �����Y

	ModelHandle = MV1LoadModel("SSAO_Stage.mqo");								// �R�c���f���̓ǂݍ���
	ModelHandleA = MV1LoadModel("SSAO_Stagea.mqo");								// �R�c���f���̓ǂݍ���
	{
		SetCreateGraphChannelBitDepth(24);
		ColorScreenHandle = MakeScreen(SCREEN_W, SCREEN_H, FALSE);				// �J���[�o�b�t�@�̍쐬

		NormalScreenHandle = MakeScreen(SCREEN_W, SCREEN_H, TRUE);				// �@���o�b�t�@�̍쐬

		SetDrawValidFloatTypeGraphCreateFlag(TRUE);
		SetCreateDrawValidGraphChannelNum(2);
		SetCreateGraphChannelBitDepth(32);
		DepthScreenHandle = MakeScreen(SCREEN_W, SCREEN_H, TRUE);				// �@���o�b�t�@�̍쐬
		SetCreateGraphChannelBitDepth(24);
		SetCreateDrawValidGraphChannelNum(4);
		SetDrawValidFloatTypeGraphCreateFlag(FALSE);
	}
	AberrationScreen = MakeScreen(SCREEN_W / EXTEND, SCREEN_H / EXTEND, TRUE);	// �`��X�N���[��

	OutputScreenHandle = MakeScreen(SCREEN_W, SCREEN_H, TRUE);					// �o�͐�o�b�t�@�̍쐬

	SetUsePixelLighting(TRUE);	// �s�N�Z���P�ʂ̃��C�e�B���O���s���`�惂�[�h�ɐݒ肷��

	SetUseZBuffer3D(TRUE);
	SetDrawZBuffer(TRUE);

	// ���C�����[�v
	float DrawTimer = 0.f;
	while (ProcessMessage() == 0) {
		if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) { break; }
		//
		float fov_cam = 60.f * DX_PI_F / 180.0f;
		//���ƂɂȂ��ʂ�G�o�b�t�@��`��
		{
			// �J���[�o�b�t�@��`��Ώ�0�ɁA�@���o�b�t�@��`��Ώ�1�A�[�x�{����O���[�X�P�[���o�b�t�@��`��Ώ�2�A����RGB�o�b�t�@��`��Ώ�3�ɐݒ�
			SetRenderTargetToShader(0, ColorScreenHandle);
			SetRenderTargetToShader(1, NormalScreenHandle);
			SetRenderTargetToShader(2, DepthScreenHandle);
			ClearDrawScreen();			// ��ʂ̃N���A
			// �J������ݒ�
			SetCameraPositionAndTarget_UpVecY(VGet(XPos, 0.f, ZPos), VGet(0.f, 0.f, 0.f));
			SetCameraNearFar(0.1f, 300.f);
			SetupCamera_Perspective(fov_cam);
			{
				MV1DrawModel(ModelHandle);				// ���f���̕`��
				MV1DrawModel(ModelHandleA);				// ���f���̕`��
			}
			// �`��Ώۂ����ׂĉ���
			SetRenderTargetToShader(0, -1);
			SetRenderTargetToShader(1, -1);
			SetRenderTargetToShader(2, -1);
		}
		//SSR�V�F�[�_�[��K�p
		SetDrawScreen(OutputScreenHandle);
		{
			SetUseTextureToShader(0, ColorScreenHandle);	//�g�p����e�N�X�`�����Z�b�g
			SetUseTextureToShader(1, NormalScreenHandle);
			SetUseTextureToShader(2, DepthScreenHandle);
			m_Shader2D.SetPixelDispSize(SCREEN_W, SCREEN_H);
			m_Shader2D.SetPixelParam(3, 0.0f, SSRScale, std::tan(fov_cam / 2.f), INTENSITY);
			m_Shader2D.SetPixelParam(4, SCALE, BIAS, SAMPLE_RAD, MAX_DISTANCE);

			m_Shader2D.Draw(m_ScreenVertex);

			SetUseTextureToShader(0, -1);
			SetUseTextureToShader(1, -1);
			SetUseTextureToShader(2, -1);
		}
		//SSR�V�F�[�_�[�ɂڂ���������
		SetDrawScreen(AberrationScreen);
		{
			SetUseTextureToShader(0, OutputScreenHandle);	//�g�p����e�N�X�`�����Z�b�g
			SetUseTextureToShader(1, ColorScreenHandle);	//�g�p����e�N�X�`�����Z�b�g

			m_Shader2D2.SetPixelDispSize(SCREEN_W, SCREEN_H);
			m_Shader2D2.Draw(m_ScreenVertex);

			SetUseTextureToShader(0, -1);
			SetUseTextureToShader(1, -1);
		}
		//
		SetDrawScreen(DX_SCREEN_BACK);	//�g�p����e�N�X�`�����Z�b�g
		{
			DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), TRUE);
			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, ColorScreenHandle, TRUE);

			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, DepthScreenHandle, TRUE);
			//DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(255, 255, 255), TRUE);
			//SetDrawBlendMode(DX_BLENDMODE_MULA, 255);
			DrawExtendGraph(0, 0, SCREEN_W, SCREEN_H, OutputScreenHandle, TRUE);
			//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			// �p�����[�^����ʂɕ\��
			/*
			DrawFormatString(0, 32 * 0, GetColor(255, 255, 255), "W/S/A/D key  (%6.2f,%6.2f,%6.2f) : �J�������W", XPos,0.f,ZPos);
			DrawFormatString(0, 32 * 1, GetColor(255, 255, 255), "FPS           %6.2f : FPS", GetFPS());
			DrawFormatString(0, 32 * 2, GetColor(255, 255, 255), "DrawTime(ms)  %6.2f : �`�掞��", DrawTimer);
			DrawFormatString(0, 32 * 3, GetColor(255, 255, 255), "P    SSR��ʂ̂ݕ\��");
			DrawFormatString(0, 32 * 4, GetColor(255, 255, 255), "Esc  �I��");
			//*/
			clsDx();
			printfDx("INTENSITY    %f\n", INTENSITY);
			printfDx("SCALE        %f\n", SCALE);
			printfDx("BIAS         %f\n", BIAS);
			printfDx("SAMPLE_RAD   %f\n", SAMPLE_RAD);
			printfDx("MAX_DISTANCE %f\n", MAX_DISTANCE);
			printfDx("FPS          %6.2f : FPS", GetFPS());
			printfDx("DrawTime(ms) %6.2f : �`�掞��", DrawTimer);
		}

		// �L�[�ɂ��p�����[�^�̑���
		switch (GetInputChar(TRUE)) {
		case 'a': XPos -= 0.3f; break;
		case 'd': XPos += 0.3f; break;
		case 'w': ZPos += 0.3f; break;
		case 's': ZPos -= 0.3f; break;
		case 'r': INTENSITY += 1.f; break;
		case 'f': INTENSITY -= 1.f; break;
		case 't': SCALE += 0.1f; break;
		case 'g': SCALE -= 0.1f; break;
		case 'y': BIAS += 0.1f; break;
		case 'h': BIAS -= 0.1f; break;
		case 'u': SAMPLE_RAD += 1.f; break;
		case 'j': SAMPLE_RAD -= 1.f; break;
		case 'i': MAX_DISTANCE += 100.f; break;
		case 'k': MAX_DISTANCE -= 100.f; break;
		default:break;
		}
		if (SSRScale <= 0.f) { SSRScale = 0.1f; }

		LONGLONG StartTime = GetNowHiPerformanceCount();		//ScreenFlip�O�Ɍ��݂̎��Ԃ����

		ScreenFlip();		// ����ʂ̓��e��\��ʂɔ��f

		DrawTimer = (float)(GetNowHiPerformanceCount() - StartTime) / 1000.f;//ScreenFlip�o�����������Ԃ��~���b�P�ʂŎ擾
	}

	// �c�w���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�̏I��
	return 0;
}
