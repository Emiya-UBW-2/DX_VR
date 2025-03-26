#include	"sub.hpp"

#include "MainScene/Player/Player.hpp"
#include "MainScene/Object/Character.hpp"

const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();

			SE->Add(SoundType::SE, (int)SoundEnum::CartFall, 6, "data/Sound/SE/gun/case.wav", false);
			SE->Add(SoundType::SE, (int)SoundEnum::MagFall, 6, "data/Sound/SE/gun/ModFall.wav", false);
			SE->Add(SoundType::SE, (int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Pin, 4, "data/Sound/SE/pin.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Throw, 4, "data/Sound/SE/throw.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::FallGrenade, 4, "data/Sound/SE/fallgrenade.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Explosion, 4, "data/Sound/SE/explosion.wav");
			for (int i = 0; i < 6; i++) {
				SE->Add(SoundType::SE, (int)SoundEnum::Cocking1_0 + i, 4, "data/Sound/SE/gun/autoM870/" + std::to_string(i) + ".wav");
				SE->Add(SoundType::SE, (int)SoundEnum::Cocking2_0 + i, 4, "data/Sound/SE/gun/autoM16/" + std::to_string(i) + ".wav");
				SE->Add(SoundType::SE, (int)SoundEnum::Cocking3_0 + i, 4, "data/Sound/SE/gun/auto1911/" + std::to_string(i) + ".wav");
				SE->Add(SoundType::SE, (int)SoundEnum::Cocking4_0 + i, 4, "data/Sound/SE/gun/autoAK/" + std::to_string(i) + ".wav");
			}
			SE->Add(SoundType::SE, (int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Heart, 3, "data/Sound/SE/move/heart.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Hit, 3, "data/Sound/SE/hit.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::HitMe, 3, "data/Sound/SE/HitMe.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::HitGuard, 3, "data/Sound/SE/Guard.wav");
			for (int i = 0; i < 5; i++) {
				SE->Add(SoundType::SE, (int)SoundEnum::HitGround0 + i, 2, "data/Sound/SE/gun/HitGround/" + std::to_string(i + 1) + ".wav");
			}

			for (int i = 0; i < 6; i++) {
				SE->Add(SoundType::SE, (int)SoundEnum::Man_Hurt1 + i, 2, "data/Sound/SE/voice/hurt_0" + std::to_string(i + 1) + ".wav");
			}
			for (int i = 0; i < 8; i++) {
				SE->Add(SoundType::SE, (int)SoundEnum::Man_Death1 + i, 2, "data/Sound/SE/voice/death_0" + std::to_string(i + 1) + ".wav");
			}
			SE->Add(SoundType::SE, (int)SoundEnum::Man_contact, 10, "data/Sound/SE/voice/contact.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Man_openfire, 10, "data/Sound/SE/voice/openfire.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Man_reload, 10, "data/Sound/SE/voice/reload.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Man_takecover, 10, "data/Sound/SE/voice/takecover.wav");
			SE->Add(SoundType::SE, (int)SoundEnum::Man_breathing, 10, "data/Sound/SE/voice/breathing.wav", false);
			SE->Add(SoundType::SE, (int)SoundEnum::Man_breathend, 10, "data/Sound/SE/voice/breathend.wav", false);
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();

			SE->Get(SoundType::SE, (int)SoundEnum::CartFall)->SetLocalVolume(48);
			SE->Get(SoundType::SE, (int)SoundEnum::MagFall)->SetLocalVolume(48);
			SE->Get(SoundType::SE, (int)SoundEnum::Trigger)->SetLocalVolume(48);
			//SE->Get(SoundType::SE, (int)SoundEnum::AmmoLoad)->SetLocalVolume(48);
			SE->Get(SoundType::SE, (int)SoundEnum::Shot2)->SetLocalVolume(216);
			SE->Get(SoundType::SE, (int)SoundEnum::Shot3)->SetLocalVolume(216);
			SE->Get(SoundType::SE, (int)SoundEnum::RunFoot)->SetLocalVolume(128);
			SE->Get(SoundType::SE, (int)SoundEnum::Heart)->SetLocalVolume(92);
			SE->Get(SoundType::SE, (int)SoundEnum::Hit)->SetLocalVolume(255);
			SE->Get(SoundType::SE, (int)SoundEnum::HitMe)->SetLocalVolume(255);
			SE->Get(SoundType::SE, (int)SoundEnum::HitGuard)->SetLocalVolume(255);

			SE->Get(SoundType::SE, (int)SoundEnum::Man_breathing)->SetLocalVolume(192);
			SE->Get(SoundType::SE, (int)SoundEnum::Man_breathend)->SetLocalVolume(192);
			for (int i = 0; i < 5; i++) {
				SE->Get(SoundType::SE, (int)SoundEnum::HitGround0 + i)->SetLocalVolume(92);
			}
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete(SoundType::SE, (int)SoundEnum::CartFall);
			SE->Delete(SoundType::SE, (int)SoundEnum::MagFall);
			SE->Delete(SoundType::SE, (int)SoundEnum::Trigger);
			SE->Delete(SoundType::SE, (int)SoundEnum::Pin);
			SE->Delete(SoundType::SE, (int)SoundEnum::Throw);
			SE->Delete(SoundType::SE, (int)SoundEnum::FallGrenade);
			SE->Delete(SoundType::SE, (int)SoundEnum::Explosion);
			for (int i = 0; i < 6; i++) {
				SE->Delete(SoundType::SE, (int)SoundEnum::Cocking1_0 + i);
				SE->Delete(SoundType::SE, (int)SoundEnum::Cocking2_0 + i);
				SE->Delete(SoundType::SE, (int)SoundEnum::Cocking3_0 + i);
				SE->Delete(SoundType::SE, (int)SoundEnum::Cocking4_0 + i);
			}
			SE->Delete(SoundType::SE, (int)SoundEnum::RunFoot);
			SE->Delete(SoundType::SE, (int)SoundEnum::StandupFoot);
			SE->Delete(SoundType::SE, (int)SoundEnum::Heart);
			SE->Delete(SoundType::SE, (int)SoundEnum::Hit);
			SE->Delete(SoundType::SE, (int)SoundEnum::HitMe);
			SE->Delete(SoundType::SE, (int)SoundEnum::HitGuard);
			for (int i = 0; i < 5; i++) {
				SE->Delete(SoundType::SE, (int)SoundEnum::HitGround0 + i);
			}
			for (int i = 0; i < 6; i++) {
				SE->Delete(SoundType::SE, (int)SoundEnum::Man_Hurt1 + i);
			}
			for (int i = 0; i < 8; i++) {
				SE->Delete(SoundType::SE, (int)SoundEnum::Man_Death1 + i);
			}
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_contact);
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_openfire);
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_reload);
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_takecover);
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_breathing);
			SE->Delete(SoundType::SE, (int)SoundEnum::Man_breathend);
		}
		//
		void			CommonBattleResource::LoadChara(const std::string& FolderName, PlayerID ID) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto& p = PlayerMngr->GetPlayer(ID);

			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";

			std::shared_ptr<ObjectBaseClass> Ptr = std::make_shared<CharacterClass>();
			ObjMngr->AddObject(Ptr);
			ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
			Ptr->Init();
			p->SetChara(Ptr);
			auto& c = (std::shared_ptr<CharacterClass>&)p->GetChara();
			c->SetPlayerID(ID);
			p->SetAI(std::make_shared<AIControl>());
			//p->GetAI()->SetPlayerID(value);
			//p->GetAI()->Init();
		}
		void			CommonBattleResource::LoadCharaGun(const std::string& FolderName, PlayerID ID, int Sel) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			std::string Path = "data/gun/";
			Path += FolderName;
			Path += "/";

			auto Ptr = std::make_shared<GunClass>();
			ObjMngr->AddObject(Ptr);
			ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
			Ptr->Init();
			auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(ID)->GetChara();
			c->SetGunPtr(Sel, Ptr);
		}
		void			CommonBattleResource::LoadOnlyGun(const std::string& FolderName, int Sel) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			std::string Path = "data/gun/";
			Path += FolderName;
			Path += "/";

			auto Ptr = std::make_shared<GunClass>();
			ObjMngr->AddObject(Ptr);
			ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
			Ptr->Init();
			PlayerMngr->SetGun(Sel, Ptr);
		}
	};
};
