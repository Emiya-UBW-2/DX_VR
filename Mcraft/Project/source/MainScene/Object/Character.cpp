#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

#include "../../CommonScene/Object/GunsModify.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const bool CharacterClass::GetIsADS(void) const noexcept {
			if (!m_IsStuckGun && GetGunPtrNow()->GetCanADS()) {
				return this->m_Input.GetPADSPress(Controls::PADS::AIM);
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
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, GetMove().GetPos()) <= 2.0f * Scale3DRate)) { return false; }

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
					//auto v2 = (Chara->GetMove().GetPos() - GetMove().GetPos()).normalized(); v2.y = (0);
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
					m_EffectControl.SetOnce(static_cast<int>(Sceneclass::Effect::ef_gndsmoke), *pEndPos, (Chara->GetMove().GetPos() - GetMove().GetPos()).normalized(), 0.25f * Scale3DRate);
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
			auto* BackGround = BackGround::BackGroundClass::Instance();
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
			this->m_Squat.Update(this->m_Input.GetPADSPress(Controls::PADS::SQUAT));
			if (this->m_Squat.trigger()) {
				this->m_IsSquat ^= 1;
				//しゃがみ音
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
			}
			if (GetGunPtrNow()) {
				//銃ひっこめ
				if (m_StuckGunTimer == 0.f) {
					m_StuckGunTimer = 0.1f;
					if (BackGround->CheckLinetoMap(GetEyePositionCache(), GetGunPtrNow()->GetFrameWorldMatParts(GunFrame::Muzzle).pos())) {//0.03ms
						switch (GetGunPtrNow()->GetGunAnime()) {
						case GunAnimeID::LowReady:
						case GunAnimeID::None:
						case GunAnimeID::Shot:
							m_IsStuckGun = true;
							break;
						default:
							break;
						}
					}
					else {
						if (m_IsStuckGun) {
							m_IsStuckGun = false;
							if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::LowReady) {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::None);
							}
						}
					}
				}
				else {
					m_StuckGunTimer = std::max(m_StuckGunTimer - DXLib_refParts->GetDeltaTime(), 0.f);
					if (m_IsStuckGun) {
						if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::LowReady) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
						}
					}
				}
				//武器切替
				if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::LowReady) {
					int Wheel = 0;//PadControl::Instance()->GetMouseWheelRot();
					if (this->m_Input.GetPADSPress(Controls::PADS::ULT)) {
						Wheel = -1;
					}
					if (Wheel != 0) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
						m_GunPtrControl.GunChangeNext((Wheel < 0));
					}
				}
				//アクション
				switch (GetGunPtrNow()->GetGunAnime()) {
				case GunAnimeID::LowReady:
					if (m_GunPtrControl.IsChangeGunSel() && GetGunPtrNow()->GetGunAnimBlendPer(GunAnimeID::LowReady) > 0.95f) {
						m_GunPtrControl.InvokeReserveGunSel();
						GetGunPtrNow()->SetGunAnime(GunAnimeID::None);
					}
					break;
				case GunAnimeID::None:
					if (GetGunPtrNow()->GetCanShot()) {
						//射撃
						if (this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
							if (GetGunPtrNow()->CanShootAmmo()) {
								GetGunPtrNow()->SetShotStart();
								//ビジュアルリコイル
								if (GetMyPlayerID() == 0) {
									Camera3D::Instance()->SetCamShake(0.1f, 0.1f);
								}
								//エフェクト
								m_EffectControl.SetOnce(static_cast<int>(Sceneclass::Effect::ef_fire2), GetGunPtrNow()->GetFrameWorldMatParts(GunFrame::Muzzle).pos(), GetMove().GetMat().zvec() * -1.f, 0.5f);
								m_EffectControl.SetEffectSpeed(static_cast<int>(Sceneclass::Effect::ef_fire2), 2.f);
							}
						}
						//Reload
						if (this->m_Input.GetPADSPress(Controls::PADS::RELOAD) ||
							((GetGunPtrNow()->GetAmmoNumTotal() == 0) && this->m_Input.GetPADSPress(Controls::PADS::SHOT))) {
							GetGunPtrNow()->ReloadStart();
						}
						//武器を眺める
						if (this->m_Input.GetPADSPress(Controls::PADS::CHECK)) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Watch);
						}
						if (!GetGunPtrNow()->GetModSlot().GetModData()->GetIsThrowWeapon()) {
							//グレネード構え
							if (this->m_Input.GetPADSPress(Controls::PADS::THROW)) {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
								//投げ武器である最初の武器に切り替え
								m_GunPtrControl.GunChangeThrowWeapon(true);
							}
						}
						else {
							//グレネード投てき
							if (this->m_Input.GetPADSPress(Controls::PADS::THROW) || this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Pin))->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
								GetGunPtrNow()->SetGunAnime(GunAnimeID::ThrowReady);
							}
						}
					}
					break;
				case GunAnimeID::Shot:
					//射撃終了フラグ
					switch (GetGunPtrNow()->GetShotType()) {
					case SHOTTYPE::FULL:
						GetGunPtrNow()->SetShotEnd(GetGunPtrNow()->CanShootAmmo() || GetGunPtrNow()->GetIsMagEmpty());
						break;
					case SHOTTYPE::SEMI:
					case SHOTTYPE::BOLT:
						GetGunPtrNow()->SetShotEnd(!this->m_Input.GetPADSPress(Controls::PADS::SHOT));
						break;
					default:
						break;
					}
					break;
				case GunAnimeID::ReloadStart_Empty:
				case GunAnimeID::ReloadStart:
					if (GetMyPlayerID() != 0) {
						//TODO:一瞬だけ
						if (GetRand(100) < 50) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_reload))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
						}
						else {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_takecover))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
						}
					}
					break;
				case GunAnimeID::Reload:
					if (GetGunPtrNow()->GetReloadType() == RELOADTYPE::AMMO) {
						if (this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
							GetGunPtrNow()->SetReloadAmmoCancel();
						}
					}
					break;
				case GunAnimeID::ThrowReady:
					if (GetGunPtrNow()->GetNowGunAnimePer() >= 1.f) {
						if (!this->m_Input.GetPADSPress(Controls::PADS::SHOT) && !this->m_Input.GetPADSPress(Controls::PADS::THROW)) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Throw);
						}
					}
					break;
				case GunAnimeID::Throw:
					if (GetGunPtrNow()->GetNowGunAnimePer() >= 0.4f) {
						if (GetGunPtrNow()->IsActive()) {
							GetGunPtrNow()->SetActiveAll(false);//手にあるものは非表示にする
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Throw))->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
							m_Grenade.SetFall(GetFrameWorldMat(CharaFrame::RightHandJoint).pos(), GetEyeRotationCache(), (GetEyeRotationCache().zvec() * -1.f).normalized() * (Scale3DRate * 15.f / 60.f), 3.5f, SoundEnum::FallGrenade, true);
						}
					}
					if (GetGunPtrNow()->GetNowGunAnimePer() >= 0.6f) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
						//投げ武器ではない最初の武器に切り替え
						m_GunPtrControl.GunChangeThrowWeapon(false);
					}
					break;
				default:
					break;
				}
			}
			this->m_MoveControl.Update(
				this->m_Input.GetPADSPress(Controls::PADS::MOVE_W),
				this->m_Input.GetPADSPress(Controls::PADS::MOVE_A),
				this->m_Input.GetPADSPress(Controls::PADS::MOVE_S),
				this->m_Input.GetPADSPress(Controls::PADS::MOVE_D));

			auto yRadFront = IsMoveFront() ? (atan2f(m_MoveControl.GetVecTotal().x, -m_MoveControl.GetVecTotal().z) * -m_MoveControl.GetVecTotal().z) : 0.f;
			yRadFront += IsMoveBack() ? (atan2f(-m_MoveControl.GetVecTotal().x, m_MoveControl.GetVecTotal().z) * m_MoveControl.GetVecTotal().z) : 0.f;

			auto RecoilRadAdd = GetGunPtrNow()->GetRecoilRadAdd() * (60.f * DXLib_ref::Instance()->GetDeltaTime());
			this->m_RotateControl.Update(this->m_Input.GetAddxRad() - RecoilRadAdd.y, this->m_Input.GetAddyRad() + RecoilRadAdd.x, m_MoveControl.GetVecPower() > 0.1f, yRadFront);
			//リーン
			this->m_LeanControl.Update(this->m_Input.GetPADSPress(Controls::PADS::LEAN_L), this->m_Input.GetPADSPress(Controls::PADS::LEAN_R));
			if (m_LeanControl.GetSwitch()) {
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
			}
			if (GetGunPtrNow()->GetModSlot().GetModData()->GetCanSwitch()) {
				Easing(&m_SwitchPer, (m_LeanControl.GetRate() <= 0 || GetGunPtrNow()->GetReloading()) ? 1.f : -1.f, 0.9f, EasingType::OutExpo);
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
				auto Time = this->GetObj_const().GetAnim(static_cast<int>(this->m_BottomAnimSelect)).GetTime();
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
			for (int i = 0; i < GetObj_const().GetAnimNum(); i++) {
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
			}
			else {
				vec.y = (this->GetMove().GetVec().y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps())));
			}
			SetMove().SetVec(vec);
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
			SetMove().SetPos(PosBuf);
			SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetYRadBottom()));
			SetMove().Update(0.5f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			Matrix3x3DX CharaRotationCache = GetCharaLocalDir() * GetMove().GetMat();
			if (IsAlive()) {
				//銃の位置を指定するアニメ
				for (int loop = 0; loop < m_GunPtrControl.GetGunNum(); loop++) {
					auto& p = m_GunPtrControl.GetGunPtr(loop);
					if (!p) { continue; }

					//スリング場所探索
					if (p->m_SlingPer < 1.f) {
						switch (loop) {
						case 0://メイン武器のスリング
							m_SlingZrad.Update(DXLib_refParts->GetDeltaTime());
							m_SlingZrad.AddRad((0.5f * (m_RotateControl.GetRad().y - m_RotateControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());

							p->m_SlingRot = Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
								(
									Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingZrad.GetRad()) *
									Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetRad().y)
								);
							p->m_SlingPos = GetFrameWorldMat(CharaFrame::Head).pos() +
								GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.3f * Scale3DRate) +
								(
									this->GetMove().GetMat().xvec() * sin(m_SlingZrad.GetRad()) +
									this->GetMove().GetMat().yvec() * cos(m_SlingZrad.GetRad())
									) * -(0.5f * Scale3DRate);
							break;
						case 1://ホルスター
						case 2://一応同じ
							p->m_SlingRot = Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-90)) *
								Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper));
							p->m_SlingPos = GetFrameWorldMat(CharaFrame::Upper).pos() +
								GetFrameWorldMat(CharaFrame::Upper).yvec() * (-0.5f * Scale3DRate) +
								GetFrameWorldMat(CharaFrame::Upper).xvec() * (-0.3f * Scale3DRate);
							break;
						default:
							break;
						}
					}

					bool IsSelect = loop == m_GunPtrControl.GetNowGunSelect();
					if (IsSelect) {
						p->SetMagazinePoachMat(GetFrameWorldMat(CharaFrame::LeftMag));
					}
					else {
						p->InitGunAnime();
						p->SetActiveAll(true);
					}
					p->UpdateGunMat(IsSelect, GetIsADS(), CharaRotationCache, GetFrameWorldMat(CharaFrame::Head).pos());
				}
				//手の位置を制御
				if ((GetMyPlayerID() == 0) || this->m_CanLookTarget) {
					//銃座標
					Matrix4x4DX HandMat;
					{
						HandMat = GetGunPtrNow()->GetRightHandMat();
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
						HandMat = GetGunPtrNow()->GetLeftHandMat();
						Easing(&m_ArmBreakPer, (((GetGunPtrNow()->GetGunAnime() == GunAnimeID::None || GetGunPtrNow()->GetGunAnime() == GunAnimeID::Shot) && m_ArmBreak) || GetGunPtrNow()->GetModSlot().GetModData()->GetIsThrowWeapon()) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
						if (m_ArmBreakPer > 0.01f) {
							m_SlingArmZrad.Update(DXLib_refParts->GetDeltaTime());
							m_SlingArmZrad.AddRad((0.5f * (m_RotateControl.GetRad().y - m_RotateControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());
							Matrix4x4DX SlingArmMat =
								(
									Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(180)) *
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
			else {
				for (int loop = 0; loop < m_GunPtrControl.GetGunNum(); ++loop) {
					auto& p = m_GunPtrControl.GetGunPtr(loop);
					if (!p) { continue; }
					if (loop == m_GunPtrControl.GetNowGunSelect()) {
						auto Mat = m_RagDollControl.lagframe_.RIGHThand_f.GetFrameWorldPosition().rotation().inverse() * m_RagDollControl.GetRagDoll().GetFrameLocalWorldMatrix(m_RagDollControl.lagframe_.RIGHThand_f.GetFrameID());
						p->SetGunMat(Matrix3x3DX::Get33DX(Mat), Mat.pos());
						p->SetActiveAll(true);
					}
					else {
						p->SetActiveAll(false);
					}
				}
			}
			m_RagDollControl.Update(GetObj(), IsAlive());													//ラグドール
			m_HitBoxControl.Update(this, (GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);									//ヒットボックス
			bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			{
				m_EyePositionCache = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f;
				if (HeadBobbing) {
					m_EyePositionCache += m_WalkSwingControl.CalcEye(CharaRotationCache, std::clamp(m_MoveControl.GetVecPower(), 0.f, 0.85f) / 0.65f, 5.f);
				}
				if (GetGunPtrNow()) {
					m_EyePositionCache = Lerp<Vector3DX>(m_EyePositionCache, GetGunPtrNow()->GetADSEyeMat().pos(), GetGunPtrNow()->GetGunAnimBlendPer(GunAnimeID::ADS));
				}
				m_EyeRotationCache = Matrix3x3DX::identity();
				if (HeadBobbing) {
					m_EyeRotationCache = m_WalkSwingControl.CalcWalk(GetEyePositionCache() - GetMove().GetPos(), std::clamp(GetMove().GetVec().magnitude() / 2.f, 0.f, 0.5f));
				}
				m_EyeRotationCache *=
					Matrix3x3DX::RotAxis(Vector3DX::forward(), m_LeanControl.GetRad() / 5.f) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), m_RotateControl.GetRad().x) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetYRadBottomChange()) *
					GetMove().GetMat();
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

					for (int i = 0, max = m_GrenadeData->GetPellet(); i < max; i++) {
						auto LastAmmo = std::make_shared<AmmoClass>();
						ObjMngr->AddObject(LastAmmo);
						LastAmmo->Init();
						auto mat =
							Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-GetRand(30))) *
							Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180)));
						LastAmmo->Put(this->m_GrenadeData, g->GetMove().GetPos() + mat.zvec() * (0.5f * Scale3DRate) + Vector3DX::up() * (0.5f * Scale3DRate), mat.zvec(), GetMyPlayerID());
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
		void			CharacterClass::LoadCharaGun(const std::string& FolderName, int Sel) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			std::string Path = "data/gun/";
			Path += FolderName;
			Path += "/";

			auto Ptr = std::make_shared<GunClass>();
			ObjMngr->AddObject(Ptr);
			ObjMngr->LoadModel(Ptr, Ptr, Path.c_str());
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

			this->m_MoveOverRideFlag = false;
			this->m_Input.ResetAllInput();
			this->m_RotateControl.Init(pxRad, pyRad);
			this->m_MoveControl.Init();
			this->m_LeanControl.Init();
			for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
			this->m_IsSquat = false;
			SetMove().SetAll(pPos, pPos, pPos, Vector3DX::zero(), Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetRad().y), Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetRad().y));
			//
			m_GunPtrControl.SelectGun(GunSel);
			for (int loop = 0; loop < m_GunPtrControl.GetGunNum(); ++loop) {
				if (!m_GunPtrControl.GetGunPtr(loop)) { continue; }
				m_GunPtrControl.GetGunPtr(loop)->Spawn();
			}
			m_SlingZrad.Init(0.05f * Scale3DRate, 3.f, deg2rad(50));
		}
		//
		void			CharacterClass::Init_Sub(void) noexcept {
			m_HitBoxControl.Init();
			m_ArmBreak = false;
			m_EffectControl.Init();

			m_Grenade.Dispose();
			m_Grenade.Init("data/model/RGD5/", 4);
			m_GrenadeData = AmmoDataManager::Instance()->LoadAction("data/ammo/FragRGD/");
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				for (int i = 0, num = GetObj_const().GetMaterialNum(); i < num; i++) {
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
			this->m_DistanceToCam = (GetObj_const().GetMatrix().pos() - GetScreenPosition()).magnitude();

			if (CheckCameraViewClip_Box(
				(GetObj_const().GetMatrix().pos() + Vector3DX::vget(-2.5f, 0.f, -2.5f) * Scale3DRate).get(),
				(GetObj_const().GetMatrix().pos() + Vector3DX::vget(2.5f, 2.5f, 2.5f) * Scale3DRate).get()) == FALSE
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
					(GetObj_const().GetMatrix().pos() + Vector3DX::vget(-2.5f * Scale3DRate, -0.5f * Scale3DRate, -2.5f * Scale3DRate)).get(),
					(GetObj_const().GetMatrix().pos() + Vector3DX::vget(2.5f * Scale3DRate, 2.f * Scale3DRate, 2.5f * Scale3DRate)).get()) == FALSE)
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
					//MV1SetMaterialTypeAll(this->GetObj_const().GetHandle(), DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
					if (IsAlive()) {
						for (int i = 0; i < GetObj_const().GetMeshNum(); i++) {
							if (GetObj_const().GetMeshSemiTransState(i) == isDrawSemiTrans) {
								GetObj_const().DrawMesh(i);
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
