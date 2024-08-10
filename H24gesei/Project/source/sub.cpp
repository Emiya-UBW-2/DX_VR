#include	"sub.hpp"

#include "MainScene/Player/Player.hpp"
#include "MainScene/Object/Character.hpp"

const FPS_n2::CommonBattleResource* SingletonBase<FPS_n2::CommonBattleResource>::m_Singleton = nullptr;
const FPS_n2::HitMark* SingletonBase<FPS_n2::HitMark>::m_Singleton = nullptr;

namespace FPS_n2 {
	void			CommonBattleResource::Load(void) noexcept {
		auto* SE = SoundPool::Instance();
		//
		SE->Add(static_cast<int>(SoundEnum::Shot), 10, "data/Sound/SE/Ship/fire/0.wav");
		SE->Add(static_cast<int>(SoundEnum::ShotBig), 10, "data/Sound/SE/Ship/fire/1.wav");
		SE->Add(static_cast<int>(SoundEnum::load), 10, "data/Sound/SE/Ship/load/0.wav");
		SE->Add(static_cast<int>(SoundEnum::Engine), 1, "data/Sound/SE/Ship/Engine.wav");

		SE->Add(static_cast<int>(SoundEnum::Hit), 3, "data/Sound/SE/Ship/Hit.wav", false);
		SE->Add(static_cast<int>(SoundEnum::Near), 3, "data/Sound/SE/Ship/near.wav", false);
		SE->Add(static_cast<int>(SoundEnum::Damage), 3, "data/Sound/SE/Ship/damage.wav", false);
		SE->Add(static_cast<int>(SoundEnum::Env), 1, "data/Sound/SE/envi.wav", false);

		SE->Add(static_cast<int>(SoundEnum::Gear), 3, "data/Sound/SE/Ship/Gear.wav", false);
		SE->Add(static_cast<int>(SoundEnum::Rotate), 3, "data/Sound/SE/Ship/Rotate.wav", false);

		SE->Add(static_cast<int>(SoundEnum::CountDown), 2, "data/Sound/SE/CountDown.wav", false);
		SE->Add(static_cast<int>(SoundEnum::Second), 1, "data/Sound/SE/second.wav", false);
		SE->Add(static_cast<int>(SoundEnum::OneMunute), 1, "data/Sound/SE/OneMinute.wav", false);
		SE->Add(static_cast<int>(SoundEnum::TimeUp), 1, "data/Sound/SE/TimeUp.wav", false);

	}
	void			CommonBattleResource::Set(void) noexcept {
		auto* SE = SoundPool::Instance();
		auto* OptionParts = OPTION::Instance();

		SE->Get(static_cast<int>(SoundEnum::Shot)).SetVol_Local(128);

		SE->Get(static_cast<int>(SoundEnum::CountDown)).SetVol_Local(128);

		SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
	}
	void			CommonBattleResource::Dispose(void) noexcept {
		auto* SE = SoundPool::Instance();
		SE->Delete(static_cast<int>(SoundEnum::Shot));
		SE->Delete(static_cast<int>(SoundEnum::ShotBig));
		SE->Delete(static_cast<int>(SoundEnum::load));
		SE->Delete(static_cast<int>(SoundEnum::Hit));
		SE->Delete(static_cast<int>(SoundEnum::Near));
		SE->Delete(static_cast<int>(SoundEnum::Damage));

		SE->Delete(static_cast<int>(SoundEnum::Engine));
		SE->Delete(static_cast<int>(SoundEnum::Gear));
		SE->Delete(static_cast<int>(SoundEnum::Rotate));

		SE->Delete(static_cast<int>(SoundEnum::CountDown));
		SE->Delete(static_cast<int>(SoundEnum::Second));
		SE->Delete(static_cast<int>(SoundEnum::OneMunute));
		SE->Delete(static_cast<int>(SoundEnum::TimeUp));
	}
	//
	void			CommonBattleResource::LoadShip(const std::string& FolderPath, PlayerID ID) noexcept {
		auto* PlayerMngr = Player::PlayerManager::Instance();
		auto* ObjMngr = ObjectManager::Instance();
		auto& p = PlayerMngr->GetPlayer(ID);

		std::shared_ptr<ObjectBaseClass> Ptr = std::make_shared<CharacterObject::CharacterClass>();
		ObjMngr->AddObject(Ptr);
		ObjMngr->LoadModel(Ptr, Ptr, FolderPath.c_str());
		Ptr->Init();
		p->SetChara(Ptr);
		auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
		c->SetPlayerID(ID);
		p->SetAI(std::make_shared<Player::AIControl>());
		//p->GetAI()->SetPlayerID(value);
		//p->GetAI()->Init();
	}
	void			CommonBattleResource::LoadShipNPC(const std::string& FolderPath, int Num) noexcept {
		auto* PlayerMngr = Player::PlayerManager::Instance();
		auto* ObjMngr = ObjectManager::Instance();
		auto& p = PlayerMngr->GetNPC(Num);

		std::shared_ptr<ObjectBaseClass> Ptr = std::make_shared<CharacterObject::CharacterClass>();
		ObjMngr->AddObject(Ptr);
		ObjMngr->LoadModel(Ptr, Ptr, FolderPath.c_str());
		Ptr->Init();
		p->SetChara(Ptr);
		//auto& c = (std::shared_ptr<CharacterObject::CharacterClass>&)p->GetChara();
		p->SetAI(std::make_shared<Player::AIControl>());
		//p->GetAI()->SetPlayerID(value);
		//p->GetAI()->Init();
	}
	void HitMark::Load(void) noexcept {
		this->MenGraph = GraphHandle::Load("data/UI/battle_hit.bmp");
	}
	void HitMark::Set(void) noexcept {
		for (auto& h : m_HitPos) {
			h.Time = -1.f;
		}
	}
	void HitMark::Update(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		for (auto& h : m_HitPos) {
			if (h.Time <= 0.f) { continue; }
			if (h.Time == h.TimeMax) {
				h.m_Pos2D.z = -1.f;
			}
			else {
				h.IsFirstTime = false;
			}
			h.Time = std::max(h.Time - 1.f / DrawParts->GetFps(), 0.f);
		}
	}
	void HitMark::CheckDraw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		for (auto& h: m_HitPos) {
			if (h.Time <= 0.f) { continue; }
			if (h.IsFirstTime) {
				auto tmp = ConvWorldPosToScreenPos(h.m_Pos.get());
				if (tmp.z >= 0.f && tmp.z <= 1.f) {
					h.m_Pos2D = tmp;
					h.m_Pos2D = h.m_Pos2D * ((float)DrawParts->GetUIY(1080) / (float)DrawParts->GetScreenY(1080));
				}
			}
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
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
				if (h.m_Color == HitType::Head) {
					SetDrawBright(255, 0, 0);
				}
				if (h.m_Color == HitType::Body) {
					SetDrawBright(0, 0, 255);
				}
				MenGraph.DrawRotaGraph((int)h.m_Pos2D.x, (int)h.m_Pos2D.y, (float)DrawParts->GetUIY((int)(Scale * 0.5f * 100.0f)) / 100.f, 0.f, true);

				auto* Fonts = FontPool::Instance();
				Fonts->Get(FontPool::FontType::MS_Gothic, DrawParts->GetUIY(32), 3)->DrawString(INVALID_ID, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
					(int)h.m_Pos2D.x, (int)h.m_Pos2D.y, White, Black,
					"%3d", h.Damage);
			}
		}
		SetDrawBright(255, 255, 255);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	void HitMark::Dispose(void) noexcept {
		this->MenGraph.Dispose();
		this->KoteGraph.Dispose();
		this->DoGraph.Dispose();
	}
};
