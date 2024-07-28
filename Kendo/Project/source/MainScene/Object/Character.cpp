#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		void			CharacterClass::PlayVoice(void) const noexcept {
			auto* SE = SoundPool::Instance();
			switch (m_CharaAction) {
			case EnumArmAnimType::Ready:
				break;
			case EnumArmAnimType::Run:
				break;
			case EnumArmAnimType::Men:
			case EnumArmAnimType::HikiMen:
				SE->Get(static_cast<int>(SoundEnum::Voice_Men)).Play_3D(0, GetEyePosition(), Scale_Rate * 35.f);
				break;
			case EnumArmAnimType::Kote:
			case EnumArmAnimType::HikiKote:
				SE->Get(static_cast<int>(SoundEnum::Voice_Kote)).Play_3D(0, GetEyePosition(), Scale_Rate * 35.f);
				break;
			case EnumArmAnimType::Dou:
			case EnumArmAnimType::HikiDou:
				SE->Get(static_cast<int>(SoundEnum::Voice_Dou)).Play_3D(0, GetEyePosition(), Scale_Rate * 35.f);
				break;
			case EnumArmAnimType::Tsuki:
				SE->Get(static_cast<int>(SoundEnum::Voice_Tsuki)).Play_3D(0, GetEyePosition(), Scale_Rate * 35.f);
				break;
			case EnumArmAnimType::Tsuba:
				break;
			case EnumArmAnimType::GuardStart:
				break;
			case EnumArmAnimType::GuardSuriage:
				break;
			case EnumArmAnimType::GuardLeft:
				break;
			case EnumArmAnimType::GuardRight:
				break;
			case EnumArmAnimType::Max:
				break;
			default:
				break;
			}
		}
		void			CharacterClass::StopVoice(void) const noexcept {
			auto* SE = SoundPool::Instance();
			SE->Get(static_cast<int>(SoundEnum::Voice_Men)).StopAll(0);
			SE->Get(static_cast<int>(SoundEnum::Voice_Kote)).StopAll(0);
			SE->Get(static_cast<int>(SoundEnum::Voice_Dou)).StopAll(0);
			SE->Get(static_cast<int>(SoundEnum::Voice_Tsuki)).StopAll(0);
		}

		void			CharacterClass::CheckTsuba(void) noexcept {
			bool IsGuard = false;


			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();

			auto TgtPos = Target->GetFramePosition(CharaFrame::LeftWrist);
			auto MyPos = this->GetFramePosition(CharaFrame::LeftWrist);

			float Len = 0.f;
			auto Vec = (TgtPos - MyPos); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();

			auto Dir = CharaMove::GetEyeMatrix().zvec() * -1.f; Dir.y = (0.f); Dir = Dir.normalized();

			auto IsFront = ((Vector3DX::Dot(Dir, Vec)) > 0.f);
			auto cross = Vector3DX::Cross(Dir, Vec).y;

			float Radius = (0.15f + 0.15f) * Scale_Rate;
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
			SE->Get(static_cast<int>(SoundEnum::Kendo_Swing)).Play_3D(0, GetFramePosition(CharaFrame::RightWrist), Scale_Rate * 5.f);
		}
		//
		void			CharacterClass::OverrideAction(void) noexcept {
			this->m_Weapon_Ptr->ResetAnim();
			//this->m_Weapon_Ptr->SetAnim();
			switch (m_CharaAction) {
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
			case EnumArmAnimType::GuardStart:
				OverrideGuardStart();
				break;
			case EnumArmAnimType::GuardSuriage:
			case EnumArmAnimType::GuardLeft:
			case EnumArmAnimType::GuardRight:
				OverrideGuard();
				break;
			case EnumArmAnimType::Max:
				break;
			default:
				break;
			}
		}
		void			CharacterClass::OverrideReady(void) noexcept {
			m_CharaAction = EnumArmAnimType::Ready;
			CharaMove::SetIsFrontAttacking(false);
			CharaMove::SetIsDouAttacking(false);
			CharaMove::SetIsBackAttacking(false);
			CharaMove::SetIsRunning(false);
		}
		void			CharacterClass::OverrideRun(void) noexcept {
			m_RunTime = 0.f;
			CharaMove::SetIsFrontAttacking(false);
			CharaMove::SetIsDouAttacking(false);
			CharaMove::SetIsBackAttacking(false);
			CharaMove::SetIsRunning(true);
		}
		void			CharacterClass::OverrideFrontAttack(void) noexcept {
			m_FrontAttackActionTime = this->m_Weapon_Ptr->GetArmAnimeTotalTime(m_CharaAction);
			CharaMove::SetIsFrontAttacking(true);
			CharaMove::SetIsDouAttacking(false);
			CharaMove::SetIsBackAttacking(false);
			SetFumikomi();
			StopVoice();
			PlayVoice();
			m_BambooVecBase.Set(0.f, 0.f);
		}
		void			CharacterClass::OverrideDo(void) noexcept {
			m_FrontAttackActionTime = this->m_Weapon_Ptr->GetArmAnimeTotalTime(m_CharaAction);
			CharaMove::SetIsFrontAttacking(false);
			CharaMove::SetIsDouAttacking(true);
			CharaMove::SetIsBackAttacking(false);
			SetFumikomi();
			StopVoice();
			PlayVoice();
			m_BambooVecBase.Set(0.f, 0.f);
		}
		void			CharacterClass::OverrideTsuba(void) noexcept {
			m_TsubaCoolDown = 0.5f;
			m_CharaAction = EnumArmAnimType::Tsuba;
			CharaMove::SetIsFrontAttacking(false);
			CharaMove::SetIsDouAttacking(false);
			CharaMove::SetIsBackAttacking(false);
			CharaMove::SetIsRunning(false);
		}
		void			CharacterClass::OverrideBackAttack(void) noexcept {
			m_BackAttackActionTime = std::max(0.75f, this->m_Weapon_Ptr->GetArmAnimeTotalTime(m_CharaAction));
			CharaMove::SetIsFrontAttacking(false);
			CharaMove::SetIsDouAttacking(true);
			CharaMove::SetIsBackAttacking(true);
			SetFumikomi();
			StopVoice();
			PlayVoice();

			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetChara();
			Target->OverrideReady();
			m_BambooVecBase.Set(0.f, 0.f);
		}
		void			CharacterClass::OverrideGuardStart(void) noexcept {
			m_GuardStartTimer = 0.f;
			m_GuardVec.Set(0.f, 0.f);
			m_GuardVecR.Set(0.f, 0.f);
		}
		void			CharacterClass::OverrideGuard(void) noexcept {
			m_BambooVecBase = Vector2DX::zero();
			m_GuardTimer = 1.f;
		}
		//
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			if (this->m_MyID == value.DamageID) {
				if (m_DamageCoolTime == 0.f && (GetRand(100) <= 30)) {
					auto* SE = SoundPool::Instance();
					SE->Get(static_cast<int>(SoundEnum::Kendo_Hit)).Play_3D(0, GetFramePosition(CharaFrame::RightWrist), Scale_Rate * 15.f);
					m_DamageCoolTime = static_cast<float>(SE->Get(static_cast<int>(SoundEnum::Kendo_Hit)).GetTotalTIme(0, 0)) / 1000.f;
				}
				//コンカッション
				if (this->m_MyID == this->m_ViewID) {
					switch (value.GetHitType()) {
					case HitType::Head://面
						m_ConcussionSwitch = true;
						break;
					case HitType::Body://胴
						m_ConcussionSwitch = true;
						break;
					case HitType::Arm://小手
						m_ConcussionSwitch = true;
						break;
					default:
						break;
					}
				}
				else {
					COLOR_U8 Color{};
					float Per = 1.f;
					switch (value.GetHitType()) {
					case HitType::Head://面
						Color = GetColorU8(255, 0, 0, 255);
						Per = static_cast<float>(value.Damage) / 100.f;
						break;
					case HitType::Arm://小手
						Color = GetColorU8(255, 128, 0, 255);
						Per = static_cast<float>(value.Damage) / 100.f;
						break;
					case HitType::Body://胴
						Color = GetColorU8(255, 255, 0, 255);
						Per = static_cast<float>(value.Damage) / 100.f;
						break;
					default:
						break;
					}
					HitMark::Instance()->Add(value.m_Pos, Color, Per);
				}
				//ダメージ
				return true;
			}
			return false;
		}
		//
		const bool		CharacterClass::CheckDamageRay(HitPoint* Damage, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, GetMove().GetPos()) <= 2.0f * Scale_Rate)) { return false; }
			//被弾処理
			auto* HitPtr = HitBoxControl::GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				//ダメージ登録
				{
					m_Damage.Add(AttackID, this->m_MyID, *Damage, HitPtr->GetColType(), *pEndPos);
				}
				return true;
			}
			return false;
		}
		void			CharacterClass::MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept {
			CharaMove::InitKey(pxRad, pyRad);
			Matrix3x3DX Mat; Mat.SetRadian(CharaMove::GetRadBuf().x, CharaMove::GetRadBuf().y, 0.f);
			ResetMove(Mat, pPos);
			m_CharaAction = EnumArmAnimType::Ready;
			for (auto& a : this->m_Arm) { a.Init(false); }
			this->m_Arm[static_cast<int>(EnumArmAnimType::Ready)].Init(true);
			this->m_CharaSound = -1;
		}
		void			CharacterClass::Init_Sub(void) noexcept {
			m_HeartUp = 0.f;
			m_HeartUpR = 0.f;
			m_BambooVec.Set(0.f, 0.f);
			m_BambooVecBase.Set(0.f, 0.f);
			StaminaControl::InitStamina();
			HitBoxControl::InitHitBox();
			this->m_MoveOverRideFlag = false;
#ifdef _USE_EFFEKSEER_
			EffectControl::Init();				//
#endif
		}
		//
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			InputControl Input = pInput;
			//ガード入力を反映
			m_GuardVec.x = std::clamp(m_GuardVec.x + Input.GetAddyRad() / 200.f, -1.5f, 1.5f);
			m_GuardVec.y = std::clamp(m_GuardVec.y + Input.GetAddxRad() / 200.f, -1.5f, 1.5f);
			Easing(&m_GuardVecR, m_GuardVec, 0.9f, EasingType::OutExpo);
			//竹刀の回転を反映
			if (!IsAttacking()) {
				Easing(&m_BambooVecBase, Vector2DX::vget(Input.GetxRad(), Input.GetyRad()), 0.7f, EasingType::OutExpo);
			}
			//自動で相手を向く
			{
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();

				auto Dir = CharaMove::GetEyeMatrix().zvec() * -1.f; Dir.y = (0.f); Dir = Dir.normalized();
				float Len = 0.f;
				auto Vec = (Target->GetMove().GetPos() - this->GetEyePosition()); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();
				float pp_y = 0.f;
				if (!CharaMove::GetRun()) {
					float sint = Vector3DX::Cross(Vec, Dir).y;
					float cost = Vector3DX::Dot(Vec, Dir);
					auto IsFront = (cost > cos(deg2rad(40)));
					if (IsFront) {
						pp_y = std::clamp(-std::atan2f(sint, cost), -deg2rad(60), deg2rad(60)) * 9.f / Frame_Rate;
					}
					else {
						pp_y = -deg2rad(60) * 9.f / Frame_Rate;
					}
				}
				Input.SetAddxRad(0.f);
				Input.SetAddyRad(pp_y);
			}
			//
			if (!pReady) {
				Input.ResetKeyInput();
			}
			CharaMove::InputKey(Input);
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				for (int i = 0, num = MV1GetMaterialNum(GetObj().GetHandle()); i < num; ++i) {
					MV1SetMaterialDifColor(GetObj().GetHandle(), i, GetColorF(0.8f, 0.8f, 0.8f, 1.f));
					MV1SetMaterialAmbColor(GetObj().GetHandle(), i, GetColorF(0.25f, 0.25f, 0.25f, 1.f));
				}
				//MV1SetMaterialTypeAll(this->GetObj().GetHandle(), DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
			}
			//
			ExecuteInput();
			ExecuteAction();
			ExecuteAnim();
			ExecuteSound();
			ExecuteMatrix();
			HitBoxControl::UpdataHitBox(this, 1.f);									//ヒットボックス
