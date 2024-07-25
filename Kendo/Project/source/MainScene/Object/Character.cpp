#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		void			CharacterClass::PlayVoice(void) const noexcept {
			auto* SE = SoundPool::Instance();
			switch (m_CharaAction) {
			case EnumWeaponAnimType::Ready:
				break;
			case EnumWeaponAnimType::Run:
				break;
			case EnumWeaponAnimType::Men:
			case EnumWeaponAnimType::HikiMen:
				SE->Get(static_cast<int>(SoundEnum::Voice_Men)).Play_3D(0, GetEyePosition(), Scale_Rate * 35.f);
				break;
			case EnumWeaponAnimType::Kote:
			case EnumWeaponAnimType::HikiKote:
				SE->Get(static_cast<int>(SoundEnum::Voice_Kote)).Play_3D(0, GetEyePosition(), Scale_Rate * 35.f);
				break;
			case EnumWeaponAnimType::Dou:
			case EnumWeaponAnimType::HikiDou:
				SE->Get(static_cast<int>(SoundEnum::Voice_Dou)).Play_3D(0, GetEyePosition(), Scale_Rate * 35.f);
				break;
			case EnumWeaponAnimType::Tsuki:
				SE->Get(static_cast<int>(SoundEnum::Voice_Tsuki)).Play_3D(0, GetEyePosition(), Scale_Rate * 35.f);
				break;
			case EnumWeaponAnimType::Tsuba:
				break;
			case EnumWeaponAnimType::GuardStart:
				break;
			case EnumWeaponAnimType::GuardSuriage:
				break;
			case EnumWeaponAnimType::Max:
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

			auto& TgtPos = Target->GetMove().GetPos();
			auto& MyPos = this->GetMove().GetPos();

			float Len = 0.f;
			auto Vec = (TgtPos - MyPos); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();

			auto Dir = this->GetEyeMatrix().zvec() * -1.f; Dir.y = (0.f); Dir = Dir.normalized();

			auto IsFront = ((Vector3DX::Dot(Dir, Vec)) > 0.f);
			auto cross = Vector3DX::Cross(Dir, Vec).y;

			float Radius = (0.25f + 0.25f) * Scale_Rate;
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
		//
		void			CharacterClass::OverrideAction(void) noexcept {
			this->m_Weapon_Ptr->ResetAnim();
			//this->m_Weapon_Ptr->SetAnim();
			switch (m_CharaAction) {
			case EnumWeaponAnimType::Ready:
				OverrideReady();
				break;
			case EnumWeaponAnimType::Run:
				OverrideRun();
				break;
			case EnumWeaponAnimType::Men:
			case EnumWeaponAnimType::Kote:
			case EnumWeaponAnimType::Tsuki:
				OverrideFrontAttack();
				break;
			case EnumWeaponAnimType::Dou:
				OverrideDo();
				break;
			case EnumWeaponAnimType::Tsuba:
				OverrideTsuba();
				break;
			case EnumWeaponAnimType::HikiMen:
			case EnumWeaponAnimType::HikiKote:
			case EnumWeaponAnimType::HikiDou:
				OverrideBackAttack();
				break;
			case EnumWeaponAnimType::GuardStart:
				OverrideGuardStart();
				break;
			case EnumWeaponAnimType::GuardSuriage:
				OverrideGuardSuriage();
				break;
			case EnumWeaponAnimType::Max:
				break;
			default:
				break;
			}
		}
		void			CharacterClass::OverrideReady(void) noexcept {
			SetIsFrontAttacking(false);
			SetIsDouAttacking(false);
			SetIsBackAttacking(false);
			KeyControl::SetIsRunning(false);
			m_CharaAction = EnumWeaponAnimType::Ready;
		}
		void			CharacterClass::OverrideRun(void) noexcept {
			SetIsFrontAttacking(false);
			SetIsDouAttacking(false);
			SetIsBackAttacking(false);
			KeyControl::SetIsRunning(true);
			m_RunTime = 0.f;
		}
		void			CharacterClass::OverrideFrontAttack(void) noexcept {
			m_FrontAttackActionTime = this->m_Weapon_Ptr->GetWeaponAnimeTotalTime(m_CharaAction);
			SetIsFrontAttacking(true);
			SetIsDouAttacking(false);
			SetIsBackAttacking(false);
			this->GetObj().SetAnim(static_cast<int>(CharaAnimeID::Bottom_Stand_Attack)).GoStart();

			this->m_CharaSound = -1;
			auto* SE = SoundPool::Instance();
			SE->Get(static_cast<int>(SoundEnum::Kendo_Swing)).Play_3D(0, GetFramePosition(CharaFrame::RightWrist), Scale_Rate * 5.f);
			StopVoice();
			PlayVoice();
		}
		void			CharacterClass::OverrideDo(void) noexcept {
			m_FrontAttackActionTime = this->m_Weapon_Ptr->GetWeaponAnimeTotalTime(m_CharaAction);
			SetIsFrontAttacking(false);
			SetIsDouAttacking(true);
			SetIsBackAttacking(false);
			this->GetObj().SetAnim(static_cast<int>(CharaAnimeID::Bottom_Stand_Attack)).GoStart();

			this->m_CharaSound = -1;
			auto* SE = SoundPool::Instance();
			SE->Get(static_cast<int>(SoundEnum::Kendo_Swing)).Play_3D(0, GetFramePosition(CharaFrame::RightWrist), Scale_Rate * 5.f);
			StopVoice();
			PlayVoice();
		}
		void			CharacterClass::OverrideTsuba(void) noexcept {
			SetIsFrontAttacking(false);
			SetIsDouAttacking(false);
			SetIsBackAttacking(false);
			KeyControl::SetIsRunning(false);
			m_TsubaCoolDown = 0.5f;
			m_CharaAction = EnumWeaponAnimType::Tsuba;
		}
		void			CharacterClass::OverrideBackAttack(void) noexcept {
			m_BackAttackActionTime = std::max(0.75f, this->m_Weapon_Ptr->GetWeaponAnimeTotalTime(m_CharaAction));
			SetIsFrontAttacking(false);
			SetIsDouAttacking(true);
			SetIsBackAttacking(true);
			this->GetObj().SetAnim(static_cast<int>(CharaAnimeID::Bottom_Stand_Attack)).GoStart();

			this->m_CharaSound = -1;
			StopVoice();
			PlayVoice();

			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetChara();
			Target->OverrideReady();
		}
		void			CharacterClass::OverrideGuardStart(void) noexcept {
			m_GuardStartTimer = 0.f;
			m_GuardVec.Set(0.f, 0.f);
			m_GuardVecR.Set(0.f, 0.f);
		}
		void			CharacterClass::OverrideGuardSuriage(void) noexcept {
			m_BambooVecBase = Vector2DX::zero();
			m_GuardTimer = 1.f;
		}
		//
		Matrix3x3DX		CharacterClass::GetEyeMatrix(void) const noexcept { return KeyControl::GetEyeRotMatrix() * KeyControl::GetBaseRotMatrix(); }
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			if (this->m_MyID == value.DamageID) {
				//ダメージ
				return true;
			}
			return false;
		}
		//
		void			CharacterClass::MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept {
			KeyControl::InitKey(pxRad, pyRad);
			Matrix3x3DX Mat; Mat.SetRadian(KeyControl::GetRadBuf().x, KeyControl::GetRadBuf().y, 0.f);
			ResetMove(Mat, pPos);
			m_CharaAction = EnumWeaponAnimType::Ready;
			for (auto& a : this->m_Arm) { a.Init(false); }
			this->m_Arm[static_cast<int>(EnumWeaponAnimType::Ready)].Init(true);
			this->m_CharaSound = -1;
		}
		void			CharacterClass::Init_Sub(void) noexcept {
			m_HeartUp = 0.f;
			m_HeartUpR = 0.f;
			m_BambooVec.Set(0.f, 0.f);
			m_BambooVecBase.Set(0.f, 0.f);
			StaminaControl::InitStamina();
			this->m_MoveOverRideFlag = false;
		}
		//
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			InputControl Input = pInput;
			//ガード入力を反映
			m_GuardVec.x = std::clamp(m_GuardVec.x + Input.GetAddyRad() / 200.f, -1.5f, 1.5f);
			m_GuardVec.y = std::clamp(m_GuardVec.y + Input.GetAddxRad() / 200.f, -1.5f, 1.5f);
			Easing(&m_GuardVecR, m_GuardVec, 0.9f, EasingType::OutExpo);
			//竹刀の回転を反映
			Easing(&m_BambooVecBase, Vector2DX::vget(Input.GetxRad(), Input.GetyRad()), 0.7f, EasingType::OutExpo);
			//自動で相手を向く
			{
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();

				auto Dir = this->GetEyeMatrix().zvec() * -1.f; Dir.y = (0.f); Dir = Dir.normalized();
				float Len = 0.f;
				auto Vec = (Target->GetMove().GetPos() - this->GetEyePosition()); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();
				float pp_y = 0.f;
				if (!KeyControl::GetRun()) {
					float sint = Vector3DX::Cross(Vec, Dir).y;
					float cost = Vector3DX::Dot(Vec, Dir);
					auto IsFront = (cost > cos(deg2rad(40)));
					if (IsFront) {
						pp_y = std::clamp(-std::atan2f(sint, cost), -deg2rad(60), deg2rad(60)) * 5.f / 60.f;
					}
					else {
						pp_y = -deg2rad(60) * 5.f / 60.f;
					}
				}
				Input.SetAddxRad(0.f);
				Input.SetAddyRad(pp_y);
			}
			//
			if (!pReady) {
				Input.ResetKeyInput();
			}
			KeyControl::InputKey(Input);
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				for (int i = 0, num = MV1GetMaterialNum(GetObj().GetHandle()); i < num; i++) {
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
			if ((m_CharaAction == EnumWeaponAnimType::Ready) && m_YaTimer <= 0.f && KeyControl::GetInputControl().GetPADSPress(PADS::JUMP)) {
				SE->Get(static_cast<int>(SoundEnum::Voice_Ya)).Play_3D(0, GetEyePosition(), Scale_Rate * 35.f);
				m_YaTimer = 15.f;
			}
			m_YaTimer = std::max(m_YaTimer - 1.f / DrawParts->GetFps(), 0.f);
			//
			auto Prev = m_CharaAction;
			switch (m_CharaAction) {
			case EnumWeaponAnimType::Ready:
			{
				//つばぜり判定
				{
					CheckTsuba();
				}
				if (KeyControl::GetInputControl().GetPADSPress(PADS::AIM)) {
					m_CharaAction = EnumWeaponAnimType::GuardStart;
				}
				{
					auto* PlayerMngr = Player::PlayerManager::Instance();
					auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();

					auto& TgtPos = Target->GetMove().GetPos();
					auto& MyPos = this->GetMove().GetPos();

					float Len = 0.f;
					auto Vec = (TgtPos - MyPos); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();

					float Radius = 3.5f * Scale_Rate;
					if (!IsOutArea && (Len < Radius)) {
						if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
							m_CharaAction = EnumWeaponAnimType::Men;
						}
						else if (KeyControl::GetInputControl().GetPADSPress(PADS::ULT)) {
							m_CharaAction = EnumWeaponAnimType::Kote;
						}
						else if (false) {
							m_CharaAction = EnumWeaponAnimType::Dou;
						}
					}
				}
				m_HeartUp = std::max(m_HeartUp - 30.f / DrawParts->GetFps(), -5.f);
			}
			break;
			case EnumWeaponAnimType::Run:
			{
				if (IsOutArea || m_RunTime >= 2.f) {
					m_CharaAction = EnumWeaponAnimType::Ready;
				}

				//つばぜり判定
				if (m_RunTime <= 0.025f) {
					CheckTsuba();
				}

				m_RunTime = std::max(m_RunTime + 1.f / DrawParts->GetFps(), 0.f);
				m_HeartUp = std::max(m_HeartUp, 2.f);
			}
			break;
			case EnumWeaponAnimType::Men:
			case EnumWeaponAnimType::Kote:
			case EnumWeaponAnimType::Tsuki:
			{
				Easing(&m_BambooVecBase, Vector2DX::zero(), 0.9f, EasingType::OutExpo);
				//技派生
				if (m_FrontAttackActionTime <= 0.25f) {//受付開始
					auto Action = m_CharaAction;
					if (m_FrontAttackActionTime <= 0.f) {//派生先がなく0秒になった
						m_CharaAction = EnumWeaponAnimType::Run;
					}
					switch (Action) {
					case EnumWeaponAnimType::Ready:
					case EnumWeaponAnimType::Run:
						break;
					case EnumWeaponAnimType::Men:
						if (false) {
							m_CharaAction = EnumWeaponAnimType::Dou;//面胴
						}
						//小手面後派生
						if (false) {
							if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
								m_CharaAction = EnumWeaponAnimType::Men;//小手面面
							}
							if (false) {
								m_CharaAction = EnumWeaponAnimType::Dou;//小手面胴
							}
						}
						break;
					case EnumWeaponAnimType::Kote:
						if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
							m_CharaAction = EnumWeaponAnimType::Men;//小手面
						}
						if (false) {
							m_CharaAction = EnumWeaponAnimType::Dou;//小手胴
						}
						break;
					case EnumWeaponAnimType::Dou:
						break;
					case EnumWeaponAnimType::Tsuki:
						if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
							m_CharaAction = EnumWeaponAnimType::Men;//突き面
						}
						break;
					case EnumWeaponAnimType::Tsuba:
					case EnumWeaponAnimType::HikiMen:
					case EnumWeaponAnimType::HikiKote:
					case EnumWeaponAnimType::HikiDou:
					case EnumWeaponAnimType::GuardStart:
					case EnumWeaponAnimType::GuardSuriage:
					case EnumWeaponAnimType::Max:
					default:
						break;
					}
				}
				m_FrontAttackActionTime = std::max(m_FrontAttackActionTime - 1.f / DrawParts->GetFps(), 0.f);
				m_HeartUp = std::max(m_HeartUp, 10.f);
			}
			break;
			case EnumWeaponAnimType::Dou:
			{
				if (m_FrontAttackActionTime <= 0.f) {
					m_CharaAction = EnumWeaponAnimType::Run;
				}
				m_FrontAttackActionTime = std::max(m_FrontAttackActionTime - 1.f / DrawParts->GetFps(), 0.f);
				m_HeartUp = std::max(m_HeartUp, 10.f);
			}
			break;
			case EnumWeaponAnimType::Tsuba:
			{
				if (m_TsubaCoolDown <= 0.f) {
					if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
						m_CharaAction = EnumWeaponAnimType::HikiMen;
					}
					else if (KeyControl::GetInputControl().GetPADSPress(PADS::ULT)) {
						m_CharaAction = EnumWeaponAnimType::HikiKote;
					}
					else if (false) {
						m_CharaAction = EnumWeaponAnimType::HikiDou;
					}
				}
				m_TsubaCoolDown = std::max(m_TsubaCoolDown - 1.f / DrawParts->GetFps(), 0.f);
				m_HeartUp = std::max(m_HeartUp - 30.f / DrawParts->GetFps(), -5.f);
			}
			break;
			case EnumWeaponAnimType::HikiMen:
			case EnumWeaponAnimType::HikiKote:
			case EnumWeaponAnimType::HikiDou:
			{
				Easing(&m_BambooVecBase, Vector2DX::zero(), 0.9f, EasingType::OutExpo);
				if (IsOutArea || m_BackAttackActionTime <= 0.f) {
					m_CharaAction = EnumWeaponAnimType::Ready;
				}
				m_BackAttackActionTime = std::max(m_BackAttackActionTime - 1.f / DrawParts->GetFps(), 0.f);
				m_HeartUp = std::max(m_HeartUp, 10.f);
			}
			break;
			case EnumWeaponAnimType::GuardStart:
			{
				Easing(&m_BambooVecBase, Vector2DX::zero(), 0.9f, EasingType::OutExpo);
				//
				bool suriage = false;
				float length = m_GuardVecR.magnitude();
				if (length > 0.8f) {
					if (m_GuardVecR.y > 0.7f) {
						suriage = true;//上ガード
					}
				}
				//
				if ((m_GuardStartTimer >= 0.1f && !KeyControl::GetInputControl().GetPADSPress(PADS::AIM)) || (m_GuardStartTimer == 1.f) || suriage) {
					if (suriage) {
						m_CharaAction = EnumWeaponAnimType::GuardSuriage;//上ガード
					}
					else {
						m_CharaAction = EnumWeaponAnimType::Ready;
					}
				}
				m_GuardStartTimer = std::min(m_GuardStartTimer + 1.f / DrawParts->GetFps(), 1.f);
			}
			break;
			case EnumWeaponAnimType::GuardSuriage:
			{
				if (m_GuardTimer == 0.f) {
					m_CharaAction = EnumWeaponAnimType::Ready;
				}
				else {
					if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
						m_CharaAction = EnumWeaponAnimType::Men;
					}
				}
				m_GuardTimer = std::max(m_GuardTimer - 1.f / DrawParts->GetFps(), 0.f);
			}
			break;
			case EnumWeaponAnimType::Max:
			default:
				break;
			}
			if (Prev != m_CharaAction) {
				//派生なので即切り替え
				if (
					(
						Prev == EnumWeaponAnimType::Men ||
						Prev == EnumWeaponAnimType::Kote ||
						Prev == EnumWeaponAnimType::Tsuki
						) &&
					(
						m_CharaAction == EnumWeaponAnimType::Men ||
						m_CharaAction == EnumWeaponAnimType::Kote ||
						m_CharaAction == EnumWeaponAnimType::Dou ||
						m_CharaAction == EnumWeaponAnimType::Tsuki
						)
					) {
					this->m_Arm[static_cast<size_t>(Prev)].Init(false);
					this->m_Arm[static_cast<size_t>(m_CharaAction)].Init(true);
				}


				OverrideAction();
			}
			KeyControl::UpdateKeyRad(this->m_move);
		}
		void			CharacterClass::ExecuteAction(void) noexcept {
			auto IsGuardAction = [](EnumWeaponAnimType value) {
				switch (value) {
				case EnumWeaponAnimType::GuardSuriage:
					return true;
				case EnumWeaponAnimType::Ready:
				case EnumWeaponAnimType::Run:
				case EnumWeaponAnimType::Men:
				case EnumWeaponAnimType::Kote:
				case EnumWeaponAnimType::Dou:
				case EnumWeaponAnimType::Tsuki:
				case EnumWeaponAnimType::Tsuba:
				case EnumWeaponAnimType::HikiMen:
				case EnumWeaponAnimType::HikiKote:
				case EnumWeaponAnimType::HikiDou:
				case EnumWeaponAnimType::GuardStart:
					//case EnumWeaponAnimType::GuardSuriage:
				case EnumWeaponAnimType::Max:
				default:
					return false;
				}
				};
			for (size_t index = 0; index < static_cast<size_t>(EnumWeaponAnimType::Max); index++) {
				this->m_Arm[index].Execute((m_CharaAction == (EnumWeaponAnimType)index), 0.1f, 0.1f, IsGuardAction((EnumWeaponAnimType)index) ? 0.75f : 0.9f);
				//this->m_Arm[index].Init((m_CharaAction == (EnumWeaponAnimType)index));
			}
		}
		void			CharacterClass::ExecuteAnim(void) noexcept {
			//上半身アニメ演算
			GetCharaAnimeBufID(CharaAnimeID::Upper_Ready) = 1.f;
			GetCharaAnimeBufID(CharaAnimeID::Hand_Ready) = 1.f;
			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop(static_cast<int>(KeyControl::GetBottomTurnAnimSel()), 0.5f);
			ObjectBaseClass::SetAnimOnce(static_cast<int>(CharaAnimeID::Bottom_Stand_Attack), KeyControl::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaAnimeID::Bottom_Stand_Run), KeyControl::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(KeyControl::GetBottomWalkAnimSel()), KeyControl::GetVecFront());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(KeyControl::GetBottomLeftStepAnimSel()), KeyControl::GetVecLeft());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(KeyControl::GetBottomWalkBackAnimSel()), KeyControl::GetVecRear());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(KeyControl::GetBottomRightStepAnimSel()), KeyControl::GetVecRight());
			//アニメ反映
			for (int i = 0; i < static_cast<int>(GetObj().GetAnimNum()); i++) {
				this->GetObj().SetAnim(i).SetPer(GetCharaAnimeBufID((CharaAnimeID)i));
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
					if (GetCharaAnimeBufID(CharaAnimeID::Bottom_Stand_Attack) > 0.5f) {
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
						if (
							(3.f < Time && Time < 4.f)
							) {
							if (this->m_CharaSound != 5) {
								this->m_CharaSound = 5;
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale_Rate * 5.f * 1.5f) {
									SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::LeftFoot), Scale_Rate * 15.f);
								}
							}
						}
						//R
						if (
							(10.f < Time && Time < 11.f)
							) {
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
			if (KeyControl::GetSquatSwitch()) {
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
				Matrix4x4DX::RotAxis(Vector3DX::right(), -XRad / 2.f) * KeyControl::GetUpperRotMatrix().Get44DX() *
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper2)),
				Matrix4x4DX::RotAxis(Vector3DX::right(), XRad / 2.f) *
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
			Vector3DX vec = KeyControl::GetVec();
			if (IsHitGround) {
				auto yPos = pos.y;
				Easing(&yPos, groundYpos, 0.8f, EasingType::OutExpo);
				pos.y = (yPos);

				vec = KeyControl::GetVec();
				vec.y = (0.f);
			}
			else {
				vec = KeyControl::GetVec();
				vec.y = (this->m_move.GetVec().y + (M_GR / (DrawParts->GetFps() * DrawParts->GetFps())));
			}
			this->m_move.SetVec(vec);
			pos += this->m_move.GetVec();
			if (m_CharaAction == EnumWeaponAnimType::Tsuba) {
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
			if (true) {
				float Radius = 0.5f * Scale_Rate;
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
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
			UpdateObjMatrix(KeyControl::GetBaseRotMatrix(), this->m_move.GetPos());
			{
				Matrix3x3DX Mat; Mat.SetRadian(KeyControl::GetRadBuf().x, KeyControl::GetRadBuf().y, 0.f);
				this->m_move.SetMat(Mat);
			}
			m_EyePosition = (GetFramePosition(CharaFrame::LeftEye) + GetFramePosition(CharaFrame::RightEye)) / 2.f + (KeyControl::GetEyeRotMatrix() * KeyControl::GetBaseRotMatrix()).zvec() * (-0.04f * Scale_Rate);
			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((this->m_MyID != this->m_ViewID) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			if (HeadBobbing) {
				m_EyePosition += KeyControl::GetEyeSwingPos();
			}
			//銃座標指定
			if (this->m_Weapon_Ptr) {

				//銃の位置を指定するアニメ
				{
					//
					Matrix4x4DX AnimMat;
					for (size_t i = 0; i < static_cast<size_t>(EnumWeaponAnimType::Max); i++) {
						Matrix4x4DX AnimData;
						if (!this->m_Weapon_Ptr->GetWeaponAnime((EnumWeaponAnimType)i, &AnimData)) { continue; }
						AnimMat = Lerp(AnimMat, AnimData, this->m_Arm[i].Per());
					}
					Vector3DX WeaponBasePos = GetFramePosition(CharaFrame::Head) + Matrix3x3DX::Vtrans(AnimMat.pos(), (KeyControl::GetUpperRotMatrix() * KeyControl::GetBaseRotMatrix()));
					auto WeaponBaseRotMat = Matrix3x3DX::RotVec2(Vector3DX::forward(), AnimMat.zvec());
					WeaponBaseRotMat *= Matrix3x3DX::RotVec2(WeaponBaseRotMat.yvec(), AnimMat.yvec());
					WeaponBaseRotMat *= KeyControl::GetWeaponSwingMat() * (KeyControl::GetUpperRotMatrix() * KeyControl::GetBaseRotMatrix());
					//竹刀同士の判定
					Matrix3x3DX WeaponAddRotMat;
					Vector2DX Prev = m_BambooVec;
					for (int index = 0; index < 10; index++) {
						m_BambooVec = Lerp(Prev, m_BambooVecBase, static_cast<float>(index + 1) / 10.f);
						WeaponAddRotMat = Matrix3x3DX::RotAxis(Vector3DX::right(), m_BambooVec.x) * Matrix3x3DX::RotAxis(Vector3DX::forward(), m_BambooVec.y);
						this->m_Weapon_Ptr->UpdateObjMatrix(WeaponAddRotMat * WeaponBaseRotMat, WeaponBasePos);

						Vector3DX StartPos = WeaponBasePos;
						Vector3DX EndPos = this->m_Weapon_Ptr->GetFramePosition(WeaponObject::WeaponFrame::End);
						Vector3DX VecA = (EndPos - StartPos).normalized();
						bool IsHit = false;
						float Radius = 0.025f * Scale_Rate * 2.f;
						for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
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
									{
										Vector3DX Vec2 = Vec0 + (Vec1 - Vec0).normalized() * (Len - Radius);
										auto ZVec = Matrix4x4DX::RotVec2(VecA, (Vec2 - StartPos).normalized()).zvec();
										m_BambooVec.y += atan2f(ZVec.x, ZVec.z) * 0.5f;
										m_BambooVec.x += atan2f(ZVec.y, std::hypotf(ZVec.x, ZVec.z)) * 0.5f;
										//m_BambooVec.Set(std::clamp(m_BambooVec.x, deg2rad(-10), deg2rad(10)), std::clamp(m_BambooVec.y, deg2rad(-30), deg2rad(30)));
									}
									{
										Vector3DX Vec2 = Vec1 + (Vec0 - Vec1).normalized() * (Len - Radius);
										auto ZVec = Matrix4x4DX::RotVec2(VecB, (Vec2 - StartPosB).normalized()).zvec();
										c->m_BambooVec.y += atan2f(ZVec.x, ZVec.z) * 1.5f;
										c->m_BambooVec.x += atan2f(ZVec.y, std::hypotf(ZVec.x, ZVec.z)) * 1.5f;
									}
									IsHit = true;
									break;
							}
						}
						if (IsHit) {
							WeaponAddRotMat = Matrix3x3DX::RotAxis(Vector3DX::right(), m_BambooVec.x) * Matrix3x3DX::RotAxis(Vector3DX::forward(), m_BambooVec.y);
							break;
						}
					}
					this->m_Weapon_Ptr->ResetMove(WeaponAddRotMat * WeaponBaseRotMat, WeaponBasePos);
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
