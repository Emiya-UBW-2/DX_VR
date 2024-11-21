#include	"Character.hpp"

#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		void			CharacterClass::PlayVoice(void) const noexcept {
			auto* SE = SoundPool::Instance();
			switch (m_ArmMoveClass.GetCharaAction()) {
			case EnumArmAnimType::Ready:
				break;
			case EnumArmAnimType::Run:
				break;
			case EnumArmAnimType::Men:
			case EnumArmAnimType::HikiMen:
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Men))->Play3D(GetEyePosition(), Scale3DRate * 35.f);
				break;
			case EnumArmAnimType::Kote:
			case EnumArmAnimType::HikiKote:
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Kote))->Play3D(GetEyePosition(), Scale3DRate * 35.f);
				break;
			case EnumArmAnimType::Dou:
			case EnumArmAnimType::HikiDou:
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Dou))->Play3D(GetEyePosition(), Scale3DRate * 35.f);
				break;
			case EnumArmAnimType::Tsuki:
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Tsuki))->Play3D(GetEyePosition(), Scale3DRate * 35.f);
				break;
			case EnumArmAnimType::Tsuba:
				break;
			case EnumArmAnimType::GuardSuriage:
				break;
			case EnumArmAnimType::Max:
				break;
			default:
				break;
			}
		}
		void			CharacterClass::StopVoice(void) const noexcept {
			auto* SE = SoundPool::Instance();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Men))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Kote))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Dou))->StopAll();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Tsuki))->StopAll();
		}

		void			CharacterClass::CheckTsuba(void) noexcept {
			bool IsGuard = false;


			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Target = PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();

			auto TgtPos = Target->GetFramePosition(CharaFrame::LeftWrist);
			auto MyPos = this->GetFramePosition(CharaFrame::LeftWrist);

			float Len = 0.f;
			auto Vec = (TgtPos - MyPos); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();

			auto Dir = m_CharaMove.GetEyeMatrix().zvec() * -1.f; Dir.y = (0.f); Dir = Dir.normalized();

			auto IsFront = ((Vector3DX::Dot(Dir, Vec)) > 0.f);
			auto cross = Vector3DX::Cross(Dir, Vec).y;

			float Radius = (0.15f + 0.15f) * Scale3DRate;
			if (Len < Radius) {
				if (IsFront) {
					if (abs(cross) < 0.4f) {
						IsGuard = true;
					}
				}
			}

			if (IsGuard) {
				OverrideTsuba();
				//相手もつばぜり合いにする
				Target->OverrideTsuba();
			}
		}
		void			CharacterClass::SetFumikomi(void) noexcept {
			this->GetObj().SetAnim(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Attack)).GoStart();

			this->m_CharaSound = -1;
			auto* SE = SoundPool::Instance();
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Swing))->Play3D(GetFramePosition(CharaFrame::RightWrist), Scale3DRate * 5.f);
		}
		//
		void			CharacterClass::OverrideAction(void) noexcept {
			this->m_Weapon_Ptr->ResetAnim();
			//this->m_Weapon_Ptr->SetAnim();
			switch (m_ArmMoveClass.GetCharaAction()) {
			case EnumArmAnimType::Ready:
				OverrideReady();
				break;
			case EnumArmAnimType::Run:
				OverrideRun();
				break;
			case EnumArmAnimType::Men:
			case EnumArmAnimType::Kote:
			case EnumArmAnimType::Tsuki:
				OverrideFrontAttack();
				break;
			case EnumArmAnimType::Dou:
				OverrideDo();
				break;
			case EnumArmAnimType::Tsuba:
				OverrideTsuba();
				break;
			case EnumArmAnimType::HikiMen:
			case EnumArmAnimType::HikiKote:
			case EnumArmAnimType::HikiDou:
				OverrideBackAttack();
				break;
			case EnumArmAnimType::GuardSuriage:
				OverrideGuard();
				break;
			case EnumArmAnimType::Max:
				break;
			default:
				break;
			}
		}
		void			CharacterClass::OverrideReady(void) noexcept {
			m_ArmMoveClass.ChangeAction(EnumArmAnimType::Ready);
			m_CharaMove.SetIsFrontAttacking(false);
			m_CharaMove.SetIsDouAttacking(false);
			m_CharaMove.SetIsBackAttacking(false);
			m_CharaMove.SetIsRunning(false);
		}
		void			CharacterClass::OverrideRun(void) noexcept {
			m_RunTime = 0.f;
			m_CharaMove.SetIsFrontAttacking(false);
			m_CharaMove.SetIsDouAttacking(false);
			m_CharaMove.SetIsBackAttacking(false);
			m_CharaMove.SetIsRunning(true);
		}
		void			CharacterClass::OverrideFrontAttack(void) noexcept {
			m_FrontAttackActionTime = this->m_Weapon_Ptr->GetArmAnimeTotalTime(m_ArmMoveClass.GetCharaAction());
			m_CharaMove.SetIsFrontAttacking(true);
			m_CharaMove.SetIsDouAttacking(false);
			m_CharaMove.SetIsBackAttacking(false);
			SetFumikomi();
			StopVoice();
			PlayVoice();
			m_BambooVecBase.Set(0.f, 0.f);
		}
		void			CharacterClass::OverrideDo(void) noexcept {
			m_FrontAttackActionTime = this->m_Weapon_Ptr->GetArmAnimeTotalTime(m_ArmMoveClass.GetCharaAction());
			m_CharaMove.SetIsFrontAttacking(false);
			m_CharaMove.SetIsDouAttacking(true);
			m_CharaMove.SetIsBackAttacking(false);
			SetFumikomi();
			StopVoice();
			PlayVoice();
			m_BambooVecBase.Set(0.f, 0.f);
		}
		void			CharacterClass::OverrideTsuba(void) noexcept {
			m_TsubaCoolDown = 0.5f;
			m_ArmMoveClass.ChangeAction(EnumArmAnimType::Tsuba);
			m_CharaMove.SetIsFrontAttacking(false);
			m_CharaMove.SetIsDouAttacking(false);
			m_CharaMove.SetIsBackAttacking(false);
			m_CharaMove.SetIsRunning(false);
		}
		void			CharacterClass::OverrideBackAttack(void) noexcept {
			m_BackAttackActionTime = std::max(0.75f, this->m_Weapon_Ptr->GetArmAnimeTotalTime(m_ArmMoveClass.GetCharaAction()));
			m_CharaMove.SetIsFrontAttacking(false);
			m_CharaMove.SetIsDouAttacking(true);
			m_CharaMove.SetIsBackAttacking(true);
			SetFumikomi();
			StopVoice();
			PlayVoice();

			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Target = PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetChara();
			Target->OverrideReady();
			m_BambooVecBase.Set(0.f, 0.f);
		}
		void			CharacterClass::OverrideGuard(void) noexcept {
			m_BambooVecBase = Vector2DX::zero();
			m_GuardTimer = 1.f;
		}
		//
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value, bool IsTraining) noexcept {
			auto* SE = SoundPool::Instance();
			auto* SideLogParts = SideLog::Instance();
			auto* HitMarkParts = HitMark::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			if (this->m_MyID == value.DamageID) {
				if (m_DamageCoolTime == 0.f) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Hit))->Play3D(GetFramePosition(CharaFrame::RightWrist), Scale3DRate * 15.f);
					m_DamageCoolTime = static_cast<float>(SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Hit))->GetTotalTIme()) / 1000.f;

					{
						int HitDamagePow = value.Damage;
						switch (value.m_WazaType) {
						case FPS_n2::WazaType::Men:
							HitDamagePow = HitDamagePow * 85 / 100;
							break;
						case FPS_n2::WazaType::SuriageMen:
							HitDamagePow = HitDamagePow * 90 / 100;
							break;
						case FPS_n2::WazaType::Hikimen:
							HitDamagePow = HitDamagePow * 150 / 100;
							break;
						case FPS_n2::WazaType::Kote:
							HitDamagePow = HitDamagePow * 450 / 100;
							break;
						case FPS_n2::WazaType::Hikigote:
							HitDamagePow = HitDamagePow * 450 / 100;
							break;
						case FPS_n2::WazaType::Dou:
							HitDamagePow = HitDamagePow * 400 / 100;
							break;
						default:
							break;
						}


						int HitPosPoints = 10;
						{
							float MinPoint = 0.5f;
							if (value.HitPer < MinPoint) {
								HitPosPoints -= static_cast<int>(Lerp(0.f, 20.f, (MinPoint - value.HitPer) / (MinPoint - 0.f)));
							}
							float MaxPoint = 0.9f;
							if (MaxPoint < value.HitPer) {
								HitPosPoints -= static_cast<int>(Lerp(0.f, 20.f, (value.HitPer - MaxPoint) / (1.f - MaxPoint)));
							}
						}

						int KihakuPoints = static_cast<int>(Lerp(-30.f, 20.f, value.KihakuPer));

						int TotalAddHits = 0;
						TotalAddHits += HitDamagePow;
						TotalAddHits += HitPosPoints;
						TotalAddHits += KihakuPoints;

						if (TotalAddHits >= 100) {
							auto* PlayerMngr = Player::PlayerManager::Instance();
							PlayerMngr->AddScore(value.ShotID, value.GetHitType());
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Kendo_GetHit))->Play(DX_PLAYTYPE_BACK, TRUE);
						}
						if (!IsTraining && TotalAddHits >= 50) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Audience_Near))->Play(DX_PLAYTYPE_BACK, TRUE);
						}
						if (this->m_MyID == this->m_ViewID) {
							//コンカッション
							switch (value.GetHitType()) {
							case HitType::Head://面
							case HitType::Body://胴
							case HitType::Arm://小手
								m_ConcussionSwitch = true;
								break;
							case HitType::Leg:
							default:
								break;
							}
							float offset = 0.f;
							switch (value.m_WazaType) {
							case FPS_n2::WazaType::Men:
								SideLogParts->Add(3.0f, offset, White, LocalizeParts->Get(7000)); offset += 0.02f;
								break;
							case FPS_n2::WazaType::SuriageMen:
								SideLogParts->Add(3.0f, offset, White, LocalizeParts->Get(7001)); offset += 0.02f;
								break;
							case FPS_n2::WazaType::Hikimen:
								SideLogParts->Add(3.0f, offset, White, LocalizeParts->Get(7002)); offset += 0.02f;
								break;
							case FPS_n2::WazaType::Kote:
								SideLogParts->Add(3.0f, offset, White, LocalizeParts->Get(7003)); offset += 0.02f;
								break;
							case FPS_n2::WazaType::Hikigote:
								SideLogParts->Add(3.0f, offset, White, LocalizeParts->Get(7004)); offset += 0.02f;
								break;
							case FPS_n2::WazaType::Dou:
								SideLogParts->Add(3.0f, offset, White, LocalizeParts->Get(7005)); offset += 0.02f;
								break;
							default:
								break;
							}
							SideLogParts->Add(3.0f, offset, White, "%s%4d", (TotalAddHits >= 0) ? "+" : "-", std::abs(TotalAddHits));
						}
						else {
							HitMarkParts->Add(value.m_Pos, value.GetHitType(), static_cast<float>(HitDamagePow) / 100.f);

							float offset = 0.f;
							switch (value.m_WazaType) {
							case FPS_n2::WazaType::Men:
								SideLogParts->Add(3.0f, offset, Yellow, LocalizeParts->Get(7000)); offset += 0.02f;
								break;
							case FPS_n2::WazaType::SuriageMen:
								SideLogParts->Add(3.0f, offset, Yellow, LocalizeParts->Get(7001)); offset += 0.02f;
								break;
							case FPS_n2::WazaType::Hikimen:
								SideLogParts->Add(3.0f, offset, Yellow, LocalizeParts->Get(7002)); offset += 0.02f;
								break;
							case FPS_n2::WazaType::Kote:
								SideLogParts->Add(3.0f, offset, Yellow, LocalizeParts->Get(7003)); offset += 0.02f;
								break;
							case FPS_n2::WazaType::Hikigote:
								SideLogParts->Add(3.0f, offset, Yellow, LocalizeParts->Get(7004)); offset += 0.02f;
								break;
							case FPS_n2::WazaType::Dou:
								SideLogParts->Add(3.0f, offset, Yellow, LocalizeParts->Get(7005)); offset += 0.02f;
								break;
							default:
								break;
							}
							SideLogParts->Add(3.0f, offset, (HitDamagePow >= 0) ? Green : Red, "%s%s%4d", LocalizeParts->Get(7010), (HitDamagePow >= 0) ? "+" : "-", std::abs(HitDamagePow)); offset += 0.02f;
							SideLogParts->Add(3.0f, offset, (HitPosPoints >= 0) ? Green : Red, "%s%s%4d", LocalizeParts->Get(7011), (HitPosPoints >= 0) ? "+" : "-", std::abs(HitPosPoints)); offset += 0.02f;
							SideLogParts->Add(3.0f, offset, (KihakuPoints >= 0) ? Green : Red, "%s%s%4d", LocalizeParts->Get(7012), (KihakuPoints >= 0) ? "+" : "-", std::abs(KihakuPoints)); offset += 0.02f;
							SideLogParts->Add(3.0f, offset, (TotalAddHits >= 0) ? Green : Red, "%s%s%4d", LocalizeParts->Get(7013), (TotalAddHits >= 0) ? "+" : "-", std::abs(TotalAddHits)); offset += 0.02f;
							if (TotalAddHits >= 100) {
								SideLogParts->Add(3.5f, offset, Yellow, LocalizeParts->Get(7014)); offset += 0.02f;
							}
							else if (TotalAddHits >= 80) {
								SideLogParts->Add(3.5f, offset, Red, LocalizeParts->Get(7015)); offset += 0.02f;
							}
						}
					}
				}
				//ダメージ
				return true;
			}
			return false;
		}
		//
		bool			CharacterClass::CheckDamageRay(HitPoint Damage, PlayerID AttackID, float Kihaku, WazaType pWazaType, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, GetMove().GetPos()) <= 2.0f * Scale3DRate)) { return false; }
			float BaseLen = (StartPos - *pEndPos).magnitude();
			//被弾処理
			auto* HitPtr = m_HitBoxControl.GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				//ダメージ登録
				{
					//撃った技と違う個所ならダメージと扱わない
					bool IsDamage = true;
					switch (pWazaType) {
					case FPS_n2::WazaType::Men:
						if (HitPtr->GetColType() != HitType::Head) {
							IsDamage = false;
						}
						break;
					case FPS_n2::WazaType::SuriageMen:
						if (HitPtr->GetColType() != HitType::Head) {
							IsDamage = false;
						}
						break;
					case FPS_n2::WazaType::Hikimen:
						if (HitPtr->GetColType() != HitType::Head) {
							IsDamage = false;
						}
						break;
					case FPS_n2::WazaType::Kote:
						if (HitPtr->GetColType() != HitType::Arm) {
							IsDamage = false;
						}
						break;
					case FPS_n2::WazaType::Hikigote:
						if (HitPtr->GetColType() != HitType::Arm) {
							IsDamage = false;
						}
						break;
					case FPS_n2::WazaType::Dou:
						if (HitPtr->GetColType() != HitType::Body) {
							IsDamage = false;
						}
						break;
					default:
						break;
					}
					if (IsDamage) {
						m_Damage.Add(AttackID, this->m_MyID, Damage, Kihaku, (StartPos - *pEndPos).magnitude() / BaseLen, HitPtr->GetColType(), pWazaType, *pEndPos);
					}
				}
				return true;
			}
			return false;
		}
		void			CharacterClass::MovePoint(float pyRad, const Vector3DX& pPos) noexcept {
			m_CharaMove.InitKey(pyRad);
			Matrix3x3DX Mat; Mat.SetRadian(0.f, m_CharaMove.GetRadBuf(), 0.f);
			ResetMove(Mat, pPos);
			OverrideReady();
			m_ArmMoveClass.Init();
			this->m_CharaSound = -1;
			m_StaminaControl.InitStamina();
		}
		void			CharacterClass::Init_Sub(void) noexcept {
			m_BambooVec.Set(0.f, 0.f);
			m_BambooVecBase.Set(0.f, 0.f);
			m_HitBoxControl.InitHitBox();
			this->m_MoveOverRideFlag = false;
#ifdef _USE_EFFEKSEER_
			m_Effect.Init();				//
#endif
		}
		//
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			InputControl Input = pInput;
			//竹刀の回転を反映
			if (!IsAttacking()) {
				Easing(&m_BambooVecBase, Vector2DX::vget(Input.GetxRad(), Input.GetyRad()), 0.7f, EasingType::OutExpo);
			}
			//自動で相手を向く
			float pp_y = 0.f;
			{
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& Target = PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();

				auto Dir = m_CharaMove.GetEyeMatrix().zvec() * -1.f; Dir.y = (0.f); Dir = Dir.normalized();
				float Len = 0.f;
				auto Vec = (Target->GetMove().GetPos() - this->GetEyePosition()); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();

				float sint = Vector3DX::Cross(Vec, Dir).y;
				float cost = Vector3DX::Dot(Vec, Dir);
				auto IsFront = (cost > cos(deg2rad(40)));
				if (!m_CharaMove.GetRun()) {
					if (IsFront) {
						pp_y = std::clamp(-std::atan2f(sint, cost), -deg2rad(60), deg2rad(60)) * 25.f / FrameRate;
					}
					else {
						pp_y = -deg2rad(60) * 9.f / FrameRate;
					}
				}
				else if (m_CharaMove.GetIsFrontAttacking()) {
					if (IsFront) {
						pp_y = std::clamp(-std::atan2f(sint, cost), -deg2rad(60), deg2rad(60)) * 40.f / FrameRate;
					}
				}
			}
			//
			if (!pReady) {
				Input.ResetKeyInput();
			}
			m_CharaMove.InputKey(Input, pp_y);
			m_CharaMove.SetSpeedMul(Lerp(0.5f, 1.f, m_StaminaControl.GetStamina() / m_StaminaControl.GetStaminaMax()));
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				for (int i = 0, num = GetObj().GetMaterialNum(); i < num; ++i) {
					GetObj().SetMaterialDifColor(i, GetColorF(0.8f, 0.8f, 0.8f, 1.f));
					GetObj().SetMaterialAmbColor(i, GetColorF(0.25f, 0.25f, 0.25f, 1.f));
				}
				//this->GetObj().SetMaterialTypeAll(DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
			}
			//
			ExecuteInput();
			m_ArmMoveClass.ExecuteAction();
			ExecuteAnim();
			ExecuteSound();
			ExecuteMatrix();
			m_HitBoxControl.UpdataHitBox(this, 1.f);									//ヒットボックス
