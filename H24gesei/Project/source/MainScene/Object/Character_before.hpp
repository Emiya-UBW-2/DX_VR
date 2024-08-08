#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "../../MainScene/NetWork.hpp"
#include "CharacterEnum.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		class ArmMovePerClass {
			float												m_ArmPer{ 0.f };
			bool												m_Armon{ false };
		public:
			void Init(bool isOn) noexcept {
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
			const auto& Per(void) const noexcept { return m_ArmPer; }
		};

		//キャラ入力
		class CharaMove {
		private://キャラパラメーター
			float												m_MoverPer{ 0.f };
			Vector3DX											m_VecTotal;
			std::array<float, 4>								m_Vec{};
			InputControl										m_Input;
			switchs												m_Squat;
			Vector3DX											m_rad_Buf;
			float												m_ZRad{};
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_yrad_UpperChange{ 0.f }, m_yrad_BottomChange{ 0.f };
			float												m_PrevRadX{ 0.f };
			float												m_PrevRadY{ 0.f };
			float												m_UpperRadX{ 0.f };
			float												m_UpperRadY{ 0.f };
			Vector3DX											m_UpperyVecNormal, m_UpperzVecNormal;
			Vector3DX											m_UpperyVec, m_UpperzVec, m_UpperPos;
			std::array<float, static_cast<int>(CharaObjAnimeID::AnimeIDMax)>	m_AnimPerBuf{};
			bool												m_TurnBody{ false };

			float												m_RunPer{};
			bool												m_IsRunning{ false };
			bool												m_IsFrontAttacking{ false };
			bool												m_IsDouAttacking{ false };
			bool												m_IsBackAttacking{ false };

			Matrix3x3DX											m_EyeMatrix;
			Matrix3x3DX											m_UpperMatrix;
			Matrix3x3DX											m_BaseMatrix;

			float												m_MoveEyePosTimer{ 0.f };
			Vector3DX											m_MoveEyePos;
		public://ゲッター
			CharaObjAnimeID										m_BottomAnimSelect{};
		public://ゲッター
			auto		GetRun(void) const noexcept { return this->m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking; }
			auto		GetRadBuf(void) const noexcept { return this->m_rad_Buf; }
			auto		GetIsSquat(void) const noexcept { return this->m_Squat.on(); }
			auto		GetEyeMatrix(void) const noexcept { return this->m_EyeMatrix * this->m_BaseMatrix; }
		public://セッター
			void			SetIsSquat(bool value) noexcept { this->m_Squat.Set(value); }
			void			SetIsRunning(bool value) noexcept { this->m_IsRunning = value; }
			void			SetIsFrontAttacking(bool value) noexcept { this->m_IsFrontAttacking = value; }
			void			SetIsDouAttacking(bool value) noexcept { this->m_IsDouAttacking = value; }
			void			SetIsBackAttacking(bool value) noexcept { this->m_IsBackAttacking = value; }
		protected:
			const auto& GetEyeSwingPos(void) const noexcept { return this->m_MoveEyePos; }
			const auto& GetEyeRotMatrix(void) const noexcept { return this->m_EyeMatrix; }
			const auto& GetUpperRotMatrix(void) const noexcept { return this->m_UpperMatrix; }
			const auto& GetBaseRotMatrix(void) const noexcept { return this->m_BaseMatrix; }
			const auto& GetInputControl(void) const noexcept { return this->m_Input; }
			const auto& GetCharaAnimeBufID(CharaObjAnimeID value) const noexcept { return this->m_AnimPerBuf.at(static_cast<size_t>(value)); }
			auto&		SetCharaAnimeBufID(CharaObjAnimeID value) noexcept { return this->m_AnimPerBuf.at(static_cast<size_t>(value)); }
			auto		GetWeaponSwingMat(void) const noexcept { return Matrix3x3DX::Axis1(m_UpperyVec.normalized(), m_UpperzVec.normalized()); }
			auto		GetSpeedPer(void) const noexcept {
				return 10.f;
				if (this->m_Input.GetPADSPress(PADS::WALK)) {
					return 0.15f;
				}
				else if (GetIsSquat()) {
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
			auto		GetVec(void) const noexcept {
				auto* DrawParts = DXDraw::Instance();
				Vector3DX vecBuf = Matrix3x3DX::Vtrans(m_VecTotal, Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_yrad_Upper));
				if (m_MoverPer > 0.f) {
					vecBuf = vecBuf.normalized() * (GetSpeedPer() * Frame_Rate / DrawParts->GetFps());
				}
				return vecBuf;
			}
			auto		GetSquatSwitch(void) const noexcept { return this->m_Squat.trigger(); }
			auto		GetVecFront(void) const noexcept { return 1.15f * this->m_Vec[0] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
			auto		GetVecRear(void) const noexcept { return 1.15f * this->m_Vec[2] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
			auto		GetVecLeft(void) const noexcept { return 1.15f * this->m_Vec[1] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
			auto		GetVecRight(void) const noexcept { return 1.15f * this->m_Vec[3] * std::clamp(GetSpeedPer() / 0.45f, 0.5f, 1.f); }
		private://キャラパラメーター
			auto		IsMove(void) const noexcept { return m_MoverPer > 0.1f; }
			auto		GetFrontP(void) const noexcept {
				auto wkey = (this->m_Input.GetPADSPress(PADS::MOVE_W) || (m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) && !m_IsBackAttacking;
				auto skey = (this->m_Input.GetPADSPress(PADS::MOVE_S) && !(m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) || m_IsBackAttacking;
				auto FrontP = (wkey && !skey) ? (atan2f(m_VecTotal.x, -m_VecTotal.z) * -m_VecTotal.z) : 0.f;
				FrontP += (!wkey && skey) ? (atan2f(-m_VecTotal.x, m_VecTotal.z) * m_VecTotal.z) : 0.f;
				return FrontP;
			}
		protected:
			void		InitKey(float pxRad, float pyRad) noexcept {
				for (size_t i = 0; i < 4; ++i) {
					this->m_Vec[i] = 0.f;
				}
				this->m_Input.ResetAllInput();
				this->m_rad_Buf.x = (pxRad);
				this->m_rad_Buf.y = (pyRad);

				SetIsSquat(false);
				this->m_yrad_Upper = this->m_rad_Buf.y;
				this->m_yrad_Bottom = this->m_rad_Buf.y;
				this->m_PrevRadX = this->m_rad_Buf.x;
				this->m_PrevRadY = this->m_rad_Buf.y;
				this->m_yrad_BottomChange = 0.f;
				this->m_TurnBody = false;
				//
				for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
				this->m_MoveEyePosTimer = 0.f;
			}
			//
			void		InputKey(const InputControl& pInput) noexcept {
				auto* DrawParts = DXDraw::Instance();
				this->m_Input = pInput;
				//入力
				this->m_Squat.Execute(false);
				//回転
				{
					this->m_rad_Buf.x = std::clamp(this->m_rad_Buf.x + this->m_Input.GetAddxRad(), deg2rad(-15.f), deg2rad(0.f));
					this->m_rad_Buf.y = this->m_rad_Buf.y + this->m_Input.GetAddyRad();
				}
				Easing(&m_RunPer, m_IsRunning ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				//移動
				auto wkey = (this->m_Input.GetPADSPress(PADS::MOVE_W) || (m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) && !m_IsBackAttacking;
				auto skey = (this->m_Input.GetPADSPress(PADS::MOVE_S) && !(m_IsRunning || m_IsFrontAttacking || m_IsDouAttacking)) || m_IsBackAttacking;

				auto akey = this->m_Input.GetPADSPress(PADS::MOVE_A) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);
				auto dkey = this->m_Input.GetPADSPress(PADS::MOVE_D) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);

				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (wkey ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (akey ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (skey ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (dkey ? 5.f : -15.f) / DrawParts->GetFps(), 0.f, 1.f);
				if (m_IsRunning || m_IsDouAttacking) {
					this->m_Vec[3] = std::clamp(this->m_Vec[3] + 5.f / DrawParts->GetFps(), 0.f, 1.f);
				}
				m_VecTotal = Vector3DX::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]);
				m_MoverPer = m_VecTotal.magnitude();
			}
			void		UpdateKeyRad(const moves& move_t) noexcept {
				float XRad = 0.f;
				float YRad = 0.f;
				move_t.GetMatBuf().GetRadian(&XRad, &YRad, nullptr);
				auto* DrawParts = DXDraw::Instance();
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
					auto bottom = 0.f;// (this->m_yrad_Upper - GetFrontP());
					// 狙い
					{
						Vector3DX Vec; Vec.Set(std::sin(this->m_yrad_Bottom), std::cos(this->m_yrad_Bottom), 0.f);
						Vector3DX vec_a; vec_a.Set(std::sin(bottom), std::cos(bottom), 0.f);
						float cost = Vector3DX::Cross(vec_a, Vec).z;
						float sint = Vector3DX::Dot(vec_a, Vec);
						YradChange = std::clamp(std::atan2f(cost, sint), deg2rad(-10), deg2rad(10)) * 1.f / DrawParts->GetFps();
						this->m_yrad_Bottom += YradChange;

						if (this->m_yrad_Bottom < 0.f) { this->m_yrad_Bottom += DX_PI_F * 2.f; }
						if (this->m_yrad_Bottom > DX_PI_F * 2.f) { this->m_yrad_Bottom -= DX_PI_F * 2.f; }
					}
				}
				Easing(&this->m_rad_Buf.z, (abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
				Easing(&m_ZRad, this->m_rad_Buf.z, 0.5f, EasingType::OutExpo);
				{
					Vector3DX Vec; Vec.Set(std::sin(this->m_yrad_Bottom), std::cos(this->m_yrad_Bottom), 0.f);
					Vector3DX vec_a; vec_a.Set(std::sin(YRad), std::cos(YRad), 0.f);
					float cost = Vector3DX::Cross(vec_a, Vec).z;
					float sint = Vector3DX::Dot(vec_a, Vec);
					this->m_yrad_BottomChange = std::atan2f(cost, sint);
				}
				//銃の揺れ
				auto mat = Matrix3x3DX::RotAxis(Vector3DX::right(), (XRad - this->m_PrevRadX) * -1.f) * Matrix3x3DX::RotAxis(Vector3DX::up(), (YRad - this->m_PrevRadY) * -1.f);
				this->m_PrevRadX = XRad;
				this->m_PrevRadY = YRad;
				Easing(&m_UpperyVecNormal, mat.yvec(), 0.8f, EasingType::OutExpo);
				Easing(&m_UpperzVecNormal, mat.zvec(), 0.8f, EasingType::OutExpo);
				Easing(&m_UpperyVec, m_UpperyVecNormal, 0.8f, EasingType::OutExpo);
				Easing(&m_UpperzVec, m_UpperzVecNormal, 0.8f, EasingType::OutExpo);

				//auto wkey = this->m_Input.GetPADSPress(PADS::MOVE_W) || m_IsRunning;
				//auto skey = this->m_Input.GetPADSPress(PADS::MOVE_S) && !m_IsRunning;

				//auto akey = this->m_Input.GetPADSPress(PADS::MOVE_A) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);
				//auto dkey = this->m_Input.GetPADSPress(PADS::MOVE_D) && !(m_IsFrontAttacking || m_IsDouAttacking || m_IsBackAttacking);

				//下半身
				//for (int i = 0; i < static_cast<int>(CharaObjAnimeID::AnimeIDMax); ++i) {
					//this->m_AnimPerBuf[static_cast<size_t>(i)] = std::clamp(this->m_AnimPerBuf[static_cast<size_t>(i)] + ((i == static_cast<int>(this->m_BottomAnimSelect)) ? 2.f : -2.f) / DrawParts->GetFps(), 0.f, 1.f);
				//}
				//
				m_BaseMatrix = Matrix3x3DX::RotAxis(Vector3DX::forward(), (this->m_ZRad / 2.f)) * Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_yrad_Bottom);
				m_UpperMatrix = Matrix3x3DX::RotAxis(Vector3DX::right(), XRad) * Matrix3x3DX::RotAxis(Vector3DX::up(), Lerp(this->m_yrad_BottomChange, 0.f, m_RunPer));
				m_EyeMatrix = Matrix3x3DX::RotAxis(Vector3DX::right(), XRad) * Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_yrad_BottomChange);
				//移動の際の視点の揺れ
				float SwingPer = GetRun() ? 0.5f : ((GetVec().magnitude() * DrawParts->GetFps() / Frame_Rate) / 0.65f);
				if (SwingPer > 0.f) {
					this->m_MoveEyePosTimer += SwingPer * deg2rad(GetRun() ? 12.f : 5.f) * Frame_Rate / DrawParts->GetFps();
				}
				else {
					this->m_MoveEyePosTimer = 0.f;
				}
				auto EyePos = Matrix3x3DX::Vtrans(Vector3DX::up() * (0.25f * SwingPer), Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_MoveEyePosTimer));
				EyePos.y = (-std::abs(EyePos.y));
				Easing(&this->m_MoveEyePos, Matrix3x3DX::Vtrans(EyePos, (this->m_UpperMatrix * this->m_BaseMatrix)), 0.9f, EasingType::OutExpo);
			}
		};

	}
}
