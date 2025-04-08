#pragma once
#include	"../../Header.hpp"

#include	"ModData.hpp"
#include	"Gun_before.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ModClass : public ObjectBaseClass {
			ModSlotControl										m_ModSlotControl{};
		public:
			ModSlotControl& SetModSlot(void) noexcept { return this->m_ModSlotControl; }
			const ModSlotControl& GetModSlot(void) const noexcept { return this->m_ModSlotControl; }
		public:
			ModClass(void) noexcept {}
			~ModClass(void) noexcept {}
		public:
			auto	GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }

			const Matrix4x4DX		GetFramePartsMat(GunFrame frame) const noexcept {
				const auto* ptr = GetModSlot().HasFrameBySlot(frame);
				if (ptr) {
					return ((std::shared_ptr<ModClass>&)(*ptr))->GetFramePartsMat(frame);
				}
				if (HaveFrame(static_cast<int>(frame))) {
					auto FrameID = GetFrame(static_cast<int>(frame));
					Matrix4x4DX Ret = GetObj().GetFrameLocalWorldMatrix(FrameID);
					if (frame == GunFrame::Sight) {
						if (GetObj().GetFrameChildNum(FrameID) > 0) {
							Vector3DX vec = (GetObj().GetChildFrameWorldMatrix(FrameID, 0).pos() - Ret.pos()).normalized();
							Ret = (Ret.rotation() * Matrix4x4DX::RotVec2(Ret.yvec(), vec)) * Matrix4x4DX::Mtrans(Ret.pos());
						}
					}
					return Ret;
				}
				return Matrix4x4DX::identity();
			}
		public:
			void			Init_Sub(void) noexcept override {
				SetModSlot().Init(GetFilePath());
				ObjectBaseClass::SetMinAABB(Vector3DX::vget(-1.f, -1.f, -1.f) * Scale3DRate);
				ObjectBaseClass::SetMaxAABB(Vector3DX::vget(1.f, 1.f, 1.f) * Scale3DRate);
				Init_Mod();
			}

			void			FirstExecute(void) noexcept override {
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				FirstExecute_Mod();
			}
			void			SetModMatrix(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept {
				SetMove().SetMat(rotation);
				SetMove().SetPos(pos);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				SetModSlot().UpdatePartsAnim(GetObj());
				SetModSlot().UpdatePartsMove(GetFramePartsMat(GunFrame::UnderRail), GunSlot::UnderRail);
				SetModSlot().UpdatePartsMove(GetFramePartsMat(GunFrame::Sight), GunSlot::Sight);
				SetModSlot().UpdatePartsMove(GetFramePartsMat(GunFrame::MuzzleAdapter), GunSlot::MuzzleAdapter);
			}
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
			void			Dispose_Sub(void) noexcept override {
				SetModSlot().Dispose();
				Dispose_Mod();
			}
		private:
			int	GetFrameNum(void) noexcept override { return static_cast<int>(GunFrame::Max); }
			const char* GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
		public:
			virtual void	Init_Mod(void) noexcept {}
			virtual void	FirstExecute_Mod(void) noexcept {}
			virtual void	Dispose_Mod(void) noexcept {}
		};


		class MagazineClass : public ModClass {
			std::array<std::shared_ptr<AmmoInChamberClass>,5>			m_Ammo{};		//
		public:
			MagazineClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Magazine); }
			~MagazineClass(void) noexcept {}
		public:
			void			Init_Mod(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				for (auto& a : this->m_Ammo) {
					a = std::make_shared<AmmoInChamberClass>();
					ObjMngr->AddObject(a);
					ObjMngr->LoadModel(a, a, GetModSlot().GetModData()->GetAmmoSpecMagTop()->GetPath().c_str());
					a->Init();
					a->SetActive(true);
				}
			}
			void			FirstExecute_Mod(void) noexcept override {
				int index = 1;
				for (auto& a : this->m_Ammo) {
					auto Mat = GetObj().GetFrameLocalWorldMatrix(index); ++index;
					a->SetMat(Mat.pos(), Matrix3x3DX::Get33DX(Mat));
				}
			}
			void			Dispose_Mod(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				for (auto& a : this->m_Ammo) {
					ObjMngr->DelObj((SharedObj*)&a);
					a.reset();
				}
			}
		public:
			void			SetAmmoActive(bool value) noexcept {
				for (auto& a : this->m_Ammo) {
					a->SetActive(value);
				}
			}
			void			SetHandMatrix(const Matrix4x4DX& value) noexcept {
				SetMove().SetMat(Matrix3x3DX::Get33DX(value));
				SetMove().SetPos(value.pos());
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
		};

		class LowerClass : public ModClass {
		private:
		public://ゲッター
		public:
			LowerClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Lower); }
			~LowerClass(void) noexcept {}
		public:
		};

		class UpperClass : public ModClass {
		public:
			UpperClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Upper); }
			~UpperClass(void) noexcept {}
		};

		class BarrelClass : public ModClass {
		private:
		public://ゲッター
		public:
			BarrelClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Barrel); }
			~BarrelClass(void) noexcept {}
		public:
		};

		class UnderRailClass : public ModClass {
		private:
		public://ゲッター
		public:
			UnderRailClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::UnderRail); }
			~UnderRailClass(void) noexcept {}
		public:
		};

		class SightClass : public ModClass {
		public:
			SightClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Sight); }
			~SightClass(void) noexcept {}
		};

		class MuzzleClass : public ModClass {
		public:
			MuzzleClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::MuzzleAdapter); }
			~MuzzleClass(void) noexcept {}
		};
	};
};
