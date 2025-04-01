#include	"Gun.hpp"

#include "../../MainScene/Player/Player.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"
#include "../../CommonScene/Object/GunsModify.hpp"
#include "Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void				GunClass::InitGunAnime() noexcept {
			for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
				bool IsActiveGunAnim = GetGunAnime() == static_cast<GunAnimeID>(loop);
				switch (static_cast<GunAnimeID>(loop)) {
				case GunAnimeID::ADS:
					IsActiveGunAnim = false;// IsADS;
					break;
				case GunAnimeID::Shot:
					IsActiveGunAnim = false;//TODO
					break;
				default:
					break;
				}
				this->m_GunAnimePer[loop].Init(IsActiveGunAnim);
			}
		}
		void				GunClass::UpdateGunAnime(bool IsADS) noexcept {
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
		void				GunClass::SetShotStart(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			PlayerMngr->GetPlayer(GetMyUserPlayerID())->AddShot(GetPelletNum());
			//
			SE->Get(SoundType::SE, (int)SoundEnum::Trigger)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 5.f);
			switch ((this->m_MuzzlePtr) ? (*this->m_MuzzlePtr)->GetModSlot().GetModData()->GetGunShootSound() : GetModSlot().GetModData()->GetGunShootSound()) {
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
			//リコイル
			float Power = 0.0001f * GetRecoilPower();
			this->m_RecoilRadAdd.Set(GetRandf(Power / 4.f), -Power);
		}
		void				GunClass::SetGunMat(const Matrix3x3DX& AnimRot, const Vector3DX& AnimPos) noexcept {
			//武器座標
			SetMove().SetMat(AnimRot);
			SetMove().SetPos(AnimPos);
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
				Matrix4x4DX MatMin;
				Matrix4x4DX MatMax;
				float BasePer = 0.f;
				float MaxPer = 0.f;

				auto SetMat = [&](const Matrix4x4DX& MaxMat, float PerMax) {
					MatMin = MatMax;
					MatMax = MaxMat;
					BasePer = MaxPer;
					MaxPer = PerMax;
					if (BasePer <= GetNowGunAnimePer() && GetNowGunAnimePer() <= MaxPer) {
						(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((GetNowGunAnimePer() - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
					}
					};

				switch (GetGunAnime()) {
				case GunAnimeID::ReloadStart_Empty:
					(*this->m_MagazinePtr)->SetHandMatrix(this->m_MagazinePoachMat);
					break;
				case GunAnimeID::ReloadStart:
					MatMax = GetFrameWorldMatParts(GunFrame::Magpos);
					SetMat(GetFrameWorldMatParts(GunFrame::Magpos), 0.3f);
					SetMat(GetFrameWorldMatParts(GunFrame::Mag2), 0.6f);
					SetMat(this->m_MagazinePoachMat, 1.f);
					break;
				case GunAnimeID::Reload:
					switch (GetReloadType()) {
					case RELOADTYPE::MAG:
						MatMax = this->m_MagazinePoachMat;
						SetMat(this->m_MagazinePoachMat, 0.1f);
						if (GetNowGunAnimePer() <= 0.1f) {
							//マグチェンジ成功確率
							this->m_isMagSuccess = GetRand(100) < 50;
							float MissPer = GetRandf(0.025f);
							this->m_MagMiss = Matrix4x4DX::Mtrans(GetMove().GetMat().xvec() * (MissPer * Scale3DRate));
							this->m_MagSuccess = Matrix4x4DX::Mtrans(
								GetMove().GetMat().yvec() * (-0.05f * Scale3DRate) +
								GetMove().GetMat().xvec() * (MissPer / 3.f * Scale3DRate)
							);
						}
						else {
							if (this->m_isMagSuccess) {
								SetMat(GetFrameWorldMatParts(GunFrame::Mag2), 0.55f);
								SetMat(GetFrameWorldMatParts(GunFrame::Magpos), 0.75f);
								SetMat(GetFrameWorldMatParts(GunFrame::Magpos), 1.0f);
							}
							else {
								SetMat(GetFrameWorldMatParts(GunFrame::Mag2) * this->m_MagMiss, 0.55f);
								SetMat(GetFrameWorldMatParts(GunFrame::Mag2) * this->m_MagSuccess, 0.75f);
								SetMat(GetFrameWorldMatParts(GunFrame::Magpos), 0.85f);
								SetMat(GetFrameWorldMatParts(GunFrame::Magpos), 1.0f);
							}
						}
						break;
					case RELOADTYPE::AMMO:
						this->m_isMagSuccess = false;

						MatMax = this->m_MagazinePoachMat;
						SetMat(GetFrameWorldMatParts(GunFrame::Mag2), 0.5f);
						SetMat(GetFrameWorldMatParts(GunFrame::Mag2), 0.7f);
						SetMat(GetFrameWorldMatParts(GunFrame::Magpos), 0.9f);
						SetMat(GetFrameWorldMatParts(GunFrame::Magpos), 1.0f);
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
		void				GunClass::UpdateGunMat(bool IsSelGun, bool IsActiveAutoAim, const Matrix3x3DX& CharaRotationCache, const Vector3DX& HeadPos) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyUserPlayerID())->GetChara();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();

			//銃の揺れ
			Vector3DX Rad = MyChara->GetRotRad();
			Easing(&m_UpperRad, (this->m_UpperPrevRad - Rad), 0.9f, EasingType::OutExpo);
			m_UpperPrevRad = Rad;
			Easing(&m_GunSwingMat, Matrix3x3DX::RotAxis(Vector3DX::right(), m_UpperRad.x) * Matrix3x3DX::RotAxis(Vector3DX::up(), m_UpperRad.y), 0.8f, EasingType::OutExpo);
			Easing(&m_GunSwingMat2, m_GunSwingMat, 0.8f, EasingType::OutExpo);
			UpdateGunAnime(MyChara->GetIsADS());
			//スリング
			Easing(&m_SlingPer, IsSelGun ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			if (m_SlingPer <= 0.001f) { m_SlingPer = 0.f; }
			if (m_SlingPer >= 0.999f) { m_SlingPer = 1.f; }
			if (IsSelGun) {
				switch (GetGunAnime()) {
				case GunAnimeID::LowReady:
					m_MagHand = false;
					break;
				case GunAnimeID::Shot:
					this->m_ShotSwitch = false;
					if (GetShotType() != SHOTTYPE::BOLT) {
						this->m_IsChamberOn = false;
						this->m_IsEject = false;
						if (GetObj_const().GetAnim(GetNowGunAnimeID()).GetTime() >= 3.f) {
							this->m_IsChamberOn = true;
							this->m_IsEject = true;
						}
					}
					SetAnimOnce(GetNowGunAnimeID(), ((float)GetModSlot().GetModData()->GetShotRate()) / 300.f);
					if (GetObj_const().GetAnim(GetNowGunAnimeID()).GetTimePer() >= 1.f && this->m_ShotEnd) {
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
					SetAnimOnce(GetNowGunAnimeID(), 1.f);
					//
					switch (GetShotType()) {
					case SHOTTYPE::BOLT:
						if ((5.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 6.f)) {
							if (this->m_boltSoundSequence != 1) {
								this->m_boltSoundSequence = 1;
								SE->Get(SoundType::SE, static_cast<int>(GetGunSoundSet().m_Cock1))->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
							}
						}
						if ((28.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 29.f)) {
							if (this->m_boltSoundSequence != 2) {
								this->m_boltSoundSequence = 2;
								SE->Get(SoundType::SE, static_cast<int>(GetGunSoundSet().m_Cock2))->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
							}
						}
						break;
					case SHOTTYPE::FULL:
					case SHOTTYPE::SEMI:
						if ((8.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 9.f)) {
							if (this->m_boltSoundSequence != 3) {
								this->m_boltSoundSequence = 3;
								SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 2.f);
							}
						}
						if ((18.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 19.f)) {
							if (this->m_boltSoundSequence != 4) {
								this->m_boltSoundSequence = 4;
								SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 2.f);
							}
						}
						break;
					default:
						break;
					}
					this->m_IsChamberOn = false;
					this->m_IsEject = false;
					if (GetNowGunAnimeTime() >= 19.f) {
						this->m_IsChamberOn = true;
						if (GetShotType() == SHOTTYPE::BOLT) {
							this->m_IsEject = true;
						}
					}
					if (GetNowGunAnimePer() >= 1.f) {
						SetGunAnime(GunAnimeID::None);
					}
					break;
				case GunAnimeID::ReloadStart_Empty:
					if (GetReloadType() == RELOADTYPE::MAG) {
						if (GetNowGunAnimePer() >= 0.5f) {
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
					//
					m_MagHand = true;
					switch (GetShotType()) {
					case SHOTTYPE::BOLT:
						if ((5.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 6.f)) {
							if (this->m_boltSoundSequence != 5) {
								this->m_boltSoundSequence = 5;
								SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
							}
						}
						break;
					case SHOTTYPE::FULL:
					case SHOTTYPE::SEMI:
						if ((0.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 1.f)) {
							if (this->m_boltSoundSequence != 6) {
								this->m_boltSoundSequence = 6;
								SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 2.f);
							}
						}
						break;
					default:
						break;
					}
					if (GetNowGunAnimePer() >= 1.f) {
						SetGunAnime(GunAnimeID::Reload);
					}
					break;
				case GunAnimeID::ReloadStart:
					this->m_MagHand = true;
					this->m_ReloadAmmoCancel = false;
					switch (GetShotType()) {
					case SHOTTYPE::BOLT:
						if ((5.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 6.f)) {
							if (this->m_boltSoundSequence != 6) {
								this->m_boltSoundSequence = 6;
								SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
							}
						}
						break;
					case SHOTTYPE::FULL:
					case SHOTTYPE::SEMI:
						if ((0.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 1.f)) {
							if (this->m_boltSoundSequence != 6) {
								this->m_boltSoundSequence = 6;
								SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 2.f);
							}
						}
						break;
					default:
						break;
					}
					if (GetNowGunAnimePer() >= 1.f) {
						SetGunAnime(GunAnimeID::Reload);
					}
					break;
				case GunAnimeID::Reload:
					switch (GetShotType()) {
					case SHOTTYPE::BOLT:
						if ((10.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 11.f)) {
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
					if (GetNowGunAnimePer() >= (this->m_isMagSuccess ? 0.75f : 1.0f)) {
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
					switch (GetShotType()) {
					case SHOTTYPE::BOLT:
						if ((8.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 9.f)) {
							if (this->m_boltSoundSequence != 9) {
								this->m_boltSoundSequence = 9;
								SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetObj_const().GetMatrix().pos(), Scale3DRate * 50.f);
							}
						}
						break;
					case SHOTTYPE::FULL:
					case SHOTTYPE::SEMI:
						if ((0.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 1.f)) {
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
						if (GetNowGunAnimePer() > 0.6f) {
							m_MagHand = false;
						}
						break;
					case RELOADTYPE::AMMO:
						m_MagHand = false;
						break;
					default:
						break;
					}
					if (GetNowGunAnimePer() >= 1.f) {
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
					if (GetNowGunAnimePer() >= 1.f) {
						SetGunAnime(GunAnimeID::None);
					}
					break;
				default:
					break;
				}
				//
				if (GetMyUserPlayerID() == 0) {
					printfDx("[%s]\n", (GetGunAnime() == GunAnimeID::None) ? "None" : GunAnimeIDName[(int)GetGunAnime()]);
					printfDx("[%f]\n", (GetGunAnime() == GunAnimeID::None) ? 0.0f : GetNowGunAnimeTime());
					printfDx("[%f]\n", GetGunAnimBlendPer(GunAnimeID::LowReady));
				}
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
			//
			Vector3DX BaseVec = GetMove().GetMat().yvec();
			auto EyeYRot = Matrix3x3DX::RotVec2(Lerp(BaseVec, GetADSEyeMat().yvec(), GetGunAnimBlendPer(GunAnimeID::ADS)), BaseVec);
			//
			Matrix4x4DX AnimMat = GetGunAnimeNow();
			Matrix3x3DX AnimRot = Matrix3x3DX::Get33DX(AnimMat) * m_GunSwingMat2 * CharaRotationCache * EyeYRot;
			Vector3DX AnimPos = AnimMat.pos();
			//AnimPos.x *= this->m_SwitchPer;
			AnimPos = HeadPos + Matrix3x3DX::Vtrans(AnimPos, CharaRotationCache);
			//オートエイム
			if (IsSelGun) {
				m_AutoAimControl.Update(IsActiveAutoAim, GetMyUserPlayerID(), MyChara->GetEyePositionCache(), GetMove().GetMat().zvec() * -1.f, GetAutoAimRadian());
				m_AutoAimControl.CalcAutoAimMat(&AnimRot);
			}
			//武器座標
			SetGunMat(Lerp(m_SlingRot * m_GunSwingMat2 * EyeYRot, AnimRot, m_SlingPer), Lerp(m_SlingPos, AnimPos, m_SlingPer));
		}
		void				GunClass::Init_Sub(void) noexcept {
			SetModSlot().Init(this->m_FilePath);
			GunsModify::Instance()->CreateSelData(shared_from_this(), false);
			{
				std::vector<const SharedObj*> PartsList;
				GetModSlot().CalcAnyBySlot([&](const SharedObj& ptr) { if (ptr) { PartsList.emplace_back(&ptr); } });
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
			InitGunAnime();
		}
		void				GunClass::FirstExecute(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			if (this->m_IsFirstLoop) {
				this->m_MuzzleSmokeControl.InitMuzzleSmoke(GetFrameWorldMatParts(GunFrame::Muzzle).pos());
			}
			else {
				this->m_MuzzleSmokeControl.ExecuteMuzzleSmoke(GetFrameWorldMatParts(GunFrame::Muzzle).pos(), GetGunAnime() != GunAnimeID::Shot);
			}
			//
			for (int i = 0; i < static_cast<int>(GunAnimeID::ChoiceOnceMax); i++) {
				int ID = GetModSlot().GetModData()->GetAnimSelectList().at(i);
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
				bool isHit = ((GetAmmoNumTotal() == 0) || ((this->m_Capacity == GetAmmoAll()) && !CanShootAmmo()) || (GetGunAnime() == GunAnimeID::Cocking && (GetNowGunAnimeTime() <= 22.f)));
				for (int i = static_cast<int>(GunAnimeID::ChoiceOnceMax); i < static_cast<int>(GunAnimeID::Max); i++) {
					int ID = GetModSlot().GetModData()->GetAnimSelectList().at(i);
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
			//リコイルの演算
			if (this->m_RecoilRadAdd.y < 0.f) {
				Easing(&this->m_RecoilRadAdd, Vector2DX::vget(0.f, 0.01f), GetRecoilReturn(), EasingType::OutExpo);
			}
			else {
				Easing(&this->m_RecoilRadAdd, Vector2DX::zero(), 0.7f, EasingType::OutExpo);
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
