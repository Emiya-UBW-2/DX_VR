#include	"Gun.hpp"

#include	"../../MainScene/Player/Player.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"Character.hpp"

namespace FPS_n2 {
	namespace Guns {
		void				GunObj::InitGunAnimePer(void) noexcept {
			for (int loop = 0; loop < static_cast<int>(Charas::GunAnimeID::ChoiceOnceMax); ++loop) {
				bool IsActiveGunAnim = GetGunAnime() == static_cast<Charas::GunAnimeID>(loop);
				if (static_cast<Charas::GunAnimeID>(loop) == Charas::GunAnimeID::ADS) {
					IsActiveGunAnim = false;// IsADS;
				}
				this->m_GunAnimePer[loop].Init(IsActiveGunAnim);
			}
		}
		void				GunObj::UpdateGunAnimePer(bool IsADS) noexcept {
			for (int loop = 0; loop < static_cast<int>(Charas::GunAnimeID::ChoiceOnceMax); ++loop) {
				bool IsActiveGunAnim = GetGunAnime() == static_cast<Charas::GunAnimeID>(loop);
				if (static_cast<Charas::GunAnimeID>(loop) == Charas::GunAnimeID::ADS) {
					IsActiveGunAnim = IsADS;
				}
#if true
				switch (static_cast<Charas::GunAnimeID>(loop)) {
				case Charas::GunAnimeID::HighReady:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.f, 0.87f, 0.87f);
					break;
				case Charas::GunAnimeID::LowReady:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.f, 0.87f, 0.87f);
					break;
				case Charas::GunAnimeID::ADS:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.2f, 0.9f, 0.9f);
					break;
				case Charas::GunAnimeID::Cocking:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.f, 0.1f, 0.9f);
					break;
				case Charas::GunAnimeID::ReloadStart_Empty:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.5f, 0.0f, 0.9f, 0.0f);
					break;
				case Charas::GunAnimeID::ReloadStart:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.2f, 0.0f, 0.9f, 0.0f);
					break;
				case Charas::GunAnimeID::ReloadWait:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.0f, 0.0f, 0.0f, 0.0f);
					break;
				case Charas::GunAnimeID::Reload:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.0f, 0.0f, 0.0f, 0.0f);
					break;
				case Charas::GunAnimeID::ReloadEnd:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.0f, 0.2f, 0.0f, 0.9f);
					break;
				case Charas::GunAnimeID::Watch:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.1f, 0.1f, 0.9f, 0.9f);
					break;
				case Charas::GunAnimeID::ThrowReady:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.1f, 0.1f, 0.8f, 0.8f);
					break;
				case Charas::GunAnimeID::Throw:
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
		//
		void				GunObj::SetShotStart(void) noexcept {
			if (!IsInChamber()) { return; }
			if (GetModifySlot()->GetMyData()->GetIsThrowWeapon()) { return; }
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//���̃v���C���[�̓y���b�g�������܂�����
			const auto& ChamberAmmoData = (*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetAmmoSpecMagTop();//�}�K�W���̈�ԏ�̒e�f�[�^���`�����o�[�C��
			PlayerMngr->GetPlayer(GetMyUserPlayerID())->AddShot(ChamberAmmoData->GetPellet());
			auto MuzzleMat = GetPartsFrameMatParent(GunFrame::Muzzle);
			//�e��
			SE->Get(SoundType::SE, GetGunSoundSet(
				(this->m_MuzzlePtr) ? (*this->m_MuzzlePtr)->GetModifySlot()->GetMyData()->GetGunShootSound() : GetModifySlot()->GetMyData()->GetGunShootSound()
			))->Play3D(MuzzleMat.pos(), Scale3DRate * 50.f);
			//�G�t�F�N�g
			EffectSingleton::Instance()->SetOnce_Any(Sceneclass::Effect::ef_fire2, MuzzleMat.pos(), MuzzleMat.zvec() * -1.f, 0.35f, 2.f);
			//���C
			for (int loop = 0, max = ChamberAmmoData->GetPellet(); loop < max; ++loop) {
				auto LastAmmo = std::make_shared<Objects::AmmoObj>();
				ObjectManager::Instance()->InitObject(LastAmmo);
				LastAmmo->Put(ChamberAmmoData, MuzzleMat.pos(),
					(
						Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(ChamberAmmoData->GetAccuracy()))) *
						Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(GetRandf(ChamberAmmoData->GetAccuracy()))) *
						Matrix3x3DX::Get33DX(MuzzleMat)
						).zvec() * -1.f, GetMyUserPlayerID());
			}
			//�`�����o�[����ɂ���
			this->m_InChamber = false;
			//�e�����̒ǉ�
			this->m_MuzzleSmokeControl.AddMuzzleSmokePower();
			//�����A�j���Ɉڍs
			SetGunAnime(Charas::GunAnimeID::Shot);
			this->m_AmmoInChamberObj->SetActive(true);
			//���R�C��
			float Power = 0.0001f * GetRecoilPower();
			this->m_RecoilRadAdd.Set(GetRandf(Power / 4.f), -Power);
			//�r�W���A�����R�C��
			if (GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
				Camera3D::Instance()->SetCamShake(0.1f, 0.1f);
			}
		}
		//
		void				GunObj::UpdateGunMat(bool IsSelectGun, bool IsActiveAutoAim, const Matrix3x3DX& CharaRotationCache, const Vector3DX& HeadPos, const Vector3DX& RotRad) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			if (IsSelectGun) {
				//�A�j���[�V����
				if (GetNowGunAnimeID() != InvalidID) {
					SetAnimOnce(GetNowGunAnimeID(), this->m_GunAnimeSpeed.at(static_cast<int>(GetGunAnime())));
				}
				switch (GetGunAnime()) {
				case Charas::GunAnimeID::Base:
					if (!IsInChamber() && this->m_Capacity != 0) {
						SetGunAnime(Charas::GunAnimeID::Cocking);//�`�����o�[����Œe�q����łȂ��Ȃ�
					}
					break;
				case Charas::GunAnimeID::Shot:
					if (IsNowGunAnimeEnd()) {
						if (this->m_ShotEnd) {
							this->m_ShotEnd = false;
							SetGunAnime(Charas::GunAnimeID::Base);
						}
					}
					break;
				case Charas::GunAnimeID::Cocking:
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(Charas::GunAnimeID::Base);
					}
					break;
				case Charas::GunAnimeID::ReloadStart_Empty:
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(Charas::GunAnimeID::ReloadWait);
					}
					break;
				case Charas::GunAnimeID::ReloadStart:
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(Charas::GunAnimeID::ReloadWait);
					}
					break;
				case Charas::GunAnimeID::ReloadWait:
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(Charas::GunAnimeID::Reload);
						SE->Get(SoundType::SE, static_cast<int>(Sceneclass::SoundEnum::StandupFoot))->Play3D(GetMove().GetPos(), Scale3DRate * 3.f);
					}
					break;
				case Charas::GunAnimeID::Reload:
					if (GetNowAnimTimePerCache() >= (this->m_isMagSuccess ? 0.75f : 1.0f)) {
						switch (GetReloadType()) {
						case RELOADTYPE::MAG:
							this->m_Capacity = GetAmmoAll();//�}�K�W�����U
							SetGunAnime(Charas::GunAnimeID::ReloadEnd);
							break;
						case RELOADTYPE::AMMO:
							++this->m_Capacity;//�}�K�W�����U
							if ((this->m_Capacity == GetAmmoAll()) || this->m_ReloadAmmoCancel) {
								this->m_ReloadAmmoCancel = false;
								SetGunAnime(Charas::GunAnimeID::ReloadEnd);
							}
							else {
								SetGunAnime(Charas::GunAnimeID::ReloadWait);
							}
							break;
						default:
							break;
						}
					}
					break;
				case Charas::GunAnimeID::ReloadEnd:
					if (!IsInChamber()) {
						SetGunAnime(Charas::GunAnimeID::Cocking);
					}
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(Charas::GunAnimeID::Base);
					}
					break;
				case Charas::GunAnimeID::Watch:
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(Charas::GunAnimeID::Base);
					}
					break;
				default:
					break;
				}
				for (auto& time : this->m_GunAnimeTime) {
					int index = static_cast<int>(&time - &this->m_GunAnimeTime.front());
					time += 60.f * DXLib_refParts->GetDeltaTime() * (this->m_GunAnimeSpeed.at(index));
				}
				//
