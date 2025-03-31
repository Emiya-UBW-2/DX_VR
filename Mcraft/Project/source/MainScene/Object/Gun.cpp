#include	"Gun.hpp"

#include "../../MainScene/BackGround/BackGround.hpp"
#include "../../CommonScene/Object/GunsModify.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const Matrix4x4DX	GunClass::GetFrameWorldMatParts(GunFrame frame) const noexcept {
			if (this->m_SightPtr) {
				switch (frame) {
				case GunFrame::Lens:
				case GunFrame::LensSize:
				case GunFrame::Eyepos:
					return (*this->m_SightPtr)->GetFramePartsMat(frame);
				default:
					break;
				}
			}
			auto* ptr = GetModSlot().HasFrameBySlot(frame);
			if (ptr) {
				return ((std::shared_ptr<ModClass>&)(*ptr))->GetFramePartsMat(frame);
			}
			if (HaveFrame((int)frame)) {
				return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame)));
			}
			return Matrix4x4DX::identity();
		}
		void				GunClass::SetActiveAll(bool value) noexcept {
			if (value != IsActive()) {
				SetModSlot().SetActiveBySlot(value);
				SetActive(value);
			}
		}
		void GunClass::UpdateSlingPer(bool IsSelGun) noexcept {
			Easing(&m_SlingPer, IsSelGun ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			if (m_SlingPer <= 0.001f) { m_SlingPer = 0.f; }
			if (m_SlingPer >= 0.999f) { m_SlingPer = 1.f; }
		}
		void GunClass::UpdateGunAnime(bool IsADS, bool IsForce) noexcept {
			//銃アニメの指定
			for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
				bool IsActiveGunAnim = GetGunAnime() == static_cast<GunAnimeID>(loop);
				switch (static_cast<GunAnimeID>(loop)) {
				case GunAnimeID::ADS:
					IsActiveGunAnim = IsADS;
					break;
				case GunAnimeID::Shot:
					IsActiveGunAnim = false;//TODO
					break;
				default:
					break;
				}
				if (IsForce) {
					this->m_GunAnimePer[loop].Init(IsActiveGunAnim);
				}
				else {
#if TRUE
					switch (static_cast<GunAnimeID>(loop)) {
					case GunAnimeID::LowReady:
						this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.f, 0.87f, 0.87f);
						break;
					case GunAnimeID::ADS:
						this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.2f, 0.2f, 0.9f, 0.9f);
						break;
					case GunAnimeID::Cocking:
						this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.f, 0.1f, 0.9f);
						break;
					case GunAnimeID::ReloadStart_Empty:
						this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.5f, 0.2f, 0.9f, 0.0f);
						break;
					case GunAnimeID::ReloadStart:
						this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.2f, 0.2f, 0.9f, 0.0f);
						break;
					case GunAnimeID::Reload:
						this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.1f, 0.2f, 0.0f, 0.0f);
						break;
					case GunAnimeID::ReloadEnd:
						this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.1f, 0.2f, 0.0f, 0.9f);
						break;
					case GunAnimeID::Watch:
						this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.1f, 0.1f, 0.8f, 0.8f);
						break;
					case GunAnimeID::ThrowReady:
						this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.1f, 0.1f, 0.8f, 0.8f);
						break;
					case GunAnimeID::Throw:
						this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.1f, 0.1f, 0.8f, 0.8f);
						break;
					default:
						break;
					}
