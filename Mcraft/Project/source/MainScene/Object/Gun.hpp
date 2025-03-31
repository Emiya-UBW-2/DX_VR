#pragma once
#include	"../../Header.hpp"

#include "GunEnum.hpp"
#include "ModData.hpp"
#include "Gun_before.hpp"
#include "CharaAnimData.hpp"

#include "../../sub.hpp"
#include "Ammo.hpp"
#include "Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass : public ObjectBaseClass , public std::enable_shared_from_this<GunClass> {
		private:
			MuzzleSmokeControl									m_MuzzleSmokeControl{};
			AimPointControl										m_AimPointControl{};
			ReticleControl										m_ReticleControl{};
			ModSlotControl										m_ModSlotControl{};

			std::shared_ptr<AmmoDataClass>						m_ChamberAmmoData{ nullptr };		//
			std::array<ArmMovePerClass, static_cast<int>(GunAnimeID::ChoiceOnceMax)>	m_GunAnimePer{};
			std::array<float, static_cast<int>(GunAnimeID::Max)>						m_GunAnimeTime{};
			bool												m_IsGunAnimChange{};
			GunAnimeID										m_GunAnime{ GunAnimeID::Shot };	//
			bool												m_ReloadAmmoCancel{ false };		//
			bool												m_ShotEnd{ false };		//
			int													m_boltSoundSequence{ -1 };			//サウンド
			bool												m_IsChamberOn{ false };				//チャンバーに弾を込めるか
			bool												m_PrevChamberOn{ false };
			bool												m_IsEject{ false };
			bool												m_PrevEject{ false };
			FallControl											m_MagFall;
			FallControl											m_CartFall;
			int													m_GunSightSel{ 0 };
			const std::shared_ptr<SightClass>*					m_SightPtr{ nullptr };
			const std::shared_ptr<MuzzleClass>*					m_MuzzlePtr{ nullptr };
			const std::shared_ptr<UpperClass>*					m_UpperPtr{ nullptr };
			const std::shared_ptr<LowerClass>*					m_LowerPtr{ nullptr };
			const std::shared_ptr<MagazineClass>*				m_MagazinePtr{ nullptr };
			int													m_ShootRate_Diff{ 0 };
			int													m_Recoil_Diff{ 0 };
			int													m_Capacity{ 0 };//弾数
			bool												m_isMagSuccess{};
			PlayerID											m_MyID{ 0 };
			Matrix4x4DX											m_MagMiss{}, m_MagSuccess{};
			Matrix4x4DX											m_MagazinePoachMat{};
			bool												m_isMagFall{};
			bool												m_ShotSwitch{ false };				//
			bool												m_MagHand{ false };
			ArmMovePerClass										m_MagArm;
		public:
			Matrix3x3DX							m_SlingRot{};
			Vector3DX							m_SlingPos{};
			float								m_SlingPer{};
		public:
			const auto&			GetMyUserPlayerID(void) const noexcept { return this->m_MyID; }
			void				SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }

			ModSlotControl& SetModSlot(void) noexcept { return this->m_ModSlotControl; }
			const ModSlotControl& GetModSlot(void) const noexcept { return this->m_ModSlotControl; }
		private:
			const auto			GetGunSoundSet(void) const noexcept { return GunSoundSets[GetModSlot().GetModData()->GetSoundSel()]; }
			const auto&			GetGunAnim(GunAnimeID AnimID) const noexcept {
				int ID = 0;
				if (this->m_LowerPtr && (*this->m_LowerPtr)->GetModSlot().GetModData()->GetHumanAnimType() != -1) {
					ID = (*this->m_LowerPtr)->GetModSlot().GetModData()->GetHumanAnimType();
				}
				else {
					ID = GetModSlot().GetModData()->GetHumanAnimType();
				}

				return GunAnimeSets[ID].Anim.at(static_cast<int>(AnimID));
			}
			const auto&			GetGunShootSound(void) const noexcept { return (this->m_MuzzlePtr) ? (*this->m_MuzzlePtr)->GetModSlot().GetModData()->GetGunShootSound() : GetModSlot().GetModData()->GetGunShootSound(); }
		public://ゲッター
			const Matrix4x4DX	GetFrameWorldMatParts(GunFrame frame) const noexcept;
			const auto			GetAimXPos(void) const noexcept { return this->m_AimPointControl.GetAimXPos(); }
			const auto			GetAimYPos(void) const noexcept { return this->m_AimPointControl.GetAimYPos(); }
			const auto			GetRecoilPower(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModSlot().GetModData()->GetIsRecoilPower()) {
					return (*this->m_UpperPtr)->GetModSlot().GetModData()->GetRecoilPower();
				}
				return GetModSlot().GetModData()->GetRecoilPower() + this->m_Recoil_Diff;
			}
			const auto&			GetRecoilReturn(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModSlot().GetModData()->GetIsRecoilReturn()) {
					return (*this->m_UpperPtr)->GetModSlot().GetModData()->GetRecoilReturn();
				}
				return GetModSlot().GetModData()->GetRecoilReturn();
			}
			const auto&			GetShotType(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModSlot().GetModData()->GetIsShotType()) {
					return (*this->m_UpperPtr)->GetModSlot().GetModData()->GetShotType();
				}
				return GetModSlot().GetModData()->GetShotType();
			}
			const auto			GetCanADS(void) const noexcept { return IsCanShoot() && GetCanShot() && GetModSlot().GetModData()->GetCanADS(); }
			const auto			GetSightZoomSize(void) const noexcept { return this->m_SightPtr ? (*this->m_SightPtr)->GetModSlot().GetModData()->GetSightZoomSize() : 1.f; }
			const auto			GetAmmoAll(void) const noexcept { return this->m_MagazinePtr ? (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoAll() : 0; }
			const auto			GetReloadType(void) const noexcept { return GetModSlot().GetModData()->GetReloadType(); }

			const auto&			GetShotSwitch(void) const noexcept { return this->m_ShotSwitch; }
			const auto&			GetGunAnime(void) const noexcept { return this->m_GunAnime; }
			const auto			GetGunAnimeID(GunAnimeID Select) const noexcept {
				if (Select == GunAnimeID::None) {
					return -1;
				}
				return GetModSlot().GetModData()->GetAnimSelectList().at(static_cast<int>(Select));
			}
			const auto			GetNowGunAnimeID(void) const noexcept { return GetGunAnimeID(GetGunAnime()); }

			const auto			GetReloading(void) const noexcept { return (GunAnimeID::ReloadStart_Empty <= GetGunAnime()) && (GetGunAnime() <= GunAnimeID::ReloadEnd); }
			const bool			IsCanShoot(void) const noexcept {
				switch (GetGunAnime()) {
				case GunAnimeID::None:
				case GunAnimeID::Shot:
					return true;
					break;
				default:
					return false;
				}
			}


			const auto			CanShootAmmo(void) const noexcept { return this->m_ChamberAmmoData != nullptr; }
			const auto			GetPelletNum(void) const noexcept { return CanShootAmmo() ? this->m_ChamberAmmoData->GetPellet() : 0; }
			const Matrix4x4DX	GetRightHandMat(void) const noexcept {
				Vector3DX GunPos = GetFrameWorldMatParts(GunFrame::RightHandPos).pos();
				Vector3DX Gunyvec = GetFrameWorldMatParts(GunFrame::RightHandYvec).pos() - GunPos;
				Vector3DX Gunzvec = GetFrameWorldMatParts(GunFrame::RightHandZvec).pos() - GunPos;
				return Matrix4x4DX::Axis1(Gunyvec.normalized(), Gunzvec.normalized(), GunPos);
			}

			const Matrix4x4DX	GetBaseLeftHandMat(void) const noexcept {
				Vector3DX HandPos = GetFrameWorldMatParts(GunFrame::LeftHandPos).pos();
				Vector3DX Handyvec = GetFrameWorldMatParts(GunFrame::LeftHandYvec).pos() - HandPos;
				Vector3DX Handzvec = GetFrameWorldMatParts(GunFrame::LeftHandZvec).pos() - HandPos;
				return Matrix4x4DX::Axis1(Handyvec.normalized(), Handzvec.normalized(), HandPos);
			}
			const Matrix4x4DX	GetMagHandMat(void) const noexcept {
				if (this->m_MagazinePtr) {
					Vector3DX MagPos = (*this->m_MagazinePtr)->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
					Vector3DX Magyvec = (*this->m_MagazinePtr)->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - MagPos;
					Vector3DX Magzvec = (*this->m_MagazinePtr)->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - MagPos;
					return Matrix4x4DX::Axis1(Magyvec.normalized(), Magzvec.normalized(), MagPos);
				}
				return Matrix4x4DX::identity();
			}
			const Matrix4x4DX	GetLeftHandMat(void) const noexcept { return Lerp(GetBaseLeftHandMat(), GetMagHandMat(), m_MagArm.Per()); }

			const auto			GetAmmoNumTotal(void) const noexcept { return this->m_Capacity + (CanShootAmmo() ? 1 : 0); }
			const auto			GetIsMagEmpty(void) const noexcept { return this->m_Capacity == 0; }//次弾がない

			const auto			GetGunAnimBlendPer(GunAnimeID ID) const noexcept { return this->m_GunAnimePer[static_cast<int>(ID)].Per(); }
			const auto			GetEyeYRot(void) const noexcept {
				Vector3DX BaseVec = GetMove().GetMat().yvec();
				return Matrix3x3DX::RotVec2(Lerp(BaseVec, GetFrameWorldMatParts(GunFrame::Eyepos).yvec(), GetGunAnimBlendPer(GunAnimeID::ADS)), BaseVec);
			}
			//銃の位置を指定するアニメ
			const auto			GetGunAnimeTime(GunAnimeID ID) const noexcept { return this->m_GunAnimeTime.at(static_cast<int>(ID)); }
			const auto			GetGunAnimePer(GunAnimeID ID) {
				auto* AnimMngr = GunAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GetGunAnim(ID));
				if (!Ptr) { return 0.f; }
				float totalTime = (float)Ptr->GetTotalTime();
				return (totalTime > 0.f) ? std::clamp(GetGunAnimeTime(ID) / totalTime, 0.f, 1.f) : 1.f;
			}
			const auto			GetGunAnimeNow(void) const noexcept {
				auto* AnimMngr = GunAnimManager::Instance();
				Matrix4x4DX AnimMat;
				{
					auto* Ptr = AnimMngr->GetAnimData(GetGunAnim(GunAnimeID::Base));
					if (Ptr) {
						AnimMat = AnimMngr->GetAnimNow(Ptr, GetGunAnimeTime(GunAnimeID::Base)).GetMatrix();
					}
				}
				for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
					auto* Ptr = AnimMngr->GetAnimData(GetGunAnim((GunAnimeID)loop));
					if (!Ptr) { continue; }
					AnimMat = Lerp(AnimMat, AnimMngr->GetAnimNow(Ptr, GetGunAnimeTime((GunAnimeID)(loop))).GetMatrix(), this->m_GunAnimePer[loop].Per());
				}
				return AnimMat;
			}
			bool				GetCanShot(void) const noexcept { return GetGunAnimBlendPer(GunAnimeID::LowReady) <= 0.1f; }
			void				UpdateSlingPer(bool IsSelGun) noexcept;
			void				UpdateGunAnime(bool IsADS, bool IsForce) noexcept;
		public:
			void				SetReloadAmmoCancel(void) noexcept { this->m_ReloadAmmoCancel = true; }
			void				SetShotEnd(bool value) noexcept { this->m_ShotEnd = value; }
			void				SetGunAnime(GunAnimeID GunAnime) noexcept {
				this->m_GunAnime = GunAnime;
				if (GetGunAnime() < GunAnimeID::Max) {
					this->m_GunAnimeTime.at((int)GetGunAnime()) = 0.f;
					this->m_IsGunAnimChange = true;
				}
			}
			void				SetShotStart(void) noexcept;//発砲
			void				ReloadStart(void) noexcept {
				if (this->m_Capacity != this->GetAmmoAll()) {//リロードの必要がある場合のみリロード
					SetGunAnime((!GetIsMagEmpty()) ? GunAnimeID::ReloadStart : GunAnimeID::ReloadStart_Empty);
				}
			}
			void				SetActiveAll(bool value) noexcept;
			void				SetMagazinePoachMat(const Matrix4x4DX& MagPoachMat) noexcept { this->m_MagazinePoachMat = MagPoachMat; }
			void				UpdateGunMat(const Matrix3x3DX& rotation, const Vector3DX& pos, bool IsSelGun) noexcept;
			void				DrawReticle(float Rad) const noexcept {
				if (this->m_ReticleControl.IsActiveReticle() && this->m_SightPtr && ((GetGunAnimBlendPer(GunAnimeID::ADS) >= 0.8f) || (GetSightZoomSize() == 1.f))) {
					auto* WindowParts = WindowSystem::DrawControl::Instance();
					WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &(*this->m_SightPtr)->GetModSlot().GetModData()->GetReitcleGraph(),
						static_cast<int>(this->m_ReticleControl.GetReticleXPos()), static_cast<int>(this->m_ReticleControl.GetReticleYPos()), 1.f, Rad, true);
				}
			}
			void				Spawn() noexcept {
				this->m_MagHand = false;
				this->m_MagArm.Init(this->m_MagHand);
				m_SlingPer = 1.f;
				SetGunAnime(GunAnimeID::None);
			}
		public:
			GunClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Gun); }
			~GunClass(void) noexcept {}
		private:
			int					GetFrameNum() noexcept override { return static_cast<int>(GunFrame::Max); }
			const char*			GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
		private: //継承
			void				Init_Sub(void) noexcept override;
			void				FirstExecute(void) noexcept override;
			void				DrawShadow(void) noexcept override;
			void				CheckDraw(void) noexcept override {
				ObjectBaseClass::CheckDraw();
				if (GetMyUserPlayerID() == 0) {
					auto* PostPassParts = PostPassEffect::Instance();
					if (!GetCanShot()) {
						return;
					}
					this->m_AimPointControl.UpdateAimPointControl(GetFrameWorldMatParts(GunFrame::Muzzle).pos() + GetMove().GetMat().zvec() * (-50.f * Scale3DRate));
					Vector3DX LensPos = GetFrameWorldMatParts(GunFrame::Lens).pos();
					this->m_ReticleControl.UpdateReticleControl(LensPos, GetFrameWorldMatParts(GunFrame::LensSize).pos(), LensPos + (LensPos - GetFrameWorldMatParts(GunFrame::Eyepos).pos()).normalized() * (5.f * Scale3DRate));
					//レンズ表示機能への反映
					if (this->m_ReticleControl.IsActiveReticle() && GetSightZoomSize() > 1.f) {
						PostPassParts->Set_is_lens(true);
						PostPassParts->Set_zoom_lens(std::max(1.f, GetSightZoomSize() / 2.f));
						PostPassParts->Set_xp_lens(this->m_ReticleControl.GetLensXPos());
						PostPassParts->Set_yp_lens(this->m_ReticleControl.GetLensYPos());
						PostPassParts->Set_size_lens(this->m_ReticleControl.GetLensSize());
					}
					else if (GetGunAnimBlendPer(GunAnimeID::ADS) < 0.5f) {
						PostPassParts->Set_is_lens(false);
						PostPassParts->Set_zoom_lens(1.f);
					}
				}
			}
			void				Draw(bool isDrawSemiTrans) noexcept override;
			void				Dispose_Sub(void) noexcept override {
				SetModSlot().DisposeModSlotControl();
				this->m_MagFall.Dispose();
				this->m_CartFall.Dispose();
			}
		};
	};
};
