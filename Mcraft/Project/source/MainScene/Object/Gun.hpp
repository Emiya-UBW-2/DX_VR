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
		struct AnimBlendParam {
			float OneOver = 0.f;//上げる際にオーバーする距離
			float ZeroOver = 0.f;//下げる際にオーバーする距離
			float OneEasingSpeed = 0.8f;//上げる際の速度
			float ZeroEasingSpeed = 0.8f;//下げる際の速度
		};
		class GunClass : public ObjectBaseClass , public std::enable_shared_from_this<GunClass> {
		private:
			MuzzleSmokeControl									m_MuzzleSmokeControl{};
			ModSlotControl										m_ModSlotControl{};
			AutoAimControl										m_AutoAimControl;

			ScreenPosition										m_AimPoint{};
			ScreenPosition										m_Reticle;
			ScreenPosition										m_Lens;
			ScreenPosition										m_LensSize;
			bool												m_Reticle_on = false;
			float												m_LensSizeLen{10000};

			FallControl											m_Grenade;
			std::shared_ptr<AmmoDataClass>						m_ChamberAmmoData{ nullptr };		//
			std::array<ArmMovePerClass, static_cast<int>(GunAnimeID::ChoiceOnceMax)>	m_GunAnimePer{};
			std::array<float, static_cast<int>(GunAnimeID::Max)>						m_GunAnimeTime{};
			std::array<float, static_cast<int>(GunAnimeID::Max)>						m_GunAnimeSpeed{};
			GunAnimeID											m_GunAnime{ GunAnimeID::None };	//
			bool												m_ReloadAmmoCancel{ false };		//
			bool												m_ShotEnd{ false };		//
			EnumGunSound										m_EnumGunSoundNow{ EnumGunSound::Max };			//サウンド
			bool												m_IsChamberOn{ false };				//チャンバーに弾を込めるか
			bool												m_IsEject{ false };
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
			Matrix3x3DX											m_GrenadeThrowRot{};
			bool												m_isMagFall{};
			bool												m_MagHand{ false };
			ArmMovePerClass										m_MagArm;
			Vector3DX											m_UpperPrevRad, m_UpperRad;
			Matrix3x3DX											m_GunSwingMat, m_GunSwingMat2;
			Vector2DX											m_RecoilRadAdd;
			float												m_SwitchPer{};
			float												m_SlingPer{};
			Matrix3x3DX											m_SlingRot{};
			Vector3DX											m_SlingPos{};
		private:
		private:
			const auto			GetGunSoundSet(EnumGunSound Select) const noexcept { return static_cast<int>(GunSoundSets[GetModSlot().GetModData()->GetSoundSel()].m_Sound.at(static_cast<int>(Select))); }
			void				PlayGunSound(EnumGunSound Select) noexcept {
				auto* SE = SoundPool::Instance();
				if (this->m_EnumGunSoundNow != Select) {
					this->m_EnumGunSoundNow = Select;
					SE->Get(SoundType::SE, GetGunSoundSet(Select))->Play3D(GetMove().GetPos(), Scale3DRate * 10.f);
				}
			}
			const auto&			GetGunAnim(GunAnimeID AnimID) const noexcept {
				int HumanAnimType = 0;
				if (this->m_LowerPtr && (*this->m_LowerPtr)->GetModSlot().GetModData()->GetHumanAnimType() != -1) {
					HumanAnimType = (*this->m_LowerPtr)->GetModSlot().GetModData()->GetHumanAnimType();
				}
				else {
					HumanAnimType = GetModSlot().GetModData()->GetHumanAnimType();
				}

				return GunAnimeSets[HumanAnimType].Anim.at(static_cast<int>(AnimID));
			}
			const auto&			GetRecoilReturn(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModSlot().GetModData()->GetIsRecoilReturn()) {
					return (*this->m_UpperPtr)->GetModSlot().GetModData()->GetRecoilReturn();
				}
				return GetModSlot().GetModData()->GetRecoilReturn();
			}
			const auto			GetRecoilPower(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModSlot().GetModData()->GetIsRecoilPower()) {
					return (*this->m_UpperPtr)->GetModSlot().GetModData()->GetRecoilPower();
				}
				return GetModSlot().GetModData()->GetRecoilPower() + this->m_Recoil_Diff;
			}
			void				ChamberIn(void) noexcept {
				if (!this->m_IsChamberOn) {
					this->m_IsChamberOn = true;
					if (this->m_Capacity != 0) {
						this->m_Capacity--;
						if (this->m_MagazinePtr) {
							this->m_ChamberAmmoData = (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoSpecMagTop();//マガジンの一番上の弾データをチャンバーイン
						}
					}
				}
			}
			void				EjectCart(void) noexcept {
				if (!this->m_IsEject) {
					this->m_IsEject = true;
					this->m_CartFall.SetFall(
						GetFrameWorldMatParts(GunFrame::Cart).pos(), GetMove().GetMat(),
						(GetFrameWorldMatParts(GunFrame::CartVec).pos() - GetFrameWorldMatParts(GunFrame::Cart).pos() + Vector3DX::vget(GetRandf(0.2f), 0.5f + GetRandf(1.f), GetRandf(0.2f))).normalized() * (Scale3DRate * 2.f / 60.f), 2.f, FallObjectType::Cart);
				}
			}
			void				ExecuteGrenade(void) noexcept;
			const auto			GetAnimDataNow(GunAnimeID ID) const noexcept {
				auto* AnimMngr = GunAnimManager::Instance();
				return AnimMngr->GetAnimNow(AnimMngr->GetAnimData(GetGunAnim(ID)), this->m_GunAnimeTime.at(static_cast<int>(ID)));
			}
			const auto			GetNowGunAnimeID(void) const noexcept {
				if (GetGunAnime() == GunAnimeID::None) { return -1; }
				return GetModSlot().GetModData()->GetAnimSelectList().at(static_cast<int>(GetGunAnime()));
			}
		public://ゲッター
			const ModSlotControl&GetModSlot(void) const noexcept { return this->m_ModSlotControl; }
			const GunAnimeID&	GetGunAnime(void) const noexcept { return this->m_GunAnime; }
			const Matrix4x4DX	GetFrameWorldMatParts(GunFrame frame) const noexcept {
				auto* ptr = GetModSlot().HasFrameBySlot(frame);
				if (ptr) {
					return ((std::shared_ptr<ModClass>&)(*ptr))->GetFramePartsMat(frame);
				}
				if (HaveFrame((int)frame)) {
					return GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame)));
				}
				return Matrix4x4DX::identity();
			}
			const auto&			GetMyUserPlayerID(void) const noexcept { return this->m_MyID; }
			const auto&			GetSwitchPer(void) const noexcept { return this->m_SwitchPer; }
			const auto&			GetAutoAimID(void) const noexcept { return this->m_AutoAimControl.GetAutoAimID(); }
			const auto			GetNowAnimTimePerCache(void) const noexcept {
				if (GetGunAnime() < GunAnimeID::Max) {
					auto* AnimMngr = GunAnimManager::Instance();
					auto* pData = AnimMngr->GetAnimData(GetGunAnim(GetGunAnime()));
					if (pData) {
						float totalTime = (float)pData->GetTotalTime();
						return (totalTime > 0.f) ? std::clamp(this->m_GunAnimeTime.at(static_cast<int>(GetGunAnime())) / totalTime, 0.f, 1.f) : 1.f;
					}
				}
				return 1.f;
			}
			const auto&			GetShotType(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModSlot().GetModData()->GetIsShotType()) {
					return (*this->m_UpperPtr)->GetModSlot().GetModData()->GetShotType();
				}
				return GetModSlot().GetModData()->GetShotType();
			}
			const auto			IsNeedCalcSling(void) const noexcept { return this->m_SlingPer < 1.f; }
			const auto			GetADSEyeMat(void) const noexcept {
				if (this->m_SightPtr) {
					return (*this->m_SightPtr)->GetFramePartsMat(GunFrame::Eyepos);
				}
				return GetFrameWorldMatParts(GunFrame::Eyepos);
			}
			const auto			GetAimXPos(void) const noexcept { return this->m_AimPoint.XScreenPos(); }
			const auto			GetAimYPos(void) const noexcept { return this->m_AimPoint.YScreenPos(); }
			const auto			GetAutoAimActive(void) const noexcept { return this->m_AutoAimControl.GetAutoAimActive(); }
			const auto			GetAutoAimRadian(void) const noexcept { return deg2rad(5) * std::clamp(100.f / std::max(0.01f, std::hypotf((float)(GetAimXPos() - 1920 / 2), (float)(GetAimYPos() - 1080 / 2))), 0.f, 1.f); }
			const auto			GetGunAnimBlendPer(GunAnimeID ID) const noexcept { return this->m_GunAnimePer[static_cast<int>(ID)].Per(); }
			const auto			GetCanShot(void) const noexcept { return GetGunAnimBlendPer(GunAnimeID::LowReady) <= 0.1f; }
			const auto			IsCanShot(void) const noexcept { return GetGunAnime() == GunAnimeID::None || GetGunAnime() == GunAnimeID::Shot; }
			const auto			GetCanADS(void) const noexcept { return IsCanShot() && GetCanShot() && GetModSlot().GetModData()->GetCanADS(); }
			const auto			GetSightZoomSize(void) const noexcept { return this->m_SightPtr ? (*this->m_SightPtr)->GetModSlot().GetModData()->GetSightZoomSize() : 1.f; }
			const auto			GetAmmoAll(void) const noexcept { return this->m_MagazinePtr ? (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoAll() : 0; }
			const auto			GetReloadType(void) const noexcept { return GetModSlot().GetModData()->GetReloadType(); }
			const auto			GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd * (60.f * DXLib_ref::Instance()->GetDeltaTime()); }
			const auto			GetReloading(void) const noexcept { return (GunAnimeID::ReloadStart_Empty <= GetGunAnime()) && (GetGunAnime() <= GunAnimeID::ReloadEnd); }
			const auto			GetPelletNum(void) const noexcept { return (this->m_ChamberAmmoData) ? this->m_ChamberAmmoData->GetPellet() : 0; }
			const auto			GetRightHandMat(void) const noexcept {
				Vector3DX GunPos = GetFrameWorldMatParts(GunFrame::RightHandPos).pos();
				Vector3DX Gunyvec = GetFrameWorldMatParts(GunFrame::RightHandYvec).pos() - GunPos;
				Vector3DX Gunzvec = GetFrameWorldMatParts(GunFrame::RightHandZvec).pos() - GunPos;
				return Matrix4x4DX::Axis1(Gunyvec.normalized(), Gunzvec.normalized(), GunPos);
			}
			const auto			GetBaseLeftHandMat(void) const noexcept {
				Vector3DX HandPos = GetFrameWorldMatParts(GunFrame::LeftHandPos).pos();
				Vector3DX Handyvec = GetFrameWorldMatParts(GunFrame::LeftHandYvec).pos() - HandPos;
				Vector3DX Handzvec = GetFrameWorldMatParts(GunFrame::LeftHandZvec).pos() - HandPos;
				return Matrix4x4DX::Axis1(Handyvec.normalized(), Handzvec.normalized(), HandPos);
			}
			const auto			GetMagHandMat(void) const noexcept {
				if (this->m_MagazinePtr) {
					Vector3DX MagPos = (*this->m_MagazinePtr)->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
					Vector3DX Magyvec = (*this->m_MagazinePtr)->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - MagPos;
					Vector3DX Magzvec = (*this->m_MagazinePtr)->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - MagPos;
					return Matrix4x4DX::Axis1(Magyvec.normalized(), Magzvec.normalized(), MagPos);
				}
				return Matrix4x4DX::identity();
			}
			const auto			GetLeftHandMat(void) const noexcept { return Lerp(GetBaseLeftHandMat(), GetMagHandMat(), this->m_MagArm.Per()); }
			const auto			GetAmmoNumTotal(void) const noexcept { return this->m_Capacity + ((this->m_ChamberAmmoData) ? 1 : 0); }
			const auto			GetNowGunAnimePer(void) const noexcept {
				if (GetGunAnime() >= GunAnimeID::Max) {
					return 1.f;
				}
				auto* AnimMngr = GunAnimManager::Instance();
				auto* pData = AnimMngr->GetAnimData(GetGunAnim(GetGunAnime()));
				if (!pData) { return 0.f; }
				float totalTime = (float)pData->GetTotalTime();
				return (totalTime > 0.f) ? std::clamp(this->m_GunAnimeTime.at(static_cast<int>(GetGunAnime())) / totalTime, 0.f, 1.f) : 1.f;
			}
			const auto			IsNowGunAnimeEnd(void) const noexcept { return GetNowAnimTimePerCache() >= 1.f; }
			const auto			GetShotSwitch(void) const noexcept { return GetGunAnime() == GunAnimeID::Shot && (GetNowAnimTimePerCache() < 0.5f); }
			const auto			GetGunAnimeFingerNow(void) const noexcept {
				FingerData	Finger = GetAnimDataNow(GunAnimeID::Base).GetFingerPer();
				for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
					Finger = Lerp(Finger, GetAnimDataNow((GunAnimeID)loop).GetFingerPer(), this->m_GunAnimePer[loop].Per());
				}
				return Finger;
			}
		public:
			void				SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
			auto&				SetModSlot(void) noexcept { return this->m_ModSlotControl; }
			void				CalcSwitchPer(bool IsRight) noexcept {
				if (GetModSlot().GetModData()->GetCanSwitch()) {
					Easing(&this->m_SwitchPer, IsRight ? 1.f : -1.f, 0.9f, EasingType::OutExpo);
				}
				else {
					this->m_SwitchPer = 1.f;
				}
			}
			void				SetSlingMat(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept {
				this->m_SlingRot = rotation;
				this->m_SlingPos = pos;
			}
			void				SetReloadAmmoCancel(void) noexcept { this->m_ReloadAmmoCancel = true; }
			void				SetShotEnd(bool value) noexcept { this->m_ShotEnd = value; }
			void				SetGunAnime(GunAnimeID GunAnime) noexcept {
				this->m_GunAnime = GunAnime;
				if (GetGunAnime() < GunAnimeID::Max) {
					this->m_GunAnimeTime.at((int)GetGunAnime()) = 0.f;
				}
			}
			void				SetShotStart(void) noexcept;//発砲
			void				ReloadStart(void) noexcept {
				if (GetModSlot().GetModData()->GetIsThrowWeapon()) {
					return;
				}
				if (this->m_Capacity == this->GetAmmoAll()) { return; }//リロードの必要がある場合のみリロード
				if (this->m_Capacity != 0) {
					SetGunAnime(GunAnimeID::ReloadStart);
				}
				else {
					SetGunAnime(GunAnimeID::ReloadStart_Empty);
				}
			}
			void				SetActiveAll(bool value) noexcept {
				if (value != IsActive()) {
					SetModSlot().CalcAnyBySlot([&](const SharedObj& ptr) { if (ptr) { ptr->SetActive(value); } });
					SetActive(value);
				}
			}
			void				SetMagazinePoachMat(const Matrix4x4DX& MagPoachMat) noexcept { this->m_MagazinePoachMat = MagPoachMat; }
			void				SetGrenadeThrowRot(const Matrix3x3DX& Rot) noexcept { this->m_GrenadeThrowRot = Rot; }
			void				DrawReticle(float Rad) const noexcept {
				if (this->m_Reticle_on && this->m_SightPtr && ((GetGunAnimBlendPer(GunAnimeID::ADS) >= 0.8f) || (GetSightZoomSize() == 1.f))) {
					auto* DrawCtrls = WindowSystem::DrawControl::Instance();
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &(*this->m_SightPtr)->GetModSlot().GetModData()->GetReitcleGraph(),
						this->m_Reticle.XScreenPos(), this->m_Reticle.YScreenPos(), 1.f, Rad, true);
				}
			}
			void				Spawn(void) noexcept {
				this->m_MagHand = false;
				this->m_MagArm.Init(this->m_MagHand);
				this->m_SlingPer = 1.f;
				this->m_IsChamberOn = false;
				SetGunAnime(GunAnimeID::None);
				this->m_Capacity = GetAmmoAll();//マガジン装填
				ChamberIn();
			}

			void				InitGunAnimePer() noexcept;
			void				UpdateGunAnimePer(bool IsADS) noexcept;
			void				SetGunMat(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept {
				//武器座標
				SetMove().SetMat(rotation);
				SetMove().SetPos(pos);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				SetModSlot().UpdatePartsAnim(GetObj());
				SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::UnderRail), GunSlot::UnderRail);
				SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Lower), GunSlot::Lower);
				SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Upper), GunSlot::Upper);
				SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Barrel), GunSlot::Barrel);
				SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Sight), GunSlot::Sight);
				SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Magpos), GunSlot::Magazine);
			}
			void				UpdateGunMat(bool IsSelGun, bool IsActiveAutoAim, const Matrix3x3DX& CharaRotationCache, const Vector3DX& HeadPos, const Vector3DX& RotRad) noexcept;
		public:
			GunClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::Gun); }
			~GunClass(void) noexcept {}
		private:
			int					GetFrameNum(void) noexcept override { return static_cast<int>(GunFrame::Max); }
			const char*			GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
		private: //継承
			void				Init_Sub(void) noexcept override;
			void				FirstExecute(void) noexcept override;
			void				DrawShadow(void) noexcept override;
			void				CheckDraw_Sub(int) noexcept override;
			void				Draw(bool isDrawSemiTrans, int Range) noexcept override;
			void				Dispose_Sub(void) noexcept override {
				SetModSlot().Dispose();
				this->m_MagFall.Dispose();
				this->m_CartFall.Dispose();
				this->m_Grenade.Dispose();
			}
		};
	};
};
