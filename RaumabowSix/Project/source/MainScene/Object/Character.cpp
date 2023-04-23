#pragma once
#include	"../../Header.hpp"


#include <math.h>

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			CharacterClass::move_RightArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept {
			ResetFrameLocalMat(CharaFrame::RightArm);
			ResetFrameLocalMat(CharaFrame::RightArm2);
			ResetFrameLocalMat(CharaFrame::RightWrist);
			auto matBase = GetParentFrameWorldMat(CharaFrame::RightArm).GetRot().Inverse();

			VECTOR_ref Gunxvec = Gunzvec.cross(Gunyvec*-1.f)*-1.f;

			VECTOR_ref Localyvec = VECTOR_ref::vget(0.f, 0.f, -1.f).Norm();
			VECTOR_ref Localzvec = VECTOR_ref::vget(-0.3f, -1.f, 0.f).Norm();

			VECTOR_ref RetPos = Lerp(GunPos, GetFrameWorldMat(CharaFrame::RightWrist).pos(), m_LeftHandPer);
			//基準
			auto vec_a1 = MATRIX_ref::Vtrans((RetPos - GetFrameWorldMat(CharaFrame::RightArm).pos()).Norm(), matBase);//基準
			auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(-1.5f, -1.f, vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
			float cos_t = GetCosFormula((GetFrameWorldMat(CharaFrame::RightWrist).pos() - GetFrameWorldMat(CharaFrame::RightArm2).pos()).size(), (GetFrameWorldMat(CharaFrame::RightArm2).pos() - GetFrameWorldMat(CharaFrame::RightArm).pos()).size(), (GetFrameWorldMat(CharaFrame::RightArm).pos() - RetPos).size());
			auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//上腕
			SetFrameLocalMat(CharaFrame::RightArm, MGetIdent());
			MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::RightArm2).pos() - GetFrameWorldMat(CharaFrame::RightArm).pos(), matBase), vec_t);
			SetFrameLocalMat(CharaFrame::RightArm, a1_inv);
			//下腕
			matBase = GetParentFrameWorldMat(CharaFrame::RightArm2).GetRot().Inverse();
			SetFrameLocalMat(CharaFrame::RightArm2, MGetIdent());
			MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
				MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::RightWrist).pos() - GetFrameWorldMat(CharaFrame::RightArm2).pos(), matBase),
				MATRIX_ref::Vtrans(RetPos - GetFrameWorldMat(CharaFrame::RightArm2).pos(), matBase));
			SetFrameLocalMat(CharaFrame::RightArm2, a2_inv);
			//手
			auto AngleOf2Vector = [&](VECTOR_ref A, VECTOR_ref B) { return acos(A.dot(B) / (A.Length() * B.Length())); };			//２つのベクトルABのなす角度θを求める
			matBase = GetParentFrameWorldMat(CharaFrame::RightWrist).GetRot().Inverse();
			MATRIX_ref mat1;
			MATRIX_ref mat2 = GetFrameLocalMat(CharaFrame::RightWrist);
			{
				auto zvec = MATRIX_ref::Vtrans(Localzvec, GetFrameWorldMat(CharaFrame::RightWrist).GetRot());
				mat1 = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(zvec, matBase), MATRIX_ref::Vtrans(Gunzvec, matBase)) * mat1;
				SetFrameLocalMat(CharaFrame::RightWrist, mat1);
				auto xvec = MATRIX_ref::Vtrans(Localyvec, GetFrameWorldMat(CharaFrame::RightWrist).GetRot());
				mat1 = MATRIX_ref::RotAxis(Localzvec, AngleOf2Vector(xvec, Gunxvec)*(((Gunyvec*-1.f).dot(xvec) > 0.f) ? -1.f : 1.f))*mat1;
			}
			MATRIX_ref RetMat = Lerp_Matrix(mat1, mat2, m_LeftHandPer);
			SetFrameLocalMat(CharaFrame::RightWrist, RetMat);
		}
		void			CharacterClass::move_LeftArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept {
			ResetFrameLocalMat(CharaFrame::LeftArm);
			ResetFrameLocalMat(CharaFrame::LeftArm2);
			ResetFrameLocalMat(CharaFrame::LeftWrist);
			auto matBase = GetParentFrameWorldMat(CharaFrame::LeftArm).GetRot().Inverse();

			VECTOR_ref Gunxvec = Gunzvec.cross(Gunyvec)*-1.f;

			VECTOR_ref Localyvec = VECTOR_ref::vget(0.f, 0.f, -1.f).Norm();
			VECTOR_ref Localzvec = VECTOR_ref::vget(0.3f, -1.f, 0.f).Norm();

			VECTOR_ref RetPos = Lerp(GunPos, GetFrameWorldMat(CharaFrame::LeftWrist).pos(), m_LeftHandPer);
			//基準
			auto vec_a1 = MATRIX_ref::Vtrans((RetPos - GetFrameWorldMat(CharaFrame::LeftArm).pos()).Norm(), matBase);//基準
			auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(1.5f, -1.f, vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
			float cos_t = GetCosFormula((GetFrameWorldMat(CharaFrame::LeftWrist).pos() - GetFrameWorldMat(CharaFrame::LeftArm2).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm2).pos() - GetFrameWorldMat(CharaFrame::LeftArm).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm).pos() - RetPos).size());
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
				MATRIX_ref::Vtrans(RetPos - GetFrameWorldMat(CharaFrame::LeftArm2).pos(), matBase));
			SetFrameLocalMat(CharaFrame::LeftArm2, a2_inv);
			//手
			auto AngleOf2Vector = [&](VECTOR_ref A, VECTOR_ref B) { return acos(A.dot(B) / (A.Length() * B.Length())); };			//２つのベクトルABのなす角度θを求める
			matBase = GetParentFrameWorldMat(CharaFrame::LeftWrist).GetRot().Inverse();
			MATRIX_ref mat1;
			MATRIX_ref mat2 = GetFrameLocalMat(CharaFrame::LeftWrist);
			{
				auto zvec = MATRIX_ref::Vtrans(Localzvec, GetFrameWorldMat(CharaFrame::LeftWrist).GetRot());
				mat1 = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(zvec, matBase), MATRIX_ref::Vtrans(Gunzvec, matBase)) * mat1;
				SetFrameLocalMat(CharaFrame::LeftWrist, mat1);
				auto xvec = MATRIX_ref::Vtrans(Localyvec, GetFrameWorldMat(CharaFrame::LeftWrist).GetRot());
				mat1 = MATRIX_ref::RotAxis(Localzvec, AngleOf2Vector(xvec, Gunxvec)*((Gunyvec.dot(xvec) > 0.f) ? -1.f : 1.f))*mat1;
			}
			MATRIX_ref RetMat = Lerp_Matrix(mat1, mat2, m_LeftHandPer);
			SetFrameLocalMat(CharaFrame::LeftWrist, RetMat);
		}
		//
		void			CharacterClass::ValueSet(float pxRad, float pyRad, bool SquatOn, const VECTOR_ref& pPos, PlayerID pID) noexcept {
			StaminaControl::InitStamina();
			LifeControl::InitLife();
			KeyControl::InitKey(pxRad, pyRad, SquatOn);
			ShapeControl::InitShape();
			//
			this->m_MyID = pID;
			//
			for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
				GetAnimeBuf((CharaAnimeID)i) = 0.f;
			}
			this->m_ReadyPer = 0.f;
			this->m_ReadyTimer = UpperTimerLimit;

			this->m_MagHansPer = 0.f;

			this->m_Speed = 0.f;
			this->m_RunPer2 = 0.f;
			this->m_PrevRunPer2 = 0.f;
			this->m_NeckPer = 0.f;
			this->m_LeftHandPer = 0.f;
			this->m_TurnBody = false;
			this->m_ShotPhase = 0;
			this->m_NeckPosOffsetPer = 0.f;
			this->m_MoveEyePosTimer = 0.f;
			this->m_RunReady = false;
			this->m_Running = false;

			this->m_Press_Shot = false;
			this->m_Press_Reload = false;
			this->m_Press_Aim = false;
			//サウンド
			this->m_CharaSound = -1;
			//動作にかかわる操作
			this->m_PosBuf = pPos;
			this->m_PosBufOverRideFlag = false;
			this->m_PosBufOverRide = pPos;
			this->m_VecBufOverRide.clear();
			//上記を反映するもの
			this->m_yrad_Upper = KeyControl::GetRad().y();
			this->m_yrad_Bottom = KeyControl::GetRad().y();
			this->m_LeanRad = 0.f;
			this->m_UpperMatrix =
				MATRIX_ref::RotZ(this->m_LeanRad) *
				MATRIX_ref::RotX(KeyControl::GetRad().x()) *
				MATRIX_ref::RotY(KeyControl::GetRad().y() - this->m_yrad_Bottom);
			this->m_move.vec.clear();
			SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), m_PosBuf);

			m_SlingZrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(90));
			m_SlingYrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(30));

			this->m_DamageSwitch = 0;
			this->m_DamageSwitchRec = this->m_DamageSwitch;
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			InputKey(pInput,pReady, StaminaControl::GetHeartRandVec(KeyControl::GetSquatPer()), StaminaControl::GetCannotRun());
			//銃切替
			if (KeyControl::GetGunChange().trigger() && GetCanshot()) {
				m_SlingZrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(90));
				m_SlingYrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(30));
				++this->m_GunSelect %= 2;
			}
			//AIM
			if (GetGunPtrNow() != nullptr) {
				this->m_Press_Shot = pInput.GetAction5() && KeyControl::GetKeyActive();
				this->m_Press_Reload = (pInput.GetAction2() && KeyControl::GetKeyActive() && (GetAmmoNum() <= GetAmmoAll()));
				if ((GetAmmoNum() == 0) && !GetCanshot()) {
					if (this->m_Press_Shot) {
						this->m_Press_Shot = false;
						this->m_Press_Reload = true;
					}
				}
				if (!GetCanshot()) {
					switch (GetGunPtrNow()->GetShotType()) {
					case SHOTTYPE::FULL:
					case SHOTTYPE::BOLT:
						this->m_Press_Shot = true;
						break;
					case SHOTTYPE::SEMI:
						if (GetAmmoNum() != 0) {
							this->m_Press_Shot = true;
						}
						break;
					default:
						break;
					}
				}
			}
			else {
				this->m_Press_Shot = false;
				this->m_Press_Reload = false;
			}
			this->m_Press_Aim = pInput.GetAction6() && KeyControl::GetKeyActive();
			//
			this->m_RunReady = (KeyControl::GetIsRun() && (this->m_ShotPhase < 2));
		}
		void			CharacterClass::SetEyeVec(const VECTOR_ref& camvec) noexcept {
			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Head));
			auto v1 = (GetFrameWorldMat(CharaFrame::Head).GetRot() * GetCharaDir().Inverse()).zvec()*-1.f;
			auto v2 = Lerp(MATRIX_ref::Vtrans(camvec.Norm(), GetCharaDir().Inverse()), v1, m_NeckPer);

			auto radlimit = deg2rad(70);
			if (v1.dot(v2) <= cos(radlimit)) {
				v2 = v1 * cos(radlimit) + v1.cross(v1.cross(v2)) * (-sin(radlimit));
			}
			SetFrameLocalMat(CharaFrame::Head, MATRIX_ref::RotVec2(v1, v2) * GetFrameLocalMat(CharaFrame::Head).GetRot());
		}
		//以前の状態保持														//
		void			CharacterClass::ExecuteSavePrev(void) noexcept {
			this->m_PrevUpperAnimSel = this->m_UpperAnimSelect;
			this->m_PrevRunPer2 = this->m_RunPer2;
		}
		//操作																	//0.01ms
		void			CharacterClass::ExecuteInput(void) noexcept {
			//
			if (GetGunPtrNow() != nullptr) {
				for (auto& p : this->m_Gun_Ptr) {
					int index = (int)(&p - &this->m_Gun_Ptr.front());
					p->SetUseMoveParts(index == this->m_GunSelect);
				}

				GetGunPtrNow()->SetIsShot(false);
				if ((this->m_Press_Shot || this->m_Press_Reload) && (this->m_ShotPhase == 0)) {
					this->m_ReadyTimer = std::min(this->m_ReadyTimer, 0.1f);
					if (this->m_ReadyPer >= 0.9f) {
						this->m_ReadyPer = 1.f;
						if (this->m_Press_Shot) {
							if (GetGunPtrNow()->GetCanShot()) {
								this->m_ShotPhase = 1;
								GetGunPtrNow()->SetBullet();
								this->m_RecoilRadAdd.Set(GetRandf(0.0015f), -0.03f, 0.f);

								this->m_RecoilRadAdd = MATRIX_ref::Vtrans(this->m_RecoilRadAdd, MATRIX_ref::RotZ(-m_LeanRad/5.f));
							}
							else {
								if (!GetGunPtrNow()->GetIsMagEmpty()) {
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
					this->m_ReadyTimer = std::clamp(this->m_ReadyTimer + 1.f / FPS, 0.f, m_Press_Aim ? 0.f : UpperTimerLimit);
				}
				if ((this->m_ShotPhase >= 2) ) {
					this->m_ReadyTimer = UpperTimerLimit;
				}
				if (KeyControl::GetIsRun()) {
					this->m_ReadyTimer = UpperTimerLimit;
				}
			}
			else {
				this->m_ReadyTimer = std::clamp(this->m_ReadyTimer + 1.f / FPS, 0.f, UpperTimerLimit);
				if (this->m_Speed > 0.6f || KeyControl::GetIsRun()) {
					this->m_ReadyTimer = UpperTimerLimit;
				}
			}
			if (m_IsStuckGun) {
				this->m_ReadyTimer = UpperTimerLimit;
			}


			if (this->m_RecoilRadAdd.y() < 0.f) {
				Easing(&this->m_RecoilRadAdd, VECTOR_ref::vget(0.f, 0.03f, 0.f), 0.85f, EasingType::OutExpo);
			}
			else {
				Easing(&this->m_RecoilRadAdd, VECTOR_ref::zero(), 0.7f, EasingType::OutExpo);
			}
			//
			Easing(&this->m_ReadyPer, (this->m_ReadyTimer < UpperTimerLimit || !(m_ShotPhase <= 1)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			//this->m_yrad_Upper、this->m_yrad_Bottom決定
			if (this->m_Speed <= 0.1f) {
				if (abs(KeyControl::GetRad().y() - this->m_yrad_Upper) > deg2rad(50.f)) {
					this->m_TurnBody = true;
				}
				if (abs(KeyControl::GetRad().y() - this->m_yrad_Upper) < deg2rad(0.5f)) {
					this->m_TurnBody = false;
				}
			}
			else {
				this->m_TurnBody = false;
			}
			KeyControl::ExcuteKey();
			//
			auto FrontP = KeyControl::GetFrontP();
			auto TmpRunPer = Lerp(0.85f, 0.7f, KeyControl::GetRunPer());
			if (this->m_TurnBody || (this->m_Speed > 0.1f)) { Easing(&this->m_yrad_Upper, KeyControl::GetRad().y(), TmpRunPer, EasingType::OutExpo); }
			auto OLDP = this->m_yrad_Bottom;
			Easing(&this->m_yrad_Bottom, m_yrad_Upper - FrontP, TmpRunPer, EasingType::OutExpo);
			KeyControl::SetRadBufZ((this->m_yrad_Bottom - OLDP) * 2.f);
		}
		//上半身回転															//0.06ms
		void			CharacterClass::ExecuteUpperMatrix(void) noexcept {
			this->m_LeanRad = deg2rad(25) * KeyControl::GetLeanRatePer();
			this->m_UpperMatrix =
				MATRIX_ref::RotZ(this->m_LeanRad) *
				MATRIX_ref::RotX(KeyControl::GetRad().x()) *
				MATRIX_ref::RotY(KeyControl::GetRad().y() - this->m_yrad_Bottom);

			auto tmpUpperMatrix =
				MATRIX_ref::RotZ(this->m_LeanRad) *
				MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f) *
				MATRIX_ref::RotY(KeyControl::GetRad().y() - this->m_yrad_Bottom);

			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Upper));
			SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * tmpUpperMatrix);
			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Upper2));
			SetFrameLocalMat(CharaFrame::Upper2, GetFrameLocalMat(CharaFrame::Upper2).GetRot() * MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f));
		}
		//SetMat指示															//0.03ms
		void			CharacterClass::ExecuteAnim(void) noexcept {
			auto SE = SoundPool::Instance();
			//アニメ演算
			{
				//上半身
				{
					this->m_UpperAnimSelect = GetDownAnimSel();
					bool canreverse = true;
					if (!this->m_RunReady && !this->m_Running) {
						this->m_RunReady = true;
						canreverse = false;
					}
					this->m_Running = this->m_RunReady;
					if (this->m_ShotPhase == 1) {
						bool ischeck = true;
						switch (GetGunPtrNow()->GetShotType()) {
						case SHOTTYPE::FULL:
							break;
						case SHOTTYPE::SEMI:
						case SHOTTYPE::BOLT:
							ischeck = !this->m_Press_Shot;
							break;
						default:
							break;
						}

						if (GetGunPtrNow()->GetNowAnime().TimeEnd() && ischeck) {
							if (!GetGunPtrNow()->GetIsMagEmpty()) {
								switch (GetGunPtrNow()->GetShotType()) {
								case SHOTTYPE::FULL:
								case SHOTTYPE::SEMI:
									this->m_ShotPhase = 0;
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
							switch (GetGunPtrNow()->GetReloadType()) {
							case RELOADTYPE::MAG:
								this->m_ShotPhase++;
								break;
							case RELOADTYPE::AMMO:
								if (GetGunPtrNow()->GetIsMagFull()) {
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
							if (GetGunPtrNow()->GetCanShot()) {
								this->m_ShotPhase = 0;
							}
							else {
								if (!GetGunPtrNow()->GetIsMagEmpty()) {
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
						Easing(&this->m_NeckPosOffsetPer, (this->m_ReadyTimer == UpperTimerLimit || this->m_ShotPhase >= 2) ? -1.f : 0.f, 0.9f, EasingType::OutExpo);
						if (this->m_Speed > 0.f) {
							this->m_MoveEyePosTimer += this->m_Speed * deg2rad(300.f + 200.f*KeyControl::GetRunPer()) / FPS;

						}
						else {
							this->m_MoveEyePosTimer = 0.f;
						}
						Easing(&this->m_MoveEyePos,
							GetEyeVecX() * 0.25f*sin(this->m_MoveEyePosTimer)*this->m_Speed
							+ GetEyeVecY() * -0.25f*std::abs(cos(this->m_MoveEyePosTimer))*this->m_Speed
							, 0.9f, EasingType::OutExpo);
					}
					//首
					Easing(&m_NeckPer, 1.f, 0.9f, EasingType::OutExpo);
				}
				//下半身
				{
					this->m_BottomAnimSelect = GetBottomStandAnimSel();

					if (KeyControl::GetGoLeftkey().press()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_Stand_LLern; }
					if (KeyControl::GetGoRightkey().press()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_Stand_RLern; }

					if (KeyControl::GetPressLeftGround()) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
					if (KeyControl::GetPressRightGround()) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
					if (KeyControl::GetPressRearGround()) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
					if (KeyControl::GetPressFrontGround()) { this->m_BottomAnimSelect = KeyControl::GetIsRun() ? GetBottomRunAnimSel() : GetBottomWalkAnimSel(); }
				}
				//
				SetAnimLoop(GetBottomTurnAnimSel(), 0.5f);
				SetAnimLoop(GetBottomRunAnimSel(), 1.25f * KeyControl::GetVecFront() * this->m_RunPer2);
				SetAnimLoop(GetBottomWalkAnimSel(), 1.15f * KeyControl::GetVecFront());
				SetAnimLoop(GetBottomLeftStepAnimSel(), 1.15f * KeyControl::GetVecLeft());
				SetAnimLoop(GetBottomWalkBackAnimSel(), 1.15f * KeyControl::GetVecRear());
				SetAnimLoop(GetBottomRightStepAnimSel(), 1.15f * KeyControl::GetVecRight());

				Easing(&this->m_LeftHandPer, 1.f, 0.8f, EasingType::OutExpo);
				this->m_LeftHandPer = 0.f;
			}
			//アニメセレクト
			{
				//下半身
				Easing(&GetAnimeBuf(GetBottomTurnAnimSel()), (this->m_TurnBody) ? abs(KeyControl::GetRad().y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
				//銃操作
				GetAnimeBuf(GetCockingAnimSel()) = ((GetCockingAnimSel() == this->m_UpperAnimSelect) ? 1.f : 0.f);
				GetAnimeBuf(GetReloadStartAnimSel()) = ((GetReloadStartAnimSel() == this->m_UpperAnimSelect) ? 1.f : 0.f);
				GetAnimeBuf(GetReloadOneAnimSel()) = ((GetReloadOneAnimSel() == this->m_UpperAnimSelect) ? 1.f : 0.f);
				GetAnimeBuf(GetReloadEndAnimSel()) = ((GetReloadEndAnimSel() == this->m_UpperAnimSelect) ? 1.f : 0.f);
				//その他
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					//上半身
					for (auto& a : this->m_CharaAnimeSet) {
						if (
							i == (int)a.m_Down ||
							i == (int)a.m_Ready ||
							i == (int)a.m_Cocking ||
							i == (int)a.m_Reload ||
							i == ((int)a.m_Reload + 1) ||
							i == ((int)a.m_Reload + 2)
							)
						{
							GetAnimeBuf((CharaAnimeID)i) = 0.f;
							break;
						}
					}
					if (
						i == (int)CharaAnimeID::Upper_Ready
						) {
						GetAnimeBuf((CharaAnimeID)i) = 1.f;
					}
					//下半身
					if (
						i == (int)CharaAnimeID::Bottom_Stand ||
						i == (int)CharaAnimeID::Bottom_Stand_Run ||
						i == (int)CharaAnimeID::Bottom_Stand_Walk ||
						i == (int)CharaAnimeID::Bottom_Stand_LeftStep ||
						i == (int)CharaAnimeID::Bottom_Stand_RightStep ||
						i == (int)CharaAnimeID::Bottom_Stand_WalkBack ||
						i == (int)CharaAnimeID::Bottom_Stand_Turn ||
						i == (int)CharaAnimeID::Bottom_Squat ||
						i == (int)CharaAnimeID::Bottom_Squat_Walk ||
						i == (int)CharaAnimeID::Bottom_Squat_LeftStep ||
						i == (int)CharaAnimeID::Bottom_Squat_RightStep ||
						i == (int)CharaAnimeID::Bottom_Squat_WalkBack ||
						i == (int)CharaAnimeID::Bottom_Squat_Turn ||
						i == (int)CharaAnimeID::Bottom_Stand_LLern ||
						i == (int)CharaAnimeID::Bottom_Stand_RLern
						)
					{
						GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_BottomAnimSelect) ? 1.f : -1.f)*2.f / FPS;
					}
					GetAnimeBuf((CharaAnimeID)i) = std::clamp(GetAnimeBuf((CharaAnimeID)i), 0.f, 1.f);
					//反映
					GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i);
				}
				//
			}
			//足音
			{
				if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
					auto Time = GetAnime(this->m_BottomAnimSelect).time;
					if (this->m_BottomAnimSelect != GetBottomRunAnimSel()) {
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
				}
			}
			//
			GetObj().work_anime();
		}
		//SetMat指示更新														//0.03ms
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* AnimMngr = GunAnimManager::Instance();

			this->m_RunPer2 = Lerp(0.35f, SpeedLimit / 2.f, KeyControl::GetRunPer());
			this->m_RunPer2 = Lerp(this->m_RunPer2, 0.15f, KeyControl::GetSquatPer());
			if (this->m_PrevRunPer2 == 0.f) {
				this->m_PrevRunPer2 = this->m_RunPer2;
			}

			auto OLDpos = this->m_PosBuf;
			VECTOR_ref vecBuf = MATRIX_ref::Vtrans(KeyControl::GetVec(), MATRIX_ref::RotY(this->m_yrad_Upper));
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
				auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(this->m_PosBuf + VECTOR_ref::up() * -1.f, m_PosBuf + VECTOR_ref::up() * 20.f);
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


			std::vector<MV1*> cols;
			cols.emplace_back((MV1*)(&this->m_BackGround->GetGroundCol()));

			bool ishit = false;
			for (const auto& c : cols) {
				auto HitDim = c->CollCheck_Capsule(GetEyePosition(), GetGunPtrNow()->GetFrameWorldMat(GunFrame::Muzzle).pos(), 0.5f);
				if (HitDim.HitNum > 0) {
					ishit = true;
					break;
				}
			}
			if (ishit) {
				m_IsStuckGun = true;
			}
			else {
				if (m_IsStuckGun) {
					this->m_ReadyTimer = 0.1f;
				}
				m_IsStuckGun = false;
			}


			col_wall(OLDpos, &this->m_PosBuf, cols);

			if (this->m_PosBufOverRideFlag) {
				this->m_PosBufOverRideFlag = false;
				this->m_PosBuf = this->m_PosBufOverRide;
				this->m_move.vec = this->m_VecBufOverRide;
				KeyControl::SetRadBufXY(this->m_RadOverRide);
			}

			this->m_move.mat = MATRIX_ref::RotZ(KeyControl::GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom);
			Easing(&this->m_move.pos, m_PosBuf, 0.9f, EasingType::OutExpo);
			UpdateMove();
			//銃座標指定(アニメアップデート含む)//0.19ms
			{
				VECTOR_ref yVec1, zVec1, Pos1;
				VECTOR_ref yVect, zVect, Post;

				VECTOR_ref yVec2, zVec2, Pos2;
				if (GetGunPtrNow() != nullptr) {
					//持ち手探索
					{
						//アニメアップデート
						GetObj().work_anime();//0.35ms
						//レディ
						{
							auto* Ptr = AnimMngr->GetAnimData(EnumGunAnim::M1911_ready);
							if (Ptr) {
								auto Now = AnimMngr->GetAnimNow(Ptr, 0.f);
								Now.GetPos();
								auto mat = MATRIX_ref::RotX(deg2rad(Now.GetRotate().y()));
								zVec1 = MATRIX_ref::Vtrans(VECTOR_ref::front()*-1.f, mat);
								yVec1 = MATRIX_ref::Vtrans(VECTOR_ref::up(), mat);
								Pos1 = GetFrameWorldMat(CharaFrame::Head).pos() +
									(
										GetEyeVecX()*Now.GetPos().x() +
										GetEyeVecY()*Now.GetPos().y() +
										GetEyeVector()*Now.GetPos().z()
										)*Scale_Rate;
							}
						}
						if (m_ShotPhase <= 1) {
							//構え
							auto* Ptr = AnimMngr->GetAnimData(EnumGunAnim::M1911_aim);
							if (Ptr) {
								auto Now = AnimMngr->GetAnimNow(Ptr, 0.f);
								auto mat = MATRIX_ref::RotX(deg2rad(Now.GetRotate().y()));
								zVect = MATRIX_ref::Vtrans(VECTOR_ref::front()*-1.f, mat);
								yVect = MATRIX_ref::Vtrans(VECTOR_ref::up(), mat);
								Post = GetFrameWorldMat(CharaFrame::Head).pos() +
									(
										GetEyeVecX()*Now.GetPos().x() +
										GetEyeVecY()*Now.GetPos().y() +
										GetEyeVector()*Now.GetPos().z()
										)*Scale_Rate;
							}
						}
						else {
							//リロード
							auto* Ptr = AnimMngr->GetAnimData(EnumGunAnim::M1911_reload);
							if (Ptr) {
								auto Now = AnimMngr->GetAnimNow(Ptr, 0.f);
								auto mat = MATRIX_ref::RotX(deg2rad(Now.GetRotate().y()));
								zVect = MATRIX_ref::Vtrans(VECTOR_ref::front()*-1.f, mat);
								yVect = MATRIX_ref::Vtrans(VECTOR_ref::up(), mat);
								Post = GetFrameWorldMat(CharaFrame::Head).pos() +
									(
										GetEyeVecX()*Now.GetPos().x() +
										GetEyeVecY()*Now.GetPos().y() +
										GetEyeVector()*Now.GetPos().z()
										)*Scale_Rate;
							}
						}
						zVec1 = Lerp(zVec1, zVect, m_ReadyPer);
						yVec1 = Lerp(yVec1, yVect, m_ReadyPer);
						Pos1 = Lerp(Pos1, Post, m_ReadyPer);
					}
					//スリング場所探索
					m_SlingZrad.Execute();
					m_SlingYrad.Execute();
					{
						//printfDx("%6.2f", rad2deg(m_SlingZrad.GetRad()));
						//printfDx("%6.2f", rad2deg(m_SlingYrad.GetRad()));
						auto mat = ((MATRIX_ref::RotZ(deg2rad(130) + this->m_SlingZrad.GetRad()) * MATRIX_ref::RotY(KeyControl::GetRad().y() + deg2rad(180) + this->m_SlingYrad.GetRad()))*GetCharaDir().Inverse());
						yVec2 = mat.xvec();
						zVec2 = mat.yvec();
						Pos2 = GetFrameWorldMat(CharaFrame::Head).pos() + GetFrameWorldMat(CharaFrame::Head).zvec() * -2.f +
							(this->m_move.mat.xvec() * sin(m_SlingZrad.GetRad()) + this->m_move.mat.yvec() * cos(m_SlingZrad.GetRad())) * -6.25f;
					}
				}
				for (auto& p : this->m_Gun_Ptr) {
					if (p != nullptr) {
						int index = (int)(&p - &this->m_Gun_Ptr.front());
						auto yVec = Lerp(yVec1, yVec2, m_SlingPer[index]);
						auto zVec = Lerp(zVec1, zVec2, m_SlingPer[index]);
						auto PosBuf = Lerp(Pos1, Pos2, m_SlingPer[index]);
						Easing(&this->m_SlingPer[index], 1.f, 0.9f, EasingType::OutExpo);
						if (index == this->m_GunSelect) { this->m_SlingPer[index] = 0; }

						auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::front()*-1.f, zVec);
						tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec);
						tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosBuf);
						p->SetGunMatrix(tmp_gunrat, (index == this->m_GunSelect) ? this->m_ShotPhase : 0);
					}
				}
				if (GetGunPtrNow() != nullptr) {
					//腕座標指定
					{
						VECTOR_ref GunPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandPos).pos();
						VECTOR_ref Gunyvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandYvec).pos() - GunPos;
						VECTOR_ref Gunzvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandZvec).pos() - GunPos;
						move_RightArm(GunPos, Gunyvec, Gunzvec);
					}
					if (GetGunPtrNow()->GetIsFrameActive(GunFrame::LeftHandPos)) {
						VECTOR_ref GunPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
						VECTOR_ref Gunyvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - GunPos;
						VECTOR_ref Gunzvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - GunPos;

						VECTOR_ref MagPos = GetFrameWorldMat(CharaFrame::LeftMag).pos();
						VECTOR_ref Magyvec = GetFrameWorldMat(CharaFrame::LeftMagyvec).pos() - MagPos;
						VECTOR_ref Magzvec = GetFrameWorldMat(CharaFrame::LeftMagzvec).pos() - MagPos;

						float Per = 0.f;
						switch (GetGunPtrNow()->GetReloadType()) {
						case RELOADTYPE::MAG:
							if (this->m_ShotPhase == 3) {
								Per = 1.f;
							}
							if (this->m_ShotPhase == 4) {
								if (GetAnime(m_UpperAnimSelect).GetTimePer() < 0.5f) {
									Per = 1.f;
								}
							}
							break;
						case RELOADTYPE::AMMO:
							break;
						default:
							break;
						}

						Easing(&m_MagHansPer, Per, 0.9f, EasingType::OutExpo);
						//LeftMag
						move_LeftArm(Lerp(GunPos, MagPos, m_MagHansPer), Lerp(Gunyvec, Magyvec*-1.f, Per), Lerp(Gunzvec, Magzvec, Per));

					}
				}
			}
			//ヒットボックス
			{
				int ID = 0;
				auto headpos = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f - this->GetEyeVector().Norm() * 1.f;
				m_HitBox[ID].Execute(headpos, 0.13f*Scale_Rate, HitType::Head); ID++;
				m_HitBox[ID].Execute((headpos + GetFrameWorldMat(CharaFrame::Upper).pos()) / 2.f, 0.16f*Scale_Rate, HitType::Body); ID++;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::Upper).pos(), 0.13f*Scale_Rate, HitType::Body); ID++;

				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::Upper).pos() + GetFrameWorldMat(CharaFrame::RightFoot1).pos()) / 2.f, 0.13f*Scale_Rate, HitType::Body); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::Upper).pos() + GetFrameWorldMat(CharaFrame::LeftFoot1).pos()) / 2.f, 0.13f*Scale_Rate, HitType::Body); ID++;

				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::RightArm).pos() + GetFrameWorldMat(CharaFrame::RightArm2).pos()) / 2.f, 0.06f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::RightArm2).pos(), 0.06*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::RightWrist).pos() + GetFrameWorldMat(CharaFrame::RightArm2).pos()) / 2.f, 0.06*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::RightWrist).pos(), 0.06*Scale_Rate, HitType::Leg); ID++;

				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::LeftArm).pos() + GetFrameWorldMat(CharaFrame::LeftArm2).pos()) / 2.f, 0.06*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::LeftArm2).pos(), 0.06*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::LeftWrist).pos() + GetFrameWorldMat(CharaFrame::LeftArm2).pos()) / 2.f, 0.06*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::LeftWrist).pos(), 0.06*Scale_Rate, HitType::Leg); ID++;

				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::RightFoot1).pos(), 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::RightFoot1).pos() + GetFrameWorldMat(CharaFrame::RightFoot2).pos()) / 2.f, 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::RightFoot2).pos(), 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::RightFoot).pos()*0.25f + GetFrameWorldMat(CharaFrame::RightFoot2).pos()*0.75f), 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::RightFoot).pos()*0.5f + GetFrameWorldMat(CharaFrame::RightFoot2).pos()*0.5f), 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::RightFoot).pos()*0.75f + GetFrameWorldMat(CharaFrame::RightFoot2).pos()*0.25f), 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::RightFoot).pos(), 0.095f*Scale_Rate, HitType::Leg); ID++;

				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::LeftFoot1).pos(), 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::LeftFoot1).pos() + GetFrameWorldMat(CharaFrame::LeftFoot2).pos()) / 2.f, 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::LeftFoot2).pos(), 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::LeftFoot).pos()*0.25f + GetFrameWorldMat(CharaFrame::LeftFoot2).pos()*0.75f), 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::LeftFoot).pos()*0.5f + GetFrameWorldMat(CharaFrame::LeftFoot2).pos()*0.5f), 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute((GetFrameWorldMat(CharaFrame::LeftFoot).pos()*0.75f + GetFrameWorldMat(CharaFrame::LeftFoot2).pos()*0.25f), 0.095f*Scale_Rate, HitType::Leg); ID++;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 0.095f*Scale_Rate, HitType::Leg); ID++;
			}
		}
		//顔																	//0.01ms
		void			CharacterClass::ExecuteShape(void) noexcept {
			ShapeControl::ExcuteShape();
			SetShapePer(CharaShape::None, 0.f);
			SetShapePer(CharaShape::O, StaminaControl::GetMousePer());
			SetShapePer(CharaShape::EYECLOSE, ShapeControl::GetEyeclosePer());
		}
		//心拍数																//0.00ms
		void			CharacterClass::ExecuteHeartRate(void) noexcept {
			if (StaminaControl::ExcuteStamina(
				this->m_RunPer2 - this->m_PrevRunPer2,
				(this->m_Speed * this->m_RunPer2 / SpeedLimit * 1.95f) / FPS,
				KeyControl::GetIsSquatGround())) {
				auto SE = SoundPool::Instance();
				SE->Get((int)SoundEnum::Heart).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper2).pos(), Scale_Rate * 1.f);
			}
		}
	};
};
