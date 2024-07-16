#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			CharacterClass::move_RightArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept {
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::RightArm));
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::RightArm2));
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::RightWrist));
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
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::LeftArm));
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::LeftArm2));
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::LeftWrist));
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
			auto tmpUpperMatrix =
				Matrix4x4DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x) *
				Matrix4x4DX::RotAxis(Vector3DX::up(), Lerp(KeyControl::GetYRadBottomChange(), 0.f, this->m_Arm[(int)EnumWeaponAnimType::Run].Per()));
			return tmpUpperMatrix * this->m_move.mat;
		}
		//
		void			CharacterClass::OverrideReady() noexcept {
			FrontAttack_End();
			Do_End();
			BackAttack_End();
			SetIsRunning(false);
			m_CharaAction = EnumWeaponAnimType::Ready;
		}
		void			CharacterClass::FrontAttack_Start() noexcept {
			GetWeaponPtrNow()->ResetAnim();
			m_NormalActionTime = GetWeaponPtrNow()->GetGunTotalTime(m_CharaAction);
			SetIsFrontAttacking(true);
			this->GetObj().SetAnim((int)CharaAnimeID::Bottom_Stand_Attack).GoStart();
			this->m_CharaSound = -1;

			auto* SE = SoundPool::Instance();
			SE->Get((int)SoundEnum::Kendo_Swing).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::RightWrist)).pos(), Scale_Rate * 5.f);
		}
		void CharacterClass::FrontAttack_End() noexcept {
			SetIsFrontAttacking(false);
			m_NormalActionTime = 0.f;
		}
		void			CharacterClass::Do_Start() noexcept {
			GetWeaponPtrNow()->ResetAnim();
			m_NormalActionTime = GetWeaponPtrNow()->GetGunTotalTime(m_CharaAction);
			SetIsDouAttacking(true);
			this->GetObj().SetAnim((int)CharaAnimeID::Bottom_Stand_Attack).GoStart();
			this->m_CharaSound = -1;
		}
		void CharacterClass::Do_End() noexcept {
			SetIsDouAttacking(false);
			m_NormalActionTime = 0.f;
		}
		void			CharacterClass::Tsuba_Start() noexcept {
			m_NormalActionTime = 0.5f;
		}
		void			CharacterClass::Tsuba_End() noexcept {
		}
		void			CharacterClass::OverrideTsuba() noexcept {
			FrontAttack_End();
			Do_End();
			BackAttack_End();
			SetIsRunning(false);
			m_CharaAction = EnumWeaponAnimType::Tsuba;
		}
		void			CharacterClass::BackAttack_Start() noexcept {
			GetWeaponPtrNow()->ResetAnim();
			m_NormalActionTime = std::max(0.75f, GetWeaponPtrNow()->GetGunTotalTime(m_CharaAction));
			SetIsBackAttacking(true);
			this->GetObj().SetAnim((int)CharaAnimeID::Bottom_Stand_Attack).GoStart();
			this->m_CharaSound = -1;
		}
		void CharacterClass::BackAttack_End() noexcept {
			SetIsBackAttacking(false);
			m_NormalActionTime = 0.f;
		}

		//
		const Matrix4x4DX CharacterClass::GetEyeMatrix(void) const noexcept {
			auto tmpUpperMatrix =
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
			if (this->m_MyID == value.DamageID) {
				return true;
			}
			return false;
		}
		//操作
		void			CharacterClass::ExecuteInput(void) noexcept {

			int num = MV1GetMaterialNum(GetObj().GetHandle());
			for (int i = 0; i < num; i++) {
				MV1SetMaterialDifColor(GetObj().GetHandle(), i, GetColorF(0.8f, 0.8f, 0.8f, 1.f));
				MV1SetMaterialAmbColor(GetObj().GetHandle(), i, GetColorF(0.25f, 0.25f, 0.25f, 1.f));
			}

			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			//
			auto PrevAction = m_CharaAction;
			//
			bool IsOutArea = false;
			{
				Vector3DX Vec = this->m_move.posbuf - Vector3DX::zero();
				float Len = 11.f / 2.f*Scale_Rate;
				if ((Vec.x < -Len || Len < Vec.x) ||
					(Vec.z < -Len || Len < Vec.z)) {
					IsOutArea = true;
				}
			}
			//掛け声
			if (KeyControl::GetInputControl().GetPADSPress(PADS::JUMP) && m_YaTimer<=0.f) {
				SE->Get((int)SoundEnum::Voice_Ya).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Head)).pos(), Scale_Rate * 35.f);
				m_YaTimer = 15.f;
			}
			m_YaTimer = std::max(m_YaTimer - 1.f / DrawParts->GetFps(), 0.f);
			//
			switch (m_CharaAction) {
				case EnumWeaponAnimType::Ready:
					if (!IsOutArea) {
						if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
							m_CharaAction = EnumWeaponAnimType::Men;
						}
						if (KeyControl::GetInputControl().GetPADSPress(PADS::ULT)) {
							m_CharaAction = EnumWeaponAnimType::Kote;
						}
						if (false) {
							m_CharaAction = EnumWeaponAnimType::Dou;
						}
					}
					if ((m_CharaAction== EnumWeaponAnimType::Ready) && KeyControl::GetInputControl().GetPADSPress(PADS::AIM)) {
						m_CharaAction = EnumWeaponAnimType::GuardStart;
					}
					m_HeartUp = std::max(m_HeartUp - 30.f / DrawParts->GetFps(), -5.f);
					break;
				case EnumWeaponAnimType::Run:
					if (m_ActionFirstFrame) {
						//つばぜり判定
						{
							bool IsGuard = false;

							auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID()).GetChara();

							auto TgtPos = Target->GetMove().posbuf;
							auto MyPos = this->GetMove().posbuf;

							auto Dir = this->GetEyeMatrix().zvec() * -1.f;
							auto Dir_XZ = Dir; Dir_XZ.y = (0.f);Dir_XZ = Dir_XZ.normalized();

							auto Vec = (TgtPos - MyPos); Vec.y = (0.f);

							auto IsFront = ((Vector3DX::Dot(Dir_XZ, Vec.normalized())) > 0.f);
							auto cross = Vector3DX::Cross(Dir_XZ, Vec.normalized()).y;

							float Radius = (0.5f+ 0.25f)*Scale_Rate;
							float Len = Vec.magnitude();
							if (Len < Radius) {
								if (IsFront) {
									if (abs(cross) < 0.4f) {
										IsGuard = true;
									}
								}
							}

							if (IsGuard) {
								m_CharaAction = EnumWeaponAnimType::Tsuba;
								//相手もつばぜり合いにする
								Target->OverrideTsuba();
							}
							else {
								SetIsRunning(true);
								m_RunTime = 2.f;
							}
						}
					}
					else {
						if (IsOutArea || m_RunTime <= 0.f) {
							SetIsRunning(false);
							m_CharaAction = EnumWeaponAnimType::Ready;
						}
						m_RunTime = std::max(m_RunTime - 1.f / DrawParts->GetFps(), 0.f);
						//SetIsRunning(false);
					}
					m_HeartUp = std::max(m_HeartUp, 2.f);
					break;
				case EnumWeaponAnimType::Men:
				case EnumWeaponAnimType::Kote:
				case EnumWeaponAnimType::Tsuki:
					if (m_ActionFirstFrame) {
						FrontAttack_Start();
						SE->Get((int)SoundEnum::Voice_Men).StopAll(0);
						SE->Get((int)SoundEnum::Voice_Kote).StopAll(0);
						SE->Get((int)SoundEnum::Voice_Dou).StopAll(0);
						SE->Get((int)SoundEnum::Voice_Tsuki).StopAll(0);
						switch (m_CharaAction) {
							case EnumWeaponAnimType::Men:
								SE->Get((int)SoundEnum::Voice_Men).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Head)).pos(), Scale_Rate * 35.f);
								break;
							case EnumWeaponAnimType::Kote:
								SE->Get((int)SoundEnum::Voice_Kote).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Head)).pos(), Scale_Rate * 35.f);
								break;
							case EnumWeaponAnimType::Tsuki:
								SE->Get((int)SoundEnum::Voice_Tsuki).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Head)).pos(), Scale_Rate * 35.f);
								break;
							default:
								break;
						}
					}
					else {
						Easing(&m_BambooVec, Vector3DX::zero(), 0.9f, EasingType::OutExpo);
						if (m_NormalActionTime <= 0.f) {
							FrontAttack_End();
							auto Action = m_CharaAction;

							m_CharaAction = EnumWeaponAnimType::Run;
							switch (Action) {
								case EnumWeaponAnimType::Men:
									if (false) {
										m_CharaAction = EnumWeaponAnimType::Dou;//面胴
									}
									//小手面後派生
									if (false) {
										if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
											m_CharaAction = EnumWeaponAnimType::Men;//小手面面
										}
										if (false) {
											m_CharaAction = EnumWeaponAnimType::Dou;//小手面胴
										}
									}
									break;
								case EnumWeaponAnimType::Kote:
									if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
										m_CharaAction = EnumWeaponAnimType::Men;//小手面
									}
									if (false) {
										m_CharaAction = EnumWeaponAnimType::Dou;//小手胴
									}
									break;
								case EnumWeaponAnimType::Tsuki:
									if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
										m_CharaAction = EnumWeaponAnimType::Men;//突き面
									}
									break;
								default:
									break;
							}
						}
						m_NormalActionTime = std::max(m_NormalActionTime - 1.f / DrawParts->GetFps(), 0.f);
					}
					m_HeartUp = std::max(m_HeartUp, 10.f);
					break;
				case EnumWeaponAnimType::Dou:
					if (m_ActionFirstFrame) {
						Do_Start();
						SE->Get((int)SoundEnum::Voice_Men).StopAll(0);
						SE->Get((int)SoundEnum::Voice_Kote).StopAll(0);
						SE->Get((int)SoundEnum::Voice_Dou).StopAll(0);
						SE->Get((int)SoundEnum::Voice_Tsuki).StopAll(0);
						SE->Get((int)SoundEnum::Voice_Dou).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Head)).pos(), Scale_Rate * 35.f);
					}
					else {
						if (m_NormalActionTime <= 0.f) {
							Do_End();
							m_CharaAction = EnumWeaponAnimType::Run;
						}
						m_NormalActionTime = std::max(m_NormalActionTime - 1.f / DrawParts->GetFps(), 0.f);
					}
					m_HeartUp = std::max(m_HeartUp, 10.f);
					break;
				case EnumWeaponAnimType::Tsuba:
					if (m_ActionFirstFrame) {
						Tsuba_Start();
					}
					else {
						if (m_NormalActionTime <= 0.f) {
							if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
								m_CharaAction = EnumWeaponAnimType::HikiMen;
							}
							if (KeyControl::GetInputControl().GetPADSPress(PADS::ULT)) {
								m_CharaAction = EnumWeaponAnimType::HikiKote;
							}
							if (false) {
								m_CharaAction = EnumWeaponAnimType::HikiDou;
							}
						}
						m_NormalActionTime = std::max(m_NormalActionTime - 1.f / DrawParts->GetFps(), 0.f);
					}
					m_HeartUp = std::max(m_HeartUp - 30.f / DrawParts->GetFps(), -5.f);
					break;
				case EnumWeaponAnimType::HikiMen:
				case EnumWeaponAnimType::HikiKote:
				case EnumWeaponAnimType::HikiDou:
					if (m_ActionFirstFrame) {
						BackAttack_Start();
						auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID()).GetChara();
						Target->OverrideReady();
						SE->Get((int)SoundEnum::Voice_Men).StopAll(0);
						SE->Get((int)SoundEnum::Voice_Kote).StopAll(0);
						SE->Get((int)SoundEnum::Voice_Dou).StopAll(0);
						SE->Get((int)SoundEnum::Voice_Tsuki).StopAll(0);
						switch (m_CharaAction) {
							case EnumWeaponAnimType::HikiMen:
								SE->Get((int)SoundEnum::Voice_Men).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Head)).pos(), Scale_Rate * 35.f);
								break;
							case EnumWeaponAnimType::HikiKote:
								SE->Get((int)SoundEnum::Voice_Kote).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Head)).pos(), Scale_Rate * 35.f);
								break;
							case EnumWeaponAnimType::HikiDou:
								SE->Get((int)SoundEnum::Voice_Dou).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Head)).pos(), Scale_Rate * 35.f);
								break;
							default:
								break;
						}
					}
					else {
						Easing(&m_BambooVec, Vector3DX::zero(), 0.9f, EasingType::OutExpo);
						if (IsOutArea || m_NormalActionTime <= 0.f) {
							BackAttack_End();
							m_CharaAction = EnumWeaponAnimType::Ready;
						}
						m_NormalActionTime = std::max(m_NormalActionTime - 1.f / DrawParts->GetFps(), 0.f);
					}
					m_HeartUp = std::max(m_HeartUp, 10.f);
					break;
				case EnumWeaponAnimType::GuardStart:
					if (m_ActionFirstFrame) {
						m_GuardTimer = 0.f;
						m_GuardVec.Set(0.f, 0.f, 0.f);
						m_GuardVecR.Set(0.f, 0.f, 0.f);
					}
					else {
						Easing(&m_BambooVec, Vector3DX::zero(), 0.9f, EasingType::OutExpo);
						m_GuardTimer = std::min(m_GuardTimer + 1.f / DrawParts->GetFps(), 1.f);

						bool suriage = false;
						float length = std::hypotf(m_GuardVecR.x, m_GuardVecR.y);
						if (length > 0.8f) {
							if (m_GuardVecR.x > 0.7f) {
								suriage = true;//上ガード
							}
						}

						if ((m_GuardTimer >= 0.1f && !KeyControl::GetInputControl().GetPADSPress(PADS::AIM)) || (m_GuardTimer == 1.f) || suriage) {
							if (suriage) {
								m_BambooVec = Vector3DX::zero();
								m_CharaAction = EnumWeaponAnimType::GuardSuriage;//上ガード
							}
							else {
								m_CharaAction = EnumWeaponAnimType::Ready;
							}
						}
					}
					break;
				case EnumWeaponAnimType::GuardSuriage:
					if (m_ActionFirstFrame) {
						m_GuardTimer = 1.f;
					}
					else {
						m_GuardTimer = std::max(m_GuardTimer - 1.f / DrawParts->GetFps(), 0.f);
						if (m_GuardTimer == 0.f) {
							m_CharaAction = EnumWeaponAnimType::Ready;
						}
						else {
							if (KeyControl::GetInputControl().GetPADSPress(PADS::SHOT)) {
								m_CharaAction = EnumWeaponAnimType::Men;
							}
						}
					}
					break;
				default:
					break;
			}
			//
			m_ActionFirstFrame = (PrevAction != m_CharaAction);
		}
		void			CharacterClass::ExecuteAction(void) noexcept {
			auto IsGuardAction = [](EnumWeaponAnimType value) {
				switch (value) {
					case  EnumWeaponAnimType::GuardSuriage:
						return true;
					default:
						return false;
				}
			};
			for (int index = 0; index < (int)EnumWeaponAnimType::Max; index++) {
				this->m_Arm[index].Execute((m_CharaAction == (EnumWeaponAnimType)index), 0.1f, 0.1f, IsGuardAction((EnumWeaponAnimType)index) ? 0.75f : 0.9f);
				//this->m_Arm[index].Init((m_CharaAction == (EnumWeaponAnimType)index));
			}
		}
		void			CharacterClass::ExecuteAnim(void) noexcept {
			//上半身アニメ演算
			GetCharaAnimeBufID(CharaAnimeID::Upper_Ready) = 1.f;
			GetCharaAnimeBufID(CharaAnimeID::Hand_Ready) = 1.f;
			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomTurnAnimSel(), 0.5f);
			ObjectBaseClass::SetAnimOnce((int)CharaAnimeID::Bottom_Stand_Attack, KeyControl::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop((int)CharaAnimeID::Bottom_Stand_Run, KeyControl::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomWalkAnimSel(), KeyControl::GetVecFront());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomLeftStepAnimSel(), KeyControl::GetVecLeft());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomWalkBackAnimSel(), KeyControl::GetVecRear());
			ObjectBaseClass::SetAnimLoop((int)KeyControl::GetBottomRightStepAnimSel(), KeyControl::GetVecRight());
			//アニメ反映
			for (int i = 0; i < GetObj().GetAnimNum(); i++) {
				this->GetObj().SetAnim(i).SetPer(GetCharaAnimeBufID((CharaAnimeID)i));
			}
			GetObj().UpdateAnimAll();
		}
		//音指示
		void			CharacterClass::ExecuteSound(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			//足音
			if (this->m_BottomAnimSelect != GetBottomStandAnimSel()) {
				auto Time = this->GetObj().SetAnim((int)this->m_BottomAnimSelect).GetTime();
				if (!GetRun()) {
					//L
					if ((9.f < Time && Time < 10.f)) {
						if (this->m_CharaSound != 1) {
							this->m_CharaSound = 1;
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(GetFrame((int)CharaFrame::LeftFoot)).pos()).magnitude() < Scale_Rate * 5.f*1.5f) {

								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::LeftFoot)).pos(), Scale_Rate * 5.f);
							}
						}
					}
					//R
					if ((27.f < Time &&Time < 28.f)) {
						if (this->m_CharaSound != 3) {
							this->m_CharaSound = 3;
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(GetFrame((int)CharaFrame::RightFoot)).pos()).magnitude() < Scale_Rate * 5.f*1.5f) {

								SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::RightFoot)).pos(), Scale_Rate * 5.f);
							}
						}
					}
				}
				else {
					if (GetCharaAnimeBufID(CharaAnimeID::Bottom_Stand_Attack) > 0.5f) {
						if ((9.f < Time && Time < 10.f)) {
							if (this->m_CharaSound != 7) {
								this->m_CharaSound = 7;
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(GetFrame((int)CharaFrame::RightFoot)).pos()).magnitude() < Scale_Rate * 5.f*1.5f) {
									SE->Get((int)SoundEnum::Kendo_Foot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::RightFoot)).pos(), Scale_Rate * 15.f);
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
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFrameWorldMat(GetFrame((int)CharaFrame::RightFoot)).pos()).magnitude() < Scale_Rate * 5.f*1.5f) {
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::RightFoot)).pos(), Scale_Rate * 15.f);
								}
							}
						}
					}
				}
			}
			else {
				this->m_CharaSound = -1;
			}
			//しゃがみ音
			if (KeyControl::GetSquatSwitch()) {
				SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(GetFrame((int)CharaFrame::Upper)).pos(), Scale_Rate * 3.f);
			}
			//心拍音
