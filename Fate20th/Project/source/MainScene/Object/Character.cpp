#pragma once
#include	"../../Header.hpp"


#include <math.h>

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			CharacterClass::move_RightArm(const VECTOR_ref& WeaponPos, const VECTOR_ref& Weaponyvec, const VECTOR_ref& Weaponzvec, const VECTOR_ref& Armyvec) noexcept {
			ResetFrameLocalMat(CharaFrame::RightArm);
			ResetFrameLocalMat(CharaFrame::RightArm2);
			ResetFrameLocalMat(CharaFrame::RightWrist);
			auto matBase = GetParentFrameWorldMat(CharaFrame::RightArm).GetRot().Inverse();

			VECTOR_ref Weaponxvec = Weaponzvec.cross(Weaponyvec*-1.f)*-1.f;

			VECTOR_ref Localyvec = VECTOR_ref::vget(0.f, 0.f, -1.f).Norm();
			VECTOR_ref Localzvec = VECTOR_ref::vget(-0.3f, -1.f, 0.f).Norm();

			VECTOR_ref RetPos = Lerp(WeaponPos, GetFrameWorldMat(CharaFrame::RightWrist).pos(), m_LeftHandPer);
			//基準
			auto vec_a1 = MATRIX_ref::Vtrans((RetPos - GetFrameWorldMat(CharaFrame::RightArm).pos()).Norm(), matBase);//基準

			auto vec_a1L1 = Armyvec * 3.f;
			vec_a1L1.z(vec_a1.y() / -abs(vec_a1.z()));
			vec_a1L1 = vec_a1L1.Norm();

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
				mat1 = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(zvec, matBase), MATRIX_ref::Vtrans(Weaponzvec, matBase)) * mat1;
				SetFrameLocalMat(CharaFrame::RightWrist, mat1);
				auto xvec = MATRIX_ref::Vtrans(Localyvec, GetFrameWorldMat(CharaFrame::RightWrist).GetRot());
				mat1 = MATRIX_ref::RotAxis(Localzvec, AngleOf2Vector(xvec, Weaponxvec)*(((Weaponyvec*-1.f).dot(xvec) > 0.f) ? -1.f : 1.f))*mat1;
			}
			MATRIX_ref RetMat = Lerp_Matrix(mat1, mat2, m_LeftHandPer);
			SetFrameLocalMat(CharaFrame::RightWrist, RetMat);
		}
		void			CharacterClass::move_LeftArm(const VECTOR_ref& WeaponPos, const VECTOR_ref& Weaponyvec, const VECTOR_ref& Weaponzvec, const VECTOR_ref& Armyvec) noexcept {
			ResetFrameLocalMat(CharaFrame::LeftArm);
			ResetFrameLocalMat(CharaFrame::LeftArm2);
			ResetFrameLocalMat(CharaFrame::LeftWrist);
			auto matBase = GetParentFrameWorldMat(CharaFrame::LeftArm).GetRot().Inverse();

			VECTOR_ref Weaponxvec = Weaponzvec.cross(Weaponyvec)*-1.f;

			VECTOR_ref Localyvec = VECTOR_ref::vget(0.f, 0.f, -1.f).Norm();
			VECTOR_ref Localzvec = VECTOR_ref::vget(0.3f, -1.f, 0.f).Norm();

			VECTOR_ref RetPos = Lerp(WeaponPos, GetFrameWorldMat(CharaFrame::LeftWrist).pos(), m_LeftHandPer);
			//基準
			auto vec_a1 = MATRIX_ref::Vtrans((RetPos - GetFrameWorldMat(CharaFrame::LeftArm).pos()).Norm(), matBase);//基準

			auto vec_a1L1 = Armyvec * 3.f;
			vec_a1L1.z(vec_a1.y() / -abs(vec_a1.z()));
			vec_a1L1 = vec_a1L1.Norm();

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
				mat1 = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(zvec, matBase), MATRIX_ref::Vtrans(Weaponzvec, matBase)) * mat1;
				SetFrameLocalMat(CharaFrame::LeftWrist, mat1);
				auto xvec = MATRIX_ref::Vtrans(Localyvec, GetFrameWorldMat(CharaFrame::LeftWrist).GetRot());
				mat1 = MATRIX_ref::RotAxis(Localzvec, AngleOf2Vector(xvec, Weaponxvec)*((Weaponyvec.dot(xvec) > 0.f) ? -1.f : 1.f))*mat1;
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

			this->m_Speed = 0.f;
			this->m_RunPer2 = 0.f;
			this->m_PrevRunPer2 = 0.f;
			this->m_NeckPer = 0.f;
			this->m_LeftHandPer = 0.f;
			this->m_TurnBody = false;
			this->m_RunReady = false;
			this->m_Running = false;

			this->m_Press_Shot = false;
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
			this->m_UpperMatrix =
				MATRIX_ref::RotZ(0.f) *
				MATRIX_ref::RotX(KeyControl::GetRad().x()) *
				MATRIX_ref::RotY(KeyControl::GetRad().y() - this->m_yrad_Bottom);
			this->m_move.vec.clear();
			SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), m_PosBuf);
			m_MoveVecRec = this->m_move.mat.zvec()*-1.f;

			this->m_DamageSwitch = 0;
			this->m_DamageSwitchRec = this->m_DamageSwitch;

			this->m_Fov = deg2rad(70);
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			auto prev = KeyControl::GetRad();

			KeyControl::InputKey(pInput, pReady, StaminaControl::GetHeartRandVec(KeyControl::GetSquatPer()), StaminaControl::GetCannotRun(), false, true, (m_Accel.size() > 0.03f) && m_Slash);
			//AIM
			if (GetWeaponPtrNow() != nullptr) {
				this->m_Press_Shot = pInput.GetAction5() && KeyControl::GetKeyActive();
			}
			else {
				this->m_Press_Shot = false;
			}
			this->m_RunReady = KeyControl::GetIsRun();
		}
		void			CharacterClass::SetEyeVec(const VECTOR_ref& camvec) noexcept {
			Easing(&m_EyeVecR, camvec, 0.8f, EasingType::OutExpo);

			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Head));
			auto v1 = (GetFrameWorldMat(CharaFrame::Head).GetRot() * GetCharaDir().Inverse()).zvec()*-1.f;
			auto v2 = Lerp(MATRIX_ref::Vtrans(m_EyeVecR.Norm(), GetCharaDir().Inverse()), v1, m_NeckPer);

			auto radlimit = deg2rad(70);
			if (v1.dot(v2) <= cos(radlimit)) {
				v2 = v1 * cos(radlimit) + v1.cross(v1.cross(v2)) * (-sin(radlimit));
			}
			SetFrameLocalMat(CharaFrame::Head, MATRIX_ref::RotX(deg2rad(-10))*MATRIX_ref::RotVec2(v1, v2) * GetFrameLocalMat(CharaFrame::Head).GetRot());
		}
		//以前の状態保持														//
		void			CharacterClass::ExecuteSavePrev(void) noexcept {
			this->m_PrevUpperAnimSel = this->m_UpperAnimSelect;
			this->m_PrevRunPer2 = this->m_RunPer2;
		}
		//操作																	//0.01ms
		void			CharacterClass::ExecuteInput(void) noexcept {
			//
			this->m_SendCamShake = false;
			if (GetWeaponPtrNow() != nullptr) {
				if (this->m_Press_Shot) {
					if (!m_Slash) {
						m_UpperAnim = 0.f;
						this->m_SendCamShake = true;
					}
					m_Slash = true;
				}
				GetWeaponPtrNow()->SetIsDrawEffect(m_Slash);
			}
			Easing(&m_SlashPer, m_Slash ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
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
			auto TmpRunPer = 0.85f;
			if (this->m_TurnBody || (this->m_Speed > 0.1f)) { Easing(&this->m_yrad_Upper, KeyControl::GetRad().y(), TmpRunPer, EasingType::OutExpo); }
			auto OLDP = this->m_yrad_Bottom;
			Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP, TmpRunPer, EasingType::OutExpo);
			KeyControl::SetRadBufZ((this->m_yrad_Bottom - OLDP) * 5.f*(this->m_RunPer2 / SpeedLimit));
			if (this->m_MyID == 0) {
				VECTOR_ref V = m_Accel; V.y(0.f);
				SetEyeVec(((V.size() > 0.01f) && m_Slash) ? V.Norm() : m_CamEyeVec);
			}
			{
				if (KeyControl::GetJump().trigger() && (m_Boost == 0.f)) {
					m_Boost = 0.5f;
				}
				auto OLD = m_Boost;
				m_Boost = std::max(m_Boost - 1.f / FPS, 0.f);
				m_BoostSwitch = (m_Boost == 0.f) && (m_Boost != OLD);
				if (m_BoostSwitch) {
					m_Accel = GetCharaDir().zvec()*-1.f  * 11.5f;
					m_Accel.y(3.f);
					m_BoostEnd = false;

					VECTOR_ref pos = this->GetFrameWorldMat(CharaFrame::Upper2).pos();

					pos += GetMatrix().zvec() * -1.f * Scale_Rate*1.f;

					EffectControl::SetOnce(EffectResource::Effect::ef_reco, pos, GetMatrix().zvec() * -1.f, 0.5f);
				}
			}
		}
		//上半身回転															//0.06ms
		void			CharacterClass::ExecuteUpperMatrix(void) noexcept {
			auto* AnimMngr = WeaponAnimManager::Instance();

			float yrad = Lerp(KeyControl::GetRad().y() - this->m_yrad_Bottom, 0.f, KeyControl::GetRunPer());

			MATRIX_ref MatRet;
			{
				std::array<MATRIX_ref, 3> MatBodyT;
				int MatSel = 0;
				//レディ
				{
					auto* Ptr = AnimMngr->GetAnimData(GetReadyWeaponAnimSel().at(std::clamp<int>(this->m_ReadyAnimeSel, 0, (int)(GetReadyWeaponAnimSel().size()) - 1)));
					if (Ptr) {
						auto Now = AnimMngr->GetAnimNow(Ptr, m_UpperAnim);
						auto Pos = Now.GetPos();
						Pos.x(Pos.x());
						MatBodyT[MatSel] = MATRIX_ref::RotY(deg2rad(Now.GetBodyRotate().x())) * MATRIX_ref::RotX(deg2rad(Now.GetBodyRotate().y())) * MATRIX_ref::RotZ(deg2rad(Now.GetBodyRotate().z()));
					}
					MatSel++;
				}
				//振り
				{
					auto* Ptr = AnimMngr->GetAnimData(GetAimWeaponAnimSel().at(std::clamp<int>(this->m_SlashAnimeSel, 0, (int)(GetAimWeaponAnimSel().size()) - 1)));
					if (Ptr) {
						auto Now = AnimMngr->GetAnimNow(Ptr, m_UpperAnim);
						auto Pos = Now.GetPos();
						Pos.x(Pos.x());
						MatBodyT[MatSel] = MATRIX_ref::RotY(deg2rad(Now.GetBodyRotate().x())) * MATRIX_ref::RotX(deg2rad(Now.GetBodyRotate().y())) * MATRIX_ref::RotZ(deg2rad(Now.GetBodyRotate().z()));
					}
					MatSel++;
				}
				//ラン
				{
					auto* Ptr = AnimMngr->GetAnimData(GetRunWeaponAnimSel());
					if (Ptr) {
						auto Now = AnimMngr->GetAnimNow(Ptr, m_UpperAnim);
						auto Pos = Now.GetPos();
						Pos.x(Pos.x());
						MatBodyT[MatSel] = MATRIX_ref::RotY(deg2rad(Now.GetBodyRotate().x())) * MATRIX_ref::RotX(deg2rad(Now.GetBodyRotate().y())) * MATRIX_ref::RotZ(deg2rad(Now.GetBodyRotate().z()));
					}
					MatSel++;
				}

				VECTOR_ref yVect0, zVect0;
				//身体
				zVect0 = MatBodyT[0].zvec();
				yVect0 = MatBodyT[0].yvec();

				zVect0 = Lerp(zVect0, MatBodyT[2].zvec(), KeyControl::GetRunPer());
				yVect0 = Lerp(yVect0, MatBodyT[2].yvec(), KeyControl::GetRunPer());

				zVect0 = Lerp(zVect0, MatBodyT[1].zvec(), m_SlashPer);
				yVect0 = Lerp(yVect0, MatBodyT[1].yvec(), m_SlashPer);

				MatRet = MATRIX_ref::Axis1_YZ(yVect0, zVect0);
			}


			this->m_UpperMatrix =
				MATRIX_ref::RotZ(0.f) *
				MATRIX_ref::RotX(KeyControl::GetRad().x()) *
				MATRIX_ref::RotY(yrad);

			auto tmpUpperMatrix =
				MatRet *
				MATRIX_ref::RotZ(0.f) *
				MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f) *
				MATRIX_ref::RotY(yrad);

			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Upper));
			SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * tmpUpperMatrix);
			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Upper2));
			SetFrameLocalMat(CharaFrame::Upper2, GetFrameLocalMat(CharaFrame::Upper2).GetRot() * MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f));
			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Neck));
			SetFrameLocalMat(CharaFrame::Neck, GetFrameLocalMat(CharaFrame::Neck).GetRot() * MatRet.Inverse());

			{
				VECTOR_ref V = this->m_move.vec; V.y(0.f);
				if (V.size() > 0.01f) {
					m_MoveVecRec = V.Norm();
				}
			}
			float spd = this->m_move.vec.size()*40.f;
			Easing(&m_BodyMoveSpd, std::clamp(spd, -20.f, 20.f), 0.9f, EasingType::OutExpo);
			spd = std::max(0.f, std::abs(spd) - 10.f) * (spd > 0.f ? 1.f : -1.f);
			Easing(&m_SkirtSpd, std::clamp(spd, -20.f, 20.f), 0.9f, EasingType::OutExpo);

			MATRIX_ref SkirtMove;
			SkirtMove = MATRIX_ref::RotAxis(VECTOR_ref::up().cross(m_MoveVecRec), deg2rad(m_BodyMoveSpd));
			SkirtMove = GetMatrix().GetRot() * SkirtMove * GetMatrix().GetRot().Inverse();

			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Bottom));
			SetFrameLocalMat(CharaFrame::Bottom, GetFrameLocalMat(CharaFrame::Bottom).GetRot() * SkirtMove);
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
					//首
					m_NeckPer = 0.f;
					//Easing(&m_NeckPer, m_Slash ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				}
				//下半身
				auto OLDBottom = this->m_BottomAnimSelect;
				{
					this->m_BottomAnimSelect = GetBottomStandAnimSel();
					if (KeyControl::GetPressLeftGround()) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
					if (KeyControl::GetPressRightGround()) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
					if (KeyControl::GetPressRearGround()) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
					if (KeyControl::GetPressFrontGround()) { this->m_BottomAnimSelect = KeyControl::GetIsRun() ? GetBottomRunAnimSel() : GetBottomWalkAnimSel(); }
					if (m_Boost > 0.f) {
						this->m_BottomAnimSelect = CharaAnimeID::Bottom_JumpReady;
					}
					if (m_Accel.size() > 0.03f && !m_Slash) {
						this->m_BottomAnimSelect = CharaAnimeID::Bottom_Jump;
					}
					if (m_BoostEnd) {
						this->m_BottomAnimSelect = CharaAnimeID::Bottom_JumpEnd;
					}
				}
				//
				SetAnimLoop(GetBottomTurnAnimSel(), 0.5f);
				if ((int)this->m_BottomAnimSelect == (int)CharaAnimeID::Bottom_JumpReady) {
					if (OLDBottom != this->m_BottomAnimSelect) {
						GetAnime((CharaAnimeID)this->m_BottomAnimSelect).time = 0.f;
					}
					SetAnimOnce((int)CharaAnimeID::Bottom_JumpReady, 1.5f);
				}
				if ((int)this->m_BottomAnimSelect == (int)CharaAnimeID::Bottom_JumpEnd) {
					if (OLDBottom != this->m_BottomAnimSelect) {
						GetAnime((CharaAnimeID)this->m_BottomAnimSelect).time = 0.f;
					}
					SetAnimOnce((int)CharaAnimeID::Bottom_JumpEnd, 0.5f);
					if (GetAnime(CharaAnimeID::Bottom_JumpEnd).TimeEnd()) {
						m_BoostEnd = false;
					}
				}
				SetAnimLoop(GetBottomRunAnimSel(), 2.f * KeyControl::GetVecFront() * (this->m_RunPer2 / (SpeedLimit  * this->m_RunSpeed / SpeedLimit)));
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
				//
				if (
					(int)this->m_BottomAnimSelect == (int)CharaAnimeID::Bottom_JumpReady ||
					(int)this->m_BottomAnimSelect == (int)CharaAnimeID::Bottom_Jump ||
					(int)this->m_BottomAnimSelect == (int)CharaAnimeID::Bottom_JumpEnd
					) {
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//下半身
						if (
							i == (int)CharaAnimeID::Bottom_Stand ||
							i == (int)CharaAnimeID::Bottom_Stand_Run ||
							i == (int)CharaAnimeID::Bottom_Stand_Walk ||
							i == (int)CharaAnimeID::Bottom_Stand_LeftStep ||
							i == (int)CharaAnimeID::Bottom_Stand_RightStep ||
							i == (int)CharaAnimeID::Bottom_Stand_WalkBack ||
							i == (int)CharaAnimeID::Bottom_Stand_Turn ||
							i == (int)CharaAnimeID::Bottom_JumpReady ||
							i == (int)CharaAnimeID::Bottom_Jump ||
							i == (int)CharaAnimeID::Bottom_JumpEnd
							)
						{
							GetAnimeBuf((CharaAnimeID)i) = 0.f;
						}
					}
					GetAnimeBuf(this->m_BottomAnimSelect) = 1.f;
				}
				//その他
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					//上半身
					for (auto& a : this->m_CharaAnimeSet) {
						if (
							i == (int)a.m_Down ||
							i == (int)a.m_Ready ||
							i == (int)a.m_Cocking
							)
						{
							GetAnimeBuf((CharaAnimeID)i) = 0.f;
							break;
						}
					}
					if (i == (int)CharaAnimeID::Upper_Ready) {
						GetAnimeBuf((CharaAnimeID)i) = 1.f - m_SlashPer;
					}
					if (i == (int)CharaAnimeID::Upper_Slash) {
						GetAnimeBuf((CharaAnimeID)i) = m_SlashPer;
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
						i == (int)CharaAnimeID::Bottom_Stand_Turn ||
						i == (int)CharaAnimeID::Bottom_JumpReady ||
						i == (int)CharaAnimeID::Bottom_Jump ||
						i == (int)CharaAnimeID::Bottom_JumpEnd
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
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale_Rate * 35.f);
							}
						}
						//R
						if ((27.f < Time &&Time < 28.f)) {
							if (this->m_CharaSound != 3) {
								this->m_CharaSound = 3;
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 35.f);
							}
						}
					}
					else {
						//L
						if (
							(15.f < Time &&Time < 16.f)
							) {
							if (this->m_CharaSound != 5) {
								this->m_CharaSound = 5;
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale_Rate * 55.f);
							}
						}
						//R
						if (
							(8.f < Time &&Time < 9.f)
							) {
							if (this->m_CharaSound != 6) {
								this->m_CharaSound = 6;
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 55.f);
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
			//auto SE = SoundPool::Instance();
			auto* AnimMngr = WeaponAnimManager::Instance();
			//auto* DrawParts = DXDraw::Instance();

			this->m_RunPer2 = Lerp(0.35f, SpeedLimit / 2.f * this->m_RunSpeed / SpeedLimit, KeyControl::GetRunPer());
			this->m_RunPer2 = Lerp(this->m_RunPer2, 0.15f, KeyControl::GetSquatPer());
			if (this->m_PrevRunPer2 == 0.f) {
				this->m_PrevRunPer2 = this->m_RunPer2;
			}

			auto OLDpos = this->m_PosBuf;
			VECTOR_ref vecBuf = MATRIX_ref::Vtrans(KeyControl::GetVec(), MATRIX_ref::RotY(this->m_yrad_Upper));
			if (m_Boost > 0.f) {
				vecBuf = VECTOR_ref::zero();
			}
			SetRadEasingPerY(0.8f);

			this->m_Speed = std::clamp(vecBuf.size(), 0.f, 1.f);

			VECTOR_ref Buf = this->m_move.vec;

			if (this->m_Speed > 0.1f) {
				Buf.x((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).x());
				Buf.z((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).z());
			}
			else {
				Buf.x(vecBuf.x());
				Buf.z(vecBuf.z());
			}
			{
				auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(this->m_PosBuf + VECTOR_ref::up() * -1.f, m_PosBuf + VECTOR_ref::up() * 20.f);
				if (HitResult.HitFlag == TRUE) {
					auto yPos = this->m_PosBuf.y();
					Easing(&yPos, HitResult.HitPosition.y, 0.8f, EasingType::OutExpo);
					this->m_PosBuf.y(yPos);
					Buf.y(0.f);

					bool IsBoostRun = (this->m_BottomAnimSelect == CharaAnimeID::Bottom_Jump) && (m_Accel.size() < 1.f);
					if (IsBoostRun) {
						m_BoostEnd = true;
					}
				}
				else {
					Buf.yadd(M_GR / (FPS * FPS) * 3.f);
				}
				Easing(&this->m_move.vec, Buf + m_Accel * 60.f / FPS, 0.7f, EasingType::OutExpo);
				m_Accel.y(0.f);
				if (HitResult.HitFlag == TRUE) {
					Easing(&m_Accel, VECTOR_ref::zero(), 0.6f, EasingType::OutExpo);//摩擦
				}
				else {
					if (m_Accel.size() >= 6.5f) {
						Easing(&m_Accel, m_Accel.Norm()*4.f, 0.3f, EasingType::OutExpo);//摩擦
					}
				}
				if (m_Accel.size() <= 0.03f) {
					m_Accel.clear();
				}
			}

			this->m_PosBuf += this->m_move.vec;

			std::vector<std::pair<MV1*, int>> cols;
			cols.emplace_back(std::make_pair((MV1*)(&this->m_BackGround->GetGroundCol()),-1));
			col_wall(OLDpos, &this->m_PosBuf, cols);

			if (this->m_PosBufOverRideFlag) {
				this->m_PosBufOverRideFlag = false;
				this->m_PosBuf = this->m_PosBufOverRide;
				this->m_move.vec = this->m_VecBufOverRide;
				KeyControl::SetRadBufXY(this->m_RadOverRide);
			}

			MATRIX_ref SkirtMove;
			
			SkirtMove = MATRIX_ref::RotAxis(VECTOR_ref::up().cross(m_MoveVecRec), deg2rad(m_SkirtSpd));

			this->m_move.mat = MATRIX_ref::RotZ(KeyControl::GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom) * SkirtMove;
			this->m_move.repos = this->m_move.pos;
			Easing(&this->m_move.pos, m_PosBuf, 0.9f, EasingType::OutExpo);
			UpdateMove();
			this->GetObj().SetMatrix(MATRIX_ref::RotY(this->m_yrad_RotateR) * this->m_move.mat * MATRIX_ref::Mtrans(this->m_move.pos));
			//銃座標指定(アニメアップデート含む)//0.19ms
			{
				auto PosO = GetFrameWorldMat(CharaFrame::Head).pos();
				VECTOR_ref yVec1, zVec1, Pos1;
				if (GetWeaponPtrNow() != nullptr) {
					std::array<MATRIX_ref, 3> MatT;
					int MatSel = 0;

					auto PosBase = GetFrameWorldMat(CharaFrame::Head).pos() - PosO;
					//持ち手探索
					{
						//アニメアップデート
						GetObj().work_anime();//0.35ms
						//レディ
						{
							auto* Ptr = AnimMngr->GetAnimData(GetReadyWeaponAnimSel().at(std::clamp<int>(this->m_ReadyAnimeSel, 0, (int)(GetReadyWeaponAnimSel().size()) - 1)));
							if (Ptr) {
								auto Now = AnimMngr->GetAnimNow(Ptr, m_UpperAnim);
								auto Pos = Now.GetPos();
								Pos.x(Pos.x());
								MatT[MatSel] = (MATRIX_ref::RotY(deg2rad(Now.GetRotate().x())) * MATRIX_ref::RotX(deg2rad(Now.GetRotate().y())) * MATRIX_ref::RotZ(deg2rad(Now.GetRotate().z()))) * MATRIX_ref::Mtrans(PosBase + MATRIX_ref::Vtrans(Pos, GetCharaDir())*Scale_Rate);
							}
							MatSel++;
						}
						//振り
						{
							auto* Ptr = AnimMngr->GetAnimData(GetAimWeaponAnimSel().at(std::clamp<int>(this->m_SlashAnimeSel, 0, (int)(GetAimWeaponAnimSel().size()) - 1)));
							if (Ptr) {
								auto Now = AnimMngr->GetAnimNow(Ptr, m_UpperAnim);
								auto Pos = Now.GetPos();
								Pos.x(Pos.x());
								MatT[MatSel] = (MATRIX_ref::RotY(deg2rad(Now.GetRotate().x())) * MATRIX_ref::RotX(deg2rad(Now.GetRotate().y())) * MATRIX_ref::RotZ(deg2rad(Now.GetRotate().z()))) * MATRIX_ref::Mtrans(PosBase + MATRIX_ref::Vtrans(Pos, GetCharaDir())*Scale_Rate);

								if (m_Slash) {
									auto Per = AnimMngr->GetAnimPer(Ptr, m_UpperAnim);
									if (0.25f <= Per && Per <= 0.35f) {
										m_Accel = GetCharaDir().zvec()*-1.f  * 5.f;
										m_Accel.y(0.f);
										if (this->m_SlashAnimeSel == 1) {
										//	m_yrad_Rotate = -deg2rad(360);
										}
									}
									if (Per >= 1.f) {
										this->m_SlashAnimeSel++;
										m_Slash = false;
										if (!m_Press_Shot || this->m_SlashAnimeSel == (int)(GetAimWeaponAnimSel().size())) {
											this->m_SlashAnimeSel = 0;
										}
										//this->m_SlashAnimeSel = 2;
										m_yrad_Rotate = deg2rad(0);
										m_yrad_RotateR = deg2rad(0);
									}
								}
								else {
									m_yrad_Rotate = deg2rad(0);
									m_yrad_RotateR = deg2rad(0);
								}
								Easing(&m_yrad_RotateR, m_yrad_Rotate, 0.9f, EasingType::OutExpo);
							}
							MatSel++;
						}
						//ラン
						{
							auto* Ptr = AnimMngr->GetAnimData(GetRunWeaponAnimSel());
							if (Ptr) {
								auto Now = AnimMngr->GetAnimNow(Ptr, m_UpperAnim);
								auto Pos = Now.GetPos();
								Pos.x(Pos.x());
								MatT[MatSel] = (MATRIX_ref::RotY(deg2rad(Now.GetRotate().x())) * MATRIX_ref::RotX(deg2rad(Now.GetRotate().y())) * MATRIX_ref::RotZ(deg2rad(Now.GetRotate().z()))) * MATRIX_ref::Mtrans(PosBase + MATRIX_ref::Vtrans(Pos, GetCharaDir())*Scale_Rate);
							}
							MatSel++;
						}

						VECTOR_ref yVect0, zVect0, Post0;
						//腕
						zVect0 = MatT[0].zvec();
						yVect0 = MatT[0].yvec();
						Post0 = MatT[0].pos();

						bool IsBoostRun = (this->m_BottomAnimSelect == CharaAnimeID::Bottom_JumpReady) ||
							(this->m_BottomAnimSelect == CharaAnimeID::Bottom_Jump) ||
							(this->m_BottomAnimSelect == CharaAnimeID::Bottom_JumpEnd)
							;

						zVect0 = Lerp(zVect0, MatT[2].zvec(), IsBoostRun ? 1.f : KeyControl::GetRunPer());
						yVect0 = Lerp(yVect0, MatT[2].yvec(), IsBoostRun ? 1.f : KeyControl::GetRunPer());
						Post0 = Lerp(Post0, MatT[2].pos(), IsBoostRun ? 1.f : KeyControl::GetRunPer());

						zVect0 = Lerp(zVect0, MatT[1].zvec(), m_SlashPer);
						yVect0 = Lerp(yVect0, MatT[1].yvec(), m_SlashPer);
						Post0 = Lerp(Post0, MatT[1].pos(), m_SlashPer);

						zVec1 = zVect0;
						yVec1 = yVect0;
						Pos1 = Post0;

						this->m_ReadyAnimeSel = 0;

						yVec1 = MATRIX_ref::Vtrans(yVec1, MATRIX_ref::RotAxis(zVec1, (KeyControl::GetRad().y() - this->m_yrad_Bottom)*0.15f));
					}

					m_UpperAnim += 60.f / FPS;

					Easing(&m_UpperRad, (KeyControl::GetRad() - this->m_UpperPrevRad)*-1.f, 0.9f, EasingType::OutExpo);
					m_UpperPrevRad = KeyControl::GetRad();
					//エイム先
					auto mat = MATRIX_ref::RotX(m_UpperRad.x()) * MATRIX_ref::RotY(m_UpperRad.y());
					auto zVecR = mat.zvec();
					auto yVecR = mat.yvec();

					Easing(&m_UpperyVecNormal, yVecR, 0.8f, EasingType::OutExpo);
					Easing(&m_UpperzVecNormal, zVecR, 0.8f, EasingType::OutExpo);
					//結果
					Easing(&m_UpperyVec, m_UpperyVecNormal, 0.8f, EasingType::OutExpo);
					Easing(&m_UpperzVec, m_UpperzVecNormal, 0.8f, EasingType::OutExpo);
				}
				for (auto& p : this->m_Weapon_Ptr) {
					if (p != nullptr) {
						int index = (int)(&p - &this->m_Weapon_Ptr.front());
						auto PosBuf = PosO + Pos1;
						
						auto tmp_Weaponrat = MATRIX_ref::RotVec2(VECTOR_ref::front(), zVec1);
						tmp_Weaponrat *= MATRIX_ref::RotVec2(tmp_Weaponrat.yvec(), yVec1);
						if (index == this->m_WeaponSelect) {
							tmp_Weaponrat *= MATRIX_ref::Axis1_YZ(m_UpperyVec.Norm(), m_UpperzVec.Norm());
						}
						tmp_Weaponrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosBuf);
						p->SetWeaponMatrix(tmp_Weaponrat, 0);
					}
				}
				//
				if (this->m_Weapon_Ptr[0] != nullptr) {
					SetFrameLocalMat(CharaFrame::Weapon, this->m_Weapon_Ptr[0]->GetFrameWorldMat(WeaponFrame::Move) * GetMatrix().Inverse());
				}

				if (GetWeaponPtrNow() != nullptr) {
					VECTOR_ref WeaponPos = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::RightHandPos).pos();
					VECTOR_ref Weaponyvec = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::RightHandYvec).pos() - WeaponPos;
					VECTOR_ref Weaponzvec = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::RightHandZvec).pos() - WeaponPos;
					VECTOR_ref HandPos;
					VECTOR_ref Handyvec;
					VECTOR_ref Handzvec;
					if (GetWeaponPtrNow()->GetIsFrameActive(WeaponFrame::LeftHandPos)) {
						HandPos = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::LeftHandPos).pos();
						Handyvec = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::LeftHandYvec).pos() - HandPos;
						Handzvec = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::LeftHandZvec).pos() - HandPos;
					}
					//腕座標指定
					{
						move_RightArm(WeaponPos, Weaponyvec, Weaponzvec, MATRIX_ref::RotZ(deg2rad(80)).yvec());
						move_LeftArm(HandPos, Handyvec, Handzvec, MATRIX_ref::RotZ(deg2rad(-80)).yvec());
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
		}
	};
};
