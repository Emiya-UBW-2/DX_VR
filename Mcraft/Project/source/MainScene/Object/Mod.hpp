#pragma once
#include	"../../Header.hpp"

#include	"ModData.hpp"
#include	"Gun_before.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunPartsClass : public ObjectBaseClass {
			std::unique_ptr<GunPartsSlotControl>								m_GunPartsSlotControl{};
		public:
			GunPartsClass(void) noexcept {}
			~GunPartsClass(void) noexcept {}
		public:
			const std::unique_ptr<GunPartsSlotControl>& GetGunPartsSlot(void) const noexcept { return this->m_GunPartsSlotControl; }
			auto					GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			const Matrix4x4DX		GetFramePartsMat(GunFrame frame) const noexcept;
		public:
			void			Init_Sub(void) noexcept override;
			void			FirstExecute(void) noexcept override { FirstExecute_GunParts(); }
			void			SetGunPartsMatrix(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept;
			void			DrawShadow(void) noexcept override {
				if (!IsActive()) { return; }

				auto* CameraParts = Camera3D::Instance();
				if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }

				GetObj().DrawModel();
			}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!IsActive()) { return; }
				if (!IsDraw(Range)) { return; }
				if (isDrawSemiTrans) { return; }

				auto* CameraParts = Camera3D::Instance();
				if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }

				GetObj().DrawModel();
			}
			void			Dispose_Sub(void) noexcept override;
		private:
			int	GetFrameNum(void) noexcept override { return static_cast<int>(GunFrame::Max); }
			const char* GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
		public:
			virtual void	Init_GunParts(void) noexcept {}
			virtual void	FirstExecute_GunParts(void) noexcept {}
			virtual void	Dispose_GunParts(void) noexcept {}
		};


		class MagazineClass : public GunPartsClass {
			std::array<std::shared_ptr<AmmoInChamberClass>,5>			m_Ammo{};		//
		public:
			MagazineClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Magazine); }
			~MagazineClass(void) noexcept {}
		public:
			void			Init_GunParts(void) noexcept override;
			void			FirstExecute_GunParts(void) noexcept override {
				int index = 1;
				for (auto& ammo : this->m_Ammo) {
					auto Mat = GetObj().GetFrameLocalWorldMatrix(index); ++index;
					ammo->SetMat(Mat.pos(), Matrix3x3DX::Get33DX(Mat));
				}
			}
			void			Dispose_GunParts(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				for (auto& ammo : this->m_Ammo) {
					ObjMngr->DelObj((SharedObj*)&ammo);
					ammo.reset();
				}
			}
		public:
			void			SetAmmoActive(bool value) noexcept {
				for (auto& ammo : this->m_Ammo) {
					ammo->SetActive(value);
				}
			}
			void			SetHandMatrix(const Matrix4x4DX& value) noexcept {
				SetMove().SetMat(Matrix3x3DX::Get33DX(value));
				SetMove().SetPos(value.pos());
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
		};

		class LowerClass : public GunPartsClass {
		private:
		public://ゲッター
		public:
			LowerClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Lower); }
			~LowerClass(void) noexcept {}
		public:
		};

		class UpperClass : public GunPartsClass {
		public:
			UpperClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Upper); }
			~UpperClass(void) noexcept {}
		};

		class BarrelClass : public GunPartsClass {
		private:
		public://ゲッター
		public:
			BarrelClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Barrel); }
			~BarrelClass(void) noexcept {}
		public:
		};

		class UnderRailClass : public GunPartsClass {
		private:
		public://ゲッター
		public:
			UnderRailClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::UnderRail); }
			~UnderRailClass(void) noexcept {}
		public:
		};

		class SightClass : public GunPartsClass {
		public:
			SightClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Sight); }
			~SightClass(void) noexcept {}
		};

		class MuzzleClass : public GunPartsClass {
		public:
			MuzzleClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::MuzzleAdapter); }
			~MuzzleClass(void) noexcept {}
		};
	};
};
