#include	"Character.hpp"

#include	"../../MainScene/Player/Player.hpp"

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
					else if (value.ArmerDamage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f, 128);
					}
					//ヒットカウント
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						PlayerMngr->GetPlayer(value.ShotID)->AddHit(1);
						//ヒット座標表示を登録
						HitMarkerPool::Instance()->AddMarker(value.m_EndPos, value.Damage, value.ArmerDamage);
					}
					if (IsDeath) {
						PlayerMngr->GetPlayer(value.ShotID)->AddScore(100);
						PlayerMngr->GetPlayer(value.ShotID)->AddKill(1);
					}
				}
				if (value.DamageID == PlayerMngr->GetWatchPlayer()) {//撃たれたキャラ
					if (value.Damage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitMe))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else if (value.ArmerDamage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f, 255);
					}
				}
				//エフェクトセット
				if (value.Damage > 0) {
					EffectSingleton::Instance()->SetOnce(Sceneclass::Effect::ef_hitblood, value.m_EndPos, Vector3DX::forward(), Scale3DRate);
					EffectSingleton::Instance()->SetEffectSpeed(Sceneclass::Effect::ef_hitblood, 2.f);
				}
				else if (value.ArmerDamage > 0) {
					EffectSingleton::Instance()->SetOnce(Sceneclass::Effect::ef_gndsmoke, value.m_EndPos, (value.m_StartPos - value.m_EndPos).normalized(), 0.25f * Scale3DRate);
				}
				//ヒットモーション
				if (value.Damage > 0) {
					this->m_HitReactionControl.SetHit(Matrix3x3DX::Vtrans(Vector3DX::Cross((value.m_EndPos - value.m_StartPos).normalized(), Vector3DX::up()) * -1.f, Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper2)).inverse()));
					switch (static_cast<HitType>(value.m_HitType)) {
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
				//ボイス
				if (IsAlive()) {
					//被弾ボイス
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_Hurt1) + GetRand(6 - 1))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
				}
				else if (IsDeath) {
					//死亡ボイス
					for (int loop = 0; loop < 6; ++loop) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_Hurt1) + loop)->StopAll();
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
		const bool		CharacterClass::CheckDamageRay(HitPoint BaseDamage, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (!IsAlive()) { return false; }
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, GetMove().GetPos()) <= 2.0f * Scale3DRate)) { return false; }

			if (AttackID != PlayerMngr->GetWatchPlayer()) {
				this->m_ConcussionSwitch = true;
			}
			//被弾処理
			auto* HitPtr = this->m_HitBoxControl.GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				HitPoint Damage = BaseDamage;
				ArmerPoint ArmerDamage = 0;
				//部位ダメージ演算
				switch (HitPtr->GetColType()) {
				case HitType::Head:
					if (GetMyPlayerID() != PlayerMngr->GetWatchPlayer()) {//自機はヘッショされない
						Damage = std::clamp<HitPoint>(BaseDamage * 1000 / 100, 0, this->m_HP.GetMax());
					}
					break;
				case HitType::Body:
					ArmerDamage = std::clamp<ArmerPoint>(BaseDamage, 0, this->m_AP.GetPoint());
					Damage = std::clamp<HitPoint>(BaseDamage - ArmerDamage, 0, this->m_HP.GetMax());
					break;
				case HitType::Arm:
					Damage = BaseDamage * 50 / 100;
					break;
				case HitType::Leg:
					Damage = BaseDamage * 70 / 100;
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
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//
			if (!IsAlive()) { return; }
			if (this->m_Input.GetPADSTrigger(Controls::PADS::SQUAT)) {
				this->m_IsSquat ^= 1;
				//しゃがみ音
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetEyePositionCache(), Scale3DRate * 3.f);
			}
			if (GetGunPtrNow()) {
				//銃ひっこめ
				if (this->m_StuckGunTimer == 0.f) {
					this->m_StuckGunTimer = 0.1f;

					Vector3DX GunStart = GetEyePositionCache();
					Vector3DX GunEnd = (GetGunPtrNow()->GetBaseMuzzleMat() * GetFrameWorldMat(CharaFrame::Head)).pos();
					bool IsHit = BackGround->CheckLinetoMap(GunStart, GunEnd);
					//ほかプレイヤーとの判定
					if(!IsHit) {
						for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
							if (loop == GetMyPlayerID()) { continue; }
							auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
							if (!chara->IsAlive()) { continue; }
							Vector3DX StartPos = chara->GetMove().GetPos();
							Vector3DX EndPos = chara->GetMove().GetPos() + Vector3DX::up() * (1.8f * Scale3DRate);
							if (GetHitCheckToCapsule(GunStart, GunEnd, 0.1f * Scale3DRate, StartPos, EndPos, 0.3f * Scale3DRate)) {
								IsHit = true;
								break;
							}
						}
					}


					if (IsHit) {//0.03ms
						if (GetGunPtrNow()->IsCanShot()) {
							this->m_IsStuckGun = true;
						}
					}
					else {
						if (this->m_IsStuckGun) {
							this->m_IsStuckGun = false;
							if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::LowReady || GetGunPtrNow()->GetGunAnime() == GunAnimeID::HighReady) {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
							}
						}
					}
				}
				else {
					this->m_StuckGunTimer = std::max(this->m_StuckGunTimer - DXLib_refParts->GetDeltaTime(), 0.f);
					if (this->m_IsStuckGun) {
						auto Zvec = GetFrameWorldMat(CharaFrame::Head).zvec() * -1.f;
						if (Zvec.y < std::sin(deg2rad(-30))) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
						}
						else {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::HighReady);
						}
					}
				}
				//武器切替
				if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::LowReady) {
					int Wheel = 0;
					if (GetMyPlayerID() == PlayerMngr->GetWatchPlayer()) {
						Wheel = -PadControl::Instance()->GetMouseWheelRot();
					}
					if (this->m_Input.GetPADSTrigger(Controls::PADS::ULT)) {
						Wheel = 1;
					}
					if (Wheel != 0) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
						this->m_GunPtrControl.GunChangeNext((Wheel > 0));
					}
				}
				//アクション
				switch (GetGunPtrNow()->GetGunAnime()) {
				case GunAnimeID::LowReady:
					if (this->m_GunPtrControl.IsChangeGunSel() && GetGunPtrNow()->GetGunAnimBlendPer(GunAnimeID::LowReady) > 0.95f) {
						this->m_GunPtrControl.InvokeReserveGunSel();
						GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
					}
					break;
				case GunAnimeID::Base:
					if (GetGunPtrNow()->GetCanShot()) {
						//武器を眺める
						if (this->m_Input.GetPADSPress(Controls::PADS::CHECK)) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Watch);
						}
						if (!GetGunPtrNow()->GetGunPartsSlot()->GetGunPartsData()->GetIsThrowWeapon()) {
							//Reload
							if (this->m_Input.GetPADSPress(Controls::PADS::RELOAD) ||
								((GetGunPtrNow()->GetAmmoNumTotal() == 0) && this->m_Input.GetPADSTrigger(Controls::PADS::SHOT))) {
								GetGunPtrNow()->ReloadStart();
								if (GetMyPlayerID() != PlayerMngr->GetWatchPlayer()) {
									if (GetRand(100) < 50) {
										SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_reload))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
									}
									else {
										SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_takecover))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
									}
								}
							}
							//射撃
							else if (this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
								if (this->m_Input.GetPADSTrigger(Controls::PADS::SHOT)) {
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Trigger))->Play3D(GetEyePositionCache(), Scale3DRate * 5.f);
								}
								GetGunPtrNow()->SetShotStart();
							}
							//グレネード構え
							else if (this->m_Input.GetPADSPress(Controls::PADS::THROW)) {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
								//投げ武器である最初の武器に切り替え
								this->m_GunPtrControl.GunChangeThrowWeapon(true);
							}
						}
						else {
							//グレネード投てき
							if (this->m_Input.GetPADSPress(Controls::PADS::THROW) || this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::ThrowReady);
							}
						}
					}
					break;
				case GunAnimeID::Shot:
					//射撃終了フラグ
					if (GetGunPtrNow()->GetShotType() == SHOTTYPE::FULL) {
						GetGunPtrNow()->SetShotEnd(true);
					}
					else {
						GetGunPtrNow()->SetShotEnd(!this->m_Input.GetPADSPress(Controls::PADS::SHOT));
					}
					break;
				case GunAnimeID::ReloadWait:
				case GunAnimeID::Reload:
					if (GetGunPtrNow()->GetReloadType() == RELOADTYPE::AMMO) {
						if (this->m_Input.GetPADSTrigger(Controls::PADS::SHOT)) {
							GetGunPtrNow()->SetReloadAmmoCancel();
						}
					}
					break;
				case GunAnimeID::ThrowReady:
					if (GetGunPtrNow()->IsNowGunAnimeEnd()) {
						if (!this->m_Input.GetPADSPress(Controls::PADS::SHOT) && !this->m_Input.GetPADSPress(Controls::PADS::THROW)) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Throw);
						}
					}
					break;
				case GunAnimeID::Throw:
					if (GetGunPtrNow()->GetNowAnimTimePerCache() >= 0.6f) {
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
			Vector2DX RecoilRadAdd;
			if (GetGunPtrNow()) {
				RecoilRadAdd = GetGunPtrNow()->GetRecoilRadAdd();
			}
			this->m_RotateControl.Update(
				this->m_Input.GetAddxRad() - RecoilRadAdd.y, this->m_Input.GetAddyRad() + RecoilRadAdd.x,
				this->m_MoveControl.GetVecPower() > 0.1f, (IsMoveFront() ? this->m_MoveControl.GoFrontRad() : 0.f) + (IsMoveBack() ? this->m_MoveControl.GoBackRad() : 0.f));
			//リーン
			if (this->m_LeanControl.Update(this->m_Input.GetPADSTrigger(Controls::PADS::LEAN_L), this->m_Input.GetPADSTrigger(Controls::PADS::LEAN_R))) {
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetEyePositionCache(), Scale3DRate * 3.f);
			}
			if (GetGunPtrNow()) {
				GetGunPtrNow()->CalcSwitchPer(this->m_LeanControl.GetRate() >= 0 || !GetGunPtrNow()->IsCanShot());
			}
			//下半身
			this->m_BottomAnimSelect = GetBottomStandAnimSel();
			if (IsMoveLeft()) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
			if (IsMoveRight()) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
			if (IsMoveBack()) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
			if (IsMoveFront()) { this->m_BottomAnimSelect = GetBottomWalkAnimSel(); }
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(GetBottomTurnAnimSel())), (!this->m_IsSquat && this->m_RotateControl.IsTurnBody()) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
			for (int loop = 0; loop < static_cast<int>(CharaAnimeID::AnimeIDMax); ++loop) {
				CharaAnimeID ID = static_cast<CharaAnimeID>(loop);
				if (
					ID == CharaAnimeID::Bottom_Stand ||
					ID == CharaAnimeID::Bottom_Stand_Run ||
					ID == CharaAnimeID::Bottom_Stand_Walk ||
					ID == CharaAnimeID::Bottom_Stand_LeftStep ||
					ID == CharaAnimeID::Bottom_Stand_RightStep ||
					ID == CharaAnimeID::Bottom_Stand_WalkBack) {
					this->m_AnimPerBuf[loop] = std::clamp(this->m_AnimPerBuf[loop] + ((ID == this->m_BottomAnimSelect) ? 6.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				}
				if (
					ID == CharaAnimeID::Bottom_Squat ||
					ID == CharaAnimeID::Bottom_Squat_Walk ||
					ID == CharaAnimeID::Bottom_Squat_LeftStep ||
					ID == CharaAnimeID::Bottom_Squat_RightStep ||
					ID == CharaAnimeID::Bottom_Squat_WalkBack) {
					this->m_AnimPerBuf[loop] = std::clamp(this->m_AnimPerBuf[loop] + ((ID == this->m_BottomAnimSelect) ? 2.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				}
			}
			//足音
			if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
				auto Time = GetObj().GetAnim(static_cast<int>(this->m_BottomAnimSelect)).GetTime();
				//L
				if ((9.f < Time && Time < 10.f)) {
					if (this->m_CharaSound != 0) {
						this->m_CharaSound = 0;
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale3DRate * 5.f);
					}
				}
				//R
				if ((27.f < Time && Time < 28.f)) {
					if (this->m_CharaSound != 1) {
						this->m_CharaSound = 1;
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale3DRate * 5.f);
					}
				}
			}
			else {
				this->m_CharaSound = -1;
			}
		}
		//SetMat指示更新
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* OptionParts = OptionManager::Instance();
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
			}
			float SwitchPer = 1.f;
			if (GetGunPtrNow()) {
				SwitchPer = GetGunPtrNow()->GetSwitchPer();
			}
			Matrix3x3DX CharaLocalRotationCache = 
				Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_LeanControl.GetRad()) *
				Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_RotateControl.GetRad().x) *
				Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadBottomChange());
			//
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)),
				(
					Matrix3x3DX::RotAxis(Vector3DX::right(), -this->m_RotateControl.GetRad().x / 2.f) *
					CharaLocalRotationCache
					).Get44DX() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper2)),
				(
					Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(32 * SwitchPer)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-35)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_RotateControl.GetRad().x / 2.f) *
					this->m_HitReactionControl.GetHitReactionMat()
					).Get44DX() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper2)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Neck)),
				(
					Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-32 * SwitchPer)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(25)) *
					Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(12 * SwitchPer))
					).Get44DX() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Neck)));
			this->m_HitReactionControl.Update();
			//上半身アニメ演算
			this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Upper_Ready)) = 1.f;
			//指演算
			if (GetGunPtrNow()) {
				auto& FingerPer = GetGunPtrNow()->GetGunAnimeNow().GetFingerPer();
				Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_Thumb)), FingerPer.GetFingerPer(0, 0), 0.8f, EasingType::OutExpo);
				if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::Shot) {//撃つときはそちらを参照する
					Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_Point)), 0.4f, 0.5f, EasingType::OutExpo);
				}
				else {
					Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_Point)), FingerPer.GetFingerPer(0, 1), 0.8f, EasingType::OutExpo);
				}
				Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_middle)), FingerPer.GetFingerPer(0, 2), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_Ring)), FingerPer.GetFingerPer(0, 3), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_Pinky)), FingerPer.GetFingerPer(0, 4), 0.8f, EasingType::OutExpo);
				//
				Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Left_Thumb)), FingerPer.GetFingerPer(1, 0), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Left_Point)), FingerPer.GetFingerPer(1, 1), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Left_middle)), FingerPer.GetFingerPer(1, 2), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Left_Ring)), FingerPer.GetFingerPer(1, 3), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Left_Pinky)), FingerPer.GetFingerPer(1, 4), 0.8f, EasingType::OutExpo);
			}
			//下半身アニメ演算
			if (this->m_AnimPerBuf[static_cast<int>(GetBottomTurnAnimSel())] > 0.f) {
				ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomTurnAnimSel()), 0.5f);
			}
			if (this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Bottom_Stand_Run)] > 0.f) {
				ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaAnimeID::Bottom_Stand_Run), GetSpeed() * 0.5f);
			}
			float AnimSpeed = 1.15f * std::clamp(GetSpeed() / 0.65f, 0.5f, 1.f);
			if (this->m_AnimPerBuf[static_cast<int>(GetBottomWalkAnimSel())] > 0.f) {
				ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomWalkAnimSel()), this->m_MoveControl.GetVecFront() * AnimSpeed);
			}
			if (this->m_AnimPerBuf[static_cast<int>(GetBottomLeftStepAnimSel())] > 0.f) {
				ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomLeftStepAnimSel()), this->m_MoveControl.GetVecLeft() * AnimSpeed);
			}
			if (this->m_AnimPerBuf[static_cast<int>(GetBottomWalkBackAnimSel())] > 0.f) {
				ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomWalkBackAnimSel()), this->m_MoveControl.GetVecRear() * AnimSpeed);
			}
			if (this->m_AnimPerBuf[static_cast<int>(GetBottomRightStepAnimSel())] > 0.f) {
				ObjectBaseClass::SetAnimLoop(static_cast<int>(GetBottomRightStepAnimSel()), this->m_MoveControl.GetVecRight() * AnimSpeed);
			}
			//アニメ反映
			for (int loop = 0, max = static_cast<int>(GetObj().GetAnimNum()); loop < max; ++loop) {
				SetObj().SetAnim(loop).SetPer(this->m_AnimPerBuf.at(loop));
			}
			SetObj().UpdateAnimAll();
			{
				Vector3DX PosBuf = GetMove().GetPosBuf();
				//素の移動ベクトル
				Vector3DX vec = (this->m_MoveControl.GetVecPower() > 0.f) ?
					Matrix3x3DX::Vtrans(this->m_MoveControl.GetVecMove() * GetSpeed() * 60.f * DXLib_refParts->GetDeltaTime(), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadUpper())) :
					Vector3DX::zero();
				//床判定
				Vector3DX EndPos = PosBuf - Vector3DX::up() * (0.5f * Scale3DRate);
				if (BackGround->CheckLinetoMap(PosBuf + Vector3DX::up() * (0.f * Scale3DRate), &EndPos)) {
					float GroundHight = EndPos.y - (0.12f * Scale3DRate);
					if ((PosBuf.y - GroundHight) > (0.008f * Scale3DRate / DXLib_refParts->GetDeltaTime())) {
						PosBuf.y = GroundHight;//高所落下の際は即時反映
					}
					else {
						Easing(&PosBuf.y, GroundHight, 0.6f, EasingType::OutExpo);//それ以外は即時反映
					}
					vec.y = 0.f;
				}
				else {
					vec.y = (GetMove().GetVec().y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps())));
				}
				//床判定を加味した移動ベクトル
				SetMove().SetVec(vec);
				PosBuf += GetMove().GetVec();
				//壁判定
				BackGround->CheckMapWall(GetMove().GetRePos(), &PosBuf, Vector3DX::up() * (0.6f * Scale3DRate + 0.1f), Vector3DX::up() * (1.6f * Scale3DRate), 0.7f * Scale3DRate);
				//ほかプレイヤーとの判定
				{
					float Radius = 2.f * 0.5f * Scale3DRate;
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						if (loop == GetMyPlayerID()) { continue; }
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						if (!chara->IsAlive()) { continue; }
						//自分が当たったら押し戻す
						Vector3DX Vec = (chara->GetMove().GetPos() - GetMove().GetPos()); Vec.y = (0.f);
						float Len = Vec.magnitude();
						if (Len < Radius) {
							PosBuf = PosBuf + Vec.normalized() * (Len - Radius);
						}
					}
				}
				//座標オーバーライド
				if (this->m_MoveOverRideFlag) {
					this->m_MoveOverRideFlag = false;
					SetMove() = this->m_OverRideInfo;
				}
				//ベースに反映
				SetMove().SetPos(PosBuf);
				SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadBottom()));
				SetMove().Update(0.8f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
			if (IsAlive()) {
				Matrix3x3DX CharaRotationCache = CharaLocalRotationCache * GetMove().GetMat();
				//銃の位置を指定するアニメ
				for (int loop = 0, max = this->m_GunPtrControl.GetGunNum(); loop < max; ++loop) {
					auto& pGun = this->m_GunPtrControl.GetGunPtr(loop);
					if (!pGun) { continue; }

					//スリング場所探索
					if (pGun->IsNeedCalcSling()) {
						switch (loop) {
						case 0://メイン武器のスリング
							this->m_SlingZrad.Update(DXLib_refParts->GetDeltaTime());
							this->m_SlingZrad.AddRad((0.5f * (this->m_RotateControl.GetRad().y - this->m_RotateControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());
							pGun->SetSlingMat(
								Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
								(
									Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingZrad.GetRad()) *
									Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetRad().y)
								),
								GetFrameWorldMat(CharaFrame::Head).pos() +
								GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.3f * Scale3DRate) +
								(
									GetMove().GetMat().xvec() * sin(this->m_SlingZrad.GetRad()) +
									GetMove().GetMat().yvec() * cos(this->m_SlingZrad.GetRad())
									) * -(0.5f * Scale3DRate)
							);
							break;
						case 1://ホルスター
							pGun->SetSlingMat(
								Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-90)) * Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper)),
								GetFrameWorldMat(CharaFrame::Upper).pos() + GetFrameWorldMat(CharaFrame::Upper).yvec() * (-0.5f * Scale3DRate) + GetFrameWorldMat(CharaFrame::Upper).xvec() * (-0.3f * Scale3DRate)
							);
							break;
						case 2://グレネード
							pGun->SetSlingMat(
								Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::LeftMag)),
								GetFrameWorldMat(CharaFrame::LeftMag).pos()
							);
							break;
						default:
							break;
						}
					}

					bool IsSelect = loop == this->m_GunPtrControl.GetNowGunSelect();
					if (IsSelect) {
						pGun->SetMagazinePoachMat(GetFrameWorldMat(CharaFrame::LeftMag));
						pGun->SetGrenadeThrowRot(GetEyeRotationCache());
						//アニメーション
						pGun->UpdateGunAnimePer(GetIsADS());
					}
					else {
						pGun->SetActiveAll(true);
						//アニメーション
						pGun->InitGunAnimePer();
					}
					pGun->UpdateGunMat(IsSelect, GetIsADS(), CharaRotationCache, GetFrameWorldMat(CharaFrame::Head).pos(), this->m_RotateControl.GetRad());
				}
				//手の位置を制御
				if ((GetMyPlayerID() == PlayerMngr->GetWatchPlayer()) || GetCanLookByPlayer()) {
					if (GetGunPtrNow()) {
						//銃座標
						{
							IK_RightArm(
								&SetObj(),
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
							Easing(&this->m_ArmBreakPer, (
								(GetGunPtrNow()->IsCanShot() && this->m_ArmBreak)
								|| (GetGunPtrNow()->GetGunAnime() == GunAnimeID::Watch)//TODO:専用の左腕アクション
								|| (GetGunPtrNow()->GetGunPartsSlot()->GetGunPartsData()->GetIsThrowWeapon() && (GetGunPtrNow()->GetGunAnime() != GunAnimeID::ThrowReady))//TODO:専用の左腕アクション
								) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
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
										).Get44DX() *
									Matrix4x4DX::Mtrans(
										GetFrameWorldMat(CharaFrame::Head).pos() +
										GetFrameWorldMat(CharaFrame::Head).xvec() * (0.15f * Scale3DRate) +
										GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.1f * Scale3DRate) +
										(
											GetMove().GetMat().xvec() * sin(this->m_SlingArmZrad.GetRad()) +
											GetMove().GetMat().yvec() * cos(this->m_SlingArmZrad.GetRad())
											) * -(0.5f * Scale3DRate)
									);
								HandMat = Lerp(HandMat, SlingArmMat, this->m_ArmBreakPer);
							}
							if (this->m_ArmBreak) {
								HandMat = HandMat.rotation() * Matrix4x4DX::Mtrans(HandMat.pos() + Vector3DX::vget(GetRandf(1.f), GetRandf(1.f), GetRandf(1.f)) * (0.002f * Scale3DRate));
							}
							IK_LeftArm(
								&SetObj(),
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
				//ヒットボックス
				this->m_HitBoxControl.Update(this, (GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);
				//目の座標取得
				{
					bool HeadBobbing = ((GetMyPlayerID() != PlayerMngr->GetWatchPlayer()) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
					this->m_EyePositionCache = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f;
					if (HeadBobbing) {
						this->m_EyePositionCache += this->m_WalkSwingControl.CalcEye(CharaRotationCache, std::clamp(std::clamp(this->m_MoveControl.GetVecPower(), 0.f, 1.f) * GetSpeed() / 0.625f, 0.f, 0.85f) / 0.65f, 5.f);
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
			else {
				for (int loop = 0, max = this->m_GunPtrControl.GetGunNum(); loop < max; ++loop) {
					auto& pGun = this->m_GunPtrControl.GetGunPtr(loop);
					if (!pGun) { continue; }
					if (loop == this->m_GunPtrControl.GetNowGunSelect()) {
						auto Mat = this->m_RagDollControl.GetFrameMat(RagFrame::RIGHThand);
						pGun->SetGunMat(Matrix3x3DX::Get33DX(Mat), Mat.pos());
						pGun->SetActiveAll(true);
					}
					else {
						pGun->SetActiveAll(false);
					}
				}
			}
			//ラグドール
			this->m_RagDollControl.Update(IsAlive());
			//視認判定系
			this->m_IsActiveCameraPos = false;
			if (GetMyPlayerID() != PlayerMngr->GetWatchPlayer()) {
				auto& ViewChara = PlayerMngr->GetPlayer(PlayerMngr->GetWatchPlayer())->GetChara();
				Vector3DX EndPos = GetEyePositionCache();
				this->m_CanLookTarget = !BackGround->CheckLinetoMap(ViewChara->GetEyePositionCache(), &EndPos);
				this->m_Length = (GetEyePositionCache() - ViewChara->GetEyePositionCache()).magnitude();
			}
		}
		//
		void			CharacterClass::LoadChara(const std::string& FolderName, PlayerID ID) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto& player = PlayerMngr->GetPlayer(ID);

			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";

			player->SetChara(std::make_shared<CharacterClass>());
			ObjMngr->AddObject(player->GetChara());
			ObjMngr->LoadModel(player->GetChara(), player->GetChara(), Path.c_str());
			player->GetChara()->Init();
			player->SetAI(std::make_shared<AIControl>(ID));
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
			ObjectBaseClass::SetMinAABB(Vector3DX::vget(-2.5f, -0.5, -2.5f) * Scale3DRate);
			ObjectBaseClass::SetMaxAABB(Vector3DX::vget(2.5f, 2.0f, 2.5f) * Scale3DRate);
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			ExecuteInput();
			ExecuteMatrix();
		}
		void			CharacterClass::CheckDraw_Sub(int) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (GetMyPlayerID() != PlayerMngr->GetWatchPlayer()) {
				this->m_IsActiveCameraPos |= this->m_CameraPos.Calc(GetEyePositionCache());
			}
		}
		void			CharacterClass::Draw(bool isDrawSemiTrans, int Range) noexcept {
			if (!IsActive()) { return; }
			if (!IsDraw(Range)) { return; }
			int fog_enable;
			int fog_mode;
			int fog_r, fog_g, fog_b;
			float fog_start, fog_end;
			float fog_density;
			fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( true:有効 false:無効 )
			fog_mode = GetFogMode();														// フォグモードを取得する
			GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
			GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
			fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )


			//キャラ描画
			SetFogEnable(true);
			SetFogColor(0, 0, 0);
			//MV1SetMaterialTypeAll(GetObj().GetHandle(), DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
			if (IsAlive()) {
				for (int loop = 0, max = GetObj().GetMeshNum(); loop < max; ++loop) {
					if (GetObj().GetMeshSemiTransState(loop) == isDrawSemiTrans) {
						GetObj().DrawMesh(loop);
					}
				}
			}
			else {
				for (int loop = 0, max = GetRagDoll().GetMeshNum(); loop < max; ++loop) {
					if (GetRagDoll().GetMeshSemiTransState(loop) == isDrawSemiTrans) {
						GetRagDoll().DrawMesh(loop);
					}
				}
			}
			//hitbox描画
			//this->m_HitBoxControl.DrawHitBox();
			//
			SetFogEnable(fog_enable);
			SetFogMode(fog_mode);
			SetFogColor(fog_r, fog_g, fog_b);
			SetFogStartEnd(fog_start, fog_end);
			SetFogDensity(fog_density);
		}
		void			CharacterClass::DrawShadow(void) noexcept {
			if (!IsActive()) { return; }
			if (IsAlive()) {
				GetObj().DrawModel();
			}
			else {
				GetRagDoll().DrawModel();
			}
		}
	};
};
