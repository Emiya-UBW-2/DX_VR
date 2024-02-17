#pragma once
#include	"../../Header.hpp"

#include "ModData.hpp"
#include "Gun_before.hpp"
#include "AmmoData.hpp"

#include "ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ModSlotControl {
		private:
			std::unique_ptr<SlotPartsControl>	m_SlotControl;
			std::shared_ptr<ModDataClass>		m_ModDataClass;
		public:
			auto&			GetSlotControl() noexcept { return this->m_SlotControl; }
			const auto&		GetSlotControl() const noexcept { return this->m_SlotControl; }
			auto&			GetModData() noexcept { return this->m_ModDataClass; }
			const auto&		GetModData() const noexcept { return this->m_ModDataClass; }
		protected:
			void			InitModSlotControl(const std::string& PilePath,bool ismod) noexcept {
				//データ
				m_SlotControl = std::make_unique<SlotPartsControl>();
				m_ModDataClass = *ModDataManager::Instance()->AddData(PilePath, ismod);
			}
			void			DisposeModSlotControl(void) noexcept {
				m_SlotControl->DisposeSlotPartsControl();
				m_ModDataClass.reset();
			}
		public:
			void			SetMod(GunSlot Slot, int ID, const MV1& BaseModel) noexcept;
			void			RemoveMod(GunSlot Slot) noexcept {
				if (this->m_SlotControl->HasParts(Slot)) {
					this->m_SlotControl->RemoveParts(Slot);
				}
			}
		};

		class ModClass :
			public ObjectBaseClass,
			public ModSlotControl
		{
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
			void			FirstExecute_Mod(void) noexcept override {
				switch (m_ReloadTypeBuf) {
				case RELOADTYPE::MAG:
					if (this->HandPer > 0.f) {
						SetMove(MATRIX_ref::RotX(deg2rad(-30.f*this->HandPer))*GetMove().mat.GetRot(), Lerp(GetMove().pos, this->HandMatrix.pos(), this->HandPer));
					}
					break;
				case RELOADTYPE::AMMO:
					break;
				default:
					break;
				}
			}
			void			Dispose_Mod(void) noexcept override {
				for (auto& A : this->m_AmmoSpec) {
					A.reset();
				}
				this->m_AmmoSpec.clear();
			}
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
			bool							m_IsRecoilPower{ false };
			bool							m_IsRecoilReturn{ false };
			bool							m_IsShotType{ false };		//

			int								m_RecoilPower{ 120 };
			float							m_RecoilReturn{ 0.9f };
			SHOTTYPE						m_ShotType{ SHOTTYPE::SEMI };		//
		public://ゲッター
			const auto& GetIsRecoilPower(void) const noexcept { return this->m_IsRecoilPower; }
			const auto& GetIsRecoilReturn(void) const noexcept { return this->m_IsRecoilReturn; }
			const auto& GetIsShotType(void) const noexcept { return this->m_IsShotType; }
			const auto& GetRecoilPower(void) const noexcept { return this->m_RecoilPower; }
			const auto& GetRecoilReturn(void) const noexcept { return this->m_RecoilReturn; }
			const auto& GetShotType(void) const noexcept { return this->m_ShotType; }
		public:
			UpperClass(void) noexcept { this->m_objType = ObjType::Upper; }
			~UpperClass(void) noexcept { }
		public:
			void			Init_Mod(void) noexcept override;
		};

		class BarrelClass : public ModClass {
		private:
		public://ゲッター
		public:
			BarrelClass(void) noexcept { this->m_objType = ObjType::Barrel; }
			~BarrelClass(void) noexcept { }
		public:
		};

		class UnderRailClass : public ModClass {
		private:
		public://ゲッター
		public:
			UnderRailClass(void) noexcept { this->m_objType = ObjType::UnderRail; }
			~UnderRailClass(void) noexcept { }
		public:
		};

		class SightClass : public ModClass {
		private:
			GraphHandle m_Reitcle;
			float m_ZoomSize{ 1.f };
		public://ゲッター
			const auto&		GetReitcleGraph(void) const noexcept { return this->m_Reitcle; }
			const auto&		GetZoomSize(void) const noexcept { return this->m_ZoomSize; }
		public:
			SightClass(void) noexcept { this->m_objType = ObjType::Sight; }
			~SightClass(void) noexcept { }
		public:
			void			Init_Mod(void) noexcept override;
		};

		class MuzzleClass : public ModClass {
		private:
			GunShootSound	m_GunShootSound{ GunShootSound::Normal };
		public://ゲッター
			const auto&		GetGunShootSound(void) const noexcept { return this->m_GunShootSound; }
		public:
			MuzzleClass(void) noexcept { this->m_objType = ObjType::MuzzleAdapter; }
			~MuzzleClass(void) noexcept { }
		public:
			void			Init_Mod(void) noexcept override;
		};

		
	};
};