#else
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim[loop], 0.2f, 0.2f, 0.9f, 0.9f);
#endif
				}
			}
			if (!IsForce) {
				auto* DXLib_refParts = DXLib_ref::Instance();
				switch (GetGunAnime()) {
				case GunAnimeID::Shot:
					SetAnimOnce(GetGunAnimeID(GetGunAnime()), ((float)GetModSlot().GetModData()->GetShotRate()) / 300.f);
					if (GetObj_const().GetAnim(GetGunAnimeID(GetGunAnime())).GetTimePer() >= 1.f && this->m_ShotEnd) {
						this->m_ShotEnd = false;
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
					if (GetReloadType() == RELOADTYPE::MAG) {
						if (GetGunAnimePer(GetGunAnime()) >= 0.5f) {
							if (!this->m_isMagFall) {
								this->m_isMagFall = true;
								auto MagposMat = GetFrameWorldMatParts(GunFrame::Magpos);
								this->m_MagFall.SetFall(
									MagposMat.pos(), Matrix3x3DX::Get33DX(MagposMat), MagposMat.yvec() * (Scale3DRate * -3.f / 60.f),
									12.f, SoundEnum::MagFall, false);
							}
						}
						else {
							this->m_isMagFall = false;
						}
					}
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
							this->m_Capacity = GetAmmoAll();//マガジン装填
							SetGunAnime(GunAnimeID::ReloadEnd);
							break;
						case RELOADTYPE::AMMO:
							this->m_Capacity++;//マガジン装填
							if ((this->m_Capacity == GetAmmoAll()) || this->m_ReloadAmmoCancel) {
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
						if (CanShootAmmo()) {
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
				printfDx("[%s]\n", (GetGunAnime() == GunAnimeID::None) ? "None" : GunAnimeIDName[(int)GetGunAnime()]);
				printfDx("[%f]\n", (GetGunAnime() == GunAnimeID::None) ? 0.0f : GetGunAnimeTime(GetGunAnime()));
				for (auto& g : this->m_GunAnimeTime) {
					int index = static_cast<int>(&g - &this->m_GunAnimeTime.front());
					if (index == (int)GetGunAnime()) {
						if (this->m_IsGunAnimChange) {
							this->m_IsGunAnimChange = false;
							continue;
						}
					}
					g += 60.f * DXLib_refParts->GetDeltaTime();
				}
			}
		}
		void				GunClass::SetShotStart(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			//
			SE->Get(SoundType::SE, (int)SoundEnum::Trigger)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 5.f);
			switch (GetGunShootSound()) {
			case GunShootSound::Normal:
				SE->Get(SoundType::SE, (int)GetGunSoundSet().m_ShotNormal)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
				break;
			case GunShootSound::Suppressor:
				SE->Get(SoundType::SE, (int)GetGunSoundSet().m_ShotSuppressor)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
				break;
			default:
				break;
			}
			for (int i = 0, max = GetPelletNum(); i < max; i++) {
				auto LastAmmo = std::make_shared<AmmoClass>();
				ObjMngr->AddObject(LastAmmo);
				LastAmmo->Init();
				auto mat =
					Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) *
					GetMove().GetMat();
				LastAmmo->Put(this->m_ChamberAmmoData, GetFrameWorldMatParts(GunFrame::Muzzle).pos(), mat.zvec() * -1.f, GetMyUserPlayerID());
			}
			this->m_ChamberAmmoData.reset(); //UnloadChamber
			this->m_MuzzleSmokeControl.AddMuzzleSmokePower();
			this->m_ShotSwitch = true;
			SetGunAnime(GunAnimeID::Shot);
		}
		void GunClass::UpdateGunMat(const Matrix3x3DX& rotation, const Vector3DX& pos, bool IsSelGun) noexcept {
			//武器座標
			SetMove().SetMat(Lerp(m_SlingRot, rotation, m_SlingPer));
			SetMove().SetPos(Lerp(m_SlingPos, pos, m_SlingPer));
			SetMove().Update(0.f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			SetModSlot().UpdatePartsAnim(GetObj_const());
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::UnderRail), GunSlot::UnderRail);
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Lower), GunSlot::Lower);
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Upper), GunSlot::Upper);
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Barrel), GunSlot::Barrel);
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Sight), GunSlot::Sight);
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Magpos), GunSlot::Magazine);
			//LeftMag
			if (this->m_MagazinePtr) {
				if (!IsSelGun) {
					(*this->m_MagazinePtr)->SetHandMatrix(GetFrameWorldMatParts(GunFrame::Magpos));
				}
				else {
					Matrix4x4DX MatMin;
					Matrix4x4DX MatMax;
					float AnimPer = 0.f;
					float BasePer = 0.f;
					float MaxPer = 0.f;
					switch (GetGunAnime()) {
					case GunAnimeID::ReloadStart_Empty:
						(*this->m_MagazinePtr)->SetHandMatrix(this->m_MagazinePoachMat);
						break;
					case GunAnimeID::ReloadStart:
						AnimPer = GetGunAnimePer(GetGunAnime());
						{
							MatMin = GetFrameWorldMatParts(GunFrame::Magpos);
							MatMax = GetFrameWorldMatParts(GunFrame::Magpos);
							MaxPer = 0.3f;
							if (BasePer <= AnimPer && AnimPer <= MaxPer) {
								(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
								break;
							}
							MatMin = MatMax;
							MatMax = GetFrameWorldMatParts(GunFrame::Mag2);
							BasePer = MaxPer;
							MaxPer = 0.6f;
							if (BasePer <= AnimPer && AnimPer <= MaxPer) {
								(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
								break;
							}

							MatMin = MatMax;
							MatMax = this->m_MagazinePoachMat;
							BasePer = MaxPer;
							MaxPer = 1.f;
							if (BasePer <= AnimPer && AnimPer <= MaxPer) {
								(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
								break;
							}
						}
						break;
					case GunAnimeID::Reload:
						AnimPer = GetGunAnimePer(GetGunAnime());
						switch (GetReloadType()) {
						case RELOADTYPE::MAG:
						{
							MatMin = this->m_MagazinePoachMat;
							MatMax = this->m_MagazinePoachMat;
							MaxPer = 0.1f;
							if (BasePer <= AnimPer && AnimPer <= MaxPer) {
								(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
								//マグチェンジ成功確率
								this->m_isMagSuccess = GetRand(100) < 50;
								float MissPer = GetRandf(0.025f);
								this->m_MagMiss = Matrix4x4DX::Mtrans(GetMove().GetMat().xvec() * (MissPer * Scale3DRate));
								this->m_MagSuccess = Matrix4x4DX::Mtrans(
									GetMove().GetMat().yvec() * (-0.05f * Scale3DRate) +
									GetMove().GetMat().xvec() * (MissPer / 3.f * Scale3DRate)
								);
								break;
							}
							if (this->m_isMagSuccess) {
								MatMin = MatMax;
								MatMax = GetFrameWorldMatParts(GunFrame::Mag2);
								BasePer = MaxPer;
								MaxPer = 0.55f;
								if (BasePer <= AnimPer && AnimPer <= MaxPer) {
									(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
									break;
								}
								MatMin = MatMax;
								MatMax = GetFrameWorldMatParts(GunFrame::Magpos);
								BasePer = MaxPer;
								MaxPer = 0.75f;
								if (BasePer <= AnimPer && AnimPer <= MaxPer) {
									(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
									break;
								}
								MatMin = MatMax;
								MatMax = GetFrameWorldMatParts(GunFrame::Magpos);
								BasePer = MaxPer;
								MaxPer = 1.f;
								if (BasePer <= AnimPer && AnimPer <= MaxPer) {
									(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
									break;
								}
							}
							else {
								MatMin = MatMax;
								MatMax = GetFrameWorldMatParts(GunFrame::Mag2) * this->m_MagMiss;
								BasePer = MaxPer;
								MaxPer = 0.55f;
								if (BasePer <= AnimPer && AnimPer <= MaxPer) {
									(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
									break;
								}
								MatMin = MatMax;
								MatMax = GetFrameWorldMatParts(GunFrame::Mag2) * this->m_MagSuccess;
								BasePer = MaxPer;
								MaxPer = 0.75f;
								if (BasePer <= AnimPer && AnimPer <= MaxPer) {
									(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
									break;
								}
								MatMin = MatMax;
								MatMax = GetFrameWorldMatParts(GunFrame::Mag2);
								BasePer = MaxPer;
								MaxPer = 0.85f;
								if (BasePer <= AnimPer && AnimPer <= MaxPer) {
									(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
									break;
								}
								MatMin = MatMax;
								MatMax = GetFrameWorldMatParts(GunFrame::Magpos);
								BasePer = MaxPer;
								MaxPer = 1.0f;
								if (BasePer <= AnimPer && AnimPer <= MaxPer) {
									(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
									break;
								}
							}
						}
						break;
						case RELOADTYPE::AMMO:
						{
							this->m_isMagSuccess = false;
							MatMin = this->m_MagazinePoachMat;
							MatMax = GetFrameWorldMatParts(GunFrame::Mag2);
							MaxPer = 0.5f;
							if (BasePer <= AnimPer && AnimPer <= MaxPer) {
								(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
								break;
							}
							MatMin = MatMax;
							MatMax = GetFrameWorldMatParts(GunFrame::Mag2);
							BasePer = MaxPer;
							MaxPer = 0.7f;
							if (BasePer <= AnimPer && AnimPer <= MaxPer) {
								(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
								break;
							}
							MatMin = MatMax;
							MatMax = GetFrameWorldMatParts(GunFrame::Magpos);
							BasePer = MaxPer;
							MaxPer = 0.9f;
							if (BasePer <= AnimPer && AnimPer <= MaxPer) {
								(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
								break;
							}
							MatMin = MatMax;
							MatMax = GetFrameWorldMatParts(GunFrame::Magpos);
							BasePer = MaxPer;
							MaxPer = 1.f;
							if (BasePer <= AnimPer && AnimPer <= MaxPer) {
								(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
								break;
							}
						}
						break;
						default:
							break;
						}
						break;
					default:
						(*this->m_MagazinePtr)->SetHandMatrix(GetFrameWorldMatParts(GunFrame::Magpos));
						break;
					}
				}
			}
		}
		void				GunClass::Init_Sub(void) noexcept {
			SetModSlot().InitModSlotControl(this->m_FilePath);
			GunsModify::Instance()->CreateSelData(shared_from_this(), false);
			{
				std::vector<const SharedObj*> PartsList;
				GetModSlot().GetChildPartsList(&PartsList);
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
				this->m_MagFall.Init((*this->m_MagazinePtr)->GetFilePath(), 1);
				this->m_CartFall.Init((*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoSpecMagTop()->GetPath(), 4);	//装填したマガジンの弾に合わせて薬莢生成
			}
			SetGunAnime(GunAnimeID::None);
			UpdateGunAnime(false, true);
			this->m_Capacity = GetAmmoAll();//マガジン装填
			if (this->m_MagazinePtr) {
				this->m_ChamberAmmoData = (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoSpecMagTop();//マガジンの一番上の弾データをチャンバーイン
			}
		}
		void				GunClass::FirstExecute(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			if (this->m_IsFirstLoop) {
				this->m_MuzzleSmokeControl.InitMuzzleSmoke(GetFrameWorldMatParts(GunFrame::Muzzle).pos());
			}
			else {
				this->m_MuzzleSmokeControl.ExecuteMuzzleSmoke(GetFrameWorldMatParts(GunFrame::Muzzle).pos(), GetGunAnime() != GunAnimeID::Shot);
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
				bool isHit = ((GetAmmoNumTotal() == 0) || ((this->m_Capacity == GetAmmoAll()) && !CanShootAmmo()) || (GetGunAnime() == GunAnimeID::Cocking && (GetGunAnimeTime(GetGunAnime()) <= 22.f)));
				for (int i = static_cast<int>(GunAnimeID::ChoiceOnceMax); i < static_cast<int>(GunAnimeID::Max); i++) {
					int ID = GetGunAnimeID((GunAnimeID)i);
					if (ID != -1) {
						switch ((GunAnimeID)i) {
						case FPS_n2::Sceneclass::GunAnimeID::Hammer:
							GetObj().SetAnim(ID).SetPer(std::clamp(GetObj_const().GetAnim(ID).GetPer() + DXLib_refParts->GetDeltaTime() * ((GetGunAnime() == GunAnimeID::Shot) ? -5.f : 5.f), 0.f, 1.f));
							break;
						case FPS_n2::Sceneclass::GunAnimeID::Open:
							GetObj().SetAnim(ID).SetPer(std::clamp(GetObj_const().GetAnim(ID).GetPer() + DXLib_refParts->GetDeltaTime() * (isHit ? 10.f : -10.f), 0.f, 1.f));
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
			case GunAnimeID::LowReady:
				m_MagHand = false;
				break;
			case GunAnimeID::Shot:
				this->m_ShotSwitch = false;
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
							SE->Get(SoundType::SE, static_cast<int>(GetGunSoundSet().m_Cock1))->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					if ((28.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 29.f)) {
						if (this->m_boltSoundSequence != 2) {
							this->m_boltSoundSequence = 2;
							SE->Get(SoundType::SE, static_cast<int>(GetGunSoundSet().m_Cock2))->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((8.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 9.f)) {
						if (this->m_boltSoundSequence != 3) {
							this->m_boltSoundSequence = 3;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					if ((18.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 19.f)) {
						if (this->m_boltSoundSequence != 4) {
							this->m_boltSoundSequence = 4;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 2.f);
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
				m_MagHand = true;
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((5.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 6.f)) {
						if (this->m_boltSoundSequence != 5) {
							this->m_boltSoundSequence = 5;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 1.f)) {
						if (this->m_boltSoundSequence != 6) {
							this->m_boltSoundSequence = 6;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				break;
			case GunAnimeID::ReloadStart:
				m_MagHand = true;
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((5.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 6.f)) {
						if (this->m_boltSoundSequence != 6) {
							this->m_boltSoundSequence = 6;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 1.f)) {
						if (this->m_boltSoundSequence != 6) {
							this->m_boltSoundSequence = 6;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 2.f);
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
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
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
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < GetGunAnimeTime(GetGunAnime()) && GetGunAnimeTime(GetGunAnime()) < 1.f)) {
						if (this->m_boltSoundSequence != 10) {
							this->m_boltSoundSequence = 10;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Load)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				switch (GetReloadType()) {
				case RELOADTYPE::MAG:
					if (GetGunAnimePer(GetGunAnime()) > 0.6f) {
						m_MagHand = false;
					}
					break;
				case RELOADTYPE::AMMO:
					m_MagHand = false;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}

			m_MagArm.Update(m_MagHand, 0.1f, 0.1f, 0.7f, 0.7f);
			GetObj().UpdateAnimAll();
			//弾薬の演算
			if (this->m_PrevChamberOn != this->m_IsChamberOn) {
				this->m_PrevChamberOn = this->m_IsChamberOn;
				if (this->m_IsChamberOn) {
					if (!GetIsMagEmpty()) {
						if (this->m_MagazinePtr) {
							this->m_ChamberAmmoData = (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoSpecMagTop();//マガジンの一番上の弾データをチャンバーイン
						}
					}
					this->m_Capacity = std::clamp(this->m_Capacity - 1, 0, GetAmmoAll());//マガジン装填
				}
			}
			if (this->m_PrevEject != this->m_IsEject) {
				this->m_PrevEject = this->m_IsEject;
				if (this->m_IsEject) {
					this->m_CartFall.SetFall(
						GetFrameWorldMatParts(GunFrame::Cart).pos(), GetMove().GetMat(),
						(GetFrameWorldMatParts(GunFrame::CartVec).pos() - GetFrameWorldMatParts(GunFrame::Cart).pos() + Vector3DX::vget(GetRandf(0.2f), 0.5f + GetRandf(1.f), GetRandf(0.2f))).normalized() * (Scale3DRate * 2.f / 60.f), 2.f, SoundEnum::CartFall, false);
				}
			}
		}
		void				GunClass::DrawShadow(void) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				auto* CameraParts = Camera3D::Instance();
				if ((GetMove().GetPos() - CameraParts->GetMainCamera().GetCamPos()).magnitude() > 10.f * Scale3DRate) { return; }
				GetObj_const().DrawModel();
			}
		}
		void				GunClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if (CheckCameraViewClip_Box(
					(GetObj_const().GetMatrix().pos() + Vector3DX::vget(-0.5f * Scale3DRate, -0.5f * Scale3DRate, -0.5f * Scale3DRate)).get(),
					(GetObj_const().GetMatrix().pos() + Vector3DX::vget(0.5f * Scale3DRate, 0.5f * Scale3DRate, 0.5f * Scale3DRate)).get()) == FALSE
					) {
					if (isDrawSemiTrans && GetMyUserPlayerID() == 0) {
						this->m_MuzzleSmokeControl.DrawMuzzleSmoke();
					}
					for (int i = 0; i < GetObj_const().GetMeshNum(); i++) {
						if (GetObj_const().GetMeshSemiTransState(i) == isDrawSemiTrans) {
							GetObj_const().DrawMesh(i);
						}
					}
				}
			}
		}
	};
};
