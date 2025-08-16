#include	"Header.hpp"

#include	"Scene/MainScene.hpp"
#include	"Scene/MainSceneLoader.hpp"
#include	"Scene/TitleScene.hpp"

#include	"MainScene/NetworkBrowser.hpp"

#if defined(DEBUG) && DEBUG_CAM
int DBG_CamSelect = InvalidID;
#endif

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	SetDoubleStartValidFlag(true);
	//SetEnableXAudioFlag(true);//Xaudio(ロードが長いとロストするので必要に応じて)
	DXLib_ref::Create();
	//使用するボタンの指定
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
	Pad->SetIsUseButton(Controls::PADS::LEAN_L, true);
	Pad->SetIsUseButton(Controls::PADS::LEAN_R, true);
	Pad->SetIsUseButton(Controls::PADS::RELOAD, true);
	Pad->SetIsUseButton(Controls::PADS::INTERACT, true);
	Pad->SetIsUseButton(Controls::PADS::THROW, true);
	Pad->SetIsUseButton(Controls::PADS::MELEE, false);
	Pad->SetIsUseButton(Controls::PADS::JUMP, false);
	Pad->SetIsUseButton(Controls::PADS::INVENTORY, true);
	Pad->SetIsUseButton(Controls::PADS::RUN, true);
	Pad->SetIsUseButton(Controls::PADS::WALK, true);
	Pad->SetIsUseButton(Controls::PADS::SHOT, true);
	Pad->SetIsUseButton(Controls::PADS::AIM, true);
	Pad->SetIsUseButton(Controls::PADS::ULT, true);
	Pad->SetIsUseButton(Controls::PADS::SQUAT, true);
	Pad->SetIsUseButton(Controls::PADS::PRONE, false);
	Pad->SetIsUseButton(Controls::PADS::CHECK, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE1, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE2, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE3, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE4, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE5, true);
	Pad->SetIsUseButton(Controls::PADS::ITEMDELETE, true);
	Pad->SetIsUseButton(Controls::PADS::HEALARMOR, true);
	//
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (DXLib_refParts->FirstBootSetting()) { return 0; }
	DXLib_refParts->StartLogic();
	//追加設定
	std::string Title = "Phantom of the Bunker";
	Title += std::to_string(GetRand(1000));
	SetMainWindowText(Title.c_str());						//タイトル
	SetUseHalfLambertLighting(true);
	MV1SetLoadModelReMakeNormal(true);
	//SetUsePixelLighting(true);
	SetUseBackCulling(true);
	SetTextureAddressMode(DX_TEXADDRESS_WRAP);
	//MV1SetLoadModelUsePackDraw(TRUE);
	//
	FPS_n2::FadeControl::Create();
	FPS_n2::UIs::ButtonControl::Create();
	//
	FPS_n2::Guns::GunPartsDataManager::Create();
	FPS_n2::Objects::AmmoDataManager::Create();
	FPS_n2::Objects::ItemObjDataManager::Create();
	//
	{
		std::string Path = "data/Item/";
		std::vector<WIN32_FIND_DATA> pData;
		GetFileNamesInDirectory((Path + "*").c_str(), &pData);
		for (auto& data : pData) {
			std::string ChildPath = Path;
			ChildPath += data.cFileName;
			ChildPath += "/";
			FPS_n2::Objects::ItemObjDataManager::Instance()->Add(ChildPath);
		}
	}
	//初期セーブ
	{
		FPS_n2::Guns::GunPartsDataManager::Instance()->m_GunList.clear();
		{
			std::string Path = "data/gun/";
			std::vector<WIN32_FIND_DATA> pData;
			GetFileNamesInDirectory((Path + "*").c_str(), &pData);
			for (auto& data : pData) {
				FPS_n2::Guns::GunPartsDataManager::Instance()->m_GunList.emplace_back(data.cFileName);
			}
		}
		//
		{
			std::string Path = "data/gun/";
			std::vector<WIN32_FIND_DATA> pData;
			GetFileNamesInDirectory((Path + "*").c_str(), &pData);
			for (auto& data : pData) {
				std::string ChildPath = Path;
				ChildPath += data.cFileName;
				ChildPath += "/";
				FPS_n2::Guns::GunPartsDataManager::Instance()->Add(ChildPath);
			}
		}
		//
		{
			std::string Path = "data/Mods/";
			std::vector<WIN32_FIND_DATA> pData;
			GetFileNamesInDirectory((Path + "*").c_str(), &pData);
			for (auto& data : pData) {
				std::string ChildPath = Path;
				ChildPath += data.cFileName;
				ChildPath += "/";
				FPS_n2::Guns::GunPartsDataManager::Instance()->Add(ChildPath);
			}
		}

		bool isEquiped = false;
		for (auto& guns : FPS_n2::Guns::GunPartsDataManager::Instance()->m_GunList) {
			if (SaveData::Instance()->GetParam(guns) <= 0) {
				SaveData::Instance()->SetParam(guns, 0);
			}
			else {
				isEquiped = true;
			}
		}
		if (!isEquiped) {
			//デフォ装備
			SaveData::Instance()->SetParam("type89", 1);
			SaveData::Instance()->SetParam("P226", 2);
			SaveData::Instance()->SetParam("RGD5", 3);
		}
	}
	SaveData::Instance()->Save();
	//BGM
	//*
	auto* SE = SoundPool::Instance();
	SE->Add(SoundType::BGM, 0, 1, "data/Sound/BGM/title.wav", false);
	SE->Add(SoundType::BGM, 1, 1, "data/Sound/BGM/opening.wav", false);
	SE->Add(SoundType::BGM, 2, 1, "data/Sound/BGM/result.wav", false);
	//*/
	//シーン
	auto TitleLoadScenePtr = std::make_shared<FPS_n2::Sceneclass::LoadScene>();
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto MainLoadScenePtr = std::make_shared<FPS_n2::Sceneclass::LoadScene>();
	auto MainGameScenePtr = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	auto TutorialLoadScenePtr = std::make_shared<FPS_n2::Sceneclass::LoadScene>();
	auto TutorialScenePtr = std::make_shared<FPS_n2::Sceneclass::MainGameScene>();
	MainGameScenePtr->SetTutorial(false);
	TutorialScenePtr->SetTutorial(true);
	//遷移先指定
	TitleLoadScenePtr->SetNextSceneList(0, Titlescene);
	Titlescene->SetNextSceneList(0, MainLoadScenePtr);
	Titlescene->SetNextSceneList(1, TutorialLoadScenePtr);
	MainLoadScenePtr->SetNextSceneList(0, MainGameScenePtr);
	TutorialLoadScenePtr->SetNextSceneList(0, TutorialScenePtr);
	MainGameScenePtr->SetNextSceneList(0, TitleLoadScenePtr);
	MainGameScenePtr->SetNextSceneList(1, MainLoadScenePtr);
	TutorialScenePtr->SetNextSceneList(0, TitleLoadScenePtr);
	TutorialScenePtr->SetNextSceneList(1, MainLoadScenePtr);
	SceneControl::Instance()->SetFirstScene(TitleLoadScenePtr);
	//SceneControl::Instance()->SetFirstScene(MainLoadScenePtr);
	//メインロジック開始
	DXLib_refParts->MainLogic();
	return 0;
}
