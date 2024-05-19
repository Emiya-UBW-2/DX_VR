#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "../../MainScene/NetWork.hpp"
#include "CharaAnimData.hpp"
#include "CharacterEnum.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ArmMovePerClass {
			float												m_ArmPer{0.f};
			bool												m_Armon{false};
		public:
			void Init(bool isOn)noexcept {
				m_Armon = isOn;
				m_ArmPer = isOn ? 1.f : 0.f;
			}
			void Execute(bool isOn, float OnOver = 0.2f, float OffOver = 0.2f, float Per = 0.8f) noexcept {
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
			const auto& Per() const noexcept { return m_ArmPer; }
		};
		//キャラのうち特定機能だけ抜き出したもの
		class StaminaControl {
		private://キャラパラメーター
			const float											HeartRateMin{60.f};//心拍数最小
			const float											HeartRateMax{180.f};//心拍数最大
			const float											StaminaMax{100.f};
		private:
			float												m_HeartRate{HeartRateMin};//心拍数
			float												m_HeartRate_r{HeartRateMin};//心拍数
			float												m_HeartRateRad{0.f};//呼吸Sin渡し
			float												m_Stamina{StaminaMax};//スタミナ
			bool												m_CannotRun{false};//スタミナ切れ
			bool												m_HeartSoundFlag{false};
		public://ゲッター
		public:
			void		InitStamina() {
				this->m_HeartRate = HeartRateMin;
				this->m_HeartRateRad = 0.f;
				this->m_Stamina = StaminaMax;
				this->m_HeartSoundFlag = false;
			}
			bool		ExcuteStamina(float addRun, float HeartRateUp, bool IsSquat) {
				auto* DrawParts = DXDraw::Instance();
				if (addRun > 0.f) {
					this->m_HeartRate_r += (10.f + GetRandf(10.f)) / DrawParts->GetFps();
				}
				else if (addRun < 0.f) {
					this->m_HeartRate_r -= (5.f + GetRandf(5.f)) / DrawParts->GetFps();
				}
				this->m_HeartRate_r += HeartRateUp;
				this->m_HeartRate_r -= (2.f + GetRandf(4.f)) / DrawParts->GetFps();
				this->m_HeartRate_r = std::clamp(this->m_HeartRate_r, HeartRateMin, HeartRateMax);

				if (this->m_HeartRate < this->m_HeartRate_r) {
					this->m_HeartRate += 5.f / DrawParts->GetFps();
				}
				else if (this->m_HeartRate >= this->m_HeartRate_r) {
					this->m_HeartRate -= 5.f / DrawParts->GetFps();
				}
				//this->m_HeartRate = this->m_HeartRate_r;
				this->m_HeartRateRad += deg2rad(this->m_HeartRate) / DrawParts->GetFps();
				if (this->m_HeartRateRad >= DX_PI_F * 2) { this->m_HeartRateRad -= DX_PI_F * 2; }
				if (
					(deg2rad(0) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(10)) ||
					(deg2rad(120) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(130)) ||
					(deg2rad(240) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(250))
					) {
					if (!this->m_HeartSoundFlag) {
						this->m_HeartSoundFlag = true;
					}
				}
				else {
					this->m_HeartSoundFlag = false;
				}


				this->m_Stamina += std::clamp((100.f - this->m_HeartRate) / 40.f, -2.5f, 2.5f) / DrawParts->GetFps();

				if (IsSquat) {
					this->m_Stamina += 1.0f / DrawParts->GetFps();
				}

				this->m_Stamina = std::clamp(this->m_Stamina, 0.f, StaminaMax);

				if (this->m_Stamina <= 0.f) {
					this->m_CannotRun = true;
				}
				if (this->m_CannotRun) {
					if (this->m_Stamina > StaminaMax * 0.3f) {
						this->m_CannotRun = false;
					}
				}
				return this->m_HeartSoundFlag;
			}
		};
		//キャラ入力
		class KeyControl {
		private://キャラパラメーター
			float												m_MoverPer{0.f};
			Vector3DX											m_VecTotal;
			std::array<float, 4>								m_Vec{0,0,0,0};
			InputControl										m_Input;
			switchs												m_Squat;
			Vector3DX											m_rad_Buf, m_rad, m_radAdd;
			float												m_yrad_Upper{0.f}, m_yrad_Bottom{0.f};
			float												m_yrad_UpperChange{0.f}, m_yrad_BottomChange{0.f};
			Vector3DX											m_UpperPrevRad;
			Vector3DX											m_UpperRad;
			Vector3DX											m_UpperyVecNormal, m_UpperzVecNormal;
			Vector3DX											m_UpperyVec, m_UpperzVec, m_UpperPos;
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{0};
			bool												m_TurnBody{false};

			bool												m_PosBufOverRideFlag{false};
			moves												m_OverRideInfo;

			bool												m_IsRunning{false};
			bool												m_IsFrontAttacking{false};
			bool												m_IsDouAttacking{false};
			bool												m_IsBackAttacking{false};
		public://ゲッター
			CharaAnimeID										m_BottomAnimSelect;
		public://ゲッター
			const auto		GetRun(void) const noexcept { return this->m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking; }
			const auto		GetRadBuf(void) const noexcept { return this->m_rad_Buf; }
			const auto		GetIsSquat(void) const noexcept { return this->m_Squat.on(); }
		public://セッター
			auto&			SetRadBuf(void) noexcept { return this->m_rad_Buf; }
			auto&			SetRad(void) noexcept { return this->m_rad; }
			void			SetIsSquat(bool value) noexcept { this->m_Squat.Set(value); }
			void			SetIsRunning(bool value) noexcept { this->m_IsRunning = value; }
			void			SetIsFrontAttacking(bool value) noexcept { this->m_IsFrontAttacking = value; }
			void			SetIsDouAttacking(bool value) noexcept { this->m_IsDouAttacking = value; }
			void			SetIsBackAttacking(bool value) noexcept { this->m_IsBackAttacking = value; }
		protected:
			const auto		GetInputControl(void) const noexcept { return this->m_Input; }
			const auto		GetRad(void) const noexcept { return this->m_rad; }
			const auto		GetYRadBottom(void) const noexcept { return this->m_yrad_Bottom; }
			const auto		GetYRadUpperChange(void) const noexcept { return this->m_yrad_UpperChange; }
			const auto		GetYRadBottomChange(void) const noexcept { return this->m_yrad_BottomChange; }
			const auto		GetMoverPer(void) const noexcept { return m_MoverPer; }
			const auto		IsMove(void) const noexcept { return m_MoverPer > 0.1f; }
			const auto		GetFrontP(void) const noexcept {
				auto wkey = (this->m_Input.GetPADSPress(PADS::MOVE_W) || (m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) && !m_IsBackAttacking;
				auto skey = (this->m_Input.GetPADSPress(PADS::MOVE_S) && !(m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) || m_IsBackAttacking;
				auto FrontP = (wkey && !skey) ? (atan2f(m_VecTotal.x, -m_VecTotal.z) * -m_VecTotal.z) : 0.f;
				FrontP += (!wkey && skey) ? (atan2f(-m_VecTotal.x, m_VecTotal.z) * m_VecTotal.z) : 0.f;
				return FrontP;
			}
			const auto		GetSquatSwitch(void) const noexcept { return this->m_Squat.trigger(); }
			const auto		GetGunSwingMat(void) const noexcept { return Matrix4x4DX::Axis1(m_UpperyVec.normalized(), m_UpperzVec.normalized()); }
			auto&			GetCharaAnimeBufID(CharaAnimeID value) noexcept { return this->m_AnimPerBuf.at((int)value); }
			const auto&		GetOverRideInfo() const noexcept { return this->m_OverRideInfo; }
			//
			const auto		GetBottomStandAnimSel(void) const noexcept { return GetIsSquat() ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return GetIsSquat() ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_Turn; }

			const auto		GetSpeedPer(void) const noexcept {
				if (this->m_Input.GetPADSPress(PADS::WALK)) {
					return 0.15f;
				}
				if (GetIsSquat()) {
					return 0.45f;
				}
				else if (m_IsRunning) {
					return 0.975f;
				}
				else if (m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking) {
					return 1.175f;
				}
				else {
					return 0.45f;
				}
			}
			const auto		GetVec(void) const noexcept {
				auto* DrawParts = DXDraw::Instance();
				Vector3DX vecBuf = m_VecTotal;
				if (m_MoverPer > 0.f) {
					vecBuf = vecBuf.normalized() * (GetSpeedPer() * 60.f / DrawParts->GetFps());
				}
				vecBuf = Matrix4x4DX::Vtrans(vecBuf, Matrix4x4DX::RotAxis(Vector3DX::up(), this->m_yrad_Upper));
				return vecBuf;
			}
			const auto		GetVecFront(void) const noexcept { return 1.15f * this->m_Vec[0] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
			const auto		GetVecRear(void) const noexcept { return 1.15f * this->m_Vec[2] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
			const auto		GetVecLeft(void) const noexcept { return 1.15f * this->m_Vec[1] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
			const auto		GetVecRight(void) const noexcept { return 1.15f * this->m_Vec[3] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
		public:
			void		InitOverride() { this->m_PosBufOverRideFlag = false; }
			void		InitKey(float pxRad, float pyRad) {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
				this->m_Input.ResetAllInput();
				this->m_radAdd.Set(0, 0, 0);
				this->m_rad_Buf.x = (pxRad);
				this->m_rad_Buf.y = (pyRad);
				this->m_rad = this->m_rad_Buf;

				SetIsSquat(false);
				this->m_yrad_Upper = this->m_rad.y;
				this->m_yrad_Bottom = this->m_rad.y;
				this->m_yrad_BottomChange = 0.f;
				this->m_TurnBody = false;
				//
				for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
			}
			//
			void		SetPosBufOverRide(const moves& o) noexcept {
				this->m_PosBufOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
			bool		PutOverride() {
				if (this->m_PosBufOverRideFlag) {
					this->m_PosBufOverRideFlag = false;

					Easing(&this->m_rad_Buf.x, this->m_OverRideInfo.rad.x, 0.9f, EasingType::OutExpo);
					this->m_rad_Buf.y = (this->m_OverRideInfo.rad.y);
					this->m_rad.y = (this->m_OverRideInfo.rad.y);

					return true;
				}
				return false;
			}
			void		InputKey(const InputControl& pInput, bool pReady, const Vector3DX& pAddRadvec) {
				auto* DrawParts = DXDraw::Instance();
				this->m_Input = pInput;
				if (!pReady) {
					this->m_Input.ResetKeyInput();
				}
				//入力
				this->m_Squat.Execute(false);
				//回転
				{
					Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);
					this->m_rad_Buf.x = (std::clamp(this->m_rad_Buf.x + this->m_Input.GetAddxRad(), deg2rad(-12.f), deg2rad(12.f)) + this->m_radAdd.x);
					this->m_rad_Buf.y = (this->m_rad_Buf.y + this->m_Input.GetAddyRad() + this->m_radAdd.y);

					Easing(&this->m_rad.x, m_rad_Buf.x, 0.5f, EasingType::OutExpo);
					Easing(&this->m_rad.y, m_rad_Buf.y, 0.8f, EasingType::OutExpo);
					Easing(&this->m_rad.z, m_rad_Buf.z, 0.5f, EasingType::OutExpo);
				}
				//移動
				auto wkey = (this->m_Input.GetPADSPress(PADS::MOVE_W) || (m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) && !m_IsBackAttacking;
				auto skey = (this->m_Input.GetPADSPress(PADS::MOVE_S) && !(m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) || m_IsBackAttacking;

				auto akey = this->m_Input.GetPADSPress(PADS::MOVE_A) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);
				auto dkey = this->m_Input.GetPADSPress(PADS::MOVE_D) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);

				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (wkey ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (akey ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (skey ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (dkey ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				if (m_IsDouAttacking) {
					this->m_Vec[3] = std::clamp(this->m_Vec[3] + 5.f / DrawParts->GetFps(), 0.f, 1.f);
				}
				m_VecTotal = Vector3DX::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]);
				m_MoverPer = m_VecTotal.magnitude();
			}
			void		UpdateKeyRad() {
				auto* DrawParts = DXDraw::Instance();
				//
				if (!IsMove()) {
					if (deg2rad(50.f) < abs(GetYRadUpperChange())) {
						this->m_TurnBody = true;
					}
					if (abs(GetYRadUpperChange()) < deg2rad(0.5f)) {
						this->m_TurnBody = false;
					}
				}
				else {
					this->m_TurnBody = false;
				}

				if (this->m_TurnBody || IsMove()) { Easing(&this->m_yrad_Upper, this->m_rad.y, 0.85f, EasingType::OutExpo); }
				auto YradChange = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - GetFrontP(), 0.85f, EasingType::OutExpo);
				YradChange = this->m_yrad_Bottom - YradChange;
				float Z = this->m_rad_Buf.z;
				Easing(&Z, (abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
				this->m_rad_Buf.z = (Z);
				this->m_yrad_UpperChange = this->m_rad.y - this->m_yrad_Upper;
				this->m_yrad_BottomChange = this->m_rad.y - this->m_yrad_Bottom;
				//銃の揺れ
				Easing(&m_UpperRad, (this->m_rad - this->m_UpperPrevRad)*-1.f, 0.9f, EasingType::OutExpo);
				m_UpperPrevRad = this->m_rad;
				auto mat = Matrix4x4DX::RotAxis(Vector3DX::right(), m_UpperRad.x) * Matrix4x4DX::RotAxis(Vector3DX::up(), m_UpperRad.y);
				Easing(&m_UpperyVecNormal, mat.yvec(), 0.8f, EasingType::OutExpo);
				Easing(&m_UpperzVecNormal, mat.zvec(), 0.8f, EasingType::OutExpo);
				Easing(&m_UpperyVec, m_UpperyVecNormal, 0.8f, EasingType::OutExpo);
				Easing(&m_UpperzVec, m_UpperzVecNormal, 0.8f, EasingType::OutExpo);

				this->m_BottomAnimSelect = GetBottomStandAnimSel();
				auto wkey = this->m_Input.GetPADSPress(PADS::MOVE_W) || m_IsRunning;
				auto skey = this->m_Input.GetPADSPress(PADS::MOVE_S) && !m_IsRunning;

				auto akey = this->m_Input.GetPADSPress(PADS::MOVE_A) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);
				auto dkey = this->m_Input.GetPADSPress(PADS::MOVE_D) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);

				if (akey && !dkey) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
				if (dkey && !akey) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
				if (skey && !wkey) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
				if (wkey && !skey) { this->m_BottomAnimSelect = m_IsRunning ? CharaAnimeID::Bottom_Stand_Run : GetBottomWalkAnimSel(); }
				if (m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_Stand_Attack; }
				//下半身
				Easing(&GetCharaAnimeBufID(GetBottomTurnAnimSel()), (!GetIsSquat() && this->m_TurnBody) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					if (
						i == (int)CharaAnimeID::Bottom_Stand ||
						i == (int)CharaAnimeID::Bottom_Stand_Attack ||
						i == (int)CharaAnimeID::Bottom_Stand_Run ||
						i == (int)CharaAnimeID::Bottom_Stand_Walk ||
						i == (int)CharaAnimeID::Bottom_Stand_LeftStep ||
						i == (int)CharaAnimeID::Bottom_Stand_RightStep ||
						i == (int)CharaAnimeID::Bottom_Stand_WalkBack) {
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((i == (int)this->m_BottomAnimSelect) ? 6.f : -2.f) / DrawParts->GetFps(), 0.f, 1.f);
					}
					if (
						i == (int)CharaAnimeID::Bottom_Squat ||
						i == (int)CharaAnimeID::Bottom_Squat_Walk) {
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((i == (int)this->m_BottomAnimSelect) ? 2.f : -2.f) / DrawParts->GetFps(), 0.f, 1.f);
					}
				}
			}
		};
		//歩く時の揺れ
		class WalkSwingControl {
		private:
			Vector3DX											m_WalkSwingRad;
			Vector3DX											m_WalkSwing;
			Vector3DX											m_WalkSwing_p;
			Vector3DX											m_WalkSwing_t;
			Vector3DX											m_PrevPos;
		public://ゲッター
			const auto		GetWalkSwingMat(void) const noexcept {
				return Matrix4x4DX::RotAxis(Vector3DX::forward(), deg2rad(m_WalkSwing.z*m_WalkSwingRad.z))*
					Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(m_WalkSwing.x*m_WalkSwingRad.x));
			}
		public:
			WalkSwingControl(void) noexcept {}
			~WalkSwingControl(void) noexcept {}
		public:
			void UpdateWalkSwing(const Vector3DX& Pos, float SwingPer)noexcept {
				auto* DrawParts = DXDraw::Instance();
				m_WalkSwingRad.Set(5.f, 0.f, 10.f);
				m_WalkSwingRad *= SwingPer;
				//X
				{
					if (m_PrevPos.y > Pos.y) {
						m_WalkSwing_t.x = (1.f);
					}
					else {
						m_WalkSwing_t.x = (std::max(m_WalkSwing_t.x - 15.f / DrawParts->GetFps(), 0.f));
					}
				}
				//Z
				{
					if (m_WalkSwing_t.x == 1.f) {
						if (m_WalkSwing_t.z >= 0.f) {
							m_WalkSwing_t.z = (-1.f);
						}
						else {
							m_WalkSwing_t.z = (1.f);
						}
					}
				}
				auto WS_tmp = m_WalkSwing_t * SwingPer;
				//XZ
				{
					Easing(&m_WalkSwing_p.x, WS_tmp.x, (m_WalkSwing_p.x > WS_tmp.x) ? 0.6f : 0.9f, EasingType::OutExpo);
					Easing(&m_WalkSwing_p.z, WS_tmp.z, 0.95f, EasingType::OutExpo);
				}
				//
				m_PrevPos = Pos;
				//
				Easing(&m_WalkSwing, m_WalkSwing_p, 0.5f, EasingType::OutExpo);
			}
		};
		class EyeSwingControl {
		private:
			float												m_MoveEyePosTimer{0.f};
			Vector3DX											m_MoveEyePos;
		public://ゲッター
			const auto&		GetEyeSwingPos(void) const noexcept { return this->m_MoveEyePos; }
		public:
			EyeSwingControl(void) noexcept {}
			~EyeSwingControl(void) noexcept {}
		public:
			void InitEyeSwing() noexcept {
				this->m_MoveEyePosTimer = 0.f;
			}
			void UpdateEyeSwing(const Matrix4x4DX& pCharaMat, float SwingPer, float SwingSpeed)noexcept {
				auto* DrawParts = DXDraw::Instance();
				if (SwingPer > 0.f) {
					this->m_MoveEyePosTimer += SwingPer * deg2rad(SwingSpeed)*60.f / DrawParts->GetFps();
				}
				else {
					this->m_MoveEyePosTimer = 0.f;
				}
				auto EyePos = Matrix4x4DX::Vtrans(Vector3DX::up()*(0.25f*SwingPer), Matrix4x4DX::RotAxis(Vector3DX::forward(), this->m_MoveEyePosTimer));
				EyePos.y = (-std::abs(EyePos.y));
				Easing(&this->m_MoveEyePos, Matrix4x4DX::Vtrans(EyePos, pCharaMat), 0.9f, EasingType::OutExpo);
			}
		};
	};
};
