#pragma once
#include	"../../Header.hpp"
#include "../../MainScene/Object/ItemData.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//戦車データ集
		class VehDataControl : public SingletonBase<VehDataControl> {
		private:
			friend class SingletonBase<VehDataControl>;
		private:
		public:
			//戦車砲データ
			class GunData {
			private:
				std::string							m_name;
				float								m_loadTime{0.f};
				int									m_Shot_Sound = -1;
				int									m_Eject_Sound = -1;
				int									m_Reload_Sound = -1;
				float								m_UpRadLimit{0.f};
				float								m_DownRadLimit{0.f};
				std::vector<std::shared_ptr<AmmoData>>m_AmmoSpec;
				std::array<frames, 3>				m_frame;
			public:
				const auto&		GetName(void) const noexcept { return this->m_name; }
				const auto&		GetLoadTime(void) const noexcept { return this->m_loadTime; }
				const auto&		GetShotSound(void) const noexcept { return this->m_Shot_Sound; }
				const auto&		GetEjectSound(void) const noexcept { return this->m_Eject_Sound; }
				const auto&		GetReloadSound(void) const noexcept { return this->m_Reload_Sound; }
				const auto&		GetAmmoSpec(void) const noexcept { return this->m_AmmoSpec; }
				const auto&		Get_frame(size_t pID)const noexcept { return this->m_frame[pID]; }
				const auto&		GetUpRadLimit(void) const noexcept { return this->m_UpRadLimit; }
				const auto&		GetDownRadLimit(void) const noexcept { return this->m_DownRadLimit; }
			public: //コンストラクタ、デストラクタ
				GunData(void) noexcept {}
				~GunData(void) noexcept {}
			public:
				void			SetFrame(const MV1& obj, int i) noexcept {
					this->m_frame[0].Set(i, obj);
					if (obj.frame_child_num(this->m_frame[0].GetFrameID()) <= 0) {
						return;
					}
					int child_num = (int)obj.frame_child(this->m_frame[0].GetFrameID(), 0);

					if (obj.frame_name(child_num).find("仰角", 0) != std::string::npos) {
						this->m_frame[1].Set(child_num, obj);
					}
					if (this->m_frame[1].GetFrameID() == -1) {
						return;
					}

					if (obj.frame_child_num(this->m_frame[1].GetFrameID()) <= 0) {
						return;
					}
					int child2_num = (int)obj.frame_child(this->m_frame[1].GetFrameID(), 0);
					{
						this->m_frame[2].Set(child2_num, obj);
					}
				}
				void			Set(int mdata) noexcept {
					this->m_name = getparams::_str(mdata);
					this->m_loadTime = getparams::_float(mdata);
					this->m_Shot_Sound = getparams::_int(mdata);//サウンド
					this->m_Eject_Sound = getparams::_int(mdata);//サウンド
					this->m_Reload_Sound = getparams::_int(mdata);//サウンド
					this->m_UpRadLimit = getparams::_float(mdata);
					this->m_DownRadLimit = getparams::_float(mdata);
					while (true) {
						auto stp = getparams::Getstr(mdata);
						if (stp.find("useammo" + std::to_string(this->m_AmmoSpec.size())) == std::string::npos) {
							break;
						}
						auto RIGHT = getparams::getright(stp);
						auto& iData = ItemDataControl::Instance()->GetData();
						for (const auto& d : iData) {
							if (d->GetPath().find(RIGHT) != std::string::npos) {
								this->m_AmmoSpec.emplace_back((const std::shared_ptr<AmmoData>&)d);
								break;
							}
						}
					}
				}
			};
			//戦車データ
			class VhehicleData {
				typedef std::pair<std::shared_ptr<GraphHandle>, int> ViewAndModule;
			private:
				std::string							m_name;							//名称
				HitPoint							m_MaxHP{ 0 };					//HP
				int									m_MaxFuel{ 0 };					//燃料
				bool								m_IsFloat{ false };				//浮くかどうか
				float								m_DownInWater{ 0.f };			//沈む判定箇所
				float								m_MaxFrontSpeed{ 0.f };			//前進速度(km/h)
				float								m_MaxBackSpeed{ 0.f };			//後退速度(km/h)
				float								m_MaxBodyRad{ 0.f };			//旋回速度(度/秒)
				float								m_MaxTurretRad{ 0.f };			//砲塔駆動速度(度/秒)
				std::shared_ptr<ItemData>			m_TrackPtr{ nullptr };
				std::vector<GunData>				m_GunFrameData;					//
				b2PolygonShape						m_DynamicBox;					//
				MV1									m_DataObj;						//
				MV1									m_DataCol;						//
				std::vector<frames>					m_wheelframe;					//転輪
				std::vector<frames>					m_wheelframe_nospring;			//誘導輪回転
				std::vector<std::pair<int, float>>	m_armer_mesh;					//装甲ID
				std::vector<int>					m_space_mesh;					//装甲ID
				std::vector<int>					m_module_mesh;					//装甲ID
				std::array<int, 4>					m_square{ 0 };					//車輛の四辺
				std::array<std::vector<frames>, 2>	m_b2upsideframe;				//履帯上
				std::array<std::vector<frames>, 2>	m_downsideframe;				//履帯
				std::array<std::vector<ViewAndModule>, 2>	m_TankViewPic;				//モジュール表示
				std::vector<std::pair<int, int>>	m_Inventory;
			private:
				const auto		GetSide(bool isLeft, bool isFront) const noexcept {
					int ans = 0;
					float tmp = 0.f;
					for (auto& f : this->m_wheelframe) {
						if ((isLeft ? 1.f : -1.f) * f.GetFrameWorldPosition().pos().x >= 0) {
							ans = f.GetFrameID();
							tmp = f.GetFrameWorldPosition().pos().z;
							break;
						}
					}
					for (auto& f : this->m_wheelframe) {
						if (ans != f.GetFrameID()) {
							if ((isLeft ? 1.f : -1.f) * f.GetFrameWorldPosition().pos().x >= 0) {
								if (isFront) {
									if (tmp > f.GetFrameWorldPosition().pos().z) {
										ans = f.GetFrameID();
										tmp = f.GetFrameWorldPosition().pos().z;
									}
								}
								else {
									if (tmp < f.GetFrameWorldPosition().pos().z) {
										ans = f.GetFrameID();
										tmp = f.GetFrameWorldPosition().pos().z;
									}
								}
							}
						}
					}
					return ans;
				}
			public:			//getter
				const auto&		GetName(void) const noexcept { return this->m_name; }
				const auto&		GetMaxHP(void) const noexcept { return this->m_MaxHP; }
				const auto&		GetMaxFuel(void) const noexcept { return this->m_MaxFuel; }
				const auto&		GetIsFloat(void) const noexcept { return this->m_IsFloat; }
				const auto&		GetDownInWater(void) const noexcept { return this->m_DownInWater; }
				const auto&		GetMaxFrontSpeed(void) const noexcept { return this->m_MaxFrontSpeed; }
				const auto&		GetMaxBackSpeed(void) const noexcept { return this->m_MaxBackSpeed; }
				const auto&		GetMaxBodyRad(void) const noexcept { return this->m_MaxBodyRad; }
				const auto&		GetMaxTurretRad(void) const noexcept { return this->m_MaxTurretRad; }
				const auto&		GetTrackPtr(void) const noexcept { return this->m_TrackPtr; }
				const auto&		Get_gunframe(void) const noexcept { return this->m_GunFrameData; }
				const auto&		GetDynamicBox(void) const noexcept { return this->m_DynamicBox; }
				const auto&		Get_wheelframe(void) const noexcept { return this->m_wheelframe; }
				const auto&		Get_wheelframe_nospring(void) const noexcept { return this->m_wheelframe_nospring; }
				const auto&		Get_armer_mesh(void) const noexcept { return this->m_armer_mesh; }
				const auto&		Get_space_mesh(void) const noexcept { return this->m_space_mesh; }
				const auto&		Get_module_mesh(void) const noexcept { return this->m_module_mesh; }
				const auto&		Get_module_view(void) const noexcept { return this->m_TankViewPic; }
				const auto&		Get_square(size_t ID_t)const noexcept { return this->m_square[ID_t]; }
				const auto&		Get_square(void) const noexcept { return this->m_square; }
				const auto&		Get_b2upsideframe(size_t ID_t)const noexcept { return this->m_b2upsideframe[ID_t]; }
				const auto&		Get_b2upsideframe(void) const noexcept { return this->m_b2upsideframe; }
				const auto&		Get_b2downsideframe(void) const noexcept { return this->m_downsideframe; }
				const auto		GetInventoryXSize(int ID) const noexcept { return this->m_Inventory[ID].first; }
				const auto		GetInventoryYSize(int ID) const noexcept { return this->m_Inventory[ID].second; }
			public: //コンストラクタ、デストラクタ
				VhehicleData(void) noexcept { }
				VhehicleData(const VhehicleData& o) noexcept {
					this->m_name = o.m_name;
					this->m_MaxHP = o.m_MaxHP;
					this->m_MaxFuel = o.m_MaxFuel;
					this->m_IsFloat = o.m_IsFloat;
					this->m_DownInWater = o.m_DownInWater;
					this->m_MaxFrontSpeed = o.m_MaxFrontSpeed;
					this->m_MaxBackSpeed = o.m_MaxBackSpeed;
					this->m_MaxBodyRad = o.m_MaxBodyRad;
					this->m_MaxTurretRad = o.m_MaxTurretRad;
					this->m_GunFrameData = o.m_GunFrameData;
					this->m_DynamicBox = o.m_DynamicBox;
					this->m_wheelframe = o.m_wheelframe;
					this->m_wheelframe_nospring = o.m_wheelframe_nospring;
					this->m_armer_mesh = o.m_armer_mesh;
					this->m_space_mesh = o.m_space_mesh;
					this->m_module_mesh = o.m_module_mesh;
					this->m_square = o.m_square;
					this->m_b2upsideframe = o.m_b2upsideframe;
					this->m_downsideframe = o.m_downsideframe;
					this->m_TankViewPic = o.m_TankViewPic;
				}
				~VhehicleData(void) noexcept { }
			public:
				//事前読み込み
				void		Set_Pre(const char* name) noexcept {
					this->m_name = name;
					MV1::Load("data/tank/" + this->m_name + "/model_DISABLE.mv1", &this->m_DataObj);//model.pmx//model_DISABLE.mv1
					MV1::Load("data/tank/" + this->m_name + "/col.mv1", &this->m_DataCol);
				}
				//メイン読み込み
				void			Set() noexcept {
					//固有
					this->m_DownInWater = 0.f;
					for (int i = 0; i < this->m_DataObj.mesh_num(); i++) {
						auto p = this->m_DataObj.mesh_maxpos(i).y;
						if (this->m_DownInWater < p) {
							this->m_DownInWater = p;
						}
					}
					this->m_DownInWater /= 2.f;
					Vector3DX minpos, maxpos;							//
					for (int i = 0; i < this->m_DataObj.frame_num(); i++) {
						std::string p = this->m_DataObj.frame_name(i);
						if (p.find("転輪", 0) != std::string::npos) {
							this->m_wheelframe.resize(this->m_wheelframe.size() + 1);
							this->m_wheelframe.back().Set(i, this->m_DataObj);
						}
						else if ((p.find("輪", 0) != std::string::npos) && (p.find("転輪", 0) == std::string::npos)) {
							this->m_wheelframe_nospring.resize(this->m_wheelframe_nospring.size() + 1);
							this->m_wheelframe_nospring.back().Set(i, this->m_DataObj);
						}
						else if (p.find("旋回", 0) != std::string::npos) {
							this->m_GunFrameData.resize(this->m_GunFrameData.size() + 1);
							this->m_GunFrameData.back().SetFrame(this->m_DataObj, i);
						}
						else if (p == "min") {
							minpos = this->m_DataObj.frame(i);
						}
						else if (p == "max") {
							maxpos = this->m_DataObj.frame(i);
						}
						else if (p.find("２D物理", 0) != std::string::npos || p.find("2D物理", 0) != std::string::npos) { //2D物理
							this->m_b2upsideframe[0].clear();
							this->m_b2upsideframe[1].clear();
							for (int z = 0; z < this->m_DataObj.frame_child_num(i); z++) {
								if (this->m_DataObj.frame(i + 1 + z).x > 0) {
									this->m_b2upsideframe[0].resize(this->m_b2upsideframe[0].size() + 1);
									this->m_b2upsideframe[0].back().Set(i + 1 + z, this->m_DataObj);
								}
								else {
									this->m_b2upsideframe[1].resize(this->m_b2upsideframe[1].size() + 1);
									this->m_b2upsideframe[1].back().Set(i + 1 + z, this->m_DataObj);
								}
							}
							std::sort(this->m_b2upsideframe[0].begin(), this->m_b2upsideframe[0].end(), [](const frames& x, const frames& y) { return x.GetFrameWorldPosition().pos().z < y.GetFrameWorldPosition().pos().z; }); //ソート
							std::sort(this->m_b2upsideframe[1].begin(), this->m_b2upsideframe[1].end(), [](const frames& x, const frames& y) { return x.GetFrameWorldPosition().pos().z < y.GetFrameWorldPosition().pos().z; }); //ソート
						}
						else if (p.find("履帯設置部", 0) != std::string::npos) { //2D物理
							this->m_downsideframe[0].clear();
							this->m_downsideframe[1].clear();
							for (int z = 0; z < this->m_DataObj.frame_child_num(i); z++) {
								if (this->m_DataObj.frame(i + 1 + z).x > 0) {
									this->m_downsideframe[0].resize(this->m_downsideframe[0].size() + 1);
									this->m_downsideframe[0].back().Set(i + 1 + z, this->m_DataObj);
								}
								else {
									this->m_downsideframe[1].resize(this->m_downsideframe[1].size() + 1);
									this->m_downsideframe[1].back().Set(i + 1 + z, this->m_DataObj);
								}
							}
						}
					}
					//
					Vector3DX minmaxsub = maxpos - minpos;
					Vector3DX minmaxadd = minpos + maxpos;
					this->m_DynamicBox.SetAsBox(minmaxsub.x / 2, minmaxsub.z / 2, b2Vec2(minmaxadd.x / 2, -minmaxadd.z / 2), 0.f);	//ダイナミックボディに別のボックスシェイプを定義
					//4隅確定
					this->m_square[0] = GetSide(true, false);			//2		左後部0
					this->m_square[1] = GetSide(true, true);			//10	左前部1
					this->m_square[2] = GetSide(false, false);			//3		右後部2
					this->m_square[3] = GetSide(false, true);			//11	右前部3
					//装甲

					//車体モジュール
					this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_body.png")), -1));
					int human_body_num = 0;
					int ammo_body_num = 0;
					for (int i = 0; i < this->m_DataCol.mesh_num(); i++) {
						std::string p = this->m_DataCol.material_name(i);
						if (p.find("armer", 0) != std::string::npos) {
							this->m_armer_mesh.emplace_back(i, std::stof(getparams::getright(p.c_str())));//装甲
						}
						else if (p.find("space", 0) != std::string::npos) {
							this->m_space_mesh.emplace_back(i);//空間装甲
						}
						else if (p.find("left_foot", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//モジュール
							this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_track_l.bmp")), (int)this->m_module_mesh.size() - 1));
						}
						else if (p.find("right_foot", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//モジュール
							this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_track_r.bmp")), (int)this->m_module_mesh.size() - 1));
						}
						else if (p.find("engine", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//モジュール
							this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_engine_body.bmp")), (int)this->m_module_mesh.size() - 1));
						}
						else if (p.find("human_body", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//モジュール
							this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_human_body_" + std::to_string(human_body_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
							human_body_num++;
						}
						else if (p.find("ammo_body", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//モジュール
							this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_ammo_body_" + std::to_string(ammo_body_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
							ammo_body_num++;
						}
					}
					this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_look_body.bmp")), -1));
					//砲塔モジュール
					this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_turret.png")), -1));
					int human_turret_num = 0;
					int ammo_turret_num = 0;
					for (int i = 0; i < this->m_DataCol.mesh_num(); i++) {
						std::string p = this->m_DataCol.material_name(i);
						if (p.find("gun", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//モジュール
							this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_gun.bmp")), (int)this->m_module_mesh.size() - 1));
						}
						else if (p.find("human_turret", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//モジュール
							this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_human_turret_" + std::to_string(human_turret_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
							human_turret_num++;
						}
						else if (p.find("ammo_turret", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//モジュール
							this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_ammo_turret_" + std::to_string(ammo_turret_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
							ammo_turret_num++;
						}
					}
					this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_look_turret.bmp")), -1));
					//data
					{
						int mdata = FileRead_open(("data/tank/" + this->m_name + "/data.txt").c_str(), FALSE);
						this->m_MaxHP = (HitPoint)getparams::_int(mdata);
						this->m_MaxFuel = getparams::_int(mdata);
						this->m_IsFloat = getparams::_bool(mdata);
						this->m_MaxFrontSpeed = getparams::_float(mdata);
						this->m_MaxBackSpeed = getparams::_float(mdata);
						this->m_MaxBodyRad = getparams::_float(mdata);
						this->m_MaxTurretRad = deg2rad(getparams::_float(mdata));
						{
							auto RIGHT = getparams::_str(mdata);
							auto& iData = ItemDataControl::Instance()->GetData();
							for (const auto& d : iData) {
								if (d->GetPath().find(RIGHT) != std::string::npos) {
									m_TrackPtr = d;
									break;
								}
							}
						}
						//インベントリ
						{
							this->m_Inventory.resize(5);
							for (auto& I : this->m_Inventory) {
								I.first = getparams::_int(mdata);
								I.second = getparams::_int(mdata);
							}
						}
						{
							for (auto& g : this->m_GunFrameData) {
								g.Set(mdata);
							}
						}
						FileRead_close(mdata);
					}
					this->m_DataObj.Dispose();
					this->m_DataCol.Dispose();
				}
			};
		private:
			std::vector<VhehicleData>	vehc_data;
		public:
			const auto&		GetVehData(void) const noexcept { return this->vehc_data; }
		public:
			void	Load() noexcept {
				std::vector<WIN32_FIND_DATA> data_t;
				GetFileNamesInDirectory("data/tank/*", &data_t);
				std::vector<std::string> DirNames;
				for (auto& d : data_t) {
					if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						DirNames.emplace_back(d.cFileName);
					}
				}
				data_t.clear();
				this->vehc_data.resize(DirNames.size());
				for (auto& d : DirNames) {
					this->vehc_data[&d - &DirNames.front()].Set_Pre(d.c_str());
				}
				DirNames.clear();
			}
			void	Set() noexcept {
				for (auto& t : this->vehc_data) {
					t.Set();
				}
			}
			void	Dispose() noexcept {
				this->vehc_data.clear();
			}
		};
	};
};
