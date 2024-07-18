#include	"sub.hpp"

#include "MainScene/Player/Player.hpp"
#include "MainScene/Object/Character.hpp"
#include "MainScene/Object/Weapon.hpp"

const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add(static_cast<int>(SoundEnum::RunFoot), 4, "data/Sound/SE/move/runfoot.wav");
			SE->Add(static_cast<int>(SoundEnum::StandupFoot), 2, "data/Sound/SE/move/standup.wav");
			SE->Add(static_cast<int>(SoundEnum::Heart), 2, "data/Sound/SE/move/heart.wav", false);
			//
			SE->Add(static_cast<int>(SoundEnum::Kendo_Swing), 2, "data/Sound/SE/Kendo/Swing.wav");
			SE->Add(static_cast<int>(SoundEnum::Kendo_Hit), 2, "data/Sound/SE/Kendo/Hit.wav");
			SE->Add(static_cast<int>(SoundEnum::Kendo_Foot), 2, "data/Sound/SE/Kendo/Foot.wav");
			//
			SE->Add(static_cast<int>(SoundEnum::Voice_Ya), 2, "data/Sound/SE/voice/Ya.wav");
			SE->Add(static_cast<int>(SoundEnum::Voice_Men), 2, "data/Sound/SE/voice/Men.wav");
			SE->Add(static_cast<int>(SoundEnum::Voice_Kote), 2, "data/Sound/SE/voice/Kote.wav");
			SE->Add(static_cast<int>(SoundEnum::Voice_Dou), 2, "data/Sound/SE/voice/Dou.wav");
			SE->Add(static_cast<int>(SoundEnum::Voice_Tsuki), 2, "data/Sound/SE/voice/Tsuki.wav");
			//
			SE->Add(static_cast<int>(SoundEnum::CountDown), 2, "data/Sound/SE/CountDown.wav", false);
			SE->Add(static_cast<int>(SoundEnum::Second), 1, "data/Sound/SE/second.wav", false);
			SE->Add(static_cast<int>(SoundEnum::OneMunute), 1, "data/Sound/SE/OneMinute.wav", false);
			SE->Add(static_cast<int>(SoundEnum::TimeUp), 1, "data/Sound/SE/TimeUp.wav", false);

		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();

			SE->Get(static_cast<int>(SoundEnum::RunFoot)).SetVol_Local(128);
			SE->Get(static_cast<int>(SoundEnum::Heart)).SetVol_Local(92);
			SE->Get(static_cast<int>(SoundEnum::CountDown)).SetVol_Local(128);

			SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete(static_cast<int>(SoundEnum::RunFoot));

			SE->Delete(static_cast<int>(SoundEnum::Kendo_Swing));
			SE->Delete(static_cast<int>(SoundEnum::Kendo_Hit));
			SE->Delete(static_cast<int>(SoundEnum::Kendo_Foot));

			SE->Delete(static_cast<int>(SoundEnum::Voice_Ya));
			SE->Delete(static_cast<int>(SoundEnum::Voice_Men));
			SE->Delete(static_cast<int>(SoundEnum::Voice_Kote));
			SE->Delete(static_cast<int>(SoundEnum::Voice_Dou));
			SE->Delete(static_cast<int>(SoundEnum::Voice_Tsuki));

			SE->Delete(static_cast<int>(SoundEnum::StandupFoot));
			SE->Delete(static_cast<int>(SoundEnum::Heart));
			SE->Delete(static_cast<int>(SoundEnum::CountDown));
			SE->Delete(static_cast<int>(SoundEnum::Second));
			SE->Delete(static_cast<int>(SoundEnum::OneMunute));
			SE->Delete(static_cast<int>(SoundEnum::TimeUp));
		}
		//
		void			CommonBattleResource::LoadChara(const std::string&FolderName, PlayerID ID) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
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
		void			CommonBattleResource::LoadGun(const std::string&FolderName, PlayerID ID) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			std::string Path = "data/Weapon/";
			Path += FolderName;
			Path += "/";

			auto Ptr = std::make_shared<WeaponClass>();
			ObjMngr->AddObject(Ptr);
			ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
			Ptr->Init();
			auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(ID)->GetChara();
			c->SetWeaponPtr(Ptr);
		}
	};
};
