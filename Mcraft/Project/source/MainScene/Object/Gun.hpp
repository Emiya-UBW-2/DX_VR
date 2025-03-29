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
		class GunClass : public ObjectBaseClass {
		private:
			MuzzleSmokeControl									m_MuzzleSmokeControl{};
			AimPointControl										m_AimPointControl{};
			ReticleControl										m_ReticleControl{};
			ModSlotControl										m_ModSlotControl{};

			std::shared_ptr<AmmoDataClass>						m_ChamberAmmoData{ nullptr };		//
			std::array<ArmMovePerClass, static_cast<int>(GunAnimeID::ChoiceOnceMax)>	m_Arm{};
			GunAnimeID										m_GunAnime{ GunAnimeID::Shot };	//
			float												m_GunAnimeTime{ 0.f };
			bool												m_ReloadAmmoCancel{ false };		//
			bool												m_ShotEnd{ false };		//
			bool												m_ShotSwitch{ false };				//
			int													m_boltSoundSequence{ -1 };			//サウンド
			bool												m_IsChamberOn{ false };				//チャンバーに弾を込めるか
			bool												m_PrevChamberOn{ false };
			bool												m_IsEject{ false };
			bool												m_PrevEject{ false };
			int													m_NextMagNum{ 0 };
			int													m_NextMagUniqueID{ 0 };
			FallControl											m_MagFall;
			FallControl											m_CartFall;
			int													m_GunSightSel{ 0 };
			float												m_GunChangePer{ 0.f };
			std::array<const std::shared_ptr<SightClass>*, 2>	m_SightPtr{ nullptr };
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
		public:
			const auto&			GetMyUserPlayerID(void) const noexcept { return this->m_MyID; }
			void				SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }

			auto				GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			auto				GetFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(GetFrame(static_cast<int>(frame))); }

			ModSlotControl& GetModSlot() noexcept { return this->m_ModSlotControl; }
			const ModSlotControl& GetModSlot() const noexcept { return this->m_ModSlotControl; }
		private:
			const auto			GetGunSoundSet(void) const noexcept { return GunSoundSets[GetModSlot().GetModData()->GetSoundSel()]; }
			const auto&			GetHumanAnimType(void) const noexcept {
				if (this->m_LowerPtr && (*this->m_LowerPtr)->GetModSlot().GetModData()->GetHumanAnimType() != -1) {
					return (*this->m_LowerPtr)->GetModSlot().GetModData()->GetHumanAnimType();
				}
				return GetModSlot().GetModData()->GetHumanAnimType();
			}
		public://ゲッター
			const bool			HasFrame(GunFrame frame) const noexcept;
			const Matrix4x4DX	GetFrameWorldMat_P(GunFrame frame, bool CheckSight = true) const noexcept;
		public:
			const auto			GetAimXPos(void) const noexcept { return this->m_AimPointControl.GetAimXPos(); }
			const auto			GetAimYPos(void) const noexcept { return this->m_AimPointControl.GetAimYPos(); }
		public://ゲッター
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
			const auto&			GetGunShootSound(void) const noexcept {
				if (this->m_MuzzlePtr) {
					return (*this->m_MuzzlePtr)->GetModSlot().GetModData()->GetGunShootSound();
				}
				return GetModSlot().GetModData()->GetGunShootSound();
			}

			const auto&			GetShotSwitch(void) const noexcept { return this->m_ShotSwitch; }
			const auto&			GetGunAnime(void) const noexcept { return this->m_GunAnime; }
			const auto			GetGunAnimeID(GunAnimeID Select) const noexcept {
				if (Select == GunAnimeID::None) {
					return -1;
				}
				return GetModSlot().GetModData()->GetAnimSelectList().at(static_cast<int>(Select));
			}
			const auto			GetNowGunAnimeID(void) const noexcept { return GetGunAnimeID(GetGunAnime()); }

			const auto			IsThrowWeapon() const noexcept { return GetModSlot().GetModData()->GetIsThrowWeapon(); }

			const auto			GetReloading(void) const noexcept { return (GunAnimeID::ReloadStart_Empty <= GetGunAnime()) && (GetGunAnime() <= GunAnimeID::ReloadEnd); }
			const auto			IsCanShoot(void) const noexcept {
				switch (GetGunAnime()) {
				case GunAnimeID::None:
				case GunAnimeID::Shot:
					return true;
					break;
				default:
					return false;
				}
			}

			const auto&			GetAmmoNum(void) const noexcept { return this->m_Capacity; }
			const auto&			GetMagazinePtr(void) const noexcept { return *this->m_MagazinePtr; }
			const auto&			IsMagLoadSuccess() const { return this->m_isMagSuccess; }
			const auto			GetInChamber(void) const noexcept { return this->m_ChamberAmmoData != nullptr; }
			const auto			GetPelletNum(void) const noexcept { return GetInChamber() ? this->m_ChamberAmmoData->GetPellet() : 0; }
			const auto			GetSightMax() const noexcept { return static_cast<int>(this->m_SightPtr.size()); }
			const auto*			GetSightPtr() const noexcept {
				if (this->m_GunChangePer < 0.5f) {
					int Prev = (this->m_GunSightSel - 1);
					if (Prev < 0) { Prev = GetSightMax() - 1; }
					return this->m_SightPtr.at(Prev);
				}
				else {
					return this->m_SightPtr.at(this->m_GunSightSel);
				}
			}
			const auto			GetAmmoAll(void) const noexcept { return this->m_MagazinePtr ? GetMagazinePtr()->GetModSlot().GetModData()->GetAmmoAll() : 0; }
			const auto			GetMagUniqueID(void) const noexcept {
				if (this->m_MagazinePtr) {
					return GetMagazinePtr()->GetModSlot().GetModData()->GetUniqueID();
				}
				return GetModSlot().GetModData()->GetUniqueID();
			}
			const Matrix4x4DX	GetMagHandMat() {
				if (this->m_MagazinePtr) {
					Vector3DX MagPos = GetMagazinePtr()->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
					Vector3DX Magyvec = GetMagazinePtr()->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - MagPos;
					Vector3DX Magzvec = GetMagazinePtr()->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - MagPos;
					return Matrix4x4DX::Axis1(Magyvec.normalized(), Magzvec.normalized(), MagPos);
				}
				return Matrix4x4DX::identity();
			}
			const auto			GetReloadType(void) const noexcept { return GetModSlot().GetModData()->GetReloadType(); }

			const auto			GetAmmoNumTotal(void) const noexcept { return this->m_Capacity + (GetInChamber() ? 1 : 0); }
			const auto			GetIsMagEmpty(void) const noexcept { return this->m_Capacity == 0; }//次弾がない
			const auto			GetIsMagFull(void) const noexcept { return this->m_Capacity == this->GetAmmoAll(); }

			const auto			GetEyePos(void) const noexcept {
				Vector3DX Pos;
				if (this->m_SightPtr.at(1)) {
					int Prev = (this->m_GunSightSel - 1);
					if (Prev < 0) { Prev = GetSightMax() - 1; }
					Pos = Lerp(
						(*this->m_SightPtr.at(Prev))->GetFrameWorldMat_P(GunFrame::Eyepos).pos(),
						(*this->m_SightPtr.at(this->m_GunSightSel))->GetFrameWorldMat_P(GunFrame::Eyepos).pos(),
						this->m_GunChangePer);
				}
				else {
					if (!this->m_SightPtr.at(0)) {
						Pos = GetFrameWorldMat_P(GunFrame::Eyepos).pos();
					}
					else {
						Pos = (*this->m_SightPtr.at(0))->GetFrameWorldMat_P(GunFrame::Eyepos).pos();
					}
					if (HasFrame(GunFrame::EyeOffsetPos)) {
						if (this->m_GunSightSel == 0) {
							Pos = Lerp(GetFrameWorldMat_P(GunFrame::EyeOffsetPos).pos(), Pos, this->m_GunChangePer);
						}
						else {
							Pos = Lerp(Pos, GetFrameWorldMat_P(GunFrame::EyeOffsetPos).pos(), this->m_GunChangePer);
						}
					}
				}
				return Pos;
			}
			const auto			GetEyeYVec(void) const noexcept {
				Vector3DX Pos;
				if (this->m_SightPtr.at(1)) {
					int Prev = (this->m_GunSightSel - 1);
					if (Prev < 0) { Prev = GetSightMax() - 1; }
					Pos = Lerp(
						(*this->m_SightPtr.at(Prev))->GetFrameWorldMat_P(GunFrame::Eyepos).yvec(),
						(*this->m_SightPtr.at(this->m_GunSightSel))->GetFrameWorldMat_P(GunFrame::Eyepos).yvec(),
						this->m_GunChangePer);
				}
				else {
					if (!this->m_SightPtr.at(0)) {
						Pos = GetFrameWorldMat_P(GunFrame::Eyepos).yvec();
					}
					else {
						Pos = (*this->m_SightPtr.at(0))->GetFrameWorldMat_P(GunFrame::Eyepos).yvec();
					}
					if (HasFrame(GunFrame::EyeOffsetPos)) {
						Vector3DX vec = GetFrameWorldMat_P(GunFrame::EyeOffsetPos).yvec();
						if (GetObj_const().GetFrameChildNum(GetFrame(static_cast<int>(GunFrame::EyeOffsetPos))) > 0) {
							vec = (GetObj_const().GetChildFrameWorldMatrix(GetFrame(static_cast<int>(GunFrame::EyeOffsetPos)), 0).pos() - GetFrameWorldMat_P(GunFrame::EyeOffsetPos).pos()).normalized();
						}
						if (this->m_GunSightSel == 0) {
							Pos = Lerp(vec, Pos, this->m_GunChangePer);
						}
						else {
							Pos = Lerp(Pos, vec, this->m_GunChangePer);
						}
					}
				}
				return Pos;
			}
			const auto			GetGunAnimePer(GunAnimeID ID) {
				//銃の位置を指定するアニメ
				auto* AnimMngr = GunAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[GetHumanAnimType()].Anim.at(static_cast<int>(ID)));
				if (!Ptr) { return 0.f; }
				float totalTime = (float)Ptr->GetTotalTime();
				return (totalTime > 0.f) ? (60.f * this->m_GunAnimeTime / totalTime) : 1.f;
			}
			const auto			GetGunAnimeNow() const noexcept {
				auto* AnimMngr = GunAnimManager::Instance();
				Matrix4x4DX AnimMat;
				{
					auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[GetHumanAnimType()].Anim.at(static_cast<int>(GunAnimeID::Aim)));
					if (Ptr) {
						AnimMat = AnimMngr->GetAnimNow(Ptr, 60.f * this->m_GunAnimeTime).GetMatrix();
					}
				}
				for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
					//銃の位置を指定するアニメ
					auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[GetHumanAnimType()].Anim.at(loop));
					if (!Ptr) { continue; }
					AnimMat = Lerp(AnimMat, AnimMngr->GetAnimNow(Ptr, 60.f * this->m_GunAnimeTime).GetMatrix(), this->m_Arm[loop].Per());
				}
				return AnimMat;
			}
			const auto			GetGunAnimBlendPer(GunAnimeID ID) const noexcept { return this->m_Arm[static_cast<int>(ID)].Per(); }
			void				UpdateGunAnime(const std::array<bool, static_cast<int>(GunAnimeID::ChoiceOnceMax)>& Array, bool IsForce) noexcept {
				for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
					if (IsForce) {
						this->m_Arm[loop].Init(Array[loop]);
					}
					else {
#if TRUE
						switch (static_cast<GunAnimeID>(loop)) {
						case GunAnimeID::LowReady:
							this->m_Arm[loop].Update(Array[loop], 0.f, 0.f, 0.87f);
							break;
						case GunAnimeID::ADS:
							this->m_Arm[loop].Update(Array[loop], 0.2f, 0.2f, 0.9f);
							break;
						case GunAnimeID::ReloadStart_Empty:
							this->m_Arm[loop].Update(Array[loop], 0.5f, 0.2f);
							break;
						case GunAnimeID::ReloadStart:
							this->m_Arm[loop].Update(Array[loop], 0.2f, 0.2f);
							break;
						case GunAnimeID::Reload:
							this->m_Arm[loop].Update(Array[loop], 0.1f, 0.2f, (GetReloadType() == RELOADTYPE::AMMO) ? 0.9f : 0.8f);
							break;
						case GunAnimeID::ReloadEnd:
							this->m_Arm[loop].Update(Array[loop], 0.1f, 0.2f, 0.9f);
							break;
						case GunAnimeID::Watch:
							this->m_Arm[loop].Update(Array[loop], 0.1f, 0.1f);
							break;
						case GunAnimeID::ThrowReady:
							this->m_Arm[loop].Update(Array[loop], 0.1f, 0.1f);
							break;
						case GunAnimeID::Throw:
							this->m_Arm[loop].Update(Array[loop], 0.1f, 0.1f);
							break;
						default:
							break;
						}
#else
						this->m_Arm[loop].Update(Array[loop], 0.2f, 0.2f, 0.9f);
#endif
					}
				}
			}
		public:
			void				SetMagLoadSuccess(bool value) { this->m_isMagSuccess = value; }
			void				SetReloadAmmoCancel() noexcept {
				if (GetReloadType() == RELOADTYPE::AMMO) {
					this->m_ReloadAmmoCancel = true;
				}
			}
			void				SetShotEnd(bool value) noexcept { m_ShotEnd = value; }
			void				SetGunAnime(GunAnimeID pShotPhase) noexcept {
				this->m_GunAnime = pShotPhase;
				this->m_GunAnimeTime = 0.f;
			}
			void				ChangeSightSel() noexcept {
				if (true) {
					this->m_GunSightSel++;
					if (this->m_GunSightSel >= GetSightMax()) {
						this->m_GunSightSel = 0;
					}
				}
				else {
					for (int i = 0; i < GetSightMax(); i++) {
						this->m_GunSightSel++;
						if (this->m_GunSightSel >= GetSightMax()) {
							this->m_GunSightSel = 0;
						}
						if (this->m_SightPtr.at(this->m_GunSightSel)) {
							break;
						}
					}
				}
				this->m_GunChangePer = 0.f;
			}
			void				SetAmmo(int value) noexcept { this->m_Capacity = std::clamp(value, 0, this->GetAmmoAll()); }
			void				CockByMag() noexcept {
				if (this->m_MagazinePtr) {
					this->m_ChamberAmmoData = GetMagazinePtr()->GetModSlot().GetModData()->GetAmmoSpecMagTop();//マガジンの一番上の弾データをチャンバーイン
				}
			}
			void				UnloadChamber() noexcept { this->m_ChamberAmmoData.reset(); }
			void				SetShotSwitchOff() noexcept { this->m_ShotSwitch = false; }
			void				SetMagFall() noexcept {
				this->m_MagFall.SetFall(
					GetFrameWorldMat_P(GunFrame::Magpos).pos(),
					Matrix3x3DX::Get33DX(GetFrameWorldMat_P(GunFrame::Magpos)),
					GetFrameWorldMat_P(GunFrame::Magpos).yvec() * -1.f * (Scale3DRate * 3.f / 60.f),
					12.f, SoundEnum::MagFall, false);
			}
			void				SetNextMagazine(int NextMagNum, int NextMagUniqueID) noexcept {
				this->m_NextMagNum = NextMagNum;
				this->m_NextMagUniqueID = NextMagUniqueID;
			}
			void				SetActiveAll(bool value) noexcept;
			void				SetBullet(void) noexcept;//発砲
			void				UpdateGunAnims(void) noexcept;
		public:
			void				SetupModSlot(void) noexcept {
				m_ModSlotControl.InitModSlotControl(this->m_FilePath);
			}
			void				SetupSpawn(const std::array<bool, static_cast<int>(GunAnimeID::ChoiceOnceMax)>& Array) noexcept;
		public:
			void				UpdateReticle() noexcept;
			void				DrawReticle(float Rad) const noexcept {
				if (m_ReticleControl.IsActiveReticle() && GetSightPtr()) {
					auto* WindowParts = WindowSystem::DrawControl::Instance();
					WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &(*GetSightPtr())->GetModSlot().GetModData()->GetReitcleGraph(),
						static_cast<int>(m_ReticleControl.GetReticleXPos()), static_cast<int>(m_ReticleControl.GetReticleYPos()), 1.f, Rad, true);
				}
			}
		public:
			void				UpdateGunMat(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept {
				SetMove().SetMat(rotation);
				SetMove().SetPos(pos);
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());

				m_ModSlotControl.UpdatePartsAnim(GetObj());
				m_ModSlotControl.UpdatePartsMove(GetFrameWorldMat_P(GunFrame::UnderRail), GunSlot::UnderRail);
				m_ModSlotControl.UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Lower), GunSlot::Lower);
				m_ModSlotControl.UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Upper), GunSlot::Upper);
				m_ModSlotControl.UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Barrel), GunSlot::Barrel);
				m_ModSlotControl.UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Sight), GunSlot::Sight);
				m_ModSlotControl.UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Magpos), GunSlot::Magazine);
			}
			void				UpdateMagazineMat(bool IsSelGun, const Matrix4x4DX& MagPoachMat) noexcept;
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
			void				Draw(bool isDrawSemiTrans) noexcept override;
			void				Dispose_Sub(void) noexcept override {
				m_ModSlotControl.DisposeModSlotControl();
			}
		};
	};
};
