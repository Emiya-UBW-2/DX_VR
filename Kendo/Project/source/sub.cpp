#include	"sub.hpp"

#include "MainScene/Player/Player.hpp"
#include "MainScene/Object/Character.hpp"
#include "MainScene/Object/Weapon.hpp"

const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::Sliding, 2, "data/Sound/SE/move/sliding.wav");
			SE->Add((int)SoundEnum::RunFoot, 4, "data/Sound/SE/move/runfoot.wav");
			SE->Add((int)SoundEnum::StandupFoot, 2, "data/Sound/SE/move/standup.wav");
			SE->Add((int)SoundEnum::Heart, 2, "data/Sound/SE/move/heart.wav", false);
			//
			SE->Add((int)SoundEnum::Kendo_Swing, 2, "data/Sound/SE/Kendo/Swing.wav");
			SE->Add((int)SoundEnum::Kendo_Hit, 2, "data/Sound/SE/Kendo/Hit.wav");
			SE->Add((int)SoundEnum::Kendo_Foot, 2, "data/Sound/SE/Kendo/Foot.wav");
			//
			SE->Add((int)SoundEnum::Voice_Ya, 2, "data/Sound/SE/voice/Ya.wav");
			SE->Add((int)SoundEnum::Voice_Men, 2, "data/Sound/SE/voice/Men.wav");
			SE->Add((int)SoundEnum::Voice_Kote, 2, "data/Sound/SE/voice/Kote.wav");
			SE->Add((int)SoundEnum::Voice_Dou, 2, "data/Sound/SE/voice/Dou.wav");
			SE->Add((int)SoundEnum::Voice_Tsuki, 2, "data/Sound/SE/voice/Tsuki.wav");
			//
			SE->Add((int)SoundEnum::CountDown, 2, "data/Sound/SE/CountDown.wav", false);
			SE->Add((int)SoundEnum::Second, 1, "data/Sound/SE/second.wav", false);
			SE->Add((int)SoundEnum::OneMunute, 1, "data/Sound/SE/OneMinute.wav", false);
			SE->Add((int)SoundEnum::TimeUp, 1, "data/Sound/SE/TimeUp.wav", false);

		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();

			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
			SE->Get((int)SoundEnum::CountDown).SetVol_Local(128);

			SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete((int)SoundEnum::Sliding);
			SE->Delete((int)SoundEnum::RunFoot);

			SE->Delete((int)SoundEnum::Kendo_Swing);
			SE->Delete((int)SoundEnum::Kendo_Hit);
			SE->Delete((int)SoundEnum::Kendo_Foot);

			SE->Delete((int)SoundEnum::Voice_Ya);
			SE->Delete((int)SoundEnum::Voice_Men);
			SE->Delete((int)SoundEnum::Voice_Kote);
			SE->Delete((int)SoundEnum::Voice_Dou);
			SE->Delete((int)SoundEnum::Voice_Tsuki);

			SE->Delete((int)SoundEnum::StandupFoot);
			SE->Delete((int)SoundEnum::Heart);
			SE->Delete((int)SoundEnum::CountDown);
			SE->Delete((int)SoundEnum::Second);
			SE->Delete((int)SoundEnum::OneMunute);
			SE->Delete((int)SoundEnum::TimeUp);
		}
		//
		void			CommonBattleResource::LoadChara(const std::string&FolderName, PlayerID ID) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";

			std::shared_ptr<ObjectBaseClass> Ptr = std::make_shared<CharacterClass>();
			ObjMngr->AddObject(Ptr);
			ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
			Ptr->Init();
			PlayerManager::Instance()->GetPlayer(ID).SetChara(Ptr);
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
			auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(ID).GetChara();
			c->SetWeaponPtr(Ptr);
		}
	};
};
