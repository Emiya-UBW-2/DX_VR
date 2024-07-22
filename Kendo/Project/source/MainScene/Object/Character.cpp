#include	"Character.hpp"

#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			CharacterClass::move_RightArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept {
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm)));
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm2)));
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightWrist)));
			auto matBase = GetParentFrameWorldMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm))).rotation().inverse();

			Vector3DX Gunxvec = Vector3DX::Cross(Gunzvec, Gunyvec*-1.f)*-1.f;

			Vector3DX Localyvec = Vector3DX::vget(0.f, 0.f, -1.f).normalized();
			Vector3DX Localzvec = Vector3DX::vget(-1.f, -1.f, 0.f).normalized();

			Vector3DX RetPos = GunPos;
			//基準
			auto vec_a1 = Matrix4x4DX::Vtrans((RetPos - GetFramePosition(CharaFrame::RightArm)).normalized(), matBase);//基準
			Vector3DX vec_a1L1 = Vector3DX::vget(-0.5f, -1.f, vec_a1.y / -abs(vec_a1.z)).normalized();//x=0とする
			float cos_t = GetCosFormula((GetFramePosition(CharaFrame::RightWrist) - GetFramePosition(CharaFrame::RightArm2)).magnitude(), (GetFramePosition(CharaFrame::RightArm2) - GetFramePosition(CharaFrame::RightArm)).magnitude(), (GetFramePosition(CharaFrame::RightArm) - RetPos).magnitude());
			auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//上腕
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm)), Matrix4x4DX::identity() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)));
			Matrix4x4DX a1_inv = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(GetFramePosition(CharaFrame::RightArm2) - GetFramePosition(CharaFrame::RightArm), matBase), vec_t);
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm)), a1_inv * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)));
			//下腕
			matBase = GetParentFrameWorldMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm2))).rotation().inverse();
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm2)), Matrix4x4DX::identity() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)));
			Matrix4x4DX a2_inv = Matrix4x4DX::RotVec2(
				Matrix4x4DX::Vtrans(GetFramePosition(CharaFrame::RightWrist) - GetFramePosition(CharaFrame::RightArm2), matBase),
				Matrix4x4DX::Vtrans(RetPos - GetFramePosition(CharaFrame::RightArm2), matBase));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm2)), a2_inv * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)));
			//手
			auto AngleOf2Vector = [&](Vector3DX A, Vector3DX B) { return acos(Vector3DX::Dot(A, B) / (A.magnitude() * B.magnitude())); };			//２つのベクトルABのなす角度θを求める
			matBase = GetParentFrameWorldMatrix(GetFrame(static_cast<int>(CharaFrame::RightWrist))).rotation().inverse();
			Matrix4x4DX mat1;
			{
				auto zvec = Matrix4x4DX::Vtrans(Localzvec, GetFrameWorldMat(GetFrame(static_cast<int>(CharaFrame::RightWrist))).rotation());
				mat1 = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(zvec, matBase), Matrix4x4DX::Vtrans(Gunzvec, matBase)) * mat1;
				GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightWrist)), mat1 * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)));
				auto xvec = Matrix4x4DX::Vtrans(Localyvec, GetFrameWorldMat(GetFrame(static_cast<int>(CharaFrame::RightWrist))).rotation());
				mat1 = Matrix4x4DX::RotAxis(Localzvec, AngleOf2Vector(xvec, Gunxvec)*((Vector3DX::Dot((Gunyvec*-1.f), xvec) > 0.f) ? -1.f : 1.f))*mat1;
			}
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightWrist)), mat1 * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)));
		}
		void			CharacterClass::move_LeftArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept {
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm)));
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm2)));
			GetObj().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftWrist)));
			auto matBase = GetParentFrameWorldMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm))).rotation().inverse();

			Vector3DX Gunxvec = Vector3DX::Cross(Gunzvec, Gunyvec)*-1.f;

			Vector3DX Localyvec = Vector3DX::vget(0.f, 0.f, -1.f).normalized();
			Vector3DX Localzvec = Vector3DX::vget(1.f, -1.f, 0.f).normalized();

			Vector3DX RetPos = GunPos;
			//基準
			auto vec_a1 = Matrix4x4DX::Vtrans((RetPos - GetFramePosition(CharaFrame::LeftArm)).normalized(), matBase);//基準
			auto vec_a1L1 = Vector3DX(Vector3DX::vget(1.5f, -1.f, vec_a1.y / -abs(vec_a1.z))).normalized();//x=0とする
			float cos_t = GetCosFormula((GetFramePosition(CharaFrame::LeftWrist) - GetFramePosition(CharaFrame::LeftArm2)).magnitude(), (GetFramePosition(CharaFrame::LeftArm2) - GetFramePosition(CharaFrame::LeftArm)).magnitude(), (GetFramePosition(CharaFrame::LeftArm) - RetPos).magnitude());
			auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
			//上腕
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm)), Matrix4x4DX::identity() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)));
			Matrix4x4DX a1_inv = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(GetFramePosition(CharaFrame::LeftArm2) - GetFramePosition(CharaFrame::LeftArm), matBase), vec_t);
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm)), a1_inv * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)));
			//下腕
			matBase = GetParentFrameWorldMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm2))).rotation().inverse();
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm2)), Matrix4x4DX::identity() * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)));
			Matrix4x4DX a2_inv = Matrix4x4DX::RotVec2(
				Matrix4x4DX::Vtrans(GetFramePosition(CharaFrame::LeftWrist) - GetFramePosition(CharaFrame::LeftArm2), matBase),
				Matrix4x4DX::Vtrans(RetPos - GetFramePosition(CharaFrame::LeftArm2), matBase));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm2)), a2_inv * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)));
			//手
			auto AngleOf2Vector = [&](Vector3DX A, Vector3DX B) { return acos(Vector3DX::Dot(A, B) / (A.magnitude() * B.magnitude())); };			//２つのベクトルABのなす角度θを求める
			matBase = GetParentFrameWorldMatrix(GetFrame(static_cast<int>(CharaFrame::LeftWrist))).rotation().inverse();
			Matrix4x4DX mat1;
			{
				auto zvec = Matrix4x4DX::Vtrans(Localzvec, GetFrameWorldMat(GetFrame(static_cast<int>(CharaFrame::LeftWrist))).rotation());
				mat1 = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(zvec, matBase), Matrix4x4DX::Vtrans(Gunzvec, matBase)) * mat1;
				GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftWrist)), mat1 * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)));
				auto xvec = Matrix4x4DX::Vtrans(Localyvec, GetFrameWorldMat(GetFrame(static_cast<int>(CharaFrame::LeftWrist))).rotation());
				mat1 = Matrix4x4DX::RotAxis(Localzvec, AngleOf2Vector(xvec, Gunxvec)*((Vector3DX::Dot(Gunyvec, xvec) > 0.f) ? -1.f : 1.f))*mat1;
			}
			Matrix4x4DX RetMat = mat1;
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftWrist)), RetMat * GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)));
		}
		const Matrix3x3DX CharacterClass::GetCharaDir(void) const noexcept {
			float XRad = 0.f;
			this->m_move.GetMat().GetRadian(&XRad, nullptr, nullptr);
			auto tmpUpperMatrix =
				Matrix3x3DX::RotAxis(Vector3DX::right(), XRad) *
				Matrix3x3DX::RotAxis(Vector3DX::up(), Lerp(KeyControl::GetYRadBottomChange(), 0.f, this->m_Arm[static_cast<int>(EnumWeaponAnimType::Run)].Per()));
			return tmpUpperMatrix * this->m_BaseMatrix;
		}
		//
		void			CharacterClass::OverrideReady(void) noexcept {
			FrontAttack_End();
			Do_End();
			BackAttack_End();
			SetIsRunning(false);
			m_CharaAction = EnumWeaponAnimType::Ready;
		}
		void			CharacterClass::FrontAttack_Start(void) noexcept {
			GetWeaponPtrNow()->ResetAnim();
			m_NormalActionTime = GetWeaponPtrNow()->GetGunTotalTime(m_CharaAction);
			SetIsFrontAttacking(true);
			this->GetObj().SetAnim(static_cast<int>(CharaAnimeID::Bottom_Stand_Attack)).GoStart();
			this->m_CharaSound = -1;

			auto* SE = SoundPool::Instance();
			SE->Get(static_cast<int>(SoundEnum::Kendo_Swing)).Play_3D(0, GetFramePosition(CharaFrame::RightWrist), Scale_Rate * 5.f);
		}
		void CharacterClass::FrontAttack_End(void) noexcept {
			SetIsFrontAttacking(false);
			m_NormalActionTime = 0.f;
		}
		void			CharacterClass::Do_Start(void) noexcept {
			GetWeaponPtrNow()->ResetAnim();
			m_NormalActionTime = GetWeaponPtrNow()->GetGunTotalTime(m_CharaAction);
			SetIsDouAttacking(true);
			this->GetObj().SetAnim(static_cast<int>(CharaAnimeID::Bottom_Stand_Attack)).GoStart();
			this->m_CharaSound = -1;
		}
		void CharacterClass::Do_End(void) noexcept {
			SetIsDouAttacking(false);
			m_NormalActionTime = 0.f;
		}
		void			CharacterClass::Tsuba_Start(void) noexcept {
			m_NormalActionTime = 0.5f;
		}
		void			CharacterClass::Tsuba_End(void) noexcept {
		}
		void			CharacterClass::OverrideTsuba(void) noexcept {
			FrontAttack_End();
			Do_End();
			BackAttack_End();
			SetIsRunning(false);
			m_CharaAction = EnumWeaponAnimType::Tsuba;
		}
		void			CharacterClass::BackAttack_Start(void) noexcept {
			GetWeaponPtrNow()->ResetAnim();
			m_NormalActionTime = std::max(0.75f, GetWeaponPtrNow()->GetGunTotalTime(m_CharaAction));
			SetIsBackAttacking(true);
			this->GetObj().SetAnim(static_cast<int>(CharaAnimeID::Bottom_Stand_Attack)).GoStart();
			this->m_CharaSound = -1;
		}
		void CharacterClass::BackAttack_End(void) noexcept {
			SetIsBackAttacking(false);
			m_NormalActionTime = 0.f;
		}

		//
		const Matrix4x4DX CharacterClass::GetEyeMatrix(void) const noexcept {
			float XRad = 0.f;
			this->m_move.GetMat().GetRadian(&XRad, nullptr, nullptr);
			Matrix3x3DX tmpUpperMatrix =
				Matrix3x3DX::RotAxis(Vector3DX::right(), XRad) *
				Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetYRadBottomChange());

			tmpUpperMatrix *= this->m_BaseMatrix;

			Vector3DX EyePosition = (GetFramePosition(CharaFrame::LeftEye) + GetFramePosition(CharaFrame::RightEye)) / 2.f + tmpUpperMatrix.zvec()*(-0.04f*Scale_Rate);

			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((this->m_MyID != this->m_ViewID) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			if (HeadBobbing) {
				EyePosition += EyeSwingControl::GetEyeSwingPos();
			}
			return tmpUpperMatrix.Get44DX() * Matrix4x4DX::Mtrans(EyePosition);
		}
		bool			CharacterClass::SetDamageEvent(const DamageEvent& value) noexcept {
			if (this->m_MyID == value.DamageID) {
				//ダメージ
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
				Vector3DX Vec = this->m_move.GetPos() - Vector3DX::zero();
				float Len = 11.f / 2.f*Scale_Rate;
				if ((Vec.x < -Len || Len < Vec.x) ||
					(Vec.z < -Len || Len < Vec.z)) {
					IsOutArea = true;
				}
			}
			//掛け声
			if (KeyControl::GetInputControl().GetPADSPress(PADS::JUMP) && m_YaTimer<=0.f) {
				SE->Get(static_cast<int>(SoundEnum::Voice_Ya)).Play_3D(0, GetFramePosition(CharaFrame::Head), Scale_Rate * 35.f);
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

							auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetChara();

							auto& TgtPos = Target->GetMove().GetPos();
							auto& MyPos = this->GetMove().GetPos();

							auto Dir = this->GetEyeMatrix().zvec() * -1.f;
							auto Dir_XZ = Dir; Dir_XZ.y = (0.f);Dir_XZ = Dir_XZ.normalized();

							auto Vec = (TgtPos - MyPos); Vec.y = (0.f);

							auto IsFront = ((Vector3DX::Dot(Dir_XZ, Vec.normalized())) > 0.f);
							auto cross = Vector3DX::Cross(Dir_XZ, Vec.normalized()).y;

							float Radius = (0.5f+ 0.5f)*Scale_Rate;
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
						SE->Get(static_cast<int>(SoundEnum::Voice_Men)).StopAll(0);
						SE->Get(static_cast<int>(SoundEnum::Voice_Kote)).StopAll(0);
						SE->Get(static_cast<int>(SoundEnum::Voice_Dou)).StopAll(0);
						SE->Get(static_cast<int>(SoundEnum::Voice_Tsuki)).StopAll(0);
						switch (m_CharaAction) {
						case EnumWeaponAnimType::Men:
							SE->Get(static_cast<int>(SoundEnum::Voice_Men)).Play_3D(0, GetFramePosition(CharaFrame::Head), Scale_Rate * 35.f);
							break;
						case EnumWeaponAnimType::Kote:
							SE->Get(static_cast<int>(SoundEnum::Voice_Kote)).Play_3D(0, GetFramePosition(CharaFrame::Head), Scale_Rate * 35.f);
							break;
						case EnumWeaponAnimType::Tsuki:
							SE->Get(static_cast<int>(SoundEnum::Voice_Tsuki)).Play_3D(0, GetFramePosition(CharaFrame::Head), Scale_Rate * 35.f);
							break;
						case FPS_n2::Sceneclass::EnumWeaponAnimType::Ready:
						case FPS_n2::Sceneclass::EnumWeaponAnimType::Run:
						//case FPS_n2::Sceneclass::EnumWeaponAnimType::Men:
						//case FPS_n2::Sceneclass::EnumWeaponAnimType::Kote:
						case FPS_n2::Sceneclass::EnumWeaponAnimType::Dou:
						//case FPS_n2::Sceneclass::EnumWeaponAnimType::Tsuki:
						case FPS_n2::Sceneclass::EnumWeaponAnimType::Tsuba:
						case FPS_n2::Sceneclass::EnumWeaponAnimType::HikiMen:
						case FPS_n2::Sceneclass::EnumWeaponAnimType::HikiKote:
						case FPS_n2::Sceneclass::EnumWeaponAnimType::HikiDou:
						case FPS_n2::Sceneclass::EnumWeaponAnimType::GuardStart:
						case FPS_n2::Sceneclass::EnumWeaponAnimType::GuardSuriage:
						case FPS_n2::Sceneclass::EnumWeaponAnimType::Max:
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
								case FPS_n2::Sceneclass::EnumWeaponAnimType::Ready:
								case FPS_n2::Sceneclass::EnumWeaponAnimType::Run:
								//case FPS_n2::Sceneclass::EnumWeaponAnimType::Men:
								//case FPS_n2::Sceneclass::EnumWeaponAnimType::Kote:
								case FPS_n2::Sceneclass::EnumWeaponAnimType::Dou:
								//case FPS_n2::Sceneclass::EnumWeaponAnimType::Tsuki:
								case FPS_n2::Sceneclass::EnumWeaponAnimType::Tsuba:
								case FPS_n2::Sceneclass::EnumWeaponAnimType::HikiMen:
								case FPS_n2::Sceneclass::EnumWeaponAnimType::HikiKote:
								case FPS_n2::Sceneclass::EnumWeaponAnimType::HikiDou:
								case FPS_n2::Sceneclass::EnumWeaponAnimType::GuardStart:
								case FPS_n2::Sceneclass::EnumWeaponAnimType::GuardSuriage:
								case FPS_n2::Sceneclass::EnumWeaponAnimType::Max:
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
						SE->Get(static_cast<int>(SoundEnum::Voice_Men)).StopAll(0);
						SE->Get(static_cast<int>(SoundEnum::Voice_Kote)).StopAll(0);
						SE->Get(static_cast<int>(SoundEnum::Voice_Dou)).StopAll(0);
						SE->Get(static_cast<int>(SoundEnum::Voice_Tsuki)).StopAll(0);
						SE->Get(static_cast<int>(SoundEnum::Voice_Dou)).Play_3D(0, GetFramePosition(CharaFrame::Head), Scale_Rate * 35.f);
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
						auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetChara();
						Target->OverrideReady();
						SE->Get(static_cast<int>(SoundEnum::Voice_Men)).StopAll(0);
						SE->Get(static_cast<int>(SoundEnum::Voice_Kote)).StopAll(0);
						SE->Get(static_cast<int>(SoundEnum::Voice_Dou)).StopAll(0);
						SE->Get(static_cast<int>(SoundEnum::Voice_Tsuki)).StopAll(0);
						switch (m_CharaAction) {
							case EnumWeaponAnimType::HikiMen:
								SE->Get(static_cast<int>(SoundEnum::Voice_Men)).Play_3D(0, GetFramePosition(CharaFrame::Head), Scale_Rate * 35.f);
								break;
							case EnumWeaponAnimType::HikiKote:
								SE->Get(static_cast<int>(SoundEnum::Voice_Kote)).Play_3D(0, GetFramePosition(CharaFrame::Head), Scale_Rate * 35.f);
								break;
							case EnumWeaponAnimType::HikiDou:
								SE->Get(static_cast<int>(SoundEnum::Voice_Dou)).Play_3D(0, GetFramePosition(CharaFrame::Head), Scale_Rate * 35.f);
								break;
							case FPS_n2::Sceneclass::EnumWeaponAnimType::Ready:
							case FPS_n2::Sceneclass::EnumWeaponAnimType::Run:
							case FPS_n2::Sceneclass::EnumWeaponAnimType::Men:
							case FPS_n2::Sceneclass::EnumWeaponAnimType::Kote:
							case FPS_n2::Sceneclass::EnumWeaponAnimType::Dou:
							case FPS_n2::Sceneclass::EnumWeaponAnimType::Tsuki:
							case FPS_n2::Sceneclass::EnumWeaponAnimType::Tsuba:
							//case FPS_n2::Sceneclass::EnumWeaponAnimType::HikiMen:
							//case FPS_n2::Sceneclass::EnumWeaponAnimType::HikiKote:
							//case FPS_n2::Sceneclass::EnumWeaponAnimType::HikiDou:
							case FPS_n2::Sceneclass::EnumWeaponAnimType::GuardStart:
							case FPS_n2::Sceneclass::EnumWeaponAnimType::GuardSuriage:
							case FPS_n2::Sceneclass::EnumWeaponAnimType::Max:
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
				case EnumWeaponAnimType::Max:
				default:
					break;
			}
			//
			m_ActionFirstFrame = (PrevAction != m_CharaAction);
		}
		void			CharacterClass::ExecuteAction(void) noexcept {
			auto IsGuardAction = [](EnumWeaponAnimType value) {
				switch (value) {
				case EnumWeaponAnimType::GuardSuriage:
					return true;
				case EnumWeaponAnimType::Ready:
				case EnumWeaponAnimType::Run:
				case EnumWeaponAnimType::Men:
				case EnumWeaponAnimType::Kote:
				case EnumWeaponAnimType::Dou:
				case EnumWeaponAnimType::Tsuki:
				case EnumWeaponAnimType::Tsuba:
				case EnumWeaponAnimType::HikiMen:
				case EnumWeaponAnimType::HikiKote:
				case EnumWeaponAnimType::HikiDou:
				case EnumWeaponAnimType::GuardStart:
				//case EnumWeaponAnimType::GuardSuriage:
				case EnumWeaponAnimType::Max:
				default:
					return false;
				}
			};
			for (size_t index = 0; index < static_cast<size_t>(EnumWeaponAnimType::Max); index++) {
				this->m_Arm[index].Execute((m_CharaAction == (EnumWeaponAnimType)index), 0.1f, 0.1f, IsGuardAction((EnumWeaponAnimType)index) ? 0.75f : 0.9f);
				//this->m_Arm[index].Init((m_CharaAction == (EnumWeaponAnimType)index));
			}
		}
		void			CharacterClass::ExecuteAnim(void) noexcept {
			//上半身アニメ演算
			GetCharaAnimeBufID(CharaAnimeID::Upper_Ready) = 1.f;
			GetCharaAnimeBufID(CharaAnimeID::Hand_Ready) = 1.f;
			//下半身アニメ演算
			ObjectBaseClass::SetAnimLoop(static_cast<int>(KeyControl::GetBottomTurnAnimSel()), 0.5f);
			ObjectBaseClass::SetAnimOnce(static_cast<int>(CharaAnimeID::Bottom_Stand_Attack), KeyControl::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(CharaAnimeID::Bottom_Stand_Run), KeyControl::GetSpeedPer());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(KeyControl::GetBottomWalkAnimSel()), KeyControl::GetVecFront());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(KeyControl::GetBottomLeftStepAnimSel()), KeyControl::GetVecLeft());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(KeyControl::GetBottomWalkBackAnimSel()), KeyControl::GetVecRear());
			ObjectBaseClass::SetAnimLoop(static_cast<int>(KeyControl::GetBottomRightStepAnimSel()), KeyControl::GetVecRight());
			//アニメ反映
			for (int i = 0; i < static_cast<int>(GetObj().GetAnimNum()); i++) {
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
				auto Time = this->GetObj().SetAnim(static_cast<int>(this->m_BottomAnimSelect)).GetTime();
				if (!GetRun()) {
					//L
					if ((9.f < Time && Time < 10.f)) {
						if (this->m_CharaSound != 1) {
							this->m_CharaSound = 1;
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale_Rate * 5.f*1.5f) {

								SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::LeftFoot), Scale_Rate * 5.f);
							}
						}
					}
					//R
					if ((27.f < Time &&Time < 28.f)) {
						if (this->m_CharaSound != 3) {
							this->m_CharaSound = 3;
							if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale_Rate * 5.f*1.5f) {

								SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::RightFoot), Scale_Rate * 5.f);
							}
						}
					}
				}
				else {
					if (GetCharaAnimeBufID(CharaAnimeID::Bottom_Stand_Attack) > 0.5f) {
						if ((9.f < Time && Time < 10.f)) {
							if (this->m_CharaSound != 7) {
								this->m_CharaSound = 7;
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale_Rate * 5.f*1.5f) {
									SE->Get(static_cast<int>(SoundEnum::Kendo_Foot)).Play_3D(0, GetFramePosition(CharaFrame::RightFoot), Scale_Rate * 15.f);
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
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::LeftFoot)).magnitude() < Scale_Rate * 5.f*1.5f) {
									SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::LeftFoot), Scale_Rate * 15.f);
								}
							}
						}
						//R
						if (
							(10.f < Time &&Time < 11.f)
							) {
							if (this->m_CharaSound != 6) {
								this->m_CharaSound = 6;
								if ((DrawParts->GetMainCamera().GetCamPos() - GetFramePosition(CharaFrame::RightFoot)).magnitude() < Scale_Rate * 5.f*1.5f) {
									SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play_3D(0, GetFramePosition(CharaFrame::RightFoot), Scale_Rate * 15.f);
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
				SE->Get(static_cast<int>(SoundEnum::StandupFoot)).Play_3D(0, GetFramePosition(CharaFrame::Upper), Scale_Rate * 3.f);
			}
			//心拍音
#ifdef DEBUG
			printfDx("HEART : %f\n", m_HeartUp);
#endif
			Easing(&m_HeartUpR, m_HeartUp, 0.99f, EasingType::OutExpo);
			if (StaminaControl::ExcuteStamina(m_HeartUpR / DrawParts->GetFps())) {
				if (this->m_MyID == this->m_ViewID) {
					SE->Get(static_cast<int>(SoundEnum::Heart)).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
			}
		}
		//SetMat指示更新
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			auto* BackGround = BackGroundClass::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			Vector3DX pos = this->m_move.GetPosBuf();
			//vector
			bool IsHitGround = false;//(pos.y <= 0.f); //高度0なら止まる
			float groundYpos = 0.f;
			{
				Vector3DX EndPos = pos - Vector3DX::up() * 5.f;
				IsHitGround = BackGround->CheckLinetoMap(pos + Vector3DX::up() * 20.f, &EndPos, true);
				groundYpos = EndPos.y;
			}
			Vector3DX vec = KeyControl::GetVec();
			if (IsHitGround) {
				auto yPos = pos.y;
				Easing(&yPos, groundYpos, 0.8f, EasingType::OutExpo);
				pos.y = (yPos);

				vec = KeyControl::GetVec();
				vec.y = (0.f);
			}
			else {
				vec = KeyControl::GetVec();
				vec.y = (this->m_move.GetVec().y + (M_GR / (DrawParts->GetFps() * DrawParts->GetFps())));
			}
			this->m_move.SetVec(vec);
			pos += this->m_move.GetVec();
			if (m_CharaAction == EnumWeaponAnimType::Tsuba) {
				auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - GetMyPlayerID())->GetChara();

				auto& TgtPos = Target->GetMove().GetPos();
				auto& MyPos = this->GetMove().GetPos();

				auto Vec = (TgtPos - MyPos); Vec.y = (0.f);Vec = Vec.normalized();

				float Radius = (0.5f + 0.25f)*Scale_Rate;
				pos = TgtPos - Vec * Radius;
			}
			//壁判定
			BackGround->CheckMapWall(this->m_move.GetRePos(), &pos, 0.6f * Scale_Rate);
			//ほかプレイヤーとの判定
			if (true) {
				float Radius = 0.5f*Scale_Rate;
				for (int i = 0; i < Player_num; i++) {
					if (i == this->m_MyID) { continue; }
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
					//自分が当たったら押し戻す
					Vector3DX Vec = (Chara->GetCharaPosition() - this->GetCharaPosition()); Vec.y = (0.f);
					float Len = Vec.magnitude();
					if (Len < Radius) {
						pos += Vec.normalized() * (Len - Radius);
					}
				}
			}
			//座標オーバーライド
			if (this->m_MoveOverRideFlag) {
				this->m_MoveOverRideFlag = false;
				this->m_move = this->m_OverRideInfo;
			}
			auto* OptionParts = OPTION::Instance();
			bool HeadBobbing = ((this->m_MyID != this->m_ViewID) || OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing));
			m_BaseMatrix = Matrix3x3DX::RotAxis(Vector3DX::forward(), HeadBobbing ? (KeyControl::GetZRad() / 2.f) : 0.f) * Matrix3x3DX::RotAxis(Vector3DX::up(), KeyControl::GetYRadBottom());

			this->m_move.SetPos(pos);
			this->m_move.Update(0.9f, 0.f);
			UpdateObjMatrix(m_BaseMatrix, this->m_move.GetPos());

			Matrix3x3DX Prev = this->m_move.GetMat();
			Prev.SetRadian(KeyControl::GetRadBuf().x, KeyControl::GetRadBuf().y, 0.f);
			this->m_move.SetMat(Prev);

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
					for (size_t i = 0; i < static_cast<size_t>(EnumWeaponAnimType::Max); i++) {
						Matrix4x4DX AnimData;
						if (!GetWeaponPtrNow()->GetGunAnime((EnumWeaponAnimType)i, &AnimData)) { continue; }
						AnimMat = Lerp(AnimMat, AnimData, this->m_Arm[i].Per());
					}
					Vector3DX Post0 = GetFramePosition(CharaFrame::Head);
					Post0 += Matrix3x3DX::Vtrans(AnimMat.pos(), GetCharaDir());

					auto tmp_gunmat = Matrix3x3DX::RotVec2(Vector3DX::forward(), AnimMat.zvec());
					auto tmp_gunpos = Post0;
					tmp_gunmat *= Matrix3x3DX::RotVec2(tmp_gunmat.yvec(), AnimMat.yvec());
					tmp_gunmat *=
						KeyControl::GetGunSwingMat() * GetCharaDir()*
						Matrix3x3DX::RotVec2(GetWeaponPtrNow()->GetObj().GetMatrix().yvec(), GetWeaponPtrNow()->GetObj().GetMatrix().yvec());
					//
					bool IsHit = false;
					for (int index = 0; index < 10; index++) {
						m_BambooVec += m_MouseVecR / 10.f;
						m_BambooVec.Set(std::clamp(m_BambooVec.x, deg2rad(-10), deg2rad(10)), std::clamp(m_BambooVec.y, deg2rad(-30), deg2rad(30)), 0.f);

						Vector3DX StartPosP = GetWeaponPtrNow()->GetMove().GetPos();
						Vector3DX EndPosP = GetWeaponPtrNow()->GetFramePosition(WeaponFrame::End);

						auto tmp_gunmat2 =
							Matrix3x3DX::RotAxis(Vector3DX::right(), m_BambooVec.x)*
							Matrix3x3DX::RotAxis(Vector3DX::forward(), m_BambooVec.y)*
							tmp_gunmat;
						GetWeaponPtrNow()->SetMove().SetMat(tmp_gunmat2);
						GetWeaponPtrNow()->SetMove().SetPos(tmp_gunpos);
						GetWeaponPtrNow()->SetMove().Update(0.f, 0.f);
						GetWeaponPtrNow()->UpdateObjMatrix(tmp_gunmat2, tmp_gunpos);

						Vector3DX StartPos = GetWeaponPtrNow()->GetMove().GetPos();
						Vector3DX EndPos = GetWeaponPtrNow()->GetFramePosition(WeaponFrame::End);
						Vector3DX VecA = (EndPos - StartPos).normalized();

						float Radius = 0.025f*Scale_Rate*2.f;
						for (int i = 0; i < Player_num; i++) {
							if (i == this->m_MyID) { continue; }
							auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
							//自分が当たったら押し戻す
							Vector3DX StartPosB = Chara->GetWeaponPtrNow()->GetMove().GetPos();;
							Vector3DX EndPosB = Chara->GetWeaponPtrNow()->GetFramePosition(WeaponFrame::End);
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
						Matrix3x3DX::RotAxis(Vector3DX::right(), m_BambooVec.x)*
						Matrix3x3DX::RotAxis(Vector3DX::forward(), m_BambooVec.y)*
						tmp_gunmat;
					GetWeaponPtrNow()->ResetMove(tmp_gunmat2, tmp_gunpos);
				}
				//手の位置を制御
				if ((m_MyID == this->m_ViewID) || this->CanLookTarget) {
					GunPos = GetWeaponPtrNow()->GetFramePosition(WeaponFrame::RightHandPos);
					Gunyvec = GetWeaponPtrNow()->GetFramePosition(WeaponFrame::RightHandYvec) - GunPos;
					Gunzvec = GetWeaponPtrNow()->GetFramePosition(WeaponFrame::RightHandZvec) - GunPos;
					{
						HandsPos = GetWeaponPtrNow()->GetFramePosition(WeaponFrame::LeftHandPos);
						Handsyvec = GetWeaponPtrNow()->GetFramePosition(WeaponFrame::LeftHandYvec) - HandsPos;
						Handszvec = GetWeaponPtrNow()->GetFramePosition(WeaponFrame::LeftHandZvec) - HandsPos;

						/*
						Vector3DX MagPos = GetFramePosition(CharaFrame::LeftMag);
						Vector3DX Magyvec = GetFramePosition(CharaFrame::LeftMagyvec) - MagPos;
						Vector3DX Magzvec = GetFramePosition(CharaFrame::LeftMagzvec) - MagPos;
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
			this->m_MoveOverRideFlag = false;
			//
			for (auto& a : this->m_Arm) { a.Init(false); }
			this->m_Arm[static_cast<int>(EnumWeaponAnimType::Ready)].Init(true);
			this->m_CharaSound = -1;
			m_CharaAction = EnumWeaponAnimType::Ready;
			m_ActionFirstFrame = true;
		}
		void			CharacterClass::MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept {
			KeyControl::InitKey(pxRad, pyRad);
			Matrix3x3DX Mat; Mat.SetRadian(KeyControl::GetRadBuf().x, KeyControl::GetRadBuf().y, 0.f);
			ResetMove(Mat, pPos);
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			auto* DrawParts = DXDraw::Instance();
			Vector3DX m_MouseVec; m_MouseVec.Set(pInput.GetAddxRad()*deg2rad(0.1f), pInput.GetAddyRad()*deg2rad(0.1f), 0.f);

			m_GuardVec += m_MouseVec;
			m_GuardVec.x = std::clamp(m_GuardVec.x, deg2rad(-30), deg2rad(30));
			m_GuardVec.y = std::clamp(m_GuardVec.y, deg2rad(-30), deg2rad(30));
			Easing(&m_GuardVecR, m_GuardVec / deg2rad(30*2/3), 0.9f, EasingType::OutExpo);

			Easing(&m_MouseVecR, m_MouseVec, 0.8f, EasingType::OutExpo);
			
			InputControl Input = pInput;
			{
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Target = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();

				auto& TgtPos = Target->GetMove().GetPos();
				auto& MyPos = this->GetMove().GetPos();

				auto Dir_XZ = this->GetEyeMatrix().zvec() * -1.f; Dir_XZ.y = (0.f);Dir_XZ = Dir_XZ.normalized();
				float Len = 0.f;
				auto Vec = (TgtPos - MyPos); Vec.y = (0.f); Len = Vec.magnitude(); Vec = Vec.normalized();
				float pp_y = 0.f;
				if ((Len > 1.5f * Scale_Rate) && !KeyControl::GetRun()) {
					float sint = Vector3DX::Cross(Vec, Dir_XZ).y;
					float cost = Vector3DX::Dot(Vec, Dir_XZ);
					auto IsFront = (cost > cos(deg2rad(40)));
					if (IsFront) {
						pp_y = std::clamp(-std::atan2f(sint, cost), -deg2rad(60), deg2rad(60)) * 5.f / 60.f;
					}
					else {
						pp_y = -deg2rad(60) * 5.f / 60.f;
					}
				}
				Easing(&pp_yR, pp_y, 0.8f, EasingType::OutExpo);
				Input.SetAddxRad(0.f);
				Input.SetAddyRad(pp_y);
			}
			//
			KeyControl::InputKey(Input, pReady);
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
			KeyControl::UpdateKeyRad(this->m_move);
			float XRad = 0.f;
			this->m_move.GetMat().GetRadian(&XRad, nullptr, nullptr);
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper)),
				Matrix4x4DX::RotAxis(Vector3DX::right(), -XRad / 2.f) *
				(GetCharaDir() * this->m_BaseMatrix.inverse()).Get44DX() *
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper)));
			GetObj().SetFrameLocalMatrix(GetFrame(static_cast<int>(CharaFrame::Upper2)),
				Matrix4x4DX::RotAxis(Vector3DX::right(), XRad / 2.f) *
				GetFrameBaseLocalMat(static_cast<int>(CharaFrame::Upper2)));
			//
			ExecuteAnim();
			//移動の際の視点制御
			EyeSwingControl::UpdateEyeSwing(GetCharaDir(), KeyControl::GetRun() ? 0.5f : ((KeyControl::GetVec().magnitude()*DrawParts->GetFps() / 60.f) / 0.65f), KeyControl::GetRun() ? 12.f : 5.f);
			//
			ExecuteSound();
			//
			ExecuteMatrix();
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
		}
	};
};
