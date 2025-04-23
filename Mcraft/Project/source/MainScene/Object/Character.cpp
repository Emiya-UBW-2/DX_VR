#include	"Character.hpp"

#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Charas {
		bool			CharacterObj::SetDamageEvent(const DamageEvent& Event) noexcept {
			if (GetMyPlayerID() == Event.DamageID) {
				auto* SE = SoundPool::Instance();
				auto* PlayerMngr = Player::PlayerManager::Instance();

				auto PrevLive = IsAlive();
				this->m_HP.Sub(Event.Damage);
				this->m_AP.Sub(Event.ArmerDamage);
				bool IsDeath = PrevLive && !IsAlive();

				if (Event.ShotID == PlayerMngr->GetWatchPlayerID()) {//�������L����
					//SE
					if (Event.Damage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Hit))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else if (Event.ArmerDamage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f, 128);
					}
					//�q�b�g�J�E���g
					if ((Event.Damage >= 0) && (Event.ArmerDamage >= 0)) {
						PlayerMngr->GetPlayer(Event.ShotID)->AddHit(1);
						//�q�b�g���W�\����o�^
						HitMarkerPool::Instance()->AddMarker(Event.m_EndPos, Event.Damage, Event.ArmerDamage);
					}
					if (IsDeath) {
						PlayerMngr->GetPlayer(Event.ShotID)->AddScore(100);
						PlayerMngr->GetPlayer(Event.ShotID)->AddKill(1);
					}
				}
				if (Event.DamageID == PlayerMngr->GetWatchPlayerID()) {//�����ꂽ�L����
					if (Event.Damage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitMe))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
					else if (Event.ArmerDamage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f, 255);
					}
				}
				//�G�t�F�N�g�Z�b�g
				if (Event.Damage > 0) {
					EffectSingleton::Instance()->SetOnce(Effect::ef_hitblood, Event.m_EndPos, Vector3DX::forward(), Scale3DRate);
					EffectSingleton::Instance()->SetEffectSpeed(Effect::ef_hitblood, 2.f);
				}
				else if (Event.ArmerDamage > 0) {
					EffectSingleton::Instance()->SetOnce(Effect::ef_gndsmoke, Event.m_EndPos, (Event.m_StartPos - Event.m_EndPos).normalized(), 0.25f * Scale3DRate);
				}
				//�q�b�g���[�V����
				if (Event.Damage > 0) {
					this->m_HitReactionControl.SetHit(Matrix3x3DX::Vtrans(Vector3DX::Cross((Event.m_EndPos - Event.m_StartPos).normalized(), Vector3DX::up()) * -1.f, Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper2)).inverse()));
					switch (static_cast<HitType>(Event.m_HitType)) {
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
				//�{�C�X
				if (IsAlive()) {
					//��e�{�C�X
					if ((Event.Damage >= 0) && (Event.ArmerDamage >= 0)) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_Hurt1) + GetRand(6 - 1))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
					}
				}
				else if (IsDeath) {
					//���S�{�C�X
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
		const bool		CharacterObj::CheckDamageRay(HitPoint BaseDamage, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			if (!IsAlive()) { return false; }
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, GetMove().GetPos()) <= 2.0f * Scale3DRate)) { return false; }

			if (AttackID != PlayerMngr->GetWatchPlayerID()) {
				this->m_ConcussionSwitch = true;
			}
			//��e����
			auto* HitPtr = this->m_HitBoxControl.GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				HitPoint Damage = BaseDamage;
				ArmerPoint ArmerDamage = 0;
				//���ʃ_���[�W���Z
				switch (HitPtr->GetColType()) {
				case HitType::Head:
					if (GetMyPlayerID() != PlayerMngr->GetWatchPlayerID()) {//���@�̓w�b�V������Ȃ�
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
				//�_���[�W�o�^
				if (AttackID >= 0) {
					PlayerMngr->GetPlayer(AttackID)->GetChara()->SetDamage(GetMyPlayerID(), Damage, ArmerDamage, static_cast<int>(HitPtr->GetColType()), StartPos, *pEndPos);
				}
				else if (AttackID == -1) {
					//PlayerMngr->GetVehicle()->SetDamage(GetMyPlayerID(), Damage, ArmerDamage, static_cast<int>(HitPtr->GetColType()), StartPos, *pEndPos);
				}
				else {
					//TODO PlayerMngr->GetHelicopter()
				}
				return true;
			}
			return false;
		}
		//����
		void			CharacterObj::UpdateInput(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//
			if (!IsAlive()) { return; }
			if (this->m_Input.GetPADSTrigger(Controls::PADS::SQUAT)) {
				this->m_IsSquat ^= 1;
				//���Ⴊ�݉�
				if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetEyePositionCache(), Scale3DRate * 3.f);
				}
			}
			if (GetGunPtrNow()) {
				//�e�Ђ�����
				if (this->m_StuckGunTimer == 0.f) {
					this->m_StuckGunTimer = 0.1f;

					Vector3DX GunStart = GetEyePositionCache();
					Vector3DX GunEnd = (GetGunPtrNow()->GetBaseMuzzleMat() * GetFrameWorldMat(CharaFrame::Head)).pos();
					bool IsHit = BackGround::BackGroundControl::Instance()->CheckLinetoMap(GunStart, GunEnd);
					//�ق��v���C���[�Ƃ̔���
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
						auto Zvec = GetFrameWorldMat(CharaFrame::Head).zvec2();
						if (Zvec.y < std::sin(deg2rad(-30))) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
						}
						else {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::HighReady);
						}
					}
				}
				//����ؑ�
				if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::LowReady) {
					int Wheel = 0;
					if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
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
				//�A�N�V����
				switch (GetGunPtrNow()->GetGunAnime()) {
				case GunAnimeID::LowReady:
					if (this->m_GunPtrControl.IsChangeGunSelect() && GetGunPtrNow()->GetGunAnimBlendPer(GunAnimeID::LowReady) > 0.95f) {
						this->m_GunPtrControl.InvokeReserveGunSelect();
						GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetEyePositionCache(), Scale3DRate * 3.f);
					}
					break;
				case GunAnimeID::Base:
					if (GetGunPtrNow()->GetCanShot()) {
						//����𒭂߂�
						if (this->m_Input.GetPADSPress(Controls::PADS::CHECK)) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Watch);
						}
						if (!GetGunPtrNow()->GetModifySlot()->GetMyData()->GetIsThrowWeapon()) {
							//Reload
							if (this->m_Input.GetPADSPress(Controls::PADS::RELOAD) ||
								((GetGunPtrNow()->GetAmmoNumTotal() == 0) && this->m_Input.GetPADSPress(Controls::PADS::SHOT))) {
								if (GetGunPtrNow()->ReloadStart()) {
									if (GetMyPlayerID() != PlayerMngr->GetWatchPlayerID()) {
										if (GetRand(100) < 50) {
											SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_reload))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
										}
										else {
											SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_takecover))->Play3D(GetEyePositionCache(), Scale3DRate * 10.f);
										}
									}
								}
							}
							//�ˌ�
							else if (this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
								if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
									if (this->m_Input.GetPADSTrigger(Controls::PADS::SHOT)) {
										SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Trigger))->Play3D(GetEyePositionCache(), Scale3DRate * 5.f);
									}
								}
								GetGunPtrNow()->SetShotStart();
							}
							//�O���l�[�h�\��
							else if (this->m_Input.GetPADSPress(Controls::PADS::THROW)) {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
								//��������ł���ŏ��̕���ɐ؂�ւ�
								this->m_GunPtrControl.GunChangeThrowWeapon(true);
							}
						}
						else {
							//�O���l�[�h���Ă�
							if (this->m_Input.GetPADSPress(Controls::PADS::THROW) || this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::ThrowReady);
							}
						}
					}
					break;
				case GunAnimeID::Shot:
					//�ˌ��I���t���O
					if (GetGunPtrNow()->GetShotType() == Guns::SHOTTYPE::FULL) {
						GetGunPtrNow()->SetShotEnd(true);
					}
					else {
						GetGunPtrNow()->SetShotEnd(!this->m_Input.GetPADSPress(Controls::PADS::SHOT));
					}
					break;
				case GunAnimeID::ReloadWait:
				case GunAnimeID::Reload:
					if (GetGunPtrNow()->GetReloadType() == Guns::RELOADTYPE::AMMO) {
						if (this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
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
						//��������ł͂Ȃ��ŏ��̕���ɐ؂�ւ�
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
			//���[��
			if (this->m_LeanControl.Update(this->m_Input.GetPADSTrigger(Controls::PADS::LEAN_L), this->m_Input.GetPADSTrigger(Controls::PADS::LEAN_R))) {
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetEyePositionCache(), Scale3DRate * 3.f);
			}
			if (GetGunPtrNow()) {
				GetGunPtrNow()->CalcSwitchPer(this->m_LeanControl.GetRate() >= 0 || !GetGunPtrNow()->IsCanShot());
			}
			//�����g
			this->m_BottomAnimSelect = GetBottomStandAnimSelect();
			if (IsMoveLeft()) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSelect(); }
			if (IsMoveRight()) { this->m_BottomAnimSelect = GetBottomRightStepAnimSelect(); }
			if (IsMoveBack()) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSelect(); }
			if (IsMoveFront()) { this->m_BottomAnimSelect = GetBottomWalkAnimSelect(); }
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(GetBottomTurnAnimSelect())), (!this->m_IsSquat && this->m_RotateControl.IsTurnBody()) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
			Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Rappelling)), 0.f, 0.8f, EasingType::OutExpo);
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
			if (this->m_IsRappelling) {
				for (int loop = 0; loop < static_cast<int>(CharaAnimeID::AnimeIDMax); ++loop) {
					CharaAnimeID ID = static_cast<CharaAnimeID>(loop);
					if (ID == CharaAnimeID::Rappelling) {
						this->m_AnimPerBuf[loop] = 1.f;
					}
					else {
						this->m_AnimPerBuf[loop] = 0.f;
					}
				}
				if (!this->m_IsRappellingEnd) {
					SetObj().SetAnim(static_cast<int>(CharaAnimeID::Rappelling)).GoStart();
				}
				else {
					SetAnimOnce(static_cast<int>(CharaAnimeID::Rappelling), 0.5f);
					if (GetObj().GetAnim(static_cast<int>(CharaAnimeID::Rappelling)).TimeEnd()) {
						this->m_IsRappelling = false;
						this->m_IsRappellingEnd = false;
					}
				}
			}

			//����
			if (this->m_BottomAnimSelect != GetBottomStandAnimSelect()) {
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
				this->m_CharaSound = InvalidID;
			}
		}
		//SetMat�w���X�V
		void			CharacterObj::UpdateMatrix(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			auto* OptionParts = OptionManager::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			//
			if (IsAlive()) {
				if (IsLowHP()) {
					this->m_HPRec += DXLib_refParts->GetDeltaTime();
					if (this->m_HPRec >= 0.f) {
						this->m_HPRec -= 2.f;
						Heal(2);
						if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing))->Play(DX_PLAYTYPE_BACK);
						}
					}
				}
				else {
					if (this->m_HPRec != 0.f) {
						if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
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
			//�㔼�g�A�j�����Z
			this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Upper_Ready)) = 1.f;
			//�w���Z
			if (GetGunPtrNow()) {
				auto& FingerPer = GetGunPtrNow()->GetGunAnimeNow().GetFingerPer();
				Easing(&this->m_AnimPerBuf.at(static_cast<int>(CharaAnimeID::Right_Thumb)), FingerPer.GetFingerPer(0, 0), 0.8f, EasingType::OutExpo);
				if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::Shot) {//���Ƃ��͂�������Q�Ƃ���
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
			//�����g�A�j�����Z
			if (IsDraw(0) || IsDraw(1) || IsDraw(2)) {
				if (this->m_AnimPerBuf[static_cast<int>(GetBottomTurnAnimSelect())] > 0.f) {
					SetAnimLoop(static_cast<int>(GetBottomTurnAnimSelect()), 0.5f);
				}
				if (this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Bottom_Stand_Run)] > 0.f) {
					SetAnimLoop(static_cast<int>(CharaAnimeID::Bottom_Stand_Run), GetSpeed() * 0.5f);
				}
				float AnimSpeed = 1.15f * std::clamp(GetSpeed() / 0.65f, 0.5f, 1.f);
				if (this->m_AnimPerBuf[static_cast<int>(GetBottomWalkAnimSelect())] > 0.f) {
					SetAnimLoop(static_cast<int>(GetBottomWalkAnimSelect()), this->m_MoveControl.GetVecFront() * AnimSpeed);
				}
				if (this->m_AnimPerBuf[static_cast<int>(GetBottomLeftStepAnimSelect())] > 0.f) {
					SetAnimLoop(static_cast<int>(GetBottomLeftStepAnimSelect()), this->m_MoveControl.GetVecLeft() * AnimSpeed);
				}
				if (this->m_AnimPerBuf[static_cast<int>(GetBottomWalkBackAnimSelect())] > 0.f) {
					SetAnimLoop(static_cast<int>(GetBottomWalkBackAnimSelect()), this->m_MoveControl.GetVecRear() * AnimSpeed);
				}
				if (this->m_AnimPerBuf[static_cast<int>(GetBottomRightStepAnimSelect())] > 0.f) {
					SetAnimLoop(static_cast<int>(GetBottomRightStepAnimSelect()), this->m_MoveControl.GetVecRight() * AnimSpeed);
				}
				//�A�j�����f
				for (int loop = 0, max = static_cast<int>(GetObj().GetAnimNum()); loop < max; ++loop) {
					SetObj().SetAnim(loop).SetPer(this->m_AnimPerBuf.at(loop));
				}
				SetObj().UpdateAnimAll();
			}
			{
				Vector3DX PosBuf = GetMove().GetPosBuf();
				//�f�̈ړ��x�N�g��
				Vector3DX vec = (this->m_MoveControl.GetVecPower() > 0.f) ?
					Matrix3x3DX::Vtrans(this->m_MoveControl.GetVecMove() * GetSpeed() * 60.f * DXLib_refParts->GetDeltaTime(), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadUpper())) :
					Vector3DX::zero();
				//������
				Vector3DX EndPos = PosBuf - Vector3DX::up() * (0.5f * Scale3DRate);
				if (BackGroundParts->CheckLinetoMap(PosBuf + Vector3DX::up() * (0.f * Scale3DRate), &EndPos)) {
					float GroundHight = EndPos.y - (0.12f * Scale3DRate);
					if ((PosBuf.y - GroundHight) > (0.008f * Scale3DRate / DXLib_refParts->GetDeltaTime())) {
						PosBuf.y = GroundHight;//���������̍ۂ͑������f
					}
					else {
						Easing(&PosBuf.y, GroundHight, 0.6f, EasingType::OutExpo);//����ȊO�͑������f
					}
					vec.y = 0.f;
					if (this->m_IsRappelling) {
						this->m_IsRappellingEnd = true;
					}
				}
				else {
					if (!this->m_IsRappelling) {
						vec.y = (GetMove().GetVec().y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps())));
					}
				}
				//����������������ړ��x�N�g��
				SetMove().SetVec(vec);
				PosBuf += GetMove().GetVec();
				//�ǔ���
				if (GetMove().GetVec().sqrMagnitude() > 0.01f * 0.01f) {
					BackGroundParts->CheckMapWall(GetMove().GetRePos(), &PosBuf, Vector3DX::up() * (0.6f * Scale3DRate + 0.1f), Vector3DX::up() * (1.6f * Scale3DRate), 0.7f * Scale3DRate);
				}
				//�ق��v���C���[�Ƃ̔���
				{
					float Radius = 2.f * 0.5f * Scale3DRate;
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						if (loop == GetMyPlayerID()) { continue; }
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						if (!chara->IsAlive()) { continue; }
						if (chara->GetIsRappelling()) { continue; }
						//���������������牟���߂�
						Vector3DX Vec = (chara->GetMove().GetPos() - GetMove().GetPos()); Vec.y = (0.f);
						float Len = Vec.magnitude();
						if (Len < Radius) {
							PosBuf = PosBuf + Vec.normalized() * (Len - Radius);
						}
					}
				}
				//�x�[�X�ɔ��f
				SetMove().SetPos(PosBuf);
				SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadBottom()));
				//���W�I�[�o�[���C�h
				if (this->m_MoveOverRideFlag) {
					this->m_MoveOverRideFlag = false;
					SetMove().SetPos(this->m_OverRideInfo.pos);
					SetMove().SetMat(this->m_OverRideInfo.mat);
					SetMove().SetVec(this->m_OverRideInfo.vec);
					this->m_RotateControl.SetRad(this->m_OverRideInfo.WatchRad);
				}
				SetMove().Update(0.8f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
			if (IsAlive()) {
				Matrix3x3DX CharaRotationCache = CharaLocalRotationCache * GetMove().GetMat();
				//�e�̈ʒu���w�肷��A�j��
				for (int loop = 0, max = this->m_GunPtrControl.GetGunNum(); loop < max; ++loop) {
					auto& pGun = this->m_GunPtrControl.GetGunPtr(loop);
					if (!pGun) { continue; }

					//�X�����O�ꏊ�T��
					if (pGun->IsNeedCalcSling()) {
						switch (loop) {
						case 0://���C������̃X�����O
							this->m_SlingZrad.Update(DXLib_refParts->GetDeltaTime());
							this->m_SlingZrad.AddRad((0.5f * (this->m_RotateControl.GetRad().y - this->m_RotateControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());
							pGun->SetSlingMat(
								Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
								(
									Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingZrad.GetRad()) *
									Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetRad().y)
								),
								GetFrameWorldMat(CharaFrame::Neck).pos() +
								GetFrameWorldMat(CharaFrame::Neck).zvec2() * (0.15f * Scale3DRate) +
								GetMove().GetMat().yvec() * (0.2f * Scale3DRate) +
								(
									GetMove().GetMat().xvec() * sin(this->m_SlingZrad.GetRad()) +
									GetMove().GetMat().yvec() * cos(this->m_SlingZrad.GetRad())
									) * -(0.5f * Scale3DRate)
							);
							break;
						case 1://�z���X�^�[
							pGun->SetSlingMat(
								Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-90)) * Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper)),
								GetFrameWorldMat(CharaFrame::Upper).pos() + GetFrameWorldMat(CharaFrame::Upper).yvec() * (-0.5f * Scale3DRate) + GetFrameWorldMat(CharaFrame::Upper).xvec() * (-0.3f * Scale3DRate)
							);
							break;
						case 2://�O���l�[�h
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
						//�A�j���[�V����
						pGun->UpdateGunAnimePer(GetIsADS());
					}
					else {
						pGun->SetActiveAll(true);
						//�A�j���[�V����
						pGun->InitGunAnimePer();
					}
					pGun->UpdateGunMat(IsSelect, GetIsADS(), CharaRotationCache, GetFrameWorldMat(CharaFrame::Head).pos(), this->m_RotateControl.GetRad());
				}
				//��̈ʒu�𐧌�
				if ((GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) || GetCanLookByPlayer()) {
					if (GetGunPtrNow()) {
						//�e���W
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
								|| (GetGunPtrNow()->GetGunAnime() == GunAnimeID::Watch)//TODO:��p�̍��r�A�N�V����
								|| (GetGunPtrNow()->GetModifySlot()->GetMyData()->GetIsThrowWeapon() && (GetGunPtrNow()->GetGunAnime() != GunAnimeID::ThrowReady))//TODO:��p�̍��r�A�N�V����
								) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
							if (this->m_ArmBreakPer > 0.01f) {
								this->m_SlingArmZrad.Update(DXLib_refParts->GetDeltaTime());
								this->m_SlingArmZrad.AddRad((0.2f * (this->m_RotateControl.GetRad().y - this->m_RotateControl.GetYRadBottom())) * DXLib_refParts->GetDeltaTime());
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
										GetFrameWorldMat(CharaFrame::Head).zvec2() * (0.1f * Scale3DRate) +
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
				//�q�b�g�{�b�N�X
				this->m_HitBoxControl.Update(this, (GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);
				//�ڂ̍��W�擾
				{
					bool HeadBobbing = ((GetMyPlayerID() != PlayerMngr->GetWatchPlayerID()) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
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
						pGun->SetGunMat(Matrix3x3DX::Get33DX(Mat) * Matrix3x3DX::RotAxis(Vector3DX::right(),deg2rad(-90)), Mat.pos());
						pGun->SetActiveAll(true);
					}
					else {
						pGun->SetActiveAll(false);
					}
				}
			}
			//���O�h�[��
			this->m_RagDollControl.Update(IsAlive());
			//���F����n
			this->m_IsActiveCameraPos = false;
			if (GetMyPlayerID() != PlayerMngr->GetWatchPlayerID()) {
				auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();
				Vector3DX EndPos = GetEyePositionCache();
				this->m_CanLookTarget = !BackGroundParts->CheckLinetoMap(ViewChara->GetEyePositionCache(), &EndPos);
				this->m_Length = (GetEyePositionCache() - ViewChara->GetEyePositionCache()).magnitude();
			}
			//�R���J�b�V����
			if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
				if (this->m_ConcussionSwitch) {
					this->m_ConcussionSwitch = false;
					this->m_Concussion = 1.f;
				}
				PostPassParts->Set_is_Blackout(this->m_Concussion > 0.f);
				if (this->m_Concussion == 1.f) {
					Camera3D::Instance()->SetCamShake(0.5f, 0.01f * Scale3DRate);
				}
				if (this->m_Concussion > 0.9f) {
					Easing(&this->m_ConcussionPer, 1.f, 0.1f, EasingType::OutExpo);
				}
				else if (this->m_Concussion > 0.25f) {
					if (this->m_ConcussionPer > 0.25f) {
						Easing(&this->m_ConcussionPer, 0.f, 0.8f, EasingType::OutExpo);
					}
					else {
						this->m_ConcussionPer = 0.25f;
					}
				}
				else {
					Easing(&this->m_ConcussionPer, 0.f, 0.8f, EasingType::OutExpo);
				}
				PostPassParts->Set_Per_Blackout(this->m_ConcussionPer * 2.f);
				this->m_Concussion = std::max(this->m_Concussion - DXLib_refParts->GetDeltaTime(), 0.f);
			}
		}
		//
		void			CharacterObj::LoadChara(const std::string& FolderName, PlayerID ID) noexcept {
			auto& player = Player::PlayerManager::Instance()->GetPlayer(ID);

			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";
			player->SetChara(std::make_shared<CharacterObj>());
			ObjectManager::Instance()->InitObject(player->GetChara(), Path.c_str());
			player->SetAI(std::make_shared<AIs::AIControl>(ID));
		}
		void			CharacterObj::LoadCharaGun(const std::string& FolderName, int Select) noexcept {
			std::string Path = "data/gun/";
			Path += FolderName;
			Path += "/";
			this->m_GunPtrControl.SetGunPtr(Select, std::make_shared<Guns::GunObj>());
			ObjectManager::Instance()->InitObject(this->m_GunPtrControl.GetGunPtr(Select), Path.c_str());
		}
		//
		void			CharacterObj::Init_Sub(void) noexcept {
			this->m_HitBoxControl.Init();
			this->m_ArmBreak = false;
			SetMinAABB(Vector3DX::vget(-2.5f, -0.5, -2.5f) * Scale3DRate);
			SetMaxAABB(Vector3DX::vget(2.5f, 2.0f, 2.5f) * Scale3DRate);
		}
		void			CharacterObj::FirstUpdate(void) noexcept {
			UpdateInput();
			UpdateMatrix();
		}
		void			CharacterObj::CheckDraw_Sub(int) noexcept {
			if (GetMyPlayerID() != Player::PlayerManager::Instance()->GetWatchPlayerID()) {
				this->m_IsActiveCameraPos |= this->m_CameraPos.Calc(GetEyePositionCache());
			}
		}
		void			CharacterObj::Draw(bool isDrawSemiTrans, int Range) noexcept {
			if (!IsActive()) { return; }
			if (!IsDraw(Range)) { return; }
			int fog_r, fog_g, fog_b;
			GetFogColor(&fog_r, &fog_g, &fog_b);											// �t�H�O�J���[���擾����
			SetFogColor(0, 0, 0);
			//�L�����`��
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
			//hitbox�`��
#if defined(DEBUG) && DRAW_HITBOX
			this->m_HitBoxControl.Draw();
#endif
			//
			SetFogColor(fog_r, fog_g, fog_b);
		}
		void			CharacterObj::DrawShadow(void) noexcept {
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
