#include	"Character.hpp"

#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Charas {
		int CharacterObj::GetWeight_gram(void) const noexcept{
		auto* PlayerMngr = Player::PlayerManager::Instance();
			int gram = 0;


			gram += 15000;//アーマー分など


			for (int loop = 0, max = this->m_GunPtrControl.GetGunNum(); loop < max; ++loop) {
				auto& pGun = this->m_GunPtrControl.GetGunPtr(loop);
				if (!pGun) { continue; }
				gram += pGun->GetWeight_gram();
			}

			for (auto& ID : PlayerMngr->GetPlayer(GetMyPlayerID())->GetInventory()) {
				if (ID.first != InvalidID) {
					auto& item = Objects::ItemObjDataManager::Instance()->GetList().at(ID.first);
					gram += item->GetWeight_gram();
				}
			}
			return gram;
		}

		float CharacterObj::GetDebuff(void) const noexcept {
			int gram = GetWeight_gram();

			if (gram == 0) {
				return 1.5f;
			}
			else if (gram < 20000) {
				return Lerp(1.5f, 1.f, static_cast<float>(gram) / 20000);
			}
			else if (gram < 80000) {
				return Lerp(1.f, 0.25f, static_cast<float>(gram - 20000) / (80000 - 20000));
			}
			else {
				return 0.25f;
			}
		}

		bool			CharacterObj::SetDamageEvent(const DamageEvent& Event) noexcept {
			if (GetMyPlayerID() == Event.DamageID) {
				auto* SE = SoundPool::Instance();
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto* SideLogParts = SideLog::Instance();
				auto* LocalizeParts = LocalizePool::Instance();

				auto PrevLive = IsAlive();

				ArmerPoint BodyArmerDamage = 0;
				ArmerPoint HeadArmerDamage = 0;
				HitPoint Damage = Event.Damage;

				//部位ダメージ演算
				if (Damage > 0) {
					switch (static_cast<HitType>(Event.HitType)) {
					case HitType::Head:
						if (Event.DamageID != PlayerMngr->GetWatchPlayerID()) {//自機はヘッショされない
							Damage = Damage * 500 / 100;
						}
						HeadArmerDamage = this->m_HeadPoint.GetPoint();
						Damage = std::clamp<HitPoint>(Damage - HeadArmerDamage, 0, this->m_HP.GetMax());
						this->m_HeadPoint.Sub(this->m_HeadPoint.GetMax() / 2);
						break;
					case HitType::Body:
						BodyArmerDamage = this->m_BodyPoint.GetPoint();
						Damage = std::clamp<HitPoint>(Damage - BodyArmerDamage, 0, this->m_HP.GetMax());
						this->m_BodyPoint.Sub(this->m_BodyPoint.GetMax() / 5);
						break;
					case HitType::Arm:
						Damage = Damage * 30 / 100;
						break;
					case HitType::Leg:
						Damage = Damage * 50 / 100;
						break;
					default:
						break;
					}
					if (Event.DamageID == PlayerMngr->GetWatchPlayerID()) {//無敵debug
						//Damage = 0;
					}
				}
				//アーマーヒール
				if (Damage < 0) {
					switch (static_cast<HitType>(Event.HitType)) {
					case HitType::Helmet:
						HeadArmerDamage = -1;
						BodyArmerDamage = -1;
						this->m_HeadPoint.Sub(-this->m_HeadPoint.GetMax());
						Damage = 0;
						break;
					case HitType::Armor:
						HeadArmerDamage = -1;
						BodyArmerDamage = -1;
						this->m_BodyPoint.Sub(-this->m_BodyPoint.GetMax());
						Damage = 0;
						break;
					default:
						break;
					}
				}

				//自機だけダメージを半減
				if (Damage > 0) {
					if (Event.DamageID == PlayerMngr->GetWatchPlayerID()) {//無敵debug
						Damage = Damage * 50 / 100;
					}
				}

				auto Prev = this->m_HP.GetPoint();
				this->m_HP.Sub(Damage);
				Damage = std::min(Damage, Prev);
				bool IsDeath = PrevLive && !IsAlive();

				if (Event.ShotID == PlayerMngr->GetWatchPlayerID()) {//撃ったキャラ
					//SE
					if (Damage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Hit))->Play3D(GetEyePositionCache(), Scale3DRate * 10.0f);
					}
					if (BodyArmerDamage > 0 || HeadArmerDamage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->Play3D(GetEyePositionCache(), Scale3DRate * 10.0f, 128);
					}
					//ヒットカウント
					if ((Damage >= 0) && (BodyArmerDamage >= 0 || HeadArmerDamage >= 0)) {
						PlayerMngr->GetPlayer(Event.ShotID)->AddHit(1);
						//ヒット座標表示を登録
						HitMarkerPool::Instance()->AddMarker(Event.EndPos, Damage, std::max(BodyArmerDamage, HeadArmerDamage));
					}
					if (IsDeath) {
						PlayerMngr->GetPlayer(Event.ShotID)->AddScore(100);
						PlayerMngr->GetPlayer(Event.ShotID)->AddKill(1);
						SideLogParts->Add(5.0f, 0.0f, Green, (LocalizeParts->Get(204) + (std::string)(" +100")).c_str());
					}
				}
				if (Event.DamageID == PlayerMngr->GetWatchPlayerID()) {//撃たれたキャラ
					if (Damage > 0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitMe))->Play3D(GetEyePositionCache(), Scale3DRate * 10.0f);
						if (IsDeath) {
							SideLogParts->Add(5.0f, 0.0f, Red50, LocalizeParts->Get(203));
						}
						else {
							SideLogParts->Add(5.0f, 0.0f, Red, (std::to_string(-Damage) + " " + LocalizeParts->Get(200)).c_str());
						}
					}
					else if (BodyArmerDamage > 0 || HeadArmerDamage>0) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGuard))->Play3D(GetEyePositionCache(), Scale3DRate * 10.0f, 255);
						SideLogParts->Add(5.0f, 0.0f, Yellow, LocalizeParts->Get(201));
					}
					else if (Damage == 0) {
						SideLogParts->Add(5.0f, 0.0f, Green, LocalizeParts->Get(202));
					}
				}
				//エフェクトセット
				auto* OptionParts = OptionManager::Instance();
				if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
					if (Damage > 0) {
						EffectSingleton::Instance()->SetOnce(Effect::ef_hitblood, Event.EndPos, Vector3DX::forward(), Scale3DRate);
						EffectSingleton::Instance()->SetEffectSpeed(Effect::ef_hitblood, 2.0f);
					}
					if (BodyArmerDamage > 0 || HeadArmerDamage > 0) {
						EffectSingleton::Instance()->SetOnce(Effect::ef_gndsmoke, Event.EndPos, (Event.StartPos - Event.EndPos).normalized(), 0.25f * Scale3DRate);
					}
				}
				//ヒットモーション
				if (Damage > 0) {
					this->m_HitReactionControl.SetHit(Matrix3x3DX::Vtrans(Vector3DX::Cross((Event.EndPos - Event.StartPos).normalized(), Vector3DX::up()) * -1.0f, Matrix3x3DX::Get33DX(GetFrameWorldMat(CharaFrame::Upper2)).inverse()));
					switch (static_cast<HitType>(Event.HitType)) {
					case HitType::Head:
						this->m_HeadShotSwitch = true;
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
					if ((Damage >= 0) && (BodyArmerDamage >= 0 || HeadArmerDamage >= 0)) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_Hurt1) + GetRand(6 - 1))->Play3D(GetEyePositionCache(), Scale3DRate * 10.0f);
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

					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_Death1) + GetRand(8 - 1))->Play3D(GetEyePositionCache(), Scale3DRate * 10.0f);
				}

				if (IsDeath) {
					for (auto& item : m_ItemList_EnableSpawnBySoldier) {
						Objects::ItemObjPool::Instance()->Put(item,
							GetFrameWorldMat(CharaFrame::Upper2).pos(),
							Vector3DX::vget(GetRandf(1.f), 1.f, GetRandf(1.f)) * Scale3DRate
						);
					}
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
				//弾道線プール
				Objects::AmmoLinePool::Instance()->Put(StartPos, (*pEndPos - StartPos), GetEyePositionCache());
			}
			//被弾処理
			auto* HitPtr = this->m_HitBoxControl.GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				//ダメージ登録
				if (AttackID >= 0) {
					PlayerMngr->GetPlayer(AttackID)->GetChara()->SetDamage(GetMyPlayerID(), BaseDamage, static_cast<int>(HitPtr->GetColType()), StartPos, *pEndPos);
				}
				else if (AttackID == -1) {
					//PlayerMngr->GetVehicle()->SetDamage(GetMyPlayerID(), BaseDamage, static_cast<int>(HitPtr->GetColType()), StartPos, *pEndPos);
				}
				else if (AttackID == -2) {
					PlayerMngr->GetHelicopter()->SetDamage(GetMyPlayerID(), BaseDamage, static_cast<int>(HitPtr->GetColType()), StartPos, *pEndPos);
				}
				else if (AttackID == -3) {
					PlayerMngr->GetTeamHelicopter()->SetDamage(GetMyPlayerID(), BaseDamage, static_cast<int>(HitPtr->GetColType()), StartPos, *pEndPos);
				}
				return true;
			}
			return false;
		}
		//操作
		void			CharacterObj::UpdateInput(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//
			if (!IsAlive()) { return; }
			if (this->m_Input.GetPADSTrigger(Controls::PADS::SQUAT)) {
				this->m_IsSquat ^= 1;
				//しゃがみ音
				if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetEyePositionCache(), Scale3DRate * 3.0f);
				}
			}
			if (GetGunPtrNow()) {
				//銃ひっこめ
				if (this->m_StuckGunTimer == 0.0f) {
					this->m_StuckGunTimer = 0.1f;

					Vector3DX GunStart = GetEyePositionCache();
					Vector3DX GunEnd = (GetGunPtrNow()->GetBaseMuzzleMat() * GetFrameWorldMat(CharaFrame::Head)).pos();
					bool IsHit = BackGround::BackGroundControl::Instance()->CheckLinetoMap(GunStart, GunEnd) != 0;
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
					this->m_StuckGunTimer = std::max(this->m_StuckGunTimer - DXLib_refParts->GetDeltaTime(), 0.0f);
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
				//武器切替
				if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::LowReady && GetGunPtrNow()->GetGunAnime() != GunAnimeID::EmergencyReady) {
					int Wheel = 0;
					if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
						Wheel = -PadControl::Instance()->GetMouseWheelRot();
						if (this->m_Input.GetPADSTrigger(Controls::PADS::ULT)) {
							Wheel = 1;
						}
					}
					if (Wheel != 0) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
						this->m_GunPtrControl.GunChangeNext((Wheel > 0));
					}
				}
				//アクション
				switch (GetGunPtrNow()->GetGunAnime()) {
				case GunAnimeID::LowReady:
					if (this->m_GunPtrControl.IsChangeGunSelect() && GetGunPtrNow()->GetGunAnimBlendPer(GunAnimeID::LowReady) > 0.95f) {
						this->m_GunPtrControl.InvokeReserveGunSelect();
						if (GetGunPtrNow()) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
						}
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetEyePositionCache(), Scale3DRate * 3.0f);
					}
					break;
				case GunAnimeID::EmergencyReady:
					if (this->m_GunPtrControl.IsChangeGunSelect() && GetGunPtrNow()->GetGunAnimBlendPer(GunAnimeID::EmergencyReady) > 0.1f) {
						this->m_GunPtrControl.InvokeReserveGunSelect();
						if (GetGunPtrNow()) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
						}
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetEyePositionCache(), Scale3DRate * 3.0f);
					}
					break;
				case GunAnimeID::Base:
					if (GetGunPtrNow()->GetCanShot()) {
						//武器を眺める
						if (this->m_Input.GetPADSPress(Controls::PADS::CHECK)) {
							if (this->m_CanWatch) {
								GetGunPtrNow()->SetGunAnime(GunAnimeID::Watch);
								this->m_CanWatch = false;
							}
						}
						else {
							this->m_CanWatch = true;
						}

						if (IsWearingArmor()) {
							{
								bool isEmergency = (GetGunPtr(0)->GetGunAnime() == GunAnimeID::EmergencyReady);
								if (isEmergency) {
									GetGunPtr(0)->SetGunAnime(GunAnimeID::LowReady);
								}
							}
							GetGunPtrNow()->SetGunAnime(GunAnimeID::LowReady);
							//オフ
							this->m_GunPtrControl.SetOnOff(false);
							m_WearArmorTime = 0.f;
						}

						bool EmptyDo = ((GetGunPtrNow()->GetAmmoNumTotal() == 0) && this->m_Input.GetPADSPress(Controls::PADS::SHOT));
						bool IsEmergencyReload = this->m_Input.GetPADSPress(Controls::PADS::RELOAD) || (EmptyDo && !GetIsADS());

						if (EmptyDo && GetIsADS()) {
							if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
								if (GetGunPtrNowSel() == 0) {
									GetGunPtrNow()->SetGunAnime(GunAnimeID::EmergencyReady);
									this->m_GunPtrControl.GunChangeNext(true);
								}
								else {
									IsEmergencyReload = true;
								}
							}
							else {
								IsEmergencyReload = true;
							}
						}

						if (!GetGunPtrNow()->GetModifySlot()->GetMyData()->GetIsThrowWeapon()) {
							//Reload
							if (IsEmergencyReload) {
								this->m_ReserveReload = true;
							}

							if ((GetGunPtrNow()->GetNowAnimTime() > 30.f) && this->m_ReserveReload) {
								this->m_ReserveReload = false;
								if (GetGunPtrNow()->ReloadStart()) {
									{
										bool isEmergency = (GetGunPtr(0)->GetGunAnime() == GunAnimeID::EmergencyReady);
										if (isEmergency) {
											GetGunPtr(0)->SetGunAnime(GunAnimeID::LowReady);
										}
									}
									if (GetMyPlayerID() != PlayerMngr->GetWatchPlayerID()) {
										if (GetRand(100) < 50) {
											SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_reload))->Play3D(GetEyePositionCache(), Scale3DRate * 10.0f);
										}
										else {
											SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_takecover))->Play3D(GetEyePositionCache(), Scale3DRate * 10.0f);
										}
									}
								}
							}
							//射撃
							else if (this->m_Input.GetPADSPress(Controls::PADS::SHOT)) {
								if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
									if (this->m_Input.GetPADSTrigger(Controls::PADS::SHOT)) {
										SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Trigger))->Play3D(GetEyePositionCache(), Scale3DRate * 5.0f);
									}
								}
								GetGunPtrNow()->SetShotStart();
							}
							//グレネード構え
							else if (GetGunPtr(2) && GetGunPtr(2)->GetAmmoNumTotal() > 0 && this->m_Input.GetPADSPress(Controls::PADS::THROW)) {
								{
									bool isEmergency = (GetGunPtr(0)->GetGunAnime() == GunAnimeID::EmergencyReady);
									if (isEmergency) {
										GetGunPtr(0)->SetGunAnime(GunAnimeID::LowReady);
									}
								}
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
						//投げ武器ではない最初の武器に切り替え
						this->m_GunPtrControl.GunChangeThrowWeapon(false);
					}
					break;
				default:
					break;
				}
			}
			else if (IsWearingArmor()) {
				m_WearArmorTime = std::clamp(m_WearArmorTime + DXLib_refParts->GetDeltaTime(), 0.f, 1.f);

				float Per = m_WearArmorTime;
				if (0.f <= Per && Per <= 0.3f) {
					m_WearArmorPer = Lerp(0.0f, 0.3f, std::clamp((Per - 0.f) / (0.3f - 0.f), 0.f, 1.f))
						;
				}
				else if (Per <= 0.5f) {
					m_WearArmorPer = Lerp(0.3f, 0.4f, std::clamp((Per - 0.3f) / (0.5f - 0.3f), 0.f, 1.f));
				}
				else if (Per <= 1.f) {
					m_WearArmorPer = Lerp(0.4f, 1.f, std::clamp((Per - 0.5f) / (1.f - 0.5f), 0.f, 1.f));
				}

				auto& Armor = (m_WearArmorFlag == 1) ? PlayerMngr->GetHelmet() : PlayerMngr->GetArmor();

				Armor->SetActive(true);

				if (m_WearArmorTime >= 1.f) {
					m_WearArmorTime = 0.f;

					m_WearArmorFlag = InvalidID;
					Armor->SetActive(false);
					//オン
					this->m_GunPtrControl.SetOnOff(true);
					this->m_GunPtrControl.InvokeReserveGunSelect();

					if (GetGunPtrNow()) {
						GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
					}

					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetEyePositionCache(), Scale3DRate * 3.0f);
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
				this->m_MoveControl.GetVecPower() > 0.1f, (IsMoveFront() ? this->m_MoveControl.GoFrontRad() : 0.0f) + (IsMoveBack() ? this->m_MoveControl.GoBackRad() : 0.0f));
			//リーン
			if (this->m_LeanControl.Update(this->m_Input.GetPADSTrigger(Controls::PADS::LEAN_L), this->m_Input.GetPADSTrigger(Controls::PADS::LEAN_R))) {
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::StandupFoot))->Play3D(GetEyePositionCache(), Scale3DRate * 3.0f);
			}
			if (GetGunPtrNow()) {
				GetGunPtrNow()->CalcSwitchPer(this->m_LeanControl.GetRate() >= 0 || !GetGunPtrNow()->IsCanShot());
			}
			//下半身
			this->m_BottomAnimSelect = GetBottomStandAnimSelect();
			if (IsMoveLeft()) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSelect(); }
			if (IsMoveRight()) { this->m_BottomAnimSelect = GetBottomRightStepAnimSelect(); }
			if (IsMoveBack()) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSelect(); }
			if (IsMoveFront()) { this->m_BottomAnimSelect = GetBottomWalkAnimSelect(); }
			Easing(&this->m_AnimPerBuf[static_cast<int>(GetBottomTurnAnimSelect())], (!this->m_IsSquat && this->m_RotateControl.IsTurnBody()) ? 1.0f : 0.0f, 0.8f, EasingType::OutExpo);
			Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Rappelling)], 0.0f, 0.8f, EasingType::OutExpo);
			for (int loop = 0; loop < static_cast<int>(CharaAnimeID::AnimeIDMax); ++loop) {
				CharaAnimeID ID = static_cast<CharaAnimeID>(loop);
				if (
					ID == CharaAnimeID::Bottom_Stand ||
					ID == CharaAnimeID::Bottom_Stand_Run ||
					ID == CharaAnimeID::Bottom_Stand_Walk ||
					ID == CharaAnimeID::Bottom_Stand_LeftStep ||
					ID == CharaAnimeID::Bottom_Stand_RightStep ||
					ID == CharaAnimeID::Bottom_Stand_WalkBack) {
					this->m_AnimPerBuf[loop] = std::clamp(this->m_AnimPerBuf[loop] + ((ID == this->m_BottomAnimSelect) ? 6.0f : -2.0f) * DXLib_refParts->GetDeltaTime(), 0.0f, 1.0f);
				}
				if (
					ID == CharaAnimeID::Bottom_Squat ||
					ID == CharaAnimeID::Bottom_Squat_Walk ||
					ID == CharaAnimeID::Bottom_Squat_LeftStep ||
					ID == CharaAnimeID::Bottom_Squat_RightStep ||
					ID == CharaAnimeID::Bottom_Squat_WalkBack) {
					this->m_AnimPerBuf[loop] = std::clamp(this->m_AnimPerBuf[loop] + ((ID == this->m_BottomAnimSelect) ? 2.0f : -2.0f) * DXLib_refParts->GetDeltaTime(), 0.0f, 1.0f);
				}
			}
			if (this->m_IsRappelling) {
				for (int loop = 0; loop < static_cast<int>(CharaAnimeID::AnimeIDMax); ++loop) {
					CharaAnimeID ID = static_cast<CharaAnimeID>(loop);
					if (ID == CharaAnimeID::Rappelling) {
						this->m_AnimPerBuf[loop] = 1.0f;
					}
					else {
						this->m_AnimPerBuf[loop] = 0.0f;
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
						if (GetGunPtrNow()) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
						}
					}
				}
			}

			//足音
			if (this->m_BottomAnimSelect != GetBottomStandAnimSelect()) {
				auto Time = GetObj().GetAnim(static_cast<int>(this->m_BottomAnimSelect)).GetTime();
				//L
				if ((9.0f < Time && Time < 10.0f)) {
					if (this->m_CharaSound != 0) {
						this->m_CharaSound = 0;
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale3DRate * 5.0f);
					}
				}
				//R
				if ((27.0f < Time && Time < 28.0f)) {
					if (this->m_CharaSound != 1) {
						this->m_CharaSound = 1;
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale3DRate * 5.0f);
					}
				}
			}
			else {
				this->m_CharaSound = InvalidID;
			}
		}
		//SetMat指示更新
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
					if (this->m_HPRec >= 0.0f) {
						this->m_HPRec -= 2.0f;
						Heal(5);
						if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing))->Play(DX_PLAYTYPE_BACK);
						}
					}
					this->m_HPLowSwitch = true;
				}
				else if (this->m_HP.GetPoint() < this->m_HP.GetMax()) {
					this->m_HPRec += DXLib_refParts->GetDeltaTime();
					if (this->m_HPRec >= 0.0f) {
						this->m_HPRec -= 2.0f;
						Heal(2);
					}
				}
				else{
					this->m_HPRec = 0.0f;
				}
				//
				if (!IsLowHP()) {
					if (this->m_HPLowSwitch) {
						this->m_HPLowSwitch = false;
						if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathing))->StopAll();
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Man_breathend))->Play(DX_PLAYTYPE_BACK);
						}
					}
				}
			}
			float SwitchPer = 1.0f;
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
					Matrix3x3DX::RotAxis(Vector3DX::right(), -this->m_RotateControl.GetRad().x / 2.0f) *
					CharaLocalRotationCache
					).Get44DX() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper2)),
				(
					Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(32 * SwitchPer)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-35)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_RotateControl.GetRad().x / 2.0f) *
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
			this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Upper_Ready)] = 1.0f;
			//指演算
			if (GetGunPtrNow()) {
				auto& FingerPer = GetGunPtrNow()->GetGunAnimeNow().GetFingerPer();
				Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Right_Thumb)], FingerPer.GetFingerPer(0, 0), 0.8f, EasingType::OutExpo);
				if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::Shot) {//撃つときはそちらを参照する
					Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Right_Point)], 0.4f, 0.5f, EasingType::OutExpo);
				}
				else {
					Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Right_Point)], FingerPer.GetFingerPer(0, 1), 0.8f, EasingType::OutExpo);
				}
				Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Right_middle)], FingerPer.GetFingerPer(0, 2), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Right_Ring)], FingerPer.GetFingerPer(0, 3), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Right_Pinky)], FingerPer.GetFingerPer(0, 4), 0.8f, EasingType::OutExpo);
				//
				Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Left_Thumb)], FingerPer.GetFingerPer(1, 0), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Left_Point)], FingerPer.GetFingerPer(1, 1), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Left_middle)], FingerPer.GetFingerPer(1, 2), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Left_Ring)], FingerPer.GetFingerPer(1, 3), 0.8f, EasingType::OutExpo);
				Easing(&this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Left_Pinky)], FingerPer.GetFingerPer(1, 4), 0.8f, EasingType::OutExpo);
			}
			//下半身アニメ演算
			if (IsDraw(0) || IsDraw(1) || IsDraw(2)) {
				if (this->m_AnimPerBuf[static_cast<int>(GetBottomTurnAnimSelect())] > 0.0f) {
					SetAnimLoop(static_cast<int>(GetBottomTurnAnimSelect()), 0.5f);
				}
				if (this->m_AnimPerBuf[static_cast<int>(CharaAnimeID::Bottom_Stand_Run)] > 0.0f) {
					SetAnimLoop(static_cast<int>(CharaAnimeID::Bottom_Stand_Run), GetSpeed() * 0.5f);
				}
				float AnimSpeed = 1.15f * std::clamp(GetSpeed() / 0.65f, 0.5f, 1.0f);
				if (this->m_AnimPerBuf[static_cast<int>(GetBottomWalkAnimSelect())] > 0.0f) {
					SetAnimLoop(static_cast<int>(GetBottomWalkAnimSelect()), this->m_MoveControl.GetVecFront() * AnimSpeed);
				}
				if (this->m_AnimPerBuf[static_cast<int>(GetBottomLeftStepAnimSelect())] > 0.0f) {
					SetAnimLoop(static_cast<int>(GetBottomLeftStepAnimSelect()), this->m_MoveControl.GetVecLeft() * AnimSpeed);
				}
				if (this->m_AnimPerBuf[static_cast<int>(GetBottomWalkBackAnimSelect())] > 0.0f) {
					SetAnimLoop(static_cast<int>(GetBottomWalkBackAnimSelect()), this->m_MoveControl.GetVecRear() * AnimSpeed);
				}
				if (this->m_AnimPerBuf[static_cast<int>(GetBottomRightStepAnimSelect())] > 0.0f) {
					SetAnimLoop(static_cast<int>(GetBottomRightStepAnimSelect()), this->m_MoveControl.GetVecRight() * AnimSpeed);
				}
				//アニメ反映
				for (int loop = 0, max = static_cast<int>(GetObj().GetAnimNum()); loop < max; ++loop) {
					SetObj().SetAnim(loop).SetPer(this->m_AnimPerBuf[loop]);
				}
				SetObj().UpdateAnimAll();
			}
			{
				Vector3DX PosBuf = GetMove().GetPosBuf();
				//素の移動ベクトル
				Vector3DX vec = (IsAlive() && this->m_MoveControl.GetVecPower() > 0.0f) ?
					Matrix3x3DX::Vtrans(this->m_MoveControl.GetVecMove() * GetSpeed() * 60.0f * DXLib_refParts->GetDeltaTime(), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadUpper())) :
					Vector3DX::zero();
				//床判定
				Vector3DX EndPos = PosBuf - Vector3DX::up() * (0.5f * Scale3DRate);
				if (BackGroundParts->CheckLinetoMap(PosBuf + Vector3DX::up() * (0.0f * Scale3DRate), &EndPos) != 0) {
					float GroundHight = EndPos.y - (0.12f * Scale3DRate);
					if ((PosBuf.y - GroundHight) > (0.008f * Scale3DRate / DXLib_refParts->GetDeltaTime())) {
						PosBuf.y = GroundHight;//高所落下の際は即時反映
					}
					else {
						Easing(&PosBuf.y, GroundHight, 0.6f, EasingType::OutExpo);//それ以外は即時反映
					}
					vec.y = 0.0f;
					if (this->m_IsRappelling) {
						this->m_IsRappellingEnd = true;

						this->m_GunPtrControl.SetOnOff(true);
						this->m_GunPtrControl.InvokeReserveGunSelect();
						if (GetGunPtrNow()) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
						}
					}
				}
				else {
					if (!GetIsRappelling()) {
						vec.y = (GetMove().GetVec().y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps())));
					}
					else if (GetMove().GetPos().y < -50.f * Scale3DRate && IsAlive()) {
						SetDamage(GetMyPlayerID(), 1000, static_cast<int>(Charas::HitType::Body), GetMove().GetPos(), GetMove().GetPos());
					}
				}
				//床判定を加味した移動ベクトル
				SetMove().SetVec(vec);
				PosBuf += GetMove().GetVec();
				//壁判定
				if (GetMove().GetVec().sqrMagnitude() > 0.01f * 0.01f) {
					
					BackGroundParts->CheckMapWall(GetMove().GetRePos(), &PosBuf, Vector3DX::up() * (0.6f * Scale3DRate + 0.1f), Vector3DX::up() * ((this->GetIsSquat() ? 0.5f : 1.6f) * Scale3DRate), 0.7f * Scale3DRate);
				}
				//ほかプレイヤーとの判定
				{
					float Radius = 2.0f * 0.5f * Scale3DRate;
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						if (loop == GetMyPlayerID()) { continue; }
						auto& chara = PlayerMngr->GetPlayer(loop)->GetChara();
						if (!chara->IsAlive()) { continue; }
						if (chara->GetIsRappelling()) { continue; }
						//自分が当たったら押し戻す
						Vector3DX Vec = (chara->GetMove().GetPos() - GetMove().GetPos()); Vec.y = (0.0f);
						float Len = Vec.magnitude();
						if (Len < Radius) {
							PosBuf = PosBuf + Vec.normalized() * (Len - Radius);
						}
					}
				}
				//ベースに反映
				SetMove().SetPos(PosBuf);
				//printfDx("[%5.2f,%5.2f]\n", PosBuf.x, PosBuf.z);
				SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadBottom()));
				//座標オーバーライド
				if (this->m_MoveOverRideFlag) {
					this->m_MoveOverRideFlag = false;
					SetMove().SetPos(this->m_OverRideInfo.pos);
					SetMove().SetMat(this->m_OverRideInfo.mat);
					SetMove().SetVec(this->m_OverRideInfo.vec);
					this->m_RotateControl.SetRad(this->m_OverRideInfo.WatchRad);
				}
				SetMove().Update(0.8f, 0.0f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			}
			if (IsAlive()) {
				Matrix3x3DX CharaRotationCache = CharaLocalRotationCache * GetMove().GetMat();
				//銃の位置を指定するアニメ

				bool isEmergency = (GetGunPtr(0)->GetGunAnime() == GunAnimeID::EmergencyReady);

				for (int loop = 0, max = this->m_GunPtrControl.GetGunNum(); loop < max; ++loop) {
					auto pGun = this->m_GunPtrControl.GetGunPtr(loop);
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
								GetFrameWorldMat(CharaFrame::Neck).pos() +
								GetFrameWorldMat(CharaFrame::Neck).zvec2() * (0.15f * Scale3DRate) +
								GetMove().GetMat().yvec() * (0.2f * Scale3DRate) +
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

					bool IsSelect = (loop == this->m_GunPtrControl.GetNowGunSelect());
					bool IsActive = IsSelect || ((loop == 0) && isEmergency);
					if (IsSelect) {
						pGun->SetMagazinePoachMat(GetFrameWorldMat(CharaFrame::LeftMag));
						pGun->SetGrenadeThrowRot(GetEyeRotationCache());
						//アニメーション
						pGun->UpdateGunAnimePer(GetIsADS());
					}
					else if (IsActive) {
						pGun->SetMagazinePoachMat(GetFrameWorldMat(CharaFrame::LeftMag));
						pGun->SetGrenadeThrowRot(GetEyeRotationCache());
						//アニメーション
						pGun->UpdateGunAnimePer(false);
					}
					else {
						pGun->SetActiveAll(true);
						//アニメーション
						pGun->InitGunAnimePer();
					}
					bool isActiveAutoAim = GetIsADS();
					if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
						if (!OptionParts->GetParamBoolean(EnumSaveParam::ActiveLockOn)) {
							isActiveAutoAim = false;
						}
					}
					pGun->UpdateGunMat(IsActive, isActiveAutoAim && IsSelect, CharaRotationCache, GetFrameWorldMat(CharaFrame::Head).pos(), this->m_RotateControl.GetRad());
				}
				//手の位置を制御
				if ((GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) || GetCanLookByPlayer()) {
					if (!GetGunPtrNow() && IsWearingArmor()) {
						float Per = m_WearArmorPer;

						float Rad{ 0.f };
						Vector3DX Pos1;
						Vector3DX Pos2;
						Vector3DX Pos3;

						auto& Armor = (m_WearArmorFlag == 1) ? PlayerMngr->GetHelmet() : PlayerMngr->GetArmor();

						if (m_WearArmorFlag == 1) {
							Rad = -50.f;
							Pos1 = Vector3DX::vget(-0.3f, 0.0f, -0.3f) * Scale3DRate;
							Pos2 = Vector3DX::vget(0.f, 0.1f, -0.4f) * Scale3DRate;
							Pos3 = Vector3DX::vget(0.f, 0.f, 0.0f) * Scale3DRate;
						}
						else {
							Rad = 30.f;
							Pos1 = Vector3DX::vget(-0.3f, 0.2f, -0.25f) * Scale3DRate;
							Pos2 = Vector3DX::vget(0.f, -0.2f, -0.4f) * Scale3DRate;
							Pos3 = Vector3DX::vget(0.f, -0.5f, 0.0f) * Scale3DRate;
						}

						Vector3DX Pos = Lerp(
							Lerp(Pos1, Pos2, std::clamp(Per / 0.5f, 0.f, 1.f)),
							Pos3,
							std::clamp((Per - 0.5f) / (1.f - 0.5f), 0.f, 1.f));
						Matrix3x3DX Rot = Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(Lerp(Rad, 0.f, Per)));

						Armor->SetMove().SetPos(
							GetFrameWorldMat(CharaFrame::Head).pos() + Matrix3x3DX::Vtrans(Pos, CharaRotationCache));
						Armor->SetMove().SetMat(Rot * CharaRotationCache);
						Armor->SetMove().Update(0.f, 0.f);
						Armor->UpdateObjMatrix(Armor->GetMove().GetMat(), Armor->GetMove().GetPos());

						IK_RightArm(
							&SetObj(),
							GetFrame(static_cast<int>(CharaFrame::RightArm)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
							GetFrame(static_cast<int>(CharaFrame::RightArm2)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
							GetFrame(static_cast<int>(CharaFrame::RightWrist)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
							Armor->GetRightHandMat());

						IK_LeftArm(
							&SetObj(),
							GetFrame(static_cast<int>(CharaFrame::LeftArm)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)),
							GetFrame(static_cast<int>(CharaFrame::LeftArm2)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)),
							GetFrame(static_cast<int>(CharaFrame::LeftWrist)),
							GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)),
							Armor->GetLeftHandMat());
					}
					else if (GetIsRappelling()) {
						SetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm)));
						SetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm2)));
						SetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightWrist)));

						SetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm)));
						SetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm2)));
						SetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftWrist)));
					}
					else {
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
							if (isEmergency) {
								IK_LeftArm(
									&SetObj(),
									GetFrame(static_cast<int>(CharaFrame::LeftArm)),
									GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)),
									GetFrame(static_cast<int>(CharaFrame::LeftArm2)),
									GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)),
									GetFrame(static_cast<int>(CharaFrame::LeftWrist)),
									GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)),
									GetGunPtr(0)->GetLeftHandMat());
							}
							else {
								Matrix4x4DX HandMat;
								HandMat = GetGunPtrNow()->GetLeftHandMat();
								Easing(&this->m_ArmBreakPer, (
									(GetGunPtrNow()->IsCanShot() && this->m_ArmBreak)
									|| (GetGunPtrNow()->GetGunAnime() == GunAnimeID::Watch)//TODO:専用の左腕アクション
									|| (GetGunPtrNow()->GetModifySlot()->GetMyData()->GetIsThrowWeapon() && (GetGunPtrNow()->GetGunAnime() != GunAnimeID::ThrowReady))//TODO:専用の左腕アクション
									) ? 1.0f : 0.0f, 0.9f, EasingType::OutExpo);
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
									HandMat = HandMat.rotation() * Matrix4x4DX::Mtrans(HandMat.pos() + Vector3DX::vget(GetRandf(1.0f), GetRandf(1.0f), GetRandf(1.0f)) * (0.002f * Scale3DRate));
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
				}
				//ヒットボックス
				this->m_HitBoxControl.Update(this, (GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.0f);
				//目の座標取得
				{
					bool HeadBobbing = ((GetMyPlayerID() != PlayerMngr->GetWatchPlayerID()) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
					this->m_EyePositionCache = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.0f;
					if (HeadBobbing) {
						this->m_EyePositionCache += this->m_WalkSwingControl.CalcEye(CharaRotationCache, std::clamp(std::clamp(this->m_MoveControl.GetVecPower(), 0.0f, 1.0f) * GetSpeed() / 0.625f, 0.0f, 0.85f) / 0.65f, 5.0f);
					}
					this->m_EyeRotationCache = Matrix3x3DX::identity();
					if (HeadBobbing) {
						this->m_EyeRotationCache = this->m_WalkSwingControl.CalcWalk(GetEyePositionCache() - GetMove().GetPos(), std::clamp(GetMove().GetVec().magnitude() / 2.0f, 0.0f, 0.5f));
					}
					this->m_EyeRotationCache *=
						Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_LeanControl.GetRad() / 5.0f) *
						Matrix3x3DX::RotAxis(Vector3DX::right(), this->m_RotateControl.GetRad().x) *
						Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetYRadBottomChange()) *
						GetMove().GetMat();
				}
			}
			else {
				for (int loop = 0, max = this->m_GunPtrControl.GetGunNum(); loop < max; ++loop) {
					auto pGun = this->m_GunPtrControl.GetGunPtr(loop);
					if (!pGun) { continue; }
					if (loop == this->m_GunPtrControl.GetNowGunSelect()) {
						if (this->m_GunFallActive) {
							Matrix3x3DX Mat = pGun->GetMove().GetMat();
							Vector3DX Pos = pGun->GetMove().GetPos() + Vector3DX::up() * m_GunyAdd;
							this->m_GunyAdd += (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));
							if (BackGroundParts->CheckLinetoMap(pGun->GetMove().GetPos(), &Pos) != 0) {
								this->m_GunFallActive = false;
								this->m_GunyAdd = 0.f;
								Mat *= Matrix3x3DX::RotVec2(Mat.xvec(), Vector3DX::up());
								Pos + Vector3DX::up() * (0.1f * Scale3DRate);
							}
							else {
								Mat = Matrix3x3DX::RotAxis(Vector3DX::vget(GetRandf(1.f), 1.f, GetRandf(1.f)).normalized(), deg2rad(180.f) * DXLib_refParts->GetDeltaTime()) * Mat;
							}
							pGun->SetGunMat(Mat, Pos);
							pGun->SetActiveAll(true);
						}
					}
					else {
						pGun->SetActiveAll(false);
					}
				}
				//目の座標取得
				if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
					auto Mat1 = this->m_RagDollControl.GetFrameMat(RagFrame::LeftEye);
					auto Mat2 = this->m_RagDollControl.GetFrameMat(RagFrame::RightEye);
					this->m_EyePositionCache = (Mat1.pos() + Mat2.pos()) / 2.0f;

					this->m_EyeRotationCache = Matrix3x3DX::Get33DX(Mat1);
				}
			}
			//ラグドール
			{
				Vector3DX StartPos = GetMove().GetPosBuf();
				if (!IsAlive()) {
					Vector3DX EndPos = StartPos - Vector3DX::up() * (20.0f * Scale3DRate);
					if (BackGroundParts->CheckLinetoMap(StartPos, &EndPos) != 0) {
						StartPos = EndPos;
					}
				}
				this->m_RagDollControl.Update(IsAlive(), StartPos);
			}
			//視認判定系
			this->m_IsActiveCameraPos = false;
			if (GetMyPlayerID() != PlayerMngr->GetWatchPlayerID()) {
				auto& ViewChara = PlayerMngr->GetWatchPlayer()->GetChara();
				Vector3DX EndPos = GetEyePositionCache();
				this->m_CanLookTarget = BackGroundParts->CheckLinetoMap(ViewChara->GetEyePositionCache(), &EndPos) == 0;
				if (this->m_CanLookTarget) {
					this->m_CanLookTargetTimer = std::max(this->m_CanLookTargetTimer + DXLib_refParts->GetDeltaTime(), 0.f);
				}
				else {
					this->m_CanLookTargetTimer = 0.f;
				}
				this->m_Length = (GetEyePositionCache() - ViewChara->GetEyePositionCache()).magnitude();
			}
			//コンカッション
			if (GetMyPlayerID() == PlayerMngr->GetWatchPlayerID()) {
				if (this->m_ConcussionSwitch) {
					this->m_ConcussionSwitch = false;
					this->m_Concussion = 1.0f;
				}
				PostPassParts->Set_is_Blackout(this->m_Concussion > 0.0f);
				if (this->m_Concussion == 1.0f) {
					Camera3D::Instance()->SetCamShake(0.5f, 0.01f * Scale3DRate);
				}
				if (this->m_Concussion > 0.9f) {
					Easing(&this->m_ConcussionPer, 1.0f, 0.1f, EasingType::OutExpo);
				}
				else if (this->m_Concussion > 0.25f) {
					if (this->m_ConcussionPer > 0.25f) {
						Easing(&this->m_ConcussionPer, 0.0f, 0.8f, EasingType::OutExpo);
					}
					else {
						this->m_ConcussionPer = 0.25f;
					}
				}
				else {
					Easing(&this->m_ConcussionPer, 0.0f, 0.8f, EasingType::OutExpo);
				}
				PostPassParts->Set_Per_Blackout(this->m_ConcussionPer * 2.0f);
				this->m_Concussion = std::max(this->m_Concussion - DXLib_refParts->GetDeltaTime(), 0.0f);
			}
		}
		//
		void			CharacterObj::LoadChara(const std::string& FolderName, PlayerID ID) noexcept {
			auto& player = Player::PlayerManager::Instance()->GetPlayer(ID);

			std::string Path = "data/Charactor/";
			Path += FolderName;
			Path += "/";
			player->SetChara(std::make_shared<CharacterObj>());
			ObjectManager::Instance()->InitObject(player->GetChara(), Path);
			player->SetAI(std::make_shared<AIs::AIControl>(ID));
		}
		void			CharacterObj::LoadCharaGun(const std::string& FolderName, int Select) noexcept {
			std::string Path = "data/gun/";
			Path += FolderName;
			Path += "/";
			this->m_GunPtrControl.SetGunPtr(Select, std::make_shared<Guns::GunObj>());
			ObjectManager::Instance()->InitObject(this->m_GunPtrControl.GetGunPtr(Select), Path);
		}
		void CharacterObj::Spawn(float pxRad, float pyRad, const Vector3DX& pPos, int GunSelect, bool CheckGround) noexcept {
			auto& player = Player::PlayerManager::Instance()->GetPlayer(this->GetMyPlayerID());
			this->m_HP.Init();
			this->m_BodyPoint.Init();
			this->m_HeadPoint.Init();
			Heal(100);
			this->m_ArmBreak = false;
			this->m_ArmBreakPer = 0.0f;
			this->m_SlingArmZrad.Init(0.08f * Scale3DRate, 3.0f, deg2rad(50));
			this->m_HPRec = 0.0f;

			this->m_MoveOverRideFlag = false;
			this->m_Input.ResetAllInput();
			this->m_RotateControl.Init(pxRad, pyRad);
			this->m_MoveControl.Init();
			this->m_LeanControl.Init();
			for (auto& per : this->m_AnimPerBuf) { per = 0.0f; }
			this->m_IsSquat = false;
			Vector3DX posBuf = pPos;
			if (CheckGround) {
				Vector3DX EndPos = posBuf - Vector3DX::up() * 50.0f * Scale3DRate;
				if (BackGround::BackGroundControl::Instance()->CheckLinetoMap(posBuf + Vector3DX::up() * 10.0f * Scale3DRate, &EndPos) != 0) {
					posBuf = EndPos;
				}
			}
			SetMove().SetAll(posBuf, posBuf, posBuf, Vector3DX::zero(), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetRad().y), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetRad().y));
			//
			this->m_GunPtrControl.SelectGun(GunSelect);
			for (int loop = 0, max = this->m_GunPtrControl.GetGunNum(); loop < max; ++loop) {
				if (!this->m_GunPtrControl.GetGunPtr(loop)) { continue; }
				this->m_GunPtrControl.GetGunPtr(loop)->Spawn();
			}
			this->m_SlingZrad.Init(0.05f * Scale3DRate, 3.0f, deg2rad(50));
			this->m_GunyAdd = 0.f;
			this->m_GunFallActive = true;

			player->InitInventory();
		}
		//
		void			CharacterObj::Init_Sub(void) noexcept {
			this->m_HitBoxControl.Init();
			this->m_ArmBreak = false;
			SetMinAABB(Vector3DX::vget(2.0f, 0.0f, 2.0f) * -Scale3DRate);
			SetMaxAABB(Vector3DX::vget(2.0f, 2.0f, 2.0f) * Scale3DRate);

			m_ItemList_EnableSpawnBySoldier.clear();
			for (auto& item : Objects::ItemObjDataManager::Instance()->GetList()) {
				if (item->EnableSpawnBySoldier()) {
					m_ItemList_EnableSpawnBySoldier.emplace_back(static_cast<int>(&item - &Objects::ItemObjDataManager::Instance()->GetList().front()));
				}
			}
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
			GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
			SetFogColor(0, 0, 0);
			//キャラ描画
			//MV1SetMaterialTypeAll(GetObj().GetHandle(), DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
			if (IsAlive()) {
				for (int loop = 0, max = GetObj().GetMeshNum(); loop < max; ++loop) {
					if (GetObj().GetMeshSemiTransState(loop) == isDrawSemiTrans) {
						GetObj().DrawMesh(loop);
					}
				}
			}
			else {
				for (int loop = 0, max = this->m_RagDollControl.GetRagDoll().GetMeshNum(); loop < max; ++loop) {
					if (this->m_RagDollControl.GetRagDoll().GetMeshSemiTransState(loop) == isDrawSemiTrans) {
						this->m_RagDollControl.GetRagDoll().DrawMesh(loop);
					}
				}
			}
			//hitbox描画
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
				this->m_RagDollControl.GetRagDoll().DrawModel();
			}
		}
	}
}
