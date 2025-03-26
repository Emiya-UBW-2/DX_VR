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
					this->m_CartFall.SetFall(
						GetFrameWorldMat_P(GunFrame::Cart).pos(),
						Matrix3x3DX::Get33DX(GetFrameWorldMat_P(GunFrame::Muzzle)),
						(GetFrameWorldMat_P(GunFrame::CartVec).pos() - GetFrameWorldMat_P(GunFrame::Cart).pos() + Vector3DX::vget(GetRandf(0.2f), 0.5f + GetRandf(1.f), GetRandf(0.2f))).normalized() * (Scale3DRate * 2.f / 60.f), 2.f, SoundEnum::CartFall, false);
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
					if (this->m_boltSoundSequence != 1) {
						this->m_boltSoundSequence = 1;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
				if ((28.f < GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() && GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() < 29.f)) {
					if (this->m_boltSoundSequence != 3) {
						this->m_boltSoundSequence = 3;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
			}
			//1
			{
				if ((5.f < GetObj().SetAnim((int)GunAnimeID::ReloadStart_Empty).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadStart_Empty).GetTime() < 6.f)) {
					if (this->m_boltSoundSequence != 5) {
						this->m_boltSoundSequence = 5;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}

				if ((5.f < GetObj().SetAnim((int)GunAnimeID::ReloadStart).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadStart).GetTime() < 6.f)) {
					if (this->m_boltSoundSequence != 5) {
						this->m_boltSoundSequence = 5;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
			}
			//2
			{
				if ((10.f < GetObj().SetAnim((int)GunAnimeID::ReloadOne).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadOne).GetTime() < 11.f)) {
					if (this->m_boltSoundSequence != 7) {
						this->m_boltSoundSequence = 7;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
				else if (GetObj().SetAnim((int)GunAnimeID::ReloadOne).GetPer() != 0.f) {
					this->m_boltSoundSequence = -1;
				}
			}
			//3
			{
				if ((8.f < GetObj().SetAnim((int)GunAnimeID::ReloadEnd).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadEnd).GetTime() < 9.f)) {
					if (this->m_boltSoundSequence != 8) {
						this->m_boltSoundSequence = 8;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
			}
			//4
			{
				if ((0.5f < GetObj().SetAnim((int)GunAnimeID::Shot).GetTime() && GetObj().SetAnim((int)GunAnimeID::Shot).GetTime() < 1.f)) {
					if (this->m_boltSoundSequence != 1) {
						this->m_boltSoundSequence = 1;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 50.f);
					}
				}
				if ((5.f < GetObj().SetAnim((int)GunAnimeID::Shot).GetTime() && GetObj().SetAnim((int)GunAnimeID::Shot).GetTime() < 6.f)) {
					if (this->m_boltSoundSequence != 3) {
						this->m_boltSoundSequence = 3;
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
					if (this->m_boltSoundSequence != 1) {
						this->m_boltSoundSequence = 1;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
					}
				}
				if ((18.f < GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() && GetObj().SetAnim((int)GunAnimeID::Cocking).GetTime() < 19.f)) {
					if (this->m_boltSoundSequence != 2) {
						this->m_boltSoundSequence = 2;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
					}
				}
			}
			//1
			{
				if ((0.f < GetObj().SetAnim((int)GunAnimeID::ReloadStart_Empty).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadStart_Empty).GetTime() < 1.f)) {
					if (this->m_boltSoundSequence != 3) {
						this->m_boltSoundSequence = 3;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
					}
				}
				if ((0.f < GetObj().SetAnim((int)GunAnimeID::ReloadStart).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadStart).GetTime() < 1.f)) {
					if (this->m_boltSoundSequence != 3) {
						this->m_boltSoundSequence = 3;
						SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj().GetMatrix().pos(), Scale3DRate * 2.f);
					}
				}
			}
			//3
			{
				if ((0.f < GetObj().SetAnim((int)GunAnimeID::ReloadEnd).GetTime() && GetObj().SetAnim((int)GunAnimeID::ReloadEnd).GetTime() < 1.f)) {
					if (this->m_boltSoundSequence != 5) {
						this->m_boltSoundSequence = 5;
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
				LastAmmo->Put(this->m_ChamberAmmoData, MuzzleMat.pos(), mat.zvec() * -1.f, GetMyPlayerID());
			}
			UnloadChamber();
			m_MuzzleSmokeControl.AddMuzzleSmokePower();
			SetGunAnime(GunAnimeID::Shot);
			this->m_ShotSwitch = true;
		}
		void				GunClass::UpdateGunAnims() noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto prev = this->m_GunAnimeFirst;
			if (this->m_GunAnimeFirst) {
				this->m_GunAnimeTime = 0.f;
			}
			switch (GetGunAnime()) {
			case GunAnimeID::Shot:
				SetAnimOnce((int)GetGunAnime(), ((float)GetModSlot().GetModData()->GetShotRate()) / 300.f);
				break;
			case GunAnimeID::Cocking:
				SetAnimOnce((int)GetGunAnime(), 1.5f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
					SetGunAnime(GunAnimeID::Base);
				}
				break;
			case GunAnimeID::ReloadStart_Empty:
				if (this->m_GunAnimeFirst) {
					this->m_Cancel = false;
				}
				SetAnimOnce((int)GetGunAnime(), 1.0f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
					SetGunAnime(GunAnimeID::ReloadOne);
				}
				break;
			case GunAnimeID::ReloadStart:
				if (this->m_GunAnimeFirst) {
					this->m_Cancel = false;
				}
				SetAnimOnce((int)GetGunAnime(), 1.0f);
				if (GetObj().SetAnim((int)GetGunAnime()).TimeEnd()) {
					SetGunAnime(GunAnimeID::ReloadOne);
				}
				if ((GetReloadType() == RELOADTYPE::MAG) && this->m_Cancel) {
					this->m_Cancel = false;
					SetGunAnime(GunAnimeID::Base);
				}
				break;
			case GunAnimeID::ReloadOne:
				SetAnimOnce((int)GetGunAnime(), 1.0f);

				if ((GetReloadType() == RELOADTYPE::MAG) && this->m_Cancel) {
					this->m_Cancel = false;
					SetGunAnime(GunAnimeID::Base);
				}
				if (IsMagLoadSuccess() ? (GetGunAnimePer(EnumGunAnimType::Reload) >= 0.75f) : (GetGunAnimePer(EnumGunAnimType::Reload) >= 1.f)) {
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
						if ((this->GetIsMagFull() || (this->m_NextMagNum <= 0)) || this->m_Cancel) {
							this->m_Cancel = false;
							SetGunAnime(GunAnimeID::ReloadEnd);
						}
						else {
							this->GetObj().SetAnim((int)GetGunAnime()).Reset();
							this->GetObj().SetAnim((int)GetGunAnime()).SetPer(1.f);
							this->m_GunAnimeTime = 0.f;
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
					SetGunAnime(GunAnimeID::Base);
				}
				break;
			case GunAnimeID::Watch:
				if (GetGunAnimePer(EnumGunAnimType::Watch) >= 1.f) {
					SetGunAnime(GunAnimeID::Base);
				}
				break;
			case GunAnimeID::Base:
			case GunAnimeID::Open:
			case GunAnimeID::Hammer:
			default:
				break;
			}
			//
			this->m_GunAnimeTime += DXLib_refParts->GetDeltaTime();
			if (prev) {
				this->m_GunAnimeFirst = false;
			}
		}

		void				GunClass::SetFallObject() noexcept {
			this->m_MagFall.Dispose();
			this->m_CartFall.Dispose();
			this->m_MagFall.Init(GetMagazinePtr()->GetFilePath(), 1);
			this->m_CartFall.Init(GetMagazinePtr()->GetModSlot().GetModData()->GetAmmoSpecMagTop()->GetPath(), 4);	//装填したマガジンの弾に合わせて薬莢生成
		}
		void				GunClass::UpdateReticle() noexcept {
			m_AimPointControl.UpdateAimPointControl(GetEyePos() + GetFrameWorldMat_P(GunFrame::Muzzle).zvec() * -100.f);
			auto LensPos = GetFrameWorldMat_P(GunFrame::Lens).pos();
			m_ReticleControl.UpdateReticleControl(
				LensPos,
				GetFrameWorldMat_P(GunFrame::LensSize).pos(),
				LensPos + (LensPos - GetEyePos()).normalized() * (5.f * Scale3DRate));
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

		void				GunClass::Init_Gun(void) noexcept {
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
			SetGunAnime(GunAnimeID::Base);
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
			for (int i = 0; i < GetObj().GetAnimNum(); i++) {
				if (GetGunAnime() == (GunAnimeID)i) {
					GetObj().SetAnim(i).SetPer(1.f);
					if (GetGunAnime() == GunAnimeID::Watch) {
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
					else if ((GunAnimeID)i == GunAnimeID::Hammer) {
						bool isHit = false;
						if (this->m_GunSightSel == 1) {
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
					if (isDrawSemiTrans && GetMyPlayerID() == 0) {
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
