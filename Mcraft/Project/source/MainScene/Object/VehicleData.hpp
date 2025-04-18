#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include "AmmoData.hpp"

namespace FPS_n2 {
	namespace Objects {
		//戦車砲データ
		class GunData {
		private:
			float								m_loadTime{};
			float								m_UpRadLimit{};
			float								m_DownRadLimit{};
			float								m_LeftRadLimit{};
			float								m_RightRadLimit{};

			std::array<int, 3>					m_AmmoSpecID{};
			std::array<frames, 3>				m_frame;
		public:
			const auto&		GetLoadTime(void) const noexcept { return this->m_loadTime; }
			const auto&		GetUpRadLimit(void) const noexcept { return this->m_UpRadLimit; }
			const auto&		GetDownRadLimit(void) const noexcept { return this->m_DownRadLimit; }
			const auto&		GetLeftRadLimit(void) const noexcept { return this->m_LeftRadLimit; }
			const auto&		GetRightRadLimit(void) const noexcept { return this->m_RightRadLimit; }
			const auto&		GetAmmoSpec(void) const noexcept { return this->m_AmmoSpecID; }
			const auto&		GetGunTurretFrame(void) const noexcept { return this->m_frame.at(0); }
			const auto&		GetGunTrunnionFrame(void) const noexcept { return this->m_frame.at(1); }
			const auto&		GetGunMuzzleFrame(void) const noexcept { return this->m_frame.at(2); }
		public: //コンストラクタ、デストラクタ
			GunData(void) noexcept { }
			~GunData(void) noexcept { }
		public:
			void			Set(int i, const MV1& obj) noexcept {
				this->m_frame[0].Set(i, obj);
				if (obj.GetFrameChildNum(GetGunTurretFrame().GetFrameID()) > 0) {
					this->m_frame[1].Set(obj.GetFrameChild(GetGunTurretFrame().GetFrameID(), 0), obj);
				}
				if (this->GetGunTrunnionFrame().GetFrameID() < 0) { return; }
				if (obj.GetFrameChildNum(this->GetGunTrunnionFrame().GetFrameID()) > 0) {
					this->m_frame[2].Set(obj.GetFrameChild(this->GetGunTrunnionFrame().GetFrameID(), 0), obj);
				}
			}
			void			SetData(FileStreamDX* File) noexcept {
				std::string ChildPath = "data/ammo/";
				ChildPath += FileStreamDX::getright(File->SeekLineAndGetStr());
				ChildPath += "/";
				this->m_AmmoSpecID.at(0) = Objects::AmmoDataManager::Instance()->Add(ChildPath.c_str());
				this->m_loadTime = std::stof(FileStreamDX::getright(File->SeekLineAndGetStr()));
				this->m_UpRadLimit = std::stof(FileStreamDX::getright(File->SeekLineAndGetStr()));
				this->m_DownRadLimit = std::stof(FileStreamDX::getright(File->SeekLineAndGetStr()));
				this->m_LeftRadLimit = std::stof(FileStreamDX::getright(File->SeekLineAndGetStr()));
				this->m_RightRadLimit = std::stof(FileStreamDX::getright(File->SeekLineAndGetStr()));
			}
		};
		//戦車データ
		class VhehicleData {
			MV1									m_DataObj;				//
			MV1									m_DataCol;				//
			float								m_MaxFrontSpeed{};		//前進速度(km/h)
			float								m_MaxBackSpeed{};		//後退速度(km/h)
			float								m_MaxBodyRad{};			//旋回速度(度/秒)
			float								m_MaxTurretRad{};		//砲塔駆動速度(度/秒)
			std::vector<GunData>				m_GunData;				//
			std::vector<frames>					m_WheelFrame;			//転輪
			std::vector<frames>					m_NoSpringWheelFrame;	//誘導輪回転
			std::vector<std::pair<int, float>>	m_ArmerMesh;			//装甲ID
			std::vector<int>					m_SpaceArmerMesh;		//装甲ID
			std::vector<int>					m_ModuleMesh;			//装甲ID
			std::array<int, 4>					m_SquareFrameID{};		//車輛の四辺
			std::array<std::vector<frames>, 2>	m_CrawlerFrame;			//履帯
		private:
			const auto		GetSide(bool isLeft, bool isFront) const noexcept {
				int ans = 0;
				float tmp = 0.0f;
				for (auto& f : this->m_WheelFrame) {
					if ((isLeft ? 1.0f : -1.0f) * f.GetFrameWorldPosition().pos().x >= 0) {
						ans = f.GetFrameID();
						tmp = f.GetFrameWorldPosition().pos().z;
						break;
					}
				}
				for (auto& f : this->m_WheelFrame) {
					if (ans != f.GetFrameID()) {
						if ((isLeft ? 1.0f : -1.0f) * f.GetFrameWorldPosition().pos().x >= 0) {
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
			const auto& GetMaxFrontSpeed(void) const noexcept { return this->m_MaxFrontSpeed; }
			const auto& GetMaxBackSpeed(void) const noexcept { return this->m_MaxBackSpeed; }
			const auto& GetMaxBodyRad(void) const noexcept { return this->m_MaxBodyRad; }
			const auto& GetMaxTurretRad(void) const noexcept { return this->m_MaxTurretRad; }
			const auto& GetGunData(void) const noexcept { return this->m_GunData; }
			const auto& GetWheelFrameList(void) const noexcept { return this->m_WheelFrame; }
			const auto& GetNoSpringWheelFrameList(void) const noexcept { return this->m_NoSpringWheelFrame; }
			const auto& GetArmerMeshIDList(void) const noexcept { return this->m_ArmerMesh; }
			const auto& GetSpaceArmerMeshIDList(void) const noexcept { return this->m_SpaceArmerMesh; }
			const auto& GetModuleMeshIDList(void) const noexcept { return this->m_ModuleMesh; }
			const auto& GetSquareFrameID(size_t ID_t)const noexcept { return this->m_SquareFrameID[ID_t]; }
			const auto& GetSquareFrameList(void) const noexcept { return this->m_SquareFrameID; }
			const auto& GetCrawlerFrameList(void) const noexcept { return this->m_CrawlerFrame; }
		public: //コンストラクタ、デストラクタ
			VhehicleData(const MV1& DataObj, const MV1&, const char* path) noexcept {
				//フレーム情報
				for (int i = 0; i < DataObj.GetFrameNum(); i++) {
					std::string p = DataObj.GetFrameName(i);
					if (p.find("転輪", 0) != std::string::npos) {
						this->m_WheelFrame.resize(this->m_WheelFrame.size() + 1);
						this->m_WheelFrame.back().Set(i, DataObj);
					}
					else if ((p.find("輪", 0) != std::string::npos) && (p.find("転輪", 0) == std::string::npos)) {
						this->m_NoSpringWheelFrame.resize(this->m_NoSpringWheelFrame.size() + 1);
						this->m_NoSpringWheelFrame.back().Set(i, DataObj);
					}
					else if (p.find("旋回", 0) != std::string::npos) {
						this->m_GunData.resize(this->m_GunData.size() + 1);
						this->m_GunData.back().Set(i, DataObj);
					}
					else if (p.find("履帯設置部", 0) != std::string::npos) { //2D物理
						this->m_CrawlerFrame[0].clear();
						this->m_CrawlerFrame[1].clear();
						for (int z = 0; z < DataObj.GetFrameChildNum(i); z++) {
							if (DataObj.GetFramePosition(i + 1 + z).x > 0) {
								this->m_CrawlerFrame[0].resize(this->m_CrawlerFrame[0].size() + 1);
								this->m_CrawlerFrame[0].back().Set(i + 1 + z, DataObj);
							}
							else {
								this->m_CrawlerFrame[1].resize(this->m_CrawlerFrame[1].size() + 1);
								this->m_CrawlerFrame[1].back().Set(i + 1 + z, DataObj);
							}
						}
					}
				}
				//メッシュ情報
				//TODO
				//4隅確定
				this->m_SquareFrameID[0] = GetSide(true, false);			//2		左後部0
				this->m_SquareFrameID[1] = GetSide(true, true);				//10	左前部1
				this->m_SquareFrameID[2] = GetSide(false, false);			//3		右後部2
				this->m_SquareFrameID[3] = GetSide(false, true);			//11	右前部3
				//data
				{
					std::string Path = path;
					FileStreamDX File((Path + "data.txt").c_str());
					this->m_MaxFrontSpeed = std::stof(FileStreamDX::getright(File.SeekLineAndGetStr()));
					this->m_MaxBackSpeed = std::stof(FileStreamDX::getright(File.SeekLineAndGetStr()));
					this->m_MaxBodyRad = std::stof(FileStreamDX::getright(File.SeekLineAndGetStr()));
					this->m_MaxTurretRad = deg2rad(std::stof(FileStreamDX::getright(File.SeekLineAndGetStr())));
					for (auto& g : this->m_GunData) {
						g.SetData(&File);
					}
					//TODO
				}
			}
			~VhehicleData(void) noexcept {
				this->m_GunData.clear();
				this->m_WheelFrame.clear();
				this->m_NoSpringWheelFrame.clear();
				this->m_ArmerMesh.clear();
				this->m_SpaceArmerMesh.clear();
				this->m_ModuleMesh.clear();
				for (auto& LR : this->m_CrawlerFrame) {
					LR.clear();
				}
			}
		};
		//砲
		class Guns {
		private:
			float							m_loadtimer{};			//装てんカウンター
			float							m_Recoil{};			//駐退
			float							m_React{};				//反動
			const GunData*					m_GunSpec{ nullptr };		//
			Vector3DX						m_ShotRadAdd;				//
			Vector2DX						m_ShotTargetRad;			//
		public:
		public:			//getter
			const auto& GetGunTurretFrame(void) const noexcept { return this->m_GunSpec->GetGunTurretFrame(); }
			const auto&	GetGunTrunnionFrame(void) const noexcept { return this->m_GunSpec->GetGunTrunnionFrame(); }
			const auto&	GetGunMuzzleFrame(void) const noexcept { return this->m_GunSpec->GetGunMuzzleFrame(); }
			const auto& GetAmmoSpec(void) const noexcept { return Objects::AmmoDataManager::Instance()->Get(this->m_GunSpec->GetAmmoSpec().at(0)); }
			const auto& GetShotTargetRad(void) const noexcept { return this->m_ShotTargetRad; }
			const auto& GetRecoil(void) const noexcept { return this->m_Recoil; }
			const auto& GetShotRadAdd(void) const noexcept { return this->m_ShotRadAdd; }
			//射撃可能
			bool		CanShot(void) const noexcept { return this->m_loadtimer == 0; }
		public:
			void		UpdateAim(const Vector3DX& NowVec, const Vector3DX& TargetVec, const std::unique_ptr<VhehicleData>& pData) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				//反映
				float a_hyp = std::hypotf(TargetVec.x, TargetVec.z);
				float z_hyp = std::hypotf(NowVec.x, NowVec.z);

				float cost = Vector3DX::Cross(NowVec, TargetVec).y / z_hyp;
				float sint = sqrtf(std::abs(1.0f - cost * cost));
				float view_YradAdd = std::atan2f(cost, sint);
				float view_XradAdd = std::atan2f(TargetVec.y, a_hyp) - std::atan2f(NowVec.y, z_hyp);

				this->m_ShotTargetRad.x += std::clamp(view_XradAdd / 5.0f * 60.f, -pData->GetMaxTurretRad(), pData->GetMaxTurretRad()) * DXLib_refParts->GetDeltaTime();
				this->m_ShotTargetRad.y += std::clamp(view_YradAdd / 5.0f * 60.f, -pData->GetMaxTurretRad(), pData->GetMaxTurretRad()) * DXLib_refParts->GetDeltaTime();
				//仰俯角制限
				if (this->m_GunSpec->GetLeftRadLimit() >= 0.f && this->m_GunSpec->GetRightRadLimit() >= 0.f) {
					this->m_ShotTargetRad.y = std::clamp(this->m_ShotTargetRad.y, deg2rad(this->m_GunSpec->GetLeftRadLimit()), deg2rad(this->m_GunSpec->GetRightRadLimit()));
				}
				this->m_ShotTargetRad.x = std::clamp(this->m_ShotTargetRad.x, deg2rad(this->m_GunSpec->GetDownRadLimit()), deg2rad(this->m_GunSpec->GetUpRadLimit()));
			}
			//射撃した
			void		Shot(void) noexcept {
				this->m_loadtimer = this->m_GunSpec->GetLoadTime();
				this->m_Recoil = 1.0f;
				this->m_React = std::clamp(this->m_React + GetAmmoSpec()->GetCaliber() * 10.0f, 0.0f, 3.0f);
			}
		public: //コンストラクタ、デストラクタ
			Guns(void) noexcept { }
			Guns(const Guns&) noexcept { }
		public:
			void		Init(const GunData* pResorce) noexcept {
				this->m_GunSpec = pResorce;
				this->m_ShotRadAdd = Vector3DX::zero();
			}
			void		Update(void) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				//射撃
				this->m_loadtimer = std::max(this->m_loadtimer - DXLib_refParts->GetDeltaTime(), 0.0f);
				this->m_Recoil = std::max(this->m_Recoil - DXLib_refParts->GetDeltaTime(), 0.0f);
				this->m_React = std::max(this->m_React - DXLib_refParts->GetDeltaTime(), 0.0f);
				auto RotateY = Matrix4x4DX::RotAxis(Vector3DX::up(), this->m_ShotTargetRad.y);
				Easing(&this->m_ShotRadAdd, RotateY.xvec() * deg2rad(this->m_React * GetAmmoSpec()->GetCaliber() * 50.0f), 0.85f, EasingType::OutExpo);
			}
			void		Dispose(void) noexcept {
				this->m_loadtimer = 0.0f;
				this->m_Recoil = 0.0f;
				this->m_React = 0.0f;
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
		//履帯
		class CrawlerFrameControl {
		private:
			frames					m_frame;
			float					m_HitHeight{ (std::numeric_limits<float>::max)() };
		public:
			CrawlerFrameControl(void) noexcept {}
			~CrawlerFrameControl(void) noexcept {}
		public://getter
			const auto		OnGround(void) const noexcept { return this->m_HitHeight != (std::numeric_limits<float>::max)(); }
			const auto&		GetHitHeight(void) const noexcept { return this->m_HitHeight; }
			const auto&		GetFrame(void) const noexcept { return this->m_frame; }
		public:
			void			Init(const frames& pFrame) noexcept { this->m_frame = pFrame; }
			//
			void			Update(bool ColRes, float Height) noexcept {
				this->m_HitHeight = (ColRes) ? Height : (std::numeric_limits<float>::max)();
			}
		};
	};
};
