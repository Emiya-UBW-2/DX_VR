#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		struct CharaAnimeSet {
			CharaAnimeID	m_Aim{ CharaAnimeID::Upper_Aim };

			CharaAnimeID	m_Shot{ CharaAnimeID::Upper_Shot };

			CharaAnimeID	m_Cocking{ CharaAnimeID::Upper_Cocking };

			CharaAnimeID	m_Reload{ CharaAnimeID::Upper_ReloadStart };
		};
		class CharacterClass : public ObjectBaseClass {
		private://キャラパラメーター
			const float SpeedLimit{ 0.8f };
			const float UpperTimerLimit = 3.f;
			const float HeartRateMin{ 60.f };//心拍数最小
			const float HeartRateMax{ 180.f };//心拍数最大
			const float StaminaMax{ 100.f };
			const float HPMax = 100.f;
			std::string											m_Name;
			CharaTypeID											m_CharaType;
			std::array<CharaAnimeSet, 3>						m_CharaAnimeSet;
			int													m_CharaAnimeSel;
		private:
			bool												m_KeyActive{ true };
			bool												m_ReadySwitch{ false };
			CharacterMoveGroundControl							m_InputGround;
			switchs												m_Rightkey;
			switchs												m_Leftkey;
			switchs												m_Upkey;
			switchs												m_Downkey;
			//
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			VECTOR_ref											m_PosBuf;
			bool												m_PosBufOverRideFlag{ false };
			VECTOR_ref											m_PosBufOverRide;
			VECTOR_ref											m_VecBufOverRide;
			VECTOR_ref											m_RadOverRide;									//

			MATRIX_ref											m_UpperMatrix;
			float												m_LeanRad{ 0.f };
			float												m_Speed{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			float												m_NeckPer{ 0.f };
			float												m_SlingPer{ 0.f };
			bool												m_TurnBody{ false };
			bool												m_ReturnStand{ false };
			bool												m_RunReady{ false };
			bool												m_RunReadyFirst{ false };
			bool												m_Running{ false };
			CharaAnimeID										m_UpperAnimSelect, m_PrevUpperAnimSel;
			CharaAnimeID										m_BottomAnimSelect;
			MATRIX_ref											m_RightWristRot;
			//銃
			float												m_ReadyTimer{ 0.f };
			float												m_ReadyPer{ 0.f };
			int													m_ShotPhase{ 0 };
			float												m_LoadAmmoPer{ 0.f };
			float												m_NeckPosOffsetPer{ 0.f };
			float												m_MoveEyePosTimer{ 0.f };
			VECTOR_ref											m_MoveEyePos;
			//入力
			bool												m_Press_Shot{ false };
			bool												m_Press_Reload{ false };
			bool												m_Press_Aim{ false };
			//体力
			float												m_HeartRate{ HeartRateMin };//心拍数
			float												m_HeartRate_r{ HeartRateMin };//心拍数
			float												m_HeartRateRad{ 0.f };//呼吸Sin渡し
			float												m_Stamina{ StaminaMax };//スタミナ
			bool												m_CannotSprint{ false };//スタミナ切れ

			float												m_HP{ 0.f };							//スコア

			float m_LeftHandPer{ 0.f };

			VECTOR_ref											m_RecoilRadAdd;
			//表情
			int													m_Eyeclose{ 0 };
			float												m_EyeclosePer{ 0.f };
			//サウンド
			int													m_CharaSound{ -1 };
			SoundHandle											m_Breath;
			bool												m_HeartSoundFlag{ false };
			std::shared_ptr<GunClass>							m_Gun_Ptr{ nullptr };			//銃
			//
			//std::shared_ptr<CharacterClass>						m_LockOn{ nullptr };
			//
			bool												m_SendCamShake{ false };
		public://ゲッター
			void			SetAnimOnce(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimOnce((int)ID, speed); }
			void			SetAnimLoop(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimLoop((int)ID, speed); }
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { GetObj().frame_Reset(m_Frames[(int)frame].first); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(m_Frames[(int)frame].first, value * m_Frames[(int)frame].second); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { m_Shapes[(int)pShape].second = Per; }
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			AddHP(float value) noexcept { this->m_HP = std::clamp(this->m_HP + value, 0.f, HPMax); }
			void			SubHP(float value) noexcept { this->m_HP = std::clamp(this->m_HP - value, 0.f, HPMax); }
			void			SetHP(float value) noexcept { this->m_HP = value; }
			void			SetPosBufOverRide(const VECTOR_ref& pPos, const VECTOR_ref& pVec, const VECTOR_ref& rad) noexcept {
				this->m_PosBufOverRideFlag = true;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide = pVec;
				this->m_RadOverRide = rad;
			}

			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->GetObj().get_anime((int)anim); }
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix(m_Frames[(int)frame].first); }
			const auto		GetParentFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix((int)this->GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix(m_Frames[(int)frame].first); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix((int)this->GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			const auto		GetRadBuf(void) const noexcept { return this->m_InputGround.GetRadBuf(); }
			//
			const auto		GetPressFrontGround(void) const noexcept { return this->m_InputGround.GetPressFront(); }
			const auto		GetPressRearGround(void) const noexcept { return this->m_InputGround.GetPressRear(); }
			const auto		GetPressLeftGround(void) const noexcept { return this->m_InputGround.GetPressLeft(); }
			const auto		GetPressRightGround(void) const noexcept { return this->m_InputGround.GetPressRight(); }
			//
			void SetGunPtr(std::shared_ptr<GunClass>& pGunPtr) noexcept {
				this->m_Gun_Ptr = pGunPtr;
				if (this->m_Gun_Ptr != nullptr) {
					this->m_CharaAnimeSel = this->m_Gun_Ptr->GetHumanAnimType();
				}
			}
			void LoadReticle(void) noexcept {
				if (this->m_Gun_Ptr != nullptr) {
					this->m_Gun_Ptr->LoadReticle();
				}
			}
			auto&			GetGunPtr(void) noexcept { return this->m_Gun_Ptr; }
			const auto		GetShotSwitch(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetIsShot() : false; }
			const auto		GetCharaDir(void) const noexcept { return this->m_UpperMatrix * this->m_move.mat; }
			const auto		GetEyeVector(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? (Lerp(GetCharaDir().zvec(), this->m_Gun_Ptr->GetMatrix().zvec(), this->m_ReadyPer) * -1.f) : (GetCharaDir().zvec() * -1.f); }
			const auto		GetEyePosition(void) const noexcept {
				return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + this->GetEyeVector().Norm() * 1.5f
					+ GetCharaDir().GetRot().yvec() * this->m_NeckPosOffsetPer
					+ m_MoveEyePos;
			}
			const auto		GetScopePos(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetScopePos() : GetEyePosition(); }
			const auto		GetLensPos(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetLensPos() : VECTOR_ref::zero(); }
			const auto		GetReticleSize(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetReticleSize() : 1.f; }
			const auto		GetReticlePos(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetReticlePos() : VECTOR_ref::zero(); }
			const auto&		GetReticleRad(void) noexcept { return this->m_LeanRad; }
			const auto		GetLensPosSize(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetLensPosSize() : VECTOR_ref::zero(); }
			const auto		GetCanshot(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? (this->m_Gun_Ptr->GetCanshot()) : false; }
			const auto		GetAmmoNum(void) const noexcept { return this->m_Gun_Ptr->GetAmmoNum(); }
			const auto		GetAmmoAll(void) const noexcept { return this->m_Gun_Ptr->GetAmmoAll(); }
			const auto&		GetReticle(void) noexcept { return this->m_Gun_Ptr->GetReticle(); }
			const auto		GetIsRun(void) const noexcept { return this->m_InputGround.GetRun(); }
			const auto&		GetHeartRate(void) const noexcept { return this->m_HeartRate; }
			const auto&		GetHeartRateRad(void) const noexcept { return this->m_HeartRateRad; }
			const auto&		GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd; }
			
			const auto		GetHeartRandVec(void) const noexcept {
				auto tmp2 = 0.2f * GetRandf(deg2rad(1.f));
				auto tmp3 = Lerp(0.5f, 0.35f, this->m_InputGround.GetSquatPer());
				VECTOR_ref tmpvec = VECTOR_ref::vget(
					tmp2 + 0.0002f * sin(this->m_HeartRateRad) * powf(this->m_HeartRate / HeartRateMin, 3.f),
					tmp2 + 0.0002f * sin(this->m_HeartRateRad * 3) * powf(this->m_HeartRate / HeartRateMin, 3.f),
					0.f
				);
				return tmpvec * tmp3;
			}
			const auto&		GetStamina(void) const noexcept { return this->m_Stamina; }
			const auto&		GetStaminaMax(void) const noexcept { return this->StaminaMax; }
			const auto		GetTurnRatePer(void) const noexcept { return this->m_InputGround.GetTurnRatePer(); }
			const auto		GetShotAnimSel(void) const noexcept { return m_CharaAnimeSet[this->m_CharaAnimeSel].m_Shot; }
			const auto		GetCockingAnimSel(void) const noexcept { return m_CharaAnimeSet[this->m_CharaAnimeSel].m_Cocking; }
			const auto		GetAimAnimSel(void) const noexcept { return m_CharaAnimeSet[this->m_CharaAnimeSel].m_Aim; }
			const auto		GetReloadStartAnimSel(void) const noexcept { return m_CharaAnimeSet[this->m_CharaAnimeSel].m_Reload; }
			const auto		GetReloadOneAnimSel(void) const noexcept { return  (CharaAnimeID)((int)GetReloadStartAnimSel() + 1); }
			const auto		GetReloadEndAnimSel(void) const noexcept { return  (CharaAnimeID)((int)GetReloadStartAnimSel() + 2); }
			const auto		GetIsADS(void) const noexcept { return this->m_ReadyTimer == 0.f; }
			const auto		GetReadyPer(void) const noexcept { return this->m_ReadyPer; }
			const auto		GetEyeVecMat(void) const noexcept { return GetCharaDir(); }
			const auto&		GetPosBuf(void) const noexcept { return this->m_PosBuf; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetHP(void) const noexcept { return this->m_HP; }
			const auto&		GetHPMax(void) const noexcept { return HPMax; }
			const auto&		GetSendCamShake(void) const noexcept { return this->m_SendCamShake; }
			const auto&		GetName(void) const noexcept { return this->m_Name; }
		public:
			//
			void			ValueSet(float pxRad, float pyRad, bool SquatOn, const VECTOR_ref& pPos, PlayerID pID) noexcept {
				this->m_MyID = pID;
				//
				this->m_KeyActive = false;
				//
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					GetAnimeBuf((CharaAnimeID)i) = 0.f;
				}
				this->m_ReadyPer = 0.f;
				this->m_ReadyTimer = UpperTimerLimit;
				this->m_HeartRate = HeartRateMin;
				this->m_Stamina = StaminaMax;

				this->m_Speed = 0.f;
				this->m_RunPer2 = 0.f;
				this->m_PrevRunPer2 = 0.f;
				this->m_NeckPer = 0.f;
				this->m_HeartRateRad = 0.f;
				this->m_LeftHandPer = 0.f;
				this->m_TurnBody = false;
				this->m_ReturnStand = false;
				this->m_ShotPhase = 0;
				this->m_NeckPosOffsetPer = -1.f;
				this->m_MoveEyePosTimer = 0.f;
				this->m_RunReady = false;
				this->m_RunReadyFirst = false;
				this->m_Running = false;
				this->m_ReadySwitch = false;

				this->m_Press_Shot = false;
				this->m_Press_Reload = false;
				this->m_Press_Aim = false;
				this->m_KeyActive = false;
				this->m_HeartSoundFlag = false;
				//this->m_UpperAnimSelect;
				//this->m_PrevUpperAnimSel;
				//this->m_BottomAnimSelect;
				this->m_HP = HPMax;
				//表情
				this->m_Eyeclose = 0;
				this->m_EyeclosePer = 0.f;
				//サウンド
				this->m_CharaSound = -1;
				//動作にかかわる操作
				this->m_InputGround.ValueSet(pxRad, pyRad, SquatOn);
				this->m_PosBuf = pPos;
				this->m_PosBufOverRideFlag = false;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide.clear();
				//上記を反映するもの
				this->m_yrad_Upper = this->m_InputGround.GetRad().y();
				this->m_yrad_Bottom = this->m_InputGround.GetRad().y();
				this->m_LeanRad = deg2rad(15) * this->m_InputGround.GetLeanRatePer();
				this->m_LeanRad = 0.f;
				this->m_UpperMatrix =
					MATRIX_ref::RotZ(this->m_LeanRad) *
					MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) *
					MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				this->m_move.vec.clear();
				SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), this->m_PosBuf);
			}
			void			SetInput(const InputControl& pInput, bool pReady) {
				this->m_ReadySwitch = (this->m_KeyActive != pReady);
				this->m_KeyActive = pReady;

				//地
				m_InputGround.SetInput(
					pInput.GetAddxRad(), pInput.GetAddyRad(),
					GetHeartRandVec(),
					pInput.GetGoFrontPress(),
					pInput.GetGoBackPress(),
					pInput.GetGoLeftPress(),
					pInput.GetGoRightPress(),
					pInput.GetAction4(),
					pInput.GetRunPress(),
					false,
					pInput.GetQPress(),
					pInput.GetEPress(),
					this->m_CannotSprint
				);
				//AIM
				if (this->m_Gun_Ptr != nullptr) {
					this->m_Press_Shot = pInput.GetAction5() && this->m_KeyActive;
					this->m_Press_Reload = (pInput.GetAction2() && this->m_KeyActive && (this->m_Gun_Ptr->GetAmmoNum() <= this->m_Gun_Ptr->GetAmmoAll()));
					if ((this->m_Gun_Ptr->GetAmmoNum() == 0) && !this->m_Gun_Ptr->GetCanshot()) {
						if (this->m_Press_Shot) {
							this->m_Press_Shot = false;
							this->m_Press_Reload = true;
						}
					}
				}
				else {
					this->m_Press_Shot = false;
					this->m_Press_Reload = false;
				}
				this->m_Press_Aim = pInput.GetAction6() && this->m_KeyActive;
				//
				m_Rightkey.GetInput(pInput.GetRightPress());
				m_Leftkey.GetInput(pInput.GetLeftPress());
				m_Upkey.GetInput(pInput.GetUpPress());
				m_Downkey.GetInput(pInput.GetDownPress());
				//
				{
					auto TmpReady = !(!GetIsRun() || (this->m_ShotPhase >= 2));
					this->m_RunReadyFirst = (TmpReady && !this->m_RunReady);
					this->m_RunReady = TmpReady;
				}
			}
			void			SetEyeVec(const VECTOR_ref& camvec) noexcept {
				GetObj().frame_Reset(this->m_Frames[(int)CharaFrame::Head].first);
				auto v1 = (GetFrameWorldMat(CharaFrame::Head).GetRot() * GetCharaDir().Inverse()).zvec()*-1.f;
				auto v2 = Lerp(MATRIX_ref::Vtrans(camvec.Norm(), GetCharaDir().Inverse()), v1, m_NeckPer);

				auto radlimit = deg2rad(70);
				if (v1.dot(v2) <= cos(radlimit)) {
					v2 = v1 * cos(radlimit) + v1.cross(v1.cross(v2)) * (-sin(radlimit));
				}
				SetFrameLocalMat(CharaFrame::Head, MATRIX_ref::RotVec2(v1, v2) * GetFrameLocalMat(CharaFrame::Head).GetRot());
			}
		private: //更新関連
			//以前の状態保持														//
			void			ExecuteSavePrev(void) noexcept {
				this->m_PrevUpperAnimSel = this->m_UpperAnimSelect;
				this->m_PrevRunPer2 = this->m_RunPer2;
			}
			//操作																	//0.01ms
			void			ExecuteInput(void) noexcept {
				//
				if (this->m_Gun_Ptr != nullptr) {
					this->m_Gun_Ptr->SetUseMoveParts(this->m_SlingPer < 0.1f);

					this->m_Gun_Ptr->SetIsShot(false);
					if ((this->m_Press_Shot || this->m_Press_Reload) && (this->m_ShotPhase == 0)) {
						if (this->m_ReadyTimer >= 0.1f) {
							this->m_ReadyTimer = 0.1f;
						}
						if (this->m_ReadyPer >= 0.9f) {
							this->m_ReadyPer = 1.f;
							if (this->m_Press_Shot) {
								if (this->m_Gun_Ptr->GetCanshot()) {
									this->m_ShotPhase = 1;
									this->m_Gun_Ptr->SetBullet();
									this->m_RecoilRadAdd.Set(GetRandf(0.005f), -0.03f, 0.f);

									this->m_RecoilRadAdd = MATRIX_ref::Vtrans(this->m_RecoilRadAdd, MATRIX_ref::RotZ(-m_LeanRad));
								}
								else {
									if (!this->m_Gun_Ptr->GetIsMagEmpty()) {
										this->m_ShotPhase = 2;
									}
									else {
										this->m_ShotPhase = 3;
									}
								}
							}
							if (this->m_Press_Reload) {
								this->m_ShotPhase = 3;
							}
						}
					}
					else {
						this->m_ReadyTimer += 1.f / FPS;
						this->m_ReadyTimer = std::clamp(this->m_ReadyTimer, 0.f, this->m_Press_Aim ? 0.f : UpperTimerLimit);
					}
					if ((this->m_ShotPhase >= 2) || GetIsRun()) {
						this->m_ReadyTimer = UpperTimerLimit;
					}
				}
				else {
					this->m_ReadyTimer += 1.f / FPS;
					this->m_ReadyTimer = std::clamp(this->m_ReadyTimer, 0.f, UpperTimerLimit);
					if (this->m_Speed > 0.6f) {
						this->m_ReadyTimer = UpperTimerLimit;
					}
					if (GetIsRun()) {
						this->m_ReadyTimer = UpperTimerLimit;
					}
				}

				if (this->m_RecoilRadAdd.y() < 0.f) {
					Easing(&this->m_RecoilRadAdd, VECTOR_ref::vget(0.f,0.09f,0.f), 0.9f, EasingType::OutExpo);
				}
				else {
					Easing(&this->m_RecoilRadAdd, VECTOR_ref::zero(), 0.7f, EasingType::OutExpo);
				}
				//
				Easing(&this->m_ReadyPer, (this->m_ReadyTimer < UpperTimerLimit) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				//this->m_yrad_Upper、this->m_yrad_Bottom決定
				this->m_InputGround.Execute();
				if (this->m_Speed <= 0.1f) {
					if (abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) > deg2rad(50.f)) {
						this->m_TurnBody = true;
					}
					if (abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) < deg2rad(0.5f)) {
						this->m_TurnBody = false;
					}
				}
				else {
					this->m_TurnBody = false;
				}
				//
				auto FrontP = this->m_InputGround.GetFrontP();
				auto TmpRunPer = Lerp(0.85f, 0.7f, this->m_InputGround.GetRunPer());
				if (this->m_TurnBody || (this->m_Speed > 0.1f)) { Easing(&this->m_yrad_Upper, this->m_InputGround.GetRad().y(), TmpRunPer, EasingType::OutExpo); }
				auto OLDP = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP, TmpRunPer, EasingType::OutExpo);
				this->m_InputGround.SetRadBufZ((this->m_yrad_Bottom - OLDP) * 2.f);
			}
			//上半身回転															//0.06ms
			void			ExecuteUpperMatrix(void) noexcept {
				this->m_LeanRad = deg2rad(15) * this->m_InputGround.GetLeanRatePer();
				this->m_UpperMatrix =
					MATRIX_ref::RotZ(this->m_LeanRad) *
					MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) *
					MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);

				GetObj().frame_Reset(this->m_Frames[(int)CharaFrame::Upper].first);
				SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * this->m_UpperMatrix);
			}
			//SetMat指示															//0.03ms
			void			ExecuteAnim(void) noexcept {
				auto SE = SoundPool::Instance();
				//アニメ演算
				{
					//上半身
					{
						this->m_SlingPer = 0.f;
						this->m_UpperAnimSelect = (!this->m_KeyActive) ? CharaAnimeID::Upper_Ready : CharaAnimeID::Upper_Down;
						if (this->m_ReadySwitch) {
							this->m_RunReadyFirst = false;
						}
						bool canreverse = true;
						if (!this->m_RunReady && !this->m_Running) {
							this->m_RunReady = true;
							canreverse = false;
						}
						if (this->m_RunReady) {
							if (!this->m_Running) {
								this->m_Running = true;
							}
						}
						else {
							this->m_Running = false;
						}
						if (this->m_ReadyTimer < UpperTimerLimit) {
							m_UpperAnimSelect = GetAimAnimSel();
						}
						if (this->m_ShotPhase == 1) {
							m_UpperAnimSelect = GetShotAnimSel();
							SetAnimOnce(m_UpperAnimSelect, 1.5f);
							bool ischeck = true;
							switch (this->m_Gun_Ptr->GetShotType()) {
							case SHOTTYPE::FULL:
								break;
							case SHOTTYPE::SEMI:
							case SHOTTYPE::BOLT:
								ischeck = !this->m_Press_Shot;
								break;
							default:
								break;
							}

							if (GetAnime(m_UpperAnimSelect).TimeEnd() && ischeck) {
								GetAnime(m_UpperAnimSelect).GoStart();
								if (!this->m_Gun_Ptr->GetIsMagEmpty()) {
									switch (this->m_Gun_Ptr->GetShotType()) {
									case SHOTTYPE::FULL:
									case SHOTTYPE::SEMI:
										this->m_ShotPhase = 0;
										m_UpperAnimSelect = GetAimAnimSel();
										break;
									case SHOTTYPE::BOLT:
										this->m_ShotPhase = 2;
										break;
									default:
										break;
									}
								}
								else {
									this->m_ShotPhase = 0;
									m_UpperAnimSelect = GetAimAnimSel();
								}
							}
						}
						//コッキング
						if (this->m_ShotPhase == 2) {
							m_UpperAnimSelect = GetCockingAnimSel();
							SetAnimLoop(m_UpperAnimSelect, 1.5f);
							if (GetAnime(m_UpperAnimSelect).time == 0.f) {
								this->m_ShotPhase = 0;
								this->m_ReadyTimer = 0.1f;
							}
						}
						//リロード開始
						if (this->m_ShotPhase == 3) {
							m_UpperAnimSelect = GetReloadStartAnimSel();
							SetAnimLoop(m_UpperAnimSelect, 1.5f);
							if (GetAnime(m_UpperAnimSelect).time == 0.f) {
								this->m_ShotPhase++;
								this->m_ReadyTimer = 0.1f;
							}
						}
						if (this->m_ShotPhase == 4) {
							m_UpperAnimSelect = GetReloadOneAnimSel();
							SetAnimLoop(m_UpperAnimSelect, 1.5f);
							if (GetAnime(m_UpperAnimSelect).time == 0.f) {
								switch (this->m_Gun_Ptr->GetReloadType()) {
								case RELOADTYPE::MAG:
									this->m_ShotPhase++;
									break;
								case RELOADTYPE::AMMO:
									if (this->m_Gun_Ptr->GetIsMagFull()) {
										this->m_ShotPhase++;
									}
									break;
								default:
									break;
								}
								this->m_ReadyTimer = 0.1f;
							}
						}
						if (this->m_ShotPhase == 5) {
							m_UpperAnimSelect = GetReloadEndAnimSel();
							SetAnimLoop(m_UpperAnimSelect, 1.5f);
							if (GetAnime(m_UpperAnimSelect).time == 0.f) {
								if (this->m_Gun_Ptr->GetCanshot()) {
									this->m_ShotPhase = 0;
								}
								else {
									if (!this->m_Gun_Ptr->GetIsMagEmpty()) {
										this->m_ShotPhase = 2;
									}
									else {
										this->m_ShotPhase = 3;
									}
								}
								this->m_ReadyTimer = 0.1f;
							}
						}
						//
						{
							Easing(&this->m_NeckPosOffsetPer, (this->m_ShotPhase <= 2) ? -1.f : 0.f, 0.9f, EasingType::OutExpo);
							if (this->m_Speed > 0.f) {
								this->m_MoveEyePosTimer += this->m_Speed * deg2rad(300.f + 200.f*this->m_InputGround.GetRunPer()) / FPS;

							}
							else {
								this->m_MoveEyePosTimer = 0.f;
							}
							Easing(&this->m_MoveEyePos,
								GetCharaDir().GetRot().xvec() * 0.25f*sin(this->m_MoveEyePosTimer)*this->m_Speed
								+ GetCharaDir().GetRot().yvec() * -0.25f*std::abs(cos(this->m_MoveEyePosTimer))*this->m_Speed
								, 0.9f, EasingType::OutExpo);
						}
						//
						switch (this->m_Gun_Ptr->GetReloadType()) {
						case RELOADTYPE::MAG:
						{
							switch (this->m_ShotPhase) {
							case 3:
								this->m_LoadAmmoPer = 0.f;
								break;
							case 4:
								if (GetAnime(m_UpperAnimSelect).time <= 35.f) {
									Easing(&this->m_LoadAmmoPer, 1.f, 0.7f, EasingType::OutExpo);
								}
								else {
									Easing(&this->m_LoadAmmoPer, 0.f, 0.7f, EasingType::OutExpo);
								}
								break;
							case 5:
								this->m_LoadAmmoPer = 0.f;
								break;
							default:
								this->m_LoadAmmoPer = 0.f;
								break;
							}
						}
							break;
						case RELOADTYPE::AMMO:
						{
							switch (this->m_ShotPhase) {
							case 3:
								if (GetAnime(m_UpperAnimSelect).time >= 37.f) {
									this->m_LoadAmmoPer = 1.f;
								}
								break;
							case 4:
								if (GetAnime(m_UpperAnimSelect).time <= 18.f) {
									this->m_LoadAmmoPer = 1.f;
								}
								else {
									Easing(&this->m_LoadAmmoPer, 0.f, 0.7f, EasingType::OutExpo);
								}
								break;
							case 5:
								this->m_LoadAmmoPer = 0.f;
								break;
							default:
								this->m_LoadAmmoPer = 0.f;
								break;
							}
						}
							break;
						default:
							break;
						}

						//首
						Easing(&m_NeckPer, (
							m_UpperAnimSelect == CharaAnimeID::Upper_Down
							|| m_UpperAnimSelect == CharaAnimeID::Upper_Running
							) ? 0.f : 1.f, 0.9f, EasingType::OutExpo);
					}
					//下半身
					{
						if (GetPressFrontGround()) {
							this->m_BottomAnimSelect = (this->m_InputGround.GetRun()) ? CharaAnimeID::Bottom_Run : CharaAnimeID::Bottom_Walk;
						}
						else {
							this->m_BottomAnimSelect = CharaAnimeID::Bottom_Stand;
							if (GetPressLeftGround()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_LeftStep; }
							if (GetPressRightGround()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_RightStep; }
							if (GetPressRearGround()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_WalkBack; }
							if (this->m_InputGround.GetSprint()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_Run; }
						}
					}
					//
					SetAnimLoop(CharaAnimeID::Upper_Sprint, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Upper_Running, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Turn, 0.5f);
					SetAnimLoop(CharaAnimeID::Bottom_Run, 1.25f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Walk, 1.15f * this->m_InputGround.GetVecFront());
					SetAnimLoop(CharaAnimeID::Bottom_LeftStep, 1.15f * this->m_InputGround.GetVecLeft());
					SetAnimLoop(CharaAnimeID::Bottom_WalkBack, 1.15f * this->m_InputGround.GetVecRear());
					SetAnimLoop(CharaAnimeID::Bottom_RightStep, 1.15f * this->m_InputGround.GetVecRight());

					Easing(&this->m_LeftHandPer,
						(
							GetAnimeBuf(CharaAnimeID::Upper_RunningStart) > 0.1f
							|| GetAnimeBuf(CharaAnimeID::Upper_Running) > 0.5f
							|| GetAnimeBuf(CharaAnimeID::Upper_Sprint) > 0.5f
							|| GetAnimeBuf(CharaAnimeID::Upper_Ready) > 0.1f
							) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
				}
				//アニメセレクト
				{
					//上半身
					if (
						m_PrevUpperAnimSel == GetAimAnimSel() && m_UpperAnimSelect == GetShotAnimSel()) {
						GetAnimeBuf(m_PrevUpperAnimSel) = 0.f;
						GetAnimeBuf(m_UpperAnimSelect) = 1.f;
					}
					if (
						m_PrevUpperAnimSel == GetShotAnimSel()
						&& m_UpperAnimSelect == GetAimAnimSel()) {
						GetAnimeBuf(m_PrevUpperAnimSel) = 0.f;
						GetAnimeBuf(m_UpperAnimSelect) = 1.f;
					}
					if (
						(m_PrevUpperAnimSel == GetShotAnimSel() || m_PrevUpperAnimSel == GetCockingAnimSel() || m_PrevUpperAnimSel == GetReloadEndAnimSel())
						&& m_UpperAnimSelect == GetAimAnimSel()) {
						GetAnimeBuf(m_PrevUpperAnimSel) = 0.f;
						GetAnimeBuf(m_UpperAnimSelect) = 1.f;
					}

					if (
						(m_PrevUpperAnimSel == GetCockingAnimSel() || m_PrevUpperAnimSel == GetReloadEndAnimSel())
						&& m_UpperAnimSelect == CharaAnimeID::Upper_RunningStart) {
						GetAnimeBuf(m_PrevUpperAnimSel) = 0.f;
						GetAnimeBuf(m_UpperAnimSelect) = 1.f;
					}

					//真ん中
					GetAnimeBuf(CharaAnimeID::Mid_Squat) = this->m_InputGround.GetSquatPer();
					//下半身
					Easing(&GetAnimeBuf(CharaAnimeID::Bottom_Turn), (this->m_TurnBody) ? abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
					//銃操作
					GetAnimeBuf(GetShotAnimSel()) = ((GetShotAnimSel() == m_UpperAnimSelect) ? 1.f : 0.f);
					GetAnimeBuf(GetCockingAnimSel()) = ((GetCockingAnimSel() == m_UpperAnimSelect) ? 1.f : 0.f);
					GetAnimeBuf(GetReloadStartAnimSel()) = ((GetReloadStartAnimSel() == m_UpperAnimSelect) ? 1.f : 0.f);
					GetAnimeBuf(GetReloadOneAnimSel()) = ((GetReloadOneAnimSel() == m_UpperAnimSelect) ? 1.f : 0.f);
					GetAnimeBuf(GetReloadEndAnimSel()) = ((GetReloadEndAnimSel() == m_UpperAnimSelect) ? 1.f : 0.f);
					//その他
					for (int i = 2; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//上半身
						if (
							i == (int)CharaAnimeID::Upper_Ready ||
							i == (int)CharaAnimeID::Upper_Down ||
							i == (int)CharaAnimeID::Upper_RunningStart ||
							i == (int)CharaAnimeID::Upper_Running ||
							i == (int)CharaAnimeID::Upper_Sprint ||
							i == (int)GetAimAnimSel() ||
							i == (int)GetShotAnimSel() ||
							i == (int)GetCockingAnimSel() ||
							i == (int)GetReloadStartAnimSel() ||
							i == (int)GetReloadOneAnimSel() ||
							i == (int)GetReloadEndAnimSel()
							)
						{
							GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_UpperAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						//下半身
						if (
							i == (int)CharaAnimeID::Bottom_Stand ||
							i == (int)CharaAnimeID::Bottom_Run ||
							i == (int)CharaAnimeID::Bottom_Walk ||
							i == (int)CharaAnimeID::Bottom_LeftStep ||
							i == (int)CharaAnimeID::Bottom_RightStep ||
							i == (int)CharaAnimeID::Bottom_WalkBack
							)
						{
							GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_BottomAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						GetAnimeBuf((CharaAnimeID)i) = std::clamp(GetAnimeBuf((CharaAnimeID)i), 0.f, 1.f);
						//反映
						GetAnime((CharaAnimeID)i).per = this->m_AnimPerBuf[i];
					}
					//
				}
				//足音
				{
					if (this->m_BottomAnimSelect != CharaAnimeID::Bottom_Stand) {
						auto Time = GetAnime(this->m_BottomAnimSelect).time;
						if (this->m_BottomAnimSelect != CharaAnimeID::Bottom_Run) {
							//L
							if ((9.f < Time && Time < 10.f)) {
								if (this->m_CharaSound != 1) {
									this->m_CharaSound = 1;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale_Rate * 5.f);
								}
							}
							//R
							if ((27.f < Time &&Time < 28.f)) {
								if (this->m_CharaSound != 3) {
									this->m_CharaSound = 3;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 5.f);
								}
							}
						}
						else {
							//L
							if (
								(18.f < Time &&Time < 19.f) ||
								(38.f < Time &&Time < 39.f)
								) {
								if (this->m_CharaSound != 5) {
									this->m_CharaSound = 5;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale_Rate * 5.f);
								}
							}
							//R
							if (
								(8.f < Time &&Time < 9.f) ||
								(28.f < Time &&Time < 29.f)
								) {
								if (this->m_CharaSound != 6) {
									this->m_CharaSound = 6;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 5.f);
								}
							}
						}
						this->m_ReturnStand = true;
					}
					else if (this->m_ReturnStand) {
						if (this->m_CharaSound != 7) {
							this->m_CharaSound = 7;
							SE->Get((int)SoundEnum::SlideFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 5.f, (int)(192.f * this->m_RunPer2 / SpeedLimit));
						}
						this->m_ReturnStand = false;
					}
				}
				//
				GetObj().work_anime();
			}
			//SetMat指示更新														//0.03ms
			void			ExecuteMatrix(void) noexcept {
				this->m_RunPer2 = Lerp(0.35f, (SpeedLimit * (1.f + 0.5f * this->m_InputGround.GetSprintPer())), this->m_InputGround.GetRunPer());
				this->m_RunPer2 = Lerp(this->m_RunPer2, 0.15f, this->m_InputGround.GetSquatPer());
				if (this->m_PrevRunPer2 == 0.f) {
					this->m_PrevRunPer2 = this->m_RunPer2;
				}

				auto OLDpos = this->m_PosBuf;
				VECTOR_ref vecBuf = MATRIX_ref::Vtrans(this->m_InputGround.GetVec(), MATRIX_ref::RotY(this->m_yrad_Upper));
				this->m_Speed = std::clamp(vecBuf.size(), 0.f, 1.f);

				if (this->m_Speed > 0.1f) {
					this->m_move.vec.x((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).x());
					this->m_move.vec.z((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).z());
				}
				else {
					this->m_move.vec.x(vecBuf.x());
					this->m_move.vec.z(vecBuf.z());
				}
				{
					auto HitResult = this->m_MapCol->CollCheck_Line(this->m_PosBuf + VECTOR_ref::up() * -1.f, this->m_PosBuf + VECTOR_ref::up() * 15.f);
					if (HitResult.HitFlag == TRUE) {
						auto yPos = this->m_PosBuf.y();
						Easing(&yPos, HitResult.HitPosition.y, 0.8f, EasingType::OutExpo);
						this->m_PosBuf.y(yPos);
						this->m_move.vec.y(0.f);
					}
					else {
						this->m_move.vec.yadd(M_GR / (FPS * FPS));
					}
				}
				this->m_PosBuf += this->m_move.vec;
				col_wall(OLDpos, &this->m_PosBuf, *this->m_MapCol);

				if (this->m_PosBufOverRideFlag) {
					this->m_PosBufOverRideFlag = false;
					this->m_PosBuf = this->m_PosBufOverRide;
					this->m_move.vec = this->m_VecBufOverRide;
					this->m_InputGround.SetRadBufX(this->m_RadOverRide.x());
					this->m_InputGround.SetRadBufY(this->m_RadOverRide.y());
				}

				this->m_move.mat = MATRIX_ref::RotZ(this->m_InputGround.GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom);
				Easing(&this->m_move.pos, this->m_PosBuf, 0.9f, EasingType::OutExpo);
				UpdateMove();
				//銃座標指定(アニメアップデート含む)//0.19ms
				if (this->m_Gun_Ptr != nullptr) {
					//持ち手探索
					VECTOR_ref yVec1, zVec1, Pos1;
					{
						int i = (int)GetCockingAnimSel();
						bool change = (GetAnime((CharaAnimeID)i).per == 1.f);
						if (!change) { i = (int)GetReloadStartAnimSel(); }
						change = (GetAnime((CharaAnimeID)i).per == 1.f);
						if (!change) { i = (int)GetReloadOneAnimSel(); }
						change = (GetAnime((CharaAnimeID)i).per == 1.f);
						if (!change) { i = (int)GetReloadEndAnimSel(); }
						change = (GetAnime((CharaAnimeID)i).per == 1.f);
						//
						if (change) {//タイムでやれば歩きイランのでは？
							GetAnime(GetAimAnimSel()).per = 1.f;
							GetAnime((CharaAnimeID)i).per = 0.f;
						}
						GetAnime(CharaAnimeID::RightHand).per = 0.f;


						GetAnime(CharaAnimeID::LeftHand).per = ((
							(m_UpperAnimSelect != CharaAnimeID::Upper_RunningStart)
							&& (m_UpperAnimSelect != CharaAnimeID::Upper_Running)
							&& (m_UpperAnimSelect != CharaAnimeID::Upper_Sprint)
							&& (m_UpperAnimSelect != CharaAnimeID::Upper_Ready)
							) ? 1.f : 0.f);

						//アニメアップデート
						GetObj().work_anime();//0.35ms

						auto mat = (GetFrameWorldMat(CharaFrame::RightWrist).GetRot()*GetCharaDir().Inverse());
						zVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(-1, -1, 0).Norm(), mat);

						//yVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, -1).Norm(), mat);
						yVec1 = zVec1.cross((MATRIX_ref::RotZ(this->m_LeanRad)*this->m_UpperMatrix.Inverse()).yvec().cross(zVec1)).Norm();
						Pos1 = GetFrameWorldMat(CharaFrame::RightHandJoint).pos();

						if (change) {
							GetAnime(GetAimAnimSel()).per = 0.f;
							GetAnime((CharaAnimeID)i).per = 1.f;
						}
						switch (this->m_Gun_Ptr->GetShotType()) {
						case SHOTTYPE::FULL:
						case SHOTTYPE::SEMI:
							GetAnime(CharaAnimeID::RightHand).per = (this->m_ShotPhase <= 2) ? ((!this->m_RunReady || (m_UpperAnimSelect != CharaAnimeID::Upper_Ready)) ? 1.f : 0.f) : 0.f;
							break;
						case SHOTTYPE::BOLT:
							GetAnime(CharaAnimeID::RightHand).per = (this->m_ShotPhase < 2) ? ((!this->m_RunReady || (m_UpperAnimSelect != CharaAnimeID::Upper_Ready)) ? 1.f : 0.f) : 0.f;
							break;
						default:
							break;
						}
						//必要最低限のアニメ再更新
						GetObj().work_anime((int)GetAimAnimSel());
						GetObj().work_anime(i);
						GetObj().work_anime((int)CharaAnimeID::RightHand);
					}
					//背負い場所探索
					VECTOR_ref yVec2, zVec2, Pos2;
					{
						yVec2 = (MATRIX_ref::RotZ(deg2rad(30)) * GetFrameWorldMat(CharaFrame::Upper2).GetRot() * GetCharaDir().Inverse()).xvec();
						zVec2 = (MATRIX_ref::RotZ(deg2rad(30)) * GetFrameWorldMat(CharaFrame::Upper2).GetRot() * GetCharaDir().Inverse()).yvec();
						Pos2 =
							GetFrameWorldMat(CharaFrame::Upper2).pos() +
							GetFrameWorldMat(CharaFrame::Upper2).GetRot().yvec() * -6.25f +
							GetFrameWorldMat(CharaFrame::Upper2).GetRot().zvec() * 1.75f;
					}
					auto yVec = Lerp(yVec1, yVec2, this->m_SlingPer);
					auto zVec = Lerp(zVec1, zVec2, this->m_SlingPer);
					auto PosBuf = Lerp(Pos1, Pos2, this->m_SlingPer);
					auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::front()*-1.f, zVec);
					tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec);
					tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosBuf);
					this->m_Gun_Ptr->SetGunMatrix(tmp_gunrat, this->m_ShotPhase);
					//弾を手持ち//スコープ内0.01ms
					{
						auto Thumb = GetFrameWorldMat(CharaFrame::RightThumb);
						auto Pointer = GetFrameWorldMat(CharaFrame::RightPointer);
						MATRIX_ref mat =
							MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, -1), MATRIX_ref::Vtrans(VECTOR_ref::vget(-1, -1, 0), Pointer.GetRot()))
							* MATRIX_ref::Mtrans((Thumb.pos() + Pointer.pos()) / 2.f);
						this->m_Gun_Ptr->SetAmmoHandMatrix(mat, this->m_LoadAmmoPer);
					}
					//左手座標指定
					move_LeftArm();
				}
			}
			//顔																	//0.01ms
			void			ExecuteShape(void) noexcept {
				SetShapePer(CharaShape::None, 0.f);
				SetShapePer(CharaShape::O, (0.75f + sin(this->m_HeartRateRad * 3)*0.25f)*(1.f - this->m_Stamina / StaminaMax));
				if (this->m_EyeclosePer <= 0.05f && (GetRand(100) == 0)) {
					this->m_Eyeclose = 1;
				}
				if (this->m_EyeclosePer >= 0.95f) {
					this->m_Eyeclose = 0;
				}
				Easing(&this->m_EyeclosePer, (float)this->m_Eyeclose, 0.5f, EasingType::OutExpo);
				SetShapePer(CharaShape::EYECLOSE, this->m_EyeclosePer);
			}
			//心拍数																//0.00ms
			void			ExecuteHeartRate(void) noexcept {
				auto SE = SoundPool::Instance();
				auto addRun = (this->m_RunPer2 - this->m_PrevRunPer2);
				if (addRun > 0.f) {
					this->m_HeartRate_r += (10.f + GetRandf(10.f)) / FPS;
				}
				else if (addRun < 0.f) {
					this->m_HeartRate_r -= (5.f + GetRandf(5.f)) / FPS;
				}
				this->m_HeartRate_r += (this->m_Speed * this->m_RunPer2 / SpeedLimit * 1.95f) / FPS;
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
						SE->Get((int)SoundEnum::Heart).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper2).pos(), Scale_Rate * 1.f);
					}
				}
				else {
					this->m_HeartSoundFlag = false;
				}


				this->m_Stamina += std::clamp((100.f - this->m_HeartRate) / 40.f, -2.5f, 2.5f) / FPS;

				if (this->m_InputGround.GetIsSquat()) {
					this->m_Stamina += 1.0f / FPS;
				}

				if (this->m_InputGround.GetSprint()) {
					this->m_Stamina += -0.75f / FPS;
				}
				else {
					this->m_Stamina += 1.0f / FPS;
				}

				this->m_Stamina = std::clamp(this->m_Stamina, 0.f, StaminaMax);

				if (this->m_Stamina <= 0.f) {
					this->m_CannotSprint = true;
				}
				if (this->m_CannotSprint) {
					if (this->m_Stamina > StaminaMax * 0.3f) {
						this->m_CannotSprint = false;
					}
				}

				if (this->m_Stamina <= StaminaMax * 0.3f) {
					if (!this->m_Breath.check()) {
						this->m_Breath.play_3D(GetFrameWorldMat(CharaFrame::Upper2).pos(), Scale_Rate * 5.f);
					}
				}
				if (this->m_Breath.check()) {
					this->m_Breath.SetPosition(GetFrameWorldMat(CharaFrame::Upper2).pos());
				}
			}
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				this->m_IsDraw = true;
			}
			~CharacterClass(void) noexcept {}
		public: //継承
			//
			void move_LeftArm(void) noexcept {
				auto matBase = GetFrameWorldMat(CharaFrame::Upper2).GetRot().Inverse();
				//matBase = GetCharaDir().Inverse();
				ResetFrameLocalMat(CharaFrame::LeftArm);
				ResetFrameLocalMat(CharaFrame::LeftArm2);
				ResetFrameLocalMat(CharaFrame::LeftWrist);
				matBase = GetParentFrameWorldMat(CharaFrame::LeftArm).GetRot().Inverse();

				VECTOR_ref GunPos = this->m_Gun_Ptr->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
				VECTOR_ref Gunyvec = this->m_Gun_Ptr->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - GunPos;
				VECTOR_ref Gunzvec = this->m_Gun_Ptr->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - GunPos;
				VECTOR_ref Gunxvec = Gunzvec.cross(Gunyvec);

				moves HandAim;// = this->LEFTHAND.move;
				HandAim.pos = Lerp(GunPos, GetFrameWorldMat(CharaFrame::LeftWrist).pos(), this->m_LeftHandPer);
				//基準
				auto vec_a1 = MATRIX_ref::Vtrans((HandAim.pos - GetFrameWorldMat(CharaFrame::LeftArm).pos()).Norm(), matBase);//基準
				auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(0.f, -1.f, vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
				//vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(0.f, -1.f, 0.f)).Norm();//x=0とする
				float cos_t = GetCosFormula((GetFrameWorldMat(CharaFrame::LeftWrist).pos() - GetFrameWorldMat(CharaFrame::LeftArm2).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm2).pos() - GetFrameWorldMat(CharaFrame::LeftArm).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm).pos() - HandAim.pos).size());
				auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
				//上腕
				SetFrameLocalMat(CharaFrame::LeftArm, MGetIdent());
				MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::LeftArm2).pos() - GetFrameWorldMat(CharaFrame::LeftArm).pos(), matBase), vec_t);
				SetFrameLocalMat(CharaFrame::LeftArm, a1_inv);
				//下腕
				matBase = GetParentFrameWorldMat(CharaFrame::LeftArm2).GetRot().Inverse();
				SetFrameLocalMat(CharaFrame::LeftArm2, MGetIdent());
				MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::LeftWrist).pos() - GetFrameWorldMat(CharaFrame::LeftArm2).pos(), matBase),
					MATRIX_ref::Vtrans(HandAim.pos - GetFrameWorldMat(CharaFrame::LeftArm2).pos(), matBase));
				SetFrameLocalMat(CharaFrame::LeftArm2, a2_inv);
				//手
				matBase = GetParentFrameWorldMat(CharaFrame::LeftWrist).GetRot().Inverse();
				MATRIX_ref mat1;
				MATRIX_ref mat2 = GetFrameLocalMat(CharaFrame::LeftWrist);
				{
					auto Lmat = GetFrameLocalMat(CharaFrame::LeftWrist);
					auto Wmat = GetFrameWorldMat(CharaFrame::LeftWrist);
					auto xvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, 0.f, -1.f), Wmat.GetRot());
					auto zvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(1.f, -1.f, 0.f), Wmat.GetRot());

					mat1 = MATRIX_ref::RotVec2(
						MATRIX_ref::Vtrans(zvec, matBase),
						MATRIX_ref::Vtrans(Gunzvec, matBase)) * mat1;

					SetFrameLocalMat(CharaFrame::LeftWrist, mat1);
					mat1 = MATRIX_ref::RotVec2(
						MATRIX_ref::Vtrans(xvec, matBase),
						MATRIX_ref::Vtrans(Gunxvec, matBase)) * mat1;

					mat1 = Lmat * mat1;

					mat1 = mat1 * MATRIX_ref::RotAxis(
						MATRIX_ref::Vtrans(VECTOR_ref::vget(1.f, -1.f, 0.f), mat1.GetRot())
						,
						deg2rad(180));

				}
				HandAim.mat = Lerp_Matrix(mat1, mat2, this->m_LeftHandPer);
				SetFrameLocalMat(CharaFrame::LeftWrist, HandAim.mat);
			}
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				GetAnime(CharaAnimeID::Bottom_Stand).per = 1.f;
				GetAnime(CharaAnimeID::Upper_RunningStart).GoEnd();
				//
				SetCreate3DSoundFlag(TRUE);
				this->m_Breath = SoundHandle::Load("data/Sound/SE/voice/WinningTicket/breath.wav");
				SetCreate3DSoundFlag(FALSE);
				this->m_Breath.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Breath.get());

				m_CharaAnimeSet[0].m_Aim = CharaAnimeID::Upper_Aim;
				m_CharaAnimeSet[0].m_Shot = CharaAnimeID::Upper_Shot;
				m_CharaAnimeSet[0].m_Cocking = CharaAnimeID::Upper_Cocking;
				m_CharaAnimeSet[0].m_Reload = CharaAnimeID::Upper_ReloadStart;


				m_CharaAnimeSet[1].m_Aim = CharaAnimeID::Upper_Aim2;
				m_CharaAnimeSet[1].m_Shot = CharaAnimeID::Upper_Shot2;
				m_CharaAnimeSet[1].m_Cocking = CharaAnimeID::Upper_Cocking2;
				m_CharaAnimeSet[1].m_Reload = CharaAnimeID::Upper_Reload2Start;


				m_CharaAnimeSet[2].m_Aim = CharaAnimeID::Upper_Aim3;
				m_CharaAnimeSet[2].m_Shot = CharaAnimeID::Upper_Shot3;
				m_CharaAnimeSet[2].m_Cocking = CharaAnimeID::Upper_Cocking3;
				m_CharaAnimeSet[2].m_Reload = CharaAnimeID::Upper_Reload3Start;
			}
			void			FirstExecute(void) noexcept override {
				//初回のみ更新する内容
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < GetObj().get_anime().size(); i++) { GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i); }
					GetAnime(CharaAnimeID::RightHand).per = 1.f;
					GetObj().work_anime();
					this->m_RightWristRot = GetFrameLocalMat(CharaFrame::RightWrist).GetRot();
				}
				ExecuteSavePrev();			//以前の状態保持
				ExecuteInput();				//操作//0.01ms
				ExecuteUpperMatrix();		//上半身回転//0.06ms
				ExecuteAnim();				//AnimUpdte//0.03ms
				ExecuteMatrix();			//SetMat指示//0.03ms
				ExecuteShape();				//顔//スコープ内0.01ms
				ExecuteHeartRate();			//心拍数//0.00ms
			}
			//void			Draw(void) noexcept override { ObjectBaseClass::Draw(); }
		};
	};
};
