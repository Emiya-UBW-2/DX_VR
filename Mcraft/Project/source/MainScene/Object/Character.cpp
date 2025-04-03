#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

#include "../../CommonScene/Object/GunsModify.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			if (GetMyPlayerID() == value.DamageID) {
				auto* SE = SoundPool::Instance();
				auto* PlayerMngr = Player::PlayerManager::Instance();

				auto PrevLive = IsAlive();
				this->m_HP.Sub(value.Damage);
				this->m_AP.Sub(value.ArmerDamage);
				bool IsDeath = PrevLive && !IsAlive();

				if (value.ShotID == PlayerMngr->GetWatchPlayer()) {//撃ったキャラ
					//SE
					if (value.Damage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Hit))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f, 128);
					}
					//ヒットカウント
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						PlayerMngr->GetPlayer(value.ShotID)->AddHit(1);
					}
					if (IsDeath) {
						PlayerMngr->GetPlayer(value.ShotID)->AddScore(100);
						PlayerMngr->GetPlayer(value.ShotID)->AddKill(1);
					}
					//ヒット座標表示を登録
					HitMarkerPool::Instance()->AddMarker(value.m_EndPos, value.Damage, value.ArmerDamage);
				}
				if (value.DamageID == PlayerMngr->GetWatchPlayer()) {//撃たれたキャラ
					if (value.Damage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitMe))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f, 255);
					}
				}
				//エフェクトセット
				if (value.Damage > 0) {
					EffectSingleton::Instance()->SetOnce(Sceneclass::Effect::ef_hitblood, value.m_EndPos, Vector3DX::forward(), Scale3DRate);
					EffectSingleton::Instance()->SetEffectSpeed(Sceneclass::Effect::ef_hitblood, 2.f);
				}
				else {
					EffectSingleton::Instance()->SetOnce(Sceneclass::Effect::ef_gndsmoke, value.m_EndPos, (value.m_StartPos - value.m_EndPos).normalized(), 0.25f * Scale3DRate);
				}
				//ヒットモーション
				if (value.Damage > 0) {
					this->m_HitReactionControl.SetHit(Matrix3x3DX::Vtrans(Vector3DX::Cross((value.m_EndPos - value.m_StartPos).normalized(), Vector3DX::up()) * -1.f, Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper2)).inverse()));
					switch (HitType::Leg) {
					case HitType::Head:
						break;
					case HitType::Body:
						break;
					case HitType::Arm:
						this->m_ArmBreak = true;
						break;
					case HitType::Leg:
						this->m_IsSquat = true;
						break;
					default:
						break;
					}
				}
				//todo : ヒットした部分に近い頂点を赤くする

				if (IsAlive()) {
					//被弾ボイス
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_Hurt1) + GetRand(6 - 1))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
				}
				else if (IsDeath) {
					//死亡ボイス
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
				}
				return true;
			}
			return false;
		}
		const bool		CharacterClass::CheckDamageRay(HitPoint Damage, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (!IsAlive()) { return false; }
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, GetMove().GetPos()) <= 2.0f * Scale3DRate)) { return false; }

			if (AttackID != PlayerMngr->GetWatchPlayer()) {
				SetConcussionSwitch();
			}
			//被弾処理
			auto* HitPtr = this->m_HitBoxControl.GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				ArmerPoint ArmerDamage = 0;
				//部位ダメージ演算
				switch (HitPtr->GetColType()) {
				case HitType::Head:
					if (GetMyPlayerID() != PlayerMngr->GetWatchPlayer()) {
						Damage = std::clamp<HitPoint>(Damage * 1000 / 100, 0, this->m_HP.GetMax());
					}
					break;
				case HitType::Body:
					ArmerDamage = std::clamp<ArmerPoint>(Damage, 0, this->m_AP.GetPoint());
					Damage = std::clamp<HitPoint>(Damage - ArmerDamage, 0, this->m_HP.GetMax());
					break;
				case HitType::Arm:
					Damage = Damage * 50 / 100;
					break;
				case HitType::Leg:
					Damage = Damage * 70 / 100;
					break;
				default:
					break;
				}
				//ダメージ登録
				this->m_Damage.Add(AttackID, GetMyPlayerID(), Damage, ArmerDamage, static_cast<int>(HitPtr->GetColType()), StartPos, *pEndPos);
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
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//
			if (IsAlive()) {
				if (IsLowHP()) {
					this->m_HPRec += DXLib_refParts->GetDeltaTime();
					if (this->m_HPRec >= 0.f) {
						this->m_HPRec -= 2.f;
						Heal(2);
						if (GetMyPlayerID() == PlayerMngr->GetWatchPlayer()) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing))->Play(DX_PLAYTYPE_BACK);
						}
					}
				}
				else {
					if (this->m_HPRec != 0.f) {
						if (GetMyPlayerID() == PlayerMngr->GetWatchPlayer()) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing))->StopAll();
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathend))->Play(DX_PLAYTYPE_BACK);
						}
					}
					this->m_HPRec = 0.f;
				}
				//AIM
				if (this->m_Input.GetPADSTrigger(Controls::PADS::SQUAT)) {
					this->m_IsSquat ^= 1;
					//しゃがみ音
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
				}
			}
			if (GetGunPtrNow()) {
				//銃ひっこめ
				if (this->m_StuckGunTimer == 0.f) {
					this->m_StuckGunTimer = 0.1f;
					if (BackGround->CheckLinetoMap(GetEyePositionCache(), GetGunPtrNow()->GetFrameWorldMatParts(GunFrame::Muzzle).pos())) {//0.03ms
						switch (GetGunPtrNow()->GetGunAnime()) {
						case GunAnimeID::LowReady:
						case GunAnimeID::None:
						case GunAnimeID::Shot:
							this->m_IsStuckGun = true;
							break;
						default:
							break;
						}
					}
					else {
						if (this->m_IsStuckGun) {
							this->m_IsStuckGun = false;
							if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::LowReady) {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::None);
							}
						}
					}
				}
				else {
					this->m_StuckGunTimer = std::max(this->m_StuckGunTimer - DXLib_refParts->GetDeltaTime(), 0.f);
					if (this->m_IsStuckGun) {
						if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::LowReady) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
						}
					}
				}
				//武器切替
				if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::LowReady) {
					int Wheel = 0;//PadControl::Instance()->GetMouseWheelRot();
					if (GetMyPlayerID() == PlayerMngr->GetWatchPlayer()) {
						Wheel = PadControl::Instance()->GetMouseWheelRot();
					}
					if (this->m_Input.GetPADSTrigger(Controls::PADS::ULT)) {
						Wheel = -1;
					}
					if (Wheel != 0) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
						this->m_GunPtrControl.GunChangeNext((Wheel < 0));
					}
				}
				//アクション
				switch (GetGunPtrNow()->GetGunAnime()) {
				case GunAnimeID::LowReady:
					if (this->m_GunPtrControl.IsChangeGunSel() && GetGunPtrNow()->GetGunAnimBlendPer(GunAnimeID::LowReady) > 0.95f) {
						this->m_GunPtrControl.InvokeReserveGunSel();
						GetGunPtrNow()->SetGunAnime(GunAnimeID::None);
					}
					break;
				case GunAnimeID::None:
					if (GetGunPtrNow()->GetCanShot()) {
						//武器を眺める
						if (this->m_Input.GetPADSPress(Controls::PADS::CHECK)) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Watch);
						}
						if (!GetGunPtrNow()->GetModSlot().GetModData()->GetIsThrowWeapon()) {
							//射撃
							if (this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
								GetGunPtrNow()->SetShotStart();
							}
							//Reload
							if (this->m_Input.GetPADSPress(Controls::PADS::RELOAD) ||
								((GetGunPtrNow()->GetAmmoNumTotal() == 0) && this->m_Input.GetPADSPress(Controls::PADS::SHOT))) {
								GetGunPtrNow()->ReloadStart();
							}
							//グレネード構え
							if (this->m_Input.GetPADSPress(Controls::PADS::THROW)) {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
								//投げ武器である最初の武器に切り替え
								this->m_GunPtrControl.GunChangeThrowWeapon(true);
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
						GetGunPtrNow()->SetShotEnd(GetGunPtrNow()->GetAmmoNumTotal() != 0);
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
					if (GetMyPlayerID() != PlayerMngr->GetWatchPlayer()) {
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
						if (this->m_Input.GetPADSTrigger(Controls::PADS::SHOT)) {
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
					if (GetGunPtrNow()->GetNowGunAnimePer() >= 0.6f) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
						//投げ武器ではない最初の武器に切り替え
						this->m_GunPtrControl.GunChangeThrowWeapon(false);
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

			auto yRadFront = (IsMoveFront() ? this->m_MoveControl.GoFrontRad() : 0.f) + (IsMoveBack() ? this->m_MoveControl.GoBackRad() : 0.f);
			auto RecoilRadAdd = GetGunPtrNow()->GetRecoilRadAdd() * (60.f * DXLib_ref::Instance()->GetDeltaTime());
			this->m_RotateControl.Update(this->m_Input.GetAddxRad() - RecoilRadAdd.y, this->m_Input.GetAddyRad() + RecoilRadAdd.x, this->m_MoveControl.GetVecPower() > 0.1f, yRadFront);
			//リーン
			this->m_LeanControl.Update(this->m_Input.GetPADSTrigger(Controls::PADS::LEAN_L), this->m_Input.GetPADSTrigger(Controls::PADS::LEAN_R));
			if (this->m_LeanControl.GetSwitch()) {
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
			}
			GetGunPtrNow()->CalcSwitchPer(this->m_LeanControl.GetRad() > deg2rad(-1) || GetGunPtrNow()->GetReloading());
			//下半身
			this->m_BottomAnimSelect = GetBottomStandAnimSel();
			if (IsMoveLeft()) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
			if (IsMoveRight()) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
			if (IsMoveBack()) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
			if (IsMoveFront()) { this->m_BottomAnimSelect = GetBottomWalkAnimSel(); }
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(GetBottomTurnAnimSel())), (!this->m_IsSquat && this->m_RotateControl.IsTurnBody()) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
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
			if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
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
			Matrix3x3DX CharaLocalRotationCache = 
				Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_LeanControl.GetRad()) *
				Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_RotateControl.GetRad().x) *
				Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadBottomChange());
			//
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)),
				(
					Matrix3x3DX::Get33DX(GetFrameLocalMat(CharaFrame::Upper)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), -this->m_RotateControl.GetRad().x / 2.f) *
					CharaLocalRotationCache
					).Get44DX() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper)));
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper2)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper2)),
				(
					Matrix3x3DX::Get33DX(GetFrameLocalMat(CharaFrame::Upper2)) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(32 * GetGunPtrNow()->GetSwitchPer())) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-35)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_RotateControl.GetRad().x / 2.f) *
					this->m_HitReactionControl.GetHitReactionMat()
					).Get44DX() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper2)));
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Neck)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Neck)),
				(
					Matrix3x3DX::Get33DX(GetFrameLocalMat(CharaFrame::Neck)) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-32 * GetGunPtrNow()->GetSwitchPer())) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(25)) *
					Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(12 * GetGunPtrNow()->GetSwitchPer()))
					).Get44DX() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Neck)));
			this->m_HitReactionControl.Update();
			//上半身アニメ演算

			this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Upper_Ready)) = 1.f;


			auto FingerPer = GetGunPtrNow()->GetGunAnimeFingerNow();
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_Thumb)) ,FingerPer.at(0).at(0), 0.8f, EasingType::OutExpo);
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_Point)) ,FingerPer.at(0).at(1), 0.8f, EasingType::OutExpo);
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_middle)) ,FingerPer.at(0).at(2), 0.8f, EasingType::OutExpo);
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_Ring)) ,FingerPer.at(0).at(3), 0.8f, EasingType::OutExpo);
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_Pinky)) ,FingerPer.at(0).at(4), 0.8f, EasingType::OutExpo);
			//
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Left_Thumb)) ,FingerPer.at(1).at(0), 0.8f, EasingType::OutExpo);
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Left_Point)) ,FingerPer.at(1).at(1), 0.8f, EasingType::OutExpo);
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Left_middle)) ,FingerPer.at(1).at(2), 0.8f, EasingType::OutExpo);
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Left_Ring)) ,FingerPer.at(1).at(3), 0.8f, EasingType::OutExpo);
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Left_Pinky)) ,FingerPer.at(1).at(4), 0.8f, EasingType::OutExpo);

			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomTurnAnimSel()), 0.5f);
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaAnimeID::Bottom_Stand_Run), GetSpeed() * 0.5f);
			float AnimSpeed = 1.15f * std::clamp(GetSpeed() / 0.65f, 0.5f, 1.f);
			ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomWalkAnimSel()), this->m_MoveControl.GetVecFront() * AnimSpeed);
			ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomLeftStepAnimSel()), this->m_MoveControl.GetVecLeft() * AnimSpeed);
			ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomWalkBackAnimSel()), this->m_MoveControl.GetVecRear() * AnimSpeed);
			ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomRightStepAnimSel()), this->m_MoveControl.GetVecRight() * AnimSpeed);
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
			if (this->m_MoveControl.GetVecPower() > 0.f) {
				vec = Matrix3x3DX::Vtrans(this->m_MoveControl.GetVecMove() * GetSpeed(), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadUpper()));
			}
			if (IsHitGround) {
				float GroundHight = EndPos.y - (0.12f * Scale3DRate);
				if ((PosBuf.y - GroundHight) > (0.0008f * Scale3DRate / DXLib_refParts->GetDeltaTime())) {
					PosBuf.y = GroundHight;//高所落下の際は即時反映
				}
				else {
					Easing(&PosBuf.y, GroundHight, 0.6f, EasingType::OutExpo);
				}

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
			SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadBottom()));
			SetMove().Update(0.5f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			Matrix3x3DX CharaRotationCache = CharaLocalRotationCache * GetMove().GetMat();
			if (IsAlive()) {
				//銃の位置を指定するアニメ
				for (int loop = 0; loop < this->m_GunPtrControl.GetGunNum(); loop++) {
					auto& p = this->m_GunPtrControl.GetGunPtr(loop);
					if (!p) { continue; }

					//スリング場所探索
					if (p->m_SlingPer < 1.f) {
						switch (loop) {
						case 0://メイン武器のスリング
							this->m_SlingZrad.Update(DXLib_refParts->GetDeltaTime());
							this->m_SlingZrad.AddRad((0.5f * (this->m_RotateControl.GetRad().y - this->m_RotateControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());

							p->m_SlingRot = Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
								(
									Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingZrad.GetRad()) *
									Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetRad().y)
								);
							p->m_SlingPos = GetFrameWorldMat(CharaFrame::Head).pos() +
								GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.3f * Scale3DRate) +
								(
									this->GetMove().GetMat().xvec() * sin(this->m_SlingZrad.GetRad()) +
									this->GetMove().GetMat().yvec() * cos(this->m_SlingZrad.GetRad())
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

					bool IsSelect = loop == this->m_GunPtrControl.GetNowGunSelect();
					if (IsSelect) {
						p->SetMagazinePoachMat(GetFrameWorldMat(CharaFrame::LeftMag));
						p->SetGrenadeThrowRot(GetEyeRotationCache());
					}
					else {
						p->InitGunAnime();
						p->SetActiveAll(true);
					}
					p->UpdateGunMat(IsSelect, GetIsADS(), CharaRotationCache, GetFrameWorldMat(CharaFrame::Head).pos());
				}
				//手の位置を制御
				if ((GetMyPlayerID() == PlayerMngr->GetWatchPlayer()) || GetCanLookByPlayer()) {
					//銃座標
					{
						IK_RightArm(
							&GetObj(),
							GetFrame(static_cast<int>(CharaFrame::RightArm)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
							GetFrame(static_cast<int>(CharaFrame::RightArm2)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
							GetFrame(static_cast<int>(CharaFrame::RightWrist)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
							GetGunPtrNow()->GetRightHandMat());
					}
					{
						Matrix4x4DX HandMat;
						HandMat = GetGunPtrNow()->GetLeftHandMat();
						Easing(&this->m_ArmBreakPer, (((GetGunPtrNow()->GetGunAnime() == GunAnimeID::None || GetGunPtrNow()->GetGunAnime() == GunAnimeID::Shot) && this->m_ArmBreak) || GetGunPtrNow()->GetModSlot().GetModData()->GetIsThrowWeapon()) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
						if (this->m_ArmBreakPer > 0.01f) {
							this->m_SlingArmZrad.Update(DXLib_refParts->GetDeltaTime());
							this->m_SlingArmZrad.AddRad((0.5f * (this->m_RotateControl.GetRad().y - this->m_RotateControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());
							Matrix4x4DX SlingArmMat =
								(
									Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(180)) *
									Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
									(
										Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingArmZrad.GetRad()) *
										Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetRad().y)
										)
									).Get44DX() * Matrix4x4DX::Mtrans(
										GetFrameWorldMat(CharaFrame::Head).pos() +
										GetFrameWorldMat(CharaFrame::Head).xvec() * (0.15f * Scale3DRate) +
										GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.1f * Scale3DRate) +
										(
											this->GetMove().GetMat().xvec() * sin(this->m_SlingArmZrad.GetRad()) +
											this->GetMove().GetMat().yvec() * cos(this->m_SlingArmZrad.GetRad())
											) * -(0.5f * Scale3DRate)
									);
							HandMat = Lerp(HandMat, SlingArmMat, this->m_ArmBreakPer);
						}
						if (this->m_ArmBreak) {
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
							HandMat);
					}
				}
			}
			else {
				for (int loop = 0; loop < this->m_GunPtrControl.GetGunNum(); ++loop) {
					auto& p = this->m_GunPtrControl.GetGunPtr(loop);
					if (!p) { continue; }
					if (loop == this->m_GunPtrControl.GetNowGunSelect()) {
						auto Mat = this->m_RagDollControl.GetFrameMat(RagFrame::RIGHThand);
						p->SetGunMat(Matrix3x3DX::Get33DX(Mat), Mat.pos());
						p->SetActiveAll(true);
					}
					else {
						p->SetActiveAll(false);
					}
				}
			}
			this->m_RagDollControl.Update(IsAlive());													//ラグドール
			this->m_HitBoxControl.Update(this, (GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);									//ヒットボックス
			bool HeadBobbing = ((GetMyPlayerID() != PlayerMngr->GetWatchPlayer()) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			{
				this->m_EyePositionCache = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f;
				if (HeadBobbing) {
					this->m_EyePositionCache += this->m_WalkSwingControl.CalcEye(CharaRotationCache, std::clamp(this->m_MoveControl.GetVecPower(), 0.f, 0.85f) / 0.65f, 5.f);
				}
				if (GetGunPtrNow()) {
					this->m_EyePositionCache = Lerp<Vector3DX>(this->m_EyePositionCache, GetGunPtrNow()->GetADSEyeMat().pos(), GetGunPtrNow()->GetGunAnimBlendPer(GunAnimeID::ADS));
				}
				this->m_EyeRotationCache = Matrix3x3DX::identity();
				if (HeadBobbing) {
					this->m_EyeRotationCache = this->m_WalkSwingControl.CalcWalk(GetEyePositionCache() - GetMove().GetPos(), std::clamp(GetMove().GetVec().magnitude() / 2.f, 0.f, 0.5f));
				}
				this->m_EyeRotationCache *=
					Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_LeanControl.GetRad() / 5.f) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_RotateControl.GetRad().x) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadBottomChange()) *
					GetMove().GetMat();
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
			this->m_GunPtrControl.SetGunPtr(Sel, Ptr);
		}
		//
		void			CharacterClass::Init_Sub(void) noexcept {
			this->m_HitBoxControl.Init();
			this->m_ArmBreak = false;
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			if (this->m_IsFirstLoop) {}//初回のみ更新する内容
			ExecuteInput();
			ExecuteMatrix();
			this->m_IsActiveCameraPos = false;
			if (GetMyPlayerID() != PlayerMngr->GetWatchPlayer()) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0)->GetChara();
				Vector3DX TgtPos = GetEyePositionCache();
				auto EndPos = TgtPos;
				this->m_CanLookTarget = !BackGround->CheckLinetoMap(Chara->GetEyePositionCache(), &EndPos);
				this->m_Length = (TgtPos - Chara->GetEyePositionCache()).magnitude();
			}
		}
		void CharacterClass::CheckDraw(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			this->m_IsDraw = false;
			this->m_DistanceToCam = (GetObj_const().GetMatrix().pos() - GetScreenPosition()).magnitude();

			if (CheckCameraViewClip_Box(
				(GetObj_const().GetMatrix().pos() + Vector3DX::vget(-2.5f, 0.f, -2.5f) * Scale3DRate).get(),
				(GetObj_const().GetMatrix().pos() + Vector3DX::vget(2.5f, 2.5f, 2.5f) * Scale3DRate).get()) == FALSE
				) {
				this->m_IsDraw |= true;
			}

			if (GetMyPlayerID() != PlayerMngr->GetWatchPlayer()) {
				this->m_IsActiveCameraPos |= this->m_CameraPos.Calc(GetEyePositionCache());
			}
		}
	};
};
