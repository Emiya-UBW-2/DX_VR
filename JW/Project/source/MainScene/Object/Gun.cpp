#include	"Gun.hpp"

#include "../../ObjectManager.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool  GunClass::HasFrame(GunFrame frame) const noexcept {
			//äYìñÉtÉåÅ[ÉÄÇ™Ç†ÇÈÇÃÇ»ÇÁè„èëÇ´
			if (m_SightPtr) {
				switch (frame) {
				case GunFrame::Eyepos:
				case GunFrame::Lens:
				case GunFrame::LensSize:
					return true;
				default:
					break;
				}
			}
			if (GetSlotControl()->HasFrame(frame)) {
				return true;
			}
			if (HaveFrame(frame)) {
				return true;
			}
			return false;
		}
		const MATRIX_ref  GunClass::GetFrameLocalMat(GunFrame frame) const noexcept {
			//äYìñÉtÉåÅ[ÉÄÇ™Ç†ÇÈÇÃÇ»ÇÁè„èëÇ´
			if (m_SightPtr) {
				switch (frame) {
				case GunFrame::Eyepos:
				case GunFrame::Lens:
				case GunFrame::LensSize:
					return (*m_SightPtr)->GetFrameLocalMat(frame);
				default:
					break;
				}
			}
			MATRIX_ref Ret;
			if (GetSlotControl()->GetPartsFrameLocalMat(frame, &Ret)) {
				return Ret;
			}
			if (HaveFrame(frame)) {
				return GetFrameLocalMatrix(GetFrame(frame));
			}
			return MATRIX_ref::zero();
		}
		const MATRIX_ref  GunClass::GetFrameWorldMat(GunFrame frame) const noexcept {
			//äYìñÉtÉåÅ[ÉÄÇ™Ç†ÇÈÇÃÇ»ÇÁè„èëÇ´
			if (m_SightPtr) {
				switch (frame) {
				case GunFrame::Eyepos:
				case GunFrame::Lens:
				case GunFrame::LensSize:
					return (*m_SightPtr)->GetFrameWorldMat(frame);
				default:
					break;
				}
			}
			MATRIX_ref Ret;
			if (GetSlotControl()->GetPartsFrameWorldMat(frame, &Ret)) {
				return Ret;
			}
			if (HaveFrame(frame)) {
				return GetFrameWorldMatrix(GetFrame(frame));
			}
			return MATRIX_ref::zero();
		}

		void GunClass::ResetFrameLocalMat(GunFrame frame) noexcept {
			GetSlotControl()->ResetPartsFrameLocalMat(frame);
			if (HaveFrame(frame)) {
				GetObj().frame_Reset(GetFrame(frame));
			}
		}
		void GunClass::SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept {
			GetSlotControl()->SetPartsFrameLocalMat(frame, value);
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
			//
			for (int i = 0; i < this->m_ChamberAmmoData->GetPellet(); i++) {
				auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->MakeObject(ObjType::Ammo));
				LastAmmo->Init();
				auto mat = 
					MATRIX_ref::RotY(deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) * 
					MATRIX_ref::RotX(deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) * 
					GetFrameWorldMat(GunFrame::Muzzle);
				LastAmmo->Put(this->m_ChamberAmmoData, mat.pos(), mat.zvec() * -1.f, m_MyID);
			}
			//
			UnloadChamber();
			//
			AddMuzzleSmokePower();
			//
			float Power = 0.0001f*GetGunDataClass()->GetRecoilPower();
			this->m_RecoilRadAdd.Set(GetRandf(Power/4.f), -Power, 0.f);
			//this->m_RecoilRadAdd = MATRIX_ref::Vtrans(this->m_RecoilRadAdd, MATRIX_ref::RotZ(-m_LeanRad / 5.f));
		}
		void	GunClass::SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept {
			m_MagFall.Dispose();
			m_CartFall.Dispose();
			m_MagFall.Init(backGround, (*m_MagazinePtr)->GetFilePath());
			m_CartFall.Init(backGround, (*m_MagazinePtr)->GetAmmoSpecMagTop()->GetPath());	//ëïìUÇµÇΩÉ}ÉKÉWÉìÇÃíeÇ…çáÇÌÇπÇƒñÚ‰∞ê∂ê¨
		}
		void	GunClass::Init_Gun(void) noexcept {
			{
				std::vector<const std::shared_ptr<ObjectBaseClass>*> PartsList;
				GetSlotControl()->GetChildPartsList(&PartsList);
				m_SightPtr = nullptr;
				m_MuzzlePtr = nullptr;
				m_UpperPtr = nullptr;
				m_MagazinePtr = nullptr;
				for (auto& p : PartsList) {
					if ((*p)->GetobjType() == ObjType::Sight) {
						const auto* Ptr = &((std::shared_ptr<SightClass>&)(*p));
						if (m_SightPtr) {
							if (!(*Ptr)->GetReitcleGraph().IsActive()) {
								continue;
							}
						}
						m_SightPtr = Ptr;
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
				}
				PartsList.clear();
			}
			(*m_MagazinePtr)->SetReloadType(GetReloadType());
			FillMag();															//É}ÉKÉWÉìÇÕÉtÉãëïìU
			CockByMag();														//É`ÉÉÉìÉoÅ[ÉCÉì
			//
			//
			this->m_ShotPhase = GunAnimeID::Base;
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
					(*m_MagazinePtr)->SubAmmo();
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
			//ÉfÅ[É^
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
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadOne) = 30;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::ReloadEnd) = 10;
				m_CharaAnimeSet.back().at((int)CharaGunAnimeID::Watch) = 60;
				//ÉnÉìÉhÉKÉì
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
				//ÉnÉìÉhÉKÉì
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
					if ((GunAnimeID)i == GunAnimeID::ReloadOne) {
						if (GetObj().get_anime(i).time == 0.f) {
							switch (GetReloadType()) {
							case RELOADTYPE::MAG:
								FillMag();
								break;
							case RELOADTYPE::AMMO:
								(*m_MagazinePtr)->AddAmmo();
								break;
							default:
								break;
							}
						}
					}
					GetObj().get_anime(i).per = 1.f;
					if (Sel == GunAnimeID::Shot) {
						SetAnimOnce(i, ((float)this->GetGunDataClass()->GetShotRate()) / 300.f);//ï™ä‘300
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
						if (GetAmmoNum() == 0) {
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
				SetFrameLocalMat(GunFrame::Center, GetFrameLocalMat(GunFrame::Center).GetRot());//1ÇÃÉtÉåÅ[ÉÄà⁄ìÆó Çñ≥éãÇ∑ÇÈ
			}
			//ã§í 
			ObjectBaseClass::FirstExecute();
			//íeñÚÇÃââéZ
			ExecuteCartInChamber();
			//ÉäÉRÉCÉãÇÃââéZ
			if (this->m_RecoilRadAdd.y() < 0.f) {
				Easing(&this->m_RecoilRadAdd, VECTOR_ref::vget(0.f, 0.01f, 0.f), this->GetGunDataClass()->GetRecoilReturn(), EasingType::OutExpo);
			}
			else {
				Easing(&this->m_RecoilRadAdd, VECTOR_ref::zero(), 0.7f, EasingType::OutExpo);
			}
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
