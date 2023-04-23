#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		struct CharaAnimeSet {
			CharaAnimeID	m_Down{ CharaAnimeID::Upper_Down1 };

			CharaAnimeID	m_Ready{ CharaAnimeID::Upper_Ready1 };

			CharaAnimeID	m_ADS{ CharaAnimeID::Upper_ADS1 };

			CharaAnimeID	m_Cocking{ CharaAnimeID::Upper_Cocking1 };

			CharaAnimeID	m_Reload{ CharaAnimeID::Upper_Reload1Start };
		};

		class Pendulum2D {
			float	m_PendulumLength = 10.f;
			float	m_PendulumMass = 2.f;
			float	m_drag_coeff = 2.02f;

			float	m_rad = deg2rad(12.f);
			float	m_vel = 0.f;
		public:
			void Init(float Length, float N, float rad) {
				m_PendulumLength = Length;
				m_PendulumMass = N;
				m_rad = rad;
				m_vel = 0.f;
			}

			void Execute() {
				m_vel += (-9.8f / this->m_PendulumLength * std::sin(m_rad) - this->m_drag_coeff / this->m_PendulumMass * this->m_vel) / 60.f;
				m_rad += this->m_vel / 60.f;
			}

			const auto GetRad() const noexcept { return this->m_rad; }

		};
		enum class HitType {
			Head,
			Body,
			Leg,
		};
		class HitBox {
			VECTOR_ref	m_pos;
			float		m_radius{ 0.f };
			HitType		m_HitType{ HitType::Body };
		public:
			void	Execute(const VECTOR_ref&pos, float radius, HitType pHitType) {
				m_pos = pos;
				m_radius = radius;
				m_HitType = pHitType;
			}
			void	Draw() {
				unsigned int color;
				switch (m_HitType) {
				case FPS_n2::Sceneclass::HitType::Head:
					color = GetColor(255, 0, 0);
					break;
				case FPS_n2::Sceneclass::HitType::Body:
					color = GetColor(0, 255, 0);
					break;
				case FPS_n2::Sceneclass::HitType::Leg:
					color = GetColor(0, 0, 255);
					break;
				default:
					break;
				}
				DrawSphere_3D(m_pos, m_radius, color, color);
			}

			bool	Colcheck(const AmmoClass& pAmmo) {
				return (HitCheck_Sphere_Capsule(
					m_pos.get(), m_radius,
					pAmmo.GetMove().repos.get(), pAmmo.GetMove().pos.get(), pAmmo.GetCaliberSize()*Scale_Rate
				) == TRUE);
			}

			const auto GetColType()const noexcept { return this->m_HitType; }
		};

		//キャラのうち特定機能だけ抜き出したもの
		class StaminaControl {
		private://キャラパラメーター
			const float											HeartRateMin{ 60.f };//心拍数最小
			const float											HeartRateMax{ 180.f };//心拍数最大
			const float											StaminaMax{ 100.f };
		private:
			float												m_HeartRate{ HeartRateMin };//心拍数
			float												m_HeartRate_r{ HeartRateMin };//心拍数
			float												m_HeartRateRad{ 0.f };//呼吸Sin渡し
			float												m_Stamina{ StaminaMax };//スタミナ
			bool												m_CannotRun{ false };//スタミナ切れ
			bool												m_HeartSoundFlag{ false };
		public://ゲッター
			//const auto&		GetStamina(void) const noexcept { return this->m_Stamina; }
			//const auto&		GetStaminaMax(void) const noexcept { return this->StaminaMax; }
			const auto&		GetCannotRun(void) const noexcept { return this->m_CannotRun; }
			const auto&		GetHeartRate(void) const noexcept { return this->m_HeartRate; }
			const auto&		GetHeartRateRad(void) const noexcept { return this->m_HeartRateRad; }
			const auto		GetHeartRandVec(float SquatPer) const noexcept {
				auto tmp2 = 0.2f * GetRandf(deg2rad(1.f));
				auto tmp3 = Lerp(0.5f, 0.35f, SquatPer);
				VECTOR_ref tmpvec = VECTOR_ref::vget(
					tmp2 + 0.00006f * sin(this->m_HeartRateRad) * powf(this->m_HeartRate / HeartRateMin, 3.f),
					tmp2 + 0.00006f * sin(this->m_HeartRateRad * 3) * powf(this->m_HeartRate / HeartRateMin, 3.f),
					0.f
				);
				return tmpvec * tmp3;
			}
			const auto		GetMousePer(void) const noexcept { return (0.75f + sin(this->m_HeartRateRad * 3)*0.25f)*(1.f - this->m_Stamina / StaminaMax); }
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
		class LifeControl {
		private://キャラパラメーター
			const HitPoint										HPMax = 100;
		private:
			HitPoint											m_HP{ 0 };							//スコア
		public://ゲッター
			//void				AddHP(HitPoint value) noexcept { this->m_HP = std::clamp<HitPoint>(this->m_HP + value, 0, HPMax); }
			//void				SetHP(HitPoint value) noexcept { this->m_HP = value; }
			const auto&		GetHP(void) const noexcept { return this->m_HP; }
			const auto&		GetHPMax(void) const noexcept { return HPMax; }
			void			SubHP(HitPoint damage_t, float)  noexcept { this->m_HP = std::clamp<HitPoint>(this->m_HP - damage_t, 0, HPMax); }
		public:
			void		InitLife() {
				this->m_HP = HPMax;
			}
			void		ExcuteLife() {
			}
		};
		class KeyControl {
		private://キャラパラメーター
		private:
			bool												m_KeyActive{ true };
			bool												m_ReadySwitch{ false };
			CharacterMoveGroundControl							m_InputGround;
			switchs												m_Rightkey;
			switchs												m_Leftkey;
			switchs												m_Upkey;
			switchs												m_Downkey;
			switchs												m_SideMove;
			switchs												m_GunChange;
			switchs												m_GoRightkey;
			switchs												m_GoLeftkey;
		public://ゲッター
			const auto		GetRad(void) const noexcept { return this->m_InputGround.GetRad(); }
			const auto		GetIsRun(void) const noexcept { return this->m_InputGround.GetRun(); }
			const auto		GetRunPer(void) const noexcept { return this->m_InputGround.GetRunPer(); }
			const auto		GetRadBuf(void) const noexcept { return this->m_InputGround.GetRadBuf(); }
			const auto		GetLeanRatePer(void) const noexcept { return this->m_InputGround.GetLeanRatePer(); }
			const auto		GetPressFrontGround(void) const noexcept { return this->m_InputGround.GetPressFront(); }
			const auto		GetPressRearGround(void) const noexcept { return this->m_InputGround.GetPressRear(); }
			const auto		GetPressLeftGround(void) const noexcept { return this->m_InputGround.GetPressLeft(); }
			const auto		GetPressRightGround(void) const noexcept { return this->m_InputGround.GetPressRight(); }
			const auto		GetIsSquatGround(void) const noexcept { return this->m_InputGround.GetIsSquat(); }
			const auto		GetSquatPer(void) const noexcept { return this->m_InputGround.GetSquatPer(); }
			const auto		GetVec(void) const noexcept { return this->m_InputGround.GetVec(); }
			const auto		GetFrontP(void) const noexcept { return this->m_InputGround.GetFrontP(); }
			const auto		GetVecFront(void) const noexcept { return this->m_InputGround.GetVecFront(); }
			const auto		GetVecLeft(void) const noexcept { return this->m_InputGround.GetVecLeft(); }
			const auto		GetVecRear(void) const noexcept { return this->m_InputGround.GetVecRear(); }
			const auto		GetVecRight(void) const noexcept { return this->m_InputGround.GetVecRight(); }
			const auto		GetKeyActive(void) const noexcept { return this->m_KeyActive; }
			const auto		GeReadySwitch(void) const noexcept { return this->m_ReadySwitch; }
			const auto		GetSideMove(void) const noexcept { return this->m_SideMove; }
			const auto		GetGunChange(void) const noexcept { return this->m_GunChange; }
			const auto		GetGoRightkey(void) const noexcept { return this->m_GoRightkey; }
			const auto		GetGoLeftkey(void) const noexcept { return this->m_GoLeftkey; }
			void			SetRadBufXY(const VECTOR_ref& buf) noexcept {
				this->m_InputGround.SetRadBufX(buf.x());
				this->m_InputGround.SetRadBufY(buf.y());
			}
			void			SetRadBufZ(const float z) noexcept {
				this->m_InputGround.SetRadBufZ(z);
			}
		public:
			void		InitKey(float pxRad, float pyRad, bool SquatOn) {
				this->m_KeyActive = false;
				this->m_ReadySwitch = false;
				this->m_InputGround.ValueSet(pxRad, pyRad, SquatOn);
			}
			void		InputKey(
				const InputControl& pInput, bool pReady,
				const VECTOR_ref& pAddRadvec, bool pCannotSprint
			) {
				this->m_ReadySwitch = (this->m_KeyActive != pReady);
				this->m_KeyActive = pReady;

				//地
				m_InputGround.SetInput(
					pInput.GetAddxRad(), pInput.GetAddyRad(),
					pAddRadvec,
					pInput.GetGoFrontPress(),
					pInput.GetGoBackPress(),
					pInput.GetGoLeftPress() && !m_SideMove.press(),
					pInput.GetGoRightPress() && !m_SideMove.press(),
					pInput.GetAction4(),
					pInput.GetRunPress(),
					false,
					(pInput.GetGoLeftPress()),
					(pInput.GetGoRightPress()),
					pCannotSprint,
					(!pInput.GetGoLeftPress() && !pInput.GetGoRightPress())
				);
				//
				m_SideMove.Execute(pInput.GetAction1());				//平行移動
				m_GunChange.Execute(pInput.GetAction3());				//銃切替

				m_GoRightkey.Execute(pInput.GetGoRightPress() && m_SideMove.press());
				m_GoLeftkey.Execute(pInput.GetGoLeftPress() && m_SideMove.press());

				m_Rightkey.Execute(pInput.GetRightPress());
				m_Leftkey.Execute(pInput.GetLeftPress());
				m_Upkey.Execute(pInput.GetUpPress());
				m_Downkey.Execute(pInput.GetDownPress());
			}
			void		ExcuteKey() {
				this->m_InputGround.Execute();
			}
		};
		class ShapeControl {
		private://キャラパラメーター
		private:
			int													m_Eyeclose{ 0 };
			float												m_EyeclosePer{ 0.f };
		public://ゲッター
			const auto&			GetEyeclosePer() const noexcept { return this->m_EyeclosePer; }
		public:
			void		InitShape() {
				this->m_Eyeclose = 0;
				this->m_EyeclosePer = 0.f;
			}
			void		ExcuteShape() {
				if (this->m_EyeclosePer <= 0.05f && (GetRand(100) == 0)) {
					this->m_Eyeclose = 1;
				}
				if (this->m_EyeclosePer >= 0.95f) {
					this->m_Eyeclose = 0;
				}
				Easing(&this->m_EyeclosePer, (float)this->m_Eyeclose, 0.5f, EasingType::OutExpo);
			}
		};
	};
};
