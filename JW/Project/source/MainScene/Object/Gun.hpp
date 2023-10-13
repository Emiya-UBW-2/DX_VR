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
			public ModSlotControl
		{
		private:
			std::shared_ptr<AmmoDataClass>			m_ChamberAmmoData{ nullptr };		//
			GunAnimeID								m_ShotPhase{ GunAnimeID::Base };	//
			bool									m_ShotSwitch{ false };				//
			int										m_boltSoundSequence{ -1 };			//サウンド
			bool									m_IsChamberOn{ false };				//チャンバーに弾を込めるか
			bool									m_IsEject{ false };
			VECTOR_ref								m_RecoilRadAdd;
			FallControl								m_MagFall;
			FallControl								m_CartFall;
			const std::shared_ptr<SightClass>*		m_SightPtr{ nullptr };
			const std::shared_ptr<MuzzleClass>*		m_MuzzlePtr{ nullptr };
			const std::shared_ptr<UpperClass>*		m_UpperPtr{ nullptr };
			const std::shared_ptr<MagazineClass>*	m_MagazinePtr{ nullptr };
		private:
			std::array<float, (int)CharaGunAnimeID::Max>									m_GunAnimFrame;
			std::vector<std::array<int, (int)CharaGunAnimeID::Max>>							m_CharaAnimeSet;
			std::vector<std::array<std::vector<EnumGunAnim>, (int)EnumGunAnimType::Max>>	m_GunAnimeSet;
			float																			m_UpperAnim{ 0.f };
		public:
			const auto&		GetUpperAnim(void) const noexcept { return this->m_UpperAnim; }
			const auto&		GetGunDataClass(void) const noexcept { return (std::shared_ptr<GunDataClass>&)this->GetModData(); }
			const auto&		GetHumanAnimType(void) const noexcept { return GetGunDataClass()->GetHumanAnimType(); }
			const auto&		GetGunAnimeSet(EnumGunAnimType ID) const noexcept { return this->m_GunAnimeSet.at(GetHumanAnimType()).at((int)ID).at(0); }
			float			GetAllTime(CharaGunAnimeID ID) { return (float)m_CharaAnimeSet.at(GetHumanAnimType()).at((int)ID); }
			auto&			GetGunAnimFrame(CharaGunAnimeID ID) { return m_GunAnimFrame.at((int)ID); }

			void			SetGunAnim(CharaGunAnimeID ID, float value) { GetGunAnimFrame(ID) = value; }
			void			UpdateGunAnim(CharaGunAnimeID ID, float speed) { GetGunAnimFrame(ID) += 30.f / FPS * speed; }
			bool			GetGunAnimZero(CharaGunAnimeID ID) { return GetGunAnimFrame(ID) <= 0.f; }
			bool			GetGunAnimEnd(CharaGunAnimeID ID) { return GetGunAnimFrame(ID) >= GetAllTime(ID); }
			float			GetTimePer(CharaGunAnimeID ID) { return (GetAllTime(ID) > 0.f) ? (GetGunAnimFrame(ID) / GetAllTime(ID)) : 1.f; }
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept;
		public://ゲッター
			const auto& GetAnime(GunAnimeID anim) noexcept { return GetObj().get_anime((int)anim); }
			const bool	HaveFrame(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].first != -1; }
			const auto& GetFrame(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].first; }
			const auto& GetFrameBaseLocalMat(GunFrame frame) const noexcept { return this->m_Frames[(int)frame].second; }
			const bool HasFrame(GunFrame frame) const noexcept;
			const MATRIX_ref GetFrameLocalMat(GunFrame frame) const noexcept;
			const MATRIX_ref GetFrameWorldMat(GunFrame frame) const noexcept;
		public:
			void ResetFrameLocalMat(GunFrame frame) noexcept;
			void SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept;
		private:
			const auto	GetGunSoundSet(void) const noexcept { return GunSoundSets[GetGunDataClass()->GetSoundSel()]; }
		public://ゲッター
			const GraphHandle* GetReticlePtr(void) const noexcept {
				if (this->m_SightPtr) {
					return &(*this->m_SightPtr)->GetReitcleGraph();
				}
				return nullptr;
			}
			const auto& GetName(void) const noexcept { return GetGunDataClass()->GetName(); }
			const auto& GetReloadType(void) const noexcept { return GetGunDataClass()->GetReloadType(); }
			const auto& GetShotSwitch(void) const noexcept { return this->m_ShotSwitch; }
			const auto& GetShotPhase(void) const noexcept { return this->m_ShotPhase; }
			const auto&	GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd; }
			const auto& GetNowAnime(void) noexcept { return GetObj().get_anime((size_t)this->m_ShotPhase); }
			const auto& GetMagazinePtr(void) const noexcept { return m_MagazinePtr; }
			const auto& GetAmmoAll(void) const noexcept { return (*m_MagazinePtr)->GetAmmoAll(); }
			const auto	GetIsMagEmpty(void) const noexcept { return (*m_MagazinePtr)->GetAmmoNum() == 0; }//次弾がない
			const auto	GetIsMagFull(void) const noexcept { return (*m_MagazinePtr)->GetAmmoNum() == this->GetAmmoAll(); }

			const auto& GetRecoilPower(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetIsRecoilPower()) {
					return (*this->m_UpperPtr)->GetRecoilPower();
				}
				return GetGunDataClass()->GetRecoilPower();
			}
			const auto& GetRecoilReturn(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetIsRecoilReturn()) {
					return (*this->m_UpperPtr)->GetRecoilReturn();
				}
				return GetGunDataClass()->GetRecoilReturn();
			}
			const auto	GetShotType(void) const noexcept {
				if (this->m_UpperPtr && (*this->m_UpperPtr)->GetIsShotType()) {
					return (*this->m_UpperPtr)->GetShotType();
				}
				return GetGunDataClass()->GetShotType();
			}
			const auto	GetGunShootSound(void) const noexcept {
				if (this->m_MuzzlePtr) {
					return (*this->m_MuzzlePtr)->GetGunShootSound();
				}
				return GetGunDataClass()->GetGunShootSound();
			}
			const auto	GetShoting(void) const noexcept { return this->m_ShotPhase == GunAnimeID::Shot; }
			const auto	GetCocking(void) const noexcept { return this->m_ShotPhase == GunAnimeID::Cocking; }
			const auto	GetReloading(void) const noexcept { return (GunAnimeID::ReloadStart_Empty <= this->m_ShotPhase) && (this->m_ShotPhase <= GunAnimeID::ReloadEnd); }
			const auto	GetChecking(void) const noexcept { return (GunAnimeID::CheckStart <= this->m_ShotPhase) && (this->m_ShotPhase <= GunAnimeID::CheckEnd); }
			const auto	GetWatching(void) const noexcept { return (this->m_ShotPhase == GunAnimeID::Watch); }
			const auto	GetShootReady(void) const noexcept { return this->m_ShotPhase <= GunAnimeID::Shot; }
			const auto	GetInChamber(void) const noexcept { return this->m_ChamberAmmoData != nullptr; }
			const auto	GetCanShot(void) const noexcept { return GetInChamber() && GetShootReady(); }

			const auto	GetEyePos(void) const noexcept { return GetFrameWorldMat(GunFrame::Eyepos).pos(); }
			const auto	GetLensPos(void) const noexcept { return GetFrameWorldMat(GunFrame::Lens).pos(); }
			const auto	GetReticlePos(void) const noexcept { return GetLensPos() + (GetLensPos() - GetEyePos()).Norm()*10.f; }

			const auto	GetCartMat(void) noexcept { return GetFrameWorldMat(GunFrame::Cart); }
			const auto	GetCartVec(void) noexcept { return (GetFrameWorldMat(GunFrame::CartVec).pos() - GetCartMat().pos()).Norm(); }
			const auto	GetAmmoNum(void) const noexcept { return (*m_MagazinePtr)->GetAmmoNum() + (GetInChamber() ? 1 : 0); }
		public:
			void		SetAmmoHandMatrix(const MATRIX_ref& value, float pPer) noexcept { (*m_MagazinePtr)->SetHandMatrix(value, pPer); }
			void		FillMag() noexcept { (*m_MagazinePtr)->SetAmmo(GetAmmoAll()); }
			void		CockByMag() noexcept { this->m_ChamberAmmoData = (*m_MagazinePtr)->GetAmmoSpecMagTop(); }//マガジンの一番上の弾データをチャンバーイン
			void		UnloadChamber() noexcept { this->m_ChamberAmmoData.reset(); }
			void		SetShotSwitchOff() noexcept { this->m_ShotSwitch = false; }
			void		SetGunMatrix(const MATRIX_ref& value) noexcept {
				SetMove(value.GetRot(), value.pos());
				GetSlotControl()->UpdatePartsAnim(GetObj());
				GetSlotControl()->UpdatePartsMove(GetFrameWorldMat(GunFrame::Lower), GunSlot::Lower);
				GetSlotControl()->UpdatePartsMove(GetFrameWorldMat(GunFrame::Upper), GunSlot::Upper);
				GetSlotControl()->UpdatePartsMove(GetFrameWorldMat(GunFrame::Barrel), GunSlot::Barrel);
				GetSlotControl()->UpdatePartsMove(GetFrameWorldMat(GunFrame::Sight), GunSlot::Sight);
				GetSlotControl()->UpdatePartsMove(GetFrameWorldMat(GunFrame::Magpos), GunSlot::Magazine);
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

			void		SetPressShot() noexcept {
				if (this->GetInChamber()) {
					this->SetBullet();
					this->m_ShotPhase = GunAnimeID::Shot;
					this->m_ShotSwitch = true;
				}
				else {
					this->m_ShotPhase = (!GetIsMagEmpty()) ? GunAnimeID::Cocking : GunAnimeID::ReloadStart_Empty;
				}
			}
			void		SetReloadStart() noexcept { this->m_ShotPhase = (!GetIsMagEmpty()) ? GunAnimeID::ReloadStart : GunAnimeID::ReloadStart_Empty; }

			void		UpdateGunAnims(bool PressShot) {
				if (this->GetShoting()) {
					bool ischeck = true;
					switch (this->GetShotType()) {
					case SHOTTYPE::FULL:
						ischeck = this->GetInChamber() || this->GetIsMagEmpty();
						break;
					case SHOTTYPE::SEMI:
					case SHOTTYPE::BOLT:
						ischeck = !PressShot;
						break;
					default:
						break;
					}

					if (this->GetNowAnime().TimeEnd() && ischeck) {
						this->SetGunAnim(CharaGunAnimeID::Down, 0.f);
						if (!this->GetIsMagEmpty()) {
							switch (this->GetShotType()) {
							case SHOTTYPE::FULL:
							case SHOTTYPE::SEMI:
								this->m_ShotPhase = GunAnimeID::Base;
								break;
							case SHOTTYPE::BOLT:
								this->m_ShotPhase = GunAnimeID::Cocking;
								break;
							default:
								break;
							}
						}
						else {
							this->m_ShotPhase = GunAnimeID::Base;
						}
					}
				}
				{
					CharaGunAnimeID GunAnimSelect = CharaGunAnimeID::Down;
					//コッキング
					if (this->m_ShotPhase == GunAnimeID::Cocking) {
						GunAnimSelect = CharaGunAnimeID::Cocking;
						UpdateGunAnim(GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::Base;
						}
					}
					//リロード開始
					if (this->m_ShotPhase == GunAnimeID::ReloadStart_Empty) {
						GunAnimSelect = CharaGunAnimeID::ReloadStart_Empty;
						if (this->GetGunAnimZero(GunAnimSelect)) {
							m_UpperAnim = 0.f;
						}
						UpdateGunAnim(GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::ReloadOne;
						}
					}
					if (this->m_ShotPhase == GunAnimeID::ReloadStart) {
						GunAnimSelect = CharaGunAnimeID::ReloadStart;
						if (this->GetGunAnimZero(GunAnimSelect)) {
							m_UpperAnim = 0.f;
						}
						UpdateGunAnim(GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::ReloadOne;
						}
					}
					//
					if (this->m_ShotPhase == GunAnimeID::ReloadOne) {
						GunAnimSelect = CharaGunAnimeID::ReloadOne;
						UpdateGunAnim(GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(GunAnimSelect)) {
							switch (this->GetReloadType()) {
							case RELOADTYPE::MAG:
								this->m_ShotPhase = GunAnimeID::ReloadEnd;
								break;
							case RELOADTYPE::AMMO:
								if (this->GetIsMagFull()) {
									this->m_ShotPhase = GunAnimeID::ReloadEnd;
								}
								else {
									this->SetGunAnim(GunAnimSelect, 0.f);
									this->GetObj().get_anime((int)this->m_ShotPhase).Reset();
								}
								break;
							default:
								break;
							}
						}
					}
					if (this->m_ShotPhase == GunAnimeID::ReloadEnd) {
						GunAnimSelect = CharaGunAnimeID::ReloadEnd;
						UpdateGunAnim(GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(GunAnimSelect)) {
							if (this->GetInChamber()) {
								this->m_ShotPhase = GunAnimeID::Base;
							}
							else {
								if (!this->GetIsMagEmpty()) {
									this->m_ShotPhase = GunAnimeID::Cocking;
								}
								else {
									this->m_ShotPhase = GunAnimeID::ReloadStart;
								}
							}
						}
					}
					//
					if (this->m_ShotPhase == GunAnimeID::CheckStart) {
						GunAnimSelect = CharaGunAnimeID::CheckStart;
						UpdateGunAnim(GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::Checking;
						}
					}
					if (this->m_ShotPhase == GunAnimeID::Checking) {
						GunAnimSelect = CharaGunAnimeID::Check;
						UpdateGunAnim(GunAnimSelect, 1.f);
						if (GetGunAnimEnd(GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::CheckEnd;
						}
					}
					if (this->m_ShotPhase == GunAnimeID::CheckEnd) {
						GunAnimSelect = CharaGunAnimeID::CheckEnd;
						UpdateGunAnim(GunAnimSelect, 2.f);
						if (GetGunAnimEnd(GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::Base;
						}
					}
					//
					if (this->m_ShotPhase == GunAnimeID::Watch) {
						GunAnimSelect = CharaGunAnimeID::Watch;
						UpdateGunAnim(GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::Base;
						}
					}
					for (int i = 0; i < (int)CharaGunAnimeID::Max; i++) {
						if (GunAnimSelect != (CharaGunAnimeID)i) {
							this->SetGunAnim((CharaGunAnimeID)i, 0.f);
						}
					}
					m_UpperAnim += 60.f / FPS;
				}
			}
		private:
			void		ExecuteCartInChamber(void) noexcept;//チャンバーへの装弾、排出
		public:
			GunClass(void) noexcept { this->m_objType = ObjType::Gun; }
			~GunClass(void) noexcept {}
		public:
			void			Init(void) noexcept override;
			void			Init_Gun(void) noexcept;
			void			FirstExecute(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			Dispose(void) noexcept override;
		};
	};
};
