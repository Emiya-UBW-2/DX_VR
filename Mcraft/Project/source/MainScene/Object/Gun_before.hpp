#pragma once
#include	"../../Header.hpp"

#include "FallObj.hpp"
#include "ModData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		class ArmMovePerClass {
			float												m_ArmPer{ 0.f };
			bool												m_Armon{ false };
		public:
			void Init(bool isOn)noexcept {
				m_Armon = isOn;
				m_ArmPer = isOn ? 1.f : 0.f;
			}
			void Update(bool isOn, float OnOver = 0.2f, float OffOver = 0.2f, float UpPer = 0.8f, float DownPer = 0.8f) noexcept {
				if (isOn) {
					if (m_Armon) {
						Easing(&this->m_ArmPer, 1.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&this->m_ArmPer, 1.f + OnOver, UpPer, EasingType::OutExpo);
						if (this->m_ArmPer >= 1.f + OnOver / 2.f) {
							m_Armon = true;
						}
					}
				}
				else {
					if (!m_Armon) {
						Easing(&this->m_ArmPer, 0.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&this->m_ArmPer, 0.f - OffOver, DownPer, EasingType::OutExpo);
						if (this->m_ArmPer <= 0.f - OffOver / 2.f) {
							m_Armon = false;
						}
					}
				}
			}
		public:
			const auto& Per(void) const noexcept { return m_ArmPer; }
		};
		//
		class AutoAimControl {
		private:
			int													m_AutoAim{ -1 };
			int													m_AutoAimPoint{ -1 };
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
				*ptmp_gunmat = Lerp(*ptmp_gunmat, (*ptmp_gunmat) * Matrix3x3DX::RotVec2(ptmp_gunmat->zvec() * -1.f, m_AutoAimVec), m_AutoAimPer);
			}
		public:
			void Update(bool isActive, PlayerID MyPlayerID, const Vector3DX& EyePos, const Vector3DX& AimVector, float Radian) noexcept;
		};
		//
		class FallControl {
		private:
		public:
			std::vector<std::shared_ptr<FallObjClass>>	m_Ptr;
			int											m_Now{ 0 };
		public:
			void		Init(const std::string& pPath, int count) {
				auto* ObjMngr = ObjectManager::Instance();
				m_Ptr.resize(count);
				for (auto& c : m_Ptr) {
					c = std::make_shared<FallObjClass>();
					ObjMngr->AddObject(c);
					ObjMngr->LoadModel(c, c, pPath.c_str());
					c->Init();
				}
			}
			void		SetFall(const Vector3DX& pPos, const Matrix3x3DX& pMat, const Vector3DX& pVec, float time, SoundEnum sound, bool IsGrenade) {
				this->m_Ptr[this->m_Now]->SetFall(pPos, pMat, pVec, time, sound, IsGrenade);
				++this->m_Now %= this->m_Ptr.size();
			}
			void		Dispose(void) noexcept {
				auto* ObjMngr = ObjectManager::Instance();
				for (auto& c : m_Ptr) {
					ObjMngr->DelObj((SharedObj*)&c);
					c.reset();
				}
				m_Ptr.clear();
			}
		};
		//
		class MuzzleSmokeControl {
		private:
			std::array<std::pair<Vector3DX,float>, 16>		m_Line;
			int								m_LineSel = 0;
			float							m_LinePer{ 0.f };
		public://ゲッター
			void			AddMuzzleSmokePower(void) noexcept { m_LinePer = std::clamp(m_LinePer + 0.1f, 0.f, 1.f); }
		public:
			void		InitMuzzleSmoke(const Vector3DX& pPos) {
				for (auto& l : this->m_Line) {
					l.first = pPos;
					l.second = 1.f;
				}
			}
			void		ExecuteMuzzleSmoke(const Vector3DX& pPos, bool IsAddSmoke) {
				auto* DXLib_refParts = DXLib_ref::Instance();
				for (auto& l : this->m_Line) {
					float Per = 0.1f * l.second;
					l.first += Vector3DX::vget(GetRandf(Per), 0.8f + GetRandf(Per), GetRandf(Per)) * Scale3DRate * DXLib_refParts->GetDeltaTime();
					Easing(&l.second, 0.f, 0.8f, EasingType::OutExpo);
				}
					this->m_Line[this->m_LineSel].first = pPos;
					this->m_Line[this->m_LineSel].second = IsAddSmoke ? 1.f : 0.f;
					++this->m_LineSel %= this->m_Line.size();
				m_LinePer = std::clamp(m_LinePer - DXLib_refParts->GetDeltaTime() / 30.f, 0.f, 1.f);
			}
			void		DrawMuzzleSmoke(void) noexcept {
				SetUseLighting(FALSE);
				SetUseHalfLambertLighting(FALSE);
				int max = static_cast<int>(this->m_Line.size());
				int min = 1 + static_cast<int>((1.f - m_LinePer) * (float)max);
				for (int i = max - 1; i >= min; i--) {
					int LS = (i + this->m_LineSel);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.f * (static_cast<float>(i - min) / max)));
					auto p1 = (LS - 1) % max;
					auto p2 = LS % max;
					if (CheckCameraViewClip_Box(
						this->m_Line[p1].first.get(),
						this->m_Line[p2].first.get()) == FALSE
						&& (this->m_Line[p2].second > 0.f)
						) {
						DrawCapsule3D(this->m_Line[p1].first.get(), this->m_Line[p2].first.get(), (0.00762f) * Scale3DRate * 1.f * (static_cast<float>(i - min) / max), 3,
							GetColor(216, 216, 216),
							GetColor(96, 96, 64),
							TRUE);
					}
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				SetUseLighting(TRUE);
				SetUseHalfLambertLighting(TRUE);
			}
		};

		class ModSlotControl {
		private:
			std::array<SharedObj, static_cast<int>(GunSlot::Max)>	m_Parts_Ptr{ nullptr };
			std::shared_ptr<ModDataClass>		m_ModDataClass;
		public:
			auto& GetModData(void) noexcept { return this->m_ModDataClass; }
			const auto& GetModData(void) const noexcept { return this->m_ModDataClass; }
		public:
			void		Init(const std::string& FilePath) noexcept { m_ModDataClass = *ModDataManager::Instance()->AddData(FilePath); }
			void		Dispose(void) noexcept {
				for (int loop = 0; loop < static_cast<int>(GunSlot::Max); loop++) {
					RemoveMod((GunSlot)loop);
				}
				m_ModDataClass.reset();
			}
		public:
			const SharedObj& SetMod(GunSlot Slot, int ID, const SharedObj& BaseModel) noexcept;
			void		RemoveMod(GunSlot Slot) noexcept;
		private:
			const bool	IsEffectParts(const SharedObj& SlotParts, GunFrame frame) const noexcept;
		public:
			void		CalcAnyBySlot(const std::function<void(const SharedObj&)>& Doing) const noexcept;
			const auto& GetPartsPtr(GunSlot Slot) const noexcept { return this->m_Parts_Ptr[static_cast<int>(Slot)]; }
			const SharedObj* HasFrameBySlot(GunFrame frame) const noexcept;
			void		UpdatePartsAnim(const MV1& pParent);
			void		UpdatePartsMove(const Matrix4x4DX& pMat, GunSlot Slot);
		};

		class ScreenPosition {
			int m_XPos{};
			int m_YPos{};
			int m_XScreenPos{};
			int m_YScreenPos{};
		public:
			ScreenPosition(void) noexcept {}
			~ScreenPosition(void) noexcept {}
		public:
			const int XPos(void) const noexcept { return this->m_XPos; }
			const int YPos(void) const noexcept { return this->m_YPos; }
			const int XScreenPos(void) const noexcept { return this->m_XScreenPos; }
			const int YScreenPos(void) const noexcept { return this->m_YScreenPos; }
		public:
			bool Calc(const Vector3DX& Pos3D) noexcept {
				Vector3DX Pos = ConvWorldPosToScreenPos(Pos3D.get());
				if (0.f < Pos.z && Pos.z < 1.f) {
					auto* WindowSizeParts = WindowSizeControl::Instance();
					m_XPos = static_cast<int>(Pos.x);
					m_YPos = static_cast<int>(Pos.y);
					m_XScreenPos = m_XPos * 1980 / WindowSizeParts->GetScreenY(1980);
					m_YScreenPos = m_YPos * 1080 / WindowSizeParts->GetScreenY(1080);
					return true;
				}
				return false;
			}
		};
	};
};
