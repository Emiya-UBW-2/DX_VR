#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {


		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public LifeControl,
			public KeyControl,
			public ShapeControl
		{
		private://�L�����p�����[�^�[
			const float											SpeedLimit{ 1.25f };
			const float											UpperTimerLimit = 10.f;
		private:
			CharaTypeID											m_CharaType;
			std::vector<CharaAnimeSet>							m_CharaAnimeSet;
			int													m_CharaAnimeSel{ 0 };
			//
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			VECTOR_ref											m_PosBuf;
			bool												m_PosBufOverRideFlag{ false };
			VECTOR_ref											m_PosBufOverRide;
			VECTOR_ref											m_VecBufOverRide;
			VECTOR_ref											m_RadOverRide;									//

			MATRIX_ref											m_UpperMatrix;
			float												m_LeanRad{ 0.f };
			float												m_Speed{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			float												m_NeckPer{ 0.f };
			bool												m_TurnBody{ false };
			bool												m_RunReady{ false };
			bool												m_Running{ false };
			CharaAnimeID										m_UpperAnimSelect, m_PrevUpperAnimSel;
			CharaAnimeID										m_BottomAnimSelect;
			//�e
			float												m_ReadyTimer{ 0.f };
			float												m_ReadyPer{ 0.f };

			int													m_ShotPhase{ 0 };

			float												m_MagHansPer{ 0.f };
			float												m_NeckPosOffsetPer{ 0.f };
			float												m_MoveEyePosTimer{ 0.f };
			VECTOR_ref											m_MoveEyePos;
			bool												m_IsStuckGun{ false };
			//����
			bool												m_Press_Shot{ false };
			bool												m_Press_Reload{ false };
			bool												m_Press_Aim{ false };
			//�̗�
			std::vector<HitBox>									m_HitBox;
			DamageEvent											m_DamageEvent;									//
			unsigned long long									m_DamageSwitch{ 0 };							//
			unsigned long long									m_DamageSwitchRec{ 0 };							//
			//
			float												m_LeftHandPer{ 0.f };
			Pendulum2D											m_SlingZrad;
			Pendulum2D											m_SlingYrad;

			VECTOR_ref											m_RecoilRadAdd;
			//�T�E���h
			int													m_CharaSound{ -1 };
			std::array<std::shared_ptr<GunClass>,2>				m_Gun_Ptr{ nullptr };			//�e
			std::array<float, 2>								m_SlingPer;
			int													m_GunSelect{ 0 };
			//
			bool												m_SendCamShake{ false };
		public://�Q�b�^�[
			//const auto		GetShotAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_ADS; }
			//const auto		GetTurnRatePer(void) const noexcept { return this->m_InputGround.GetTurnRatePer(); }
			//const auto		GetReadyPer(void) const noexcept { return this->m_ReadyPer; }
			//const auto&		GetPosBuf(void) const noexcept { return this->m_PosBuf; }
			//const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			//void				SetAnimOnce(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimOnce((int)ID, speed); }
			//const auto		GetParentFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix((int)this->GetObj_const().frame_parent(GetCharaFrame(frame))); }
		public://�Q�b�^�[(���b�p�[)
			const auto		GetBottomStandAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat_WalkBack : CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat_LeftStep : CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat_RightStep : CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return KeyControl::GetIsSquatGround() ? CharaAnimeID::Bottom_Squat_Turn : CharaAnimeID::Bottom_Stand_Turn; }
			const auto		GetBottomRunAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_Run; }
			const auto		GetCharaFrame(CharaFrame frame) const noexcept { return m_Frames[(int)frame].first; }
		public://�Q�b�^�[
			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->GetObj().get_anime((int)anim); }
			auto&			GetGunPtr(int ID) noexcept { return this->m_Gun_Ptr[ID]; }
			auto&			GetGunPtrNow(void) noexcept { return this->m_Gun_Ptr[this->m_GunSelect]; }
			const auto&		GetGunPtrNow_Const(void) const noexcept { return this->m_Gun_Ptr[this->m_GunSelect]; }
			const auto&		GetGunPtrNowID() const noexcept { return this->m_GunSelect; }
			const auto&		GetReticleRad(void) const noexcept { return this->m_LeanRad; }
			const auto&		GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd; }
			const auto&		GetSendCamShake(void) const noexcept { return this->m_SendCamShake; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto&		GetDamageSwitch(void) const noexcept { return this->m_DamageSwitch; }
			const auto&		GetDamageSwitchRec(void) const noexcept { return this->m_DamageSwitchRec; }
		public://�Z�b�^�[
			void			SetDamageSwitchRec(unsigned long long value) noexcept { this->m_DamageSwitchRec = value; }
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { GetObj().frame_Reset(GetCharaFrame(frame)); }
			void			LoadReticle(void) noexcept {
				for (auto& p : this->m_Gun_Ptr) {
					if (p != nullptr) {
						p->LoadReticle();
					}
				}
			}
			bool			SetDamageEvent(const DamageEvent& value) noexcept {
				if (this->m_MyID == value.ID && this->m_objType == value.CharaType) {
					SubHP(value.Damage, value.rad);
					return true;
				}
				return false;
			}
			void			SetAnimLoop(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimLoop((int)ID, speed); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(GetCharaFrame(frame), value * this->m_Frames[(int)frame].second); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { this->m_Shapes[(int)pShape].second = Per; }
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			SetPosBufOverRide(const VECTOR_ref& pPos, const VECTOR_ref& pVec, const VECTOR_ref& rad) noexcept {
				this->m_PosBufOverRideFlag = true;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide = pVec;
				this->m_RadOverRide = rad;
			}
			void			SetGunPtr(std::shared_ptr<GunClass>& pGunPtr0, std::shared_ptr<GunClass>& pGunPtr1) noexcept {
				this->m_Gun_Ptr[0] = pGunPtr0;
				this->m_SlingPer[0] = 0.f;
				this->m_Gun_Ptr[1] = pGunPtr1;
				this->m_SlingPer[1] = 1.f;
				if (this->GetGunPtr(0) != nullptr) {
					this->m_CharaAnimeSel = this->GetGunPtr(0)->GetHumanAnimType();
				}
				for (auto& p : this->m_Gun_Ptr) {
					p->SetPlayerID(this->m_MyID);
				}
				this->m_GunSelect = 1;
			}
		public://�Q�b�^�[
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix(GetCharaFrame(frame)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix(GetCharaFrame(frame)); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix((int)this->GetObj_const().frame_parent(GetCharaFrame(frame))); }
			const auto		GetGunPtrNum() const noexcept { return this->m_Gun_Ptr.size(); }
			const auto		GetCharaDir(void) const noexcept { return this->m_UpperMatrix * this->m_move.mat; }
			const auto		GetEyeVecX(void) const noexcept { return GetCharaDir().xvec(); }
			const auto		GetEyeVecY(void) const noexcept { return GetCharaDir().yvec(); }
			const auto		GetEyeVector(void) const noexcept { return GetCharaDir().zvec() * -1.f; }
			const auto		GetEyePosition(void) const noexcept {
				return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f
					+ this->GetEyeVector().Norm() * 0.5f
					+ this->GetEyeVecY() * this->m_NeckPosOffsetPer
					+ this->m_MoveEyePos;
			}
			const auto		GetScopePos(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetScopePos() : GetEyePosition(); }
			const auto		GetLensPos(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetLensPos() : VECTOR_ref::zero(); }
			const auto		GetReticleSize(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetReticleSize() : 1.f; }
			const auto		GetReticlePos(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetReticlePos() : VECTOR_ref::zero(); }
			const auto		GetLensPosSize(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetLensPosSize() : VECTOR_ref::zero(); }
			const auto		GetCanshot(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? (GetGunPtrNow_Const()->GetCanShot() && (this->m_ShotPhase <= 1)) : false; }
			const auto		GetAmmoNum(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetAmmoNum() : 0; }
			const auto		GetAmmoAll(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetAmmoAll() : 0; }
			const auto		GetShotSwitch(void) const noexcept { return this->m_ShotPhase == 1; }
			const auto		GetDownAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Down; }
			const auto		GetAimAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Ready; }
			const auto		GetCockingAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Cocking; }
			const auto		GetReloadStartAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Reload; }
			const auto		GetReloadOneAnimSel(void) const noexcept { return  (CharaAnimeID)((int)GetReloadStartAnimSel() + 1); }
			const auto		GetReloadEndAnimSel(void) const noexcept { return  (CharaAnimeID)((int)GetReloadStartAnimSel() + 2); }
			const auto		GetIsADS(void) const noexcept { return this->m_ReadyTimer == 0.f; }
			const auto		GetEyeVecMat(void) const noexcept { return GetCharaDir(); }
		private:
			//��e�`�F�b�N
			const auto		CheckAmmoHited(const AmmoClass& pAmmo) noexcept {
				bool is_Hit = false;
				for (auto& h : this->m_HitBox) {
					is_Hit |= h.Colcheck(pAmmo);
				}
				return is_Hit;
			}
			//��e����
			const auto		CalcAmmoHited(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept {
				//auto SE = SoundPool::Instance();
				for (auto& h : this->m_HitBox) {
					if (h.Colcheck(*pAmmo)) {
						h.GetColType();
						pAmmo->Penetrate();	//�ђ�
						//�_���[�W�v�Z
						auto v1 = GetEyeVector();
						auto v2 = (pShooterPos - this->m_move.pos).Norm(); v2.y(0);
						this->m_DamageEvent.SetEvent(this->m_MyID, m_objType, pAmmo->GetDamage(), atan2f(v1.cross(v2).y(), v1.dot(v2)));
						++this->m_DamageSwitch;// %= 255;//
						//�G�t�F�N�g�Z�b�g
						return true;
					}
				}
				return false;
			}
		public:
			const auto		CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept {
				std::pair<bool, bool> isDamaged{ false,false };
				if (GetMinLenSegmentToPoint(pAmmo->GetMove().repos, pAmmo->GetMove().pos, m_move.pos) <= 2.0f*Scale_Rate) {
					if (this->CheckAmmoHited(*pAmmo)) {									//�Ƃ肠���������������ǂ����T��
						isDamaged.first = true;
						if (this->CalcAmmoHited(pAmmo, pShooterPos)) {
							isDamaged.second = true;
						}
					}
				}
				return isDamaged;
			}
			void			move_RightArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			void			move_LeftArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			//
			void			ValueSet(float pxRad, float pyRad, bool SquatOn, const VECTOR_ref& pPos, PlayerID pID) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
			void			SetEyeVec(const VECTOR_ref& camvec) noexcept;
		private: //�X�V�֘A
			void			ExecuteSavePrev(void) noexcept;			//�ȑO�̏�ԕێ�														//
			void			ExecuteInput(void) noexcept;			//����																	//0.01ms
			void			ExecuteUpperMatrix(void) noexcept;		//�㔼�g��]															//0.06ms
			void			ExecuteAnim(void) noexcept;				//SetMat�w��															//0.03ms
			void			ExecuteMatrix(void) noexcept;			//SetMat�w���X�V														//0.03ms
			void			ExecuteShape(void) noexcept;			//��																	//0.01ms
			void			ExecuteHeartRate(void) noexcept;		//�S����																//0.00ms
		public: //�R���X�g���N�^�A�f�X�g���N�^
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				this->m_IsDraw = true;
			}
			~CharacterClass(void) noexcept {}
		public: //�p��
			//
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				GetAnime(GetBottomStandAnimSel()).per = 1.f;
				//
				m_CharaAnimeSet.clear();
				//M4
				m_CharaAnimeSet.resize(m_CharaAnimeSet.size() + 1);
				m_CharaAnimeSet.back().m_Down = CharaAnimeID::Upper_Down1;
				m_CharaAnimeSet.back().m_Ready = CharaAnimeID::Upper_Ready1;
				m_CharaAnimeSet.back().m_ADS = CharaAnimeID::Upper_ADS1;
				m_CharaAnimeSet.back().m_Cocking = CharaAnimeID::Upper_Cocking1;
				m_CharaAnimeSet.back().m_Reload = CharaAnimeID::Upper_Reload1Start;
				//�n���h�K��
				m_CharaAnimeSet.resize(m_CharaAnimeSet.size() + 1);
				m_CharaAnimeSet.back().m_Down = CharaAnimeID::Upper_Down2;
				m_CharaAnimeSet.back().m_Ready = CharaAnimeID::Upper_Ready2;
				m_CharaAnimeSet.back().m_ADS = CharaAnimeID::Upper_ADS2;
				m_CharaAnimeSet.back().m_Cocking = CharaAnimeID::Upper_Cocking2;
				m_CharaAnimeSet.back().m_Reload = CharaAnimeID::Upper_Reload2Start;

				m_HitBox.resize(27);
			}
			void			FirstExecute(void) noexcept override {
				//����̂ݍX�V������e
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < GetObj().get_anime().size(); i++) { GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i); }
					GetObj().work_anime();
				}
				if (GetGunPtrNow() != nullptr) {
					auto prev = this->m_CharaAnimeSel;
					auto newtmp = GetGunPtrNow()->GetHumanAnimType();
					bool ischange = true;
					if (prev != newtmp) {
						if (this->m_UpperAnimSelect == GetDownAnimSel()) {
							ischange = false;
						}
						if (ischange) {
							GetAnimeBuf(GetAimAnimSel()) = 0.f;
						}
						this->m_ReadyTimer = 0.1f;
						this->m_ReadyPer = 0.f;
					}
					this->m_CharaAnimeSel = newtmp;
					if (prev != newtmp) {
						if (ischange) {
							GetAnimeBuf(GetDownAnimSel()) = 1.f;
						}
					}
				}
				ExecuteSavePrev();			//�ȑO�̏�ԕێ�
				ExecuteInput();				//����//0.01ms
				ExecuteUpperMatrix();		//�㔼�g��]//0.06ms
				ExecuteAnim();				//AnimUpdte//0.03ms
				ExecuteMatrix();			//SetMat�w��//0.03ms
				ExecuteShape();				//��//�X�R�[�v��0.01ms
				ExecuteHeartRate();			//�S����//0.00ms
			}
			void			Draw(void) noexcept override {
				int fog_enable;
				int fog_mode;
				int fog_r, fog_g, fog_b;
				float fog_start, fog_end;
				float fog_density;

				fog_enable = GetFogEnable();													// �t�H�O���L�����ǂ������擾����( TRUE:�L��  FALSE:���� )
				fog_mode = GetFogMode();														// �t�H�O���[�h���擾����
				GetFogColor(&fog_r, &fog_g, &fog_b);											// �t�H�O�J���[���擾����
				GetFogStartEnd(&fog_start, &fog_end);											// �t�H�O���n�܂鋗���ƏI�����鋗�����擾����( 0.0f �` 1.0f )
				fog_density = GetFogDensity();													// �t�H�O�̖��x���擾����( 0.0f �` 1.0f )

				SetFogEnable(TRUE);
				SetFogColor(0, 0, 0);
				SetFogStartEnd(Scale_Rate*1.f, Scale_Rate*25.f);

				//
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-20, 0, -20)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(20, 20, 20)).get()) == FALSE
						) {
						auto* DrawParts = DXDraw::Instance();
						DrawParts->SetUseFarShadowDraw(false);
						this->GetObj().DrawModel();
						DrawParts->SetUseFarShadowDraw(true);
					}
				}
				//hitbox�`��
				if (false) {
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

				SetFogEnable(fog_enable);
				SetFogMode(fog_mode);
				SetFogColor(fog_r, fog_g, fog_b);
				SetFogStartEnd(fog_start, fog_end);
				SetFogDensity(fog_density);
			}
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					this->GetObj().DrawModel();
				}
			}
		};
	};
};
