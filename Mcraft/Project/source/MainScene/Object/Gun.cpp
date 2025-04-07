#include	"Gun.hpp"

#include "../../MainScene/Player/Player.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"
#include "../../CommonScene/Object/GunsModify.hpp"
#include "Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void				GunClass::InitGunAnimePer() noexcept {
			for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
				bool IsActiveGunAnim = GetGunAnime() == static_cast<GunAnimeID>(loop);
				if (static_cast<GunAnimeID>(loop) == GunAnimeID::ADS) {
					IsActiveGunAnim = false;// IsADS;
				}
				this->m_GunAnimePer[loop].Init(IsActiveGunAnim);
			}
		}
		void				GunClass::UpdateGunAnimePer(bool IsADS) noexcept {
			for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
				bool IsActiveGunAnim = GetGunAnime() == static_cast<GunAnimeID>(loop);
				if (static_cast<GunAnimeID>(loop) == GunAnimeID::ADS) {
					IsActiveGunAnim = IsADS;
				}
#if TRUE
				switch (static_cast<GunAnimeID>(loop)) {
				case GunAnimeID::LowReady:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.f, 0.87f, 0.87f);
					break;
				case GunAnimeID::ADS:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.2f, 0.9f, 0.9f);
					break;
				case GunAnimeID::Cocking:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.f, 0.1f, 0.9f);
					break;
				case GunAnimeID::ReloadStart_Empty:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.5f, 0.0f, 0.9f, 0.0f);
					break;
				case GunAnimeID::ReloadStart:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.2f, 0.0f, 0.9f, 0.0f);
					break;
				case GunAnimeID::ReloadWait:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.0f, 0.0f, 0.0f, 0.0f);
					break;
				case GunAnimeID::Reload:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.0f, 0.0f, 0.0f, 0.0f);
					break;
				case GunAnimeID::ReloadEnd:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.0f, 0.2f, 0.0f, 0.9f);
					break;
				case GunAnimeID::Watch:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.1f, 0.1f, 0.9f, 0.9f);
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
		//
		void				GunClass::SetShotStart(void) noexcept {
			if (!this->m_ChamberAmmoData) { return; }
			if (GetModSlot().GetModData()->GetIsThrowWeapon()) { return; }
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//このプレイヤーはペレット発撃ちましたよ
			PlayerMngr->GetPlayer(GetMyUserPlayerID())->AddShot(GetPelletNum());
			auto MuzzleMat = GetFrameWorldMatParts(GunFrame::Muzzle);
			//銃声
			SE->Get(SoundType::SE, GetGunSoundSet(
				(this->m_MuzzlePtr) ? (*this->m_MuzzlePtr)->GetModSlot().GetModData()->GetGunShootSound() : GetModSlot().GetModData()->GetGunShootSound()
			))->Play3D(MuzzleMat.pos(), Scale3DRate * 50.f);
			//エフェクト
			EffectSingleton::Instance()->SetOnce_Any(Sceneclass::Effect::ef_fire2, MuzzleMat.pos(), MuzzleMat.zvec() * -1.f, 0.35f, 2.f);
			//発砲
			for (int loop = 0, max = GetPelletNum(); loop < max; loop++) {
				auto LastAmmo = std::make_shared<AmmoClass>();
				ObjMngr->AddObject(LastAmmo);
				LastAmmo->Init();
				LastAmmo->Put(this->m_ChamberAmmoData, MuzzleMat.pos(),
					(
						Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) *
						Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(GetRandf(this->m_ChamberAmmoData->GetAccuracy()))) *
						Matrix3x3DX::Get33DX(MuzzleMat)
						).zvec() * -1.f, GetMyUserPlayerID());
			}
			//チャンバーを空にする
			this->m_ChamberAmmoData.reset();
			//銃口煙の追加
			this->m_MuzzleSmokeControl.AddMuzzleSmokePower();
			//撃ちアニメに移行
			SetGunAnime(GunAnimeID::Shot);
			this->m_AmmoInChamberClass->SetActive(true);
			//リコイル
			float Power = 0.0001f * GetRecoilPower();
			this->m_RecoilRadAdd.Set(GetRandf(Power / 4.f), -Power);
			//ビジュアルリコイル
			if (GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
				Camera3D::Instance()->SetCamShake(0.1f, 0.1f);
			}
		}
		//
		void				GunClass::UpdateGunMat(bool IsSelGun, bool IsActiveAutoAim, const Matrix3x3DX& CharaRotationCache, const Vector3DX& HeadPos, const Vector3DX& RotRad) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			if (IsSelGun) {
				//アニメーション
				if (GetNowGunAnimeID() != -1) {
					SetAnimOnce(GetNowGunAnimeID(), this->m_GunAnimeSpeed.at(static_cast<int>(GetGunAnime())));
				}
				switch (GetGunAnime()) {
				case GunAnimeID::Base:
					if (!this->m_ChamberAmmoData && this->m_Capacity != 0) {
						SetGunAnime(GunAnimeID::Cocking);//チャンバーが空で弾倉が空でないなら
					}
					break;
				case GunAnimeID::Shot:
					if (IsNowGunAnimeEnd()) {
						if (this->m_ShotEnd) {
							this->m_ShotEnd = false;
							SetGunAnime(GunAnimeID::Base);
						}
					}
					break;
				case GunAnimeID::Cocking:
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(GunAnimeID::Base);
					}
					break;
				case GunAnimeID::ReloadStart_Empty:
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(GunAnimeID::ReloadWait);
					}
					break;
				case GunAnimeID::ReloadStart:
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(GunAnimeID::ReloadWait);
					}
					break;
				case GunAnimeID::ReloadWait:
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(GunAnimeID::Reload);
					}
					break;
				case GunAnimeID::Reload:
					if (GetNowAnimTimePerCache() >= (this->m_isMagSuccess ? 0.75f : 1.0f)) {
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
								SetGunAnime(GunAnimeID::ReloadWait);
							}
							break;
						default:
							break;
						}
					}
					break;
				case GunAnimeID::ReloadEnd:
					if (!this->m_ChamberAmmoData) {
						SetGunAnime(GunAnimeID::Cocking);
					}
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(GunAnimeID::Base);
					}
					break;
				case GunAnimeID::Watch:
					if (IsNowGunAnimeEnd()) {
						SetGunAnime(GunAnimeID::Base);
					}
					break;
				default:
					break;
				}
				for (auto& g : this->m_GunAnimeTime) {
					int index = static_cast<int>(&g - &this->m_GunAnimeTime.front());
					g += 60.f * DXLib_refParts->GetDeltaTime() * (this->m_GunAnimeSpeed.at(index));
				}
				//
