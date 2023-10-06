#include	"Gun.hpp"

#include "../../ObjectManager.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool  GunClass::HasFrame(GunFrame frame) const noexcept {
			//ŠY“–ƒtƒŒ[ƒ€‚ª‚ ‚é‚Ì‚È‚çã‘‚«
			if (m_ModControl.HasFrame(frame)) {
				return true;
			}
			if (HaveFrame(frame)) {
				return true;
			}
			return false;
		}
		const MATRIX_ref  GunClass::GetFrameLocalMat(GunFrame frame) const noexcept {
			//ŠY“–ƒtƒŒ[ƒ€‚ª‚ ‚é‚Ì‚È‚çã‘‚«
			MATRIX_ref Ret;
			if (m_ModControl.GetPartsFrameLocalMat(frame, &Ret)) {
				return Ret;
			}
			if (HaveFrame(frame)) {
				return GetFrameLocalMatrix(GetFrame(frame));
			}
			return MATRIX_ref::zero();
		}
		const MATRIX_ref  GunClass::GetFrameWorldMat(GunFrame frame) const noexcept {
			//ŠY“–ƒtƒŒ[ƒ€‚ª‚ ‚é‚Ì‚È‚çã‘‚«
			MATRIX_ref Ret;
			if (m_ModControl.GetPartsFrameWorldMat(frame, &Ret)) {
				return Ret;
			}
			if (HaveFrame(frame)) {
				return GetFrameWorldMatrix(GetFrame(frame));
			}
			return MATRIX_ref::zero();
		}

		void GunClass::ResetFrameLocalMat(GunFrame frame) noexcept {
			m_ModControl.ResetPartsFrameLocalMat(frame);
			if (HaveFrame(frame)) {
				GetObj().frame_Reset(GetFrame(frame));
			}
		}
		void GunClass::SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept {
			m_ModControl.SetPartsFrameLocalMat(frame, value);
			if (HaveFrame(frame)) {
				GetObj().SetFrameLocalMatrix(GetFrame(frame), value * this->GetFrameBaseLocalMat(frame));
			}
		}


		void GunClass::SetBullet(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			//
			this->m_ShotPhase = GunAnimeID::Shot;
			this->m_ShotSwitch = true;
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
			float Power = 0.0001f*this->m_GunDataClass->GetRecoilPower();
			this->m_RecoilRadAdd.Set(GetRandf(Power/4.f), -Power, 0.f);
			//this->m_RecoilRadAdd = MATRIX_ref::Vtrans(this->m_RecoilRadAdd, MATRIX_ref::RotZ(-m_LeanRad / 5.f));
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
					this->m_IsEject = false;
					this->m_IsEject |= (GetShoting() && (1.f <= GetNowAnime().time && GetNowAnime().time <= 2.f));
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

			//ƒf[ƒ^
			m_GunDataClass = GunDataManager::Instance()->LoadAction(this->m_FilePath);
			//ƒ}ƒKƒWƒ“‚Ì—pˆÓ
			if (m_GunDataClass->GetPartsSlot(GunSlot::Magazine)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Magazine)->m_Items.size();
				int ID = GetRand(size - 1);
				if (ID != size) {
					m_ModControl.SetParts(m_GunDataClass->GetPartsSlot(GunSlot::Magazine)->m_Items.at(ID), ObjType::Magazine, GetObj());
				}
			}
			//
			if (m_GunDataClass->GetPartsSlot(GunSlot::Upper)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Upper)->m_Items.size();
				int ID = GetRand(size - 1);
				if (ID != size) {
					m_ModControl.SetParts(m_GunDataClass->GetPartsSlot(GunSlot::Upper)->m_Items.at(ID), ObjType::Upper, GetObj());
				}
			}
			if (m_GunDataClass->GetPartsSlot(GunSlot::Lower)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Lower)->m_Items.size();
				int ID = GetRand(size - 1);
				if (ID != size) {
					m_ModControl.SetParts(m_GunDataClass->GetPartsSlot(GunSlot::Lower)->m_Items.at(ID), ObjType::Lower, GetObj());
				}
			}
			if (m_GunDataClass->GetPartsSlot(GunSlot::Barrel)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Barrel)->m_Items.size();
				int ID = GetRand(size - 1);
				if (ID != size) {
					m_ModControl.SetParts(m_GunDataClass->GetPartsSlot(GunSlot::Barrel)->m_Items.at(ID), ObjType::Barrel, GetObj());
				}
			}
			if (m_GunDataClass->GetPartsSlot(GunSlot::Sight)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Sight)->m_Items.size();
				int ID = GetRand(size);
				if (ID != size) {
					m_ModControl.SetParts(m_GunDataClass->GetPartsSlot(GunSlot::Sight)->m_Items.at(ID), ObjType::Sight, GetObj());
				}
			}
			{
				std::vector<const std::shared_ptr<ObjectBaseClass>*> PartsList;
				m_ModControl.GetChildPartsList(&PartsList);
				m_SightPtr = nullptr;
				m_MuzzlePtr = nullptr;
				m_UpperPtr = nullptr;
				m_MagazinePtr = nullptr;
				for (auto& p : PartsList) {
					if ((*p)->GetobjType() == ObjType::Sight) {
						m_SightPtr = &((std::shared_ptr<SightClass>&)(*p));
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
			//
			(*m_MagazinePtr)->SetReloadType(GetReloadType());
			FillMag();															//ƒ}ƒKƒWƒ“‚Íƒtƒ‹‘•“U
			CockByMag();														//ƒ`ƒƒƒ“ƒo[ƒCƒ“
			//
			m_MagFall.Init(m_BackGround, (*m_MagazinePtr)->GetFilePath());
			m_CartFall.Init(m_BackGround, (*m_MagazinePtr)->GetAmmoSpecMagTop()->GetPath());	//‘•“U‚µ‚½ƒ}ƒKƒWƒ“‚Ì’e‚É‡‚í‚¹‚Ä–òä°¶¬
			//
			this->m_ShotPhase = GunAnimeID::Base;
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
						SetAnimOnce(i, ((float)this->m_GunDataClass->GetShotRate()) / 300.f);//•ªŠÔ300
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
				SetFrameLocalMat(GunFrame::Center, GetFrameLocalMat(GunFrame::Center).GetRot());//1‚ÌƒtƒŒ[ƒ€ˆÚ“®—Ê‚ð–³Ž‹‚·‚é
			}
			//‹¤’Ê
			ObjectBaseClass::FirstExecute();
			//’e–ò‚Ì‰‰ŽZ
			ExecuteCartInChamber();
			//ƒŠƒRƒCƒ‹‚Ì‰‰ŽZ
			if (this->m_RecoilRadAdd.y() < 0.f) {
				Easing(&this->m_RecoilRadAdd, VECTOR_ref::vget(0.f, 0.01f, 0.f), this->m_GunDataClass->GetRecoilReturn(), EasingType::OutExpo);
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
			m_BackGround.reset();
			m_GunDataClass.reset();
		}
	};
};
