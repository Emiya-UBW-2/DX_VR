#include	"Gun.hpp"

#include "../../ObjectManager.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const MATRIX_ref  GunClass::GetFrameLocalMat(GunFrame frame) const noexcept {
			//ŠY“–ƒtƒŒ[ƒ€‚ª‚ ‚é‚Ì‚È‚çã‘‚«
			MATRIX_ref Ret;
			if (m_ModControl.GetPartsFrameLocalMat(frame, &Ret)) {
				return Ret;
			}
			if (GetFrame(frame) != -1) {
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
			if (GetFrame(frame) != -1) {
				return GetFrameWorldMatrix(GetFrame(frame));
			}
			return MATRIX_ref::zero();
		}

		void GunClass::ResetFrameLocalMat(GunFrame frame) noexcept {
			m_ModControl.ResetPartsFrameLocalMat(frame);
			if (GetFrame(frame) != -1) {
				GetObj().frame_Reset(GetFrame(frame));
			}
		}
		void GunClass::SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept {
			m_ModControl.SetPartsFrameLocalMat(frame, value);
			if (GetFrame(frame) != -1) {
				GetObj().SetFrameLocalMatrix(GetFrame(frame), value * this->m_Frames[(int)frame].second);
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
			SE->Get((int)GetGunSoundSet().m_Shot).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
			//
			{
				auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->MakeObject(ObjType::Ammo));
				LastAmmo->Init();
				auto mat = MATRIX_ref::RotZ(deg2rad(GetRandf(1.f))) * MATRIX_ref::RotX(deg2rad(GetRandf(1.f))) * GetMuzzleMatrix();
				LastAmmo->Put(this->m_ChamberAmmoData, mat.pos(), mat.zvec() * -1.f, m_MyID);
			}
			//
			UnloadChamber();
			//
			AddMuzzleSmokePower();
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
					this->m_IsChamberOn |= ((this->m_ShotPhase == GunAnimeID::ReloadEnd) && (GetNowAnime().time >= 5.f));
					this->m_IsChamberOn |= (GetCocking() && (GetNowAnime().time >= 25.f));
					break;
				default:
					break;
				}
				if (this->m_IsChamberOn && (this->m_IsChamberOn != Prev)) {
					if (!GetIsMagEmpty()) {
						CockByMag();
					}
					GetMagazinePtr()->SubAmmo();
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
					m_CartFall.SetFall(GetCartMat().pos(), GetMuzzleMatrix().GetRot(), GetCartVec()*(Scale_Rate * 2.f / 60.f), 2.f);
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
				m_ModControl.SetParts(m_GunDataClass->GetPartsSlot(GunSlot::Magazine)->m_Items.at(GetRand(size - 1)), ObjType::Magazine, GetObj());
			}
			//
			if (m_GunDataClass->GetPartsSlot(GunSlot::Lower)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Lower)->m_Items.size();
				m_ModControl.SetParts(m_GunDataClass->GetPartsSlot(GunSlot::Lower)->m_Items.at(GetRand(size - 1)),ObjType::Lower, GetObj());
			}
			//
			if (m_GunDataClass->GetPartsSlot(GunSlot::Upper)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Upper)->m_Items.size();
				m_ModControl.SetParts(m_GunDataClass->GetPartsSlot(GunSlot::Upper)->m_Items.at(GetRand(size - 1)), ObjType::Upper, GetObj());
			}

			if (m_GunDataClass->GetPartsSlot(GunSlot::Barrel)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Barrel)->m_Items.size();
				m_ModControl.SetParts(m_GunDataClass->GetPartsSlot(GunSlot::Barrel)->m_Items.at(GetRand(size - 1)), ObjType::Barrel, GetObj());
			}
			m_PartsList.clear();
			m_ModControl.GetChildPartsList(&m_PartsList);
			m_SightPtr = nullptr;
			for (auto& p : m_PartsList) {
				if ((*p)->GetobjType() == ObjType::Sight) {
					m_SightPtr = &((std::shared_ptr<SightClass>&)(*p));
				}
			}
			//
			GetMagazinePtr()->SetReloadType(GetReloadType());
			FillMag();															//ƒ}ƒKƒWƒ“‚Íƒtƒ‹‘•“U
			CockByMag();														//ƒ`ƒƒƒ“ƒo[ƒCƒ“
			//
			m_MagFall.Init(m_BackGround, GetMagazinePtr()->GetFilePath());
			m_CartFall.Init(m_BackGround, GetMagazinePtr()->GetAmmoSpecMagTop()->GetPath());	//‘•“U‚µ‚½ƒ}ƒKƒWƒ“‚Ì’e‚É‡‚í‚¹‚Ä–òä°¶¬
			//
			this->m_ShotPhase = GunAnimeID::Base;
		}
		void GunClass::FirstExecute(void) noexcept {
			auto SE = SoundPool::Instance();
			if (this->m_IsFirstLoop) {
				InitMuzzleSmoke(GetMuzzleMatrix().pos());
			}
			else {
				ExecuteMuzzleSmoke(GetMuzzleMatrix().pos());
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
								GetMagazinePtr()->AddAmmo();
								break;
							default:
								break;
							}
						}
					}
					GetObj().get_anime(i).per = 1.f;
					if (Sel == GunAnimeID::Shot) {
						SetAnimOnce(i, 1.5f);
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