#if defined(DEBUG)
				auto* PlayerMngr = Player::PlayerManager::Instance();
				if (GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
					printfDx("[%s]\n", (GetGunAnime() == GunAnimeID::Base) ? "Base" : GunAnimeIDName[(int)GetGunAnime()]);
					printfDx("[%f]\n", (GetGunAnime() == GunAnimeID::Base) ? 0.0f : GetNowAnimTimePerCache());
				}
#endif
			}
			//銃の揺れ
			if (!IsFirstLoop()) {
				Easing(&this->m_UpperRad, (this->m_UpperPrevRad - RotRad), 0.9f, EasingType::OutExpo);
			}
			this->m_UpperPrevRad = RotRad;
			Easing(&this->m_GunSwingMat, Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_UpperRad.x) * Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_UpperRad.y), 0.8f, EasingType::OutExpo);
			Easing(&this->m_GunSwingMat2, this->m_GunSwingMat, 0.8f, EasingType::OutExpo);
			//スリング
			Easing(&this->m_SlingPer, IsSelGun ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			if (this->m_SlingPer <= 0.001f) { this->m_SlingPer = 0.f; }
			if (this->m_SlingPer >= 0.999f) { this->m_SlingPer = 1.f; }
			//
			auto EyeYRot = Matrix3x3DX::RotVec2(Lerp(GetMove().GetMat().yvec(), GetADSEyeMat().yvec(), GetGunAnimBlendPer(GunAnimeID::ADS)), GetMove().GetMat().yvec());
			//アニメ演算の結果をキャッシュ
			m_AnimNowCache = GetAnimDataNow(GunAnimeID::Aim);
			for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); ++loop) {
				if (this->m_GunAnimePer[loop].Per() != 0.f) {
					m_AnimNowCache.LerpNow(GetAnimDataNow((GunAnimeID)loop), this->m_GunAnimePer[loop].Per());
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
				//オートエイム
				if (IsSelGun) {
					this->m_AutoAimControl.Update(IsActiveAutoAim, GetMyUserPlayerID(), GetADSEyeMat().pos(), GetMove().GetMat().zvec() * -1.f, GetAutoAimRadian());
					this->m_AutoAimControl.CalcAutoAimMat(&AnimRot);
				}
			}
			//武器座標
			SetGunMat(Lerp(this->m_SlingRot * this->m_GunSwingMat2 * EyeYRot, AnimRot, this->m_SlingPer), Lerp(this->m_SlingPos, AnimPos, this->m_SlingPer));
			//アニメーションに応じたいろいろの更新
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
				case GunAnimeID::LowReady:
					this->m_MagHand = false;
					break;
				case GunAnimeID::Shot:
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
				case GunAnimeID::Cocking:
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
				case GunAnimeID::ReloadStart_Empty:
					if (GetShotType() == SHOTTYPE::PUMP) {
						MatMax = GetFrameWorldMatParts(GunFrame::Magpos);
						SetMagMatLerp(this->m_MagazinePoachMat, 15.f / 60.f);
						SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Mag2), 25.f / 60.f);
						SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Magpos), 35.f / 60.f);
						SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Magpos), 48.f / 60.f);
						SetMagMatLerp(this->m_MagazinePoachMat, 1.0f);

						if (0.5f < GetNowAnimTimePerCache() && GetNowAnimTimePerCache() < 0.7f) {
							PlayGunSound(EnumGunSound::LoadMag);
							if (!this->m_IsChamberOn) {
								this->m_Capacity++;
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
									auto MagposMat = GetFrameWorldMatParts(GunFrame::Magpos);
									this->m_MagFall.SetFall(
										MagposMat.pos(), Matrix3x3DX::Get33DX(MagposMat), MagposMat.yvec() * (Scale3DRate * -3.f / 60.f),
										12.f, FallObjectType::Magazine);
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
				case GunAnimeID::ReloadStart:
					MatMax = GetFrameWorldMatParts(GunFrame::Magpos);
					SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Magpos), 0.3f);
					SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Mag2), 0.6f);
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
				case GunAnimeID::ReloadWait:
					this->m_MagHand = true;
					if (this->m_MagazinePtr) {
						(*this->m_MagazinePtr)->SetHandMatrix(this->m_MagazinePoachMat);
					}
					break;
				case GunAnimeID::Reload:
					this->m_MagHand = true;
					switch (GetReloadType()) {
					case RELOADTYPE::MAG:
						MatMax = this->m_MagazinePoachMat;
						SetMagMatLerp(this->m_MagazinePoachMat, 0.1f);
						if (GetNowAnimTimePerCache() <= 0.1f) {
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
								SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Mag2), 0.55f);
								SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Magpos), 0.75f);
								SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Magpos), 1.0f);
								if (GetNowAnimTimePerCache() >= 0.55f) {
									PlayGunSound(EnumGunSound::LoadMag);
								}
							}
							else {
								SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Mag2) * this->m_MagMiss, 0.55f);
								SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Mag2) * this->m_MagSuccess, 0.75f);
								SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Magpos), 0.85f);
								SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Magpos), 1.0f);
								if (GetNowAnimTimePerCache() >= 0.75f) {
									PlayGunSound(EnumGunSound::LoadMag);
								}
							}
						}
						break;
					case RELOADTYPE::AMMO:
						this->m_isMagSuccess = false;

						MatMax = this->m_MagazinePoachMat;
						SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Mag2), 0.5f);
						SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Mag2), 0.7f);
						SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Magpos), 0.9f);
						SetMagMatLerp(GetFrameWorldMatParts(GunFrame::Magpos), 1.0f);

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
				case GunAnimeID::ReloadEnd:
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
				case GunAnimeID::ThrowReady:
					if (GetNowAnimTimePerCache() >= 0.55f) {
						if (!m_ReleasePin) {
							m_ReleasePin = true;
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Pin))->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
						}
					}
					else {
						m_ReleasePin = false;
					}
					break;
				case GunAnimeID::Throw:
					if (GetNowAnimTimePerCache() >= 0.4f) {
						if (IsActive()) {
							SetActiveAll(false);//手にあるものは非表示にする
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Throw))->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
							this->m_Grenade.SetFall(GetMove().GetPos(), this->m_GrenadeThrowRot, (this->m_GrenadeThrowRot.zvec() * -1.f).normalized() * (Scale3DRate * 15.f / 60.f), 3.5f, FallObjectType::Grenade);
						}
					}
					break;
				default:
					break;
				}
			}
		}
		//グレネード更新
		void				GunClass::ExecuteGrenade(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();

			for (const auto& g : this->m_Grenade.GetPtrList()) {
				if (g->PopIsEndFall()) {
					for (int loop = 0, max = GetModSlot().GetModData()->GetAmmoSpecMagTop()->GetPellet(); loop < max; loop++) {
						auto LastAmmo = std::make_shared<AmmoClass>();
						ObjMngr->AddObject(LastAmmo);
						LastAmmo->Init();
						//円周上にまき散らす
						auto mat = Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-GetRand(30))) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180)));
						LastAmmo->Put(GetModSlot().GetModData()->GetAmmoSpecMagTop(), g->GetMove().GetPos() + mat.zvec() * (0.5f * Scale3DRate) + Vector3DX::up() * (0.5f * Scale3DRate), mat.zvec(), GetMyUserPlayerID());
					}

					//破壊
					int								xput = 6;
					int								yput = 8;
					int								zput = 6;
					auto Put = BackGround->GetPoint(g->GetMove().GetPos());
					for (int xp = -xput / 2; xp < xput / 2; xp++) {
						for (int yp = 0; yp < yput; yp++) {
							for (int zp = -zput / 2; zp < zput / 2; zp++) {
								auto& cell = BackGround->GetCellBuf((Put.x + xp), (Put.y + yp), (Put.z + zp));
								if (cell.m_Cell == 1) {
									continue;
								}
								BackGround->SetBlick((Put.x + xp), (Put.y + yp), (Put.z + zp), FPS_n2::BackGround::s_EmptyBlick);
							}
						}
					}
				}
			}
		}
		//
		void				GunClass::Init_Sub(void) noexcept {
			SetModSlot().Init(GetFilePath());
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
				auto* ObjMngr = ObjectManager::Instance();
				this->m_AmmoInChamberClass = std::make_shared<AmmoInChamberClass>();
				ObjMngr->AddObject(this->m_AmmoInChamberClass);
				ObjMngr->LoadModel(this->m_AmmoInChamberClass, this->m_AmmoInChamberClass, (*this->m_MagazinePtr)->GetModSlot().GetModData()->GetAmmoSpecMagTop()->GetPath().c_str());
				this->m_AmmoInChamberClass->Init();
			}
			if (GetModSlot().GetModData()->GetIsThrowWeapon()) {
				this->m_Grenade.Init(GetFilePath(), 1);
			}
			for (auto& g : this->m_GunAnimeSpeed) {
				g = 1.f;
			}
			this->m_GunAnimeSpeed.at(static_cast<int>(GunAnimeID::Shot)) = ((float)GetModSlot().GetModData()->GetShotRate()) / 60.f / 10.f;
			InitGunAnimePer();
			ObjectBaseClass::SetMinAABB(Vector3DX::vget(-0.5f, -0.5f, -0.5f) * Scale3DRate);
			ObjectBaseClass::SetMaxAABB(Vector3DX::vget(0.5f, 0.5f, 0.5f) * Scale3DRate);
		}
		void				GunClass::FirstExecute(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			if (IsFirstLoop()) {
				this->m_MuzzleSmokeControl.InitMuzzleSmoke(GetFrameWorldMatParts(GunFrame::Muzzle).pos());
			}
			else {
				this->m_MuzzleSmokeControl.ExecuteMuzzleSmoke(GetFrameWorldMatParts(GunFrame::Muzzle).pos(), GetGunAnime() != GunAnimeID::Shot && m_SlingPer >= 1.f);
			}
			//
			for (int loop = 0; loop < static_cast<int>(GunAnimeID::ChoiceOnceMax); loop++) {
				int ID = GetModSlot().GetModData()->GetAnimSelectList().at(loop);
				if (ID != -1) {
					if (GetGunAnime() == (GunAnimeID)loop) {
						SetObj().SetAnim(ID).SetPer(1.f);
					}
					else {
						SetObj().SetAnim(ID).Reset();
					}
				}
			}
			for (int loop = static_cast<int>(GunAnimeID::ChoiceOnceMax); loop < static_cast<int>(GunAnimeID::Max); loop++) {
				int ID = GetModSlot().GetModData()->GetAnimSelectList().at(loop);
				if (ID != -1) {
					switch ((GunAnimeID)loop) {
					case GunAnimeID::Hammer:
						SetObj().SetAnim(ID).SetPer(std::clamp(GetObj().GetAnim(ID).GetPer() + DXLib_refParts->GetDeltaTime() * (GetShotSwitch() ? -10.f : 10.f), 0.f, 1.f));
						break;
					case GunAnimeID::Open:
					{
						//射撃時のボルトアニメを行わせる
						switch (GetGunAnime()) {
						case GunAnimeID::Shot:
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
						case GunAnimeID::Cocking:
						{
							float Per = 19.f / 35.f;
							if (GetNowAnimTimePerCache() < Per) {
								//元から開いていたら1のまま
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
						case GunAnimeID::ReloadStart_Empty:
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
			//リコイルの演算
			if (this->m_RecoilRadAdd.y < 0.f) {
				Easing(&this->m_RecoilRadAdd, Vector2DX::vget(0.f, 0.01f), GetRecoilReturn(), EasingType::OutExpo);
			}
			else {
				Easing(&this->m_RecoilRadAdd, Vector2DX::zero(), 0.7f, EasingType::OutExpo);
			}
			//グレネード演算
			ExecuteGrenade();
		}
		void				GunClass::DrawShadow(void) noexcept {
			if (!IsActive()) { return; }
			GetObj().DrawModel();
		}
		void				GunClass::CheckDraw_Sub(int) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
				auto* PostPassParts = PostPassEffect::Instance();
				if (!GetCanShot()) {
					PostPassParts->Set_is_lens(false);
					return;
				}
				this->m_AimPoint.Calc(GetFrameWorldMatParts(GunFrame::Muzzle).pos() + GetMove().GetMat().zvec() * (-50.f * Scale3DRate));
				Vector3DX LensPos;
				Vector3DX LensSizeFrame;
				if (this->m_SightPtr) {//サイトがあるならそれを最優先とする
					LensPos = (*this->m_SightPtr)->GetFramePartsMat(GunFrame::Lens).pos();
					LensSizeFrame = (*this->m_SightPtr)->GetFramePartsMat(GunFrame::LensSize).pos();
				}
				else {
					LensPos = GetFrameWorldMatParts(GunFrame::Lens).pos();
					LensSizeFrame = GetFrameWorldMatParts(GunFrame::LensSize).pos();
				}
				if (this->m_Lens.Calc(LensPos)) {
					if (this->m_LensSize.Calc(LensSizeFrame)) {
						this->m_LensSizeLen = static_cast<float>(std::hypot(this->m_Lens.XPos() - this->m_LensSize.XPos(), this->m_Lens.YPos() - this->m_LensSize.YPos()));
					}
					if (this->m_Reticle.Calc(LensPos + (LensPos - GetADSEyeMat().pos()).normalized() * (5.f * Scale3DRate))) {
						this->m_Reticle_on = (this->m_LensSizeLen > std::hypot(this->m_Lens.XPos() - this->m_Reticle.XPos(), this->m_Lens.YPos() - this->m_Reticle.YPos()));
					}
				}
				//レンズ表示機能への反映
				if (GetSightZoomSize() > 1.f) {
					if (this->m_Reticle_on) {
						PostPassParts->Set_is_lens(true);
						PostPassParts->Set_zoom_lens(std::max(1.f, GetSightZoomSize() / 2.f));
						PostPassParts->Set_xp_lens(static_cast<float>(this->m_Lens.XPos()));
						PostPassParts->Set_yp_lens(static_cast<float>(this->m_Lens.YPos()));
						PostPassParts->Set_size_lens(this->m_LensSizeLen);
					}
					else if (GetGunAnimBlendPer(GunAnimeID::ADS) < 0.5f) {
						PostPassParts->Set_is_lens(false);
						PostPassParts->Set_zoom_lens(1.f);
					}
				}
			}
		}
		void				GunClass::Draw(bool isDrawSemiTrans, int Range) noexcept {
			if (!IsActive()) { return; }
			if (!IsDraw(Range)) { return; }
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (isDrawSemiTrans && GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
				this->m_MuzzleSmokeControl.DrawMuzzleSmoke();
			}
			ObjectBaseClass::Draw(isDrawSemiTrans, Range);
		}
	};
};
