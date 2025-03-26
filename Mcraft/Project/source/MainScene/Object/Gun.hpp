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

			bool												m_GunAnimeFirst{ false };
			GunAnimeID											m_GunAnime{ GunAnimeID::Base };	//
			float												m_GunAnimeTime{ 0.f };

			bool												m_Cancel{ false };			//
			bool												m_ShotSwitch{ false };				//
			int													m_boltSoundSequence{ -1 };			//サウンド
			bool												m_IsChamberOn{ false };				//チャンバーに弾を込めるか
			bool												m_IsEject{ false };
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
		public:
			const auto&			GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			void				SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }

			auto				GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			auto				GetFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(GetFrame(static_cast<int>(frame))); }

			ModSlotControl& GetModSlot() noexcept { return this->m_ModSlotControl; }
			const ModSlotControl& GetModSlot() const noexcept { return this->m_ModSlotControl; }
		private:
			void				ExecuteCartInChamber(void) noexcept;//チャンバーへの装弾、排出
			void				ExecuteSound(void) noexcept;//チャンバーへの装弾、排出
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
			const auto&			GetAimXPos(void) const noexcept { return this->m_AimPointControl.GetAimXPos(); }
			const auto&			GetAimYPos(void) const noexcept { return this->m_AimPointControl.GetAimYPos(); }
			const auto			GetAutoAimRadian() const noexcept {
				float Len = std::max(0.01f, std::hypotf((float)(GetAimXPos() - 1920 / 2), (float)(GetAimYPos() - 1080 / 2)));
				Len = std::clamp(100.f / Len, 0.f, 1.f);
				return deg2rad(5) * Len;
			}
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
			const auto&			GetAmmoNum(void) const noexcept { return this->m_Capacity; }
			const auto&			GetMagazinePtr(void) const noexcept { return *this->m_MagazinePtr; }
			const auto&			IsMagLoadSuccess() const { return this->m_isMagSuccess; }
			const auto			GetInChamber(void) const noexcept { return this->m_ChamberAmmoData != nullptr; }
			const auto			GetPelletNum(void) const noexcept { return GetInChamber() ? this->m_ChamberAmmoData->GetPellet() : 0; }
			const auto			GetSightMax() const noexcept { return (int)this->m_SightPtr.size(); }
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
			const auto			GetReloadType(void) const noexcept { return GetModSlot().GetModData()->GetReloadType(); }

			const auto			GetAmmoNumTotal(void) const noexcept { return this->m_Capacity + (GetInChamber() ? 1 : 0); }
			const auto			GetIsMagEmpty(void) const noexcept { return this->m_Capacity == 0; }//次弾がない
			const auto			GetIsMagFull(void) const noexcept { return this->m_Capacity == this->GetAmmoAll(); }

			const auto			GetReloading(void) const noexcept { return (GunAnimeID::ReloadStart_Empty <= GetGunAnime()) && (GetGunAnime() <= GunAnimeID::ReloadEnd); }
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
						if (GetObj_const().GetFrameChildNum(GetFrame((int)GunFrame::EyeOffsetPos)) > 0) {
							vec = (GetObj_const().GetChildFrameWorldMatrix(GetFrame((int)GunFrame::EyeOffsetPos), 0).pos() - GetFrameWorldMat_P(GunFrame::EyeOffsetPos).pos()).normalized();
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
			const auto			GetGunAnimePer(EnumGunAnimType ID) {
				//銃の位置を指定するアニメ
				auto* AnimMngr = GunAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[GetHumanAnimType()].Anim.at((int)ID));
				if (!Ptr) { return 0.f; }
				float totalTime = (float)Ptr->GetTotalTime();
				return (totalTime > 0.f) ? (60.f * this->m_GunAnimeTime / totalTime) : 1.f;
			}
			const auto			GetGunAnimeNow(EnumGunAnimType ID, Matrix4x4DX* Ret) {
				//銃の位置を指定するアニメ
				auto* AnimMngr = GunAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[GetHumanAnimType()].Anim.at((int)ID));
				if (!Ptr) { return false; }
				*Ret = AnimMngr->GetAnimNow(Ptr, 60.f * this->m_GunAnimeTime).GetMatrix();
				return true;
			}
		public:
			void				SetMagLoadSuccess(bool value) { this->m_isMagSuccess = value; }
			void				SetCancel(bool value) noexcept { this->m_Cancel = value; }
			void				SetGunAnime(GunAnimeID pShotPhase) noexcept {
				this->m_GunAnimeFirst = (this->m_GunAnime != pShotPhase);
				this->m_GunAnime = pShotPhase;
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
			void				CockByMag() noexcept { this->m_ChamberAmmoData = GetMagazinePtr()->GetModSlot().GetModData()->GetAmmoSpecMagTop(); }//マガジンの一番上の弾データをチャンバーイン
			void				UnloadChamber() noexcept { this->m_ChamberAmmoData.reset(); }
			void				SetShotSwitchOff() noexcept { this->m_ShotSwitch = false; }
			void				SetGunMatrix(const Matrix3x3DX& rotation, const Vector3DX& pos) noexcept {
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
			void				SetMagFall() noexcept {
				this->m_MagFall.SetFall(
					GetFrameWorldMat_P(GunFrame::Magpos).pos(),
					Matrix3x3DX::Get33DX(GetFrameWorldMat_P(GunFrame::Magpos)),
					GetFrameWorldMat_P(GunFrame::Magpos).yvec() * -1.f * (Scale3DRate * 3.f / 60.f),
					12.f, SoundEnum::MagFall, false);
			}
			void				SetReloadStart(int NextMagNum, int NextMagUniqueID) noexcept {
				this->m_NextMagNum = NextMagNum;
				switch (GetReloadType()) {
				case RELOADTYPE::MAG:
					this->m_NextMagUniqueID = NextMagUniqueID;
					break;
				case RELOADTYPE::AMMO:
					this->m_NextMagUniqueID = 0;
					break;
				default:
					break;
				}
				SetGunAnime((!GetIsMagEmpty()) ? GunAnimeID::ReloadStart : GunAnimeID::ReloadStart_Empty);
			}
			void				SetActiveAll(bool value) noexcept;
			void				SetBullet(void) noexcept;//発砲
			void				UpdateGunAnims(void) noexcept;
		public:
			void				SetFallObject() noexcept;
		public:
			void				UpdateReticle() noexcept;
			void				DrawReticle(float Rad) const noexcept {
				auto* WindowSizeParts = WindowSizeControl::Instance();
				if (m_ReticleControl.IsActiveReticle() && GetSightPtr()) {
					auto* WindowParts = WindowSystem::DrawControl::Instance();
					WindowParts->SetDrawRotaGraph(WindowSystem::DrawLayer::Normal, &(*GetSightPtr())->GetModSlot().GetModData()->GetReitcleGraph(),
						static_cast<int>(m_ReticleControl.GetReticleXPos() * 1980 / WindowSizeParts->GetScreenY(1980)),
						static_cast<int>(m_ReticleControl.GetReticleYPos() * 1080 / WindowSizeParts->GetScreenY(1080)),
						1.f, Rad, true);
				}
			}
		public:
			void				Init_Gun(void) noexcept;
		public:
			GunClass(void) noexcept { this->m_objType = (int)ObjType::Gun; }
			~GunClass(void) noexcept {}
		private:
			int					GetFrameNum() noexcept override { return (int)GunFrame::Max; }
			const char*			GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
		private: //継承
			void				Init_Sub(void) noexcept override {
				m_ModSlotControl.InitModSlotControl(this->m_FilePath);
			}
			void				FirstExecute(void) noexcept override;
			void				DrawShadow(void) noexcept override;
			void				Draw(bool isDrawSemiTrans) noexcept override;
			void				Dispose_Sub(void) noexcept override {
				m_ModSlotControl.DisposeModSlotControl();
			}
		};
	};
};
