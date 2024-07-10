#include	"Header.hpp"

#include	"sub.hpp"
#include	"MainScene/Player/Player.hpp"
#include	"MainScene/BackGround/BackGround.hpp"
// 
#include	"CommonScene/Object/Object2DManager.hpp"
#include	"Scene/TitleScene.hpp"
#include	"Scene/MainGameScene.hpp"
#include	"Scene/TestGameScene.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// SetEnableXAudioFlag(TRUE);// Xaudio(���[�h�������ƃ��X�g����̂ŕK�v�ɉ�����)
	DXLib_ref::Create();
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (!DXLib_refParts->StartLogic()) { return 0; }
	// �ǉ��ݒ�
	SetMainWindowText("Title");					// �^�C�g��
	// MV1SetLoadModelUsePackDraw(TRUE);
	// SetUseHalfLambertLighting(TRUE);	// �n�[�t�����o�[�g��
	// 
	FPS_n2::Sceneclass::Object2DManager::Create();
	FPS_n2::Sceneclass::PlayerManager::Create();
	FPS_n2::Sceneclass::CommonBattleResource::Create();
	FPS_n2::Sceneclass::Cam2DControl::Create();
	FPS_n2::Sceneclass::BackGroundClassBase::Create();
	FPS_n2::Sceneclass::ButtonControl::Create();

	// 
	auto* SaveDataParts = SaveDataClass::Instance();
	auto* BGM = BGMPool::Instance();
	auto* OptionParts = OPTION::Instance();
	// �����J��
	// SaveDataParts->SetParam("UnlockHardMode", 1);
	// bool IsFirstGame = (SaveDataParts->GetParam("FirstGame") != 1);
	SaveDataParts->Save();
	// BGM
	// BGM->Add(0, "data/Sound/BGM/Title.wav");
	BGM->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
	// �V�[��
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto MainGamescene = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	auto TestGamescene = std::make_shared<FPS_n2::Sceneclass::TestGameScene>();
	// �J�ڐ�w��
	Titlescene->SetNextSceneList(0, MainGamescene);
	Titlescene->SetNextSceneList(1, TestGamescene);
	MainGamescene->SetNextSceneList(0, Titlescene);
	MainGamescene->SetNextSceneList(1, MainGamescene);
	TestGamescene->SetNextSceneList(0, Titlescene);

	auto* SceneParts = SceneControl::Instance();
	/*
	SceneParts->AddList(Titlescene);
	SceneParts->AddList(MainGamescene);
	SceneParts->AddList(TestGamescene);
	// */
	// *
	SceneParts->AddList(MainGamescene);
	SceneParts->AddList(Titlescene);
	SceneParts->AddList(TestGamescene);
	// */
	// �ŏ��̓ǂݍ���
	if (!DXLib_refParts->MainLogic()) { return 0; }
	return 0;
}
