#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
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
			VECTOR_ref vec_a1L1 = VECTOR_ref::vget(-0.5f, -1.f, vec_a1.y() / -abs(vec_a1.z())).Norm();//x=0とする
			if (IsGun0Select()) {
				vec_a1L1 = VECTOR_ref::vget(-1.5f, -1.f, vec_a1.y() / -abs(vec_a1.z())).Norm();//x=0とする
			}
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
		const MATRIX_ref CharacterClass::GetCharaDir(void) const noexcept {
			auto tmpUpperMatrix = MATRIX_ref::RotZ(KeyControl::GetLeanRad());

			tmpUpperMatrix *=
				MATRIX_ref::RotX(KeyControl::GetRad().x()) *
				MATRIX_ref::RotY(Lerp(KeyControl::GetYRadBottomChange(), 0.f, this->m_Arm[(int)EnumGunAnimType::Run].Per()));
			return tmpUpperMatrix * this->m_move.mat;
		}
		//
		void			CharacterClass::Shot_Start() noexcept {
			if (this->m_MyID == 0) {
				auto* PlayerMngr = PlayerManager::Instance();
				PlayerMngr->GetPlayer(GetMyPlayerID()).AddShot(GetGunPtrNow()->GetPelletNum());
			}
			GetGunPtrNow()->SetBullet();
			if (IsULTSelect()) {
				ULTControl::AddULT(-3, true);
			}
			//
			float Power = 0.0001f*GetGunPtrNow()->GetRecoilPower();
			if (GetIsSquat()) {
				Power *= 0.5f;
			}
			if (m_ArmBreak) {
				Power *= 1.5f;
			}
			this->m_RecoilRadAdd.Set(GetRandf(Power / 4.f), -Power, 0.f);
			//
			if (this->m_MyID == 0) {
				auto* DrawParts = DXDraw::Instance();
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
			//音
			this->m_SoundPower = 1.f * (CanLookTarget ? 1.f : 0.5f);
		}
		void			CharacterClass::Reload_Start() noexcept {
			auto* SE = SoundPool::Instance();
			if (IsGun0Select()) {
				int Num = GetGunPtrNow()->GetAmmoNum();
				int All = GetGunPtrNow()->GetAmmoAll();
				int UniqueID = GetGunPtrNow()->GetMagUniqueID();
				GetGunPtrNow()->SetReloadStart(MagStockControl::GetNowMag().AmmoNum, MagStockControl::GetNowMag().ModUniqueID);
				MagStockControl::SetOldMag(Num, All, UniqueID);
				//todo::Ammoの通常武器状態
			}
			else {
				GetGunPtrNow()->SetReloadStart(GetGunPtrNow()->GetAmmoAll(), GetGunPtrNow()->GetMagUniqueID());
			}
			if (this->m_MyID != 0) {
				if (GetRand(1) == 0) {
					SE->Get((int)SoundEnum::Man_reload).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
				}
				else {
					SE->Get((int)SoundEnum::Man_takecover).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
				}
				this->m_SoundPower = 0.6f * (CanLookTarget ? 1.f : 0.5f);
			}
		}
		//
		const MATRIX_ref CharacterClass::GetEyeMatrix(void) const noexcept {
			auto tmpUpperMatrix = MATRIX_ref::RotZ(KeyControl::GetLeanRad() / 5.f);

			tmpUpperMatrix *=
				MATRIX_ref::RotX(KeyControl::GetRad().x()) *
				MATRIX_ref::RotY(KeyControl::GetYRadBottomChange());

			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->Get_HeadBobbing());
			if (HeadBobbing) {
				tmpUpperMatrix = WalkSwingControl::GetWalkSwingMat() * tmpUpperMatrix;
			}
			tmpUpperMatrix *= this->m_move.mat;

			auto EyePosition = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + tmpUpperMatrix.zvec()*(-0.04f*Scale_Rate);

			if (HeadBobbing) {
				EyePosition += EyeSwingControl::GetEyeSwingPos();
			}
			if (GetGunPtrNow()) {
				EyePosition = Lerp<VECTOR_ref>(EyePosition, GetGunPtrNow()->GetEyePos(), GunReadyControl::GetADSPer());
			}
			return tmpUpperMatrix * MATRIX_ref::Mtrans(EyePosition);
		}
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();

			if (this->m_MyID == value.DamageID) {
				auto PrevLive = LifeControl::IsAlive();
				LifeControl::SubHP(value.Damage);
				LifeControl::SubAP(value.ArmerDamage);
				if (value.ShotID == 0) {
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						PlayerMngr->GetPlayer(value.ShotID).AddHit(1);
					}
				}
				if (LifeControl::IsAlive()) {
					if ((value.Damage >= 0) && (value.ArmerDamage >= 0)) {
						SE->Get((int)SoundEnum::Man_Hurt1 + GetRand(6 - 1)).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
					}
					else {
						//SE->Get((int)SoundEnum::Man_Hurt1 + GetRand(6 - 1)).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
						//
						if (!PrevLive) {
							ItemPopControl::RepopItem(this->m_BackGround);
						}
					}
				}
				else if (PrevLive) {
					for (int i = 0; i < 6; i++) {
						SE->Get((int)SoundEnum::Man_Hurt1 + i).StopAll(0);
					}
					SE->Get((int)SoundEnum::Man_contact).StopAll(0);
					SE->Get((int)SoundEnum::Man_openfire).StopAll(0);
					SE->Get((int)SoundEnum::Man_reload).StopAll(0);
					SE->Get((int)SoundEnum::Man_takecover).StopAll(0);
					SE->Get((int)SoundEnum::Man_breathing).StopAll(0);
					SE->Get((int)SoundEnum::Man_breathend).StopAll(0);

					SE->Get((int)SoundEnum::Man_Death1 + GetRand(8 - 1)).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
					if (value.ShotID == 0) {
						PlayerMngr->GetPlayer(value.ShotID).AddScore(100 + ((IsGun0Select() && (value.Damage >= 100)) ? 20 : 0));
						PlayerMngr->GetPlayer(value.ShotID).AddKill(1);

						auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(value.ShotID).GetChara();
						if (Chara->GetAutoAimActive()) {

							Chara->AddULT(IsULTSelect() ? 1 : 6, Chara->IsGun0Select());
						}
						else {
							Chara->AddULT(IsULTSelect() ? 9 : 20, Chara->IsGun0Select());
						}
					}
					ItemPopControl::SetPop(GetCharaPosition() + VECTOR_ref::up()*(0.5f*Scale_Rate));
				}
				this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
				return true;
			}
			return false;
		}
		const bool		CharacterClass::CheckDamageRay(HitPoint* Damage, ArmerPoint* ArmerDamage, bool CheckBodyParts, PlayerID AttackID, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept {
			if (!LifeControl::IsAlive()) { return false; }
			if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, GetCharaPosition()) <= 2.0f*Scale_Rate)) { return false; }

			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			//被弾処理
			auto* HitPtr = HitBoxControl::GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(AttackID).GetChara();
				//部位ダメージ演算
				if (CheckBodyParts) {
					switch (HitPtr->GetColType()) {
						case HitType::Head:
							if (m_MyID == 0) {
								*Damage = *Damage;
								*ArmerDamage = 0;
								m_HeadShotSwitch = true;
							}
							else {
								*Damage = *Damage * 100 / 10;
								*ArmerDamage = 0;
							}
							break;
						case HitType::Body:
							if (LifeControl::IsArmerActive()) {
								*ArmerDamage = std::clamp((ArmerPoint)*Damage, (ArmerPoint)0, LifeControl::GetAP());
								*Damage -= (HitPoint)*ArmerDamage;
							}
							else {
								*ArmerDamage = 0;
								*Damage = *Damage;
							}
							break;
						case HitType::Arm:
							*Damage = *Damage * 5 / 10;
							*ArmerDamage = 0;
							break;
						case HitType::Leg:
							*Damage = *Damage * 7 / 10;
							*ArmerDamage = 0;
							break;
						default:
							break;
					}
					*Damage = std::min(*Damage, LifeControl::GetHPMax());
					*ArmerDamage = std::min(*ArmerDamage, LifeControl::GetAPMax());
				}
				//ダメージ登録
				{
					//auto v1 = GetEyeMatrix().zvec() * -1.f;
					//auto v2 = (Chara->GetCharaPosition() - this->GetCharaPosition()).Norm(); v2.y(0);
					//atan2f(v1.cross(v2).y(), v1.dot(v2))
					LifeControl::SetSubHPEvent(AttackID, this->m_MyID, *Damage, *ArmerDamage);
				}
				//SE
				if (AttackID == 0) {
					if (*Damage > 0) {
						SE->Get((int)SoundEnum::Hit).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
					}
					else {
						SE->Get((int)SoundEnum::HitGuard).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f, 128);
					}
				}
				if (m_MyID == 0 && m_IsMainGame) {
					if (*Damage > 0) {
						SE->Get((int)SoundEnum::HitMe).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f);
					}
					else {
						SE->Get((int)SoundEnum::HitGuard).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 10.f, 255);
					}
				}
				//エフェクトセット
				if (*Damage > 0) {
					EffectControl::SetOnce(EffectResource::Effect::ef_hitblood, *pEndPos, VECTOR_ref::front(), Scale_Rate);
					EffectControl::SetEffectSpeed(EffectResource::Effect::ef_hitblood, 2.f);
				}
				else {
					EffectControl::SetOnce(EffectResource::Effect::ef_gndsmoke, *pEndPos, (Chara->GetCharaPosition() - this->GetCharaPosition()).Norm(), 0.25f * Scale_Rate);
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
		void			CharacterClass::LoadExtends() noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			//アーマー
			{
				auto* ArmerPtr = ObjMngr->MakeObject(ObjType::Armer);
				auto& a = (std::shared_ptr<ArmerClass>&)(*ArmerPtr);

				ObjMngr->LoadObjectModel((*ArmerPtr).get(), "data/model/PlateCarrler/");
				(*ArmerPtr)->Init();
				this->SetArmer(a);
			}
			{
				auto* MorphinePtr = ObjMngr->MakeObject(ObjType::Morphine);
				auto& a = (std::shared_ptr<MorphineClass>&)(*MorphinePtr);

				ObjMngr->LoadObjectModel((*MorphinePtr).get(), "data/model/Morphine/");
				(*MorphinePtr)->Init();
				this->SetMorphine(a);
			}
		}
		//操作
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			auto* ItemLogParts = GetItemLog::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* OptionParts = OPTION::Instance();
			//
			auto PrevAction = m_CharaAction;
			//開始演出
			if (GetGunPtrNow() && m_ReadyAnim > 0.f) {
				m_ReadyAnim = std::max(m_ReadyAnim - 1.f / FPS, 0.f);
				switch (m_ReadyAnimPhase) {
					case 0:
						GunReadyControl::SetAim();
						if (m_ReadyAnim < 4.f) {
							m_ReadyAnimPhase++;
							//リロード
							m_CharaAction = CharaActionID::Reload;
							DrawParts->SetCamShake(0.1f, 2.f);
						}
						break;
					case 1:
						if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadEnd) {
							m_ReadyAnimPhase++;
							DrawParts->SetCamShake(0.1f, 2.f);
						}
						break;
					case 2:
						GunReadyControl::SetReady();
						if (IsLowReadyPer()) {
							m_ReadyAnimPhase++;
							GunReadyControl::SetAim();
							SelectGun(0);
							m_CharaAction = CharaActionID::Check;
							DrawParts->SetCamShake(0.1f, 2.f);
						}
						break;
					case 3:
						if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::CheckEnd) {
							m_ReadyAnimPhase++;
						}
						break;
					default:
						m_ReadyAnim = -1.f;
						break;
				}
			}
			//
			bool Press_Shot = KeyControl::GetInputControl().GetPADSPress(PADS::SHOT) && !m_IsChanging;
			bool Press_Reload = KeyControl::GetInputControl().GetPADSPress(PADS::RELOAD) && !m_IsChanging;
			if (GetGunPtrNow()->GetAmmoNumTotal() == 0 && GetGunPtrNow()->GetGunAnime() == GunAnimeID::Base) {
				Press_Reload |= Press_Shot;				//打ち切りで自動リロード
			}
			bool Press_Aim = KeyControl::GetInputControl().GetPADSPress(PADS::AIM) && !m_IsChanging;
			switch (GetGunPtrNow()->GetShotType()) {
				case SHOTTYPE::BOLT:
					if (GetGunPtrNow()->GetGunAnime() > GunAnimeID::Shot) {//撃てない状態
						Press_Aim = false;
					}
					break;
				default:
					break;
			}
			//
			if (PrevAction == m_CharaAction) {
				switch (m_CharaAction) {
					case CharaActionID::Ready:
						if (m_ActionFirstFrame) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
						}
						//モルヒネ
						if (KeyControl::GetInputControl().GetPADSPress(PADS::THROW)) {
							if ((this->m_MorphineStock != 0) && (LifeControl::GetHP() < LifeControl::GetHPMax())) {
								this->m_MorphineStock = std::max(this->m_MorphineStock - 1, 0);
								m_CharaAction = CharaActionID::Morphine;
							}
						}
						//アーマー着用/弾込め
						if (KeyControl::GetInputControl().GetPADSPress(PADS::CHECK)) {
							if ((this->m_ArmerStock != 0) && (LifeControl::GetAP() < LifeControl::GetAPMax())) {
								this->m_ArmerStock = std::max(this->m_ArmerStock - 1, 0);
								m_CharaAction = CharaActionID::WearArmer;
							}
							else if (GetGunPtrNow()) {
								if (IsGun0Select() && MagStockControl::GetNeedAmmoLoad(GetGunPtrNow()->GetIsMagFull(), GetGunPtrNow()->GetIsMagEmpty())) {
									m_CharaAction = CharaActionID::AmmoLoad;
								}
								else if(OptionParts->Get_EnableCheck()){
									m_CharaAction = CharaActionID::Watch;
								}
							}
						}
						//射撃
						if (!KeyControl::GetRun() && Press_Shot) {
							if (IsAimPer()) {
								if (GetGunPtrNow() && (GetGunPtrNow()->GetGunAnime() == GunAnimeID::Base) && GetGunPtrNow()->GetInChamber()) {
									Shot_Start();
								}
							}
							GunReadyControl::SetAimOrADS();
						}
						//リロード/マガジンチェック
						if (!KeyControl::GetRun() && Press_Reload) {
							bool IsCheck = GetGunPtrNow()->GetIsMagFull();
							if (IsGun0Select() && GetGunPtrNow()->GetReloadType() == RELOADTYPE::MAG) {
								IsCheck = (GetGunPtrNow()->GetAmmoNum() >= MagStockControl::GetMag(0));
							}
							if (!IsCheck) {
								m_CharaAction = CharaActionID::Reload;
							}
							else if (OptionParts->Get_EnableCheck()) {
								m_CharaAction = CharaActionID::Check;
							}
						}
						//姿勢指定
						if (Press_Aim) {
							if (!m_IsStuckGun) {
								GunReadyControl::SetADS();
							}
							else {
								GunReadyControl::SetAim();
							}
						}
						if (KeyControl::GetRun()) {
							GunReadyControl::SetReady();
						}
						//
						if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::Shot) {
							bool ischeck = true;
							switch (GetGunPtrNow()->GetShotType()) {
								case SHOTTYPE::FULL:
									ischeck = GetGunPtrNow()->GetInChamber() || GetGunPtrNow()->GetIsMagEmpty();
									break;
								case SHOTTYPE::SEMI:
								case SHOTTYPE::BOLT:
									ischeck = !Press_Shot;
									break;
								default:
									break;
							}

							if (GetGunPtrNow()->GetAnime(GunAnimeID::Shot).TimeEnd() && ischeck) {
								if (!GetGunPtrNow()->GetIsMagEmpty()) {
									switch (GetGunPtrNow()->GetShotType()) {
										case SHOTTYPE::FULL:
										case SHOTTYPE::SEMI:
											GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
											break;
										case SHOTTYPE::BOLT:
											m_CharaAction = CharaActionID::Cocking;
											break;
										default:
											break;
									}
								}
								else {
									GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
								}
							}
						}
						break;
					case CharaActionID::Cocking:
						if (m_ActionFirstFrame) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Cocking);
						}
						//
						if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::Cocking) {
							m_CharaAction = CharaActionID::Ready;
						}
						//姿勢指定
						GunReadyControl::SetAim();
						break;
					case CharaActionID::Check:
						if (m_ActionFirstFrame) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::CheckStart);
						}
						//
						if (!GetGunPtrNow()->GetChecking()) {
							m_CharaAction = CharaActionID::Ready;
						}
						//姿勢指定
						GunReadyControl::SetAim();
						break;
					case CharaActionID::Reload:
						if (m_ActionFirstFrame) {
							Reload_Start();
						}
						else {
							if ((GetGunPtrNow()->GetReloadType() == RELOADTYPE::AMMO) && (GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadOne)) {
								if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
									GetGunPtrNow()->SetCancel(true);
								}
							}
							if (!GetGunPtrNow()->GetReloading()) {
								if (GetGunPtrNow()->GetInChamber()) {
									m_CharaAction = CharaActionID::Ready;
								}
								else {
									if (!GetGunPtrNow()->GetIsMagEmpty()) {
										m_CharaAction = CharaActionID::Cocking;
									}
									else {
										m_CharaAction = CharaActionID::Ready;
									}
								}
							}
						}
						//姿勢指定
						GunReadyControl::SetAim();
						break;
					case CharaActionID::Watch:
						if (m_ActionFirstFrame) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Watch);
						}
						//
						if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::Watch) {
							m_CharaAction = CharaActionID::Ready;
						}
						//姿勢指定
						GunReadyControl::SetAim();
						break;
					case CharaActionID::Melee:
						if (m_ActionFirstFrame) {
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Melee);
							m_MeleeCoolDown = 1.f;
							if (this->m_MyID == 0) {
								DrawParts->SetCamShake(0.1f, 5.f);
							}
						}
						//
						if (GetGunPtrNow()->GetGunAnime() != GunAnimeID::Melee) {
							m_CharaAction = CharaActionID::Ready;
						}
						//姿勢指定
						GunReadyControl::SetAim();
						break;
					case CharaActionID::AmmoLoad:
						if (m_ActionFirstFrame) {
							this->m_AmmoLoadStart = true;
						}
						if (GetGunPtrNow()) {
							if (this->m_AmmoLoadStart) {
								if (IsLowReadyPer()) {
									this->m_AmmoLoadStart = false;
									GetGunPtrNow()->SetGunAnime(GunAnimeID::AmmoLoading);
									GetGunPtrNow()->SetCancel(false);
									this->m_AmmoLoadCount = 0;
									this->m_AmmoLoadSwitch = true;
								}
								GunReadyControl::SetReady();
							}
							else {
								if (GetGunPtrNow()->GetGunAnime() == GunAnimeID::AmmoLoading) {
									//銃表示
									GetGunPtrNow()->SetActiveAll(false);
									(*GetGunPtrNow()->GetMagazinePtr())->SetActive(true);
									//
									GunReadyControl::SetAim();
									if (GetGunPtrNow()->GetGunAnimZero(GunAnimeID::AmmoLoading)) {
										this->m_AmmoLoadSwitch = true;
									}

									if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
										GetGunPtrNow()->SetCancel(true);
									}
									if (this->m_AmmoLoadSwitch) {
										this->m_AmmoLoadSwitch = false;
										if (this->m_AmmoLoadCount < 4) {
											int AmmoCount = (m_AmmoLoadCount >= 1) ? MagStockControl::GetMag(this->m_AmmoLoadCount - 1) : GetGunPtrNow()->GetAmmoNum();
											int InAmmo = std::clamp(MagStockControl::GetAmmoStock(), 0, GetGunPtrNow()->GetAmmoAll() - AmmoCount);
											AmmoCount += InAmmo;
											MagStockControl::SubAmmoStock(InAmmo);
											for (int i = 2;i >= this->m_AmmoLoadCount;i--) {
												InAmmo = std::clamp(MagStockControl::GetMag(i), 0, GetGunPtrNow()->GetAmmoAll() - AmmoCount);
												AmmoCount += InAmmo;
												MagStockControl::SetMag(i, MagStockControl::GetMag(i) - InAmmo);
											}
											if (m_AmmoLoadCount == 0) {
												GetGunPtrNow()->SetAmmo(AmmoCount);
											}
											else {
												MagStockControl::SetMag(this->m_AmmoLoadCount - 1, AmmoCount);
											}
											this->m_AmmoLoadCount++;
											if (this->m_AmmoLoadCount >= 4) {
												this->m_AmmoLoadCount = 0;
												GetGunPtrNow()->SetCancel(true);
											}
										}
									}
								}
								else {
									GunReadyControl::SetReady();
									if (IsLowReadyPer()) {
										if (!GetGunPtrNow()->GetInChamber()) {
											m_CharaAction = CharaActionID::Cocking;
										}
										else {
											m_CharaAction = CharaActionID::Ready;
										}
										GetGunPtrNow()->SetActiveAll(true);
									}
								}
							}
						}
						break;
					case CharaActionID::WearArmer:
						if (m_ActionFirstFrame) {
							this->m_ArmerWearPhase = ArmerWearPhase::Have;
						}
						switch (this->m_ArmerWearPhase) {
							case ArmerWearPhase::Have:
								if (IsLowReadyPer()) {
									this->m_ArmerWearPhase = ArmerWearPhase::Wear;
									this->m_Wear_Armer.Init(false);
									if (this->m_Armer_Ptr) {
										this->m_Armer_Ptr->SetActive(true);
									}
									//銃表示
									if (GetGunPtrNow()) {
										GetGunPtrNow()->SetActiveAll(false);
									}
									SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
								}
								GunReadyControl::SetReady();
								break;
							case ArmerWearPhase::Wear:
								this->m_Wear_Armer.Execute(1.f, 0.1f, 0.1f, 0.95f);
								if ((this->m_Wear_Armer.Per() < 0.f) || (1.f < this->m_Wear_Armer.Per())) {
									this->m_ArmerWearPhase = ArmerWearPhase::Have;
									m_CharaAction = CharaActionID::Ready;
									if (this->m_Armer_Ptr) {
										this->m_Armer_Ptr->SetActive(false);
									}
									//銃表示
									if (GetGunPtrNow()) {
										GetGunPtrNow()->SetActiveAll(true);
									}
									if (m_IsMainGame) {
										ItemLogParts->AddLog(3.f, GetColor(25, 122, 75), "アーマーを着用 +%4d", 10);
										PlayerMngr->GetPlayer(GetMyPlayerID()).AddScore(10);
									}
									GunReadyControl::SetAim();
									LifeControl::SetHealEvent(this->m_MyID, this->m_MyID, 0, LifeControl::GetAPMax());
								}
								break;
							default:
								break;
						}
						break;
					case CharaActionID::Morphine:
						if (m_ActionFirstFrame) {
							this->m_MorphinePhase = MorphinePhase::Have;
						}
						switch (this->m_MorphinePhase) {
							case MorphinePhase::Have:
								if (IsLowReadyPer()) {
									this->m_MorphinePhase = MorphinePhase::Wear;
									this->m_Wear_Morphine.Init(false);
									if (this->m_Morphine_Ptr) {
										this->m_Morphine_Ptr->SetActive(true);
									}
									//銃表示
									if (GetGunPtrNow()) {
										GetGunPtrNow()->SetActiveAll(false);
									}
									this->m_Wear_MorphineFrame = 0.f;
									this->m_Wear_MorphinePer = 0.f;
									SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
									SE->Get((int)SoundEnum::Stim).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
								}
								GunReadyControl::SetReady();
								break;
							case MorphinePhase::Wear:
								this->m_Wear_MorphineFrame += 1.f / FPS;
								this->m_Wear_Morphine.Execute((this->m_Wear_MorphineFrame < 1.5f), 0.1f, 0.1f, 0.8f);
								if (this->m_Wear_MorphineFrame > 2.f) {
									this->m_MorphinePhase = MorphinePhase::Have;
									m_CharaAction = CharaActionID::Ready;
									if (this->m_Morphine_Ptr) {
										this->m_Morphine_Ptr->SetActive(false);
									}
									//銃表示
									if (GetGunPtrNow()) {
										GetGunPtrNow()->SetActiveAll(true);
									}
									GunReadyControl::SetAim();
									if (m_IsMainGame) {
										ItemLogParts->AddLog(3.f, GetColor(25, 122, 75), "モルヒネ注射 +%4d", 50);
										PlayerMngr->GetPlayer(GetMyPlayerID()).AddScore(50);
									}
								}

								this->m_Wear_MorphinePer += 1.f / FPS;
								if (this->m_Wear_MorphinePer >= 0.f) {
									this->m_Wear_MorphinePer -= 0.4f;
									Heal(20, true);
								}
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}
				//近接
				if (!KeyControl::GetRun() && KeyControl::GetInputControl().GetPADSPress(PADS::MELEE) && m_MeleeCoolDown == 0.f && IsAimPer() && !GetIsADS()) {
					m_CharaAction = CharaActionID::Melee;
				}
			}
			//
			m_ActionFirstFrame = (PrevAction != m_CharaAction);
		}
		//音指示
		void			CharacterClass::ExecuteSound(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			//足音
			if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
				auto Time = this->GetObj().get_anime((int)this->m_BottomAnimSelect).time;
				if (!GetRun()) {
					//L
					if ((9.f < Time && Time < 10.f)) {
						if (this->m_CharaSound != 1) {
							this->m_CharaSound = 1;
							this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(CharaFrame::LeftFoot).pos()).Length() < Scale_Rate * 5.f*1.5f) {

								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), Scale_Rate * 5.f);
								if (!GunReadyControl::GetIsADS()) {
									if (this->m_MyID == 0) {
										auto* OptionParts = OPTION::Instance();
										bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->Get_HeadBobbing());
										if (HeadBobbing) {
											DrawParts->SetCamShake(0.1f, 1.f);
										}
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
								if (!GunReadyControl::GetIsADS()) {
									if (this->m_MyID == 0) {
										auto* OptionParts = OPTION::Instance();
										bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->Get_HeadBobbing());
										if (HeadBobbing) {
											DrawParts->SetCamShake(0.1f, 1.f);
										}
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
								if (!GunReadyControl::GetIsADS()) {
									if (this->m_MyID == 0) {
										auto* OptionParts = OPTION::Instance();
										bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->Get_HeadBobbing());
										if (HeadBobbing) {
											DrawParts->SetCamShake(0.1f, 1.f);
										}
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
								if (!GunReadyControl::GetIsADS()) {
									if (this->m_MyID == 0) {
										auto* OptionParts = OPTION::Instance();
										bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->Get_HeadBobbing());
										if (HeadBobbing) {
											DrawParts->SetCamShake(0.1f, 1.f);
										}
									}
								}
							}
						}
					}
				}
			}
			//しゃがみ音
			if (KeyControl::GetSquatSwitch()) {
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale_Rate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//リーン音
			if (KeyControl::GetLeanSwitch()) {
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale_Rate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//心拍音
			if (this->m_MyID == 0) {
				if (StaminaControl::ExcuteStamina(0.f, this->m_move.Speed / FPS, KeyControl::GetIsSquat())) {
					if (this->m_BackGround) {//todo:タイトル用仮
						SE->Get((int)SoundEnum::Heart).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 0.5f);
					}
				}
			}
		}
		//SetMat指示更新
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			//vector
			bool IsHitGround = (this->m_move.posbuf.y() <= 0.f); //高度0なら止まる
			float groundYpos = 0.f;
			if (!m_IsMainGame && this->m_BackGround) {
				VECTOR_ref EndPos = this->m_move.posbuf + VECTOR_ref::up() * 20.f;
				IsHitGround = this->m_BackGround->CheckLinetoMap(this->m_move.posbuf + VECTOR_ref::up() * -1.f, &EndPos, false);
				groundYpos = EndPos.y();
			}
			if (IsHitGround) {
				auto yPos = this->m_move.posbuf.y();
				Easing(&yPos, groundYpos, 0.8f, EasingType::OutExpo);
				this->m_move.posbuf.y(yPos);

				this->m_move.vec = KeyControl::GetVec();
				this->m_move.vec.y(0.f);
			}
			else {
				float Y = this->m_move.vec.y();
				this->m_move.vec = KeyControl::GetVec();
				this->m_move.vec.y(Y + (M_GR / (FPS * FPS)));
			}
			this->m_move.posbuf += this->m_move.vec;
			//壁判定
			std::vector<std::pair<MV1*, int>> cols;
			if (this->m_BackGround) {
				if (m_IsMainGame) {
					auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);
					for (auto& C : MainGB->GetBuildData()) {
						auto Vec = (C.GetMatrix().pos() - this->GetCharaPosition()); Vec.y(0.f);
						if (Vec.Length() <= 2.f*Scale_Rate) {
							cols.emplace_back(std::make_pair((MV1*)(&C.GetCol()), C.GetMeshSel()));
						}
					}
					for (const auto& C : MainGB->GetDrumControl().GetDrum()) {
						auto Vec = (C.m_Pos - this->GetCharaPosition()); Vec.y(0.f);
						if (Vec.Length() <= 2.f*Scale_Rate) {
							cols.emplace_back(std::make_pair((MV1*)(&C.m_Col), -1));
						}
					}
				}
				else {
					cols.emplace_back(std::make_pair((MV1*)(&this->m_BackGround->GetGroundCol()), -1));
				}
			}
			col_wall(this->m_move.repos, &this->m_move.posbuf, cols);
			//ほかプレイヤーとの判定
			if (m_IsMainGame) {
				float Radius = 2.f*0.5f*Scale_Rate;
				for (int i = 0; i < Player_num; i++) {
					if (i == this->m_MyID) { continue; }
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i).GetChara();
					if (!Chara->IsAlive()) { continue; }
					//自分が当たったら押し戻す
					VECTOR_ref Vec = (Chara->GetCharaPosition() - this->GetCharaPosition()); Vec.y(0.f);
					float Len = Vec.Length();
					if (Len < Radius) {
						this->m_move.posbuf += Vec.Norm() * (Len - Radius);
					}
				}
			}
			//座標オーバーライド
			if (KeyControl::PutOverride()) {
				this->m_move.posbuf = KeyControl::GetOverRideInfo().pos;
				this->m_move.vec = KeyControl::GetOverRideInfo().vec;
			}
			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->Get_HeadBobbing());
			this->m_move.mat = MATRIX_ref::RotZ(HeadBobbing ? (KeyControl::GetRad().z() / 2.f) : 0.f) * MATRIX_ref::RotY(KeyControl::GetYRadBottom());
			this->m_move.UpdatePosBuf(this->m_move.posbuf, 0.9f);
			UpdateMove();
			//スリング場所探索
			int GunSel = m_IsHardMode ? 1 : 0;
			if (GetGunPtr(GunSel)) {
				m_SlingMat[GunSel] =
					MATRIX_ref::RotX(deg2rad(-90))*
					GetFrameWorldMat(CharaFrame::Upper).GetRot()*
					GetCharaDir().Inverse()*
					MATRIX_ref::Mtrans(
						GetFrameWorldMat(CharaFrame::Upper).pos() +
						GetFrameWorldMat(CharaFrame::Upper).yvec() * (-0.5f*Scale_Rate) +
						GetFrameWorldMat(CharaFrame::Upper).xvec() * (-0.3f*Scale_Rate)
					);
			}
			GunSel = 1 - GunSel;
			if (GetGunPtr(GunSel)) {
				m_SlingZrad.Update();
				m_SlingZrad.AddRad(((1.f - m_SlingPer.at(GetOtherGunSelect())) + 0.5f  * (KeyControl::GetRad().y() - KeyControl::GetYRadBottom())) / FPS);
				m_SlingMat[GunSel] =
					MATRIX_ref::RotX(deg2rad(-30)) * MATRIX_ref::RotY(deg2rad(-90)) *
					(
						MATRIX_ref::RotZ(-this->m_SlingZrad.GetRad()) *
						MATRIX_ref::RotY(KeyControl::GetRad().y())*
						GetCharaDir().Inverse()
						)*
					MATRIX_ref::Mtrans(
						GetFrameWorldMat(CharaFrame::Head).pos() +
						GetFrameWorldMat(CharaFrame::Head).zvec() * (-0.3f*Scale_Rate) +
						(
						this->m_move.mat.xvec() * sin(m_SlingZrad.GetRad()) +
						this->m_move.mat.yvec() * cos(m_SlingZrad.GetRad())
						) * -(0.5f*Scale_Rate)
					);
			}
			//銃座標指定
			if ((this->m_ArmerWearPhase == ArmerWearPhase::Wear) || (this->m_MorphinePhase == MorphinePhase::Wear)) {
				//アーマー
				if (this->m_Armer_Ptr && this->m_Armer_Ptr->IsActive()) {
					MATRIX_ref tmp_gunrat = GetFrameWorldMat(CharaFrame::Head);
					tmp_gunrat = MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, -0.5f, 0.2f)*Scale_Rate*std::clamp(1.f - this->m_Wear_Armer.Per() * 1.5f, 0.f, 1.f)) * tmp_gunrat;
					tmp_gunrat = MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.15f, -0.5f)*Scale_Rate*(1.f - this->m_Wear_Armer.Per()))*tmp_gunrat;
					tmp_gunrat = MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, -0.30f, 0.f)*Scale_Rate*this->m_Wear_Armer.Per())*tmp_gunrat;
					tmp_gunrat = MATRIX_ref::RotX(deg2rad(-30.f)*(1.f - this->m_Wear_Armer.Per()))*tmp_gunrat;
					this->m_Armer_Ptr->SetMove(tmp_gunrat.GetRot(), tmp_gunrat.pos());
					this->m_Armer_Ptr->UpdateMove();

					VECTOR_ref GunPos = this->m_Armer_Ptr->GetRightHandMat().pos();
					VECTOR_ref Gunyvec = this->m_Armer_Ptr->GetRightHandMat().GetRot().xvec()*-1.f;
					VECTOR_ref Gunzvec = this->m_Armer_Ptr->GetRightHandMat().GetRot().yvec();
					VECTOR_ref HandsPos = this->m_Armer_Ptr->GetLeftHandMat().pos();
					VECTOR_ref Handsyvec = this->m_Armer_Ptr->GetLeftHandMat().GetRot().xvec();
					VECTOR_ref Handszvec = this->m_Armer_Ptr->GetLeftHandMat().GetRot().zvec()*-1.f;

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
			}
			else if (GetGunPtrNow()) {
				//銃ひっこめ
				if (this->m_BackGround) {
					if (m_StuckGunTimer == 0.f) {
						m_StuckGunTimer = 0.1f;
						auto EndPos = GetGunPtrNow()->GetFrameWorldMat(GunFrame::Muzzle).pos();
						if (this->m_BackGround->CheckLinetoMap(GetFrameWorldMat(CharaFrame::Head).pos(), &EndPos, false)) {//0.03ms
							m_IsStuckGun = true;
						}
						else {
							if (m_IsStuckGun) {
								GunReadyControl::SetAim();
							}
							m_IsStuckGun = false;
						}
					}
					else {
						m_StuckGunTimer = std::max(m_StuckGunTimer - 1.f / FPS, 0.f);
					}
					if (m_IsStuckGun) {
						GunReadyControl::SetReady();
					}
				}
				//オートエイム
				AutoAimControl::UpdateAutoAim(GetGunPtrNow()->IsAutoAimActive());
				//
				if (LifeControl::IsAlive()) {
					//銃の位置を指定するアニメ
					VECTOR_ref yVect0, zVect0, Post0;
					GunAnimNow AnimData;
					for (int i = 0; i < (int)EnumGunAnimType::Max; i++) {
						if (!GetGunPtrNow()->GetGunAnime((EnumGunAnimType)i,&AnimData)) { continue; }
						zVect0 = Lerp(zVect0, AnimData.GetRot().zvec(), this->m_Arm[i].Per());
						yVect0 = Lerp(yVect0, AnimData.GetRot().yvec(), this->m_Arm[i].Per());
						Post0 = Lerp(Post0, AnimData.GetPos(), this->m_Arm[i].Per());
					}
					Post0 = GetFrameWorldMat(CharaFrame::Head).pos() + MATRIX_ref::Vtrans(Post0, GetCharaDir());
					//
					this->m_ULTBar.Execute(IsGun0Select(),0.1f,0.1f,0.7f);
					//
					for (int index = 0;index < 2;index++) {
						auto& p = GetGunPtr(index);
						if (p) {
							bool IsSelGun = (index == GetNowGunSelect());
							auto& Mat = this->m_SlingMat[index];
							auto& Per = this->m_SlingPer[index];
							Easing(&Per, IsSelGun ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
							if (Per <= 0.01f) { Per = 0.f; }
							if (Per >= 0.99f) { Per = 1.f; }

							auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::front(), Lerp(Mat.zvec(), zVect0, Per));

							//オートエイム
							if (IsSelGun && GunReadyControl::GetIsADS() && GetGunPtrNow()->IsAutoAimActive() && m_IsMainGame) {
								VECTOR_ref BasePos = p->GetFrameWorldMat(GunFrame::Muzzle).pos();
								float Range = GetIsLaserActive() ? 13.f : 7.f;

								for (int i = 0; i < Player_num; i++) {
									if (i == this->m_MyID) { continue; }
									auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i).GetChara();
									if (!Chara->IsAlive()) { continue; }
									if (!Chara->CanLookTarget) { continue; }
									VECTOR_ref Vec = (Chara->GetFrameWorldMat(CharaFrame::Head).pos() - BasePos);
									if (Vec.Length() >= (Scale_Rate* Range)) { continue; }
									if ((GetEyeMatrix().zvec() * -1.f).dot(Vec.Norm()) < cos(deg2rad(15))) { continue; }

									AutoAimControl::SetAutoAimActive(i);
									break;
								}

								if (GetAutoAimActive()) {
									auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetAutoAimID()).GetChara();
									VECTOR_ref Vec = (Chara->GetFrameWorldMat(CharaFrame::Head).pos() - BasePos);
									tmp_gunrat = MATRIX_ref::RotVec2(GetEyeMatrix().zvec() * -1.f, Vec.Norm()) * tmp_gunrat;

								}
							}

							tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), Lerp(Mat.yvec(), yVect0, Per));
							tmp_gunrat *=
								KeyControl::GetGunSwingMat() * GetCharaDir()*
								MATRIX_ref::RotVec2(Lerp(p->GetMatrix().yvec(), p->GetEyeYVec(), GunReadyControl::GetADSPer()), p->GetMatrix().yvec())*
								MATRIX_ref::Mtrans(Lerp(Mat.pos(), Post0, Per));
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
					if (GetGunPtrOther()) {
						//仮の画面外指定
						auto tmp_gunrat = GetFrameWorldMat(CharaFrame::Upper);
						tmp_gunrat = MATRIX_ref::RotX(deg2rad(90))*tmp_gunrat;
						GetGunPtrOther()->SetGunMatrix(tmp_gunrat);
					}
				}
				//銃表示
				if (GetGunPtrOther()) {
					GetGunPtrOther()->SetActiveAll(true);
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
								switch (GetGunPtrNow()->GetGunAnime()) {
									case GunAnimeID::ReloadStart_Empty:
										if (GetGunPtrNow()->GetTimePer(GunAnimeID::ReloadStart_Empty) > 0.5f) {
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
										if (GetGunPtrNow()->GetTimePer(GunAnimeID::ReloadStart) > 0.8f) {
											m_MagHand = true;
										}
										break;
									case GunAnimeID::ReloadOne:
										if (GetGunPtrNow()->GetGunAnimePer(EnumGunAnimType::Reload) < 0.6f) {
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
								switch (GetGunPtrNow()->GetGunAnime()) {
									case GunAnimeID::ReloadStart_Empty:
										if (GetGunPtrNow()->GetTimePer(GunAnimeID::ReloadStart_Empty) > 0.8f) {
											m_MagHand = true;
										}
										break;
									case GunAnimeID::ReloadStart:
										if (GetGunPtrNow()->GetTimePer(GunAnimeID::ReloadStart) > 0.8f) {
											m_MagHand = true;
										}
										break;
									case GunAnimeID::ReloadOne:
										if (GetGunPtrNow()->GetGunAnimePer(EnumGunAnimType::Reload) < 0.6f) {
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
						if (GetCharaAction() == CharaActionID::Melee) {
							m_MagHand = IsGun0Select();
						}
						if (GetCharaAction() == CharaActionID::Watch) {
							if (m_ArmBreak) {
								m_MagHand = true;
							}
							else {
								if (this->IsGun0Select()) {
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
						switch (GetGunPtrNow()->GetGunAnime()) {
							case GunAnimeID::Cocking:
							case GunAnimeID::ReloadStart_Empty:
							case GunAnimeID::ReloadStart:
							case GunAnimeID::ReloadOne:
							case GunAnimeID::ReloadEnd:
							case GunAnimeID::CheckStart:
							case GunAnimeID::Checking:
							case GunAnimeID::CheckEnd:
							case GunAnimeID::AmmoLoading:
							case GunAnimeID::AmmoLoadEnd:
								StackLeftHandControl::ResetStackLeftHand();
								break;
							default:
								if (!GetRun() && !m_ArmBreak) {
									auto StartPos = GetFrameWorldMat(CharaFrame::Head).pos() + MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, -0.1f, -0.2f)*Scale_Rate, GetCharaDir());
									auto EndPos = StartPos + MATRIX_ref::Vtrans(VECTOR_ref::vget(0.5f, -0.1f, -0.7f)*Scale_Rate, GetCharaDir());
									VECTOR_ref Normal;
									if (this->m_BackGround && this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, true, &Normal)) {
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
						Handszvec = Lerp(Handszvec, StackLeftHandControl::GetStuckLeftHandYVec().cross(GetEyeMatrix().yvec()).Norm(), StackLeftHandControl::GetStuckLeftHandPer());
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
					switch (GetGunPtrNow()->GetGunAnime()) {
						case GunAnimeID::ReloadStart_Empty:
							m_AmmoHandR = 1.f;
							m_AmmoHand = 1.f;
							break;
						case GunAnimeID::ReloadStart:
							m_AmmoHandR = 1.f;
							m_AmmoHand = 1.f;
							break;
						case GunAnimeID::ReloadOne:
							if (GetGunPtrNow()->GetGunAnimePer(EnumGunAnimType::Reload) < 0.86f) {
								m_AmmoHandR = 1.f;
								m_AmmoHand = 1.f;
							}
							if (GetGunPtrNow()->GetGunAnimePer(EnumGunAnimType::Reload) > 0.96f) {
								m_AmmoHandR = 0.f;
								m_AmmoHand = 0.f;
							}
							break;
						case GunAnimeID::CheckStart:
							if (GetGunPtrNow()->GetTimePer(GunAnimeID::CheckStart) > 0.8f) {
								m_AmmoHandR = 1.f;
							}
						case GunAnimeID::Checking:
							if (GetGunPtrNow()->GetTimePer(GunAnimeID::Checking) < 0.8f) {
								m_AmmoHandR = 1.f;
								m_AmmoHand = 1.f;
							}
							if (GetGunPtrNow()->GetTimePer(GunAnimeID::Checking) > 0.8f) {
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
					if (GetGunPtrNow()->GetReloadType() == RELOADTYPE::AMMO) {
						m_AmmoHand = 0.f;
						isDirect = false;
					}
					(*GetGunPtrNow()->GetMagazinePtr())->SetHandMatrix(Mat, m_AmmoHand, isDirect);
					if (GetGunPtrOther()) {
						(*GetGunPtrOther()->GetMagazinePtr())->SetHandMatrix(Mat, 0.f, false);
					}
					Easing(&m_AmmoHand, m_AmmoHandR, 0.9f, EasingType::OutExpo);
				}
			}
		}
		//
		void			CharacterClass::ValueSet(PlayerID pID, bool IsMainGame, CharaTypeID value) noexcept {
			this->m_IsMainGame = IsMainGame;
			this->m_CharaType = value;
			ItemPopControl::RepopItem(this->m_BackGround);
			if (this->m_BackGround) {
				GetGunPtr(0)->SetFallObject(this->m_BackGround);
				if (GetGunPtr(1)) {
					GetGunPtr(1)->SetFallObject(this->m_BackGround);
				}
			}
			StaminaControl::InitStamina();
			LifeControl::InitLife();
			StackLeftHandControl::InitStackLeftHand();
			ULTControl::InitULT();
			//
			this->m_MyID = pID;
			//
			for (auto& a : this->m_Arm) { a.Init(false); }
			this->m_Arm[(int)EnumGunAnimType::Aim].Init(true);
			//
			this->m_MagArm.Init(false);
			this->m_MagHand = false;
			EyeSwingControl::InitEyeSwing();
			this->m_IsChanging = false;
			MorphinePtrControl::InitMorphinePtr();
			ArmerPtrControl::InitArmerPtr();
			//
			//サウンド
			this->m_CharaSound = -1;
			//動作にかかわる操作
			KeyControl::InitOverride();
			//銃のIDセットアップ
			if (GetGunPtr(0)) {
				GetGunPtr(0)->SetPlayerID(this->m_MyID);
			}
			if (GetGunPtr(1)) {
				GetGunPtr(1)->SetPlayerID(this->m_MyID);
			}
			m_SlingZrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(50));
			m_HPRec = 0.f;
			m_CharaAction = CharaActionID::Ready;
			m_ActionFirstFrame = true;
		}
		void			CharacterClass::MovePoint(float pxRad, float pyRad, const VECTOR_ref& pPos, int GunSel) noexcept {
			KeyControl::InitKey(pxRad, pyRad);
			this->m_move.vec.clear();
			this->m_move.repos = pPos;
			SetMove(MATRIX_ref::RotY(KeyControl::GetRad().y()), pPos);
			if (GetGunPtr(0)) {
				MagStockControl::Init_MagStockControl(GetGunPtr(0)->GetAmmoNum(), GetGunPtr(0)->GetAmmoAll(), GetGunPtr(0)->GetMagUniqueID());
			}
			SelectGun(GunSel);
			this->m_ULTBar.Init(IsGun0Select());
			this->m_SlingPer[0] = (IsGun0Select()) ? 0.f : 1.f;
			this->m_SlingPer[1] = 1.f - this->m_SlingPer[0];
			if (GunSel == 1) {
				m_ReadyAnim = 5.f;
				m_ReadyAnimPhase = 0;
			}
			else {
				m_ReadyAnim = -1.f;
			}
			GunReadyControl::SetReady();
			//
			this->m_MorphineStock = !m_IsHardMode ? 3 : 1;
			this->m_ArmerStock = 0;
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			auto* SE = SoundPool::Instance();
			KeyControl::InputKey(pInput, pReady, StaminaControl::GetHeartRandVec(0.f));
			//AIM
			if (GetGunPtrNow()) {
				if (pReady) {
					if (!m_IsChanging) {
						if (!m_ULTActive) {
							if (m_ULTUp > 0.5f) {
								m_ULTUp -= 0.5f;
								ULTControl::AddULT(1, true);
							}
							m_ULTUp += 1.f / FPS;
							if (ULTControl::IsULTActive() && KeyControl::GetULTKey()) {
								m_IsChanging = true;
								m_ULTActive = true;
								m_ULTUp = 0.f;
							}
						}
						else {
							if (m_ULTUp > 0.25f) {
								m_ULTUp -= 0.25f;
								ULTControl::AddULT(-1, true);
							}
							m_ULTUp += 1.f / FPS;
							if (ULTControl::GetULT() == 0 || (GetIsAim() && KeyControl::GetULTKey())) {
								m_IsChanging = true;
								m_ULTActive = false;
								m_ULTUp = 0.f;
								ULTControl::AddULT(-20, false);
							}
						}
					}
					if (IsAlive()) {
						if (IsLowHP()) {
							if (m_HPRec >= 0.f) {
								m_HPRec -= 2.f;
								Heal(2, false);
								if (m_MyID == 0) {
									SE->Get((int)SoundEnum::Man_breathing).Play(0, DX_PLAYTYPE_BACK);
								}
							}
							m_HPRec += 1.f / FPS;
						}
						else {
							if (m_HPRec != 0.f) {
								if (m_MyID == 0) {
									SE->Get((int)SoundEnum::Man_breathing).StopAll(0);
									SE->Get((int)SoundEnum::Man_breathend).Play(0, DX_PLAYTYPE_BACK);
								}
							}
							m_HPRec = 0.f;
						}
					}
				}
				if (m_IsChanging) {
					GunReadyControl::SetReady();
					if (IsLowReadyPer()) {
						GunReadyControl::SetAim();
						if (m_ULTActive) {
							SelectGun(1);
							GetGunPtrNow()->SetAmmo(GetGunPtrNow()->GetAmmoAll());
							GetGunPtrNow()->UnloadChamber();
							GetGunPtrNow()->CockByMag();
							GetGunPtrNow()->SetGunAnime(GunAnimeID::Base);
						}
						else {
							SelectGun(0);
						}
						m_IsChanging = false;
					}
				}
			}
		}
		//
		void			CharacterClass::Init(void) noexcept {
			ObjectBaseClass::Init();
			HitBoxControl::InitHitBox();
			m_ArmBreak = false;
			this->SetArmer(nullptr);
			this->SetMorphine(nullptr);
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {}
			this->m_SoundPower = std::max(this->m_SoundPower - 1.f / FPS, 0.f);
			GunReadyControl::UpdateReady();
			m_MeleeCoolDown = std::max(m_MeleeCoolDown - 1.f / FPS, 0.f);
			if (GetGunPtrNow()) {
				GetGunPtrNow()->SetShotSwitchOff();
				//リコイルの演算
				if (this->m_RecoilRadAdd.y() < 0.f) {
					Easing(&this->m_RecoilRadAdd, VECTOR_ref::vget(0.f, 0.01f, 0.f), GetGunPtrNow()->GetRecoilReturn(), EasingType::OutExpo);
				}
				else {
					Easing(&this->m_RecoilRadAdd, VECTOR_ref::zero(), 0.7f, EasingType::OutExpo);
				}
			}
			//
			ExecuteInput();
			if (GetGunPtrNow()) {
				m_SightChange.Execute(GetIsADS() && KeyControl::GetInputControl().GetPADSPress(PADS::JUMP));
				if (m_SightChange.trigger()) {
					GetGunPtrNow()->ChangeSightSel();
				}
			}
			//
			this->m_Arm[(int)EnumGunAnimType::ADS].Execute(GunReadyControl::GetIsADS(), 0.2f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::Run].Execute(!GetIsAim() && KeyControl::GetRun(), 0.1f, 0.2f, 0.95f);
			this->m_Arm[(int)EnumGunAnimType::ReloadStart_Empty].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadStart_Empty, 0.5f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::ReloadStart].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadStart, 0.2f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Reload].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadOne, 0.1f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::ReloadEnd].Execute(GetGunPtrNow()->GetGunAnime() == GunAnimeID::ReloadEnd, 0.1f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::Ready].Execute(!GetIsAim(), 0.1f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::Check].Execute(GetGunPtrNow()->GetGunAnime() >= GunAnimeID::CheckStart && GetGunPtrNow()->GetGunAnime() <= GunAnimeID::Checking, 0.05f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Watch].Execute(GetCharaAction() == CharaActionID::Watch, 0.1f, 0.1f);
			this->m_Arm[(int)EnumGunAnimType::Melee].Execute(GetCharaAction() == CharaActionID::Melee, 1.1f, 0.1f);
			this->m_Arm[(int)EnumGunAnimType::AmmoLoad].Execute(GetIsAim() && (GunAnimeID::AmmoLoading <= GetGunPtrNow()->GetGunAnime()) && (GetGunPtrNow()->GetGunAnime() <= GunAnimeID::AmmoLoadEnd), 0.1f, 0.1f);
			//
			KeyControl::UpdateKeyRad();
			GetObj().frame_Reset(GetFrame(CharaFrame::Upper));
			SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * MATRIX_ref::RotX(-KeyControl::GetRad().x() / 2.f) * (GetCharaDir()*this->m_move.mat.Inverse()).GetRot());
			GetObj().frame_Reset(GetFrame(CharaFrame::Upper2));
			SetFrameLocalMat(CharaFrame::Upper2, GetFrameLocalMat(CharaFrame::Upper2).GetRot() * MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f) * HitReactionControl::GetHitReactionMat());
			HitReactionControl::Execute_HitReactionControl();
			//上半身演算
			if (GetGunPtrNow()) {
				GetGunPtrNow()->UpdateGunAnims();
			}
			//上半身アニメ演算
			GetCharaAnimeBufID(CharaAnimeID::Upper_Ready) = 1.f;
			GetCharaAnimeBufID(CharaAnimeID::Hand_Ready) = 1.f;
			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomTurnAnimSel(), 0.5f);
			ObjectBaseClass::SetAnimLoop((int)CharaAnimeID::Bottom_Stand_Run, KeyControl::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomWalkAnimSel(), KeyControl::GetVecFront());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomLeftStepAnimSel(), KeyControl::GetVecLeft());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomWalkBackAnimSel(), KeyControl::GetVecRear());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomRightStepAnimSel(), KeyControl::GetVecRight());
			//アニメ反映
			for (int i = 0; i < GetObj().get_anime().size(); i++) {
				this->GetObj().get_anime(i).per = GetCharaAnimeBufID((CharaAnimeID)i);
			}
			GetObj().work_anime();
			//移動の際の視点制御
			EyeSwingControl::UpdateEyeSwing(GetCharaDir(), KeyControl::GetVec().Length() / 0.65f, KeyControl::GetRun() ? 8.f : 5.f);
			//
			ExecuteSound();
			//
			ExecuteMatrix();
			//
			RagDollControl::Execute_RagDollControl(this->GetObj(), LifeControl::IsAlive());													//ラグドール
			HitBoxControl::UpdataHitBox(this, (this->GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);									//ヒットボックス
			WalkSwingControl::UpdateWalkSwing(GetEyeMatrix().pos() - GetCharaPosition(), std::clamp(this->m_move.Speed / 2.f, 0.f, 1.f));
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
		void			CharacterClass::Dispose(void) noexcept {
			ObjectBaseClass::Dispose();
			ItemPopControl::DisposeItemPop();
			EffectControl::Dispose();
			this->m_BackGround.reset();
			GunPtrControl::DisposeGunPtr();
		}
	};
};
