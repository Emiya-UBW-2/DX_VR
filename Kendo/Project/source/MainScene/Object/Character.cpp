#include	"Character.hpp"

#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
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
						if (IsPlayerChara()) {
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
		void			CharacterClass::SetInput(const InputControl& pInput) noexcept {
			//自動で相手を向く
			float pp_y = 0.f;
			{
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& Target = PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();

				auto Dir = m_CharaMove.GetEyeMatrix().zvec() * -1.f; Dir.y = (0.f); Dir = Dir.normalized();
				float Len = 0.f;
				auto Vec = (Target->GetMove().GetPos() - this->m_CharaMove.GetEyePosition()); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();

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
			m_CharaMove.InputKey(pInput, pp_y);
		}
		void			CharacterClass::UpdateBambooAttack(void) noexcept {
			if (!IsAttackAction(this->GetCharaAction())) { return; }
			if (m_GuardHit > 0.f) { return; }//ガードで竹刀を抑えた
			auto* PlayerMngr = Player::PlayerManager::Instance();
			Vector3DX StartPos = m_Weapon_Ptr->GetMove().GetPos();
			Vector3DX EndPos = m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::End);
			StartPos = StartPos + (EndPos - StartPos) * 0.5f;

			for (int index2 = 0; index2 < PlayerMngr->GetPlayerNum(); ++index2) {
				if (GetMyPlayerID() == index2) { continue; }
				auto& tgt = PlayerMngr->GetPlayer(index2)->GetChara();
				tgt->CheckDamageRay(this, StartPos, &EndPos);
			}
		}
		//
		bool			CharacterClass::CheckDamageRay(CharacterClass* pAttacker, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, GetMove().GetPos()) <= 2.0f * Scale3DRate)) { return false; }
			float BaseLen = (StartPos - *pEndPos).magnitude();
			//被弾処理
			auto* HitPtr = m_HitBoxControl.GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				bool IsDamage = true;
				{
					//撃った技と違う個所ならダメージと扱わない
					switch (pAttacker->m_CharaMove.GetWazaType()) {
					case FPS_n2::WazaType::Men:
					case FPS_n2::WazaType::SuriageMen:
					case FPS_n2::WazaType::Hikimen:
						if (HitPtr->GetColType() != HitType::Head) {
							IsDamage = false;
						}
						break;
					case FPS_n2::WazaType::Kote:
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
				}
				if (IsDamage) {
					//ダメージ登録
					m_Damage.Add(
						pAttacker->GetMyPlayerID(),
						this->GetMyPlayerID(),
						static_cast<HitPoint>(100.f * pAttacker->m_Weapon_Ptr->GetMoveSpeed() / 5.f),
						pAttacker->m_CharaMove.GetYaTimerPer(),
						(StartPos - *pEndPos).magnitude() / BaseLen,
						HitPtr->GetColType(),
						pAttacker->m_CharaMove.GetWazaType(),
						*pEndPos);
				}
				return true;
			}
			return false;
		}
		bool			CharacterClass::CheckTsuba(void) const noexcept {
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
			return ((Len < Radius) && IsFront && (abs(cross) < 0.4f));
		}
		void			CharacterClass::ChangeAction(EnumArmAnimType o) noexcept {
			this->m_Weapon_Ptr->ResetAnim();
			m_CharaMove.ChangeAction(o, this->m_Weapon_Ptr->GetArmAnimeTotalTime(m_CharaMove.GetCharaAction()));
		}
		//
		void			CharacterClass::Init_Sub(void) noexcept {
			m_BambooVec.Set(0.f, 0.f);
			m_BambooVecBase.Set(0.f, 0.f);
			m_HitBoxControl.Init();
			this->m_MoveOverRideFlag = false;
#ifdef _USE_EFFEKSEER_
			m_Effect.Init();				//
#endif
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				for (int i = 0, num = GetObj().GetMaterialNum(); i < num; ++i) {
					GetObj().SetMaterialDifColor(i, GetColorF(0.8f, 0.8f, 0.8f, 1.f));
					GetObj().SetMaterialAmbColor(i, GetColorF(0.25f, 0.25f, 0.25f, 1.f));
				}
				//this->GetObj().SetMaterialTypeAll(DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
			}
			//
			m_DamageCoolTime = std::max(m_DamageCoolTime - DXLib_refParts->GetDeltaTime(), 0.f);
			m_GuardHit = std::max(m_GuardHit - DXLib_refParts->GetDeltaTime(), 0.f);
			//
			ExecuteInput();
			ExecuteAnim();
			ExecuteSound();
			ExecuteMatrix();
			if (this->m_Weapon_Ptr) {
				ExecuteWeapon();
			}
			m_HitBoxControl.Update(this, 1.f);									//ヒットボックス