#ifdef _USE_EFFEKSEER_
			EffectControl::Execute();				//
#endif
		}
		//
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			//
			bool IsOutArea = false;
			{
				Vector3DX Vec = this->m_move.GetPos() - Vector3DX::zero();
				float Len = 11.f / 2.f * Scale_Rate;
				if ((Vec.x < -Len || Len < Vec.x) ||
					(Vec.z < -Len || Len < Vec.z)) {
					IsOutArea = true;
				}
			}

			//掛け声
			if ((m_CharaAction == EnumArmAnimType::Ready) && m_YaTimer <= 0.f && CharaMove::GetInputControl().GetPADSPress(PADS::JUMP)) {
				SE->Get(static_cast<int>(SoundEnum::Voice_Ya)).Play_3D(0, GetEyePosition(), Scale_Rate * 35.f);
				m_YaTimer = GetYaTimerMax();
			}
			m_YaTimer = std::max(m_YaTimer - 1.f / DrawParts->GetFps(), 0.f);
			m_DamageCoolTime = std::max(m_DamageCoolTime - 1.f / DrawParts->GetFps(), 0.f);
			m_TsubaSoundCoolTime = std::max(m_TsubaSoundCoolTime - 1.f / DrawParts->GetFps(), 0.f);
			m_GuardHit = std::max(m_GuardHit - 1.f / DrawParts->GetFps(), 0.f);
			//
			auto Prev = m_CharaAction;
			switch (m_CharaAction) {
			case EnumArmAnimType::Ready:
			{
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();
				//つばぜり判定
				if(Target->GetCharaAction()==GetCharaAction()){
					CheckTsuba();
				}
				if (CharaMove::GetInputControl().GetPADSPress(PADS::AIM)) {
					m_CharaAction = EnumArmAnimType::GuardStart;
				}
				{
					auto Dir = CharaMove::GetEyeMatrix().zvec() * -1.f; Dir.y = (0.f); Dir = Dir.normalized();
					float Len = 0.f;
					auto Vec = (Target->GetMove().GetPos() - this->GetEyePosition()); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();

					float cost = Vector3DX::Dot(Vec, Dir);
					auto IsFront = (cost > cos(deg2rad(40)));
					float Radius = 3.5f * Scale_Rate;
					if (!IsOutArea && (Len < Radius) && IsFront) {
						if (CharaMove::GetInputControl().GetPADSPress(PADS::SHOT)) {
							m_CharaAction = EnumArmAnimType::Men;
						}
						else if (CharaMove::GetInputControl().GetPADSPress(PADS::ULT)) {
							m_CharaAction = EnumArmAnimType::Kote;
						}
						else if (false) {
							m_CharaAction = EnumArmAnimType::Dou;
						}
					}
				}
				m_HeartUp = std::max(m_HeartUp - 30.f / DrawParts->GetFps(), -5.f);
			}
			break;
			case EnumArmAnimType::Run:
			{
				if (IsOutArea || m_RunTime >= 1.25f) {
					m_CharaAction = EnumArmAnimType::Ready;
				}

				//つばぜり判定
				if (m_RunTime <= 0.025f) {
					CheckTsuba();
				}

				m_RunTime = std::max(m_RunTime + 1.f / DrawParts->GetFps(), 0.f);
				m_HeartUp = std::max(m_HeartUp + 0.5f * 30.f / DrawParts->GetFps(), 3.f);
			}
			break;
			case EnumArmAnimType::Men:
			case EnumArmAnimType::Kote:
			case EnumArmAnimType::Tsuki:
			{
				//技派生
				if (m_FrontAttackActionTime <= 0.06f) {//受付開始
					auto Action = m_CharaAction;
					if (m_FrontAttackActionTime <= 0.f) {//派生先がなく0秒になった
						m_CharaAction = EnumArmAnimType::Run;
					}
					switch (Action) {
					case EnumArmAnimType::Ready:
					case EnumArmAnimType::Run:
						break;
					case EnumArmAnimType::Men:
						if (false) {
							m_CharaAction = EnumArmAnimType::Dou;//面胴
						}
						//小手面後派生
						if (false) {
							if (CharaMove::GetInputControl().GetPADSPress(PADS::SHOT)) {
								m_CharaAction = EnumArmAnimType::Men;//小手面面
							}
							if (false) {
								m_CharaAction = EnumArmAnimType::Dou;//小手面胴
							}
						}
						break;
					case EnumArmAnimType::Kote:
						if (CharaMove::GetInputControl().GetPADSPress(PADS::SHOT)) {
							m_CharaAction = EnumArmAnimType::Men;//小手面
						}
						if (false) {
							m_CharaAction = EnumArmAnimType::Dou;//小手胴
						}
						break;
					case EnumArmAnimType::Dou:
						break;
					case EnumArmAnimType::Tsuki:
						if (CharaMove::GetInputControl().GetPADSPress(PADS::SHOT)) {
							m_CharaAction = EnumArmAnimType::Men;//突き面
						}
						break;
					case EnumArmAnimType::Tsuba:
					case EnumArmAnimType::HikiMen:
					case EnumArmAnimType::HikiKote:
					case EnumArmAnimType::HikiDou:
					case EnumArmAnimType::GuardStart:
					case EnumArmAnimType::GuardSuriage:
					case EnumArmAnimType::GuardLeft:
					case EnumArmAnimType::GuardRight:
					case EnumArmAnimType::Max:
					default:
						break;
					}
				}
				m_FrontAttackActionTime = std::max(m_FrontAttackActionTime - 1.f / DrawParts->GetFps(), 0.f);
				m_HeartUp = std::max(m_HeartUp + 0.1f * 30.f / DrawParts->GetFps(), 10.f);
			}
			break;
			case EnumArmAnimType::Dou:
			{
				if (m_FrontAttackActionTime <= 0.f) {
					m_CharaAction = EnumArmAnimType::Run;
				}
				m_FrontAttackActionTime = std::max(m_FrontAttackActionTime - 1.f / DrawParts->GetFps(), 0.f);
				m_HeartUp = std::max(m_HeartUp + 0.1f * 30.f / DrawParts->GetFps(), 10.f);
			}
			break;
			case EnumArmAnimType::Tsuba:
			{
				if (m_TsubaCoolDown <= 0.f) {
					if (CharaMove::GetInputControl().GetPADSPress(PADS::SHOT)) {
						m_CharaAction = EnumArmAnimType::HikiMen;
					}
					else if (CharaMove::GetInputControl().GetPADSPress(PADS::ULT)) {
						m_CharaAction = EnumArmAnimType::HikiKote;
					}
					else if (false) {
						m_CharaAction = EnumArmAnimType::HikiDou;
					}
				}
				m_TsubaCoolDown = std::max(m_TsubaCoolDown - 1.f / DrawParts->GetFps(), 0.f);
				m_HeartUp = std::max(m_HeartUp - 30.f / DrawParts->GetFps(), -5.f);
			}
			break;
			case EnumArmAnimType::HikiMen:
			case EnumArmAnimType::HikiKote:
			case EnumArmAnimType::HikiDou:
			{
				if (m_BackAttackActionTime <= 0.f) {
					m_CharaAction = EnumArmAnimType::Ready;
				}
				m_BackAttackActionTime = std::max(m_BackAttackActionTime - 1.f / DrawParts->GetFps(), 0.f);
				m_HeartUp = std::max(m_HeartUp + 0.1f * 30.f / DrawParts->GetFps(), 10.f);
			}
			break;
			case EnumArmAnimType::GuardStart:
			{
				Easing(&m_BambooVecBase, Vector2DX::zero(), 0.9f, EasingType::OutExpo);
				//
				bool suriage = false;
				bool Gleft = false;
				bool Gright = false;
				float length = m_GuardVecR.magnitude();
				if (length > 0.8f) {
					if (m_GuardVecR.y > 0.25f) {
						suriage = true;//上ガード
					}
					else {
						if (m_GuardVecR.x > 0.f) {
							Gright = true;
						}
						else {
							Gleft = true;
						}
					}
				}
				//
				if ((m_GuardStartTimer >= 0.1f && !CharaMove::GetInputControl().GetPADSPress(PADS::AIM)) || (m_GuardStartTimer == 1.f) || suriage || Gleft || Gright) {
					if (suriage) {
						m_CharaAction = EnumArmAnimType::GuardSuriage;//上ガード
					}
					else if (Gleft) {
						m_CharaAction = EnumArmAnimType::GuardLeft;//左ガード
					}
					else if (Gright) {
						m_CharaAction = EnumArmAnimType::GuardRight;//右ガード
					}
					else {
						m_CharaAction = EnumArmAnimType::Ready;
					}
				}
				m_GuardStartTimer = std::min(m_GuardStartTimer + 1.f / DrawParts->GetFps(), 1.f);
			}
			break;
			case EnumArmAnimType::GuardSuriage:
			{
				if (m_GuardTimer == 0.f) {
					m_CharaAction = EnumArmAnimType::Ready;
				}
				else {
					if (CharaMove::GetInputControl().GetPADSPress(PADS::SHOT)) {
						m_CharaAction = EnumArmAnimType::Men;
					}
				}
				m_GuardTimer = std::max(m_GuardTimer - 1.f / DrawParts->GetFps(), 0.f);
			}
			break;
			case EnumArmAnimType::GuardLeft:
			case EnumArmAnimType::GuardRight:
			{
				if (m_GuardTimer == 0.f) {
					m_CharaAction = EnumArmAnimType::Ready;
				}
				m_GuardTimer = std::max(m_GuardTimer - 1.f / DrawParts->GetFps(), 0.f);
			}
			break;
			case EnumArmAnimType::Max:
			default:
				break;
			}
			if (Prev != m_CharaAction) {
				//派生なので即切り替え
				if (
					(
						Prev == EnumArmAnimType::Men ||
						Prev == EnumArmAnimType::Kote ||
						Prev == EnumArmAnimType::Tsuki
						) &&
					(
						m_CharaAction == EnumArmAnimType::Men ||
						m_CharaAction == EnumArmAnimType::Kote ||
						m_CharaAction == EnumArmAnimType::Dou ||
						m_CharaAction == EnumArmAnimType::Tsuki
						)
					) {
					this->m_Arm[static_cast<size_t>(Prev)].Init(false);
					this->m_Arm[static_cast<size_t>(m_CharaAction)].Init(true);
				}


				OverrideAction();
			}
			CharaMove::UpdateKeyRad(this->m_move);
		}
		void			CharacterClass::ExecuteAction(void) noexcept {
			for (size_t index = 0; index < static_cast<size_t>(EnumArmAnimType::Max); ++index) {
				this->m_Arm[index].Execute((m_CharaAction == (EnumArmAnimType)index), 0.1f, 0.1f, IsGuardAction((EnumArmAnimType)index) ? 0.75f : 0.9f);
				//this->m_Arm[index].Init((m_CharaAction == (EnumArmAnimType)index));
			}
		}
		void			CharacterClass::ExecuteAnim(void) noexcept {
			//上半身アニメ演算
			CharaMove::SetCharaAnimeBufID(CharaObjAnimeID::Upper_Ready) = 1.f;
			CharaMove::SetCharaAnimeBufID(CharaObjAnimeID::Hand_Ready) = 1.f;
			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Turn), 0.5f);
			ObjectBaseClass::SetAnimOnce(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Attack), CharaMove::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_Run), CharaMove::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaMove::GetBottomWalkAnimSel()), CharaMove::GetVecFront());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_LeftStep), CharaMove::GetVecLeft());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_WalkBack), CharaMove::GetVecRear());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaObjAnimeID::Bottom_Stand_RightStep), CharaMove::GetVecRight());
			//アニメ反映
			for (int i = 0, Num = static_cast<int>(GetObj().GetAnimNum()); i < Num; ++i) {
				this->GetObj().SetAnim(i).SetPer(CharaMove::GetCharaAnimeBufID((CharaObjAnimeID)i));
			}
			GetObj().UpdateAnimAll();
		}
		void			CharacterClass::ExecuteSound(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			//足音
			if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
				auto Time = this->GetObj().SetAnim(static_cast<int>(this->m_BottomAnimSelect)).GetTime();
				if (!GetRun()) {
					//L
					if ((9.f < Time && Time < 10.f)) {
						if (this->m_CharaSound != 1) {
							this->m_CharaSound = 1;
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {

								SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::LeftFoot), Scale_Rate * 5.f);
							}
						}
					}
					//R
					if ((27.f < Time && Time < 28.f)) {
						if (this->m_CharaSound != 3) {
							this->m_CharaSound = 3;
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {

								SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::RightFoot), Scale_Rate * 5.f);
							}
						}
					}
				}
				else {
					if (CharaMove::GetCharaAnimeBufID(CharaObjAnimeID::Bottom_Stand_Attack) > 0.5f) {
						if ((9.f < Time && Time < 10.f)) {
							if (this->m_CharaSound != 7) {
								this->m_CharaSound = 7;
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {
									SE->Get(static_cast<int>(SoundEnum::Kendo_Foot)).Play_3D(0, GetFramePosition(CharaFrame::RightFoot), Scale_Rate * 15.f);
								}
							}
						}
					}
					else {
						//L
						if ((3.f < Time && Time < 4.f)) {
							if (this->m_CharaSound != 5) {
								this->m_CharaSound = 5;
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {
									SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::LeftFoot), Scale_Rate * 15.f);
								}
							}
						}
						//R
						if ((10.f < Time && Time < 11.f)							) {
							if (this->m_CharaSound != 6) {
								this->m_CharaSound = 6;
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {
									SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::RightFoot), Scale_Rate * 15.f);
								}
							}
						}
					}
				}
			}
			else {
				this->m_CharaSound = -1;
			}
			//しゃがみ音
			if (CharaMove::GetSquatSwitch()) {
				SE->Get(static_cast<int>(SoundEnum::StandupFoot)).Play_3D(0, GetFramePosition(CharaFrame::Upper), Scale_Rate * 3.f);
			}
			//心拍音
