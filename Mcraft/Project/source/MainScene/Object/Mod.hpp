#pragma once
#include	"../../Header.hpp"

#include "ModData.hpp"
#include "Gun_before.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ModClass : public ObjectBaseClass, public ModSlotControl {
		public:
			ModClass(void) noexcept {}
			~ModClass(void) noexcept {}
		public:
			auto	GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			auto	GetFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(GetFrame(static_cast<int>(frame))); }

			const auto		GetFrameWorldMat_P(GunFrame frame) const noexcept {
				//該当フレームがあるのなら上書き
				Matrix4x4DX Ret;
				if (ModSlotControl::GetPartsFrameWorldMat(frame, &Ret)) {
					return Ret;
				}
				if (HaveFrame((int)frame)) {
					Ret = GetFrameWorldMat(frame);
					if (frame == GunFrame::Sight) {
						if (GetObj_const().GetFrameChildNum(GetFrame((int)frame)) > 0) {
							Vector3DX vec = (GetObj_const().GetChildFrameWorldMatrix(GetFrame((int)frame), 0).pos() - Ret.pos()).normalized();
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
				ModSlotControl::InitModSlotControl(this->m_FilePath);
			}

			void			FirstExecute(void) noexcept override {
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				FirstExecute_Mod();
			}
			void			SetModMatrix(const Matrix4x4DX& value) noexcept {
				SetMove().SetMat(Matrix3x3DX::Get33DX(value.rotation()));
				SetMove().SetPos(value.pos());
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				ModSlotControl::UpdatePartsAnim(GetObj());
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat_P(GunFrame::UnderRail), GunSlot::UnderRail);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Sight), GunSlot::Sight);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat_P(GunFrame::MuzzleAdapter), GunSlot::MuzzleAdapter);
			}
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					auto* CameraParts = Camera3D::Instance();
					if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }
					this->m_obj.DrawModel();
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-1.f * Scale3DRate, -1.f * Scale3DRate, -1.f * Scale3DRate)).get(),
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(1.f * Scale3DRate, 1.f * Scale3DRate, 1.f * Scale3DRate)).get()) == FALSE
						) {
						if (!isDrawSemiTrans) {
							auto* CameraParts = Camera3D::Instance();
							if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }
							this->m_obj.DrawModel();
						}
					}
				}
			}
			void			Dispose_Sub(void) noexcept override {
				ModSlotControl::DisposeModSlotControl();
			}
		private:
			int	GetFrameNum() noexcept override { return (int)GunFrame::Max; }
			const char* GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
		public:
			virtual void	FirstExecute_Mod(void) noexcept {}
		};


		class MagazineClass : public ModClass {
		private:
			Matrix4x4DX									HandMatrix;
			float										HandPer{ 0.f };
			bool										m_isDirect{ false };
		public:
			MagazineClass(void) noexcept { this->m_objType = (int)ObjType::Magazine; }
			~MagazineClass(void) noexcept {}
		public:
			void			FirstExecute_Mod(void) noexcept override {
				if (this->m_isDirect) {
					SetMove().SetMat(Matrix3x3DX::Get33DX(this->HandMatrix.rotation()));
					SetMove().SetPos(this->HandMatrix.pos());
				}
				else if (this->HandPer > 0.f) {
					SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30.f * this->HandPer)) * GetMove().GetMat());
					SetMove().SetPos(Lerp(GetMove().GetPos(), this->HandMatrix.pos(), this->HandPer));
				}
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
		public:
			void			SetHandMatrix(const Matrix4x4DX& value, float pPer, bool isDirect) noexcept {
				this->HandMatrix = value;
				this->HandPer = pPer;
				this->m_isDirect = isDirect;
			}
		};

		class LowerClass : public ModClass {
		private:
		public://ゲッター
		public:
			LowerClass(void) noexcept { this->m_objType = (int)ObjType::Lower; }
			~LowerClass(void) noexcept {}
		public:
		};

		class UpperClass : public ModClass {
		public:
			UpperClass(void) noexcept { this->m_objType = (int)ObjType::Upper; }
			~UpperClass(void) noexcept {}
		};

		class BarrelClass : public ModClass {
		private:
		public://ゲッター
		public:
			BarrelClass(void) noexcept { this->m_objType = (int)ObjType::Barrel; }
			~BarrelClass(void) noexcept {}
		public:
		};

		class UnderRailClass : public ModClass {
		private:
		public://ゲッター
		public:
			UnderRailClass(void) noexcept { this->m_objType = (int)ObjType::UnderRail; }
			~UnderRailClass(void) noexcept {}
		public:
		};

		class SightClass : public ModClass {
		public:
			SightClass(void) noexcept { this->m_objType = (int)ObjType::Sight; }
			~SightClass(void) noexcept {}
		};

		class MuzzleClass : public ModClass {
		public:
			MuzzleClass(void) noexcept { this->m_objType = (int)ObjType::MuzzleAdapter; }
			~MuzzleClass(void) noexcept {}
		};
	};
};