#ifdef _USE_EFFEKSEER_
			m_Effect.Execute();				//
#endif
		}
		//
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* SE = SoundPool::Instance();
			//掛け声
			if (
				(m_ArmMoveClass.GetCharaAction() == EnumArmAnimType::Ready || m_ArmMoveClass.GetCharaAction() == EnumArmAnimType::Tsuba) &&
				GetYaTimer() == 0.f &&
				m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::JUMP)
				) {
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Ya))->Play3D(GetEyePosition(), Scale3DRate * 35.f);
				m_StaminaControl.m_YaTimer = GetYaTimerMax();
				m_StaminaControl.m_HeartUp = 40.f;
			}
			m_DamageCoolTime = std::max(m_DamageCoolTime - DXLib_refParts->GetDeltaTime(), 0.f);
			m_TsubaSoundCoolTime = std::max(m_TsubaSoundCoolTime - DXLib_refParts->GetDeltaTime(), 0.f);
			m_GuardHit = std::max(m_GuardHit - DXLib_refParts->GetDeltaTime(), 0.f);
			//
			auto Prev = m_ArmMoveClass.GetCharaAction();
			auto SetAction = m_ArmMoveClass.GetCharaAction();
			switch (m_ArmMoveClass.GetCharaAction()) {
			case EnumArmAnimType::Ready:
			{
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& Target = PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();
				//つばぜり判定
				if(Target->GetCharaAction()==GetCharaAction()){
					CheckTsuba();
				}
				if ((m_GuardCoolDownTimer == 0.f) && m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::AIM)) {
					SetAction = EnumArmAnimType::GuardSuriage;//上ガード
					m_GuardCoolDownTimer = GetGuardCoolDownTimerMax();
				}
				{
					auto Dir = m_CharaMove.GetEyeMatrix().zvec() * -1.f; Dir.y = (0.f); Dir = Dir.normalized();
					float Len = 0.f;
					auto Vec = (Target->GetMove().GetPos() - this->GetEyePosition()); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();

					float cost = Vector3DX::Dot(Vec, Dir);
					auto IsFront = (cost > cos(deg2rad(40)));
					float Radius = 3.5f * Scale3DRate;
					if (!IsOutArea() && (Len < Radius) && IsFront) {
						if (m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
							SetAction = EnumArmAnimType::Men;
							m_WazaType = WazaType::Men;
						}
						else if (m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::ULT)) {
							SetAction = EnumArmAnimType::Kote;
							m_WazaType = WazaType::Kote;
						}
						else if (false) {
							SetAction = EnumArmAnimType::Dou;
							m_WazaType = WazaType::Dou;
						}
					}
				}
				m_StaminaControl.m_HeartUp = std::max(m_StaminaControl.m_HeartUp - 30.f * DXLib_refParts->GetDeltaTime(), -5.f);
			}
			break;
			case EnumArmAnimType::Run:
			{
				if (IsOutArea() || m_RunTime >= 1.25f) {
					SetAction = EnumArmAnimType::Ready;
				}

				//つばぜり判定
				if (m_RunTime <= 0.025f) {
					CheckTsuba();
				}

				m_RunTime = std::max(m_RunTime + DXLib_refParts->GetDeltaTime(), 0.f);
				m_StaminaControl.m_HeartUp = std::max(m_StaminaControl.m_HeartUp + 0.5f * 30.f * DXLib_refParts->GetDeltaTime(), 3.f);
			}
			break;
			case EnumArmAnimType::Men:
			case EnumArmAnimType::Kote:
			case EnumArmAnimType::Tsuki:
			{
				//技派生
				if (m_FrontAttackActionTime <= 0.06f) {//受付開始
					if (m_FrontAttackActionTime <= 0.f) {//派生先がなく0秒になった
						SetAction = EnumArmAnimType::Run;
					}
					switch (Prev) {
					case EnumArmAnimType::Ready:
					case EnumArmAnimType::Run:
						break;
					case EnumArmAnimType::Men:
						if (false) {
							SetAction = EnumArmAnimType::Dou;//面胴
							m_WazaType = WazaType::Dou;
						}
						//小手面後派生
						if (false) {
							if (m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
								SetAction = EnumArmAnimType::Men;//小手面面
								m_WazaType = WazaType::Men;
							}
							if (false) {
								SetAction = EnumArmAnimType::Dou;//小手面胴
								m_WazaType = WazaType::Dou;
							}
						}
						break;
					case EnumArmAnimType::Kote:
						if (m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
							SetAction = EnumArmAnimType::Men;//小手面
							m_WazaType = WazaType::Men;
						}
						if (false) {
							SetAction = EnumArmAnimType::Dou;//小手胴
							m_WazaType = WazaType::Dou;
						}
						break;
					case EnumArmAnimType::Dou:
						break;
					case EnumArmAnimType::Tsuki:
						if (m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
							SetAction = EnumArmAnimType::Men;//突き面
							m_WazaType = WazaType::Men;
						}
						break;
					case EnumArmAnimType::Tsuba:
					case EnumArmAnimType::HikiMen:
					case EnumArmAnimType::HikiKote:
					case EnumArmAnimType::HikiDou:
					case EnumArmAnimType::GuardSuriage:
					case EnumArmAnimType::Max:
					default:
						break;
					}
				}
				m_FrontAttackActionTime = std::max(m_FrontAttackActionTime - DXLib_refParts->GetDeltaTime(), 0.f);
				m_StaminaControl.m_HeartUp = std::max(m_StaminaControl.m_HeartUp + 0.1f * 30.f * DXLib_refParts->GetDeltaTime(), 10.f);
			}
			break;
			case EnumArmAnimType::Dou:
			{
				if (m_FrontAttackActionTime <= 0.f) {
					SetAction = EnumArmAnimType::Run;
				}
				m_FrontAttackActionTime = std::max(m_FrontAttackActionTime - DXLib_refParts->GetDeltaTime(), 0.f);
				m_StaminaControl.m_HeartUp = std::max(m_StaminaControl.m_HeartUp + 0.1f * 30.f * DXLib_refParts->GetDeltaTime(), 10.f);
			}
			break;
			case EnumArmAnimType::Tsuba:
			{
				if (m_TsubaCoolDown <= 0.f) {
					if (m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
						SetAction = EnumArmAnimType::HikiMen;
						m_WazaType = WazaType::Hikimen;
					}
					else if (m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::ULT)) {
						SetAction = EnumArmAnimType::HikiKote;
						m_WazaType = WazaType::Hikigote;
					}
					else if (false) {
						SetAction = EnumArmAnimType::HikiDou;
					}
				}
				m_TsubaCoolDown = std::max(m_TsubaCoolDown - DXLib_refParts->GetDeltaTime(), 0.f);
				m_StaminaControl.m_HeartUp = std::max(m_StaminaControl.m_HeartUp - 30.f * DXLib_refParts->GetDeltaTime(), -5.f);
			}
			break;
			case EnumArmAnimType::HikiMen:
			case EnumArmAnimType::HikiKote:
			case EnumArmAnimType::HikiDou:
			{
				if (m_BackAttackActionTime <= 0.f) {
					SetAction = EnumArmAnimType::Ready;
				}
				m_BackAttackActionTime = std::max(m_BackAttackActionTime - DXLib_refParts->GetDeltaTime(), 0.f);
				m_StaminaControl.m_HeartUp = std::max(m_StaminaControl.m_HeartUp + 0.1f * 30.f * DXLib_refParts->GetDeltaTime(), 10.f);
			}
			break;
			case EnumArmAnimType::GuardSuriage:
			{
				if (m_GuardTimer == 0.f) {
					SetAction = EnumArmAnimType::Ready;
				}
				else {
					if (m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
						SetAction = EnumArmAnimType::Men;
						m_WazaType = WazaType::SuriageMen;
					}
					else if (m_CharaMove.GetInputControl().GetPADSPress(Controls::PADS::ULT)) {
						SetAction = EnumArmAnimType::Dou;
						m_WazaType = WazaType::Dou;
					}
				}
				m_GuardTimer = std::max(m_GuardTimer - DXLib_refParts->GetDeltaTime(), 0.f);
				m_StaminaControl.m_HeartUp = std::max(m_StaminaControl.m_HeartUp - 60.f * DXLib_refParts->GetDeltaTime(), -15.f);
			}
			break;
			case EnumArmAnimType::Max:
			default:
				break;
			}
			if (Prev != SetAction) {
				m_ArmMoveClass.ChangeAction(SetAction);
				OverrideAction();
			}
			m_CharaMove.UpdateKeyRad(this->m_move);
			if (m_ArmMoveClass.GetCharaAction() != EnumArmAnimType::Run && m_ArmMoveClass.GetCharaAction() != EnumArmAnimType::GuardSuriage) {
				m_GuardCoolDownTimer = std::max(m_GuardCoolDownTimer - DXLib_refParts->GetDeltaTime(), 0.f);
			}
		}
		void			CharacterClass::ExecuteAnim(void) noexcept {
			//上半身アニメ演算
			m_CharaMove.SetCharaAnimeBufID(CharaObjAnimeID::Upper_Ready) = 1.f;
			m_CharaMove.SetCharaAnimeBufID(CharaObjAnimeID::Hand_Ready) = 1.f;
			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Turn), 0.5f);
			ObjectBaseClass::SetAnimOnce(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Attack), m_CharaMove.GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Run), m_CharaMove.GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(m_CharaMove.GetBottomWalkAnimSel()), m_CharaMove.GetVecFront());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_LeftStep), m_CharaMove.GetVecLeft());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_WalkBack), m_CharaMove.GetVecRear());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_RightStep), m_CharaMove.GetVecRight());
			//アニメ反映
			for (int i = 0, Num = static_cast<int>(GetObj().GetAnimNum()); i < Num; ++i) {
				this->GetObj().SetAnim(i).SetPer(m_CharaMove.GetCharaAnimeBufID((CharaObjAnimeID)i));
			}
			GetObj().UpdateAnimAll();
		}
		void			CharacterClass::ExecuteSound(void) noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* SE = SoundPool::Instance();
			//足音
			if (m_CharaMove.m_BottomAnimSelect != m_CharaMove.GetBottomStandAnimSel()) {
				auto Time = this->GetObj().SetAnim(static_cast<int>(m_CharaMove.m_BottomAnimSelect)).GetTime();
				if (!m_CharaMove.GetRun()) {
					//L
					if ((9.f < Time && Time < 10.f)) {
						if (this->m_CharaSound != 1) {
							this->m_CharaSound = 1;
							if ((WindowSizeParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale3DRate * 5.f * 1.5f) {

								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFramePosition(CharaFrame::LeftFoot), Scale3DRate * 5.f);
							}
						}
					}
					//R
					if ((27.f < Time && Time < 28.f)) {
						if (this->m_CharaSound != 3) {
							this->m_CharaSound = 3;
							if ((WindowSizeParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale3DRate * 5.f * 1.5f) {

								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFramePosition(CharaFrame::RightFoot), Scale3DRate * 5.f);
							}
						}
					}
				}
				else {
					if (m_CharaMove.GetCharaAnimeBufID(CharaObjAnimeID::Bottom_Stand_Attack) > 0.5f) {
						if ((9.f < Time && Time < 10.f)) {
							if (this->m_CharaSound != 7) {
								this->m_CharaSound = 7;
								if ((WindowSizeParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale3DRate * 5.f * 1.5f) {
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Foot))->Play3D(GetFramePosition(CharaFrame::RightFoot), Scale3DRate * 15.f);
								}
							}
						}
					}
					else {
						//L
						if ((3.f < Time && Time < 4.f)) {
							if (this->m_CharaSound != 5) {
								this->m_CharaSound = 5;
								if ((WindowSizeParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale3DRate * 5.f * 1.5f) {
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFramePosition(CharaFrame::LeftFoot), Scale3DRate * 15.f);
								}
							}
						}
						//R
						if ((10.f < Time && Time < 11.f)							) {
							if (this->m_CharaSound != 6) {
								this->m_CharaSound = 6;
								if ((WindowSizeParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale3DRate * 5.f * 1.5f) {
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFramePosition(CharaFrame::RightFoot), Scale3DRate * 15.f);
								}
							}
						}
					}
				}
			}
			else {
				this->m_CharaSound = -1;
			}
			//心拍音
