#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "../CharaAnimData.hpp"
#include "CharacterEnum.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		struct CharaAnimeSet {
			std::array<int, (int)CharaGunAnimeID::Max> m_GunAnimFrame;
		};
		struct GunAnimeSet {
			EnumGunAnim	m_Run{ EnumGunAnim::M1911_run };
			std::vector<EnumGunAnim>	m_Ready;
			std::vector<EnumGunAnim>	m_Aim;
			std::vector<EnumGunAnim>	m_Check;
			EnumGunAnim	m_ADS{ EnumGunAnim::M1911_ads };
			EnumGunAnim	m_Reload{ EnumGunAnim::M1911_reload };
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

		//キャラ入力
		class KeyControl {
		private://キャラパラメーター
		private:
			bool												m_KeyActive{ true };
			bool												m_ReadySwitch{ false };

			std::array<float, 4>								m_Vec{ 0,0,0,0 };
			bool												m_Press_GoFront{ false };
			bool												m_Press_GoRear{ false };
			bool												m_Press_GoLeft{ false };
			bool												m_Press_GoRight{ false };

			int													m_LeanRate{ 0 };
			switchs												m_QKey;
			switchs												m_EKey;

			switchs												m_RKey;
			switchs												m_FKey;
			switchs												m_ShotKey;
			switchs												m_ADSKey;
			switchs												m_Squat;
			switchs												m_Run;
			switchs												m_Action;
			VECTOR_ref											m_rad_Buf, m_rad, m_radAdd;
			VECTOR_ref											m_radEasingPer;
		public:
			const auto		GetRadBuf(void) const noexcept { return  this->m_rad_Buf; }
			const auto		GetRad(void) const noexcept { return  this->m_rad; }

			const auto		GetPressFront(void) const noexcept { return this->m_Press_GoFront; }
			const auto		GetPressRear(void) const noexcept { return this->m_Press_GoRear; }
			const auto		GetPressLeft(void) const noexcept { return this->m_Press_GoLeft; }
			const auto		GetPressRight(void) const noexcept { return this->m_Press_GoRight; }

			const auto		GetVecFront(void) const noexcept { return  this->m_Vec[0]; }
			const auto		GetVecRear(void) const noexcept { return this->m_Vec[2]; }
			const auto		GetVecLeft(void) const noexcept { return this->m_Vec[1]; }
			const auto		GetVecRight(void) const noexcept { return this->m_Vec[3]; }
			const auto		GetVec(void) const noexcept { return VECTOR_ref::vget(GetVecLeft() - GetVecRight(), 0, GetVecRear() - GetVecFront()); }
			const auto		GetFrontP(void) const noexcept {
				auto FrontP = ((GetPressFront() && !GetPressRear())) ? (atan2f(GetVec().x(), -GetVec().z()) * GetVecFront()) : 0.f;
				FrontP += (!GetPressFront() && GetPressRear()) ? (atan2f(-GetVec().x(), GetVec().z()) * GetVecRear()) : 0.f;
				return FrontP;
			}

		public://ゲッター
			const auto		GetQKey(void) const noexcept { return this->m_QKey; }
			const auto		GetEKey(void) const noexcept { return this->m_EKey; }
			const auto		GetLeanRate(void) const noexcept { return this->m_LeanRate; }
			const auto		GetRKey(void) const noexcept { return this->m_RKey; }
			const auto		GetFKey(void) const noexcept { return this->m_FKey; }
			const auto		GetIsSquat(void) const noexcept { return this->m_Squat.on(); }
			const auto		GetRun(void) const noexcept { return this->m_Run.press(); }
			const auto		GetAction(void) const noexcept { return this->m_Action.press(); }
			const auto		GetShotKey(void) const noexcept { return this->m_ShotKey; }
			const auto		GetADSKey(void) const noexcept { return this->m_ADSKey; }
		public://セッター
			void			SetRadBufXY(const VECTOR_ref& buf) noexcept {
				//
				auto xbuf = this->m_rad_Buf.x();
				Easing(&xbuf, buf.x(), 0.9f, EasingType::OutExpo);
				this->m_rad_Buf.x(xbuf);
				//
				this->m_rad_Buf.y(buf.y());
				this->m_rad.y(buf.y());
			}
			void			SetRad_BufY(float y) noexcept {
				this->m_rad_Buf.y(y);
			}
			void			SetRadBufZ(float z) noexcept {
				auto zbuf = this->m_rad_Buf.z();
				Easing(&zbuf, z, 0.9f, EasingType::OutExpo);
				this->m_rad_Buf.z(zbuf);
			}
			void			ResetLeanRate(void) noexcept { this->m_LeanRate = 0; }
		private: //内部
			void			SetVec(int pDir, bool Press) {
				this->m_Vec[pDir] += (Press ? 1.f : -3.f)*5.f / FPS;
				this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
			}
		public:
			void		InitKey(float pxRad, float pyRad);
			void		InputKey(const InputControl& pInput, bool pReady, const VECTOR_ref& pAddRadvec);
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
		class OverrideControl {
		private://キャラパラメーター
			bool												m_PosBufOverRideFlag{ false };
			VECTOR_ref											m_PosBufOverRide;
			VECTOR_ref											m_VecBufOverRide;
			VECTOR_ref											m_RadOverRide;									//
		private:
		public://ゲッター
			//const auto&			GetEyeclosePer() const noexcept { return this->m_EyeclosePer; }
			void			SetPosBufOverRide(const VECTOR_ref& pPos, const VECTOR_ref& pVec, const VECTOR_ref& rad) noexcept {
				this->m_PosBufOverRideFlag = true;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide = pVec;
				this->m_RadOverRide = rad;
			}
		public:
			void		InitOverride(const VECTOR_ref& pPos) {
				this->m_PosBufOverRideFlag = false;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide.clear();
			}
			bool		PutOverride(VECTOR_ref* pPos, VECTOR_ref* pVec, VECTOR_ref* pRad) {
				if (this->m_PosBufOverRideFlag) {
					this->m_PosBufOverRideFlag = false;
					*pPos = this->m_PosBufOverRide;
					*pVec = this->m_VecBufOverRide;
					*pRad = this->m_RadOverRide;
					return true;
				}
				return false;
			}
		};
	};
};
