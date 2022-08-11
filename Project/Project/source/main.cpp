#include"Header.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	OPTION::Create();
	auto* OptionParts = OPTION::Instance();
	OptionParts->Load();																										//�ݒ�ǂݍ���
	DXDraw::Create(("Realy3D_" + std::to_string(GetNowHiPerformanceCount())).c_str(), FPS_n2::Frame_Rate);						//�ėp
	FontPool::Create();
	LightPool::Create();
	//MV1SetLoadModelUsePackDraw(TRUE);
#ifdef DEBUG
	DebugClass::Create(FPS_n2::Frame_Rate);
#endif // DEBUG
	OptionParts->Set_useVR(DXDraw::Instance()->use_vr);
	auto scene = std::make_unique<FPS_n2::Sceneclass::SceneControl>();															//�V�[���R���g���[��
	PostPassEffect::Create();																									//�V�F�[�_�[
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();														//�V�[��
	FPS_n2::effectControl.Init();																								//�G�t�F�N�g
	MAINLOOPscene->Set_Next(MAINLOOPscene, FPS_n2::Sceneclass::scenes::MAIN_LOOP);												//�J�ڐ�w��
	scene->ChangeScene(FPS_n2::Sceneclass::scenes::MAIN_LOOP, (std::shared_ptr<FPS_n2::Sceneclass::TEMPSCENE>&)MAINLOOPscene);	//�J�n���J�ڐ�
	do {
		scene->StartScene();																									//�J�n����
		while (true) {
			if (scene->Execute()) { break; }																					//�X�V����
			if (!scene->isPause()) {
				FPS_n2::effectControl.Calc();																					//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
			}
			scene->Draw();																										//�`��
			scene->Vsync();																										//FPS�ҋ@
		}
		scene->NextScene();																										//�I������
	} while (!scene->isEnd());
	FPS_n2::effectControl.Dispose();
	return 0;
}
