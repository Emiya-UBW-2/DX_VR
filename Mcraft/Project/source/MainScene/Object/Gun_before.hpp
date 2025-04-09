#pragma once
#include	"../../Header.hpp"

#include	"FallObj.hpp"
#include	"ModData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		class ArmMovePerClass {
			float												m_ArmPer{ 0.f };
			bool												m_Armon{ false };
		public:
			void Init(bool isOn)noexcept {
				this->m_Armon = isOn;
				this->m_ArmPer = isOn ? 1.f : 0.f;
			}
			void Update(bool isOn, float OnOver = 0.2f, float OffOver = 0.2f, float UpPer = 0.8f, float DownPer = 0.8f) noexcept {
				if (isOn) {
					if (this->m_Armon) {
						Easing(&this->m_ArmPer, 1.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&this->m_ArmPer, 1.f + OnOver, UpPer, EasingType::OutExpo);
						if (this->m_ArmPer >= 1.f + OnOver / 2.f) {
							this->m_Armon = true;
						}
					}
				}
				else {
					if (!this->m_Armon) {
						Easing(&this->m_ArmPer, 0.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&this->m_ArmPer, 0.f - OffOver, DownPer, EasingType::OutExpo);
						if (this->m_ArmPer <= 0.f - OffOver / 2.f) {
							this->m_Armon = false;
						}
					}
				}
			}
		public:
			const auto& Per(void) const noexcept { return this->m_ArmPer; }
		};
		//
		class AutoAimControl {
		private:
			int													m_AutoAim{ InvalidID };
			int													m_AutoAimPoint{ InvalidID };
			float												m_AutoAimTimer{ 0.f };
			Vector3DX											m_AutoAimVec{};
			bool												m_AutoAimActive{};
			float												m_AutoAimPer{ 0.f };
		public:
			AutoAimControl(void) noexcept {}
			~AutoAimControl(void) noexcept {}
		public://ゲッター
			const auto& GetAutoAimID(void) const noexcept { return this->m_AutoAim; }
			const auto		GetAutoAimActive(void) const noexcept { return this->m_AutoAimActive; }
		public:
			void CalcAutoAimMat(Matrix3x3DX* ptmp_gunmat) const noexcept {
				*ptmp_gunmat = Lerp(*ptmp_gunmat, (*ptmp_gunmat) * Matrix3x3DX::RotVec2(ptmp_gunmat->zvec() * -1.f, this->m_AutoAimVec), this->m_AutoAimPer);
			}
		public:
			void Update(bool isActive, PlayerID MyPlayerID, const Vector3DX& EyePos, const Vector3DX& AimVector, float Radian) noexcept;
		};
		//
		class FallControl {
		private:
			std::vector<std::shared_ptr<FallObjClass>>	m_Ptr;
			int											m_Now{ 0 };
		public:
			const auto& GetPtrList(void) const noexcept { return this->m_Ptr; }
		public:
			void		Init(const std::string& pPath, int count) {
				auto* ObjMngr = ObjectManager::Instance();
				this->m_Ptr.resize(count);
				for (auto& ptr : this->m_Ptr) {
					ptr = std::make_shared<FallObjClass>();
					ObjMngr->AddObject(ptr);
					ObjMngr->LoadModel(ptr, ptr, pPath.c_str());
					ptr->Init();
				}
			}
			void		SetFall(const Vector3DX& pPos, const Matrix3x3DX& pMat, const Vector3DX& pVec, float time, FallObjectType Type) {
				this->m_Ptr[this->m_Now]->SetFall(pPos, pMat, pVec, time, Type);
				++this->m_Now %= this->m_Ptr.size();
			}
			void		Dispose(void) noexcept {
				auto* ObjMngr = ObjectManager::Instance();
				for (auto& ptr : this->m_Ptr) {
					ObjMngr->DelObj((SharedObj*)&ptr);
					ptr.reset();
				}
				this->m_Ptr.clear();
			}
		};
		//
		class MuzzleSmokeControl {
		private:
			std::array<std::pair<Vector3DX,float>, 16>		m_Line;
			int												m_LineSel = 0;
			float											m_LinePer{ 0.f };
		public://ゲッター
			void			AddMuzzleSmokePower(void) noexcept { this->m_LinePer = std::clamp(this->m_LinePer + 0.1f, 0.f, 1.f); }
		public:
			void		InitMuzzleSmoke(const Vector3DX& pPos) {
				for (auto& line : this->m_Line) {
					line.first = pPos;
					line.second = 1.f;
				}
			}
			void		ExecuteMuzzleSmoke(const Vector3DX& pPos, bool IsAddSmoke) {
				auto* DXLib_refParts = DXLib_ref::Instance();
				for (auto& line : this->m_Line) {
					float Per = 0.1f * line.second;
					line.first += Vector3DX::vget(GetRandf(Per), 0.8f + GetRandf(Per), GetRandf(Per)) * Scale3DRate * DXLib_refParts->GetDeltaTime();
					Easing(&line.second, 0.f, 0.8f, EasingType::OutExpo);
				}
				this->m_Line[this->m_LineSel].first = pPos;
				this->m_Line[this->m_LineSel].second = IsAddSmoke ? 1.f : 0.f;
				++this->m_LineSel %= this->m_Line.size();
				this->m_LinePer = std::clamp(this->m_LinePer - DXLib_refParts->GetDeltaTime() / 30.f, 0.f, 1.f);
			}
			void		DrawMuzzleSmoke(void) noexcept {
				SetUseLighting(false);
				SetUseHalfLambertLighting(false);
				int max = static_cast<int>(this->m_Line.size());
				int min = 1 + static_cast<int>((1.f - this->m_LinePer) * static_cast<float>(max));
				for (int loop = max - 1; loop >= min; --loop) {
					int LS = (loop + this->m_LineSel);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.f * (static_cast<float>(loop - min) / max)));
					auto p1 = (LS - 1) % max;
					auto p2 = LS % max;
					if (CheckCameraViewClip_Box(
						this->m_Line[p1].first.get(),
						this->m_Line[p2].first.get()) == false
						&& (this->m_Line[p2].second > 0.f)
						) {
						DrawCapsule3D(this->m_Line[p1].first.get(), this->m_Line[p2].first.get(), (0.00762f) * Scale3DRate * 1.f * (static_cast<float>(loop - min) / max), 3,
							GetColor(216, 216, 216),
							GetColor(96, 96, 64),
							true);
					}
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				SetUseLighting(true);
				SetUseHalfLambertLighting(true);
			}
		};
		//
		class GunPartsClass;

		using SharedGunParts = std::shared_ptr<GunPartsClass>;

		class GunPartsSlotControl {
		private:
			std::array<SharedGunParts, static_cast<int>(GunSlot::Max)>	m_Parts_Ptr{ nullptr };
			const std::shared_ptr<GunPartsDataClass>*					m_GunPartsDataClass{ nullptr };
		public:
			GunPartsSlotControl(const std::string& FilePath) noexcept {
				this->m_GunPartsDataClass = GunPartsDataManager::Instance()->AddData(FilePath);
			}
			~GunPartsSlotControl(void) noexcept {
				for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
					RemoveGunParts((GunSlot)loop);
				}
				this->m_GunPartsDataClass = nullptr;
			}
		private:
			const bool	IsEffectParts(const SharedGunParts& SlotParts, GunFrame frame) const noexcept;
			void		AddGunParts(GunSlot gunSlot, const char* FilePath, const SharedGunParts& NewObj, const SharedObj& BaseModel) noexcept;
			auto&		SetPartsPtr(GunSlot gunSlot) noexcept { return this->m_Parts_Ptr[static_cast<int>(gunSlot)]; }
		public:
			void		AttachGunParts(GunSlot gunSlot, int ID, const SharedObj& BaseModel) noexcept;
			void		RemoveGunParts(GunSlot gunSlot) noexcept;
		public:
			const auto& GetGunPartsData(void) const noexcept { return *this->m_GunPartsDataClass; }
			const auto& GetPartsPtr(GunSlot gunSlot) const noexcept { return this->m_Parts_Ptr[static_cast<int>(gunSlot)]; }
			void		CalcAnyBySlot(const std::function<void(const SharedGunParts&)>& Doing) const noexcept;
			const bool	GetFramePartsMat(GunFrame frame, Matrix4x4DX* pOutMat) const noexcept;
			void		UpdatePartsAnim(const MV1& pParent);
			void		UpdatePartsMove(const Matrix4x4DX& pMat, GunSlot gunSlot);
		};
	};
};
