#include"Header.hpp"

#include	"Scene/TitleScene.hpp"
#include	"Scene/MainScene.hpp"
#include	"Scene/EndScene.hpp"

#include	"MainScene/NetworkBrowser.hpp"

#ifdef DEBUG
int DBG_CamSel = -1;
#endif

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	SetDoubleStartValidFlag(TRUE);
	//SetMultiThreadFlag(TRUE);
	//SetEnableXAudioFlag(TRUE);//Xaudio(���[�h�������ƃ��X�g����̂ŕK�v�ɉ�����)
	DXLib_ref::Create();
	//�g�p����{�^���̎w��
	//*
	auto* Pad = PadControl::Instance();
	Pad->SetIsUseButton(Controls::PADS::MOVE_W, true);
	Pad->SetIsUseButton(Controls::PADS::MOVE_A, true);
	Pad->SetIsUseButton(Controls::PADS::MOVE_S, true);
	Pad->SetIsUseButton(Controls::PADS::MOVE_D, true);
	Pad->SetIsUseButton(Controls::PADS::MOVE_STICK, true);
	Pad->SetIsUseButton(Controls::PADS::DIR_UP, false);
	Pad->SetIsUseButton(Controls::PADS::DIR_DOWN, false);
	Pad->SetIsUseButton(Controls::PADS::DIR_LEFT, false);
	Pad->SetIsUseButton(Controls::PADS::DIR_RIGHT, false);
	Pad->SetIsUseButton(Controls::PADS::DIR_STICK, true);
	Pad->SetIsUseButton(Controls::PADS::LEAN_L, true);//UI�̂�
	Pad->SetIsUseButton(Controls::PADS::LEAN_R, true);//UI�̂�
	Pad->SetIsUseButton(Controls::PADS::RELOAD, true);//UI�̂�
	Pad->SetIsUseButton(Controls::PADS::INTERACT, true);//UI�̂�
	Pad->SetIsUseButton(Controls::PADS::THROW, false);
	Pad->SetIsUseButton(Controls::PADS::MELEE, false);
	Pad->SetIsUseButton(Controls::PADS::JUMP, true);
	Pad->SetIsUseButton(Controls::PADS::INVENTORY, false);
	Pad->SetIsUseButton(Controls::PADS::RUN, false);
	Pad->SetIsUseButton(Controls::PADS::WALK, true);
	Pad->SetIsUseButton(Controls::PADS::SHOT, true);
	Pad->SetIsUseButton(Controls::PADS::AIM, true);
	Pad->SetIsUseButton(Controls::PADS::ULT, true);
	Pad->SetIsUseButton(Controls::PADS::SQUAT, false);
	Pad->SetIsUseButton(Controls::PADS::PRONE, false);
	Pad->SetIsUseButton(Controls::PADS::CHECK, false);
	Pad->SetIsUseButton(Controls::PADS::Escape, true);
	//*/
	//
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (DXLib_refParts->FirstBootSetting()) { return 0; }
	DXLib_refParts->StartLogic();
	//�ǉ��ݒ�
	SetMainWindowText("����-�邬 �݂�-");						//�^�C�g��
	SetUseHalfLambertLighting(TRUE);
	MV1SetLoadModelReMakeNormal(TRUE);
	//
	FPS_n2::ArmAnimManager::Create();
	FPS_n2::ArmAnimManager::Instance()->Load("data/CharaAnime/");
	FPS_n2::Player::PlayerManager::Create();
	FPS_n2::BackGround::BackGroundClass::Create();
	FPS_n2::NetWorkBrowser::Create();
	FPS_n2::Sceneclass::ButtonControl::Create();
	FPS_n2::Sceneclass::FadeControl::Create();						// UI�p�t�F�[�h�C���}�l�[�W���[
	FPS_n2::HitMark::Create();
	//�V�[��
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();

	auto MainGameScenePtr = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	MainGameScenePtr->SetGameMode(FPS_n2::GameMode::Main);
	auto MainGameSceneTrainingPtr = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	MainGameSceneTrainingPtr->SetGameMode(FPS_n2::GameMode::Training);
	auto MainGameSceneReplayPtr = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	MainGameSceneReplayPtr->SetGameMode(FPS_n2::GameMode::Replay);

	auto EndScenePtr = std::make_shared<FPS_n2::Sceneclass::EndScene>();

	//�J�ڐ�w��
	Titlescene->SetNextSceneList(0, MainGameScenePtr);
	Titlescene->SetNextSceneList(1, MainGameSceneTrainingPtr);
	Titlescene->SetNextSceneList(2, MainGameSceneReplayPtr);
	Titlescene->SetNextSceneList(3, EndScenePtr);
	MainGameScenePtr->SetNextSceneList(0, Titlescene);
	MainGameSceneTrainingPtr->SetNextSceneList(0, Titlescene);
	MainGameSceneReplayPtr->SetNextSceneList(0, Titlescene);
	EndScenePtr->SetNextSceneList(0, Titlescene);

	auto* SceneParts = SceneControl::Instance();
	SceneParts->SetFirstScene(Titlescene);

	DXLib_refParts->MainLogic();
	return 0;
}
