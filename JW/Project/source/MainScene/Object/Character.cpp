#include	"Character.hpp"


#include "../../ObjectManager.hpp"
//#include "../../MainScene/BackGround/BackGround.hpp"

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
			VECTOR_ref Localzvec = VECTOR_ref::vget(-1.f, -1.f, 0.f).Norm();

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
			VECTOR_ref Localzvec = VECTOR_ref::vget(1.f, -1.f, 0.f).Norm();

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
		void			CharacterClass::ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos, PlayerID pID) noexcept {
			StaminaControl::InitStamina();
			LifeControl::InitLife();
			KeyControl::InitKey(pxRad, pyRad);
			ShapeControl::InitShape();
			//
			this->m_MyID = pID;
			//
			for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
				GetAnimeBuf((CharaAnimeID)i) = 0.f;
			}
			this->m_ReadyPer = 0.f;

			this->m_ReadyArm.Init(false);
			this->m_ReloadStartEmptyArm.Init(false);
			this->m_ReloadStartArm.Init(false);
			this->m_ReloadArm.Init(false);
			this->m_CheckArm.Init(false);

			this->m_ReloadEyePer = 0.f;
			this->m_ReadyTimer = UpperTimerLimit;

			this->m_MagArm.Init(false);
			this->m_MagHand = false;

			this->m_Speed = 0.f;
			this->m_RunPer2 = 0.f;
			this->m_PrevRunPer2 = 0.f;
			this->m_NeckPer = 0.f;
			this->m_LeftHandPer = 0.f;
			this->m_TurnBody = false;
			this->m_ShotPhase = GunAnimeID::Base;
			this->m_MoveEyePosTimer = 0.f;
			this->m_RunReady = false;
			this->m_Running = false;

			this->m_Press_Shot = false;
			this->m_Press_Reload = false;
			this->m_Press_Aim = false;
			this->m_Press_Check = false;
			//サウンド
			this->m_CharaSound = -1;
			//動作にかかわる操作
			this->m_PosBuf = pPos;
			OverrideControl::InitOverride(pPos);
			//上記を反映するもの
			this->m_yrad_Upper = KeyControl::GetRad().y();
			this->m_yrad_Bottom = KeyControl::GetRad().y();
			this->m_LateLeanRad = 0.f;
			this->m_LeanRad = 0.f;
			this->m_UpperMatrix =
				MATRIX_ref::RotZ(this->m_LeanRad) *
				MATRIX_ref::RotX(KeyControl::GetRad().x()) *
				MATRIX_ref::RotY(KeyControl::GetRad().y() - this->m_yrad_Bottom);
			this->m_move.vec.clear();
			SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), m_PosBuf);

			this->m_DamageSwitch = 0;
			this->m_DamageSwitchRec = this->m_DamageSwitch;

			this->m_LaserSwitch.Set(true);

			this->m_SquatPer = 0.f;
			this->m_RunPer = 0.f;
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			auto prev = KeyControl::GetRad();

			KeyControl::InputKey(pInput, pReady, StaminaControl::GetHeartRandVec(0.f));
			{
				auto Vec = KeyControl::GetRad() - prev;
				this->LaserEndPos2D.xadd(Vec.y()*1000.f);
				this->LaserEndPos2D.yadd(Vec.x()*1000.f);
			}
			//AIM
			if (GetGunPtrNow() != nullptr) {
				this->m_Press_Shot = KeyControl::GetShotKey().press();
				this->m_Press_Reload = (KeyControl::GetRKey().press() && (GetAmmoNum() <= GetAmmoAll()));
				this->m_Press_Check = (KeyControl::GetRKey().press() && !(GetAmmoNum() <= GetAmmoAll()));

				if (!GetCanshot()) {
					if (GetAmmoNum() == 0) {
						if (this->m_Press_Shot) {
							this->m_Press_Shot = false;
							this->m_Press_Reload = true;
						}
					}
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
					this->m_Press_Check = false;
				}
			}
			else {
				this->m_Press_Shot = false;
				this->m_Press_Reload = false;
				this->m_Press_Check = false;
			}
			this->m_Press_Aim = KeyControl::GetADSKey().press();
			//
			this->m_RunReady = (KeyControl::GetRun() && (this->m_ShotPhase <= GunAnimeID::Shot));
			//
			this->m_LaserSwitch.Execute(KeyControl::GetAction());
			if (this->m_LaserSwitch.trigger()) {
				auto SE = SoundPool::Instance();
				SE->Get((int)SoundEnum::LaserSwitch).Play(0, DX_PLAYTYPE_BACK);
			}
		}
		void			CharacterClass::SetEyeVec() noexcept {
			return;
			Easing(&m_EyeVecR, m_CamEyeVec, 0.8f, EasingType::OutExpo);

			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Head));
			auto v1 = (GetFrameWorldMat(CharaFrame::Head).GetRot() * GetCharaDir().Inverse()).zvec()*-1.f;
			auto v2 = Lerp(MATRIX_ref::Vtrans(m_EyeVecR.Norm(), GetCharaDir().Inverse()), v1, 0.f);

			auto radlimit = deg2rad(70);
			if (v1.dot(v2) <= cos(radlimit)) {
				v2 = v1 * cos(radlimit) + v1.cross(v1.cross(v2)) * (-sin(radlimit));
			}
			SetFrameLocalMat(CharaFrame::Head, MATRIX_ref::RotX(deg2rad(-10))*MATRIX_ref::RotVec2(v1, v2) * GetFrameLocalMat(CharaFrame::Head).GetRot());
		}
		//以前の状態保持														//
		void			CharacterClass::ExecuteSavePrev(void) noexcept {
			this->m_PrevRunPer2 = this->m_RunPer2;
			m_SendCamShake = false;
		}
		//操作																	//0.01ms
		void			CharacterClass::ExecuteInput(void) noexcept {
			//
			if (GetGunPtrNow() != nullptr) {
				GetGunPtrNow()->SetUseMoveParts(true);

				GetGunPtrNow()->SetIsShot(false);
				if ((this->m_Press_Shot || this->m_Press_Reload || this->m_Press_Check) && (this->m_ShotPhase == GunAnimeID::Base)) {
					this->m_ReadyTimer = std::min(this->m_ReadyTimer, 0.1f);
					if (this->m_ReadyPer >= 0.9f) {
						this->m_ReadyPer = 1.f;
						if (this->m_Press_Shot) {
							if (GetGunPtrNow()->GetCanShot()) {
								this->m_ShotPhase = GunAnimeID::Shot;
								GetGunPtrNow()->SetBullet();
								this->m_GunShakePer = 1.f;
								this->m_RecoilRadAdd.Set(GetRandf(0.007f), -0.01f, 0.f);

								this->m_RecoilRadAdd = MATRIX_ref::Vtrans(this->m_RecoilRadAdd, MATRIX_ref::RotZ(-m_LeanRad / 5.f));
							}
							else {
								if (!GetGunPtrNow()->GetIsMagEmpty()) {
									this->m_ShotPhase = GunAnimeID::Cocking;
								}
								else {
									this->m_ShotPhase = GunAnimeID::ReloadStart_Empty;
								}
							}
						}
						if (this->m_Press_Reload) {
							if (!GetGunPtrNow()->GetIsMagEmpty()) {
								this->m_ShotPhase = GunAnimeID::ReloadStart;
							}
							else {
								this->m_ShotPhase = GunAnimeID::ReloadStart_Empty;
							}
						}
						if (this->m_Press_Check) {
							this->m_ShotPhase = GunAnimeID::CheckStart;
						}

					}
				}
				else {
					this->m_ReadyTimer = std::clamp(this->m_ReadyTimer + 1.f / FPS, 0.f, m_Press_Aim ? 0.f : UpperTimerLimit);
				}
				if ((this->m_ShotPhase >= GunAnimeID::Cocking)) {
					this->m_ReadyTimer = UpperTimerLimit;
				}
				if (KeyControl::GetRun()) {
					this->m_ReadyTimer = UpperTimerLimit;
				}
			}
			else {
				this->m_ReadyTimer = std::clamp(this->m_ReadyTimer + 1.f / FPS, 0.f, UpperTimerLimit);
				if (this->m_Speed > 0.6f || KeyControl::GetRun()) {
					this->m_ReadyTimer = UpperTimerLimit;
				}
			}
			if (m_IsStuckGun) {
				this->m_ReadyTimer = UpperTimerLimit;
			}

			if (this->m_RecoilRadAdd.y() < 0.f) {
				Easing(&this->m_RecoilRadAdd, VECTOR_ref::vget(0.f, 0.01f, 0.f), 0.7f, EasingType::OutExpo);
			}
			else {
				Easing(&this->m_RecoilRadAdd, VECTOR_ref::zero(), 0.7f, EasingType::OutExpo);
			}
			//
			Easing(&this->m_ReloadEyePer, (this->m_ShotPhase >= GunAnimeID::Cocking && this->m_ShotPhase <= GunAnimeID::ReloadEnd) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			Easing(&this->m_CheckEyePer, (this->m_ShotPhase >= GunAnimeID::Checking) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			Easing(&this->m_ReadyPer, (this->m_ReadyTimer < UpperTimerLimit || !(m_ShotPhase <= GunAnimeID::Shot)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

			this->m_ReloadStartArm.Execute(this->m_ShotPhase == GunAnimeID::ReloadStart, 0.2f, 0.2f);
			this->m_ReloadStartEmptyArm.Execute(this->m_ShotPhase == GunAnimeID::ReloadStart_Empty, 0.5f, 0.2f);
			this->m_ReloadArm.Execute(this->m_ShotPhase >= GunAnimeID::ReloadOne && this->m_ShotPhase <= GunAnimeID::ReloadEnd, 0.1f, 0.2f);
			this->m_ReadyArm.Execute(!(this->m_ReadyTimer < UpperTimerLimit || !(m_ShotPhase <= GunAnimeID::Shot)), 0.1f, 0.2f);
			this->m_CheckArm.Execute(this->m_ShotPhase >= GunAnimeID::CheckStart && this->m_ShotPhase <= GunAnimeID::Checking, 0.05f, 0.2f);
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
			//
			{
				if (KeyControl::GetRun()) {
					KeyControl::ResetLeanRate();
				}
				Easing(&this->m_LeanRatePer, (float)(-KeyControl::GetLeanRate()), 0.9f, EasingType::OutExpo);

				if (m_LeanSoundReq != KeyControl::GetLeanRate()) {
					auto SE = SoundPool::Instance();
					SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale_Rate * 3.f);
				}
				m_LeanSoundReq = KeyControl::GetLeanRate();
			}
			//
			{
				Easing(&this->m_SquatPer, KeyControl::GetIsSquat() ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				Easing(&this->m_RunPer, KeyControl::GetRun() ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			}
			//
			auto FrontP = KeyControl::GetFrontP();
			auto TmpRunPer = 0.85f;
			if (this->m_TurnBody || (this->m_Speed > 0.1f)) { Easing(&this->m_yrad_Upper, KeyControl::GetRad().y(), TmpRunPer, EasingType::OutExpo); }
			auto OLDP = this->m_yrad_Bottom;
			Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP, TmpRunPer, EasingType::OutExpo);
			KeyControl::SetRadBufZ((this->m_yrad_Bottom - OLDP) * 2.f);

			SetEyeVec();
		}
		//上半身回転															//0.06ms
		void			CharacterClass::ExecuteUpperMatrix(void) noexcept {
			Easing(&this->m_LateLeanRad, this->m_LeanRad, 0.9f, EasingType::OutExpo);
			this->m_LeanRad = deg2rad(25) * this->m_LeanRatePer;

			float yrad = Lerp(KeyControl::GetRad().y() - this->m_yrad_Bottom, 0.f, this->m_RunPer);

			this->m_UpperMatrix =
				MATRIX_ref::RotZ(this->m_LeanRad) *
				MATRIX_ref::RotX(KeyControl::GetRad().x()) *
				MATRIX_ref::RotY(yrad);

			auto tmpUpperMatrix =
				MATRIX_ref::RotZ(this->m_LeanRad) *
				MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f) *
				MATRIX_ref::RotY(yrad);

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
					this->m_GunAnimSelect = CharaGunAnimeID::Down;
					bool canreverse = true;
					if (!this->m_RunReady && !this->m_Running) {
						this->m_RunReady = true;
						canreverse = false;
					}
					this->m_Running = this->m_RunReady;
					if (GetShotSwitch()) {
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
									this->m_ShotPhase = GunAnimeID::Base;
									break;
								case SHOTTYPE::BOLT:
									this->m_ShotPhase = GunAnimeID::Cocking;
									break;
								default:
									break;
								}
							}
							else {
								this->m_ShotPhase = GunAnimeID::Base;
							}
						}
					}
					//コッキング
					if (this->m_ShotPhase == GunAnimeID::Cocking) {
						this->m_GunAnimSelect = CharaGunAnimeID::Cocking;
						UpdateGunAnim(this->m_GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(this->m_GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::Base;
							this->m_ReadyTimer = 0.1f;
						}
					}
					//リロード開始
					if (this->m_ShotPhase == GunAnimeID::ReloadStart_Empty) {
						this->m_GunAnimSelect = CharaGunAnimeID::ReloadStart_Empty;
						if (GetGunAnimZero(this->m_GunAnimSelect)) {
							m_UpperAnim = 0.f;
						}
						UpdateGunAnim(this->m_GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(this->m_GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::ReloadOne;
							this->m_ReadyTimer = 0.1f;
						}
					}
					if (this->m_ShotPhase == GunAnimeID::ReloadStart) {
						this->m_GunAnimSelect = CharaGunAnimeID::ReloadStart;
						if (GetGunAnimZero(this->m_GunAnimSelect)) {
							m_UpperAnim = 0.f;
						}
						UpdateGunAnim(this->m_GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(this->m_GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::ReloadOne;
							this->m_ReadyTimer = 0.1f;
						}
					}
					//
					if (this->m_ShotPhase == GunAnimeID::ReloadOne) {
						this->m_GunAnimSelect = CharaGunAnimeID::ReloadOne;
						UpdateGunAnim(this->m_GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(this->m_GunAnimSelect)) {
							switch (GetGunPtrNow()->GetReloadType()) {
							case RELOADTYPE::MAG:
								this->m_ShotPhase = GunAnimeID::ReloadEnd;
								break;
							case RELOADTYPE::AMMO:
								if (GetGunPtrNow()->GetIsMagFull()) {
									this->m_ShotPhase = GunAnimeID::ReloadEnd;
								}
								break;
							default:
								break;
							}
							this->m_ReadyTimer = 0.1f;
						}
					}
					if (this->m_ShotPhase == GunAnimeID::ReloadEnd) {
						this->m_GunAnimSelect = CharaGunAnimeID::ReloadEnd;
						UpdateGunAnim(this->m_GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(this->m_GunAnimSelect)) {
							if (GetGunPtrNow()->GetCanShot()) {
								this->m_ShotPhase = GunAnimeID::Base;
							}
							else {
								if (!GetGunPtrNow()->GetIsMagEmpty()) {
									this->m_ShotPhase = GunAnimeID::Cocking;
								}
								else {
									this->m_ShotPhase = GunAnimeID::ReloadStart;
								}
							}
							this->m_ReadyTimer = 0.1f;
						}
					}
					//
					if (this->m_ShotPhase == GunAnimeID::CheckStart) {
						this->m_GunAnimSelect = CharaGunAnimeID::CheckStart;
						UpdateGunAnim(this->m_GunAnimSelect, 1.5f);
						if (GetGunAnimEnd(this->m_GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::Checking;
						}
					}
					if (this->m_ShotPhase == GunAnimeID::Checking) {
						this->m_GunAnimSelect = CharaGunAnimeID::Check;
						UpdateGunAnim(this->m_GunAnimSelect, 1.f);
						if (GetGunAnimEnd(this->m_GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::CheckEnd;
						}
					}
					if (this->m_ShotPhase == GunAnimeID::CheckEnd) {
						this->m_GunAnimSelect = CharaGunAnimeID::CheckEnd;
						UpdateGunAnim(this->m_GunAnimSelect, 2.f);
						if (GetGunAnimEnd(this->m_GunAnimSelect)) {
							this->m_ShotPhase = GunAnimeID::Base;
							this->m_ReadyTimer = 0.1f;
						}
					}
					for (int i = 0; i < (int)CharaGunAnimeID::Max; i++) {
						if (this->m_GunAnimSelect != (CharaGunAnimeID)i) {
							m_GunAnimFrame.at(i) = 0.f;
						}
					}
					//
					{
						if (this->m_Speed > 0.f) {
							this->m_MoveEyePosTimer += this->m_Speed * deg2rad(300.f + 200.f*this->m_RunPer) / FPS;

						}
						else {
							this->m_MoveEyePosTimer = 0.f;
						}
						Easing(&this->m_MoveEyePos,
							GetCharaVecX() * 0.25f*sin(this->m_MoveEyePosTimer)*this->m_Speed
							+ GetCharaVecY() * -0.25f*std::abs(cos(this->m_MoveEyePosTimer))*this->m_Speed
							, 0.9f, EasingType::OutExpo);
					}
					//首
					Easing(&m_NeckPer, 0.f, 0.9f, EasingType::OutExpo);
				}
				//下半身
				{
					this->m_BottomAnimSelect = GetBottomStandAnimSel();

					if (KeyControl::GetPressLeft()) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
					if (KeyControl::GetPressRight()) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
					if (KeyControl::GetPressRear()) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
					if (KeyControl::GetPressFront()) { this->m_BottomAnimSelect = KeyControl::GetRun() ? GetBottomRunAnimSel() : GetBottomWalkAnimSel(); }
				}
				//
				SetAnimLoop(GetBottomTurnAnimSel(), 0.5f);
				SetAnimLoop(GetBottomRunAnimSel(), 1.f * KeyControl::GetVecFront() * this->m_RunPer2);
				SetAnimLoop(GetBottomWalkAnimSel(), 1.15f * KeyControl::GetVecFront());
				SetAnimLoop(GetBottomLeftStepAnimSel(), 1.15f * KeyControl::GetVecLeft());
				SetAnimLoop(GetBottomWalkBackAnimSel(), 1.15f * KeyControl::GetVecRear());
				SetAnimLoop(GetBottomRightStepAnimSel(), 1.15f * KeyControl::GetVecRight());

				Easing(&this->m_LeftHandPer, 1.f, 0.8f, EasingType::OutExpo);
				this->m_LeftHandPer = 0.f;

				Easing(&this->m_ADSPer, this->GetIsADS() ? 1.f : 0.f, 0.8f, EasingType::OutExpo);//
			}
			//アニメセレクト
			{
				//下半身
				Easing(&GetAnimeBuf(GetBottomTurnAnimSel()), (this->m_TurnBody) ? abs(KeyControl::GetRad().y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
				//その他
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					//上半身
					if (i == (int)CharaAnimeID::Upper_Ready) {
						GetAnimeBuf((CharaAnimeID)i) = 1.f;
					}
					if (i == (int)CharaAnimeID::Hand_Ready) {
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
						i == (int)CharaAnimeID::Bottom_Stand_Turn) {
						GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_BottomAnimSelect) ? 3.f : -1.f)*2.f / FPS;
					}
					if (
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
								m_SendCamShake = true;
							}
						}
						//R
						if ((27.f < Time &&Time < 28.f)) {
							if (this->m_CharaSound != 3) {
								this->m_CharaSound = 3;
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 5.f);
								m_SendCamShake = true;
							}
						}
					}
					else {
						//L
						if (
							(3.f < Time &&Time < 4.f)
							) {
							if (this->m_CharaSound != 5) {
								this->m_CharaSound = 5;
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale_Rate * 15.f);
								m_SendCamShake = true;
							}
						}
						//R
						if (
							(10.f < Time &&Time < 11.f)
							) {
							if (this->m_CharaSound != 6) {
								this->m_CharaSound = 6;
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 15.f);
								m_SendCamShake = true;
							}
						}
					}
				}
				if (this->m_SquatSoundReq != KeyControl::GetIsSquat()) {
					this->m_SquatSoundReq = KeyControl::GetIsSquat();
					SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale_Rate * 3.f);
				}
			}
			//
			GetObj().work_anime();
		}
		//SetMat指示更新														//0.03ms
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			//auto SE = SoundPool::Instance();
			auto* AnimMngr = GunAnimManager::Instance();
			//auto* DrawParts = DXDraw::Instance();

			this->m_RunPer2 = Lerp(0.65f, SpeedLimit / 2.f, this->m_RunPer);
			this->m_RunPer2 = Lerp(this->m_RunPer2, 0.45f, this->m_SquatPer);
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
#if FALSE
				auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(this->m_PosBuf + VECTOR_ref::up() * -1.f, m_PosBuf + VECTOR_ref::up() * 20.f);
				if (HitResult.HitFlag == TRUE) {
					auto yPos = this->m_PosBuf.y();
					Easing(&yPos, HitResult.HitPosition.y, 0.8f, EasingType::OutExpo);
					this->m_PosBuf.y(yPos);
					this->m_move.vec.y(0.f);
				}
