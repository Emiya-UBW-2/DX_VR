#include	"Character.hpp"


#include "../../MainScene/Player/Player.hpp"
#include "../../ObjectManager.hpp"
//#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			if (this->m_MyID == value.DamageID) {
				auto PrevLive = LifeControl::IsAlive();
				LifeControl::SubHP(value.Damage);
				LifeControl::SubAP(value.ArmerDamage);
				auto SE = SoundPool::Instance();
				if (LifeControl::IsAlive()) {
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						SE->Get((int)SoundEnum::Man_Hurt1 + GetRand(6 - 1)).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
					}
					else {
						//SE->Get((int)SoundEnum::Man_Hurt1 + GetRand(6 - 1)).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
						//
						if (!PrevLive) {
							m_ItemFallControl.at(0).Dispose();
							m_ItemFallControl.at(1).Dispose();
							m_ItemFallControl.at(0).Init(this->m_BackGround, "data/model/AmmoBox/", ItemType::AMMO);
							m_ItemFallControl.at(1).Init(this->m_BackGround, "data/model/Armer/", ItemType::ARMER);
						}
					}
				}
				else if (PrevLive) {
					SE->Get((int)SoundEnum::Man_Death1 + GetRand(8 - 1)).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
					if (value.ShotID == 0) {
						auto* PlayerMngr = PlayerManager::Instance();
						PlayerMngr->GetPlayer(value.ShotID).AddScore(100 + (((m_GunSelect==0) && (value.Damage >= 100)) ? 20 : 0));

						auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(value.ShotID).GetChara();
						if (m_AutoAim != -1) {

							Chara->AddULT((m_GunSelect == 1) ? 1 : 6);
						}
						else {
							Chara->AddULT((m_GunSelect == 1) ? 9 : 20);
						}
					}
					int Rand = GetRand(99);
					if (Rand < 30) {
						m_ItemFallControl.at(0).SetFall(GetMove().pos + VECTOR_ref::up()*(0.5f*Scale_Rate), VECTOR_ref::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / FPS);
					}
					else if (Rand < 60) {
						m_ItemFallControl.at(1).SetFall(GetMove().pos + VECTOR_ref::up()*(0.5f*Scale_Rate), VECTOR_ref::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / FPS);
					}
					else {
						m_ItemFallControl.at(0).SetFall(GetMove().pos + VECTOR_ref::up()*(0.5f*Scale_Rate), VECTOR_ref::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / FPS);
						m_ItemFallControl.at(1).SetFall(GetMove().pos + VECTOR_ref::up()*(0.5f*Scale_Rate), VECTOR_ref::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / FPS);
					}
				}
				this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
				return true;
			}
			return false;
		}

		const bool		CharacterClass::CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept {
			if (!LifeControl::IsAlive()) { return false; }
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, m_move.pos) <= 2.0f*Scale_Rate)) { return false; }
			//被弾処理
			auto* HitPtr = HitBoxControl::GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(pAmmo->GetShootedID()).GetChara();
				HitPoint Damage = pAmmo->GetDamage();
				ArmerPoint ArmerDamage = 0;
				switch (HitPtr->GetColType()) {
					case HitType::Head:
						if (m_MyID == 0) {
							Damage = Damage;
							ArmerDamage = 0;

							auto* SE = SoundPool::Instance();
							SE->Get((int)SoundEnum::Tinnitus).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
							m_HeadShotSwitch = true;
							m_HeadShotPer = 1.f;
						}
						else {
							Damage = Damage * 100 / 10;
							ArmerDamage = 0;
						}
						break;
					case HitType::Body:
						if (LifeControl::IsArmerActive()) {
							ArmerDamage = std::clamp((ArmerPoint)Damage, (ArmerPoint)0, LifeControl::GetAP());
							Damage -= (HitPoint)ArmerDamage;
						}
						else {
							ArmerDamage = 0;
							Damage = Damage;
						}
						break;
					case HitType::Arm:
						Damage = Damage * 5 / 10;
						ArmerDamage = 0;
						break;
					case HitType::Leg:
						Damage = Damage * 7 / 10;
						ArmerDamage = 0;
						break;
					default:
						break;
				}
				Damage = std::min(Damage, LifeControl::GetHPMax());
				ArmerDamage = std::min(ArmerDamage, LifeControl::GetAPMax());
				//
				pAmmo->Penetrate(Damage, ArmerDamage);
				//ダメージ計算
				{
					auto v1 = GetCharaDir().zvec() * -1.f;
					auto v2 = (Chara->GetMove().pos - this->m_move.pos).Norm(); v2.y(0);
					//atan2f(v1.cross(v2).y(), v1.dot(v2))
					LifeControl::SetSubHPEvent((PlayerID)pAmmo->GetShootedID(), this->m_MyID, Damage, ArmerDamage);
				}
				//SE
				if (pAmmo->GetShootedID() == 0) {
					auto SE = SoundPool::Instance();
					if (Damage > 0) {
						SE->Get((int)SoundEnum::Hit).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
					}
					else {
						SE->Get((int)SoundEnum::HitGuard).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f,128);
					}
				}
				if (m_MyID == 0 && m_IsMainGame) {
					auto SE = SoundPool::Instance();
					if (Damage > 0) {
						SE->Get((int)SoundEnum::HitMe).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
					}
					else {
						SE->Get((int)SoundEnum::HitGuard).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f, 255);
					}
				}
				//エフェクトセット
				if (Damage > 0) {
					EffectControl::SetOnce(EffectResource::Effect::ef_hitblood, *pEndPos, VECTOR_ref::front(), Scale_Rate);
					EffectControl::SetEffectSpeed(EffectResource::Effect::ef_hitblood, 2.f);
				}
				else {
					EffectControl::SetOnce(EffectResource::Effect::ef_gndsmoke, *pEndPos, (Chara->GetMove().pos - this->m_move.pos).Norm(), 0.25f * Scale_Rate);
				}
				//ヒットモーション
				{
					HitReactionControl::SetHit(MATRIX_ref::Vtrans((*pEndPos - StartPos).Norm().cross(VECTOR_ref::up())*-1.f, GetFrameWorldMat(CharaFrame::Upper2).GetRot().Inverse()));
					if (HitPtr->GetColType() == HitType::Leg) {
						KeyControl::SetIsSquat(true);
					}
					if (HitPtr->GetColType() == HitType::Arm && !m_ArmBreak) {
						m_ArmBreak = true;
					}
				}
				//todo : ヒットした部分に近い頂点を赤くする
				return true;
			}
			return false;
		}

		const bool		CharacterClass::CheckMeleeHit(PlayerID MeleeID, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept {
			if (!LifeControl::IsAlive()) { return false; }
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, m_move.pos) <= 2.0f*Scale_Rate)) { return false; }
			//被弾処理
			auto* HitPtr = HitBoxControl::GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(MeleeID).GetChara();
				HitPoint Damage = 60;
				Damage = std::min(Damage, LifeControl::GetHPMax());
				//ダメージ計算
				{
					auto v1 = GetCharaDir().zvec() * -1.f;
					auto v2 = (Chara->GetMove().pos - this->m_move.pos).Norm(); v2.y(0);
					//atan2f(v1.cross(v2).y(), v1.dot(v2))
					LifeControl::SetSubHPEvent(MeleeID, this->m_MyID, Damage, 0);
				}
				//SE
				if (MeleeID == 0) {
					auto SE = SoundPool::Instance();
					SE->Get((int)SoundEnum::Hit).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
				}
				//エフェクトセット
				{
					EffectControl::SetOnce(EffectResource::Effect::ef_hitblood, *pEndPos, VECTOR_ref::front(), Scale_Rate);
					EffectControl::SetEffectSpeed(EffectResource::Effect::ef_hitblood, 2.f);
				}
				//ヒットモーション
				{
					HitReactionControl::SetHit(MATRIX_ref::Vtrans((*pEndPos - StartPos).Norm().cross(VECTOR_ref::up())*-1.f, GetFrameWorldMat(CharaFrame::Upper2).GetRot().Inverse()));
					if (HitPtr->GetColType() == HitType::Leg) {
						KeyControl::SetIsSquat(true);
					}
					if (HitPtr->GetColType() == HitType::Arm && !m_ArmBreak) {
						m_ArmBreak = true;
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
		void			CharacterClass::ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos, PlayerID pID, int GunSel) noexcept {
			StaminaControl::InitStamina();
			LifeControl::InitLife();
			KeyControl::InitKey(pxRad, pyRad);
			ShapeControl::InitShape();
			StackLeftHandControl::InitStackLeftHand();
			ULTControl::InitULT();
			//
			this->m_MyID = pID;
			//
			for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
			for (auto& a : this->m_Arm) { a.Init(false); }
			this->m_Arm[(int)EnumGunAnimType::Aim].Init(true);
			//
			SetReady();

			this->m_MagArm.Init(false);
			this->m_MagHand = false;

			this->m_MoverPer = 0.f;
			this->m_TurnBody = false;
			this->m_MoveEyePosTimer = 0.f;

			this->m_Press_Shot = false;
			this->m_Press_Reload = false;
			this->m_Press_Aim = false;
			this->m_IsChanging = false;

			this->m_Wear_Morphine.Init(false);
			this->m_IsWearMorphine = false;
			this->m_CanWearMorphine = false;
			this->m_IsWearingMorphine = false;
			this->m_WearMorphine = false;
			if (this->m_Morphine_Ptr) {
				this->m_Morphine_Ptr->SetActive(false);
			}

			//m_ArmBreak = true;
			this->m_CanWearMorphine = true;

			this->m_Wear_Armer.Init(false);
			this->m_IsWearArmer = false;
			this->m_CanWearArmer = false;
			this->m_IsWearingArmer = false;
			this->m_WearArmer = false;
			if (this->m_Armer_Ptr) {
				this->m_Armer_Ptr->SetActive(false);
			}
			//サウンド
			this->m_CharaSound = -1;
			//動作にかかわる操作
			this->m_PosBuf = pPos;
			OverrideControl::InitOverride();
			//上記を反映するもの
			this->m_yrad_Upper = KeyControl::GetRad().y();
			this->m_yrad_Bottom = KeyControl::GetRad().y();
			this->m_yrad_BottomChange = 0.f;
			this->m_LateLeanRad = 0.f;
			this->m_LeanRad = 0.f;
			this->m_move.vec.clear();
			SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), m_PosBuf);
			//銃のIDセットアップ
			if (GetGunPtrNow()) {
				GetGunPtrNow()->SetPlayerID(this->m_MyID);
				MagStockControl::Init_MagStockControl(GetGunPtrNow()->GetAmmoNum(), GetGunPtrNow()->GetAmmoAll(), GetGunPtrNow()->GetMagUniqueID());
			}

			m_SlingZrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(50));
			m_SlingYrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(50));

			m_GunSelect = GunSel;
			this->m_SlingPer[0] = (m_GunSelect == 0) ? 0.f : 1.f;
			this->m_SlingPer[1] = 1.f - this->m_SlingPer[0];
			if (GunSel == 1) {
				m_ReadyAnim = 5.f;
				m_ReadyAnimPhase = 0;
			}
			else {
				m_ReadyAnim = -1.f;
			}
			m_HPRec = 0.f;
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			KeyControl::InputKey(pInput, pReady, StaminaControl::GetHeartRandVec(0.f));
			//AIM
			if (GetGunPtrNow()) {
				this->m_Press_Shot = KeyControl::GetInputControl().GetPADSPress(PADS::SHOT) && !m_IsChanging;
				this->m_Press_Reload = KeyControl::GetInputControl().GetPADSPress(PADS::RELOAD) && !m_IsChanging;
				this->m_Press_Aim = KeyControl::GetInputControl().GetPADSPress(PADS::AIM) && !m_IsChanging;
				if (!GetGunPtrNow()->GetCanShot()) {
					if (GetGunPtrNow()->GetIsMagEmpty()) {
						if (this->m_Press_Shot && !GetGunPtrNow()->GetShoting()) {
							this->m_Press_Reload = true;
						}
					}
					switch (GetGunPtrNow()->GetShotType()) {
						case SHOTTYPE::FULL:
							this->m_Press_Shot = true;
							break;
						case SHOTTYPE::SEMI:
							if (!GetGunPtrNow()->GetIsMagEmpty()) {
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
				}
				if (GetIsCheck()) {
					this->m_Press_Reload = false;
				}

				if (pReady) {
					if (!m_IsChanging) {
						if (!m_ULTActive) {
							if (m_ULTUp > 0.5f) {
								m_ULTUp -= 0.5f;
								ULTControl::AddULT(1);
							}
							m_ULTUp += 1.f / FPS;
							if (ULTControl::IsULTActive()) {
								if (KeyControl::GetULTKey().trigger()) {
									m_IsChanging = true;
									m_ULTActive = true;
									m_ULTUp = 0.f;
								}
							}
						}
						else {
							if (m_ULTUp > 0.25f) {
								m_ULTUp -= 0.25f;
								ULTControl::AddULT(-1);
							}
							m_ULTUp += 1.f / FPS;
							if (ULTControl::GetULT() == 0 || (GetIsAim() && KeyControl::GetULTKey().trigger())) {
								m_IsChanging = true;
								m_ULTActive = false;
								m_ULTUp = 0.f;
								ULTControl::AddULT(-20);
							}
						}
					}
					if (IsAlive()) {
						int per = 100 * LifeControl::GetHP() / LifeControl::GetHPMax();
						if (per < 35) {
							if (m_HPRec >= 0.f) {
								m_HPRec -= 2.f;
								Heal(2);
								if (m_MyID == 0) {
									auto SE = SoundPool::Instance();
									SE->Get((int)SoundEnum::Man_breathing).Play(0, DX_PLAYTYPE_BACK);
								}
							}
							m_HPRec += 1.f / FPS;
						}
						else {
							if (m_HPRec != 0.f) {
								if (m_MyID == 0) {
									auto SE = SoundPool::Instance();
									SE->Get((int)SoundEnum::Man_breathing).StopAll(0);
									SE->Get((int)SoundEnum::Man_breathend).Play(0, DX_PLAYTYPE_BACK);
								}
							}
							m_HPRec = 0.f;
						}
					}
				}
				if (m_IsChanging) {
					SetReady();
					if (this->m_Arm[(int)EnumGunAnimType::Ready].Per() >= 0.95f) {
						SetAim();
						if (m_ULTActive) {
							m_GunSelect = 1;
							GetGunPtrNow()->SetAmmo(GetGunPtrNow()->GetAmmoAll());
							GetGunPtrNow()->UnloadChamber();
							GetGunPtrNow()->CockByMag();
							GetGunPtrNow()->SetShotPhase(GunAnimeID::Base);
						}
						else {
							m_GunSelect = 0;
						}
						m_IsChanging = false;
					}
				}
			}
			else {
				this->m_Press_Shot = false;
				this->m_Press_Reload = false;
				this->m_Press_Aim = false;
			}
		}
		//操作
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//
			this->m_ReadyTimer = std::clamp(this->m_ReadyTimer + 1.f / FPS, 0.f, UpperTimerLimit);
			if ((
				this->m_Press_Shot ||
				this->m_Press_Reload ||
				((KeyControl::GetInputControl().GetPADSPress(PADS::RELOAD) && GetIsCheck()) && GetGunPtrNow()->GetCanShot()) ||
				(KeyControl::GetInputControl().GetPADSPress(PADS::MELEE) && GetGunPtrNow()->GetCanShot())
				) && (GetShotPhase() == GunAnimeID::Base) && !m_IsChanging) {
				this->m_ReadyTimer = std::min(this->m_ReadyTimer, 0.1f);
			}
			//アーマーを着る
			if (this->m_Armer_Ptr) {
				this->m_WearArmerSwitch = false;
				if (this->m_IsWearArmer) {
					if (!this->m_IsWearingArmer) {
						if (this->m_Arm[(int)EnumGunAnimType::Ready].Per() >= 0.95f) {
							this->m_IsWearingArmer = true;

							this->m_Armer_Ptr->SetActive(true);

							//this->m_Wear_Armer.Init(this->m_WearArmer);
							//this->m_WearArmer ^= 1;

							this->m_WearArmer = true;
							this->m_Wear_Armer.Init(false);

							auto SE = SoundPool::Instance();
							SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetEyePosition(), Scale_Rate * 10.f);
						}
					}
					else {
						this->m_Wear_Armer.Execute(this->m_WearArmer, 0.1f, 0.1f, 0.95f);
						if ((this->m_Wear_Armer.Per() < 0.f) || (1.f < this->m_Wear_Armer.Per())) {
							this->m_IsWearArmer = false;
							this->m_IsWearingArmer = false;

							this->m_Armer_Ptr->SetActive(false);
							SetAim();

							LifeControl::SetHealEvent(this->m_MyID, this->m_MyID, 0, LifeControl::GetAPMax());
							this->m_WearArmerSwitch = true;
						}
					}
				}
			}
			//Morphine
			if (this->m_Morphine_Ptr) {
				this->m_MorphineSwitch = false;
				if (!this->m_IsWearMorphine) {
					if (KeyControl::GetInputControl().GetPADSPress(PADS::THROW) && CanWearMorphine()) {
						this->m_IsWearMorphine = true;
						this->m_CanWearMorphine = false;
					}
				}
				else if (!this->m_IsWearingMorphine) {
					if (this->m_Arm[(int)EnumGunAnimType::Ready].Per() >= 0.95f) {
						this->m_IsWearingMorphine = true;

						this->m_Morphine_Ptr->SetActive(true);

						//this->m_Wear_Morphine.Init(this->m_WearMorphine);
						//this->m_WearMorphine ^= 1;

						this->m_WearMorphine = true;
						this->m_Wear_Morphine.Init(false);
						this->m_Wear_MorphineFrame = 0.f;
						this->m_Wear_MorphinePer = 0.f;

						auto SE = SoundPool::Instance();
						SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetEyePosition(), Scale_Rate * 10.f);


						SE->Get((int)SoundEnum::Stim).Play_3D(0, GetEyePosition(), Scale_Rate * 10.f);
					}
				}
				else {
					this->m_Wear_MorphineFrame += 1.f / FPS;
					this->m_Wear_MorphinePer += 1.f / FPS;

					this->m_Wear_Morphine.Execute((this->m_Wear_MorphineFrame < 1.5f), 0.1f, 0.1f, 0.8f);

					if (this->m_Wear_MorphineFrame > 2.f) {
						this->m_IsWearMorphine = false;
						this->m_IsWearingMorphine = false;

						this->m_Morphine_Ptr->SetActive(false);
						SetAim();

						//this->m_CanWearMorphine = true;
						this->m_MorphineSwitch = true;
					}
					if (this->m_Wear_MorphinePer >= 0.f) {
						this->m_Wear_MorphinePer -= 0.4f;
						Heal(20);
					}
				}
			}
			//射撃
			if (GetGunPtrNow()) {
				//リコイルの演算
				if (this->m_RecoilRadAdd.y() < 0.f) {
					Easing(&this->m_RecoilRadAdd, VECTOR_ref::vget(0.f, 0.01f, 0.f), GetGunPtrNow()->GetGunDataClass()->GetRecoilReturn(), EasingType::OutExpo);
				}
				else {
					Easing(&this->m_RecoilRadAdd, VECTOR_ref::zero(), 0.7f, EasingType::OutExpo);
				}
				if (m_ReadyAnim > 0.f) {
					m_ReadyAnim = std::max(m_ReadyAnim - 1.f / FPS, 0.f);
					switch (m_ReadyAnimPhase) {
						case 0:
							if (m_ReadyAnim < 4.f) {
								m_ReadyAnimPhase++;
							}
							break;
						case 1:
							{
								if (!GetGunPtrNow()->GetReloading()) {
									switch (GetGunPtrNow()->GetReloadType()) {
										case RELOADTYPE::MAG:
											{
												GetGunPtrNow()->SetAmmo(0);
												GetGunPtrNow()->SetReloadStartMag(GetGunPtrNow()->GetAmmoAll(), GetGunPtrNow()->GetMagUniqueID());
											}
											break;
										case RELOADTYPE::AMMO:
											{
												GetGunPtrNow()->SetReloadStartAmmo(GetGunPtrNow()->GetAmmoAll());
											}
											break;
										default:
											break;
									}
									DrawParts->SetCamShake(0.1f, 2.f);
								}
								if (GetGunPtrNow()->GetShotPhase() == GunAnimeID::ReloadEnd) {
									DrawParts->SetCamShake(0.1f, 2.f);
									m_ReadyAnimPhase++;
								}
							}
							break;
						case 2:
							{
								SetReady();
								if (this->m_Arm[(int)EnumGunAnimType::Ready].Per() >= 0.95f) {
									SetAim();
									m_GunSelect = 0;
									m_ReadyAnimPhase++;
									DrawParts->SetCamShake(0.1f, 2.f);
								}
							}
							break;
						case 3:
							{
								if (!GetGunPtrNow()->GetChecking() && (this->m_Arm[(int)EnumGunAnimType::Ready].Per() <= 0.1f)) {
									GetGunPtrNow()->SetShotPhase(GunAnimeID::CheckStart);
								}
								if (GetGunPtrNow()->GetShotPhase() == GunAnimeID::CheckEnd) {
									m_ReadyAnimPhase++;
								}
							}
							break;
						default:
							m_ReadyAnim = -1.f;
							break;
					}
				}

				GetGunPtrNow()->SetShotSwitchOff();
				if ((GetShotPhase() == GunAnimeID::Base) && this->m_Arm[(int)EnumGunAnimType::Ready].Per() <= 0.1f) {
					if (this->m_Press_Shot) {
						if (GetGunPtrNow()->GetInChamber()) {
							GetGunPtrNow()->SetBullet();
							if (m_GunSelect == 1) {
								ULTControl::AddULT(-3);
							}
							float Power = 0.0001f*GetGunPtrNow()->GetGunDataClass()->GetRecoilPower();
							if (GetIsSquat()) {
								Power *= 0.5f;
							}
							this->m_RecoilRadAdd.Set(GetRandf(Power / 4.f), -Power, 0.f);
							//this->m_RecoilRadAdd = MATRIX_ref::Vtrans(this->m_RecoilRadAdd, MATRIX_ref::RotZ(-m_LeanRad / 5.f));
							if (this->m_MyID == 0) {
								DrawParts->SetCamShake(0.1f, 0.3f);
							}
							//エフェクト
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
						else {
						//	GetGunPtrNow()->SetPressShotEmpty();
						}
						this->m_SoundPower = 1.f * (CanLookTarget ? 1.f : 0.5f);
					}
					if (this->m_Press_Reload) {
						switch (GetGunPtrNow()->GetReloadType()) {
							case RELOADTYPE::MAG:
								{
									if (m_GunSelect == 0) {
										int Num = GetGunPtrNow()->GetAmmoNum();
										int All = GetGunPtrNow()->GetAmmoAll();
										int UniqueID = GetGunPtrNow()->GetMagUniqueID();
										GetGunPtrNow()->SetReloadStartMag(MagStockControl::GetNowMag().AmmoNum, MagStockControl::GetNowMag().ModUniqueID);
										MagStockControl::SetOldMag(Num, All, UniqueID);
									}
									else {
										GetGunPtrNow()->SetReloadStartMag(GetGunPtrNow()->GetAmmoAll(), GetGunPtrNow()->GetMagUniqueID());
									}
								}
								break;
							case RELOADTYPE::AMMO:
								{
									if (m_GunSelect == 0) {
										//todo:弾残数に応じて変更
										GetGunPtrNow()->SetReloadStartAmmo(MagStockControl::GetNowMag().AmmoNum);
									}
									else {
										GetGunPtrNow()->SetReloadStartAmmo(GetGunPtrNow()->GetAmmoAll());
									}
								}
								break;
							default:
								break;
						}


						if (this->m_MyID != 0) {
							auto SE = SoundPool::Instance();
							if (GetRand(1) == 0) {
								SE->Get((int)SoundEnum::Man_reload).Play_3D(0, GetEyePosition(), Scale_Rate * 10.f);
							}
							else {
								SE->Get((int)SoundEnum::Man_takecover).Play_3D(0, GetEyePosition(), Scale_Rate * 10.f);
							}
							this->m_SoundPower = 0.6f * (CanLookTarget ? 1.f : 0.5f);
						}
					}
					{
						if ((KeyControl::GetInputControl().GetPADSPress(PADS::RELOAD) && GetIsCheck()) && GetGunPtrNow()->GetCanShot()) {
							GetGunPtrNow()->SetShotPhase(GunAnimeID::CheckStart);
						}
					}
					m_MeleeCoolDown = std::max(m_MeleeCoolDown - 1.f / FPS, 0.f);
					if (KeyControl::GetInputControl().GetPADSPress(PADS::MELEE) && m_MeleeCoolDown == 0.f) {
						GetGunPtrNow()->SetShotPhase(GunAnimeID::Melee);
						m_MeleeCoolDown = 1.f;
						if (this->m_MyID == 0) {
							DrawParts->SetCamShake(0.1f, 5.f);
						}
					}
					m_SightChange.Execute(KeyControl::GetInputControl().GetPADSPress(PADS::JUMP));
					if (m_SightChange.trigger()) {
						GetGunPtrNow()->ChangeSightSel();
					}
					//テスト系
				}
				if (KeyControl::GetInputControl().GetPADSPress(PADS::CHECK) && (GetShotPhase() == GunAnimeID::Base) && !this->m_IsWearArmer && !this->m_IsWearMorphine) {
					if (CanWearArmer()) {
						this->m_IsWearArmer = true;
						this->m_CanWearArmer = false;
					}
					else if (
						(m_GunSelect == 0) &&
						MagStockControl::GetNeedAmmoLoad(GetGunPtrNow()->GetIsMagFull(), GetGunPtrNow()->GetIsMagEmpty()) &&
						!GetGunPtrNow()->IsAmmoLoading()
						) {
						GetGunPtrNow()->SetShotPhase(GunAnimeID::AmmoLoadStart);
					}
					else if (GetGunPtrNow()->GetCanShot()) {
						GetGunPtrNow()->SetShotPhase(GunAnimeID::Watch);
					}
					else if (GetGunPtrNow()->GetShootReady()) {
						GetGunPtrNow()->SetShotPhase(GunAnimeID::Watch);//空
					}
				}

				if ((GetShotPhase() == GunAnimeID::ReloadOne) || (GetShotPhase() == GunAnimeID::AmmoLoading)) {
					if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
						GetGunPtrNow()->SetCancel();
					}
				}
				if (GetGunPtrNow()->GetAmmoLoadSwitch()) {
					int Select = GetGunPtrNow()->GetAmmoLoadCount() - 1 - 1;
					if (GetGunPtrNow()->GetAmmoLoadCount() == 1) {
						int InAmmo = std::clamp(MagStockControl::GetAmmoStock(), 0, GetGunPtrNow()->GetAmmoAll() - GetGunPtrNow()->GetAmmoNum());
						GetGunPtrNow()->SetAmmo(GetGunPtrNow()->GetAmmoNum() + InAmmo);
						MagStockControl::SubAmmoStock(InAmmo);
						for (int i = 2;i >= Select + 1;i--) {
							InAmmo = std::clamp(MagStockControl::GetMag(i), 0, GetGunPtrNow()->GetAmmoAll() - GetGunPtrNow()->GetAmmoNum());
							GetGunPtrNow()->SetAmmo(GetGunPtrNow()->GetAmmoNum() + InAmmo);
							MagStockControl::SetMag(i, MagStockControl::GetMag(i) - InAmmo);
						}
						if ((Select < 2) && (MagStockControl::GetMag(Select + 1) == 0) && (MagStockControl::GetAmmoStock() == 0)) {
							GetGunPtrNow()->SetCancel();
						}
					}
					else {
						int InAmmo = std::clamp(MagStockControl::GetAmmoStock(), 0, GetGunPtrNow()->GetAmmoAll() - MagStockControl::GetMag(Select));
						MagStockControl::SetMag(Select, MagStockControl::GetMag(Select) + InAmmo);
						MagStockControl::SubAmmoStock(InAmmo);
						for (int i = 2;i >= Select + 1;i--) {
							InAmmo = std::clamp(MagStockControl::GetMag(i), 0, GetGunPtrNow()->GetAmmoAll() - MagStockControl::GetMag(Select));
							MagStockControl::SetMag(Select, MagStockControl::GetMag(Select) + InAmmo);
							MagStockControl::SetMag(i, MagStockControl::GetMag(i) - InAmmo);
						}
						if ((Select < 2) && (MagStockControl::GetMag(Select + 1) == 0) && (MagStockControl::GetAmmoStock() == 0)) {
							GetGunPtrNow()->SetCancel();
						}
					}
				}
				//
				if (m_Press_Aim) {
					if (!m_IsStuckGun) {
						SetADS();
					}
					else {
						SetAim();
					}
				}
				if (KeyControl::GetRun() || this->m_IsWearArmer || this->m_IsWearMorphine || (GetShotPhase() == GunAnimeID::AmmoLoadStart || GetShotPhase() == GunAnimeID::AmmoLoadEnd)) {
					SetReady();
				}
				if (
					(
						GetGunPtrNow()->GetCocking() ||
						GetGunPtrNow()->GetReloading() ||
						GetGunPtrNow()->GetChecking() ||
						GetGunPtrNow()->GetWatching() ||
						GetGunPtrNow()->IsMelee() ||
						(GunAnimeID::AmmoLoading == GetShotPhase())
						) &&
					!m_IsChanging) {
					SetAim();
				}
			}
			else {
				if (this->m_MoverPer > 0.6f || KeyControl::GetRun()) {
					SetReady();
				}
			}
			//
			this->m_Arm[(int)EnumGunAnimType::ADS].Execute(this->GetIsADS(), 0.2f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::Run].Execute(KeyControl::GetRun() && !GetIsAim(), 0.1f, 0.2f, 0.95f);
			this->m_Arm[(int)EnumGunAnimType::ReloadStart_Empty].Execute(GetShotPhase() == GunAnimeID::ReloadStart_Empty, 0.5f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::ReloadStart].Execute(GetShotPhase() == GunAnimeID::ReloadStart, 0.2f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Reload].Execute(GetShotPhase() >= GunAnimeID::ReloadOne && GetShotPhase() <= GunAnimeID::ReloadEnd, 0.1f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::Ready].Execute(!GetIsAim() && (GetShootReady() || (GetShotPhase() == GunAnimeID::AmmoLoadStart || GetShotPhase() == GunAnimeID::AmmoLoadEnd)), 0.1f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::Check].Execute(GetShotPhase() >= GunAnimeID::CheckStart && GetShotPhase() <= GunAnimeID::Checking, 0.05f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Watch].Execute(GetShotPhase() == GunAnimeID::Watch, 0.1f, 0.1f);
			this->m_Arm[(int)EnumGunAnimType::Melee].Execute(GetShotPhase() == GunAnimeID::Melee, 1.1f, 0.1f);
			this->m_Arm[(int)EnumGunAnimType::AmmoLoad].Execute(GetShotPhase() == GunAnimeID::AmmoLoading, 0.1f, 0.1f);
			//
			if (this->m_MoverPer <= 0.1f) {
				if (deg2rad(50.f) < abs(this->m_yrad_UpperChange)) {
					this->m_TurnBody = true;
				}
				if (abs(this->m_yrad_UpperChange) < deg2rad(0.5f)) {
					this->m_TurnBody = false;
				}
			}
			else {
				this->m_TurnBody = false;
			}
			//
			if (KeyControl::GetRun()) {
				KeyControl::ResetLeanRate();
			}

			//
			if (this->m_TurnBody || (this->m_MoverPer > 0.1f)) { Easing(&this->m_yrad_Upper, KeyControl::GetRad().y(), 0.85f, EasingType::OutExpo); }
			auto YradChange = this->m_yrad_Bottom;
			Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - KeyControl::GetFrontP(), 0.85f, EasingType::OutExpo);
			YradChange = this->m_yrad_Bottom - YradChange;
			KeyControl::SetRadBuf().SetEasingZ((abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f);
			this->m_yrad_UpperChange = KeyControl::GetRad().y() - this->m_yrad_Upper;
			this->m_yrad_BottomChange = KeyControl::GetRad().y() - this->m_yrad_Bottom;
		}
		//上半身回転
		void			CharacterClass::ExecuteUpperMatrix(void) noexcept {
			Easing(&this->m_LateLeanRad, this->m_LeanRad, 0.9f, EasingType::OutExpo);
			Easing(&this->m_LeanRad, ((float)(-KeyControl::GetLeanRate())+this->m_HeadShotPer)*deg2rad(25), 0.9f, EasingType::OutExpo);
			Easing(&this->m_HeadShotPer, 0.f, 0.9f, EasingType::OutExpo);
			
			auto tmpUpperMatrix =
				MATRIX_ref::RotZ(this->m_LeanRad) *
				MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f) *
				MATRIX_ref::RotY(KeyControl::GetRun() ? 0.f : this->m_yrad_BottomChange);

			GetObj().frame_Reset(GetFrame(CharaFrame::Upper));
			SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * tmpUpperMatrix);
			GetObj().frame_Reset(GetFrame(CharaFrame::Upper2));
			SetFrameLocalMat(CharaFrame::Upper2, GetFrameLocalMat(CharaFrame::Upper2).GetRot() * MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f) * HitReactionControl::GetHitReactionMat());
			HitReactionControl::Execute_HitReactionControl();
		}
		//SetMat指示
		void			CharacterClass::ExecuteAnim(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			//上半身演算
			if (GetGunPtrNow()) {
				GetGunPtrNow()->UpdateGunAnims(this->m_Press_Shot);
			}
			Easing(&this->m_ADSPer, this->GetIsADS() ? 1.f : 0.f, 0.9f, EasingType::OutExpo);//
			//アニメ演算
			{
				//下半身
				this->m_BottomAnimSelect = GetBottomStandAnimSel();
				if (KeyControl::GetInputControl().GetPADSPress(PADS::MOVE_A)) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
				if (KeyControl::GetInputControl().GetPADSPress(PADS::MOVE_D)) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
				if (KeyControl::GetInputControl().GetPADSPress(PADS::MOVE_S)) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
				if (KeyControl::GetInputControl().GetPADSPress(PADS::MOVE_W)) { this->m_BottomAnimSelect = KeyControl::GetRun() ? GetBottomRunAnimSel() : GetBottomWalkAnimSel(); }
				SetAnimLoop(GetBottomTurnAnimSel(), 0.5f);
				SetAnimLoop(GetBottomRunAnimSel(), 1.f * KeyControl::GetVecFront() * GetSpeedPer());

				float Per = std::clamp(GetSpeedPer() / 0.65f, 0.5f, 1.f);
				SetAnimLoop(GetBottomWalkAnimSel(), 1.15f * KeyControl::GetVecFront() * Per);
				SetAnimLoop(GetBottomLeftStepAnimSel(), 1.15f * KeyControl::GetVecLeft() * Per);
				SetAnimLoop(GetBottomWalkBackAnimSel(), 1.15f * KeyControl::GetVecRear() * Per);
				SetAnimLoop(GetBottomRightStepAnimSel(), 1.15f * KeyControl::GetVecRight() * Per);
				//
				Easing(&GetAnimeBuf(GetBottomTurnAnimSel()), (this->m_TurnBody) ? abs(this->m_yrad_UpperChange) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
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
					i == (int)CharaAnimeID::Bottom_Stand_RLern) {
					GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_BottomAnimSelect) ? 1.f : -1.f)*2.f / FPS;
				}
			}
			FlipAnime();
			//足音
			if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
				auto Time = GetAnime(this->m_BottomAnimSelect).time;
				if (this->m_BottomAnimSelect != GetBottomRunAnimSel()) {
					//L
					if ((9.f < Time && Time < 10.f)) {
						if (this->m_CharaSound != 1) {
							this->m_CharaSound = 1;
							this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::LeftFoot).pos()).Length() < Scale_Rate * 5.f*1.5f) {

								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale_Rate * 5.f);
								if (!GetIsADS()) {
									if (this->m_MyID == 0) {
										DrawParts->SetCamShake(0.1f, 1.f);
									}
								}
							}
						}
					}
					//R
					if ((27.f < Time &&Time < 28.f)) {
						if (this->m_CharaSound != 3) {
							this->m_CharaSound = 3;
							this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::RightFoot).pos()).Length() < Scale_Rate * 5.f*1.5f) {

								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 5.f);
								if (!GetIsADS()) {
									if (this->m_MyID == 0) {
										DrawParts->SetCamShake(0.1f, 1.f);
									}
								}
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
							this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::LeftFoot).pos()).Length() < Scale_Rate * 5.f*1.5f) {
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale_Rate * 15.f);
								if (!GetIsADS()) {
									if (this->m_MyID == 0) {
										DrawParts->SetCamShake(0.1f, 1.f);
									}
								}
							}
						}
					}
					//R
					if (
						(10.f < Time &&Time < 11.f)
						) {
						if (this->m_CharaSound != 6) {
							this->m_CharaSound = 6;
							this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::RightFoot).pos()).Length() < Scale_Rate * 5.f*1.5f) {
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), Scale_Rate * 15.f);
								if (!GetIsADS()) {
									if (this->m_MyID == 0) {
										DrawParts->SetCamShake(0.1f, 1.f);
									}
								}
							}
						}
					}
				}
			}
			//しゃがみ音
			if (this->m_SquatSoundReq != KeyControl::GetIsSquat()) {
				this->m_SquatSoundReq = KeyControl::GetIsSquat();
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale_Rate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//リーン音
			if (this->m_LeanSoundReq != KeyControl::GetLeanRate()) {
				this->m_LeanSoundReq = KeyControl::GetLeanRate();
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale_Rate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//心拍音
			if (this->m_MyID == 0) {
				if (StaminaControl::ExcuteStamina(0.f, this->m_Speed / FPS, KeyControl::GetIsSquat())) {
					SE->Get((int)SoundEnum::Heart).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 0.5f);
				}
			}
			//移動の際の視点制御
			{
				if (this->m_MoverPer > 0.f) {
					this->m_MoveEyePosTimer += this->m_MoverPer * deg2rad(KeyControl::GetRun() ? 8.f : 5.f)*60.f / FPS;
				}
				else {
					this->m_MoveEyePosTimer = 0.f;
				}
				auto EyePos = MATRIX_ref::Vtrans(VECTOR_ref::up()*(0.25f*this->m_MoverPer), MATRIX_ref::RotZ(this->m_MoveEyePosTimer));
				EyePos.y(-std::abs(EyePos.y()));
				Easing(&this->m_MoveEyePos, MATRIX_ref::Vtrans(EyePos, GetCharaDir()), 0.9f, EasingType::OutExpo);
			}
			//
			GetObj().work_anime();
		}
		//SetMat指示更新
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();

			auto OLDpos = this->m_PosBuf;
			float Yadd = this->m_move.vec.y();
			VECTOR_ref vecBuf = KeyControl::GetVec();
			this->m_MoverPer = std::clamp(vecBuf.Length(), 0.f, 1.f);
			if (this->m_MoverPer > 0.f) {
				vecBuf = MATRIX_ref::Vtrans(vecBuf.Norm(), MATRIX_ref::RotY(this->m_yrad_Upper)) * (GetSpeedPer() * 60.f / FPS);
			}
			else {
				vecBuf = MATRIX_ref::Vtrans(vecBuf, MATRIX_ref::RotY(this->m_yrad_Upper));
			}
			if (m_IsMainGame) {
				//高度0なら止まる
				if (this->m_PosBuf.y() <= 0.f) {
					auto yPos = this->m_PosBuf.y();
					Easing(&yPos, 0.f, 0.8f, EasingType::OutExpo);
					this->m_PosBuf.y(yPos);
					Yadd = 0.f;
				}
				else {
					Yadd += M_GR / (FPS * FPS);
				}
				vecBuf.y(Yadd);
			}
			else {
				VECTOR_ref EndPos = m_PosBuf + VECTOR_ref::up() * 20.f;
				if (this->m_BackGround->CheckLinetoMap(this->m_PosBuf + VECTOR_ref::up() * -1.f, &EndPos, false)) {
					auto yPos = this->m_PosBuf.y();
					Easing(&yPos, EndPos.y(), 0.8f, EasingType::OutExpo);
					this->m_PosBuf.y(yPos);
					Yadd = 0.f;
				}
				else {
					Yadd += M_GR / (FPS * FPS);
				}
				vecBuf.y(Yadd);
			}
			this->m_move.vec = vecBuf;
			this->m_PosBuf += this->m_move.vec;
			//銃ひっこめ
			if (GetGunPtrNow() && this->m_BackGround) {
				if (m_StuckGunTimer == 0.f) {
					m_StuckGunTimer = 0.1f;
					auto EndPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::Muzzle).pos();
					if (this->m_BackGround->CheckLinetoMap(GetFrameWorldMat(CharaFrame::Head).pos(), &EndPos, false)) {//0.03ms
						m_IsStuckGun = true;
					}
					else {
						if (m_IsStuckGun) {
							SetAim();
						}
						m_IsStuckGun = false;
					}
				}
				else {
					m_StuckGunTimer = std::max(m_StuckGunTimer - 1.f / FPS, 0.f);
				}
				if (m_IsStuckGun) {
					SetReady();
				}
			}
			//壁判定
			if (this->m_BackGround) {
				std::vector<std::pair<MV1*, int>> cols;
				if (m_IsMainGame) {
					auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);
					for (auto& C : MainGB->GetBuildData()) {
						auto Vec = (C.GetMatrix().pos() - this->m_move.pos); Vec.y(0.f);
						if (Vec.Length() <= 2.f*Scale_Rate) {
							cols.emplace_back(std::make_pair((MV1*)(&C.GetCol()), C.GetMeshSel()));
						}
					}
					for (const auto& C : MainGB->GetDrumControl().GetDrum()) {
						auto Vec = (C.m_Pos - this->m_move.pos); Vec.y(0.f);
						if (Vec.Length() <= 2.f*Scale_Rate) {
							cols.emplace_back(std::make_pair((MV1*)(&C.m_Col), -1));
						}
					}
				}
				else {
					cols.emplace_back(std::make_pair((MV1*)(&this->m_BackGround->GetGroundCol()), -1));
				}
				col_wall(OLDpos, &this->m_PosBuf, cols);
			}
			//ほかプレイヤーとの判定
			if (m_IsMainGame) {
				float Radius = 2.f*0.5f*Scale_Rate;
				for (int i = 0; i < Player_num; i++) {
					if (i == this->m_MyID) { continue; }
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i).GetChara();
					if (Chara->GetHP() <= 0) { continue; }
					//自分が当たったら押し戻す
					VECTOR_ref Vec = (Chara->GetMove().pos - this->GetMove().pos); Vec.y(0.f);
					float Len = Vec.Length();
					if (Len < Radius) {
						this->m_PosBuf += Vec.Norm() * (Len - Radius);
					}
				}
			}
			//
			if (OverrideControl::PutOverride()) {
				auto& info_move = OverrideControl::GetOverRideInfo();
				this->m_PosBuf = info_move.pos;
				this->m_move.vec = info_move.vec;
				KeyControl::SetRadBuf().SetEasingX(info_move.rad.x(), 0.9f);
				KeyControl::SetRadBuf().y(info_move.rad.y());
				KeyControl::SetRad().y(info_move.rad.y());
			}
			this->m_move.mat = MATRIX_ref::RotZ(KeyControl::GetRad().z() / 2.f) * MATRIX_ref::RotY(this->m_yrad_Bottom);
			this->m_move.repos = this->m_move.pos;
			Easing(&this->m_move.pos, this->m_PosBuf, 0.9f, EasingType::OutExpo);
			this->m_Speed = (this->m_move.pos - this->m_move.repos).Length();
			UpdateMove();
			//スリング場所探索//0.02ms↓
			if (m_Gun_Ptr[0]) {
				m_SlingMat[0] =
					MATRIX_ref::RotX(deg2rad(-90))*
					GetFrameWorldMat(CharaFrame::Upper).GetRot()*
					GetCharaDir().Inverse()*
					MATRIX_ref::Mtrans(
						GetFrameWorldMat(CharaFrame::Upper).pos() +
						GetFrameWorldMat(CharaFrame::Upper).yvec() * (-0.5f*Scale_Rate) +
						GetFrameWorldMat(CharaFrame::Upper).xvec() * (-0.3f*Scale_Rate)
					);
			}
			if (m_Gun_Ptr[1]) {
				m_SlingZrad.Execute();
				m_SlingYrad.Execute();
				m_SlingZrad.AddRad(1.f / FPS * (1.f - m_SlingPer.at((int)(1 - this->m_GunSelect))));
				m_SlingZrad.AddRad(1.f / FPS * (KeyControl::GetRad().y() - this->m_yrad_Bottom));
				auto mat = MATRIX_ref::RotX(deg2rad(-30)) * MATRIX_ref::RotY(deg2rad(-90)) *
					(
						MATRIX_ref::RotZ(-this->m_SlingZrad.GetRad()) *
						MATRIX_ref::RotY(KeyControl::GetRad().y() + this->m_SlingYrad.GetRad())*
						GetCharaDir().Inverse()
						);
				m_SlingMat[1] = mat * MATRIX_ref::Mtrans(
					GetFrameWorldMat(CharaFrame::Head).pos() +
					GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.3f*Scale_Rate) +
					(
						this->m_move.mat.xvec() * sin(m_SlingZrad.GetRad()) +
						this->m_move.mat.yvec() * cos(m_SlingZrad.GetRad())
						) * -6.25f);
			}
			//銃座標指定
			if (!this->m_IsWearingArmer && !this->m_IsWearingMorphine) {
				if (GetGunPtrNow()) {
					//オートエイム
					{
						auto prev = m_AutoAimTimer;
						m_AutoAimTimer = std::max(m_AutoAimTimer - 1.f / FPS, 0.f);
						if (prev > 0.f && m_AutoAimTimer == 0.f) {
							m_AutoAim = -1;
						}
						if (m_AutoAim != -1) {
							auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(m_AutoAim).GetChara();
							if (Chara->GetHP() <= 0) {
								m_AutoAim = -1;
							}
						}

						Easing(&m_AutoAimOn, IsAutoAimActive() ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
					}
					//
					if (LifeControl::IsAlive()) {
						//銃の位置を指定するアニメ
						VECTOR_ref yVect0, zVect0, Post0;
						auto* AnimMngr = GunAnimManager::Instance();
						for (int i = 0; i < (int)EnumGunAnimType::Max; i++) {
							MATRIX_ref MatT;
							auto* Ptr = AnimMngr->GetAnimData(GetGunPtrNow()->GetGunAnimeSet((EnumGunAnimType)i));
							if (Ptr) {
								auto Now = AnimMngr->GetAnimNow(Ptr, GetGunPtrNow()->GetUpperAnim());
								MatT = Now.GetRot() * MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(Now.GetPos(), GetCharaDir()));
							}
							zVect0 = Lerp(zVect0, MatT.zvec(), this->m_Arm[i].Per());
							yVect0 = Lerp(yVect0, MatT.yvec(), this->m_Arm[i].Per());
							Post0 = Lerp(Post0, MatT.pos(), this->m_Arm[i].Per());
						}
						Post0 = GetFrameWorldMat(CharaFrame::Head).pos() + Post0;
						//銃の揺れ
						yVect0 = MATRIX_ref::Vtrans(yVect0, MATRIX_ref::RotAxis(zVect0, GetGunRadAdd() - this->m_LeanRad));
						//エイム先
						GunSwingControl::UpdateGunSwing(KeyControl::GetRad());
						//
						for (auto& p : this->m_Gun_Ptr) {
							if (p) {
								int index = (int)(&p - &this->m_Gun_Ptr.front());
								bool IsSelGun = (index == this->m_GunSelect);
								auto& Mat = this->m_SlingMat[index];
								auto& Per = this->m_SlingPer[index];
								Easing(&Per, IsSelGun ? 0.f : 1.f, IsSelGun ? 0.9f : 0.9f, EasingType::OutExpo);
								if (Per <= 0.01f) { Per = 0.f; }
								if (Per >= 0.99f) { Per = 1.f; }

								auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::front(), Lerp(zVect0, Mat.zvec(), Per));

								//オートエイム
								if (IsSelGun && GetIsADS() && IsAutoAimActive() && m_IsMainGame) {
									VECTOR_ref BasePos = p->GetFrameWorldMat(GunFrame::Muzzle).pos();
									float Range = GetIsLaserActive() ? 13.f : 7.f;

									for (int i = 0; i < Player_num; i++) {
										if (i == this->m_MyID) { continue; }
										auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i).GetChara();
										if (Chara->GetHP() <= 0) { continue; }
										if (!Chara->CanLookTarget) { continue; }
										VECTOR_ref Vec = (Chara->GetFrameWorldMat(CharaFrame::Head).pos() - BasePos);
										if (Vec.Length() >= (Scale_Rate* Range)) { continue; }
										if (GetEyeVector().dot(Vec.Norm()) < cos(deg2rad(15))) { continue; }

										m_AutoAimTimer = 1.f;
										m_AutoAim = i;
										break;
									}

									if (m_AutoAim != -1) {
										auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(m_AutoAim).GetChara();
										VECTOR_ref Vec = (Chara->GetFrameWorldMat(CharaFrame::Head).pos() - BasePos);
										tmp_gunrat = MATRIX_ref::RotVec2(GetEyeVector(), Vec.Norm()) * tmp_gunrat;

									}
								}

								tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), Lerp(yVect0, Mat.yvec(), Per));
								tmp_gunrat *=
									GunSwingControl::GetGunSwingMat() * GetCharaDir()*
									MATRIX_ref::RotVec2(Lerp(p->GetMatrix().yvec(), p->GetEyeYVec(), this->m_ADSPer), p->GetMatrix().yvec())*
									MATRIX_ref::Mtrans(Lerp(Post0, Mat.pos(), Per));
								p->SetGunMatrix(tmp_gunrat);
							}
						}
					}
					else {
						{
							auto tmp_gunrat =
								lagframe_.RIGHThand_f.GetFrameWorldPosition().GetRot().Inverse() *
								RagDollControl::GetRagDoll().GetFrameLocalWorldMatrix(lagframe_.RIGHThand_f.GetFrameID());

							GetGunPtrNow()->SetGunMatrix(tmp_gunrat);
						}
						auto& GunPtr = this->m_Gun_Ptr[(size_t)(1 - (size_t)m_GunSelect)];
						if (GunPtr) {
							//仮の画面外指定
							auto tmp_gunrat = GetFrameWorldMat(CharaFrame::Upper);
							tmp_gunrat = MATRIX_ref::RotX(deg2rad(90))*tmp_gunrat;
							GunPtr->SetGunMatrix(tmp_gunrat);
						}
					}
					//
					{
						bool isActive = GetGunPtrNow()->IsActive();
						if (GetShotPhase() >= GunAnimeID::AmmoLoading && GetShotPhase() <= GunAnimeID::AmmoLoadEnd) {
							if (isActive) {
								GetGunPtrNow()->SetActiveBySlot(false);
								(*GetGunPtrNow()->GetMagazinePtr())->SetActive(true);
							}
							GetGunPtrNow()->SetActive(false);
						}
						else {
							if (!isActive) {
								GetGunPtrNow()->SetActiveBySlot(true);
							}
							GetGunPtrNow()->SetActive(true);
						}
					}
					//手の位置を制御
					if ((m_MyID == 0) || this->CanLookTarget) {
						VECTOR_ref GunPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandPos).pos();
						VECTOR_ref Gunyvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandYvec).pos() - GunPos;
						VECTOR_ref Gunzvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::RightHandZvec).pos() - GunPos;
						VECTOR_ref HandsPos;
						VECTOR_ref Handsyvec;
						VECTOR_ref Handszvec;
						{
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
										case GunAnimeID::Base:
										case GunAnimeID::ReloadEnd:
										case GunAnimeID::CheckEnd:
										case GunAnimeID::Shot:
											m_MagHand = false;
											if (m_ArmBreak) {
												m_MagHand = true;
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
										case GunAnimeID::Base:
										case GunAnimeID::ReloadEnd:
										case GunAnimeID::CheckEnd:
										case GunAnimeID::Shot:
											m_MagHand = false;
											if (m_ArmBreak) {
												m_MagHand = true;
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
							if (GetShotPhase() == GunAnimeID::Melee) {
								m_MagHand = true;
							}
							if (GetShotPhase() == GunAnimeID::Watch) {
								if (m_ArmBreak) {
									m_MagHand = true;
								}
								else {
									if (this->m_GunSelect == 0) {
										m_MagHand = true;
									}
									else {
										m_MagHand = false;
									}
								}
							}

							m_MagArm.Execute(m_MagHand, 0.3f, 0.5f);

							VECTOR_ref HandPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
							VECTOR_ref Handyvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - HandPos;
							VECTOR_ref Handzvec = GetGunPtrNow()->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - HandPos;

							VECTOR_ref MagPos = GetFrameWorldMat(CharaFrame::LeftMag).pos();
							VECTOR_ref Magyvec = GetFrameWorldMat(CharaFrame::LeftMagyvec).pos() - MagPos;
							VECTOR_ref Magzvec = GetFrameWorldMat(CharaFrame::LeftMagzvec).pos() - MagPos;
							HandsPos = Lerp(HandPos, MagPos, m_MagArm.Per());
							Handsyvec = Lerp(Handyvec, Magyvec, 0.f);
							Handszvec = Lerp(Handzvec, Magzvec, 0.f);
						}
						//腕座標指定
						{
							switch (GetShotPhase()) {
								case GunAnimeID::Cocking:
								case GunAnimeID::ReloadStart_Empty:
								case GunAnimeID::ReloadStart:
								case GunAnimeID::ReloadOne:
								case GunAnimeID::ReloadEnd:
								case GunAnimeID::CheckStart:
								case GunAnimeID::Checking:
								case GunAnimeID::CheckEnd:
								case GunAnimeID::AmmoLoadStart:
								case GunAnimeID::AmmoLoading:
								case GunAnimeID::AmmoLoadEnd:
									StackLeftHandControl::ResetStackLeftHand();
									break;
								default:
									if (!GetRun() && !m_ArmBreak) {
										auto Dir = GetCharaDir();
										auto StartPos = GetFrameWorldMat(CharaFrame::Head).pos() + MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, -0.1f, -0.2f)*Scale_Rate, Dir);
										auto EndPos = StartPos + MATRIX_ref::Vtrans(VECTOR_ref::vget(0.5f, -0.1f, -0.7f)*Scale_Rate, Dir);
										VECTOR_ref Normal;
										if (this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, true, &Normal)) {
											StackLeftHandControl::SetStackLeftHand(EndPos, Normal);
										}
										else {
											StackLeftHandControl::ResetStackLeftHand();
										}
									}
									break;
							}
							StackLeftHandControl::UpdateStackLeftHand();

							HandsPos = Lerp(HandsPos, StackLeftHandControl::GetStuckLeftHandPos(), StackLeftHandControl::GetStuckLeftHandPer());
							Handsyvec = Lerp(Handsyvec, StackLeftHandControl::GetStuckLeftHandYVec(), StackLeftHandControl::GetStuckLeftHandPer());
							Handszvec = Lerp(Handszvec, StackLeftHandControl::GetStuckLeftHandYVec().cross(GetEyeVecY()).Norm(), StackLeftHandControl::GetStuckLeftHandPer());
						}
						if (m_ArmBreak) {
							HandsPos += VECTOR_ref::vget(GetRandf(1.f*Scale_Rate), GetRandf(1.f*Scale_Rate), GetRandf(1.f*Scale_Rate))*0.002f;
						}

						move_RightArm(GunPos, Gunyvec, Gunzvec);
						move_LeftArm(HandsPos, Handsyvec, Handszvec);
					}
					//LeftMag
					{
						MATRIX_ref Mat = GetFrameWorldMat(CharaFrame::LeftHandJoint);
						Mat = MATRIX_ref::RotX(deg2rad(-90))*Mat;
						bool isDirect = false;
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
							case GunAnimeID::AmmoLoading:
							case GunAnimeID::AmmoLoadEnd:
								m_AmmoHandR = 1.f;
								m_AmmoHand = 1.f;
								isDirect = true;
								break;
							default:
								m_AmmoHandR = 0.f;
								Mat = GetGunPtrNow()->GetFrameWorldMat(GunFrame::Magpos);
								break;
						}
						switch (GetGunPtrNow()->GetReloadType()) {
							case RELOADTYPE::AMMO:
								m_AmmoHand = 0.f;
								isDirect = false;
								break;
							default:
								break;
						}
						(*GetGunPtrNow()->GetMagazinePtr())->SetHandMatrix(Mat, m_AmmoHand, isDirect);
						Easing(&m_AmmoHand, m_AmmoHandR, 0.9f, EasingType::OutExpo);
					}
				}
			}
			else {
				if (this->m_Armer_Ptr) {
					auto tmp_gunrat = GetFrameWorldMat(CharaFrame::Head);
					tmp_gunrat = MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, -0.5f, 0.2f)*Scale_Rate*std::clamp(1.f - this->m_Wear_Armer.Per() * 1.5f, 0.f, 1.f))*tmp_gunrat;

					tmp_gunrat = MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.15f, -0.5f)*Scale_Rate*(1.f - this->m_Wear_Armer.Per()))*tmp_gunrat;
					tmp_gunrat = MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, -0.30f, 0.f)*Scale_Rate*this->m_Wear_Armer.Per())*tmp_gunrat;
					tmp_gunrat = MATRIX_ref::RotX(deg2rad(-30.f)*(1.f - this->m_Wear_Armer.Per()))*tmp_gunrat;
					this->m_Armer_Ptr->SetMove(tmp_gunrat.GetRot(), tmp_gunrat.pos());
					this->m_Armer_Ptr->UpdateMove();

					VECTOR_ref GunPos = this->m_Armer_Ptr->GetRightHandMat().pos();
					VECTOR_ref Gunyvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(-1.f, 0.f, 0.f), this->m_Armer_Ptr->GetRightHandMat().GetRot());
					VECTOR_ref Gunzvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, 1.f, 0.f), this->m_Armer_Ptr->GetRightHandMat().GetRot());
					VECTOR_ref HandsPos = this->m_Armer_Ptr->GetLeftHandMat().pos();
					VECTOR_ref Handsyvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(1.f, 0.f, 0.f), this->m_Armer_Ptr->GetLeftHandMat().GetRot());
					VECTOR_ref Handszvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, 0.f, -1.f), this->m_Armer_Ptr->GetLeftHandMat().GetRot());

					move_RightArm(GunPos, Gunyvec, Gunzvec);
					move_LeftArm(HandsPos, Handsyvec, Handszvec);
				}
				//Morphine
				if (this->m_Morphine_Ptr && this->m_Morphine_Ptr->IsActive()) {
					auto tmp_armrat = GetFrameWorldMat(CharaFrame::Head);
					VECTOR_ref HandsPos = tmp_armrat.pos()
						+ tmp_armrat.xvec()*(-0.2f*Scale_Rate)
						+ tmp_armrat.yvec()*(0.01f*Scale_Rate)
						+ tmp_armrat.zvec()*(-0.5f*Scale_Rate)
						;
					VECTOR_ref Handsyvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(1.f, 0.f, 0.f), tmp_armrat.GetRot());
					VECTOR_ref Handszvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, 0.f, -1.f), tmp_armrat.GetRot());
					move_LeftArm(HandsPos, Handsyvec, Handszvec);

					tmp_armrat = GetFrameWorldMat(CharaFrame::LeftWrist);

					auto tmp_injmat = GetFrameWorldMat(CharaFrame::Head);
					tmp_injmat = MATRIX_ref::RotX(deg2rad(-90.f))*MATRIX_ref::RotY(deg2rad(-45.f))*tmp_injmat;

					VECTOR_ref GunPos = tmp_armrat.pos()
						+ tmp_injmat.zvec()*(0.5f*Scale_Rate * (1.f - this->m_Wear_Morphine.Per()))
						
						+ tmp_armrat.xvec()*(-0.1f*Scale_Rate)
						+ tmp_armrat.yvec()*(0.2f*Scale_Rate)
						+ tmp_armrat.zvec()*(-0.05f*Scale_Rate)
						;
					VECTOR_ref Gunyvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(-1.f, 0.f, 0.f), tmp_injmat.GetRot());
					VECTOR_ref Gunzvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, 1.f, 0.f), tmp_injmat.GetRot());

					move_RightArm(GunPos, Gunyvec, Gunzvec);

					auto tmp_gunrat = GetFrameWorldMat(CharaFrame::RightHandJoint);
					this->m_Morphine_Ptr->SetMove(tmp_gunrat.GetRot(), tmp_gunrat.pos());
					this->m_Morphine_Ptr->UpdateMove();
				}
				if (GetGunPtrNow()) {
					//仮の画面外指定
					auto tmp_gunrat = GetFrameWorldMat(CharaFrame::Upper);
					tmp_gunrat = MATRIX_ref::RotX(deg2rad(90))*tmp_gunrat;
					GetGunPtrNow()->SetGunMatrix(tmp_gunrat);
				}
				auto& GunPtr = this->m_Gun_Ptr[(size_t)(1 - (size_t)m_GunSelect)];
				if (GunPtr) {
					//仮の画面外指定
					auto tmp_gunrat = GetFrameWorldMat(CharaFrame::Upper);
					tmp_gunrat = MATRIX_ref::RotX(deg2rad(90))*tmp_gunrat;
					GunPtr->SetGunMatrix(tmp_gunrat);
				}
			}
			//ラグドール//0.01ms↓
			RagDollControl::Execute_RagDollControl(this->GetObj(), LifeControl::IsAlive());
			//ヒットボックス
			HitBoxControl::UpdataHitBox(this, (this->GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);
			//
			WalkSwingControl::UpdateWalkSwing(GetEyePosition() - GetMatrix().pos(), std::clamp(this->m_Speed / 2.f, 0.f, 1.f));
		}
		//顔
		void			CharacterClass::ExecuteShape(void) noexcept {
			ShapeControl::ExcuteShape();
			SetShapePer(CharaShape::None, 0.f);
			SetShapePer(CharaShape::O, StaminaControl::GetMousePer());
			SetShapePer(CharaShape::EYECLOSE, ShapeControl::GetEyeclosePer());
		}



		void			CharacterClass::Init(void) noexcept {
			ObjectBaseClass::Init();
			RagDollControl::Init_RagDollControl(this->GetObj());
			for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
			GetAnime(GetBottomStandAnimSel()).per = 1.f;
			HitBoxControl::InitHitBox();
			m_ArmBreak = false;
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				FlipAnime();
				GetObj().work_anime();
			}
			this->m_SoundPower = std::max(this->m_SoundPower - 1.f / FPS, 0.f);
			ExecuteInput();				//操作//0.0ms↓
			ExecuteUpperMatrix();		//上半身回転//0.03ms↓
			ExecuteAnim();				//AnimUpdte//0.02ms↓
			ExecuteMatrix();			//SetMat指示//0.18ms↓
			ExecuteShape();				//顔//スコープ内//0.0ms↓
			EffectControl::Execute();//0.0ms↓
		}
		void			CharacterClass::Draw(bool isDrawSemiTrans) noexcept {
			//return;
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
					fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効 FALSE:無効 )
					fog_mode = GetFogMode();														// フォグモードを取得する
					GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
					GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
					fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )


					//キャラ描画
					SetFogEnable(TRUE);
					SetFogColor(0, 0, 0);
					//MV1SetMaterialTypeAll(this->GetObj().get(), DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
					if (LifeControl::IsAlive()) {
						for (int i = 0; i < this->GetObj().mesh_num(); i++) {
							if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
					}
					else {
						for (int i = 0; i < RagDollControl::GetRagDoll().mesh_num(); i++) {
							if ((MV1GetMeshSemiTransState(RagDollControl::GetRagDoll().get(), i) == TRUE) == isDrawSemiTrans) {
								RagDollControl::GetRagDoll().DrawMesh(i);
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
				if (LifeControl::IsAlive()) {
					this->GetObj().DrawModel();
				}
				else {
					RagDollControl::GetRagDoll().DrawModel();
				}
			}
		}
		//
		void			CharacterClass::Dispose(void) noexcept {
			ObjectBaseClass::Dispose();
			EffectControl::Dispose();
			this->m_BackGround.reset();
			auto* ObjMngr = ObjectManager::Instance();
			for (auto& g : m_Gun_Ptr) {
				if (g) {
					ObjMngr->DelObj((SharedObj*)&g);
					g.reset();
				}
			}
		}
	};
};
