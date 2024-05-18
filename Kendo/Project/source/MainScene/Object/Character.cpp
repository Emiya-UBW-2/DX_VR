#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			CharacterClass::move_RightArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept {
			GetObj().frame_Reset(GetFrame((int)CharaFrame::RightArm));
			GetObj().frame_Reset(GetFrame((int)CharaFrame::RightArm2));
			GetObj().frame_Reset(GetFrame((int)CharaFrame::RightWrist));
			auto matBase = GetParentFrameWorldMatrix(GetFrame((int)CharaFrame::RightArm)).rotation().inverse();

			Vector3DX Gunxvec = Vector3DX::Cross(Gunzvec, Gunyvec*-1.f)*-1.f;

			Vector3DX Localyvec = Vector3DX::vget(0.f, 0.f, -1.f).normalized();
			Vector3DX Localzvec = Vector3DX::vget(-1.f, -1.f, 0.f).normalized();

			Vector3DX RetPos = GunPos;
			//基準
			auto vec_a1 = Matrix4x4DX::Vtrans((RetPos - GetFrameWorldMat(GetFrame((int)CharaFrame::RightArm)).pos()).normalized(), matBase);//基準
			Vector3DX vec_a1L1 = Vector3DX::vget(-0.5f, -1.f, vec_a1.y / -abs(vec_a1.z)).normalized();//x=0とする
			float cos_t = GetCosFormula((GetFrameWorldMat(GetFrame((int)CharaFrame::RightWrist)).pos() - GetFrameWorldMat(GetFrame((int)CharaFrame::RightArm2)).pos()).magnitude(), (GetFrameWorldMat(GetFrame((int)CharaFrame::RightArm2)).pos() - GetFrameWorldMat(GetFrame((int)CharaFrame::RightArm)).pos()).magnitude(), (GetFrameWorldMat(GetFrame((int)CharaFrame::RightArm)).pos() - RetPos).magnitude());
			auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//上腕
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::RightArm), Matrix4x4DX::identity() * GetFrameBaseLocalMat((int)CharaFrame::RightArm));
			Matrix4x4DX a1_inv = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(GetFrameWorldMat(GetFrame((int)CharaFrame::RightArm2)).pos() - GetFrameWorldMat(GetFrame((int)CharaFrame::RightArm)).pos(), matBase), vec_t);
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::RightArm), a1_inv * GetFrameBaseLocalMat((int)CharaFrame::RightArm));
			//下腕
			matBase = GetParentFrameWorldMatrix(GetFrame((int)CharaFrame::RightArm2)).rotation().inverse();
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::RightArm2), Matrix4x4DX::identity() * GetFrameBaseLocalMat((int)CharaFrame::RightArm2));
			Matrix4x4DX a2_inv = Matrix4x4DX::RotVec2(
				Matrix4x4DX::Vtrans(GetFrameWorldMat(GetFrame((int)CharaFrame::RightWrist)).pos() - GetFrameWorldMat(GetFrame((int)CharaFrame::RightArm2)).pos(), matBase),
				Matrix4x4DX::Vtrans(RetPos - GetFrameWorldMat(GetFrame((int)CharaFrame::RightArm2)).pos(), matBase));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::RightArm2), a2_inv * GetFrameBaseLocalMat((int)CharaFrame::RightArm2));
			//手
			auto AngleOf2Vector = [&](Vector3DX A, Vector3DX B) { return acos(Vector3DX::Dot(A, B) / (A.magnitude() * B.magnitude())); };			//２つのベクトルABのなす角度θを求める
			matBase = GetParentFrameWorldMatrix(GetFrame((int)CharaFrame::RightWrist)).rotation().inverse();
			Matrix4x4DX mat1;
			{
				auto zvec = Matrix4x4DX::Vtrans(Localzvec, GetFrameWorldMat(GetFrame((int)CharaFrame::RightWrist)).rotation());
				mat1 = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(zvec, matBase), Matrix4x4DX::Vtrans(Gunzvec, matBase)) * mat1;
				GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::RightWrist), mat1 * GetFrameBaseLocalMat((int)CharaFrame::RightWrist));
				auto xvec = Matrix4x4DX::Vtrans(Localyvec, GetFrameWorldMat(GetFrame((int)CharaFrame::RightWrist)).rotation());
				mat1 = Matrix4x4DX::RotAxis(Localzvec, AngleOf2Vector(xvec, Gunxvec)*((Vector3DX::Dot((Gunyvec*-1.f), xvec) > 0.f) ? -1.f : 1.f))*mat1;
			}
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::RightWrist), mat1 * GetFrameBaseLocalMat((int)CharaFrame::RightWrist));
		}
		void			CharacterClass::move_LeftArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept {
			GetObj().frame_Reset(GetFrame((int)CharaFrame::LeftArm));
			GetObj().frame_Reset(GetFrame((int)CharaFrame::LeftArm2));
			GetObj().frame_Reset(GetFrame((int)CharaFrame::LeftWrist));
			auto matBase = GetParentFrameWorldMatrix(GetFrame((int)CharaFrame::LeftArm)).rotation().inverse();

			Vector3DX Gunxvec = Vector3DX::Cross(Gunzvec, Gunyvec)*-1.f;

			Vector3DX Localyvec = Vector3DX::vget(0.f, 0.f, -1.f).normalized();
			Vector3DX Localzvec = Vector3DX::vget(1.f, -1.f, 0.f).normalized();

			Vector3DX RetPos = GunPos;
			//基準
			auto vec_a1 = Matrix4x4DX::Vtrans((RetPos - GetFrameWorldMat(GetFrame((int)CharaFrame::LeftArm)).pos()).normalized(), matBase);//基準
			auto vec_a1L1 = Vector3DX(Vector3DX::vget(1.5f, -1.f, vec_a1.y / -abs(vec_a1.z))).normalized();//x=0とする
			float cos_t = GetCosFormula((GetFrameWorldMat(GetFrame((int)CharaFrame::LeftWrist)).pos() - GetFrameWorldMat(GetFrame((int)CharaFrame::LeftArm2)).pos()).magnitude(), (GetFrameWorldMat(GetFrame((int)CharaFrame::LeftArm2)).pos() - GetFrameWorldMat(GetFrame((int)CharaFrame::LeftArm)).pos()).magnitude(), (GetFrameWorldMat(GetFrame((int)CharaFrame::LeftArm)).pos() - RetPos).magnitude());
			auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//上腕
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::LeftArm), Matrix4x4DX::identity() * GetFrameBaseLocalMat((int)CharaFrame::LeftArm));
			Matrix4x4DX a1_inv = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(GetFrameWorldMat(GetFrame((int)CharaFrame::LeftArm2)).pos() - GetFrameWorldMat(GetFrame((int)CharaFrame::LeftArm)).pos(), matBase), vec_t);
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::LeftArm), a1_inv * GetFrameBaseLocalMat((int)CharaFrame::LeftArm));
			//下腕
			matBase = GetParentFrameWorldMatrix(GetFrame((int)CharaFrame::LeftArm2)).rotation().inverse();
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::LeftArm2), Matrix4x4DX::identity() * GetFrameBaseLocalMat((int)CharaFrame::LeftArm2));
			Matrix4x4DX a2_inv = Matrix4x4DX::RotVec2(
				Matrix4x4DX::Vtrans(GetFrameWorldMat(GetFrame((int)CharaFrame::LeftWrist)).pos() - GetFrameWorldMat(GetFrame((int)CharaFrame::LeftArm2)).pos(), matBase),
				Matrix4x4DX::Vtrans(RetPos - GetFrameWorldMat(GetFrame((int)CharaFrame::LeftArm2)).pos(), matBase));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::LeftArm2), a2_inv * GetFrameBaseLocalMat((int)CharaFrame::LeftArm2));
			//手
			auto AngleOf2Vector = [&](Vector3DX A, Vector3DX B) { return acos(Vector3DX::Dot(A, B) / (A.magnitude() * B.magnitude())); };			//２つのベクトルABのなす角度θを求める
			matBase = GetParentFrameWorldMatrix(GetFrame((int)CharaFrame::LeftWrist)).rotation().inverse();
			Matrix4x4DX mat1;
			{
				auto zvec = Matrix4x4DX::Vtrans(Localzvec, GetFrameWorldMat(GetFrame((int)CharaFrame::LeftWrist)).rotation());
				mat1 = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(zvec, matBase), Matrix4x4DX::Vtrans(Gunzvec, matBase)) * mat1;
				GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::LeftWrist), mat1 * GetFrameBaseLocalMat((int)CharaFrame::LeftWrist));
				auto xvec = Matrix4x4DX::Vtrans(Localyvec, GetFrameWorldMat(GetFrame((int)CharaFrame::LeftWrist)).rotation());
				mat1 = Matrix4x4DX::RotAxis(Localzvec, AngleOf2Vector(xvec, Gunxvec)*((Vector3DX::Dot(Gunyvec, xvec) > 0.f) ? -1.f : 1.f))*mat1;
			}
			Matrix4x4DX RetMat = mat1;
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::LeftWrist), RetMat * GetFrameBaseLocalMat((int)CharaFrame::LeftWrist));
		}
		const Matrix4x4DX CharacterClass::GetCharaDir(void) const noexcept {
			auto tmpUpperMatrix = Matrix4x4DX::RotAxis(Vector3DX::forward(), KeyControl::GetLeanRad());

			tmpUpperMatrix *=
				Matrix4x4DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x) *
				Matrix4x4DX::RotAxis(Vector3DX::up(), Lerp(KeyControl::GetYRadBottomChange(), 0.f, this->m_Arm[(int)EnumGunAnimType::Run].Per()));
			return tmpUpperMatrix * this->m_move.mat;
		}
		//
		void			CharacterClass::Shot_Start() noexcept {
		}
		void			CharacterClass::Reload_Start() noexcept {
		}
		//
		const Matrix4x4DX CharacterClass::GetEyeMatrix(void) const noexcept {
			auto tmpUpperMatrix = Matrix4x4DX::RotAxis(Vector3DX::forward(), KeyControl::GetLeanRad() / 5.f);

			tmpUpperMatrix *=
				Matrix4x4DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x) *
				Matrix4x4DX::RotAxis(Vector3DX::up(), KeyControl::GetYRadBottomChange());

			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((this->m_MyID != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			if (HeadBobbing) {
				tmpUpperMatrix = WalkSwingControl::GetWalkSwingMat() * tmpUpperMatrix;
			}
			tmpUpperMatrix *= this->m_move.mat;

			auto EyePosition = (GetFrameWorldMat(GetFrame((int)CharaFrame::LeftEye)).pos() + GetFrameWorldMat(GetFrame((int)CharaFrame::RightEye)).pos()) / 2.f + tmpUpperMatrix.zvec()*(-0.04f*Scale_Rate);

			if (HeadBobbing) {
				EyePosition += EyeSwingControl::GetEyeSwingPos();
			}
			return tmpUpperMatrix * Matrix4x4DX::Mtrans(EyePosition);
		}
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			auto* SE = SoundPool::Instance();
			if (this->m_MyID == value.DamageID) {
				auto PrevLive = LifeControl::IsAlive();
				LifeControl::SubHP(value.Damage);
				if (value.ShotID == 0) {
					if ((value.Damage >= 0)) {
						//PlayerMngr->GetPlayer(value.ShotID).AddHit(1);
					}
				}
				if (LifeControl::IsAlive()) {
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

					SE->Get((int)SoundEnum::Man_Death1 + GetRand(8 - 1)).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
				}
				this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
				return true;
			}
			return false;
		}
		const bool		CharacterClass::CheckDamageRay(HitPoint* Damage, bool CheckBodyParts, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
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
							*Damage = *Damage * 100 / 10;
							break;
						case HitType::Body:
							*Damage = *Damage;
							break;
						case HitType::Arm:
							*Damage = *Damage * 5 / 10;
							break;
						case HitType::Leg:
							*Damage = *Damage * 7 / 10;
							break;
						default:
							break;
					}
					*Damage = std::min(*Damage, LifeControl::GetHPMax());
				}
				//ダメージ登録
				{
					//auto v1 = GetEyeMatrix().zvec() * -1.f;
					//auto v2 = (Chara->GetCharaPosition() - this->GetCharaPosition()).normalized(); v2.y = (0);
					//atan2f(Vector3DX::Cross(v1, v2).y, Vector3DX::Dot(v1, v2))
					LifeControl::SetSubHPEvent(AttackID, this->m_MyID, *Damage);
				}
				//SE
				if (AttackID == 0) {
					if (*Damage > 0) {
						SE->Get((int)SoundEnum::Hit).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
					}
					else {
						SE->Get((int)SoundEnum::HitGuard).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f, 128);
					}
				}
				if (m_MyID == 0) {
					if (*Damage > 0) {
						SE->Get((int)SoundEnum::HitMe).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f);
					}
					else {
						SE->Get((int)SoundEnum::HitGuard).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 10.f, 255);
					}
				}
				//エフェクトセット
				if (*Damage > 0) {
					EffectControl::SetOnce(EffectResource::Effect::ef_hitblood, *pEndPos, Vector3DX::forward(), Scale_Rate);
					EffectControl::SetEffectSpeed(EffectResource::Effect::ef_hitblood, 2.f);
				}
				else {
					EffectControl::SetOnce(EffectResource::Effect::ef_gndsmoke, *pEndPos, (Chara->GetCharaPosition() - this->GetCharaPosition()).normalized(), 0.25f * Scale_Rate);
				}
				//ヒットモーション
				{
					HitReactionControl::SetHit(Matrix4x4DX::Vtrans(Vector3DX::Cross((*pEndPos - StartPos).normalized(), Vector3DX::up())*-1.f, GetFrameWorldMat(GetFrame((int)CharaFrame::Upper2)).rotation().inverse()));
					if (HitPtr->GetColType() == HitType::Leg) {
						KeyControl::SetIsSquat(true);
					}
				}
				//todo : ヒットした部分に近い頂点を赤くする
				return true;
			}
			return false;
		}
		void			CharacterClass::LoadExtends() noexcept {
			//アーマー
			/*
			auto* ObjMngr = ObjectManager::Instance();
			{
				auto ArmerPtr = std::make_shared<ArmerClass>();
				ObjMngr->AddObject(ArmerPtr);
				ObjMngr->LoadModel(ArmerPtr, nullptr, "data/model/PlateCarrler/");
				ArmerPtr->Init();
				this->SetArmer(ArmerPtr);
			}
			//*/
		}
		//操作
		void			CharacterClass::ExecuteInput(void) noexcept {
			//
			auto PrevAction = m_CharaAction;
			//
			if (PrevAction == m_CharaAction) {
				switch (m_CharaAction) {
					case CharaActionID::Ready:
						if (m_ActionFirstFrame) {
						}
						//アーマー着用/弾込め
						if (KeyControl::GetInputControl().GetPADSPress(PADS::CHECK)) {
							if ((this->m_ArmerStock != 0)) {
								this->m_ArmerStock = std::max(this->m_ArmerStock - 1, 0);
								m_CharaAction = CharaActionID::WearArmer;
							}
						}
						break;
					case CharaActionID::WearArmer:
						/*
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
										ItemLogParts->AddLog(3.f, GetColor(25, 122, 75), "%s +%4d", LocalizePool::Instance()->Get(200), 10);
										PlayerMngr->GetPlayer(this->m_MyID).AddScore(10);
									}
									GunReadyControl::SetAim();
									LifeControl::SetHealEvent(this->m_MyID, this->m_MyID, 0, LifeControl::GetAPMax());
								}
								break;
							default:
								break;
						}
						//*/
						break;
					default:
						break;
				}
				//近接
				if (!KeyControl::GetRun() && KeyControl::GetInputControl().GetPADSPress(PADS::MELEE) && m_MeleeCoolDown == 0.f && IsAimPer()) {
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
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(GetFrame((int)CharaFrame::LeftFoot)).pos()).magnitude() < Scale_Rate * 5.f*1.5f) {

								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::LeftFoot)).pos(), Scale_Rate * 5.f);
							}
						}
					}
					//R
					if ((27.f < Time &&Time < 28.f)) {
						if (this->m_CharaSound != 3) {
							this->m_CharaSound = 3;
							this->m_SoundPower = 0.5f * (CanLookTarget ? 1.f : 0.5f);
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(GetFrame((int)CharaFrame::RightFoot)).pos()).magnitude() < Scale_Rate * 5.f*1.5f) {

								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::RightFoot)).pos(), Scale_Rate * 5.f);
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
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(GetFrame((int)CharaFrame::LeftFoot)).pos()).magnitude() < Scale_Rate * 5.f*1.5f) {
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::LeftFoot)).pos(), Scale_Rate * 15.f);
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
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(GetFrame((int)CharaFrame::RightFoot)).pos()).magnitude() < Scale_Rate * 5.f*1.5f) {
								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::RightFoot)).pos(), Scale_Rate * 15.f);
							}
						}
					}
				}
			}
			//しゃがみ音
			if (KeyControl::GetSquatSwitch()) {
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Upper)).pos(), Scale_Rate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//リーン音
			if (KeyControl::GetLeanSwitch()) {
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Upper)).pos(), Scale_Rate * 3.f);
				this->m_SoundPower = 0.1f * (CanLookTarget ? 1.f : 0.5f);
			}
			//心拍音
			if (this->m_MyID == 0) {
				if (StaminaControl::ExcuteStamina(0.f, this->m_move.Speed / DrawParts->GetFps(), KeyControl::GetIsSquat())) {
					if (this->m_BackGround) {//todo:タイトル用仮
						SE->Get((int)SoundEnum::Heart).Play_3D(0, GetEyeMatrix().pos(), Scale_Rate * 0.5f);
					}
				}
			}
		}
		//SetMat指示更新
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			//vector
			bool IsHitGround = false;//(this->m_move.posbuf.y <= 0.f); //高度0なら止まる
			float groundYpos = 0.f;
			if (this->m_BackGround) {
				Vector3DX EndPos = this->m_move.posbuf - Vector3DX::up() * 5.f;
				IsHitGround = this->m_BackGround->CheckLinetoMap(this->m_move.posbuf + Vector3DX::up() * 20.f, &EndPos, true);
				groundYpos = EndPos.y;
			}
			if (IsHitGround) {
				auto yPos = this->m_move.posbuf.y;
				Easing(&yPos, groundYpos, 0.8f, EasingType::OutExpo);
				this->m_move.posbuf.y = (yPos);

				this->m_move.vec = KeyControl::GetVec();
				this->m_move.vec.y = (0.f);
			}
			else {
				float Y = this->m_move.vec.y;
				this->m_move.vec = KeyControl::GetVec();
				this->m_move.vec.y = (Y + (M_GR / (DrawParts->GetFps() * DrawParts->GetFps())));
			}
			this->m_move.posbuf += this->m_move.vec;
			//壁判定
			std::vector<std::pair<MV1*, int>> cols;
			if (this->m_BackGround) {
				cols.emplace_back(std::make_pair((MV1*)(&this->m_BackGround->GetGroundCol()), -1));
			}
			col_wall(this->m_move.repos, &this->m_move.posbuf, cols);
			//ほかプレイヤーとの判定
			if (true) {
				float Radius = 0.5f*Scale_Rate;
				for (int i = 0; i < Player_num; i++) {
					if (i == this->m_MyID) { continue; }
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i).GetChara();
					if (!Chara->IsAlive()) { continue; }
					//自分が当たったら押し戻す
					Vector3DX Vec = (Chara->GetCharaPosition() - this->GetCharaPosition()); Vec.y = (0.f);
					float Len = Vec.magnitude();
					if (Len < Radius) {
						this->m_move.posbuf += Vec.normalized() * (Len - Radius);
					}
				}
			}
			//座標オーバーライド
			if (KeyControl::PutOverride()) {
				this->m_move.posbuf = KeyControl::GetOverRideInfo().pos;
				this->m_move.vec = KeyControl::GetOverRideInfo().vec;
			}
			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((this->m_MyID != 0) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			this->m_move.mat = Matrix4x4DX::RotAxis(Vector3DX::forward(), HeadBobbing ? (KeyControl::GetRad().z / 2.f) : 0.f) * Matrix4x4DX::RotAxis(Vector3DX::up(), KeyControl::GetYRadBottom());
			this->m_move.UpdatePosBuf(this->m_move.posbuf, 0.9f);
			UpdateMove();
			//銃座標指定
			Vector3DX GunPos;
			Vector3DX Gunyvec;
			Vector3DX Gunzvec;
			Vector3DX HandsPos;
			Vector3DX Handsyvec;
			Vector3DX Handszvec;
			/*
			//アーマー
			if (this->m_Armer_Ptr && this->m_Armer_Ptr->IsActive()) {
				Matrix4x4DX tmp_gunmat = GetFrameWorldMat(GetFrame((int)CharaFrame::Head));
				tmp_gunmat = Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, -0.5f, 0.2f)*Scale_Rate*std::clamp(1.f - this->m_Wear_Armer.Per() * 1.5f, 0.f, 1.f)) * tmp_gunmat;
				tmp_gunmat = Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.15f, -0.5f)*Scale_Rate*(1.f - this->m_Wear_Armer.Per()))*tmp_gunmat;
				tmp_gunmat = Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, -0.30f, 0.f)*Scale_Rate*this->m_Wear_Armer.Per())*tmp_gunmat;
				tmp_gunmat = Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(-30.f)*(1.f - this->m_Wear_Armer.Per()))*tmp_gunmat;
				this->m_Armer_Ptr->SetMove(tmp_gunmat.rotation(), tmp_gunmat.pos());
				this->m_Armer_Ptr->UpdateMove();

				GunPos = this->m_Armer_Ptr->GetRightHandMat().pos();
				Gunyvec = this->m_Armer_Ptr->GetRightHandMat().rotation().xvec()*-1.f;
				Gunzvec = this->m_Armer_Ptr->GetRightHandMat().rotation().yvec();
				HandsPos = this->m_Armer_Ptr->GetLeftHandMat().pos();
				Handsyvec = this->m_Armer_Ptr->GetLeftHandMat().rotation().xvec();
				Handszvec = this->m_Armer_Ptr->GetLeftHandMat().rotation().zvec()*-1.f;
			}
			move_RightArm(GunPos, Gunyvec, Gunzvec);
			move_LeftArm(HandsPos, Handsyvec, Handszvec);

			//*/
		}
		//
		void			CharacterClass::ValueSet(PlayerID pID, bool, CharaTypeID value) noexcept {
			this->m_CharaType = value;
			StaminaControl::InitStamina();
			LifeControl::InitLife();
			//
			this->m_MyID = pID;
			//
			for (auto& a : this->m_Arm) { a.Init(false); }
			this->m_Arm[(int)EnumGunAnimType::Aim].Init(true);
			//
			EyeSwingControl::InitEyeSwing();
			//ArmerPtrControl::InitArmerPtr();
			//
			//サウンド
			this->m_CharaSound = -1;
			//動作にかかわる操作
			KeyControl::InitOverride();
			m_HPRec = 0.f;
			m_CharaAction = CharaActionID::Ready;
			m_ActionFirstFrame = true;
		}
		void			CharacterClass::MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept {
			KeyControl::InitKey(pxRad, pyRad);
			this->m_move.vec.Set(0, 0, 0);
			this->m_move.repos = pPos;
			this->m_move.Speed = 0.f;
			SetMove(Matrix4x4DX::RotAxis(Vector3DX::up(), KeyControl::GetRad().y), pPos);
			this->m_ArmerStock = 1;
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			InputControl Input = pInput;
			{
				float pp_y = 0.f;
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID()).GetChara();

				auto TgtPos = Target->GetMove().posbuf;
				auto MyPos = this->GetMove().posbuf;

				auto Dir = this->GetEyeMatrix().zvec() * -1.f;
				auto Dir_XZ = Dir; Dir_XZ.y = (0.f);

				auto Vec = (TgtPos - MyPos).normalized(); Vec.y = (0.f);

				auto IsFront = ((Vector3DX::Dot(Dir_XZ.normalized(), Vec.normalized())) > 0.f);
				auto cross = Vector3DX::Cross(Dir_XZ.normalized(), Vec.normalized()).y;
				if (IsFront) {
					if (abs(cross) < 0.4f) {
						pp_y = (cross * 0.14f);
					}
					else {
						pp_y = (cross * 0.35f);
					}
				}
				else {
					pp_y = -0.5f;
				}
				pp_y *= std::clamp(1.f - this->m_Arm[(int)EnumGunAnimType::Run].Per(), 0.f, 1.f);
				pp_y = std::clamp(pp_y*0.5f, -0.2f, 0.2f);
				Input.SetAddyRad(pp_y);
			}
			//
			KeyControl::InputKey(Input, pReady, StaminaControl::GetHeartRandVec(KeyControl::GetIsSquat() ? 1.f : 0.f));
		}
		//
		void			CharacterClass::Init_Sub(void) noexcept {
			HitBoxControl::InitHitBox();
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				if (GetMyPlayerID() == 0) {
					this->GetObj().SetOpacityRate(0.5f);
				}
			}
			this->m_SoundPower = std::max(this->m_SoundPower - 1.f / DrawParts->GetFps(), 0.f);
			m_MeleeCoolDown = std::max(m_MeleeCoolDown - 1.f / DrawParts->GetFps(), 0.f);
			//
			ExecuteInput();
			//
			this->m_Arm[(int)EnumGunAnimType::ADS].Execute(false, 0.2f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::Run].Execute(KeyControl::GetRun(), 0.1f, 0.2f, 0.95f);
			this->m_Arm[(int)EnumGunAnimType::ReloadStart_Empty].Execute(false, 0.5f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::ReloadStart].Execute(false, 0.2f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Reload].Execute(false, 0.1f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::ReloadEnd].Execute(false, 0.1f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::Ready].Execute(false, 0.1f, 0.2f, 0.9f);
			this->m_Arm[(int)EnumGunAnimType::Check].Execute(false, 0.05f, 0.2f);
			this->m_Arm[(int)EnumGunAnimType::Watch].Execute(GetCharaAction() == CharaActionID::Watch, 0.1f, 0.1f);
			this->m_Arm[(int)EnumGunAnimType::Melee].Execute(GetCharaAction() == CharaActionID::Melee, 1.1f, 0.1f);
			this->m_Arm[(int)EnumGunAnimType::AmmoLoad].Execute(false, 0.1f, 0.1f);
			//
			KeyControl::UpdateKeyRad();
			GetObj().frame_Reset(GetFrame((int)CharaFrame::Upper));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::Upper),
										 GetFrameLocalMat(GetFrame((int)CharaFrame::Upper)).rotation() * Matrix4x4DX::RotAxis(Vector3DX::right(), -KeyControl::GetRad().x / 2.f) * (GetCharaDir()*this->m_move.mat.inverse()).rotation()
										 * GetFrameBaseLocalMat((int)CharaFrame::Upper));
			GetObj().frame_Reset(GetFrame((int)CharaFrame::Upper2));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::Upper2),
										 GetFrameLocalMat(GetFrame((int)CharaFrame::Upper2)).rotation() * Matrix4x4DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x / 2.f) * HitReactionControl::GetHitReactionMat()
										 * GetFrameBaseLocalMat((int)CharaFrame::Upper2));
			HitReactionControl::Execute_HitReactionControl();
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
			EyeSwingControl::UpdateEyeSwing(GetCharaDir(), KeyControl::GetRun() ? 0.5f : (KeyControl::GetVec().magnitude() / 0.65f), KeyControl::GetRun() ? 12.f : 5.f);
			//
			ExecuteSound();
			//
			ExecuteMatrix();
			//
			HitBoxControl::UpdataHitBox(this, (this->GetCharaType() == CharaTypeID::Enemy) ? 1.1f : 1.f);									//ヒットボックス
			WalkSwingControl::UpdateWalkSwing(GetEyeMatrix().pos() - GetCharaPosition(), std::clamp(this->m_move.Speed / 2.f, 0.f, 1.f));
			EffectControl::Execute();
		}
		void			CharacterClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if ((CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f*Scale_Rate, -0.5f*Scale_Rate, -2.5f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f*Scale_Rate, 2.f*Scale_Rate, 2.5f*Scale_Rate)).get()) == FALSE)
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
					if (GetMyPlayerID() == 0) {
						if (isDrawSemiTrans) {
							this->GetObj().DrawModel();
						}
					}
					else {
						for (int i = 0; i < this->GetObj().mesh_num(); i++) {
							if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
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
		void			CharacterClass::Dispose_Sub(void) noexcept {
			EffectControl::Dispose();
			this->m_BackGround.reset();
		}
	};
};
