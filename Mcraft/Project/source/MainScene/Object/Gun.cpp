#include	"Gun.hpp"

#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool			GunClass::HasFrame(GunFrame frame) const noexcept {
			//該当フレームがあるのなら上書き
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
			if (m_ModSlotControl.HasFrameBySlot(frame)) {
				return true;
			}
			if (HaveFrame((int)frame)) {
				return true;
			}
			return false;
		}
		const Matrix4x4DX	GunClass::GetFrameWorldMat_P(GunFrame frame, bool CheckSight) const noexcept {
			//該当フレームがあるのなら上書き
			if (m_SightPtr && CheckSight) {
				switch (frame) {
				case GunFrame::Eyepos:
				case GunFrame::Lens:
				case GunFrame::LensSize:
					return (*m_SightPtr)->GetFrameWorldMat_P(frame);
				default:
					break;
				}
			}
			Matrix4x4DX Ret;
			if (m_ModSlotControl.GetPartsFrameWorldMat(frame, &Ret)) {
				return Ret;
			}
			if (HaveFrame((int)frame)) {
				return GetFrameWorldMat(frame);
			}
			return Matrix4x4DX::identity();
		}
		void				GunClass::SetActiveAll(bool value) noexcept {
			if (value != this->IsActive()) {
				GetModSlot().SetActiveBySlot(value);
				this->SetActive(value);
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
			for (int i = 0, max = GetPelletNum(); i < max; i++) {
				auto LastAmmo = std::make_shared<AmmoClass>();
				ObjMngr->AddObject(LastAmmo);
				LastAmmo->Init();
				auto MuzzleMat = GetFrameWorldMat_P(GunFrame::Muzzle);
				auto mat =
					Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) *
					Matrix3x3DX::Get33DX(MuzzleMat);
				LastAmmo->Put(this->m_ChamberAmmoData, MuzzleMat.pos(), mat.zvec() * -1.f, GetMyUserPlayerID());
			}
			UnloadChamber();
			m_MuzzleSmokeControl.AddMuzzleSmokePower();
			this->m_ShotSwitch = true;
		}
		void				GunClass::UpdateGunAnims() noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			switch (GetGunAnime()) {
			case GunAnimeID::Shot:
				SetAnimOnce(GetGunAnimeID(GetGunAnime()), ((float)GetModSlot().GetModData()->GetShotRate()) / 300.f);
				if (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTimePer() >= 1.f && m_ShotEnd) {
					m_ShotEnd = false;
					if (!GetIsMagEmpty()) {
						switch (GetShotType()) {
						case SHOTTYPE::FULL:
						case SHOTTYPE::SEMI:
							SetGunAnime(GunAnimeID::None);
							break;
						case SHOTTYPE::BOLT:
							SetGunAnime(GunAnimeID::Cocking);
							break;
						default:
							break;
						}
					}
					else {
						SetGunAnime(GunAnimeID::None);
					}
				}
				break;
			case GunAnimeID::Cocking:
				SetAnimOnce(GetGunAnimeID(GetGunAnime()), 1.f);
				if (GetGunAnimePer(GetGunAnime()) >= 1.f) {
					SetGunAnime(GunAnimeID::None);
				}
				break;
			case GunAnimeID::ReloadStart_Empty:
				if (GetGunAnimePer(GetGunAnime()) >= 1.f) {
					SetGunAnime(GunAnimeID::Reload);
				}
				break;
			case GunAnimeID::ReloadStart:
				if (GetGunAnimePer(GetGunAnime()) >= 1.f) {
					SetGunAnime(GunAnimeID::Reload);
				}
				break;
			case GunAnimeID::Reload:
				if (this->m_isMagSuccess ? (GetGunAnimePer(GetGunAnime()) >= 0.75f) : (GetGunAnimePer(GetGunAnime()) >= 1.f)) {
					switch (GetReloadType()) {
					case RELOADTYPE::MAG:
						//ユニークIDが異なる場合登録するオブジェと切り替える(現状不要なのでいったんオフ)
						//if (this->m_NextMagUniqueID != GetMagazinePtr()->GetModSlot().GetModData()->GetUniqueID()) {
						//	this->m_MagazinePtr = &((std::shared_ptr<MagazineClass>&)(m_ModSlotControl.SetMod(GunSlot::Magazine, this->m_NextMagUniqueID, this->m_obj)));
						//}
						SetAmmo(this->m_NextMagNum);
						SetGunAnime(GunAnimeID::ReloadEnd);
						break;
					case RELOADTYPE::AMMO:
						SetAmmo(this->m_Capacity + 1);
						this->m_NextMagNum--;
						if ((this->GetIsMagFull() || (this->m_NextMagNum <= 0)) || this->m_ReloadAmmoCancel) {
							this->m_ReloadAmmoCancel = false;
							SetGunAnime(GunAnimeID::ReloadEnd);
						}
						else {
							SetGunAnime(GunAnimeID::Reload);
						}
						break;
					default:
						break;
					}
				}
				break;
			case GunAnimeID::ReloadEnd:
				if (GetGunAnimePer(GetGunAnime()) >= 1.f) {
					if (GetInChamber()) {
						SetGunAnime(GunAnimeID::None);
					}
					else {
						if (!GetIsMagEmpty()) {
							SetGunAnime(GunAnimeID::Cocking);
						}
						else {
							SetGunAnime(GunAnimeID::None);
						}
					}
				}
				break;
			case GunAnimeID::Watch:
				if (GetGunAnimePer(GetGunAnime()) >= 1.f) {
					SetGunAnime(GunAnimeID::None);
				}
				break;
			default:
				break;
			}
			//
			//printfDx("[%s]\n", (GetGunAnime() == GunAnimeID::None) ? "None" : GunAnimeIDName[(int)GetGunAnime()]);
			//printfDx("[%f]\n", (GetGunAnime() == GunAnimeID::None) ? 0.0f : GetGunAnimeTime(GetGunAnime()));
			for (auto& g : this->m_GunAnimeTime) {
				int index = static_cast<int>(&g - &this->m_GunAnimeTime.front());
				if (index == (int)GetGunAnime()) {
					if (m_IsGunAnimChange) {
						m_IsGunAnimChange = false;
						continue;
					}
				}
				g += 60.f * DXLib_refParts->GetDeltaTime();
			}
		}
		void				GunClass::UpdateMagazineMat(bool IsSelGun) noexcept {
			if (!this->m_MagazinePtr) { return; }

			if (!IsSelGun) {
				GetMagazinePtr()->SetHandMatrix(GetFrameWorldMat_P(GunFrame::Magpos));
				return;
			}

			Matrix4x4DX MatMin;
			Matrix4x4DX MatMax;
			float AnimPer = 0.f;
			float BasePer = 0.f;
			float MaxPer = 0.f;
			switch (GetGunAnime()) {
			case GunAnimeID::ReloadStart_Empty:
				GetMagazinePtr()->SetHandMatrix(m_MagazinePoachMat);
				break;
			case GunAnimeID::ReloadStart:
				AnimPer = GetGunAnimePer(GetGunAnime());
				{
					MatMin = GetFrameWorldMat_P(GunFrame::Magpos);
					MatMax = GetFrameWorldMat_P(GunFrame::Magpos);
					MaxPer = 0.3f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
						break;
					}
					MatMin = MatMax;
					MatMax = GetFrameWorldMat_P(GunFrame::Mag2);
					BasePer = MaxPer;
					MaxPer = 0.6f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
						break;
					}

					MatMin = MatMax;
					MatMax = m_MagazinePoachMat;
					BasePer = MaxPer;
					MaxPer = 1.f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
						break;
					}
				}
				break;
			case GunAnimeID::Reload:
				AnimPer = GetGunAnimePer(GetGunAnime());
				switch (GetReloadType()) {
				case RELOADTYPE::MAG:
				{
					MatMin = m_MagazinePoachMat;
					MatMax = m_MagazinePoachMat;
					MaxPer = 0.1f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
						//マグチェンジ成功確率
						SetMagLoadSuccess(GetRand(100) < 50);
						float MissPer = GetRandf(0.025f);
						m_MagMiss = Matrix4x4DX::Mtrans(GetMove().GetMat().xvec() * (MissPer * Scale3DRate));
						m_MagSuccess = Matrix4x4DX::Mtrans(
							GetMove().GetMat().yvec() * (-0.05f * Scale3DRate) +
							GetMove().GetMat().xvec() * (MissPer / 3.f * Scale3DRate)
						);
						break;
					}
					if (this->m_isMagSuccess) {
						MatMin = MatMax;
						MatMax = GetFrameWorldMat_P(GunFrame::Mag2);
						BasePer = MaxPer;
						MaxPer = 0.55f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
							break;
						}
						MatMin = MatMax;
						MatMax = GetFrameWorldMat_P(GunFrame::Magpos);
						BasePer = MaxPer;
						MaxPer = 0.75f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
							break;
						}
						MatMin = MatMax;
						MatMax = GetFrameWorldMat_P(GunFrame::Magpos);
						BasePer = MaxPer;
						MaxPer = 1.f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
							break;
						}
					}
					else {
						MatMin = MatMax;
						MatMax = GetFrameWorldMat_P(GunFrame::Mag2) * m_MagMiss;
						BasePer = MaxPer;
						MaxPer = 0.55f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
							break;
						}
						MatMin = MatMax;
						MatMax = GetFrameWorldMat_P(GunFrame::Mag2) * m_MagSuccess;
						BasePer = MaxPer;
						MaxPer = 0.75f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
							break;
						}
						MatMin = MatMax;
						MatMax = GetFrameWorldMat_P(GunFrame::Mag2);
						BasePer = MaxPer;
						MaxPer = 0.85f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
							break;
						}
						MatMin = MatMax;
						MatMax = GetFrameWorldMat_P(GunFrame::Magpos);
						BasePer = MaxPer;
						MaxPer = 1.0f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
							break;
						}
					}
				}
				break;
				case RELOADTYPE::AMMO:
				{
					SetMagLoadSuccess(false);
					MatMin = m_MagazinePoachMat;
					MatMax = GetFrameWorldMat_P(GunFrame::Mag2);
					MaxPer = 0.5f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
						break;
					}
					MatMin = MatMax;
					MatMax = GetFrameWorldMat_P(GunFrame::Mag2);
					BasePer = MaxPer;
					MaxPer = 0.7f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
						break;
					}
					MatMin = MatMax;
					MatMax = GetFrameWorldMat_P(GunFrame::Magpos);
					BasePer = MaxPer;
					MaxPer = 0.9f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
						break;
					}
					MatMin = MatMax;
					MatMax = GetFrameWorldMat_P(GunFrame::Magpos);
					BasePer = MaxPer;
					MaxPer = 1.f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						GetMagazinePtr()->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
						break;
					}
				}
				break;
				default:
					break;
				}
				break;
			default:
				GetMagazinePtr()->SetHandMatrix(GetFrameWorldMat_P(GunFrame::Magpos));
				break;
			}
		}

		void				GunClass::Init_Sub(void) noexcept {
			{
				std::vector<const SharedObj*> PartsList;
				m_ModSlotControl.GetChildPartsList(&PartsList);
				this->m_SightPtr = nullptr;
				this->m_MuzzlePtr = nullptr;
				this->m_UpperPtr = nullptr;
				this->m_LowerPtr = nullptr;
				this->m_MagazinePtr = nullptr;

				for (auto& p : PartsList) {
					if ((*p)->GetobjType() == (int)ObjType::Sight) {
						this->m_SightPtr = &((std::shared_ptr<SightClass>&)(*p));
					}
					if ((*p)->GetobjType() == (int)ObjType::MuzzleAdapter) {
						this->m_MuzzlePtr = &((std::shared_ptr<MuzzleClass>&)(*p));
					}
					if ((*p)->GetobjType() == (int)ObjType::Upper) {
						this->m_UpperPtr = &((std::shared_ptr<UpperClass>&)(*p));
					}
					if ((*p)->GetobjType() == (int)ObjType::Lower) {
						this->m_LowerPtr = &((std::shared_ptr<LowerClass>&)(*p));
					}
					if ((*p)->GetobjType() == (int)ObjType::Magazine) {
						this->m_MagazinePtr = &((std::shared_ptr<MagazineClass>&)(*p));
					}
				}

				this->m_ShootRate_Diff = 0;
				this->m_Recoil_Diff = 0;
				for (auto& p : PartsList) {
					//性能周り
					auto& d = ((std::shared_ptr<ModClass>&)(*p))->GetModSlot().GetModData();
					this->m_ShootRate_Diff += d->GetShootRate_Diff();
					this->m_Recoil_Diff += d->GetRecoil_Diff();
				}
				PartsList.clear();
			}
			if (this->m_MagazinePtr) {
				this->m_MagFall.Init(GetMagazinePtr()->GetFilePath(), 1);
				this->m_CartFall.Init(GetMagazinePtr()->GetModSlot().GetModData()->GetAmmoSpecMagTop()->GetPath(), 4);	//装填したマガジンの弾に合わせて薬莢生成
			}
			SetGunAnime(GunAnimeID::None);
			std::array<bool, static_cast<int>(GunAnimeID::ChoiceOnceMax)>			tmp{};
			for (auto& a : tmp) { a = false; }
			tmp[static_cast<int>(GunAnimeID::LowReady)] = true;
			UpdateGunAnime(tmp, true);
			SetAmmo(GetAmmoAll());								//マガジン装填
			CockByMag();										//チャンバーイン
		}
		void				GunClass::FirstExecute(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			if (this->m_IsFirstLoop) {
				m_MuzzleSmokeControl.InitMuzzleSmoke(GetFrameWorldMat_P(GunFrame::Muzzle).pos());
			}
			else {
				m_MuzzleSmokeControl.ExecuteMuzzleSmoke(GetFrameWorldMat_P(GunFrame::Muzzle).pos(), GetGunAnime() != GunAnimeID::Shot);
			}
			//
			for (int i = 0; i < static_cast<int>(GunAnimeID::ChoiceOnceMax); i++) {
				int ID = GetGunAnimeID((GunAnimeID)i);
				if (ID != -1) {
					if (GetGunAnime() == (GunAnimeID)i) {
						GetObj().SetAnim(ID).SetPer(1.f);
					}
					else {
						GetObj().SetAnim(ID).Reset();
					}
				}
			}
			{
				bool isHit = ((GetAmmoNumTotal() == 0) || (GetIsMagFull() && !GetInChamber()) || (GetGunAnime() == GunAnimeID::Cocking && (GetGunAnimeTime(GetGunAnime()) <= 22.f)));
				for (int i = static_cast<int>(GunAnimeID::ChoiceOnceMax); i < static_cast<int>(GunAnimeID::Max); i++) {
					int ID = GetGunAnimeID((GunAnimeID)i);
					if (ID != -1) {
						switch ((GunAnimeID)i) {
						case FPS_n2::Sceneclass::GunAnimeID::Hammer:
							GetObj().SetAnim(ID).SetPer(std::clamp(GetObj().GetAnim(ID).GetPer() + DXLib_refParts->GetDeltaTime() * ((GetGunAnime() != GunAnimeID::Shot) ? 5.f : -5.f), 0.f, 1.f));
							break;
						case FPS_n2::Sceneclass::GunAnimeID::Open:
							GetObj().SetAnim(ID).SetPer(std::clamp(GetObj().GetAnim(ID).GetPer() + DXLib_refParts->GetDeltaTime() * (isHit ? 10.f : -10.f), 0.f, 1.f));
							break;
						default:
							break;
						}
					}
				}
			}
			//
			this->m_IsChamberOn = false;
			this->m_IsEject = false;
			switch (GetGunAnime()) {
			case GunAnimeID::Shot:
				if (GetShotType() != SHOTTYPE::BOLT) {
					if (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime() >= 3.f) {
						this->m_IsChamberOn = true;
						this->m_IsEject = true;
					}
				}
				break;
			case GunAnimeID::Cocking:
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((5.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 6.f)) {
						if (this->m_boltSoundSequence != 1) {
							this->m_boltSoundSequence = 1;
							SE->Get(SoundType::SE, static_cast<int>(GetGunSoundSet().m_Cock1))->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					if ((28.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 29.f)) {
						if (this->m_boltSoundSequence != 2) {
							this->m_boltSoundSequence = 2;
							SE->Get(SoundType::SE, static_cast<int>(GetGunSoundSet().m_Cock2))->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((8.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 9.f)) {
						if (this->m_boltSoundSequence != 3) {
							this->m_boltSoundSequence = 3;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					if ((18.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 19.f)) {
						if (this->m_boltSoundSequence != 4) {
							this->m_boltSoundSequence = 4;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				if (GetGunAnimeTime(GetGunAnime()) >= 19.f) {
					this->m_IsChamberOn = true;
					if (GetShotType() == SHOTTYPE::BOLT) {
						this->m_IsEject = true;
					}
				}
				break;
			case GunAnimeID::ReloadStart_Empty:
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((5.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 6.f)) {
						if (this->m_boltSoundSequence != 5) {
							this->m_boltSoundSequence = 5;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 1.f)) {
						if (this->m_boltSoundSequence != 6) {
							this->m_boltSoundSequence = 6;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				break;
			case GunAnimeID::ReloadStart:
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((5.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 6.f)) {
						if (this->m_boltSoundSequence != 6) {
							this->m_boltSoundSequence = 6;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 1.f)) {
						if (this->m_boltSoundSequence != 6) {
							this->m_boltSoundSequence = 6;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				break;
			case GunAnimeID::Reload:
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((10.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 11.f)) {
						if (this->m_boltSoundSequence != 8) {
							this->m_boltSoundSequence = 8;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					else {
						this->m_boltSoundSequence = -1;
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					break;
				default:
					break;
				}
				break;
			case GunAnimeID::ReloadEnd:
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((8.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 9.f)) {
						if (this->m_boltSoundSequence != 9) {
							this->m_boltSoundSequence = 9;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 1.f)) {
						if (this->m_boltSoundSequence != 10) {
							this->m_boltSoundSequence = 10;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Load)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			GetObj().UpdateAnimAll();
			//弾薬の演算
			if (this->m_PrevChamberOn != this->m_IsChamberOn) {
				this->m_PrevChamberOn = this->m_IsChamberOn;
				if (this->m_IsChamberOn) {
					if (!GetIsMagEmpty()) {
						CockByMag();
					}
					SetAmmo(this->m_Capacity - 1);
				}
			}
			if (this->m_PrevEject != this->m_IsEject) {
				this->m_PrevEject = this->m_IsEject;
				if (this->m_IsEject) {
					this->m_CartFall.SetFall(
						GetFrameWorldMat_P(GunFrame::Cart).pos(),
						Matrix3x3DX::Get33DX(GetFrameWorldMat_P(GunFrame::Muzzle)),
						(GetFrameWorldMat_P(GunFrame::CartVec).pos() - GetFrameWorldMat_P(GunFrame::Cart).pos() + Vector3DX::vget(GetRandf(0.2f), 0.5f + GetRandf(1.f), GetRandf(0.2f))).normalized() * (Scale3DRate * 2.f / 60.f), 2.f, SoundEnum::CartFall, false);
				}
			}
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
					if (isDrawSemiTrans && GetMyUserPlayerID() == 0) {
						m_MuzzleSmokeControl.DrawMuzzleSmoke();
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
