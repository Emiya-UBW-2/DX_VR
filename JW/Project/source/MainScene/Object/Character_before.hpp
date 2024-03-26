#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "../../CommonScene/NetWork.hpp"
#include "CharaAnimData.hpp"
#include "CharacterEnum.hpp"
#include "ObjectBase.hpp"
#include "ItemObj.hpp"

#include "Gun.hpp"
#include "Armer.hpp"
#include "Morphine.hpp"

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


		enum class HitType {
			Head,
			Body,
			Arm,
			Leg,
		};
		class HitBox {
			VECTOR_ref	m_pos;
			float		m_radius{0.f};
			HitType		m_HitType{HitType::Body};
		public:
			void	Execute(const VECTOR_ref&pos, float radius, HitType pHitType) {
				m_pos = pos;
				m_radius = radius;
				m_HitType = pHitType;
			}
			void	Draw() {
				unsigned int color;
				switch (m_HitType) {
					case HitType::Head:
						color = GetColor(255, 0, 0);
						break;
					case HitType::Body:
						color = GetColor(0, 255, 0);
						break;
					case HitType::Arm:
						color = GetColor(0, 0, 255);
						break;
					case HitType::Leg:
						color = GetColor(0, 0, 255);
						break;
					default:
						break;
				}
				DrawSphere_3D(m_pos, m_radius, color, color);
			}

			bool	Colcheck(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				if (HitCheck_Sphere_Capsule(
					m_pos.get(), m_radius,
					StartPos.get(), pEndPos->get(), 0.001f*Scale_Rate
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
			//const auto&		GetStamina(void) const noexcept { return this->m_Stamina; }
			//const auto&		GetStaminaMax(void) const noexcept { return this->StaminaMax; }
			//const auto&		GetCannotRun(void) const noexcept { return this->m_CannotRun; }
			//const auto&		GetHeartRate(void) const noexcept { return this->m_HeartRate; }
			//const auto&		GetHeartRateRad(void) const noexcept { return this->m_HeartRateRad; }

			//シェーダーに渡す場合
			//Set_is_Blackout(true);
			//Set_Per_Blackout(0.5f + (1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));

			const auto		GetHeartRandVec(float SquatPer) const noexcept {
				auto tmp2 = 0.2f * GetRandf(deg2rad(1.f));
				auto tmp3 = Lerp(0.5f, 0.35f, SquatPer);
				VECTOR_ref tmpvec = VECTOR_ref::vget(
					tmp2 + 0.00006f * sin(this->m_HeartRateRad) * powf(this->m_HeartRate / HeartRateMin, 3.f),
					tmp2 + 0.00006f * sin(this->m_HeartRateRad * 3) * powf(this->m_HeartRate / HeartRateMin, 3.f),
					0.f
				);
				return tmpvec * tmp3 * (60.f / FPS);
			}
		public:
			void		InitStamina() {
				this->m_HeartRate = HeartRateMin;
				this->m_HeartRateRad = 0.f;
				this->m_Stamina = StaminaMax;
				this->m_HeartSoundFlag = false;
			}
			bool		ExcuteStamina(float addRun, float HeartRateUp, bool IsSquat) {
				if (addRun > 0.f) {
					this->m_HeartRate_r += (10.f + GetRandf(10.f)) / FPS;
				}
				else if (addRun < 0.f) {
					this->m_HeartRate_r -= (5.f + GetRandf(5.f)) / FPS;
				}
				this->m_HeartRate_r += HeartRateUp;
				this->m_HeartRate_r -= (2.f + GetRandf(4.f)) / FPS;
				this->m_HeartRate_r = std::clamp(this->m_HeartRate_r, HeartRateMin, HeartRateMax);

				if (this->m_HeartRate < this->m_HeartRate_r) {
					this->m_HeartRate += 5.f / FPS;
				}
				else if (this->m_HeartRate >= this->m_HeartRate_r) {
					this->m_HeartRate -= 5.f / FPS;
				}
				//this->m_HeartRate = this->m_HeartRate_r;
				this->m_HeartRateRad += deg2rad(this->m_HeartRate) / FPS;
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


				this->m_Stamina += std::clamp((100.f - this->m_HeartRate) / 40.f, -2.5f, 2.5f) / FPS;

				if (IsSquat) {
					this->m_Stamina += 1.0f / FPS;
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
		//
		class LifeControl {
		private://キャラパラメーター
			const HitPoint										HPMax = 100;
			const ArmerPoint									APMax = 100;
		private:
			HitPoint											m_HP{0};							//スコア
			ArmerPoint											m_AP{0};							//スコア
			std::vector<DamageEvent>							m_DamageEvent;						//ダメージイベント
		protected:
			void			SetHealEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint value, ArmerPoint Armervalue) noexcept {
				this->m_DamageEvent.resize(this->m_DamageEvent.size() + 1);
				this->m_DamageEvent.back().SetEvent(ShotID_t, DamageID_t, -value, -Armervalue);
			}
			void			SetSubHPEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint value, ArmerPoint Armervalue) noexcept {
				this->m_DamageEvent.resize(this->m_DamageEvent.size() + 1);
				this->m_DamageEvent.back().SetEvent(ShotID_t, DamageID_t, value, Armervalue);
			}
		public://ゲッター
			const auto		IsAlive(void) const noexcept { return this->m_HP != 0; }
			const auto&		GetHP(void) const noexcept { return this->m_HP; }
			const auto&		GetHPMax(void) const noexcept { return HPMax; }
			void			SubHP(HitPoint damage_t) noexcept { this->m_HP = std::clamp<HitPoint>(this->m_HP - damage_t, 0, HPMax); }

			const auto		IsLowHP(void) const noexcept { return GetHP() < (GetHPMax() * 35 / 100); }

			const auto		IsArmerActive(void) const noexcept { return this->m_AP != 0; }
			const auto&		GetAP(void) const noexcept { return this->m_AP; }
			const auto&		GetAPMax(void) const noexcept { return APMax; }
			void			SubAP(ArmerPoint damage_t) noexcept { this->m_AP = std::clamp<ArmerPoint>(this->m_AP - damage_t, 0, APMax); }

			auto&			GetDamageEvent(void) noexcept { return this->m_DamageEvent; }
		public:
			void		InitLife() {
				this->m_HP = HPMax;
				this->m_AP = APMax;
			}
		};
		//
		class ULTControl {
		private://キャラパラメーター
			const int										ULTMax = 100;
		private:
			int											m_ULT{0};							//スコア
			bool										m_ULTSwitch{false};
		public://ゲッター
			const auto		IsULTActive(void) const noexcept { return this->m_ULT == ULTMax; }
			const auto		ULTActiveSwitch(void) noexcept {
				auto ret = this->m_ULTSwitch;
				this->m_ULTSwitch = false;
				return ret;
			}
			const auto&		GetULT(void) const noexcept { return this->m_ULT; }
			const auto&		GetULTMax(void) const noexcept { return ULTMax; }
			void			AddULT(int damage_t, bool SwitchOn) noexcept {
				auto prev = this->m_ULT;
				this->m_ULT = std::clamp<int>(this->m_ULT + damage_t, 0, ULTMax);
				if (SwitchOn) {
					m_ULTSwitch |= ((prev != ULTMax) && IsULTActive());
				}
			}
		public:
			void		InitULT() {
				this->m_ULT = 0;
				this->m_ULTSwitch = false;
			}
		};
		//
		class CharaTypeControl {
		private:
			CharaTypeID											m_CharaType;
		public://ゲッター
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
		};
		//
		class GunReadyControl {
		private://キャラパラメーター
			const float											UpperTimerLimit = 10.f;
		private:
			float												m_ReadyTimer{0.f};
			float												m_ADSPer{0.f};
		public://ゲッター
			const auto		GetIsADS(void) const noexcept { return this->m_ReadyTimer == 0.f; }
			const auto		GetIsAim(void) const noexcept { return !(this->m_ReadyTimer == UpperTimerLimit); }
			const auto&		GetADSPer(void) const noexcept { return this->m_ADSPer; }
		protected:
			void			SetReady(void) noexcept { this->m_ReadyTimer = UpperTimerLimit; }
			void			SetAim(void) noexcept { this->m_ReadyTimer = 0.1f; }
			void			SetADS(void) noexcept { this->m_ReadyTimer = 0.f; }
			void			UpdateReady(void) noexcept {
				Easing(&this->m_ADSPer, this->GetIsADS() ? 1.f : 0.f, 0.9f, EasingType::OutExpo);//
				this->m_ReadyTimer = std::clamp(this->m_ReadyTimer + 1.f / FPS, 0.f, UpperTimerLimit);
			}
			void			SetAimOrADS(void) noexcept {
				this->m_ReadyTimer = std::min(this->m_ReadyTimer, 0.1f);
			}
		};
		//キャラ入力
		class KeyControl {
		private://キャラパラメーター
			float												m_MoverPer{0.f};
			VECTOR_ref											m_VecTotal;
			std::array<float, 4>								m_Vec{0,0,0,0};
			InputControl										m_Input;
			switchs												m_ULTKey;
			switchs												m_Squat;
			int													m_LeanRate{0};
			bool												m_LeanSwitch{false};
			switchs												m_QKey;
			switchs												m_EKey;
			VECTOR_ref											m_rad_Buf, m_rad, m_radAdd;
		public://ゲッター
			const auto		GetInputControl(void) const noexcept { return this->m_Input; }
			const auto		GetRadBuf(void) const noexcept { return this->m_rad_Buf; }
			const auto		GetRad(void) const noexcept { return this->m_rad; }
			const auto		GetVecFront(void) const noexcept { return this->m_Vec[0]; }
			const auto		GetVecRear(void) const noexcept { return this->m_Vec[2]; }
			const auto		GetVecLeft(void) const noexcept { return this->m_Vec[1]; }
			const auto		GetVecRight(void) const noexcept { return this->m_Vec[3]; }
			const auto		GetVec(void) const noexcept { return m_VecTotal; }
			const auto		GetMoverPer(void) const noexcept { return m_MoverPer; }
			const auto		IsMove(void) const noexcept { return m_MoverPer > 0.1f; }
			const auto		GetFrontP(void) const noexcept {
				auto wkey = this->m_Input.GetPADSPress(PADS::MOVE_W);
				auto skey = this->m_Input.GetPADSPress(PADS::MOVE_S);
				auto FrontP = (wkey && !skey) ? (atan2f(m_VecTotal.x(), -m_VecTotal.z()) * -m_VecTotal.z()) : 0.f;
				FrontP += (!wkey && skey) ? (atan2f(-m_VecTotal.x(), m_VecTotal.z()) * m_VecTotal.z()) : 0.f;
				return FrontP;
			}
			const auto		GetLeanRate(void) const noexcept { return this->m_LeanRate; }
			const auto		GetLeanSwitch(void) const noexcept { return this->m_LeanSwitch; }
			const auto		GetULTKey(void) const noexcept { return this->m_ULTKey; }
			const auto		GetIsSquat(void) const noexcept { return this->m_Squat.on(); }
			const auto		GetSquatSwitch(void) const noexcept { return this->m_Squat.trigger(); }
			const auto		GetRun(void) const noexcept { return this->m_Input.GetPADSPress(PADS::RUN) && this->m_Input.GetPADSPress(PADS::MOVE_W); }
		public://セッター
			auto&			SetRadBuf(void) noexcept { return this->m_rad_Buf; }
			auto&			SetRad(void) noexcept { return this->m_rad; }
			void			ResetLeanRate(void) noexcept {
				m_LeanSwitch = (this->m_LeanRate != 0);
				this->m_LeanRate = 0;
			}
			void			SetIsSquat(bool value) noexcept { this->m_Squat.Set(value); }
		private: //内部
			void			SetVec(bool Press0, bool Press1, bool Press2, bool Press3) {
				this->m_Vec[0] = std::clamp(this->m_Vec[0] + (Press0 ? 5.f : -15.f) / FPS, 0.f, 1.f);
				this->m_Vec[1] = std::clamp(this->m_Vec[1] + (Press1 ? 5.f : -15.f) / FPS, 0.f, 1.f);
				this->m_Vec[2] = std::clamp(this->m_Vec[2] + (Press2 ? 5.f : -15.f) / FPS, 0.f, 1.f);
				this->m_Vec[3] = std::clamp(this->m_Vec[3] + (Press3 ? 5.f : -15.f) / FPS, 0.f, 1.f);
				m_VecTotal = VECTOR_ref::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]);
				m_MoverPer = m_VecTotal.Length();
			}
		public:
			void		InitKey(float pxRad, float pyRad);
			void		InputKey(const InputControl& pInput, bool pReady, const VECTOR_ref& pAddRadvec);
		};
		//
		class OverrideControl {
		private://キャラパラメーター
			bool												m_PosBufOverRideFlag{false};
			moves												m_OverRideInfo;
		private:
		public://ゲッター
			const auto&			GetOverRideInfo() const noexcept { return this->m_OverRideInfo; }
		public:
			void		InitOverride() { this->m_PosBufOverRideFlag = false; }
			void		SetPosBufOverRide(const moves& o) noexcept {
				this->m_PosBufOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
			bool		PutOverride() {
				if (this->m_PosBufOverRideFlag) {
					this->m_PosBufOverRideFlag = false;
					return true;
				}
				return false;
			}
		};
		//
		class LaserSightClass {
		private:
			bool												m_IsLaserActive{false};
			VECTOR_ref											LaserStartPos;
			VECTOR_ref											LaserEndPos;
		public://ゲッター
			const auto&			GetIsLaserActive() const noexcept { return this->m_IsLaserActive; }
			void			SetIsLaserActive(bool value) noexcept { m_IsLaserActive = value; }
			void			SetLaserStartPos(const VECTOR_ref& value) noexcept { LaserStartPos = value; }
			void			SetLaserEndPos(const VECTOR_ref& value) noexcept { LaserEndPos = value; }

			void			DrawLaser() noexcept {
				if (m_IsLaserActive) {
					SetUseLighting(FALSE);
					DrawSphere_3D(LaserEndPos, 0.01f*Scale_Rate, GetColor(255, 24, 24), GetColor(0, 0, 0));
					DrawCapsule_3D(LaserStartPos, LaserEndPos, 0.0015f*Scale_Rate, GetColor(255, 24, 24), GetColor(0, 0, 0));
					SetUseLighting(TRUE);
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
		protected:
			const HitBox*		GetLineHit(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				for (auto& h : this->m_HitBox) {
					if (h.Colcheck(StartPos, pEndPos)) {
						return &h;
					}
				}
				return nullptr;
			}
		public:
			void		CheckLineHitNearest(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				for (auto& h : this->m_HitBox) {
					h.Colcheck(StartPos, pEndPos);
				}
			}
		public:
			HitBoxControl(void) noexcept {}
			~HitBoxControl(void) noexcept {}
		protected:
			void InitHitBox() noexcept {
				m_HitBox.resize(27);
			}
			void UpdataHitBox(const ObjectBaseClass* ptr, float SizeRate) noexcept;
			void DrawHitBox() noexcept {
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
			VECTOR_ref											m_WalkSwingRad;
			VECTOR_ref											m_WalkSwing;
			VECTOR_ref											m_WalkSwing_p;
			VECTOR_ref											m_WalkSwing_t;
			VECTOR_ref											m_PrevPos;
		public://ゲッター
			const auto		GetWalkSwingMat(void) const noexcept {
				return MATRIX_ref::RotZ(deg2rad(m_WalkSwing.z()*m_WalkSwingRad.z()))*
					MATRIX_ref::RotX(deg2rad(m_WalkSwing.x()*m_WalkSwingRad.x()));
			}
		public:
			WalkSwingControl(void) noexcept {}
			~WalkSwingControl(void) noexcept {}
		public:
			void UpdateWalkSwing(const VECTOR_ref& Pos, float SwingPer)noexcept {
				m_WalkSwingRad.Set(5.f, 0.f, 10.f);
				//X
				{
					if (m_PrevPos.y() > Pos.y()) {
						m_WalkSwing_t.x(1.f);
					}
					else {
						m_WalkSwing_t.x(std::max(m_WalkSwing_t.x() - 15.f / FPS, 0.f));
					}
				}
				//Z
				{
					if (m_WalkSwing_t.x() == 1.f) {
						if (m_WalkSwing_t.z() >= 0.f) {
							m_WalkSwing_t.z(-1.f);
						}
						else {
							m_WalkSwing_t.z(1.f);
						}
					}
				}
				auto WS_tmp = m_WalkSwing_t * SwingPer;
				//X
				{
					auto tmp = m_WalkSwing_p.x();
					Easing(&tmp, WS_tmp.x(), (m_WalkSwing_p.x() > WS_tmp.x()) ? 0.6f : 0.9f, EasingType::OutExpo);
					m_WalkSwing_p.x(tmp);
				}
				//Z
				{
					auto tmp = m_WalkSwing_p.z();
					Easing(&tmp, WS_tmp.z(), 0.95f, EasingType::OutExpo);
					m_WalkSwing_p.z(tmp);
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
			VECTOR_ref											m_MoveEyePos;
		public://ゲッター
			const auto&		GetEyeSwingPos(void) const noexcept { return this->m_MoveEyePos; }
		public:
			EyeSwingControl(void) noexcept {}
			~EyeSwingControl(void) noexcept {}
		public:
			void InitEyeSwing() noexcept {
				this->m_MoveEyePosTimer = 0.f;
			}
			void UpdateEyeSwing(const MATRIX_ref& pCharaMat, float SwingPer, float SwingSpeed)noexcept {
				if (SwingPer > 0.f) {
					this->m_MoveEyePosTimer += SwingPer * deg2rad(SwingSpeed)*60.f / FPS;
				}
				else {
					this->m_MoveEyePosTimer = 0.f;
				}
				auto EyePos = MATRIX_ref::Vtrans(VECTOR_ref::up()*(0.25f*SwingPer), MATRIX_ref::RotZ(this->m_MoveEyePosTimer));
				EyePos.y(-std::abs(EyePos.y()));
				Easing(&this->m_MoveEyePos, MATRIX_ref::Vtrans(EyePos, pCharaMat), 0.9f, EasingType::OutExpo);
			}
		};
		//
		class GunPtrControl {
		private:
			int													m_GunSelect{0};
			std::array<std::shared_ptr<GunClass>, 2>			m_Gun_Ptr{nullptr , nullptr};			//銃
		public://ゲッター
			auto&			GetGunPtr(int ID) noexcept { return this->m_Gun_Ptr[ID]; }
			auto&			GetGunPtrNow(void) noexcept { return this->m_Gun_Ptr[m_GunSelect]; }
			const auto&		GetGunPtrNow(void) const noexcept { return this->m_Gun_Ptr[m_GunSelect]; }

			auto&			GetGunPtrOther(void) noexcept { return this->m_Gun_Ptr[GetOtherGunSelect()]; }
			const auto&		GetGunPtrOther(void) const noexcept { return this->m_Gun_Ptr[GetOtherGunSelect()]; }

			const auto		GetShotPhase(void) const noexcept { return (GetGunPtrNow()) ? GetGunPtrNow()->GetShotPhase() : GunAnimeID::Base; }
			const auto		GetShootReady(void) const noexcept { return (GetGunPtrNow()) ? GetGunPtrNow()->GetShootReady() : false; }
			const auto		GetAmmoLoading() const noexcept { return GetGunPtrNow()->IsAmmoLoading(); }
			const auto		IsSightActive() const noexcept { return GetGunPtrNow()->IsSightActive(); }
			const auto		IsSightPtrActive() const noexcept { return GetGunPtrNow()->IsSightPtrActive(); }
			const auto		IsAutoAimActive() const noexcept { return GetGunPtrNow()->IsAutoAimActive(); }

			const auto&		GetSightReitcleGraphPtr() const noexcept { return GetGunPtrNow()->GetSightPtr()->GetModData()->GetReitcleGraph(); }
			const auto		GetSightZoomSize() const noexcept {
				if (!IsSightPtrActive()) { return 1.f; }
				return GetGunPtrNow()->GetSightPtr()->GetModData()->GetZoomSize();
			}

			const int		GetNowGunSelect() const noexcept { return this->m_GunSelect; }
			const int		GetOtherGunSelect() const noexcept { return 1 - this->m_GunSelect; }

			const auto		IsGun0Select() const noexcept { return this->m_GunSelect == 0; }
			const auto		IsULTSelect() const noexcept { return this->m_GunSelect == 1; }
		public://セッター
			void			SelectGun(int ID) noexcept { this->m_GunSelect = ID; }
			void			SetGunPtr(int ID, const std::shared_ptr<GunClass>& pGunPtr0) noexcept { this->m_Gun_Ptr[ID] = pGunPtr0; }
		public:
			GunPtrControl(void) noexcept {}
			~GunPtrControl(void) noexcept {}
		public:
			//void UpdateEyeSwing(const MATRIX_ref& pCharaMat, float SwingPer, float SwingSpeed)noexcept {}
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
		enum class ArmerWearPhase {
			Ready,
			Have,
			Wear,
		};
		class ArmerPtrControl {
		private:
		protected:
			std::shared_ptr<ArmerClass>							m_Armer_Ptr{nullptr};
			ArmMovePerClass										m_Wear_Armer;
			ArmerWearPhase										m_ArmerWearPhase{ArmerWearPhase::Ready};
		protected:
			const auto		isWearingArmer() const noexcept { return (this->m_ArmerWearPhase != ArmerWearPhase::Ready); }
		public:
			ArmerPtrControl(void) noexcept {}
			~ArmerPtrControl(void) noexcept {}
		public:
			void InitArmerPtr() {
				this->m_Wear_Armer.Init(false);
				this->m_ArmerWearPhase = ArmerWearPhase::Ready;
				if (this->m_Armer_Ptr) {
					this->m_Armer_Ptr->SetActive(false);
				}
			}
			void			SetArmer(const std::shared_ptr<ArmerClass>& pArmer) noexcept { this->m_Armer_Ptr = pArmer; }
		};

		enum class MorphinePhase {
			Ready,
			Have,
			Wear,
		};
		class MorphinePtrControl {
		private:
		protected:
			std::shared_ptr<MorphineClass>						m_Morphine_Ptr{nullptr};
			ArmMovePerClass										m_Wear_Morphine;
			float												m_Wear_MorphineFrame{0.f};
			float												m_Wear_MorphinePer{0.f};
			MorphinePhase										m_MorphinePhase{MorphinePhase::Ready};
		protected:
			const auto		isMorphineing() const noexcept { return (this->m_MorphinePhase != MorphinePhase::Ready); }
		public:
			MorphinePtrControl(void) noexcept {}
			~MorphinePtrControl(void) noexcept {}
		public:
			void			SetMorphine(const std::shared_ptr<MorphineClass>& pMorphine) noexcept { this->m_Morphine_Ptr = pMorphine; }
		public:
			void InitMorphinePtr() {
				this->m_Wear_Morphine.Init(false);
				if (this->m_Morphine_Ptr) {
					this->m_Morphine_Ptr->SetActive(false);
				}
			}
		};
		//
		class AutoAimControl {
		private:
			int													m_AutoAim{-1};
			float												m_AutoAimOn{0.f};
			float												m_AutoAimTimer{0.f};
		public://ゲッター
		public:
			AutoAimControl(void) noexcept {}
			~AutoAimControl(void) noexcept {}
		public:
			const auto&		GetAutoAimID(void) const noexcept { return this->m_AutoAim; }
			const auto&		GetAutoAimOn(void) const noexcept { return this->m_AutoAimOn; }

			const auto		GetAutoAimActive(void) const noexcept { return this->m_AutoAim != -1; }
			const auto		GetAutoAimPer(void) const noexcept { return this->m_AutoAimTimer / 1.f; }
		public:
			void SetAutoAimActive(int ID) noexcept {
				m_AutoAimTimer = 1.f;
				m_AutoAim = ID;
			}
			void UpdateAutoAim(bool isActive) noexcept;
		};
		//銃の揺れ
		class GunSwingControl {
		private:
			VECTOR_ref											m_UpperPrevRad;
			VECTOR_ref											m_UpperRad;
			VECTOR_ref											m_UpperyVecNormal, m_UpperzVecNormal;
			VECTOR_ref											m_UpperyVec, m_UpperzVec, m_UpperPos;
		public://ゲッター
			const auto		GetGunSwingMat(void) const noexcept {
				return MATRIX_ref::Axis1_YZ(m_UpperyVec.Norm(), m_UpperzVec.Norm());
			}
		public:
			GunSwingControl(void) noexcept {}
			~GunSwingControl(void) noexcept {}
		public:
			void UpdateGunSwing(const VECTOR_ref& CharaRad)noexcept {
				Easing(&m_UpperRad, (CharaRad - this->m_UpperPrevRad)*-1.f, 0.9f, EasingType::OutExpo);
				m_UpperPrevRad = CharaRad;
				auto mat = MATRIX_ref::RotX(m_UpperRad.x()) * MATRIX_ref::RotY(m_UpperRad.y());
				Easing(&m_UpperyVecNormal, mat.yvec(), 0.8f, EasingType::OutExpo);
				Easing(&m_UpperzVecNormal, mat.zvec(), 0.8f, EasingType::OutExpo);
				Easing(&m_UpperyVec, m_UpperyVecNormal, 0.8f, EasingType::OutExpo);
				Easing(&m_UpperzVec, m_UpperzVecNormal, 0.8f, EasingType::OutExpo);
			}
		};
		//
		class StackLeftHandControl {
		private:
			float												m_IsStuckLeftHandTimer{0.f};
			bool												m_IsStuckLeftHand{false};
			ArmMovePerClass										m_StuckLeftHand;
			VECTOR_ref											m_StuckLeftHandPos;
			VECTOR_ref											m_StuckLeftHandNormal;
			VECTOR_ref											m_StuckLeftHandPos_R;
			VECTOR_ref											m_StuckLeftHandYVec;
		public://ゲッター
			const auto&		GetStuckLeftHandPos(void) const noexcept { return m_StuckLeftHandPos_R; }
			const auto&		GetStuckLeftHandYVec(void) const noexcept { return m_StuckLeftHandYVec; }
			const auto		GetStuckLeftHandPer(void) const noexcept { return m_StuckLeftHand.Per(); }
		public:
			StackLeftHandControl(void) noexcept {}
			~StackLeftHandControl(void) noexcept {}
		public:
			void InitStackLeftHand() {
				this->m_IsStuckLeftHandTimer = 0.f;
				this->m_IsStuckLeftHand = false;
				this->m_StuckLeftHand.Init(false);
			}
			void UpdateStackLeftHand() {
				Easing(&m_StuckLeftHandPos_R, m_StuckLeftHandPos, 0.9f, EasingType::OutExpo);
				Easing(&m_StuckLeftHandYVec, m_StuckLeftHandNormal, 0.9f, EasingType::OutExpo);
				m_StuckLeftHand.Execute(m_IsStuckLeftHand, 0.2f, 0.2f);
			}
		public:
			void ResetStackLeftHand() {
				m_IsStuckLeftHand = false;
				m_IsStuckLeftHandTimer = 0.f;
			}
			void SetStackLeftHand(const VECTOR_ref& Pos, const VECTOR_ref& Normal) {
				if (m_IsStuckLeftHandTimer >= 0.5f) {
					if (!m_IsStuckLeftHand) {
						m_StuckLeftHandPos = Pos;
						m_StuckLeftHandNormal = Normal;
						m_StuckLeftHandPos_R = Pos;
						m_StuckLeftHandYVec = Normal;
					}
					else {
						if ((m_StuckLeftHandPos - Pos).Length() > 0.3f*Scale_Rate) {
							m_StuckLeftHandPos = Pos;
							m_StuckLeftHandNormal = Normal;
						}
						m_StuckLeftHandPos.y(Pos.y());
					}
					m_IsStuckLeftHand = true;
				}
				m_IsStuckLeftHandTimer = std::min(m_IsStuckLeftHandTimer + 1.f / FPS, 0.5f);
			}
		};
		//
		class MagStockControl {
		private:
			class MagStock {
			public:
				int AmmoNum{0};
				int AmmoAll{0};
				int ModUniqueID{-1};
			};

			std::array<MagStock, 3>								m_MagazineStock;
		public:
			int													m_AmmoStock{0};
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
			void SetMag(int select, int AmmoNum) noexcept { m_MagazineStock[select].AmmoNum = AmmoNum; }
			void SetOldMag(int OLDAmmoNum, int OLDAmmoAll, int OLDModUniqueID) noexcept {
				m_MagazineStock[0].AmmoNum = OLDAmmoNum;
				m_MagazineStock[0].AmmoAll = OLDAmmoAll;
				m_MagazineStock[0].ModUniqueID = OLDModUniqueID;
				std::sort(m_MagazineStock.begin(), m_MagazineStock.end(), [&](const MagStock&A, const MagStock&B) {return A.AmmoNum > B.AmmoNum; });
			}
			bool GetNeedAmmoLoad(bool MagInGunFull, bool MagInGunEmpty) noexcept;
		};
		//
		class HitReactionControl {
		private:
			VECTOR_ref											m_HitAxis{VECTOR_ref::front()};
			float												m_HitPower{0.f};
			float												m_HitPowerR{0.f};
		private:
		public:
			HitReactionControl(void) noexcept {}
			~HitReactionControl(void) noexcept {}
		public:
			const auto GetHitReactionMat() const noexcept { return MATRIX_ref::RotAxis(m_HitAxis, m_HitPowerR*deg2rad(90.f)); }
			const auto IsDamaging(void) const noexcept { return m_HitPower > 0.f; }
		public:
			void SetHit(const VECTOR_ref& Axis) noexcept {
				m_HitAxis = Axis;
				m_HitPower = 1.f;
			}
			void Execute_HitReactionControl() noexcept {
				Easing(&this->m_HitPowerR, this->m_HitPower, 0.8f, EasingType::OutExpo);
				this->m_HitPower = std::max(this->m_HitPower - 1.f / FPS / 0.3f, 0.f);
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
					for (int i = 0; i < int(obj_.frame_num()); ++i) {
						std::string p = obj_.frame_name(i);
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
							//head_hight = obj_.frame(this->head_f.first).y();
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
					for (int i = 0; i < tgt->get_anime().size(); ++i) {
						tgt->get_anime(i).per = mine.get_anime(i).per;
						tgt->get_anime(i).time = mine.get_anime(i).time;
					}
				}
				//
			};
		private:
			MV1													m_RagDoll;
			float												m_RagDollTimer{0.f};						//ラグドールの物理演算フラグ
		protected:
			//体のフレーム情報
			frame_body lagframe_;							//フレーム
			frame_body frame_s;								//フレーム
		public:
			RagDollControl(void) noexcept {}
			~RagDollControl(void) noexcept {}
		public:
			auto&			GetRagDoll(void) noexcept { return this->m_RagDoll; }
		public:
			void Init_RagDollControl(MV1& obj_body_t) noexcept {
				//身体
				this->frame_s.Get_frame(obj_body_t);
				//ラグドール
				this->lagframe_.Get_frame(m_RagDoll);
			}
			void Execute_RagDollControl(MV1& obj_body_t, bool isAlive) noexcept {
				if (isAlive) {
					this->m_RagDollTimer = 0.f;
				}
				else {
					this->m_RagDollTimer = std::min(this->m_RagDollTimer + 1.f / FPS, 3.f);
				}
				if (this->m_RagDollTimer < 3.f) {
					MV1SetPrioritizePhysicsOverAnimFlag(this->m_RagDoll.get(), TRUE);
					this->frame_s.copy_frame(obj_body_t, this->lagframe_, &m_RagDoll);
					this->m_RagDoll.work_anime();
					if (this->m_RagDollTimer == 0.f) {
						this->m_RagDoll.PhysicsResetState();
					}
					else {
						this->m_RagDoll.PhysicsCalculation(1000.f / FPS);
					}
				}
			}
		};
		//
		class ItemFallControl {
		private:
			std::array<std::shared_ptr<ItemObjClass>, 1>	m_Ptr;
			int												m_Now{0};
		public:
			void		Init(const std::shared_ptr<BackGroundClassBase>& backGround, const std::string& pPath, ItemType type);
			void		SetFall(const VECTOR_ref& pPos, const VECTOR_ref& pVec);
			void		Dispose() noexcept;
		};
	};
};
