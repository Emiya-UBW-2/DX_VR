#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "../../MainScene/NetWork.hpp"
#include "CharaAnimData.hpp"
#include "CharacterEnum.hpp"

#include "Gun.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//キャラのうち特定機能だけ抜き出したもの
		//
		class ArmMovePerClass {
			float												m_ArmPer{ 0.f };
			bool												m_Armon{ false };
		public:
			void Init(bool isOn)noexcept {
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
			const auto& Per() const noexcept { return m_ArmPer; }
		};
		//
		enum class HitType {
			Head,
			Body,
			Arm,
			Leg,
		};
		class HitBox {
			Vector3DX	m_pos;
			float		m_radius{ 0.f };
			HitType		m_HitType{ HitType::Body };
		public:
			void	Update(const Vector3DX& pos, float radius, HitType pHitType) {
				m_pos = pos;
				m_radius = radius;
				m_HitType = pHitType;
			}
			void	Draw() const noexcept {
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
			const auto GetColType()const noexcept { return this->m_HitType; }
			const auto GetPos()const noexcept { return this->m_pos; }
		};
		//ヒットポイントなどのパラメーター
		template<class Point,int MaxPoint>
		class PointControl {
		private://パラメーター
			const Point											Max = MaxPoint;
		private:
			Point												m_HP{ 0 };							//スコア
		public://ゲッター
			const auto		IsNotZero(void) const noexcept { return this->m_HP != 0; }
			const auto&		GetPoint(void) const noexcept { return this->m_HP; }
			const auto&		GetMax(void) const noexcept { return Max; }
			void			Sub(Point damage_t) noexcept { this->m_HP = std::clamp<Point>(this->m_HP - damage_t, 0, Max); }
		public:
			void		Init() { Sub(-Max); }
		};
		//
		enum class EnumGunReadySeq {
			LowReady,
			Aim,
			ADS,
		};
		//キャラ入力
		class KeyControl {
		private://キャラパラメーター
			switchs												m_ULTKey;
			switchs												m_ThrowKey;
			switchs												m_Squat;
			switchs												m_QKey;
			switchs												m_EKey;

			float												m_LeanRad{ 0.f };
			float												m_MoverPer{ 0.f };
			Vector3DX											m_VecTotal;
			std::array<float, 4>								m_Vec{ 0,0,0,0 };
			InputControl										m_Input;
			bool												m_IsSquat{ false };
			int													m_LeanRate{ 0 };
			bool												m_LeanSwitch{ false };
			Vector3DX											m_rad_Buf, m_rad;
			Vector2DX											m_radAdd;
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_yrad_UpperChange{ 0.f }, m_yrad_BottomChange{ 0.f };
			Vector3DX											m_UpperPrevRad;
			Vector3DX											m_UpperRad;
			Vector3DX											m_UpperyVecNormal, m_UpperzVecNormal;
			Vector3DX											m_UpperyVec, m_UpperzVec, m_UpperPos;
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			bool												m_TurnBody{ false };
			CharaAnimeID										m_BottomAnimSelect{};

			float												m_SwitchPer{};
			bool												m_IsSwitchRight{};
		public://セッター
			void			SetIsSquat(bool value) noexcept { this->m_IsSquat = value; }
			void			SetIsSwitchRight(bool value) noexcept { this->m_IsSwitchRight = value; }
			auto&			SetCharaAnimeBufID(CharaAnimeID value) noexcept { return this->m_AnimPerBuf.at((int)value); }
		public://ゲッター
			const auto&		GetCharaAnimeBufID(CharaAnimeID value) const noexcept { return this->m_AnimPerBuf.at((int)value); }
			const auto&		GetSwitchPer(void) const noexcept { return this->m_SwitchPer; }
			const auto&		GetBottomAnimSelectNow(void) const noexcept { return this->m_BottomAnimSelect; }
			const auto&		GetLeanRad(void) const noexcept { return this->m_LeanRad; }
			const auto&		GetIsSquat(void) const noexcept { return this->m_IsSquat; }

			const auto		GetInputControl(void) const noexcept { return this->m_Input; }

			const auto		IsMoveFront(void) const noexcept { return GetInputControl().GetPADSPress(Controls::PADS::MOVE_W) && !GetInputControl().GetPADSPress(Controls::PADS::MOVE_S); }
			const auto		IsMoveBack(void) const noexcept { return GetInputControl().GetPADSPress(Controls::PADS::MOVE_S) && !GetInputControl().GetPADSPress(Controls::PADS::MOVE_W); }
			const auto		IsMoveLeft(void) const noexcept { return GetInputControl().GetPADSPress(Controls::PADS::MOVE_A) && !GetInputControl().GetPADSPress(Controls::PADS::MOVE_D); }
			const auto		IsMoveRight(void) const noexcept { return GetInputControl().GetPADSPress(Controls::PADS::MOVE_D) && !GetInputControl().GetPADSPress(Controls::PADS::MOVE_A); }

			const auto		GetRad(void) const noexcept { return this->m_rad; }
			const auto		GetYRadBottom(void) const noexcept { return this->m_yrad_Bottom; }
			const auto		GetFrontP(void) const noexcept {
				auto FrontP = IsMoveFront() ? (atan2f(m_VecTotal.x, -m_VecTotal.z) * -m_VecTotal.z) : 0.f;
				FrontP += IsMoveBack() ? (atan2f(-m_VecTotal.x, m_VecTotal.z) * m_VecTotal.z) : 0.f;
				return FrontP;
			}
			const auto		GetLeanRate(void) const noexcept { return this->m_LeanRate; }
			const auto		GetLeanSwitch(void) const noexcept { return this->m_LeanSwitch; }
			const auto		GetULTKey(void) const noexcept { return this->m_ULTKey.trigger(); }
			const auto		GetThrowKey(void) const noexcept { return this->m_ThrowKey; }
			const auto		GetSquatSwitch(void) const noexcept { return this->m_Squat.trigger(); }
			const auto		GetGunSwingMat(void) const noexcept { return Matrix3x3DX::Axis1(m_UpperyVec.normalized(), m_UpperzVec.normalized()); }
			const auto		GetCharaLocalDir(void) const noexcept {
				return
					Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_LeanRad) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), GetRad().x) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_yrad_BottomChange)
					;
			}
			const auto		GetEyeLocalDir(void) const noexcept {
				return
					Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_LeanRad / 5.f) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), GetRad().x) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_yrad_BottomChange)
					;
			}
			const auto		GetBottomStandAnimSel(void) const noexcept { return GetIsSquat() ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return GetIsSquat() ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return GetIsSquat() ? CharaAnimeID::Bottom_Squat_WalkBack : CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return GetIsSquat() ? CharaAnimeID::Bottom_Squat_LeftStep : CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return GetIsSquat() ? CharaAnimeID::Bottom_Squat_RightStep : CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return GetIsSquat() ? CharaAnimeID::Bottom_Squat_Turn : CharaAnimeID::Bottom_Stand_Turn; }
			const auto		GetIsStop(void) const noexcept { return m_BottomAnimSelect == GetBottomStandAnimSel(); }
			const auto		GetSpeedPer(void) const noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (GetInputControl().GetPADSPress(Controls::PADS::WALK)) {
					return 1.25f * 60.f * DXLib_refParts->GetDeltaTime();
				}
				if (GetIsSquat()) {
					return 0.45f * 60.f * DXLib_refParts->GetDeltaTime();
				}
				else {
					return 2.25f * 60.f * DXLib_refParts->GetDeltaTime();
				}
			}
			const auto		GetVec(void) const noexcept {
				Vector3DX vecBuf = m_VecTotal;
				if (m_MoverPer > 0.f) {
					vecBuf = vecBuf.normalized() * GetSpeedPer();
				}
				vecBuf = Matrix3x3DX::Vtrans(vecBuf, Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_yrad_Upper));
				return vecBuf;
			}
			const auto		GetVecFront(void) const noexcept { return 1.15f * this->m_Vec[0] * std::clamp(GetSpeedPer() / 0.65f, 0.5f, 1.f); }
			const auto		GetVecRear(void) const noexcept { return 1.15f * this->m_Vec[2] * std::clamp(GetSpeedPer() / 0.65f, 0.5f, 1.f); }
			const auto		GetVecLeft(void) const noexcept { return 1.15f * this->m_Vec[1] * std::clamp(GetSpeedPer() / 0.65f, 0.5f, 1.f); }
			const auto		GetVecRight(void) const noexcept { return 1.15f * this->m_Vec[3] * std::clamp(GetSpeedPer() / 0.65f, 0.5f, 1.f); }
		public:
			void			Init(float pxRad, float pyRad) {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
				this->m_Input.ResetAllInput();
				this->m_radAdd.Set(0, 0);
				this->m_rad_Buf.x = (pxRad);
				this->m_rad_Buf.y = (pyRad);
				this->m_rad = this->m_rad_Buf;

				SetIsSquat(false);
				this->m_yrad_Upper = GetRad().y;
				this->m_yrad_Bottom = GetRad().y;
				this->m_yrad_BottomChange = 0.f;
				this->m_LeanRad = 0.f;
				this->m_TurnBody = false;
				//
				for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
			}
			void			Input(const InputControl& pInput, bool pReady, const Vector2DX& pAddRadvec) {
				auto* DXLib_refParts = DXLib_ref::Instance();
				this->m_Input = pInput;
				if (!pReady) {
					this->m_Input.ResetKeyInput();
				}
				//入力
				this->m_QKey.Update(GetInputControl().GetPADSPress(Controls::PADS::LEAN_L));
				this->m_EKey.Update(GetInputControl().GetPADSPress(Controls::PADS::LEAN_R));
				this->m_ULTKey.Update(GetInputControl().GetPADSPress(Controls::PADS::ULT));
				this->m_ThrowKey.Update(GetInputControl().GetPADSPress(Controls::PADS::THROW));
				this->m_Squat.Update(GetInputControl().GetPADSPress(Controls::PADS::SQUAT));
				if (this->m_Squat.trigger()) {
					m_IsSquat ^= 1;
				}
				//回転
				{
					Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);
					this->m_rad_Buf.x = std::clamp(this->m_rad_Buf.x + GetInputControl().GetAddxRad(), deg2rad(-70.f), deg2rad(24.f)) + this->m_radAdd.x;
					this->m_rad_Buf.y = this->m_rad_Buf.y + GetInputControl().GetAddyRad() + this->m_radAdd.y;
					Easing(&this->m_rad.x, m_rad_Buf.x, 0.5f, EasingType::OutExpo);
					Easing(&this->m_rad.y, m_rad_Buf.y, 0.8f, EasingType::OutExpo);
					Easing(&this->m_rad.z, m_rad_Buf.z, 0.5f, EasingType::OutExpo);
				}
				//移動
				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (GetInputControl().GetPADSPress(Controls::PADS::MOVE_W) ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (GetInputControl().GetPADSPress(Controls::PADS::MOVE_A) ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (GetInputControl().GetPADSPress(Controls::PADS::MOVE_S) ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (GetInputControl().GetPADSPress(Controls::PADS::MOVE_D) ? 5.f : -15.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
				m_VecTotal = Vector3DX::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]);
				m_MoverPer = m_VecTotal.magnitude();
				//リーン
				auto Prev = this->m_LeanRate;
				if (true) {//トグル式
					if (this->m_QKey.trigger()) {
						switch (this->m_LeanRate) {
						case -1:
							this->m_LeanRate = 1;
							break;
						case 0:
							this->m_LeanRate = 1;
							break;
						case 1:
							this->m_LeanRate = 0;
							break;
						default:
							break;
						}
					}
					if (this->m_EKey.trigger()) {
						switch (this->m_LeanRate) {
						case -1:
							this->m_LeanRate = 0;
							break;
						case 0:
							this->m_LeanRate = -1;
							break;
						case 1:
							this->m_LeanRate = -1;
							break;
						default:
							break;
						}
					}
				}
				else {
					this->m_LeanRate = 0;
					if (this->m_QKey.press()) {
						this->m_LeanRate = 1;
					}
					if (this->m_EKey.press()) {
						this->m_LeanRate = -1;
					}
				}
				this->m_LeanRate = std::clamp(this->m_LeanRate, -1, 1);
				this->m_LeanSwitch = (Prev != this->m_LeanRate);
			}
			void			Update() {
				auto* DXLib_refParts = DXLib_ref::Instance();

				Easing(&m_SwitchPer, m_IsSwitchRight ? 1.f : -1.f, 0.9f, EasingType::OutExpo);
				//
				if (!(m_MoverPer > 0.1f)) {
					if (deg2rad(50.f) < abs(this->m_yrad_UpperChange)) {
						this->m_TurnBody = true;
					}
					if (abs(this->m_yrad_UpperChange) < deg2rad(0.5f)) {
						this->m_TurnBody = false;
					}
				}
				else {
					this->m_TurnBody = false;
				}

				if (this->m_TurnBody || (m_MoverPer > 0.1f)) { Easing(&this->m_yrad_Upper, GetRad().y, 0.85f, EasingType::OutExpo); }
				auto YradChange = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - GetFrontP(), 0.85f, EasingType::OutExpo);
				YradChange = this->m_yrad_Bottom - YradChange;
				float Z = this->m_rad_Buf.z;
				Easing(&Z, (abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
				this->m_rad_Buf.z = (Z);
				this->m_yrad_UpperChange = GetRad().y - this->m_yrad_Upper;
				this->m_yrad_BottomChange = GetRad().y - this->m_yrad_Bottom;
				Easing(&this->m_LeanRad, static_cast<float>(-this->m_LeanRate) * deg2rad(25), 0.9f, EasingType::OutExpo);
				//銃の揺れ
				Easing(&m_UpperRad, (GetRad() - this->m_UpperPrevRad) * -1.f, 0.9f, EasingType::OutExpo);
				m_UpperPrevRad = GetRad();
				auto mat = Matrix3x3DX::RotAxis(Vector3DX::right(), m_UpperRad.x) * Matrix3x3DX::RotAxis(Vector3DX::up(), m_UpperRad.y);
				Easing(&m_UpperyVecNormal, mat.yvec(), 0.8f, EasingType::OutExpo);
				Easing(&m_UpperzVecNormal, mat.zvec(), 0.8f, EasingType::OutExpo);
				Easing(&m_UpperyVec, m_UpperyVecNormal, 0.8f, EasingType::OutExpo);
				Easing(&m_UpperzVec, m_UpperzVecNormal, 0.8f, EasingType::OutExpo);

				this->m_BottomAnimSelect = GetBottomStandAnimSel();
				if (IsMoveLeft()) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
				if (IsMoveRight()) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
				if (IsMoveBack()) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
				if (IsMoveFront()) { this->m_BottomAnimSelect = GetBottomWalkAnimSel(); }
				//下半身
				Easing(&SetCharaAnimeBufID(GetBottomTurnAnimSel()), (!GetIsSquat() && this->m_TurnBody) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					if (
						i == (int)CharaAnimeID::Bottom_Stand ||
						i == (int)CharaAnimeID::Bottom_Stand_Run ||
						i == (int)CharaAnimeID::Bottom_Stand_Walk ||
						i == (int)CharaAnimeID::Bottom_Stand_LeftStep ||
						i == (int)CharaAnimeID::Bottom_Stand_RightStep ||
						i == (int)CharaAnimeID::Bottom_Stand_WalkBack) {
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((i == (int)this->m_BottomAnimSelect) ? 6.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
					}
					if (
						i == (int)CharaAnimeID::Bottom_Squat ||
						i == (int)CharaAnimeID::Bottom_Squat_Walk ||
						i == (int)CharaAnimeID::Bottom_Squat_LeftStep ||
						i == (int)CharaAnimeID::Bottom_Squat_RightStep ||
						i == (int)CharaAnimeID::Bottom_Squat_WalkBack) {
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i] + ((i == (int)this->m_BottomAnimSelect) ? 2.f : -2.f) * DXLib_refParts->GetDeltaTime(), 0.f, 1.f);
					}
				}
			}
		};
		//
		class LaserSightClass {
		private:
			bool												m_IsLaserActive{ false };
			Vector3DX											LaserStartPos;
			Vector3DX											LaserEndPos;
		public://ゲッター
			const auto& GetIsLaserActive() const noexcept { return this->m_IsLaserActive; }
			void			SetIsLaserActive(bool value) noexcept { m_IsLaserActive = value; }
			void			SetLaserStartPos(const Vector3DX& value) noexcept { LaserStartPos = value; }
			void			SetLaserEndPos(const Vector3DX& value) noexcept { LaserEndPos = value; }

			void			DrawLaser() const noexcept {
				if (m_IsLaserActive) {
					/*
					auto P = LaserEndPos / Scale3DRate;
					clsDx();
					printfDx("(%5.2f,%5.2f,%5.2f)\n", P.x, P.y, P.z);
					//*/
					SetUseLighting(FALSE);
					SetUseHalfLambertLighting(FALSE);
					DrawSphere_3D(LaserEndPos, 0.01f * Scale3DRate, GetColor(255, 24, 24), Black);
					DrawCapsule_3D(LaserStartPos, LaserEndPos, 0.0015f * Scale3DRate, GetColor(255, 24, 24), Black);
					SetUseLighting(TRUE);
					SetUseHalfLambertLighting(TRUE);
				}
			}
		public:
			LaserSightClass(void) noexcept {}
			~LaserSightClass(void) noexcept {}
		public:
		};
		//
		class HitBoxControl {
		private:
			std::vector<HitBox>									m_HitBox;
		public:
			const HitBox* GetLineHit(const Vector3DX& StartPos, Vector3DX* pEndPos) const noexcept {
				for (auto& h : this->m_HitBox) {
					if (h.Colcheck(StartPos, pEndPos)) {
						return &h;
					}
				}
				return nullptr;
			}
		public:
			void		CheckLineHitNearest(const Vector3DX& StartPos, Vector3DX* pEndPos) const noexcept {
				for (auto& h : this->m_HitBox) {
					h.Colcheck(StartPos, pEndPos);
				}
			}
			const auto& GetHitBoxPointList() const { return m_HitBox; }
		public:
			HitBoxControl(void) noexcept {}
			~HitBoxControl(void) noexcept {}
		public:
			void Init() noexcept {
				m_HitBox.resize(27);
			}
			void Update(const ObjectBaseClass* ptr, float SizeRate) noexcept;
			void Draw() noexcept {
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
				return Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(m_WalkSwing.z * m_WalkSwingRad.z)) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(m_WalkSwing.x * m_WalkSwingRad.x));
			}
		public:
			WalkSwingControl(void) noexcept {}
			~WalkSwingControl(void) noexcept {}
		public:
			void Update(const Vector3DX& Pos, float SwingPer)noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				m_WalkSwingRad.Set(5.f, 0.f, 10.f);
				//X
				{
					if (m_PrevPos.y > Pos.y) {
						m_WalkSwing_t.x = (1.f);
					}
					else {
						m_WalkSwing_t.x = (std::max(m_WalkSwing_t.x - 15.f * DXLib_refParts->GetDeltaTime(), 0.f));
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
				//X
				{
					auto tmp = m_WalkSwing_p.x;
					Easing(&tmp, WS_tmp.x, (m_WalkSwing_p.x > WS_tmp.x) ? 0.6f : 0.9f, EasingType::OutExpo);
					m_WalkSwing_p.x = (tmp);
				}
				//Z
				{
					auto tmp = m_WalkSwing_p.z;
					Easing(&tmp, WS_tmp.z, 0.95f, EasingType::OutExpo);
					m_WalkSwing_p.z = (tmp);
				}
				//
				m_PrevPos = Pos;
				//
				Easing(&m_WalkSwing, m_WalkSwing_p, 0.5f, EasingType::OutExpo);
			}
		};
		class EyeSwingControl {
		private:
			float												m_MoveEyePosTimer{ 0.f };
			Vector3DX											m_MoveEyePos;
		public://ゲッター
			const auto& GetEyeSwingPos(void) const noexcept { return this->m_MoveEyePos; }
		public:
			EyeSwingControl(void) noexcept {}
			~EyeSwingControl(void) noexcept {}
		public:
			void Init() noexcept {
				this->m_MoveEyePosTimer = 0.f;
			}
			void Update(const Matrix3x3DX& pCharaMat, float SwingPer, float SwingSpeed)noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (SwingPer > 0.f) {
					this->m_MoveEyePosTimer += SwingPer * deg2rad(SwingSpeed) * 60.f * DXLib_refParts->GetDeltaTime();
				}
				else {
					this->m_MoveEyePosTimer = 0.f;
				}
				auto EyePos = Matrix3x3DX::Vtrans(Vector3DX::up() * (0.25f * SwingPer), Matrix3x3DX::RotAxis(Vector3DX::forward(), this->m_MoveEyePosTimer));
				EyePos.y = (-std::abs(EyePos.y));
				Easing(&this->m_MoveEyePos, Matrix3x3DX::Vtrans(EyePos, pCharaMat), 0.9f, EasingType::OutExpo);
			}
		};
		//
		class MagStockControl {
		private:
			class MagStock {
			public:
				int AmmoNum{ 0 };
				int AmmoAll{ 0 };
				int ModUniqueID{ -1 };
			};

			std::array<MagStock, 3>								m_MagazineStock;
		public:
			int													m_AmmoStock{ 0 };
			const auto& GetAmmoStock() const noexcept { return m_AmmoStock; }
			void AddAmmoStock(int Ammo) noexcept { m_AmmoStock += Ammo; }
			void SubAmmoStock(int Ammo) noexcept { m_AmmoStock -= Ammo; }
		public:
			MagStockControl(void) noexcept {}
			~MagStockControl(void) noexcept {}
		public:
			const auto& GetNowMag() const noexcept { return m_MagazineStock[0]; }
			const auto& GetMagDatas() const noexcept { return m_MagazineStock; }
			const auto& GetMag(int select) const noexcept { return m_MagazineStock[select].AmmoNum; }
		public:
			void Init_MagStockControl(int AmmoNum, int AmmoAll, int ModUniqueID) noexcept {
				size_t Total = m_MagazineStock.size();
				for (size_t i = 0; i < Total; i++) {
					m_MagazineStock[i].AmmoNum = AmmoNum;
					m_MagazineStock[i].AmmoAll = AmmoAll;
					m_MagazineStock[i].ModUniqueID = ModUniqueID;
				}
				m_AmmoStock = 0;
			}
			void SetMag(int select, int AmmoNum) noexcept {
				m_MagazineStock[select].AmmoNum = AmmoNum;
				m_MagazineStock[select].AmmoNum = m_MagazineStock[select].AmmoAll;
			}
			void SetOldMag(int OLDAmmoNum, int OLDAmmoAll, int OLDModUniqueID) noexcept {
				m_MagazineStock[0].AmmoNum = OLDAmmoNum;
				m_MagazineStock[0].AmmoNum = m_MagazineStock[0].AmmoAll;
				m_MagazineStock[0].AmmoAll = OLDAmmoAll;
				m_MagazineStock[0].ModUniqueID = OLDModUniqueID;
				std::sort(m_MagazineStock.begin(), m_MagazineStock.end(), [&](const MagStock& A, const MagStock& B) {return A.AmmoNum > B.AmmoNum; });
			}
			bool GetNeedAmmoLoad(bool MagInGunFull, bool MagInGunEmpty) noexcept;
		};
		//
		class GunPtrControl {
		private:
			int													m_GunSelect{ 0 };
			int													m_ReserveGunSelect{ 0 };
			std::array<std::shared_ptr<GunClass>, 2>			m_Gun_Ptr{ nullptr , nullptr };			//銃
			std::array<MagStockControl, 2>						m_MagStockControl;
			std::array<Matrix3x3DX, 2>							m_SlingRot;
			std::array<Vector3DX, 2>							m_SlingPos;
		public:
			std::array<float, 2>								m_SlingPer{};
		public://ゲッター
			auto&				GetGunPtr(int ID) noexcept { return this->m_Gun_Ptr[ID]; }

			const int			GetNowGunSelect() const noexcept { return this->m_GunSelect; }
			const int			GetOtherGunSelect() const noexcept { return 1 - GetNowGunSelect(); }

			auto& GetGunPtrNow(void) noexcept { return GetGunPtr(GetNowGunSelect()); }
			const auto& GetGunPtrNow(void) const noexcept { return this->m_Gun_Ptr[GetNowGunSelect()]; }

			auto& GetGunPtrOther(void) noexcept { return GetGunPtr(GetOtherGunSelect()); }
			const auto& GetGunPtrOther(void) const noexcept { return this->m_Gun_Ptr[GetOtherGunSelect()]; }

			const auto			GetSightZoomSize() const noexcept { return (GetGunPtrNow() && GetGunPtrNow()->GetSightPtr()) ? (*GetGunPtrNow()->GetSightPtr())->GetModSlot().GetModData()->GetZoomSize() : 1.f; }
			bool				IsNeedReload() const noexcept {
				if (!GetGunPtrNow()) { return false; }
				if (GetGunPtrNow() && GetGunPtrNow()->GetReloadType() == RELOADTYPE::MAG) {
					return !(GetGunPtrNow()->GetAmmoNum() >= m_MagStockControl.at(GetNowGunSelect()).GetMag(0));
				}
				return !GetGunPtrNow()->GetIsMagFull();
			}
			void				SetGunMountMat(int ID, const Matrix3x3DX& Rot, const Vector3DX& Pos) noexcept {
				m_SlingRot[ID] = Rot;
				m_SlingPos[ID] = Pos;
			}
			void				UpdateGun(int ID) noexcept {
				bool IsSelGun = (ID == GetNowGunSelect());
				Easing(&m_SlingPer[ID], IsSelGun ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				if (m_SlingPer[ID] <= 0.001f) { m_SlingPer[ID] = 0.f; }
				if (m_SlingPer[ID] >= 0.999f) { m_SlingPer[ID] = 1.f; }
			}
			const Matrix3x3DX	GetGunRotMat(int ID, const Matrix3x3DX& AimGunRot) const noexcept {
				Matrix3x3DX tmp_gunmat = Matrix3x3DX::RotVec2(Vector3DX::forward(), Lerp(m_SlingRot[ID].zvec(), AimGunRot.zvec(), m_SlingPer[ID]));
				tmp_gunmat *= Matrix3x3DX::RotVec2(tmp_gunmat.yvec(), Lerp(m_SlingRot[ID].yvec(), AimGunRot.yvec(), m_SlingPer[ID]));
				return tmp_gunmat;
			}
			const Vector3DX		GetGunPos(int ID, const Vector3DX& AimGunPos) const noexcept {
				bool IsSelGun = (ID == GetNowGunSelect());
				float PAdd = 0.f;
				if (!IsSelGun) {
					PAdd = -1.f * Scale3DRate * m_SlingPer[ID];
				}
				return Lerp(m_SlingPos[ID] + Vector3DX::up() * PAdd, AimGunPos, m_SlingPer[ID]);
			}
		public://セッター
			void				SelectReserveGun(int ID) noexcept {
				m_ReserveGunSelect = ID;
			}
			void				InvokeReserveGunSel() noexcept {
				SelectGun(m_ReserveGunSelect);
			}
			void				SelectGun(int ID, bool IsForceSling = true) noexcept {
				this->m_GunSelect = ID;
				SelectReserveGun(ID);
				if (IsForceSling) {
					m_SlingPer[0] = (GetNowGunSelect() == 0) ? 0.f : 1.f;
					m_SlingPer[1] = 1.f - m_SlingPer[0];
				}
			}
			void				SetGunPtr(int ID, const std::shared_ptr<GunClass>& pGunPtr0) noexcept { this->m_Gun_Ptr[ID] = pGunPtr0; }
			void				SetupMagazine(void) noexcept {
				for (int loop = 0; loop < 2; ++loop) {
					if (GetGunPtr(loop)) {
						m_MagStockControl.at(loop).Init_MagStockControl(GetGunPtr(loop)->GetAmmoNum(), GetGunPtr(loop)->GetAmmoAll(), GetGunPtr(loop)->GetMagUniqueID());
					}
				}
			}
			void				SwapMagazine(void) noexcept {
				if (GetGunPtrNow()) {
					int Num = GetGunPtrNow()->GetAmmoNum();
					int All = GetGunPtrNow()->GetAmmoAll();
					int UniqueID = GetGunPtrNow()->GetMagUniqueID();
					auto& MagStock = m_MagStockControl.at(GetNowGunSelect());
					GetGunPtrNow()->SetReloadStart(MagStock.GetNowMag().AmmoNum, MagStock.GetNowMag().ModUniqueID);
					MagStock.SetOldMag(Num, All, UniqueID);
				}
			}
		public:
			GunPtrControl(void) noexcept {}
			~GunPtrControl(void) noexcept {}
		public:
			void DisposeGunPtr() noexcept {
				auto* ObjMngr = ObjectManager::Instance();
				for (auto& g : m_Gun_Ptr) {
					if (g) {
						ObjMngr->DelObj((SharedObj*)&g);
						g.reset();
					}
				}
			}
		};
		//
		class AutoAimControl {
		private:
			int													m_AutoAim{ -1 };
			int													m_AutoAimPoint{ -1 };
			float												m_AutoAimOn{ 0.f };
			float												m_AutoAimTimer{ 0.f };
		public://ゲッター
			float												m_AutoAimPer{ 0.f };
			Vector3DX											m_AutoAimVec{};
		public:
			AutoAimControl(void) noexcept {}
			~AutoAimControl(void) noexcept {}
		public:
			const auto& GetAutoAimID(void) const noexcept { return this->m_AutoAim; }
			const auto& GetAutoAimPoint(void) const noexcept { return this->m_AutoAimPoint; }
			const auto& GetAutoAimOn(void) const noexcept { return this->m_AutoAimOn; }

			const auto		GetAutoAimActive(void) const noexcept { return this->m_AutoAim != -1; }
			const auto		GetAutoAimPer(void) const noexcept { return this->m_AutoAimTimer / 1.f; }
		public:
			void SetAutoAimActive(int ID, int Point) noexcept {
				m_AutoAimTimer = 1.f;
				m_AutoAim = ID;
				m_AutoAimPoint = Point;
			}
			void Update(bool isActive) noexcept;
		};
		//
		class HitReactionControl {
		private:
			Vector3DX											m_HitAxis{ Vector3DX::forward() };
			float												m_HitPower{ 0.f };
			float												m_HitPowerR{ 0.f };
		private:
		public:
			HitReactionControl(void) noexcept {}
			~HitReactionControl(void) noexcept {}
		public:
			const auto GetHitReactionMat() const noexcept { return Matrix3x3DX::RotAxis(m_HitAxis, m_HitPowerR * deg2rad(90.f)); }
			const auto IsDamaging(void) const noexcept { return m_HitPower > 0.f; }
		public:
			void SetHit(const Vector3DX& Axis) noexcept {
				m_HitAxis = Axis;
				m_HitPower = 1.f;
			}
			void Update() noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				Easing(&this->m_HitPowerR, this->m_HitPower, 0.8f, EasingType::OutExpo);
				this->m_HitPower = std::max(this->m_HitPower - DXLib_refParts->GetDeltaTime() / 0.3f, 0.f);
			}
		};
		//ラグドール
		class RagDollControl {
		private:
			//体のフレーム情報
			class frame_body {
			public:
				//頭
				frames head_f;
				//胴体
				frames bodyg_f;
				frames bodyc_f;
				frames bodyb_f;
				frames body_f;
				//右手座標系
				frames RIGHThand2_f;
				frames RIGHThand_f;
				frames RIGHTarm2_f;
				frames RIGHTarm1_f;
				//左手座標系
				frames LEFThand2_f;
				frames LEFThand_f;
				frames LEFTarm2_f;
				frames LEFTarm1_f;

				//右手座標系
				frames RIGHTreg2_f;
				frames RIGHTreg_f;
				frames RIGHTfoot2_f;
				frames RIGHTfoot1_f;
				//左手座標系
				frames LEFTreg2_f;
				frames LEFTreg_f;
				frames LEFTfoot2_f;
				frames LEFTfoot1_f;
			public:
				//
				void Get_frame(MV1& obj_) noexcept {
					for (int i = 0; i < int(obj_.GetFrameNum()); ++i) {
						std::string p = obj_.GetFrameName(i);
						if (p == std::string("グルーブ")) {
							this->bodyg_f.Set(i, obj_);
						}
						else if (p == std::string("下半身")) {
							this->bodyc_f.Set(i, obj_);
						}

						else if (p.find("左足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
							this->LEFTfoot1_f.Set(i, obj_);
						}
						else if (p.find("左ひざ") != std::string::npos) {
							this->LEFTfoot2_f.Set(i, obj_);
						}
						else if (p.find("左足首") != std::string::npos && p.find("先") == std::string::npos) {
							this->LEFTreg_f.Set(i, obj_);
						}
						else if (p.find("左足首先") != std::string::npos) {
							this->LEFTreg2_f.Set(i, obj_);
						}

						else if (p.find("右足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
							this->RIGHTfoot1_f.Set(i, obj_);
						}
						else if (p.find("右ひざ") != std::string::npos) {
							this->RIGHTfoot2_f.Set(i, obj_);
						}
						else if (p.find("右足首") != std::string::npos && p.find("先") == std::string::npos) {
							this->RIGHTreg_f.Set(i, obj_);
						}
						else if (p.find("右足首先") != std::string::npos) {
							this->RIGHTreg2_f.Set(i, obj_);
						}
						else if (p.find("上半身") != std::string::npos && p.find("上半身2") == std::string::npos) {
							this->bodyb_f.Set(i, obj_);
						}
						else if (p.find("上半身2") != std::string::npos) {
							this->body_f.Set(i, obj_);
						}
						else if (p.find("頭") != std::string::npos && p.find("先") == std::string::npos) {
							this->head_f.Set(i, obj_);
							//head_hight = obj_.frame(this->head_f.first).y;
						}

						else if (p.find("右腕") != std::string::npos && p.find("捩") == std::string::npos) {
							this->RIGHTarm1_f.Set(i, obj_);
						}
						else if (p.find("右ひじ") != std::string::npos) {
							this->RIGHTarm2_f.Set(i, obj_);
						}
						else if (p == std::string("右手首")) {
							this->RIGHThand_f.Set(i, obj_);
						}
						else if (p == std::string("右手先") || p == std::string("右手首先")) {
							this->RIGHThand2_f.Set(i, obj_);
						}

						else if (p.find("左腕") != std::string::npos && p.find("捩") == std::string::npos) {
							this->LEFTarm1_f.Set(i, obj_);
						}
						else if (p.find("左ひじ") != std::string::npos) {
							this->LEFTarm2_f.Set(i, obj_);
						}
						else if (p == std::string("左手首")) {
							this->LEFThand_f.Set(i, obj_);
						}
						else if (p == std::string("左手先") || p == std::string("左手首先")) {
							this->LEFThand2_f.Set(i, obj_);
						}
					}
				}
				//
				void copy_frame(MV1& mine, frame_body& frame_tgt_, MV1* tgt) noexcept {
					tgt->SetMatrix(mine.GetMatrix());
					//
					tgt->SetFrameLocalMatrix(frame_tgt_.head_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.head_f.GetFrameID()));
					//
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyg_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.bodyg_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyc_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.bodyc_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyb_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.bodyb_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.body_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.body_f.GetFrameID()));
					//右手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHThand_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.GetFrameID()));
					//左手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFThand2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFThand_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.GetFrameID()));
					//右手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.GetFrameID()));
					//左手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTreg_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.GetFrameID()));
					for (int i = 0; i < tgt->GetAnimNum(); ++i) {
						tgt->SetAnim(i).SetPer(mine.SetAnim(i).GetPer());
						tgt->SetAnim(i).SetTime(mine.SetAnim(i).GetTime());
					}
				}
				//
			};
		private:
			MV1													m_RagDoll;
			float												m_RagDollTimer{ 0.f };						//ラグドールの物理演算フラグ
		public:
			//体のフレーム情報
			frame_body lagframe_;							//フレーム
			frame_body frame_s;								//フレーム
		public:
			RagDollControl(void) noexcept {}
			~RagDollControl(void) noexcept {}
		public:
			auto& GetRagDoll(void) noexcept { return this->m_RagDoll; }
		public:
			void Init(MV1& obj_body_t) noexcept {
				//身体
				this->frame_s.Get_frame(obj_body_t);
				//ラグドール
				this->lagframe_.Get_frame(m_RagDoll);
			}
			void Update(MV1& obj_body_t, bool isAlive) noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (isAlive) {
					this->m_RagDollTimer = 0.f;
				}
				else {
					this->m_RagDollTimer = std::min(this->m_RagDollTimer + DXLib_refParts->GetDeltaTime(), 3.f);
				}
				if (this->m_RagDollTimer < 3.f) {
					this->m_RagDoll.SetPrioritizePhysicsOverAnimFlag(true);
					this->frame_s.copy_frame(obj_body_t, this->lagframe_, &m_RagDoll);
					this->m_RagDoll.UpdateAnimAll();
					if (this->m_RagDollTimer == 0.f) {
						this->m_RagDoll.PhysicsResetState();
					}
					else {
						this->m_RagDoll.PhysicsCalculation(1000.f * DXLib_refParts->GetDeltaTime());
					}
				}
			}
		};
	};
};
