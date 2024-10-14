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
		class GunClass : public ObjectBaseClass, public MuzzleSmokeControl, public ModSlotControl, public ReticleControl {
		private:
			std::shared_ptr<AmmoDataClass>			m_ChamberAmmoData{ nullptr };		//
			bool									m_GunAnimeFirst{ false };
			GunAnimeID								m_GunAnime{ GunAnimeID::Base };	//
			bool									m_Cancel{ false };			//
			bool									m_ShotSwitch{ false };				//
			int										m_boltSoundSequence{ -1 };			//サウンド
			bool									m_IsChamberOn{ false };				//チャンバーに弾を込めるか
			bool									m_IsEject{ false };
			int										m_NextMagNum{ 0 };
			int										m_NextMagUniqueID{ 0 };
			FallControl								m_MagFall;
			FallControl								m_CartFall;
			int										m_GunSightSel{ 0 };
			float									m_GunChangePer{ 0.f };
			std::array<const std::shared_ptr<SightClass>*, 2>	m_SightPtr{ nullptr };
			const std::shared_ptr<MuzzleClass>* m_MuzzlePtr{ nullptr };
			const std::shared_ptr<UpperClass>* m_UpperPtr{ nullptr };
			const std::shared_ptr<LowerClass>* m_LowerPtr{ nullptr };
			const std::shared_ptr<MagazineClass>* m_MagazinePtr{ nullptr };
			int										m_ShootRate_Diff{ 0 };
			int										m_Recoil_Diff{ 0 };
			int										m_Capacity{ 0 };//弾数
			float																			m_UpperAnim{ 0.f };
		private:
			PlayerID											m_MyID{ 0 };
		public:
			const auto& GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }

			auto	GetFrameWorldMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			auto	GetFrameLocalMat(GunFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(GetFrame(static_cast<int>(frame))); }
		private:
			void		ExecuteCartInChamber(void) noexcept;//チャンバーへの装弾、排出
			void		ExecuteSound(void) noexcept;//チャンバーへの装弾、排出
		private:
			const auto	GetGunSoundSet(void) const noexcept { return GunSoundSets[this->GetModData()->GetSoundSel()]; }
			const auto& GetHumanAnimType(void) const noexcept {
				if (this->m_LowerPtr && (*this->m_LowerPtr)->GetModData()->GetHumanAnimType() != -1) {
					return (*this->m_LowerPtr)->GetModData()->GetHumanAnimType();
				}
				return GetModData()->GetHumanAnimType();
			}
		public://ゲッター
			const bool	HasFrame(GunFrame frame) const noexcept;
			const Matrix4x4DX GetFrameWorldMat_P(GunFrame frame, bool CheckSight = true) const noexcept;
		public://ゲッター
			const auto GetRecoilPower(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModData()->GetIsRecoilPower()) {
					return (*this->m_UpperPtr)->GetModData()->GetRecoilPower();
				}
				return GetModData()->GetRecoilPower() + m_Recoil_Diff;
			}
			const auto& GetRecoilReturn(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModData()->GetIsRecoilReturn()) {
					return (*this->m_UpperPtr)->GetModData()->GetRecoilReturn();
				}
				return GetModData()->GetRecoilReturn();
			}
			const auto& GetShotType(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetModData()->GetIsShotType()) {
					return (*this->m_UpperPtr)->GetModData()->GetShotType();
				}
				return GetModData()->GetShotType();
			}
			const auto& GetGunShootSound(void) const noexcept {
				if (this->m_MuzzlePtr) {
					return (*this->m_MuzzlePtr)->GetModData()->GetGunShootSound();
				}
				return GetModData()->GetGunShootSound();
			}

			const auto& GetShotSwitch(void) const noexcept { return this->m_ShotSwitch; }
			const auto& GetGunAnime(void) const noexcept { return this->m_GunAnime; }
			const auto& GetAmmoNum(void) const noexcept { return this->m_Capacity; }
			const auto& GetMagazinePtr(void) const noexcept { return m_MagazinePtr; }
			const auto	GetPelletNum(void) const noexcept { return this->m_ChamberAmmoData ? this->m_ChamberAmmoData->GetPellet() : 0; }
			const auto	GetSightMax() const noexcept { return (int)this->m_SightPtr.size(); }
			const auto* GetSightPtr() const noexcept {
				if (m_GunChangePer < 0.5f) {
					int Prev = (m_GunSightSel - 1);
					if (Prev < 0) { Prev = GetSightMax() - 1; }
					return this->m_SightPtr.at(Prev);
				}
				else {
					return this->m_SightPtr.at(m_GunSightSel);
				}
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

			const auto	GetReloading(void) const noexcept { return (GunAnimeID::ReloadStart_Empty <= GetGunAnime()) && (GetGunAnime() <= GunAnimeID::ReloadEnd); }
			const auto	GetChecking(void) const noexcept { return (GunAnimeID::CheckStart <= GetGunAnime()) && (GetGunAnime() <= GunAnimeID::CheckEnd); }
			const auto	GetEyePos(void) const noexcept {
				Vector3DX Pos;
				if (this->m_SightPtr.at(1)) {
					int Prev = (m_GunSightSel - 1);
					if (Prev < 0) { Prev = GetSightMax() - 1; }
					Pos = Lerp(
						(*m_SightPtr.at(Prev))->GetFrameWorldMat_P(GunFrame::Eyepos).pos(),
						(*m_SightPtr.at(m_GunSightSel))->GetFrameWorldMat_P(GunFrame::Eyepos).pos(),
						m_GunChangePer);
				}
				else {
					if (!this->m_SightPtr.at(0)) {
						Pos = GetFrameWorldMat_P(GunFrame::Eyepos).pos();
					}
					else {
						Pos = (*m_SightPtr.at(0))->GetFrameWorldMat_P(GunFrame::Eyepos).pos();
					}
					if (HasFrame(GunFrame::EyeOffsetPos)) {
						if (m_GunSightSel == 0) {
							Pos = Lerp(GetFrameWorldMat_P(GunFrame::EyeOffsetPos).pos(), Pos, m_GunChangePer);
						}
						else {
							Pos = Lerp(Pos, GetFrameWorldMat_P(GunFrame::EyeOffsetPos).pos(), m_GunChangePer);
						}
					}
				}
				return Pos;
			}
			const auto	GetEyeYVec(void) const noexcept {
				Vector3DX Pos;
				if (this->m_SightPtr.at(1)) {
					int Prev = (m_GunSightSel - 1);
					if (Prev < 0) { Prev = GetSightMax() - 1; }
					Pos = Lerp(
						(*m_SightPtr.at(Prev))->GetFrameWorldMat_P(GunFrame::Eyepos).yvec(),
						(*m_SightPtr.at(m_GunSightSel))->GetFrameWorldMat_P(GunFrame::Eyepos).yvec(),
						m_GunChangePer);
				}
				else {
					if (!this->m_SightPtr.at(0)) {
						Pos = GetFrameWorldMat_P(GunFrame::Eyepos).yvec();
					}
					else {
						Pos = (*m_SightPtr.at(0))->GetFrameWorldMat_P(GunFrame::Eyepos).yvec();
					}
					if (HasFrame(GunFrame::EyeOffsetPos)) {
						Vector3DX vec = GetFrameWorldMat_P(GunFrame::EyeOffsetPos).yvec();
						if (GetObj_const().GetFrameChildNum(GetFrame((int)GunFrame::EyeOffsetPos)) > 0) {
							vec = (GetObj_const().GetChildFrameWorldMatrix(GetFrame((int)GunFrame::EyeOffsetPos), 0).pos() - GetFrameWorldMat_P(GunFrame::EyeOffsetPos).pos()).normalized();
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
			const auto	GetGunAnimePer(EnumGunAnimType ID) {
				//銃の位置を指定するアニメ
				auto* AnimMngr = GunAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[GetHumanAnimType()].Anim.at((int)ID));
				if (!Ptr) { return 0.f; }
				float totalTime = (float)Ptr->GetTotalTime();
				return (totalTime > 0.f) ? (this->m_UpperAnim / totalTime) : 1.f;
			}
			const auto	GetGunAnimeNow(EnumGunAnimType ID, Matrix4x4DX* Ret) {
				//銃の位置を指定するアニメ
				auto* AnimMngr = GunAnimManager::Instance();
				auto* Ptr = AnimMngr->GetAnimData(GunAnimeSets[GetHumanAnimType()].Anim.at((int)ID));
				if (!Ptr) { return false; }
				*Ret = AnimMngr->GetAnimNow(Ptr, this->m_UpperAnim).GetMatrix();
				return true;
			}
		public:
			void		SetCancel(bool value) noexcept { m_Cancel = value; }
			void		SetGunAnime(GunAnimeID pShotPhase) noexcept {
				this->m_GunAnimeFirst = (this->m_GunAnime != pShotPhase);
				this->m_GunAnime = pShotPhase;
			}
			void		ChangeSightSel() noexcept {
				if (true) {
					m_GunSightSel++;
					if (m_GunSightSel >= GetSightMax()) {
						m_GunSightSel = 0;
					}
				}
				else {
					for (int i = 0; i < GetSightMax(); i++) {
						m_GunSightSel++;
						if (m_GunSightSel >= GetSightMax()) {
							m_GunSightSel = 0;
						}
						if (this->m_SightPtr.at(m_GunSightSel)) {
							break;
						}
					}
				}
				m_GunChangePer = 0.f;
			}
			void		SetAmmo(int value) noexcept { this->m_Capacity = std::clamp(value, 0, this->GetAmmoAll()); }
			void		CockByMag() noexcept { this->m_ChamberAmmoData = (*m_MagazinePtr)->GetModData()->GetAmmoSpecMagTop(); }//マガジンの一番上の弾データをチャンバーイン
			void		UnloadChamber() noexcept { this->m_ChamberAmmoData.reset(); }
			void		SetShotSwitchOff() noexcept { this->m_ShotSwitch = false; }
			void		SetGunMatrix(const Matrix4x4DX& value) noexcept {
				SetMove().SetMat(Matrix3x3DX::Get33DX(value.rotation()));
				SetMove().SetPos(value.pos());
				SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());

				ModSlotControl::UpdatePartsAnim(GetObj());
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat_P(GunFrame::UnderRail), GunSlot::UnderRail);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Lower), GunSlot::Lower);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Upper), GunSlot::Upper);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Barrel), GunSlot::Barrel);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Sight), GunSlot::Sight);
				ModSlotControl::UpdatePartsMove(GetFrameWorldMat_P(GunFrame::Magpos), GunSlot::Magazine);
			}
			void		SetMagFall() noexcept {
				m_MagFall.SetFall(
					GetFrameWorldMat_P(GunFrame::Magpos).pos(),
					Matrix3x3DX::Get33DX(GetFrameWorldMat_P(GunFrame::Magpos).rotation()),
					GetFrameWorldMat_P(GunFrame::Magpos).yvec() * -1.f * (Scale_Rate * 3.f / 60.f),
					12.f, SoundEnum::MagFall);
			}
			void		SetReloadStart(int NextMagNum, int NextMagUniqueID) noexcept {
				this->m_NextMagNum = NextMagNum;
				switch (this->GetModData()->GetReloadType()) {
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
			void		SetActiveAll(bool value) noexcept;
			void		SetBullet(void) noexcept;//発砲
			void		UpdateGunAnims(void) noexcept;
		public:
			GunClass(void) noexcept { this->m_objType = (int)ObjType::Gun; }
			~GunClass(void) noexcept {}
		public:
			void			SetFallObject() noexcept;
			void			UpdateReticle() noexcept;
		private:
			int	GetFrameNum() noexcept override { return (int)GunFrame::Max; }
			const char* GetFrameStr(int id) noexcept override { return GunFrameName[id]; }
		private: //継承
			void			Init_Sub(void) noexcept override {
				ModSlotControl::InitModSlotControl(this->m_FilePath);
			}
			void			FirstExecute(void) noexcept override;
			void			DrawShadow(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			Dispose_Sub(void) noexcept override {
				ModSlotControl::DisposeModSlotControl();
			}
		public:
			void			Init_Gun(void) noexcept;
		};
	};
};
