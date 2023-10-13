#include	"Character.hpp"


#include "../../MainScene/Player/Player.hpp"
#include "../../ObjectManager.hpp"
//#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		const bool		CharacterClass::CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept {
			if (!IsAlive()) { return false; }
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, m_move.pos) <= 2.0f*Scale_Rate)) { return false; }
			//被弾処理
			auto* HitPtr = HitBoxControl::GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(pAmmo->GetMyPlayerID()).GetChara();
				HitPoint Damage = pAmmo->GetDamage();
				switch (HitPtr->GetColType()) {
				case HitType::Head:
					Damage = Damage * 100 / 10;
					break;
				case HitType::Body:
					Damage = Damage;
					break;
				case HitType::Arm:
					Damage = Damage * 5 / 10;
					break;
				case HitType::Leg:
					Damage = Damage * 7 / 10;
					break;
				default:
					break;
				}
				Damage = std::min(Damage, GetHPMax());
				//
				pAmmo->Penetrate(Damage);
				//ダメージ計算
				{
					auto v1 = GetCharaDir().zvec() * -1.f;
					auto v2 = (Chara->GetMove().pos - this->m_move.pos).Norm(); v2.y(0);
					this->m_DamageEvent.resize(this->m_DamageEvent.size() + 1);
					this->m_DamageEvent.back().SetEvent(this->m_MyID, m_objType, Damage, atan2f(v1.cross(v2).y(), v1.dot(v2)));
				}
				//エフェクトセット
				{
					EffectControl::SetOnce(EffectResource::Effect::ef_hitblood, *pEndPos, VECTOR_ref::front(), 12.5f);
					EffectControl::SetEffectSpeed(EffectResource::Effect::ef_hitblood, 2.f);
				}
				//ヒットモーション
				{
					m_HitAxis = MATRIX_ref::Vtrans((*pEndPos - StartPos).Norm().cross(VECTOR_ref::up())*-1.f, GetFrameWorldMat(CharaFrame::Upper2).GetRot().Inverse());
					m_HitPower = 1.f;
					if (HitPtr->GetColType() == HitType::Leg) {
						KeyControl::SetIsSquat(true);
					}
				}
				//todo : ヒットした部分に近い頂点を赤くする
				return true;
			}
			return false;
		}
		void			CharacterClass::move_RightArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept {
			ResetFrameLocalMat(CharaFrame::RightArm);
			ResetFrameLocalMat(CharaFrame::RightArm2);
			ResetFrameLocalMat(CharaFrame::RightWrist);
			auto matBase = GetParentFrameWorldMat(CharaFrame::RightArm).GetRot().Inverse();

			VECTOR_ref Gunxvec = Gunzvec.cross(Gunyvec*-1.f)*-1.f;

			VECTOR_ref Localyvec = VECTOR_ref::vget(0.f, 0.f, -1.f).Norm();
			VECTOR_ref Localzvec = VECTOR_ref::vget(-1.f, -1.f, 0.f).Norm();

			VECTOR_ref RetPos = GunPos;
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
			{
				auto zvec = MATRIX_ref::Vtrans(Localzvec, GetFrameWorldMat(CharaFrame::RightWrist).GetRot());
				mat1 = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(zvec, matBase), MATRIX_ref::Vtrans(Gunzvec, matBase)) * mat1;
				SetFrameLocalMat(CharaFrame::RightWrist, mat1);
				auto xvec = MATRIX_ref::Vtrans(Localyvec, GetFrameWorldMat(CharaFrame::RightWrist).GetRot());
				mat1 = MATRIX_ref::RotAxis(Localzvec, AngleOf2Vector(xvec, Gunxvec)*(((Gunyvec*-1.f).dot(xvec) > 0.f) ? -1.f : 1.f))*mat1;
			}
			MATRIX_ref RetMat = mat1;
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

			VECTOR_ref RetPos = GunPos;
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
			{
				auto zvec = MATRIX_ref::Vtrans(Localzvec, GetFrameWorldMat(CharaFrame::LeftWrist).GetRot());
				mat1 = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(zvec, matBase), MATRIX_ref::Vtrans(Gunzvec, matBase)) * mat1;
				SetFrameLocalMat(CharaFrame::LeftWrist, mat1);
				auto xvec = MATRIX_ref::Vtrans(Localyvec, GetFrameWorldMat(CharaFrame::LeftWrist).GetRot());
				mat1 = MATRIX_ref::RotAxis(Localzvec, AngleOf2Vector(xvec, Gunxvec)*((Gunyvec.dot(xvec) > 0.f) ? -1.f : 1.f))*mat1;
			}
			MATRIX_ref RetMat = mat1;
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

			for (auto& a : this->m_Arm) {
				a.Init(false);
			}

			m_IsStuckLeftHandTimer = 0.f;
			m_IsStuckLeftHand = false;
			m_StuckLeftHand.Init(false);

			SetReady();

			this->m_MagArm.Init(false);
			this->m_MagHand = false;

			this->m_Speed = 0.f;
			this->m_RunPer2 = 0.f;
			this->m_PrevRunPer2 = 0.f;
			this->m_TurnBody = false;
			this->m_MoveEyePosTimer = 0.f;
			this->m_RunReady = false;
			this->m_Running = false;

			this->m_Press_Shot = false;
			this->m_Press_Reload = false;
			this->m_Press_Aim = false;
			this->m_Press_Check = false;
			this->m_Press_Watch = false;
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
			this->m_SquatPer = 0.f;
			this->m_RunPer = 0.f;
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			auto prev = KeyControl::GetRad();

			KeyControl::InputKey(pInput, pReady, StaminaControl::GetHeartRandVec(0.f));
			//AIM
			if (GetGunPtrNow() != nullptr) {
				this->m_Press_Shot = KeyControl::GetShotKey().press();
				this->m_Press_Reload = (KeyControl::GetRKey().press() && (GetAmmoNum() <= GetAmmoAll()));
				this->m_Press_Check = (KeyControl::GetRKey().press() && !(GetAmmoNum() <= GetAmmoAll()));
				this->m_Press_Watch = KeyControl::GetAction();
				this->m_Press_Aim = KeyControl::GetADSKey().press();


				if (!GetGunPtrNow()->GetCanShot()) {
					if (GetAmmoNum() == 0) {
						if (this->m_Press_Shot) {
							this->m_Press_Shot = false;
							this->m_Press_Reload = true;
						}
					}
					switch (GetGunPtrNow()->GetShotType()) {
					case SHOTTYPE::FULL:
						this->m_Press_Shot = true;
						break;
					case SHOTTYPE::SEMI:
						if (GetAmmoNum() != 0) {
							this->m_Press_Shot = true;
						}
						break;
					case SHOTTYPE::BOLT:
						this->m_Press_Shot = false;
						this->m_Press_Aim = false;
						break;
					default:
						break;
					}
					this->m_Press_Check = false;
					this->m_Press_Watch = false;
				}
			}
			else {
				this->m_Press_Shot = false;
				this->m_Press_Reload = false;
				this->m_Press_Check = false;
				this->m_Press_Watch = false;
				this->m_Press_Aim = false;
			}
			//
			this->m_RunReady = (KeyControl::GetRun() && GetShootReady());
		}
		//以前の状態保持														//
		void			CharacterClass::ExecuteSavePrev(void) noexcept {
			this->m_PrevRunPer2 = this->m_RunPer2;
			m_SendCamShake = false;
		}
		//操作																	//0.01ms
		void			CharacterClass::ExecuteInput(void) noexcept {
			//
			this->m_ReadyTimer = std::clamp(this->m_ReadyTimer + 1.f / FPS, 0.f, UpperTimerLimit);
			if ((this->m_Press_Shot || this->m_Press_Reload || this->m_Press_Check || this->m_Press_Watch) && (GetShotPhase() == GunAnimeID::Base)) {
				this->m_ReadyTimer = std::min(this->m_ReadyTimer, 0.1f);
			}
			//射撃
			if (GetGunPtrNow() != nullptr) {
				GetGunPtrNow()->SetShotSwitchOff();
				if ((GetShotPhase() == GunAnimeID::Base) && this->m_Arm[(int)EnumGunAnimType::Ready].Per() <= 0.1f) {
					if (this->m_Press_Shot) {
						GetGunPtrNow()->SetPressShot();
					}
					if (this->m_Press_Reload) {
						GetGunPtrNow()->SetReloadStart();
					}
					if (this->m_Press_Check) {
						GetGunPtrNow()->SetShotPhase(GunAnimeID::CheckStart);
					}

					if (this->m_Press_Watch) {
						GetGunPtrNow()->SetShotPhase(GunAnimeID::Watch);
					}
				}
				if (GetGunPtrNow()->GetShotSwitch()) {
					this->m_GunShakePer = 1.f;
				}
				//
				if (m_Press_Aim) {
					SetADS();
				}
				if (KeyControl::GetRun()) {
					SetReady();
				}
				if (
					GetGunPtrNow_Const()->GetCocking() ||
					GetGunPtrNow_Const()->GetReloading() ||
					GetGunPtrNow_Const()->GetChecking() ||
					GetGunPtrNow_Const()->GetWatching()) {
					SetAim();
				}
			}
			else {
				if (this->m_Speed > 0.6f || KeyControl::GetRun()) {
					SetReady();
				}
			}
			if (m_IsStuckGun) {
				SetReady();
			}
			//
			this->m_Arm[(int)EnumGunAnimType::ADS].Execute(this->GetIsADS(), 0.2f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Run].Execute(KeyControl::GetRun() && !GetIsAim(), 0.1f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::ReloadStart].Execute(GetShotPhase() == GunAnimeID::ReloadStart, 0.2f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::ReloadStart_Empty].Execute(GetShotPhase() == GunAnimeID::ReloadStart_Empty, 0.5f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Reload].Execute(GetShotPhase() >= GunAnimeID::ReloadOne && GetShotPhase() <= GunAnimeID::ReloadEnd, 0.1f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Ready].Execute(!(GetIsAim() || !GetShootReady()), 0.1f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Check].Execute(GetShotPhase() >= GunAnimeID::CheckStart && GetShotPhase() <= GunAnimeID::Checking, 0.05f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Watch].Execute(GetShotPhase() == GunAnimeID::Watch, 0.1f, 0.1f);


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
			KeyControl::SetRadBufZ(
				(abs((this->m_yrad_Bottom - OLDP)) > deg2rad(10)) ?
				0.f :
				std::clamp((this->m_yrad_Bottom - OLDP) * 3.f, -deg2rad(10), deg2rad(10))
			);
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

			GetObj().frame_Reset(GetFrame(CharaFrame::Upper));
			SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * tmpUpperMatrix);
			GetObj().frame_Reset(GetFrame(CharaFrame::Upper2));
			SetFrameLocalMat(CharaFrame::Upper2, GetFrameLocalMat(CharaFrame::Upper2).GetRot() * MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f)*
				MATRIX_ref::RotAxis(m_HitAxis, m_HitPowerR*deg2rad(90.f))
			);
			Easing(&this->m_HitPowerR, this->m_HitPower, 0.8f, EasingType::OutExpo);
			this->m_HitPower = std::max(this->m_HitPower - 1.f / FPS / 0.3f, 0.f);
		}
		//SetMat指示															//0.03ms
		void			CharacterClass::ExecuteAnim(void) noexcept {
			auto SE = SoundPool::Instance();
			//上半身演算
			{
				bool canreverse = true;
				if (!this->m_RunReady && !this->m_Running) {
					this->m_RunReady = true;
					canreverse = false;
				}
				this->m_Running = this->m_RunReady;
				if (GetGunPtrNow()) {
					GetGunPtrNow()->UpdateGunAnims(this->m_Press_Shot);
				}

				Easing(&this->m_ADSPer, this->GetIsADS() ? 1.f : 0.f, 0.8f, EasingType::OutExpo);//
			}
			//アニメ演算
			{
				//下半身
				this->m_BottomAnimSelect = GetBottomStandAnimSel();
				if (KeyControl::GetPressLeft()) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
				if (KeyControl::GetPressRight()) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
				if (KeyControl::GetPressRear()) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
				if (KeyControl::GetPressFront()) { this->m_BottomAnimSelect = KeyControl::GetRun() ? GetBottomRunAnimSel() : GetBottomWalkAnimSel(); }
				SetAnimLoop(GetBottomTurnAnimSel(), 0.5f);
				SetAnimLoop(GetBottomRunAnimSel(), 1.f * KeyControl::GetVecFront() * this->m_RunPer2);
				SetAnimLoop(GetBottomWalkAnimSel(), 1.15f * KeyControl::GetVecFront());
				SetAnimLoop(GetBottomLeftStepAnimSel(), 1.15f * KeyControl::GetVecLeft());
				SetAnimLoop(GetBottomWalkBackAnimSel(), 1.15f * KeyControl::GetVecRear());
				SetAnimLoop(GetBottomRightStepAnimSel(), 1.15f * KeyControl::GetVecRight());
				//
				Easing(&GetAnimeBuf(GetBottomTurnAnimSel()), (this->m_TurnBody) ? abs(KeyControl::GetRad().y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
			}
			//アニメセレクト
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
								if (!GetIsADS()) {
									m_SendCamShake = true;
								}
							}
						}
						//R
						if ((27.f < Time &&Time < 28.f)) {
							if (this->m_CharaSound != 3) {
								this->m_CharaSound = 3;
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 5.f);
								if (!GetIsADS()) {
									m_SendCamShake = true;
								}
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
								if (!GetIsADS()) {
									m_SendCamShake = true;
								}
							}
						}
						//R
						if (
							(10.f < Time &&Time < 11.f)
							) {
							if (this->m_CharaSound != 6) {
								this->m_CharaSound = 6;
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 15.f);
								if (!GetIsADS()) {
									m_SendCamShake = true;
								}
							}
						}
					}
				}
				if (this->m_SquatSoundReq != KeyControl::GetIsSquat()) {
					this->m_SquatSoundReq = KeyControl::GetIsSquat();
					SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale_Rate * 3.f);
				}
			}
			//移動の際の視点制御
			{
				if (this->m_Speed > 0.f) {
					this->m_MoveEyePosTimer += this->m_Speed * deg2rad(300.f + 200.f*this->m_RunPer) / FPS;
				}
				else {
					this->m_MoveEyePosTimer = 0.f;
				}
				Easing(&this->m_MoveEyePos,
					MATRIX_ref::Vtrans(
						VECTOR_ref::vget(sin(this->m_MoveEyePosTimer), -std::abs(cos(this->m_MoveEyePosTimer)), 0.f)*(0.25f*this->m_Speed) ,
						GetCharaDir()),
					0.9f, EasingType::OutExpo);
			}
			//
			GetObj().work_anime();
		}
		//SetMat指示更新														//0.03ms
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* AnimMngr = GunAnimManager::Instance();

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

			if(GetGunPtrNow()){
				//銃ひっこめ
				if (this->m_BackGround) {
					auto EndPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::Muzzle).pos();
					if (this->m_BackGround->CheckLinetoMap(GetFrameWorldMat(CharaFrame::Head).pos(), &EndPos, true)) {
						m_IsStuckGun = true;
					}
					else {
						if (m_IsStuckGun) {
							SetAim();
						}
						m_IsStuckGun = false;
					}
				}
				//エフェクト
				if (GetGunPtrNow()->GetShotSwitch()) {
					auto mat = GetGunPtrNow()->GetFrameWorldMat(GunFrame::Muzzle);
					switch (GetGunPtrNow()->GetGunShootSound()) {
					case GunShootSound::Normal:
						EffectControl::SetOnce(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 0.5f);
						EffectControl::SetEffectSpeed(EffectResource::Effect::ef_fire2, 2.f);
						break;
					case GunShootSound::Suppressor:
						EffectControl::SetOnce(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 0.25f);
						EffectControl::SetEffectSpeed(EffectResource::Effect::ef_fire2, 2.f);
						break;
					default:
						break;
					}
				}
			}
			std::vector<std::pair<MV1*, int>> cols;
			//cols.emplace_back(std::make_pair((MV1*)(&this->m_BackGround->GetGroundCol()), -1));
			if (this->m_BackGround) {
				auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);
				for (auto& C : MainGB->GetBuildDatas()) {
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

			this->m_move.mat = MATRIX_ref::RotZ(KeyControl::GetRad().z() / 2.f) * MATRIX_ref::RotY(this->m_yrad_Bottom);
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
							auto* Ptr = AnimMngr->GetAnimData(GetGunPtrNow()->GetGunAnimeSet((EnumGunAnimType)i));
							if (Ptr) {
								auto Now = AnimMngr->GetAnimNow(Ptr, GetGunPtrNow()->GetUpperAnim());
								MatT[i] = Now.GetRot() * MATRIX_ref::Mtrans(PosBase + MATRIX_ref::Vtrans(Now.GetPos(), GetCharaDir()));
							}
						}
						//
						VECTOR_ref yVect0, zVect0, Post0;

						zVect0 = MatT[(int)EnumGunAnimType::Aim].zvec();
						yVect0 = MatT[(int)EnumGunAnimType::Aim].yvec();
						Post0 = MatT[(int)EnumGunAnimType::Aim].pos();

						for (int i = 1; i < (int)EnumGunAnimType::Max; i++) {
							zVect0 = Lerp(zVect0, MatT[i].zvec(), this->m_Arm[i].Per());
							yVect0 = Lerp(yVect0, MatT[i].yvec(), this->m_Arm[i].Per());
							Post0 = Lerp(Post0, MatT[i].pos(), this->m_Arm[i].Per());
						}
						//

						zVec1 = zVect0;
						yVec1 = MATRIX_ref::Vtrans(yVect0, MATRIX_ref::RotAxis(zVect0, (KeyControl::GetRad().y() - this->m_yrad_Bottom)*0.15f + (this->m_LateLeanRad - this->m_LeanRad)));
						Pos1 = Post0 + m_GunShake_r;
						//銃振動
						m_GunShakePer = std::max(m_GunShakePer - 1.f / FPS / 0.1f, 0.f);
						Easing(&m_GunShake, VECTOR_ref::vget(GetRandf(1.f), 0.5f + GetRandf(1.f), GetRandf(1.f))*Scale_Rate, 0.7f, EasingType::OutExpo);
						Easing(&m_GunShake_r, m_GunShake*m_GunShakePer*0.1f, 0.9f, EasingType::OutExpo);
					}
					//
					Easing(&m_UpperRad, (KeyControl::GetRad() - this->m_UpperPrevRad)*-1.f, 0.9f, EasingType::OutExpo);
					m_UpperPrevRad = KeyControl::GetRad();
					//エイム先
					auto mat = MATRIX_ref::RotX(m_UpperRad.x()) * MATRIX_ref::RotY(m_UpperRad.y());
					Easing(&m_UpperyVecNormal, mat.yvec(), 0.8f, EasingType::OutExpo);
					Easing(&m_UpperzVecNormal, mat.zvec(), 0.8f, EasingType::OutExpo);
					Easing(&m_UpperyVec, m_UpperyVecNormal, 0.8f, EasingType::OutExpo);
					Easing(&m_UpperzVec, m_UpperzVecNormal, 0.8f, EasingType::OutExpo);
					if (IsAlive()) {
						auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::front(), zVec1);
						tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec1);
						tmp_gunrat *= MATRIX_ref::Axis1_YZ(m_UpperyVec.Norm(), m_UpperzVec.Norm());
						tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosO + Pos1);
						GetGunPtrNow()->SetGunMatrix(tmp_gunrat);
					}
					else {
						auto tmp_gunrat =
							lagframe_.RIGHThand_f.GetFrameWorldPosition().GetRot().Inverse() *
							m_RagDoll.GetFrameLocalWorldMatrix(lagframe_.RIGHThand_f.GetFrameID());
						GetGunPtrNow()->SetGunMatrix(tmp_gunrat);
					}
					{
						VECTOR_ref GunPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandPos).pos();
						VECTOR_ref Gunyvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandYvec).pos() - GunPos;
						VECTOR_ref Gunzvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandZvec).pos() - GunPos;
						VECTOR_ref HandPos;
						VECTOR_ref Handyvec;
						VECTOR_ref Handzvec;
						{
							HandPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
							Handyvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - HandPos;
							Handzvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - HandPos;

							VECTOR_ref MagPos = GetFrameWorldMat(CharaFrame::LeftMag).pos();
							VECTOR_ref Magyvec = GetFrameWorldMat(CharaFrame::LeftMagyvec).pos() - MagPos;
							VECTOR_ref Magzvec = GetFrameWorldMat(CharaFrame::LeftMagzvec).pos() - MagPos;

							if (GetShotPhase() == GunAnimeID::Watch) {
								m_MagHand = true;
							}
							switch (GetGunPtrNow()->GetReloadType()) {
							case RELOADTYPE::MAG:
								switch (GetShotPhase()) {
								case GunAnimeID::ReloadStart_Empty:
									if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::ReloadStart_Empty) > 0.5f) {
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
									if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::ReloadStart) > 0.8f) {
										m_MagHand = true;
									}
									break;
								case GunAnimeID::ReloadOne:
									if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::ReloadOne) < 0.6f) {
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
								switch (GetShotPhase()) {
								case GunAnimeID::ReloadStart_Empty:
									if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::ReloadStart_Empty) > 0.8f) {
										m_MagHand = true;
									}
									break;
								case GunAnimeID::ReloadStart:
									if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::ReloadStart) > 0.8f) {
										m_MagHand = true;
									}
									break;
								case GunAnimeID::ReloadOne:
									if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::ReloadOne) < 0.6f) {
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
							move_RightArm(GunPos, Gunyvec, Gunzvec);

							auto StartPos = GetFrameWorldMat(CharaFrame::Head).pos() + MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, -0.1f, -0.2f)*Scale_Rate, GetCharaDir());
							auto EndPos = StartPos + MATRIX_ref::Vtrans(VECTOR_ref::vget(0.5f, -0.1f, -0.7f)*Scale_Rate, GetCharaDir());
							VECTOR_ref Normal;

							switch (GetShotPhase()) {
							case GunAnimeID::Cocking:
							case GunAnimeID::ReloadStart_Empty:
							case GunAnimeID::ReloadStart:
							case GunAnimeID::ReloadOne:
							case GunAnimeID::ReloadEnd:
							case GunAnimeID::CheckStart:
							case GunAnimeID::Checking:
							case GunAnimeID::CheckEnd:
								m_IsStuckLeftHand = false;
								m_IsStuckLeftHandTimer = 0.f;
								break;
							default:
								if (!GetRun()) {
									if (this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, true,&Normal)) {
										if (m_IsStuckLeftHandTimer >= 0.5f) {
											if (!m_IsStuckLeftHand) {
												m_StuckLeftHandPos = EndPos;
												m_StuckLeftHandNormal = Normal;
												m_StuckLeftHandPos_R = EndPos;
												m_StuckLeftHandNormal_R = Normal;
											}
											else {
												if ((m_StuckLeftHandPos - EndPos).Length() > 0.3f*Scale_Rate) {
													m_StuckLeftHandPos = EndPos;
													m_StuckLeftHandNormal = Normal;
												}
												m_StuckLeftHandPos.y(EndPos.y());
											}
											m_IsStuckLeftHand = true;
										}
										m_IsStuckLeftHandTimer = std::min(m_IsStuckLeftHandTimer + 1.f / FPS, 0.5f);
									}
									else {
										m_IsStuckLeftHand = false;
										m_IsStuckLeftHandTimer = 0.f;
									}
								}
								break;
							}
							Easing(&m_StuckLeftHandPos_R, m_StuckLeftHandPos, 0.9f, EasingType::OutExpo);
							Easing(&m_StuckLeftHandNormal_R, m_StuckLeftHandNormal, 0.9f, EasingType::OutExpo);


							m_StuckLeftHand.Execute(m_IsStuckLeftHand, 0.2f, 0.2f);
							move_LeftArm(
								Lerp(HandPos, m_StuckLeftHandPos_R, m_StuckLeftHand.Per()),
								Lerp(Handyvec, m_StuckLeftHandNormal_R, m_StuckLeftHand.Per()),
								Lerp(Handzvec, m_StuckLeftHandNormal_R.cross(GetEyeVecY()).Norm(), m_StuckLeftHand.Per()));
						}
						//LeftMag
						{
							MATRIX_ref Mat = GetFrameWorldMat(CharaFrame::LeftHandJoint);
							switch (GetShotPhase()) {
							case GunAnimeID::ReloadStart_Empty:
								m_AmmoHandR = 1.f;
								m_AmmoHand = 1.f;
								break;
							case GunAnimeID::ReloadStart:
								m_AmmoHandR = 1.f;
								m_AmmoHand = 1.f;
								break;
							case GunAnimeID::ReloadOne:
								if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::ReloadOne) < 0.86f) {
									m_AmmoHandR = 1.f;
									m_AmmoHand = 1.f;
								}

								if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::ReloadOne) > 0.96f) {
									m_AmmoHandR = 0.f;
									m_AmmoHand = 0.f;
								}
								break;
							case GunAnimeID::CheckStart:
								if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::CheckStart) > 0.8f) {
									m_AmmoHandR = 1.f;
								}
							case GunAnimeID::Checking:
								if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::Check) < 0.8f) {
									m_AmmoHandR = 1.f;
									m_AmmoHand = 1.f;
								}
								if (GetGunPtrNow()->GetTimePer(CharaGunAnimeID::Check) > 0.8f) {
									m_AmmoHandR = 0.f;
									m_AmmoHand = 0.f;
								}
								break;
							default:
								m_AmmoHandR = 0.f;
								Mat = GetGunPtrNow()->GetFrameWorldMat(GunFrame::Magpos);
								break;
							}
							GetGunPtrNow()->SetAmmoHandMatrix(Mat, m_AmmoHand);
							Easing(&m_AmmoHand, m_AmmoHandR, 0.9f, EasingType::OutExpo);
						}
					}
				}
			}
			//ラグドール
			{
				if (IsAlive()) {
					this->m_RagDollTimer = 0.f;
				}
				else {
					this->m_RagDollTimer = std::min(this->m_RagDollTimer + 1.f / FPS, 3.f);
				}
				if (this->m_RagDollTimer < 3.f) {
					MV1SetPrioritizePhysicsOverAnimFlag(this->m_RagDoll.get(), TRUE);
					HumanControl::Frame_Copy_Lag(this->GetObj(), &this->m_RagDoll);
					this->m_RagDoll.work_anime();
					if (this->m_RagDollTimer == 0.f) {
						this->m_RagDoll.PhysicsResetState();
					}
					else {
						this->m_RagDoll.PhysicsCalculation(1000.f / FPS);
					}
				}
			}
			//ヒットボックス
			HitBoxControl::UpdataHitBox(this, (this->GetCharaType() == CharaTypeID::Enemy) ? 2.f : 1.f);
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
				MV1::Load((this->m_FilePath + "model_Rag.mv1").c_str(), &this->m_RagDoll, DX_LOADMODEL_PHYSICS_REALTIME, -1.f);//身体ラグドール
				MV1::SetAnime(&this->m_RagDoll, GetObj());
				HumanControl::Set_Body(this->GetObj(), this->m_RagDoll);
			}
			{
				for (int i = 0; i < GetObj().get_anime().size(); i++) { GetAnimeBuf((CharaAnimeID)i) = 0.f; }
				GetAnime(GetBottomStandAnimSel()).per = 1.f;
				//
				HitBoxControl::InitHitBox();
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
					if (IsAlive()) {
						for (int i = 0; i < this->GetObj().mesh_num(); i++) {
							if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
					}
					else {
						for (int i = 0; i < this->m_RagDoll.mesh_num(); i++) {
							if ((MV1GetMeshSemiTransState(this->m_RagDoll.get(), i) == TRUE) == isDrawSemiTrans) {
								this->m_RagDoll.DrawMesh(i);
							}
						}
					}
					//hitbox描画
					//HitBoxControl::DrawHitBox();
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
			ObjectBaseClass::Dispose();
			EffectControl::Dispose();
			this->m_BackGround.reset();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DelObj((SharedObj*)&m_Gun_Ptr);
			m_Gun_Ptr.reset();
		}
	};
};
