#pragma once
#include	"../../Header.hpp"

#include "ModData.hpp"
#include "Gun_before.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ModClass : public ObjectBaseClass {
			ModSlotControl										m_ModSlotControl{};
		public:
			ModSlotControl& GetModSlot() noexcept { return this->m_ModSlotControl; }
			const ModSlotControl& GetModSlot() const noexcept { return this->m_ModSlotControl; }
		public:
			ModClass(void) noexcept {}
			~ModClass(void) noexcept {}
		public:
			auto	GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			auto	GetFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(GetFrame(static_cast<int>(frame))); }

			const auto		GetFrameWorldMat_P(GunFrame frame) const noexcept {
				//該当フレームがあるのなら上書き
				Matrix4x4DX Ret;
				if (m_ModSlotControl.GetPartsFrameWorldMat(frame, &Ret)) {
					return Ret;
				}
				if (HaveFrame(static_cast<int>(frame))) {
					Ret = GetFrameWorldMat(frame);
					if (frame == GunFrame::Sight) {
						if (GetObj_const().GetFrameChildNum(GetFrame(static_cast<int>(frame))) > 0) {
							Vector3DX vec = (GetObj_const().GetChildFrameWorldMatrix(GetFrame(static_cast<int>(frame)), 0).pos() - Ret.pos()).normalized();
							//Vector3DX::Cross(pRet->xvec(), vec)
							Ret = (Ret.rotation() * Matrix4x4DX::RotVec2(Ret.yvec(), vec)) * Matrix4x4DX::Mtrans(Ret.pos());
						}
					}
					return Ret;
				}
				return Matrix4x4DX::identity();
			}
		public:
			void			Init_Sub(void) noexcept override {
				m_ModSlotControl.InitModSlotControl(this->m_FilePath);
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
				m_ModSlotControl.UpdatePartsAnim(GetObj());
				m_ModSlotControl.UpdatePartsMove(GetFrameWorldMat_P(GunFrame::UnderRail), GunSlot::UnderRail);
				m_ModSlotControl.UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Sight), GunSlot::Sight);
				m_ModSlotControl.UpdatePartsMove(GetFrameWorldMat_P(GunFrame::MuzzleAdapter), GunSlot::MuzzleAdapter);
			}
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					auto* CameraParts = Camera3D::Instance();
					if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }
					this->m_obj.DrawModel();
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive) {
					if (!isDrawSemiTrans) {
						auto* CameraParts = Camera3D::Instance();
						if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }
						this->m_obj.DrawModel();
					}
				}
			}
			void			Dispose_Sub(void) noexcept override {
				m_ModSlotControl.DisposeModSlotControl();
			}
		private:
			int	GetFrameNum() noexcept override { return static_cast<int>(GunFrame::Max); }
			const char* GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
		public:
			virtual void	FirstExecute_Mod(void) noexcept {}
		};


		class MagazineClass : public ModClass {
		public:
			MagazineClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Magazine); }
			~MagazineClass(void) noexcept {}
		public:
			void			FirstExecute_Mod(void) noexcept override {
			}
		public:
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