#ifdef _USE_EFFEKSEER_
			m_Effect.Execute();				//
#endif
		}
		void			CharacterClass::CheckDraw(void) noexcept {
			this->m_IsDraw = false;
			this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetScreenPosition()).magnitude();
			if (CheckCameraViewClip_Box(
				(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f * Scale3DRate, -0.f * Scale3DRate, -2.5f * Scale3DRate)).get(),
				(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f * Scale3DRate, 2.5f * Scale3DRate, 2.5f * Scale3DRate)).get()) == FALSE
				) {
				this->m_IsDraw |= true;
			}
		}
		void			CharacterClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if (IsPlayerChara()) {
					if (isDrawSemiTrans) {
						this->GetObj().DrawModel();
					}
				}
				else {
					for (int i = 0, Num = this->GetObj_const().GetMeshNum(); i < Num; ++i) {
						if (this->GetObj_const().GetMeshSemiTransState(i) == isDrawSemiTrans) {
							this->GetObj().DrawMesh(i);
						}
					}
					//hitbox描画
					//m_HitBoxControl.Draw();
				}
			}
		}
		void			CharacterClass::DrawShadow(void) noexcept {
			if (this->m_IsActive) {
				this->GetObj().DrawModel();
			}
		}
		void			CharacterClass::Dispose_Sub(void) noexcept {
			m_Weapon_Ptr.reset();
#ifdef _USE_EFFEKSEER_
			m_Effect.Dispose();
#endif
		}
		//
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Target = PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();
			//
			if (m_CharaMove.IsStartAction()) {
				if (IsAttackAction(m_CharaMove.GetCharaAction())) {
					auto* SE = SoundPool::Instance();
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Men))->StopAll();
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Kote))->StopAll();
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Dou))->StopAll();
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Tsuki))->StopAll();
					switch (m_CharaMove.GetCharaAction()) {
					case EnumArmAnimType::Men:
					case EnumArmAnimType::HikiMen:
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Men))->Play3D(m_CharaMove.GetEyePosition(), Scale3DRate * 35.f);
						break;
					case EnumArmAnimType::Kote:
					case EnumArmAnimType::HikiKote:
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Kote))->Play3D(m_CharaMove.GetEyePosition(), Scale3DRate * 35.f);
						break;
					case EnumArmAnimType::Dou:
					case EnumArmAnimType::HikiDou:
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Dou))->Play3D(m_CharaMove.GetEyePosition(), Scale3DRate * 35.f);
						break;
					case EnumArmAnimType::Tsuki:
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Tsuki))->Play3D(m_CharaMove.GetEyePosition(), Scale3DRate * 35.f);
						break;
					default:
						break;
					}
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Swing))->Play3D(GetFramePosition(CharaFrame::RightWrist), Scale3DRate * 5.f);
					this->m_CharaSound = -1;
					//
					this->GetObj().SetAnim(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Attack)).GoStart();
				}
				if (IsBackAttackAction(m_CharaMove.GetCharaAction())) {
					Target->ChangeAction(EnumArmAnimType::Ready);
				}
				if (IsGuardAction(m_CharaMove.GetCharaAction())) {
					m_BambooVecBase.Set(0.f, 0.f);
				}
			}
			auto SetAction = m_CharaMove.UpdateAction();
			if (m_CharaMove.GetCharaAction() == SetAction) {
				if (m_CharaMove.GetCharaAction() == EnumArmAnimType::Ready) {
					//つばぜり判定
					if (Target->m_CharaMove.GetCharaAction() == this->m_CharaMove.GetCharaAction()) {
						if (CheckTsuba()) {
							SetAction = EnumArmAnimType::Tsuba;
							//相手もつばぜり合いにする
							Target->ChangeAction(EnumArmAnimType::Tsuba);
						}
					}
					//打突可能範囲にいるか判定
					auto Dir = this->m_CharaMove.GetEyeMatrix().zvec() * -1.f; Dir.y = (0.f); Dir = Dir.normalized();
					auto Vec = (Target->GetMove().GetPos() - this->m_CharaMove.GetEyePosition()); Vec.y = (0.f);
					float Len = Vec.magnitude();
					Vec = Vec.normalized();
					auto IsFront = (Vector3DX::Dot(Vec, Dir) > cos(deg2rad(40)));
					float Radius = 3.5f * Scale3DRate;
					m_CharaMove.SetCanFrontAttack(!IsOutArea() && (Len < Radius) && IsFront);
				}
				if (m_CharaMove.GetCharaAction() == EnumArmAnimType::Run) {
					if (IsOutArea()) {
						SetAction = EnumArmAnimType::Ready;
					}
					//つばぜり判定
					if (m_CharaMove.GetActionTime() <= 0.025f) {
						if (CheckTsuba()) {
							SetAction = EnumArmAnimType::Tsuba;
							//相手もつばぜり合いにする
							Target->ChangeAction(EnumArmAnimType::Tsuba);
						}
					}
				}
			}
			if (m_CharaMove.GetCharaAction() != SetAction) {
				ChangeAction(SetAction);
			}
			{
				float YRad = 0.f;
				this->m_move.GetMatBuf().GetRadian(nullptr, &YRad, nullptr);
				YRad += m_CharaMove.GetRadAdd();
				m_CharaMove.Update(YRad);
				Matrix3x3DX Mat; Mat.SetRadian(0.f, YRad, 0.f);
				this->m_move.SetMat(Mat);
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
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Walk), m_CharaMove.GetVecFront());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_LeftStep), m_CharaMove.GetVecLeft());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_WalkBack), m_CharaMove.GetVecRear());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_RightStep), m_CharaMove.GetVecRight());
			//アニメ反映
			for (int i = 0, Num = static_cast<int>(this->GetObj().GetAnimNum()); i < Num; ++i) {
				this->GetObj().SetAnim(i).SetPer(m_CharaMove.GetCharaAnimeBufID((CharaObjAnimeID)i));
			}
			this->GetObj().UpdateAnimAll();
		}
		void			CharacterClass::ExecuteSound(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			//足音
			if (m_CharaMove.GetBottomAnimSelect() != CharaObjAnimeID::Bottom_Stand) {
				auto Time = this->GetObj().SetAnim(static_cast<int>(m_CharaMove.GetBottomAnimSelect())).GetTime();
				if (!m_CharaMove.GetRun()) {
					//L
					if ((9.f < Time && Time < 10.f)) {
						if (this->m_CharaSound != 1) {
							this->m_CharaSound = 1;
							if ((CameraParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale3DRate * 5.f * 1.5f) {

								SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFramePosition(CharaFrame::LeftFoot), Scale3DRate * 5.f);
							}
						}
					}
					//R
					if ((27.f < Time && Time < 28.f)) {
						if (this->m_CharaSound != 3) {
							this->m_CharaSound = 3;
							if ((CameraParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale3DRate * 5.f * 1.5f) {

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
								if ((CameraParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale3DRate * 5.f * 1.5f) {
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
								if ((CameraParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale3DRate * 5.f * 1.5f) {
									SE->Get(SoundType::SE, static_cast<int>(SoundEnum::RunFoot))->Play3D(GetFramePosition(CharaFrame::LeftFoot), Scale3DRate * 15.f);
								}
							}
						}
						//R
						if ((10.f < Time && Time < 11.f)							) {
							if (this->m_CharaSound != 6) {
								this->m_CharaSound = 6;
								if ((CameraParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale3DRate * 5.f * 1.5f) {
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
			//
			if (IsPlayerChara()) {
				//心拍音
				if (m_CharaMove.GetHeartSwitch()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Heart))->Play(DX_PLAYTYPE_BACK, TRUE);
				}
				//息
				if (m_CharaMove.GetBreathSwitch()) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Breath))->Play(DX_PLAYTYPE_BACK, TRUE);
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Breath))->SetLocalVolume(static_cast<int>(Lerp(255.f, 92.f, m_CharaMove.GetStaminaPer())));
				}
				if (m_CharaMove.GetYaTimerPer() >= 0.9f) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Breath))->StopAll();
				}
			}
			//
			if (m_TsubaSoundCoolTime == 0.f) {
				if (m_TsubaSoundSwitch) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Tsuba))->Play3D(GetFramePosition(CharaFrame::RightWrist), Scale3DRate * 15.f, static_cast<int>(128.f * this->m_Weapon_Ptr->GetMoveSpeed()));
					m_TsubaSoundCoolTime = static_cast<float>(SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Kendo_Tsuba))->GetTotalTIme()) / 1000.f * 0.5f;
				}
			}
			else {
				m_TsubaSoundCoolTime = std::max(m_TsubaSoundCoolTime - DXLib_refParts->GetDeltaTime(), 0.f);
			}
		}
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//
			this->m_move.SetVec(m_CharaMove.GetVec());
			Vector3DX pos = this->m_move.GetPosBuf() + this->m_move.GetVec(); pos.y = 0.f;
			if (m_CharaMove.GetCharaAction() == EnumArmAnimType::Tsuba) {
				auto& Target = PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetChara();
				auto& TgtPos = Target->GetMove().GetPos();
				auto& MyPos = this->GetMove().GetPos();
				auto Vec = (TgtPos - MyPos); Vec.y = (0.f); Vec = Vec.normalized();
				float Radius = (0.5f + 0.25f) * Scale3DRate;
				pos = TgtPos - Vec * Radius;
			}
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
			//壁判定
			BackGround->CheckMapWall(this->m_move.GetRePos(), &pos, 0.6f * Scale3DRate);
			//座標オーバーライド
			if (this->m_MoveOverRideFlag) {
				this->m_MoveOverRideFlag = false;
				this->m_move = this->m_OverRideInfo;
			}
			this->m_move.SetPos(pos);
			this->m_move.Update(0.9f, 0.f);
			UpdateObjMatrix(m_CharaMove.GetBaseRotMatrix(), this->m_move.GetPos());
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)),
				m_CharaMove.GetUpperRotMatrix().Get44DX() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper)));
			m_CharaMove.SetEyePosition(
				(GetFramePosition(CharaFrame::LeftEye) + GetFramePosition(CharaFrame::RightEye)) / 2.f + m_CharaMove.GetEyeMatrix().zvec() * (-0.04f * Scale3DRate)
			);
		}
		void			CharacterClass::ExecuteWeapon(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//銃の位置を指定するアニメ
			{
				//竹刀の回転を反映
				if (!IsAttackAction(m_CharaMove.GetCharaAction())) {
					Easing(&m_BambooVecBase, Vector2DX::vget(m_CharaMove.GetInputControl().GetxRad(), m_CharaMove.GetInputControl().GetyRad()), 0.7f, EasingType::OutExpo);
				}
				else {
					m_BambooVecBase.Set(0.f, 0.f);
				}
				//
				Matrix4x4DX WeaponBaseMat;
				for (size_t i = 0; i < static_cast<size_t>(EnumArmAnimType::Max); ++i) {
					Matrix4x4DX AnimData;
					if (!this->m_Weapon_Ptr->GetArmAnimeNowMatrix((EnumArmAnimType)i, &AnimData)) { continue; }
					WeaponBaseMat = Lerp(WeaponBaseMat, AnimData, this->m_CharaMove.GetArmPer((EnumArmAnimType)i));
				}
				Vector3DX PrevPos = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::End);

				Vector3DX WeaponBasePos = GetFramePosition(CharaFrame::Head) + Matrix3x3DX::Vtrans(WeaponBaseMat.pos(), m_CharaMove.GetUpperMatrix());
				auto WeaponBaseRotMat = Matrix3x3DX::RotVec2(Vector3DX::forward(), WeaponBaseMat.zvec());
				WeaponBaseRotMat *= Matrix3x3DX::RotVec2(WeaponBaseRotMat.yvec(), WeaponBaseMat.yvec());
				WeaponBaseRotMat *= m_CharaMove.GetUpperMatrix();
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

							m_TsubaSoundSwitch = (this->m_Weapon_Ptr->GetMoveSpeed() > 1.f);
							WeaponAddRotMat = Matrix3x3DX::RotAxis(Vector3DX::right(), m_BambooVec.x) * Matrix3x3DX::RotAxis(Vector3DX::forward(), m_BambooVec.y);
							if (IsGuardAction(m_CharaMove.GetCharaAction()) || IsAttackAction(m_CharaMove.GetCharaAction())) {
								if (m_GuardHit == 0.f) {
#ifdef _USE_EFFEKSEER_
									m_Effect.SetOnce((EffectControl::Effect)0, HitPos, (c->m_CharaMove.GetEyePosition() - this->m_CharaMove.GetEyePosition()).normalized(), 0.1f * Scale3DRate);
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

				this->m_Weapon_Ptr->SetMoveSpeed((PrevPos - this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::End)).magnitude() * DXLib_refParts->GetFps() / FrameRate);
			}
			//手の位置を制御
			{
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
