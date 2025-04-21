#pragma once
#include	"../../Header.hpp"

#include	"GunEnum.hpp"
#include	"ModData.hpp"
#include	"Gun_before.hpp"
#include	"CharaAnimData.hpp"

#include	"../../sub.hpp"
#include	"Ammo.hpp"
#include	"Mod.hpp"

namespace FPS_n2 {
	namespace Guns {
		class GunObj : public BaseObject , public std::enable_shared_from_this<GunObj> {
		private:
			MuzzleSmokeControl									m_MuzzleSmokeControl{};
			std::unique_ptr<ModifySlot>							m_ModifySlot{};
			AutoAimControl										m_AutoAimControl;
			std::unique_ptr<GunsModify>							m_GunsModify;

			ScreenPosition										m_AimPoint{};
			ScreenPosition										m_Reticle;
			ScreenPosition										m_Lens;
			ScreenPosition										m_LensSize;
			bool												m_Reticle_on = false;
			float												m_LensSizeLen{10000};

			FallControl											m_Grenade;
			bool												m_InChamber{ false };
			std::array<ArmMovePer, static_cast<int>(Charas::GunAnimeID::ChoiceOnceMax)>	m_GunAnimePer{};
			std::array<float, static_cast<int>(Charas::GunAnimeID::Max)>	m_GunAnimeTime{};
			std::array<float, static_cast<int>(Charas::GunAnimeID::Max)>	m_GunAnimeSpeed{};
			Charas::GunAnimeID									m_GunAnime{ Charas::GunAnimeID::Base };	//
			bool												m_ReloadAmmoCancel{ false };		//
			bool												m_ShotEnd{ false };		//
			EnumGunSound										m_EnumGunSoundNow{ EnumGunSound::Max };			//サウンド
			bool												m_IsChamberOn{ false };				//チャンバーに弾を込めるか
			bool												m_IsEject{ false };
			FallControl											m_MagFall;
			FallControl											m_CartFall;
			std::shared_ptr<Objects::AmmoInChamberObj>			m_AmmoInChamberObj{ nullptr };
			int													m_GunSightSelect{ 0 };
			const std::shared_ptr<SightPartsObj>*				m_SightPtr{ nullptr };
			const std::shared_ptr<MuzzlePartsObj>*				m_MuzzlePtr{ nullptr };
			const std::shared_ptr<UpperPartsObj>*				m_UpperPtr{ nullptr };
			const std::shared_ptr<LowerPartsObj>*				m_LowerPtr{ nullptr };
			const std::shared_ptr<MagazinePartsObj>*			m_MagazinePtr{ nullptr };
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
			ArmMovePer											m_MagArm;
			bool												m_CockHand{ false };
			ArmMovePer											m_CockArm;
			Vector3DX											m_UpperPrevRad, m_UpperRad;
			Matrix3x3DX											m_GunSwingMat, m_GunSwingMat2;
			Vector2DX											m_RecoilRadAdd;
			float												m_SwitchPer{};
			float												m_SlingPer{};
			Matrix3x3DX											m_SlingRot{};
			Vector3DX											m_SlingPos{};
			bool												m_ReleasePin{ false };
			Matrix4x4DX											m_BaseMuzzle{};
		private:
			Charas::GunAnimNow								m_AnimNowCache{};
		private:
			const auto			GetGunSoundSet(EnumGunSound Select) const noexcept { return static_cast<int>(GunSoundSets[GetModifySlot()->GetMyData()->GetSoundSelect()].m_Sound.at(static_cast<int>(Select))); }
			void				PlayGunSound(EnumGunSound Select) noexcept {
				auto* SE = SoundPool::Instance();
				if (this->m_EnumGunSoundNow != Select) {
					this->m_EnumGunSoundNow = Select;
					SE->Get(SoundType::SE, GetGunSoundSet(Select))->Play3D(GetMove().GetPos(), Scale3DRate * 10.f);
				}
			}
			const auto&			GetGunAnim(Charas::GunAnimeID AnimID) const noexcept {
				int HumanAnimType = 0;
				if (this->m_LowerPtr && (*this->m_LowerPtr)->GetModifySlot()->GetMyData()->GetHumanAnimType() != InvalidID) {
					HumanAnimType = (*this->m_LowerPtr)->GetModifySlot()->GetMyData()->GetHumanAnimType();
				}
				else {
					HumanAnimType = GetModifySlot()->GetMyData()->GetHumanAnimType();
				}

				return Charas::GunAnimeSets[HumanAnimType].Anim.at(static_cast<int>(AnimID));
			}
			const auto&			GetRecoilReturn(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModifySlot()->GetMyData()->GetIsRecoilReturn()) {
					return (*this->m_UpperPtr)->GetModifySlot()->GetMyData()->GetRecoilReturn();
				}
				return GetModifySlot()->GetMyData()->GetRecoilReturn();
			}
			const auto			GetRecoilPower(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModifySlot()->GetMyData()->GetIsRecoilPower()) {
					return (*this->m_UpperPtr)->GetModifySlot()->GetMyData()->GetRecoilPower();
				}
				return GetModifySlot()->GetMyData()->GetRecoilPower() + this->m_Recoil_Diff;
			}
			const auto&			IsInChamber(void) const noexcept { return this->m_InChamber; }
			void				ChamberIn(void) noexcept {
				if (!this->m_IsChamberOn) {
					this->m_IsChamberOn = true;
					if (this->m_Capacity != 0) {
						--this->m_Capacity;
						this->m_InChamber = true;
					}
				}
			}
			void				EjectCart(void) noexcept {
				if (!this->m_IsEject) {
					this->m_IsEject = true;
					this->m_AmmoInChamberObj->SetActive(false);
					this->m_CartFall.SetFall(
						GetPartsFrameMatParent(GunFrame::Cart).pos(), GetMove().GetMat(),
						(GetPartsFrameMatParent(GunFrame::CartVec).pos() - GetPartsFrameMatParent(GunFrame::Cart).pos() + Vector3DX::vget(GetRandf(0.2f), 0.5f + GetRandf(1.f), GetRandf(0.2f))).normalized() * (Scale3DRate * 2.f / 60.f), 2.f, Objects::FallObjectType::Cart);
				}
			}
			void				UpdateGrenade(void) noexcept;
			const auto			GetAnimDataNow(Charas::GunAnimeID ID) const noexcept {
				auto* AnimMngr = Charas::GunAnimManager::Instance();
				return AnimMngr->GetAnimNow(GetMyUserPlayerID(), AnimMngr->GetAnimData(GetGunAnim(ID)), this->m_GunAnimeTime.at(static_cast<int>(ID)));
			}
			const auto			GetNowGunAnimeID(void) const noexcept {
				if (GetGunAnime() == Charas::GunAnimeID::Base) { return InvalidID; }
				return GetModifySlot()->GetMyData()->GetAnimSelectList().at(static_cast<int>(GetGunAnime()));
			}
			const auto			GetBaseLeftHandMat(void) const noexcept {
				Vector3DX HandPos = GetPartsFrameMatParent(GunFrame::LeftHandPos).pos();
				Vector3DX Handyvec = GetPartsFrameMatParent(GunFrame::LeftHandYvec).pos() - HandPos;
				Vector3DX Handzvec = GetPartsFrameMatParent(GunFrame::LeftHandZvec).pos() - HandPos;
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
			const auto			GetCockLeftHandMat(void) const noexcept {
				Vector3DX HandPos = GetPartsFrameMatParent(GunFrame::LeftHandPosCock).pos();
				Vector3DX Handyvec = GetPartsFrameMatParent(GunFrame::LeftHandYvecCock).pos() - HandPos;
				Vector3DX Handzvec = GetPartsFrameMatParent(GunFrame::LeftHandZvecCock).pos() - HandPos;
				return Matrix4x4DX::Axis1(Handyvec.normalized(), Handzvec.normalized(), HandPos);
			}
		public://ゲッター
			const std::unique_ptr<ModifySlot>& GetModifySlot(void) const noexcept { return this->m_ModifySlot; }
			const Charas::GunAnimeID&	GetGunAnime(void) const noexcept { return this->m_GunAnime; }
			const Matrix4x4DX	GetPartsFrameMatParent(GunFrame frame) const noexcept {
				if (this->m_SightPtr) {
					//サイトがあるならそれを最優先とする
					switch (frame) {
					case GunFrame::Eyepos:
					case GunFrame::Lens:
					case GunFrame::LensSize:
						return (*this->m_SightPtr)->GetFramePartsMat(frame);
					default:
						break;
					}
				}
				Matrix4x4DX Mat;
				if (GetModifySlot()->GetPartsFrameMatChild(frame, &Mat)) {
					return Mat;
				}
				if (HaveFrame(static_cast<int>(frame))) {
					return GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame)));
				}
				return Matrix4x4DX::identity();
			}
			const PlayerID&		GetMyUserPlayerID(void) const noexcept { return this->m_MyID; }
			const auto&			GetBaseMuzzleMat(void) const noexcept { return this->m_BaseMuzzle; }
			const auto&			GetSwitchPer(void) const noexcept { return this->m_SwitchPer; }
			const auto&			GetAutoAimID(void) const noexcept { return this->m_AutoAimControl.GetAutoAimID(); }
			const auto&			GetAutoAimPos(void) const noexcept { return this->m_AutoAimControl.GetAutoAimPos(); }
			const auto&			GetGunAnimeNow(void) const noexcept { return this->m_AnimNowCache; }
			const auto			GetNowAnimTimePerCache(void) const noexcept {
				if (GetGunAnime() < Charas::GunAnimeID::Max) {
					auto* AnimMngr = Charas::GunAnimManager::Instance();
					auto* pData = AnimMngr->GetAnimData(GetGunAnim(GetGunAnime()));
					if (pData) {
						float totalTime = static_cast<float>((*pData)->GetTotalTime());
						return (totalTime > 0.f) ? std::clamp(this->m_GunAnimeTime.at(static_cast<int>(GetGunAnime())) / totalTime, 0.f, 1.f) : 1.f;
					}
				}
				return 1.f;
			}
			const auto&			GetShotType(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModifySlot()->GetMyData()->GetIsShotType()) {
					return (*this->m_UpperPtr)->GetModifySlot()->GetMyData()->GetShotType();
				}
				return GetModifySlot()->GetMyData()->GetShotType();
			}
			const auto			IsNeedCalcSling(void) const noexcept { return this->m_SlingPer < 1.f; }
			const auto			GetAimXPos(void) const noexcept { return this->m_AimPoint.XScreenPos(); }
			const auto			GetAimYPos(void) const noexcept { return this->m_AimPoint.YScreenPos(); }
			const auto			GetAutoAimActive(void) const noexcept { return this->m_AutoAimControl.GetAutoAimActive(); }
			const auto			GetAutoAimRadian(void) const noexcept { return deg2rad(5) * std::clamp(100.f / std::max(0.01f, std::hypotf(static_cast<float>(GetAimXPos() - 1920 / 2), static_cast<float>(GetAimYPos() - 1080 / 2))), 0.f, 1.f); }
			const auto			GetGunAnimBlendPer(Charas::GunAnimeID ID) const noexcept { return this->m_GunAnimePer[static_cast<int>(ID)].Per(); }
			const auto			GetCanShot(void) const noexcept { return GetGunAnimBlendPer(Charas::GunAnimeID::LowReady) <= 0.1f && GetGunAnimBlendPer(Charas::GunAnimeID::HighReady) <= 0.1f; }
			const auto			IsCanShot(void) const noexcept { return GetGunAnime() == Charas::GunAnimeID::Base || GetGunAnime() == Charas::GunAnimeID::Shot; }
			const auto			GetCanADS(void) const noexcept { return IsCanShot() && GetCanShot() && GetModifySlot()->GetMyData()->GetCanADS(); }
			const auto			GetSightZoomSize(void) const noexcept { return this->m_SightPtr ? (*this->m_SightPtr)->GetModifySlot()->GetMyData()->GetSightZoomSize() : 1.f; }
			const auto			GetAmmoAll(void) const noexcept { return this->m_MagazinePtr ? (*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetAmmoAll() : 0; }
			const auto			GetReloadType(void) const noexcept { return GetModifySlot()->GetMyData()->GetReloadType(); }
			const auto			GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd * (60.f * DXLib_ref::Instance()->GetDeltaTime()); }
			const auto			GetReloading(void) const noexcept { return (Charas::GunAnimeID::ReloadStart_Empty <= GetGunAnime()) && (GetGunAnime() <= Charas::GunAnimeID::ReloadEnd); }
			const auto			GetRightHandMat(void) const noexcept {
				Vector3DX GunPos = GetPartsFrameMatParent(GunFrame::RightHandPos).pos();
				Vector3DX Gunyvec = GetPartsFrameMatParent(GunFrame::RightHandYvec).pos() - GunPos;
				Vector3DX Gunzvec = GetPartsFrameMatParent(GunFrame::RightHandZvec).pos() - GunPos;
				return Matrix4x4DX::Axis1(Gunyvec.normalized(), Gunzvec.normalized(), GunPos);
			}
			const auto			GetLeftHandMat(void) const noexcept { return Lerp(Lerp(GetBaseLeftHandMat(), GetMagHandMat(), this->m_MagArm.Per()), GetCockLeftHandMat(), this->m_CockArm.Per()); }
			const auto			GetAmmoNumTotal(void) const noexcept { return this->m_Capacity + (IsInChamber() ? 1 : 0); }
			const auto			GetNowGunAnimePer(void) const noexcept {
				if (GetGunAnime() >= Charas::GunAnimeID::Max) {
					return 1.f;
				}
				auto* AnimMngr = Charas::GunAnimManager::Instance();
				auto* pData = AnimMngr->GetAnimData(GetGunAnim(GetGunAnime()));
				if (!pData) { return 0.f; }
				float totalTime = static_cast<float>((*pData)->GetTotalTime());
				return (totalTime > 0.f) ? std::clamp(this->m_GunAnimeTime.at(static_cast<int>(GetGunAnime())) / totalTime, 0.f, 1.f) : 1.f;
			}
			const auto			IsNowGunAnimeEnd(void) const noexcept { return GetNowAnimTimePerCache() >= 1.f; }
			const auto			GetShotSwitch(void) const noexcept { return GetGunAnime() == Charas::GunAnimeID::Shot && (GetNowAnimTimePerCache() < 0.5f); }
		public:
			void				OverrideAutoAimID(PlayerID ID, int pos) noexcept { this->m_AutoAimControl.OverrideAutoAimID(ID, pos); }
			void				SetPlayerID(PlayerID ID) noexcept { this->m_MyID = ID; }
			void				CalcSwitchPer(bool IsRight) noexcept {
				if (GetModifySlot()->GetMyData()->GetCanSwitch()) {
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
			void				SetGunAnime(Charas::GunAnimeID GunAnime) noexcept {
				this->m_GunAnime = GunAnime;
				if (GetGunAnime() < Charas::GunAnimeID::Max) {
					this->m_GunAnimeTime.at(static_cast<int>(GetGunAnime())) = 0.f;
				}
			}
			void				SetShotStart(void) noexcept;//発砲
			bool				ReloadStart(void) noexcept {
				if (GetModifySlot()->GetMyData()->GetIsThrowWeapon()) {
					return false;
				}
				if (this->m_Capacity == this->GetAmmoAll()) { return false; }//リロードの必要がある場合のみリロード
				if (this->m_Capacity != 0) {
					SetGunAnime(Charas::GunAnimeID::ReloadStart);
				}
				else {
					SetGunAnime(Charas::GunAnimeID::ReloadStart_Empty);
				}
				return true;
			}
			void				SetActiveAll(bool value) noexcept {
				if (value != IsActive()) {
					SetActive(value);
					//小孫にも実行
					GetModifySlot()->GetAnyByChild([&](const SharedGunParts& ptr) { if (ptr) { ptr->SetActive(IsActive()); } });
				}
			}
			void				SetMagazinePoachMat(const Matrix4x4DX& MagPoachMat) noexcept { this->m_MagazinePoachMat = MagPoachMat; }
			void				SetGrenadeThrowRot(const Matrix3x3DX& Rot) noexcept { this->m_GrenadeThrowRot = Rot; }
			void				DrawReticle(float Rad) const noexcept {
				if (GetCanShot() && this->m_Reticle_on && this->m_SightPtr && ((GetGunAnimBlendPer(Charas::GunAnimeID::ADS) >= 0.8f) || (GetSightZoomSize() == 1.f))) {
					auto* DrawCtrls = WindowSystem::DrawControl::Instance();
					DrawCtrls->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &(*this->m_SightPtr)->GetModifySlot()->GetMyData()->GetReitcleGraph(),
						this->m_Reticle.XScreenPos(), this->m_Reticle.YScreenPos(), 1.f, Rad, true);
				}
			}
			void				Spawn(void) noexcept {
				this->m_MagHand = false;
				this->m_MagArm.Init(this->m_MagHand);
				this->m_CockHand = false;
				this->m_CockArm.Init(this->m_CockHand);
				this->m_SlingPer = 1.f;
				this->m_IsChamberOn = false;
				SetGunAnime(Charas::GunAnimeID::Base);
				this->m_Capacity = GetAmmoAll();//マガジン装填
				ChamberIn();
			}

			void				InitGunAnimePer(void) noexcept;
			void				UpdateGunAnimePer(bool IsADS) noexcept;
			void				SetGunMat(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept {
				//武器座標
				SetMove().SetMat(rotation);
				SetMove().SetPos(pos);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				this->m_ModifySlot->UpdatePartsAnim(GetObj());
				this->m_ModifySlot->UpdatePartsMove(GunSlot::UnderRail, GetPartsFrameMatParent(GunFrame::UnderRail));
				this->m_ModifySlot->UpdatePartsMove(GunSlot::Lower, GetPartsFrameMatParent(GunFrame::Lower));
				this->m_ModifySlot->UpdatePartsMove(GunSlot::Upper, GetPartsFrameMatParent(GunFrame::Upper));
				this->m_ModifySlot->UpdatePartsMove(GunSlot::Barrel, GetPartsFrameMatParent(GunFrame::Barrel));
				this->m_ModifySlot->UpdatePartsMove(GunSlot::Sight, GetPartsFrameMatParent(GunFrame::Sight));
				this->m_ModifySlot->UpdatePartsMove(GunSlot::Magazine, GetPartsFrameMatParent(GunFrame::Magpos));
				if (this->m_AmmoInChamberObj) {
					this->m_AmmoInChamberObj->SetMat(GetPartsFrameMatParent(GunFrame::Cart).pos(), GetMove().GetMat());
				}
			}
			void				UpdateGunMat(bool IsSelectGun, bool IsActiveAutoAim, const Matrix3x3DX& CharaRotationCache, const Vector3DX& HeadPos, const Vector3DX& RotRad) noexcept;
		public:
			GunObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Gun); }
			virtual ~GunObj(void) noexcept {}
		private:
			int					GetFrameNum(void) noexcept override { return static_cast<int>(GunFrame::Max); }
			const char*			GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
		public:
			void				SetupGun(void) noexcept;
		private: //継承
			void				Init_Sub(void) noexcept override;
			void				FirstUpdate(void) noexcept override;
			void				DrawShadow(void) noexcept override;
			void				CheckDraw_Sub(int) noexcept override;
			void				Draw(bool isDrawSemiTrans, int Range) noexcept override;
			void				Dispose_Sub(void) noexcept override;
		};
	};
};
