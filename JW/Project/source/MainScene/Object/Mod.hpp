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
			const bool		HaveFrame(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].first != -1; }
			const auto&		GetFrame(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].first; }
			const auto&		GetFrameBaseLocalMat(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].second; }
			const auto		GetFrameLocalMat(GunFrame frame) const noexcept { return GetFrameLocalMatrix(GetFrame(frame)); }
			const auto		GetFrameWorldMat(GunFrame frame) const noexcept { return GetFrameWorldMatrix(GetFrame(frame)); }
			void			ResetFrameLocalMat(GunFrame frame) noexcept { GetObj().frame_Reset(GetFrame(frame)); }
			void			SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(GetFrame(frame), value * GetFrameBaseLocalMat(frame)); }
		public:
			void			Init(void) noexcept override;

			void			FirstExecute(void) noexcept override {
				ObjectBaseClass::FirstExecute();
				SetMove(GetMove().mat.GetRot(), GetMove().pos);
				FirstExecute_Mod();
			}
			void			SetGunMatrix(const MATRIX_ref& value) noexcept {
				SetMove(value.GetRot(), value.pos());
				GetSlotControl()->UpdatePartsAnim(GetObj());
				GetSlotControl()->UpdatePartsMove(GetFrameWorldMat(GunFrame::UnderRail), GunSlot::UnderRail);
				GetSlotControl()->UpdatePartsMove(GetFrameWorldMat(GunFrame::Sight), GunSlot::Sight);
				GetSlotControl()->UpdatePartsMove(GetFrameWorldMat(GunFrame::MuzzleAdapter), GunSlot::MuzzleAdapter);
			}
			void			DrawShadow(void) noexcept override {
				this->m_obj.DrawModel();
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				Draw_Mod(isDrawSemiTrans);
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-0.1f*Scale_Rate, -0.1f*Scale_Rate, -0.1f*Scale_Rate)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(0.1f*Scale_Rate, 0.1f*Scale_Rate, 0.1f*Scale_Rate)).get()) == FALSE
						) {
						if (!isDrawSemiTrans) {
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
			MATRIX_ref									HandMatrix;
			float										HandPer{0.f};
			bool										m_isDirect{false};
		public:
			MagazineClass(void) noexcept { this->m_objType = ObjType::Magazine; }
			~MagazineClass(void) noexcept {}
		public:
			void			FirstExecute_Mod(void) noexcept override {
				if (this->m_isDirect) {
					SetMove(this->HandMatrix.GetRot(), this->HandMatrix.pos());
				}
				else if (this->HandPer > 0.f) {
					SetMove(MATRIX_ref::RotX(deg2rad(-30.f*this->HandPer))*GetMove().mat.GetRot(), Lerp(GetMove().pos, this->HandMatrix.pos(), this->HandPer));
				}
			}
			void			Dispose_Mod(void) noexcept override {}
		public:
			void			SetHandMatrix(const MATRIX_ref& value, float pPer, bool isDirect) noexcept {
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