#ifdef DEBUG
			printfDx("HEART : %f\n", m_HeartUp);
#endif
			Easing(&m_HeartUpR, m_HeartUp, 0.99f, EasingType::OutExpo);
			if (StaminaControl::ExcuteStamina(m_HeartUpR / DrawParts->GetFps())) {
				if (this->m_MyID == 0) {
					if (this->m_BackGround) {//todo:タイトル用仮
						SE->Get((int)SoundEnum::Heart).Play(0, DX_PLAYTYPE_BACK, TRUE);
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
			if (m_CharaAction == EnumWeaponAnimType::Tsuba) {
				auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID()).GetChara();

				auto TgtPos = Target->GetMove().posbuf;
				auto MyPos = this->GetMove().posbuf;

				auto Vec = (TgtPos - MyPos); Vec.y = (0.f);Vec = Vec.normalized();

				float Radius = (0.5f + 0.25f)*Scale_Rate;
				this->m_move.posbuf = TgtPos - Vec * Radius;
			}
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
			if (GetWeaponPtrNow()) {
				//銃座標指定
				Vector3DX GunPos;
				Vector3DX Gunyvec;
				Vector3DX Gunzvec;
				Vector3DX HandsPos;
				Vector3DX Handsyvec;
				Vector3DX Handszvec;

				//銃の位置を指定するアニメ
				{
					Matrix4x4DX AnimMat;
					for (int i = 0; i < (int)EnumWeaponAnimType::Max; i++) {
						Matrix4x4DX AnimData;
						if (!GetWeaponPtrNow()->GetGunAnime((EnumWeaponAnimType)i, &AnimData)) { continue; }
						AnimMat = Lerp(AnimMat, AnimData, this->m_Arm[i].Per());
					}
					Vector3DX Post0 = GetFrameWorldMat(GetFrame((int)CharaFrame::Head)).pos() + Matrix4x4DX::Vtrans(AnimMat.pos(), GetCharaDir());

					auto tmp_gunmat = Matrix4x4DX::RotVec2(Vector3DX::forward(), AnimMat.zvec());

					tmp_gunmat *= Matrix4x4DX::RotVec2(tmp_gunmat.yvec(), AnimMat.yvec());
					tmp_gunmat *=
						KeyControl::GetGunSwingMat() * GetCharaDir()*
						Matrix4x4DX::RotVec2(GetWeaponPtrNow()->GetObj().GetMatrix().yvec(), GetWeaponPtrNow()->GetObj().GetMatrix().yvec())*
						Matrix4x4DX::Mtrans(Post0);

					//
					bool IsHit = false;
					for (int index = 0; index < 10; index++) {
						m_BambooVec += m_MouseVecR / 10.f;
						m_BambooVec.Set(std::clamp(m_BambooVec.x, deg2rad(-10), deg2rad(10)), std::clamp(m_BambooVec.y, deg2rad(-30), deg2rad(30)), 0.f);

						Vector3DX StartPosP = GetWeaponPtrNow()->GetMove().pos;
						Vector3DX EndPosP = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::End).pos();

						auto tmp_gunmat2 =
							Matrix4x4DX::RotAxis(Vector3DX::right(), m_BambooVec.x)*
							Matrix4x4DX::RotAxis(Vector3DX::forward(), m_BambooVec.y)*
							tmp_gunmat.rotation() *
							Matrix4x4DX::Mtrans(tmp_gunmat.pos());
						GetWeaponPtrNow()->SetMove(tmp_gunmat2.rotation(), tmp_gunmat2.pos());

						Vector3DX StartPos = GetWeaponPtrNow()->GetMove().pos;
						Vector3DX EndPos = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::End).pos();
						Vector3DX VecA = (EndPos - StartPos).normalized();

						float Radius = 0.025f*Scale_Rate*2.f;
						for (int i = 0; i < Player_num; i++) {
							if (i == this->m_MyID) { continue; }
							auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i).GetChara();
							//自分が当たったら押し戻す
							Vector3DX StartPosB = Chara->GetWeaponPtrNow()->GetMove().pos;;
							Vector3DX EndPosB = Chara->GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::End).pos();
							Vector3DX VecB = (EndPosB - StartPosB).normalized();

							SEGMENT_SEGMENT_RESULT Ret;
							GetSegmenttoSegment(StartPos, EndPos, StartPosB, EndPosB, &Ret);
							float Len = std::sqrtf(Ret.SegA_SegB_MinDist_Square);

							Vector3DX Vec0 = Ret.SegA_MinDist_Pos;
							Vector3DX Vec1 = Ret.SegB_MinDist_Pos;

							if (Len <= Radius &&
								(0.f < Ret.SegA_MinDist_Pos1_Pos2_t && Ret.SegA_MinDist_Pos1_Pos2_t < 1.f) &&
								(0.f < Ret.SegB_MinDist_Pos1_Pos2_t && Ret.SegB_MinDist_Pos1_Pos2_t < 1.f)
								) {
								//*
									{
										Vector3DX Vec2 = Vec0 + (Vec1 - Vec0).normalized() * (Len - Radius);
										auto RetMat = Matrix4x4DX::RotVec2(VecA, (Vec2 - StartPos).normalized());//*tmp_gunmat2.rotation().inverse();
										auto ZVec = RetMat.zvec();
										auto XZ = std::hypotf(ZVec.x, ZVec.z);
										m_BambooVec.y += atan2f(ZVec.x, ZVec.z)*0.5f;
										m_BambooVec.x += atan2f(ZVec.y, XZ)*0.5f;
										m_BambooVec.Set(std::clamp(m_BambooVec.x, deg2rad(-10), deg2rad(10)), std::clamp(m_BambooVec.y, deg2rad(-30), deg2rad(30)), 0.f);
									}
									//*/
									{
										Vector3DX Vec2 = Vec1 + (Vec0 - Vec1).normalized() * (Len - Radius);
										auto RetMat = Matrix4x4DX::RotVec2(VecB, (Vec2 - StartPosB).normalized());//*tmp_gunmat2.rotation().inverse();
										auto ZVec = RetMat.zvec();
										auto XZ = std::hypotf(ZVec.x, ZVec.z);
										Chara->m_BambooVec.y += atan2f(ZVec.x, ZVec.z)*1.5f;
										Chara->m_BambooVec.x += atan2f(ZVec.y, XZ)*1.5f;
									}
									IsHit = true;
							}
						}
						if (IsHit) { break; }
					}
					auto tmp_gunmat2 =
						Matrix4x4DX::RotAxis(Vector3DX::right(), m_BambooVec.x)*
						Matrix4x4DX::RotAxis(Vector3DX::forward(), m_BambooVec.y)*
						tmp_gunmat.rotation() *
						Matrix4x4DX::Mtrans(tmp_gunmat.pos());
					GetWeaponPtrNow()->SetMove(tmp_gunmat2.rotation(), tmp_gunmat2.pos());
				}
				//手の位置を制御
				if ((m_MyID == 0) || this->CanLookTarget) {
					GunPos = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::RightHandPos).pos();
					Gunyvec = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::RightHandYvec).pos() - GunPos;
					Gunzvec = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::RightHandZvec).pos() - GunPos;
					{
						HandsPos = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::LeftHandPos).pos();
						Handsyvec = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::LeftHandYvec).pos() - HandsPos;
						Handszvec = GetWeaponPtrNow()->GetFrameWorldMat(WeaponFrame::LeftHandZvec).pos() - HandsPos;

						/*
						Vector3DX MagPos = GetFrameWorldMat(GetFrame((int)CharaFrame::LeftMag)).pos();
						Vector3DX Magyvec = GetFrameWorldMat(GetFrame((int)CharaFrame::LeftMagyvec)).pos() - MagPos;
						Vector3DX Magzvec = GetFrameWorldMat(GetFrame((int)CharaFrame::LeftMagzvec)).pos() - MagPos;
						HandsPos = Lerp(HandsPos, MagPos, 0.f);
						Handsyvec = Lerp(Handsyvec, Magyvec, 0.f);
						Handszvec = Lerp(Handszvec, Magzvec, 0.f);
						//*/
					}
					//腕座標指定
					move_RightArm(GunPos, Gunyvec, Gunzvec);
					move_LeftArm(HandsPos, Handsyvec, Handszvec);
				}
			}
		}
		//
		void			CharacterClass::ValueSet(PlayerID pID, bool, CharaTypeID value) noexcept {
			this->m_CharaType = value;
			this->m_MyID = pID;
			//
			StaminaControl::InitStamina();
			EyeSwingControl::InitEyeSwing();
			KeyControl::InitOverride();
			//
			for (auto& a : this->m_Arm) { a.Init(false); }
			this->m_Arm[(int)EnumWeaponAnimType::Ready].Init(true);
			this->m_CharaSound = -1;
			m_CharaAction = EnumWeaponAnimType::Ready;
			m_ActionFirstFrame = true;
		}
		void			CharacterClass::MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept {
			KeyControl::InitKey(pxRad, pyRad);
			this->m_move.vec.Set(0, 0, 0);
			this->m_move.repos = pPos;
			this->m_move.Speed = 0.f;
			SetMove(Matrix4x4DX::RotAxis(Vector3DX::up(), KeyControl::GetRad().y), pPos);
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			Vector3DX m_MouseVec; m_MouseVec.Set(pInput.GetAddxRad()*deg2rad(0.1f), pInput.GetAddyRad()*deg2rad(0.1f), 0.f);

			m_GuardVec += m_MouseVec;
			m_GuardVec.x = std::clamp(m_GuardVec.x, deg2rad(-30), deg2rad(30));
			m_GuardVec.y = std::clamp(m_GuardVec.y, deg2rad(-30), deg2rad(30));
			Easing(&m_GuardVecR, m_GuardVec / deg2rad(30*2/3), 0.9f, EasingType::OutExpo);

			Easing(&m_MouseVecR, m_MouseVec, 0.8f, EasingType::OutExpo);
			
			InputControl Input = pInput;
			{
				float pp_y = 0.f;
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID()).GetChara();

				auto TgtPos = Target->GetMove().posbuf;
				auto MyPos = this->GetMove().posbuf;

				auto Dir = this->GetEyeMatrix().zvec() * -1.f;
				auto Dir_XZ = Dir; Dir_XZ.y = (0.f);Dir_XZ = Dir_XZ.normalized();

				auto Vec = (TgtPos - MyPos); Vec.y = (0.f);Vec = Vec.normalized();

				auto IsFront = ((Vector3DX::Dot(Dir_XZ, Vec)) > 0.f);
				auto cross = Vector3DX::Cross(Dir_XZ, Vec).y;
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
				pp_y *= (KeyControl::GetRun() ? 0.f : 1.f);
				pp_y = std::clamp(pp_y*0.5f, -0.2f, 0.2f);
				Input.SetAddxRad(0.f);
				Input.SetAddyRad(pp_y);
			}
			//
			KeyControl::InputKey(Input, pReady, Vector3DX::zero());
		}
		//
		void			CharacterClass::Init_Sub(void) noexcept {
			m_HeartUp = 0.f;
			m_HeartUpR = 0.f;
		}
		void			CharacterClass::FirstExecute(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				if (GetMyPlayerID() == 0) {
					//this->GetObj().SetOpacityRate(0.25f);
					//GetObj().material_AlphaTestAll(true, DX_CMP_GREATER, 0);								//本体
				}
				m_BambooVec.Set(0.f, 0.f, 0.f);
				m_MouseVecR.Set(0.f, 0.f, 0.f);
			}
			//
			ExecuteInput();
			//
			ExecuteAction();
			//
			KeyControl::UpdateKeyRad();
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::Upper));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::Upper),
										 GetFrameLocalMat(GetFrame((int)CharaFrame::Upper)).rotation() * Matrix4x4DX::RotAxis(Vector3DX::right(), -KeyControl::GetRad().x / 2.f) * (GetCharaDir()*this->m_move.mat.inverse()).rotation()
										 * GetFrameBaseLocalMat((int)CharaFrame::Upper));
			GetObj().ResetFrameUserLocalMatrix(GetFrame((int)CharaFrame::Upper2));
			GetObj().SetFrameLocalMatrix(GetFrame((int)CharaFrame::Upper2),
										 GetFrameLocalMat(GetFrame((int)CharaFrame::Upper2)).rotation() * Matrix4x4DX::RotAxis(Vector3DX::right(), KeyControl::GetRad().x / 2.f)
										 * GetFrameBaseLocalMat((int)CharaFrame::Upper2));
			//
			ExecuteAnim();
			//移動の際の視点制御
			EyeSwingControl::UpdateEyeSwing(GetCharaDir(), KeyControl::GetRun() ? 0.5f : ((KeyControl::GetVec().magnitude()*DrawParts->GetFps() / 60.f) / 0.65f), KeyControl::GetRun() ? 12.f : 5.f);
			//
			ExecuteSound();
			//
			ExecuteMatrix();
			//
			WalkSwingControl::UpdateWalkSwing(GetEyeMatrix().pos() - GetCharaPosition(), std::clamp(this->m_move.Speed / 2.f, 0.f, 1.f));
		}
		void			CharacterClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if ((CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f*Scale_Rate, -0.5f*Scale_Rate, -2.5f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f*Scale_Rate, 2.f*Scale_Rate, 2.5f*Scale_Rate)).get()) == FALSE)
					) {
					//キャラ描画
					//MV1SetMaterialTypeAll(this->GetObj().GetHandle(), DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM);
					if (GetMyPlayerID() == 0) {
						if (isDrawSemiTrans) {
							this->GetObj().DrawModel();
						}
					}
					else {
						for (int i = 0; i < this->GetObj().GetMeshNum(); i++) {
							if ((MV1GetMeshSemiTransState(this->GetObj().GetHandle(), i) == TRUE) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
					}
				}
			}
		}
		void			CharacterClass::DrawShadow(void) noexcept {
			if (this->m_IsActive) {
				this->GetObj().DrawModel();
			}
		}
		void			CharacterClass::Dispose_Sub(void) noexcept {
			this->m_BackGround.reset();
		}
	};
};