#ifdef DEBUG
			printfDx("HEART : %f\n", m_HeartUp);
#endif
			Easing(&m_HeartUpR, m_HeartUp, 0.99f, EasingType::OutExpo);
			if (StaminaControl::ExcuteStamina(m_HeartUpR / DrawParts->GetFps())) {
				if (this->m_MyID == this->m_ViewID) {
					SE->Get(static_cast<int>(SoundEnum::Heart)).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
			}
		}
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			//
			float XRad = 0.f;
			this->m_move.GetMat().GetRadian(&XRad, nullptr, nullptr);
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)),
				(Matrix3x3DX::RotAxis(Vector3DX::right(), -XRad / 2.f) * CharaMove::GetUpperRotMatrix()).Get44DX() *
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper2)),
				Matrix3x3DX::RotAxis(Vector3DX::right(), XRad / 2.f).Get44DX() *
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper2)));
			//
			Vector3DX pos = this->m_move.GetPosBuf();
			//vector
			bool IsHitGround = false;//(pos.y <= 0.f); //高度0なら止まる
			float groundYpos = 0.f;
			{
				Vector3DX EndPos = pos - Vector3DX::up() * 5.f;
				IsHitGround = BackGround->CheckLinetoMap(pos + Vector3DX::up() * 20.f, &EndPos, true);
				groundYpos = EndPos.y;
			}
			Vector3DX vec = CharaMove::GetVec();
			if (IsHitGround) {
				auto yPos = pos.y;
				Easing(&yPos, groundYpos, 0.8f, EasingType::OutExpo);
				pos.y = (yPos);
				vec.y = (0.f);
			}
			else {
				vec.y = (this->m_move.GetVec().y + (M_GR / (DrawParts->GetFps() * DrawParts->GetFps())));
			}
			this->m_move.SetVec(vec);
			pos += this->m_move.GetVec();
			if (m_CharaAction == EnumArmAnimType::Tsuba) {
				auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetChara();

				auto& TgtPos = Target->GetMove().GetPos();
				auto& MyPos = this->GetMove().GetPos();

				auto Vec = (TgtPos - MyPos); Vec.y = (0.f); Vec = Vec.normalized();

				float Radius = (0.5f + 0.25f) * Scale_Rate;
				pos = TgtPos - Vec * Radius;
			}
			//壁判定
			BackGround->CheckMapWall(this->m_move.GetRePos(), &pos, 0.6f * Scale_Rate);
			//ほかプレイヤーとの判定
			{
				float Radius = 0.5f * Scale_Rate;
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
					if (i == this->m_MyID) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
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
			UpdateObjMatrix(CharaMove::GetBaseRotMatrix(), this->m_move.GetPos());
			{
				Matrix3x3DX Mat; Mat.SetRadian(CharaMove::GetRadBuf().x, CharaMove::GetRadBuf().y, 0.f);
				this->m_move.SetMat(Mat);
			}
			m_EyePosition = (GetFramePosition(CharaFrame::LeftEye) + GetFramePosition(CharaFrame::RightEye)) / 2.f + (CharaMove::GetEyeRotMatrix() * CharaMove::GetBaseRotMatrix()).zvec() * (-0.04f * Scale_Rate);
			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((this->m_MyID != this->m_ViewID) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			if (HeadBobbing) {
				m_EyePosition += CharaMove::GetEyeSwingPos();
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
						WeaponBaseMat = Lerp(WeaponBaseMat, AnimData, this->m_Arm[i].Per());
					}
					Vector3DX PrevPos = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::End);

					Vector3DX WeaponBasePos = GetFramePosition(CharaFrame::Head) + Matrix3x3DX::Vtrans(WeaponBaseMat.pos(), (CharaMove::GetUpperRotMatrix() * CharaMove::GetBaseRotMatrix()));
					auto WeaponBaseRotMat = Matrix3x3DX::RotVec2(Vector3DX::forward(), WeaponBaseMat.zvec());
					WeaponBaseRotMat *= Matrix3x3DX::RotVec2(WeaponBaseRotMat.yvec(), WeaponBaseMat.yvec());
					WeaponBaseRotMat *= CharaMove::GetWeaponSwingMat() * (CharaMove::GetUpperRotMatrix() * CharaMove::GetBaseRotMatrix());
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
						float Radius = 0.025f * Scale_Rate * 2.f;
						Vector3DX HitPos;
						for (int i = 0; i < PlayerMngr->GetPlayerNum(); ++i) {
							if (i == this->m_MyID) { continue; }
							auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
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
										SE->Get(static_cast<int>(SoundEnum::Kendo_Tsuba)).Play_3D(0, GetFramePosition(CharaFrame::RightWrist), Scale_Rate * 15.f, static_cast<int>(128.f * this->m_Weapon_Ptr->GetMoveSpeed()));
										m_TsubaSoundCoolTime = static_cast<float>(SE->Get(static_cast<int>(SoundEnum::Kendo_Tsuba)).GetTotalTIme(0, 0)) / 1000.f * 0.5f;
									}
									WeaponAddRotMat = Matrix3x3DX::RotAxis(Vector3DX::right(), m_BambooVec.x) * Matrix3x3DX::RotAxis(Vector3DX::forward(), m_BambooVec.y);
									if (IsGuardAction(m_CharaAction) || IsAttacking()) {
										if (m_GuardHit == 0.f) {
#ifdef _USE_EFFEKSEER_
											EffectControl::SetOnce(EffectResource::Effect::ef_gndsmoke, HitPos, (c->GetEyePosition() - GetEyePosition()).normalized(), 0.1f * Scale_Rate);
#endif
										}
										m_GuardHit = 3.f / Frame_Rate;
									}
									break;
							}
						}
						if (IsHit) {
							break;
						}
					}
					this->m_Weapon_Ptr->ResetMove(WeaponAddRotMat * WeaponBaseRotMat, WeaponBasePos);

					this->m_Weapon_Ptr->SetMoveSpeed((PrevPos - this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::End)).magnitude()* DrawParts->GetFps() / Frame_Rate);
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
