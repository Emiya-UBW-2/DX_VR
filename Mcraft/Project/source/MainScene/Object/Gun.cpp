#include	"Gun.hpp"

#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool			GunClass::HasFrame(GunFrame frame) const noexcept {
			//該当フレームがあるのなら上書き
			if (this->m_SightPtr.at(this->m_GunSightSel)) {
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
			if (this->m_SightPtr.at(this->m_GunSightSel) && CheckSight) {
				switch (frame) {
				case GunFrame::Eyepos:
				case GunFrame::Lens:
				case GunFrame::LensSize:
					return (*this->m_SightPtr.at(this->m_GunSightSel))->GetFrameWorldMat_P(frame);
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
				SetAnimOnce(GetGunAnimeID(GetGunAnime()), 1.5f);
				if (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTimePer() >= 1.f) {
					SetGunAnime(GunAnimeID::None);
				}
				break;
			case GunAnimeID::ReloadStart_Empty:
				SetAnimOnce(GetGunAnimeID(GetGunAnime()), 1.0f);
				if (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTimePer() >= 1.f) {
					SetGunAnime(GunAnimeID::Reload);
				}
				break;
			case GunAnimeID::ReloadStart:
				SetAnimOnce(GetGunAnimeID(GetGunAnime()), 1.0f);
				if (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTimePer() >= 1.f) {
					SetGunAnime(GunAnimeID::Reload);
				}
				break;
			case GunAnimeID::Reload:
				SetAnimOnce(GetGunAnimeID(GetGunAnime()), 1.0f);
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
							this->GetObj().SetAnim(GetGunAnimeID(GetGunAnime())).Reset();
							this->GetObj().SetAnim(GetGunAnimeID(GetGunAnime())).SetPer(1.f);
							this->m_GunAnimeTime = 0.f;
						}
						break;
					default:
						break;
					}
				}
				break;
			case GunAnimeID::ReloadEnd:
				SetAnimOnce(GetGunAnimeID(GetGunAnime()), 1.0f);
				if (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTimePer() >= 1.f) {
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
			this->m_GunAnimeTime += DXLib_refParts->GetDeltaTime();
		}
		void				GunClass::SetupSpawn(const std::array<bool, static_cast<int>(GunAnimeID::ChoiceOnceMax)>& Array) noexcept {
			this->m_MagFall.Dispose();
			this->m_CartFall.Dispose();
			if (this->m_MagazinePtr) {
				this->m_MagFall.Init(GetMagazinePtr()->GetFilePath(), 1);
				this->m_CartFall.Init(GetMagazinePtr()->GetModSlot().GetModData()->GetAmmoSpecMagTop()->GetPath(), 4);	//装填したマガジンの弾に合わせて薬莢生成
			}
			SetGunAnime(GunAnimeID::None);
			UpdateGunAnime(Array, true);
		}
		void				GunClass::UpdateReticle() noexcept {
			m_AimPointControl.UpdateAimPointControl(GetEyePos() + GetFrameWorldMat_P(GunFrame::Muzzle).zvec() * -100.f);
			auto LensPos = GetFrameWorldMat_P(GunFrame::Lens).pos();
			m_ReticleControl.UpdateReticleControl(LensPos, GetFrameWorldMat_P(GunFrame::LensSize).pos(), LensPos + (LensPos - GetEyePos()).normalized() * (5.f * Scale3DRate));
			//レンズ表示機能への反映
			auto* PostPassParts = PostPassEffect::Instance();
			float Zoom = (GetSightPtr()) ? (*GetSightPtr())->GetModSlot().GetModData()->GetZoomSize() : 1.f;
			bool IsActive = m_ReticleControl.IsActiveReticle() && Zoom > 1.f;
			PostPassParts->Set_is_lens(IsActive);
			if (IsActive) {
				PostPassParts->Set_zoom_lens(std::max(1.f, Zoom / 2.f));
				PostPassParts->Set_xp_lens(m_ReticleControl.GetLensXPos());
				PostPassParts->Set_yp_lens(m_ReticleControl.GetLensYPos());
				PostPassParts->Set_size_lens(m_ReticleControl.GetLensSize());
			}
		}
		void				GunClass::UpdateMagazineMat(bool IsSelGun, const Matrix4x4DX& MagPoachMat) noexcept {
			if (!this->m_MagazinePtr) { return; }

			if (!IsSelGun) {
				GetMagazinePtr()->SetHandMatrix(GetFrameWorldMat_P(GunFrame::Magpos));
				return;
			}

			float AnimPer = 0.f;
			float BasePer = 0.f;
			float MaxPer = 0.f;
			switch (GetGunAnime()) {
			case GunAnimeID::ReloadStart_Empty:
				AnimPer = GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTimePer();
				GetMagazinePtr()->SetHandMatrix(MagPoachMat);
				break;
			case GunAnimeID::ReloadStart:
				AnimPer = GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTimePer();
				{
					MaxPer = 0.3f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						GetMagazinePtr()->SetHandMatrix(GetFrameWorldMat_P(GunFrame::Magpos));
					}
					BasePer = MaxPer;
					MaxPer = 0.6f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
						GetMagazinePtr()->SetHandMatrix(Lerp(GetFrameWorldMat_P(GunFrame::Magpos), GetFrameWorldMat_P(GunFrame::Mag2), Per));
					}

					BasePer = MaxPer;
					MaxPer = 1.f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
						GetMagazinePtr()->SetHandMatrix(Lerp(GetFrameWorldMat_P(GunFrame::Mag2), MagPoachMat, Per));
					}
				}
				break;
			case GunAnimeID::Reload:
				AnimPer = GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTimePer();
				switch (GetReloadType()) {
				case RELOADTYPE::MAG:
				{
					if (AnimPer <= 0.1f) {
						float MissPer = GetRandf(0.025f);
						SetMagLoadSuccess(GetRand(100) < 50);
						m_MagMiss = Matrix4x4DX::Mtrans(GetMove().GetMat().xvec() * (MissPer * Scale3DRate));
						m_MagSuccess = Matrix4x4DX::Mtrans(
							GetMove().GetMat().yvec() * (-0.05f * Scale3DRate) +
							GetMove().GetMat().xvec() * (MissPer / 3.f * Scale3DRate)
						);
					}
					if (this->m_isMagSuccess) {
						MaxPer = 0.55f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							float Per = GetGunAnimBlendPer(GetGunAnime());
							GetMagazinePtr()->SetHandMatrix(Lerp(MagPoachMat, GetFrameWorldMat_P(GunFrame::Mag2), Per));
						}
						BasePer = MaxPer;
						MaxPer = 0.75f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
							GetMagazinePtr()->SetHandMatrix(Lerp(GetFrameWorldMat_P(GunFrame::Mag2), GetFrameWorldMat_P(GunFrame::Magpos), Per));
						}
					}
					else {
						MaxPer = 0.55f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							float Per = GetGunAnimBlendPer(GetGunAnime());
							GetMagazinePtr()->SetHandMatrix(Lerp(MagPoachMat, GetFrameWorldMat_P(GunFrame::Mag2) * m_MagMiss, Per));
						}
						BasePer = MaxPer;
						MaxPer = 0.75f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
							GetMagazinePtr()->SetHandMatrix(Lerp(GetFrameWorldMat_P(GunFrame::Mag2) * m_MagMiss, GetFrameWorldMat_P(GunFrame::Mag2) * m_MagSuccess, Per));
						}
						BasePer = MaxPer;
						MaxPer = 0.85f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
							GetMagazinePtr()->SetHandMatrix(Lerp(GetFrameWorldMat_P(GunFrame::Mag2) * m_MagSuccess, GetFrameWorldMat_P(GunFrame::Mag2), Per));
						}
						BasePer = MaxPer;
						MaxPer = 1.0f;
						if (BasePer <= AnimPer && AnimPer <= MaxPer) {
							float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
							GetMagazinePtr()->SetHandMatrix(Lerp(GetFrameWorldMat_P(GunFrame::Mag2), GetFrameWorldMat_P(GunFrame::Magpos), Per));
						}
					}
				}
				break;
				case RELOADTYPE::AMMO:
				{
					SetMagLoadSuccess(false);
					MaxPer = 0.5f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						float Per = GetGunAnimBlendPer(GetGunAnime());
						GetMagazinePtr()->SetHandMatrix(Lerp(MagPoachMat, GetFrameWorldMat_P(GunFrame::Mag2), Per));
					}
					BasePer = MaxPer;
					MaxPer = 0.7f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						GetMagazinePtr()->SetHandMatrix(GetFrameWorldMat_P(GunFrame::Mag2));
					}
					BasePer = MaxPer;
					MaxPer = 0.9f;
					if (BasePer <= AnimPer && AnimPer <= MaxPer) {
						float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
						GetMagazinePtr()->SetHandMatrix(Lerp(GetFrameWorldMat_P(GunFrame::Mag2), GetFrameWorldMat_P(GunFrame::Magpos), Per));
					}
				}
				break;
				default:
					GetMagazinePtr()->SetHandMatrix(GetFrameWorldMat_P(GunFrame::Magpos));
					break;
				}
				break;
			default:
				break;
			}
		}

		void				GunClass::Init_Sub(void) noexcept {
			{
				std::vector<const SharedObj*> PartsList;
				m_ModSlotControl.GetChildPartsList(&PartsList);
				this->m_SightPtr[0] = nullptr;
				this->m_SightPtr[1] = nullptr;
				this->m_MuzzlePtr = nullptr;
				this->m_UpperPtr = nullptr;
				this->m_LowerPtr = nullptr;
				this->m_MagazinePtr = nullptr;

				int SightSel = 0;
				for (auto& p : PartsList) {
					if ((*p)->GetobjType() == (int)ObjType::Sight) {
						if (SightSel < this->m_SightPtr.size()) {
							const auto* Ptr = &((std::shared_ptr<SightClass>&)(*p));
							this->m_SightPtr.at(SightSel) = Ptr;
							SightSel++;
						}
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

				//2つ以上サイトがあるときアイアンサイトを省く
				if (SightSel >= this->m_SightPtr.size()) {
					for (int i = 0; i < this->m_SightPtr.size(); i++) {
						if (this->m_SightPtr.at(i)) {
							if ((*this->m_SightPtr.at(i))->GetModSlot().GetModData()->IsIronSight()) {
								for (int k = i; k < this->m_SightPtr.size() - 1; k++) {
									this->m_SightPtr.at(k) = this->m_SightPtr.at(static_cast<size_t>(k + 1));
								}
								this->m_SightPtr.back() = nullptr;
							}
						}
					}
				}
				PartsList.clear();
			}
			SetAmmo(GetAmmoAll());								//マガジン装填
			CockByMag();										//チャンバーイン
		}
		void				GunClass::FirstExecute(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
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
				bool isHit = ((GetAmmoNumTotal() == 0) || (GetIsMagFull() && !GetInChamber()) || (GetGunAnime() == GunAnimeID::Cocking && (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime() <= 22.f)));
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
			auto SE = SoundPool::Instance();

			float Time{};
			switch (GetGunAnime()) {
			case GunAnimeID::Shot:
				Time = GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime();
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((0.5f < Time && Time < 1.f)) {
						if (this->m_boltSoundSequence != 1) {
							this->m_boltSoundSequence = 1;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					if ((5.f < Time && Time < 6.f)) {
						if (this->m_boltSoundSequence != 3) {
							this->m_boltSoundSequence = 3;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					break;
				default:
					break;
				}
				break;
			case GunAnimeID::Cocking:
				Time = GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime();
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((5.f < Time && Time < 6.f)) {
						if (this->m_boltSoundSequence != 1) {
							this->m_boltSoundSequence = 1;
							SE->Get(SoundType::SE, static_cast<int>(GetGunSoundSet().m_Cock1))->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					if ((28.f < Time && Time < 29.f)) {
						if (this->m_boltSoundSequence != 3) {
							this->m_boltSoundSequence = 3;
							SE->Get(SoundType::SE, static_cast<int>(GetGunSoundSet().m_Cock2))->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((8.f < Time && Time < 9.f)) {
						if (this->m_boltSoundSequence != 1) {
							this->m_boltSoundSequence = 1;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					if ((18.f < Time && Time < 19.f)) {
						if (this->m_boltSoundSequence != 2) {
							this->m_boltSoundSequence = 2;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				break;
			case GunAnimeID::ReloadStart_Empty:
				Time = GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime();
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((5.f < Time && Time < 6.f)) {
						if (this->m_boltSoundSequence != 5) {
							this->m_boltSoundSequence = 5;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < Time && Time < 1.f)) {
						if (this->m_boltSoundSequence != 3) {
							this->m_boltSoundSequence = 3;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				break;
			case GunAnimeID::ReloadStart:
				Time = GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime();
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((5.f < Time && Time < 6.f)) {
						if (this->m_boltSoundSequence != 5) {
							this->m_boltSoundSequence = 5;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < Time && Time < 1.f)) {
						if (this->m_boltSoundSequence != 3) {
							this->m_boltSoundSequence = 3;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				break;
			case GunAnimeID::Reload:
				Time = GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime();
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((10.f < Time && Time < 11.f)) {
						if (this->m_boltSoundSequence != 7) {
							this->m_boltSoundSequence = 7;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					else if (GetObj().GetAnim(GetGunAnimeID(GetGunAnime())).GetPer() != 0.f) {
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
				Time = GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime();
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((8.f < Time && Time < 9.f)) {
						if (this->m_boltSoundSequence != 8) {
							this->m_boltSoundSequence = 8;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < Time && Time < 1.f)) {
						if (this->m_boltSoundSequence != 5) {
							this->m_boltSoundSequence = 5;
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
			//
			{
				//1のフレーム移動量を無視する
				m_ModSlotControl.ResetPartsFrameLocalMat(GunFrame::Center);
				if (HaveFrame((int)GunFrame::Center)) {
					GetObj().ResetFrameUserLocalMatrix(GetFrame((int)GunFrame::Center));
				}
				GetObj().UpdateAnimAll();
				auto Rot = GetFrameLocalMat(GunFrame::Center).rotation();
				m_ModSlotControl.SetPartsFrameLocalMat(GunFrame::Center, Rot);
				if (HaveFrame((int)GunFrame::Center)) {
					GetObj().SetFrameLocalMatrix(GetFrame((int)GunFrame::Center), Rot * GetFrameBaseLocalMat((int)GunFrame::Center));
				}
			}
			//
			Easing(&this->m_GunChangePer, 1.f, 0.8f, EasingType::OutExpo);

			switch (GetGunAnime()) {
			case GunAnimeID::Shot:
				if ((GetShotType() != SHOTTYPE::BOLT) && GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime() >= 3.f) {
					this->m_IsChamberOn = true;
				}
				break;
			case GunAnimeID::Cocking:
				if (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime() >= 25.f) {
					this->m_IsChamberOn = true;
				}
				this->m_IsEject = (GetShotType() != SHOTTYPE::BOLT) && this->m_ShotSwitch;
				if (GetShotType() == SHOTTYPE::BOLT) {
					this->m_IsEject = (GetShotType() == SHOTTYPE::BOLT) && (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime() >= 19.f);
				}
				break;
			case GunAnimeID::ReloadStart_Empty:
				this->m_IsChamberOn = false;
				this->m_IsEject = (GetShotType() != SHOTTYPE::BOLT) && this->m_ShotSwitch;
				if (GetShotType() == SHOTTYPE::BOLT) {
					this->m_IsEject = (GetShotType() == SHOTTYPE::BOLT) && (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime() >= 19.f);
				}
				break;
			case GunAnimeID::ReloadStart:
				this->m_IsChamberOn = false;
				this->m_IsEject = (GetShotType() != SHOTTYPE::BOLT) && this->m_ShotSwitch;
				if (GetShotType() == SHOTTYPE::BOLT) {
					this->m_IsEject = (GetShotType() == SHOTTYPE::BOLT) && (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTime() >= 19.f);
				}
				break;
			default:
				this->m_IsChamberOn = false;
				this->m_IsEject = (GetShotType() != SHOTTYPE::BOLT) && this->m_ShotSwitch;
				break;
			}
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
