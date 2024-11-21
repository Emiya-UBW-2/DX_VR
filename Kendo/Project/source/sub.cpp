#include	"sub.hpp"

#include	"MainScene/Player/Player.hpp"
#include	"MainScene/Object/Character.hpp"
#include	"MainScene/Object/Weapon.hpp"
#include	"MainScene/Object/Judge.hpp"

const FPS_n2::CommonBattleResource* SingletonBase<FPS_n2::CommonBattleResource>::m_Singleton = nullptr;
const FPS_n2::HitMark* SingletonBase<FPS_n2::HitMark>::m_Singleton = nullptr;

namespace FPS_n2 {
	void			CommonBattleResource::Load(void) noexcept {
		auto* SE = SoundPool::Instance();
		//
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Taiko1), 1, "data/Sound/SE/taiko.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::EV1_1), 1, "data/Sound/Voice/EV1_1.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::EV2_1), 1, "data/Sound/Voice/EV2_1.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::EV2_2), 1, "data/Sound/Voice/EV2_2.wav", false);
		//
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::RunFoot), 4, "data/Sound/SE/move/runfoot.wav");
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot), 2, "data/Sound/SE/move/standup.wav");
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Heart), 5, "data/Sound/SE/move/heart.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Breath), 6, "data/Sound/SE/move/breath.wav", false);
		//
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Swing), 2, "data/Sound/SE/Kendo/Swing.wav");
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Hit), 2, "data/Sound/SE/Kendo/Hit.wav");
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Kendo_GetHit), 1, "data/Sound/SE/Kendo/GetHit.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Foot), 2, "data/Sound/SE/Kendo/Foot.wav");
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Tsuba), 2, "data/Sound/SE/Kendo/Tsuba.wav");
		//
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Voice_Ya), 2, "data/Sound/SE/voice/Ya.wav");
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Voice_Men), 2, "data/Sound/SE/voice/Men.wav");
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Voice_Kote), 2, "data/Sound/SE/voice/Kote.wav");
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Voice_Dou), 2, "data/Sound/SE/voice/Dou.wav");
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Voice_Tsuki), 2, "data/Sound/SE/voice/Tsuki.wav");

		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Men), 2, "data/Sound/Voice/Men.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Kote), 2, "data/Sound/Voice/Kote.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Dou), 2, "data/Sound/Voice/Dou.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Start), 2, "data/Sound/Voice/Start.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Stop), 2, "data/Sound/Voice/Stop.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Win), 2, "data/Sound/Voice/Win.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Draw), 2, "data/Sound/Voice/Draw.wav", false);
		//
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::CountDown), 2, "data/Sound/SE/CountDown.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::TimeUp), 1, "data/Sound/SE/TimeUp.wav", false);

		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base), 1, "data/Sound/SE/Audience_Base.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Audience_Near), 3, "data/Sound/SE/Audience_Near.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundEnum::Audience_Good), 3, "data/Sound/SE/Audience_Good.wav", false);

	}
	void			CommonBattleResource::Set(void) noexcept {
		auto* SE = SoundPool::Instance();
		SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Taiko1))->SetLocalVolume(216);
		SE->Get(SoundType::SE, static_cast<int>(SoundEnum::EV1_1))->SetLocalVolume(128);

		SE->Get(SoundType::SE, static_cast<int>(SoundEnum::EV2_1))->SetLocalVolume(128);
		SE->Get(SoundType::SE, static_cast<int>(SoundEnum::EV2_2))->SetLocalVolume(128);

		SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->SetLocalVolume(128);
		SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heart))->SetLocalVolume(92);
	}
	void			CommonBattleResource::Dispose(void) noexcept {
		auto* SE = SoundPool::Instance();
		//
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Taiko1));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::EV1_1));

		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::EV2_1));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::EV2_2));
		//
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::RunFoot));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Swing));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Hit));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Kendo_GetHit));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Foot));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Tsuba));

		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Voice_Ya));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Voice_Men));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Voice_Kote));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Voice_Dou));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Voice_Tsuki));

		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Men));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Kote));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Dou));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Start));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Stop));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Win));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::JudgeVoice_Draw));

		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Heart));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Breath));

		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::CountDown));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::TimeUp));

		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Audience_Base));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Audience_Near));
		SE->Delete(SoundType::SE, static_cast<int>(SoundEnum::Audience_Good));
	}
	//
	void			CommonBattleResource::LoadChara(const std::string& FolderName, PlayerID ID) noexcept {
		auto* PlayerMngr = Player::PlayerManager::Instance();
		auto* ObjMngr = ObjectManager::Instance();
		auto& p = PlayerMngr->GetPlayer(ID);

		std::string Path = "data/Charactor/";
		Path += FolderName;
		Path += "/";

		p->SetChara(std::make_shared<CharacterObject::CharacterClass>());
		ObjMngr->AddObject(p->GetChara());
		ObjMngr->LoadModel(p->GetChara(), p->GetChara(), Path.c_str());
		p->GetChara()->Init();
		p->GetChara()->SetPlayerID(ID);
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
		auto& c = PlayerMngr->GetPlayer(ID)->GetChara();
		c->SetWeaponPtr(Ptr);
	}
	void CommonBattleResource::LoadJudge(const std::string& FolderName) noexcept
	{
		auto* ObjMngr = ObjectManager::Instance();

		std::string Path = "data/Charactor/";
		Path += FolderName;
		Path += "/";

		auto Ptr = std::make_shared<Sceneclass::JudgeClass>();
		ObjMngr->AddObject(Ptr);
		ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
		Ptr->Init();
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
		auto* DXLib_refParts = DXLib_ref::Instance();
		for (auto& h : m_HitPos) {
			if (h.Time <= 0.f) { continue; }
			if (h.Time == h.TimeMax || h.m_Pos2D.z == -100.f) {
				auto tmp = ConvWorldPosToScreenPos(h.m_Pos.get());
				if (tmp.z >= 0.f && tmp.z <= 1.f) {
					h.m_Pos2D = tmp;
				}
			}
			h.Time = std::max(h.Time - DXLib_refParts->GetDeltaTime(), 0.f);
		}
	}
	void HitMark::Draw(void) noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		for (auto& h : m_HitPos) {
			if (h.Time <= 0.f) { continue; }
			if (h.m_Pos2D.z >= 0.f && h.m_Pos2D.z <= 1.f) {
				float		Per = std::clamp(1.f - h.Time / h.TimeMax, 0.f, 1.f);
				int			Alpha = std::clamp(static_cast<int>(std::sin(Per * 2.f * DX_PI_F) * h.m_Per * 255.f), 0, 255);
				float		Scale = Per * 10.f * h.m_Per;
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, Alpha);
				switch (h.m_Color) {
				case HitType::Head://–Ê
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 0, 0);
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, 
						&MenGraph, static_cast<int>(h.m_Pos2D.x), static_cast<int>(h.m_Pos2D.y), static_cast<float>(static_cast<int>(Scale * 0.5f * 100.0f)) / 100.f, 0.f, true);
					break;
				case HitType::Arm://¬Žè
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 128, 0);
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, 
						&KoteGraph, static_cast<int>(h.m_Pos2D.x), static_cast<int>(h.m_Pos2D.y), static_cast<float>(static_cast<int>(Scale * 0.5f * 100.0f)) / 100.f, 0.f, true);
					break;
				case HitType::Body://“·
					DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 0);
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, 
						&DoGraph, static_cast<int>(h.m_Pos2D.x), static_cast<int>(h.m_Pos2D.y), static_cast<float>(static_cast<int>(Scale * 0.5f * 100.0f)) / 100.f, 0.f, true);
					break;
				case HitType::Leg:
				default:
					break;
				}

			}
		}
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
		DrawCtrls->SetBright(WindowSystem::DrawLayer::Normal, 255, 255, 255);
	}
	void HitMark::Dispose(void) noexcept {
		this->MenGraph.Dispose();
		this->KoteGraph.Dispose();
		this->DoGraph.Dispose();
	}
};
