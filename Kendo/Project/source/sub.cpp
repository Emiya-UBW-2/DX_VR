#include	"sub.hpp"

#include "MainScene/Player/Player.hpp"
#include "MainScene/Object/Character.hpp"
#include "MainScene/Object/Weapon.hpp"

const FPS_n2::CommonBattleResource* SingletonBase<FPS_n2::CommonBattleResource>::m_Singleton = nullptr;
const FPS_n2::HitMark* SingletonBase<FPS_n2::HitMark>::m_Singleton = nullptr;

namespace FPS_n2 {
	void			CommonBattleResource::Load(void) noexcept {
		auto* SE = SoundPool::Instance();
		//
		SE->Add((int)SoundEnum::Taiko1, 1, "data/Sound/SE/taiko.wav", false);
		SE->Add((int)SoundEnum::EV1_1, 1, "data/Sound/Voice/EV1_1.wav", false);
		SE->Add((int)SoundEnum::EV2_1, 1, "data/Sound/Voice/EV2_1.wav", false);
		SE->Add((int)SoundEnum::EV2_2, 1, "data/Sound/Voice/EV2_2.wav", false);
		//
		SE->Add(static_cast<int>(SoundEnum::RunFoot), 4, "data/Sound/SE/move/runfoot.wav");
		SE->Add(static_cast<int>(SoundEnum::StandupFoot), 2, "data/Sound/SE/move/standup.wav");
		SE->Add(static_cast<int>(SoundEnum::Heart), 5, "data/Sound/SE/move/heart.wav", false);
		//
		SE->Add(static_cast<int>(SoundEnum::Kendo_Swing), 2, "data/Sound/SE/Kendo/Swing.wav");
		SE->Add(static_cast<int>(SoundEnum::Kendo_Hit), 2, "data/Sound/SE/Kendo/Hit.wav");
		SE->Add(static_cast<int>(SoundEnum::Kendo_Foot), 2, "data/Sound/SE/Kendo/Foot.wav");
		SE->Add(static_cast<int>(SoundEnum::Kendo_Tsuba), 2, "data/Sound/SE/Kendo/Tsuba.wav");
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
		SE->Get((int)SoundEnum::Taiko1).SetVol_Local(216);
		SE->Get((int)SoundEnum::EV1_1).SetVol_Local(128);

		SE->Get((int)SoundEnum::EV2_1).SetVol_Local(128);
		SE->Get((int)SoundEnum::EV2_2).SetVol_Local(128);

		SE->Get(static_cast<int>(SoundEnum::RunFoot)).SetVol_Local(128);
		SE->Get(static_cast<int>(SoundEnum::Heart)).SetVol_Local(92);
		SE->Get(static_cast<int>(SoundEnum::CountDown)).SetVol_Local(128);

		SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
	}
	void			CommonBattleResource::Dispose(void) noexcept {
		auto* SE = SoundPool::Instance();
		//
		SE->Delete((int)SoundEnum::Taiko1);
		SE->Delete((int)SoundEnum::EV1_1);

		SE->Delete((int)SoundEnum::EV2_1);
		SE->Delete((int)SoundEnum::EV2_2);
		//
		SE->Delete(static_cast<int>(SoundEnum::RunFoot));
		SE->Delete(static_cast<int>(SoundEnum::Kendo_Swing));
		SE->Delete(static_cast<int>(SoundEnum::Kendo_Hit));
		SE->Delete(static_cast<int>(SoundEnum::Kendo_Foot));
		SE->Delete(static_cast<int>(SoundEnum::Kendo_Tsuba));

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
	void			CommonBattleResource::LoadChara(const std::string& FolderName, PlayerID ID) noexcept {
		auto* PlayerMngr = Player::PlayerManager::Instance();
		auto* ObjMngr = ObjectManager::Instance();
		auto& p = PlayerMngr->GetPlayer(ID);

		std::string Path = "data/Charactor/";
		Path += FolderName;
		Path += "/";

		std::shared_ptr<ObjectBaseClass> Ptr = std::make_shared<CharacterObject::CharacterClass>();
		ObjMngr->AddObject(Ptr);
		ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
		Ptr->Init();
		p->SetChara(Ptr);
		auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
		c->SetPlayerID(ID);
		p->SetAI(std::make_shared<Player::AIControl>());
		//p->GetAI()->SetPlayerID(value);
		//p->GetAI()->Init();
	}
	void			CommonBattleResource::LoadWeapon(const std::string& FolderName, PlayerID ID) noexcept {
		auto* ObjMngr = ObjectManager::Instance();
		auto* PlayerMngr = Player::PlayerManager::Instance();

		std::string Path = "data/Weapon/";
		Path += FolderName;
		Path += "/";

		auto Ptr = std::make_shared<WeaponObject::WeaponClass>();
		ObjMngr->AddObject(Ptr);
		ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
		Ptr->Init();
		auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(ID)->GetChara();
		c->SetWeaponPtr(Ptr);
	}
	void HitMark::Load(void) noexcept {
		this->MenGraph.Load("data/UI/hit_Men.bmp");
		this->KoteGraph.Load("data/UI/hit_Kote.bmp");
		this->DoGraph.Load("data/UI/hit_Do.bmp");
	}
	void HitMark::Set(void) noexcept {
		for (auto& h : m_HitPos) {
			h.Time = -1.f;
		}
	}
	void HitMark::Update(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		for (auto& h: m_HitPos) {
			if (h.Time <= 0.f) { continue; }
			if (h.Time == h.TimeMax) {
				auto tmp = ConvWorldPosToScreenPos(h.m_Pos.get());
				if (tmp.z >= 0.f && tmp.z <= 1.f) {
					h.m_Pos2D = tmp;
					h.m_Pos2D = h.m_Pos2D * ((float)DrawParts->GetUIY(1080) / (float)DrawParts->GetScreenY(1080));
				}
			}
			h.Time = std::max(h.Time - 1.f / DrawParts->GetFps(), 0.f);
		}
	}
	void HitMark::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		for (auto& h : m_HitPos) {
			if (h.Time <= 0.f) { continue; }
			if (h.m_Pos2D.z >= 0.f && h.m_Pos2D.z <= 1.f) {
				float		Per = std::clamp(1.f - h.Time / h.TimeMax, 0.f, 1.f);
				int			Alpha = std::clamp((int)(std::sin(Per * 2.f * DX_PI_F) * h.m_Per * 255.f), 0, 255);
				float		Scale = Per * 10.f * h.m_Per;
				WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, Alpha);
				switch (h.m_Color) {
				case HitType::Head://–Ê
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);
					WindowSystem::DrawControl::Instance()->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, 
						&MenGraph, (int)h.m_Pos2D.x, (int)h.m_Pos2D.y, (float)DrawParts->GetUIY((int)(Scale * 0.5f * 100.0f)) / 100.f, 0.f, true);
					break;
				case HitType::Arm://¬Žè
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 128, 0);
					WindowSystem::DrawControl::Instance()->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, 
						&KoteGraph, (int)h.m_Pos2D.x, (int)h.m_Pos2D.y, (float)DrawParts->GetUIY((int)(Scale * 0.5f * 100.0f)) / 100.f, 0.f, true);
					break;
				case HitType::Body://“·
					WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 0);
					WindowSystem::DrawControl::Instance()->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, 
						&DoGraph, (int)h.m_Pos2D.x, (int)h.m_Pos2D.y, (float)DrawParts->GetUIY((int)(Scale * 0.5f * 100.0f)) / 100.f, 0.f, true);
					break;
				case HitType::Leg:
				default:
					break;
				}

			}
		}
		WindowSystem::DrawControl::Instance()->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
		WindowSystem::DrawControl::Instance()->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
	}
	void HitMark::Dispose(void) noexcept {
		this->MenGraph.Dispose();
		this->KoteGraph.Dispose();
		this->DoGraph.Dispose();
	}
};
