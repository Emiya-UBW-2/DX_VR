#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

#include "../../CommonScene/Object/GunsModify.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool CharacterClass::GetIsADS(void) const noexcept {
			auto& NowGunPtrNow = m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect());
			if (NowGunPtrNow.m_Gun_Ptr->GetCanADS()) {
				if (!m_IsStuckGun && NowGunPtrNow.m_Gun_Ptr->GetCanShot()) {
					return this->m_Input.GetPADSPress(Controls::PADS::AIM);
				}
			}
			return false;
		}
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			if (GetMyPlayerID() == value.DamageID) {
				auto PrevLive = IsAlive();
				this->m_HP.Sub(value.Damage);
				this->m_AP.Sub(value.ArmerDamage);
				if (value.ShotID == 0) {
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						PlayerMngr->GetPlayer(value.ShotID)->AddHit(1);
					}
				}
				if (IsAlive()) {
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_Hurt1) + GetRand(6 - 1))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else {
						//回復
					}
				}
				else if (PrevLive) {
					//死亡
					for (int i = 0; i < 6; i++) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_Hurt1) + i)->StopAll();
					}
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_contact))->StopAll();
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_openfire))->StopAll();
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_reload))->StopAll();
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_takecover))->StopAll();
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing))->StopAll();
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathend))->StopAll();

					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_Death1) + GetRand(8 - 1))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					if (value.ShotID == 0) {
						PlayerMngr->GetPlayer(value.ShotID)->AddScore(100);
						PlayerMngr->GetPlayer(value.ShotID)->AddKill(1);
					}
				}
				return true;
			}
			return false;
		}
		const bool		CharacterClass::CheckDamageRay(HitPoint* Damage, ArmerPoint* ArmerDamage, bool CheckBodyParts, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			if (!IsAlive()) { return false; }
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, this->GetMove().GetPos()) <= 2.0f * Scale3DRate)) { return false; }

			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			//被弾処理
			auto* HitPtr = m_HitBoxControl.GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(AttackID)->GetChara();
				//部位ダメージ演算
				if (CheckBodyParts) {
					switch (HitPtr->GetColType()) {
					case HitType::Head:
						if (GetMyPlayerID() == 0) {
							*Damage = *Damage;
							*ArmerDamage = 0;
						}
						else {
							*Damage = *Damage * 100 / 10;
							*ArmerDamage = 0;
						}
						break;
					case HitType::Body:
						if (m_AP.IsNotZero()) {
							*ArmerDamage = std::clamp((ArmerPoint)*Damage, (ArmerPoint)0, m_AP.GetPoint());
							*Damage -= (HitPoint)*ArmerDamage;
						}
						else {
							*ArmerDamage = 0;
							*Damage = *Damage;
						}
						break;
					case HitType::Arm:
						*Damage = *Damage * 5 / 10;
						*ArmerDamage = 0;
						break;
					case HitType::Leg:
						*Damage = *Damage * 7 / 10;
						*ArmerDamage = 0;
						break;
					default:
						break;
					}
					*Damage = std::min(*Damage, m_HP.GetMax());
					*ArmerDamage = std::min(*ArmerDamage, m_AP.GetMax());
				}
				//ダメージ登録
				{
					//auto v1 = GetEyeRotationCache().zvec() * -1.f;
					//auto v2 = (Chara->GetMove().GetPos() - this->GetMove().GetPos()).normalized(); v2.y = (0);
					//atan2f(Vector3DX::Cross(v1, v2).y, Vector3DX::Dot(v1, v2))
					m_Damage.Add(AttackID, GetMyPlayerID(), *Damage, *ArmerDamage, Vector3DX::up());
				}
				//SE
				if (AttackID == 0) {
					if (*Damage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Hit))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f, 128);
					}
				}
				if (GetMyPlayerID() == 0) {
					if (*Damage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitMe))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f, 255);
					}
				}
				//エフェクトセット
				if (*Damage > 0) {
					m_EffectControl.SetOnce(static_cast<int>(Sceneclass::Effect::ef_hitblood), *pEndPos, Vector3DX::forward(), Scale3DRate);
					m_EffectControl.SetEffectSpeed(static_cast<int>(Sceneclass::Effect::ef_hitblood), 2.f);
				}
				else {
					m_EffectControl.SetOnce(static_cast<int>(Sceneclass::Effect::ef_gndsmoke), *pEndPos, (Chara->GetMove().GetPos() - this->GetMove().GetPos()).normalized(), 0.25f * Scale3DRate);
				}
				//ヒットモーション
				{
					m_HitReactionControl.SetHit(Matrix3x3DX::Vtrans(Vector3DX::Cross((*pEndPos - StartPos).normalized(), Vector3DX::up()) * -1.f, Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper2)).inverse()));
					if (HitPtr->GetColType() == HitType::Leg) {
						this->m_IsSquat = true;
					}
					if (HitPtr->GetColType() == HitType::Arm && !m_ArmBreak) {
						m_ArmBreak = true;
					}
				}
				//todo : ヒットした部分に近い頂点を赤くする
				return true;
			}
			return false;
		}
		//操作
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* CameraParts = Camera3D::Instance();
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			//
			if (IsAlive()) {
				if (IsLowHP()) {
					m_HPRec += DXLib_refParts->GetDeltaTime();
					if (m_HPRec >= 0.f) {
						m_HPRec -= 2.f;
						Heal(2);
						if (GetMyPlayerID() == 0) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing))->Play(DX_PLAYTYPE_BACK);
						}
					}
				}
				else {
					if (m_HPRec != 0.f) {
						if (GetMyPlayerID() == 0) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing))->StopAll();
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathend))->Play(DX_PLAYTYPE_BACK);
						}
					}
					m_HPRec = 0.f;
				}
			}
			//AIM
			auto& NowGunPtrNow = m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect());
			this->m_Squat.Update(this->m_Input.GetPADSPress(Controls::PADS::SQUAT));
			if (this->m_Squat.trigger()) {
				this->m_IsSquat ^= 1;
				//しゃがみ音
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
			}
			if (NowGunPtrNow.m_Gun_Ptr) {
				NowGunPtrNow.m_Gun_Ptr->SetShotSwitchOff();
				//リコイルの演算
				if (this->m_RecoilRadAdd.y < 0.f) {
					Easing(&this->m_RecoilRadAdd, Vector3DX::vget(0.f, 0.01f, 0.f), NowGunPtrNow.m_Gun_Ptr->GetRecoilReturn(), EasingType::OutExpo);
				}
				else {
					Easing(&this->m_RecoilRadAdd, Vector3DX::zero(), 0.7f, EasingType::OutExpo);
				}
				//アクション

				//武器切替
				if (NowGunPtrNow.m_Gun_Ptr->GetGunAnime() != GunAnimeID::LowReady) {
					int Wheel = 0;//PadControl::Instance()->GetMouseWheelRot();
					if (this->m_Input.GetPADSPress(Controls::PADS::ULT) || (Wheel != 0)) {
						m_GunPtrControl.GunChangeNext((Wheel < 0));
					}
				}
				switch (NowGunPtrNow.m_Gun_Ptr->GetGunAnime()) {
				case GunAnimeID::LowReady:
					m_MagHand = false;
					if (m_GunPtrControl.IsChangeGunSel() && NowGunPtrNow.m_Gun_Ptr->GetGunAnimBlendPer(NowGunPtrNow.m_Gun_Ptr->GetGunAnime()) > 0.95f) {
						m_GunPtrControl.InvokeReserveGunSel();
					}
					break;
				case GunAnimeID::None:
					if (NowGunPtrNow.m_Gun_Ptr->GetCanShot()) {
						//射撃
						if (this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
							if (NowGunPtrNow.m_Gun_Ptr->GetInChamber()) {
								auto* PlayerMngr = Player::PlayerManager::Instance();
								PlayerMngr->GetPlayer(GetMyPlayerID())->AddShot(NowGunPtrNow.m_Gun_Ptr->GetPelletNum());
								NowGunPtrNow.m_Gun_Ptr->SetShotStart();
								//リコイル
								float Power = 0.0001f * NowGunPtrNow.m_Gun_Ptr->GetRecoilPower();
								this->m_RecoilRadAdd.Set(GetRandf(Power / 4.f), -Power, 0.f);
								//ビジュアルリコイル
								if (GetMyPlayerID() == 0) {
									Camera3D::Instance()->SetCamShake(0.1f, 0.1f);
								}
								//エフェクト
								auto mat = NowGunPtrNow.m_Gun_Ptr->GetFrameWorldMat_P(GunFrame::Muzzle);
								switch (NowGunPtrNow.m_Gun_Ptr->GetGunShootSound()) {
								case GunShootSound::Normal:
									m_EffectControl.SetOnce(static_cast<int>(Sceneclass::Effect::ef_fire2), mat.pos(), mat.zvec() * -1.f, 0.5f);
									m_EffectControl.SetEffectSpeed(static_cast<int>(Sceneclass::Effect::ef_fire2), 2.f);
									break;
								case GunShootSound::Suppressor:
									m_EffectControl.SetOnce(static_cast<int>(Sceneclass::Effect::ef_fire2), mat.pos(), mat.zvec() * -1.f, 0.25f);
									m_EffectControl.SetEffectSpeed(static_cast<int>(Sceneclass::Effect::ef_fire2), 2.f);
									break;
								default:
									break;
								}
							}
						}
						//Reload
						bool Press_Reload = this->m_Input.GetPADSPress(Controls::PADS::RELOAD);
						Press_Reload |= (NowGunPtrNow.m_Gun_Ptr->GetAmmoNumTotal() == 0) && this->m_Input.GetPADSPress(Controls::PADS::SHOT);				//打ち切りで自動リロード
						if (Press_Reload) {
							NowGunPtrNow.m_Gun_Ptr->ReloadStart();
						}
						//武器を眺める
						if (this->m_Input.GetPADSPress(Controls::PADS::CHECK)) {
							NowGunPtrNow.m_Gun_Ptr->SetGunAnime(GunAnimeID::Watch);
						}
						if (!NowGunPtrNow.m_Gun_Ptr->IsThrowWeapon()) {
							//グレネード構え
							if (this->m_Input.GetPADSPress(Controls::PADS::THROW)) {
								//投げ武器である最初の武器に切り替え
								m_GunPtrControl.GunChangeThrowWeapon(true);
							}
						}
						else {
							//グレネード投てき
							if (this->m_Input.GetPADSPress(Controls::PADS::THROW) || this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Pin))->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
								NowGunPtrNow.m_Gun_Ptr->SetGunAnime(GunAnimeID::ThrowReady);
								m_ThrowByShot = this->m_Input.GetPADSPress(Controls::PADS::SHOT);
							}
						}
					}
					break;
				case GunAnimeID::Shot:
					//射撃終了フラグ
					switch (NowGunPtrNow.m_Gun_Ptr->GetShotType()) {
					case SHOTTYPE::FULL:
						NowGunPtrNow.m_Gun_Ptr->SetShotEnd(NowGunPtrNow.m_Gun_Ptr->GetInChamber() || NowGunPtrNow.m_Gun_Ptr->GetIsMagEmpty());
						break;
					case SHOTTYPE::SEMI:
					case SHOTTYPE::BOLT:
						NowGunPtrNow.m_Gun_Ptr->SetShotEnd(!this->m_Input.GetPADSPress(Controls::PADS::SHOT));
						break;
					default:
						break;
					}
					break;
				case GunAnimeID::ReloadStart_Empty:
				case GunAnimeID::ReloadStart:
					if (GetMyPlayerID() != 0) {
						if (!m_MagHand) {
							if (GetRand(100) < 50) {
								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_reload))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
							}
							else {
								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_takecover))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
							}
						}
					}
					m_MagHand = true;
					break;
				case GunAnimeID::Reload:
					if (this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
						NowGunPtrNow.m_Gun_Ptr->SetReloadAmmoCancel();
					}
					switch (NowGunPtrNow.m_Gun_Ptr->GetReloadType()) {
					case RELOADTYPE::MAG:
						m_MagHand = true;
						break;
					case RELOADTYPE::AMMO:
						m_MagHand = (NowGunPtrNow.m_Gun_Ptr->GetGunAnimePer(NowGunPtrNow.m_Gun_Ptr->GetGunAnime()) < 0.5f);
						break;
					default:
						break;
					}

					break;
				case GunAnimeID::ReloadEnd:
					switch (NowGunPtrNow.m_Gun_Ptr->GetReloadType()) {
					case RELOADTYPE::MAG:
						if (NowGunPtrNow.m_Gun_Ptr->GetGunAnimePer(NowGunPtrNow.m_Gun_Ptr->GetGunAnime()) > 0.6f) {
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
				case GunAnimeID::ThrowReady:
					if (NowGunPtrNow.m_Gun_Ptr->GetGunAnimePer(NowGunPtrNow.m_Gun_Ptr->GetGunAnime()) >= 1.f) {
						if (m_ThrowByShot ? !this->m_Input.GetPADSPress(Controls::PADS::SHOT) : !this->m_Input.GetPADSPress(Controls::PADS::THROW)) {
							NowGunPtrNow.m_Gun_Ptr->SetGunAnime(GunAnimeID::Throw);
						}
					}
					break;
				case GunAnimeID::Throw:
					if (NowGunPtrNow.m_Gun_Ptr->GetGunAnimePer(NowGunPtrNow.m_Gun_Ptr->GetGunAnime()) >= 0.4f) {
						if (NowGunPtrNow.m_Gun_Ptr->IsActive()) {
							NowGunPtrNow.m_Gun_Ptr->SetActiveAll(false);
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Throw))->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
							m_Grenade.SetFall(
								GetFrameWorldMat(CharaFrame::RightHandJoint).pos(),
								GetEyeRotationCache(),
								(GetEyeRotationCache().zvec() * -1.f).normalized() * (Scale3DRate * 15.f / 60.f), 3.5f, SoundEnum::FallGrenade, true);
						}
					}
					if (NowGunPtrNow.m_Gun_Ptr->GetGunAnimePer(NowGunPtrNow.m_Gun_Ptr->GetGunAnime()) >= 0.6f) {
						//投げ武器ではない最初の武器に切り替え
						m_GunPtrControl.GunChangeThrowWeapon(false);
					}
					break;
				default:
					break;
				}

				m_MagArm.Update(m_MagHand, 0.1f, 0.1f, 0.7f, 0.7f);
			}
			this->m_MoveControl.Update(
				this->m_Input.GetPADSPress(Controls::PADS::MOVE_W),
				this->m_Input.GetPADSPress(Controls::PADS::MOVE_A),
				this->m_Input.GetPADSPress(Controls::PADS::MOVE_S),
				this->m_Input.GetPADSPress(Controls::PADS::MOVE_D));

			auto yRadFront = IsMoveFront() ? (atan2f(m_MoveControl.GetVecTotal().x, -m_MoveControl.GetVecTotal().z) * -m_MoveControl.GetVecTotal().z) : 0.f;
			yRadFront += IsMoveBack() ? (atan2f(-m_MoveControl.GetVecTotal().x, m_MoveControl.GetVecTotal().z) * m_MoveControl.GetVecTotal().z) : 0.f;
			this->m_RotateControl.Update(this->m_Input.GetAddxRad(), this->m_Input.GetAddyRad(), Vector2DX::zero(), m_MoveControl.GetVecPower() > 0.1f, yRadFront);
			//リーン
			this->m_LeanControl.Update(this->m_Input.GetPADSPress(Controls::PADS::LEAN_L), this->m_Input.GetPADSPress(Controls::PADS::LEAN_R));
			if (m_LeanControl.GetSwitch()) {
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
			}
			if (NowGunPtrNow.m_Gun_Ptr->GetCanSwitch()) {
				Easing(&m_SwitchPer, (m_LeanControl.GetRate() <= 0 || NowGunPtrNow.m_Gun_Ptr->GetReloading()) ? 1.f : -1.f, 0.9f, EasingType::OutExpo);
			}
			else {
				m_SwitchPer = 1.f;
			}
			//下半身
			this->m_BottomAnimSelect = GetBottomStandAnimSel();
			if (IsMoveLeft()) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
			if (IsMoveRight()) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
			if (IsMoveBack()) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
			if (IsMoveFront()) { this->m_BottomAnimSelect = GetBottomWalkAnimSel(); }
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(GetBottomTurnAnimSel())), (!this->m_IsSquat && m_RotateControl.IsTurnBody()) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
			for (int i = 0; i < static_cast<int>(CharaAnimeID::AnimeIDMax); i++) {
				CharaAnimeID ID = static_cast<CharaAnimeID>(i);
				if (
					ID == CharaAnimeID::Bottom_Stand ||
					ID == CharaAnimeID::Bottom_Stand_Run ||
					ID == CharaAnimeID::Bottom_Stand_Walk ||
					ID == CharaAnimeID::Bottom_Stand_LeftStep ||
					ID == CharaAnimeID::Bottom_Stand_RightStep ||
					ID == CharaAnimeID::Bottom_Stand_WalkBack) {
					this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((ID == this->m_BottomAnimSelect) ? 6.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				}
				if (
					ID == CharaAnimeID::Bottom_Squat ||
					ID == CharaAnimeID::Bottom_Squat_Walk ||
					ID == CharaAnimeID::Bottom_Squat_LeftStep ||
					ID == CharaAnimeID::Bottom_Squat_RightStep ||
					ID == CharaAnimeID::Bottom_Squat_WalkBack) {
					this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((ID == this->m_BottomAnimSelect) ? 2.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				}
			}
			//足音
			if (m_BottomAnimSelect != GetBottomStandAnimSel()) {
				auto Time = this->GetObj().SetAnim(static_cast<int>(this->m_BottomAnimSelect)).GetTime();
				//L
				if ((9.f < Time && Time < 10.f)) {
					if (this->m_CharaSound != 0) {
						this->m_CharaSound = 0;
						if ((CameraParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::LeftFoot).pos()).magnitude() < Scale3DRate * 5.f * 1.5f) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale3DRate * 5.f);
						}
					}
				}
				//R
				if ((27.f < Time && Time < 28.f)) {
					if (this->m_CharaSound != 1) {
						this->m_CharaSound = 1;
						if ((CameraParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::RightFoot).pos()).magnitude() < Scale3DRate * 5.f * 1.5f) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale3DRate * 5.f);
						}
					}
				}
			}
			else {
				this->m_CharaSound = -1;
			}
		}
		//SetMat指示更新
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* OptionParts = OptionManager::Instance();
			//
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)),
				(Matrix3x3DX::Get33DX(GetFrameLocalMat(CharaFrame::Upper)) * Matrix3x3DX::RotAxis(Vector3DX::right(), -m_RotateControl.GetRad().x / 2.f) * GetCharaLocalDir()).Get44DX()
				* GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper)));
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper2)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper2)),
				(Matrix3x3DX::Get33DX(GetFrameLocalMat(CharaFrame::Upper2)) * Matrix3x3DX::RotAxis(Vector3DX::right(), m_RotateControl.GetRad().x / 2.f) * m_HitReactionControl.GetHitReactionMat()).Get44DX()
				* GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper2)));
			m_HitReactionControl.Update();
			//上半身演算
			for (int loop = 0; loop < m_GunPtrControl.GetGunNum(); loop++) {
				auto& p = m_GunPtrControl.GetParam(loop).m_Gun_Ptr;
				if (p) {
					bool IsSelGun = loop == m_GunPtrControl.GetNowGunSelect();
					if (IsSelGun) {
						p->UpdateGunAnime(GetIsADS(), false);
						p->UpdateGunAnims();
					}
					else {
						p->UpdateGunAnime(false, true);
					}
				}
			}
			//上半身アニメ演算
			this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Upper_Ready)) = 1.f;
			this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Hand_Ready)) = 1.f;
			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomTurnAnimSel()), 0.5f);
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaAnimeID::Bottom_Stand_Run), GetSpeed() * 0.5f);
			ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomWalkAnimSel()), m_MoveControl.GetVecFront() * 1.15f * GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomLeftStepAnimSel()), m_MoveControl.GetVecLeft() * 1.15f * GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomWalkBackAnimSel()), m_MoveControl.GetVecRear() * 1.15f * GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomRightStepAnimSel()), m_MoveControl.GetVecRight() * 1.15f * GetSpeedPer());
			//アニメ反映
			for (int i = 0; i < GetObj().GetAnimNum(); i++) {
				this->GetObj().SetAnim(i).SetPer(this->m_AnimPerBuf.at(i));
			}
			GetObj().UpdateAnimAll();
			//移動の際の視点制御
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			//vector
			Vector3DX PosBuf = this->GetMove().GetPos();
			Vector3DX EndPos = PosBuf - Vector3DX::up() * (0.5f * Scale3DRate);
			bool IsHitGround = BackGround->CheckLinetoMap(PosBuf + Vector3DX::up() * (0.f * Scale3DRate), &EndPos);
			Vector3DX vec = Vector3DX::zero();
			if (m_MoveControl.GetVecPower() > 0.f) {
				vec = Matrix3x3DX::Vtrans(m_MoveControl.GetVecTotal().normalized() * GetSpeed(), Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetYRadUpper()));
			}
			if (IsHitGround) {
				auto yPos = PosBuf.y;
				Easing(&yPos, EndPos.y - (0.12f * Scale3DRate), 0.6f, EasingType::OutExpo);
				PosBuf.y = (yPos);

				vec.y = 0.f;
				this->SetMove().SetVec(vec);
			}
			else {
				float Y = this->GetMove().GetVec().y;
				vec.y = (Y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps())));
				this->SetMove().SetVec(vec);
			}
			PosBuf += this->GetMove().GetVec();
			//壁判定
			BackGround->CheckMapWall(this->m_move.GetRePos(), &PosBuf, Vector3DX::up() * (0.6f * Scale3DRate + 0.1f), Vector3DX::up() * (1.6f * Scale3DRate), 0.6f * Scale3DRate);
			//ほかプレイヤーとの判定
			{
				float Radius = 2.f * 0.5f * Scale3DRate;
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
					if (!c->IsAlive()) { continue; }
					//自分が当たったら押し戻す
					Vector3DX Vec = (c->GetMove().GetPos() - this->GetMove().GetPos()); Vec.y = (0.f);
					float Len = Vec.magnitude();
					if (Len < Radius) {
						PosBuf = PosBuf + Vec.normalized() * (Len - Radius);
					}
				}
			}
			//座標オーバーライド
			if (this->m_MoveOverRideFlag) {
				this->m_MoveOverRideFlag = false;
				this->m_move = this->m_OverRideInfo;
			}
			this->SetMove().SetPos(PosBuf);
			this->SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetYRadBottom()));
			this->SetMove().Update(0.5f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			m_CharaRotationCache = GetCharaLocalDir() * this->GetMove().GetMat();
			auto& NowGunPtrNow = m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect());
			if (NowGunPtrNow.m_Gun_Ptr) {
				for (int loop = 0; loop < m_GunPtrControl.GetGunNum(); loop++) {
					auto& p = m_GunPtrControl.GetParam(loop).m_Gun_Ptr;
					if (p) {
						//スリング場所探索
						switch (loop) {
						case 0://メイン武器のスリング
							if (m_GunPtrControl.GetParam(loop).m_SlingPer < 1.f) {
								m_SlingZrad.Update(DXLib_refParts->GetDeltaTime());
								m_SlingZrad.AddRad((0.5f * (m_RotateControl.GetRad().y - m_RotateControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());

								m_GunPtrControl.GetParam(loop).SetGunMountMat(
									Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
									(
										Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingZrad.GetRad()) *
										Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetRad().y) *
										m_CharaRotationCache.inverse()
										),
									GetFrameWorldMat(CharaFrame::Head).pos() +
									GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.3f * Scale3DRate) +
									(
										this->GetMove().GetMat().xvec() * sin(m_SlingZrad.GetRad()) +
										this->GetMove().GetMat().yvec() * cos(m_SlingZrad.GetRad())
										) * -(0.5f * Scale3DRate)
								);
							}
							break;
						case 1://ホルスター
						case 2://一応同じ
							if (m_GunPtrControl.GetParam(loop).m_SlingPer < 1.f) {
								m_GunPtrControl.GetParam(loop).SetGunMountMat(
									Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-90)) *
									Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper)) *
									m_CharaRotationCache.inverse(),
									GetFrameWorldMat(CharaFrame::Upper).pos() +
									GetFrameWorldMat(CharaFrame::Upper).yvec() * (-0.5f * Scale3DRate) +
									GetFrameWorldMat(CharaFrame::Upper).xvec() * (-0.3f * Scale3DRate)
								);
							}
							break;
						default:
							break;
						}
					}
				}
				//銃ひっこめ
				if (m_StuckGunTimer == 0.f) {
					m_StuckGunTimer = 0.1f;
					auto EndPost = NowGunPtrNow.m_Gun_Ptr->GetFrameWorldMat_P(GunFrame::Muzzle).pos();
					if (BackGround->CheckLinetoMap(GetEyePositionCache(), &EndPost)) {//0.03ms
						if ((NowGunPtrNow.m_Gun_Ptr->GetGunAnime() == GunAnimeID::LowReady) || NowGunPtrNow.m_Gun_Ptr->IsCanShoot()) {
							m_IsStuckGun = true;
						}
					}
					else {
						if (m_IsStuckGun) {
							m_IsStuckGun = false;
							if (NowGunPtrNow.m_Gun_Ptr->GetGunAnime() == GunAnimeID::LowReady) {
								NowGunPtrNow.m_Gun_Ptr->SetGunAnime(GunAnimeID::None);
							}
						}
					}
				}
				else {
					m_StuckGunTimer = std::max(m_StuckGunTimer - DXLib_refParts->GetDeltaTime(), 0.f);
					if (m_IsStuckGun) {
						if (NowGunPtrNow.m_Gun_Ptr->GetGunAnime() != GunAnimeID::LowReady) {
							NowGunPtrNow.m_Gun_Ptr->SetGunAnime(GunAnimeID::LowReady);
						}
					}
				}
				//オートエイム
				m_AutoAimControl.Update( GetIsADS(), GetMyPlayerID(),
					GetEyePositionCache(),
					NowGunPtrNow.m_Gun_Ptr->GetFrameWorldMat_P(GunFrame::Muzzle).zvec() * -1.f,
					GetAutoAimRadian());
				//
				if (IsAlive()) {
					//銃の位置を指定するアニメ
					Matrix4x4DX AnimMat = NowGunPtrNow.m_Gun_Ptr->GetGunAnimeNow();
					Matrix3x3DX AnimRot = Matrix3x3DX::Get33DX(AnimMat);
					Vector3DX AnimPos = AnimMat.pos();
					AnimPos.x *= this->m_SwitchPer;
					Vector3DX Post0 = GetFrameWorldMat(CharaFrame::Head).pos() + Matrix3x3DX::Vtrans(AnimPos, m_CharaRotationCache);
					//
					for (int loop = 0; loop < m_GunPtrControl.GetGunNum(); loop++) {
						bool IsSelGun = (loop == m_GunPtrControl.GetNowGunSelect());
						auto& p = m_GunPtrControl.GetParam(loop).m_Gun_Ptr;
						if (!p) { continue; }
						m_GunPtrControl.GetParam(loop).Update(IsSelGun);
						Matrix3x3DX tmp_gunmat =
							m_GunPtrControl.GetParam(loop).GetGunRotMat(AnimRot) *
							m_RotateControl.GetGunSwingMat() * m_CharaRotationCache *
							p->GetEyeYRot();
						//オートエイム
						if (IsSelGun) {
							m_AutoAimControl.CalcAutoAimMat(&tmp_gunmat);
						}
						p->UpdateGunMat(tmp_gunmat, m_GunPtrControl.GetParam(loop).GetGunPos(Post0));
						//LeftMag
						p->SetMagazinePoachMat(GetFrameWorldMat(CharaFrame::LeftMag));
						p->UpdateMagazineMat(IsSelGun);
						if (!IsSelGun) {
							//銃表示
							p->SetActiveAll(true);
						}
					}
				}
				else {
					for (int loop = 0; loop < m_GunPtrControl.GetGunNum(); ++loop) {
						bool IsSelGun = (loop == m_GunPtrControl.GetNowGunSelect());
						auto& p = m_GunPtrControl.GetParam(loop).m_Gun_Ptr;
						if (!p) { continue; }
						if (IsSelGun) {
							auto Mat = m_RagDollControl.lagframe_.RIGHThand_f.GetFrameWorldPosition().rotation().inverse() * m_RagDollControl.GetRagDoll().GetFrameLocalWorldMatrix(m_RagDollControl.lagframe_.RIGHThand_f.GetFrameID());
							p->UpdateGunMat(Matrix3x3DX::Get33DX(Mat), Mat.pos());
							p->SetActiveAll(true);
						}
						else {
							p->SetActiveAll(false);
						}
					}
				}
				//手の位置を制御
				if ((GetMyPlayerID() == 0) || this->m_CanLookTarget) {
					//銃座標
					Matrix4x4DX HandMat;
					{
						HandMat = NowGunPtrNow.m_Gun_Ptr->GetRightHandMat();
						IK_RightArm(
							&GetObj(),
							GetFrame(static_cast<int>(CharaFrame::RightArm)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
							GetFrame(static_cast<int>(CharaFrame::RightArm2)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
							GetFrame(static_cast<int>(CharaFrame::RightWrist)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
							HandMat.pos(), HandMat.yvec(), HandMat.zvec());
					}
					{
						HandMat = Lerp(NowGunPtrNow.m_Gun_Ptr->GetLeftHandMat(), NowGunPtrNow.m_Gun_Ptr->GetMagHandMat(), m_MagArm.Per());

						Easing(&m_ArmBreakPer, ((NowGunPtrNow.m_Gun_Ptr->IsCanShoot() && m_ArmBreak) || NowGunPtrNow.m_Gun_Ptr->IsThrowWeapon()) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
						if (m_ArmBreakPer > 0.01f) {
							m_SlingArmZrad.Update(DXLib_refParts->GetDeltaTime());
							m_SlingArmZrad.AddRad((0.5f * (m_RotateControl.GetRad().y - m_RotateControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());
							Matrix4x4DX SlingArmMat =
								(
									Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(180))*
									Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
									(
										Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingArmZrad.GetRad()) *
										Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetRad().y)
										)
									).Get44DX() * Matrix4x4DX::Mtrans(
								GetFrameWorldMat(CharaFrame::Head).pos() +
								GetFrameWorldMat(CharaFrame::Head).xvec() * (0.15f * Scale3DRate) +
								GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.1f * Scale3DRate) +
								(
									this->GetMove().GetMat().xvec() * sin(m_SlingArmZrad.GetRad()) +
									this->GetMove().GetMat().yvec() * cos(m_SlingArmZrad.GetRad())
									) * -(0.5f * Scale3DRate)
									);

							HandMat = Lerp(HandMat, SlingArmMat, m_ArmBreakPer);
						}
						if (m_ArmBreak) {
							HandMat = HandMat.rotation() * Matrix4x4DX::Mtrans(HandMat.pos() + Vector3DX::vget(GetRandf(1.f), GetRandf(1.f), GetRandf(1.f)) * (0.002f * Scale3DRate));
						}
						IK_LeftArm(
							&GetObj(),
							GetFrame(static_cast<int>(CharaFrame::LeftArm)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)),
							GetFrame(static_cast<int>(CharaFrame::LeftArm2)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)),
							GetFrame(static_cast<int>(CharaFrame::LeftWrist)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)),
							HandMat.pos(), HandMat.yvec(), HandMat.zvec());
					}
				}
			}
			m_RagDollControl.Update(this->GetObj(), IsAlive());													//ラグドール
			m_HitBoxControl.Update(this, (this->GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);									//ヒットボックス
			m_EyeSwingControl.Update(m_CharaRotationCache, std::clamp(m_MoveControl.GetVecPower() / 0.65f, 0.f, 0.85f / 0.65f), 5.f);
			bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			{

				auto EyePosition = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f;
				if (HeadBobbing) {
					EyePosition += m_EyeSwingControl.GetEyeSwingPos();
				}
				if (NowGunPtrNow.m_Gun_Ptr) {
					EyePosition = NowGunPtrNow.m_Gun_Ptr->GetGunPosition(EyePosition);
				}
				m_EyePositionCache = EyePosition;
			}
			m_WalkSwingControl.Update(GetEyePositionCache() - GetMove().GetPos(), std::clamp(this->GetMove().GetVec().magnitude() / 2.f, 0.f, 0.5f));
			{
				Matrix3x3DX tmpUpperMatrix{};
				if (HeadBobbing) {
					tmpUpperMatrix = m_WalkSwingControl.GetWalkSwingMat();
				}
				tmpUpperMatrix *= 
					Matrix3x3DX::RotAxis(Vector3DX::forward(), m_LeanControl.GetRad() / 5.f) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), m_RotateControl.GetRad().x) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetYRadBottomChange()) *
					this->GetMove().GetMat();
				m_EyeRotationCache = tmpUpperMatrix;
			}
		}
		//グレネード更新
		void			CharacterClass::ExecuteGrenade(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();

			for (auto& g : m_Grenade.m_Ptr) {
				if (g->PopGrenadeBombSwitch()) {
					m_EffectControl.SetOnce(static_cast<int>(Sceneclass::Effect::ef_greexp), g->GetMove().GetPos(), Vector3DX::forward(), 0.5f * Scale3DRate);
					m_EffectControl.SetEffectSpeed(static_cast<int>(Sceneclass::Effect::ef_greexp), 2.f);
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Explosion))->Play3D(g->GetMove().GetPos(), Scale3DRate * 25.f);

					for (int i = 0, max = m_ChamberAmmoData->GetPellet(); i < max; i++) {
						auto LastAmmo = std::make_shared<AmmoClass>();
						ObjMngr->AddObject(LastAmmo);
						LastAmmo->Init();
						auto mat =
							Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-GetRand(30))) *
							Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180)));
						LastAmmo->Put(this->m_ChamberAmmoData, g->GetMove().GetPos() + mat.zvec() * (0.5f * Scale3DRate) + Vector3DX::up() * (0.5f * Scale3DRate), mat.zvec(), GetMyPlayerID());
					}

					//破壊
					int								xput = 6;
					int								yput = 8;
					int								zput = 6;
					Vector3DX						PutPos = g->GetMove().GetPos();
					auto Put = BackGround->GetPoint(PutPos);
					for (int xp = 0; xp < xput; xp++) {
						for (int yp = 0; yp < yput; yp++) {
							for (int zp = 0; zp < zput; zp++) {
								auto& cell = BackGround->GetCellBuf((Put.x + xp - xput / 2), (Put.y + yp), (Put.z + zp - zput / 2));
								if (cell.m_Cell == 1) {
									continue;
								}
								BackGround->SetBlick((Put.x + xp - xput / 2), (Put.y + yp), (Put.z + zp - zput / 2), FPS_n2::BackGround::s_EmptyBlick);
							}
						}
					}
				}
			}
		}
		//
		void			CharacterClass::LoadChara(const std::string& FolderName, PlayerID ID) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto& p = PlayerMngr->GetPlayer(ID);

			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";

			auto Ptr = std::make_shared<CharacterClass>();
			ObjMngr->AddObject(Ptr);
			ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
			Ptr->Init();
			p->SetChara(Ptr);
			p->SetAI(std::make_shared<AIControl>());
			p->GetAI()->Init(ID);
		}
		void			CharacterClass::LoadCharaGun(const std::string& FolderName, int Sel, bool IsPreset) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			std::string Path = "data/gun/";
			Path += FolderName;
			Path += "/";

			auto Ptr = std::make_shared<GunClass>();
			ObjMngr->AddObject(Ptr);
			ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
			Ptr->SetupModSlot();
			GunsModify::Instance()->CreateSelData(Ptr, IsPreset);
			Ptr->Init();
			m_GunPtrControl.SetGunPtr(Sel, Ptr);
		}
		void			CharacterClass::Spawn(float pxRad, float pyRad, const Vector3DX& pPos, int GunSel) noexcept {
			this->m_HP.Init();
			this->m_AP.Init();
			Heal(100);
			m_ArmBreak = false;
			m_ArmBreakPer = 0.f;
			m_SlingArmZrad.Init(0.08f * Scale3DRate, 3.f, deg2rad(50));
			m_HPRec = 0.f;

			this->m_MagArm.Init(false);
			this->m_MagHand = false;
			m_EyeSwingControl.Init();
			this->m_MoveOverRideFlag = false;
			this->m_Input.ResetAllInput();
			this->m_RotateControl.Init(pxRad, pyRad);
			this->m_MoveControl.Init();
			this->m_LeanControl.Init();
			for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
			this->m_IsSquat = false;
			SetMove().SetAll(pPos, pPos, pPos, Vector3DX::zero(), Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetRad().y), Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetRad().y));
			//
			m_GunPtrControl.SelectGun(GunSel, true);
			m_SlingZrad.Init(0.05f * Scale3DRate, 3.f, deg2rad(50));
		}
		//
		void			CharacterClass::Init_Sub(void) noexcept {
			m_HitBoxControl.Init();
			m_ArmBreak = false;
			m_EffectControl.Init();

			m_Grenade.Dispose();
			m_Grenade.Init("data/model/RGD5/", 4);	//装填したマガジンの弾に合わせて薬莢生成
			m_ChamberAmmoData = AmmoDataManager::Instance()->LoadAction("data/ammo/FragRGD/");
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				int num = GetObj().GetMaterialNum();
				for (int i = 0; i < num; i++) {
					GetObj().SetMaterialDifColor(i, GetColorF(0.8f, 0.8f, 0.8f, 1.f));
					GetObj().SetMaterialAmbColor(i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
				}
			}
			ExecuteInput();
			ExecuteMatrix();
			ExecuteGrenade();
			m_EffectControl.Execute();
		}
		void			CharacterClass::CheckDraw(void) noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();

			this->m_IsDraw = false;
			this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetScreenPosition()).magnitude();

			if (CheckCameraViewClip_Box(
				(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f, 0.f, -2.5f) * Scale3DRate).get(),
				(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f, 2.5f, 2.5f) * Scale3DRate).get()) == FALSE
				) {
				this->m_IsDraw |= true;
			}

			if (GetMyPlayerID() != 0) {
				Vector3DX EyePos = ConvWorldPosToScreenPos(GetEyePositionCache().get());
				if (0.f < EyePos.z && EyePos.z < 1.f) {
					m_CameraPos.x = static_cast<float>(static_cast<int>(EyePos.x * 1980 / WindowSizeParts->GetScreenY(1980)));
					m_CameraPos.y = static_cast<float>(static_cast<int>(EyePos.y * 1080 / WindowSizeParts->GetScreenY(1080)));
					m_CameraPos.z = EyePos.z;
				}
			}
		}
		void			CharacterClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if ((CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f * Scale3DRate, -0.5f * Scale3DRate, -2.5f * Scale3DRate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f * Scale3DRate, 2.f * Scale3DRate, 2.5f * Scale3DRate)).get()) == FALSE)
					) {
					//
					int fog_enable;
					int fog_mode;
					int fog_r, fog_g, fog_b;
					float fog_start, fog_end;
					float fog_density;
					fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効 FALSE:無効 )
					fog_mode = GetFogMode();														// フォグモードを取得する
					GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
					GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
					fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )


					//キャラ描画
					SetFogEnable(TRUE);
					SetFogColor(0, 0, 0);
					//MV1SetMaterialTypeAll(this->GetObj().GetHandle(), DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
					if (IsAlive()) {
						for (int i = 0; i < this->GetObj().GetMeshNum(); i++) {
							if (this->GetObj().GetMeshSemiTransState(i) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
					}
					else {
						for (int i = 0; i < m_RagDollControl.GetRagDoll().GetMeshNum(); i++) {
							if (m_RagDollControl.GetRagDoll().GetMeshSemiTransState(i) == isDrawSemiTrans) {
								m_RagDollControl.GetRagDoll().DrawMesh(i);
							}
						}
					}
					//hitbox描画
					//m_HitBoxControl.DrawHitBox();
					//
					SetFogEnable(fog_enable);
					SetFogMode(fog_mode);
					SetFogColor(fog_r, fog_g, fog_b);
					SetFogStartEnd(fog_start, fog_end);
					SetFogDensity(fog_density);
				}
			}
		}
	};
};
