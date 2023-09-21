#include	"Gun.hpp"

#include "../../ObjectManager.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const MATRIX_ref  GunClass::GetFrameLocalMat(GunFrame frame) const noexcept {
			if (GetFrame(frame) != -1) {
				return GetObj_const().GetFrameLocalMatrix(GetFrame(frame));
			}
			else {
				switch (frame) {
				case FPS_n2::Sceneclass::GunFrame::Center:
				case FPS_n2::Sceneclass::GunFrame::Move:
				case FPS_n2::Sceneclass::GunFrame::MoveAxis:
				case FPS_n2::Sceneclass::GunFrame::MovePoint:
					break;
				case FPS_n2::Sceneclass::GunFrame::Muzzle:
					if (HasParts(ObjType::Barrel)) {
						return GetPartsPtr(ObjType::Barrel)->GetFrameLocalMat(frame);
					}
					break;
				case FPS_n2::Sceneclass::GunFrame::Cart:
				case FPS_n2::Sceneclass::GunFrame::CartVec:
				case FPS_n2::Sceneclass::GunFrame::Eyepos:
				case FPS_n2::Sceneclass::GunFrame::Lens:
				case FPS_n2::Sceneclass::GunFrame::LensSize:
					if (HasParts(ObjType::Upper)) {
						return GetPartsPtr(ObjType::Upper)->GetFrameLocalMat(frame);
					}
					break;

				case FPS_n2::Sceneclass::GunFrame::Magpos:
				case FPS_n2::Sceneclass::GunFrame::LeftHandPos:
				case FPS_n2::Sceneclass::GunFrame::LeftHandYvec:
				case FPS_n2::Sceneclass::GunFrame::LeftHandZvec:
				case FPS_n2::Sceneclass::GunFrame::RightHandPos:
				case FPS_n2::Sceneclass::GunFrame::RightHandYvec:
				case FPS_n2::Sceneclass::GunFrame::RightHandZvec:
					if (HasParts(ObjType::Lower)) {
						return GetPartsPtr(ObjType::Lower)->GetFrameLocalMat(frame);
					}
					break;
				default:
					break;
				}
			}
			return MATRIX_ref::zero();
		}
		const MATRIX_ref  GunClass::GetFrameWorldMat(GunFrame frame) const noexcept {
			if (GetFrame(frame) != -1) {
				return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(frame));
			}
			else {
				switch (frame) {
				case FPS_n2::Sceneclass::GunFrame::Center:
				case FPS_n2::Sceneclass::GunFrame::Move:
				case FPS_n2::Sceneclass::GunFrame::MoveAxis:
				case FPS_n2::Sceneclass::GunFrame::MovePoint:
					break;
				case FPS_n2::Sceneclass::GunFrame::Muzzle:
					if (HasParts(ObjType::Barrel)) {
						return GetPartsPtr(ObjType::Barrel)->GetFrameWorldMat(frame);
					}
					break;
				case FPS_n2::Sceneclass::GunFrame::Cart:
				case FPS_n2::Sceneclass::GunFrame::CartVec:
				case FPS_n2::Sceneclass::GunFrame::Eyepos:
				case FPS_n2::Sceneclass::GunFrame::Lens:
				case FPS_n2::Sceneclass::GunFrame::LensSize:
					if (HasParts(ObjType::Upper)) {
						return GetPartsPtr(ObjType::Upper)->GetFrameWorldMat(frame);
					}
					break;

				case FPS_n2::Sceneclass::GunFrame::Magpos:
				case FPS_n2::Sceneclass::GunFrame::LeftHandPos:
				case FPS_n2::Sceneclass::GunFrame::LeftHandYvec:
				case FPS_n2::Sceneclass::GunFrame::LeftHandZvec:
				case FPS_n2::Sceneclass::GunFrame::RightHandPos:
				case FPS_n2::Sceneclass::GunFrame::RightHandYvec:
				case FPS_n2::Sceneclass::GunFrame::RightHandZvec:
					if (HasParts(ObjType::Lower)) {
						return GetPartsPtr(ObjType::Lower)->GetFrameWorldMat(frame);
					}
					break;
				default:
					break;
				}
			}
			return MATRIX_ref::zero();
		}

		void GunClass::ResetFrameLocalMat(GunFrame frame) noexcept {
			if (GetFrame(frame) != -1) {
				GetObj().frame_Reset(GetFrame(frame));
			}
			else {
				switch (frame) {
				case FPS_n2::Sceneclass::GunFrame::Center:
				case FPS_n2::Sceneclass::GunFrame::Move:
				case FPS_n2::Sceneclass::GunFrame::MoveAxis:
				case FPS_n2::Sceneclass::GunFrame::MovePoint:
					break;
				case FPS_n2::Sceneclass::GunFrame::Muzzle:
					if (HasParts(ObjType::Barrel)) {
						GetPartsPtr(ObjType::Barrel)->ResetFrameLocalMat(frame);
					}
					break;
				case FPS_n2::Sceneclass::GunFrame::Cart:
				case FPS_n2::Sceneclass::GunFrame::CartVec:
				case FPS_n2::Sceneclass::GunFrame::Eyepos:
				case FPS_n2::Sceneclass::GunFrame::Lens:
				case FPS_n2::Sceneclass::GunFrame::LensSize:
					if (HasParts(ObjType::Upper)) {
						GetPartsPtr(ObjType::Upper)->ResetFrameLocalMat(frame);
					}
					break;

				case FPS_n2::Sceneclass::GunFrame::Magpos:
				case FPS_n2::Sceneclass::GunFrame::LeftHandPos:
				case FPS_n2::Sceneclass::GunFrame::LeftHandYvec:
				case FPS_n2::Sceneclass::GunFrame::LeftHandZvec:
				case FPS_n2::Sceneclass::GunFrame::RightHandPos:
				case FPS_n2::Sceneclass::GunFrame::RightHandYvec:
				case FPS_n2::Sceneclass::GunFrame::RightHandZvec:
					if (HasParts(ObjType::Lower)) {
						GetPartsPtr(ObjType::Lower)->ResetFrameLocalMat(frame);
					}
					break;
				default:
					break;
				}
			}
		}
		void GunClass::SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept {
			if (GetFrame(frame) != -1) {
				GetObj().SetFrameLocalMatrix(GetFrame(frame), value * this->m_Frames[(int)frame].second);
			}
			else {
				switch (frame) {
				case FPS_n2::Sceneclass::GunFrame::Center:
				case FPS_n2::Sceneclass::GunFrame::Move:
				case FPS_n2::Sceneclass::GunFrame::MoveAxis:
				case FPS_n2::Sceneclass::GunFrame::MovePoint:
					break;
				case FPS_n2::Sceneclass::GunFrame::Muzzle:
					if (HasParts(ObjType::Barrel)) {
						GetPartsPtr(ObjType::Barrel)->SetFrameLocalMat(frame, value);
					}
					break;
				case FPS_n2::Sceneclass::GunFrame::Cart:
				case FPS_n2::Sceneclass::GunFrame::CartVec:
				case FPS_n2::Sceneclass::GunFrame::Eyepos:
				case FPS_n2::Sceneclass::GunFrame::Lens:
				case FPS_n2::Sceneclass::GunFrame::LensSize:
					if (HasParts(ObjType::Upper)) {
						GetPartsPtr(ObjType::Upper)->SetFrameLocalMat(frame, value);
					}
					break;

				case FPS_n2::Sceneclass::GunFrame::Magpos:
				case FPS_n2::Sceneclass::GunFrame::LeftHandPos:
				case FPS_n2::Sceneclass::GunFrame::LeftHandYvec:
				case FPS_n2::Sceneclass::GunFrame::LeftHandZvec:
				case FPS_n2::Sceneclass::GunFrame::RightHandPos:
				case FPS_n2::Sceneclass::GunFrame::RightHandYvec:
				case FPS_n2::Sceneclass::GunFrame::RightHandZvec:
					if (HasParts(ObjType::Lower)) {
						GetPartsPtr(ObjType::Lower)->SetFrameLocalMat(frame, value);
					}
					break;
				default:
					break;
				}
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
			auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->AddObject(ObjType::Ammo));
			auto mat = MATRIX_ref::RotZ(deg2rad(GetRandf(1.f))) * MATRIX_ref::RotX(deg2rad(GetRandf(1.f))) * GetMuzzleMatrix();
			LastAmmo->Put(this->m_ChamberAmmoData, mat.pos(), mat.zvec() * -1.f, m_MyID);
			//
			UnloadChamber();
			//
			AddMuzzleSmokePower();
		}
		void GunClass::SetMagFall() noexcept { SetFallMagControl(GetFrameWorldMat(GunFrame::Magpos)); }

		void GunClass::ExecuteCartInChamber(void) noexcept {
			auto Prev = this->m_IsChamberOn;//GetNowAnime
			switch (GetShotType()) {
			case SHOTTYPE::FULL:
			case SHOTTYPE::SEMI:
				this->m_IsChamberOn = false;
				this->m_IsChamberOn |= (GetShoting() && (GetNowAnime().time >= 3.f));
				this->m_IsChamberOn |= (GetCocking() && (GetNowAnime().time >= 25.f));
				if (GetShoting() && (1.f <= GetNowAnime().time && GetNowAnime().time <= 2.f)) {
					SetCart();
				}
				break;
			case SHOTTYPE::BOLT:
				this->m_IsChamberOn = false;
				this->m_IsChamberOn |= ((this->m_ShotPhase == GunAnimeID::ReloadEnd) && (GetNowAnime().time >= 5.f));
				this->m_IsChamberOn |= (GetCocking() && (GetNowAnime().time >= 25.f));
				{
					bool IsEject = false;
					IsEject |= ((this->m_ShotPhase == GunAnimeID::ReloadStart_Empty || this->m_ShotPhase == GunAnimeID::ReloadStart) && (GetNowAnime().time >= 19.f && !GetNowAnime().TimeEnd()));
					IsEject |= (GetCocking() && (GetNowAnime().time >= 19.f && !GetNowAnime().TimeEnd()));
					if (IsEject) {
						SetCart();
					}
				}
				break;
			default:
				break;
			}

			if (this->m_IsChamberOn && (this->m_IsChamberOn != Prev)) {
				if (!GetIsMagEmpty()) {
					CockByMag();
				}
				SubOnceMag();
			}
		}


		void GunClass::Init(void) noexcept {
			ObjectBaseClass::Init();

			//ƒf[ƒ^
			m_GunDataClass = GunDataManager::Instance()->LoadAction(this->m_FilePath);
			//ƒ}ƒKƒWƒ“‚Ì—pˆÓ
			{
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Magazine)->m_Items.size();
				std::string magName = "data/Mods/mag/" + m_GunDataClass->GetPartsSlot(GunSlot::Magazine)->m_Items.at(GetRand(size - 1)) + "/";
				HaveMag(magName, GetReloadType());
				InitFallMagControl(magName);
				FillMag();												//ƒ}ƒKƒWƒ“‚Íƒtƒ‹‘•“U
				InitFallCartControl(GetAmmoSpecMagTop()->GetPath());	//‘•“U‚µ‚½ƒ}ƒKƒWƒ“‚Ì’e‚É‡‚í‚¹‚Ä–òä°¶¬
				CockByMag();											//ƒ`ƒƒƒ“ƒo[ƒCƒ“
			}
			//
			if (m_GunDataClass->GetPartsSlot(GunSlot::Lower)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Lower)->m_Items.size();
				std::string magName = "data/Mods/Lower/" + m_GunDataClass->GetPartsSlot(GunSlot::Lower)->m_Items.at(GetRand(size - 1)) + "/";
				SetParts(magName,ObjType::Lower);
			}
			//
			if (m_GunDataClass->GetPartsSlot(GunSlot::Upper)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Upper)->m_Items.size();
				std::string magName = "data/Mods/Upper/" + m_GunDataClass->GetPartsSlot(GunSlot::Upper)->m_Items.at(GetRand(size - 1)) + "/";
				SetParts(magName, ObjType::Upper);
			}

			if (m_GunDataClass->GetPartsSlot(GunSlot::Barrel)) {
				int size = (int)m_GunDataClass->GetPartsSlot(GunSlot::Barrel)->m_Items.size();
				std::string magName = "data/Mods/Barrel/" + m_GunDataClass->GetPartsSlot(GunSlot::Barrel)->m_Items.at(GetRand(size - 1)) + "/";
				SetParts(magName, ObjType::Barrel);
			}
			

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
								AddOnceMag();
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
			ExecuteFallCartControl(GetCartMat().pos(), GetMuzzleMatrix().GetRot(), GetCartVec());
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
			m_GunDataClass.reset();
		}
	};
};
