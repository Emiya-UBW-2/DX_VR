#include	"Gun.hpp"

#include "../../ObjectManager.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void				GunClass::ExecuteCartInChamber(void) noexcept {
			{
				auto Prev = this->m_IsChamberOn;
				switch (GetShotType()) {
					case SHOTTYPE::FULL:
					case SHOTTYPE::SEMI:
						this->m_IsChamberOn = false;
						this->m_IsChamberOn |= ((GetGunAnime() == GunAnimeID::Shot) && (GetNowAnime().time >= 3.f));
						this->m_IsChamberOn |= ((GetGunAnime() == GunAnimeID::Cocking) && (GetNowAnime().time >= 25.f));
						break;
					case SHOTTYPE::BOLT:
						this->m_IsChamberOn = false;
						this->m_IsChamberOn |= ((GetGunAnime() == GunAnimeID::Cocking) && (GetNowAnime().time >= 25.f));
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
						this->m_IsEject |= ((GetGunAnime() == GunAnimeID::ReloadStart_Empty || GetGunAnime() == GunAnimeID::ReloadStart) && (GetNowAnime().time >= 19.f && !GetNowAnime().TimeEnd()));
						this->m_IsEject |= ((GetGunAnime() == GunAnimeID::Cocking) && (GetNowAnime().time >= 19.f && !GetNowAnime().TimeEnd()));
						break;
					default:
						break;
				}
				if (this->m_IsEject && (this->m_IsEject != Prev)) {
					m_CartFall.SetFall(GetCartMat().pos(), GetFrameWorldMat(GunFrame::Muzzle).GetRot(), GetCartVec()*(Scale_Rate * 2.f / 60.f), 2.f, SoundEnum::CartFall);
				}
			}
		}
		void				GunClass::ExecuteSound(void) noexcept {
			auto SE = SoundPool::Instance();
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
		}
		//
		const bool			GunClass::HasFrame(GunFrame frame) const noexcept {
			//該当フレームがあるのなら上書き
			if (m_SightPtr.at(m_GunSightSel)) {
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
		const MATRIX_ref	GunClass::GetFrameLocalMat(GunFrame frame) const noexcept {
			//該当フレームがあるのなら上書き
			if (m_SightPtr.at(m_GunSightSel)) {
				switch (frame) {
					case GunFrame::Eyepos:
					case GunFrame::Lens:
					case GunFrame::LensSize:
						return (*m_SightPtr.at(m_GunSightSel))->GetFrameLocalMat(frame);
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
		const MATRIX_ref	GunClass::GetFrameWorldMat(GunFrame frame, bool CheckSight) const noexcept {
			//該当フレームがあるのなら上書き
			if (m_SightPtr.at(m_GunSightSel) && CheckSight) {
				switch (frame) {
					case GunFrame::Eyepos:
					case GunFrame::Lens:
					case GunFrame::LensSize:
						return (*m_SightPtr.at(m_GunSightSel))->GetFrameWorldMat(frame);
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

		void				GunClass::ResetFrameLocalMat(GunFrame frame) noexcept {
			ModSlotControl::ResetPartsFrameLocalMat(frame);
			if (HaveFrame(frame)) {
				GetObj().frame_Reset(GetFrame(frame));
			}
		}
		void				GunClass::SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept {
			ModSlotControl::SetPartsFrameLocalMat(frame, value);
			if (HaveFrame(frame)) {
				GetObj().SetFrameLocalMatrix(GetFrame(frame), value * this->GetFrameBaseLocalMat(frame));
			}
		}
		void				GunClass::SetActiveAll(bool value) noexcept {
			bool isActive = this->IsActive();
			if (value) {
				if (!isActive) {
					this->SetActiveBySlot(true);
					this->SetActive(true);
				}
			}
			else {
				if (isActive) {
					this->SetActiveBySlot(false);
					this->SetActive(false);
				}
			}
		}
		void				GunClass::SetBullet(void) noexcept {
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
			SetGunAnime(GunAnimeID::Shot);
			this->m_ShotSwitch = true;
		}
		void				GunClass::UpdateGunAnims() noexcept {
			auto prev = this->m_GunAnimeFirst;
			switch (GetGunAnime()) {
				case GunAnimeID::Base:
					break;
				case GunAnimeID::Shot:
					UpdateGunAnim(GetGunAnime(), ((float)this->GetGunDataClass()->GetShotRate()) / 300.f);//分間300
					break;
				case GunAnimeID::Cocking:
					UpdateGunAnim(GetGunAnime(), 1.5f);
					if (GetGunAnimEnd(GetGunAnime())) {
						SetGunAnime(GunAnimeID::Base);
					}
					break;
				case GunAnimeID::ReloadStart_Empty:
					if (this->m_GunAnimeFirst) {
						m_UpperAnim = 0.f;
					}
					UpdateGunAnim(GetGunAnime(), 1.5f);
					if (GetGunAnimEnd(GetGunAnime())) {
						SetGunAnime(GunAnimeID::ReloadOne);
					}
					break;
				case GunAnimeID::ReloadStart:
					if (this->m_GunAnimeFirst) {
						m_UpperAnim = 0.f;
					}
					UpdateGunAnim(GetGunAnime(), 1.5f);
					if (GetGunAnimEnd(GetGunAnime())) {
						SetGunAnime(GunAnimeID::ReloadOne);
					}
					break;
				case GunAnimeID::ReloadOne:
					if (this->m_GunAnimeFirst) {
						m_UpperAnim = 0.f;
					}
					UpdateGunAnim(GetGunAnime(), 1.5f);
					if (GetGunAnimePer(EnumGunAnimType::Reload) >= 1.f) {
						switch (this->GetGunDataClass()->GetReloadType()) {
							case RELOADTYPE::MAG:
								//ユニークIDが異なる場合登録するオブジェと切り替える
								if (this->m_NextMagUniqueID != (*m_MagazinePtr)->GetModData()->GetUniqueID()) {
									m_MagazinePtr = &((std::shared_ptr<MagazineClass>&)(ModSlotControl::SetMod(GunSlot::Magazine, this->m_NextMagUniqueID, this->GetObj())));
								}
								SetAmmo(m_NextMagNum);
								SetGunAnime(GunAnimeID::ReloadEnd);
								break;
							case RELOADTYPE::AMMO:
								SetAmmo(this->m_Capacity + 1);
								m_NextMagNum--;
								if ((this->GetIsMagFull() || (m_NextMagNum <= 0)) || this->m_Cancel) {
									this->m_Cancel = false;
									SetGunAnime(GunAnimeID::ReloadEnd);
								}
								else {
									this->GetObj().get_anime((int)GetGunAnime()).Reset();
									this->GetObj().get_anime((int)GetGunAnime()).per = 1.f;
									m_UpperAnim = 0.f;
								}
								break;
							default:
								break;
						}
					}
					break;
				case GunAnimeID::ReloadEnd:
					UpdateGunAnim(GetGunAnime(), 1.5f);
					if (GetGunAnimEnd(GetGunAnime())) {
						SetGunAnime(GunAnimeID::Checking);
					}
					break;
				case GunAnimeID::CheckStart:
					UpdateGunAnim(GetGunAnime(), 1.5f);
					if (GetGunAnimEnd(GetGunAnime())) {
						SetGunAnime(GunAnimeID::Checking);
					}
					break;
				case GunAnimeID::Checking:
					UpdateGunAnim(GetGunAnime(), 1.f);
					if (GetGunAnimEnd(GetGunAnime())) {
						SetGunAnime(GunAnimeID::CheckEnd);
					}
					break;
				case GunAnimeID::CheckEnd:
					UpdateGunAnim(GetGunAnime(), 2.f);
					if (GetGunAnimEnd(GetGunAnime())) {
						SetGunAnime(GunAnimeID::Base);
					}
					break;
				case GunAnimeID::Open:
					break;
				case GunAnimeID::Watch:
					if (this->m_GunAnimeFirst) {
						m_UpperAnim = 0.f;
					}
					if (GetGunAnimePer(EnumGunAnimType::Watch) >= 1.f) {
						SetGunAnime(GunAnimeID::Base);
					}
					break;
				case GunAnimeID::Melee:
					if (this->m_GunAnimeFirst) {
						m_UpperAnim = 0.f;
					}
					if (GetGunAnimePer(EnumGunAnimType::Melee) >= 1.f) {
						SetGunAnime(GunAnimeID::Base);
					}
					break;
				case GunAnimeID::AmmoLoading:
					if (this->m_GunAnimeFirst) {
						this->m_boltSoundSequence = -1;
					}
					UpdateGunAnim(GetGunAnime(), 1.0f);
					if (GetGunAnimEnd(GetGunAnime())) {
						if (this->m_Cancel) {
							this->m_Cancel = false;
							SetGunAnime(GunAnimeID::AmmoLoadEnd);
						}
						else {
							this->GetObj().get_anime((int)GetGunAnime()).Reset();
							this->GetObj().get_anime((int)GetGunAnime()).per = 1.f;
						}
					}
					//サウンド
					if (GetTimePer(GetGunAnime()) > 0.3f) {
						if (m_boltSoundSequence != 9) {
							m_boltSoundSequence = 9;
							auto* SE = SoundPool::Instance();
							SE->Get((int)SoundEnum::AmmoLoad).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
						}
					}
					break;
				case GunAnimeID::AmmoLoadEnd:
					UpdateGunAnim(GetGunAnime(), 1.5f);
					break;
				case GunAnimeID::OffsetAnim:
					break;
				default:
					break;
			}
			//
			m_UpperAnim += 60.f / FPS;
			if (prev) {
				this->m_GunAnimeFirst = false;
			}
		}

		void				GunClass::SetFallObject(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept {
			m_MagFall.Dispose();
			m_CartFall.Dispose();
			m_MagFall.Init(backGround, (*m_MagazinePtr)->GetFilePath(), 1);
			m_CartFall.Init(backGround, (*m_MagazinePtr)->GetModData()->GetAmmoSpecMagTop()->GetPath(), 4);	//装填したマガジンの弾に合わせて薬莢生成
		}
		void				GunClass::UpdateReticle() noexcept {
			auto LensPos = GetFrameWorldMat(GunFrame::Lens).pos();
			ReticleControl::UpdateReticleControl(
				LensPos,
				GetFrameWorldMat(GunFrame::LensSize).pos(),
				LensPos + (LensPos - GetEyePos()).Norm()*(5.f*Scale_Rate));
		}

		void				GunClass::Init(void) noexcept {
			ObjectBaseClass::Init();
			//データ
			InitModSlotControl(this->m_FilePath, false);
		}
		void				GunClass::Init_Gun(void) noexcept {
			{
				std::vector<const SharedObj*> PartsList;
				ModSlotControl::GetChildPartsList(&PartsList);
				m_SightPtr[0] = nullptr;
				m_SightPtr[1] = nullptr;
				m_MuzzlePtr = nullptr;
				m_UpperPtr = nullptr;
				m_LowerPtr = nullptr;
				m_MagazinePtr = nullptr;
				m_ShootRate_Diff = 0;
				m_ReloadRate_Diff = 0;

				int SightSel = 0;
				for (auto& p : PartsList) {
					if ((*p)->GetobjType() == ObjType::Sight) {
						if (SightSel < m_SightPtr.size()) {
							const auto* Ptr = &((std::shared_ptr<SightClass>&)(*p));
							m_SightPtr.at(SightSel) = Ptr;
							SightSel++;
						}
					}
					if ((*p)->GetobjType() == ObjType::MuzzleAdapter) {
						m_MuzzlePtr = &((std::shared_ptr<MuzzleClass>&)(*p));
					}
					if ((*p)->GetobjType() == ObjType::Upper) {
						m_UpperPtr = &((std::shared_ptr<UpperClass>&)(*p));
					}
					if ((*p)->GetobjType() == ObjType::Lower) {
						m_LowerPtr = &((std::shared_ptr<LowerClass>&)(*p));
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
				//2つ以上サイトがあるときアイアンサイトを省く
				if (SightSel >= m_SightPtr.size()) {
					for (int i = 0;i < m_SightPtr.size();i++) {
						if (m_SightPtr.at(i)) {
							if ((*m_SightPtr.at(i))->GetModData()->GetIronSight()) {
								for (int k = i;k < m_SightPtr.size() - 1;k++) {
									m_SightPtr.at(k) = m_SightPtr.at(k + 1);
								}
								m_SightPtr.back() = nullptr;
							}
						}
					}
				}
				PartsList.clear();
			}
			SetAmmo(GetAmmoAll());								//マガジン装填
			CockByMag();										//チャンバーイン
			SetGunAnime(GunAnimeID::Base);
		}
		void				GunClass::FirstExecute(void) noexcept {
			if (this->m_IsFirstLoop) {
				InitMuzzleSmoke(GetFrameWorldMat(GunFrame::Muzzle).pos());
			}
			else {
				ExecuteMuzzleSmoke(GetFrameWorldMat(GunFrame::Muzzle).pos());
			}
			//
			for (int i = 0; i < GetObj().get_anime().size(); i++) {
				if (GetGunAnime() == (GunAnimeID)i) {
					GetObj().get_anime(i).per = 1.f;
					if (GetGunAnime() == GunAnimeID::Watch) {
						GetObj().get_anime(i).per = 0.f;
					}
					if (GetGunAnime() == GunAnimeID::Melee) {
						GetObj().get_anime(i).per = 0.f;
					}
				}
				else {
					if ((GunAnimeID)i == GunAnimeID::Base) {
						GetObj().get_anime(i).per = 1.f;
					}
					else if ((GunAnimeID)i == GunAnimeID::OffsetAnim) {
						bool isHit = false;
						if (m_GunSightSel == 1) {
							isHit = true;
						}
						GetObj().get_anime(i).per = std::clamp(GetObj().get_anime(i).per + 1.f / FPS * (isHit ? 1.f : -1.f), 0.f, 1.f);
					}
					else if ((GunAnimeID)i == GunAnimeID::Open) {
						bool isHit = false;
						if (GetAmmoNumTotal() == 0) {
							isHit = true;
						}
						else {
							if (GetGunAnime() == GunAnimeID::Cocking) {
								if (GetObj().get_anime((int)GetGunAnime()).time <= 22.f) {
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
			//
			ExecuteSound();
			//
			{
				ResetFrameLocalMat(GunFrame::Center);
				GetObj().work_anime();
				SetFrameLocalMat(GunFrame::Center, GetFrameLocalMat(GunFrame::Center).GetRot());//1のフレーム移動量を無視する
			}
			//
			Easing(&m_GunChangePer, 1.f, 0.8f, EasingType::OutExpo);
			//弾薬の演算
			ExecuteCartInChamber();
		}
		void				GunClass::DrawShadow(void) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				auto* DrawParts = DXDraw::Instance();
				if ((GetMove().pos - DrawParts->GetMainCamera().GetCamPos()).Length() > 10.f*Scale_Rate) { return; }
				this->GetObj().DrawModel();
			}
		}
		void				GunClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-0.5f*Scale_Rate, -0.5f*Scale_Rate, -0.5f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(0.5f*Scale_Rate, 0.5f*Scale_Rate, 0.5f*Scale_Rate)).get()) == FALSE
					) {
					if (isDrawSemiTrans) {
#if HIGH_FPS_ROM
#else
						if (m_MyID == 0) {
							DrawMuzzleSmoke();
						}
#endif
					}
					for (int i = 0; i < this->GetObj().mesh_num(); i++) {
						if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
							this->GetObj().DrawMesh(i);
						}
					}
				}
			}
		}
		void				GunClass::Dispose(void) noexcept {
			ObjectBaseClass::Dispose();
			DisposeModSlotControl();
		}
	};
};