#ifdef DEBUG
			//printfDx("HEART : %f\n", m_HeartUp);
#endif
			if (m_StaminaControl.ExcuteStamina()) {
				if (this->m_MyID == this->m_ViewID) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heart))->Play(DX_PLAYTYPE_BACK, TRUE);
				}
			}
			if (m_StaminaControl.ExcuteBreath()) {
				if (this->m_MyID == this->m_ViewID) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Breath))->Play(DX_PLAYTYPE_BACK, TRUE);
				}
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Breath))->SetLocalVolume(static_cast<int>(Lerp(255.f, 92.f, m_StaminaControl.GetStamina() / m_StaminaControl.GetStaminaMax())));
			}
			if (GetYaTimer() >= GetYaTimerMax() * 0.9f) {
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Breath))->StopAll();
			}
		}
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)),
				(m_CharaMove.GetUpperRotMatrix()).Get44DX() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper)));
			//
			Vector3DX pos = this->m_move.GetPosBuf();
			//vector
			bool IsHitGround = (pos.y <= 0.f); //高度0なら止まる
			float groundYpos = 0.f;
			Vector3DX vec = m_CharaMove.GetVec();
			if (IsHitGround) {
				auto yPos = pos.y;
				Easing(&yPos, groundYpos, 0.8f, EasingType::OutExpo);
				pos.y = (yPos);
				vec.y = (0.f);
			}
			else {
				vec.y = (this->m_move.GetVec().y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps())));
			}
			this->m_move.SetVec(vec);
			pos += this->m_move.GetVec();
			if (m_ArmMoveClass.GetCharaAction() == EnumArmAnimType::Tsuba) {
				auto& Target = PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetChara();

				auto& TgtPos = Target->GetMove().GetPos();
				auto& MyPos = this->GetMove().GetPos();

				auto Vec = (TgtPos - MyPos); Vec.y = (0.f); Vec = Vec.normalized();

				float Radius = (0.5f + 0.25f) * Scale3DRate;
				pos = TgtPos - Vec * Radius;
			}
			//壁判定
			BackGround->CheckMapWall(this->m_move.GetRePos(), &pos, 0.6f * Scale3DRate);
			//ほかプレイヤーとの判定
			{
				float Radius = 0.5f * Scale3DRate;
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
					if (i == this->m_MyID) { continue; }
					auto& c = PlayerMngr->GetPlayer(i)->GetChara();
					//自分が当たったら押し戻す
					Vector3DX Vec = (c->GetMove().GetPos() - this->GetMove().GetPos()); Vec.y = (0.f);
					float Len = Vec.magnitude();
					if (Len < Radius) {
						pos += Vec.normalized() * (Len - Radius);
					}
				}
			}
			//座標オーバーライド
			if (this->m_MoveOverRideFlag) {
				this->m_MoveOverRideFlag = false;
				this->m_move = this->m_OverRideInfo;
			}

			this->m_move.SetPos(pos);
			this->m_move.Update(0.9f, 0.f);
			UpdateObjMatrix(m_CharaMove.GetBaseRotMatrix(), this->m_move.GetPos());
			{
				Matrix3x3DX Mat; Mat.SetRadian(0.f, m_CharaMove.GetRadBuf(), 0.f);
				this->m_move.SetMat(Mat);
			}
			m_EyePosition = (GetFramePosition(CharaFrame::LeftEye) + GetFramePosition(CharaFrame::RightEye)) / 2.f + (m_CharaMove.GetEyeRotMatrix() * m_CharaMove.GetBaseRotMatrix()).zvec() * (-0.04f * Scale3DRate);
			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((this->m_MyID != this->m_ViewID) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			if (HeadBobbing) {
				m_EyePosition += m_CharaMove.GetEyeSwingPos();
			}
			//銃座標指定
			if (this->m_Weapon_Ptr) {

				//銃の位置を指定するアニメ
				{
					//
					Matrix4x4DX WeaponBaseMat;
					for (size_t i = 0; i < static_cast<size_t>(EnumArmAnimType::Max); ++i) {
						Matrix4x4DX AnimData;
						if (!this->m_Weapon_Ptr->GetArmAnimeNowMatrix((EnumArmAnimType)i, &AnimData)) { continue; }
						WeaponBaseMat = Lerp(WeaponBaseMat, AnimData, this->m_ArmMoveClass.GetArmPer((EnumArmAnimType)i));
					}
					Vector3DX PrevPos = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::End);

					Vector3DX WeaponBasePos = GetFramePosition(CharaFrame::Head) + Matrix3x3DX::Vtrans(WeaponBaseMat.pos(), (m_CharaMove.GetUpperRotMatrix() * m_CharaMove.GetBaseRotMatrix()));
					auto WeaponBaseRotMat = Matrix3x3DX::RotVec2(Vector3DX::forward(), WeaponBaseMat.zvec());
					WeaponBaseRotMat *= Matrix3x3DX::RotVec2(WeaponBaseRotMat.yvec(), WeaponBaseMat.yvec());
					WeaponBaseRotMat *= m_CharaMove.GetWeaponSwingMat() * (m_CharaMove.GetUpperRotMatrix() * m_CharaMove.GetBaseRotMatrix());
					//竹刀同士の判定
					Matrix3x3DX WeaponAddRotMat;
					Vector2DX Prev = m_BambooVec;
					for (int index = 0; index < 10; ++index) {
						m_BambooVec = Lerp(Prev, m_BambooVecBase, static_cast<float>(index + 1) / 10.f);
						WeaponAddRotMat = Matrix3x3DX::RotAxis(Vector3DX::right(), m_BambooVec.x) * Matrix3x3DX::RotAxis(Vector3DX::forward(), m_BambooVec.y);
						this->m_Weapon_Ptr->UpdateObjMatrix(WeaponAddRotMat * WeaponBaseRotMat, WeaponBasePos);

						Vector3DX StartPos = WeaponBasePos;
						Vector3DX EndPos = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::End);
						Vector3DX VecA = (EndPos - StartPos).normalized();
						bool IsHit = false;
						float Radius = 0.025f * Scale3DRate * 2.f;
						Vector3DX HitPos;
						for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
							if (i == this->m_MyID) { continue; }
							auto& c = PlayerMngr->GetPlayer(i)->GetChara();
							//自分が当たったら押し戻す
							Vector3DX StartPosB = c->m_Weapon_Ptr->GetMove().GetPos();
							Vector3DX EndPosB = c->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::End);
							Vector3DX VecB = (EndPosB - StartPosB).normalized();

							SEGMENT_SEGMENT_RESULT Ret;
							GetSegmenttoSegment(StartPos, EndPos, StartPosB, EndPosB, &Ret);
							float Len = std::sqrtf(Ret.SegA_SegB_MinDist_Square);

							Vector3DX Vec0 = Ret.SegA_MinDist_Pos;
							Vector3DX Vec1 = Ret.SegB_MinDist_Pos;

							if (Len <= Radius &&
								(0.f < Ret.SegA_MinDist_Pos1_Pos2_t && Ret.SegA_MinDist_Pos1_Pos2_t < 1.f) &&
								(0.f < Ret.SegB_MinDist_Pos1_Pos2_t && Ret.SegB_MinDist_Pos1_Pos2_t < 1.f)
								) {
									HitPos = Vec0;
									{
										Vector3DX Vec2 = Vec0 + (Vec1 - Vec0).normalized() * (Len - Radius);
										auto ZVec = Matrix3x3DX::RotVec2(VecA, (Vec2 - StartPos).normalized()).zvec();
										m_BambooVec.y += atan2f(ZVec.x, ZVec.z) * 0.5f;
										m_BambooVec.x += atan2f(ZVec.y, std::hypotf(ZVec.x, ZVec.z)) * 0.5f;
										m_BambooVec.Set(std::clamp(m_BambooVec.x, deg2rad(-10), deg2rad(10)), std::clamp(m_BambooVec.y, deg2rad(-30), deg2rad(30)));
									}
									{
										Vector3DX Vec2 = Vec1 + (Vec0 - Vec1).normalized() * (Len - Radius);
										auto ZVec = Matrix3x3DX::RotVec2(VecB, (Vec2 - StartPosB).normalized()).zvec();
										c->m_BambooVec.y += atan2f(ZVec.x, ZVec.z) * 1.5f;
										c->m_BambooVec.x += atan2f(ZVec.y, std::hypotf(ZVec.x, ZVec.z)) * 1.5f;
									}
									IsHit = true;

									if (m_TsubaSoundCoolTime == 0.f && (this->m_Weapon_Ptr->GetMoveSpeed() > 1.f)) {
										auto* SE = SoundPool::Instance();
										SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Tsuba))->Play3D(GetFramePosition(CharaFrame::RightWrist), Scale3DRate * 15.f, static_cast<int>(128.f * this->m_Weapon_Ptr->GetMoveSpeed()));
										m_TsubaSoundCoolTime = static_cast<float>(SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Tsuba))->GetTotalTIme()) / 1000.f * 0.5f;
									}
									WeaponAddRotMat = Matrix3x3DX::RotAxis(Vector3DX::right(), m_BambooVec.x) * Matrix3x3DX::RotAxis(Vector3DX::forward(), m_BambooVec.y);
									if (ArmMoveClass::IsGuardAction(m_ArmMoveClass.GetCharaAction()) || IsAttacking()) {
										if (m_GuardHit == 0.f) {
#ifdef _USE_EFFEKSEER_
											m_Effect.SetOnce(EffectResource::Effect::ef_gndsmoke, HitPos, (c->GetEyePosition() - GetEyePosition()).normalized(), 0.1f * Scale3DRate);
#endif
										}
										m_GuardHit = 3.f / FrameRate;
									}
									break;
							}
						}
						if (IsHit) {
							break;
						}
					}
					this->m_Weapon_Ptr->ResetMove(WeaponAddRotMat * WeaponBaseRotMat, WeaponBasePos);

					this->m_Weapon_Ptr->SetMoveSpeed((PrevPos - this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::End)).magnitude()* DXLib_refParts->GetFps() / FrameRate);
				}
				//手の位置を制御
				if ((m_MyID == this->m_ViewID) || this->CanLookTarget) {
					Vector3DX WeaponPos = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::RightHandPos);
					Vector3DX Weaponyvec = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::RightHandYvec) - WeaponPos;
					Vector3DX Weaponzvec = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::RightHandZvec) - WeaponPos;

					Vector3DX HandsPos = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::LeftHandPos);
					Vector3DX Handsyvec = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::LeftHandYvec) - HandsPos;
					Vector3DX Handszvec = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::LeftHandZvec) - HandsPos;

					IK_RightArm(
						&GetObj(),
						GetFrame(static_cast<int>(CharaFrame::RightArm)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
						GetFrame(static_cast<int>(CharaFrame::RightArm2)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
						GetFrame(static_cast<int>(CharaFrame::RightWrist)),
						GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
						WeaponPos, Weaponyvec, Weaponzvec);
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
}
