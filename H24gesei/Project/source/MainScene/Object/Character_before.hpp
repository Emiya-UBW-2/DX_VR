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
			Vector3DX											m_VecTotal;
			std::array<float, 4>								m_Vec{};
			InputControl										m_Input;
			Vector3DX											m_rad_Buf;
			Vector3DX											m_LookVec;
			Vector3DX											m_TurretVec;
			float												m_ZRad{};
			float												m_ShipSwing;
			float												m_ShipSwingSinR;

			float												m_SpeedR{};
			float												m_YRadAddR{};
			float												m_YRadChange{};

			Matrix3x3DX											m_EyeMatrix;
			Matrix3x3DX											m_UpperMatrix;
			Matrix3x3DX											m_BaseMatrix;

			switchs												m_WKeyTrig;
			switchs												m_SKeyTrig;
			int													m_WSLevel{ 0 };

			switchs												m_AKeyTrig;
			switchs												m_DKeyTrig;
			int													m_ADLevel{ 0 };
		public://ゲッター
			CharaObjAnimeID										m_BottomAnimSelect{};
		public://ゲッター
			auto		GetRadBuf(void) const noexcept { return this->m_rad_Buf; }
			auto		GetEyeMatrix(void) const noexcept { return this->m_EyeMatrix; }
			const auto& GetGunMatrix(void) const noexcept { return Matrix3x3DX::RotAxis(Vector3DX::up(), m_TurretVec.y); }
			const auto& GetYRadChange(void) const noexcept { return this->m_YRadChange; }
			const auto& GetWSLevel(void) const noexcept { return this->m_WSLevel; }
			const auto& GetADLevel(void) const noexcept { return this->m_ADLevel; }
			const auto& GetEyeRad(void) const noexcept { return this->m_LookVec; }
		public://セッター
		protected:
			const auto& GetUpperRotMatrix(void) const noexcept { return this->m_UpperMatrix; }
			const auto& GetBaseRotMatrix(void) const noexcept { return this->m_BaseMatrix; }
			const auto& GetInputControl(void) const noexcept { return this->m_Input; }
			auto		GetShipSwingMat(void) const noexcept { return Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(3) * this->m_ShipSwingSinR); }
			auto		GetSpeedMax(void) const noexcept {
				return 10.f;
			}
			auto		GetSpeedPer(void) const noexcept {
				switch (m_WSLevel)
				{
				case -3:
					return -5.f;
				case -2:
					return -3.f;
				case -1:
					return -1.f;
				case 0:
					return 0.f;
				case 1:
					return 3.f;
				case 2:
					return 7.f;
				case 3:
					return 10.f;
				default:
					return 0.f;
				}
			}
			const auto& GetSpeedrPer(void) const noexcept { return this->m_SpeedR; }
			const auto& GetMoverPer(void) const noexcept { return this->m_SpeedR / GetSpeedMax(); }
			auto		GetRotPer(void) const noexcept {
				switch (m_ADLevel)
				{
				case -3:
					return -15.f;
				case -2:
					return -10.f;
				case -1:
					return -5.f;
				case 0:
					return 0.f;
				case 1:
					return 5.f;
				case 2:
					return 10.f;
				case 3:
					return 15.f;
				default:
					return 0.f;
				}
			}
			auto		GetVec(void) const noexcept {
				auto* DrawParts = DXDraw::Instance();
				Vector3DX vecBuf = Vector3DX::zero();
				vecBuf.z = -(m_SpeedR * Frame_Rate / DrawParts->GetFps());
				vecBuf = Matrix3x3DX::Vtrans(vecBuf, m_BaseMatrix);
				return vecBuf;
			}
		private://キャラパラメーター
			auto		IsMove(void) const noexcept { return m_SpeedR > 0.0f; }
		protected:
			void		InitKey(float pxRad, float pyRad) noexcept {
				for (size_t i = 0; i < 4; ++i) {
					this->m_Vec[i] = 0.f;
				}
				this->m_Input.ResetAllInput();
				this->m_LookVec.x = (pxRad);
				this->m_LookVec.y = (pyRad);

				this->m_rad_Buf.y = (pyRad);
				//
				this->m_ShipSwing = 0.f;
				this->m_ShipSwingSinR = 0.f;

				m_WSLevel = 1.f;
			}
			//
			void		InputKey(const InputControl& pInput,bool SEOn) noexcept {
				auto* DrawParts = DXDraw::Instance();
				this->m_Input = pInput;
				//回転
				{
					this->m_LookVec.x = std::clamp(this->m_LookVec.x + this->m_Input.GetAddxRad(), deg2rad(-15.f), deg2rad(0.f));
					this->m_LookVec.y = this->m_LookVec.y + this->m_Input.GetAddyRad();
					if (!this->m_Input.GetPADSPress(PADS::AIM)) {
						m_TurretVec = m_LookVec;
					}
				}
				//移動
				m_WKeyTrig.Execute(this->m_Input.GetPADSPress(PADS::MOVE_W));
				m_SKeyTrig.Execute(this->m_Input.GetPADSPress(PADS::MOVE_S));
				m_AKeyTrig.Execute(this->m_Input.GetPADSPress(PADS::MOVE_A));
				m_DKeyTrig.Execute(this->m_Input.GetPADSPress(PADS::MOVE_D));
				auto PrevWS = m_WSLevel;
				m_WSLevel = std::clamp(m_WSLevel
					+ (m_WKeyTrig.trigger() ? 1 : 0)
					- (m_SKeyTrig.trigger() ? 1 : 0)
					, -3, 3);
				auto PrevAD = m_ADLevel;
				m_ADLevel = std::clamp(m_ADLevel
					+ (m_DKeyTrig.trigger() ? 1 : 0)
					- (m_AKeyTrig.trigger() ? 1 : 0)
					, -3, 3);
				if (SEOn) {
					auto* SE = SoundPool::Instance();
					if (PrevWS != m_WSLevel) {
						SE->Get(static_cast<int>(SoundEnum::Gear)).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (PrevAD != m_ADLevel) {
						SE->Get(static_cast<int>(SoundEnum::Rotate)).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
				}
			}
			void		UpdateKeyRad(const moves& move_t) noexcept {
				auto* DrawParts = DXDraw::Instance();
				//
				float YradChange = 0.f;
				m_SpeedR += ((m_SpeedR < GetSpeedPer()) ? 1.f : -1.f) * 1.f / DrawParts->GetFps();
				float GonePer = GetRotPer() * GetSpeedPer() / GetSpeedMax();
				m_YRadAddR += ((m_YRadAddR < GonePer) ? 1.f : -1.f) * 0.1f / DrawParts->GetFps();

				Easing(&m_YRadChange, (abs(m_YRadAddR - GonePer) > 0.1f) ?
					((m_YRadAddR < GonePer) ? 1.f : -1.f)
					: 0.f, 0.9f, EasingType::OutExpo);
				{
					YradChange = deg2rad(m_YRadAddR) / DrawParts->GetFps();
					this->m_rad_Buf.y += YradChange;

					if (this->m_rad_Buf.y < 0.f) { this->m_rad_Buf.y += DX_PI_F * 2.f; }
					if (this->m_rad_Buf.y > DX_PI_F * 2.f) { this->m_rad_Buf.y -= DX_PI_F * 2.f; }
				}
				Easing(&this->m_rad_Buf.z, (abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
				Easing(&m_ZRad, this->m_rad_Buf.z, 0.5f, EasingType::OutExpo);
				//下半身
				//
				m_BaseMatrix = Matrix3x3DX::RotAxis(Vector3DX::forward(), (this->m_ZRad / 2.f)) * Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_rad_Buf.y);
				m_EyeMatrix = Matrix3x3DX::RotAxis(Vector3DX::right(), m_LookVec.x)* Matrix3x3DX::RotAxis(Vector3DX::up(), m_LookVec.y);
				//移動の際の視点の揺れ
				this->m_ShipSwing += deg2rad(30.f) / DrawParts->GetFps();
				Easing(&this->m_ShipSwingSinR, sin(this->m_ShipSwing), 0.9f, EasingType::OutExpo);
			}
		};

	}
}
