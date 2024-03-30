#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"
#include "../../ObjectManager.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		const MATRIX_ref CharacterClass::GetEyeMatrix(void) const noexcept {
			auto tmpUpperMatrix =
				MATRIX_ref::RotZ(this->m_LeanRad / 5.f) *
				MATRIX_ref::RotX(KeyControl::GetRad().x()) *
				MATRIX_ref::RotY(this->m_yrad_BottomChange);

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
				EyePosition = Lerp<VECTOR_ref>(EyePosition, GunPtrControl::GetGunPtrNow()->GetEyePos(), GunReadyControl::GetADSPer());
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

						auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(value.ShotID).GetChara();
						if (Chara->GetAutoAimActive()) {

							Chara->AddULT(IsULTSelect() ? 1 : 6, Chara->IsGun0Select());
						}
						else {
							Chara->AddULT(IsULTSelect() ? 9 : 20, Chara->IsGun0Select());
						}
					}
					int Rand = GetRand(99);
					if (Rand < 45) {
						m_ItemFallControl.at(0).SetFall(GetCharaPosition() + VECTOR_ref::up()*(0.5f*Scale_Rate), VECTOR_ref::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / FPS);
					}
					else if (Rand < 45 + 15) {
						m_ItemFallControl.at(1).SetFall(GetCharaPosition() + VECTOR_ref::up()*(0.5f*Scale_Rate), VECTOR_ref::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / FPS);
					}
					else if (Rand < 45 + 15 + 20) {
						m_ItemFallControl.at(0).SetFall(GetCharaPosition() + VECTOR_ref::up()*(0.5f*Scale_Rate), VECTOR_ref::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / FPS);
						m_ItemFallControl.at(1).SetFall(GetCharaPosition() + VECTOR_ref::up()*(0.5f*Scale_Rate), VECTOR_ref::vget(GetRandf(2.f), 0.f, GetRandf(2.f))*Scale_Rate / FPS);
					}
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
			//��e����
			auto* HitPtr = HitBoxControl::GetLineHit(StartPos, pEndPos);
			if (HitPtr) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(AttackID).GetChara();
				//���ʃ_���[�W���Z
				if (CheckBodyParts) {
					switch (HitPtr->GetColType()) {
						case HitType::Head:
							if (m_MyID == 0) {
								*Damage = *Damage;
								*ArmerDamage = 0;

								SE->Get((int)SoundEnum::Tinnitus).Play_3D(0, GetCharaPosition(), Scale_Rate*50.f);
								m_HeadShotSwitch = true;
								m_HeadShotPer = 1.f;
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
				//�_���[�W�o�^
				{
					//auto v1 = GetCharaDir().zvec() * -1.f;
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
				//�G�t�F�N�g�Z�b�g
				if (*Damage > 0) {
					EffectControl::SetOnce(EffectResource::Effect::ef_hitblood, *pEndPos, VECTOR_ref::front(), Scale_Rate);
					EffectControl::SetEffectSpeed(EffectResource::Effect::ef_hitblood, 2.f);
				}
				else {
					EffectControl::SetOnce(EffectResource::Effect::ef_gndsmoke, *pEndPos, (Chara->GetCharaPosition() - this->GetCharaPosition()).Norm(), 0.25f * Scale_Rate);
				}
				//�q�b�g���[�V����
				{
					HitReactionControl::SetHit(MATRIX_ref::Vtrans((*pEndPos - StartPos).Norm().cross(VECTOR_ref::up())*-1.f, GetFrameWorldMat(CharaFrame::Upper2).GetRot().Inverse()));
					if (HitPtr->GetColType() == HitType::Leg) {
						KeyControl::SetIsSquat(true);
					}
					if (HitPtr->GetColType() == HitType::Arm && !m_ArmBreak) {
						m_ArmBreak = true;
					}
				}
				//todo : �q�b�g���������ɋ߂����_��Ԃ�����
				return true;
			}
			return false;
		}
		const float		CharacterClass::GetSpeedPer(void) const noexcept {
			if (KeyControl::GetInputControl().GetPADSPress(PADS::WALK)) {
				return 0.15f;
			}
			if (KeyControl::GetIsSquat()) {
				return 0.45f;
			}
			else if (KeyControl::GetRun()) {
				return 0.875f;
			}
			else {
				return 0.65f;
			}
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
			//�
			auto vec_a1 = MATRIX_ref::Vtrans((RetPos - GetFrameWorldMat(CharaFrame::RightArm).pos()).Norm(), matBase);//�
			auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(-1.5f, -1.f, vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0�Ƃ���
			float cos_t = GetCosFormula((GetFrameWorldMat(CharaFrame::RightWrist).pos() - GetFrameWorldMat(CharaFrame::RightArm2).pos()).size(), (GetFrameWorldMat(CharaFrame::RightArm2).pos() - GetFrameWorldMat(CharaFrame::RightArm).pos()).size(), (GetFrameWorldMat(CharaFrame::RightArm).pos() - RetPos).size());
			auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//��r
			SetFrameLocalMat(CharaFrame::RightArm, MGetIdent());
			MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::RightArm2).pos() - GetFrameWorldMat(CharaFrame::RightArm).pos(), matBase), vec_t);
			SetFrameLocalMat(CharaFrame::RightArm, a1_inv);
			//���r
			matBase = GetParentFrameWorldMat(CharaFrame::RightArm2).GetRot().Inverse();
			SetFrameLocalMat(CharaFrame::RightArm2, MGetIdent());
			MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
				MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::RightWrist).pos() - GetFrameWorldMat(CharaFrame::RightArm2).pos(), matBase),
				MATRIX_ref::Vtrans(RetPos - GetFrameWorldMat(CharaFrame::RightArm2).pos(), matBase));
			SetFrameLocalMat(CharaFrame::RightArm2, a2_inv);
			//��
			auto AngleOf2Vector = [&](VECTOR_ref A, VECTOR_ref B) { return acos(A.dot(B) / (A.Length() * B.Length())); };			//�Q�̃x�N�g��AB�̂Ȃ��p�x�Ƃ����߂�
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
			//�
			auto vec_a1 = MATRIX_ref::Vtrans((RetPos - GetFrameWorldMat(CharaFrame::LeftArm).pos()).Norm(), matBase);//�
			auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(1.5f, -1.f, vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0�Ƃ���
			float cos_t = GetCosFormula((GetFrameWorldMat(CharaFrame::LeftWrist).pos() - GetFrameWorldMat(CharaFrame::LeftArm2).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm2).pos() - GetFrameWorldMat(CharaFrame::LeftArm).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm).pos() - RetPos).size());
			auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//��r
			SetFrameLocalMat(CharaFrame::LeftArm, MGetIdent());
			MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::LeftArm2).pos() - GetFrameWorldMat(CharaFrame::LeftArm).pos(), matBase), vec_t);
			SetFrameLocalMat(CharaFrame::LeftArm, a1_inv);
			//���r
			matBase = GetParentFrameWorldMat(CharaFrame::LeftArm2).GetRot().Inverse();
			SetFrameLocalMat(CharaFrame::LeftArm2, MGetIdent());
			MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
				MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::LeftWrist).pos() - GetFrameWorldMat(CharaFrame::LeftArm2).pos(), matBase),
				MATRIX_ref::Vtrans(RetPos - GetFrameWorldMat(CharaFrame::LeftArm2).pos(), matBase));
			SetFrameLocalMat(CharaFrame::LeftArm2, a2_inv);
			//��
			auto AngleOf2Vector = [&](VECTOR_ref A, VECTOR_ref B) { return acos(A.dot(B) / (A.Length() * B.Length())); };			//�Q�̃x�N�g��AB�̂Ȃ��p�x�Ƃ����߂�
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

		void			CharacterClass::LoadExtends() noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			//�A�[�}�[
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
		//����
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			auto* ItemLogParts = GetItemLog::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			//
			GunReadyControl::UpdateReady();
			if ((
				this->m_Press_Shot ||
				this->m_Press_Reload ||
				((KeyControl::GetInputControl().GetPADSPress(PADS::RELOAD) && GetIsCheck()) && GetGunPtrNow()->GetCanShot()) ||
				(KeyControl::GetInputControl().GetPADSPress(PADS::MELEE) && GetGunPtrNow()->GetCanShot())
				) && (GetShotPhase() == GunAnimeID::Base) && !m_IsChanging) {
				GunReadyControl::SetAimOrADS();
			}

			//�ˌ�
			if (GetGunPtrNow()) {
				//���R�C���̉��Z
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
								GunReadyControl::SetReady();
								if (IsLowReadyPer()) {
									GunReadyControl::SetAim();
									SelectGun(0);
									m_ReadyAnimPhase++;
									DrawParts->SetCamShake(0.1f, 2.f);
								}
							}
							break;
						case 3:
							{
								if (!GetGunPtrNow()->GetChecking() && IsAimPer()) {
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
				if ((GetShotPhase() == GunAnimeID::Base) && IsAimPer()) {
					if (this->m_Press_Shot) {
						if (GetGunPtrNow()->GetInChamber()) {
							GetGunPtrNow()->SetBullet();
							if (IsULTSelect()) {
								ULTControl::AddULT(-3, true);
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
							//�G�t�F�N�g
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
									if (IsGun0Select()) {
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
									if (IsGun0Select()) {
										//todo:�e�c���ɉ����ĕύX
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
							if (GetRand(1) == 0) {
								SE->Get((int)SoundEnum::Man_reload).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
							}
							else {
								SE->Get((int)SoundEnum::Man_takecover).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
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
					//�e�X�g�n
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
						GunReadyControl::SetADS();
					}
					else {
						GunReadyControl::SetAim();
					}
				}

				if (GetShotPhase() == GunAnimeID::AmmoLoadStart || GetShotPhase() == GunAnimeID::AmmoLoadEnd) {
					GunReadyControl::SetReady();
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
					GunReadyControl::SetAim();
				}
			}
			if (KeyControl::GetRun()) {
				GunReadyControl::SetReady();
			}

			//�A�[�}�[�𒅂�
			switch (this->m_ArmerWearPhase) {
				case ArmerWearPhase::Ready:
					break;
				case ArmerWearPhase::Have:
					if (IsLowReadyPer()) {
						this->m_ArmerWearPhase = ArmerWearPhase::Wear;
						this->m_Wear_Armer.Init(false);
						if (this->m_Armer_Ptr) {
							this->m_Armer_Ptr->SetActive(true);
						}
						SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
					}
					GunReadyControl::SetReady();
					break;
				case ArmerWearPhase::Wear:
					this->m_Wear_Armer.Execute(1.f, 0.1f, 0.1f, 0.95f);
					if ((this->m_Wear_Armer.Per() < 0.f) || (1.f < this->m_Wear_Armer.Per())) {
						this->m_ArmerWearPhase = ArmerWearPhase::Ready;
						if (this->m_Armer_Ptr) {
							this->m_Armer_Ptr->SetActive(false);
						}
						if (m_IsMainGame) {
							ItemLogParts->AddLog(3.f, GetColor(25, 122, 75), "�A�[�}�[�𒅗p +%4d", 10);
							PlayerMngr->GetPlayer(GetMyPlayerID()).AddScore(10);
						}

						GunReadyControl::SetAim();
						LifeControl::SetHealEvent(this->m_MyID, this->m_MyID, 0, LifeControl::GetAPMax());
					}
					break;
				default:
					break;
			}
			//Morphine
			switch (this->m_MorphinePhase) {
				case MorphinePhase::Ready:
					break;
				case MorphinePhase::Have:
					if (IsLowReadyPer()) {
						this->m_MorphinePhase = MorphinePhase::Wear;
						this->m_Wear_Morphine.Init(false);
						if (this->m_Morphine_Ptr) {
							this->m_Morphine_Ptr->SetActive(true);
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
						this->m_MorphinePhase = MorphinePhase::Ready;
						if (this->m_Morphine_Ptr) {
							this->m_Morphine_Ptr->SetActive(false);
						}
						GunReadyControl::SetAim();
						if (m_IsMainGame) {
							ItemLogParts->AddLog(3.f, GetColor(25, 122, 75), "�����q�l���� +%4d", 50);
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

			if (!isWearingArmer() && !isMorphineing()) {
				if (KeyControl::GetInputControl().GetPADSPress(PADS::THROW)) {
					//�����q�l
					if ((this->m_MorphineStock != 0) && (LifeControl::GetHP() < LifeControl::GetHPMax())) {
						this->m_MorphineStock = std::max(this->m_MorphineStock - 1, 0);
						this->m_MorphinePhase = MorphinePhase::Have;
					}
				}
				if (KeyControl::GetInputControl().GetPADSPress(PADS::CHECK)) {
					//�A�[�}�[���p
					if ((this->m_ArmerStock != 0) && (LifeControl::GetAP() < LifeControl::GetAPMax())) {
						this->m_ArmerStock = std::max(this->m_ArmerStock - 1, 0);
						this->m_ArmerWearPhase = ArmerWearPhase::Have;
					}
					else if (GetGunPtrNow() && (GetShotPhase() == GunAnimeID::Base)) {
						if (
							IsGun0Select() &&
							MagStockControl::GetNeedAmmoLoad(GetGunPtrNow()->GetIsMagFull(), GetGunPtrNow()->GetIsMagEmpty()) &&
							!GetGunPtrNow()->IsAmmoLoading()
							) {
							GetGunPtrNow()->SetShotPhase(GunAnimeID::AmmoLoadStart);
						}
						else if (GetGunPtrNow()->GetCanShot()) {
							GetGunPtrNow()->SetShotPhase(GunAnimeID::Watch);
						}
					}
				}
			}
			//
			this->m_Arm[(int)EnumGunAnimType::ADS].Execute(GunReadyControl::GetIsADS(), 0.2f, 0.2f, 0.9f);
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
			if (!KeyControl::IsMove()) {
				auto* OptionParts = OPTION::Instance();
				bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->Get_HeadBobbing());
				if (HeadBobbing) {
					if (deg2rad(50.f) < abs(this->m_yrad_UpperChange)) {
						this->m_TurnBody = true;
					}
					if (abs(this->m_yrad_UpperChange) < deg2rad(0.5f)) {
						this->m_TurnBody = false;
					}
				}
				else {
					this->m_TurnBody = true;
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
			if (this->m_TurnBody || KeyControl::IsMove()) { Easing(&this->m_yrad_Upper, KeyControl::GetRad().y(), 0.85f, EasingType::OutExpo); }
			auto YradChange = this->m_yrad_Bottom;
			Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - KeyControl::GetFrontP(), 0.85f, EasingType::OutExpo);
			YradChange = this->m_yrad_Bottom - YradChange;
			float Z = KeyControl::SetRadBuf().z();
			Easing(&Z, (abs(YradChange) > deg2rad(10)) ? 0.f : std::clamp(YradChange * 3.f, -deg2rad(10), deg2rad(10)), 0.9f, EasingType::OutExpo);
			KeyControl::SetRadBuf().z(Z);
			this->m_yrad_UpperChange = KeyControl::GetRad().y() - this->m_yrad_Upper;
			this->m_yrad_BottomChange = KeyControl::GetRad().y() - this->m_yrad_Bottom;
		}
		//�㔼�g��]
		void			CharacterClass::ExecuteUpperMatrix(void) noexcept {
			Easing(&this->m_LateLeanRad, this->m_LeanRad, 0.9f, EasingType::OutExpo);
			Easing(&this->m_LeanRad, ((float)(-KeyControl::GetLeanRate()) + this->m_HeadShotPer)*deg2rad(25), 0.9f, EasingType::OutExpo);
			Easing(&this->m_HeadShotPer, 0.f, 0.9f, EasingType::OutExpo);

			auto tmpUpperMatrix =
				MATRIX_ref::RotZ(this->m_LeanRad) *
				MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f) *
				MATRIX_ref::RotY(Lerp(this->m_yrad_BottomChange, 0.f, this->m_Arm[(int)EnumGunAnimType::Run].Per()));

			GetObj().frame_Reset(GetFrame(CharaFrame::Upper));
			SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * tmpUpperMatrix);
			GetObj().frame_Reset(GetFrame(CharaFrame::Upper2));
			SetFrameLocalMat(CharaFrame::Upper2, GetFrameLocalMat(CharaFrame::Upper2).GetRot() * MATRIX_ref::RotX(KeyControl::GetRad().x() / 2.f) * HitReactionControl::GetHitReactionMat());
			HitReactionControl::Execute_HitReactionControl();
		}
		//SetMat�w��
		void			CharacterClass::ExecuteAnim(void) noexcept {
			//�㔼�g���Z
			if (GetGunPtrNow()) {
				GetGunPtrNow()->UpdateGunAnims(this->m_Press_Shot);
			}
			//�A�j�����Z
			{
				//�㔼�g
				this->m_AnimPerBuf.at((int)CharaAnimeID::Upper_Ready) = 1.f;
				this->m_AnimPerBuf.at((int)CharaAnimeID::Hand_Ready) = 1.f;
				//�����g
				this->m_BottomAnimSelect = GetBottomStandAnimSel();
				if (KeyControl::GetInputControl().GetPADSPress(PADS::MOVE_A)) { this->m_BottomAnimSelect = GetBottomLeftStepAnimSel(); }
				if (KeyControl::GetInputControl().GetPADSPress(PADS::MOVE_D)) { this->m_BottomAnimSelect = GetBottomRightStepAnimSel(); }
				if (KeyControl::GetInputControl().GetPADSPress(PADS::MOVE_S)) { this->m_BottomAnimSelect = GetBottomWalkBackAnimSel(); }
				if (KeyControl::GetInputControl().GetPADSPress(PADS::MOVE_W)) { this->m_BottomAnimSelect = KeyControl::GetRun() ? CharaAnimeID::Bottom_Stand_Run : GetBottomWalkAnimSel(); }
				ObjectBaseClass::SetAnimLoop((int)GetBottomTurnAnimSel(), 0.5f);
				ObjectBaseClass::SetAnimLoop((int)CharaAnimeID::Bottom_Stand_Run, 1.f * KeyControl::GetVecFront() * GetSpeedPer());

				float Per = std::clamp(GetSpeedPer() / 0.65f, 0.5f, 1.f);
				ObjectBaseClass::SetAnimLoop((int)GetBottomWalkAnimSel(), 1.15f * KeyControl::GetVecFront() * Per);
				ObjectBaseClass::SetAnimLoop((int)GetBottomLeftStepAnimSel(), 1.15f * KeyControl::GetVecLeft() * Per);
				ObjectBaseClass::SetAnimLoop((int)GetBottomWalkBackAnimSel(), 1.15f * KeyControl::GetVecRear() * Per);
				ObjectBaseClass::SetAnimLoop((int)GetBottomRightStepAnimSel(), 1.15f * KeyControl::GetVecRight() * Per);
				Easing(&this->m_AnimPerBuf.at((int)GetBottomTurnAnimSel()), (this->m_TurnBody) ? abs(this->m_yrad_UpperChange) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					if (
						i == (int)CharaAnimeID::Bottom_Stand ||
						i == (int)CharaAnimeID::Bottom_Stand_Run ||
						i == (int)CharaAnimeID::Bottom_Stand_Walk ||
						i == (int)CharaAnimeID::Bottom_Stand_LeftStep ||
						i == (int)CharaAnimeID::Bottom_Stand_RightStep ||
						i == (int)CharaAnimeID::Bottom_Stand_WalkBack ||
						i == (int)CharaAnimeID::Bottom_Stand_Turn) {
						this->m_AnimPerBuf.at((int)(CharaAnimeID)i) += ((i == (int)this->m_BottomAnimSelect) ? 6.f : -2.f) / FPS;
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
						this->m_AnimPerBuf.at((int)(CharaAnimeID)i) += ((i == (int)this->m_BottomAnimSelect) ? 2.f : -2.f) / FPS;
					}
				}
				FlipAnime();
			}
			//�ړ��̍ۂ̎��_����
			EyeSwingControl::UpdateEyeSwing(GetCharaDir(), KeyControl::GetMoverPer(), KeyControl::GetRun() ? 8.f : 5.f);
		}
		//���w��
		void			CharacterClass::ExecuteSound(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			//����
			if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
				auto Time = this->GetObj().get_anime((int)this->m_BottomAnimSelect).time;
				if (this->m_BottomAnimSelect != CharaAnimeID::Bottom_Stand_Run) {
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
			//���Ⴊ�݉�
			if (KeyControl::GetSquatSwitch()) {
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale_Rate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//���[����
			if (KeyControl::GetLeanSwitch()) {
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper).pos(), Scale_Rate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//�S����
			if (this->m_MyID == 0) {
				if (StaminaControl::ExcuteStamina(0.f, this->m_move.Speed / FPS, KeyControl::GetIsSquat())) {
					SE->Get((int)SoundEnum::Heart).Play_3D(0, GetFrameWorldMat(CharaFrame::Head).pos(), Scale_Rate * 0.5f);
				}
			}
		}
		//SetMat�w���X�V
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto* AnimMngr = GunAnimManager::Instance();
			//vector
			{
				VECTOR_ref vecBuf = KeyControl::GetVec();
				if (KeyControl::GetMoverPer() > 0.f) {
					vecBuf = vecBuf.Norm() * (GetSpeedPer() * 60.f / FPS);
				}
				vecBuf = MATRIX_ref::Vtrans(vecBuf, MATRIX_ref::RotY(this->m_yrad_Upper));
				bool IsHitGround = false;
				float groundYpos = 0.f;
				if (m_IsMainGame) {
					IsHitGround = (this->m_move.posbuf.y() <= 0.f); //���x0�Ȃ�~�܂�
					groundYpos = 0.f;
				}
				else if (this->m_BackGround) {
					VECTOR_ref EndPos = this->m_move.posbuf + VECTOR_ref::up() * 20.f;
					IsHitGround = this->m_BackGround->CheckLinetoMap(this->m_move.posbuf + VECTOR_ref::up() * -1.f, &EndPos, false);
					groundYpos = EndPos.y();
				}
				if (IsHitGround) {
					auto yPos = this->m_move.posbuf.y();
					Easing(&yPos, groundYpos, 0.8f, EasingType::OutExpo);
					this->m_move.posbuf.y(yPos);
					vecBuf.y(this->m_move.vec.y());
				}
				else {
					vecBuf.y(this->m_move.vec.y() + (M_GR / (FPS * FPS)));
				}
				this->m_move.vec = vecBuf;
			}
			this->m_move.posbuf += this->m_move.vec;
			//�ǔ���
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
			//�ق��v���C���[�Ƃ̔���
			if (m_IsMainGame) {
				float Radius = 2.f*0.5f*Scale_Rate;
				for (int i = 0; i < Player_num; i++) {
					if (i == this->m_MyID) { continue; }
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i).GetChara();
					if (!Chara->IsAlive()) { continue; }
					//���������������牟���߂�
					VECTOR_ref Vec = (Chara->GetCharaPosition() - this->GetCharaPosition()); Vec.y(0.f);
					float Len = Vec.Length();
					if (Len < Radius) {
						this->m_move.posbuf += Vec.Norm() * (Len - Radius);
					}
				}
			}
			//
			if (OverrideControl::PutOverride()) {
				auto& info_move = OverrideControl::GetOverRideInfo();
				this->m_move.posbuf = info_move.pos;
				this->m_move.vec = info_move.vec;

				float X = KeyControl::SetRadBuf().x();
				Easing(&X, info_move.rad.x(), 0.9f, EasingType::OutExpo);
				KeyControl::SetRadBuf().x(X);
				KeyControl::SetRadBuf().y(info_move.rad.y());
				KeyControl::SetRad().y(info_move.rad.y());
			}
			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((GetMyPlayerID() != 0) || OptionParts->Get_HeadBobbing());
			this->m_move.mat = MATRIX_ref::RotZ(HeadBobbing ? (KeyControl::GetRad().z() / 2.f) : 0.f) * MATRIX_ref::RotY(this->m_yrad_Bottom);
			this->m_move.UpdatePosBuf(this->m_move.posbuf, 0.9f);
			UpdateMove();
			//�e�Ђ�����
			if (GetGunPtrNow() && this->m_BackGround) {
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
			//�X�����O�ꏊ�T��//0.02ms��
			if (GetGunPtr(1)) {
				m_SlingZrad.Update();
				m_SlingZrad.AddRad(1.f / FPS * (1.f - m_SlingPer.at(GetOtherGunSelect())));
				m_SlingZrad.AddRad(1.f / FPS * (KeyControl::GetRad().y() - this->m_yrad_Bottom));
			}
			if (GetGunPtr(0)) {
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
			if (GetGunPtr(1)) {
				m_SlingMat[1] = 
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
			//�e���W�w��
			if (!(this->m_ArmerWearPhase== ArmerWearPhase::Wear) && !(this->m_MorphinePhase == MorphinePhase::Wear)) {
				if (GetGunPtrNow()) {
					//�I�[�g�G�C��
					AutoAimControl::UpdateAutoAim(IsAutoAimActive());
					//
					if (LifeControl::IsAlive()) {
						//�e�̈ʒu���w�肷��A�j��
						VECTOR_ref yVect0, zVect0, Post0;
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
						//�e�̗h��
						yVect0 = MATRIX_ref::Vtrans(yVect0, MATRIX_ref::RotAxis(zVect0, GetGunRadAdd() - this->m_LeanRad));
						//�G�C����
						GunSwingControl::UpdateGunSwing(KeyControl::GetRad());
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

								//�I�[�g�G�C��
								if (IsSelGun && GunReadyControl::GetIsADS() && IsAutoAimActive() && m_IsMainGame) {
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
									GunSwingControl::GetGunSwingMat() * GetCharaDir()*
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
						auto& GunPtr = GetGunPtrOther();
						if (GunPtr) {
							//���̉�ʊO�w��
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
					//��̈ʒu�𐧌�
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
						//�r���W�w��
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
				if (this->m_Armer_Ptr && isWearingArmer()) {
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
				if (GetGunPtrNow()) {
					//���̉�ʊO�w��
					auto tmp_gunrat = GetFrameWorldMat(CharaFrame::Upper);
					tmp_gunrat = MATRIX_ref::RotX(deg2rad(90))*tmp_gunrat;
					GetGunPtrNow()->SetGunMatrix(tmp_gunrat);
				}
				auto& GunPtr = GetGunPtrOther();
				if (GunPtr) {
					//���̉�ʊO�w��
					auto tmp_gunrat = GetFrameWorldMat(CharaFrame::Upper);
					tmp_gunrat = MATRIX_ref::RotX(deg2rad(90))*tmp_gunrat;
					GunPtr->SetGunMatrix(tmp_gunrat);
				}
			}
			//���O�h�[��//0.01ms��
			RagDollControl::Execute_RagDollControl(this->GetObj(), LifeControl::IsAlive());
			//�q�b�g�{�b�N�X
			HitBoxControl::UpdataHitBox(this, (this->GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);
			//
			WalkSwingControl::UpdateWalkSwing(GetEyeMatrix().pos() - GetCharaPosition(), std::clamp(this->m_move.Speed / 2.f, 0.f, 1.f));
		}

		//
		void			CharacterClass::SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround, bool IsMainGame) noexcept {
			m_BackGround.reset();
			m_ItemFallControl.at(0).Dispose();
			m_ItemFallControl.at(1).Dispose();
			this->m_BackGround = backGround;
			this->m_IsMainGame = IsMainGame;
			m_ItemFallControl.at(0).Init(this->m_BackGround, "data/model/AmmoBox/", ItemType::AMMO);
			m_ItemFallControl.at(1).Init(this->m_BackGround, "data/model/Armer/", ItemType::ARMER);
			GetGunPtr(0)->SetMapCol(this->m_BackGround);
			if (GetGunPtr(1)) {
				GetGunPtr(1)->SetMapCol(this->m_BackGround);
			}
		}
		void			CharacterClass::ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos, PlayerID pID, int GunSel) noexcept {
			StaminaControl::InitStamina();
			LifeControl::InitLife();
			KeyControl::InitKey(pxRad, pyRad);
			StackLeftHandControl::InitStackLeftHand();
			ULTControl::InitULT();
			//
			this->m_MyID = pID;
			//
			for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
			for (auto& a : this->m_Arm) { a.Init(false); }
			this->m_Arm[(int)EnumGunAnimType::Aim].Init(true);
			//
			GunReadyControl::SetReady();

			this->m_MagArm.Init(false);
			this->m_MagHand = false;

			this->m_TurnBody = false;

			EyeSwingControl::InitEyeSwing();

			this->m_Press_Shot = false;
			this->m_Press_Reload = false;
			this->m_Press_Aim = false;
			this->m_IsChanging = false;

			MorphinePtrControl::InitMorphinePtr();
			ArmerPtrControl::InitArmerPtr();

			this->m_MorphineStock = 1;
			this->m_ArmerStock = 0;
			//
			this->m_ArmBreak = false;
			//�T�E���h
			this->m_CharaSound = -1;
			//����ɂ�����鑀��
			OverrideControl::InitOverride();
			//��L�𔽉f�������
			this->m_yrad_Upper = KeyControl::GetRad().y();
			this->m_yrad_Bottom = KeyControl::GetRad().y();
			this->m_yrad_BottomChange = 0.f;
			this->m_LateLeanRad = 0.f;
			this->m_LeanRad = 0.f;
			this->m_move.vec.clear();
			SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), pPos);
			//�e��ID�Z�b�g�A�b�v
			if (GetGunPtrNow()) {
				GetGunPtrNow()->SetPlayerID(this->m_MyID);
				MagStockControl::Init_MagStockControl(GetGunPtrNow()->GetAmmoNum(), GetGunPtrNow()->GetAmmoAll(), GetGunPtrNow()->GetMagUniqueID());
			}

			m_SlingZrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(50));

			SelectGun(GunSel);
			this->m_SlingPer[0] = (IsGun0Select()) ? 0.f : 1.f;
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
			auto* SE = SoundPool::Instance();
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
								ULTControl::AddULT(1, true);
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
								ULTControl::AddULT(-1, true);
							}
							m_ULTUp += 1.f / FPS;
							if (ULTControl::GetULT() == 0 || (GetIsAim() && KeyControl::GetULTKey().trigger())) {
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
							GetGunPtrNow()->SetShotPhase(GunAnimeID::Base);
						}
						else {
							SelectGun(0);
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

		//
		void			CharacterClass::Init(void) noexcept {
			ObjectBaseClass::Init();
			RagDollControl::Init_RagDollControl(this->GetObj());
			for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
			this->GetObj().get_anime((int)GetBottomStandAnimSel()).per = 1.f;
			HitBoxControl::InitHitBox();
			m_ArmBreak = false;
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			//����̂ݍX�V������e
			if (this->m_IsFirstLoop) {
				FlipAnime();
			}
			this->m_SoundPower = std::max(this->m_SoundPower - 1.f / FPS, 0.f);
			ExecuteInput();
			ExecuteUpperMatrix();
			ExecuteAnim();
			ExecuteSound();
			ExecuteMatrix();
			ExecuteShape();
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
					fog_enable = GetFogEnable();													// �t�H�O���L�����ǂ������擾����( TRUE:�L�� FALSE:���� )
					fog_mode = GetFogMode();														// �t�H�O���[�h���擾����
					GetFogColor(&fog_r, &fog_g, &fog_b);											// �t�H�O�J���[���擾����
					GetFogStartEnd(&fog_start, &fog_end);											// �t�H�O���n�܂鋗���ƏI�����鋗�����擾����( 0.0f �` 1.0f )
					fog_density = GetFogDensity();													// �t�H�O�̖��x���擾����( 0.0f �` 1.0f )


					//�L�����`��
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
					//hitbox�`��
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
			EffectControl::Dispose();
			this->m_BackGround.reset();
			GunPtrControl::DisposeGunPtr();
		}
	};
};
