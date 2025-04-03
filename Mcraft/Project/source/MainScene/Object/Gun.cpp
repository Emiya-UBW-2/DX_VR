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
		void				GunClass::UpdateGunAnimePer(bool IsADS) noexcept {
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
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.2f, 0.9f, 0.9f);
					break;
				case GunAnimeID::Shot:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.f, 0.f, 0.5f, 0.5f);
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
		//
		void				GunClass::SetShotStart(void) noexcept {
			if (GetModSlot().GetModData()->GetIsThrowWeapon()) {
				return;
			}
			if (!(this->m_ChamberAmmoData)) {
				return;
			}
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			PlayerMngr->GetPlayer(GetMyUserPlayerID())->AddShot(GetPelletNum());
			//
			SE->Get(SoundType::SE, (int)SoundEnum::Trigger)->Play3D(GetMove().GetPos(), Scale3DRate * 5.f);
			switch ((this->m_MuzzlePtr) ? (*this->m_MuzzlePtr)->GetModSlot().GetModData()->GetGunShootSound() : GetModSlot().GetModData()->GetGunShootSound()) {
			case GunShootSound::Normal:
				SE->Get(SoundType::SE, (int)GetGunSoundSet().m_ShotNormal)->Play3D(GetMove().GetPos(), Scale3DRate * 50.f);
				break;
			case GunShootSound::Suppressor:
				SE->Get(SoundType::SE, (int)GetGunSoundSet().m_ShotSuppressor)->Play3D(GetMove().GetPos(), Scale3DRate * 20.f);
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
			SetGunAnime(GunAnimeID::Shot);
			//リコイル
			float Power = 0.0001f * GetRecoilPower();
			this->m_RecoilRadAdd.Set(GetRandf(Power / 4.f), -Power);
			//ビジュアルリコイル
			if (GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
				Camera3D::Instance()->SetCamShake(0.1f, 0.1f);
			}
			//エフェクト
			EffectSingleton::Instance()->SetOnce_Any(Sceneclass::Effect::ef_fire2, GetFrameWorldMatParts(GunFrame::Muzzle).pos(), GetMove().GetMat().zvec() * -1.f, 0.35f, 2.f);
		}
		//
		void				GunClass::SetGunMat(const Matrix3x3DX& AnimRot, const Vector3DX& AnimPos) noexcept {
			//武器座標
			SetMove().SetMat(AnimRot);
			SetMove().SetPos(AnimPos);
			SetMove().Update(0.f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			SetModSlot().UpdatePartsAnim(GetObj());
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::UnderRail), GunSlot::UnderRail);
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Lower), GunSlot::Lower);
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Upper), GunSlot::Upper);
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Barrel), GunSlot::Barrel);
			SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Sight), GunSlot::Sight);
		}
		void				GunClass::SetMagMat(bool IsNeedGunAnim) noexcept {
			if (!this->m_MagazinePtr) {
				return;
			}
			if (!IsNeedGunAnim) {
				SetModSlot().UpdatePartsMove(GetFrameWorldMatParts(GunFrame::Magpos), GunSlot::Magazine);
				return;
			}
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
		//
		void				GunClass::UpdateGunAnime(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			//アニメーション
			if (GetNowGunAnimeID() != -1) {
				SetAnimOnce(GetNowGunAnimeID(), this->m_GunAnimeSpeed.at(static_cast<int>(GetGunAnime())));
			}
			switch (GetGunAnime()) {
			case GunAnimeID::LowReady:
				this->m_MagHand = false;
				break;
			case GunAnimeID::Shot:
				if (GetShotType() != SHOTTYPE::BOLT) {
					if (GetNowGunAnimePer() >= 0.6f) {
						if (!this->m_IsChamberOn) {
							this->m_IsChamberOn = true;
							ChamberIn();
						}
						if (!this->m_IsEject) {
							this->m_IsEject = true;
							EjectCart();
						}
					}
					else {
						this->m_IsChamberOn = false;
						this->m_IsEject = false;
					}
				}
				if (GetNowGunAnimePer() >= 1.f && this->m_ShotEnd) {
					this->m_ShotEnd = false;
					if (this->m_Capacity != 0) {
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
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((5.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 6.f)) {
						if (this->m_boltSoundSequence != 1) {
							this->m_boltSoundSequence = 1;
							SE->Get(SoundType::SE, static_cast<int>(GetGunSoundSet().m_Cock1))->Play3D(GetMove().GetPos(), Scale3DRate * 50.f);
						}
					}
					if ((28.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 29.f)) {
						if (this->m_boltSoundSequence != 2) {
							this->m_boltSoundSequence = 2;
							SE->Get(SoundType::SE, static_cast<int>(GetGunSoundSet().m_Cock2))->Play3D(GetMove().GetPos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((8.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 9.f)) {
						if (this->m_boltSoundSequence != 3) {
							this->m_boltSoundSequence = 3;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
						}
					}
					if ((18.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 19.f)) {
						if (this->m_boltSoundSequence != 4) {
							this->m_boltSoundSequence = 4;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				if (GetNowGunAnimeTime() >= 19.f) {
					if (!this->m_IsChamberOn) {
						this->m_IsChamberOn = true;
						ChamberIn();
					}
					if (GetShotType() == SHOTTYPE::BOLT) {
						if (!this->m_IsEject) {
							this->m_IsEject = true;
							EjectCart();
						}
					}
				}
				else {
					this->m_IsChamberOn = false;
					this->m_IsEject = false;
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
								12.f, FallObjectType::Magazine);
						}
					}
					else {
						this->m_isMagFall = false;
					}
				}
				//
				this->m_MagHand = true;
				switch (GetShotType()) {
				case SHOTTYPE::BOLT:
					if ((5.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 6.f)) {
						if (this->m_boltSoundSequence != 5) {
							this->m_boltSoundSequence = 5;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetMove().GetPos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 1.f)) {
						if (this->m_boltSoundSequence != 6) {
							this->m_boltSoundSequence = 6;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
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
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetMove().GetPos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 1.f)) {
						if (this->m_boltSoundSequence != 6) {
							this->m_boltSoundSequence = 6;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Unload)->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
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
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock1)->Play3D(GetMove().GetPos(), Scale3DRate * 50.f);
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
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Cock2)->Play3D(GetMove().GetPos(), Scale3DRate * 50.f);
						}
					}
					break;
				case SHOTTYPE::FULL:
				case SHOTTYPE::SEMI:
					if ((0.f < GetNowGunAnimeTime() && GetNowGunAnimeTime() < 1.f)) {
						if (this->m_boltSoundSequence != 10) {
							this->m_boltSoundSequence = 10;
							SE->Get(SoundType::SE, (int)GetGunSoundSet().m_Load)->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
						}
					}
					break;
				default:
					break;
				}
				switch (GetReloadType()) {
				case RELOADTYPE::MAG:
					if (GetNowGunAnimePer() > 0.6f) {
						this->m_MagHand = false;
					}
					break;
				case RELOADTYPE::AMMO:
					this->m_MagHand = false;
					break;
				default:
					break;
				}
				if (GetNowGunAnimePer() >= 1.f) {
					//チャンバーに弾がないがマガジンには弾がある場合
					if (!(this->m_ChamberAmmoData) && (this->m_Capacity != 0)) {
						SetGunAnime(GunAnimeID::Cocking);
					}
					else {
						SetGunAnime(GunAnimeID::None);
					}
				}
				break;
			case GunAnimeID::Watch:
				if (GetNowGunAnimePer() >= 1.f) {
					SetGunAnime(GunAnimeID::None);
				}
				break;
			case GunAnimeID::Throw:
				if (GetNowGunAnimePer() >= 0.4f) {
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
			//
#if defined(DEBUG)
			if (GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
				printfDx("[%s]\n", (GetGunAnime() == GunAnimeID::None) ? "None" : GunAnimeIDName[(int)GetGunAnime()]);
				printfDx("[%f]\n", (GetGunAnime() == GunAnimeID::None) ? 0.0f : GetNowGunAnimeTime());
				printfDx("[%f]\n", GetGunAnimBlendPer(GunAnimeID::LowReady));
			}
#endif
		}
		//グレネード更新
		void				GunClass::ExecuteGrenade(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();

			for (const auto& g : this->m_Grenade.GetPtrList()) {
				if (g->PopGrenadeBombSwitch()) {
					for (int i = 0, max = this->m_ChamberAmmoData->GetPellet(); i < max; i++) {
						auto LastAmmo = std::make_shared<AmmoClass>();
						ObjMngr->AddObject(LastAmmo);
						LastAmmo->Init();
						auto mat =
							Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-GetRand(30))) *
							Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180)));
						LastAmmo->Put(this->m_ChamberAmmoData, g->GetMove().GetPos() + mat.zvec() * (0.5f * Scale3DRate) + Vector3DX::up() * (0.5f * Scale3DRate), mat.zvec(), GetMyUserPlayerID());
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
			}
			if (GetModSlot().GetModData()->GetIsThrowWeapon()) {
				this->m_Grenade.Init(GetFilePath(), 1);
			}

			for (auto& g : this->m_GunAnimeSpeed) {
				g = 1.f;
			}
			this->m_GunAnimeSpeed.at(static_cast<int>(GunAnimeID::Shot)) = ((float)GetModSlot().GetModData()->GetShotRate()) / 300.f;
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
				this->m_MuzzleSmokeControl.ExecuteMuzzleSmoke(GetFrameWorldMatParts(GunFrame::Muzzle).pos(), GetGunAnime() != GunAnimeID::Shot);
			}
			//
			for (int i = 0; i < static_cast<int>(GunAnimeID::ChoiceOnceMax); i++) {
				int ID = GetModSlot().GetModData()->GetAnimSelectList().at(i);
				if (ID != -1) {
					if (GetGunAnime() == (GunAnimeID)i) {
						SetObj().SetAnim(ID).SetPer(1.f);
					}
					else {
						SetObj().SetAnim(ID).Reset();
					}
				}
			}
			for (int i = static_cast<int>(GunAnimeID::ChoiceOnceMax); i < static_cast<int>(GunAnimeID::Max); i++) {
				int ID = GetModSlot().GetModData()->GetAnimSelectList().at(i);
				if (ID != -1) {
					switch ((GunAnimeID)i) {
					case FPS_n2::Sceneclass::GunAnimeID::Hammer:
						SetObj().SetAnim(ID).SetPer(std::clamp(GetObj().GetAnim(ID).GetPer() + DXLib_refParts->GetDeltaTime() * (GetShotSwitch() ? -10.f : 10.f), 0.f, 1.f));
						break;
					case FPS_n2::Sceneclass::GunAnimeID::Open:
						SetObj().SetAnim(ID).SetPer(std::clamp(GetObj().GetAnim(ID).GetPer() + DXLib_refParts->GetDeltaTime() * (

							((GetAmmoNumTotal() == 0) || ((this->m_Capacity == GetAmmoAll()) && !(this->m_ChamberAmmoData)) || (GetGunAnime() == GunAnimeID::Cocking && (GetNowGunAnimeTime() <= 22.f)))

							? 10.f : -10.f), 0.f, 1.f));
						break;
					default:
						break;
					}
				}
			}
			//
			this->m_MagArm.Update(this->m_MagHand, 0.1f, 0.1f, 0.7f, 0.7f);
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

			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<Sceneclass::CharacterClass>&)PlayerMngr->GetPlayer(GetMyUserPlayerID())->GetChara();
			if (Chara->GetLengthToPlayer() > 10.f * Scale3DRate) { return; }

			GetObj().DrawModel();
		}
		void				GunClass::CheckDraw_Sub(int) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
				auto* PostPassParts = PostPassEffect::Instance();
				if (!GetCanShot()) {
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
			auto& Chara = (std::shared_ptr<Sceneclass::CharacterClass>&)PlayerMngr->GetPlayer(GetMyUserPlayerID())->GetChara();
			if (Chara->GetLengthToPlayer() > 10.f * Scale3DRate) { return; }

			if (isDrawSemiTrans && GetMyUserPlayerID() == PlayerMngr->GetWatchPlayer()) {
				this->m_MuzzleSmokeControl.DrawMuzzleSmoke();
			}
			ObjectBaseClass::Draw(isDrawSemiTrans, Range);
		}
	};
};
