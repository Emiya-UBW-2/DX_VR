#pragma once
#include	"../../Header.hpp"

#include "FallObj.hpp"
#include "ModData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		class FallControl {
		private:
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
			void		Dispose() noexcept {
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
		private://キャラパラメーター
			std::array<std::pair<Vector3DX,float>, 16>		m_Line;
			int								m_LineSel = 0;
			float							m_LinePer{ 0.f };
		private:
		public://ゲッター
			void			AddMuzzleSmokePower() noexcept { m_LinePer = std::clamp(m_LinePer + 0.1f, 0.f, 1.f); }
		protected:
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
			void		DrawMuzzleSmoke() noexcept {
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
			std::array<SharedObj, (int)GunSlot::Max>	m_Parts_Ptr{ nullptr };
			std::shared_ptr<ModDataClass>		m_ModDataClass;
		public:
			auto& GetModData() noexcept { return this->m_ModDataClass; }
			const auto& GetModData() const noexcept { return this->m_ModDataClass; }
		protected:
			void			InitModSlotControl(const std::string& PilePath) noexcept {
				m_ModDataClass = *ModDataManager::Instance()->AddData(PilePath);
			}
			void			DisposeModSlotControl(void) noexcept {
				for (int loop = 0; loop < (int)GunSlot::Max; loop++) {
					RemoveMod((GunSlot)loop);
				}
				m_ModDataClass.reset();
			}
		public:
			const SharedObj& SetMod(GunSlot Slot, int ID, const SharedObj& BaseModel) noexcept;
			void			RemoveMod(GunSlot Slot) noexcept;
		private:
			const bool	IsEffectParts(GunSlot Slot, GunFrame frame) const noexcept;
		public:
			const auto& GetPartsPtr(GunSlot Slot) const noexcept { return this->m_Parts_Ptr[(int)Slot]; }
			const bool	HasFrameBySlot(GunFrame frame) const noexcept;
			const bool	GetPartsFrameLocalMatBySlot(GunFrame frame, Matrix4x4DX* pRet) const noexcept;
			const bool	GetPartsFrameWorldMat(GunFrame frame, Matrix4x4DX* pRet) const noexcept;

			void		ResetPartsFrameLocalMat(GunFrame frame) noexcept;
			void		SetPartsFrameLocalMat(GunFrame frame, const Matrix4x4DX& value) noexcept;
			void		GetChildPartsList(std::vector<const SharedObj*>* Ret) const noexcept;
			void		SetActiveBySlot(bool value) noexcept;
		public:
			void		UpdatePartsAnim(MV1& pParent);
			void		UpdatePartsMove(const Matrix4x4DX& pMat, GunSlot Slot);
		};

		class AimPointControl {
			int Reticle_xpos = 0;
			int Reticle_ypos = 0;
		public:
			AimPointControl() {}
			~AimPointControl() {}
		public:
			void UpdateAimPointControl(const Vector3DX& ReticlePos) noexcept {
				Vector3DX ReticlePosBuf = ConvWorldPosToScreenPos(ReticlePos.get());
				if (0.f < ReticlePosBuf.z && ReticlePosBuf.z < 1.f) {
					auto* WindowSizeParts = WindowSizeControl::Instance();
					Reticle_xpos = static_cast<int>(ReticlePosBuf.x * 1980 / WindowSizeParts->GetScreenY(1980));
					Reticle_ypos = static_cast<int>(ReticlePosBuf.y * 1080 / WindowSizeParts->GetScreenY(1080));
				}
			}
		public:
			const auto& GetAimXPos(void) const noexcept { return this->Reticle_xpos; }
			const auto& GetAimYPos(void) const noexcept { return this->Reticle_ypos; }
		private:
		};

		class ReticleControl {
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;
			float Lens_xpos = 0;
			float Lens_ypos = 0;
			float LensSize = 10000;
		public:
			ReticleControl() {}
			~ReticleControl() {}
		public:
			void UpdateReticleControl(const Vector3DX& LensPos, const Vector3DX& LensPos2, const Vector3DX& ReticlePos) noexcept {
				Vector3DX LensPosBuf = ConvWorldPosToScreenPos(LensPos.get());
				if (!(0.f < LensPosBuf.z && LensPosBuf.z < 1.f)) {
					return;
				}
				Lens_xpos = LensPosBuf.x;
				Lens_ypos = LensPosBuf.y;
				Vector3DX LensSizeBuf = ConvWorldPosToScreenPos(LensPos2.get());
				if (0.f < LensSizeBuf.z && LensSizeBuf.z < 1.f) {
					LensSize = std::hypotf(Lens_xpos - LensSizeBuf.x, Lens_ypos - LensSizeBuf.y);
				}
				Vector3DX ReticlePosBuf = ConvWorldPosToScreenPos(ReticlePos.get());
				if (0.f < ReticlePosBuf.z && ReticlePosBuf.z < 1.f) {
					Reticle_xpos = ReticlePosBuf.x;
					Reticle_ypos = ReticlePosBuf.y;
					Reticle_on = (LensSize > std::hypotf(Lens_xpos - Reticle_xpos, Lens_ypos - Reticle_ypos));
				}
			}
		public:
			const auto& IsActiveReticle(void) const noexcept { return this->Reticle_on; }
			const auto& GetReticleXPos(void) const noexcept { return this->Reticle_xpos; }
			const auto& GetReticleYPos(void) const noexcept { return this->Reticle_ypos; }
			const auto& GetLensXPos(void) const noexcept { return this->Lens_xpos; }
			const auto& GetLensYPos(void) const noexcept { return this->Lens_ypos; }
			const auto& GetLensSize(void) const noexcept { return this->LensSize; }
		private:
		};
	};
};
