#include	"../Header.hpp"
#include	"MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MAINLOOP::Set_Sub(void) noexcept {
			WeaponAnimManager::Create();
			auto* AnimMngr = WeaponAnimManager::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			SetAmbientShadow(
				VECTOR_ref::vget(Scale_Rate*-100.f, Scale_Rate*-10.f, Scale_Rate*-100.f),
				VECTOR_ref::vget(Scale_Rate*100.f, Scale_Rate*50.f, Scale_Rate*100.f),
				VECTOR_ref::vget(-0.8f, -0.5f, -0.1f),
				GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
			//Load
			//BG
			this->m_BackGround = std::make_shared<BackGroundClass>();
			this->m_BackGround->Init();
			//
			ObjMngr->Init(this->m_BackGround);
			for (int i = 0; i < Chara_num / 2; i++) {
				character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/saber/")));
				//character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/berserker/")));
				this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
			}
			for (int i = Chara_num / 2; i < Chara_num; i++) {
				character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Human, "data/Charactor/berserker/")));
				this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
			}
			m_Shader[0].Init("CubeMapTestShader_VS.vso", "CubeMapTestShader_PS.pso");
			m_RealTimeCubeMap.Init();
			for (int i = 0; i < Weapon_num / 2; i++) {
				ObjMngr->AddObject(ObjType::Weapon, "data/Weapon/Excalibur/");
				//ObjMngr->AddObject(ObjType::Weapon, "data/Weapon/berserker/");
			}
			for (int i = Weapon_num / 2; i < Weapon_num; i++) {
				ObjMngr->AddObject(ObjType::Weapon, "data/Weapon/berserker/");
			}
			//ロード
			SetCreate3DSoundFlag(FALSE);
			this->m_Env = SoundHandle::Load("data/Sound/SE/envi.wav");
			this->m_AimOn = SoundHandle::Load("data/Sound/SE/aim_on.wav");
			SetCreate3DSoundFlag(FALSE);
			this->m_Env.vol(64);
			//UI
			this->m_UIclass.Set();

			autoaimpoint_Graph = GraphHandle::Load("data/UI/battle_autoaimpoint.bmp");
			Enemyaimpoint_Graph = GraphHandle::Load("data/UI/battle_enemyaimpoint.bmp");
			hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
			aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
			stand_Graph[0] = GraphHandle::Load("data/UI/Stand.bmp");
			stand_Graph[1] = GraphHandle::Load("data/UI/Squat.bmp");
			stand_Graph[2] = GraphHandle::Load("data/UI/StandR.bmp");
			stand_Graph[3] = GraphHandle::Load("data/UI/SquatR.bmp");
			stand_Graph[4] = GraphHandle::Load("data/UI/Stand_2.bmp");
			stand_Graph[5] = GraphHandle::Load("data/UI/Squat_2.bmp");
			stand_Graph[6] = GraphHandle::Load("data/UI/StandR_2.bmp");
			stand_Graph[7] = GraphHandle::Load("data/UI/SquatR_2.bmp");
			stand_AnimTime = 5.f;
			//Set
			//人
			for (int loop = 0; loop < (int)EnumWeaponAnim::Max; loop++) {
				std::string Path = "data/CharaAnime/";
				Path += EnumWeaponAnimName[loop];
				Path += ".txt";
				AnimMngr->LoadAction(Path.c_str(), (EnumWeaponAnim)loop);
			}
			for (auto& c : this->character_Pool) {
				size_t index = &c - &this->character_Pool.front();

				VECTOR_ref pos_t;
				float rad_t = 0.f;
				if (index < Chara_num / 2) {
					pos_t = VECTOR_ref::vget(22.f*Scale_Rate - (float)(index)*2.f*Scale_Rate, 0.f, 22.f*Scale_Rate);
					rad_t = deg2rad(45.f);
				}
				else {
					pos_t = VECTOR_ref::vget(-22.f*Scale_Rate + (float)((index - Chara_num / 2))*2.f*Scale_Rate, 0.f, -22.f*Scale_Rate);
					rad_t = deg2rad(180.f + 45.f);
				}



				auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
				if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
				c->ValueSet(deg2rad(0.f), rad_t, false, pos_t, (PlayerID)index);
				c->SetWeaponPtr((std::shared_ptr<WeaponClass>&)(*ObjMngr->GetObj(ObjType::Weapon, (int)(index))));
				if (index < Chara_num / 2) {
					c->SetUseRealTimePhysics(true);
					//c->SetUseRealTimePhysics(false);
					c->SetCharaType(CharaTypeID::Team);
				}
				else {
					c->SetUseRealTimePhysics(true);
					//c->SetUseRealTimePhysics(false);
					c->SetCharaType(CharaTypeID::Enemy);
				}
			}
			//player
			PlayerMngr->Init(Player_num);
			for (int i = 0; i < Player_num; i++) {
				PlayerMngr->GetPlayer(i).SetChara((std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i)));
				//PlayerMngr->GetPlayer(i).SetChara(nullptr);

				this->m_AICtrl[i]->Init(&this->character_Pool, this->m_BackGround, PlayerMngr->GetPlayer(i).GetChara());
			}
			this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetChara()->GetHP();
			this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();

			//Cam
			SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
			SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
			//サウンド
			auto SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/Weapon/trigger.wav");
			for (int i = 0; i < 4; i++) {
				SE->Add((int)SoundEnum::Cocking1_0 + i, 3, "data/Sound/SE/Weapon/bolt/" + std::to_string(i) + ".wav");
			}
			for (int i = 0; i < 5; i++) {
				SE->Add((int)SoundEnum::Cocking2_0 + i, 3, "data/Sound/SE/Weapon/autoM16/" + std::to_string(i) + ".wav");
			}
			for (int i = 0; i < 5; i++) {
				SE->Add((int)SoundEnum::Cocking3_0 + i, 3, "data/Sound/SE/Weapon/auto1911/" + std::to_string(i) + ".wav");
			}


			SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add((int)SoundEnum::SlideFoot, 9, "data/Sound/SE/move/sliding.wav");
			SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
			SE->Add((int)SoundEnum::Heart, Chara_num * 2, "data/Sound/SE/move/heart.wav");
			SE->Add((int)SoundEnum::Switch, Chara_num, "data/Sound/SE/move/standup.wav");


			SE->Get((int)SoundEnum::Trigger).SetVol_Local(48);
			for (int i = 0; i < 4; i++) {
				SE->Get((int)SoundEnum::Cocking1_0 + i).SetVol_Local(128);
			}
			for (int i = 0; i < 2; i++) {
				SE->Get((int)SoundEnum::Cocking2_0 + i).SetVol_Local(255);
			}
			SE->Get((int)SoundEnum::Shot2).SetVol_Local(216);
			SE->Get((int)SoundEnum::Unload2).SetVol_Local(255);
			SE->Get((int)SoundEnum::Load2).SetVol_Local(255);
			for (int i = 0; i < 2; i++) {
				SE->Get((int)SoundEnum::Cocking3_0 + i).SetVol_Local(255);
			}
			SE->Get((int)SoundEnum::Shot3).SetVol_Local(216);
			SE->Get((int)SoundEnum::Unload3).SetVol_Local(255);
			SE->Get((int)SoundEnum::Load3).SetVol_Local(255);



			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
			SE->Get((int)SoundEnum::Switch).SetVol_Local(255);

			//入力
			this->m_FPSActive.Set(false);
			this->m_MouseActive.Set(true);

			this->m_DamageEvents.clear();

			m_NetWorkBrowser.Init();

			Timer = 3.f;
		}
	};
};
