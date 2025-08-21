#pragma once
#include	"../../Header.hpp"

#include	"ModData.hpp"
#include	"Gun_before.hpp"

namespace FPS_n2 {
	namespace Guns {
		class GunPartsObj : public BaseObject {
			std::unique_ptr<ModifySlot>								m_ModifySlot{};

			std::vector<int>													m_IsMeshDraw{};
		public:
			GunPartsObj(void) noexcept {}
			virtual ~GunPartsObj(void) noexcept {}
		public:
			const std::unique_ptr<ModifySlot>& GetModifySlot(void) const noexcept { return this->m_ModifySlot; }
			auto					GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			const Matrix4x4DX		GetFramePartsMat(GunFrame frame) const noexcept;

			void						SetIsDrawMesh(int meshID, bool IsDraw) noexcept {
				this->m_IsMeshDraw[meshID] = IsDraw ? TRUE : FALSE;
			}
		private:
			bool			IsDrawAllMesh(void) const noexcept {
				for (auto& isdraw : this->m_IsMeshDraw) {
					if (isdraw != TRUE) {
						return false;
					}
				}
				return true;
			}
			void			DrawCommon(void) const noexcept {
				if (!(GetMove().GetPos() - Camera3D::Instance()->GetMainCamera().GetCamPos()).IsRangeSmaller(10.0f * Scale3DRate)) { return; }
				if (IsDrawAllMesh()) {
					GetObj().DrawModel();
				}
				else {
					for (auto& isdraw : this->m_IsMeshDraw) {
						if (isdraw == TRUE) {
							int index = static_cast<int>(&isdraw - &this->m_IsMeshDraw.front());
							GetObj().DrawMesh(index);
						}
					}
				}
			}
		public:
			void			Init_Sub(void) noexcept override;
			void			FirstUpdate(void) noexcept override { FirstUpdate_GunParts(); }
			void			SetGunPartsMatrix(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept;
			void			DrawShadow(void) noexcept override {
				if (!IsActive()) { return; }
				DrawCommon();
			}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!IsActive()) { return; }
				if (!IsDraw(Range)) { return; }
				if (isDrawSemiTrans) { return; }
				DrawCommon();
			}
			void			Dispose_Sub(void) noexcept override;
		private:
			int	GetFrameNum(void) noexcept override { return static_cast<int>(GunFrame::Max); }
			const char* GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
		public:
			virtual void	Init_GunParts(void) noexcept {}
			virtual void	FirstUpdate_GunParts(void) noexcept {}
			virtual void	Dispose_GunParts(void) noexcept {}
		};


		class MagazinePartsObj : public GunPartsObj {
			std::array<std::shared_ptr<Objects::AmmoInChamberObj>,5>			m_Ammo{};		//
		public:
			MagazinePartsObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Magazine); }
			virtual ~MagazinePartsObj(void) noexcept {}
		public:
			void			Init_GunParts(void) noexcept override;
			void			FirstUpdate_GunParts(void) noexcept override {
				for (auto& ammo : this->m_Ammo) {
					int index = static_cast<int>(&ammo - &this->m_Ammo.front());
					auto Mat = GetObj().GetFrameLocalWorldMatrix(1 + index);
					ammo->SetMat(Mat.pos(), Matrix3x3DX::Get33DX(Mat));
				}
			}
			void			Dispose_GunParts(void) noexcept override {
				for (auto& ammo : this->m_Ammo) {
					ObjectManager::Instance()->DelObj(ammo);
					ammo.reset();
				}
			}
		public:
			void			SetAmmoActive(bool value) noexcept {
				for (auto& ammo : this->m_Ammo) {
					ammo->SetActive(value);
				}
			}
			void			SetHandMatrix(const Matrix4x4DX& Mat) noexcept {
				SetMove().SetMat(Matrix3x3DX::Get33DX(Mat));
				SetMove().SetPos(Mat.pos());
				SetMove().Update(0.0f, 0.0f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
		};

		class LowerPartsObj : public GunPartsObj {
		private:
		public://ゲッター
		public:
			LowerPartsObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Lower); }
			virtual ~LowerPartsObj(void) noexcept {}
		public:
		};

		class UpperPartsObj : public GunPartsObj {
		public:
			UpperPartsObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Upper); }
			virtual ~UpperPartsObj(void) noexcept {}
		};

		class BarrelPartsObj : public GunPartsObj {
		private:
		public://ゲッター
		public:
			BarrelPartsObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Barrel); }
			virtual ~BarrelPartsObj(void) noexcept {}
		public:
		};

		class UnderRailPartsObj : public GunPartsObj {
		private:
		public://ゲッター
		public:
			UnderRailPartsObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::UnderRail); }
			virtual ~UnderRailPartsObj(void) noexcept {}
		public:
		};

		class SightPartsObj : public GunPartsObj {
		public:
			SightPartsObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Sight); }
			virtual ~SightPartsObj(void) noexcept {}
		public:
			void SetScopeAlpha(float Per) noexcept {
				SetObj().SetMaterialOpacityRate(1, Per);
				SetIsDrawMesh(1, Per > 0.2f);
			}
		};

		class MuzzlePartsObj : public GunPartsObj {
		public:
			MuzzlePartsObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::MuzzleAdapter); }
			virtual ~MuzzlePartsObj(void) noexcept {}
		};
	}
}
