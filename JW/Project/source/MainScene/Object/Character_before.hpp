#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "CharaAnimData.hpp"
#include "CharacterEnum.hpp"
#include "ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ArmMovePerClass {
			float												m_ArmPer{ 0.f };
			bool												m_Armon{ false };
		public:
			void Init(bool isOn)noexcept {
				m_Armon = isOn;
				m_ArmPer = isOn ? 1.f : 0.f;
			}
			void Execute(bool isOn, float OnOver = 0.2f, float OffOver = 0.2f) noexcept {
				if (isOn) {
					if (m_Armon) {
						Easing(&this->m_ArmPer, 1.f, 0.9f, EasingType::OutExpo);
					}
					else {
						Easing(&this->m_ArmPer, 1.f + OnOver, 0.8f, EasingType::OutExpo);
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
						Easing(&this->m_ArmPer, 0.f - OffOver, 0.8f, EasingType::OutExpo);
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
			std::vector<DamageEvent>							m_DamageEvent;						//ダメージイベント
		protected:
			void			SetHealEvent(PlayerID pID, Sceneclass::ObjType pCharaType, HitPoint value) noexcept {
				this->m_DamageEvent.resize(this->m_DamageEvent.size() + 1);
				this->m_DamageEvent.back().SetEvent(pID, pCharaType, -value, DX_PI_F);
			}
			void			SetSubHPEvent(PlayerID pID, Sceneclass::ObjType pCharaType, HitPoint value,float Rad) noexcept {
				this->m_DamageEvent.resize(this->m_DamageEvent.size() + 1);
				this->m_DamageEvent.back().SetEvent(pID, pCharaType, value, Rad);
			}
		public://ゲッター
			const auto		IsAlive(void) const noexcept { return this->m_HP != 0; }
			const auto&		GetHP(void) const noexcept { return this->m_HP; }
			const auto&		GetHPMax(void) const noexcept { return HPMax; }
			void			SubHP(HitPoint damage_t, float)  noexcept { this->m_HP = std::clamp<HitPoint>(this->m_HP - damage_t, 0, HPMax); }
			auto&			GetDamageEvent(void) noexcept { return this->m_DamageEvent; }
		public:
			void		InitLife() {
				this->m_HP = HPMax;
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
			void			SetIsSquat(bool value) noexcept { this->m_Squat.Set(value); }
			const auto		GetRun(void) const noexcept { return this->m_Run.press() && GetPressFront(); }
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

		class LaserSightClass {
		private:
			bool												m_IsLaserActive{ false };
			VECTOR_ref											LaserStartPos;
			VECTOR_ref											LaserEndPos;
		public://ゲッター
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
			LaserSightClass(void) noexcept { }
			~LaserSightClass(void) noexcept { }
		public:
		};

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
			HitBoxControl(void) noexcept { }
			~HitBoxControl(void) noexcept { }
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
		//人間のフレーム、アニメ管理
		class HumanControl {
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
		protected:
			//体のフレーム情報
			frame_body lagframe_;							//フレーム
			frame_body frame_s;								//フレーム
		protected:
			void Set_Body(MV1& obj_body_t, MV1& obj_lag_t) noexcept {
				//身体
				this->frame_s.Get_frame(obj_body_t);
				//ラグドール
				this->lagframe_.Get_frame(obj_lag_t);
			}
			void Frame_Copy_Lag(MV1& obj_body_t, MV1* obj_lag_t) { this->frame_s.copy_frame(obj_body_t, this->lagframe_, obj_lag_t); }
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
			WalkSwingControl(void) noexcept { }
			~WalkSwingControl(void) noexcept { }
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
			GunSwingControl(void) noexcept { }
			~GunSwingControl(void) noexcept { }
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
			float												m_IsStuckLeftHandTimer{ 0.f };
			bool												m_IsStuckLeftHand{ false };
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
			StackLeftHandControl(void) noexcept { }
			~StackLeftHandControl(void) noexcept { }
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
	};
};
