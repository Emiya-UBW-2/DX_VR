#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class BulletType {
			FireBall,
			Thunder,
		};
		class BulletClass {
		private://キャラパラメーター
			bool m_IsActive{ false };
			moves m_move;
			float m_speed{ 0.f };
			float m_yAdd{ 0.f };
			float m_Timer{ 0.f };
			float m_HitTimer{ 0.f };
			moves m_move_Hit;
			BulletType m_Type{ BulletType::FireBall };
		public:
			void Set(const MATRIX_ref& mat, const VECTOR_ref& pos, BulletType pType,float pBaseSpeed) noexcept {
				this->m_IsActive = true;
				this->m_move.mat = mat;
				this->m_move.pos = pos;
				this->m_move.repos = this->m_move.pos;
				this->m_yAdd = 0.f;
				this->m_Timer = 0.f;
				this->m_Type = pType;
				switch (this->m_Type) {
				case BulletType::FireBall:
					this->m_speed = (pBaseSpeed + 200.f) * 1000.f / 3600.f * Scale_Rate;
					this->m_HitTimer = 10.f;
					break;
				case BulletType::Thunder:
					this->m_speed = (pBaseSpeed + 800.f) * 1000.f / 3600.f * Scale_Rate;
					this->m_HitTimer = 3.f;
					break;
				default:
					break;
				}
			}
			void Execute(void) noexcept {
				if (this->m_IsActive) {
					this->m_move.repos = this->m_move.pos;
					this->m_move.vec = this->m_move.mat.zvec() * (-1.f * this->m_speed / FPS) + VECTOR_ref::up()*this->m_yAdd;
					this->m_move.pos += this->m_move.vec;
					//this->m_yAdd += (M_GR / (FPS*FPS));
				}
				if (this->m_Timer > this->m_HitTimer) {
					this->m_IsActive = false;
				}
				this->m_Timer += 1.f / FPS;
			}
			bool CheckBullet(const MV1* pCol) noexcept {
				if (this->m_IsActive) {
					auto HitResult = pCol->CollCheck_Line(this->m_move.repos, this->m_move.pos);
					if (HitResult.HitFlag == TRUE) {
						this->m_move_Hit.pos = HitResult.HitPosition;
						this->m_move_Hit.vec = HitResult.Normal;
						return true;
					}
				}
				return false;
			}
			void SetHit() noexcept {
				if (this->m_IsActive) {
					this->m_HitTimer = this->m_Timer + 0.5f;
					this->m_IsActive = false;
				}
			}
		public:
			auto* Get_Move(void) noexcept { return (this->m_IsActive) ? &this->m_move : (moves*)nullptr; }
			const auto& GetMoveHit(void) noexcept { return this->m_move_Hit; }
			const auto& GetType(void) noexcept { return this->m_Type; }
		};

		class CharacterClass : public ObjectBaseClass, public Effect_UseControl {
		private://キャラパラメーター
			const float SpeedLimit{ 2.f };
			const float UpperTimerLimit = 3.f;

			const float FlightSpeedMin = 20.f;
			const float FlightSpeedMax = 200.f;

			const float FlightHightLimit = 5000.f;

			const float HPMax = 100.f;
			const float MPMax = 100.f;
			std::string											m_Name;
			CharaTypeID											m_CharaType;
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
			MATRIX_ref											m_UpperMatrix;
			float												m_Speed{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			float												m_NeckPer{ 0.f };
			bool												m_TurnBody{ false };
			bool												m_ReturnStand{ false };
			bool												m_RunReady{ false };
			bool												m_RunReadyFirst{ false };
			bool												m_Running{ false };
			CharaAnimeID										m_UpperAnimSelect, m_PrevUpperAnimSel;
			CharaAnimeID										m_BottomAnimSelect;
			//
			std::array<BulletClass, 6>							m_Bullet;
			int													m_NowBullet{ 0 };
			//
			float												m_HP{ 0.f };							//スコア
			float												m_Score{ 0.f };							//スコア
			//表情
			int													m_Eyeclose{ 0 };
			float												m_EyeclosePer{ 0.f };
			//サウンド
			int													m_CharaSound{ -1 };
			//
			//std::shared_ptr<CharacterClass>						m_LockOn{ nullptr };
			//
			bool												m_SendCamShake{ false };
		public://ゲッター
			void			SetAnimOnce(CharaAnimeID ID, float speed) noexcept { ObjectBaseClass::SetAnimOnce((int)ID, speed); }
			void			SetAnimLoop(CharaAnimeID ID, float speed) noexcept { ObjectBaseClass::SetAnimLoop((int)ID, speed); }
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { this->GetObj().frame_Reset(m_Frames[(int)frame].first); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { this->GetObj().SetFrameLocalMatrix(m_Frames[(int)frame].first, value * m_Frames[(int)frame].second); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { m_Shapes[(int)pShape].second = Per; }
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			AddScore(float value) noexcept { this->m_Score += value; }
			void			SubScore(float value) noexcept { this->m_Score -= value; }
			void			SetScore(float value) noexcept { this->m_Score = value; }
			void			AddHP(float value) noexcept { this->m_HP = std::clamp(this->m_HP + value, 0.f, HPMax); }
			void			SubHP(float value) noexcept { this->m_HP = std::clamp(this->m_HP - value, 0.f, HPMax); }
			void			SetHP(float value) noexcept { this->m_HP = value; }
			void			SetPosBufOverRide(const VECTOR_ref& pPos, const VECTOR_ref& pVec) {
				this->m_PosBufOverRideFlag = true;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide = pVec;
			}

			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->GetObj().get_anime((int)anim); }
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix(m_Frames[(int)frame].first); }
			const auto		GetParentFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix((int)this->GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix(m_Frames[(int)frame].first); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix((int)this->GetObj_const().frame_parent(m_Frames[(int)frame].first)); }
			const auto		GetRadBuf(void) const noexcept { return this->m_InputGround.GetRadBuf(); }
			const auto		GetIsRun(void) const noexcept { return this->m_InputGround.GetRun(); }
			const auto		GetTurnRatePer(void) const noexcept { return this->m_InputGround.GetTurnRatePer(); }
			const auto		GetCharaDir(void) const noexcept { return this->m_UpperMatrix * this->m_move.mat; }
			const auto		GetEyeVecMat(void) const noexcept { return GetCharaDir(); }
			const auto		GetEyePosition(void) const noexcept { return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + this->GetEyeVecMat().zvec() * -1.5f; }
			const auto&		GetPosBuf(void) const noexcept { return this->m_PosBuf; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetScore(void) const noexcept { return this->m_Score; }
			const auto&		GetHP(void) const noexcept { return this->m_HP; }
			const auto&		GetHPMax(void) const noexcept { return HPMax; }
			const auto&		GetMPMax(void) const noexcept { return MPMax; }
			const auto&		GetSendCamShake(void) const noexcept { return this->m_SendCamShake; }
			const auto&		GetName(void) const noexcept { return this->m_Name; }
			//
			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos) noexcept {
				this->m_KeyActive = false;
				//
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					GetAnimeBuf((CharaAnimeID)i) = 0.f;
				}
				this->m_Speed = 0.f;
				this->m_RunPer2 = 0.f;
				this->m_PrevRunPer2 = 0.f;
				this->m_NeckPer = 0.f;
				this->m_TurnBody = false;
				this->m_ReturnStand = false;
				this->m_RunReady = false;
				this->m_RunReadyFirst = false;
				this->m_Running = false;
				this->m_ReadySwitch = false;

				//this->m_UpperAnimSelect;
				//this->m_PrevUpperAnimSel;
				//this->m_BottomAnimSelect;
				this->m_HP = HPMax;
				this->m_Score = 0.f;							//スコア
				//表情
				this->m_Eyeclose = 0;
				this->m_EyeclosePer = 0.f;
				//サウンド
				this->m_CharaSound = -1;
				//動作にかかわる操作
				this->m_InputGround.ValueSet(pxRad, pyRad);
				this->m_PosBuf = pPos;
				this->m_PosBufOverRideFlag = false;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide.clear();
				//上記を反映するもの
				this->m_yrad_Upper = this->m_InputGround.GetRad().y();
				this->m_yrad_Bottom = this->m_InputGround.GetRad().y();
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) * MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				this->m_move.vec.clear();
				SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), this->m_PosBuf);
			}
			void			ExecuteRadBuf(const InputControl& pInput) { m_InputGround.ExecuteRadBuf(pInput.GetAddxRad(), pInput.GetAddyRad(), VECTOR_ref::zero()); }
			void			SetInput(const InputControl& pInput, bool pReady) {
				this->m_ReadySwitch = (this->m_KeyActive != pReady);
				this->m_KeyActive = pReady;

				//地
				m_InputGround.SetInput(
					pInput.GetxRad(), pInput.GetyRad(),
					pInput.GetGoFrontPress(),
					pInput.GetGoBackPress(),
					pInput.GetGoLeftPress(),
					pInput.GetGoRightPress(),
					pInput.GetRunPress(),
					false,
					pInput.GetQPress(),
					pInput.GetEPress()
				);
				//
				m_Rightkey.GetInput(pInput.GetRightPress());
				m_Leftkey.GetInput(pInput.GetLeftPress());
				m_Upkey.GetInput(pInput.GetUpPress());
				m_Downkey.GetInput(pInput.GetDownPress());
				//
				{
					auto TmpReady = GetIsRun();
					this->m_RunReadyFirst = (TmpReady && !this->m_RunReady);
					this->m_RunReady = TmpReady;
				}
			}
			void			SetEyeVec(const VECTOR_ref& camvec) noexcept {
				this->GetObj().frame_Reset(this->m_Frames[(int)CharaFrame::Head].first);
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
				//this->m_yrad_Upper、this->m_yrad_Bottom、this->m_rad.z()決定
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
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) * MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				this->GetObj().frame_Reset(this->m_Frames[(int)CharaFrame::Upper].first);
				SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * this->m_UpperMatrix);
			}
			//SetMat指示															//0.03ms
			auto*		GetLatestBulletMove(void) noexcept {
				auto Now = this->m_NowBullet - 1;
				if (Now < 0) { Now = (int)(this->m_Bullet.size()) - 1; }
				return this->m_Bullet[Now].Get_Move();
			}
			void			ExecuteAnim(void) noexcept {
				auto SE = SoundPool::Instance();
				//アニメ演算
				{
					//上半身
					{
						this->m_SendCamShake = false;
						this->m_UpperAnimSelect = CharaAnimeID::Upper_Down;
						{
							//auto* nowptr = GetLatestBulletMove();

							for (auto& b : this->m_Bullet) {
								if (b.CheckBullet(this->m_MapCol)) {
									auto mat = b.GetMoveHit();
									switch (b.GetType()) {
									case BulletType::FireBall:
										break;
									default:
										break;
									}
									b.SetHit();
								}
							}
						}

						if (this->m_ReadySwitch) {
							this->m_RunReadyFirst = false;
							GetAnime(CharaAnimeID::Upper_RunningStart).GoEnd();
						}
						if (!this->m_RunReady && !this->m_Running && !GetAnime(CharaAnimeID::Upper_RunningStart).TimeEnd()) {
							this->m_RunReady = true;
						}
						if (this->m_RunReadyFirst) {
							GetAnime(CharaAnimeID::Upper_RunningStart).GoStart();
							GetAnime(CharaAnimeID::Upper_RunningStart).per = 1.f;
						}
						if (!this->m_RunReady) {
							this->m_Running = false;
						}
						else if (!this->m_Running) {
							SetAnimOnce(CharaAnimeID::Upper_RunningStart, 2.f);
							this->m_InputGround.SetSprintPer(0.f);
							if (GetAnime(CharaAnimeID::Upper_RunningStart).TimeEnd()) {
								this->m_Running = true;
								GetAnime(CharaAnimeID::Upper_Running).GoStart();
							}
							m_UpperAnimSelect = CharaAnimeID::Upper_RunningStart;
						}
						else {
							m_UpperAnimSelect = (!this->m_InputGround.GetSprint()) ? CharaAnimeID::Upper_Running : CharaAnimeID::Upper_Sprint;
						}
						//首
						Easing(&m_NeckPer, (
							m_UpperAnimSelect == CharaAnimeID::Upper_Down
							|| m_UpperAnimSelect == CharaAnimeID::Upper_Running
							) ? 0.f : 1.f, 0.9f, EasingType::OutExpo);
					}
					//下半身
					{
						if (this->m_InputGround.GetPressFront()) {
							this->m_BottomAnimSelect = (this->m_InputGround.GetRun()) ? CharaAnimeID::Bottom_Run : CharaAnimeID::Bottom_Walk;
						}
						else {
							this->m_BottomAnimSelect = CharaAnimeID::Bottom_Stand;
							if (this->m_InputGround.GetPressLeft()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_LeftStep; }
							if (this->m_InputGround.GetPressRight()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_RightStep; }
							if (this->m_InputGround.GetPressRear()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_WalkBack; }
							if (this->m_InputGround.GetSprint()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_Run; }
						}
					}
					//
					SetAnimLoop(CharaAnimeID::Upper_Sprint, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Upper_Running, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Turn, 0.5f);
					SetAnimLoop(CharaAnimeID::Bottom_Run, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Walk, 1.15f * this->m_InputGround.GetVecFront());
					SetAnimLoop(CharaAnimeID::Bottom_LeftStep, 1.15f * this->m_InputGround.GetVecLeft());
					SetAnimLoop(CharaAnimeID::Bottom_WalkBack, 1.15f * this->m_InputGround.GetVecRear());
					SetAnimLoop(CharaAnimeID::Bottom_RightStep, 1.15f * this->m_InputGround.GetVecRight());
				}
				//アニメセレクト
				{
					//上半身
					//下半身
					Easing(&GetAnimeBuf(CharaAnimeID::Bottom_Turn), (this->m_TurnBody) ? abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
					//その他
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//上半身
						if (
							i == (int)CharaAnimeID::Upper_Down ||
							i == (int)CharaAnimeID::Upper_RunningStart ||
							i == (int)CharaAnimeID::Upper_Running ||
							i == (int)CharaAnimeID::Upper_Sprint ||
							i == (int)CharaAnimeID::Upper_UseItem ||
							i == (int)CharaAnimeID::Upper_SetMagic ||
							i == (int)CharaAnimeID::Upper_UseMagic1
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
						GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i);
					}
					//
				}
				//足音
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
				//
				this->GetObj().work_anime();
			}
			//SetMat指示更新														//0.03ms
			void			ExecuteMatrix(void) noexcept {
				this->m_RunPer2 = Lerp(0.35f, (SpeedLimit * (1.f + 0.5f * this->m_InputGround.GetSprintPer())), this->m_InputGround.GetRunPer());
				if (this->m_PrevRunPer2 == 0.f) {
					this->m_PrevRunPer2 = this->m_RunPer2;
				}

				auto OLDpos = this->m_PosBuf;
				VECTOR_ref vecBuf = MATRIX_ref::Vtrans(this->m_InputGround.GetVec(), MATRIX_ref::RotY(this->m_yrad_Upper));
				this->m_Speed = std::clamp(vecBuf.size(), 0.f, 1.f);

				if (this->m_Speed > 0.1f) {
					this->m_move.vec.x((vecBuf.Norm() * this->m_RunPer2 * Frame_Rate / FPS).x());
					this->m_move.vec.z((vecBuf.Norm() * this->m_RunPer2 * Frame_Rate / FPS).z());
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

				if (this->m_PosBufOverRideFlag) {
					this->m_PosBufOverRideFlag = false;
					this->m_PosBuf = this->m_PosBufOverRide;
					this->m_move.vec = this->m_VecBufOverRide;
				}

				//
				auto OLDbuf = this->m_PosBuf;
				if (col_wall(OLDpos, &this->m_PosBuf, *this->m_MapCol)) {}
				auto mat_tmp = MATRIX_ref::RotZ(this->m_InputGround.GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom);
				Easing_Matrix(&this->m_move.mat, mat_tmp, 0.8f, EasingType::OutExpo);
				Easing(&this->m_move.pos, this->m_PosBuf, 0.9f, EasingType::OutExpo);
				UpdateMove();
				//
			}
			//顔																	//0.01ms
			void			ExecuteShape(void) noexcept {
				SetShapePer(CharaShape::None, 0.f);
				if (this->m_EyeclosePer <= 0.05f && (GetRand(100) == 0)) {
					this->m_Eyeclose = 1;
				}
				if (this->m_EyeclosePer >= 0.95f) {
					this->m_Eyeclose = 0;
				}
				Easing(&this->m_EyeclosePer, (float)this->m_Eyeclose, 0.5f, EasingType::OutExpo);
				SetShapePer(CharaShape::EYECLOSE, this->m_EyeclosePer);
			}
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				ValueSet(deg2rad(0.f), deg2rad(0.f), VECTOR_ref::vget(0.f, 0.f, 0.f));
			}
			~CharacterClass(void) noexcept {}
		public: //継承
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				GetAnime(CharaAnimeID::Bottom_Stand).per = 1.f;
				GetAnime(CharaAnimeID::Upper_RunningStart).GoEnd();
				this->m_Name = "Name";
				this->m_CharaType = CharaTypeID::Enemy;
			}
			void			FirstExecute(void) noexcept override {
				//初回のみ更新する内容
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < this->GetObj().get_anime().size(); i++) { GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i); }
					this->GetObj().work_anime();
				}
				ExecuteSavePrev();			//以前の状態保持
				ExecuteInput();				//操作//0.01ms
				ExecuteUpperMatrix();		//上半身回転//0.06ms
				ExecuteAnim();				//AnimUpdte//0.03ms
				ExecuteMatrix();			//SetMat指示//0.03ms
				ExecuteShape();				//顔//スコープ内0.01ms
				for (auto& b : this->m_Bullet) {
					b.Execute();
				}
				Effect_UseControl::Execute_Effect();
			}
			//void			Draw(void) noexcept override { ObjectBaseClass::Draw(); }
			void			Dispose(void) noexcept override {
				ObjectBaseClass::Dispose();
				Effect_UseControl::Dispose_Effect();
			}
		};
	};
};
