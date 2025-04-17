#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include "AmmoData.hpp"

namespace FPS_n2 {
	namespace Objects {
		//戦車砲データ
		class GunData {
		private:
			std::string							m_name;
			float								m_loadTime{ 0.f };
			int									m_Shot_Sound = -1;
			int									m_Eject_Sound = -1;
			int									m_Reload_Sound = -1;
			float								m_UpRadLimit{ 0.f };
			float								m_DownRadLimit{ 0.f };
			std::array<int, 3>					m_AmmoSpecID{};
			std::array<frames, 3>				m_frame;
		public:
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		GetLoadTime(void) const noexcept { return this->m_loadTime; }
			const auto&		GetShotSound(void) const noexcept { return this->m_Shot_Sound; }
			const auto&		GetEjectSound(void) const noexcept { return this->m_Eject_Sound; }
			const auto&		GetReloadSound(void) const noexcept { return this->m_Reload_Sound; }
			const auto&		GetAmmoSpec(void) const noexcept { return this->m_AmmoSpecID; }
			const auto&		Get_frame(size_t pID)const noexcept { return this->m_frame[pID]; }
			const auto&		GetUpRadLimit(void) const noexcept { return this->m_UpRadLimit; }
			const auto&		GetDownRadLimit(void) const noexcept { return this->m_DownRadLimit; }
		public: //コンストラクタ、デストラクタ
			GunData(void) noexcept { }
			~GunData(void) noexcept { }
		public:
			void			SetFrame(const MV1& obj, int i) noexcept {
				this->m_frame[0].Set(i, obj);
				if (obj.GetFrameChildNum(this->m_frame[0].GetFrameID()) <= 0) {
					return;
				}
				int child_num = (int)obj.GetFrameChild(this->m_frame[0].GetFrameID(), 0);

				if (obj.GetFrameName(child_num).find("仰角", 0) != std::string::npos) {
					this->m_frame[1].Set(child_num, obj);
				}
				if (this->m_frame[1].GetFrameID() == -1) {
					return;
				}

				if (obj.GetFrameChildNum(this->m_frame[1].GetFrameID()) <= 0) {
					return;
				}
				int child2_num = (int)obj.GetFrameChild(this->m_frame[1].GetFrameID(), 0);
				{
					this->m_frame[2].Set(child2_num, obj);
				}
			}
			void			Set(FileStreamDX* File) noexcept {
				this->m_name = FileStreamDX::getright(File->SeekLineAndGetStr());
				this->m_loadTime = std::stof(FileStreamDX::getright(File->SeekLineAndGetStr()));
				this->m_Shot_Sound = std::stoi(FileStreamDX::getright(File->SeekLineAndGetStr()));//サウンド
				this->m_Eject_Sound = std::stoi(FileStreamDX::getright(File->SeekLineAndGetStr()));//サウンド
				this->m_Reload_Sound = std::stoi(FileStreamDX::getright(File->SeekLineAndGetStr()));//サウンド
				this->m_UpRadLimit = std::stof(FileStreamDX::getright(File->SeekLineAndGetStr()));
				this->m_DownRadLimit = std::stof(FileStreamDX::getright(File->SeekLineAndGetStr()));
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
			std::vector<GunData>				m_GunFrameData;					//
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
			const auto& GetName(void) const noexcept { return this->m_name; }
			const auto& GetMaxFrontSpeed(void) const noexcept { return this->m_MaxFrontSpeed; }
			const auto& GetMaxBackSpeed(void) const noexcept { return this->m_MaxBackSpeed; }
			const auto& GetMaxBodyRad(void) const noexcept { return this->m_MaxBodyRad; }
			const auto& GetMaxTurretRad(void) const noexcept { return this->m_MaxTurretRad; }
			const auto& Get_gunframe(void) const noexcept { return this->m_GunFrameData; }
			const auto& Get_wheelframe(void) const noexcept { return this->m_wheelframe; }
			const auto& Get_wheelframe_nospring(void) const noexcept { return this->m_wheelframe_nospring; }
			const auto& Get_armer_mesh(void) const noexcept { return this->m_armer_mesh; }
			const auto& Get_space_mesh(void) const noexcept { return this->m_space_mesh; }
			const auto& Get_module_mesh(void) const noexcept { return this->m_module_mesh; }
			const auto& Get_square(size_t ID_t)const noexcept { return this->m_square[ID_t]; }
			const auto& Get_square(void) const noexcept { return this->m_square; }
			const auto& Get_b2upsideframe(size_t ID_t)const noexcept { return this->m_b2upsideframe[ID_t]; }
			const auto& Get_b2upsideframe(void) const noexcept { return this->m_b2upsideframe; }
			const auto& Get_b2downsideframe(void) const noexcept { return this->m_downsideframe; }
		public: //コンストラクタ、デストラクタ
			VhehicleData(void) noexcept {}
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
				this->m_wheelframe = o.m_wheelframe;
				this->m_wheelframe_nospring = o.m_wheelframe_nospring;
				this->m_armer_mesh = o.m_armer_mesh;
				this->m_space_mesh = o.m_space_mesh;
				this->m_module_mesh = o.m_module_mesh;
				this->m_square = o.m_square;
				this->m_b2upsideframe = o.m_b2upsideframe;
				this->m_downsideframe = o.m_downsideframe;
			}
			~VhehicleData(void) noexcept {}
		public:
			//事前読み込み
			void		Set_Pre(const char* name) noexcept {
				this->m_name = name;
				MV1::Load("data/model/" + this->m_name + "/model_LOADCALC.mv1", &this->m_DataObj);//model.pmx//model_LOADCALC.mv1
				MV1::Load("data/model/" + this->m_name + "/col.mv1", &this->m_DataCol);
			}
			//メイン読み込み
			void			Set(void) noexcept {
				//固有
				this->m_DownInWater = 0.f;
				for (int i = 0; i < this->m_DataObj.GetMeshNum(); i++) {
					auto p = this->m_DataObj.GetMeshMaxPosition(i).y;
					if (this->m_DownInWater < p) {
						this->m_DownInWater = p;
					}
				}
				this->m_DownInWater /= 2.f;
				Vector3DX minpos, maxpos;							//
				for (int i = 0; i < this->m_DataObj.GetFrameNum(); i++) {
					std::string p = this->m_DataObj.GetFrameName(i);
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
						minpos = this->m_DataObj.GetFramePosition(i);
					}
					else if (p == "max") {
						maxpos = this->m_DataObj.GetFramePosition(i);
					}
					else if (p.find("２D物理", 0) != std::string::npos || p.find("2D物理", 0) != std::string::npos) { //2D物理
						this->m_b2upsideframe[0].clear();
						this->m_b2upsideframe[1].clear();
						for (int z = 0; z < this->m_DataObj.GetFrameChildNum(i); z++) {
							if (this->m_DataObj.GetFramePosition(i + 1 + z).x > 0) {
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
						for (int z = 0; z < this->m_DataObj.GetFrameChildNum(i); z++) {
							if (this->m_DataObj.GetFramePosition(i + 1 + z).x > 0) {
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
				//4隅確定
				this->m_square[0] = GetSide(true, false);			//2		左後部0
				this->m_square[1] = GetSide(true, true);			//10	左前部1
				this->m_square[2] = GetSide(false, false);			//3		右後部2
				this->m_square[3] = GetSide(false, true);			//11	右前部3
				//装甲

				//data
				{
					FileStreamDX File(("data/model/" + this->m_name + "/data.txt").c_str());
					this->m_MaxHP = (HitPoint)std::stoi(FileStreamDX::getright(File.SeekLineAndGetStr()));
					this->m_MaxFuel = std::stoi(FileStreamDX::getright(File.SeekLineAndGetStr()));
					this->m_IsFloat = (FileStreamDX::getright(File.SeekLineAndGetStr()) == "true");
					this->m_MaxFrontSpeed = std::stof(FileStreamDX::getright(File.SeekLineAndGetStr()));
					this->m_MaxBackSpeed = std::stof(FileStreamDX::getright(File.SeekLineAndGetStr()));
					this->m_MaxBodyRad = std::stof(FileStreamDX::getright(File.SeekLineAndGetStr()));
					this->m_MaxTurretRad = deg2rad(std::stof(FileStreamDX::getright(File.SeekLineAndGetStr())));
					{
						for (auto& g : this->m_GunFrameData) {
							g.Set(&File);
						}
					}
				}
				this->m_DataObj.Dispose();
				this->m_DataCol.Dispose();
			}
		};
		//砲
		class Guns {
		private:
			float							m_loadtimer{ 0 };			//装てんカウンター
			bool							m_reloadSEFlag{ true };		//
			float							m_Recoil{ 0.f };			//駐退
			float							m_React{ 0.f };				//反動
			const GunData*					m_GunSpec{ nullptr };		//
			Vector3DX						m_ShotRadAdd;				//
		public:			//getter
			const auto&	Getloadtime(void) const noexcept { return m_loadtimer; }
			const auto&	GetTotalloadtime(void) const noexcept { return this->m_GunSpec->GetLoadTime(); }
			const auto&	GetGunSpec(void) const noexcept { return this->m_GunSpec; }
			const int	GetAmmoSpecNum() const noexcept { return (int)this->m_GunSpec->GetAmmoSpec().size(); }
			const auto&	GetAmmoSpec(int select) const noexcept { return this->m_GunSpec->GetAmmoSpec()[select]; }
			const auto&	GetCaliberSize(void) const noexcept { return Objects::AmmoDataManager::Instance()->Get(GetAmmoSpec(0))->GetCaliber(); }
			const auto&	GetGunTrunnionFrameID(void) const noexcept { return this->m_GunSpec->Get_frame(1).GetFrameID(); }
			const auto&	GetGunMuzzleFrameID(void) const noexcept { return this->m_GunSpec->Get_frame(2).GetFrameID(); }
			//銃反動
			void		FireReaction(Matrix3x3DX* mat_t) const noexcept {
				(*mat_t) *= Matrix3x3DX::RotAxis(mat_t->xvec(), this->m_ShotRadAdd.x) * Matrix3x3DX::RotAxis(mat_t->zvec(), this->m_ShotRadAdd.z);
			}
		public:
			//角度指示
			void		ExecuteGunFrame(float pGunXrad, float pGunYrad, MV1* obj_body_t, MV1* col_body_t) noexcept {
				float yrad = pGunYrad;
				float xrad = std::clamp(pGunXrad, deg2rad(this->m_GunSpec->GetDownRadLimit()), deg2rad(this->m_GunSpec->GetUpRadLimit()));
				frames id;
				id = this->m_GunSpec->Get_frame(0);
				if (id.GetFrameID() >= 0) {
					obj_body_t->SetFrameLocalMatrix(id.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::up(), yrad) * Matrix4x4DX::Mtrans(id.GetFrameLocalPosition().pos()));
					col_body_t->SetFrameLocalMatrix(id.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::up(), yrad) * Matrix4x4DX::Mtrans(id.GetFrameLocalPosition().pos()));
				}
				id = this->m_GunSpec->Get_frame(1);
				if (id.GetFrameID() >= 0) {
					obj_body_t->SetFrameLocalMatrix(id.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::right(), xrad) * Matrix4x4DX::Mtrans(id.GetFrameLocalPosition().pos()));
					col_body_t->SetFrameLocalMatrix(id.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::right(), xrad) * Matrix4x4DX::Mtrans(id.GetFrameLocalPosition().pos()));
				}
				id = this->m_GunSpec->Get_frame(2);
				if (id.GetFrameID() >= 0) {
					obj_body_t->SetFrameLocalMatrix(id.GetFrameID(), Matrix4x4DX::Mtrans(Vector3DX::forward() * (this->m_Recoil * 0.5f * Scale3DRate)) * Matrix4x4DX::Mtrans(id.GetFrameLocalPosition().pos()));
					col_body_t->SetFrameLocalMatrix(id.GetFrameID(), Matrix4x4DX::Mtrans(Vector3DX::forward() * (this->m_Recoil * 0.5f * Scale3DRate)) * Matrix4x4DX::Mtrans(id.GetFrameLocalPosition().pos()));
				}
				Easing(&this->m_ShotRadAdd, Matrix4x4DX::RotAxis(Vector3DX::up(), yrad).xvec() * -1.f * deg2rad(-this->m_React * GetCaliberSize() * 50.f), 0.85f, EasingType::OutExpo);
			}
		public: //コンストラクタ、デストラクタ
			Guns(void) noexcept { }
			Guns(const Guns&) noexcept { }
		public:
			void		Init(const GunData* pResorce) noexcept {
				this->m_GunSpec = pResorce;
				this->m_ShotRadAdd.Set(0, 0, 0);
			}
			bool		Execute(bool key) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				auto* SE = SoundPool::Instance();
				bool isshot = (key && this->m_loadtimer == 0);
				//射撃
				if (isshot) {
					this->m_loadtimer = this->m_GunSpec->GetLoadTime();
					this->m_Recoil = 1.f;
					this->m_React = std::clamp(this->m_React + GetCaliberSize() * 10.f, 0.f, 3.f);
					this->m_reloadSEFlag = true;
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_Eject))->Play();
				}
				if (this->m_reloadSEFlag && this->m_loadtimer < 1.f) {
					this->m_reloadSEFlag = false;
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_Reload))->Play();
				}
				this->m_loadtimer = std::max(this->m_loadtimer - 1.f * DXLib_refParts->GetDeltaTime(), 0.f);
				this->m_Recoil = std::max(this->m_Recoil - 1.f * DXLib_refParts->GetDeltaTime(), 0.f);
				this->m_React = std::max(this->m_React - 1.f * DXLib_refParts->GetDeltaTime(), 0.f);
				return isshot;
			}
			void		Dispose(void) noexcept {
				this->m_loadtimer = 0.f;
				this->m_reloadSEFlag = true;
				this->m_Recoil = 0.f;
				this->m_React = 0.f;
				this->m_GunSpec = nullptr;
				this->m_ShotRadAdd = Vector3DX::zero();
			}
		};
		//命中関連
		class HitSortInfo {
			size_t					m_hitmesh{ SIZE_MAX };
			float					m_hitDistance{ -1 };
		public:
			const auto		operator<(const HitSortInfo& tgt) const noexcept { return this->m_hitDistance < tgt.m_hitDistance; }
			void			Set(size_t f_t, float s_t = (std::numeric_limits<float>::max)())noexcept {
				this->m_hitmesh = f_t;
				this->m_hitDistance = s_t;
			}
			const auto		GetHitMesh(void) const noexcept { return this->m_hitmesh; }
			const auto		IsHit(void) const noexcept { return (this->m_hitDistance != (std::numeric_limits<float>::max)()); }
		};
		//履帯BOX2D
		class FootWorld {
		private:
			class cat_frame {
			private:
				frames					m_frame;
				float					m_Res_y{ (std::numeric_limits<float>::max)() };
				//EffectControl::EffectS	m_gndsmkeffcs;
				float					m_gndsmksize{ 1.f };
			public:
				cat_frame(void) noexcept {}
				//~cat_frame(void) noexcept {}
			public://getter
				const auto&		GetColResult_Y(void) const noexcept { return this->m_Res_y; }
			public:
				void			Init(const frames& pFrame) noexcept {
					this->m_frame = pFrame;
					//this->m_gndsmkeffcs.SetLoop(EffectResource::Instance()->m_Sorce.back(), Vector3DX::vget(0, -1, 0), Vector3DX::vget(0, 0, 1), 0.1f*Scale3DRate);
					this->m_gndsmksize = 0.1f;
				}
				//
				void			FrameExecute(MV1* pTargetObj) noexcept;
				void			EffectExecute(MV1* pTargetObj, float pSpd) noexcept {
					if (this->m_Res_y != (std::numeric_limits<float>::max)()) {
						Easing(&this->m_gndsmksize, 0.2f + std::abs(pSpd) * 5.5f, 0.975f, EasingType::OutExpo);
					}
					else {
						this->m_gndsmksize = 0.2f;
					}
					//this->m_gndsmkeffcs.SetEffectPos(pTargetObj->GetFramePosition(this->m_frame.GetFrameID()) + pTargetObj->GetMatrix().yvec() * (-this->m_frame.GetFrameWorldPosition().pos().y));
					//this->m_gndsmkeffcs.SetEffectScale(std::clamp(this->m_gndsmksize, 0.2f, 1.5f)*Scale3DRate);
				}
				//
				void			Dispose(void) noexcept {
					//this->m_gndsmkeffcs.Dispose();
				}
			};
		private:
			std::vector<cat_frame>		m_downsideframe;			//履帯
		public:			//getter
			const auto&			Getdownsideframe(void) const noexcept { return this->m_downsideframe; }
		public:
			void			Init(bool IsLeft, const VhehicleData* pUseVeh) noexcept {
				//履帯
				{
					const auto& w = pUseVeh->Get_b2downsideframe()[((IsLeft) ? 0 : 1)];
					this->m_downsideframe.resize(w.size());
					for (auto& t : this->m_downsideframe) {
						t.Init(w[&t - &this->m_downsideframe.front()]);
					}
				}
			}
			//
			void			FirstExecute(MV1* pTargetObj) noexcept {
				for (auto& t : this->m_downsideframe) {
					t.FrameExecute(pTargetObj);
				}
			}
			void			LateExecute(MV1* pTargetObj, float pSpd) noexcept {
				//エフェクト更新
				for (auto& t : this->m_downsideframe) {
					t.EffectExecute(pTargetObj, pSpd);
				}
			}
			//
			void			Dispose(void) noexcept {
				for (auto& t : this->m_downsideframe) {
					t.Dispose();
				}
				this->m_downsideframe.clear();
			}
		};
	};
};
