#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		static auto* CreateB2Body(std::shared_ptr<b2World>& world, b2BodyType type, float32 x_, float32 y_, float angle = 0) noexcept {
			b2BodyDef f_bodyDef;
			f_bodyDef.type = type;
			f_bodyDef.position.Set(x_, y_);
			f_bodyDef.angle = angle;
			return world->CreateBody(&f_bodyDef);
		}
		static const size_t max_bullet{ 64 };
		//静的弾痕
		HIT_PASSIVE hit_obj_p;
		//弾データ
		class AmmoData {
		private:
			std::string		m_name;
			std::string		m_path;
			float			m_caliber{ 0.f };
			float			m_speed{ 100.f };				//弾速
			float			m_penetration{ 10.f };			//貫通
			int				m_damage{ 10 };					//ダメージ
		public://getter
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		GetPath(void) const noexcept { return this->m_path; }
			const auto&		GetCaliber(void) const noexcept { return this->m_caliber; }
			const auto&		GetSpeed(void) const noexcept { return this->m_speed; }
			const auto&		GetPenetration(void) const noexcept { return this->m_penetration; }
			const auto&		GetDamage(void) const noexcept { return this->m_damage; }
		public:
			void			Set(std::string path_, std::string named) {
				this->m_name = named;
				this->m_path = path_ + named;
				int mdata = FileRead_open((this->m_path + "/data.txt").c_str(), FALSE);
				this->m_caliber = getparams::_float(mdata) * 0.001f;//口径
				this->m_speed = getparams::_float(mdata);	//弾速
				this->m_penetration = getparams::_float(mdata);	//貫通
				this->m_damage = getparams::_int(mdata);//ダメージ
				FileRead_close(mdata);
			}
		};
		class AmmoObj {
		private:
			bool			m_IsActive{ false };
			bool			m_IsDraw{ false };
			bool			m_IsHit{ false };
			float			m_HitTimer{ 0.f };
			int				m_RicochetCnt{ 0 };
			AmmoData*		m_AmmoData{ nullptr };
			moves			m_move;
			float			m_speed{ 0.f };
			float			m_penetration{ 0.f };
			const MV1*		m_MapCol{ nullptr };
			float			Hit_alpha{ 0.f };
			int				Hit_window_x{ 0 };
			int				Hit_window_y{ 0 };
		public://getter
			const auto&		IsActive(void) const noexcept { return this->m_IsActive; }
			const auto&		GetMove(void) const noexcept { return this->m_move; }
			const auto&		GetDamage(void) const noexcept { return this->m_AmmoData->GetDamage(); }
			const auto&		GetCaliberSize(void) const noexcept { return this->m_AmmoData->GetCaliber(); }
			const auto		GetEffectSize(void) const noexcept { return ((this->m_AmmoData->GetCaliber() >= 0.020f) ? this->m_AmmoData->GetCaliber() : 0.025f) / 0.1f; }
		public:
			void			Put(AmmoData* pAmmoData, const VECTOR_ref& pPos, const VECTOR_ref& pVec) noexcept {
				this->m_IsActive = true;
				this->m_IsDraw = true;
				this->m_RicochetCnt = 0;
				this->m_IsHit = false;
				this->m_HitTimer = 0.f;
				this->m_move.pos = pPos;
				this->m_move.repos = this->m_move.pos;
				this->m_move.vec = pVec;
				this->m_AmmoData = pAmmoData;
				this->m_speed = this->m_AmmoData->GetSpeed() * Scale_Rate;
				this->m_penetration = this->m_AmmoData->GetPenetration();
			}
			void			Init(const MV1* pMapPtr) noexcept {
				this->m_IsActive = false;
				this->m_IsDraw = false;
				this->m_MapCol = pMapPtr;
			}
			void			Execute(void) noexcept {
				if (this->m_IsHit) {
					this->m_IsHit = false;
					this->m_HitTimer = 0.25f;
				}
				this->m_HitTimer = std::clamp(this->m_HitTimer - 1.f / FPS, 0.f, 0.25f);
				if (this->Hit_alpha > 0.f) {
					Easing(&this->Hit_alpha, (this->m_HitTimer > 0.f) ? 2.f : 0.f, 0.95f, EasingType::OutExpo);
					if (this->Hit_alpha <= 0.01f) {
						this->Hit_alpha = 0;
					}
				}
				if (this->m_IsActive) {
					//移動確定
					this->m_move.SetPos(this->m_move.pos + (this->m_move.vec * (this->m_speed / FPS)));
					//消す(スピードが0以下、貫通が0以下、5回反射する)
					if (this->m_speed <= 0.f || this->m_penetration <= 0.f || this->m_RicochetCnt > 5) {
						this->m_IsActive = false;
					}
					//this->m_speed -= 5.f / FPS;
					//this->m_penetration -= 5.f / FPS;
				}
			}
			void			Draw(void) noexcept {
				if (this->m_IsHit) {
					auto p = ConvWorldPosToScreenPos(this->m_move.pos.get());
					if (p.z >= 0.f && p.z <= 1.f) {
						this->Hit_alpha = 1.f;
						this->Hit_window_x = int(p.x);
						this->Hit_window_y = int(p.y);
					}
				}
				if (this->m_IsDraw) {
					if (!this->m_IsActive) {
						this->m_IsDraw = false;
					}
					DrawCapsule_3D(this->m_move.pos, this->m_move.repos, ((this->m_AmmoData->GetCaliber() - 0.00762f) * 0.1f + 0.00762f)*Scale_Rate, GetColor(255, 255, 172), GetColor(255, 255, 255));
				}
			}
			void			Draw_Hit_UI(GraphHandle& Hit_Graph) noexcept {
				if (this->Hit_alpha >= 10.f / 255.f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f * this->Hit_alpha));
					Hit_Graph.DrawRotaGraph(this->Hit_window_x, this->Hit_window_y, (float)y_r(this->Hit_alpha * 0.5f * 100.0f) / 100.f, 0.f, true);
				}
			}
			void			Dispose(void) noexcept { this->m_MapCol = nullptr; }
		public:
			const auto		ColCheckGround(void) noexcept {
				MV1_COLL_RESULT_POLY ColResGround; ColResGround.HitFlag = FALSE;
				if (this->m_IsActive) {
					ColResGround = this->m_MapCol->CollCheck_Line(this->m_move.repos, this->m_move.pos);
					if (ColResGround.HitFlag == TRUE) {
						this->m_move.pos = ColResGround.HitPosition;
					}
				}
				return ColResGround;
			}
			const auto		PenetrationCheck(float pArmer, const VECTOR_ref& normal) const noexcept { return (this->m_penetration > (pArmer * (1.0f / std::abs(this->m_move.vec.Norm().dot(normal))))); }
			void			Penetrate(void) noexcept {
				this->m_IsActive = false;
				this->m_IsHit = true;
			}
			void			Ricochet(const VECTOR_ref& position, const VECTOR_ref& normal) noexcept {
				this->m_IsHit = true;
				//this->m_penetration *= 0.8f;
				this->m_RicochetCnt++;
				this->m_move.vec = (this->m_move.vec + normal * ((this->m_move.vec.dot(normal)) * -2.0f)).Norm();
				this->m_move.pos = this->m_move.vec * (0.1f) + position;
			}
			void			HitGround(void) noexcept {
				this->m_IsActive = false;
			}
		};
		//戦車砲データ
		class gun_frame {
			int						m_Ammo_Cap = 0;
			std::string				m_name;
			std::vector<AmmoData>	m_Ammo;
			float					m_load_time{ 0.f };
			std::array<frames, 3>	m_frame;
			int						m_Shot_Sound = -1;
			int						m_Reload_Sound = -1;
			int						m_Eject_Sound = -1;
		public:
			const auto&		Get_Ammo_Cap(void) const noexcept { return this->m_Ammo_Cap; }
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		Get_Spec(void) const noexcept { return this->m_Ammo; }
			const auto&		Get_load_time(void) const noexcept { return this->m_load_time; }
			const auto&		Get_frame(size_t ID_t)const noexcept { return this->m_frame[ID_t]; }
			const auto&		GetShotSound(void) const noexcept { return this->m_Shot_Sound; }
			const auto&		GetReloadSound(void) const noexcept { return this->m_Reload_Sound; }
			const auto&		GetEjectSound(void) const noexcept { return this->m_Eject_Sound; }
		public:
			void			SetFrame(const MV1& obj, int i) {
				this->m_frame[0].Set_World(i, obj);
				this->m_frame[2].first = -1;
				this->m_frame[1].first = -1;
				auto p2 = obj.frame_parent(this->m_frame[0].first);
				if (p2 >= 0) {
					this->m_frame[0].second -= obj.frame(int(p2)); //親がいる時引いとく
				}
				if (obj.frame_child_num(this->m_frame[0].first) <= 0) {
					return;
				}
				int child_num = (int)obj.frame_child(this->m_frame[0].first, 0);
				if (obj.frame_name(child_num).find("仰角", 0) != std::string::npos) {
					this->m_frame[1].Set_Local(child_num, obj);
				}
				if (this->m_frame[1].first == -1) {
					return;
				}
				if (obj.frame_child_num(this->m_frame[1].first) <= 0) {
					return;
				}
				{
					this->m_frame[2].Set_Local((int)obj.frame_child(this->m_frame[1].first, 0), obj);
				}
			}
			void			Set(int mdata, const std::string& stt) {
				this->m_name = getparams::getright(stt);
				this->m_load_time = getparams::_float(mdata);
				this->m_Ammo_Cap = getparams::_int(mdata);
				this->m_Shot_Sound = getparams::_int(mdata);//サウンド
				this->m_Eject_Sound = getparams::_int(mdata);//サウンド
				this->m_Reload_Sound = getparams::_int(mdata);//サウンド
				while (true) {
					auto stp = getparams::Getstr(mdata);
					if (stp.find("useammo" + std::to_string(this->m_Ammo.size())) == std::string::npos) {
						break;
					}
					this->m_Ammo.resize(this->m_Ammo.size() + 1);
					this->m_Ammo.back().Set("data/ammo/", getparams::getright(stp));
				}
			}
		};
		//戦車データ
		class VhehicleData {
			typedef std::pair<int, float> ArmerMeshInfo;
		private:
			std::string				m_name;							//名称
			int						m_MaxHP{ 0 };					//HP
			bool					m_IsFloat{ false };				//浮くかどうか
			float					m_DownInWater{ 0.f };			//沈む判定箇所
			float					m_MaxFrontSpeed{ 0.f };			//前進速度(km/h)
			float					m_MaxBackSpeed{ 0.f };			//後退速度(km/h)
			float					m_MaxBodyRad{ 0.f };			//旋回速度(度/秒)
			float					m_MaxTurretRad{ 0.f };			//砲塔駆動速度(度/秒)
			std::vector<gun_frame>	m_GunFrameData;					//
			b2PolygonShape			m_DynamicBox;
			MV1						m_DataObj;
			MV1						m_DataCol;						//
			std::vector<frames>		wheelframe;						//転輪
			std::vector<frames>		wheelframe_nospring;			//誘導輪回転
			std::vector<ArmerMeshInfo>	armer_mesh;					//装甲ID
			std::vector<int>		space_mesh;						//装甲ID
			std::vector<int>		module_mesh;					//装甲ID
			std::array<int, 4>		square{ 0 };					//車輛の四辺
			std::array<std::vector<frames>, 2>	b2upsideframe;		//履帯上
			std::array<std::vector<frames>, 2>	downsideframe;	//履帯
		private:
			const auto		GetSide(bool isLeft, bool isFront) const noexcept {
				int ans = 0;
				float tmp = 0.f;
				for (auto& f : this->wheelframe) {
					if ((isLeft ? 1.f : -1.f) * f.second.x() >= 0) {
						ans = f.first;
						tmp = f.second.z();
						break;
					}
				}
				for (auto& f : this->wheelframe) {
					if (ans != f.first) {
						if ((isLeft ? 1.f : -1.f) * f.second.x() >= 0) {
							if (isFront) {
								if (tmp > f.second.z()) {
									ans = f.first;
									tmp = f.second.z();
								}
							}
							else {
								if (tmp < f.second.z()) {
									ans = f.first;
									tmp = f.second.z();
								}
							}
						}
					}
				}
				return ans;
			}
		public:
			//getter
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		GetMaxHP(void) const noexcept { return this->m_MaxHP; }
			const auto&		GetIsFloat(void) const noexcept { return this->m_IsFloat; }
			const auto&		GetDownInWater(void) const noexcept { return this->m_DownInWater; }
			const auto&		GetMaxFrontSpeed(void) const noexcept { return this->m_MaxFrontSpeed; }
			const auto&		GetMaxBackSpeed(void) const noexcept { return this->m_MaxBackSpeed; }
			const auto&		GetMaxBodyRad(void) const noexcept { return this->m_MaxBodyRad; }
			const auto&		GetMaxTurretRad(void) const noexcept { return this->m_MaxTurretRad; }
			const auto&		Get_gunframe(void) const noexcept { return this->m_GunFrameData; }
			const auto&		GetDynamicBox(void) const noexcept { return this->m_DynamicBox; }
			const auto&		Get_wheelframe(void) const noexcept { return wheelframe; }
			const auto&		Get_wheelframe_nospring(void) const noexcept { return wheelframe_nospring; }
			const auto&		Get_armer_mesh(void) const noexcept { return armer_mesh; }
			const auto&		Get_space_mesh(void) const noexcept { return space_mesh; }
			const auto&		Get_module_mesh(void) const noexcept { return module_mesh; }
			const auto&		Get_square(size_t ID_t)const noexcept { return square[ID_t]; }
			const auto&		Get_square(void) const noexcept { return square; }
			const auto&		Get_b2upsideframe(size_t ID_t)const noexcept { return b2upsideframe[ID_t]; }
			const auto&		Get_b2upsideframe(void) const noexcept { return b2upsideframe; }
			const auto&		Get_b2downsideframe(void) const noexcept { return downsideframe; }
		public: //コンストラクタ、デストラクタ
			VhehicleData(void) noexcept { }
		public:
			//事前読み込み
			static void		Set_Pre(std::vector<VhehicleData>* veh_, const char* name) {
				auto data_t = GetFileNamesInDirectory(name);
				for (auto& d : data_t) {
					if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						veh_->resize(veh_->size() + 1);
						veh_->back().m_name = d.cFileName;
					}
				}
				for (auto& t : *veh_) {
					MV1::Load(std::string(name) + t.m_name + "/model_LOADCALC.mv1", &t.m_DataObj);
					MV1::Load(std::string(name) + t.m_name + "/col.mv1", &t.m_DataCol);
				}
			}
			//メイン読み込み
			void			Set(void) noexcept {
				//固有
				this->m_DownInWater = 0.f;
				for (int i = 0; i < this->m_DataObj.mesh_num(); i++) {
					auto p = this->m_DataObj.mesh_maxpos(i).y();
					if (this->m_DownInWater < p) {
						this->m_DownInWater = p;
					}
				}
				this->m_DownInWater /= 2.f;
				VECTOR_ref minpos, maxpos;							//
				for (int i = 0; i < this->m_DataObj.frame_num(); i++) {
					std::string p = this->m_DataObj.frame_name(i);
					if (p.find("転輪", 0) != std::string::npos) {
						this->wheelframe.resize(this->wheelframe.size() + 1);
						this->wheelframe.back().Set_World(i, this->m_DataObj);
					}
					else if ((p.find("輪", 0) != std::string::npos) && (p.find("転輪", 0) == std::string::npos)) {
						this->wheelframe_nospring.resize(this->wheelframe_nospring.size() + 1);
						this->wheelframe_nospring.back().Set_World(i, this->m_DataObj);
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
						this->b2upsideframe[0].clear();
						this->b2upsideframe[1].clear();
						for (int z = 0; z < this->m_DataObj.frame_child_num(i); z++) {
							if (this->m_DataObj.frame(i + 1 + z).x() > 0) {
								this->b2upsideframe[0].resize(this->b2upsideframe[0].size() + 1);
								this->b2upsideframe[0].back().Set_World(i + 1 + z, this->m_DataObj);
							}
							else {
								this->b2upsideframe[1].resize(this->b2upsideframe[1].size() + 1);
								this->b2upsideframe[1].back().Set_World(i + 1 + z, this->m_DataObj);
							}
						}
						std::sort(this->b2upsideframe[0].begin(), this->b2upsideframe[0].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //ソート
						std::sort(this->b2upsideframe[1].begin(), this->b2upsideframe[1].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //ソート
					}
					else if (p.find("履帯設置部", 0) != std::string::npos) { //2D物理
						this->downsideframe[0].clear();
						this->downsideframe[1].clear();
						for (int z = 0; z < this->m_DataObj.frame_child_num(i); z++) {
							if (this->m_DataObj.frame(i + 1 + z).x() > 0) {
								this->downsideframe[0].resize(this->downsideframe[0].size() + 1);
								this->downsideframe[0].back().Set_World(i + 1 + z, this->m_DataObj);
							}
							else {
								this->downsideframe[1].resize(this->downsideframe[1].size() + 1);
								this->downsideframe[1].back().Set_World(i + 1 + z, this->m_DataObj);
							}
						}
					}
				}
				//
				VECTOR_ref minmaxsub = maxpos - minpos;
				VECTOR_ref minmaxadd = minpos + maxpos;
				this->m_DynamicBox.SetAsBox(minmaxsub.x() / 2, minmaxsub.z() / 2, b2Vec2(minmaxadd.x() / 2, -minmaxadd.z() / 2), 0.f);	/*ダイナミックボディに別のボックスシェイプを定義します。*/
				//4隅確定
				this->square[0] = GetSide(true, false);			//2		左後部0
				this->square[1] = GetSide(true, true);			//10	左前部1
				this->square[2] = GetSide(false, false);		//3		右後部2
				this->square[3] = GetSide(false, true);			//11	右前部3
				//装甲
				for (int i = 0; i < this->m_DataCol.mesh_num(); i++) {
					std::string p = this->m_DataCol.material_name(i);
					if (p.find("armer", 0) != std::string::npos) {
						this->armer_mesh.emplace_back(i, std::stof(getparams::getright(p.c_str())));//装甲
					}
					else if (p.find("space", 0) != std::string::npos) {
						this->space_mesh.emplace_back(i);//空間装甲
					}
					else {
						this->module_mesh.emplace_back(i);//モジュール
					}
				}
				//data
				{
					int mdata = FileRead_open(("data/tank/" + this->m_name + "/data.txt").c_str(), FALSE);
					this->m_MaxHP = getparams::_int(mdata);
					this->m_IsFloat = getparams::_bool(mdata);
					this->m_MaxFrontSpeed = getparams::_float(mdata);
					this->m_MaxBackSpeed = getparams::_float(mdata);
					this->m_MaxBodyRad = getparams::_float(mdata);
					this->m_MaxTurretRad = deg2rad(getparams::_float(mdata));
					auto stt = getparams::_str(mdata);
					for (auto& g : this->m_GunFrameData) { g.Set(mdata, stt); }
					FileRead_close(mdata);
				}
				this->m_DataObj.Dispose();
				this->m_DataCol.Dispose();
			}
		};
		//
		class Guns {
		private:
			std::array<AmmoObj, max_bullet>	bullet;					/*確保する弾*/
			size_t							use_bullet{ 0 };		/*確保する弾*/
			float							loadcnt{ 0 };			/*装てんカウンター*/
			bool							reload_se_f{ true };
			float							fired{ 0.f };			/*駐退*/
			float							firereact{ 0.f };		/*反動*/
			int								rounds{ 0 };			/*弾数*/
			gun_frame						gun_info;				/**/
			std::vector<AmmoData>			Spec;					/**/
			VECTOR_ref						m_ShotradAdd;			//
			VECTOR_ref						m_MuzzlePos;
			VECTOR_ref						m_MuzzleVec;
		public:
			//getter
			auto&		GetBullet(void) noexcept { return bullet; }
			const auto&	Getrounds(void) const noexcept { return rounds; }
			const auto&	Getloadtime(void) const noexcept { return loadcnt; }
			const auto&	GetCaliberSize(void) const noexcept { return this->Spec[0].GetCaliber(); }
			const auto&	GetGunTrunnionFrameID(void) const noexcept { return this->gun_info.Get_frame(1).first; }
			const auto&	GetGunMuzzleFrameID(void) const noexcept { return this->gun_info.Get_frame(2).first; }
			const auto&	GetShotSound(void) const noexcept { return this->gun_info.GetShotSound(); }
			const auto&	GetReloadSound(void) const noexcept { return this->gun_info.GetReloadSound(); }
			const auto&	GetEjectSound(void) const noexcept { return this->gun_info.GetEjectSound(); }
			//銃反動
			void		FireReaction(MATRIX_ref* mat_t) noexcept {
				(*mat_t) *= MATRIX_ref::RotAxis(mat_t->xvec(), this->m_ShotradAdd.x()) * MATRIX_ref::RotAxis(mat_t->zvec(), this->m_ShotradAdd.z());
			}
		public:
			//角度指示
			void		ExecuteGunFrame(const float pGunXrad, const float pGunYrad, MV1* obj_body_t, MV1* col_body_t) noexcept {
				float yrad = pGunYrad;
				float xrad = std::clamp(pGunXrad, deg2rad(-10), deg2rad(20));
				frames id;
				id = this->gun_info.Get_frame(0);
				if (id.first >= 0) {
					obj_body_t->SetFrameLocalMatrix(id.first, MATRIX_ref::RotY(yrad) * MATRIX_ref::Mtrans(id.second));
					col_body_t->SetFrameLocalMatrix(id.first, MATRIX_ref::RotY(yrad) * MATRIX_ref::Mtrans(id.second));
				}
				id = this->gun_info.Get_frame(1);
				if (id.first >= 0) {
					obj_body_t->SetFrameLocalMatrix(id.first, MATRIX_ref::RotX(xrad) * MATRIX_ref::Mtrans(id.second));
					col_body_t->SetFrameLocalMatrix(id.first, MATRIX_ref::RotX(xrad) * MATRIX_ref::Mtrans(id.second));
				}
				id = this->gun_info.Get_frame(2);
				if (id.first >= 0) {
					obj_body_t->SetFrameLocalMatrix(id.first, MATRIX_ref::Mtrans(VECTOR_ref::front() * (this->fired * 0.5f * Scale_Rate)) * MATRIX_ref::Mtrans(id.second));
					col_body_t->SetFrameLocalMatrix(id.first, MATRIX_ref::Mtrans(VECTOR_ref::front() * (this->fired * 0.5f * Scale_Rate)) * MATRIX_ref::Mtrans(id.second));
				}
				this->m_MuzzlePos = obj_body_t->frame(GetGunMuzzleFrameID());
				this->m_MuzzleVec = obj_body_t->GetFrameLocalWorldMatrix(GetGunTrunnionFrameID()).zvec() * -1.f;
				Easing(&this->m_ShotradAdd, MATRIX_ref::RotY(yrad).xvec() * -1.f * deg2rad(-this->firereact * this->Spec[0].GetCaliber() * 50.f), 0.85f, EasingType::OutExpo);
			}
		public: //コンストラクタ、デストラクタ
			Guns(void) noexcept { }
			Guns(const Guns&) noexcept { }
		public:
			void		Init(const gun_frame& resorce, const MV1* pMapPtr) noexcept {
				this->gun_info = resorce;
				this->rounds = this->gun_info.Get_Ammo_Cap();
				//使用砲弾
				this->Spec = this->gun_info.Get_Spec();
				for (auto& a : this->bullet) {
					a.Init(pMapPtr);
				}
			}
			//銃演算
			bool		Execute(bool key, bool playSound) noexcept {
				auto SE = SoundPool::Instance();
				bool isshot = (key && this->loadcnt == 0 && this->rounds > 0);
				//射撃
				if (isshot) {
					this->loadcnt = this->gun_info.Get_load_time();
					this->rounds = std::max<int>(this->rounds - 1, 0);
					this->fired = 1.f;
					this->firereact = std::clamp(this->firereact + this->Spec[0].GetCaliber() * 10.f, 0.f, 3.f);

					this->bullet[this->use_bullet].Put(&this->Spec[0], this->m_MuzzlePos, this->m_MuzzleVec);
					++this->use_bullet %= this->bullet.size();

					this->reload_se_f = true;
					if (playSound) { SE->Get((int)SoundEnum::Tank_Eject).Play(GetEjectSound()); }
				}
				if (this->reload_se_f && this->loadcnt < 1.f) {
					this->reload_se_f = false;
					if (playSound) { SE->Get((int)SoundEnum::Tank_Reload).Play(GetReloadSound()); }
				}
				this->loadcnt = std::max(this->loadcnt - 1.f / FPS, 0.f);
				this->fired = std::max(this->fired - 1.f / FPS, 0.f);
				this->firereact = std::max(this->firereact - 1.f / FPS, 0.f);
				return isshot;
			}
			/*弾道描画*/
			void		Draw_ammo(void) noexcept {
				SetUseLighting(FALSE);
				for (auto& a : this->bullet) {
					a.Draw();
				}
				SetUseLighting(TRUE);
			}
			/* UI向けにヒット部分を表示*/
			void		Draw_Hit_UI(GraphHandle& Hit_Graph) noexcept {
				for (auto& a : this->bullet) { a.Draw_Hit_UI(Hit_Graph); }
			}
			/*おわり*/
			void		Dispose(void) noexcept {
				for (auto& a : this->bullet) {
					a.Dispose();
				}
				this->fired = 0.f;
				this->loadcnt = 0.f;
				this->reload_se_f = true;
				this->rounds = 0;
				this->Spec.clear();
			}
		};
		//操作共通
		class VehicleClass : public ObjectBaseClass, public Effect_UseControl {
		private:
			//弾痕
			class HIT_ACTIVE {
				struct Hit {		      /**/
					bool Flag{ false };   /*弾痕フラグ*/
					int use{ 0 };	      /*使用フレーム*/
					MV1 pic;	      /*弾痕モデル*/
					VECTOR_ref pos;	      /*座標*/
					MATRIX_ref mat;	      /**/
				};								      /**/
				std::array<Hit, 24> hit_obj;					      /*弾痕*/
				int hitbuf = 0;		       /*使用弾痕*/
			public:
				void			Set(const MV1& hit_pic) {
					for (auto& h : this->hit_obj) {
						h.Flag = false;
						h.pic = hit_pic.Duplicate();
						h.use = 0;
						h.mat = MGetIdent();
						h.pos = VGet(0.f, 0.f, 0.f);
					}
				}
				void			Set(const moves& this_move, const  VECTOR_ref& Put_pos, const  VECTOR_ref& Put_normal, const VECTOR_ref& ammo_nomal/*this->move.vec.Norm()*/, const float&caliber, bool isPene) {
					float asize = 200.f*caliber;
					auto y_vec = MATRIX_ref::Vtrans(Put_normal, this_move.mat.Inverse());
					auto z_vec = MATRIX_ref::Vtrans(Put_normal.cross(ammo_nomal).Norm(), this_move.mat.Inverse());
					auto scale = VECTOR_ref::vget(asize / std::abs(ammo_nomal.dot(Put_normal)), asize, asize);

					this->hit_obj[this->hitbuf].use = (isPene) ? 0 : 1;				//弾痕
					this->hit_obj[this->hitbuf].mat = MATRIX_ref::GetScale(scale)* MATRIX_ref::Axis1_YZ(y_vec, z_vec);
					this->hit_obj[this->hitbuf].pos = MATRIX_ref::Vtrans(Put_pos - this_move.pos, this_move.mat.Inverse()) + y_vec * 0.005f;
					this->hit_obj[this->hitbuf].Flag = true;
					++this->hitbuf %= this->hit_obj.size();
				}
				void			Update(const moves& this_move) {
					//弾痕
					for (auto& h : this->hit_obj) {
						if (h.Flag) {
							h.pic.SetMatrix(h.mat* this_move.mat*MATRIX_ref::Mtrans(this_move.pos + MATRIX_ref::Vtrans(h.pos, this_move.mat)));
						}
					}
				}
				void			Draw(void) noexcept {
					for (auto& h : this->hit_obj) {
						if (h.Flag) {
							h.pic.DrawFrame(h.use);
						}
					}
				}
				void			Dispose(void) noexcept {
					for (auto& h : this->hit_obj) {
						h.Flag = false;
						h.use = 0;
						h.pic.Dispose();
						h.pos = VGet(0, 0, 0);
						h.mat = MGetIdent();
					}
				}
			};
			//HP関連
			class Damages {
			private:
				int HP{ 100 };
				int HP_full{ 100 };
				std::vector<int> HP_parts;
			public:
				//getter
				const auto		Get_alive(void) const noexcept { return this->HP != 0; }																								//生きているか
				const auto		HP_Per(void) const noexcept { return float(this->HP) / this->HP_full; }
				const auto&		Get_HP(void) const noexcept { return HP; }
				const auto&		Get_HP_full(void) const noexcept { return HP_full; }
				const auto&		Get_HP_parts(void) const noexcept { return HP_parts; }
			public:
				void			SubHP_Parts(int damage_t, int parts_Set_t) noexcept { this->HP_parts[parts_Set_t] = std::max<int>(this->HP_parts[parts_Set_t] - damage_t, 0); }
				void			SubHP(int damage_t, float rad_t)  noexcept { this->HP = std::max<int>(this->HP - damage_t, 0); }
				void			AddHP(int repair_t) noexcept { this->HP = std::clamp<int>(this->HP + repair_t, 0, this->HP_full); }
			public:
				void			Init(int parts_num, int Full_t = -1) noexcept {
					if (Full_t > 0) {
						this->HP_full = Full_t;
					}
					this->HP = this->HP_full;
					this->HP_parts.resize(parts_num);
					for (auto& h : this->HP_parts) { h = this->HP_full; }//モジュール耐久
				}
				void			Dispose(void) noexcept {
					this->HP = 0;
					this->HP_parts.clear();
				}
			};
			//命中関連
			class VEHICLE_HitControl {
			private:
				class HitSortInfo {
					size_t m_hitmesh{ SIZE_MAX };
					float m_hitDistance{ -1 };
				public:
					const auto		operator<(const HitSortInfo& tgt) const noexcept { return this->m_hitDistance < tgt.m_hitDistance; }
					void			Set(size_t f_t, float s_t)noexcept {
						this->m_hitmesh = f_t;
						this->m_hitDistance = s_t;
					}
					const auto		GetHitMesh(void) const noexcept { return this->m_hitmesh; }
					const auto		IsHit(void) const noexcept { return (this->m_hitDistance != (std::numeric_limits<float>::max)()); }
				};
			private:
				std::vector<HitSortInfo> hitssort;											/*フレームに当たった順番*/
				std::vector<MV1_COLL_RESULT_POLY> hitres;									/*確保*/
			public:
				void			HitSort(void) noexcept { std::sort(this->hitssort.begin(), this->hitssort.end(), [](const HitSortInfo& x, const HitSortInfo& y) { return x < y; }); }
				const auto&		Get_hitssort(void) const noexcept { return this->hitssort; }
				const auto&		Get_hitres(size_t ID_t) const noexcept { return this->hitres[ID_t]; }
				const auto		HitCheck(int m, const VECTOR_ref& repos, const MV1_COLL_RESULT_POLY& hitres_t) {
					this->hitres[m] = hitres_t;
					if (this->hitres[m].HitFlag == TRUE) {
						this->hitssort[m].Set(m, (repos - this->hitres[m].HitPosition).size());
						return true;
					}
					else {
						this->hitssort[m].Set(m, (std::numeric_limits<float>::max)());
						return false;
					}
				}
				void			Set(size_t siz_t) {
					this->hitres.resize(siz_t);		//モジュールごとの当たり判定結果を確保
					this->hitssort.resize(siz_t);	//モジュールごとの当たり判定順序を確保
				}
				void			Reset(void) noexcept {
					this->hitres.clear();
					this->hitssort.clear();
				}
			};
			//履帯BOX2D
			class FootWorld {
				class cat_frame {
				public:
					frames frame;
					float will_y{ 0.f };
					MV1_COLL_RESULT_POLY colres{};
					EffectS gndsmkeffcs;
					float gndsmksize = 1.f;

					cat_frame(void) noexcept {
						frame.first = -1;
						will_y = 0.f;
						gndsmksize = 1.f;
					}
					cat_frame(const cat_frame& tgt) {
						frame = tgt.frame;
						will_y = 0.f;
						gndsmksize = 1.f;
					}
					void			operator=(const cat_frame& tgt) { frame = tgt.frame; }
				};

				std::shared_ptr<b2World>	m_FootWorld{ nullptr };		/*足world*/
				b2RevoluteJointDef			f_jointDef;					/*ジョイント*/
				std::vector<b2Pats>			Foot, Wheel, Yudo;			//
				std::vector<cat_frame>		downsideframe;				/*履帯*/
			public:			//getter
				const auto&		Getb2downsideframe(void) const noexcept { return this->downsideframe; }
			public:
				void			Init(bool IsLeft, VhehicleData* pUseVeh, MV1* pTargetObj) {
					this->m_FootWorld = std::make_unique<b2World>(b2Vec2(0.0f, 0.0f));
					float LorR = ((IsLeft) ? 1.f : -1.f);
					//履帯
					this->Foot.clear();
					{
						b2BodyDef bd;
						b2Body* BodyB = this->m_FootWorld->CreateBody(&bd);
						b2EdgeShape shape;
						shape.Set(b2Vec2(-40.0f, -10.0f), b2Vec2(40.0f, -10.0f));
						BodyB->CreateFixture(&shape, 0.0f);
						b2Body* BodyA = BodyB;
						b2Vec2 anchor;
						for (const auto& w : pUseVeh->Get_b2upsideframe(IsLeft ? 0 : 1)) {
							anchor = b2Vec2(w.second.z(), w.second.y());
							if (w.second.x() * LorR > 0) {
								this->Foot.resize(this->Foot.size() + 1);
								b2PolygonShape f_dynamicBox; /*ダイナミックボディに別のボックスシェイプを定義します。*/
								f_dynamicBox.SetAsBox(0.2f, 0.05f);
								this->Foot.back().Set(CreateB2Body(this->m_FootWorld, b2_dynamicBody, anchor.x, anchor.y), &f_dynamicBox);
								this->f_jointDef.Initialize(BodyA, this->Foot.back().Get(), anchor);
								this->m_FootWorld->CreateJoint(&this->f_jointDef);
								BodyA = this->Foot.back().Get();
							}
						}
						if (!this->Foot.empty()) {
							this->f_jointDef.Initialize(BodyA, BodyB, anchor);
							this->m_FootWorld->CreateJoint(&this->f_jointDef);
						}
					}
					{
						const auto& w = pUseVeh->Get_b2downsideframe()[((IsLeft) ? 0 : 1)];
						auto& w2 = this->downsideframe;
						w2.resize(w.size());
						for (auto& t : w2) {
							t.frame = w[&t - &w2.front()];
							t.gndsmkeffcs.set_loop(effectControl.effsorce.back());
							t.gndsmkeffcs.put_loop(VECTOR_ref::vget(0, -1, 0), VECTOR_ref::vget(0, 0, 1), 0.1f*Scale_Rate);
							t.gndsmksize = 0.1f;
						}
					}
					//転輪
					this->Wheel.clear();
					this->Yudo.clear();
					if (!this->Foot.empty()) {
						//転輪(動く)
						for (const auto& w : pUseVeh->Get_wheelframe()) {
							VECTOR_ref vects = pTargetObj->GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LorR > 0) {
								this->Wheel.resize(this->Wheel.size() + 1);
								b2CircleShape shape;
								shape.m_radius = vects.y() - 0.1f;
								this->Wheel.back().Set(CreateB2Body(this->m_FootWorld, b2_kinematicBody, vects.z(), vects.y()), &shape);
							}
						}
						//誘導輪(動かない)
						for (const auto& w : pUseVeh->Get_wheelframe_nospring()) {
							VECTOR_ref vects = pTargetObj->GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LorR > 0) {
								this->Yudo.resize(this->Yudo.size() + 1);
								b2CircleShape shape;
								shape.m_radius = 0.05f;
								this->Yudo.back().Set(CreateB2Body(this->m_FootWorld, b2_kinematicBody, vects.z(), vects.y()), &shape);
							}
						}
					}
				}
				void			FirstExecute(MV1* pTargetObj, const MV1* pMapPtr) {
					auto y_vec = pTargetObj->GetMatrix().yvec();
					for (auto& t : this->downsideframe) {
						if (t.frame.first >= 0) {
							pTargetObj->frame_Reset(t.frame.first);
							auto startpos = pTargetObj->frame(t.frame.first);
							t.colres = pMapPtr->CollCheck_Line(startpos + y_vec * ((-t.frame.second.y()) + 2.f*Scale_Rate), startpos + y_vec * ((-t.frame.second.y()) - 0.3f*Scale_Rate));

							pTargetObj->SetFrameLocalMatrix(t.frame.first,
								MATRIX_ref::Mtrans(VECTOR_ref::up() * ((t.colres.HitFlag == TRUE) ? (t.colres.HitPosition.y + y_vec.y() * t.frame.second.y() - startpos.y()) : -0.4f*Scale_Rate)) *
								MATRIX_ref::Mtrans(t.frame.second)
							);
						}
					}
				}
				void			LateExecute(bool IsLeft, VhehicleData* pUseVeh, MV1* pTargetObj, const b2Vec2& pGravity, float pWheelRotate, float pSpdAdd) {
					int LR_t = (IsLeft ? 1 : -1);
					if (this->Foot.size() != 0) {
						auto Wheel_t = this->Wheel.begin();
						for (const auto& w : pUseVeh->Get_wheelframe()) {
							VECTOR_ref vects = pTargetObj->GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LR_t > 0) {
								Wheel_t->SetTransform(b2Vec2(vects.z(), vects.y()), pWheelRotate);
								Wheel_t++;
							}
						}
						size_t i = 0;
						for (const auto& w : pUseVeh->Get_wheelframe_nospring()) {
							VECTOR_ref vects = pTargetObj->GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LR_t > 0) {
								this->Yudo[i++].SetTransform(b2Vec2(vects.z(), vects.y()), pWheelRotate);
							}
						}
						for (auto& t : this->Foot) {
							t.SetLinearVelocity(pGravity); //
						}

						this->m_FootWorld->Step(0.1f, 3, 3);

						for (auto& t : this->Foot) {
							t.pos = VECTOR_ref::vget(t.pos.x(), t.Pos().y, t.Pos().x);
						}
					}
					{
						size_t i = 0;
						for (const auto& w : pUseVeh->Get_b2upsideframe((IsLeft) ? 0 : 1)) {
							if (w.second.x() * LR_t > 0) {
								auto& t = this->Foot[i++];
								t.pos = VECTOR_ref::vget(w.second.x(), t.pos.y(), t.pos.z());
								pTargetObj->SetFrameLocalMatrix(w.first, MATRIX_ref::Mtrans(t.pos));
							}
						}
					}
					//エフェクト更新
					for (auto& t : this->downsideframe) {
						if (t.colres.HitFlag == TRUE) {
							Easing(&t.gndsmksize, 0.1f + std::abs(pSpdAdd) / ((0.01f / 3.6f) / FPS) * 0.5f, 0.975f, EasingType::OutExpo);
						}
						else {
							t.gndsmksize = 0.1f;
						}
						t.gndsmkeffcs.handle.SetPos(pTargetObj->frame(t.frame.first) + pTargetObj->GetMatrix().yvec() * (-t.frame.second.y()));
						t.gndsmkeffcs.handle.SetScale(std::clamp(t.gndsmksize, 0.1f, 0.5f)*Scale_Rate);
					}
				}
				void			Dispose(void) noexcept {
					for (auto& t : this->Foot) { t.Dispose(); }
					this->Foot.clear();
					for (auto& t : this->Wheel) { t.Dispose(); }
					this->Wheel.clear();
					for (auto& t : this->Yudo) { t.Dispose(); }
					this->Yudo.clear();
					this->m_FootWorld.reset();

					for (auto& t : this->downsideframe) {
						t.gndsmkeffcs.handle.Dispose();
					}
					this->downsideframe.clear();
				}
			};
		private:
			//操作
			bool												m_ReadySwitch{ false };							//
			bool												m_KeyActive{ true };							//
			std::array<bool, 6>									m_key{ false };									//キー
			MATRIX_ref											m_MouseVec;										//マウスエイム用変数確保
			//
			float												m_AimingDistance = 100.f;						//
			std::vector<Guns>									m_Gun;											//
			float												m_speed{ 0.f };									//移動速度
			float												m_speed_add{ 0.f };								//
			float												m_speed_sub{ 0.f };								//
			VECTOR_ref											m_Tilt;											//
			VECTOR_ref											m_radAdd;										//
			float												m_yradadd_left{ 0.f };							//
			float												m_yradadd_right{ 0.f };							//
			//box2d
			b2Pats												m_b2mine;										//BOX2D
			float												m_spd_buf{ 0.f };								//BOX2D
			std::array<FootWorld, 2>							m_b2Foot;										//履帯BOX2D
			//転輪
			float												m_wheel_Left{ 0.f };							//転輪回転
			float												m_wheel_Right{ 0.f };							//転輪回転
			std::vector<float>									m_wheel_frameYpos;								//転輪のY方向保持
			VECTOR_ref											m_BodyNormal{ VECTOR_ref::up() };				//
			float												m_spd_rec;										//
			bool												m_view_override{ false };						//
			float												m_view_xrad{ 0.f };								//
			float												m_view_yrad{ 0.f };								//
			float												m_engine_time{ 0.f };							/*エンジン音声*/
			float												m_range{ 6.0f };								//
			float												m_range_r = m_range;							//
			float												m_range_change = m_range / 10.f;				//
			float												m_ratio = 1.f;									//
			bool												m_changeview = false;							//
			HIT_ACTIVE											m_Hit_active;									//
			bool												m_PosBufOverRideFlag{ false };					//
			VECTOR_ref											m_PosBufOverRide;
			VECTOR_ref											m_VecBufOverRide;
			float												m_yRadOverRide;
			VECTOR_ref											m_add_vec_real;									//移動ベクトルバッファ
			Damages												m_DamageControl;
			char												m_MyID{ 0 };
			std::vector<std::shared_ptr<VehicleClass>>*			m_ALL_v{ nullptr };
			VEHICLE_HitControl									m_HitControl;
			VhehicleData*										m_VecData{ nullptr };							/*固有値*/
			CharaTypeID											m_CharaType{ CharaTypeID::Enemy };
			float												m_Score{ 0.f };									//スコア
			DamageEvent											m_DamageEvent;
			unsigned char										m_DamageSwitch;
			unsigned char										m_DamageSwitchRec;
		public:
			//setter
			const auto		SetDamageEvent(const DamageEvent& value) noexcept {
				if (this->m_MyID == value.ID) {
					this->m_DamageControl.SubHP(value.Damage, value.rad);
					return true;
				}
				return false;
			}
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			SetDamageSwitchRec(char value) noexcept { this->m_DamageSwitchRec = value; }
			void			SetAimingDistance(float value) noexcept { this->m_AimingDistance = value; }
			//getter
			const auto&		GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto&		GetDamageSwitch(void) const noexcept { return this->m_DamageSwitch; }
			const auto&		GetDamageSwitchRec(void) const noexcept { return this->m_DamageSwitchRec; }
			const auto&		GetScore(void) const noexcept { return this->m_Score; }
			const auto&		GetHP(void) const noexcept { return this->m_DamageControl.Get_HP(); }
			const auto&		GetHPMax(void) const noexcept { return this->m_DamageControl.Get_HP_full(); }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetName(void) const noexcept { return this->m_VecData->GetName(); }
			const auto&		GetLookVec(void) const noexcept { return this->m_MouseVec; }
			const auto&		Get_pseed_per(void) const noexcept { return this->m_add_vec_real.size() / (this->m_VecData->GetMaxFrontSpeed() / 3.6f); }						//移動速度のパーセンテージ
			const auto&		Gunloadtime(size_t id_t) const noexcept { return this->m_Gun[id_t].Getloadtime(); }
			const auto&		Gunround(size_t id_t) const noexcept { return this->m_Gun[id_t].Getrounds(); }
			const auto&		Get_Gunsize(void) const noexcept { return this->m_Gun.size(); }
			const auto&		Get_ratio(void) const noexcept { return this->m_ratio; }																				//UI用
			const auto&		Get_changeview(void) const noexcept { return this->m_changeview; }																	//照準変更時
			const auto&		GetViewxRad(void) const noexcept { return this->m_view_xrad; }
			const auto&		GetViewyRad(void) const noexcept { return this->m_view_yrad; }
			const auto		Get_body_yrad(void) const noexcept { auto pp = this->m_move.mat.zvec()*-1.f; return atan2f(pp.x(), pp.z()); }
			const auto		is_ADS(void) const noexcept { return this->m_range == 0.f; }																			//ADS中

			const auto		GetGunMuzzlePos(void) const noexcept { return GetObj_const().frame(this->m_Gun[0].GetGunMuzzleFrameID()); }
			const auto		GetGunMuzzleVec(void) const noexcept { return (GetGunMuzzlePos() - GetObj_const().frame(this->m_Gun[0].GetGunTrunnionFrameID())).Norm(); }
			

			const auto		Get_EyePos_Base(void) const noexcept { return (is_ADS()) ? (GetObj_const().frame(this->m_Gun[0].GetGunTrunnionFrameID())) : (this->m_move.pos + (this->m_move.mat.yvec() * 3.f * Scale_Rate)); }
			const auto		Set_MidPos(void) noexcept { return (this->m_move.pos + (this->m_move.mat.yvec() * 1.5f * Scale_Rate)); }							/*HPバーを表示する場所*/
			//
			void			ReSet_range(void) noexcept { this->m_range = 6.f; }
			void			SetPosBufOverRide(const VECTOR_ref& pPos, const VECTOR_ref& pVec, float yrad) {
				this->m_PosBufOverRideFlag = true;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide = pVec;
				this->m_yRadOverRide = yrad;
			}
			//
			void			ValueSet(
				float pxRad, float pyRad, const VECTOR_ref& pPos,
				VhehicleData* pVeh_data, const MV1& hit_pic, std::shared_ptr<b2World>& pB2World,
				std::vector<std::shared_ptr<VehicleClass>>* ALL_v_t, char pID
			) {
				//使用するポインタ設定
				this->m_ALL_v = ALL_v_t;
				this->m_MyID = pID;
				//
				this->m_move.mat = MATRIX_ref::RotX(pxRad) * MATRIX_ref::RotY(pyRad);
				this->m_move.pos = pPos;
				this->m_move.vec.clear();
				this->m_VecData = pVeh_data;
				this->m_Hit_active.Set(hit_pic);				//弾痕
				//本体
				GetObj().material_AlphaTestAll(true, DX_CMP_GREATER, 128);
				//コリジョン
				this->m_HitControl.Set(this->m_col.mesh_num());
				//
				b2PolygonShape dynamicBox = this->m_VecData->GetDynamicBox();
				this->m_b2mine.Set(CreateB2Body(pB2World, b2_dynamicBody, this->m_move.pos.x(), this->m_move.pos.z(), atan2f(-this->m_move.mat.zvec().x(), -this->m_move.mat.zvec().z())), &dynamicBox);	/*シェイプをボディに追加*/
				for (auto& f : this->m_b2Foot) {
					f.Init(&f == &this->m_b2Foot.front(), this->m_VecData, &GetObj());
				}
				this->m_wheel_frameYpos.resize(this->m_VecData->Get_wheelframe().size());
				for (auto& w : this->m_wheel_frameYpos) { w = 0.f; }
				//砲
				this->m_Gun.resize(this->m_VecData->Get_gunframe().size());
				for (auto& g : this->m_Gun) {
					g.Init(this->m_VecData->Get_gunframe()[&g - &this->m_Gun.front()], this->m_MapCol);
				}
				//ヒットポイント
				this->m_DamageControl.Init((int)this->m_col.mesh_num(), this->m_VecData->GetMaxHP());
				/*戦車スポーン*/
				this->m_b2mine.SetTransform(b2Vec2(this->m_move.pos.x(), this->m_move.pos.z()), Get_body_yrad());
				this->m_PosBufOverRideFlag = false;
				this->m_DamageSwitch = 0;
				this->m_DamageSwitchRec = this->m_DamageSwitch;

				this->m_MouseVec = this->m_move.mat.Inverse();

				this->m_speed = 0.f;
				this->m_speed_add = 0.f;
				this->m_speed_sub = 0.f;
				this->m_wheel_Left = 0.f;
				this->m_wheel_Right = 0.f;
				this->m_Tilt.clear();
				this->m_radAdd.clear();
				this->m_yradadd_left = 0.f;
				this->m_yradadd_right = 0.f;
			}
			//
			void			SetInput(const InputControl& pInput, bool pReady, bool isOverrideView) {
				this->m_ReadySwitch = (this->m_KeyActive != pReady);
				this->m_KeyActive = pReady;
				//エイム
				auto x_mouse = std::atan2f(-this->m_MouseVec.zvec().y(), std::hypotf(this->m_MouseVec.zvec().x(), this->m_MouseVec.zvec().z()));
				auto prevx_mouse = x_mouse;
				x_mouse = std::clamp(x_mouse + pInput.GetAddxRad(), -deg2rad(60.f), deg2rad(60.f));
				this->m_MouseVec = MATRIX_ref::RotX(x_mouse - prevx_mouse) *this->m_MouseVec* MATRIX_ref::RotY(pInput.GetAddyRad());
				//
				if (isOverrideView) {
					this->m_view_override = true;
					this->m_view_xrad = pInput.GetxRad();
					this->m_view_yrad = pInput.GetyRad();
				}
				this->m_key[0] = pInput.GetAction5() && pReady && this->m_DamageControl.Get_alive();			//射撃
				this->m_key[1] = false && pReady && this->m_DamageControl.Get_alive();							//マシンガン
				this->m_key[2] = pInput.GetGoFrontPress() && pReady && this->m_DamageControl.Get_alive();		//前進
				this->m_key[3] = pInput.GetGoBackPress() && pReady && this->m_DamageControl.Get_alive();		//後退
				this->m_key[4] = pInput.GetGoRightPress() && pReady && this->m_DamageControl.Get_alive();		//右
				this->m_key[5] = pInput.GetGoLeftPress() && pReady && this->m_DamageControl.Get_alive();		//左
			}
			//カメラ設定出力
			void			Setcamera(cam_info& camera_main, const float fov_base) noexcept {
				VECTOR_ref eyepos, eyetgt;
				auto OLD = this->m_range;
				if (is_ADS()) {
					eyepos = Get_EyePos_Base();
					this->m_ratio = std::clamp(this->m_ratio + float(GetMouseWheelRotVolWithCheck()) * 2.0f, 0.0f, 30.f);
					if (this->m_ratio == 0.f) {
						this->m_range = 0.f + this->m_range_change;
						this->m_range_r = this->m_range;
					}
					else {
						Easing(&camera_main.fov, fov_base / this->m_ratio, 0.9f, EasingType::OutExpo);
					}
				}
				else {
					eyepos = Get_EyePos_Base() + this->m_MouseVec.zvec() * this->m_range_r*Scale_Rate;
					eyetgt = eyepos + this->m_MouseVec.zvec() * -1.f * (this->m_range_r * Scale_Rate);
					if (GetMapColNearest(eyepos, &eyetgt)) { eyepos = eyetgt; }

					this->m_ratio = 2.0f;
					this->m_range = std::clamp(this->m_range - float(GetMouseWheelRotVolWithCheck()) * this->m_range_change, 0.f, 9.f);
					Easing(&this->m_range_r, this->m_range, 0.8f, EasingType::OutExpo);

					Easing(&camera_main.fov, fov_base, 0.9f, EasingType::OutExpo);
				}
				eyetgt = Get_EyePos_Base() + this->m_MouseVec.zvec() * -1.f * std::max(this->m_range_r*Scale_Rate, 1.f);

				this->m_changeview = ((this->m_range != OLD) && (this->m_range == 0.f || OLD == 0.f));
				camera_main.set_cam_pos(eyepos, eyetgt, this->m_move.mat.yvec());

				Easing(&camera_main.near_, 1.f + 2.f*((is_ADS()) ? this->m_ratio : 0.f), 0.9f, EasingType::OutExpo);
				Easing(&camera_main.far_, std::max(this->m_AimingDistance, Scale_Rate * 20.f) + Scale_Rate * 20.f, 0.9f, EasingType::OutExpo);
			}
		private:
			//被弾チェック
			const auto		CheckAmmoHited(const AmmoObj& pAmmo) {
				auto HitCheck_Tank = [&](int m, const moves& ray) { return this->m_HitControl.HitCheck(m, ray.repos, GetColLine(ray.repos, ray.pos, m)); };
				bool is_Hit = false;
				for (auto& m : this->m_VecData->Get_module_mesh()) { is_Hit |= HitCheck_Tank(m, pAmmo.GetMove()); }//モジュール
				for (auto& m : this->m_VecData->Get_space_mesh()) { is_Hit |= HitCheck_Tank(m, pAmmo.GetMove()); }				//空間装甲
				//装甲(一番近い位置のものに限定する)
				int t = -1;
				MV1_COLL_RESULT_POLY colres; colres.HitFlag = FALSE;
				VECTOR_ref EndPos = pAmmo.GetMove().pos;
				for (const auto& m : this->m_VecData->Get_armer_mesh()) { this->m_HitControl.HitCheck(m.first, pAmmo.GetMove().repos, colres); }//全リセット
				for (const auto& m : this->m_VecData->Get_armer_mesh()) {
					auto colres_t = GetColLine(pAmmo.GetMove().repos, EndPos, m.first);
					if (colres_t.HitFlag == TRUE) {
						t = m.first;
						colres = colres_t;
						EndPos = colres_t.HitPosition;
					}
				}
				if (t != -1) {
					is_Hit |= this->m_HitControl.HitCheck(t, pAmmo.GetMove().repos, colres);
				}
				return is_Hit;
			}
			//被弾処理
			const auto		CalcAmmoHited(AmmoObj* pAmmo, const VECTOR_ref& pShooterPos) {
				auto SE = SoundPool::Instance();
				this->m_HitControl.HitSort();				//当たり判定を近い順にソート
				bool isDamage = false;
				//ダメージ面に届くまで判定
				for (const auto& tt : this->m_HitControl.Get_hitssort()) {
					if (tt.IsHit()) {
						VECTOR_ref HitPos = this->m_HitControl.Get_hitres(tt.GetHitMesh()).HitPosition;
						VECTOR_ref HitNormal = this->m_HitControl.Get_hitres(tt.GetHitMesh()).Normal;
						HitNormal = HitNormal.Norm();

						for (auto& mesh : this->m_VecData->Get_space_mesh()) {
							if (tt.GetHitMesh() == mesh) {
								//空間装甲に当たったのでモジュールに30ダメ
								Effect_UseControl::Set_Effect(Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate);
								//this->m_DamageControl.SubHP_Parts(30, (int)tt.GetHitMesh());
							}
						}
						for (auto& mesh : this->m_VecData->Get_module_mesh()) {
							if (tt.GetHitMesh() == mesh) {
								//モジュールに当たったのでモジュールに30ダメ
								Effect_UseControl::Set_Effect(Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate);
								//this->m_DamageControl.SubHP_Parts(30, (int)tt.GetHitMesh());
							}
						}
						//ダメージ面に当たった
						for (auto& mesh : this->m_VecData->Get_armer_mesh()) {
							if (tt.GetHitMesh() == mesh.first) {
								//弾がダメージ層に届いた
								if (pAmmo->PenetrationCheck(mesh.second, HitNormal)) {								//ダメージ面に当たった時に装甲値に勝てるかどうか
									pAmmo->Penetrate();	//貫通
									SE->Get((int)SoundEnum::Tank_Damage).Play_3D(GetRand(1), HitPos, 100.f*Scale_Rate, 127);
									//ダメージ計算
									auto v1 = MATRIX_ref::RotY(Get_body_yrad()).zvec();
									auto v2 = (pShooterPos - this->m_move.pos).Norm(); v2.y(0);
									this->m_DamageEvent.SetEvent(this->m_MyID, pAmmo->GetDamage(), atan2f(v1.cross(v2).y(), v1.dot(v2)));
									++this->m_DamageSwitch;// %= 255;//
									//this->m_DamageControl.SubHP_Parts(pAmmo->GetDamage(), (int)tt.GetHitMesh());
									if (!this->m_DamageControl.Get_alive()) {
										//撃破
										Effect_UseControl::Set_Effect(Effect::ef_greexp2, this->m_move.pos, this->m_move.mat.zvec(), Scale_Rate*2.f);
									}
									isDamage = true;
								}
								else {
									pAmmo->Ricochet(HitPos, HitNormal);	//跳弾
									SE->Get((int)SoundEnum::Tank_Ricochet).Play_3D(GetRand(16), HitPos, 100.f*Scale_Rate, 127);
								}
								//エフェクトセット
								Effect_UseControl::Set_Effect(Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate*10.f);
								this->m_Hit_active.Set(this->m_move, HitPos, HitNormal, pAmmo->GetMove().vec.Norm(), pAmmo->GetCaliberSize()*Scale_Rate, !pAmmo->IsActive());
								break;
							}
						}
					}
					else {
						break;
					}
				}
				return isDamage;
			}
		public:
			const auto		CheckAmmoHit(AmmoObj* pAmmo, const VECTOR_ref& pShooterPos) {
				std::pair<bool, bool> isDamaged{ false,false };
				if (this->RefreshCol(pAmmo->GetMove().repos, pAmmo->GetMove().pos, 10.0f*Scale_Rate)) {
					if (this->CheckAmmoHited(*pAmmo)) {									//とりあえず当たったかどうか探す
						isDamaged.first = true;
						if (this->CalcAmmoHited(pAmmo, pShooterPos)) {
							isDamaged.second = true;
						}
					}
				}
				return isDamaged;
			}
		private: //更新関連
			//以前の状態保持														//
			void			ExecuteSavePrev(void) noexcept {
				this->m_spd_rec = this->m_speed;
				this->m_ColActive = false;
			}
			//操作																	//
			void			ExecuteInput(void) noexcept {
				if (!this->m_view_override) {
					float												view_YradAdd{ 0.f };							/**/
					float												view_XradAdd{ 0.f };							/**/
					if (this->m_DamageControl.Get_alive()) {
						if (false) { //砲塔ロック
							view_XradAdd = 0.f;
							view_YradAdd = 0.f;
						}
						else {
							//狙い
							VECTOR_ref StartPos = GetGunMuzzlePos();
							VECTOR_ref EndPos = StartPos + (this->m_MouseVec.zvec() * -1.f).Norm() * (500.f*Scale_Rate);
							GetMapColNearest(StartPos, &EndPos);

							StartPos = GetObj().frame(this->m_Gun[0].GetGunTrunnionFrameID());
							//反映
							auto vec_a = (EndPos - StartPos).Norm();
							auto vec_z = (GetGunMuzzlePos() - StartPos);
							float z_hyp = std::hypotf(vec_z.x(), vec_z.z());
							float cost = vec_z.cross(vec_a).y() / z_hyp;

							float limit = this->m_VecData->GetMaxTurretRad() / FPS;
							view_YradAdd = std::clamp((atan2f(cost, sqrtf(std::abs(1.f - cost * cost)))) / 5.f, -limit, limit);
							view_XradAdd = std::clamp((atan2f(vec_a.y(), 1.f) - atan2f(vec_z.y(), z_hyp)) / 5.f, -limit, limit);
						}
					}
					else {
						view_XradAdd = 0.f;
						view_YradAdd = 0.f;
					}
					this->m_view_xrad = std::clamp(this->m_view_xrad + view_XradAdd, deg2rad(-10), deg2rad(20));
					this->m_view_yrad += view_YradAdd;
				}
				this->m_view_override = false;
			}
			//フレーム操作															//
			void			ExecuteFrame(void) noexcept {
				//砲塔旋回
				for (auto& g : this->m_Gun) {
					g.ExecuteGunFrame(this->m_view_xrad, this->m_view_yrad, &GetObj(), &this->m_col);
				}
				//転輪
				auto y_vec = GetObj().GetMatrix().yvec();
				for (auto& f : this->m_VecData->Get_wheelframe()) {
					if (f.first >= 0) {
						auto ID = &f - &this->m_VecData->Get_wheelframe().front();
						GetObj().frame_Reset(f.first);
						auto startpos = GetObj().frame(f.first);
						auto ColResGround = this->m_MapCol->CollCheck_Line(startpos + y_vec * ((-f.second.y()) + 2.f*Scale_Rate), startpos + y_vec * ((-f.second.y()) - 0.3f*Scale_Rate));
						Easing(&this->m_wheel_frameYpos[ID], (ColResGround.HitFlag == TRUE) ? (ColResGround.HitPosition.y + y_vec.y() * f.second.y() - startpos.y()) : -0.3f*Scale_Rate, 0.9f, EasingType::OutExpo);
						GetObj().SetFrameLocalMatrix(f.first,
							MATRIX_ref::RotX((f.second.x() >= 0) ? this->m_wheel_Left : this->m_wheel_Right) *
							MATRIX_ref::Mtrans(VECTOR_ref::up() * this->m_wheel_frameYpos[ID]) *
							MATRIX_ref::Mtrans(f.second)
						);
					}
				}
				for (const auto& f : this->m_VecData->Get_wheelframe_nospring()) {
					if (f.first >= 0) {
						GetObj().SetFrameLocalMatrix(f.first, MATRIX_ref::RotX((f.second.x() >= 0) ? this->m_wheel_Left : this->m_wheel_Right) * MATRIX_ref::Mtrans(f.second));
					}
				}
			}
			//移動操作																//
			void			ExecuteMove(void) noexcept {
				auto SE = SoundPool::Instance();
				bool isfloat = this->m_VecData->GetIsFloat() && (this->m_move.pos.y() == -this->m_VecData->GetDownInWater());
				//傾きの取得
				{
					const auto bNormal = (isfloat) ? VECTOR_ref::up() : this->m_BodyNormal;
					float yradBody = 0.f;
					if (this->m_PosBufOverRideFlag) {
						yradBody = this->m_yRadOverRide;
					}
					else {
						auto pp = (this->m_move.mat * MATRIX_ref::RotVec2(VECTOR_ref::up(), bNormal).Inverse()).zvec() * -1.f;
						yradBody = atan2f(pp.x(), pp.z());
					}
					this->m_move.mat = MATRIX_ref::Axis1_YZ(bNormal, (MATRIX_ref::RotY(yradBody)*MATRIX_ref::RotVec2(VECTOR_ref::up(), bNormal)).zvec() * -1.f);
					if (this->m_PosBufOverRideFlag) {
						//一旦その場で地形判定
						this->m_move.pos = this->m_PosBufOverRide;
						GetObj().SetMatrix(this->m_move.MatIn());
					}
					Easing(&this->m_BodyNormal, ((GetObj().frame(this->m_VecData->Get_square(0)) - GetObj().frame(this->m_VecData->Get_square(3))).cross(GetObj().frame(this->m_VecData->Get_square(1)) - GetObj().frame(this->m_VecData->Get_square(2)))).Norm(), 0.8f, EasingType::OutExpo);
				}
				//地面判定
				{
					auto pos_ytmp = this->m_move.pos.y();
					float hight_t = 0.f;
					int cnt_t = 0;
					//履帯
					for (auto& f : this->m_b2Foot) {
						f.FirstExecute(&GetObj(), this->m_MapCol);
						for (const auto& t : f.Getb2downsideframe()) {
							if (t.colres.HitFlag == TRUE) {
								hight_t += t.colres.HitPosition.y;
								cnt_t++;
							}
						}
					}
					int cnt_sq = cnt_t;
					for (const auto& s : this->m_VecData->Get_square()) {
						auto p_t = GetObj().frame(s);
						auto ColResGround = this->m_MapCol->CollCheck_Line(p_t + (VECTOR_ref::up() * 3.f*Scale_Rate), p_t + (VECTOR_ref::up() * -0.5f*Scale_Rate));
						if (ColResGround.HitFlag == TRUE) {
							hight_t += ColResGround.HitPosition.y;
							cnt_t++;
						}
					}
					if (cnt_t > 0) {
						Easing(&pos_ytmp, (hight_t / cnt_t), 0.9f, EasingType::OutExpo);
					}
					this->m_move.pos.y(pos_ytmp);
					//地面or水面にいるかどうか
					if ((cnt_t > 0) || isfloat) {
						//前進後退
						{
							const auto spdold = this->m_speed_add + this->m_speed_sub;
							this->m_speed_add = (this->m_key[2]) ? std::min(this->m_speed_add + (0.15f / 3.6f) * (60.f / FPS), (this->m_VecData->GetMaxFrontSpeed() / 3.6f)) : std::max(this->m_speed_add - (0.7f / 3.6f) * (60.f / FPS), 0.f);
							this->m_speed_sub = (this->m_key[3]) ? std::max(this->m_speed_sub - (0.15f / 3.6f) * (60.f / FPS), (this->m_VecData->GetMaxBackSpeed() / 3.6f)) : std::min(this->m_speed_sub + (0.7f / 3.6f) * (60.f / FPS), 0.f);
							this->m_speed = (spdold + ((this->m_speed_add + this->m_speed_sub) - spdold) * 0.1f) / FPS;
							auto vec = this->m_move.mat.zvec() * -1.f * (this->m_speed*Scale_Rate);
							this->m_move.vec.x(vec.x());
							if ((cnt_t - cnt_sq) >= 3) {
								this->m_move.vec.y(vec.y());
							}
							else {
								this->m_move.vec.yadd(M_GR / (FPS * FPS));
							}
							this->m_move.vec.z(vec.z());
						}
						//旋回
						{
							const auto radold = this->m_radAdd;
							this->m_yradadd_left = (this->m_key[4]) ? std::max(this->m_yradadd_left - deg2rad(3.5f * (60.f / FPS)), deg2rad(-this->m_VecData->GetMaxBodyRad())) : std::min(this->m_yradadd_left + deg2rad(2.1f * (60.f / FPS)), 0.f);
							this->m_yradadd_right = (this->m_key[5]) ? std::min(this->m_yradadd_right + deg2rad(3.5f * (60.f / FPS)), deg2rad(this->m_VecData->GetMaxBodyRad())) : std::max(this->m_yradadd_right - deg2rad(2.1f * (60.f / FPS)), 0.f);
							this->m_radAdd.y((this->m_yradadd_left + this->m_yradadd_right) / FPS);
							//慣性
							this->m_radAdd.x(deg2rad(-(this->m_speed / (60.f / FPS)) / (0.1f / 3.6f) * 50.f));
							this->m_radAdd.z(deg2rad(-this->m_radAdd.y() / (deg2rad(5.f) / FPS) * 5.f));
							Easing(&this->m_Tilt, VECTOR_ref::vget(std::clamp(this->m_radAdd.x() - radold.x(), deg2rad(-30.f), deg2rad(30.f)), 0.f, std::clamp(this->m_radAdd.z() - radold.z(), deg2rad(-15.f), deg2rad(15.f))), 0.95f, EasingType::OutExpo);
							this->m_move.mat *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), -this->m_Tilt.x()) * MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_Tilt.z());
						}
						//
					}
					else {
						this->m_move.vec.yadd(M_GR / (FPS * FPS));
					}
				}
				//射撃反動
				for (auto& cg : this->m_Gun) {
					cg.FireReaction(&this->m_move.mat);
					//射撃
					if (cg.Execute(this->m_key[(&cg == &this->m_Gun.front()) ? 0 : 1], this->m_CharaType == CharaTypeID::Mine)) {
						SE->Get((int)SoundEnum::Tank_Shot).Play_3D(cg.GetShotSound(), this->m_move.pos, 250.f*Scale_Rate);							//サウンド
						Effect_UseControl::Set_Effect(Effect::ef_fire, GetObj().frame(cg.GetGunMuzzleFrameID()), GetObj().GetFrameLocalWorldMatrix(cg.GetGunTrunnionFrameID()).zvec() * -1.f, cg.GetCaliberSize() / 0.1f * Scale_Rate);				//銃発砲エフェクトのセット
					}
				}
				//移動
				if (this->m_PosBufOverRideFlag) {
					this->m_move.pos = this->m_PosBufOverRide;
					this->m_move.vec = this->m_VecBufOverRide;
				}
				else {
					this->m_move.pos += this->m_move.vec;
					//浮く
					if (this->m_VecData->GetIsFloat()) {
						this->m_move.pos.y(std::max(this->m_move.pos.y(), -this->m_VecData->GetDownInWater()));
					}
				}
				//転輪
				this->m_wheel_Left += (-(this->m_speed * 2.f - this->m_radAdd.y() * 5.f));
				this->m_wheel_Right += (-(this->m_speed * 2.f + this->m_radAdd.y() * 5.f));
			}
			//SetMat指示更新														//
			void			ExecuteMatrix(void) noexcept {
				auto OldPos = this->m_move.pos;
				//戦車座標反映
				this->m_move.mat *= MATRIX_ref::RotY(-this->m_b2mine.Rad() - Get_body_yrad());
				this->m_move.pos = VECTOR_ref::vget(this->m_b2mine.Pos().x, this->m_move.pos.y(), this->m_b2mine.Pos().y);

				float spdrec = this->m_spd_buf;
				Easing(&this->m_spd_buf, this->m_b2mine.Speed() * ((this->m_spd_buf > 0) ? 1.f : -1.f), 0.99f, EasingType::OutExpo);
				this->m_speed = this->m_spd_buf - spdrec;

				//転輪
				b2Vec2 Gravity2D = b2Vec2((M_GR / FPS / 2.f) * (this->m_move.mat.zvec().dot(VECTOR_ref::up())), (M_GR / FPS / 2.f) * (this->m_move.mat.yvec().dot(VECTOR_ref::up())));
				for (auto& f : this->m_b2Foot) {
					f.LateExecute(&f == &this->m_b2Foot.front(), this->m_VecData, &GetObj(), Gravity2D, (&f == &this->m_b2Foot.front()) ? this->m_wheel_Left : this->m_wheel_Right, this->m_speed - this->m_spd_rec);
				}
				UpdateMove();
				this->m_add_vec_real = this->m_move.pos - OldPos;
			}
		public: //コンストラクタ、デストラクタ
			VehicleClass(void) noexcept { this->m_objType = ObjType::Vehicle; }
			~VehicleClass(void) noexcept {}
		public: //継承
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_IsDraw = true;
			}
			void			FirstExecute(void) noexcept override {
				auto SE = SoundPool::Instance();
				//初回のみ更新する内容
				if (this->m_IsFirstLoop) {
					this->m_view_override = false;
				}
				ExecuteSavePrev();			//以前の状態保持
				//エンジン音
				{
					if (this->m_engine_time == 0.f) {
						SE->Get((int)SoundEnum::Tank_engine).Play_3D(0, this->m_move.pos, 50.f*Scale_Rate, 64);//, DX_PLAYTYPE_LOOP
						this->m_engine_time = 1.f;
					}
					else {
						this->m_engine_time -= 1.f / FPS;
						if (this->m_engine_time <= 0.f) {
							this->m_engine_time = 0.f;
						}
					}
				}

				ExecuteInput();				//操作
				ExecuteFrame();				//フレーム操作
				ExecuteMove();				//移動操作
				//戦車壁判定
				if (this->m_PosBufOverRideFlag) {
					this->m_b2mine.SetTransform(b2Vec2(this->m_move.pos.x(), this->m_move.pos.z()), Get_body_yrad());
				}
				else {
					this->m_b2mine.Update(this->m_move.vec, this->m_radAdd.y());
				}
				//弾の更新
				for (auto& cg : this->m_Gun) {
					for (auto& a : cg.GetBullet()) {
						a.Execute();
						if (a.IsActive()) {
							//AmmoObj
							MV1_COLL_RESULT_POLY ColResGround = a.ColCheckGround();
							bool is_HitAll = false;
							for (auto& tgt : *this->m_ALL_v) {
								if (tgt->m_MyID == this->m_MyID) { continue; }
								auto res = tgt->CheckAmmoHit(&a, this->m_move.pos);
								is_HitAll |= res.first;
								if (res.second) { break; }
							}
							if (ColResGround.HitFlag == TRUE && !is_HitAll) {
								a.HitGround();
								Effect_UseControl::Set_FootEffect(a.GetMove().pos, ColResGround.Normal, 0.05f / 0.1f * Scale_Rate);
								hit_obj_p.Set(a.GetCaliberSize() * Scale_Rate, a.GetMove().pos, ColResGround.Normal, a.GetMove().vec);	//弾痕
							}
						}
					}

				}
				this->m_PosBufOverRideFlag = false;
			}
			void			LateExecute(void) noexcept override {
				ExecuteMatrix();			//SetMat指示//0.03ms
				this->m_Hit_active.Update(this->m_move);
				Effect_UseControl::Update_Effect();
			}
			void			Draw(void) noexcept override {
				if (!is_ADS()) {
					MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_wheel_Left * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
					GetObj().DrawMesh(0);
					MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_wheel_Right * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
					GetObj().DrawMesh(1);
					MV1ResetFrameTextureAddressTransform(GetObj().get(), 0);
					for (int i = 2; i < GetObj().mesh_num(); i++) {
						GetObj().DrawMesh(i);
					}
					//this->m_col.DrawModel();
					this->m_Hit_active.Draw();
				}
				//if (!is_ADS())
				if (this->m_AimingDistance > 0)
				{
					SetUseLighting(FALSE);
					{
						auto StartPos = GetGunMuzzlePos();
						auto EndPos = StartPos + GetGunMuzzleVec() * this->m_AimingDistance;
						float laser_siz = 0.01f;
						DrawCapsule_3D(StartPos, EndPos, laser_siz*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
						DrawSphere_3D(EndPos, std::clamp(powf((EndPos - GetCameraPosition()).size() + 0.5f, 2) * 0.2f + 8.f, 0.1f, 5.f)*laser_siz*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
					}
					SetUseLighting(TRUE);
				}
				//
				for (auto& cg : this->m_Gun) {
					cg.Draw_ammo();
				}
			}
			void			Draw_Hit_UI(GraphHandle& hit_Graph) noexcept {
				for (auto& cg : this->m_Gun) {
					cg.Draw_Hit_UI(hit_Graph);
				}
			}
			void			Dispose(void) noexcept override {
				ObjectBaseClass::Dispose();
				Effect_UseControl::Dispose_Effect();
				this->m_b2mine.Dispose();						/*Box2D*/
				for (auto& f : this->m_b2Foot) {
					f.Dispose();
				}
				this->m_wheel_frameYpos.clear();
				for (auto& cg : this->m_Gun) {
					cg.Dispose();
				}
				this->m_Gun.clear();
				this->m_Hit_active.Dispose();
				this->m_DamageControl.Dispose();
				this->m_HitControl.Reset();
			}
		};
	};
};
