#pragma once
#include	"../../Header.hpp"

#include "ObjectBase.hpp"
#include "AmmoData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ModClass : public ObjectBaseClass {
		private:
		public:
			ModClass(void) noexcept { }
			~ModClass(void) noexcept { }
		public:
			auto& GetAnime(GunAnimeID anim) noexcept { return GetObj().get_anime((int)anim); }
			const auto& GetFrame(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].first; }
			const auto GetFrameLocalMat(GunFrame frame) const noexcept { return GetFrameLocalMatrix(GetFrame(frame)); }
			const auto GetFrameWorldMat(GunFrame frame) const noexcept { return GetFrameWorldMatrix(GetFrame(frame)); }
		public:
			void ResetFrameLocalMat(GunFrame frame) noexcept { GetObj().frame_Reset(GetFrame(frame)); }
			void SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(GetFrame(frame), value * this->m_Frames[(int)frame].second); }
		public:
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				Init_Mod();
			}
			void			FirstExecute(void) noexcept override {
				ObjectBaseClass::FirstExecute();
				SetMove(GetMove().mat.GetRot(), GetMove().pos);
				FirstExecute_Mod();
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
				this->GetObj().Dispose();
				this->m_col.Dispose();
			}
		public:
			virtual void			Init_Mod(void) noexcept {}
			virtual void			FirstExecute_Mod(void) noexcept {}
			virtual void			Draw_Mod(bool) noexcept {}
			virtual void			Dispose_Mod(void) noexcept {}
		};


		class MagazineClass : public ModClass {
		private:
			std::vector<std::shared_ptr<AmmoDataClass>>	m_AmmoSpec;
			int											m_Capacity{ 0 };
			int											m_CapacityMax{ 0 };
			MATRIX_ref									HandMatrix;
			float										HandPer{ 0.f };
			RELOADTYPE									m_ReloadTypeBuf{ RELOADTYPE::MAG };
		public:
			void			SetReloadType(RELOADTYPE ReloadType) noexcept { this->m_ReloadTypeBuf = ReloadType; }
			void			SetHandMatrix(const MATRIX_ref& value, float pPer) noexcept {
				this->HandMatrix = value;
				this->HandPer = pPer;
			}
			void			SetAmmo(int value) noexcept { this->m_Capacity = std::clamp(value, 0, m_CapacityMax); }
			void			SubAmmo(void) noexcept { SetAmmo(this->m_Capacity - 1); }
			void			AddAmmo(void) noexcept { SetAmmo(this->m_Capacity + 1); }
		public://ゲッター
			const auto&		GetAmmoSpecMagTop(void) const noexcept { return this->m_AmmoSpec[0]; }
			const auto&		GetAmmoNum(void) const noexcept { return this->m_Capacity; }
			const auto&		GetAmmoAll(void) const noexcept { return  this->m_CapacityMax; }
		public:
			MagazineClass(void) noexcept { this->m_objType = ObjType::Magazine; }
			~MagazineClass(void) noexcept { }
		public:
			void			Init_Mod(void) noexcept override;
			void			FirstExecute_Mod(void) noexcept override;
			void			Dispose_Mod(void) noexcept override;
		public:
		};

		class LowerClass : public ModClass {
		private:
		public://ゲッター
		public:
			LowerClass(void) noexcept { this->m_objType = ObjType::Lower; }
			~LowerClass(void) noexcept { }
		public:
		};

		class UpperClass : public ModClass {
		private:
		public://ゲッター
		public:
			UpperClass(void) noexcept { this->m_objType = ObjType::Upper; }
			~UpperClass(void) noexcept { }
		public:
		};

		class BarrelClass : public ModClass {
		private:
		public://ゲッター
		public:
			BarrelClass(void) noexcept { this->m_objType = ObjType::Barrel; }
			~BarrelClass(void) noexcept { }
		public:
		};
	};
};
