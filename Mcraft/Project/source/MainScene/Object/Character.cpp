#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const Matrix4x4DX CharacterClass::GetCharaDir(void) const noexcept {
			auto tmpUpperMatrix = Matrix4x4DX::RotAxis(Vector3DX::forward(), KeyControl::GetLeanRad());

			tmpUpperMatrix *=
				Matrix4x4DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x) *
				Matrix4x4DX::RotAxis(Vector3DX::up(), Lerp(KeyControl::GetYRadBottomChange(), 0.f, this->m_Arm[(int)EnumGunAnimType::Run].Per()));
			return tmpUpperMatrix * this->GetMove().GetMat().Get44DX();
		}
		//
		void			CharacterClass::Shot_Start() noexcept {
			if (this->m_MyID == 0) {
				auto* PlayerMngr = Player::PlayerManager::Instance();
				PlayerMngr->GetPlayer(this->m_MyID)->AddShot(GetGunPtrNow()->GetPelletNum());
			}
			GetGunPtrNow()->SetBullet();
			if (IsULTSelect()) {
				ULTControl::AddULT(-3, true);
			}
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
			if (this->m_MyID == 0) {
				CameraShake::Instance()->SetCamShake(0.1f, 0.3f);
			}
			//エフェクト
			auto mat = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Muzzle);
			switch (GetGunPtrNow()->GetGunShootSound()) {
			case GunShootSound::Normal:
				EffectControl::SetOnce(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec() * -1.f, 0.5f);
				EffectControl::SetEffectSpeed(EffectResource::Effect::ef_fire2, 2.f);
				break;
			case GunShootSound::Suppressor:
				EffectControl::SetOnce(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec() * -1.f, 0.25f);
				EffectControl::SetEffectSpeed(EffectResource::Effect::ef_fire2, 2.f);
				break;
			default:
				break;
			}
			//音
			this->m_SoundPower = 1.f * (CanLookTarget ? 1.f : 0.5f);
		}
		void			CharacterClass::Reload_Start() noexcept {
			auto* SE = SoundPool::Instance();
			if (IsGun0Select()) {
				int Num = GetGunPtrNow()->GetAmmoNum();
				int All = GetGunPtrNow()->GetAmmoAll();
				int UniqueID = GetGunPtrNow()->GetMagUniqueID();
				GetGunPtrNow()->SetReloadStart(MagStockControl::GetNowMag().AmmoNum, MagStockControl::GetNowMag().ModUniqueID);
				MagStockControl::SetOldMag(Num, All, UniqueID);
				//todo::Ammoの通常武器状態
			}
			else {
				GetGunPtrNow()->SetReloadStart(GetGunPtrNow()->GetAmmoAll(), GetGunPtrNow()->GetMagUniqueID());
			}
			if (this->m_MyID != 0) {
				if (GetRand(1) == 0) {
					SE->Get((int)SoundEnum::Man_reload).Play_3D(0, GetEyeMatrix().pos(), Scale3DRate * 10.f);
				}
				else {
					SE->Get((int)SoundEnum::Man_takecover).Play_3D(0, GetEyeMatrix().pos(), Scale3DRate * 10.f);
				}
				this->m_SoundPower = 0.6f * (CanLookTarget ? 1.f : 0.5f);
			}
		}
		//
		const Matrix4x4DX CharacterClass::GetEyeMatrix(void) const noexcept {
			auto tmpUpperMatrix = Matrix4x4DX::RotAxis(Vector3DX::forward(), KeyControl::GetLeanRad() / 5.f);

			tmpUpperMatrix *=
				Matrix4x4DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x) *
				Matrix4x4DX::RotAxis(Vector3DX::up(), KeyControl::GetYRadBottomChange());

			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((this->m_MyID != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			if (HeadBobbing) {
				tmpUpperMatrix = WalkSwingControl::GetWalkSwingMat() * tmpUpperMatrix;
			}
			tmpUpperMatrix *= this->GetMove().GetMat().Get44DX();

			auto EyePosition = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + tmpUpperMatrix.zvec() * (-0.04f * Scale3DRate);

			if (HeadBobbing) {
				EyePosition += EyeSwingControl::GetEyeSwingPos();
			}
			if (GetGunPtrNow()) {
				EyePosition = Lerp<Vector3DX>(EyePosition, GetGunPtrNow()->GetEyePos(), GunReadyControl::GetADSPer());
			}
			return tmpUpperMatrix * Matrix4x4DX::Mtrans(EyePosition);
		}
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			if (this->m_MyID == value.DamageID) {
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
						SE->Get((int)SoundEnum::Man_Hurt1 + GetRand(6 - 1)).Play_3D(0, GetEyeMatrix().pos(), Scale3DRate * 10.f);
					}
					else {
						//SE->Get((int)SoundEnum::Man_Hurt1 + GetRand(6 - 1)).Play_3D(0, GetEyeMatrix().pos(), Scale3DRate * 10.f);
						//
						if (!PrevLive) {
						}
					}
				}
				else if (PrevLive) {
					for (int i = 0; i < 6; i++) {
						SE->Get((int)SoundEnum::Man_Hurt1 + i).StopAll(0);
					}
					SE->Get((int)SoundEnum::Man_contact).StopAll(0);
					SE->Get((int)SoundEnum::Man_openfire).StopAll(0);
					SE->Get((int)SoundEnum::Man_reload).StopAll(0);
					SE->Get((int)SoundEnum::Man_takecover).StopAll(0);
					SE->Get((int)SoundEnum::Man_breathing).StopAll(0);
					SE->Get((int)SoundEnum::Man_breathend).StopAll(0);

					SE->Get((int)SoundEnum::Man_Death1 + GetRand(8 - 1)).Play_3D(0, GetEyeMatrix().pos(), Scale3DRate * 10.f);
					if (value.ShotID == 0) {
						PlayerMngr->GetPlayer(value.ShotID)->AddScore(100 + ((IsGun0Select() && (value.Damage >= 100)) ? 20 : 0));
						PlayerMngr->GetPlayer(value.ShotID)->AddKill(1);

						auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(value.ShotID)->GetChara();
						if (Chara->GetAutoAimActive()) {

							Chara->AddULT(IsULTSelect() ? 1 : 6, Chara->IsGun0Select());
						}
						else {
							Chara->AddULT(IsULTSelect() ? 9 : 20, Chara->IsGun0Select());
						}
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
						if (m_MyID == 0) {
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
					//auto v1 = GetEyeMatrix().zvec() * -1.f;
					//auto v2 = (Chara->GetCharaPosition() - this->GetCharaPosition()).normalized(); v2.y = (0);
					//atan2f(Vector3DX::Cross(v1, v2).y, Vector3DX::Dot(v1, v2))
					LifeControl::SetSubHPEvent(AttackID, this->m_MyID, *Damage, *ArmerDamage);
				}
				//SE
				if (AttackID == 0) {
					if (*Damage > 0) {
						SE->Get((int)SoundEnum::Hit).Play_3D(0, GetEyeMatrix().pos(), Scale3DRate * 10.f);
					}
					else {
						SE->Get((int)SoundEnum::HitGuard).Play_3D(0, GetEyeMatrix().pos(), Scale3DRate * 10.f, 128);
					}
				}
				if (m_MyID == 0 && m_IsMainGame) {
					if (*Damage > 0) {
						SE->Get((int)SoundEnum::HitMe).Play_3D(0, GetEyeMatrix().pos(), Scale3DRate * 10.f);
					}
					else {
						SE->Get((int)SoundEnum::HitGuard).Play_3D(0, GetEyeMatrix().pos(), Scale3DRate * 10.f, 255);
					}
				}
				//エフェクトセット
				if (*Damage > 0) {
					EffectControl::SetOnce(EffectResource::Effect::ef_hitblood, *pEndPos, Vector3DX::forward(), Scale3DRate);
					EffectControl::SetEffectSpeed(EffectResource::Effect::ef_hitblood, 2.f);
				}
				else {
					EffectControl::SetOnce(EffectResource::Effect::ef_gndsmoke, *pEndPos, (Chara->GetCharaPosition() - this->GetCharaPosition()).normalized(), 0.25f * Scale3DRate);
				}
				//ヒットモーション
				{
					HitReactionControl::SetHit(Matrix4x4DX::Vtrans(Vector3DX::Cross((*pEndPos - StartPos).normalized(), Vector3DX::up()) * -1.f, GetFrameWorldMat(CharaFrame::Upper2).rotation().inverse()));
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
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//auto* OptionParts = OPTION::Instance();
			//
			auto PrevAction = m_CharaAction;
			//開始演出
			if (GetGunPtrNow() && m_ReadyAnim > 0.f) {
				m_ReadyAnim = std::max(m_ReadyAnim - 1.f / DrawParts->GetFps(), 0.f);
				switch (m_ReadyAnimPhase) {
				case 0:
					GunReadyControl::SetAim();
					if (m_ReadyAnim < 4.f) {
						m_ReadyAnimPhase++;
						//リロード
						m_CharaAction = CharaActionID::Reload;
						CameraShake::Instance()->SetCamShake(0.1f, 2.f);
					}
					break;
				case 1:
					if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadEnd) {
						m_ReadyAnimPhase++;
						CameraShake::Instance()->SetCamShake(0.1f, 2.f);
					}
					break;
				case 2:
					GunReadyControl::SetReady();
					if (IsLowReadyPer()) {
						m_ReadyAnimPhase++;
						GunReadyControl::SetAim();
						SelectGun(0);
						m_CharaAction = CharaActionID::Check;
						CameraShake::Instance()->SetCamShake(0.1f, 2.f);
					}
					break;
				case 3:
					if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::CheckEnd) {
						m_ReadyAnimPhase++;
					}
					break;
				default:
					m_ReadyAnim = -1.f;
					break;
				}
			}
			//
			bool Press_Shot = KeyControl::GetInputControl().GetPADSPress(PADS::SHOT) && !m_IsChange;
			bool Press_Reload = KeyControl::GetInputControl().GetPADSPress(PADS::RELOAD) && !m_IsChange;
			if (GetGunPtrNow() && GetGunPtrNow()->GetAmmoNumTotal() == 0 && GetGunPtrNow()->GetGunAnime() == GunAnimeID::Base) {
				Press_Reload |= Press_Shot;				//打ち切りで自動リロード
			}
			bool Press_Aim = KeyControl::GetInputControl().GetPADSPress(PADS::AIM) && !m_IsChange;
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
					if (i == this->m_MyID) { continue; }
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
					//アーマー着用/弾込め
					if (KeyControl::GetInputControl().GetPADSPress(PADS::CHECK)) {
						if (GetGunPtrNow()) {
							if (IsGun0Select() && MagStockControl::GetNeedAmmoLoad(GetGunPtrNow()->GetIsMagFull(), GetGunPtrNow()->GetIsMagEmpty())) {
								m_CharaAction = CharaActionID::AmmoLoad;
							}
							else if (true) {
								m_CharaAction = CharaActionID::Watch;
							}
						}
					}
					//射撃
					if (!KeyControl::GetRun() && Press_Shot) {
						if (IsAimPer()) {
							if (GetGunPtrNow() && (GetGunPtrNow()->GetGunAnime() == GunAnimeID::Base) && GetGunPtrNow()->GetInChamber()) {
								Shot_Start();
							}
						}
						GunReadyControl::SetAimOrADS();
					}
					//リロード/マガジンチェック
					if (!KeyControl::GetRun() && Press_Reload && GetGunPtrNow()) {
						bool IsCheck = GetGunPtrNow()->GetIsMagFull();
						if (IsGun0Select() && GetGunPtrNow()->GetModData()->GetReloadType() == RELOADTYPE::MAG) {
							IsCheck = (GetGunPtrNow()->GetAmmoNum() >= MagStockControl::GetMag(0));
						}
						if (!IsCheck) {
							m_CharaAction = CharaActionID::Reload;
						}
						else if (true) {
							m_CharaAction = CharaActionID::Check;
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
					if (KeyControl::GetRun()) {
						GunReadyControl::SetReady();
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
				case CharaActionID::Check:
					if (m_ActionFirstFrame && GetGunPtrNow()) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::CheckStart);
					}
					//
					if (GetGunPtrNow() && !GetGunPtrNow()->GetChecking()) {
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
						if ((GetGunPtrNow()->GetModData()->GetReloadType() == RELOADTYPE::AMMO) && (GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadOne)) {
							if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
								GetGunPtrNow()->SetCancel(true);
							}
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
				case CharaActionID::Melee:
					if (m_ActionFirstFrame) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::Melee);
						m_MeleeCoolDown = 1.f;
						if (this->m_MyID == 0) {
							CameraShake::Instance()->SetCamShake(0.1f, 5.f);
						}
					}
					//
					if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::Melee) {
						m_CharaAction = CharaActionID::Ready;
					}
					//姿勢指定
					GunReadyControl::SetAim();
					break;
				case CharaActionID::AmmoLoad:
					if (m_ActionFirstFrame) {
						this->m_AmmoLoadStart = true;
					}
					if (GetGunPtrNow()) {
						if (this->m_AmmoLoadStart) {
							if (IsLowReadyPer()) {
								this->m_AmmoLoadStart = false;
								GetGunPtrNow()->SetGunAnime(GunAnimeID::AmmoLoading);
								GetGunPtrNow()->SetCancel(false);
								this->m_AmmoLoadCount = 0;
								this->m_AmmoLoadSwitch = true;
							}
							GunReadyControl::SetReady();
						}
						else {
							if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::AmmoLoading) {
								//銃表示
								GetGunPtrNow()->SetActiveAll(false);
								(*GetGunPtrNow()->GetMagazinePtr())->SetActive(true);
								//
								GunReadyControl::SetAim();
								if (GetGunPtrNow()->GetObj().SetAnim((int)GunAnimeID::AmmoLoading).TimeStart()) {
									this->m_AmmoLoadSwitch = true;
								}

								if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
									GetGunPtrNow()->SetCancel(true);
								}
								if (this->m_AmmoLoadSwitch) {
									this->m_AmmoLoadSwitch = false;
									if (this->m_AmmoLoadCount < 4) {
										int AmmoCount = (m_AmmoLoadCount >= 1) ? MagStockControl::GetMag(this->m_AmmoLoadCount - 1) : GetGunPtrNow()->GetAmmoNum();
										int InAmmo = std::clamp(MagStockControl::GetAmmoStock(), 0, GetGunPtrNow()->GetAmmoAll() - AmmoCount);
										AmmoCount += InAmmo;
										MagStockControl::SubAmmoStock(InAmmo);
										for (int i = 2; i >= this->m_AmmoLoadCount; i--) {
											InAmmo = std::clamp(MagStockControl::GetMag(i), 0, GetGunPtrNow()->GetAmmoAll() - AmmoCount);
											AmmoCount += InAmmo;
											MagStockControl::SetMag(i, MagStockControl::GetMag(i) - InAmmo);
										}
										if (m_AmmoLoadCount == 0) {
											GetGunPtrNow()->SetAmmo(AmmoCount);
										}
										else {
											MagStockControl::SetMag(this->m_AmmoLoadCount - 1, AmmoCount);
										}
										this->m_AmmoLoadCount++;
										if (this->m_AmmoLoadCount >= 4) {
											this->m_AmmoLoadCount = 0;
											GetGunPtrNow()->SetCancel(true);
										}
									}
								}
							}
							else {
								GunReadyControl::SetReady();
								if (IsLowReadyPer()) {
									if (!GetGunPtrNow()->GetInChamber()) {
										m_CharaAction = CharaActionID::Cocking;
									}
									else {
										m_CharaAction = CharaActionID::Ready;
									}
									GetGunPtrNow()->SetActiveAll(true);
								}
							}
						}
					}
					break;
				default:
					break;
				}
				//近接
				if (!KeyControl::GetRun() && KeyControl::GetInputControl().GetPADSPress(PADS::MELEE) && m_MeleeCoolDown == 0.f && IsAimPer() && !GetIsADS() && GetGunPtrNow()) {
					m_CharaAction = CharaActionID::Melee;
				}
			}
			//
			m_ActionFirstFrame = (PrevAction != m_CharaAction);
		}
		//音指示
		void			CharacterClass::ExecuteSound(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			//足音
			if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
				auto Time = this->GetObj().SetAnim((int)this->m_BottomAnimSelect).GetTime();
				if (!GetRun()) {
					//L
					if ((9.f < Time && Time < 10.f)) {
						if (this->m_CharaSound != 1) {
							this->m_CharaSound = 1;
							this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::LeftFoot).pos()).magnitude() < Scale3DRate * 5.f * 1.5f) {

								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale3DRate * 5.f);
								if (!GunReadyControl::GetIsADS()) {
									if (this->m_MyID == 0) {
										auto* OptionParts = OPTION::Instance();
										bool HeadBobbing = ((this->m_MyID != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
										if (HeadBobbing) {
											//CameraShake::Instance()->SetCamShake(0.1f, 1.f);
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
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::RightFoot).pos()).magnitude() < Scale3DRate * 5.f * 1.5f) {

								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale3DRate * 5.f);
								if (!GunReadyControl::GetIsADS()) {
									if (this->m_MyID == 0) {
										auto* OptionParts = OPTION::Instance();
										bool HeadBobbing = ((this->m_MyID != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
										if (HeadBobbing) {
											//CameraShake::Instance()->SetCamShake(0.1f, 1.f);
										}
									}
								}
							}
						}
					}
				}
				else {
					//L
					if (
						(3.f < Time && Time < 4.f)
						) {
						if (this->m_CharaSound != 5) {
							this->m_CharaSound = 5;
							this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::LeftFoot).pos()).magnitude() < Scale3DRate * 5.f * 1.5f) {
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale3DRate * 15.f);
								if (!GunReadyControl::GetIsADS()) {
									if (this->m_MyID == 0) {
										auto* OptionParts = OPTION::Instance();
										bool HeadBobbing = ((this->m_MyID != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
										if (HeadBobbing) {
											//CameraShake::Instance()->SetCamShake(0.1f, 1.f);
										}
									}
								}
							}
						}
					}
					//R
					if (
						(10.f < Time && Time < 11.f)
						) {
						if (this->m_CharaSound != 6) {
							this->m_CharaSound = 6;
							this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::RightFoot).pos()).magnitude() < Scale3DRate * 5.f * 1.5f) {
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale3DRate * 15.f);
								if (!GunReadyControl::GetIsADS()) {
									if (this->m_MyID == 0) {
										auto* OptionParts = OPTION::Instance();
										bool HeadBobbing = ((this->m_MyID != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
										if (HeadBobbing) {
											//CameraShake::Instance()->SetCamShake(0.1f, 1.f);
										}
									}
								}
							}
						}
					}
				}
			}
			//しゃがみ音
			if (KeyControl::GetSquatSwitch()) {
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//リーン音
			if (KeyControl::GetLeanSwitch()) {
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale3DRate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//心拍音
			if (this->m_MyID == 0) {
				if (StaminaControl::ExcuteStamina(0.f, this->GetMove().GetVec().magnitude() / DrawParts->GetFps(), KeyControl::GetIsSquat())) {
					SE->Get((int)SoundEnum::Heart).Play_3D(0, GetEyeMatrix().pos(), Scale3DRate * 0.5f);
				}
			}
		}
		//SetMat指示更新
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			//vector
			Vector3DX PosBuf = this->GetMove().GetPos();
			bool IsHitGround = (PosBuf.y <= 0.f); //高度0なら止まる
			float groundYpos = 0.f;
			Vector3DX EndPos = PosBuf - Vector3DX::up() * (0.5f * Scale3DRate);
			IsHitGround = BackGround->CheckLinetoMap(PosBuf + Vector3DX::up() * (0.f * Scale3DRate), &EndPos);
			groundYpos = EndPos.y;
			if (IsHitGround) {
				auto yPos = PosBuf.y;
				Easing(&yPos, groundYpos, 0.6f, EasingType::OutExpo);
				PosBuf.y = (yPos);

				Vector3DX vec = KeyControl::GetVec(); vec.y = 0.f;
				this->SetMove().SetVec(vec);
				this->SetMove().Update(0.f, 0.f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
			else {
				float Y = this->GetMove().GetVec().y;
				Vector3DX vec = KeyControl::GetVec(); vec.y = (Y + (GravityRate / (DrawParts->GetFps() * DrawParts->GetFps())));
				this->SetMove().SetVec(vec);
			}
			PosBuf += this->GetMove().GetVec();
			//壁判定
			BackGround->CheckMapWall(this->m_move.GetRePos(), &PosBuf, Vector3DX::up() * (0.6f * Scale3DRate + 0.1f), Vector3DX::up() * (1.6f * Scale3DRate), 0.6f * Scale3DRate);
			//ほかプレイヤーとの判定
			if (m_IsMainGame) {
				float Radius = 2.f * 0.5f * Scale3DRate;
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
					if (i == this->m_MyID) { continue; }
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
			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((this->m_MyID != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			HeadBobbing = false;
			this->SetMove().SetPos(PosBuf);
			this->SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::forward(), HeadBobbing ? (KeyControl::GetRad().z / 2.f) : 0.f) * Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetYRadBottom()));
			this->SetMove().Update(0.9f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			//スリング場所探索
			int GunSel = m_IsHardMode ? 1 : 0;
			if (GetGunPtr(GunSel)) {
				m_SlingMat[GunSel] =
					Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(-90)) *
					GetFrameWorldMat(CharaFrame::Upper).rotation() *
					GetCharaDir().inverse() *
					Matrix4x4DX::Mtrans(
						GetFrameWorldMat(CharaFrame::Upper).pos() +
						GetFrameWorldMat(CharaFrame::Upper).yvec() * (-0.5f * Scale3DRate) +
						GetFrameWorldMat(CharaFrame::Upper).xvec() * (-0.3f * Scale3DRate)
					);
			}
			GunSel = 1 - GunSel;
			if (GetGunPtr(GunSel)) {
				m_SlingZrad.Update();
				m_SlingZrad.AddRad(((1.f - m_SlingPer.at(GetOtherGunSelect())) + 0.5f * (KeyControl::GetRad().y - KeyControl::GetYRadBottom())) / DrawParts->GetFps());
				m_SlingMat[GunSel] =
					Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(-30)) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(-90)) *
					(
						Matrix4x4DX::RotAxis(Vector3DX::forward(), -this->m_SlingZrad.GetRad()) *
						Matrix4x4DX::RotAxis(Vector3DX::up(), KeyControl::GetRad().y) *
						GetCharaDir().inverse()
						) *
					Matrix4x4DX::Mtrans(
						GetFrameWorldMat(CharaFrame::Head).pos() +
						GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.3f * Scale3DRate) +
						(
							this->GetMove().GetMat().xvec() * sin(m_SlingZrad.GetRad()) +
							this->GetMove().GetMat().yvec() * cos(m_SlingZrad.GetRad())
							) * -(0.5f * Scale3DRate)
					);
			}
			//銃座標指定
			Vector3DX GunPos;
			Vector3DX Gunyvec;
			Vector3DX Gunzvec;
			Vector3DX HandsPos;
			Vector3DX Handsyvec;
			Vector3DX Handszvec;
			if (GetGunPtrNow()) {
				//銃ひっこめ
				{
					if (m_StuckGunTimer == 0.f) {
						m_StuckGunTimer = 0.1f;
						auto EndPost = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Muzzle).pos();
						if (BackGround->CheckLinetoMap(GetEyeMatrix().pos(), &EndPost)) {//0.03ms
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
						m_StuckGunTimer = std::max(m_StuckGunTimer - 1.f / DrawParts->GetFps(), 0.f);
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
					Vector3DX Post0 = GetFrameWorldMat(CharaFrame::Head).pos() + Matrix4x4DX::Vtrans(AnimMat.pos(), GetCharaDir());
					//
					this->m_ULTBar.Execute(IsGun0Select(), 0.1f, 0.1f, 0.7f);
					//
					for (int index = 0; index < 2; index++) {
						auto& p = GetGunPtr(index);
						if (p) {
							bool IsSelGun = (index == GetNowGunSelect());
							auto& Mat = this->m_SlingMat[index];
							auto& Per = this->m_SlingPer[index];
							Easing(&Per, IsSelGun ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
							if (Per <= 0.001f) { Per = 0.f; }
							if (Per >= 0.999f) { Per = 1.f; }

							auto tmp_gunmat = Matrix4x4DX::RotVec2(Vector3DX::forward(), Lerp(Mat.zvec(), AnimMat.zvec(), Per));

							//オートエイム
							if (IsSelGun && GunReadyControl::GetIsADS() && GetGunPtrNow()->IsAutoAimActive() && m_IsMainGame) {
								Vector3DX BasePos = p->GetFrameWorldMat_P(GunFrame::Muzzle).pos();
								float Range = GetIsLaserActive() ? 13.f : 7.f;

								for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
									if (i == this->m_MyID) { continue; }
									auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
									if (!Chara->IsAlive()) { continue; }
									if (!Chara->CanLookTarget) { continue; }
									Vector3DX Vec = (Chara->GetEyeMatrix().pos() - BasePos);
									if (Vec.magnitude() >= (Scale3DRate * Range)) { continue; }
									if (Vector3DX::Dot((GetEyeMatrix().zvec() * -1.f), Vec.normalized()) < cos(deg2rad(15))) { continue; }

									AutoAimControl::SetAutoAimActive(i);
									break;
								}

								if (GetAutoAimActive()) {
									auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetAutoAimID())->GetChara();
									Vector3DX Vec = (Chara->GetEyeMatrix().pos() - BasePos);
									tmp_gunmat = Matrix4x4DX::RotVec2(GetEyeMatrix().zvec() * -1.f, Vec.normalized()) * tmp_gunmat;

								}
							}

							float PAdd = 0.f;
							if (!IsSelGun) {
								PAdd = -1.f * Scale3DRate * Per;
							}

							tmp_gunmat *= Matrix4x4DX::RotVec2(tmp_gunmat.yvec(), Lerp(Mat.yvec(), AnimMat.yvec(), Per));
							tmp_gunmat *=
								KeyControl::GetGunSwingMat() * GetCharaDir() *
								Matrix4x4DX::RotVec2(Lerp(p->GetObj().GetMatrix().yvec(), p->GetEyeYVec(), GunReadyControl::GetADSPer()), p->GetObj().GetMatrix().yvec()) *
								Matrix4x4DX::Mtrans(Lerp(Mat.pos() + Vector3DX::up() * PAdd, Post0, Per));
							p->SetGunMatrix(tmp_gunmat);
						}
					}
				}
				else {
					{
						auto tmp_gunmat =
							lagframe_.RIGHThand_f.GetFrameWorldPosition().rotation().inverse() *
							RagDollControl::GetRagDoll().GetFrameLocalWorldMatrix(lagframe_.RIGHThand_f.GetFrameID());

						GetGunPtrNow()->SetGunMatrix(tmp_gunmat);
					}
					if (GetGunPtrOther()) {
						//仮の画面外指定
						auto tmp_gunmat = GetFrameWorldMat(CharaFrame::Upper);
						tmp_gunmat = Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(90)) * tmp_gunmat;
						GetGunPtrOther()->SetGunMatrix(tmp_gunmat);
					}
				}
				//銃表示
				if (GetGunPtrOther()) {
					GetGunPtrOther()->SetActiveAll(true);
				}
				//手の位置を制御
				if ((m_MyID == 0) || this->CanLookTarget) {
					GunPos = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::RightHandPos).pos();
					Gunyvec = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::RightHandYvec).pos() - GunPos;
					Gunzvec = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::RightHandZvec).pos() - GunPos;
					{
						switch (GetGunPtrNow()->GetModData()->GetReloadType()) {
						case RELOADTYPE::MAG:
							switch (GetGunPtrNow()->GetGunAnime()) {
							case GunAnimeID::ReloadStart_Empty:
								if (GetGunPtrNow()->GetObj().SetAnim((int)GunAnimeID::ReloadStart_Empty).GetTimePer() > 0.5f) {
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
								if (GetGunPtrNow()->GetObj().SetAnim((int)GunAnimeID::ReloadStart).GetTimePer() > 0.8f) {
									m_MagHand = true;
								}
								break;
							case GunAnimeID::ReloadOne:
								if (GetGunPtrNow()->GetGunAnimePer(EnumGunAnimType::Reload) < 0.6f) {
									m_MagHand = true;
								}
								else {
									m_MagHand = false;
								}
								break;
							case GunAnimeID::Base:
							case GunAnimeID::ReloadEnd:
							case GunAnimeID::CheckEnd:
							case GunAnimeID::Shot:
								m_MagHand = false;
								if (m_ArmBreak) {
									m_MagHand = true;
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
								if (GetGunPtrNow()->GetObj().SetAnim((int)GunAnimeID::ReloadStart_Empty).GetTimePer() > 0.8f) {
									m_MagHand = true;
								}
								break;
							case GunAnimeID::ReloadStart:
								if (GetGunPtrNow()->GetObj().SetAnim((int)GunAnimeID::ReloadStart).GetTimePer() > 0.8f) {
									m_MagHand = true;
								}
								break;
							case GunAnimeID::ReloadOne:
								if (GetGunPtrNow()->GetGunAnimePer(EnumGunAnimType::Reload) < 0.6f) {
									m_MagHand = true;
								}
								else {
									m_MagHand = false;
								}
								break;
							case GunAnimeID::Base:
							case GunAnimeID::ReloadEnd:
							case GunAnimeID::CheckEnd:
							case GunAnimeID::Shot:
								m_MagHand = false;
								if (m_ArmBreak) {
									m_MagHand = true;
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
						if (GetCharaAction() == CharaActionID::Melee) {
							m_MagHand = true;
						}
						if (GetCharaAction() == CharaActionID::Watch) {
							if (m_ArmBreak) {
								m_MagHand = true;
							}
							else {
								m_MagHand = this->IsGun0Select();
							}
						}

						m_MagArm.Execute(m_MagHand, 0.3f, 0.5f);

						Vector3DX HandPos = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::LeftHandPos).pos();
						Vector3DX Handyvec = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::LeftHandYvec).pos() - HandPos;
						Vector3DX Handzvec = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::LeftHandZvec).pos() - HandPos;

						Vector3DX MagPos = GetFrameWorldMat(CharaFrame::LeftMag).pos();
						Vector3DX Magyvec = GetFrameWorldMat(CharaFrame::LeftMagyvec).pos() - MagPos;
						Vector3DX Magzvec = GetFrameWorldMat(CharaFrame::LeftMagzvec).pos() - MagPos;
						HandsPos = Lerp(HandPos, MagPos, m_MagArm.Per());
						Handsyvec = Lerp(Handyvec, Magyvec, 0.f);
						Handszvec = Lerp(Handzvec, Magzvec, 0.f);
					}
					//腕座標指定
					{
						switch (GetGunPtrNow()->GetGunAnime()) {
						case GunAnimeID::Cocking:
						case GunAnimeID::ReloadStart_Empty:
						case GunAnimeID::ReloadStart:
						case GunAnimeID::ReloadOne:
						case GunAnimeID::ReloadEnd:
						case GunAnimeID::CheckStart:
						case GunAnimeID::Checking:
						case GunAnimeID::CheckEnd:
						case GunAnimeID::AmmoLoading:
						case GunAnimeID::AmmoLoadEnd:
							StackLeftHandControl::ResetStackLeftHand();
							break;
						default:
							if (!GetRun() && !m_ArmBreak) {
								auto StartPos = GetFrameWorldMat(CharaFrame::Head).pos() + Matrix4x4DX::Vtrans(Vector3DX::vget(0.f, -0.1f, -0.2f) * Scale3DRate, GetCharaDir());
								auto EndPost = StartPos + Matrix4x4DX::Vtrans(Vector3DX::vget(0.5f, -0.1f, -0.7f) * Scale3DRate, GetCharaDir());
								Vector3DX Normal;
								if (BackGround->CheckLinetoMap(StartPos, &EndPost, &Normal)) {
									StackLeftHandControl::SetStackLeftHand(EndPost, Normal);
								}
								else {
									StackLeftHandControl::ResetStackLeftHand();
								}
							}
							break;
						}
						StackLeftHandControl::UpdateStackLeftHand();

						HandsPos = Lerp(HandsPos, StackLeftHandControl::GetStuckLeftHandPos(), StackLeftHandControl::GetStuckLeftHandPer());
						Handsyvec = Lerp(Handsyvec, StackLeftHandControl::GetStuckLeftHandYVec(), StackLeftHandControl::GetStuckLeftHandPer());
						Handszvec = Lerp(Handszvec, Vector3DX::Cross(StackLeftHandControl::GetStuckLeftHandYVec(), GetEyeMatrix().yvec()).normalized(), StackLeftHandControl::GetStuckLeftHandPer());
					}
					if (m_ArmBreak) {
						HandsPos += Vector3DX::vget(GetRandf(1.f * Scale3DRate), GetRandf(1.f * Scale3DRate), GetRandf(1.f * Scale3DRate)) * 0.002f;
					}

					IK_RightArm(
						&GetObj(),
						GetFrame(static_cast<int>(CharaFrame::RightArm)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
						GetFrame(static_cast<int>(CharaFrame::RightArm2)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
						GetFrame(static_cast<int>(CharaFrame::RightWrist)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
						GunPos, Gunyvec, Gunzvec);
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
				//LeftMag
				{
					auto tmp_gunmat = Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(-90)) * GetFrameWorldMat(CharaFrame::LeftHandJoint);
					bool isDirect = false;
					switch (GetGunPtrNow()->GetGunAnime()) {
					case GunAnimeID::ReloadStart_Empty:
						m_AmmoHandR = 1.f;
						m_AmmoHand = 1.f;
						break;
					case GunAnimeID::ReloadStart:
						m_AmmoHandR = 1.f;
						m_AmmoHand = 1.f;
						break;
					case GunAnimeID::ReloadOne:
						if (GetGunPtrNow()->GetGunAnimePer(EnumGunAnimType::Reload) < 0.86f) {
							m_AmmoHandR = 1.f;
							m_AmmoHand = 1.f;
						}
						if (GetGunPtrNow()->GetGunAnimePer(EnumGunAnimType::Reload) > 0.96f) {
							m_AmmoHandR = 0.f;
							m_AmmoHand = 0.f;
						}
						break;
					case GunAnimeID::CheckStart:
						if (GetGunPtrNow()->GetObj().SetAnim((int)GunAnimeID::CheckStart).GetTimePer() > 0.5f) {
							m_AmmoHandR = 1.f;
						}
						break;
					case GunAnimeID::Checking:
						m_AmmoHandR = 1.f;
						break;
					case GunAnimeID::CheckEnd:
						m_AmmoHandR = 0.f;
						break;
					case GunAnimeID::AmmoLoading:
						m_AmmoHandR = 1.f;
						m_AmmoHand = 1.f;
						isDirect = true;
						break;
					case GunAnimeID::AmmoLoadEnd:
						m_AmmoHandR = 0.f;
						isDirect = true;
						break;
					default:
						m_AmmoHandR = 0.f;
						tmp_gunmat = GetGunPtrNow()->GetFrameWorldMat_P(GunFrame::Magpos);
						break;
					}
					if (GetGunPtrNow()->GetModData()->GetReloadType() == RELOADTYPE::AMMO) {
						m_AmmoHand = 0.f;
						isDirect = false;
					}
					(*GetGunPtrNow()->GetMagazinePtr())->SetHandMatrix(tmp_gunmat, m_AmmoHand, isDirect);
					if (GetGunPtrOther()) {
						(*GetGunPtrOther()->GetMagazinePtr())->SetHandMatrix(tmp_gunmat, 0.f, false);
					}
					Easing(&m_AmmoHand, m_AmmoHandR, 0.9f, EasingType::OutExpo);
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
			ULTControl::InitULT();
			//
			this->m_MyID = pID;
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
				GetGunPtr(0)->SetPlayerID(this->m_MyID);
			}
			if (GetGunPtr(1)) {
				GetGunPtr(1)->SetPlayerID(this->m_MyID);
			}
			m_SlingZrad.Init(0.05f * Scale3DRate, 3.f, deg2rad(50));
			m_HPRec = 0.f;
			m_CharaAction = CharaActionID::Ready;
			m_ActionFirstFrame = true;
		}
		void			CharacterClass::MovePoint(float pxRad, float pyRad, const Vector3DX& pPos, int GunSel) noexcept {
			KeyControl::InitKey(pxRad, pyRad);
			SetMove().SetAll(pPos, pPos, pPos, Vector3DX::zero(), Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetRad().y), Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetRad().y));
			if (GetGunPtr(0)) {
				MagStockControl::Init_MagStockControl(GetGunPtr(0)->GetAmmoNum(), GetGunPtr(0)->GetAmmoAll(), GetGunPtr(0)->GetMagUniqueID());
			}
			SelectGun(GunSel);
			this->m_ULTBar.Init(IsGun0Select());
			this->m_SlingPer[0] = (IsGun0Select()) ? 0.f : 1.f;
			this->m_SlingPer[1] = 1.f - this->m_SlingPer[0];
			if (GunSel == 1) {
				m_ReadyAnim = 5.f;
				m_ReadyAnimPhase = 0;
			}
			else {
				m_ReadyAnim = -1.f;
			}
			GunReadyControl::SetReady();
			//
			this->m_MorphineStock = !m_IsHardMode ? 3 : 1;
			this->m_ArmerStock = 0;
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			KeyControl::InputKey(pInput, pReady, StaminaControl::GetHeartRandVec(KeyControl::GetIsSquat() ? 1.f : 0.f));
			//AIM
			if (GetGunPtrNow()) {
				if (pReady) {
					if (!m_IsChanging) {
						if (!m_ULTActive) {
							if (m_ULTUp > 0.5f) {
								m_ULTUp -= 0.5f;
								ULTControl::AddULT(1, true);
							}
							m_ULTUp += 1.f / DrawParts->GetFps();
							if (ULTControl::IsULTActive() && KeyControl::GetULTKey()) {
								m_IsChanging = true;
								m_IsChange = true;
								m_ULTActive = true;
								m_ULTUp = 0.f;
							}
						}
						else {
							if (m_ULTUp > 0.25f) {
								m_ULTUp -= 0.25f;
								ULTControl::AddULT(-1, true);
							}
							m_ULTUp += 1.f / DrawParts->GetFps();
							if (ULTControl::GetULT() == 0 || (GetIsAim() && KeyControl::GetULTKey())) {
								m_IsChanging = true;
								m_IsChange = true;
								m_ULTActive = false;
								m_ULTUp = 0.f;
								ULTControl::AddULT(-20, false);
							}
						}
					}
					if (IsAlive()) {
						if (IsLowHP()) {
							if (m_HPRec >= 0.f) {
								m_HPRec -= 2.f;
								Heal(2, false);
								if (m_MyID == 0) {
									SE->Get((int)SoundEnum::Man_breathing).Play(0, DX_PLAYTYPE_BACK);
								}
							}
							m_HPRec += 1.f / DrawParts->GetFps();
						}
						else {
							if (m_HPRec != 0.f) {
								if (m_MyID == 0) {
									SE->Get((int)SoundEnum::Man_breathing).StopAll(0);
									SE->Get((int)SoundEnum::Man_breathend).Play(0, DX_PLAYTYPE_BACK);
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
							GetGunPtrNow()->SetAmmo(GetGunPtrNow()->GetAmmoAll());
							GetGunPtrNow()->UnloadChamber();
							GetGunPtrNow()->CockByMag();
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
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
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
			}
			int num = GetObj().GetMaterialNum();
			for (int i = 0; i < num; i++) {
				GetObj().SetMaterialDifColor(i, GetColorF(0.8f, 0.8f, 0.8f, 1.f));
				GetObj().SetMaterialAmbColor(i, GetColorF(0.25f, 0.25f, 0.25f, 1.f));
			}
			this->m_SoundPower = std::max(this->m_SoundPower - 1.f / DrawParts->GetFps(), 0.f);
			GunReadyControl::UpdateReady();
			m_MeleeCoolDown = std::max(m_MeleeCoolDown - 1.f / DrawParts->GetFps(), 0.f);
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
				m_SightChange.Execute(GetIsADS() && KeyControl::GetInputControl().GetPADSPress(PADS::JUMP));
				if (m_SightChange.trigger()) {
					GetGunPtrNow()->ChangeSightSel();
				}
			}
			//
			this->m_Arm[(int)EnumGunAnimType::ADS].Execute(GunReadyControl::GetIsADS(), 0.2f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::Run].Execute(!GetIsAim() && KeyControl::GetRun(), 0.1f, 0.2f, 0.95f);
			if (GetGunPtrNow()) {
				this->m_Arm[(int)EnumGunAnimType::ReloadStart_Empty].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadStart_Empty, 0.5f, 0.2f);
				this->m_Arm[(int)EnumGunAnimType::ReloadStart].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadStart, 0.2f, 0.2f);
				this->m_Arm[(int)EnumGunAnimType::Reload].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadOne, 0.1f, 0.2f, 0.9f);
				this->m_Arm[(int)EnumGunAnimType::ReloadEnd].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadEnd, 0.1f, 0.2f, 0.9f);
				this->m_Arm[(int)EnumGunAnimType::Ready].Execute(!GetIsAim(), 0.1f, 0.2f, 0.9f);
				this->m_Arm[(int)EnumGunAnimType::Check].Execute(GetGunPtrNow()->GetGunAnime() >= GunAnimeID::CheckStart && GetGunPtrNow()->GetGunAnime() <= GunAnimeID::Checking, 0.05f, 0.2f);
				this->m_Arm[(int)EnumGunAnimType::Watch].Execute(GetCharaAction() == CharaActionID::Watch, 0.1f, 0.1f);
				this->m_Arm[(int)EnumGunAnimType::Melee].Execute(GetCharaAction() == CharaActionID::Melee, 1.1f, 0.1f);
				this->m_Arm[(int)EnumGunAnimType::AmmoLoad].Execute(GetIsAim() && (GunAnimeID::AmmoLoading <= GetGunPtrNow()->GetGunAnime()) && (GetGunPtrNow()->GetGunAnime() <= GunAnimeID::AmmoLoadEnd), 0.1f, 0.1f);
			}
			//
			KeyControl::UpdateKeyRad();
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::Upper));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::Upper),
				GetFrameLocalMat(CharaFrame::Upper).rotation() * Matrix4x4DX::RotAxis(Vector3DX::right(), -KeyControl::GetRad().x / 2.f) * (GetCharaDir() * this->GetMove().GetMat().Get44DX().inverse()).rotation()
				* GetFrameBaseLocalMat((int)CharaFrame::Upper));
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::Upper2));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::Upper2),
				GetFrameLocalMat(CharaFrame::Upper2).rotation() * Matrix4x4DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x / 2.f) * HitReactionControl::GetHitReactionMat()
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
			EyeSwingControl::UpdateEyeSwing(GetCharaDir(), std::clamp(KeyControl::GetVec().magnitude() / 0.65f, 0.f, 0.85f / 0.65f), KeyControl::GetRun() ? 8.f : 5.f);
			//
			ExecuteSound();
			//
			ExecuteMatrix();
			//
			RagDollControl::Execute_RagDollControl(this->GetObj(), LifeControl::IsAlive());													//ラグドール
			HitBoxControl::UpdataHitBox(this, (this->GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);									//ヒットボックス
			WalkSwingControl::UpdateWalkSwing(GetEyeMatrix().pos() - GetCharaPosition(), std::clamp(this->GetMove().GetVec().magnitude() / 2.f, 0.f, 0.5f));
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
