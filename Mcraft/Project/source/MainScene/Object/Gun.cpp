﻿#include	"Gun.hpp"

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
			this->m_AimSpeedBase = 0.f;
		}
		void				GunObj::UpdateGunAnimePer(bool IsADS) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();

			for (int loop = 0; loop < static_cast<int>(Charas::GunAnimeID::ChoiceOnceMax); ++loop) {
				bool IsActiveGunAnim = GetGunAnime() == static_cast<Charas::GunAnimeID>(loop);
				if (static_cast<Charas::GunAnimeID>(loop) == Charas::GunAnimeID::ADS) {
					IsActiveGunAnim = IsADS;
				}
#if true
				switch (static_cast<Charas::GunAnimeID>(loop)) {
				case Charas::GunAnimeID::HighReady:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.0f, 0.0f, 0.87f, 0.87f);
					break;
				case Charas::GunAnimeID::LowReady:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.0f, 0.0f, 0.87f, 0.87f);
					break;
				case Charas::GunAnimeID::EmergencyReady:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.0f, 0.0f, 0.87f, 0.87f);
					break;
				case Charas::GunAnimeID::ADS:
				{
					if (IsActiveGunAnim) {
						this->m_AimSpeedBase = std::clamp(this->m_AimSpeedBase + DXLib_refParts->GetDeltaTime() / 0.3f * GetAimSpeed(), 0.f, 1.f);
					}else{
						this->m_AimSpeedBase = std::clamp(this->m_AimSpeedBase - DXLib_refParts->GetDeltaTime() / 0.3f * GetAimSpeed(), 0.f, 1.f);
					}
					Easing(&this->m_GunAnimePer[loop].m_ArmPer, this->m_AimSpeedBase, 0.8f, EasingType::OutExpo);
				}
					break;
				case Charas::GunAnimeID::Cocking:
					this->m_GunAnimePer[loop].Update(IsActiveGunAnim, 0.0f, 0.0f, 0.1f, 0.9f);
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
			//このプレイヤーはペレット発撃ちましたよ
			const auto& AmmoSpec = Objects::AmmoDataManager::Instance()->Get((*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetAmmoSpecID(0));//マガジンの一番上の弾データをチャンバーイン
			PlayerMngr->GetPlayer(GetMyUserPlayerID())->AddShot(AmmoSpec->GetPellet());
			auto MuzzleMat = GetPartsFrameMatParent(GunFrame::Muzzle);
			//銃声
			SE->Get(SoundType::SE, GetGunSoundSet(
				(this->m_MuzzlePtr) ? (*this->m_MuzzlePtr)->GetModifySlot()->GetMyData()->GetGunShootSound() : GetModifySlot()->GetMyData()->GetGunShootSound()
			))->Play3D(MuzzleMat.pos(), Scale3DRate * 50.0f);
			//エフェクト
			auto* OptionParts = OptionManager::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
				EffectSingleton::Instance()->SetOnce_Any(Effect::ef_fire2, MuzzleMat.pos(), MuzzleMat.zvec2(), 0.35f * this->m_MuzzleSmokeSize / (0.00762f * Scale3DRate), 2.0f);
			}
			if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 2) {
				if (GetMyUserPlayerID() == Player::PlayerManager::Instance()->GetWatchPlayerID()) {
					if (HaveFrame(static_cast<int>(GunFrame::smoke1))) {
						int Frame = GetFrame(static_cast<int>(GunFrame::smoke1));
						auto Mat = GetObj().GetFrameLocalWorldMatrix(Frame);
						Vector3DX Pos = Mat.pos();
						Vector3DX Vec = (GetObj().GetFramePosition(GetObj().GetFrameChild(Frame, 0)) - Pos).normalized();
						Vec = Matrix3x3DX::Vtrans(Vec, Matrix3x3DX::RotAxis(Vector3DX::Cross(Vec, Mat.yvec()), deg2rad(GetRandf(10))));

						EffectSingleton::Instance()->SetOnce_Any(Effect::ef_shotdust, Pos, Vec, 1.f, 2.f);
					}
					if (HaveFrame(static_cast<int>(GunFrame::smoke2))) {
						int Frame = GetFrame(static_cast<int>(GunFrame::smoke2));
						auto Mat = GetObj().GetFrameLocalWorldMatrix(Frame);
						Vector3DX Pos = Mat.pos();
						Vector3DX Vec = (GetObj().GetFramePosition(GetObj().GetFrameChild(Frame, 0)) - Pos).normalized();
						Vec = Matrix3x3DX::Vtrans(Vec, Matrix3x3DX::RotAxis(Vector3DX::Cross(Vec, Mat.yvec()), deg2rad(GetRandf(10))));

						EffectSingleton::Instance()->SetOnce_Any(Effect::ef_shotdust, Pos, Vec, 1.f, 2.f);
					}
					if (HaveFrame(static_cast<int>(GunFrame::smoke3))) {
						int Frame = GetFrame(static_cast<int>(GunFrame::smoke3));
						auto Mat = GetObj().GetFrameLocalWorldMatrix(Frame);
						Vector3DX Pos = Mat.pos();
						Vector3DX Vec = (GetObj().GetFramePosition(GetObj().GetFrameChild(Frame, 0)) - Pos).normalized();
						Vec = Matrix3x3DX::Vtrans(Vec, Matrix3x3DX::RotAxis(Vector3DX::Cross(Vec, Mat.yvec()), deg2rad(GetRandf(10))));

						EffectSingleton::Instance()->SetOnce_Any(Effect::ef_shotdust, Pos, Vec, 1.f, 2.f);
					}
					if (HaveFrame(static_cast<int>(GunFrame::smoke4))) {
						int Frame = GetFrame(static_cast<int>(GunFrame::smoke4));
						auto Mat = GetObj().GetFrameLocalWorldMatrix(Frame);
						Vector3DX Pos = Mat.pos();
						Vector3DX Vec = (GetObj().GetFramePosition(GetObj().GetFrameChild(Frame, 0)) - Pos).normalized();
						Vec = Matrix3x3DX::Vtrans(Vec, Matrix3x3DX::RotAxis(Vector3DX::Cross(Vec, Mat.yvec()), deg2rad(GetRandf(10))));

						EffectSingleton::Instance()->SetOnce_Any(Effect::ef_shotdust, Pos, Vec, 1.f, 2.f);
					}
					if (HaveFrame(static_cast<int>(GunFrame::smoke5))) {
						int Frame = GetFrame(static_cast<int>(GunFrame::smoke5));
						auto Mat = GetObj().GetFrameLocalWorldMatrix(Frame);
						Vector3DX Pos = Mat.pos();
						Vector3DX Vec = (GetObj().GetFramePosition(GetObj().GetFrameChild(Frame, 0)) - Pos).normalized();
						Vec = Matrix3x3DX::Vtrans(Vec, Matrix3x3DX::RotAxis(Vector3DX::Cross(Vec, Mat.yvec()), deg2rad(GetRandf(10))));

						EffectSingleton::Instance()->SetOnce_Any(Effect::ef_shotdust, Pos, Vec, 1.f, 2.f);
					}
				}
			}


			//発砲
			for (int loop = 0, max = AmmoSpec->GetPellet(); loop < max; ++loop) {
				Objects::AmmoPool::Instance()->Put(&AmmoSpec, MuzzleMat.pos(),
					(
						Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(AmmoSpec->GetAccuracy()))) *
						Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(GetRandf(AmmoSpec->GetAccuracy()))) *
						Matrix3x3DX::Get33DX(MuzzleMat)
						).zvec2(), GetMyUserPlayerID());
			}
			//チャンバーを空にする
			this->m_InChamber = false;
			//銃口煙の追加
			this->m_MuzzleSmokeControl.AddMuzzleSmokePower();
			//撃ちアニメに移行
			SetGunAnime(Charas::GunAnimeID::Shot);
			if (GetMyUserPlayerID() == Player::PlayerManager::Instance()->GetWatchPlayerID()) {
				this->m_AmmoInChamberObj->SetActive(true);
			}
			//リコイル
			AddRecoil();
			//ビジュアルリコイル
			if (GetMyUserPlayerID() == PlayerMngr->GetWatchPlayerID()) {
				Camera3D::Instance()->SetCamShake(0.1f, 0.1f* GetRecoilRandViewScale());
			}
		}
		//
		void				GunObj::UpdateGunMat(bool IsSelectGun, bool IsActiveAutoAim, const Matrix3x3DX& CharaRotationCache, const Vector3DX& HeadPos, const Vector3DX& RotRad) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			if (IsSelectGun) {
				//アニメーション
				if (GetNowGunAnimeID() != InvalidID) {
					SetAnimOnce(GetNowGunAnimeID(), this->m_GunAnimeSpeed[static_cast<int>(GetGunAnime())]);
				}
				switch (GetGunAnime()) {
				case Charas::GunAnimeID::Base:
					if (!IsInChamber() && this->m_Capacity != 0) {
						SetGunAnime(Charas::GunAnimeID::Cocking);//チャンバーが空で弾倉が空でないなら
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
					if ((*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetIsJungle() && this->m_JungleFast) {
						if (GetNowAnimTimePerCache() >= 0.5f) {
							SetGunAnime(Charas::GunAnimeID::Reload);
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetMove().GetPos(), Scale3DRate * 3.0f);
						}
					}
					else {
						if (IsNowGunAnimeEnd()) {
							SetGunAnime(Charas::GunAnimeID::Reload);
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetMove().GetPos(), Scale3DRate * 3.0f);
						}
					}
					break;
				case Charas::GunAnimeID::Reload:
					if (GetNowAnimTimePerCache() >= (this->m_isMagSuccess ? 0.75f : 1.0f)) {
						switch (GetReloadType()) {
						case RELOADTYPE::MAG:
							this->m_Capacity = GetAmmoAll();//マガジン装填
							SetGunAnime(Charas::GunAnimeID::ReloadEnd);
							this->m_JungleFast ^= 1;
							break;
						case RELOADTYPE::AMMO:
							++this->m_Capacity;//マガジン装填
							if ((this->m_Capacity == GetAmmoAll()) || this->m_ReloadAmmoCancel) {
								this->m_ReloadAmmoCancel = false;
								SetGunAnime(Charas::GunAnimeID::ReloadEnd);
								this->m_JungleFast ^= 1;
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
					time += 60.0f * DXLib_refParts->GetDeltaTime() * (this->m_GunAnimeSpeed[index]);
				}
				m_CommonGunAnimeTime += 60.0f * DXLib_refParts->GetDeltaTime();
				//
#if defined(DEBUG) && FALSE
				if (GetMyUserPlayerID() == Player::PlayerManager::Instance()->GetWatchPlayerID()) {
					printfDx("[%s]\n", (GetGunAnime() == Charas::GunAnimeID::Base) ? "Base" : Charas::GunAnimeIDName[static_cast<int>(GetGunAnime())]);
					printfDx("[%f]\n", (GetGunAnime() == Charas::GunAnimeID::Base) ? 0.0f : GetNowAnimTimePerCache());
				}
#endif
			}
			//銃の揺れ
			if (!IsFirstLoop()) {
				Vector3DX Target = Vector3DX::zero();
				if ((GetGunAnime() == Charas::GunAnimeID::Shot) && (GetNowAnimTimePerCache() <= 0.4f) && GetGunAnimBlendPer(Charas::GunAnimeID::ADS) < 0.5f) {
					switch (GetStockType()) {
					case Guns::STOCKTYPE::none:
						Target.x += deg2rad(15);
						break;
					case Guns::STOCKTYPE::curve:
						Target.x += deg2rad(5);
						break;
					default:
						break;
					}
					Easing(&this->m_UpperRad2, Target, 0.f, EasingType::OutExpo);
				}
				else {
					Easing(&this->m_UpperRad2, Target, 0.8f, EasingType::OutExpo);
				}
				Easing(&this->m_UpperRad, (this->m_UpperPrevRad - RotRad), 0.9f, EasingType::OutExpo);
			}

			this->m_UpperPrevRad = RotRad;
			Easing(&this->m_GunSwingMat,
				Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_UpperRad.x) *
				Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_UpperRad.y) *
				Matrix3x3DX::RotAxis(Vector3DX::forward(), std::clamp(
					-this->m_UpperRad.y * 5.0f * std::clamp(1.0f - GetGunAnimBlendPer(Charas::GunAnimeID::ADS), 0.0f, 1.0f),
					deg2rad(-5), deg2rad(5))),
					0.8f, EasingType::OutExpo);
			Easing(&this->m_GunSwingMat2, this->m_GunSwingMat, 0.8f, EasingType::OutExpo);

			Easing(&this->m_GunSwingShotMat,
				Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_UpperRad2.x)*
				Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_UpperRad2.y),
				0.8f, EasingType::OutExpo);
			//スリング
			Easing(&this->m_SlingPer, IsSelectGun ? 1.0f : 0.0f, 0.9f, EasingType::OutExpo);
			if (this->m_SlingPer <= 0.001f) { this->m_SlingPer = 0.0f; }
			if (this->m_SlingPer >= 0.999f) { this->m_SlingPer = 1.0f; }
			//
			auto EyeYRot = Matrix3x3DX::RotVec2(Lerp(GetMove().GetMat().yvec(), GetPartsFrameMatParent(GunFrame::Eyepos).yvec(), GetGunAnimBlendPer(Charas::GunAnimeID::ADS)), GetMove().GetMat().yvec());
			//アニメ演算の結果をキャッシュ
			this->m_AnimNowCache = GetAnimDataNow(Charas::GunAnimeID::Aim);
			for (int loop = 0; loop < static_cast<int>(Charas::GunAnimeID::ChoiceOnceMax); ++loop) {
				if (GetGunAnimBlendPer((Charas::GunAnimeID)loop) != 0.0f) {
					this->m_AnimNowCache.LerpNow(GetAnimDataNow((Charas::GunAnimeID)loop), GetGunAnimBlendPer((Charas::GunAnimeID)loop));
				}
			}
			//
			Matrix3x3DX AnimRot;
			Vector3DX AnimPos;
			if (this->m_SlingPer > 0.0f) {
				AnimRot = GetGunAnimeNow().GetRot() * this->m_GunSwingMat2 * this->m_GunSwingShotMat * CharaRotationCache * EyeYRot;
				AnimPos = GetGunAnimeNow().GetPos();
				AnimPos.x *= GetSwitchPer();
				AnimPos = HeadPos + Matrix3x3DX::Vtrans(AnimPos, CharaRotationCache);
				//オートエイム
				if (IsSelectGun) {
					if (IsActiveAutoAim) {
						m_IsActiveAutoAimTimer = std::clamp(m_IsActiveAutoAimTimer + DXLib_refParts->GetDeltaTime() / 0.6f * GetAutoAimSpeed(), 0.f, 1.f);
					}
					else {
						m_IsActiveAutoAimTimer = 0.f;
					}
					this->m_AutoAimControl.Update(IsActiveAutoAim && (m_IsActiveAutoAimTimer >= 1.f), GetMyUserPlayerID(), GetPartsFrameMatParent(GunFrame::Eyepos).pos(), GetMove().GetMat().zvec2(), GetAutoAimRadian());
					this->m_AutoAimControl.CalcAutoAimMat(&AnimRot);
				}
				Easing(&m_GunShotZrandR, GetShotSwitch() ? GetRandf(90.0f) : 0.0f, 0.8f, EasingType::OutExpo);
				Easing(&m_GunShotZrand, m_GunShotZrandR, 0.8f, EasingType::OutExpo);
				AnimRot = AnimRot * Matrix3x3DX::RotAxis(AnimRot.zvec2(), deg2rad(m_GunShotZrand * std::clamp(1.0f - GetGunAnimBlendPer(Charas::GunAnimeID::ADS), 0.0f, 1.0f)));
			}
			//武器座標
			SetGunMat(Lerp(this->m_SlingRot * this->m_GunSwingMat2* this->m_GunSwingShotMat* EyeYRot, AnimRot, this->m_SlingPer), Lerp(this->m_SlingPos, AnimPos, this->m_SlingPer));
			//アニメーションに応じたいろいろの更新
			{
				Matrix4x4DX MatMin;
				Matrix4x4DX MatMax;
				float BasePer = 0.0f;
				float MaxPer = 0.0f;

				auto SetMagMatLerp = [&](const Matrix4x4DX& MaxMat, float PerMax) {
					MatMin = MatMax;
					MatMax = MaxMat;
					BasePer = MaxPer;
					MaxPer = PerMax;
					if (BasePer <= GetNowAnimTimePerCache() && GetNowAnimTimePerCache() <= MaxPer) {
						if (this->m_MagazinePtr) {
							(*this->m_MagazinePtr)->SetHandMatrix(Lerp(MatMin, MatMax, std::clamp((GetNowAnimTimePerCache() - BasePer) / (MaxPer - BasePer), 0.0f, 1.0f)));
						}
					}
					};

				Matrix4x4DX JungleMat;
				if ((*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetIsJungle()) {

					Vector3DX Pos = (*this->m_MagazinePtr)->GetFrameBaseLocalWorldMat(static_cast<int>(GunFrame::SecondPos)).pos();
					Vector3DX YVec = ((*this->m_MagazinePtr)->GetFrameBaseLocalWorldMat(static_cast<int>(GunFrame::SecondPos_yvec)).pos() - Pos).normalized();
					Vector3DX ZVec = ((*this->m_MagazinePtr)->GetFrameBaseLocalWorldMat(static_cast<int>(GunFrame::SecondPos_zvec)).pos() - Pos).normalized();
					JungleMat = Matrix4x4DX::Axis1(YVec, ZVec*-1.f, Pos);

					JungleMat = Matrix3x3DX::Get33DX(JungleMat).Get44DX() * Matrix4x4DX::Mtrans(JungleMat.pos());

					JungleMat = JungleMat.inverse();
				}
				auto CalcJungleMat = [&](const Matrix4x4DX& Mat) {
					return (JungleMat.rotation() * Mat.rotation()) * Matrix4x4DX::Mtrans(Matrix4x4DX::Vtrans(JungleMat.pos(), Mat.rotation()) + Mat.pos());
					};


				switch (GetGunAnime()) {
				case Charas::GunAnimeID::HighReady:
				case Charas::GunAnimeID::LowReady:
				case Charas::GunAnimeID::EmergencyReady:
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
						if ((5.0f / 35.0f < GetNowAnimTimePerCache() && GetNowAnimTimePerCache() < 6.0f / 35.0f)) {
							PlayGunSound(EnumGunSound::CockingPull);
						}
						if ((28.0f / 35.0f < GetNowAnimTimePerCache() && GetNowAnimTimePerCache() < 29.0f / 35.0f)) {
							PlayGunSound(EnumGunSound::CockingRelease);
						}
						break;
					case SHOTTYPE::FULL:
					case SHOTTYPE::SEMI:
						if ((8.0f / 35.0f < GetNowAnimTimePerCache() && GetNowAnimTimePerCache() < 9.0f / 35.0f)) {
							PlayGunSound(EnumGunSound::CockingPull);
						}
						if ((18.0f / 35.0f < GetNowAnimTimePerCache() && GetNowAnimTimePerCache() < 19.0f / 35.0f)) {
							PlayGunSound(EnumGunSound::CockingRelease);
						}
						break;
					default:
						break;
					}
					if (GetNowAnimTimePerCache() >= 25.0f / 35.0f) {
						this->m_CockHand = false;
					}
					if (GetNowAnimTimePerCache() >= 19.0f / 35.0f) {
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
					if ((*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetIsJungle() && this->m_JungleFast) {
						MatMax = GetPartsFrameMatParent(GunFrame::Magpos);
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2), 1.0f);
						this->m_MagHand = true;
						if (0.95f <= GetNowAnimTimePerCache()) {
							PlayGunSound(EnumGunSound::UnloadMag);
						}
					}
					else {
						if (GetShotType() == SHOTTYPE::PUMP) {
							MatMax = GetPartsFrameMatParent(GunFrame::Magpos);
							SetMagMatLerp(this->m_MagazinePoachMat, 15.0f / 60.0f);
							SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2), 25.0f / 60.0f);
							SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 35.0f / 60.0f);
							SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 48.0f / 60.0f);
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
							float Per = 35.0f / 60.0f;
							this->m_MagHand = true;
							if (Per <= GetNowAnimTimePerCache()) {
								this->m_MagHand = false;
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
											MagposMat.pos(), Matrix3x3DX::Get33DX(MagposMat), MagposMat.yvec() * (Scale3DRate * -3.0f / 60.0f),
											12.0f, Objects::FallObjectType::Magazine);
									}
								}
								else {
									this->m_isMagFall = false;
								}
								if (0.0f < GetNowAnimTimePerCache()) {
									PlayGunSound(EnumGunSound::UnloadMag);
								}
							}
						}
					}
					break;
				case Charas::GunAnimeID::ReloadStart:
					if ((*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetIsJungle() && this->m_JungleFast) {
						MatMax = GetPartsFrameMatParent(GunFrame::Magpos);
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2), 1.0f);
						this->m_MagHand = true;
						if (0.95f <= GetNowAnimTimePerCache()) {
							PlayGunSound(EnumGunSound::UnloadMag);
						}
					}
					else {
						MatMax = GetPartsFrameMatParent(GunFrame::Magpos);
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Magpos), 0.3f);
						SetMagMatLerp(GetPartsFrameMatParent(GunFrame::Mag2), 0.6f);
						SetMagMatLerp(this->m_MagazinePoachMat, 1.0f);

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
					}
					break;
				case Charas::GunAnimeID::ReloadWait:
					if ((*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetIsJungle() && this->m_JungleFast) {
						this->m_MagHand = true;
						MatMax = GetPartsFrameMatParent(GunFrame::Mag2);
						SetMagMatLerp(CalcJungleMat(GetPartsFrameMatParent(GunFrame::Mag2)), 0.5f);
					}
					else {
						this->m_MagHand = true;
						if (this->m_MagazinePtr) {
							(*this->m_MagazinePtr)->SetHandMatrix(this->m_MagazinePoachMat);
						}
					}
					break;
				case Charas::GunAnimeID::Reload:
					if ((*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetIsJungle() && this->m_JungleFast) {
						this->m_MagHand = true;
						MatMax = CalcJungleMat(GetPartsFrameMatParent(GunFrame::Mag2));
						SetMagMatLerp(CalcJungleMat(GetPartsFrameMatParent(GunFrame::Mag2)), 0.55f);
						SetMagMatLerp(CalcJungleMat(GetPartsFrameMatParent(GunFrame::Magpos)), 0.75f);
						SetMagMatLerp(CalcJungleMat(GetPartsFrameMatParent(GunFrame::Magpos)), 1.0f);
						if (GetNowAnimTimePerCache() >= 0.55f) {
							PlayGunSound(EnumGunSound::LoadMag);
						}
					}
					else {
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
									GetMove().GetMat().xvec() * (MissPer / 3.0f * Scale3DRate)
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
					}
					break;
				case Charas::GunAnimeID::ReloadEnd:
					if ((*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetIsJungle() && this->m_JungleFast) {
						this->m_MagHand = false;
						if (GetNowAnimTimePerCache() > 0.95f) {
							if (this->m_MagazinePtr) {
								(*this->m_MagazinePtr)->SetHandMatrix(CalcJungleMat(GetPartsFrameMatParent(GunFrame::Magpos)));
							}
						}
					}
					else {
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
					}
					break;
				case Charas::GunAnimeID::ThrowReady:
					if (GetNowAnimTimePerCache() >= 0.55f) {
						if (!this->m_ReleasePin) {
							this->m_ReleasePin = true;
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Pin))->Play3D(GetMove().GetPos(), Scale3DRate * 2.0f);
						}
					}
					else {
						this->m_ReleasePin = false;
					}
					break;
				case Charas::GunAnimeID::Throw:
					if (GetNowAnimTimePerCache() >= 0.4f) {
						if (IsActive()) {
							SetActiveAll(false);//手にあるものは非表示にする
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Throw))->Play3D(GetMove().GetPos(), Scale3DRate * 2.0f);
							this->m_Grenade.SetFall(GetMove().GetPos(), this->m_GrenadeThrowRot, (this->m_GrenadeThrowRot.zvec2()).normalized() * (Scale3DRate * 15.0f / 60.0f), 3.5f, Objects::FallObjectType::Grenade);

							this->m_IsChamberOn = false;
							if (this->m_Capacity != 0) {
								ChamberIn();
							}
							else {
								this->m_InChamber = false;
							}
						}
					}
					break;
				default:
					break;
				}
			}
		}
		void GunObj::EjectCart(void) noexcept {
			if (!this->m_IsEject) {
				this->m_IsEject = true;
				this->m_AmmoInChamberObj->SetActive(false);
				auto* PlayerMngr = Player::PlayerManager::Instance();
				if (this->GetMyUserPlayerID() == PlayerMngr->GetWatchPlayerID()) {
					this->m_CartFall.SetFall(
						GetPartsFrameMatParent(GunFrame::Cart).pos(), GetMove().GetMat(),
						(GetPartsFrameMatParent(GunFrame::CartVec).pos() - GetPartsFrameMatParent(GunFrame::Cart).pos() + Vector3DX::vget(GetRandf(0.2f), 0.5f + GetRandf(1.0f), GetRandf(0.2f))).normalized() * (Scale3DRate * 2.0f / 60.0f), 2.0f, Objects::FallObjectType::Cart);
				}
			}
		}
		//グレネード更新
		void				GunObj::UpdateGrenade(void) noexcept {
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();

			for (const auto& grenade : this->m_Grenade.GetPtrList()) {
				if (grenade->PopIsEndFall()) {
					auto& AmmoSpec = Objects::AmmoDataManager::Instance()->Get(GetModifySlot()->GetMyData()->GetAmmoSpecID(0));
					for (int loop = 0, max = AmmoSpec->GetPellet(); loop < max; ++loop) {
						//円周上にまき散らす
						auto mat = Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-GetRand(30))) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180)));
						Objects::AmmoPool::Instance()->Put(&AmmoSpec,
							grenade->GetMove().GetPos() + mat.zvec2() * (0.5f * Scale3DRate) + Vector3DX::up() * (Lerp(0.5f, 1.5f, static_cast<float>(loop) / max) * Scale3DRate)
							, mat.zvec2(), GetMyUserPlayerID());
					}

					//破壊
					int								xput = 8;
					int								yput = 8;
					int								zput = 8;
					auto Put = BackGroundParts->GetPoint(grenade->GetMove().GetPos());
					bool IsChanged = false;
					for (int xp = -xput / 2; xp < xput / 2; ++xp) {
						for (int yp = 0; yp < yput; ++yp) {
							for (int zp = -zput / 2; zp < zput / 2; ++zp) {
								if (BackGroundParts->DamageCell(Put.x + xp, Put.y + yp, Put.z + zp, 100)) {
									IsChanged = true;
								}
							}
						}
					}
					if (IsChanged) {
						auto* OptionParts = OptionManager::Instance();
						if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 2) {
							EffectSingleton::Instance()->SetOnce_Any(Effect::ef_break, BackGroundParts->GetPos(Put.x, Put.y, Put.z),
								Matrix3x3DX::Vtrans(Vector3DX::forward(), Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180.0f)))), 3.0f + GetRandf(2.0f), 3.0f);
						}
					}
				}
			}
		}
		void				GunObj::SetupGun(void) noexcept {
			this->m_ModifySlot = std::make_unique<ModifySlot>(GetFilePath());
			this->m_GunsModify = std::make_unique<GunsModify>(shared_from_this(), false);
			{
				//小孫として登録されているパーツをリスト化
				std::vector<const SharedGunParts*> PartsList;
				GetModifySlot()->GetAnyByChild([&](const SharedGunParts& ptr) { if (ptr) { PartsList.emplace_back(&ptr); } });
				this->m_SightPtr = nullptr;
				this->m_MuzzlePtr = nullptr;
				this->m_UpperPtr = nullptr;
				this->m_LowerPtr = nullptr;
				this->m_MagazinePtr = nullptr;

				for (auto& parts : PartsList) {
					ObjType ObjectType = static_cast<ObjType>((*parts)->GetobjType());
					if (ObjectType == ObjType::Sight) {
						this->m_SightPtr = &((std::shared_ptr<SightPartsObj>&)(*parts));
					}
					if (ObjectType == ObjType::MuzzleAdapter) {
						this->m_MuzzlePtr = &((std::shared_ptr<MuzzlePartsObj>&)(*parts));
					}
					if (ObjectType == ObjType::Upper) {
						this->m_UpperPtr = &((std::shared_ptr<UpperPartsObj>&)(*parts));
					}
					if (ObjectType == ObjType::Lower) {
						this->m_LowerPtr = &((std::shared_ptr<LowerPartsObj>&)(*parts));
					}
					if (ObjectType == ObjType::Magazine) {
						this->m_MagazinePtr = &((std::shared_ptr<MagazinePartsObj>&)(*parts));
					}
				}

				this->m_ShootRate_Diff = 0;
				this->m_Recoil_Diff = 0;
				for (auto& parts : PartsList) {
					//性能周り
					auto& data = (*parts)->GetModifySlot()->GetMyData();
					this->m_ShootRate_Diff += data->GetShootRate_Diff();
					this->m_Recoil_Diff += data->GetRecoil_Diff();
				}
				PartsList.clear();
			}
			if (this->m_MagazinePtr) {
				this->m_MagFall.Init((*this->m_MagazinePtr)->GetFilePath(), 1);
				const auto& AmmoSpecPath = Objects::AmmoDataManager::Instance()->Get((*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetAmmoSpecID(0))->GetPath();
				this->m_CartFall.Init(AmmoSpecPath, 4);	//装填したマガジンの弾に合わせて薬莢生成
				this->m_AmmoInChamberObj = std::make_shared<Objects::AmmoInChamberObj>();
				ObjectManager::Instance()->InitObject(this->m_AmmoInChamberObj, AmmoSpecPath);
				this->m_AmmoInChamberObj->SetActive(false);
				(*this->m_MagazinePtr)->SetAmmoActive(false);
			}
			if (GetModifySlot()->GetMyData()->GetIsThrowWeapon()) {
				this->m_Grenade.Init(GetFilePath(), 2);
			}
			for (auto& speed : this->m_GunAnimeSpeed) {
				speed = 1.0f;
			}
			this->m_GunAnimeSpeed[static_cast<int>(Charas::GunAnimeID::Shot)] = static_cast<float>(GetModifySlot()->GetMyData()->GetShotRate()) / 60.0f / 10.0f;

			this->m_GunAnimeSpeed[static_cast<int>(Charas::GunAnimeID::ReloadStart_Empty)] = GetReloadSpeed();
			this->m_GunAnimeSpeed[static_cast<int>(Charas::GunAnimeID::ReloadStart)] = GetReloadSpeed();
			this->m_GunAnimeSpeed[static_cast<int>(Charas::GunAnimeID::ReloadWait)] = GetReloadSpeed();
			this->m_GunAnimeSpeed[static_cast<int>(Charas::GunAnimeID::Reload)] = GetReloadSpeed();
			this->m_GunAnimeSpeed[static_cast<int>(Charas::GunAnimeID::ReloadEnd)] = GetReloadSpeed();

			auto BaseAnim = GetAnimDataNow(Charas::GunAnimeID::Aim);
			SetGunMat(BaseAnim.GetRot(), BaseAnim.GetPos());
			this->m_BaseMuzzle = GetPartsFrameMatParent(GunFrame::Muzzle);
		}
		//
		void				GunObj::Init_Sub(void) noexcept {
			SetMinAABB(Vector3DX::vget(-0.5f, -0.5f, -0.5f) * Scale3DRate);
			SetMaxAABB(Vector3DX::vget(0.5f, 0.5f, 0.5f) * Scale3DRate);
			InitGunAnimePer();
			this->m_IsMeshDraw.resize(GetObj().GetMeshNum());
			for (auto& isdraw : this->m_IsMeshDraw) {
				isdraw = true;
			}
			m_Weight_gram = InvalidID;
		}
		void				GunObj::FirstUpdate(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			if (IsFirstLoop()) {
				this->m_MuzzleSmokeControl.InitMuzzleSmoke(GetPartsFrameMatParent(GunFrame::Muzzle).pos());
				if (this->m_MagazinePtr) {
					const auto& AmmoSpec = Objects::AmmoDataManager::Instance()->Get((*this->m_MagazinePtr)->GetModifySlot()->GetMyData()->GetAmmoSpecID(0));//マガジンの一番上の弾データをチャンバーイン
					this->m_MuzzleSmokeSize = AmmoSpec->GetCaliber() * AmmoSpec->GetPellet() * Scale3DRate;
				}
			}
			else {
				this->m_MuzzleSmokeControl.UpdateMuzzleSmoke(GetPartsFrameMatParent(GunFrame::Muzzle).pos(), GetGunAnime() != Charas::GunAnimeID::Shot && !IsNeedCalcSling());
			}
			if (this->m_MagazinePtr) {
				(*this->m_MagazinePtr)->SetAmmoActive(
					(GetMyUserPlayerID() == Player::PlayerManager::Instance()->GetWatchPlayerID()) &&
					(GetReloadType() == RELOADTYPE::MAG) &&//弾込め式ならマガジン内部の弾は描画しない
				(GetReloading()));
			}
			//
			for (int loop = 0; loop < static_cast<int>(Charas::GunAnimeID::ChoiceOnceMax); ++loop) {
				int ID = GetModifySlot()->GetMyData()->GetAnimSelectList()[loop];
				if (ID != InvalidID) {
					if (GetGunAnime() == (Charas::GunAnimeID)loop) {
						SetObj().SetAnim(ID).SetPer(1.0f);
					}
					else {
						SetObj().SetAnim(ID).Reset();
					}
				}
			}
			for (int loop = static_cast<int>(Charas::GunAnimeID::ChoiceOnceMax); loop < static_cast<int>(Charas::GunAnimeID::Max); ++loop) {
				int ID = GetModifySlot()->GetMyData()->GetAnimSelectList()[loop];
				if (ID != InvalidID) {
					switch ((Charas::GunAnimeID)loop) {
					case Charas::GunAnimeID::Hammer:
						SetObj().SetAnim(ID).SetPer(std::clamp(GetObj().GetAnim(ID).GetPer() + DXLib_refParts->GetDeltaTime() * (GetShotSwitch() ? -10.0f : 10.0f), 0.0f, 1.0f));
						break;
					case Charas::GunAnimeID::Open:
					{
						//射撃時のボルトアニメを行わせる
						switch (GetGunAnime()) {
						case Charas::GunAnimeID::Shot:
							if (GetShotType() == SHOTTYPE::FULL || GetShotType() == SHOTTYPE::SEMI) {
								float Per = 0.6f;
								if (GetNowAnimTimePerCache() < Per) {
									SetObj().SetAnim(ID).SetPer(std::clamp(GetNowAnimTimePerCache() / Per, 0.0f, 1.0f));
								}
								else {
									if (GetAmmoNumTotal() == 0 && GetModifySlot()->GetMyData()->GetisHoldOpen()) {
										SetObj().SetAnim(ID).SetPer(1.0f);
									}
									else {
										SetObj().SetAnim(ID).SetPer(std::clamp(1.0f - (GetNowAnimTimePerCache() - Per) / (1.0f - Per), 0.0f, 1.0f));
									}
								}
							}
							else {
								SetObj().SetAnim(ID).SetPer(0.0f);
							}
							break;
						case Charas::GunAnimeID::Cocking:
						{
							float Per = 19.0f / 35.0f;
							if (GetNowAnimTimePerCache() < Per) {
								//元から開いていたら1のまま
								SetObj().SetAnim(ID).SetPer(std::max(GetObj().GetAnim(ID).GetPer(), std::clamp(GetNowAnimTimePerCache() / Per, 0.0f, 1.0f)));
							}
							else {
								if (GetShotType() == SHOTTYPE::BOLT || GetShotType() == SHOTTYPE::PUMP) {
									if (GetAmmoNumTotal() == 0 && GetModifySlot()->GetMyData()->GetisHoldOpen()) {
										SetObj().SetAnim(ID).SetPer(1.0f);
									}
									else {
										SetObj().SetAnim(ID).SetPer(std::clamp(1.0f - (GetNowAnimTimePerCache() - Per) / (1.0f - Per), 0.0f, 1.0f));
									}
								}
								else {
									SetObj().SetAnim(ID).SetPer(std::clamp(1.0f - (GetNowAnimTimePerCache() - Per) / (1.0f - Per), 0.0f, 1.0f));
								}
							}
						}
							break;
						case Charas::GunAnimeID::ReloadStart_Empty:
						{
							float Per = 35.0f / 60.0f;
							if (GetShotType() == SHOTTYPE::PUMP) {
								if (Per <= GetNowAnimTimePerCache()) {
									SetObj().SetAnim(ID).SetPer(std::clamp(1.0f - (GetNowAnimTimePerCache() - Per) / (1.0f - Per), 0.0f, 1.0f));
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
			CalcRecoil();
			//グレネード演算
			UpdateGrenade();
			//
			if (this->m_SightPtr) {
				if (GetSightZoomSize() > 1.0f) {
					float Per = 1.0f - std::clamp(GetGunAnimBlendPer(Charas::GunAnimeID::ADS), 0.0f, 1.0f);
					if (Per < 1.0f) {
						(*this->m_SightPtr)->SetScopeAlpha(Per);
					}
				}
			}
			else {
				if (GetSightZoomSize() > 1.0f) {
					float Per = 1.0f - std::clamp(GetGunAnimBlendPer(Charas::GunAnimeID::ADS), 0.0f, 1.0f);
					if (Per < 1.0f) {
						SetScopeAlpha(Per);
					}
				}
			}
		}
		void				GunObj::DrawShadow(void) noexcept {
			if (!IsActive()) { return; }
			GetObj().DrawModel();
		}
		void				GunObj::CheckDraw_Sub(int) noexcept {
			if (GetMyUserPlayerID() == Player::PlayerManager::Instance()->GetWatchPlayerID()) {
				auto* PostPassParts = PostPassEffect::Instance();
				if (!GetCanShot()) {
					if (!IsNeedCalcSling()) {
						this->m_Reticle_on = false;
						PostPassParts->Set_is_lens(false);
					}
					return;
				}
				this->m_AimPoint.Calc(GetPartsFrameMatParent(GunFrame::Muzzle).pos() + GetMove().GetMat().zvec2() * (50.0f * Scale3DRate));
				Vector3DX LensPos = GetPartsFrameMatParent(GunFrame::Lens).pos();
				if (this->m_Lens.Calc(LensPos)) {
					if (this->m_LensSize.Calc(GetPartsFrameMatParent(GunFrame::LensSize).pos())) {
						this->m_LensSizeLen = static_cast<float>(std::hypot(this->m_Lens.XPos() - this->m_LensSize.XPos(), this->m_Lens.YPos() - this->m_LensSize.YPos()));
					}
					if (this->m_Reticle.Calc(LensPos + (LensPos - GetPartsFrameMatParent(GunFrame::Eyepos).pos()).normalized() * (1.0f * Scale3DRate))) {
						this->m_Reticle_on = (this->m_LensSizeLen > std::hypot(this->m_Lens.XPos() - this->m_Reticle.XPos(), this->m_Lens.YPos() - this->m_Reticle.YPos()));
					}
					else {
						this->m_Reticle_on = false;
					}
				}
				//レンズ表示機能への反映
				if (GetSightZoomSize() > 1.0f) {
					if (this->m_Reticle_on) {
						PostPassParts->Set_is_lens(true);
						PostPassParts->Set_zoom_lens(std::max(1.0f, GetSightZoomSize() / 2.0f));
						PostPassParts->Set_xp_lens(static_cast<float>(this->m_Lens.XPos()));
						PostPassParts->Set_yp_lens(static_cast<float>(this->m_Lens.YPos()));
						PostPassParts->Set_size_lens(this->m_LensSizeLen);
					}
					else if (GetGunAnimBlendPer(Charas::GunAnimeID::ADS) < 0.5f) {
						PostPassParts->Set_is_lens(false);
						PostPassParts->Set_zoom_lens(1.0f);
					}
				}
			}
		}
		void				GunObj::Draw(bool isDrawSemiTrans, int Range) noexcept {
			if (!IsActive()) { return; }
			if (!IsDraw(Range)) { return; }
			if (isDrawSemiTrans && GetMyUserPlayerID() == Player::PlayerManager::Instance()->GetWatchPlayerID()) {
				this->m_MuzzleSmokeControl.DrawMuzzleSmoke(this->m_MuzzleSmokeSize);
			}

			if (!isDrawSemiTrans) {
				if (IsDrawAllMesh()) {
					GetObj().DrawModel();
				}
				else {
					for (auto& isdraw : this->m_IsMeshDraw) {
						if (isdraw == TRUE) {
							int index = static_cast<int>(&isdraw - &this->m_IsMeshDraw.front());
							GetObj().DrawMesh(index);
						}
					}
				}
			}

			if (isDrawSemiTrans && GetMyUserPlayerID() == Player::PlayerManager::Instance()->GetWatchPlayerID()) {
				if (GetGunAnime() == Charas::GunAnimeID::ThrowReady) {
					SetUseLighting(false);
					SetUseHalfLambertLighting(false);

					Vector3DX Pos = GetMove().GetPos();
					Vector3DX Vect = (this->m_GrenadeThrowRot.zvec2()).normalized() * (Scale3DRate * 15.0f / 60.0f);
					float yAdd = 0.0f;
					Vector3DX RePosDraw = Pos;
					for (int loop = 0; loop < 60; ++loop) {
						Vector3DX RePos = Pos;
						Pos = Pos + Vect + Vector3DX::up() * yAdd;
						yAdd += (GravityRate / (60.0f * 60.0f));
						{
							Vector3DX EndPos = Pos;
							Vector3DX Normal;
							if (BackGround::BackGroundControl::Instance()->CheckLinetoMap(RePos, &EndPos, &Normal) != 0) {
								Pos = EndPos + Normal * (0.5f * Scale3DRate);
								Vect = (Vector3DX::Reflect(Vect, Normal) * 0.5f);
								yAdd = 0.0f;
							}
						}
						if (loop % 3 == 0) {
							DrawCapsule_3D(RePos.get(), Pos.get(), 0.01f * Scale3DRate, GetColor(192, 0, 0), GetColor(0, 0, 0));
							RePosDraw = Pos;
						}
					}
					SetUseLighting(true);
					SetUseHalfLambertLighting(true);
				}
			}
		}
		void GunObj::Dispose_Sub(void) noexcept {
			this->m_IsMeshDraw.clear();
			
			this->m_GunsModify->DeleteAllSlot();

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
	}
}