#if defined(DEBUG)
				auto* PlayerMngr = Player::PlayerManager::Instance();
				if (GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
					printfDx("[%s]\n", (GetGunAnime() == Charas::GunAnimeID::Base) ? "Base" : Charas::GunAnimeIDName[static_cast<int>(GetGunAnime())]);
					printfDx("[%f]\n", (GetGunAnime() == Charas::GunAnimeID::Base) ? 0.0f : GetNowAnimTimePerCache());
				}
#endif
			}
			//�e�̗h��
			if (!IsFirstLoop()) {
				Easing(&this->m_UpperRad, (this->m_UpperPrevRad - RotRad), 0.9f, EasingType::OutExpo);
			}
			this->m_UpperPrevRad = RotRad;
			Easing(&this->m_GunSwingMat, Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_UpperRad.x) * Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_UpperRad.y), 0.8f, EasingType::OutExpo);
			Easing(&this->m_GunSwingMat2, this->m_GunSwingMat, 0.8f, EasingType::OutExpo);
			//�X�����O
			Easing(&this->m_SlingPer, IsSelectGun ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			if (this->m_SlingPer <= 0.001f) { this->m_SlingPer = 0.f; }
			if (this->m_SlingPer >= 0.999f) { this->m_SlingPer = 1.f; }
			//
			auto EyeYRot = Matrix3x3DX::RotVec2(Lerp(GetMove().GetMat().yvec(), GetPartsFrameMatParent(GunFrame::Eyepos).yvec(), GetGunAnimBlendPer(Charas::GunAnimeID::ADS)), GetMove().GetMat().yvec());
			//�A�j�����Z�̌��ʂ��L���b�V��
			this->m_AnimNowCache = GetAnimDataNow(Charas::GunAnimeID::Aim);
			for (int loop = 0; loop < static_cast<int>(Charas::GunAnimeID::ChoiceOnceMax); ++loop) {
				if (GetGunAnimBlendPer((Charas::GunAnimeID)loop) != 0.f) {
					this->m_AnimNowCache.LerpNow(GetAnimDataNow((Charas::GunAnimeID)loop), GetGunAnimBlendPer((Charas::GunAnimeID)loop));
				}
			}
			//
			Matrix3x3DX AnimRot;
			Vector3DX AnimPos;
			if (this->m_SlingPer > 0.f) {
				AnimRot = GetGunAnimeNow().GetRot() * this->m_GunSwingMat2 * CharaRotationCache * EyeYRot;
				AnimPos = GetGunAnimeNow().GetPos();
				AnimPos.x *= GetSwitchPer();
				AnimPos = HeadPos + Matrix3x3DX::Vtrans(AnimPos, CharaRotationCache);
				//�I�[�g�G�C��
				if (IsSelectGun) {
					this->m_AutoAimControl.Update(IsActiveAutoAim, GetMyUserPlayerID(), GetPartsFrameMatParent(GunFrame::Eyepos).pos(), GetMove().GetMat().zvec() * -1.f, GetAutoAimRadian());
					this->m_AutoAimControl.CalcAutoAimMat(&AnimRot);
				}
			}
			//������W
			SetGunMat(Lerp(this->m_SlingRot * this->m_GunSwingMat2 * EyeYRot, AnimRot, this->m_SlingPer), Lerp(this->m_SlingPos, AnimPos, this->m_SlingPer));
			//�A�j���[�V�����ɉ��������낢��̍X�V
			{
				Matrix4x4DX MatMin;
				Matrix4x4DX MatMax;
				float BasePer = 0.f;
				float MaxPer = 0.f;

				auto SetMagMatLerp = [&](const Matrix4x4DX& MaxMat, float PerMax) {
					MatMin = MatMax;
					MatMax = MaxMat;
					BasePer = MaxPer;
					MaxPer = PerMax;
					if (BasePer <= GetNowAnimTimePerCache() && GetNowAnimTimePerCache() <= MaxPer) {
						if (this->m_MagazinePtr) {
							(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((GetNowAnimTimePerCache() - BasePer) / (MaxPer - BasePer), 0.f, 1.f)));
						}
					}
					};
				switch (GetGunAnime()) {
				case Charas::GunAnimeID::HighReady:
				case Charas::GunAnimeID::LowReady:
					this->m_MagHand = false;
					break;
				case Charas::GunAnimeID::Shot:
					if (GetShotType() == SHOTTYPE::FULL || GetShotType() == SHOTTYPE::SEMI) {
						if (GetNowAnimTimePerCache() >= 0.6f) {
							EjectCart();
						}
						else {
							this->m_IsEject = false;
						}
						if (GetNowAnimTimePerCache() >= 0.8f) {
							ChamberIn();
						}
						else {
							this->m_IsChamberOn = false;
						}
					}
					break;
				case Charas::GunAnimeID::Cocking:
					this->m_MagHand = false;
					this->m_CockHand = true;
					switch (GetShotType()) {
					case SHOTTYPE::BOLT:
					case SHOTTYPE::PUMP:
						if ((5.f / 35.f < GetNowAnimTimePerCache() && GetNowAnimTimePerCache() < 6.f / 35.f)) {
							PlayGunSound(EnumGunSound::CockingPull);
						}
						if ((28.f / 35.f < GetNowAnimTimePerCache() && GetNowAnimTimePerCache() < 29.f / 35.f)) {
							PlayGunSound(EnumGunSound::CockingRelease);
						}
						break;
					case SHOTTYPE::FULL:
					case SHOTTYPE::SEMI:
						if ((8.f / 35.f < GetNowAnimTimePerCache() && GetNowAnimTimePerCache() < 9.f / 35.f)) {
							PlayGunSound(EnumGunSound::CockingPull);
						}
						if ((18.f / 35.f < GetNowAnimTimePerCache() && GetNowAnimTimePerCache() < 19.f / 35.f)) {
							PlayGunSound(EnumGunSound::CockingRelease);
						}
						break;
					default:
						break;
					}
					if (GetNowAnimTimePerCache() >= 25.f / 35.f) {
						this->m_CockHand = false;
					}
					if (GetNowAnimTimePerCache() >= 19.f / 35.f) {
						ChamberIn();
						if (GetShotType() == SHOTTYPE::BOLT || GetShotType() == SHOTTYPE::PUMP) {
							EjectCart();
						}
					}
					else {
						this->m_IsChamberOn = false;
						this->m_IsEject = false;
					}
					break;
				case Charas::GunAnimeID::ReloadStart_Empty:
					if (GetShotType() == SHOTTYPE::PUMP) {
						MatMax = GetPartsFrameMatParent(GunFrame::Magpos);
						SetMagMatLerp(this->m_MagazinePoachMat, 15.f / 60.f);
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2), 25.f / 60.f);
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 35.f / 60.f);
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 48.f / 60.f);
						SetMagMatLerp(this->m_MagazinePoachMat, 1.0f);

						if (0.5f < GetNowAnimTimePerCache() && GetNowAnimTimePerCache() < 0.7f) {
							PlayGunSound(EnumGunSound::LoadMag);
							if (!this->m_IsChamberOn) {
								++this->m_Capacity;
							}
							ChamberIn();
						}
						else {
							this->m_EnumGunSoundNow = EnumGunSound::Max;
							this->m_IsChamberOn = false;
						}
						float Per = 35.f / 60.f;
						if (GetShotType() == SHOTTYPE::PUMP) {
							this->m_MagHand = true;
							if (Per <= GetNowAnimTimePerCache()) {
								this->m_MagHand = false;
							}
						}
					}
					else {
						if (this->m_MagazinePtr) {
							(*this->m_MagazinePtr)->SetHandMatrix(this->m_MagazinePoachMat);
						}
						this->m_MagHand = true;
						if (GetReloadType() == RELOADTYPE::MAG) {
							if (GetNowAnimTimePerCache() >= 0.5f) {
								if (!this->m_isMagFall) {
									this->m_isMagFall = true;
									auto MagposMat = GetPartsFrameMatParent(GunFrame::Magpos);
									this->m_MagFall.SetFall(
										MagposMat.pos(), Matrix3x3DX::Get33DX(MagposMat), MagposMat.yvec() * (Scale3DRate * -3.f / 60.f),
										12.f, Objects::FallObjectType::Magazine);
								}
							}
							else {
								this->m_isMagFall = false;
							}
							if (0.f < GetNowAnimTimePerCache()) {
								PlayGunSound(EnumGunSound::UnloadMag);
							}
						}
					}
					break;
				case Charas::GunAnimeID::ReloadStart:
					MatMax = GetPartsFrameMatParent(GunFrame::Magpos);
					SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 0.3f);
					SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2), 0.6f);
					SetMagMatLerp(this->m_MagazinePoachMat, 1.f);

					this->m_MagHand = true;
					this->m_ReloadAmmoCancel = false;

					switch (GetReloadType()) {
					case RELOADTYPE::MAG:
						if (0.3f <= GetNowAnimTimePerCache()) {
							PlayGunSound(EnumGunSound::UnloadMag);
						}
						break;
					case RELOADTYPE::AMMO:
						break;
					default:
						break;
					}
					break;
				case Charas::GunAnimeID::ReloadWait:
					this->m_MagHand = true;
					if (this->m_MagazinePtr) {
						(*this->m_MagazinePtr)->SetHandMatrix(this->m_MagazinePoachMat);
					}
					break;
				case Charas::GunAnimeID::Reload:
					this->m_MagHand = true;
					switch (GetReloadType()) {
					case RELOADTYPE::MAG:
						MatMax = this->m_MagazinePoachMat;
						SetMagMatLerp(this->m_MagazinePoachMat, 0.1f);
						if (GetNowAnimTimePerCache() <= 0.1f) {
							//�}�O�`�F���W�����m��
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
								SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2), 0.55f);
								SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 0.75f);
								SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 1.0f);
								if (GetNowAnimTimePerCache() >= 0.55f) {
									PlayGunSound(EnumGunSound::LoadMag);
								}
							}
							else {
								SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2) * this->m_MagMiss, 0.55f);
								SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2) * this->m_MagSuccess, 0.75f);
								SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 0.85f);
								SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 1.0f);
								if (GetNowAnimTimePerCache() >= 0.75f) {
									PlayGunSound(EnumGunSound::LoadMag);
								}
							}
						}
						break;
					case RELOADTYPE::AMMO:
						this->m_isMagSuccess = false;

						MatMax = this->m_MagazinePoachMat;
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2), 0.5f);
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2), 0.7f);
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 0.9f);
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 1.0f);

						if (0.5f < GetNowAnimTimePerCache()) {
							PlayGunSound(EnumGunSound::LoadMag);
						}
						else {
							this->m_EnumGunSoundNow = EnumGunSound::Max;
						}
						break;
					default:
						break;
					}
					break;
				case Charas::GunAnimeID::ReloadEnd:
					switch (GetReloadType()) {
					case RELOADTYPE::MAG:
						if (GetNowAnimTimePerCache() > 0.6f) {
							this->m_MagHand = false;
						}
						break;
					case RELOADTYPE::AMMO:
						this->m_MagHand = false;
						break;
					default:
						break;
					}
					break;
				case Charas::GunAnimeID::ThrowReady:
					if (GetNowAnimTimePerCache() >= 0.55f) {
						if (!this->m_ReleasePin) {
							this->m_ReleasePin = true;
							SE->Get(SoundType::SE, static_cast<int>(Sceneclass::SoundEnum::Pin))->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
						}
					}
					else {
						this->m_ReleasePin = false;
					}
					break;
				case Charas::GunAnimeID::Throw:
					if (GetNowAnimTimePerCache() >= 0.4f) {
						if (IsActive()) {
							SetActiveAll(false);//��ɂ�����͔̂�\���ɂ���
							SE->Get(SoundType::SE, static_cast<int>(Sceneclass::SoundEnum::Throw))->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
							this->m_Grenade.SetFall(GetMove().GetPos(), this->m_GrenadeThrowRot, (this->m_GrenadeThrowRot.zvec() * -1.f).normalized() * (Scale3DRate * 15.f / 60.f), 3.5f, Objects::FallObjectType::Grenade);
						}
					}
					break;
				default:
					break;
				}
			}
		}
		//�O���l�[�h�X�V
		void				GunObj::ExecuteGrenade(void) noexcept {
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();

			for (const auto& grenade : this->m_Grenade.GetPtrList()) {
				if (grenade->PopIsEndFall()) {
					for (int loop = 0, max = GetModifySlot()->GetMyData()->GetAmmoSpecMagTop()->GetPellet(); loop < max; ++loop) {
						auto LastAmmo = std::make_shared<Objects::AmmoObj>();
						ObjectManager::Instance()->InitObject(LastAmmo);
						//�~����ɂ܂��U�炷
						auto mat = Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-GetRand(30))) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180)));
						LastAmmo->Put(GetModifySlot()->GetMyData()->GetAmmoSpecMagTop(), grenade->GetMove().GetPos() + mat.zvec() * (0.5f * Scale3DRate) + Vector3DX::up() * (0.5f * Scale3DRate), mat.zvec(), GetMyUserPlayerID());
					}

					//�j��
					int								xput = 6;
					int								yput = 8;
					int								zput = 6;
					auto Put = BackGroundParts->GetPoint(grenade->GetMove().GetPos());
					for (int xp = -xput / 2; xp < xput / 2; ++xp) {
						for (int yp = 0; yp < yput; ++yp) {
							for (int zp = -zput / 2; zp < zput / 2; ++zp) {
								auto& cell = BackGroundParts->GetCellBuf((Put.x + xp), (Put.y + yp), (Put.z + zp));
								if (cell.m_Cell == 1) {
									continue;
								}
								BackGroundParts->SetBlick((Put.x + xp), (Put.y + yp), (Put.z + zp), FPS_n2::BackGround::s_EmptyBlick);
							}
						}
					}
				}
			}
		}
		//
		void				GunObj::Init_Sub(void) noexcept {
			this->m_ModifySlot = std::make_unique<ModifySlot>(GetFilePath());
			this->m_GunsModify = std::make_unique<GunsModify>(shared_from_this(), false);
			{
				//�����Ƃ��ēo�^����Ă���p�[�c�����X�g��
				std::vector<const SharedGunParts*> PartsList;
				GetModifySlot()->GetAnyByChild([&](const SharedGunParts& ptr) { if (ptr) { PartsList.emplace_back(&ptr); } });
				this->m_SightPtr = nullptr;
				this->m_MuzzlePtr = nullptr;
				this->m_UpperPtr = nullptr;
				this->m_LowerPtr = nullptr;
				this->m_MagazinePtr = nullptr;

				for (auto& parts : PartsList) {
					Sceneclass::ObjType ObjectType = static_cast<Sceneclass::ObjType>((*parts)->GetobjType());
					if (ObjectType == Sceneclass::ObjType::Sight) {
						this->m_SightPtr = &((std::shared_ptr<SightPartsObj>&)(*parts));
					}
					if (ObjectType == Sceneclass::ObjType::MuzzleAdapter) {
						this->m_MuzzlePtr = &((std::shared_ptr<MuzzlePartsObj>&)(*parts));
					}
					if (ObjectType == Sceneclass::ObjType::Upper) {
						this->m_UpperPtr = &((std::shared_ptr<UpperPartsObj>&)(*parts));
					}
					if (ObjectType == Sceneclass::ObjType::Lower) {
						this->m_LowerPtr = &((std::shared_ptr<LowerPartsObj>&)(*parts));
					}
					if (ObjectType == Sceneclass::ObjType::Magazine) {
						this->m_MagazinePtr = &((std::shared_ptr<MagazinePartsObj>&)(*parts));
					}
				}

				this->m_ShootRate_Diff = 0;
				this->m_Recoil_Diff = 0;
				for (auto& parts : PartsList) {
					//���\����
					auto& data = (*parts)->GetModifySlot()->GetMyData();
					this->m_ShootRate_Diff += data->GetShootRate_Diff();
					this->m_Recoil_Diff += data->GetRecoil_Diff();
				}
				PartsList.clear();
			}
			if (this->m_MagazinePtr) {
				this->m_MagFall.Init((*this->m_MagazinePtr)->GetFilePath(), 1);
				auto& AmmoSpec = (*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetAmmoSpecMagTop();
				this->m_CartFall.Init(AmmoSpec->GetPath(), 4);	//���U�����}�K�W���̒e�ɍ��킹�Ė�䰐���
				this->m_AmmoInChamberObj = std::make_shared<Objects::AmmoInChamberObj>();
				ObjectManager::Instance()->InitObject(this->m_AmmoInChamberObj, AmmoSpec->GetPath().c_str());
				(*this->m_MagazinePtr)->SetAmmoActive(GetReloadType()== RELOADTYPE::MAG);//�e���ߎ��Ȃ�}�K�W�������̒e�͕`�悵�Ȃ�
			}
			if (GetModifySlot()->GetMyData()->GetIsThrowWeapon()) {
				this->m_Grenade.Init(GetFilePath(), 1);
			}
			for (auto& speed : this->m_GunAnimeSpeed) {
				speed = 1.f;
			}
			this->m_GunAnimeSpeed.at(static_cast<int>(Charas::GunAnimeID::Shot)) = static_cast<float>(GetModifySlot()->GetMyData()->GetShotRate()) / 60.f / 10.f;
			InitGunAnimePer();
			SetMinAABB(Vector3DX::vget(-0.5f, -0.5f, -0.5f) * Scale3DRate);
			SetMaxAABB(Vector3DX::vget(0.5f, 0.5f, 0.5f) * Scale3DRate);

			auto BaseAnim = GetAnimDataNow(Charas::GunAnimeID::Aim);
			SetGunMat(BaseAnim.GetRot(), BaseAnim.GetPos());
			this->m_BaseMuzzle = GetPartsFrameMatParent(GunFrame::Muzzle);
		}
		void				GunObj::FirstExecute(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			if (IsFirstLoop()) {
				this->m_MuzzleSmokeControl.InitMuzzleSmoke(GetPartsFrameMatParent(GunFrame::Muzzle).pos());
			}
			else {
				this->m_MuzzleSmokeControl.ExecuteMuzzleSmoke(GetPartsFrameMatParent(GunFrame::Muzzle).pos(), GetGunAnime() != Charas::GunAnimeID::Shot && this->m_SlingPer >= 1.f);
			}
			//
			for (int loop = 0; loop < static_cast<int>(Charas::GunAnimeID::ChoiceOnceMax); ++loop) {
				int ID = GetModifySlot()->GetMyData()->GetAnimSelectList().at(loop);
				if (ID != InvalidID) {
					if (GetGunAnime() == (Charas::GunAnimeID)loop) {
						SetObj().SetAnim(ID).SetPer(1.f);
					}
					else {
						SetObj().SetAnim(ID).Reset();
					}
				}
			}
			for (int loop = static_cast<int>(Charas::GunAnimeID::ChoiceOnceMax); loop < static_cast<int>(Charas::GunAnimeID::Max); ++loop) {
				int ID = GetModifySlot()->GetMyData()->GetAnimSelectList().at(loop);
				if (ID != InvalidID) {
					switch ((Charas::GunAnimeID)loop) {
					case Charas::GunAnimeID::Hammer:
						SetObj().SetAnim(ID).SetPer(std::clamp(GetObj().GetAnim(ID).GetPer() + DXLib_refParts->GetDeltaTime() * (GetShotSwitch() ? -10.f : 10.f), 0.f, 1.f));
						break;
					case Charas::GunAnimeID::Open:
					{
						//�ˌ����̃{���g�A�j�����s�킹��
						switch (GetGunAnime()) {
						case Charas::GunAnimeID::Shot:
							if (GetShotType() == SHOTTYPE::FULL || GetShotType() == SHOTTYPE::SEMI) {
								float Per = 0.6f;
								if (GetNowAnimTimePerCache() < Per) {
									SetObj().SetAnim(ID).SetPer(std::clamp(GetNowAnimTimePerCache() / Per, 0.f, 1.f));
								}
								else {
									if (GetAmmoNumTotal() == 0) {
										SetObj().SetAnim(ID).SetPer(1.f);
									}
									else {
										SetObj().SetAnim(ID).SetPer(std::clamp(1.f - (GetNowAnimTimePerCache() - Per) / (1.f - Per), 0.f, 1.f));
									}
								}
							}
							else {
								SetObj().SetAnim(ID).SetPer(0.f);
							}
							break;
						case Charas::GunAnimeID::Cocking:
						{
							float Per = 19.f / 35.f;
							if (GetNowAnimTimePerCache() < Per) {
								//������J���Ă�����1�̂܂�
								SetObj().SetAnim(ID).SetPer(std::max(GetObj().GetAnim(ID).GetPer(), std::clamp(GetNowAnimTimePerCache() / Per, 0.f, 1.f)));
							}
							else {
								if (GetShotType() == SHOTTYPE::PUMP) {
									if (GetAmmoNumTotal() == 0) {
										SetObj().SetAnim(ID).SetPer(1.f);
									}
									else {
										SetObj().SetAnim(ID).SetPer(std::clamp(1.f - (GetNowAnimTimePerCache() - Per) / (1.f - Per), 0.f, 1.f));
									}
								}
								else {
									SetObj().SetAnim(ID).SetPer(std::clamp(1.f - (GetNowAnimTimePerCache() - Per) / (1.f - Per), 0.f, 1.f));
								}
							}
						}
							break;
						case Charas::GunAnimeID::ReloadStart_Empty:
						{
							float Per = 35.f / 60.f;
							if (GetShotType() == SHOTTYPE::PUMP) {
								if (Per <= GetNowAnimTimePerCache()) {
									SetObj().SetAnim(ID).SetPer(std::clamp(1.f - (GetNowAnimTimePerCache() - Per) / (1.f - Per), 0.f, 1.f));
								}
							}
						}
						break;
						default:
							break;
						}
					}
						break;
					default:
						break;
					}
				}
			}
			//
			this->m_MagArm.Update(this->m_MagHand, 0.1f, 0.1f, 0.7f, 0.7f);
			this->m_CockArm.Update(this->m_CockHand, 0.0f, 0.1f, 0.7f, 0.7f);
			SetObj().UpdateAnimAll();
			//���R�C���̉��Z
			if (this->m_RecoilRadAdd.y < 0.f) {
				Easing(&this->m_RecoilRadAdd, Vector2DX::vget(0.f, 0.01f), GetRecoilReturn(), EasingType::OutExpo);
			}
			else {
				Easing(&this->m_RecoilRadAdd, Vector2DX::zero(), 0.7f, EasingType::OutExpo);
			}
			//�O���l�[�h���Z
			ExecuteGrenade();
			//
			if (this->m_SightPtr && GetSightZoomSize() > 1.f) {
				float Per = 1.f - std::clamp(GetGunAnimBlendPer(Charas::GunAnimeID::ADS), 0.f, 1.f);
				if (Per < 1.f) {
					(*this->m_SightPtr)->SetScopeAlpha(Per);
				}
			}
		}
		void				GunObj::DrawShadow(void) noexcept {
			if (!IsActive()) { return; }
			GetObj().DrawModel();
		}
		void				GunObj::CheckDraw_Sub(int) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
				auto* PostPassParts = PostPassEffect::Instance();
				if (!GetCanShot()) {
					if (this->m_SlingPer >= 1.f) {
						this->m_Reticle_on = false;
						PostPassParts->Set_is_lens(false);
					}
					return;
				}
				this->m_AimPoint.Calc(GetPartsFrameMatParent(GunFrame::Muzzle).pos() + GetMove().GetMat().zvec() * (-50.f * Scale3DRate));
				Vector3DX LensPos = GetPartsFrameMatParent(GunFrame::Lens).pos();
				if (this->m_Lens.Calc(LensPos)) {
					if (this->m_LensSize.Calc(GetPartsFrameMatParent(GunFrame::LensSize).pos())) {
						this->m_LensSizeLen = static_cast<float>(std::hypot(this->m_Lens.XPos() - this->m_LensSize.XPos(), this->m_Lens.YPos() - this->m_LensSize.YPos()));
					}
					if (this->m_Reticle.Calc(LensPos + (LensPos - GetPartsFrameMatParent(GunFrame::Eyepos).pos()).normalized() * (5.f * Scale3DRate))) {
						this->m_Reticle_on = (this->m_LensSizeLen > std::hypot(this->m_Lens.XPos() - this->m_Reticle.XPos(), this->m_Lens.YPos() - this->m_Reticle.YPos()));
					}
				}
				//�����Y�\���@�\�ւ̔��f
				if (GetSightZoomSize() > 1.f) {
					if (this->m_Reticle_on) {
						PostPassParts->Set_is_lens(true);
						PostPassParts->Set_zoom_lens(std::max(1.f, GetSightZoomSize() / 2.f));
						PostPassParts->Set_xp_lens(static_cast<float>(this->m_Lens.XPos()));
						PostPassParts->Set_yp_lens(static_cast<float>(this->m_Lens.YPos()));
						PostPassParts->Set_size_lens(this->m_LensSizeLen);
					}
					else if (GetGunAnimBlendPer(Charas::GunAnimeID::ADS) < 0.5f) {
						PostPassParts->Set_is_lens(false);
						PostPassParts->Set_zoom_lens(1.f);
					}
				}
			}
		}
		void				GunObj::Draw(bool isDrawSemiTrans, int Range) noexcept {
			if (!IsActive()) { return; }
			if (!IsDraw(Range)) { return; }
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (isDrawSemiTrans && GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
				this->m_MuzzleSmokeControl.DrawMuzzleSmoke();
			}
			ObjectBaseClass::Draw(isDrawSemiTrans, Range);
		}
		void GunObj::Dispose_Sub(void) noexcept {
			this->m_GunsModify.reset();
			this->m_ModifySlot.reset();
			this->m_MagFall.Dispose();
			this->m_CartFall.Dispose();
			this->m_Grenade.Dispose();
			if (this->m_AmmoInChamberObj) {
				ObjectManager::Instance()->DelObj(this->m_AmmoInChamberObj);
				this->m_AmmoInChamberObj.reset();
			}
		}
	};
};
