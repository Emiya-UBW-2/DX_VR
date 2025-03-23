#include	"Gun.hpp"

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
					this->m_IsChamberOn |= ((GetGunAnime() == GunAnimeID::Shot) && (GetObj().SetAnim(static_cast<size_t>(GetGunAnime())).GetTime() >= 3.f));
					this->m_IsChamberOn |= ((GetGunAnime() == GunAnimeID::Cocking) && (GetObj().SetAnim(static_cast<size_t>(GetGunAnime())).GetTime() >= 25.f));
					break;
				case SHOTTYPE::BOLT:
					this->m_IsChamberOn = false;
					this->m_IsChamberOn |= ((GetGunAnime() == GunAnimeID::Cocking) && (GetObj().SetAnim(static_cast<size_t>(GetGunAnime())).GetTime() >= 25.f));
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
					this->m_IsEject |= ((GetGunAnime() == GunAnimeID::ReloadStart_Empty || GetGunAnime() == GunAnimeID::ReloadStart) && (GetObj().SetAnim(static_cast<size_t>(GetGunAnime())).GetTime() >= 19.f && !GetObj().SetAnim(static_cast<size_t>(GetGunAnime())).TimeEnd()));
					this->m_IsEject |= ((GetGunAnime() == GunAnimeID::Cocking) && (GetObj().SetAnim(static_cast<size_t>(GetGunAnime())).GetTime() >= 19.f && !GetObj().SetAnim(static_cast<size_t>(GetGunAnime())).TimeEnd()));
					break;
				default:
					break;
				}
				if (this->m_IsEject && (this->m_IsEject != Prev)) {
					m_CartFall.SetFall(
						GetFrameWorldMat_P(GunFrame::Cart).pos(),
						Matrix3x3DX::Get33DX(GetFrameWorldMat_P(GunFrame::Muzzle).rotation()),
						(GetFrameWorldMat_P(GunFrame::CartVec).pos() - GetFrameWorldMat_P(GunFrame::Cart).pos() + Vector3DX::vget(GetRandf(0.2f), 0.5f + GetRandf(1.f), GetRandf(0.2f))).normalized() * (Scale3DRate * 2.f / 60.f), 2.f, SoundEnum::CartFall);
				}
			}
		}
		void				GunClass::ExecuteSound(void) noexcept {
			auto SE = SoundPool::Instance();
			switch (GetShotType()) {
			case SHOTTYPE::BOLT:
				//0
			{
				if ((5.f < GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() && GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() < 6.f)) {
					if (m_boltSoundSequence != 1) {
						m_boltSoundSequence = 1;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
				if ((28.f < GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() && GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() < 29.f)) {
					if (m_boltSoundSequence != 3) {
						m_boltSoundSequence = 3;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
			}
			//1
			{
				if ((5.f < GetObj().SetAnim((int)GunAnimeID::ReloadStart_Empty).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadStart_Empty).GetTime() < 6.f)) {
					if (m_boltSoundSequence != 5) {
						m_boltSoundSequence = 5;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}

				if ((5.f < GetObj().SetAnim((int)GunAnimeID::ReloadStart).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadStart).GetTime() < 6.f)) {
					if (m_boltSoundSequence != 5) {
						m_boltSoundSequence = 5;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
			}
			//2
			{
				if ((10.f < GetObj().SetAnim((int)GunAnimeID::ReloadOne).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadOne).GetTime() < 11.f)) {
					if (m_boltSoundSequence != 7) {
						m_boltSoundSequence = 7;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
				else if (GetObj().SetAnim((int)GunAnimeID::ReloadOne).GetPer() != 0.f) {
					m_boltSoundSequence = -1;
				}
			}
			//3
			{
				if ((8.f < GetObj().SetAnim((int)GunAnimeID::ReloadEnd).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadEnd).GetTime() < 9.f)) {
					if (m_boltSoundSequence != 8) {
						m_boltSoundSequence = 8;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
			}
			//4
			{
				if ((0.5f < GetObj().SetAnim((int)GunAnimeID::Shot).GetTime() && GetObj().SetAnim((int)GunAnimeID::Shot).GetTime() < 1.f)) {
					if (m_boltSoundSequence != 1) {
						m_boltSoundSequence = 1;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
				if ((5.f < GetObj().SetAnim((int)GunAnimeID::Shot).GetTime() && GetObj().SetAnim((int)GunAnimeID::Shot).GetTime() < 6.f)) {
					if (m_boltSoundSequence != 3) {
						m_boltSoundSequence = 3;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
			}
			break;
			case SHOTTYPE::FULL:
			case SHOTTYPE::SEMI:
				//0
			{
				if ((8.f < GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() && GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() < 9.f)) {
					if (m_boltSoundSequence != 1) {
						m_boltSoundSequence = 1;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
					}
				}
				if ((18.f < GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() && GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() < 19.f)) {
					if (m_boltSoundSequence != 2) {
						m_boltSoundSequence = 2;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
					}
				}
			}
			//1
			{
				if ((0.f < GetObj().SetAnim((int)GunAnimeID::ReloadStart_Empty).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadStart_Empty).GetTime() < 1.f)) {
					if (m_boltSoundSequence != 3) {
						m_boltSoundSequence = 3;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
					}
				}
				if ((0.f < GetObj().SetAnim((int)GunAnimeID::ReloadStart).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadStart).GetTime() < 1.f)) {
					if (m_boltSoundSequence != 3) {
						m_boltSoundSequence = 3;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
					}
				}
			}
			//3
			{
				if ((0.f < GetObj().SetAnim((int)GunAnimeID::ReloadEnd).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadEnd).GetTime() < 1.f)) {
					if (m_boltSoundSequence != 5) {
						m_boltSoundSequence = 5;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Load)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
					}
				}
			}
			//6
			{
				if ((0.f < GetObj().SetAnim((int)GunAnimeID::CheckStart).GetTime() && GetObj().SetAnim((int)GunAnimeID::CheckStart).GetTime() < 1.f)) {
					if (m_boltSoundSequence != 6) {
						m_boltSoundSequence = 6;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
					}
				}
			}
			//7
			{
				if ((25.f < GetObj().SetAnim((int)GunAnimeID::Checking).GetTime() && GetObj().SetAnim((int)GunAnimeID::Checking).GetTime() < 26.f)) {
					if (m_boltSoundSequence != 7) {
						m_boltSoundSequence = 7;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Load)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
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
			if (HaveFrame((int)frame)) {
				return true;
			}
			return false;
		}
		const Matrix4x4DX	GunClass::GetFrameWorldMat_P(GunFrame frame, bool CheckSight) const noexcept {
			//該当フレームがあるのなら上書き
			if (m_SightPtr.at(m_GunSightSel) && CheckSight) {
				switch (frame) {
				case GunFrame::Eyepos:
				case GunFrame::Lens:
				case GunFrame::LensSize:
					return (*m_SightPtr.at(m_GunSightSel))->GetFrameWorldMat_P(frame);
				default:
					break;
				}
			}
			Matrix4x4DX Ret;
			if (ModSlotControl::GetPartsFrameWorldMat(frame, &Ret)) {
				return Ret;
			}
			if (HaveFrame((int)frame)) {
				return GetFrameWorldMat(frame);
			}
			return Matrix4x4DX::identity();
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
			SE->Get(SoundType::SE, (int)SoundEnum::Trigger)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 5.f);
			switch (GetGunShootSound()) {
			case GunShootSound::Normal:
				SE->Get(SoundType::SE, (int)GetGunSoundSet().m_ShotNormal)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
				break;
			case GunShootSound::Suppressor:
				SE->Get(SoundType::SE, (int)GetGunSoundSet().m_ShotSuppressor)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
				break;
			default:
				break;
			}
			for (int i = 0; i < this->m_ChamberAmmoData->GetPellet(); i++) {
				auto LastAmmo = std::make_shared<AmmoClass>();
				ObjMngr->AddObject(LastAmmo);
				LastAmmo->Init();
				auto mat =
					Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) *
					Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) *
					GetFrameWorldMat_P(GunFrame::Muzzle);
				LastAmmo->Put(this->m_ChamberAmmoData, mat.pos(), mat.zvec() * -1.f, m_MyID);
			}
			UnloadChamber();
			AddMuzzleSmokePower();
			SetGunAnime(GunAnimeID::Shot);
			this->m_ShotSwitch = true;
		}
		void				GunClass::UpdateGunAnims() noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto prev = this->m_GunAnimeFirst;
			switch (GetGunAnime()) {
			case GunAnimeID::Base:
				break;
			case GunAnimeID::Shot:
				SetAnimOnce((int)GetGunAnime(), ((float)this->GetModData()->GetShotRate()) / 300.f);
				break;
			case GunAnimeID::Cocking:
				SetAnimOnce((int)GetGunAnime(), 1.5f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
					SetGunAnime(GunAnimeID::Base);
				}
				break;
			case GunAnimeID::ReloadStart_Empty:
				if (this->m_GunAnimeFirst) {
					m_UpperAnim = 0.f;
					this->m_Cancel = false;
				}
				SetAnimOnce((int)GetGunAnime(), 1.0f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
					SetGunAnime(GunAnimeID::ReloadOne);
				}
				break;
			case GunAnimeID::ReloadStart:
				if (this->m_GunAnimeFirst) {
					m_UpperAnim = 0.f;
					this->m_Cancel = false;
				}
				SetAnimOnce((int)GetGunAnime(), 1.0f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
					SetGunAnime(GunAnimeID::ReloadOne);
				}
				if ((this->GetModData()->GetReloadType() == RELOADTYPE::MAG) && this->m_Cancel) {
					this->m_Cancel = false;
					SetGunAnime(GunAnimeID::Base);
				}
				break;
			case GunAnimeID::ReloadOne:
				if (this->m_GunAnimeFirst) {
					m_UpperAnim = 0.f;
				}
				SetAnimOnce((int)GetGunAnime(), 1.0f);

				if ((this->GetModData()->GetReloadType() == RELOADTYPE::MAG) && this->m_Cancel) {
					this->m_Cancel = false;
					SetGunAnime(GunAnimeID::Base);
				}
				if (m_isMagSuccess ? (GetGunAnimePer(EnumGunAnimType::Reload) >= 0.75f) : (GetGunAnimePer(EnumGunAnimType::Reload) >= 1.f)) {
					switch (this->GetModData()->GetReloadType()) {
					case RELOADTYPE::MAG:
						//ユニークIDが異なる場合登録するオブジェと切り替える(現状不要なのでいったんオフ)
						//if (this->m_NextMagUniqueID != (*m_MagazinePtr)->GetModData()->GetUniqueID()) {
						//	m_MagazinePtr = &((std::shared_ptr<MagazineClass>&)(ModSlotControl::SetMod(GunSlot::Magazine, this->m_NextMagUniqueID, this->m_obj)));
						//}
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
							this->GetObj().SetAnim((int)GetGunAnime()).Reset();
							this->GetObj().SetAnim((int)GetGunAnime()).SetPer(1.f);
							m_UpperAnim = 0.f;
						}
						break;
					default:
						break;
					}
				}
				break;
			case GunAnimeID::ReloadEnd:
				SetAnimOnce((int)GetGunAnime(), 1.0f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
					SetGunAnime(GunAnimeID::Checking);
				}
				break;
			case GunAnimeID::CheckStart:
				SetAnimOnce((int)GetGunAnime(), 1.5f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
					SetGunAnime(GunAnimeID::Checking);
				}
				break;
			case GunAnimeID::Checking:
				SetAnimOnce((int)GetGunAnime(), 1.f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
					SetGunAnime(GunAnimeID::CheckEnd);
				}
				break;
			case GunAnimeID::CheckEnd:
				SetAnimOnce((int)GetGunAnime(), 2.f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
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
					this->m_Cancel = false;
				}
				SetAnimOnce((int)GetGunAnime(), 1.0f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
					if (this->m_Cancel) {
						this->m_Cancel = false;
						SetGunAnime(GunAnimeID::AmmoLoadEnd);
					}
					else {
						this->GetObj().SetAnim((int)GetGunAnime()).Reset();
						this->GetObj().SetAnim((int)GetGunAnime()).SetPer(1.f);
						this->m_boltSoundSequence = -1;
					}
				}
				//サウンド
				if (GetObj().SetAnim((int)GetGunAnime()).GetTimePer() > 0.3f) {
					if (m_boltSoundSequence != 9) {
						m_boltSoundSequence = 9;
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, (int)SoundEnum::AmmoLoad)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
				break;
			case GunAnimeID::AmmoLoadEnd:
				SetAnimOnce((int)GetGunAnime(), 1.5f);
				break;
			case GunAnimeID::OffsetAnim:
				break;
			default:
				break;
			}
			//
			m_UpperAnim += 60.f * DXLib_refParts->GetDeltaTime();
			if (prev) {
				this->m_GunAnimeFirst = false;
			}
		}

		void				GunClass::SetFallObject() noexcept {
			m_MagFall.Dispose();
			m_CartFall.Dispose();
			m_MagFall.Init((*m_MagazinePtr)->GetFilePath(), 1);
			m_CartFall.Init((*m_MagazinePtr)->GetModData()->GetAmmoSpecMagTop()->GetPath(), 4);	//装填したマガジンの弾に合わせて薬莢生成
		}
		void				GunClass::UpdateReticle() noexcept {
			AimPointControl::UpdateAimPointControl(GetEyePos() + GetFrameWorldMat_P(GunFrame::Muzzle).zvec() * -100.f);
			auto LensPos = GetFrameWorldMat_P(GunFrame::Lens).pos();
			ReticleControl::UpdateReticleControl(
				LensPos,
				GetFrameWorldMat_P(GunFrame::LensSize).pos(),
				LensPos + (LensPos - GetEyePos()).normalized() * (5.f * Scale3DRate));
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

				int SightSel = 0;
				for (auto& p : PartsList) {
					if ((*p)->GetobjType() == (int)ObjType::Sight) {
						if (SightSel < m_SightPtr.size()) {
							const auto* Ptr = &((std::shared_ptr<SightClass>&)(*p));
							m_SightPtr.at(SightSel) = Ptr;
							SightSel++;
						}
					}
					if ((*p)->GetobjType() == (int)ObjType::MuzzleAdapter) {
						m_MuzzlePtr = &((std::shared_ptr<MuzzleClass>&)(*p));
					}
					if ((*p)->GetobjType() == (int)ObjType::Upper) {
						m_UpperPtr = &((std::shared_ptr<UpperClass>&)(*p));
					}
					if ((*p)->GetobjType() == (int)ObjType::Lower) {
						m_LowerPtr = &((std::shared_ptr<LowerClass>&)(*p));
					}
					if ((*p)->GetobjType() == (int)ObjType::Magazine) {
						m_MagazinePtr = &((std::shared_ptr<MagazineClass>&)(*p));
					}
				}

				m_ShootRate_Diff = 0;
				m_Recoil_Diff = 0;
				for (auto& p : PartsList) {
					//性能周り
					auto& d = ((std::shared_ptr<ModClass>&)(*p))->GetModData();
					m_ShootRate_Diff += d->GetShootRate_Diff();
					m_Recoil_Diff += d->GetRecoil_Diff();
				}

				//2つ以上サイトがあるときアイアンサイトを省く
				if (SightSel >= m_SightPtr.size()) {
					for (int i = 0; i < m_SightPtr.size(); i++) {
						if (m_SightPtr.at(i)) {
							if ((*m_SightPtr.at(i))->GetModData()->GetIronSight()) {
								for (int k = i; k < m_SightPtr.size() - 1; k++) {
									m_SightPtr.at(k) = m_SightPtr.at(static_cast<size_t>(k + 1));
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
			auto* DXLib_refParts = DXLib_ref::Instance();
			if (this->m_IsFirstLoop) {
				InitMuzzleSmoke(GetFrameWorldMat_P(GunFrame::Muzzle).pos());
			}
			else {
				ExecuteMuzzleSmoke(GetFrameWorldMat_P(GunFrame::Muzzle).pos(), GetGunAnime() != GunAnimeID::Shot);
			}
			//
			for (int i = 0; i < GetObj().GetAnimNum(); i++) {
				if (GetGunAnime() == (GunAnimeID)i) {
					GetObj().SetAnim(i).SetPer(1.f);
					if (GetGunAnime() == GunAnimeID::Watch) {
						GetObj().SetAnim(i).SetPer(0.f);
					}
					if (GetGunAnime() == GunAnimeID::Melee) {
						GetObj().SetAnim(i).SetPer(0.f);
					}
				}
				else {
					if ((GunAnimeID)i == GunAnimeID::Base) {
						bool isHit = true;
						if ((GetGunAnime() == GunAnimeID::Shot)) {
							if (GetObj().SetAnim((int)GetGunAnime()).GetTimePer() < 0.7f) {
								isHit = false;
							}
							isHit = false;
						}
						GetObj().SetAnim(i).SetPer(std::clamp(GetObj().SetAnim(i).GetPer() + DXLib_refParts->GetDeltaTime() * (isHit ? 5.f : -5.f), 0.f, 1.f));
					}
					else if ((GunAnimeID)i == GunAnimeID::OffsetAnim) {
						bool isHit = false;
						if (m_GunSightSel == 1) {
							isHit = true;
						}
						GetObj().SetAnim(i).SetPer(std::clamp(GetObj().SetAnim(i).GetPer() + DXLib_refParts->GetDeltaTime() * (isHit ? 1.f : -1.f), 0.f, 1.f));
					}
					else if ((GunAnimeID)i == GunAnimeID::Open) {
						bool isHit = false;
						if (GetAmmoNumTotal() == 0) {
							isHit = true;
						}
						else {
							if (GetGunAnime() == GunAnimeID::Cocking) {
								if (GetObj().SetAnim((int)GetGunAnime()).GetTime() <= 22.f) {
									isHit = true;
								}
							}
							else if (GetIsMagFull() && !GetInChamber()) {
								isHit = true;
							}
						}
						GetObj().SetAnim(i).SetPer(std::clamp(GetObj().SetAnim(i).GetPer() + 10.f * DXLib_refParts->GetDeltaTime() * (isHit ? 1.f : -1.f), 0.f, 1.f));
					}
					else {
						GetObj().SetAnim(i).Reset();
					}
				}
			}
			//
			ExecuteSound();
			//
			{
				//1のフレーム移動量を無視する
				ModSlotControl::ResetPartsFrameLocalMat(GunFrame::Center);
				if (HaveFrame((int)GunFrame::Center)) {
					GetObj().ResetFrameUserLocalMatrix(GetFrame((int)GunFrame::Center));
				}
				GetObj().UpdateAnimAll();
				auto Rot = GetFrameLocalMat(GunFrame::Center).rotation();
				ModSlotControl::SetPartsFrameLocalMat(GunFrame::Center, Rot);
				if (HaveFrame((int)GunFrame::Center)) {
					GetObj().SetFrameLocalMatrix(GetFrame((int)GunFrame::Center), Rot * GetFrameBaseLocalMat((int)GunFrame::Center));
				}
			}
			//
			Easing(&m_GunChangePer, 1.f, 0.8f, EasingType::OutExpo);
			//弾薬の演算
			ExecuteCartInChamber();
		}
		void				GunClass::DrawShadow(void) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				auto* CameraParts = Camera3D::Instance();
				if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }
				this->GetObj().DrawModel();
			}
		}
		void				GunClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-0.5f * Scale3DRate, -0.5f * Scale3DRate, -0.5f * Scale3DRate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(0.5f * Scale3DRate, 0.5f * Scale3DRate, 0.5f * Scale3DRate)).get()) == FALSE
					) {
					if (isDrawSemiTrans) {
#if HIGH_FPS_ROM
#else
						if (m_MyID == 0) {
							DrawMuzzleSmoke();
						}
#endif
					}
					for (int i = 0; i < this->GetObj().GetMeshNum(); i++) {
						if (this->GetObj().GetMeshSemiTransState(i) == isDrawSemiTrans) {
							this->GetObj().DrawMesh(i);
						}
					}
				}
			}
		}
	};
};
