#include	"sub.hpp"

#include "MainScene/Player/Player.hpp"
#include "MainScene/Object/Character.hpp"

const FPS_n2::GetItemLog* SingletonBase<FPS_n2::GetItemLog>::m_Singleton = nullptr;
const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::Env, 1, "data/Sound/SE/envi.wav", false);
			SE->Add((int)SoundEnum::Env2, 1, "data/Sound/SE/envi2.wav", false);

			SE->Add((int)SoundEnum::CartFall, 6, "data/Sound/SE/gun/case.wav", false);
			SE->Add((int)SoundEnum::MagFall, 6, "data/Sound/SE/gun/ModFall.wav", false);
			SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
			SE->Add((int)SoundEnum::AmmoLoad, 1, "data/Sound/SE/gun/ammoload.wav", false);
			SE->Add((int)SoundEnum::Tinnitus, 2, "data/Sound/SE/Tinnitus.wav");
			for (int i = 0; i < 6; i++) {
				SE->Add((int)SoundEnum::Cocking1_0 + i, 4, "data/Sound/SE/gun/autoM870/" + std::to_string(i) + ".wav");
				SE->Add((int)SoundEnum::Cocking2_0 + i, 4, "data/Sound/SE/gun/autoM16/" + std::to_string(i) + ".wav");
				SE->Add((int)SoundEnum::Cocking3_0 + i, 4, "data/Sound/SE/gun/auto1911/" + std::to_string(i) + ".wav");
				SE->Add((int)SoundEnum::Cocking4_0 + i, 4, "data/Sound/SE/gun/autoAK/" + std::to_string(i) + ".wav");
			}
			SE->Add((int)SoundEnum::StandUp, 1, "data/Sound/SE/move/sliding.wav", false);
			SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add((int)SoundEnum::SlideFoot, 3, "data/Sound/SE/move/sliding.wav");
			SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
			SE->Add((int)SoundEnum::GetAmmo, 3, "data/Sound/SE/move/getammo.wav");
			SE->Add((int)SoundEnum::Heart, 3, "data/Sound/SE/move/heart.wav");
			SE->Add((int)SoundEnum::Hit, 3, "data/Sound/SE/hit.wav");
			SE->Add((int)SoundEnum::HitMe, 3, "data/Sound/SE/HitMe.wav");
			SE->Add((int)SoundEnum::HitGuard, 3, "data/Sound/SE/Guard.wav");
			SE->Add((int)SoundEnum::CountDown, 2, "data/Sound/SE/CountDown.wav", false);
			for (int i = 0; i < 5; i++) {
				SE->Add((int)SoundEnum::HitGround0 + i, 2, "data/Sound/SE/gun/HitGround/" + std::to_string(i + 1) + ".wav");
			}

			for (int i = 0; i < 6; i++) {
				SE->Add((int)SoundEnum::Man_Hurt1 + i, 2, "data/Sound/SE/voice/hurt_0" + std::to_string(i + 1) + ".wav");
			}
			for (int i = 0; i < 8; i++) {
				SE->Add((int)SoundEnum::Man_Death1 + i, 2, "data/Sound/SE/voice/death_0" + std::to_string(i + 1) + ".wav");
			}
			SE->Add((int)SoundEnum::Man_contact, 10, "data/Sound/SE/voice/contact.wav");
			SE->Add((int)SoundEnum::Man_openfire, 10, "data/Sound/SE/voice/openfire.wav");
			SE->Add((int)SoundEnum::Man_reload, 10, "data/Sound/SE/voice/reload.wav");
			SE->Add((int)SoundEnum::Man_takecover, 10, "data/Sound/SE/voice/takecover.wav");
			SE->Add((int)SoundEnum::Man_breathing, 10, "data/Sound/SE/voice/breathing.wav", false);
			SE->Add((int)SoundEnum::Man_breathend, 10, "data/Sound/SE/voice/breathend.wav", false);


			SE->Add((int)SoundEnum::Near, 5, "data/Sound/SE/near.wav");
			SE->Add((int)SoundEnum::Stim, 1, "data/Sound/SE/Stim.wav", false);

			SE->Add((int)SoundEnum::Second, 1, "data/Sound/SE/second.wav", false);
			SE->Add((int)SoundEnum::OneMunute, 1, "data/Sound/SE/OneMinute.wav", false);
			SE->Add((int)SoundEnum::TimeUp, 1, "data/Sound/SE/TimeUp.wav", false);
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();

			SE->Get((int)SoundEnum::CartFall).SetVol_Local(48);
			SE->Get((int)SoundEnum::MagFall).SetVol_Local(48);
			SE->Get((int)SoundEnum::Trigger).SetVol_Local(48);
			//SE->Get((int)SoundEnum::AmmoLoad).SetVol_Local(48);
			SE->Get((int)SoundEnum::Shot2).SetVol_Local(216);
			SE->Get((int)SoundEnum::Shot3).SetVol_Local(216);
			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
			SE->Get((int)SoundEnum::Hit).SetVol_Local(255);
			SE->Get((int)SoundEnum::HitMe).SetVol_Local(255);
			SE->Get((int)SoundEnum::HitGuard).SetVol_Local(255);
			SE->Get((int)SoundEnum::CountDown).SetVol_Local(128);

			SE->Get((int)SoundEnum::Man_breathing).SetVol_Local(192);
			SE->Get((int)SoundEnum::Man_breathend).SetVol_Local(192);
			for (int i = 0; i < 5; i++) {
				SE->Get((int)SoundEnum::HitGround0 + i).SetVol_Local(92);
			}
			SE->SetVol(OptionParts->Get_SE());
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete((int)SoundEnum::CartFall);
			SE->Delete((int)SoundEnum::MagFall);
			SE->Delete((int)SoundEnum::Env);
			SE->Delete((int)SoundEnum::Env2);
			SE->Delete((int)SoundEnum::StandUp);
			SE->Delete((int)SoundEnum::Trigger);
			SE->Delete((int)SoundEnum::AmmoLoad);
			SE->Delete((int)SoundEnum::Tinnitus);
			for (int i = 0; i < 6; i++) {
				SE->Delete((int)SoundEnum::Cocking1_0 + i);
				SE->Delete((int)SoundEnum::Cocking2_0 + i);
				SE->Delete((int)SoundEnum::Cocking3_0 + i);
				SE->Delete((int)SoundEnum::Cocking4_0 + i);
			}
			SE->Delete((int)SoundEnum::RunFoot);
			SE->Delete((int)SoundEnum::SlideFoot);
			SE->Delete((int)SoundEnum::StandupFoot);
			SE->Delete((int)SoundEnum::GetAmmo);
			SE->Delete((int)SoundEnum::Heart);
			SE->Delete((int)SoundEnum::Hit);
			SE->Delete((int)SoundEnum::HitMe);
			SE->Delete((int)SoundEnum::HitGuard);
			SE->Delete((int)SoundEnum::CountDown);
			for (int i = 0; i < 5; i++) {
				SE->Delete((int)SoundEnum::HitGround0 + i);
			}
			for (int i = 0; i < 6; i++) {
				SE->Delete((int)SoundEnum::Man_Hurt1 + i);
			}
			for (int i = 0; i < 8; i++) {
				SE->Delete((int)SoundEnum::Man_Death1 + i);
			}
			SE->Delete((int)SoundEnum::Man_contact);
			SE->Delete((int)SoundEnum::Man_openfire);
			SE->Delete((int)SoundEnum::Man_reload);
			SE->Delete((int)SoundEnum::Man_takecover);
			SE->Delete((int)SoundEnum::Man_breathing);
			SE->Delete((int)SoundEnum::Man_breathend);

			SE->Delete((int)SoundEnum::Near);
			SE->Delete((int)SoundEnum::Stim);

			SE->Delete((int)SoundEnum::Second);
			SE->Delete((int)SoundEnum::OneMunute);
			SE->Delete((int)SoundEnum::TimeUp);
		}
		//
		void			CommonBattleResource::LoadChara(const std::string&FolderName, PlayerID ID) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			auto* Ptr = ObjMngr->MakeObject(ObjType::Human);
			PlayerMngr->GetPlayer(ID).SetChara(*Ptr);

			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";
			ObjMngr->LoadObjectModel((*Ptr).get(), Path.c_str());
			MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());

			(*Ptr)->Init();
		}
		void			CommonBattleResource::LoadGun(const std::string&FolderName, PlayerID ID, int Sel) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			auto* Ptr = ObjMngr->MakeObject(ObjType::Gun);
			PlayerMngr->GetPlayer(ID).SetGun(Sel, *Ptr);

			std::string Path = "data/gun/";
			Path += FolderName;
			Path += "/";
			ObjMngr->LoadObjectModel((*Ptr).get(), Path.c_str());
			MV1::SetAnime(&(*Ptr)->GetObj(), (*Ptr)->GetObj());

			(*Ptr)->Init();
		}
	};
};
