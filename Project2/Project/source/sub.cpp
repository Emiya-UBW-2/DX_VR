#include	"sub.hpp"

#include "MainScene/NetworkBrowser.hpp"

#include "MainScene/Object/VehicleData.hpp"

const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::Environment, 1, "data/Sound/SE/envi.wav");
			for (int i = 0; i < 9; i++) {
				SE->Add((int)SoundEnum::Tank_Shot, 3, "data/Sound/SE/gun/fire/" + std::to_string(i) + ".wav");
			}
			for (int i = 0; i < 17; i++) {
				SE->Add((int)SoundEnum::Tank_Ricochet, 3, "data/Sound/SE/ricochet/" + std::to_string(i) + ".wav");
			}
			for (int i = 0; i < 2; i++) {
				SE->Add((int)SoundEnum::Tank_Damage, 3, "data/Sound/SE/damage/" + std::to_string(i) + ".wav");
			}
			SE->Add((int)SoundEnum::Tank_move, Player_num, "data/Sound/SE/Engine/move.wav");
			SE->Add((int)SoundEnum::Tank_engine, Player_num, "data/Sound/SE/Engine/engine.wav");
			SE->Add((int)SoundEnum::Tank_Start, Player_num, "data/Sound/SE/Engine/start.wav");
			for (int i = 0; i < 7; i++) {
				SE->Add((int)SoundEnum::Tank_Eject, 3, "data/Sound/SE/gun/reload/eject/" + std::to_string(i) + ".wav", false);
			}
			for (int i = 0; i < 5; i++) {
				SE->Add((int)SoundEnum::Tank_Reload, 3, "data/Sound/SE/gun/reload/hand/" + std::to_string(i) + ".wav", false);
			}
			SE->Add((int)SoundEnum::Tank_near, 10, "data/Sound/SE/near.wav");
			VehDataControl::Instance()->Load();
			ItemDataControl::Instance()->Load();
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Get((int)SoundEnum::Environment).SetVol(0.25f);
			SE->Get((int)SoundEnum::Tank_Shot).SetVol(0.5f);
			SE->Get((int)SoundEnum::Tank_engine).SetVol(0.5f);
			SE->Get((int)SoundEnum::Tank_Start).SetVol(0.5f);
			SE->Get((int)SoundEnum::Tank_move).SetVol(0.25f);
			SE->Get((int)SoundEnum::Tank_Ricochet).SetVol(0.65f);
			SE->Get((int)SoundEnum::Tank_Damage).SetVol(0.65f);
			SE->Get((int)SoundEnum::Tank_Eject).SetVol(0.25f);
			SE->Get((int)SoundEnum::Tank_Reload).SetVol(0.25f);
			SE->Get((int)SoundEnum::Tank_near).SetVol(1.f);
			VehDataControl::Instance()->Set();
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete((int)SoundEnum::Environment);
			SE->Delete((int)SoundEnum::Tank_Shot);
			SE->Delete((int)SoundEnum::Tank_engine);
			SE->Delete((int)SoundEnum::Tank_Start);
			SE->Delete((int)SoundEnum::Tank_move);
			SE->Delete((int)SoundEnum::Tank_Ricochet);
			SE->Delete((int)SoundEnum::Tank_Damage);
			SE->Delete((int)SoundEnum::Tank_Eject);
			SE->Delete((int)SoundEnum::Tank_Reload);
			SE->Delete((int)SoundEnum::Tank_near);
			VehDataControl::Instance()->Dispose();
			ItemDataControl::Instance()->Dispose();
		}
		//
		void			CommonBattleResource::LoadTank(const std::string&FolderName, PlayerID ID) noexcept {
			//auto* ObjMngr = ObjectManager::Instance();

			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";

			//std::shared_ptr<ObjectBaseClass> Ptr = std::make_shared<CharacterClass>();
			//ObjMngr->AddObject(Ptr);
			//ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
			//Ptr->Init();
			//PlayerManager::Instance()->GetPlayer(ID).SetChara(Ptr);
		}
	};
};
