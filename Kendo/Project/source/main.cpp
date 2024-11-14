#include"Header.hpp"

#include	"Scene/TitleScene.hpp"
#include	"Scene/MainScene.hpp"

#include	"MainScene/NetworkBrowser.hpp"

#ifdef DEBUG
int DBG_CamSel = -1;
#endif

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	SetDoubleStartValidFlag(TRUE);
	//SetEnableXAudioFlag(TRUE);//Xaudio(���[�h�������ƃ��X�g����̂ŕK�v�ɉ�����)
	DXLib_ref::Create();
	//�g�p����{�^���̎w��
	//*
	auto* Pad = PadControl::Instance();
	Pad->SetIsUseButton(PADS::MOVE_W, true);
	Pad->SetIsUseButton(PADS::MOVE_A, true);
	Pad->SetIsUseButton(PADS::MOVE_S, true);
	Pad->SetIsUseButton(PADS::MOVE_D, true);
	Pad->SetIsUseButton(PADS::MOVE_STICK, true);
	Pad->SetIsUseButton(PADS::DIR_UP, false);
	Pad->SetIsUseButton(PADS::DIR_DOWN, false);
	Pad->SetIsUseButton(PADS::DIR_LEFT, false);
	Pad->SetIsUseButton(PADS::DIR_RIGHT, false);
	Pad->SetIsUseButton(PADS::DIR_STICK, true);
	Pad->SetIsUseButton(PADS::LEAN_L, true);//UI�̂�
	Pad->SetIsUseButton(PADS::LEAN_R, true);//UI�̂�
	Pad->SetIsUseButton(PADS::RELOAD, true);//UI�̂�
	Pad->SetIsUseButton(PADS::INTERACT, true);//UI�̂�
	Pad->SetIsUseButton(PADS::THROW, false);
	Pad->SetIsUseButton(PADS::MELEE, false);
	Pad->SetIsUseButton(PADS::JUMP, true);
	Pad->SetIsUseButton(PADS::INVENTORY, false);
	Pad->SetIsUseButton(PADS::RUN, false);
	Pad->SetIsUseButton(PADS::WALK, true);
	Pad->SetIsUseButton(PADS::SHOT, true);
	Pad->SetIsUseButton(PADS::AIM, true);
	Pad->SetIsUseButton(PADS::ULT, true);
	Pad->SetIsUseButton(PADS::SQUAT, false);
	Pad->SetIsUseButton(PADS::PRONE, false);
	Pad->SetIsUseButton(PADS::CHECK, false);
	Pad->SetIsUseButton(PADS::Escape, true);
	//*/
	//
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (DXLib_refParts->FirstBootSetting()) { return 0; }
	DXLib_refParts->StartLogic();
	//�ǉ��ݒ�
	SetMainWindowText("����-�邬 �݂�-");						//�^�C�g��
	//SetUseHalfLambertLighting(TRUE);
	MV1SetLoadModelReMakeNormal(TRUE);
	//
	FPS_n2::ArmAnimManager::Create();
	FPS_n2::Player::PlayerManager::Create();
	FPS_n2::CommonBattleResource::Create();
	FPS_n2::ArmAnimManager::Instance()->Load("data/CharaAnime/");
	FPS_n2::BackGround::BackGroundClass::Create();
	FPS_n2::NetWorkBrowser::Create();
	FPS_n2::Sceneclass::ButtonControl::Create();
	FPS_n2::Sceneclass::FadeControl::Create();						// UI�p�t�F�[�h�C���}�l�[�W���[
	FPS_n2::HitMark::Create();
	//�V�[��
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto MainGameScenePtr = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	auto MainGameSceneTrainingPtr = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	auto MainGameSceneReplayPtr = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	MainGameScenePtr->SetGameMode(FPS_n2::GameMode::Main);
	MainGameSceneTrainingPtr->SetGameMode(FPS_n2::GameMode::Training);
	MainGameSceneReplayPtr->SetGameMode(FPS_n2::GameMode::Replay);


	//�J�ڐ�w��
	Titlescene->SetNextSceneList(0, MainGameScenePtr);
	Titlescene->SetNextSceneList(1, MainGameSceneTrainingPtr);
	Titlescene->SetNextSceneList(2, MainGameSceneReplayPtr);
	MainGameScenePtr->SetNextSceneList(0, Titlescene);
	MainGameSceneTrainingPtr->SetNextSceneList(0, Titlescene);
	MainGameSceneReplayPtr->SetNextSceneList(0, Titlescene);

	auto* SceneParts = SceneControl::Instance();
	SceneParts->SetFirstScene(Titlescene);

	DXLib_refParts->MainLogic();
	return 0;
}
