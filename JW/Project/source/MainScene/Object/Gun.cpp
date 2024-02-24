#include	"Gun.hpp"

#include "../../ObjectManager.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool  GunClass::HasFrame(GunFrame frame) const noexcept {
			//該当フレームがあるのなら上書き
			if (m_SightPtr[m_GunSightSel]) {
				switch (frame) {
					case GunFrame::Eyepos:
					case GunFrame::Lens:
					case GunFrame::LensSize:
						return true;
					default:
						break;
				}
			}
			if (HasFrameBySlot(frame)) {
				return true;
			}
			if (HaveFrame(frame)) {
				return true;
			}
			return false;
		}
		const MATRIX_ref  GunClass::GetFrameLocalMat(GunFrame frame) const noexcept {
			//該当フレームがあるのなら上書き
			if (m_SightPtr[m_GunSightSel]) {
				switch (frame) {
					case GunFrame::Eyepos:
					case GunFrame::Lens:
					case GunFrame::LensSize:
						return (*m_SightPtr[m_GunSightSel])->GetFrameLocalMat(frame);
					default:
						break;
				}
			}
			MATRIX_ref Ret;
			if (ModSlotControl::GetPartsFrameLocalMatBySlot(frame, &Ret)) {
				return Ret;
			}
			if (HaveFrame(frame)) {
				return GetFrameLocalMatrix(GetFrame(frame));
			}
			return MATRIX_ref::zero();
		}
		const MATRIX_ref  GunClass::GetFrameWorldMat(GunFrame frame) const noexcept {
			//該当フレームがあるのなら上書き
			if (m_SightPtr[m_GunSightSel]) {
				switch (frame) {
					case GunFrame::Eyepos:
					case GunFrame::Lens:
					case GunFrame::LensSize:
						return (*m_SightPtr[m_GunSightSel])->GetFrameWorldMat(frame);
					default:
						break;
				}
			}
			MATRIX_ref Ret;
			if (ModSlotControl::GetPartsFrameWorldMat(frame, &Ret)) {
				return Ret;
			}
			if (HaveFrame(frame)) {
				return GetFrameWorldMatrix(GetFrame(frame));
			}
			return MATRIX_ref::zero();
		}

		void GunClass::ResetFrameLocalMat(GunFrame frame) noexcept {
			ModSlotControl::ResetPartsFrameLocalMat(frame);
			if (HaveFrame(frame)) {
				GetObj().frame_Reset(GetFrame(frame));
			}
		}
		void GunClass::SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept {
			ModSlotControl::SetPartsFrameLocalMat(frame, value);
			if (HaveFrame(frame)) {
				GetObj().SetFrameLocalMatrix(GetFrame(frame), value * this->GetFrameBaseLocalMat(frame));
			}
		}


		void GunClass::SetBullet(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			//
			SE->Get((int)SoundEnum::Trigger).Play_3D(0, GetMatrix().pos(), Scale_Rate*5.f);
			switch (GetGunShootSound()) {
				case GunShootSound::Normal:
					SE->Get((int)GetGunSoundSet().m_ShotNormal).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
					break;
				case GunShootSound::Suppressor:
					SE->Get((int)GetGunSoundSet().m_ShotSuppressor).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
					break;
				default:
					break;
			}
			for (int i = 0; i < this->m_ChamberAmmoData->GetPellet(); i++) {
				auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->MakeObject(ObjType::Ammo));
				LastAmmo->Init();
				auto mat =
					MATRIX_ref::RotY(deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) *
					MATRIX_ref::RotX(deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) *
					GetFrameWorldMat(GunFrame::Muzzle);
				LastAmmo->Put(this->m_ChamberAmmoData, mat.pos(), mat.zvec() * -1.f, m_MyID);
			}
			UnloadChamber();
			AddMuzzleSmokePower();
			this->m_ShotPhase = GunAnimeID::Shot;
			this->m_ShotSwitch = true;
		}
		void	GunClass::SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept {
			m_MagFall.Dispose();
			m_CartFall.Dispose();
			m_MagFall.Init(backGround, (*m_MagazinePtr)->GetFilePath());
			m_CartFall.Init(backGround, (*m_MagazinePtr)->GetModData()->GetAmmoSpecMagTop()->GetPath());	//装填したマガジンの弾に合わせて薬莢生成
		}
		void	GunClass::Init_Gun(void) noexcept {
			{
				std::vector<const SharedObj*> PartsList;
				ModSlotControl::GetChildPartsList(&PartsList);
				m_SightPtr[0] = nullptr;
				m_SightPtr[1] = nullptr;
				int SightSel = 0;
				m_MuzzlePtr = nullptr;
				m_UpperPtr = nullptr;
				m_MagazinePtr = nullptr;
				m_ShootRate_Diff = 0;
				m_ReloadRate_Diff = 0;

				for (auto& p : PartsList) {
					if ((*p)->GetobjType() == ObjType::Sight) {
						if (SightSel < m_SightPtr.size()) {
							const auto* Ptr = &((std::shared_ptr<SightClass>&)(*p));
							if (m_SightPtr[SightSel]) {
								if (!(*Ptr)->GetModData()->GetReitcleGraph().IsActive()) {
									continue;
								}
							}
							m_SightPtr[SightSel] = Ptr;
							SightSel++;
						}
					}
					if ((*p)->GetobjType() == ObjType::MuzzleAdapter) {
						m_MuzzlePtr = &((std::shared_ptr<MuzzleClass>&)(*p));
					}
					if ((*p)->GetobjType() == ObjType::Upper) {
						m_UpperPtr = &((std::shared_ptr<UpperClass>&)(*p));
					}
					if ((*p)->GetobjType() == ObjType::Magazine) {
						m_MagazinePtr = &((std::shared_ptr<MagazineClass>&)(*p));
					}

					//性能周り
					{
						auto& d = ((std::shared_ptr<ModClass>&)(*p))->GetModData();
						m_ShootRate_Diff += d->GetShootRate_Diff();
						m_ReloadRate_Diff += d->GetReloadRate_Diff();
					}
				}
				PartsList.clear();
			}
			SetAmmo(GetAmmoAll());								//マガジン装填
			CockByMag();										//チャンバーイン
			this->m_ShotPhase = GunAnimeID::Base;
		}

		void	GunClass::UpdateGunAnims(bool PressShot) noexcept {
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
								//ユニークIDが異なる場合登録するオブジェと切り替える
								if (this->m_NextMagUniqueID != (*m_MagazinePtr)->GetModData()->GetUniqueID()) {
									m_MagazinePtr = &((std::shared_ptr<MagazineClass>&)(ModSlotControl::SetMod(GunSlot::Magazine, this->m_NextMagUniqueID, this->GetObj())));
								}
								SetAmmo(m_NextMagNum);
								this->m_ShotPhase = GunAnimeID::ReloadEnd;
								break;
							case RELOADTYPE::AMMO:
								SetAmmo(this->m_Capacity + 1);
								m_NextMagNum--;
								if ((this->GetIsMagFull() || (m_NextMagNum <= 0)) || this->m_ReloadCancel) {
									this->m_ReloadCancel = false;
									this->m_ShotPhase = GunAnimeID::ReloadEnd;
								}
								else {
									this->SetGunAnim(GunAnimSelect, 0.f);
									this->GetObj().get_anime((int)this->m_ShotPhase).Reset();
									this->GetObj().get_anime((int)this->m_ShotPhase).per = 1.f;
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
				//
				if (this->m_ShotPhase == GunAnimeID::Melee) {
					GunAnimSelect = CharaGunAnimeID::Melee;
					UpdateGunAnim(GunAnimSelect, 1.5f);
					if (GetGunAnimEnd(GunAnimSelect)) {
						this->m_ShotPhase = GunAnimeID::Base;
					}
				}
				//
				if (this->m_ShotPhase == GunAnimeID::AmmoLoadStart) {
					GunAnimSelect = CharaGunAnimeID::AmmoLoadStart;
					UpdateGunAnim(GunAnimSelect, 1.5f);
					if (GetGunAnimEnd(GunAnimSelect)) {
						this->m_ShotPhase = GunAnimeID::AmmoLoading;
						this->m_AmmoLoadCount = 1;
						this->m_IsAmmoLoadCount = false;
						this->m_AmmoLoadSwitch = true;
					}
				}
				else if (this->m_ShotPhase == GunAnimeID::AmmoLoading) {
					GunAnimSelect = CharaGunAnimeID::AmmoLoading;
					UpdateGunAnim(GunAnimSelect, 1.0f);

					if (!this->m_IsAmmoLoadCount && (GetGunAnimFrame(GunAnimSelect) >= GetAllTime(GunAnimSelect) * 3 / 10)) {
						auto* SE = SoundPool::Instance();
						//
						SE->Get((int)SoundEnum::AmmoLoad).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						this->m_IsAmmoLoadCount = true;
					}
					this->m_AmmoLoadSwitch = false;
					if (GetGunAnimEnd(GunAnimSelect)) {
						if (this->m_AmmoLoadCount >= 3) {
							this->m_ShotPhase = GunAnimeID::AmmoLoadEnd;
						}
						else {
							this->SetGunAnim(GunAnimSelect, 0.f);
							this->GetObj().get_anime((int)this->m_ShotPhase).Reset();
							this->GetObj().get_anime((int)this->m_ShotPhase).per = 1.f;
							this->m_AmmoLoadCount++;
							this->m_IsAmmoLoadCount = false;
							this->m_AmmoLoadSwitch = true;
						}
					}
				}
				if (this->m_ShotPhase == GunAnimeID::AmmoLoadEnd) {
					GunAnimSelect = CharaGunAnimeID::AmmoLoadEnd;
					UpdateGunAnim(GunAnimSelect, 1.5f);
					if (GetGunAnimEnd(GunAnimSelect)) {
						this->m_ShotPhase = GunAnimeID::Base;
					}
				}
				//
				for (int i = 0; i < (int)CharaGunAnimeID::Max; i++) {
					if (GunAnimSelect != (CharaGunAnimeID)i) {
						this->SetGunAnim((CharaGunAnimeID)i, 0.f);
					}
				}
				m_UpperAnim += 60.f / FPS;
			}
		}

		void GunClass::ExecuteCartInChamber(void) noexcept {
			{
				auto Prev = this->m_IsChamberOn;
				switch (GetShotType()) {
					case SHOTTYPE::FULL:
					case SHOTTYPE::SEMI:
						this->m_IsChamberOn = false;
						this->m_IsChamberOn |= (GetShoting() && (GetNowAnime().time >= 3.f));
						this->m_IsChamberOn |= (GetCocking() && (GetNowAnime().time >= 25.f));
						break;
					case SHOTTYPE::BOLT:
						this->m_IsChamberOn = false;
						this->m_IsChamberOn |= (GetCocking() && (GetNowAnime().time >= 25.f));
						break;
					default:
						break;
				}
				if (this->m_IsChamberOn && (this->m_IsChamberOn != Prev)) {
					if (!GetIsMagEmpty()) {
						CockByMag();
					}
					SetAmmo(this->m_Capacity - 1);
				}
			}
			{
				auto Prev = this->m_IsEject;
				switch (GetShotType()) {
					case SHOTTYPE::FULL:
					case SHOTTYPE::SEMI:
						this->m_IsEject = this->m_ShotSwitch;
						break;
					case SHOTTYPE::BOLT:
						this->m_IsEject = false;
						this->m_IsEject |= ((this->m_ShotPhase == GunAnimeID::ReloadStart_Empty || this->m_ShotPhase == GunAnimeID::ReloadStart) && (GetNowAnime().time >= 19.f && !GetNowAnime().TimeEnd()));
						this->m_IsEject |= (GetCocking() && (GetNowAnime().time >= 19.f && !GetNowAnime().TimeEnd()));
						break;
					default:
						break;
				}
				if (this->m_IsEject && (this->m_IsEject != Prev)) {
					m_CartFall.SetFall(GetCartMat().pos(), GetFrameWorldMat(GunFrame::Muzzle).GetRot(), GetCartVec()*(Scale_Rate * 2.f / 60.f), 2.f, SoundEnum::CartFall);
				}
			}
		}


		void GunClass::Init(void) noexcept {
			ObjectBaseClass::Init();
			//データ
			InitModSlotControl(this->m_FilePath, false);
			//
			{
				m_CharaAnimeSet.clear();
				//M4
				m_CharaAnimeSet.resize(m_CharaAnimeSet.size() + 1);
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Down) = 0;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Ready) = 0;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ADS) = 0;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Cocking) = 35;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::CheckStart) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Check) = 30;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::CheckEnd) = 30;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadStart_Empty) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadStart) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadOne) = 45;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadEnd) = 20;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Watch) = 60;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Melee) = 10;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::AmmoLoadStart) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::AmmoLoading) = 36;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::AmmoLoadEnd) = 10;
				//ハンドガン
				m_CharaAnimeSet.resize(m_CharaAnimeSet.size() + 1);
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Down) = 0;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Ready) = 0;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ADS) = 0;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Cocking) = 35;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::CheckStart) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Check) = 30;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::CheckEnd) = 30;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadStart_Empty) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadStart) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadOne) = 30;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadEnd) = 10;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Watch) = 60;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Melee) = 10;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::AmmoLoadStart) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::AmmoLoading) = 36;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::AmmoLoadEnd) = 10;
				//M870
				m_CharaAnimeSet.resize(m_CharaAnimeSet.size() + 1);
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Down) = 0;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Ready) = 0;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ADS) = 0;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Cocking) = 35;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::CheckStart) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Check) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::CheckEnd) = 30;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadStart_Empty) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadStart) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadOne) = 30;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadEnd) = 10;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Watch) = 60;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Melee) = 10;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::AmmoLoadStart) = 15;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::AmmoLoading) = 36;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::AmmoLoadEnd) = 10;
				//
				m_GunAnimeSet.clear();
				//M4
				m_GunAnimeSet.resize(m_GunAnimeSet.size() + 1);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Aim).emplace_back(EnumGunAnim::M16_aim);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::ADS).emplace_back(EnumGunAnim::M16_ads);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::ReloadStart_Empty).emplace_back(EnumGunAnim::M16_reloadstart_empty);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::ReloadStart).emplace_back(EnumGunAnim::M16_reloadstart);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Reload).emplace_back(EnumGunAnim::M16_reload);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Ready).emplace_back(EnumGunAnim::M16_ready1);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Ready).emplace_back(EnumGunAnim::M16_ready2);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Run).emplace_back(EnumGunAnim::M16_run);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Check).emplace_back(EnumGunAnim::M16_check1);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Watch).emplace_back(EnumGunAnim::M1911_watch);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Melee).emplace_back(EnumGunAnim::M1911_melee);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::AmmoLoad).emplace_back(EnumGunAnim::M1911_reload);
				//ハンドガン
				m_GunAnimeSet.resize(m_GunAnimeSet.size() + 1);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Aim).emplace_back(EnumGunAnim::M1911_aim1);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Aim).emplace_back(EnumGunAnim::M1911_aim2);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::ADS).emplace_back(EnumGunAnim::M1911_ads);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::ReloadStart_Empty).emplace_back(EnumGunAnim::M1911_reloadstart_empty);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::ReloadStart).emplace_back(EnumGunAnim::M1911_reloadstart);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Reload).emplace_back(EnumGunAnim::M1911_reload);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Ready).emplace_back(EnumGunAnim::M1911_ready1);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Ready).emplace_back(EnumGunAnim::M1911_ready2);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Run).emplace_back(EnumGunAnim::M1911_run);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Check).emplace_back(EnumGunAnim::M1911_check1);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Watch).emplace_back(EnumGunAnim::M1911_watch);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Melee).emplace_back(EnumGunAnim::M1911_melee);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::AmmoLoad).emplace_back(EnumGunAnim::M1911_reload);
				//M870
				m_GunAnimeSet.resize(m_GunAnimeSet.size() + 1);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Aim).emplace_back(EnumGunAnim::M16_aim);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::ADS).emplace_back(EnumGunAnim::M16_ads);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::ReloadStart_Empty).emplace_back(EnumGunAnim::M16_reloadstart_empty);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::ReloadStart).emplace_back(EnumGunAnim::M16_reloadstart);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Reload).emplace_back(EnumGunAnim::M16_reload);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Ready).emplace_back(EnumGunAnim::M16_ready1);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Ready).emplace_back(EnumGunAnim::M16_ready2);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Run).emplace_back(EnumGunAnim::M16_run);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Check).emplace_back(EnumGunAnim::M16_check1);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Watch).emplace_back(EnumGunAnim::M1911_watch);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::Melee).emplace_back(EnumGunAnim::M1911_melee);
				m_GunAnimeSet.back().at((int)EnumGunAnimType::AmmoLoad).emplace_back(EnumGunAnim::M1911_reload);
			}
		}
		void GunClass::FirstExecute(void) noexcept {
			auto SE = SoundPool::Instance();
			if (this->m_IsFirstLoop) {
				InitMuzzleSmoke(GetFrameWorldMat(GunFrame::Muzzle).pos());
			}
			else {
				ExecuteMuzzleSmoke(GetFrameWorldMat(GunFrame::Muzzle).pos());
			}

			GunAnimeID Sel = (GunAnimeID)(this->m_ShotPhase);
			for (int i = 0; i < GetObj().get_anime().size(); i++) {
				if (Sel == (GunAnimeID)i) {
					GetObj().get_anime(i).per = 1.f;
					if (Sel == GunAnimeID::Shot) {
						SetAnimOnce(i, ((float)this->GetGunDataClass()->GetShotRate()) / 300.f);//分間300
					}
					else {
						SetAnimOnce(i, 1.5f);
					}
				}
				else {
					if ((GunAnimeID)i == GunAnimeID::Base) {
						GetObj().get_anime(i).per = 1.f;
					}
					else if ((GunAnimeID)i == GunAnimeID::Open) {
						bool isHit = false;
						if (GetAmmoNumTotal() == 0) {
							isHit = true;
						}
						else {
							if (Sel == GunAnimeID::Cocking) {
								if (GetObj().get_anime((int)Sel).time <= 22.f) {
									isHit = true;
								}
							}
							else if (GetIsMagFull() && !GetInChamber()) {
								isHit = true;
							}
						}
						GetObj().get_anime(i).per = std::clamp(GetObj().get_anime(i).per + 10.f / FPS * (isHit ? 1.f : -1.f), 0.f, 1.f);
					}
					else {
						GetObj().get_anime(i).Reset();
					}
				}
			}
			switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					//0
					{
						if ((5.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 6.f)) {
							if (m_boltSoundSequence != 1) {
								m_boltSoundSequence = 1;
								SE->Get((int)GetGunSoundSet().m_Cock1).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
							}
						}
						if ((28.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 29.f)) {
							if (m_boltSoundSequence != 3) {
								m_boltSoundSequence = 3;
								SE->Get((int)GetGunSoundSet().m_Cock2).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
							}
						}
					}
					//1
					{
						if ((5.f < GetAnime(GunAnimeID::ReloadStart_Empty).time && GetAnime(GunAnimeID::ReloadStart_Empty).time < 6.f)) {
							if (m_boltSoundSequence != 5) {
								m_boltSoundSequence = 5;
								SE->Get((int)GetGunSoundSet().m_Cock1).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
							}
						}

						if ((5.f < GetAnime(GunAnimeID::ReloadStart).time && GetAnime(GunAnimeID::ReloadStart).time < 6.f)) {
							if (m_boltSoundSequence != 5) {
								m_boltSoundSequence = 5;
								SE->Get((int)GetGunSoundSet().m_Cock1).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
							}
						}
					}
					//2
					{
						if ((10.f < GetAnime(GunAnimeID::ReloadOne).time && GetAnime(GunAnimeID::ReloadOne).time < 11.f)) {
							if (m_boltSoundSequence != 7) {
								m_boltSoundSequence = 7;
								SE->Get((int)GetGunSoundSet().m_Cock1).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
							}
						}
						else if (GetAnime(GunAnimeID::ReloadOne).per != 0.f) {
							m_boltSoundSequence = -1;
						}
					}
					//3
					{
						if ((8.f < GetAnime(GunAnimeID::ReloadEnd).time && GetAnime(GunAnimeID::ReloadEnd).time < 9.f)) {
							if (m_boltSoundSequence != 8) {
								m_boltSoundSequence = 8;
								SE->Get((int)GetGunSoundSet().m_Cock2).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
							}
						}
					}
					//4
					{
						if ((0.5f < GetAnime(GunAnimeID::Shot).time && GetAnime(GunAnimeID::Shot).time < 1.f)) {
							if (m_boltSoundSequence != 1) {
								m_boltSoundSequence = 1;
								SE->Get((int)GetGunSoundSet().m_Cock1).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
							}
						}
						if ((5.f < GetAnime(GunAnimeID::Shot).time && GetAnime(GunAnimeID::Shot).time < 6.f)) {
							if (m_boltSoundSequence != 3) {
								m_boltSoundSequence = 3;
								SE->Get((int)GetGunSoundSet().m_Cock2).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
							}
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					//0
					{
						if ((8.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 9.f)) {
							if (m_boltSoundSequence != 1) {
								m_boltSoundSequence = 1;
								SE->Get((int)GetGunSoundSet().m_Cock1).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
							}
						}
						if ((18.f < GetAnime(GunAnimeID::Cocking).time && GetAnime(GunAnimeID::Cocking).time < 19.f)) {
							if (m_boltSoundSequence != 2) {
								m_boltSoundSequence = 2;
								SE->Get((int)GetGunSoundSet().m_Cock2).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
							}
						}
					}
					//1
					{
						if ((0.f < GetAnime(GunAnimeID::ReloadStart_Empty).time && GetAnime(GunAnimeID::ReloadStart_Empty).time < 1.f)) {
							if (m_boltSoundSequence != 3) {
								m_boltSoundSequence = 3;
								SE->Get((int)GetGunSoundSet().m_Unload).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
							}
						}
						if ((0.f < GetAnime(GunAnimeID::ReloadStart).time && GetAnime(GunAnimeID::ReloadStart).time < 1.f)) {
							if (m_boltSoundSequence != 3) {
								m_boltSoundSequence = 3;
								SE->Get((int)GetGunSoundSet().m_Unload).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
							}
						}
					}
					//3
					{
						if ((0.f < GetAnime(GunAnimeID::ReloadEnd).time && GetAnime(GunAnimeID::ReloadEnd).time < 1.f)) {
							if (m_boltSoundSequence != 5) {
								m_boltSoundSequence = 5;
								SE->Get((int)GetGunSoundSet().m_Load).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
							}
						}
					}
					//6
					{
						if ((0.f < GetAnime(GunAnimeID::CheckStart).time && GetAnime(GunAnimeID::CheckStart).time < 1.f)) {
							if (m_boltSoundSequence != 6) {
								m_boltSoundSequence = 6;
								SE->Get((int)GetGunSoundSet().m_Unload).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
							}
						}
					}
					//7
					{
						if ((25.f < GetAnime(GunAnimeID::Checking).time && GetAnime(GunAnimeID::Checking).time < 26.f)) {
							if (m_boltSoundSequence != 7) {
								m_boltSoundSequence = 7;
								SE->Get((int)GetGunSoundSet().m_Load).Play_3D(0, GetMatrix().pos(), Scale_Rate*2.f);
							}
						}
					}
					break;
				default:
					break;
			}
			{
				ResetFrameLocalMat(GunFrame::Center);
				GetObj().work_anime();
				SetFrameLocalMat(GunFrame::Center, GetFrameLocalMat(GunFrame::Center).GetRot());//1のフレーム移動量を無視する
			}

			Easing(&m_GunChangePer, 1.f, 0.8f, EasingType::OutExpo);
			//共通
			ObjectBaseClass::FirstExecute();
			//弾薬の演算
			ExecuteCartInChamber();
		}
		void GunClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-0.5f*Scale_Rate, -0.5f*Scale_Rate, -0.5f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(0.5f*Scale_Rate, 0.5f*Scale_Rate, 0.5f*Scale_Rate)).get()) == FALSE
					) {
					if (isDrawSemiTrans) {
						DrawMuzzleSmoke();
					}
					for (int i = 0; i < this->GetObj().mesh_num(); i++) {
						if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
							this->GetObj().DrawMesh(i);
						}
					}
				}
			}
		}
		void GunClass::Dispose(void) noexcept {
			ObjectBaseClass::Dispose();
			DisposeModSlotControl();
		}
	};
};
