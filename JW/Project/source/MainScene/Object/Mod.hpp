#pragma once
#include	"../../Header.hpp"

#include "ModData.hpp"
#include "Gun_before.hpp"

#include "ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ModClass :
			public ObjectBaseClass,
			public ModSlotControl {
		public:
			ModClass(void) noexcept {}
			~ModClass(void) noexcept {}
		public:
			auto&			GetAnime(GunAnimeID anim) noexcept { return GetObj().get_anime((int)anim); }
			const auto		GetFrameWorldMat(GunFrame frame) const noexcept {
				//該当フレームがあるのなら上書き
				Matrix4x4DX Ret;
				if (ModSlotControl::GetPartsFrameWorldMat(frame, &Ret)) {
					return Ret;
				}
				if (HaveFrame((int)frame)) {
					Ret = GetFrameWorldMatrix(GetFrame((int)frame));
					if (frame == GunFrame::Sight) {
						if (GetChildFrameNum(GetFrame((int)frame)) > 0) {
							Vector3DX vec = (GetChildFrameWorldMatrix(GetFrame((int)frame), 0).pos() - Ret.pos()).normalized();
							//Vector3DX::Cross(pRet->xvec(), vec)
							Ret = (Ret.rotation()*Matrix4x4DX::RotVec2(Ret.yvec(), vec)) * Matrix4x4DX::Mtrans(Ret.pos());
						}
					}
					return Ret;
				}
				return Matrix4x4DX::identity();
			}
		public:
			void			Init(void) noexcept override;

			void			FirstExecute(void) noexcept override {
				UpdateMove();
				FirstExecute_Mod();
			}
			void			SetModMatrix(const Matrix4x4DX& value) noexcept {
				SetMove(value.rotation(), value.pos());
				ModSlotControl::UpdatePartsAnim(GetObj());
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat(GunFrame::UnderRail), GunSlot::UnderRail);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat(GunFrame::Sight), GunSlot::Sight);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat(GunFrame::MuzzleAdapter), GunSlot::MuzzleAdapter);
			}
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					auto* DrawParts = DXDraw::Instance();
					if ((GetMove().pos - DrawParts->GetMainCamera().GetCamPos()).magnitude() > 10.f*Scale_Rate) { return; }
					this->m_obj.DrawModel();
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				Draw_Mod(isDrawSemiTrans);
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-1.f*Scale_Rate, -1.f*Scale_Rate, -1.f*Scale_Rate)).get(),
						(this->GetObj().GetMatrix().pos() + Vector3DX::vget(1.f*Scale_Rate, 1.f*Scale_Rate, 1.f*Scale_Rate)).get()) == FALSE
						) {
						if (!isDrawSemiTrans) {
							auto* DrawParts = DXDraw::Instance();
							if ((GetMove().pos - DrawParts->GetMainCamera().GetCamPos()).magnitude() > 10.f*Scale_Rate) { return; }
							this->m_obj.DrawModel();
						}
					}
				}
			}
			void			Dispose(void) noexcept override {
				Dispose_Mod();
				DisposeModSlotControl();
				this->GetObj().Dispose();
				this->m_col.Dispose();
			}
		public:
			virtual void	Init_Mod(void) noexcept {}
			virtual void	FirstExecute_Mod(void) noexcept {}
			virtual void	Draw_Mod(bool) noexcept {}
			virtual void	Dispose_Mod(void) noexcept {}
		};


		class MagazineClass : public ModClass {
		private:
			Matrix4x4DX									HandMatrix;
			float										HandPer{0.f};
			bool										m_isDirect{false};
		public:
			MagazineClass(void) noexcept { this->m_objType = ObjType::Magazine; }
			~MagazineClass(void) noexcept {}
		public:
			void			FirstExecute_Mod(void) noexcept override {
				if (this->m_isDirect) {
					SetMove(this->HandMatrix.rotation(), this->HandMatrix.pos());
				}
				else if (this->HandPer > 0.f) {
					SetMove(Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(-30.f*this->HandPer))*GetMove().mat.rotation(), Lerp(GetMove().pos, this->HandMatrix.pos(), this->HandPer));
				}
			}
			void			Dispose_Mod(void) noexcept override {}
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
			LowerClass(void) noexcept { this->m_objType = ObjType::Lower; }
			~LowerClass(void) noexcept {}
		public:
		};

		class UpperClass : public ModClass {
		public:
			UpperClass(void) noexcept { this->m_objType = ObjType::Upper; }
			~UpperClass(void) noexcept {}
		};

		class BarrelClass : public ModClass {
		private:
		public://ゲッター
		public:
			BarrelClass(void) noexcept { this->m_objType = ObjType::Barrel; }
			~BarrelClass(void) noexcept {}
		public:
		};

		class UnderRailClass : public ModClass {
		private:
		public://ゲッター
		public:
			UnderRailClass(void) noexcept { this->m_objType = ObjType::UnderRail; }
			~UnderRailClass(void) noexcept {}
		public:
		};

		class SightClass : public ModClass {
		public:
			SightClass(void) noexcept { this->m_objType = ObjType::Sight; }
			~SightClass(void) noexcept {}
		};

		class MuzzleClass : public ModClass {
		public:
			MuzzleClass(void) noexcept { this->m_objType = ObjType::MuzzleAdapter; }
			~MuzzleClass(void) noexcept {}
		};
	};
};