#else
				//高度0なら止まる
				if (this->m_PosBuf.y() <= 0.f) {
					auto yPos = this->m_PosBuf.y();
					Easing(&yPos, 0.f, 0.8f, EasingType::OutExpo);
					this->m_PosBuf.y(yPos);
					this->m_move.vec.y(0.f);
				}
#endif
				else {
					this->m_move.vec.yadd(M_GR / (FPS * FPS));
				}
			}
			this->m_PosBuf += this->m_move.vec;


			auto EndPos = GetGunPtrNow()->GetMuzzleMatrix().pos();
			if (this->m_BackGround->CheckLinetoMap(GetFrameWorldMat(CharaFrame::Head).pos(), &EndPos, true)) {
				m_IsStuckGun = true;
			}
			else {
				if (m_IsStuckGun) {
					this->m_ReadyTimer = 0.1f;
				}
				m_IsStuckGun = false;
			}

			std::vector<std::pair<MV1*, int>> cols;
			//cols.emplace_back(std::make_pair((MV1*)(&this->m_BackGround->GetGroundCol()), -1));
			{
				auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);
				for (auto& C : MainGB->GetBuildCol()) {
					auto Vec = (C.GetMatrix().pos() - this->m_move.pos); Vec.y(0.f);
					if (Vec.Length() <= 2.f*Scale_Rate) {
						cols.emplace_back(std::make_pair((MV1*)(&C.GetCol()), C.GetMeshSel()));
					}
				}
			}
			col_wall(OLDpos, &this->m_PosBuf, cols);

			VECTOR_ref RadOverRide;
			if (OverrideControl::PutOverride(&this->m_PosBuf, &this->m_move.vec, &RadOverRide)) {
				KeyControl::SetRadBufXY(RadOverRide);
			}

			this->m_move.mat = MATRIX_ref::RotZ(KeyControl::GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom);
			this->m_move.repos = this->m_move.pos;
			Easing(&this->m_move.pos, m_PosBuf, 0.9f, EasingType::OutExpo);
			UpdateMove();
			//銃座標指定(アニメアップデート含む)//0.19ms
			{
				auto PosO = GetFrameWorldMat(CharaFrame::Head).pos();
				VECTOR_ref yVec1, zVec1, Pos1;
				if (GetGunPtrNow() != nullptr) {
					//持ち手探索
					{
						//アニメアップデート
						GetObj().work_anime();
						//銃の位置を指定するアニメ
						auto PosBase = GetFrameWorldMat(CharaFrame::Head).pos() - PosO;
						std::array<MATRIX_ref, (int)EnumGunAnimType::Max> MatT;
						for (int i = 0; i < (int)EnumGunAnimType::Max; i++) {
							auto* Ptr = AnimMngr->GetAnimData(this->m_GunAnimeSet[this->m_CharaAnimeSel].m_Anim.at(i).at(this->m_ReadyAnimeSel));
							if (Ptr) {
								auto Now = AnimMngr->GetAnimNow(Ptr, m_UpperAnim);
								MatT[i] = Now.GetRot() * MATRIX_ref::Mtrans(PosBase + MATRIX_ref::Vtrans(Now.GetPos(), GetCharaDir()));
							}
						}
						//
						VECTOR_ref yVect0, zVect0, Post0;

						zVect0 = MatT[(int)EnumGunAnimType::Aim].zvec();
						yVect0 = MatT[(int)EnumGunAnimType::Aim].yvec();
						Post0 = MatT[(int)EnumGunAnimType::Aim].pos();

						zVect0 = Lerp(zVect0, MatT[(int)EnumGunAnimType::ADS].zvec(), this->m_ADSPer);
						yVect0 = Lerp(yVect0, MatT[(int)EnumGunAnimType::ADS].yvec(), this->m_ADSPer);
						Post0 = Lerp(Post0, MatT[(int)EnumGunAnimType::ADS].pos(), this->m_ADSPer);

						zVect0 = Lerp(zVect0, MatT[(int)EnumGunAnimType::ReloadStart_Empty].zvec(), this->m_ReloadStartEmptyArm.Per());
						yVect0 = Lerp(yVect0, MatT[(int)EnumGunAnimType::ReloadStart_Empty].yvec(), this->m_ReloadStartEmptyArm.Per());
						Post0 = Lerp(Post0, MatT[(int)EnumGunAnimType::ReloadStart_Empty].pos(), this->m_ReloadStartEmptyArm.Per());
						
						zVect0 = Lerp(zVect0, MatT[(int)EnumGunAnimType::ReloadStart].zvec(), this->m_ReloadStartArm.Per());
						yVect0 = Lerp(yVect0, MatT[(int)EnumGunAnimType::ReloadStart].yvec(), this->m_ReloadStartArm.Per());
						Post0 = Lerp(Post0, MatT[(int)EnumGunAnimType::ReloadStart].pos(), this->m_ReloadStartArm.Per());

						zVect0 = Lerp(zVect0, MatT[(int)EnumGunAnimType::Reload].zvec(), this->m_ReloadArm.Per());
						yVect0 = Lerp(yVect0, MatT[(int)EnumGunAnimType::Reload].yvec(), this->m_ReloadArm.Per());
						Post0 = Lerp(Post0, MatT[(int)EnumGunAnimType::Reload].pos(), this->m_ReloadArm.Per());

						zVect0 = Lerp(zVect0, MatT[(int)EnumGunAnimType::Ready].zvec(), this->m_ReadyArm.Per());
						yVect0 = Lerp(yVect0, MatT[(int)EnumGunAnimType::Ready].yvec(), this->m_ReadyArm.Per());
						Post0 = Lerp(Post0, MatT[(int)EnumGunAnimType::Ready].pos(), this->m_ReadyArm.Per());

						zVect0 = Lerp(zVect0, MatT[(int)EnumGunAnimType::Run].zvec(), this->m_RunPer);
						yVect0 = Lerp(yVect0, MatT[(int)EnumGunAnimType::Run].yvec(), this->m_RunPer);
						Post0 = Lerp(Post0, MatT[(int)EnumGunAnimType::Run].pos(), this->m_RunPer);

						zVect0 = Lerp(zVect0, MatT[(int)EnumGunAnimType::Check].zvec(), this->m_CheckArm.Per());
						yVect0 = Lerp(yVect0, MatT[(int)EnumGunAnimType::Check].yvec(), this->m_CheckArm.Per());
						Post0 = Lerp(Post0, MatT[(int)EnumGunAnimType::Check].pos(), this->m_CheckArm.Per());
						//

						zVec1 = zVect0;
						yVec1 = MATRIX_ref::Vtrans(yVect0, MATRIX_ref::RotAxis(zVect0, (KeyControl::GetRad().y() - this->m_yrad_Bottom)*0.15f + (this->m_LateLeanRad - this->m_LeanRad)));
						Pos1 = Post0 + m_GunShake_r;
						//銃振動
						m_GunShakePer = std::max(m_GunShakePer - 1.f / FPS / 0.1f, 0.f);
						Easing(&m_GunShake, VECTOR_ref::vget(GetRandf(1.f), 0.5f + GetRandf(1.f), GetRandf(1.f))*Scale_Rate, 0.7f, EasingType::OutExpo);
						Easing(&m_GunShake_r, m_GunShake*m_GunShakePer*0.1f, 0.9f, EasingType::OutExpo);
					}

					m_UpperAnim += 60.f / FPS;
					//スリング場所探索
					Easing(&m_UpperRad, (KeyControl::GetRad() - this->m_UpperPrevRad)*-1.f, 0.9f, EasingType::OutExpo);
					m_UpperPrevRad = KeyControl::GetRad();
					//エイム先
					auto mat = MATRIX_ref::RotX(m_UpperRad.x()) * MATRIX_ref::RotY(m_UpperRad.y());
					Easing(&m_UpperyVecNormal, mat.yvec(), 0.8f, EasingType::OutExpo);
					Easing(&m_UpperzVecNormal, mat.zvec(), 0.8f, EasingType::OutExpo);
					Easing(&m_UpperyVec, m_UpperyVecNormal, 0.8f, EasingType::OutExpo);
					Easing(&m_UpperzVec, m_UpperzVecNormal, 0.8f, EasingType::OutExpo);
				}
				if (GetGunPtrNow() != nullptr) {
					auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::front(), zVec1);
					tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec1);
					tmp_gunrat *= MATRIX_ref::Axis1_YZ(m_UpperyVec.Norm(), m_UpperzVec.Norm());
					tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosO + Pos1);
					GetGunPtrNow()->SetGunMatrix(tmp_gunrat, this->m_ShotPhase);
				}
				if (GetGunPtrNow() != nullptr) {
					VECTOR_ref GunPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandPos).pos();
					VECTOR_ref Gunyvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandYvec).pos() - GunPos;
					VECTOR_ref Gunzvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandZvec).pos() - GunPos;
					VECTOR_ref HandPos;
					VECTOR_ref Handyvec;
					VECTOR_ref Handzvec;
					if (GetGunPtrNow()->GetIsFrameActive(GunFrame::LeftHandPos)) {
						HandPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
						Handyvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - HandPos;
						Handzvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - HandPos;

						VECTOR_ref MagPos = GetFrameWorldMat(CharaFrame::LeftMag).pos();
						VECTOR_ref Magyvec = GetFrameWorldMat(CharaFrame::LeftMagyvec).pos() - MagPos;
						VECTOR_ref Magzvec = GetFrameWorldMat(CharaFrame::LeftMagzvec).pos() - MagPos;

						switch (GetGunPtrNow()->GetReloadType()) {
						case RELOADTYPE::MAG:
							switch (this->m_ShotPhase) {
							case GunAnimeID::ReloadStart_Empty:
								if (GetTimePer(m_GunAnimSelect) > 0.5f) {
									if (!m_MagHand) {
										GetGunPtrNow()->SetMagFall();
									}
									m_MagHand = true;
								}
								else {
									m_MagHand = false;
								}
								break;
							case GunAnimeID::ReloadStart:
								if (GetTimePer(m_GunAnimSelect) > 0.8f) {
									m_MagHand = true;
								}
								break;
							case GunAnimeID::ReloadOne:
								if (GetTimePer(m_GunAnimSelect) < 0.6f) {
									m_MagHand = true;
								}
								else {
									m_MagHand = false;
								}
								break;
							default:
								m_MagHand = false;
								break;
							}
							break;
						case RELOADTYPE::AMMO:
							break;
						default:
							break;
						}
						m_MagArm.Execute(m_MagHand, 0.3f, 0.5f);

						HandPos = Lerp(HandPos, MagPos, m_MagArm.Per());
						Handyvec = Lerp(Handyvec, Magyvec, m_MagHand ? 1.f : 0.f);
						Handzvec = Lerp(Handzvec, Magzvec, m_MagHand ? 1.f : 0.f);
					}
					//腕座標指定
					{
						auto LerpPer = 1.f;
						move_RightArm(Lerp(GunPos, HandPos, 1.f - LerpPer), Lerp(Gunyvec, Handyvec*-1.f, 1.f - LerpPer), Lerp(Gunzvec, Handzvec, 1.f - LerpPer));
						move_LeftArm(Lerp(GunPos, HandPos, LerpPer), Lerp(Gunyvec*-1.f, Handyvec, LerpPer), Lerp(Gunzvec, Handzvec, LerpPer));
					}
					//LeftMag
					{
						MATRIX_ref Mat = GetGunPtrNow()->GetFrameWorldMat(GunFrame::Magpos);
						if (
							this->m_ShotPhase == GunAnimeID::ReloadStart_Empty ||
							this->m_ShotPhase == GunAnimeID::ReloadStart ||
							this->m_ShotPhase == GunAnimeID::ReloadOne ||
							this->m_ShotPhase == GunAnimeID::CheckStart ||
							this->m_ShotPhase == GunAnimeID::Checking
							) {
							Mat = GetFrameWorldMat(CharaFrame::LeftHandJoint);
						}
						switch (this->m_ShotPhase) {
						case GunAnimeID::ReloadStart_Empty:
							m_AmmoHandR = 1.f;
							m_AmmoHand = 1.f;
							break;
						case GunAnimeID::ReloadStart:
							m_AmmoHandR = 1.f;
							m_AmmoHand = 1.f;
							break;
						case GunAnimeID::ReloadOne:
							if (GetTimePer(m_GunAnimSelect) < 0.86f) {
								m_AmmoHandR = 1.f;
								m_AmmoHand = 1.f;
							}

							if (GetTimePer(m_GunAnimSelect) > 0.96f) {
								m_AmmoHandR = 0.f;
								m_AmmoHand = 0.f;
							}
							break;
						case GunAnimeID::CheckStart:
							if (GetTimePer(m_GunAnimSelect) > 0.8f) {
								m_AmmoHandR = 1.f;
							}
						case GunAnimeID::Checking:
							if (GetTimePer(m_GunAnimSelect) < 0.8f) {
								m_AmmoHandR = 1.f;
								m_AmmoHand = 1.f;
							}
							if (GetTimePer(m_GunAnimSelect) > 0.8f) {
								m_AmmoHandR = 0.f;
								m_AmmoHand = 0.f;
							}
							break;
						default:
							m_AmmoHandR = 0.f;
							break;
						}
						GetGunPtrNow()->SetAmmoHandMatrix(Mat, m_AmmoHand);
						Easing(&m_AmmoHand, m_AmmoHandR, 0.9f, EasingType::OutExpo);
					}
				}
			}
			//ヒットボックス
			{
				int ID = 0;
				auto headpos = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f - this->GetCharaVector().Norm() * 1.f;
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
			//
			{
				m_WalkSwingRad.Set(5.f, 0.f, 10.f);
				//
				auto Pos = GetEyePosition() - GetMatrix().pos();
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
				auto WS_tmp = m_WalkSwing_t * std::clamp(this->m_Speed * this->m_RunPer2 / SpeedLimit, 0.f, 1.f);

				WS_tmp.xadd(-2.f*m_ReloadEyePer);
				WS_tmp.xadd(-1.f*this->m_CheckEyePer);
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
				KeyControl::GetIsSquat())) {
				auto SE = SoundPool::Instance();
				SE->Get((int)SoundEnum::Heart).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 0.5f);
			}
		}



		void			CharacterClass::Init(void) noexcept {
			ObjectBaseClass::Init();
			{
				for (int i = 0; i < GetObj().get_anime().size(); i++) { GetAnimeBuf((CharaAnimeID)i) = 0.f; }
				GetAnime(GetBottomStandAnimSel()).per = 1.f;
				//
				m_CharaAnimeSet.clear();
				//M4
				m_CharaAnimeSet.resize(m_CharaAnimeSet.size() + 1);
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::Down) = 0;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::Ready) = 0;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::ADS) = 0;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::Cocking) = 35;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::CheckStart) = 15;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::Check) = 30;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::CheckEnd) = 30;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::ReloadStart_Empty) = 15;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::ReloadStart) = 15;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::ReloadOne) = 30;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::ReloadEnd) = 10;
				//ハンドガン
				m_CharaAnimeSet.resize(m_CharaAnimeSet.size() + 1);
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::Down) = 0;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::Ready) = 0;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::ADS) = 0;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::Cocking) = 35;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::CheckStart) = 15;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::Check) = 30;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::CheckEnd) = 30;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::ReloadStart_Empty) = 15;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::ReloadStart) = 15;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::ReloadOne) = 30;
				m_CharaAnimeSet.back().m_GunAnimAllFrame.at((int)CharaGunAnimeID::ReloadEnd) = 10;
				//
				m_GunAnimeSet.clear();
				//M4
				m_GunAnimeSet.resize(m_GunAnimeSet.size() + 1);
				
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Aim).emplace_back(EnumGunAnim::M16_aim);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::ADS).emplace_back(EnumGunAnim::M16_ads);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::ReloadStart_Empty).emplace_back(EnumGunAnim::M1911_reloadstart_empty);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::ReloadStart).emplace_back(EnumGunAnim::M1911_reloadstart);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Reload).emplace_back(EnumGunAnim::M16_reload);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Ready).emplace_back(EnumGunAnim::M16_ready1);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Ready).emplace_back(EnumGunAnim::M16_ready2);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Run).emplace_back(EnumGunAnim::M16_run);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Check).emplace_back(EnumGunAnim::M1911_check1);
				//ハンドガン
				m_GunAnimeSet.resize(m_GunAnimeSet.size() + 1);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Aim).emplace_back(EnumGunAnim::M1911_aim1);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Aim).emplace_back(EnumGunAnim::M1911_aim2);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::ADS).emplace_back(EnumGunAnim::M1911_ads);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::ReloadStart_Empty).emplace_back(EnumGunAnim::M1911_reloadstart_empty);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::ReloadStart).emplace_back(EnumGunAnim::M1911_reloadstart);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Reload).emplace_back(EnumGunAnim::M1911_reload);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Ready).emplace_back(EnumGunAnim::M1911_ready1);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Ready).emplace_back(EnumGunAnim::M1911_ready2);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Run).emplace_back(EnumGunAnim::M1911_run);
				m_GunAnimeSet.back().m_Anim.at((int)EnumGunAnimType::Check).emplace_back(EnumGunAnim::M1911_check1);
				//
				m_HitBox.resize(27);
			}
			//
			{
				int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
				//getparams::_str(mdata);
				FileRead_close(mdata);
			}
			//
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				for (int i = 0; i < GetObj().get_anime().size(); i++) { GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i); }
				GetObj().work_anime();
			}
			if (GetGunPtrNow() != nullptr) {
				auto prev = this->m_CharaAnimeSel;
				auto newtmp = GetGunPtrNow()->GetHumanAnimType();
				bool ischange = true;
				if (prev != newtmp) {
					if (this->m_GunAnimSelect == CharaGunAnimeID::Down) {
						ischange = false;
					}
					this->m_ReadyTimer = 0.1f;
					this->m_ReadyPer = 0.f;
				}
				this->m_CharaAnimeSel = newtmp;
			}
			ExecuteSavePrev();			//以前の状態保持
			ExecuteInput();				//操作//0.01ms
			ExecuteUpperMatrix();		//上半身回転//0.06ms
			ExecuteAnim();				//AnimUpdte//0.03ms
			ExecuteMatrix();			//SetMat指示//0.03ms
			ExecuteShape();				//顔//スコープ内0.01ms
			ExecuteHeartRate();			//心拍数//0.00ms
			EffectControl::Execute();
		}
		void			CharacterClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if ((CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-2.5f*Scale_Rate, -0.5f*Scale_Rate, -2.5f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(2.5f*Scale_Rate, 2.f*Scale_Rate, 2.5f*Scale_Rate)).get()) == FALSE)
					) {
					//
					int fog_enable;
					int fog_mode;
					int fog_r, fog_g, fog_b;
					float fog_start, fog_end;
					float fog_density;
					fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
					fog_mode = GetFogMode();														// フォグモードを取得する
					GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
					GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
					fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )


					//キャラ描画
					SetFogEnable(TRUE);
					SetFogColor(0, 0, 0);
					//MV1SetMaterialTypeAll(this->GetObj().get(), DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
					for (int i = 0; i < this->GetObj().mesh_num(); i++) {
						if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
							this->GetObj().DrawMesh(i);
						}
					}
					//hitbox描画
					if (false)
					{
						//this->GetObj().SetOpacityRate(0.5f);
						SetFogEnable(FALSE);
						SetUseLighting(FALSE);
						SetUseZBuffer3D(FALSE);

						for (auto& h : this->m_HitBox) {
							h.Draw();
						}

						SetUseZBuffer3D(TRUE);
						SetUseLighting(TRUE);
					}
					//
					SetFogEnable(fog_enable);
					SetFogMode(fog_mode);
					SetFogColor(fog_r, fog_g, fog_b);
					SetFogStartEnd(fog_start, fog_end);
					SetFogDensity(fog_density);
				}
			}
		}
		void			CharacterClass::DrawShadow(void) noexcept {
			if (this->m_IsActive) {
				this->GetObj().DrawModel();
			}
		}
		//
		void			CharacterClass::Dispose(void) noexcept {
			this->m_BackGround.reset();
			this->GetObj().Dispose();
			this->m_col.Dispose();
			this->m_move.vec.clear();
			EffectControl::Dispose();
		}
	};
};
