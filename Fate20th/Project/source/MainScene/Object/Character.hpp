#pragma once
#include	"../../Header.hpp"
#include "../CharaAnimData.hpp"
#include "Character_before.hpp"

namespace FPS_n2 {
	namespace Sceneclass {


		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public LifeControl,
			public KeyControl,
			public ShapeControl,

			public EffectControl
		{
		private://�L�����p�����[�^�[
			const float											SpeedLimit{ 4.5f };
		private:
			class Impl;
		private:
			CharaTypeID											m_CharaType;
			std::vector<CharaAnimeSet>							m_CharaAnimeSet;
			std::vector<WeaponAnimeSet>							m_WeaponAnimeSet;
			int													m_CharaAnimeSel{ 0 };
			int													m_ReadyAnimeSel{ 0 };
			int													m_SlashAnimeSel{ 0 };
			//
			float												m_RunSpeed{ 1.f };
			//
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			VECTOR_ref											m_PosBuf;
			bool												m_PosBufOverRideFlag{ false };
			VECTOR_ref											m_PosBufOverRide;
			VECTOR_ref											m_VecBufOverRide;
			VECTOR_ref											m_RadOverRide;									//

			MATRIX_ref											m_UpperMatrix;
			float												m_Speed{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_yrad_Rotate{ 0.f };
			float												m_yrad_RotateR{ 0.f };
			float												m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			float												m_NeckPer{ 0.f };
			bool												m_TurnBody{ false };
			bool												m_RunReady{ false };
			bool												m_Running{ false };
			CharaAnimeID										m_UpperAnimSelect, m_PrevUpperAnimSel;
			CharaAnimeID										m_BottomAnimSelect;
			//����
			bool												m_Press_Shot{ false };
			//�̗�
			std::vector<HitBox>									m_HitBox;
			DamageEvent											m_DamageEvent;									//
			unsigned long long									m_DamageSwitch{ 0 };							//
			unsigned long long									m_DamageSwitchRec{ 0 };							//
			//
			float												m_LeftHandPer{ 0.f };
			//�T�E���h
			int													m_CharaSound{ -1 };
			std::array<std::shared_ptr<WeaponClass>,1>			m_Weapon_Ptr{ nullptr };			//�e
			int													m_WeaponSelect{ 0 };
			//
			bool												m_SendCamShake{ false };
			//
			VECTOR_ref											m_UpperPrevRad;
			VECTOR_ref											m_UpperRad;
			VECTOR_ref											m_UpperyVecNormal, m_UpperzVecNormal;
			VECTOR_ref											m_UpperyVec, m_UpperzVec, m_UpperPos;
			float												m_UpperAnim{ 0.f };

			float												m_Fov = 0.f;

			bool												m_Slash{ false };
			float												m_SlashPer{ 0.f };

			VECTOR_ref											m_PrevPos;

			VECTOR_ref											m_Accel;

			VECTOR_ref											m_CamEyeVec;
			VECTOR_ref											m_EyeVecR;

			VECTOR_ref											m_MoveVecRec;
			float												m_BodyMoveSpd{ 0.f };
			float												m_SkirtSpd{ 0.f };

			float												m_Boost{ 0.f };
			bool												m_BoostSwitch{ false };
			bool												m_BoostEnd{ false };
		public://�Q�b�^�[
			//const auto		GetShotAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_ADS; }
			//const auto		GetTurnRatePer(void) const noexcept { return this->m_InputGround.GetTurnRatePer(); }
			//const auto		GetReadyPer(void) const noexcept { return this->m_ReadyPer; }
			//const auto&		GetPosBuf(void) const noexcept { return this->m_PosBuf; }
			//void				SetAnimOnce(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimOnce((int)ID, speed); }
			//const auto		GetParentFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix((int)this->GetObj_const().frame_parent(GetCharaFrame(frame))); }
		public://�Q�b�^�[(���b�p�[)
			const auto		GetBottomStandAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_Turn; }
			const auto		GetBottomRunAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_Run; }
			const auto		GetCharaFrame(CharaFrame frame) const noexcept { return m_Frames[(int)frame].first; }
		public://�Q�b�^�[
			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->GetObj().get_anime((int)anim); }
			auto&			GetWeaponPtr(int ID) noexcept { return this->m_Weapon_Ptr[ID]; }
			auto&			GetWeaponPtrNow(void) noexcept { return this->m_Weapon_Ptr[this->m_WeaponSelect]; }
			const auto&		GetWeaponPtrNow_Const(void) const noexcept { return this->m_Weapon_Ptr[this->m_WeaponSelect]; }
			const auto&		GetWeaponPtrNowID() const noexcept { return this->m_WeaponSelect; }
			const auto&		GetReticleRad(void) const noexcept { return (KeyControl::GetRad().y() - this->m_yrad_Bottom)*0.15f; }
			const auto&		GetSendCamShake(void) const noexcept { return this->m_SendCamShake; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto&		GetDamageSwitch(void) const noexcept { return this->m_DamageSwitch; }
			const auto&		GetDamageSwitchRec(void) const noexcept { return this->m_DamageSwitchRec; }
			const auto&		GetSpeed(void) const noexcept { return this->m_Speed; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetSlash(void) const noexcept { return this->m_Slash; }
			const auto&		GetSlashPer(void) const noexcept { return this->m_SlashPer; }
		public://�Z�b�^�[
			void			SetCamEyeVec(const VECTOR_ref& value) noexcept { this->m_CamEyeVec = value; }
			void			SetDamageSwitchRec(unsigned long long value) noexcept { this->m_DamageSwitchRec = value; }
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { GetObj().frame_Reset(GetCharaFrame(frame)); }
			void			LoadReticle(void) noexcept {
				for (auto& p : this->m_Weapon_Ptr) {
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
			void			SetWeaponPtr(std::shared_ptr<WeaponClass>& pWeaponPtr0) noexcept {
				this->m_Weapon_Ptr[0] = pWeaponPtr0;
				for (auto& p : this->m_Weapon_Ptr) {
					p->SetPlayerID(this->m_MyID);
				}
				this->m_WeaponSelect = 0;
			}
		public://�Q�b�^�[
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix(GetCharaFrame(frame)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix(GetCharaFrame(frame)); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix((int)this->GetObj_const().frame_parent(GetCharaFrame(frame))); }
			const auto		GetWeaponPtrNum() const noexcept { return this->m_Weapon_Ptr.size(); }
			const auto		GetCharaDir(void) const noexcept { return this->m_UpperMatrix * this->m_move.mat; }
			const auto		GetCharaVecX(void) const noexcept { return GetCharaDir().xvec(); }
			const auto		GetCharaVecY(void) const noexcept { return GetCharaDir().yvec(); }
			const auto		GetCharaVector(void) const noexcept { return GetCharaDir().zvec() * -1.f; }

			const auto		GetDownAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Down; }
			const auto		GetAimAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Ready; }
			const auto		GetCockingAnimSel(void) const noexcept { return this->m_CharaAnimeSet[this->m_CharaAnimeSel].m_Cocking; }

			const auto		GetRunWeaponAnimSel(void) const noexcept { return this->m_WeaponAnimeSet[this->m_CharaAnimeSel].m_Run; }
			const auto&		GetReadyWeaponAnimSel(void) const noexcept { return this->m_WeaponAnimeSet[this->m_CharaAnimeSel].m_Ready; }
			const auto&		GetAimWeaponAnimSel(void) const noexcept { return this->m_WeaponAnimeSet[this->m_CharaAnimeSel].m_Aim; }

			const auto		GetEyeVecMat(void) const noexcept {
				auto tmpUpperMatrix =
					MATRIX_ref::RotX(KeyControl::GetRad().x()) *
					MATRIX_ref::RotY(KeyControl::GetRad().y() - this->m_yrad_Bottom);
				return tmpUpperMatrix * this->m_move.mat;
			}
			const auto		GetEyeVecX(void) const noexcept { return GetEyeVecMat().xvec(); }
			const auto		GetEyeVecY(void) const noexcept { return GetEyeVecMat().yvec(); }
			const auto		GetEyeVector(void) const noexcept { return GetEyeVecMat().zvec() * -1.f; }
			const auto		GetEyePosition(void) const noexcept {
				return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f
					+ this->GetEyeVecY() * 0.f
					+ this->GetEyeVector() * 0.5f;
			}
		private:
			//��e�`�F�b�N
			const auto		CheckLineHited(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				bool is_Hit = false;
				for (auto& h : this->m_HitBox) {
					is_Hit |= h.Colcheck(StartPos, pEndPos);
				}
				return is_Hit;
			}
		public:
			const auto&		GetFov(void) const noexcept { return this->m_Fov; }
			const auto		CalcFov() noexcept {
				float fov = 0;
				if (this->GetIsRun()) {
					fov = deg2rad(70);
				}
				else {
					fov = deg2rad(60);
				}
				if (m_Slash) {
					fov += deg2rad(10);
				}
				Easing(&this->m_Fov, fov, 0.9f, EasingType::OutExpo);
			}
			const auto		CheckLineHit(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				if (GetMinLenSegmentToPoint(StartPos, *pEndPos, m_move.pos) <= 2.0f*Scale_Rate) {
					if (this->CheckLineHited(StartPos, pEndPos)) {									//�Ƃ肠���������������ǂ����T��
						return true;
					}
				}
				return false;
			}
			void			move_RightArm(const VECTOR_ref& WeaponPos, const VECTOR_ref& Weaponyvec, const VECTOR_ref& Weaponzvec, const VECTOR_ref& Armyvec) noexcept;
			void			move_LeftArm(const VECTOR_ref& WeaponPos, const VECTOR_ref& Weaponyvec, const VECTOR_ref& Weaponzvec, const VECTOR_ref& Armyvec) noexcept;
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
			void			Init(void) noexcept override;
			void			FirstExecute(void) noexcept override;
			void			Draw(void) noexcept override;
			void			DrawShadow(void) noexcept override;
			//
			void			Dispose(void) noexcept override;
		};
	};
};
