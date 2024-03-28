#pragma once
#include	"../../Header.hpp"

#include "GunEnum.hpp"
#include "GunData.hpp"
#include "Gun_before.hpp"
#include "CharaAnimData.hpp"

#include "../../sub.hpp"
#include "ObjectBase.hpp"
#include "Ammo.hpp"
#include "Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass :
			public ObjectBaseClass,
			public MuzzleSmokeControl,
			public ModSlotControl {
		private:
			std::shared_ptr<AmmoDataClass>			m_ChamberAmmoData{nullptr};		//
			GunAnimeID								m_ShotPhase{GunAnimeID::Base};	//
			bool									m_Cancel{false};			//
			bool									m_ShotSwitch{false};				//
			int										m_boltSoundSequence{-1};			//サウンド
			bool									m_IsChamberOn{false};				//チャンバーに弾を込めるか
			bool									m_IsEject{false};
			int										m_NextMagNum{0};
			int										m_NextMagUniqueID{0};

			int										m_AmmoLoadCount{0};
			bool									m_AmmoLoadSwitch{false};
			bool									m_IsAmmoLoadCount{false};

			FallControl								m_MagFall;
			FallControl								m_CartFall;

			int										m_GunSightSel{0};
			float									m_GunChangePer{0.f};
			std::array<const std::shared_ptr<SightClass>*, 2>	m_SightPtr{nullptr};
			const std::shared_ptr<MuzzleClass>*		m_MuzzlePtr{nullptr};
			const std::shared_ptr<UpperClass>*		m_UpperPtr{nullptr};
			const std::shared_ptr<LowerClass>*		m_LowerPtr{nullptr};
			const std::shared_ptr<MagazineClass>*	m_MagazinePtr{nullptr};

			int										m_ShootRate_Diff{0};
			int										m_ReloadRate_Diff{0};

			int										m_Capacity{0};//弾数
		private:
			std::array<float, (int)CharaGunAnimeID::Max>									m_GunAnimFrame;
			std::vector<std::array<int, (int)CharaGunAnimeID::Max>>							m_CharaAnimeSet;
			std::vector<std::array<std::vector<EnumGunAnim>, (int)EnumGunAnimType::Max>>	m_GunAnimeSet;
			float																			m_UpperAnim{0.f};
		public:
			const auto&		GetUpperAnim(void) const noexcept { return this->m_UpperAnim; }
			const auto&		GetGunDataClass(void) const noexcept { return (std::shared_ptr<GunDataClass>&)this->GetModData(); }
			auto&			GetGunAnimFrame(CharaGunAnimeID ID) { return m_GunAnimFrame.at((int)ID); }

			void			SetGunAnim(CharaGunAnimeID ID, float value) { GetGunAnimFrame(ID) = value; }
			void			UpdateGunAnim(CharaGunAnimeID ID, float speed) { GetGunAnimFrame(ID) += 30.f / FPS * speed; }
			bool			GetGunAnimZero(CharaGunAnimeID ID) { return GetGunAnimFrame(ID) <= 0.f; }
			bool			GetGunAnimEnd(CharaGunAnimeID ID) { return GetGunAnimFrame(ID) >= GetAllTime(ID); }
			float			GetTimePer(CharaGunAnimeID ID) { return (GetAllTime(ID) > 0.f) ? (GetGunAnimFrame(ID) / GetAllTime(ID)) : 1.f; }
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept;
			void			SetCancel() noexcept { m_Cancel = true; }
		public://ゲッター
			const auto& GetAnime(GunAnimeID anim) noexcept { return GetObj().get_anime((int)anim); }
			const bool	HaveFrame(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].first != -1; }
			const auto& GetFrame(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].first; }
			const auto& GetFrameBaseLocalMat(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].second; }
			const bool HasFrame(GunFrame frame) const noexcept;
			const MATRIX_ref GetFrameLocalMat(GunFrame frame) const noexcept;
			const MATRIX_ref GetFrameWorldMat(GunFrame frame, bool CheckSight = true) const noexcept;

			const auto		GetChildFrameNum(GunFrame frame) const noexcept { return ObjectBaseClass::GetChildFrameNum(GetFrame(frame)); }
			const auto		GetChildFrameWorldMat(GunFrame frame, int ID) const noexcept { return ObjectBaseClass::GetChildFrameWorldMatrix(GetFrame(frame), ID); }

			const auto&		GetAmmoLoadCount(void) const noexcept { return m_AmmoLoadCount; }
			const auto&		GetAmmoLoadSwitch(void) const noexcept { return m_AmmoLoadSwitch; }
		public:
			void ResetFrameLocalMat(GunFrame frame) noexcept;
			void SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept;
		private:
			const auto	GetGunSoundSet(void) const noexcept { return GunSoundSets[GetGunDataClass()->GetSoundSel()]; }
		public://ゲッター
			const auto& GetName(void) const noexcept { return GetGunDataClass()->GetName(); }
			const auto& GetReloadType(void) const noexcept { return GetGunDataClass()->GetReloadType(); }
			const auto& GetShotSwitch(void) const noexcept { return this->m_ShotSwitch; }
			const auto& GetShotPhase(void) const noexcept { return this->m_ShotPhase; }
			const auto& GetNowAnime(void) noexcept { return GetObj().get_anime((size_t)this->m_ShotPhase); }
			const auto&	GetAmmoNum(void) const noexcept { return this->m_Capacity; }
			const auto& GetMagazinePtr(void) const noexcept { return m_MagazinePtr; }
			const auto& GetRecoilPower(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModData()->GetIsRecoilPower()) {
					return (*this->m_UpperPtr)->GetModData()->GetRecoilPower();
				}
				return GetGunDataClass()->GetRecoilPower();
			}
			const auto& GetRecoilReturn(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModData()->GetIsRecoilReturn()) {
					return (*this->m_UpperPtr)->GetModData()->GetRecoilReturn();
				}
				return GetGunDataClass()->GetRecoilReturn();
			}

			const auto	GetSightMax() const noexcept { return (int)this->m_SightPtr.size(); }
			const bool	IsSightActive() const noexcept { return this->m_SightPtr.at(m_GunSightSel) != nullptr; }
			const bool	IsSightPtrActive() const noexcept {
				if (m_GunChangePer < 0.5f) {
					int Prev = (m_GunSightSel - 1);
					if (Prev < 0) { Prev = GetSightMax() - 1; }
					return this->m_SightPtr.at(Prev) != nullptr;

				}
				else {
					return this->m_SightPtr.at(m_GunSightSel) != nullptr;
				}
			}
			const auto&	GetSightPtr() const noexcept {
				if (m_GunChangePer < 0.5f) {
					int Prev = (m_GunSightSel - 1);
					if (Prev < 0) { Prev = GetSightMax() - 1; }
					return *this->m_SightPtr.at(Prev);

				}
				else {
					return *this->m_SightPtr.at(m_GunSightSel);
				}
			}
			void ChangeSightSel() noexcept {
				if (true) {
					m_GunSightSel++;
					if (m_GunSightSel >= GetSightMax()) {
						m_GunSightSel = 0;
					}
				}
				else {
					for (int i = 0;i < GetSightMax();i++) {
						m_GunSightSel++;
						if (m_GunSightSel >= GetSightMax()) {
							m_GunSightSel = 0;
						}
						if (IsSightActive()) {
							break;
						}
					}
				}
				m_GunChangePer = 0.f;
			}
			const bool	IsAutoAimActive() const noexcept { return ((m_GunSightSel == 1) && !this->m_SightPtr.at(1)); }

			const auto	GetInChamber(void) const noexcept { return this->m_ChamberAmmoData != nullptr; }
			const auto	GetAmmoAll(void) const noexcept {
				if (this->m_MagazinePtr) {
					return (*m_MagazinePtr)->GetModData()->GetAmmoAll();
				}
				return 0;
			}
			const auto	GetMagUniqueID(void) const noexcept {
				if (this->m_MagazinePtr) {
					return (*m_MagazinePtr)->GetModData()->GetUniqueID();
				}
				return GetModData()->GetUniqueID();
			}
			const auto	GetAmmoNumTotal(void) const noexcept { return this->m_Capacity + (GetInChamber() ? 1 : 0); }
			const auto	GetIsMagEmpty(void) const noexcept { return this->m_Capacity == 0; }//次弾がない
			const auto	GetIsMagFull(void) const noexcept { return this->m_Capacity == this->GetAmmoAll(); }
			const auto	GetShotType(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModData()->GetIsShotType()) {
					return (*this->m_UpperPtr)->GetModData()->GetShotType();
				}
				return GetGunDataClass()->GetShotType();
			}
			const auto	GetGunShootSound(void) const noexcept {
				if (this->m_MuzzlePtr) {
					return (*this->m_MuzzlePtr)->GetModData()->GetGunShootSound();
				}
				return GetGunDataClass()->GetGunShootSound();
			}
			const auto	GetHumanAnimType(void) const noexcept {
				if (this->m_LowerPtr && (*this->m_LowerPtr)->GetModData()->GetHumanAnimType() != -1) {
					return (*this->m_LowerPtr)->GetModData()->GetHumanAnimType();
				}
				return GetGunDataClass()->GetHumanAnimType();
			}
			//const auto&		GetHumanAnimType(void) const noexcept { return GetGunDataClass()->GetHumanAnimType(); }
			const auto&		GetGunAnimeSet(EnumGunAnimType ID) const noexcept { return this->m_GunAnimeSet.at(GetHumanAnimType()).at((int)ID).at(0); }
			float			GetAllTime(CharaGunAnimeID ID) { return (float)m_CharaAnimeSet.at(GetHumanAnimType()).at((int)ID); }

			const auto	GetShoting(void) const noexcept { return this->m_ShotPhase == GunAnimeID::Shot; }
			const auto	GetCocking(void) const noexcept { return this->m_ShotPhase == GunAnimeID::Cocking; }
			const auto	GetReloading(void) const noexcept { return (GunAnimeID::ReloadStart_Empty <= this->m_ShotPhase) && (this->m_ShotPhase <= GunAnimeID::ReloadEnd); }
			const auto	GetChecking(void) const noexcept { return (GunAnimeID::CheckStart <= this->m_ShotPhase) && (this->m_ShotPhase <= GunAnimeID::CheckEnd); }
			const auto	GetWatching(void) const noexcept { return (this->m_ShotPhase == GunAnimeID::Watch); }
			const auto	IsMelee(void) const noexcept { return (this->m_ShotPhase == GunAnimeID::Melee); }
			const auto	IsAmmoLoading(void) const noexcept { return (GunAnimeID::AmmoLoadStart <= this->m_ShotPhase) && (this->m_ShotPhase <= GunAnimeID::AmmoLoadEnd); }
			const auto	GetShootReady(void) const noexcept { return this->m_ShotPhase <= GunAnimeID::Shot; }
			const auto	GetCanShot(void) const noexcept { return GetInChamber() && GetShootReady(); }

			const auto	GetEyePos(void) const noexcept {
				VECTOR_ref Pos;
				if (this->m_SightPtr.at(1)) {
					int Prev = (m_GunSightSel - 1);
					if (Prev < 0) { Prev = GetSightMax() - 1; }
					Pos = Lerp(
						(*m_SightPtr[Prev])->GetFrameWorldMat(GunFrame::Eyepos).pos(),
						(*m_SightPtr[m_GunSightSel])->GetFrameWorldMat(GunFrame::Eyepos).pos(),
						m_GunChangePer);
				}
				else {
					if (!this->m_SightPtr.at(0)) {
						Pos = GetFrameWorldMat(GunFrame::Eyepos).pos();
					}
					else {
						Pos = (*m_SightPtr[0])->GetFrameWorldMat(GunFrame::Eyepos).pos();
					}
					if (HasFrame(GunFrame::EyeOffsetPos)) {
						if (m_GunSightSel == 0) {
							Pos = Lerp(GetFrameWorldMat(GunFrame::EyeOffsetPos).pos(), Pos, m_GunChangePer);
						}
						else {
							Pos = Lerp(Pos, GetFrameWorldMat(GunFrame::EyeOffsetPos).pos(), m_GunChangePer);
						}
					}
				}
				return Pos;
			}

			const auto	GetEyeYVec(void) const noexcept {
				VECTOR_ref Pos;
				if (this->m_SightPtr.at(1)) {
					int Prev = (m_GunSightSel - 1);
					if (Prev < 0) { Prev = GetSightMax() - 1; }
					Pos = Lerp(
						(*m_SightPtr[Prev])->GetFrameWorldMat(GunFrame::Eyepos).yvec(),
						(*m_SightPtr[m_GunSightSel])->GetFrameWorldMat(GunFrame::Eyepos).yvec(),
						m_GunChangePer);
				}
				else {
					if (!this->m_SightPtr.at(0)) {
						Pos = GetFrameWorldMat(GunFrame::Eyepos).yvec();
					}
					else {
						Pos = (*m_SightPtr[0])->GetFrameWorldMat(GunFrame::Eyepos).yvec();
					}
					if (HasFrame(GunFrame::EyeOffsetPos)) {
						VECTOR_ref vec = GetFrameWorldMat(GunFrame::EyeOffsetPos).yvec();
						if (GetChildFrameNum(GunFrame::EyeOffsetPos) > 0) {
							vec = (GetChildFrameWorldMat(GunFrame::EyeOffsetPos, 0).pos() - GetFrameWorldMat(GunFrame::EyeOffsetPos).pos()).Norm();
						}
						if (m_GunSightSel == 0) {
							Pos = Lerp(vec, Pos, m_GunChangePer);
						}
						else {
							Pos = Lerp(Pos, vec, m_GunChangePer);
						}
					}
				}
				return Pos;
			}

			const auto	GetLensPos(void) const noexcept { return GetFrameWorldMat(GunFrame::Lens).pos(); }
			const auto	GetReticlePos(void) const noexcept { return GetLensPos() + (GetLensPos() - GetEyePos()).Norm()*10.f; }

			const auto	GetCartMat(void) noexcept { return GetFrameWorldMat(GunFrame::Cart); }
			const auto	GetCartVec(void) noexcept { return (GetFrameWorldMat(GunFrame::CartVec).pos() - GetCartMat().pos()).Norm(); }

		public:
			void		SetAmmo(int value) noexcept { this->m_Capacity = std::clamp(value, 0, this->GetAmmoAll()); }
			void		CockByMag() noexcept { this->m_ChamberAmmoData = (*m_MagazinePtr)->GetModData()->GetAmmoSpecMagTop(); }//マガジンの一番上の弾データをチャンバーイン
			void		UnloadChamber() noexcept { this->m_ChamberAmmoData.reset(); }
			void		SetShotSwitchOff() noexcept { this->m_ShotSwitch = false; }
			void		SetGunMatrix(const MATRIX_ref& value) noexcept {
				SetMove(value.GetRot(), value.pos());
				ModSlotControl::UpdatePartsAnim(GetObj());
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat(GunFrame::UnderRail), GunSlot::UnderRail);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat(GunFrame::Lower), GunSlot::Lower);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat(GunFrame::Upper), GunSlot::Upper);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat(GunFrame::Barrel), GunSlot::Barrel);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat(GunFrame::Sight), GunSlot::Sight);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat(GunFrame::Magpos), GunSlot::Magazine);
			}
			void		SetShotPhase(GunAnimeID pShotPhase) noexcept { this->m_ShotPhase = pShotPhase; }
			void		SetBullet(void) noexcept;//発砲
			void		SetMagFall() noexcept {
				m_MagFall.SetFall(
					GetFrameWorldMat(GunFrame::Magpos).pos(),
					GetFrameWorldMat(GunFrame::Magpos).GetRot(),
					GetFrameWorldMat(GunFrame::Magpos).yvec()*-1.f*(Scale_Rate * 3.f / 60.f),
					12.f, SoundEnum::MagFall);
			}

			void		SetPressShotEmpty() noexcept {
				this->m_ShotPhase = (!GetIsMagEmpty()) ? GunAnimeID::Cocking : GunAnimeID::ReloadStart_Empty;
			}
			void		SetReloadStartMag(int NextMagNum, int NextMagUniqueID) noexcept {
				if (this->GetReloadType() != RELOADTYPE::MAG) { return; }
				this->m_NextMagNum = NextMagNum;
				this->m_NextMagUniqueID = NextMagUniqueID;
				this->m_ShotPhase = (!GetIsMagEmpty()) ? GunAnimeID::ReloadStart : GunAnimeID::ReloadStart_Empty;
			}
			void		SetReloadStartAmmo(int NextMagNum) noexcept {
				if (this->GetReloadType() != RELOADTYPE::AMMO) { return; }
				this->m_NextMagNum = NextMagNum;
				this->m_NextMagUniqueID = 0;
				this->m_ShotPhase = (!GetIsMagEmpty()) ? GunAnimeID::ReloadStart : GunAnimeID::ReloadStart_Empty;
			}

			void		UpdateGunAnims(bool PressShot) noexcept;
		private:
			void		ExecuteCartInChamber(void) noexcept;//チャンバーへの装弾、排出
		public:
			GunClass(void) noexcept { this->m_objType = ObjType::Gun; }
			~GunClass(void) noexcept {}
		public:
			void			Init(void) noexcept override;
			void			Init_Gun(void) noexcept;
			void			FirstExecute(void) noexcept override;
			void			DrawShadow(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			Dispose(void) noexcept override;
		};
	};
};
