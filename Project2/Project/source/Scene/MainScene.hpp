#pragma once
#include	"../Header.hpp"
#include "../MainScene/Input.hpp"

#include "../MainScene/BackGround/BackGround.hpp"

#include "../MainScene/NetWorks.hpp"

#include "../MainScene/Object/Vehicle.hpp"
#include "../MainScene/Object/HindD.hpp"
#include "../MainScene/Object/Item.hpp"
#include "../MainScene/Object/Ammo.hpp"

#include "../MainScene/Object/ItemData.hpp"
#include "../MainScene/Player/CPU.hpp"

#include "../MainScene/UI/MainSceneUIControl.hpp"
#include "../MainScene/Inventory.hpp"

#include "../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public EffectControl {
		private:
			static const int		Chara_num = 0;
			static const int		Vehicle_num = Player_num;
			static const int		gun_num = Chara_num;
		private:
			//リソース関連
			std::shared_ptr<BackGroundClass>			m_BackGround;					//BG
			MV1											m_hit_pic;						//弾痕  
			//いちいち探査しないよう別持ち
			std::vector<std::shared_ptr<VehicleClass>>	m_vehicle_Pool;					//ポインター別持ち
			std::vector<std::shared_ptr<ItemData>>		m_ItemData;						//アイテムデータ
			std::shared_ptr<VehDataControl>				m_VehDataControl;				//戦車データ
			std::vector<std::shared_ptr<AIControl>>		m_AICtrl;						//AI
			//操作関連
			Matrix4x4DX									m_FreeLookMat;
			float										m_Concussion{ 0.f };
			float										m_ConcussionPer{ 0.f };
			float										m_XScope{ 0.f };
			float										m_YScope{ 0.f };
			Vector3DX									m_VehRadAdd;
			bool										m_IsChangeView{ false };
			float										m_ChangeViewPer{ 0.f };
			//UI関連
			UIClass										m_UIclass;
			float										m_HPBuf{ 0.f };
			float										m_ScoreBuf{ 0.f };
			GraphHandle									m_hit_Graph;
			GraphHandle									m_aim_Graph;
			GraphHandle									m_scope_Graph;
			bool										m_Reticle_on = false;
			float										m_Reticle_xpos = 0;
			float										m_Reticle_ypos = 0;
			NetWorkBrowser								m_NetWorkBrowser;
			InventoryClass								m_InventoryClass;
			//
			std::vector<DamageEvent>					m_DamageEvents;
			float										m_fov_base{ deg2rad(45.f) };
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			MAINLOOP(void) noexcept {
				this->m_BackGround = std::make_shared<BackGroundClass>();
				this->m_VehDataControl = std::make_shared<VehDataControl>();
				for (int i = 0; i < Player_num; i++) {
					this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
				}
			}
			~MAINLOOP(void) noexcept {}
		public:
			//Load
			void			Load_Sub(void) noexcept override {
				//BG
				this->m_BackGround->Load();
				//
				this->m_VehDataControl->Load();
				MV1::Load("data/model/hit/model.mv1", &this->m_hit_pic);

				this->m_hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				this->m_aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
				this->m_scope_Graph = GraphHandle::Load("data/UI/battle_scope.png");
			}
			void			Set_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto* SE = SoundPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto* OptionParts = OPTION::Instance();

				this->m_BackGround->Init();

				Vector3DX LightVec = Vector3DX::vget(-0.8f, -0.5f, -0.1f);
				DrawParts->SetAmbientLight(LightVec, GetColorF(0.92f, 0.91f, 0.90f, 1.0f));
				DrawParts->SetShadow(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 0);
				DrawParts->SetShadow(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 1);
				DrawParts->SetShadow(LightVec, Vector3DX::vget(-300.f, -10.f, -300.f)*Scale_Rate, Vector3DX::vget(300.f, 50.f, 300.f)*Scale_Rate, 2);

				for (int i = 0; i < Vehicle_num; i++) {
					this->m_vehicle_Pool.emplace_back(std::make_shared<VehicleClass>());
					ObjMngr->AddObject(this->m_vehicle_Pool.back());
					this->m_vehicle_Pool.back()->SetMapCol(this->m_BackGround);
					this->m_vehicle_Pool.back()->ObjectBaseClass::Init();

				}
				{
					auto Hind = std::make_shared<HindDClass>();
					ObjMngr->AddObject(Hind);
					ObjMngr->LoadModel(Hind, Hind, "data/model/hindD/");

					Hind->Init();

					Hind->SetMove(Matrix4x4DX::identity(), Vector3DX::vget(0.f, 30.f*Scale_Rate, 0.f));
					Hind->GetObj().get_anime(0).per = 1.f;
				}
				{
					std::vector<WIN32_FIND_DATA> data_t;
					std::string Path = "data/item/";
					GetFileNamesInDirectory("data/item/*", &data_t);
					for (auto& d : data_t) {
						if (d.cFileName[0] != '.') {
							m_ItemData.emplace_back(std::make_shared<ItemData>());
							m_ItemData.back()->Set(Path + d.cFileName + "/");
						}
					}
					data_t.clear();
				}
				{
					std::vector<WIN32_FIND_DATA> data_t;
					std::string Path = "data/ammo/";
					GetFileNamesInDirectory("data/ammo/*", &data_t);
					for (auto& d : data_t) {
						if (d.cFileName[0] != '.') {
							m_ItemData.emplace_back(std::make_shared<AmmoData>());
							m_ItemData.back()->Set(Path + d.cFileName + "/");
						}
					}
					data_t.clear();
				}
				this->m_VehDataControl->Set(m_ItemData);
				//UI
				this->m_UIclass.Set();
				//Set
				//戦車
				{
					Vector3DX BasePos;

					std::vector<int> OtherSelect;
					for (auto& v : this->m_vehicle_Pool) {
						size_t index = &v - &this->m_vehicle_Pool.front();
						int ID = 0;
						while (true) {
							ID = GetRand(this->m_BackGround->GetRoadPointNum() - 1);
							bool Hit = (std::find_if(OtherSelect.begin(), OtherSelect.end(), [&](int tmp) { return tmp == ID; }) != OtherSelect.end());

							if (index != 0) {
								auto LEN = (BasePos - this->m_BackGround->GetRoadPoint(ID)->pos()); LEN.y = (0.f);
								if (LEN.magnitude() <= 100.f*Scale_Rate) {
									Hit = true;
								}
							}
							if (!Hit) {
								auto Mat = *this->m_BackGround->GetRoadPoint(ID);
								Vector3DX pos_t = Mat.pos();
								if (
									(-280.f*Scale_Rate / 2.f < pos_t.x && pos_t.x < 290.f*Scale_Rate / 2.f) &&
									(-280.f*Scale_Rate / 2.f < pos_t.z && pos_t.z < 280.f*Scale_Rate / 2.f)
									) {
									OtherSelect.emplace_back(ID);
									break;
								}
							}
						}

						auto Mat = *this->m_BackGround->GetRoadPoint(ID);
						Vector3DX pos_t = Mat.pos();
						float rad_t = std::atan2f(Mat.zvec().x, -Mat.zvec().z);
						auto pos_t1 = pos_t + Vector3DX::up() * 1250.f;
						auto pos_t2 = pos_t + Vector3DX::up() * -1250.f;
						if (this->m_BackGround->CheckLinetoMap(pos_t1, &pos_t2, true, false)) {
							pos_t = pos_t2;
						}
						if (index == 0) {
							BasePos = pos_t;
						}

						auto& vehc_datas = this->m_VehDataControl->GetVehData();
						auto* vehc_data = &vehc_datas[
							GetRand((int)vehc_datas.size() - 1)
								//index != 0 ? GetRand((int)vehc_datas.size() - 1) : 0
						];

						ObjMngr->LoadModel(v, nullptr, ("data/tank/" + vehc_data->GetName() + "/").c_str());

						v->ValueInit(vehc_data, this->m_hit_pic, this->m_BackGround->GetBox2Dworld(), (PlayerID)index);
						v->ValueSet(deg2rad(0), rad_t, pos_t);
					}
				}
				//player
				PlayerMngr->Init(Player_num);
				for (int i = 0; i < Player_num; i++) {
					//PlayerMngr->GetPlayer(i).SetVehicle(nullptr);
					PlayerMngr->GetPlayer(i).SetVehicle((std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj((int)ObjType::Vehicle, i)));
					auto& Vehicle = PlayerMngr->GetPlayer(i).GetVehicle();
					for (int loop = 0; loop < 5; loop++) {
						PlayerMngr->GetPlayer(i).SetInventory(loop, Vehicle->GetInventoryXSize(loop), Vehicle->GetInventoryYSize(loop));
					}
					{
						if (Vehicle->Get_Gunsize() >= 2) {
							PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[0].GetAmmoSpec(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0) / 2, PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
							PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[1].GetAmmoSpec(0), PlayerMngr->GetPlayer(i).GetInventoryXSize(0) / 2, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0), PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
						}
						else {
							PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[0].GetAmmoSpec(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0), PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
						}
						PlayerMngr->GetPlayer(i).FillInventory(1, Vehicle->GetGun()[0].GetAmmoSpec(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(1), 6);
					}
					{
						PlayerMngr->GetPlayer(i).FillInventory(2, Vehicle->GetTrackPtr(), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(2), PlayerMngr->GetPlayer(i).GetInventoryYSize(2));
						PlayerMngr->GetPlayer(i).FillInventory(3, Vehicle->GetTrackPtr(), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(3), PlayerMngr->GetPlayer(i).GetInventoryYSize(3));
					}
					{
						auto Select = std::find_if(m_ItemData.begin(), m_ItemData.end(), [&](const std::shared_ptr<ItemData>& d) {return (d->GetPath().find("DieselMiniTank") != std::string::npos); });
						if (Select != m_ItemData.end()) {
							for (int x = 0; x < 5; x++) {
								PlayerMngr->GetPlayer(i).PutInventory(4, x * 2, 0, *Select, -1, false);
							}
						}
					}
					this->m_AICtrl[i]->Init(&this->m_vehicle_Pool, this->m_BackGround, PlayerMngr->GetPlayer(i).GetVehicle());
				}
				this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetVehicle()->GetHP();
				this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();
				//Cam
				DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamInt(EnumSaveParam::fov)), 1.f, 100.f);
				DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 15, -20), Vector3DX::vget(0, 15, 0), Vector3DX::vget(0, 1, 0));
				//サウンド
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
				SE->Add((int)SoundEnum::Tank_move, Vehicle_num, "data/Sound/SE/Engine/move.wav");
				SE->Add((int)SoundEnum::Tank_engine, Vehicle_num, "data/Sound/SE/Engine/engine.wav");
				SE->Add((int)SoundEnum::Tank_Start, Vehicle_num, "data/Sound/SE/Engine/start.wav");
				for (int i = 0; i < 7; i++) {
					SE->Add((int)SoundEnum::Tank_Eject, 3, "data/Sound/SE/gun/reload/eject/" + std::to_string(i) + ".wav", false);
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Tank_Reload, 3, "data/Sound/SE/gun/reload/hand/" + std::to_string(i) + ".wav", false);
				}
				SE->Add((int)SoundEnum::Tank_near, 10, "data/Sound/SE/near.wav");

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
				//
				this->m_DamageEvents.clear();
				this->m_NetWorkBrowser.Init();
				this->m_InventoryClass.Init();
			}
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();

				this->m_NetWorkBrowser.Dispose();
				this->m_InventoryClass.Dispose();
				for (auto& v : this->m_vehicle_Pool) {
					v.reset();
				}
				this->m_vehicle_Pool.clear();
				EffectControl::Dispose();
				PlayerMngr->Dispose();
				ObjMngr->DeleteAll();
				this->m_BackGround->Dispose();
				this->m_VehDataControl->Dispose();
			}
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			void			BG_Draw_Sub(void) noexcept override {
				this->m_BackGround->BG_Draw();
			}
			void			ShadowDraw_Far_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_Far();
			}

			void			ShadowDraw_NearFar_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_NearFar();
			}
			void			ShadowDraw_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw();
				ObjectManager::Instance()->Draw_Shadow();
			}
			void			MainDraw_Sub(void) noexcept override {
				auto* PlayerMngr = PlayerManager::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
				SetFogStartEnd(DrawParts->GetMainCamera().GetCamNear(), DrawParts->GetMainCamera().GetCamFar()*2.f);
				this->m_BackGround->Draw();
				ObjectManager::Instance()->Draw();
				//ObjectManager::Instance()->Draw_Depth();
				//シェーダー描画用パラメーターセット
				auto EndPos = Vehicle->GetGunMuzzlePos(0) + Vehicle->GetGunMuzzleVec(0) * Vehicle->GetAimingDistance();
				Vector3DX LensPos = ConvWorldPosToScreenPos(EndPos.get());
				if (0.f < LensPos.z && LensPos.z < 1.f) {
					this->m_Reticle_xpos = LensPos.x;
					this->m_Reticle_ypos = LensPos.y;
					this->m_Reticle_on = true;
					//if (!this->m_InputClass->GetMouseActive().on()) {
					//	this->m_Reticle_on = false;
					//}
				}
				for (int i = 0; i < Player_num; i++) {
					m_AICtrl[i]->Draw();
				}

			}
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj((int)ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
							if (GetMyPlayerID() == a->GetMyPlayerID()) {
								a->Draw_Hit_UI(this->m_hit_Graph);
							}
						}
						else {
							break;
						}
						loop++;
					}
				}
			}
			void			DrawUI_In_Sub(void) noexcept override {
				auto* PlayerMngr = PlayerManager::Instance();
				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
				auto* Pad = PadControl::Instance();
				//UI
				if (Vehicle->is_ADS()) {
					auto RadAdd = Vehicle->GetRadAdd() - m_VehRadAdd;
					{
						auto Base = Pad->GetLS_X()*1.f + RadAdd.z*1000.f;
						Easing(&this->m_XScope, Base, 0.95f, EasingType::OutExpo);
					}
					{
						auto Base = Pad->GetLS_Y()*1.f + RadAdd.x*1000.f;
						Easing(&this->m_YScope, -Base, 0.9f, EasingType::OutExpo);
					}
					int xpos = (int)(this->m_XScope);
					int ypos = (int)(this->m_YScope);
					int xsize = DrawParts->m_DispXSize;
					int ysize = DrawParts->m_DispYSize;
					DrawBox(0, 0, xpos, ypos + ysize, Black, TRUE);
					DrawBox(xpos, 0, DrawParts->m_DispXSize, ypos, Black, TRUE);
					DrawBox(xpos + xsize, ypos, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
					DrawBox(0, ypos + ysize, xpos + xsize, DrawParts->m_DispYSize, Black, TRUE);
					this->m_scope_Graph.DrawExtendGraph(xpos, ypos, xpos + xsize, ypos + ysize, true);
				}
				{
					if (m_IsChangeView != Vehicle->is_ADS()) {
						this->m_ChangeViewPer = 1.f;
					}
					else {
						Easing(&this->m_ChangeViewPer, 0.f, 0.95f, EasingType::OutExpo);
					}
					if ((this->m_ChangeViewPer*255.f) > 1.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*this->m_ChangeViewPer), 0, 255));
						DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
				m_VehRadAdd = Vehicle->GetRadAdd();
				m_IsChangeView = Vehicle->is_ADS();

				if (Vehicle->Get_alive()) {
					this->m_UIclass.Draw();
					Vehicle->DrawModuleView(y_r(50 + 100), DrawParts->m_DispYSize - y_r(100 + 150), y_r(200));
				}
				//通信設定
				//if (!this->m_MouseActive.on()) {
				//	this->m_NetWorkBrowser.Draw();
				//}
				//Pause
				this->m_InventoryClass.Draw(Vehicle->GetMove().pos);
				if (Vehicle->Get_alive()) {
					if (this->m_Reticle_on) {
						this->m_aim_Graph.DrawRotaGraph((int)this->m_Reticle_xpos, (int)this->m_Reticle_ypos, (float)(y_r(100)) / 100.f, 0.f, true);

						unsigned int color = GetColor(0, 255, 0);
						auto Time = Vehicle->GetTotalloadtime(0);
						if (Vehicle->Gunloadtime(0) != 0.f) {
							color = GetColor(255, 0, 0);
							Time = Vehicle->Gunloadtime(0);
						}
						Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, (int)this->m_Reticle_xpos + y_r(50), (int)this->m_Reticle_ypos, color, GetColor(0, 0, 0), "%05.2f s", Time);
					}
				}
			}
		};
	};
};
