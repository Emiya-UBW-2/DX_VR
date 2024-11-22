#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"

#include	"../../sub.hpp"
#include	"../../MainScene/NetWork.hpp"
#include	"CharaAnimData.hpp"
#include	"CharacterEnum.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		//�L�����̂�������@�\���������o��������
		//
		class ArmMovePerClass {
			float												m_ArmPer{ 0.f };
			bool												m_Armon{ false };
		public:
			void Init(bool isOn) noexcept {
				m_Armon = isOn;
				m_ArmPer = isOn ? 1.f : 0.f;
			}
			void Update(bool isOn, float OnOver = 0.2f, float OffOver = 0.2f, float Per = 0.8f) noexcept {
				if (isOn) {
					if (m_Armon) {
						Easing(&this->m_ArmPer, 1.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&this->m_ArmPer, 1.f + OnOver, Per, EasingType::OutExpo);
						if (this->m_ArmPer >= 1.f + OnOver / 2.f) {
							m_Armon = true;
						}
					}
				}
				else {
					if (!m_Armon) {
						Easing(&this->m_ArmPer, 0.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&this->m_ArmPer, 0.f - OffOver, Per, EasingType::OutExpo);
						if (this->m_ArmPer <= 0.f - OffOver / 2.f) {
							m_Armon = false;
						}
					}
				}
			}
		public:
			const auto& Per(void) const noexcept { return m_ArmPer; }
		};
		class CharaMove {
		private://�L�����p�����[�^�[
			const float											HeartRateMin{ 60.f };//�S�����ŏ�
			const float											HeartRateStamona{ 100.f };//�S�������郉�C��
			const float											HeartRateMax{ 150.f };//�S�����ő�
			const float											StaminaMax{ 30.f };
			const float											YaTimerMax{ 10.f };
			const float											GuardCoolDownTimerMax{ 4.f };
		public:
			static const bool			IsAttackAction(EnumArmAnimType value) noexcept {
				switch (value) {
				case EnumArmAnimType::Men:
				case EnumArmAnimType::Kote:
				case EnumArmAnimType::Dou:
				case EnumArmAnimType::Tsuki:
				case EnumArmAnimType::HikiMen:
				case EnumArmAnimType::HikiKote:
				case EnumArmAnimType::HikiDou:
					return true;
				case EnumArmAnimType::Ready:
				case EnumArmAnimType::Run:
				case EnumArmAnimType::Tsuba:
				case EnumArmAnimType::GuardSuriage:
				case EnumArmAnimType::Max:
				default:
					return false;
				}
			}
			static const bool			IsGuardAction(EnumArmAnimType value) noexcept {
				switch (value) {
				case EnumArmAnimType::GuardSuriage:
					return true;
				case EnumArmAnimType::Ready:
				case EnumArmAnimType::Run:
				case EnumArmAnimType::Men:
				case EnumArmAnimType::Kote:
				case EnumArmAnimType::Dou:
				case EnumArmAnimType::Tsuki:
				case EnumArmAnimType::Tsuba:
				case EnumArmAnimType::HikiMen:
				case EnumArmAnimType::HikiKote:
				case EnumArmAnimType::HikiDou:
				case EnumArmAnimType::Max:
				default:
					return false;
				}
			}
		public:
			bool												m_CanFrontAttack{ false };
			float												m_RunTime{ 0.f };
			WazaType											m_WazaType{ WazaType::Men };
		private:
			float												m_HeartRate{ HeartRateMin };//�S����
			float												m_HeartRate_r{ HeartRateMin };//�S����
			float												m_HeartRateRad{ 0.f };//�ċzSin�n��
			float												m_HeartRatePow{ 0.f };//�S����
			bool												m_HeartSoundFlag{ false };
			bool												m_HeartSoundSwitch{ false };
			float												m_HeartUpR{ 0.f };
			float												m_HeartUp{ 0.f };
			float												m_BreathRad{ 0.f };
			bool												m_BreathSoundFlag{ false };
			bool												m_BreathSoundSwitch{ false };
			float												m_Stamina{ StaminaMax };//�X�^�~�i
			bool												m_StaminaLoss{ false };//�X�^�~�i�؂�
			float												m_YaTimer{ 0.f };
			bool												m_YaSwitch{ false };
			float												m_GuardCoolDownTimer{ 0.f };
			float												m_FrontAttackActionTime{ 0.f };
			float												m_BackAttackActionTime{ 0.f };
			float												m_GuardTimer{ 0.f };
			float												m_TsubaCoolDown{ 0.f };
			float												m_MoverPer{ 0.f };
			Vector3DX											m_VecTotal;
			std::array<float, 4>								m_Vec{};
			InputControl										m_Input;
			float												m_Yrad_Buf{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_yrad_UpperChange{ 0.f }, m_yrad_BottomChange{ 0.f };
			float												m_Zrad_Buf{ 0.f };
			float												m_ZRad{};
			float												m_PrevRadY{ 0.f };
			Vector3DX											m_UpperyVecNormal, m_UpperzVecNormal;
			Vector3DX											m_UpperyVec, m_UpperzVec, m_UpperPos;
			std::array<float, static_cast<int>(CharaObjAnimeID::AnimeIDMax)>	m_AnimPerBuf{};
			bool												m_TurnBody{ false };
			float												m_RunPer{};
			bool												m_IsRunning{ false };
			bool												m_IsFrontAttacking{ false };
			bool												m_IsDouAttacking{ false };
			bool												m_IsBackAttacking{ false };
			Vector3DX											m_EyePosition;
			Matrix3x3DX											m_EyeMatrix;
			Matrix3x3DX											m_UpperMatrix;
			Matrix3x3DX											m_BaseMatrix;
			float												m_MoveEyePosTimer{ 0.f };
			Vector3DX											m_MoveEyePos;
			float												m_SpeedMul{ 1.f };
			CharaObjAnimeID										m_BottomAnimSelect{};
			EnumArmAnimType										m_CharaAction{ EnumArmAnimType::Ready };
			std::array<ArmMovePerClass, static_cast<int>(EnumArmAnimType::Max)>	m_Arm;
		private://�L�����p�����[�^�[
			const auto	IsMove(void) const noexcept { return m_MoverPer > 0.1f; }
			const auto	GetFrontP(void) const noexcept {
				auto wkey = (this->m_Input.GetPADSPress(Controls::PADS::MOVE_W) || (m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) && !m_IsBackAttacking;
				auto skey = (this->m_Input.GetPADSPress(Controls::PADS::MOVE_S) && !(m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) || m_IsBackAttacking;
				auto FrontP = (wkey && !skey) ? (atan2f(m_VecTotal.x, -m_VecTotal.z) * -m_VecTotal.z) : 0.f;
				FrontP += (!wkey && skey) ? (atan2f(-m_VecTotal.x, m_VecTotal.z) * m_VecTotal.z) : 0.f;
				return FrontP;
			}
		public://�Q�b�^�[
			const auto& GetBottomAnimSelect(void) const noexcept { return this->m_BottomAnimSelect; }
			const auto& GetEyePosition(void) const noexcept { return this->m_EyePosition; }
			const auto& GetUpperRotMatrix(void) const noexcept { return this->m_UpperMatrix; }
			const auto& GetBaseRotMatrix(void) const noexcept { return this->m_BaseMatrix; }
			const auto& GetInputControl(void) const noexcept { return this->m_Input; }
			const auto& GetHeartRate(void) const noexcept { return m_HeartRate; }
			const auto& GetHeartRatePow(void) const noexcept { return m_HeartRatePow; }
			const auto& GetHeartSwitch(void) const noexcept { return m_HeartSoundSwitch; }
			const auto& GetBreathSwitch(void) const noexcept { return m_BreathSoundSwitch; }
			const auto& GetStamina(void) const noexcept { return m_Stamina; }
			const auto& GetStaminaMax(void) const noexcept { return StaminaMax; }
			const auto& GetYaTimer(void) const noexcept { return m_YaTimer; }
			const auto& GetYaTimerMax(void) const noexcept { return YaTimerMax; }
			const auto& GetYaSwitch(void) const noexcept { return m_YaSwitch; }
			const auto& GetGuardCoolDownTimer(void) const noexcept { return m_GuardCoolDownTimer; }
			const auto& GetCharaAction(void) const noexcept { return this->m_CharaAction; }
			const auto& GetArmPer(EnumArmAnimType sel) const noexcept { return this->m_Arm[static_cast<size_t>(sel)].Per(); }
			const auto& GetCharaAnimeBufID(CharaObjAnimeID value) const noexcept { return this->m_AnimPerBuf.at(static_cast<size_t>(value)); }
			auto& SetCharaAnimeBufID(CharaObjAnimeID value) noexcept { return this->m_AnimPerBuf.at(static_cast<size_t>(value)); }
			const auto	GetRun(void) const noexcept { return this->m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking; }
			const auto	GetIsFrontAttacking(void) const noexcept { return this->m_IsFrontAttacking; }
			const auto	GetRadBuf(void) const noexcept { return this->m_Yrad_Buf; }
			const auto	GetEyeMatrix(void) const noexcept { return this->m_EyeMatrix * this->m_BaseMatrix; }
			const auto	GetUpperMatrix(void) const noexcept { return this->m_UpperMatrix * this->m_BaseMatrix; }
			const auto	GetWeaponSwingMat(void) const noexcept { return Matrix3x3DX::Axis1(m_UpperyVec.normalized(), m_UpperzVec.normalized()); }
			const auto	GetSpeedPer(void) const noexcept {
				if (this->m_Input.GetPADSPress(Controls::PADS::WALK)) {
					return 0.15f * this->m_SpeedMul;
				}
				else if (m_IsRunning) {
					return 0.975f;
				}
				else if (m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking) {
					return 1.175f;
				}
				else {
					return 0.45f * this->m_SpeedMul;
				}
			}
			const auto	GetVec(void) const noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				Vector3DX vecBuf = Matrix3x3DX::Vtrans(m_VecTotal, Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_yrad_Upper));
				if (m_MoverPer > 0.f) {
					vecBuf = vecBuf.normalized() * (GetSpeedPer() * FrameRate * DXLib_refParts->GetDeltaTime());
				}
				return vecBuf;
			}
			const auto	GetBottomStandAnimSel(void) const noexcept { return CharaObjAnimeID::Bottom_Stand; }
			const auto	GetBottomWalkAnimSel(void) const noexcept { return CharaObjAnimeID::Bottom_Stand_Walk; }
			const auto	GetVecFront(void) const noexcept { return 1.15f * this->m_Vec[0] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
			const auto	GetVecRear(void) const noexcept { return 1.15f * this->m_Vec[2] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
			const auto	GetVecLeft(void) const noexcept { return 1.15f * this->m_Vec[1] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
			const auto	GetVecRight(void) const noexcept { return 1.15f * this->m_Vec[3] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
			const auto	GetGuardCoolDownTimerMax(void) const noexcept { return GuardCoolDownTimerMax; }
			const auto	GetStaminaPer(void) const noexcept { return GetStamina() / GetStaminaMax(); }
			const auto	GetYaTimerPer(void) const noexcept { return GetYaTimer() / GetYaTimerMax(); }
			const auto	GetGuardCoolDownTimerPer(void) const noexcept { return GetGuardCoolDownTimer() / GetGuardCoolDownTimerMax(); }
		public://�Z�b�^�[
			void			SetEyePosition(const Vector3DX& Pos)noexcept {
				m_EyePosition = Pos;
				auto* OptionParts = OptionManager::Instance();
				bool HeadBobbing = OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing);
				if (HeadBobbing) {
					m_EyePosition += this->m_MoveEyePos;
				}
			}
			void			SetActionOverRide(EnumArmAnimType o) noexcept { this->m_CharaAction = o; }
			void			ChangeAction(EnumArmAnimType SetAction) noexcept {
				if (
					(
						m_CharaAction == EnumArmAnimType::Men ||
						m_CharaAction == EnumArmAnimType::Kote ||
						m_CharaAction == EnumArmAnimType::Tsuki
						) &&
					(
						SetAction == EnumArmAnimType::Men ||
						SetAction == EnumArmAnimType::Kote ||
						SetAction == EnumArmAnimType::Dou ||
						SetAction == EnumArmAnimType::Tsuki
						)
					) {
					this->m_Arm[static_cast<size_t>(m_CharaAction)].Init(false);
					this->m_Arm[static_cast<size_t>(SetAction)].Init(true);
				}
				m_CharaAction = SetAction;
			}
			void			PlayVoice(void) const noexcept {
				auto* SE = SoundPool::Instance();
				switch (this->m_CharaAction) {
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
			void			StopVoice(void) const noexcept {
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Men))->StopAll();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Kote))->StopAll();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Dou))->StopAll();
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Tsuki))->StopAll();
			}
			void			OverrideReady(void) noexcept {
				this->ChangeAction(EnumArmAnimType::Ready);
				this->m_IsFrontAttacking = false;
				this->m_IsDouAttacking = false;
				this->m_IsBackAttacking = false;
				this->m_IsRunning = false;
			}
			void			OverrideRun(void) noexcept {
				this->m_RunTime = 0.f;
				this->m_IsFrontAttacking = false;
				this->m_IsDouAttacking = false;
				this->m_IsBackAttacking = false;
				this->m_IsRunning = true;
			}
			void			OverrideTsuba(void) noexcept {
				this->m_TsubaCoolDown = 0.5f;
				this->ChangeAction(EnumArmAnimType::Tsuba);
				this->m_IsFrontAttacking = false;
				this->m_IsDouAttacking = false;
				this->m_IsBackAttacking = false;
				this->m_IsRunning = false;
			}
			void			OverrideFrontAttack(float ActionTime) noexcept {
				m_FrontAttackActionTime = ActionTime;
				this->m_IsFrontAttacking = true;
				this->m_IsDouAttacking = false;
				this->m_IsBackAttacking = false;
				StopVoice();
				PlayVoice();
			}
			void			OverrideDo(float ActionTime) noexcept {
				m_FrontAttackActionTime = ActionTime;
				this->m_IsFrontAttacking = false;
				this->m_IsDouAttacking = true;
				this->m_IsBackAttacking = false;
				StopVoice();
				PlayVoice();
			}
			void			OverrideBackAttack(float ActionTime) noexcept {
				m_BackAttackActionTime = ActionTime;
				this->m_IsFrontAttacking = false;
				this->m_IsDouAttacking = false;
				this->m_IsBackAttacking = true;
				StopVoice();
				PlayVoice();
			}
			void			OverrideGuard(void) noexcept {
				m_GuardTimer = 1.f;
			}
		public:
			void			Init(float pyRad) noexcept {
				for (size_t i = 0; i < 4; ++i) {
					this->m_Vec[i] = 0.f;
				}
				m_VecTotal = Vector3DX::zero();
				this->m_Input.ResetAllInput();
				this->m_Yrad_Buf = pyRad;
				this->m_Zrad_Buf = 0.f;

				this->m_yrad_Upper = this->m_Yrad_Buf;
				this->m_yrad_Bottom = this->m_Yrad_Buf;
				this->m_PrevRadY = this->m_Yrad_Buf;
				this->m_yrad_BottomChange = 0.f;
				this->m_TurnBody = false;
				//
				for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
				this->m_MoveEyePosTimer = 0.f;
				//
				this->m_HeartRate = HeartRateMin;
				this->m_HeartRateRad = 0.f;
				this->m_Stamina = GetStaminaMax();
				this->m_HeartSoundFlag = false;
				this->m_HeartUp = 0.f;
				this->m_HeartUpR = 0.f;
				this->m_YaTimer = 0.f;
				//
				OverrideReady();
				//
				for (auto& a : this->m_Arm) { a.Init(false); }
				this->m_Arm[static_cast<int>(EnumArmAnimType::Ready)].Init(true);
			}
			void			InputKey(const InputControl& pInput, float pyRadAdd) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				this->m_Input = pInput;
				//��]
				{
					this->m_Yrad_Buf = this->m_Yrad_Buf + pyRadAdd;
				}
				Easing(&m_RunPer, m_IsRunning ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				//�ړ�
				auto wkey = (this->m_Input.GetPADSPress(Controls::PADS::MOVE_W) || (m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) && !m_IsBackAttacking;
				auto skey = (this->m_Input.GetPADSPress(Controls::PADS::MOVE_S) && !(m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) || m_IsBackAttacking;

				auto akey = this->m_Input.GetPADSPress(Controls::PADS::MOVE_A) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);
				auto dkey = this->m_Input.GetPADSPress(Controls::PADS::MOVE_D) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);

				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (wkey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (akey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (skey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (dkey ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				if (m_IsRunning || m_IsDouAttacking) {
					this->m_Vec[3] = std::clamp(this->m_Vec[3] + 5.f * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				}
				m_VecTotal = Vector3DX::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]);
				m_MoverPer = m_VecTotal.magnitude();
			}
			EnumArmAnimType UpdateAction() noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				auto SetAction = this->m_CharaAction;
				switch (this->m_CharaAction) {
				case EnumArmAnimType::Ready:
				{
					if ((this->GetGuardCoolDownTimer() == 0.f) && this->GetInputControl().GetPADSPress(Controls::PADS::AIM)) {
						SetAction = EnumArmAnimType::GuardSuriage;//��K�[�h
						this->m_GuardCoolDownTimer = GetGuardCoolDownTimerMax();
					}
					if (m_CanFrontAttack) {
						if (this->GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
							SetAction = EnumArmAnimType::Men;
							this->m_WazaType = WazaType::Men;
						}
						else if (this->GetInputControl().GetPADSPress(Controls::PADS::ULT)) {
							SetAction = EnumArmAnimType::Kote;
							this->m_WazaType = WazaType::Kote;
						}
						else if (false) {
							SetAction = EnumArmAnimType::Dou;
							this->m_WazaType = WazaType::Dou;
						}
					}
					this->m_HeartUp = std::max(this->m_HeartUp - 30.f * DXLib_refParts->GetDeltaTime(), -5.f);
				}
				break;
				case EnumArmAnimType::Run:
				{
					if (this->m_RunTime >= 1.25f) {
						SetAction = EnumArmAnimType::Ready;
					}

					this->m_RunTime = std::max(this->m_RunTime + DXLib_refParts->GetDeltaTime(), 0.f);
					this->m_HeartUp = std::max(this->m_HeartUp + 0.5f * 30.f * DXLib_refParts->GetDeltaTime(), 3.f);
				}
				break;
				case EnumArmAnimType::Men:
				case EnumArmAnimType::Kote:
				case EnumArmAnimType::Tsuki:
				{
					//�Z�h��
					if (this->m_FrontAttackActionTime <= 0.06f) {//��t�J�n
						if (this->m_FrontAttackActionTime <= 0.f) {//�h���悪�Ȃ�0�b�ɂȂ���
							SetAction = EnumArmAnimType::Run;
						}
						switch (this->m_CharaAction) {
						case EnumArmAnimType::Ready:
						case EnumArmAnimType::Run:
							break;
						case EnumArmAnimType::Men:
							if (false) {
								SetAction = EnumArmAnimType::Dou;//�ʓ�
								this->m_WazaType = WazaType::Dou;
							}
							//����ʌ�h��
							if (false) {
								if (this->GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
									SetAction = EnumArmAnimType::Men;//����ʖ�
									this->m_WazaType = WazaType::Men;
								}
								if (false) {
									SetAction = EnumArmAnimType::Dou;//����ʓ�
									this->m_WazaType = WazaType::Dou;
								}
							}
							break;
						case EnumArmAnimType::Kote:
							if (this->GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
								SetAction = EnumArmAnimType::Men;//�����
								this->m_WazaType = WazaType::Men;
							}
							if (false) {
								SetAction = EnumArmAnimType::Dou;//���蓷
								this->m_WazaType = WazaType::Dou;
							}
							break;
						case EnumArmAnimType::Dou:
							break;
						case EnumArmAnimType::Tsuki:
							if (this->GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
								SetAction = EnumArmAnimType::Men;//�˂���
								this->m_WazaType = WazaType::Men;
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
					this->m_FrontAttackActionTime = std::max(this->m_FrontAttackActionTime - DXLib_refParts->GetDeltaTime(), 0.f);
					this->m_HeartUp = std::max(this->m_HeartUp + 0.1f * 30.f * DXLib_refParts->GetDeltaTime(), 10.f);
				}
				break;
				case EnumArmAnimType::Dou:
				{
					if (this->m_FrontAttackActionTime <= 0.f) {
						SetAction = EnumArmAnimType::Run;
					}
					this->m_FrontAttackActionTime = std::max(this->m_FrontAttackActionTime - DXLib_refParts->GetDeltaTime(), 0.f);
					this->m_HeartUp = std::max(this->m_HeartUp + 0.1f * 30.f * DXLib_refParts->GetDeltaTime(), 10.f);
				}
				break;
				case EnumArmAnimType::Tsuba:
				{
					if (this->m_TsubaCoolDown <= 0.f) {
						if (this->GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
							SetAction = EnumArmAnimType::HikiMen;
							this->m_WazaType = WazaType::Hikimen;
						}
						else if (this->GetInputControl().GetPADSPress(Controls::PADS::ULT)) {
							SetAction = EnumArmAnimType::HikiKote;
							this->m_WazaType = WazaType::Hikigote;
						}
						else if (false) {
							SetAction = EnumArmAnimType::HikiDou;
						}
					}
					this->m_TsubaCoolDown = std::max(this->m_TsubaCoolDown - DXLib_refParts->GetDeltaTime(), 0.f);
					this->m_HeartUp = std::max(this->m_HeartUp - 30.f * DXLib_refParts->GetDeltaTime(), -5.f);
				}
				break;
				case EnumArmAnimType::HikiMen:
				case EnumArmAnimType::HikiKote:
				case EnumArmAnimType::HikiDou:
				{
					if (this->m_BackAttackActionTime <= 0.f) {
						SetAction = EnumArmAnimType::Ready;
					}
					this->m_BackAttackActionTime = std::max(this->m_BackAttackActionTime - DXLib_refParts->GetDeltaTime(), 0.f);
					this->m_HeartUp = std::max(this->m_HeartUp + 0.1f * 30.f * DXLib_refParts->GetDeltaTime(), 10.f);
				}
				break;
				case EnumArmAnimType::GuardSuriage:
				{
					if (this->m_GuardTimer == 0.f) {
						SetAction = EnumArmAnimType::Ready;
					}
					else {
						if (this->GetInputControl().GetPADSPress(Controls::PADS::SHOT)) {
							SetAction = EnumArmAnimType::Men;
							this->m_WazaType = WazaType::SuriageMen;
						}
						else if (this->GetInputControl().GetPADSPress(Controls::PADS::ULT)) {
							SetAction = EnumArmAnimType::Dou;
							this->m_WazaType = WazaType::Dou;
						}
					}
					this->m_GuardTimer = std::max(this->m_GuardTimer - DXLib_refParts->GetDeltaTime(), 0.f);
					this->m_HeartUp = std::max(this->m_HeartUp - 60.f * DXLib_refParts->GetDeltaTime(), -15.f);
				}
				break;
				case EnumArmAnimType::Max:
				default:
					break;
				}
				return SetAction;
			}
			void			Update(const moves& move_t) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				auto* SE = SoundPool::Instance();
				{
					float YRad = 0.f;
					move_t.GetMatBuf().GetRadian(nullptr, &YRad, nullptr);
					//
					if (!IsMove()) {
						float yrad_UpperChange = YRad - this->m_yrad_Upper;
						if (deg2rad(50.f) < abs(yrad_UpperChange)) {
							this->m_TurnBody = true;
						}
						if (abs(yrad_UpperChange) < deg2rad(0.5f)) {
							this->m_TurnBody = false;
						}
					}
					else {
						this->m_TurnBody = false;
					}

					if (this->m_TurnBody || IsMove()) { this->m_yrad_Upper = YRad; }
					float YradChange = 0.f;
					{
						auto bottom = (this->m_yrad_Upper - GetFrontP());
						// �_��
						{
							Vector3DX Vec; Vec.Set(std::sin(this->m_yrad_Bottom), std::cos(this->m_yrad_Bottom), 0.f);
							Vector3DX vec_a; vec_a.Set(std::sin(bottom), std::cos(bottom), 0.f);
							float cost = Vector3DX::Cross(vec_a, Vec).z;
							float sint = Vector3DX::Dot(vec_a, Vec);
							YradChange = std::clamp(std::atan2f(cost, sint), deg2rad(-10), deg2rad(10)) * 30.f * DXLib_refParts->GetDeltaTime();
							this->m_yrad_Bottom += YradChange;

							if (this->m_yrad_Bottom < 0.f) { this->m_yrad_Bottom += DX_PI_F * 2.f; }
							if (this->m_yrad_Bottom > DX_PI_F * 2.f) { this->m_yrad_Bottom -= DX_PI_F * 2.f; }
						}
					}
					Easing(&this->m_Zrad_Buf, (abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
					Easing(&m_ZRad, this->m_Zrad_Buf, 0.5f, EasingType::OutExpo);
					{
						Vector3DX Vec; Vec.Set(std::sin(this->m_yrad_Bottom), std::cos(this->m_yrad_Bottom), 0.f);
						Vector3DX vec_a; vec_a.Set(std::sin(YRad), std::cos(YRad), 0.f);
						float cost = Vector3DX::Cross(vec_a, Vec).z;
						float sint = Vector3DX::Dot(vec_a, Vec);
						this->m_yrad_BottomChange = std::atan2f(cost, sint);
					}
					//�e�̗h��
					auto mat = Matrix3x3DX::RotAxis(Vector3DX::up(), (YRad - this->m_PrevRadY) * -1.f);
					this->m_PrevRadY = YRad;
					Easing(&m_UpperyVecNormal, mat.yvec(), 0.8f, EasingType::OutExpo);
					Easing(&m_UpperzVecNormal, mat.zvec(), 0.8f, EasingType::OutExpo);
					Easing(&m_UpperyVec, m_UpperyVecNormal, 0.8f, EasingType::OutExpo);
					Easing(&m_UpperzVec, m_UpperzVecNormal, 0.8f, EasingType::OutExpo);

					this->m_BottomAnimSelect = GetBottomStandAnimSel();
					auto wkey = this->m_Input.GetPADSPress(Controls::PADS::MOVE_W) || m_IsRunning;
					auto skey = this->m_Input.GetPADSPress(Controls::PADS::MOVE_S) && !m_IsRunning;

					auto akey = this->m_Input.GetPADSPress(Controls::PADS::MOVE_A) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);
					auto dkey = this->m_Input.GetPADSPress(Controls::PADS::MOVE_D) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);

					if (akey && !dkey) { this->m_BottomAnimSelect = CharaObjAnimeID::Bottom_Stand_LeftStep; }
					if (dkey && !akey) { this->m_BottomAnimSelect = CharaObjAnimeID::Bottom_Stand_RightStep; }
					if (skey && !wkey) { this->m_BottomAnimSelect = CharaObjAnimeID::Bottom_Stand_WalkBack; }
					if (wkey && !skey) { this->m_BottomAnimSelect = m_IsRunning ? CharaObjAnimeID::Bottom_Stand_Run : GetBottomWalkAnimSel(); }
					if (m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking) { this->m_BottomAnimSelect = CharaObjAnimeID::Bottom_Stand_Attack; }
					//�����g
					Easing(&CharaMove::SetCharaAnimeBufID(CharaObjAnimeID::Bottom_Stand_Turn), (this->m_TurnBody) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
					for (int i = 0; i < static_cast<int>(CharaObjAnimeID::AnimeIDMax); ++i) {
						if (
							i == static_cast<int>(CharaObjAnimeID::Bottom_Stand) ||
							i == static_cast<int>(CharaObjAnimeID::Bottom_Stand_Attack) ||
							i == static_cast<int>(CharaObjAnimeID::Bottom_Stand_Run) ||
							i == static_cast<int>(CharaObjAnimeID::Bottom_Stand_Walk) ||
							i == static_cast<int>(CharaObjAnimeID::Bottom_Stand_LeftStep) ||
							i == static_cast<int>(CharaObjAnimeID::Bottom_Stand_RightStep) ||
							i == static_cast<int>(CharaObjAnimeID::Bottom_Stand_WalkBack)) {
							this->m_AnimPerBuf[static_cast<size_t>(i)] = std::clamp(this->m_AnimPerBuf[static_cast<size_t>(i)] + ((i == static_cast<int>(this->m_BottomAnimSelect)) ? 6.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
						}
						if (
							i == static_cast<int>(CharaObjAnimeID::Bottom_Squat) ||
							i == static_cast<int>(CharaObjAnimeID::Bottom_Squat_Walk)) {
							this->m_AnimPerBuf[static_cast<size_t>(i)] = std::clamp(this->m_AnimPerBuf[static_cast<size_t>(i)] + ((i == static_cast<int>(this->m_BottomAnimSelect)) ? 2.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
						}
					}
					//
					m_BaseMatrix = Matrix3x3DX::RotAxis(Vector3DX::forward(), (this->m_ZRad / 2.f)) * Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_yrad_Bottom);
					m_UpperMatrix = Matrix3x3DX::RotAxis(Vector3DX::up(), Lerp(this->m_yrad_BottomChange, 0.f, m_RunPer));
					m_EyeMatrix = Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_yrad_BottomChange);
					//�ړ��̍ۂ̎��_�̗h��
					float SwingPer = GetRun() ? 0.5f : ((GetVec().magnitude() * DXLib_refParts->GetFps() / FrameRate) / 0.65f);
					if (SwingPer > 0.f) {
						this->m_MoveEyePosTimer += SwingPer * deg2rad(GetRun() ? 12.f : 5.f) * FrameRate * DXLib_refParts->GetDeltaTime();
					}
					else {
						this->m_MoveEyePosTimer = 0.f;
					}
					auto EyePos = Matrix3x3DX::Vtrans(Vector3DX::up() * (0.25f * SwingPer), Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_MoveEyePosTimer));
					EyePos.y = (-std::abs(EyePos.y));
					Easing(&this->m_MoveEyePos, Matrix3x3DX::Vtrans(EyePos, (this->m_UpperMatrix * this->m_BaseMatrix)), 0.9f, EasingType::OutExpo);
				}
				{
					//�|����
					if (
						(this->m_CharaAction == EnumArmAnimType::Ready || this->m_CharaAction == EnumArmAnimType::Tsuba) &&
						GetYaTimerPer() == 0.f &&
						GetInputControl().GetPADSPress(Controls::PADS::JUMP)
						) {
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Voice_Ya))->Play3D(GetEyePosition(), Scale3DRate * 35.f);
						this->m_YaTimer = GetYaTimerMax();
						m_HeartUp = 40.f;
					}
					m_YaTimer = std::max(m_YaTimer - DXLib_refParts->GetDeltaTime(), 0.f);

					Easing(&m_HeartUpR, m_HeartUp, 0.99f, EasingType::OutExpo);
					float HeartRateUp = m_HeartUpR * DXLib_refParts->GetDeltaTime();
					this->m_HeartRate_r += HeartRateUp * ((HeartRateUp > 0.f) ? std::clamp((1.f - (this->m_HeartRate - HeartRateMin) / (HeartRateMax - HeartRateMin)), 0.f, 1.f) : 1.f);
					this->m_HeartRate_r = std::clamp(this->m_HeartRate_r, HeartRateMin, HeartRateMax);
					if (this->m_HeartRate < this->m_HeartRate_r) {
						this->m_HeartRate += 5.f * DXLib_refParts->GetDeltaTime();
					}
					else if (this->m_HeartRate >= this->m_HeartRate_r) {
						this->m_HeartRate -= 5.f * DXLib_refParts->GetDeltaTime();
					}
					this->m_HeartRate = std::clamp(this->m_HeartRate, HeartRateMin, HeartRateMax);

					this->m_HeartRateRad += deg2rad(this->m_HeartRate) * DXLib_refParts->GetDeltaTime();
					if (this->m_HeartRateRad >= DX_PI_F * 2) { this->m_HeartRateRad -= DX_PI_F * 2; }
					auto Prev = this->m_HeartSoundFlag;
					if (
						(deg2rad(0) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(10)) ||
						(deg2rad(60) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(70)) ||
						(deg2rad(120) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(130)) ||
						(deg2rad(180) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(190)) ||
						(deg2rad(240) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(250)) ||
						(deg2rad(300) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(310))
						) {
						if (!this->m_HeartSoundFlag) {
							this->m_HeartSoundFlag = true;
							this->m_HeartRatePow = 1.f;
						}
					}
					else {
						this->m_HeartSoundFlag = false;
					}
					if (!this->m_HeartSoundFlag) {
						Easing(&this->m_HeartRatePow, 0.f, 0.95f, EasingType::OutExpo);
					}

					this->m_Stamina += std::clamp((HeartRateStamona - this->m_HeartRate) / (HeartRateStamona - HeartRateMin), -2.5f, 2.5f) * DXLib_refParts->GetDeltaTime();
					this->m_Stamina = std::clamp(this->m_Stamina, 0.f, GetStaminaMax());

					if (this->m_Stamina <= 0.f) {
						this->m_StaminaLoss = true;
					}
					if (this->m_StaminaLoss) {
						if (this->GetStaminaPer() > 0.3f) {
							this->m_StaminaLoss = false;
						}
					}
					this->m_HeartSoundSwitch = this->m_HeartSoundFlag && (Prev != this->m_HeartSoundFlag);
				}
				{
					this->m_BreathRad += deg2rad(Lerp(60.f, 40.f, GetStaminaPer())) * DXLib_refParts->GetDeltaTime();
					if (this->m_BreathRad >= DX_PI_F * 2) { this->m_BreathRad -= DX_PI_F * 2; }
					auto Prev = this->m_BreathSoundFlag;
					if (
						(deg2rad(0) <= this->m_BreathRad && this->m_BreathRad <= deg2rad(10)) ||
						(deg2rad(60) <= this->m_BreathRad && this->m_BreathRad <= deg2rad(70)) ||
						(deg2rad(120) <= this->m_BreathRad && this->m_BreathRad <= deg2rad(130)) ||
						(deg2rad(180) <= this->m_BreathRad && this->m_BreathRad <= deg2rad(190)) ||
						(deg2rad(240) <= this->m_BreathRad && this->m_BreathRad <= deg2rad(250)) ||
						(deg2rad(300) <= this->m_BreathRad && this->m_BreathRad <= deg2rad(310))
						) {
						this->m_BreathSoundFlag = true;
					}
					else {
						this->m_BreathSoundFlag = false;
					}
					this->m_BreathSoundSwitch = this->m_BreathSoundFlag && (Prev != this->m_BreathSoundFlag);
				}
				if (this->m_CharaAction != EnumArmAnimType::Run && this->m_CharaAction != EnumArmAnimType::GuardSuriage) {
					m_GuardCoolDownTimer = std::max(m_GuardCoolDownTimer - DXLib_refParts->GetDeltaTime(), 0.f);
				}
				for (size_t index = 0; index < static_cast<size_t>(EnumArmAnimType::Max); ++index) {
					this->m_Arm[index].Update((m_CharaAction == (EnumArmAnimType)index), 0.1f, 0.1f, IsGuardAction((EnumArmAnimType)index) ? 0.75f : 0.9f);
					//this->m_Arm[index].Init((m_CharaAction == (EnumArmAnimType)index));
				}
				this->m_SpeedMul = Lerp(0.5f, 1.f, GetStaminaPer());
			}
		public:
		};
		//
		class HitBox {
			Vector3DX	m_pos;
			float		m_radius{ 0.f };
			HitType		m_HitType{ HitType::Body };
		public:
			void	Update(const Vector3DX& pos, float radius, HitType pHitType) noexcept {
				m_pos = pos;
				m_radius = radius;
				m_HitType = pHitType;
			}
			void	Draw(void) const noexcept {
				unsigned int color;
				switch (m_HitType) {
				case HitType::Head:
					color = Red;
					break;
				case HitType::Body:
					color = Green;
					break;
				case HitType::Arm:
					color = Blue;
					break;
				case HitType::Leg:
					color = Blue;
					break;
				default:
					break;
				}
				DrawSphere_3D(m_pos, m_radius, color, color);
			}

			bool	Colcheck(const Vector3DX& StartPos, Vector3DX* pEndPos) const noexcept {
				if (HitCheck_Sphere_Capsule(
					m_pos.get(), m_radius,
					StartPos.get(), pEndPos->get(), 0.001f * Scale3DRate
				) == TRUE) {
					VECTOR pos1 = StartPos.get();
					VECTOR pos2 = pEndPos->get();
					VECTOR pos3 = m_pos.get();
					SEGMENT_POINT_RESULT Res;
					Segment_Point_Analyse(&pos1, &pos2, &pos3, &Res);

					*pEndPos = Res.Seg_MinDist_Pos;

					return TRUE;
				}
				return FALSE;
			}
			const auto& GetColType()const noexcept { return this->m_HitType; }
		};
		class HitBoxControl {
			std::vector<HitBox>									m_HitBox;
		public:
			HitBoxControl(void) noexcept {}
			~HitBoxControl(void) noexcept {}
		public:
			const HitBox* GetLineHit(const Vector3DX& StartPos, Vector3DX* pEndPos, bool Nearest = false) const noexcept {
				const HitBox* Answer = nullptr;
				for (auto& h : this->m_HitBox) {
					if (h.Colcheck(StartPos, pEndPos)) {
						Answer = &h;
						if (Nearest == false) {
							return Answer;
						}
					}
				}
				return Answer;
			}
		public:
			void Init(void) noexcept {
				m_HitBox.resize(6);
			}
			void Update(const ObjectBaseClass* ptr, float SizeRate) noexcept;
			void Draw(void) noexcept {
				//this->GetObj().SetOpacityRate(0.5f);
				SetFogEnable(FALSE);
				SetUseLighting(FALSE);
				//SetUseZBuffer3D(FALSE);

				for (auto& h : this->m_HitBox) {
					h.Draw();
				}

				//SetUseZBuffer3D(TRUE);
				SetUseLighting(TRUE);
			}
		};
		//
	}
}
