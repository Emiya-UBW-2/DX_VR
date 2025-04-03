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
			bool												m_IsGunAnimChange{};
			GunAnimeID											m_GunAnime{ GunAnimeID::Shot };	//
			bool												m_ReloadAmmoCancel{ false };		//
			bool												m_ShotEnd{ false };		//
			int													m_boltSoundSequence{ -1 };			//サウンド
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
		public:
			const auto&			GetMyUserPlayerID(void) const noexcept { return this->m_MyID; }
			void				SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }

			auto&				SetModSlot(void) noexcept { return this->m_ModSlotControl; }
			const auto&			GetModSlot(void) const noexcept { return this->m_ModSlotControl; }

			const auto&			GetSwitchPer(void) const noexcept { return this->m_SwitchPer; }
			void				CalcSwitchPer(bool IsRight) noexcept {
				if (GetModSlot().GetModData()->GetCanSwitch()) {
					Easing(&this->m_SwitchPer, IsRight ? 1.f : -1.f, 0.9f, EasingType::OutExpo);
				}
				else {
					this->m_SwitchPer = 1.f;
				}
			}
		private:
			const auto&			GetGunSoundSet(void) const noexcept { return GunSoundSets[GetModSlot().GetModData()->GetSoundSel()]; }
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
			const auto&			GetNowGunAnimeTime(void) const noexcept { return this->m_GunAnimeTime.at(static_cast<int>(GetGunAnime())); }
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
				if (this->m_Capacity != 0) {
					this->m_Capacity--;
					if (this->m_MagazinePtr) {
						this->m_ChamberAmmoData = (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoSpecMagTop();//マガジンの一番上の弾データをチャンバーイン
					}
				}
			}
			void				EjectCart(void) noexcept {
				this->m_CartFall.SetFall(
					GetFrameWorldMatParts(GunFrame::Cart).pos(), GetMove().GetMat(),
					(GetFrameWorldMatParts(GunFrame::CartVec).pos() - GetFrameWorldMatParts(GunFrame::Cart).pos() + Vector3DX::vget(GetRandf(0.2f), 0.5f + GetRandf(1.f), GetRandf(0.2f))).normalized() * (Scale3DRate * 2.f / 60.f), 2.f, FallObjectType::Cart);
			}
			void				ExecuteGrenade(void) noexcept;
			const auto			GetAnimData(GunAnimeID ID) const noexcept {
				auto* AnimMngr = GunAnimManager::Instance();
				return AnimMngr->GetAnimNow(AnimMngr->GetAnimData(GetGunAnim(ID)), this->m_GunAnimeTime.at(static_cast<int>(ID)));
			}
			const auto			IsActiveAnimData(GunAnimeID ID) const noexcept {
				auto* AnimMngr = GunAnimManager::Instance();
				return AnimMngr->GetAnimData(GetGunAnim(ID)) != nullptr;
			}
			const auto			GetGunAnimeNow(void) const noexcept {
				Matrix4x4DX AnimMat = GetAnimData(GunAnimeID::Base).GetMatrix();
				for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
					AnimMat = Lerp(AnimMat, GetAnimData((GunAnimeID)loop).GetMatrix(), this->m_GunAnimePer[loop].Per());
				}
				return AnimMat;
			}
		public://ゲッター
			const GunAnimeID&	GetGunAnime(void) const noexcept { return this->m_GunAnime; }
			const auto&			GetAutoAimID(void) const noexcept { return this->m_AutoAimControl.GetAutoAimID(); }
			const auto&			GetShotType(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModSlot().GetModData()->GetIsShotType()) {
					return (*this->m_UpperPtr)->GetModSlot().GetModData()->GetShotType();
				}
				return GetModSlot().GetModData()->GetShotType();
			}
			const auto			IsNeedCalcSling(void) const noexcept { return this->m_SlingPer < 1.f; }
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
			const auto			GetCanADS(void) const noexcept { return (GetGunAnime() == GunAnimeID::None || GetGunAnime() == GunAnimeID::Shot) && GetCanShot() && GetModSlot().GetModData()->GetCanADS(); }
			const auto			GetSightZoomSize(void) const noexcept { return this->m_SightPtr ? (*this->m_SightPtr)->GetModSlot().GetModData()->GetSightZoomSize() : 1.f; }
			const auto			GetAmmoAll(void) const noexcept { return this->m_MagazinePtr ? (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoAll() : 0; }
			const auto			GetReloadType(void) const noexcept { return GetModSlot().GetModData()->GetReloadType(); }
			const auto			GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd * (60.f * DXLib_ref::Instance()->GetDeltaTime()); }
			const auto			GetNowGunAnimeID(void) const noexcept {
				if (GetGunAnime() == GunAnimeID::None) { return -1; }
				return GetModSlot().GetModData()->GetAnimSelectList().at(static_cast<int>(GetGunAnime()));
			}
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
				if (!IsActiveAnimData(GetGunAnime())) { return 0.f; }
				auto* AnimMngr = GunAnimManager::Instance();
				float totalTime = (float)AnimMngr->GetAnimData(GetGunAnim(GetGunAnime()))->GetTotalTime();
				return (totalTime > 0.f) ? std::clamp(GetNowGunAnimeTime() / totalTime, 0.f, 1.f) : 1.f;
			}
			const auto			GetShotSwitch(void) const noexcept { return GetGunAnime() == GunAnimeID::Shot && (GetNowGunAnimePer() < 0.5f); }
			const auto			GetGunAnimeFingerNow(void) const noexcept {
				std::array<std::array<float, 5>, 2>	Finger = GetAnimData(GunAnimeID::Base).GetFingerPerArray();
				for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
					auto& AnimArray = GetAnimData((GunAnimeID)loop).GetFingerPerArray();
					int LRindex = 0;
					for (const auto& LR : AnimArray) {
						int Numberindex = 0;
						for (auto& f : LR) {
							Finger.at(LRindex).at(Numberindex) = Lerp(Finger.at(LRindex).at(Numberindex), f, this->m_GunAnimePer[loop].Per());
							Numberindex++;
						}
						LRindex++;
					}
				}
				return Finger;
			}
		public:
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
					this->m_IsGunAnimChange = true;
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
				SetGunAnime(GunAnimeID::None);
				this->m_Capacity = GetAmmoAll();//マガジン装填
				if (this->m_MagazinePtr) {
					this->m_ChamberAmmoData = (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoSpecMagTop();//マガジンの一番上の弾データをチャンバーイン
				}
				//TODO:投げ武器の弾データ
				//*
				if (GetModSlot().GetModData()->GetIsThrowWeapon()) {
					this->m_ChamberAmmoData = GetModSlot().GetModData()->GetAmmoSpecMagTop();
				}
				//*/
			}

			void				InitGunAnimePer() noexcept;
			void				UpdateGunAnimePer(bool IsADS) noexcept;
			void				SetGunMat(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept;
			void				SetMagMat(bool IsNeedGunAnim) noexcept;
			void				UpdateGunAnime(void) noexcept;
			void				UpdateGunMat(bool IsSelGun, bool IsActiveAutoAim, const Matrix3x3DX& CharaRotationCache, const Vector3DX& HeadPos, const Vector3DX& RotRad) noexcept {
				//銃の揺れ
				if (!IsFirstLoop()) {
					Easing(&this->m_UpperRad, (this->m_UpperPrevRad - RotRad), 0.9f, EasingType::OutExpo);
				}
				this->m_UpperPrevRad = RotRad;
				Easing(&this->m_GunSwingMat, Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_UpperRad.x) * Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_UpperRad.y), 0.8f, EasingType::OutExpo);
				Easing(&this->m_GunSwingMat2, this->m_GunSwingMat, 0.8f, EasingType::OutExpo);
				//スリング
				Easing(&this->m_SlingPer, IsSelGun ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				if (this->m_SlingPer <= 0.001f) { this->m_SlingPer = 0.f; }
				if (this->m_SlingPer >= 0.999f) { this->m_SlingPer = 1.f; }
				//
				auto EyeYRot = Matrix3x3DX::RotVec2(Lerp(GetMove().GetMat().yvec(), GetADSEyeMat().yvec(), GetGunAnimBlendPer(GunAnimeID::ADS)), GetMove().GetMat().yvec());
				//
				Matrix4x4DX AnimMat = GetGunAnimeNow();
				Matrix3x3DX AnimRot = Matrix3x3DX::Get33DX(AnimMat) * this->m_GunSwingMat2 * CharaRotationCache * EyeYRot;
				Vector3DX AnimPos = AnimMat.pos();
				AnimPos.x *= GetSwitchPer();
				AnimPos = HeadPos + Matrix3x3DX::Vtrans(AnimPos, CharaRotationCache);
				//オートエイム
				if (IsSelGun) {
					this->m_AutoAimControl.Update(IsActiveAutoAim, GetMyUserPlayerID(), GetADSEyeMat().pos(), GetMove().GetMat().zvec() * -1.f, GetAutoAimRadian());
					this->m_AutoAimControl.CalcAutoAimMat(&AnimRot);
				}
				//武器座標
				SetGunMat(Lerp(this->m_SlingRot * this->m_GunSwingMat2 * EyeYRot, AnimRot, this->m_SlingPer), Lerp(this->m_SlingPos, AnimPos, this->m_SlingPer));
				SetMagMat(true);
			}
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
