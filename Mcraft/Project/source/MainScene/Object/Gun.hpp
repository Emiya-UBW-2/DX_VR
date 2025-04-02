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
			bool												m_isMagFall{};
			bool												m_ShotSwitch{ false };				//
			bool												m_MagHand{ false };
			ArmMovePerClass										m_MagArm;
			Vector3DX											m_UpperPrevRad, m_UpperRad;
			Matrix3x3DX											m_GunSwingMat, m_GunSwingMat2;
			Vector2DX											m_RecoilRadAdd;
			float												m_SwitchPer{};
		public:

			Matrix3x3DX											m_SlingRot{};
			Vector3DX											m_SlingPos{};
			float												m_SlingPer{};
		public:
			const auto&			GetMyUserPlayerID(void) const noexcept { return this->m_MyID; }
			void				SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }

			ModSlotControl& SetModSlot(void) noexcept { return this->m_ModSlotControl; }
			const ModSlotControl& GetModSlot(void) const noexcept { return this->m_ModSlotControl; }

			const auto&			GetSwitchPer(void) const noexcept { return m_SwitchPer; }
			void				CalcSwitchPer(bool IsRight) noexcept {
				if (GetModSlot().GetModData()->GetCanSwitch()) {
					Easing(&m_SwitchPer, IsRight ? 1.f : -1.f, 0.9f, EasingType::OutExpo);
				}
				else {
					m_SwitchPer = 1.f;
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
					(GetFrameWorldMatParts(GunFrame::CartVec).pos() - GetFrameWorldMatParts(GunFrame::Cart).pos() + Vector3DX::vget(GetRandf(0.2f), 0.5f + GetRandf(1.f), GetRandf(0.2f))).normalized() * (Scale3DRate * 2.f / 60.f), 2.f, SoundEnum::CartFall, false);
			}
		public://ゲッター
			const Matrix4x4DX	GetFrameWorldMatParts(GunFrame frame) const noexcept {
				auto* ptr = GetModSlot().HasFrameBySlot(frame);
				if (ptr) {
					return ((std::shared_ptr<ModClass>&)(*ptr))->GetFramePartsMat(frame);
				}
				if (HaveFrame((int)frame)) {
					return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame)));
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
			const auto&			GetAutoAimID(void) const noexcept { return this->m_AutoAimControl.GetAutoAimID(); }
			const auto			GetAutoAimActive(void) const noexcept { return this->m_AutoAimControl.GetAutoAimActive(); }
			const auto			GetAutoAimRadian(void) const noexcept { return deg2rad(5) * std::clamp(100.f / std::max(0.01f, std::hypotf((float)(GetAimXPos() - 1920 / 2), (float)(GetAimYPos() - 1080 / 2))), 0.f, 1.f); }
			const auto&			GetShotType(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModSlot().GetModData()->GetIsShotType()) {
					return (*this->m_UpperPtr)->GetModSlot().GetModData()->GetShotType();
				}
				return GetModSlot().GetModData()->GetShotType();
			}
			const auto			GetCanADS(void) const noexcept { return (GetGunAnime() == GunAnimeID::None || GetGunAnime() == GunAnimeID::Shot) && GetCanShot() && GetModSlot().GetModData()->GetCanADS(); }
			const auto			GetSightZoomSize(void) const noexcept { return this->m_SightPtr ? (*this->m_SightPtr)->GetModSlot().GetModData()->GetSightZoomSize() : 1.f; }
			const int			GetAmmoAll(void) const noexcept { return this->m_MagazinePtr ? (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoAll() : 0; }
			const auto			GetReloadType(void) const noexcept { return GetModSlot().GetModData()->GetReloadType(); }
			const auto			GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd * (60.f * DXLib_ref::Instance()->GetDeltaTime()); }
			const auto&			GetShotSwitch(void) const noexcept { return this->m_ShotSwitch; }
			const GunAnimeID&	GetGunAnime(void) const noexcept { return this->m_GunAnime; }
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
			const auto			GetLeftHandMat(void) const noexcept { return Lerp(GetBaseLeftHandMat(), GetMagHandMat(), m_MagArm.Per()); }
			const auto			GetAmmoNumTotal(void) const noexcept { return this->m_Capacity + ((this->m_ChamberAmmoData) ? 1 : 0); }
			const auto			GetGunAnimBlendPer(GunAnimeID ID) const noexcept { return this->m_GunAnimePer[static_cast<int>(ID)].Per(); }
			const auto			GetNowGunAnimePer(void) const noexcept {
				auto* AnimMngr = GunAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GetGunAnim(GetGunAnime()));
				if (!Ptr) { return 0.f; }
				float totalTime = (float)Ptr->GetTotalTime();
				return (totalTime > 0.f) ? std::clamp(GetNowGunAnimeTime() / totalTime, 0.f, 1.f) : 1.f;
			}
			const auto			GetGunAnimeNow(void) const noexcept {
				auto* AnimMngr = GunAnimManager::Instance();
				Matrix4x4DX AnimMat;
				{
					auto* Ptr = AnimMngr->GetAnimData(GetGunAnim(GunAnimeID::Base));
					if (Ptr) {
						AnimMat = AnimMngr->GetAnimNow(Ptr, this->m_GunAnimeTime.at(static_cast<int>(GunAnimeID::Base))).GetMatrix();
					}
				}
				for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
					auto* Ptr = AnimMngr->GetAnimData(GetGunAnim((GunAnimeID)loop));
					if (!Ptr) { continue; }
					AnimMat = Lerp(AnimMat, AnimMngr->GetAnimNow(Ptr, this->m_GunAnimeTime.at(loop)).GetMatrix(), this->m_GunAnimePer[loop].Per());
				}
				return AnimMat;
			}
			bool				GetCanShot(void) const noexcept { return GetGunAnimBlendPer(GunAnimeID::LowReady) <= 0.1f; }
			void				InitGunAnime() noexcept;
			void				UpdateGunAnime(bool IsADS) noexcept;
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
			void				SetGunMat(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept;
			void				UpdateGunMat(bool IsSelGun, bool IsActiveAutoAim, const Matrix3x3DX& CharaRotationCache, const Vector3DX& HeadPos) noexcept;
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
				m_SlingPer = 1.f;
				SetGunAnime(GunAnimeID::None);
				this->m_Capacity = GetAmmoAll();//マガジン装填
				if (this->m_MagazinePtr) {
					this->m_ChamberAmmoData = (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoSpecMagTop();//マガジンの一番上の弾データをチャンバーイン
				}
				//TODO:投げ武器の弾データ
				/*
				if (GetModSlot().GetModData()->GetIsThrowWeapon()) {
					this->m_ChamberAmmoData = GetModSlot().GetModData()->GetAmmoSpecMagTop();
				}
				//*/
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
			void				DrawShadow(void) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					auto* CameraParts = Camera3D::Instance();
					if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }
					GetObj_const().DrawModel();
				}
			}
			void				CheckDraw(void) noexcept override;
			void				Draw(bool isDrawSemiTrans) noexcept override;
			void				Dispose_Sub(void) noexcept override {
				SetModSlot().Dispose();
				this->m_MagFall.Dispose();
				this->m_CartFall.Dispose();
			}
		};
	};
};
