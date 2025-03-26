#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const Matrix3x3DX CharacterClass::GetCharaDir(void) const noexcept {
			auto tmpUpperMatrix = Matrix3x3DX::RotAxis(Vector3DX::forward(), KeyControl::GetLeanRad());

			tmpUpperMatrix *=
				Matrix3x3DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x) *
				Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetYRadBottomChange());
			return tmpUpperMatrix * this->GetMove().GetMat();
		}
		//
		void			CharacterClass::Shot_Start() noexcept {
			if (GetMyPlayerID() == 0) {
				auto* PlayerMngr = Player::PlayerManager::Instance();
				PlayerMngr->GetPlayer(GetMyPlayerID())->AddShot(GetGunPtrNow()->GetPelletNum());
			}
			GetGunPtrNow()->SetBullet();
			//
			float Power = 0.0001f * GetGunPtrNow()->GetRecoilPower();
			if (GetIsSquat()) {
				Power *= 0.5f;
			}
			if (m_ArmBreak) {
				Power *= 1.5f;
			}
			this->m_RecoilRadAdd.Set(GetRandf(Power / 4.f), -Power, 0.f);
			//
			if (GetMyPlayerID() == 0) {
				Camera3D::Instance()->SetCamShake(0.1f, 0.1f);
			}
			//エフェクト
			auto mat = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Muzzle);
			switch (GetGunPtrNow()->GetGunShootSound()) {
			case GunShootSound::Normal:
				EffectControl::SetOnce((int)Sceneclass::Effect::ef_fire2, mat.pos(), mat.zvec() * -1.f, 0.5f);
				EffectControl::SetEffectSpeed((int)Sceneclass::Effect::ef_fire2, 2.f);
				break;
			case GunShootSound::Suppressor:
				EffectControl::SetOnce((int)Sceneclass::Effect::ef_fire2, mat.pos(), mat.zvec() * -1.f, 0.25f);
				EffectControl::SetEffectSpeed((int)Sceneclass::Effect::ef_fire2, 2.f);
				break;
			default:
				break;
			}
			//音
			this->m_SoundPower = 1.f * (CanLookTarget ? 1.f : 0.5f);
		}
		void			CharacterClass::Reload_Start() noexcept {
			auto* SE = SoundPool::Instance();
			if (GetGunPtrNow()) {
				int Num = GetGunPtrNow()->GetAmmoNum();
				int All = GetGunPtrNow()->GetAmmoAll();
				int UniqueID = GetGunPtrNow()->GetMagUniqueID();
				GetGunPtrNow()->SetReloadStart(m_MagStockControl.at(GetNowGunSelect()).GetNowMag().AmmoNum, m_MagStockControl.at(GetNowGunSelect()).GetNowMag().ModUniqueID);
				m_MagStockControl.at(GetNowGunSelect()).SetOldMag(Num, All, UniqueID);
			}
			if (GetMyPlayerID() != 0) {
				if (GetRand(1) == 0) {
					SE->Get(SoundType::SE, (int)SoundEnum::Man_reload)->Play3D(GetEyePosition(), Scale3DRate * 10.f);
				}
				else {
					SE->Get(SoundType::SE, (int)SoundEnum::Man_takecover)->Play3D(GetEyePosition(), Scale3DRate * 10.f);
				}
				this->m_SoundPower = 0.6f * (CanLookTarget ? 1.f : 0.5f);
			}
		}
		//
		const Matrix3x3DX CharacterClass::GetEyeRotation(void) const noexcept {
			Matrix3x3DX tmpUpperMatrix = Matrix3x3DX::RotAxis(Vector3DX::forward(), KeyControl::GetLeanRad() / 5.f);

			tmpUpperMatrix *=
				Matrix3x3DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x) *
				Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetYRadBottomChange());

			auto* OptionParts = OptionManager::Instance();
			bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			if (HeadBobbing) {
				tmpUpperMatrix = WalkSwingControl::GetWalkSwingMat() * tmpUpperMatrix;
			}
			tmpUpperMatrix *= this->GetMove().GetMat();

			return tmpUpperMatrix;
		}
		const Vector3DX CharacterClass::GetEyePosition(void) const noexcept {
			auto* OptionParts = OptionManager::Instance();
			bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));

			auto EyePosition = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f;
			if (HeadBobbing) {
				EyePosition += EyeSwingControl::GetEyeSwingPos();
			}
			if (GetGunPtrNow()) {
				EyePosition = Lerp<Vector3DX>(EyePosition, GetGunPtrNow()->GetEyePos(), GunReadyControl::GetADSPer());
			}
			return EyePosition;
		}
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			if (GetMyPlayerID() == value.DamageID) {
				auto PrevLive = LifeControl::IsAlive();
				LifeControl::SubHP(value.Damage);
				LifeControl::SubAP(value.ArmerDamage);
				if (value.ShotID == 0) {
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						PlayerMngr->GetPlayer(value.ShotID)->AddHit(1);
					}
				}
				if (LifeControl::IsAlive()) {
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						SE->Get(SoundType::SE, (int)SoundEnum::Man_Hurt1 + GetRand(6 - 1))->Play3D(GetEyePosition(), Scale3DRate * 10.f);
					}
					else {
						//SE->Get(SoundType::SE, (int)SoundEnum::Man_Hurt1 + GetRand(6 - 1))->Play3D(GetEyePosition(), Scale3DRate * 10.f);
						//
						if (!PrevLive) {
						}
					}
				}
				else if (PrevLive) {
					for (int i = 0; i < 6; i++) {
						SE->Get(SoundType::SE, (int)SoundEnum::Man_Hurt1 + i)->StopAll();
					}
					SE->Get(SoundType::SE, (int)SoundEnum::Man_contact)->StopAll();
					SE->Get(SoundType::SE, (int)SoundEnum::Man_openfire)->StopAll();
					SE->Get(SoundType::SE, (int)SoundEnum::Man_reload)->StopAll();
					SE->Get(SoundType::SE, (int)SoundEnum::Man_takecover)->StopAll();
					SE->Get(SoundType::SE, (int)SoundEnum::Man_breathing)->StopAll();
					SE->Get(SoundType::SE, (int)SoundEnum::Man_breathend)->StopAll();

					SE->Get(SoundType::SE, (int)SoundEnum::Man_Death1 + GetRand(8 - 1))->Play3D(GetEyePosition(), Scale3DRate * 10.f);
					if (value.ShotID == 0) {
						PlayerMngr->GetPlayer(value.ShotID)->AddScore(100 + ((IsGun0Select() && (value.Damage >= 100)) ? 20 : 0));
						PlayerMngr->GetPlayer(value.ShotID)->AddKill(1);
					}
				}
				this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
				return true;
			}
			return false;
		}
		const bool		CharacterClass::CheckDamageRay(HitPoint* Damage, ArmerPoint* ArmerDamage, bool CheckBodyParts, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			if (!LifeControl::IsAlive()) { return false; }
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, GetCharaPosition()) <= 2.0f * Scale3DRate)) { return false; }

			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			//被弾処理
			auto* HitPtr = HitBoxControl::GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(AttackID)->GetChara();
				//部位ダメージ演算
				if (CheckBodyParts) {
					switch (HitPtr->GetColType()) {
					case HitType::Head:
						if (GetMyPlayerID() == 0) {
							*Damage = *Damage;
							*ArmerDamage = 0;
							m_HeadShotSwitch = true;
						}
						else {
							*Damage = *Damage * 100 / 10;
							*ArmerDamage = 0;
						}
						break;
					case HitType::Body:
						if (LifeControl::IsArmerActive()) {
							*ArmerDamage = std::clamp((ArmerPoint)*Damage, (ArmerPoint)0, LifeControl::GetAP());
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
					*Damage = std::min(*Damage, LifeControl::GetHPMax());
					*ArmerDamage = std::min(*ArmerDamage, LifeControl::GetAPMax());
				}
				//ダメージ登録
				{
					//auto v1 = GetEyeRotation().zvec() * -1.f;
					//auto v2 = (Chara->GetCharaPosition() - this->GetCharaPosition()).normalized(); v2.y = (0);
					//atan2f(Vector3DX::Cross(v1, v2).y, Vector3DX::Dot(v1, v2))
					LifeControl::SetSubHPEvent(AttackID, GetMyPlayerID(), *Damage, *ArmerDamage);
				}
				//SE
				if (AttackID == 0) {
					if (*Damage > 0) {
						SE->Get(SoundType::SE, (int)SoundEnum::Hit)->Play3D(GetEyePosition(), Scale3DRate * 10.f);
					}
					else {
						SE->Get(SoundType::SE, (int)SoundEnum::HitGuard)->Play3D(GetEyePosition(), Scale3DRate * 10.f, 128);
					}
				}
				if (GetMyPlayerID() == 0 && m_IsMainGame) {
					if (*Damage > 0) {
						SE->Get(SoundType::SE, (int)SoundEnum::HitMe)->Play3D(GetEyePosition(), Scale3DRate * 10.f);
					}
					else {
						SE->Get(SoundType::SE, (int)SoundEnum::HitGuard)->Play3D(GetEyePosition(), Scale3DRate * 10.f, 255);
					}
				}
				//エフェクトセット
				if (*Damage > 0) {
					EffectControl::SetOnce((int)(int)Sceneclass::Effect::ef_hitblood, *pEndPos, Vector3DX::forward(), Scale3DRate);
					EffectControl::SetEffectSpeed((int)Sceneclass::Effect::ef_hitblood, 2.f);
				}
				else {
					EffectControl::SetOnce((int)Sceneclass::Effect::ef_gndsmoke, *pEndPos, (Chara->GetCharaPosition() - this->GetCharaPosition()).normalized(), 0.25f * Scale3DRate);
				}
				//ヒットモーション
				{
					HitReactionControl::SetHit(Matrix3x3DX::Vtrans(Vector3DX::Cross((*pEndPos - StartPos).normalized(), Vector3DX::up()) * -1.f, Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper2)).inverse()));
					if (HitPtr->GetColType() == HitType::Leg) {
						KeyControl::SetIsSquat(true);
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
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//auto* OptionParts = OptionManager::Instance();

			LookEnemy = false;
			for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
				if (index == 0) { continue; }
				auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
				LookEnemy |= c->IsAlive() && c->CanLookTarget;
			}
			//
			auto PrevAction = m_CharaAction;
			//
			bool Press_Shot = KeyControl::GetInputControl().GetPADSPress(Controls::PADS::SHOT) && !m_IsChange;
			bool Press_Reload = KeyControl::GetInputControl().GetPADSPress(Controls::PADS::RELOAD) && !m_IsChange;
			if (GetGunPtrNow() && GetGunPtrNow()->GetAmmoNumTotal() == 0 && GetGunPtrNow()->GetGunAnime() == GunAnimeID::Base) {
				Press_Reload |= Press_Shot;				//打ち切りで自動リロード
			}
			bool Press_Aim = KeyControl::GetInputControl().GetPADSPress(Controls::PADS::AIM) && !m_IsChange;
			if (GetGunPtrNow()) {
				switch (GetGunPtrNow()->GetShotType()) {
				case SHOTTYPE::BOLT:
					if (GetGunPtrNow()->GetGunAnime() > GunAnimeID::Shot) {//撃てない状態
						Press_Aim = false;
					}
					break;
				default:
					break;
				}
			}
			//ターゲットが見える場合かまえっぱなし
			if (m_IsMainGame && !m_IsChanging) {
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
					if (!Chara->IsAlive()) { continue; }
					if (!Chara->CanLookTarget) { continue; }
					GunReadyControl::SetAimOrADS();
					break;
				}
			}
			//
			if (PrevAction == m_CharaAction) {
				switch (m_CharaAction) {
				case CharaActionID::Ready:
					if (m_ActionFirstFrame && GetGunPtrNow()) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
					}
					//射撃
					if (Press_Shot) {
						if (IsAimPer()) {
							if (GetGunPtrNow() && (GetGunPtrNow()->GetGunAnime() == GunAnimeID::Base) && GetGunPtrNow()->GetInChamber()) {
								Shot_Start();
							}
						}
						GunReadyControl::SetAimOrADS();
					}
					//リロード/マガジンチェック
					if (Press_Reload && GetGunPtrNow()) {
						bool IsCheck = GetGunPtrNow()->GetIsMagFull();
						if (GetGunPtrNow() && GetGunPtrNow()->GetModData()->GetReloadType() == RELOADTYPE::MAG) {
							IsCheck = (GetGunPtrNow()->GetAmmoNum() >= m_MagStockControl.at(GetNowGunSelect()).GetMag(0));
						}
						if (!IsCheck) {
							m_CharaAction = CharaActionID::Reload;
						}
					}
					//姿勢指定
					if (Press_Aim) {
						if (!m_IsStuckGun) {
							GunReadyControl::SetADS();
						}
						else {
							GunReadyControl::SetAim();
						}
					}
					//
					if (GetGunPtrNow() && GetGunPtrNow()->GetGunAnime() == GunAnimeID::Shot) {
						bool ischeck = true;
						switch (GetGunPtrNow()->GetShotType()) {
						case SHOTTYPE::FULL:
							ischeck = GetGunPtrNow()->GetInChamber() || GetGunPtrNow()->GetIsMagEmpty();
							break;
						case SHOTTYPE::SEMI:
						case SHOTTYPE::BOLT:
							ischeck = !Press_Shot;
							break;
						default:
							break;
						}

						if (GetGunPtrNow()->GetObj().SetAnim((int)GunAnimeID::Shot).TimeEnd() && ischeck) {
							if (!GetGunPtrNow()->GetIsMagEmpty()) {
								switch (GetGunPtrNow()->GetShotType()) {
								case SHOTTYPE::FULL:
								case SHOTTYPE::SEMI:
									GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
									break;
								case SHOTTYPE::BOLT:
									m_CharaAction = CharaActionID::Cocking;
									break;
								default:
									break;
								}
							}
							else {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
							}
						}
					}
					break;
				case CharaActionID::Cocking:
					if (m_ActionFirstFrame && GetGunPtrNow()) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::Cocking);
					}
					//
					if (GetGunPtrNow() && GetGunPtrNow()->GetGunAnime() != GunAnimeID::Cocking) {
						m_CharaAction = CharaActionID::Ready;
					}
					//姿勢指定
					GunReadyControl::SetAim();
					break;
				case CharaActionID::Reload:
					if (m_ActionFirstFrame) {
						Reload_Start();
					}
					else {
						if ((GetGunPtrNow()->GetModData()->GetReloadType() == RELOADTYPE::AMMO) && (GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadOne)) 
						{
							if (KeyControl::GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
								GetGunPtrNow()->SetCancel(true);
							}
						}
						if (KeyControl::GetULTKey()) {
							GetGunPtrNow()->SetCancel(true);
							m_CharaAction = CharaActionID::Ready;
						}
						if (!GetGunPtrNow()->GetReloading()) {
							if (GetGunPtrNow()->GetInChamber()) {
								m_CharaAction = CharaActionID::Ready;
							}
							else {
								if (!GetGunPtrNow()->GetIsMagEmpty()) {
									m_CharaAction = CharaActionID::Cocking;
								}
								else {
									m_CharaAction = CharaActionID::Ready;
								}
							}
						}
					}
					//姿勢指定
					GunReadyControl::SetAim();
					break;
				case CharaActionID::Watch:
					if (m_ActionFirstFrame) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::Watch);
					}
					//
					if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::Watch) {
						m_CharaAction = CharaActionID::Ready;
					}
					//姿勢指定
					GunReadyControl::SetAim();
					break;
				default:
					break;
				}
			}
			//
			m_ActionFirstFrame = (PrevAction != m_CharaAction);

			//グレネード投てき
			auto* SE = SoundPool::Instance();
			{
				if (KeyControl::GetThrowKey().trigger()) {
					SE->Get(SoundType::SE, (int)SoundEnum::Pin)->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
				}
				if (KeyControl::GetThrowKey().release_trigger()) {
					SE->Get(SoundType::SE, (int)SoundEnum::Throw)->Play3D(GetMove().GetPos(), Scale3DRate * 2.f);
					m_Grenade.SetFall(
						GetFrameWorldMat(CharaFrame::RightHandJoint).pos(),
						GetEyeRotation(),
						(GetEyeRotation().zvec() * -1.f).normalized()* (Scale3DRate * 15.f / 60.f), 3.5f, SoundEnum::FallGrenade, true);
				}
			}
			for (auto& g : m_Grenade.m_Ptr) {
				if (g->PopGrenadeBombSwitch()) {
					EffectControl::SetOnce((int)Sceneclass::Effect::ef_greexp, g->GetMove().GetPos(), Vector3DX::forward(), 0.5f * Scale3DRate);
					EffectControl::SetEffectSpeed((int)Sceneclass::Effect::ef_greexp, 2.f);
					//
					SE->Get(SoundType::SE, (int)SoundEnum::Explosion)->Play3D(g->GetMove().GetPos(), Scale3DRate * 25.f);
				}
			}
		}
		//音指示
		void			CharacterClass::ExecuteSound(void) noexcept {
			auto* CameraParts = Camera3D::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			//足音
			if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
				auto Time = this->GetObj().SetAnim((int)this->m_BottomAnimSelect).GetTime();
				//L
				if ((9.f < Time && Time < 10.f)) {
					if (this->m_CharaSound != 1) {
						this->m_CharaSound = 1;
						this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
						if ((CameraParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::LeftFoot).pos()).magnitude() < Scale3DRate * 5.f * 1.5f) {

							SE->Get(SoundType::SE, (int)SoundEnum::RunFoot)->Play3D(GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale3DRate * 5.f);
							if (!GunReadyControl::GetIsADS()) {
								if (GetMyPlayerID() == 0) {
									auto* OptionParts = OptionManager::Instance();
									bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
									if (HeadBobbing) {
										//Camera3D::Instance()->SetCamShake(0.1f, 1.f);
									}
								}
							}
						}
					}
				}
				//R
				if ((27.f < Time && Time < 28.f)) {
					if (this->m_CharaSound != 3) {
						this->m_CharaSound = 3;
						this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
						if ((CameraParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::RightFoot).pos()).magnitude() < Scale3DRate * 5.f * 1.5f) {

							SE->Get(SoundType::SE, (int)SoundEnum::RunFoot)->Play3D(GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale3DRate * 5.f);
							if (!GunReadyControl::GetIsADS()) {
								if (GetMyPlayerID() == 0) {
									auto* OptionParts = OptionManager::Instance();
									bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
									if (HeadBobbing) {
										//Camera3D::Instance()->SetCamShake(0.1f, 1.f);
									}
								}
							}
						}
					}
				}
			}
			//しゃがみ音
			if (KeyControl::GetSquatSwitch()) {
				SE->Get(SoundType::SE, (int)SoundEnum::StandupFoot)->Play3D(GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//リーン音
			if (KeyControl::GetLeanSwitch()) {
				SE->Get(SoundType::SE, (int)SoundEnum::StandupFoot)->Play3D(GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//心拍音
			if (GetMyPlayerID() == 0) {
				if (StaminaControl::ExcuteStamina(0.f, this->GetMove().GetVec().magnitude() * DXLib_refParts->GetDeltaTime(), KeyControl::GetIsSquat())) {
					SE->Get(SoundType::SE, (int)SoundEnum::Heart)->Play3D(GetEyePosition(), Scale3DRate * 0.5f);
				}
			}
		}
		//SetMat指示更新
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			//vector
			Vector3DX PosBuf = this->GetMove().GetPos();
			bool IsHitGround = (PosBuf.y <= 0.f); //高度0なら止まる
			float groundYpos = 0.f;
			Vector3DX EndPos = PosBuf - Vector3DX::up() * (0.5f * Scale3DRate);
			IsHitGround = BackGround->CheckLinetoMap(PosBuf + Vector3DX::up() * (0.f * Scale3DRate), &EndPos);
			groundYpos = EndPos.y - (0.12f * Scale3DRate);
			if (IsHitGround) {
				auto yPos = PosBuf.y;
				Easing(&yPos, groundYpos, 0.6f, EasingType::OutExpo);
				PosBuf.y = (yPos);

				Vector3DX vec = KeyControl::GetVec(); vec.y = 0.f;
				this->SetMove().SetVec(vec);
				this->SetMove().Update(0.5f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
			else {
				float Y = this->GetMove().GetVec().y;
				Vector3DX vec = KeyControl::GetVec(); vec.y = (Y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps())));
				this->SetMove().SetVec(vec);
				this->SetMove().Update(0.5f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
			PosBuf += this->GetMove().GetVec();
			//壁判定
			BackGround->CheckMapWall(this->m_move.GetRePos(), &PosBuf, Vector3DX::up() * (0.6f * Scale3DRate + 0.1f), Vector3DX::up() * (1.6f * Scale3DRate), 0.6f * Scale3DRate);
			//ほかプレイヤーとの判定
			if (m_IsMainGame) {
				float Radius = 2.f * 0.5f * Scale3DRate;
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
					if (i == GetMyPlayerID()) { continue; }
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
					if (!Chara->IsAlive()) { continue; }
					//自分が当たったら押し戻す
					Vector3DX Vec = (Chara->GetCharaPosition() - this->GetCharaPosition()); Vec.y = (0.f);
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
			this->SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetYRadBottom()));
			this->SetMove().Update(0.9f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			//スリング場所探索
			int GunSel = 1;
			if (GetGunPtr(GunSel) && this->m_SlingPer[GunSel] < 1.f) {
				m_SlingMat[GunSel] =
					(
						Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-90)) *
						Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper)) *
						GetCharaDir().inverse()
						).Get44DX() *
					Matrix4x4DX::Mtrans(
						GetFrameWorldMat(CharaFrame::Upper).pos() +
						GetFrameWorldMat(CharaFrame::Upper).yvec() * (-0.5f * Scale3DRate) +
						GetFrameWorldMat(CharaFrame::Upper).xvec() * (-0.3f * Scale3DRate)
					);
			}
			GunSel = 1 - GunSel;
			if (GetGunPtr(GunSel) && this->m_SlingPer[GunSel] < 1.f) {
				m_SlingZrad.Update(DXLib_refParts->GetDeltaTime());
				m_SlingZrad.AddRad(((1.f - m_SlingPer.at(GetOtherGunSelect())) + 0.5f * (KeyControl::GetRad().y - KeyControl::GetYRadBottom())) * DXLib_refParts->GetDeltaTime());
				m_SlingMat[GunSel] =
					(
						Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
						(
							Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingZrad.GetRad()) *
							Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetRad().y) *
							GetCharaDir().inverse()
						)
					).Get44DX() *
					Matrix4x4DX::Mtrans(
						GetFrameWorldMat(CharaFrame::Head).pos() +
						GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.3f * Scale3DRate) +
						(
							this->GetMove().GetMat().xvec() * sin(m_SlingZrad.GetRad()) +
							this->GetMove().GetMat().yvec() * cos(m_SlingZrad.GetRad())
							) * -(0.5f * Scale3DRate)
					);
			}
			if (GetGunPtrNow()) {
				//銃ひっこめ
				{
					if (m_StuckGunTimer == 0.f) {
						m_StuckGunTimer = 0.1f;
						auto EndPost = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Muzzle).pos();
						if (BackGround->CheckLinetoMap(GetEyePosition(), &EndPost)) {//0.03ms
							m_IsStuckGun = true;
						}
						else {
							if (m_IsStuckGun) {
								GunReadyControl::SetAim();
							}
							m_IsStuckGun = false;
						}
					}
					else {
						m_StuckGunTimer = std::max(m_StuckGunTimer - DXLib_refParts->GetDeltaTime(), 0.f);
					}
					if (m_IsStuckGun) {
						GunReadyControl::SetReady();
					}
				}
				//オートエイム
				AutoAimControl::UpdateAutoAim(GetGunPtrNow()->IsAutoAimActive());
				//
				if (LifeControl::IsAlive()) {
					//銃の位置を指定するアニメ
					Matrix4x4DX AnimMat;
					for (int i = 0; i < (int)EnumGunAnimType::Max; i++) {
						Matrix4x4DX AnimData;
						if (!GetGunPtrNow()->GetGunAnimeNow((EnumGunAnimType)i, &AnimData)) { continue; }
						AnimMat = Lerp(AnimMat, AnimData, this->m_Arm[i].Per());
					}
					{
						auto Pos = AnimMat.pos();

						Easing(&m_LeanSwitch, (GetLeanRate() <= 0 || GetGunPtrNow()->GetReloading()) ? 1.f : -1.f, 0.9f, EasingType::OutExpo);
						Pos.x *= m_LeanSwitch;
						AnimMat = AnimMat.rotation() * Matrix4x4DX::Mtrans(Pos);
					}
					Vector3DX Post0 = GetFrameWorldMat(CharaFrame::Head).pos() + Matrix3x3DX::Vtrans(AnimMat.pos(), GetCharaDir());
					//
					this->m_ULTBar.Execute(IsGun0Select(), 0.1f, 0.1f, 0.7f);
					//
					Easing(&m_AutoAimPer, (GunReadyControl::GetIsADS() && GetAutoAimActive()) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

					for (int index = 0; index < 2; index++) {
						auto& p = GetGunPtr(index);
						if (p) {
							bool IsSelGun = (index == GetNowGunSelect());



							auto& Mat = this->m_SlingMat[index];
							auto& Per = this->m_SlingPer[index];
							Easing(&Per, IsSelGun ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
							if (Per <= 0.001f) { Per = 0.f; }
							if (Per >= 0.999f) { Per = 1.f; }

							Matrix3x3DX tmp_gunmat = Matrix3x3DX::RotVec2(Vector3DX::forward(), Lerp(Mat.zvec(), AnimMat.zvec(), Per));
							tmp_gunmat *= Matrix3x3DX::RotVec2(tmp_gunmat.yvec(), Lerp(Mat.yvec(), AnimMat.yvec(), Per));

							float PAdd = 0.f;
							if (!IsSelGun) {
								PAdd = -1.f * Scale3DRate * Per;
							}

							tmp_gunmat *=
								KeyControl::GetGunSwingMat() * GetCharaDir() *
								Matrix3x3DX::RotVec2(Lerp(p->GetObj().GetMatrix().yvec(), p->GetEyeYVec(), GunReadyControl::GetADSPer()), p->GetObj().GetMatrix().yvec());



							Vector3DX BasePos = GetEyePosition();
							//オートエイム
							if (IsSelGun && GunReadyControl::GetIsADS() && p->IsAutoAimActive() && m_IsMainGame) {
								//float Range = 13.f;// GetIsLaserActive() ? 13.f : 7.f;

								float Len = std::max(0.01f, std::hypotf((float)(p->GetAimXPos() - 1920 / 2), (float)(p->GetAimYPos() - 1080 / 2)));
								Len = std::clamp(100.f / Len, 0.f, 1.f);

								for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
									if (i == GetMyPlayerID()) { continue; }
									auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
									if (!Chara->IsAlive()) { continue; }
									if (!Chara->CanLookTarget) { continue; }
									int pos = -1;
									float CosMax = -1.f;
									for (const auto& h : Chara->GetHitBoxPointList()) {
										auto EndPost = h.GetPos();
										auto ColResGround = BackGround->CheckLinetoMap(BasePos, &EndPost);
										if (ColResGround) { continue; }
										Vector3DX Vec = (h.GetPos() - BasePos);
										//if (Vec.magnitude() >= (Scale3DRate * Range)) { continue; }
										switch (h.GetColType())										{
										case FPS_n2::Sceneclass::HitType::Head:
										case FPS_n2::Sceneclass::HitType::Body:
											break;
										default:
											continue;
											break;
										}
										float Cos = Vector3DX::Dot((p->GetFrameWorldMat_P(GunFrame::Muzzle).zvec() * -1.f), Vec.normalized());
										if (Cos > cos(deg2rad(5) * Len)) {
											if (CosMax < Cos) {
												CosMax = Cos;
												pos = (int)(&h - &Chara->GetHitBoxPointList().front());
											}
										}
									}
									if (pos != -1) {
										AutoAimControl::SetAutoAimActive(i, pos);
										break;
									}
								}
							}

							if (IsSelGun) {
								if (GetAutoAimActive()) {
									auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(AutoAimControl::GetAutoAimID())->GetChara();
									Easing(&m_AutoAimVec, (Chara->GetHitBoxPointList().at(AutoAimControl::GetAutoAimPoint()).GetPos() - BasePos).normalized(), 0.8f, EasingType::OutExpo);
								}
								else {
									Easing(&m_AutoAimVec, tmp_gunmat.zvec() * -1.f, 0.95f, EasingType::OutExpo);
								}
								tmp_gunmat = Lerp(tmp_gunmat, tmp_gunmat * Matrix3x3DX::RotVec2(tmp_gunmat.zvec() * -1.f, m_AutoAimVec), m_AutoAimPer);
							}

							p->SetGunMatrix(tmp_gunmat, Lerp(Mat.pos() + Vector3DX::up() * PAdd, Post0, Per));
						}
					}
				}
				else {
					auto Mat = lagframe_.RIGHThand_f.GetFrameWorldPosition().rotation().inverse() * RagDollControl::GetRagDoll().GetFrameLocalWorldMatrix(lagframe_.RIGHThand_f.GetFrameID());
					GetGunPtrNow()->SetGunMatrix(Matrix3x3DX::Get33DX(Mat), Mat.pos());
					if (GetGunPtrOther()) {
						//仮の画面外指定
						Mat = Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(90)).Get44DX() * GetFrameWorldMat(CharaFrame::Upper);
						GetGunPtrOther()->SetGunMatrix(Matrix3x3DX::Get33DX(Mat), Mat.pos());
					}
				}
				//LeftMag
				{
					auto tmp_gunmat = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos);
					float AnimPer = GetGunPtrNow()->GetObj().SetAnim((int)GetGunPtrNow()->GetGunAnime()).GetTimePer();
					float BasePer = 0.f;
					float MaxPer = 0.f;
					switch (GetGunPtrNow()->GetGunAnime()) {
					case GunAnimeID::ReloadStart_Empty:
						tmp_gunmat = GetFrameWorldMat(CharaFrame::LeftMag);
						break;
					case GunAnimeID::ReloadStart:
						BasePer = 0.3f;
						MaxPer = 0.6f;
						if (AnimPer <= MaxPer) {
							float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
							tmp_gunmat = Lerp(GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos), GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), Per);
							break;
						}

						BasePer = 0.5f;
						MaxPer = 1.f;
						if (AnimPer <= MaxPer) {
							float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
							tmp_gunmat = Lerp(GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), GetFrameWorldMat(CharaFrame::LeftMag), Per);
							break;
						}
						break;
					case GunAnimeID::ReloadOne:
					{
						switch (GetGunPtrNow()->GetModData()->GetReloadType()) {
						case RELOADTYPE::MAG:
						{
							if (AnimPer <= 0.1f) {
								float MissPer = GetRandf(0.025f);
								GetGunPtrNow()->SetMagLoadSuccess(GetRand(100) < 50);
								m_MagMiss = Matrix4x4DX::Mtrans(GetGunPtrNow()->GetMove().GetMat().xvec() * (MissPer * Scale3DRate));
								m_MagSuccess = Matrix4x4DX::Mtrans(
									GetGunPtrNow()->GetMove().GetMat().yvec() * (-0.05f * Scale3DRate) +
									GetGunPtrNow()->GetMove().GetMat().xvec() * (MissPer / 3.f * Scale3DRate)
								);
							}
							BasePer = 0.f;
							MaxPer = 0.55f;
							if (AnimPer <= MaxPer) {
								//float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
								float Per = this->m_Arm[(int)EnumGunAnimType::Reload].Per();
								if (GetGunPtrNow()->IsMagLoadSuccess()) {
									tmp_gunmat = Lerp(GetFrameWorldMat(CharaFrame::LeftMag), GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), Per);
								}
								else {
									tmp_gunmat = Lerp(GetFrameWorldMat(CharaFrame::LeftMag), GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2) * m_MagMiss, Per);
								}
								break;
							}
							if (GetGunPtrNow()->IsMagLoadSuccess()) {
								BasePer = MaxPer;
								MaxPer = 0.75f;
								if (AnimPer <= MaxPer) {
									float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
									tmp_gunmat = Lerp(GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos), Per);
									break;
								}
							}
							else {
								BasePer = MaxPer;
								MaxPer = 0.75f;
								if (AnimPer <= MaxPer) {
									float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
									tmp_gunmat = Lerp(GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2) * m_MagMiss, GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2) * m_MagSuccess, Per);
									break;
								}
								BasePer = MaxPer;
								MaxPer = 0.85f;
								if (AnimPer <= MaxPer) {
									float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
									tmp_gunmat = Lerp(GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2) * m_MagSuccess, GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), Per);
									break;
								}
								BasePer = MaxPer;
								MaxPer = 1.0f;
								if (AnimPer <= MaxPer) {
									float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
									tmp_gunmat = Lerp(GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos), Per);
									break;
								}
							}
						}
							break;
						case RELOADTYPE::AMMO:
						{
							GetGunPtrNow()->SetMagLoadSuccess(false);
							BasePer = 0.f;
							MaxPer = 0.5f;
							if (AnimPer <= MaxPer) {
								float Per = this->m_Arm[(int)EnumGunAnimType::Reload].Per();
								tmp_gunmat = Lerp(GetFrameWorldMat(CharaFrame::LeftMag), GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), Per);
								break;
							}
							BasePer = 0.7f;
							MaxPer = 0.9f;
							if (AnimPer <= MaxPer) {
								float Per = std::clamp((AnimPer - BasePer) / (MaxPer - BasePer), 0.f, 1.f);
								tmp_gunmat = Lerp(GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Mag2), GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos), Per);
								break;
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
					(*GetGunPtrNow()->GetMagazinePtr())->SetHandMatrix(tmp_gunmat);
					if (GetGunPtrOther()) {
						(*GetGunPtrOther()->GetMagazinePtr())->SetHandMatrix(GetGunPtrOther()->GetFrameWorldMat_P(GunFrame::Magpos));
					}
				}
				//銃表示
				if (GetGunPtrOther()) {
					GetGunPtrOther()->SetActiveAll(true);
				}
				//手の位置を制御
				if ((GetMyPlayerID() == 0) || this->CanLookTarget) {
					//銃座標
					Vector3DX GunPos = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::RightHandPos).pos();
					Vector3DX Gunyvec = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::RightHandYvec).pos() - GunPos;
					Vector3DX Gunzvec = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::RightHandZvec).pos() - GunPos;
					IK_RightArm(
						&GetObj(),
						GetFrame(static_cast<int>(CharaFrame::RightArm)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
						GetFrame(static_cast<int>(CharaFrame::RightArm2)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
						GetFrame(static_cast<int>(CharaFrame::RightWrist)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
						GunPos, Gunyvec, Gunzvec);

					{
						bool IsBreak = false;
						switch (GetGunPtrNow()->GetModData()->GetReloadType()) {
						case RELOADTYPE::MAG:
							switch (GetGunPtrNow()->GetGunAnime()) {
							case GunAnimeID::ReloadStart_Empty:
								if (GetGunPtrNow()->GetObj().SetAnim((int)GetGunPtrNow()->GetGunAnime()).GetTimePer() > 0.5f) {
									if (!m_MagHand) {
										GetGunPtrNow()->SetMagFall();
									}
									m_MagHand = true;
								}
								else {
									m_MagHand = false;
								}
								break;
							case GunAnimeID::ReloadStart:
								m_MagHand = true;
								break;
							case GunAnimeID::ReloadOne:
								m_MagHand = true;
								break;
							case GunAnimeID::ReloadEnd:
								if (GetGunPtrNow()->GetObj().SetAnim((int)GetGunPtrNow()->GetGunAnime()).GetTimePer() > 0.6f) {
									m_MagHand = false;
								}
								break;
							case GunAnimeID::Base:
							case GunAnimeID::Shot:
								m_MagHand = false;
								if (m_ArmBreak) {
									IsBreak = true;
								}
								break;
							default:
								m_MagHand = false;
								break;
							}
							break;
						case RELOADTYPE::AMMO:
							switch (GetGunPtrNow()->GetGunAnime()) {
							case GunAnimeID::ReloadStart_Empty:
								m_MagHand = true;
								break;
							case GunAnimeID::ReloadStart:
								m_MagHand = true;
								break;
							case GunAnimeID::ReloadOne:
								m_MagHand = true;
								break;
							case GunAnimeID::ReloadEnd:
								m_MagHand = false;
								break;
							case GunAnimeID::Base:
							case GunAnimeID::Shot:
								m_MagHand = false;
								if (m_ArmBreak) {
									IsBreak = true;
								}
								break;
							default:
								m_MagHand = false;
								break;
							}
							break;
						default:
							break;
						}
						m_MagArm.Execute(m_MagHand, 0.1f, 0.1f,0.7f);

						Vector3DX HandPos = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::LeftHandPos).pos();
						Vector3DX Handyvec = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::LeftHandYvec).pos() - HandPos;
						Vector3DX Handzvec = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::LeftHandZvec).pos() - HandPos;

						Vector3DX MagPos = (*GetGunPtrNow()->GetMagazinePtr())->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
						Vector3DX Magyvec = (*GetGunPtrNow()->GetMagazinePtr())->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - MagPos;
						Vector3DX Magzvec = (*GetGunPtrNow()->GetMagazinePtr())->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - MagPos;
						/*
						Vector3DX MagPos = GetFrameWorldMat(CharaFrame::LeftMag).pos();
						Vector3DX Magyvec = GetFrameWorldMat(CharaFrame::LeftMagyvec).pos() - MagPos;
						Vector3DX Magzvec = GetFrameWorldMat(CharaFrame::LeftMagzvec).pos() - MagPos;
						//*/
						Vector3DX HandsPos = Lerp(HandPos, MagPos, m_MagArm.Per());
						Vector3DX Handsyvec = Lerp(Handyvec, Magyvec, m_MagArm.Per());
						Vector3DX Handszvec = Lerp(Handzvec, Magzvec, m_MagArm.Per());

						Easing(&m_ArmBreakPer, IsBreak ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
						if(m_ArmBreakPer>0.01f){
							m_SlingArmZrad.Update(DXLib_refParts->GetDeltaTime());
							m_SlingArmZrad.AddRad((0.5f * (KeyControl::GetRad().y - KeyControl::GetYRadBottom()))* DXLib_refParts->GetDeltaTime());
							Matrix3x3DX SlingArmMat =
								(
									Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
									(
										Matrix3x3DX::RotAxis(Vector3DX::forward(), -this->m_SlingArmZrad.GetRad()) *
										Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetRad().y)
										)
								);
							Vector3DX SlingArmPos =
								GetFrameWorldMat(CharaFrame::Head).pos() +
								GetFrameWorldMat(CharaFrame::Head).xvec() * (0.15f * Scale3DRate) +
								GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.1f * Scale3DRate) +
								(
									this->GetMove().GetMat().xvec() * sin(m_SlingArmZrad.GetRad()) +
									this->GetMove().GetMat().yvec() * cos(m_SlingArmZrad.GetRad())
									) * -(0.5f * Scale3DRate)
								;

							HandPos = SlingArmPos;
							Handyvec = SlingArmMat.yvec();
							Handzvec = SlingArmMat.zvec() * -1.f;

							HandsPos = Lerp(HandsPos, HandPos, m_ArmBreakPer);
							Handsyvec = Lerp(Handsyvec, Handyvec, m_ArmBreakPer);
							Handszvec = Lerp(Handszvec, Handzvec, m_ArmBreakPer);
						}
						if (m_ArmBreak) {
							HandsPos += Vector3DX::vget(GetRandf(1.f * Scale3DRate), GetRandf(1.f * Scale3DRate), GetRandf(1.f * Scale3DRate)) * 0.002f;
						}

						IK_LeftArm(
							&GetObj(),
							GetFrame(static_cast<int>(CharaFrame::LeftArm)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)),
							GetFrame(static_cast<int>(CharaFrame::LeftArm2)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)),
							GetFrame(static_cast<int>(CharaFrame::LeftWrist)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)),
							HandsPos, Handsyvec, Handszvec);
					}
				}
			}
		}
		//
		void			CharacterClass::ValueSet(PlayerID pID, bool IsMainGame, CharaTypeID value) noexcept {
			this->m_IsMainGame = IsMainGame;
			this->m_CharaType = value;
			if (GetGunPtr(0)) {
				GetGunPtr(0)->SetFallObject();
				if (GetGunPtr(1)) {
					GetGunPtr(1)->SetFallObject();
				}
			}
			StaminaControl::InitStamina();
			LifeControl::InitLife();
			StackLeftHandControl::InitStackLeftHand();
			//
			SetPlayerID(pID);
			//
			for (auto& a : this->m_Arm) { a.Init(false); }
			this->m_Arm[(int)EnumGunAnimType::Aim].Init(true);
			//
			this->m_MagArm.Init(false);
			this->m_MagHand = false;
			EyeSwingControl::InitEyeSwing();
			this->m_IsChanging = false;
			this->m_IsChange = false;
			//
			//サウンド
			this->m_CharaSound = -1;
			//動作にかかわる操作
			this->m_MoveOverRideFlag = false;
			//銃のIDセットアップ
			if (GetGunPtr(0)) {
				GetGunPtr(0)->SetPlayerID(GetMyPlayerID());
			}
			if (GetGunPtr(1)) {
				GetGunPtr(1)->SetPlayerID(GetMyPlayerID());
			}
			m_SlingZrad.Init(0.05f * Scale3DRate, 3.f, deg2rad(50));
			m_SlingArmZrad.Init(0.08f * Scale3DRate, 3.f, deg2rad(50));
			m_ArmBreakPer = 0.f;
			m_HPRec = 0.f;
			m_CharaAction = CharaActionID::Ready;
			m_ActionFirstFrame = true;
		}
		void			CharacterClass::MovePoint(float pxRad, float pyRad, const Vector3DX& pPos, int GunSel) noexcept {
			KeyControl::InitKey(pxRad, pyRad);
			SetMove().SetAll(pPos, pPos, pPos, Vector3DX::zero(), Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetRad().y), Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetRad().y));
			for (int loop = 0; loop < 2; ++loop) {
				if (GetGunPtr(loop)) {
					m_MagStockControl.at(loop).Init_MagStockControl(GetGunPtr(loop)->GetAmmoNum(), GetGunPtr(loop)->GetAmmoAll(), GetGunPtr(loop)->GetMagUniqueID());
				}
			}
			SelectGun(GunSel);
			this->m_ULTBar.Init(IsGun0Select());
			this->m_SlingPer[0] = (IsGun0Select()) ? 0.f : 1.f;
			this->m_SlingPer[1] = 1.f - this->m_SlingPer[0];
			GunReadyControl::SetReady();
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			KeyControl::InputKey(pInput, pReady, StaminaControl::GetHeartRandVec(KeyControl::GetIsSquat() ? 1.f : 0.f) * 0.f);
			//AIM
			if (GetGunPtrNow()) {
				if (pReady) {
					if (!m_IsChanging) {
						if (!m_ULTActive) {
							if (m_ULTUp > 0.5f) {
								m_ULTUp -= 0.5f;
							}
							m_ULTUp += DXLib_refParts->GetDeltaTime();
							if (KeyControl::GetULTKey()) {
								m_IsChanging = true;
								m_IsChange = true;
								m_ULTActive = true;
								m_ULTUp = 0.f;
							}
						}
						else {
							if (m_ULTUp > 0.25f) {
								m_ULTUp -= 0.25f;
							}
							m_ULTUp += DXLib_refParts->GetDeltaTime();
							if (GetIsAim() && KeyControl::GetULTKey()) {
								m_IsChanging = true;
								m_IsChange = true;
								m_ULTActive = false;
								m_ULTUp = 0.f;
							}
						}
					}
					if (IsAlive()) {
						if (IsLowHP()) {
							if (m_HPRec >= 0.f) {
								m_HPRec -= 2.f;
								Heal(2, false);
								if (GetMyPlayerID() == 0) {
									SE->Get(SoundType::SE, (int)SoundEnum::Man_breathing)->Play(DX_PLAYTYPE_BACK);
								}
							}
							m_HPRec += DXLib_refParts->GetDeltaTime();
						}
						else {
							if (m_HPRec != 0.f) {
								if (GetMyPlayerID() == 0) {
									SE->Get(SoundType::SE, (int)SoundEnum::Man_breathing)->StopAll();
									SE->Get(SoundType::SE, (int)SoundEnum::Man_breathend)->Play(DX_PLAYTYPE_BACK);
								}
							}
							m_HPRec = 0.f;
						}
					}
				}
				if (m_IsChanging) {
					GunReadyControl::SetReady();
					if (IsLowReadyPer()) {
						GunReadyControl::SetAim();
						if (m_ULTActive) {
							SelectGun(1);
						}
						else {
							SelectGun(0);
						}
						m_IsChanging = false;
					}
				}
				else if (this->m_SlingPer[GetNowGunSelect()] > 0.9f) {
					m_IsChange = false;
				}
			}
		}
		//
		void			CharacterClass::Init_Sub(void) noexcept {
			HitBoxControl::InitHitBox();
			m_ArmBreak = false;
			EffectControl::Init();
			m_Grenade.Dispose();
			m_Grenade.Init("data/model/RGD5/", 4);	//装填したマガジンの弾に合わせて薬莢生成
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
			}
			int num = GetObj().GetMaterialNum();
			for (int i = 0; i < num; i++) {
				GetObj().SetMaterialDifColor(i, GetColorF(0.8f, 0.8f, 0.8f, 1.f));
				GetObj().SetMaterialAmbColor(i, GetColorF(0.5f, 0.5f, 0.5f, 1.f));
			}
			this->m_SoundPower = std::max(this->m_SoundPower - DXLib_refParts->GetDeltaTime(), 0.f);
			GunReadyControl::UpdateReady();
			m_MeleeCoolDown = std::max(m_MeleeCoolDown - DXLib_refParts->GetDeltaTime(), 0.f);
			if (GetGunPtrNow()) {
				GetGunPtrNow()->SetShotSwitchOff();
				//リコイルの演算
				if (this->m_RecoilRadAdd.y < 0.f) {
					Easing(&this->m_RecoilRadAdd, Vector3DX::vget(0.f, 0.01f, 0.f), GetGunPtrNow()->GetRecoilReturn(), EasingType::OutExpo);
				}
				else {
					Easing(&this->m_RecoilRadAdd, Vector3DX::zero(), 0.7f, EasingType::OutExpo);
				}
			}
			//
			ExecuteInput();
			if (GetGunPtrNow()) {
				m_SightChange.Update(GetIsADS() && KeyControl::GetInputControl().GetPADSPress(Controls::PADS::JUMP));
				if (m_SightChange.trigger()) {
					GetGunPtrNow()->ChangeSightSel();
				}
			}
			//
			this->m_Arm[(int)EnumGunAnimType::ADS].Execute(GunReadyControl::GetIsADS(), 0.2f, 0.2f, 0.9f);
			if (GetGunPtrNow()) {
				this->m_Arm[(int)EnumGunAnimType::ReloadStart_Empty].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadStart_Empty, 0.5f, 0.2f);
				this->m_Arm[(int)EnumGunAnimType::ReloadStart].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadStart, 0.2f, 0.2f);
				{
					bool IsReload = false;
					switch (GetGunPtrNow()->GetModData()->GetReloadType()) {
					case RELOADTYPE::MAG:
					{
						IsReload = (GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadOne);
						this->m_Arm[(int)EnumGunAnimType::Reload].Execute(IsReload, 0.1f, 0.2f, 0.8f);
					}
					break;
					case RELOADTYPE::AMMO:
					{
						IsReload = (GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadOne) && (GetGunPtrNow()->GetObj().SetAnim((int)GetGunPtrNow()->GetGunAnime()).GetTimePer() < 0.5f);
						this->m_Arm[(int)EnumGunAnimType::Reload].Execute(IsReload, 0.1f, 0.2f, 0.9f);
					}
					break;
					default:
						break;
					}
				}
				this->m_Arm[(int)EnumGunAnimType::ReloadEnd].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadEnd, 0.1f, 0.2f, 0.9f);
				this->m_Arm[(int)EnumGunAnimType::Ready].Execute(!GetIsAim(), 0.1f, 0.2f, 0.87f);
				this->m_Arm[(int)EnumGunAnimType::Watch].Execute(GetCharaAction() == CharaActionID::Watch, 0.1f, 0.1f);
			}
			//
			KeyControl::UpdateKeyRad();
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::Upper));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::Upper),
				(Matrix3x3DX::Get33DX(GetFrameLocalMat(CharaFrame::Upper)) * Matrix3x3DX::RotAxis(Vector3DX::right(), -KeyControl::GetRad().x / 2.f) * (GetCharaDir() * this->GetMove().GetMat().inverse())).Get44DX()
				* GetFrameBaseLocalMat((int)CharaFrame::Upper));
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::Upper2));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::Upper2),
				(Matrix3x3DX::Get33DX(GetFrameLocalMat(CharaFrame::Upper2)) * Matrix3x3DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x / 2.f) * HitReactionControl::GetHitReactionMat()).Get44DX()
				* GetFrameBaseLocalMat((int)CharaFrame::Upper2));
			HitReactionControl::Execute_HitReactionControl();
			//上半身演算
			if (GetGunPtrNow()) {
				GetGunPtrNow()->UpdateGunAnims();
			}
			//上半身アニメ演算
			GetCharaAnimeBufID(CharaAnimeID::Upper_Ready) = 1.f;
			GetCharaAnimeBufID(CharaAnimeID::Hand_Ready) = 1.f;
			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomTurnAnimSel(), 0.5f);
			ObjectBaseClass::SetAnimLoop((int)CharaAnimeID::Bottom_Stand_Run, KeyControl::GetSpeedPer()/2.f);
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomWalkAnimSel(), KeyControl::GetVecFront());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomLeftStepAnimSel(), KeyControl::GetVecLeft());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomWalkBackAnimSel(), KeyControl::GetVecRear());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomRightStepAnimSel(), KeyControl::GetVecRight());
			//アニメ反映
			for (int i = 0; i < GetObj().GetAnimNum(); i++) {
				this->GetObj().SetAnim(i).SetPer(GetCharaAnimeBufID((CharaAnimeID)i));
			}
			GetObj().UpdateAnimAll();
			//移動の際の視点制御
			EyeSwingControl::UpdateEyeSwing(GetCharaDir(), std::clamp(KeyControl::GetVec().magnitude() / 0.65f, 0.f, 0.85f / 0.65f), 5.f);
			//
			ExecuteSound();
			//
			ExecuteMatrix();
			//
			RagDollControl::Execute_RagDollControl(this->GetObj(), LifeControl::IsAlive());													//ラグドール
			HitBoxControl::UpdataHitBox(this, (this->GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);									//ヒットボックス
			WalkSwingControl::UpdateWalkSwing(GetEyePosition() - GetCharaPosition(), std::clamp(this->GetMove().GetVec().magnitude() / 2.f, 0.f, 0.5f));
			EffectControl::Execute();
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
					if (LifeControl::IsAlive()) {
						for (int i = 0; i < this->GetObj().GetMeshNum(); i++) {
							if (this->GetObj().GetMeshSemiTransState(i) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
					}
					else {
						for (int i = 0; i < RagDollControl::GetRagDoll().GetMeshNum(); i++) {
							if (RagDollControl::GetRagDoll().GetMeshSemiTransState(i) == isDrawSemiTrans) {
								RagDollControl::GetRagDoll().DrawMesh(i);
							}
						}
					}
					//hitbox描画
					//HitBoxControl::DrawHitBox();
					//
					SetFogEnable(fog_enable);
					SetFogMode(fog_mode);
					SetFogColor(fog_r, fog_g, fog_b);
					SetFogStartEnd(fog_start, fog_end);
					SetFogDensity(fog_density);
				}
			}
		}
		void			CharacterClass::DrawShadow(void) noexcept {
			if (this->m_IsActive) {
				if (LifeControl::IsAlive()) {
					this->GetObj().DrawModel();
				}
				else {
					RagDollControl::GetRagDoll().DrawModel();
				}
			}
		}
		void			CharacterClass::Dispose_Sub(void) noexcept {
			EffectControl::Dispose();
			GunPtrControl::DisposeGunPtr();
		}
	};
};
