#pragma once
#include	"../../Header.hpp"
#include "CharacterEnum.hpp"
#include "Character_before.hpp"
#include "CharaAnimData.hpp"


#include "../../sub.hpp"
#include "ObjectBase.hpp"
#include "AmmoData.hpp"
#include "Ammo.hpp"
#include "Gun.hpp"

namespace FPS_n2 {
	namespace Sceneclass {


		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public LifeControl,
			public KeyControl,
			public ShapeControl,
			public OverrideControl,
			public EffectControl
		{
		private://キャラパラメーター
			const float											SpeedLimit{ 1.75f };
			const float											UpperTimerLimit = 10.f;
		private:
			CharaTypeID											m_CharaType;
			std::vector<std::array<int, (int)CharaGunAnimeID::Max>>							m_CharaAnimeSet;
			std::vector<std::array<std::vector<EnumGunAnim>, (int)EnumGunAnimType::Max>>							m_GunAnimeSet;

			int													m_CharaAnimeSel{ 0 };
			int													m_ReadyAnimeSel{ 0 };
			int													m_AimAnimeSel{ 0 };
			int													m_CheckAnimeSel{ 0 };
			//
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			VECTOR_ref											m_PosBuf;

			MATRIX_ref											m_UpperMatrix;
			float												m_LeanRad{ 0.f };
			float												m_LateLeanRad{ 0.f };
			float												m_Speed{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			float												m_NeckPer{ 0.f };
			bool												m_TurnBody{ false };
			bool												m_RunReady{ false };
			bool												m_Running{ false };
			CharaGunAnimeID										m_GunAnimSelect;
			CharaAnimeID										m_BottomAnimSelect;
			std::array<float, (int)CharaGunAnimeID::Max>		m_GunAnimFrame;
			float			GetAllTime(CharaGunAnimeID ID) { return (float)m_CharaAnimeSet.at(this->m_CharaAnimeSel).at((int)ID); }
			void			UpdateGunAnim(CharaGunAnimeID ID, float speed) { m_GunAnimFrame.at((int)ID) += 30.f / FPS * speed; }
			bool			GetGunAnimZero(CharaGunAnimeID ID) { return m_GunAnimFrame.at((int)ID) <= 0.f; }
			bool			GetGunAnimEnd(CharaGunAnimeID ID) { return m_GunAnimFrame.at((int)ID) >= GetAllTime(ID); }
			float			GetTimePer(CharaGunAnimeID ID) { return (GetAllTime(ID) > 0.f) ? (m_GunAnimFrame.at((int)ID) / GetAllTime(ID)) : 1.f; }

			//銃
			float												m_ReadyTimer{ 0.f };

			std::array<ArmMovePerClass, (int)EnumGunAnimType::Max>	m_Arm;

			float												m_ReloadEyePer{ 0.f };
			float												m_CheckEyePer{ 0.f };

			bool												m_MagHand{ false };
			ArmMovePerClass										m_MagArm;
			float												m_MoveEyePosTimer{ 0.f };
			VECTOR_ref											m_MoveEyePos;
			bool												m_IsStuckGun{ false };
			//入力
			bool												m_Press_Shot{ false };
			bool												m_Press_Reload{ false };
			bool												m_Press_Aim{ false };
			bool												m_Press_Check{ false };
			bool												m_Press_Watch{ false };
			//体力
			std::vector<HitBox>									m_HitBox;
			DamageEvent											m_DamageEvent;									//
			unsigned long long									m_DamageSwitch{ 0 };							//
			unsigned long long									m_DamageSwitchRec{ 0 };							//
			//
			VECTOR_ref											m_RecoilRadAdd;
			//サウンド
			int													m_CharaSound{ -1 };
			//
			std::shared_ptr<GunClass>							m_Gun_Ptr{ nullptr };			//銃
			//
			bool												m_SendCamShake{ false };
			//
			VECTOR_ref											m_UpperPrevRad;
			VECTOR_ref											m_UpperRad;
			VECTOR_ref											m_UpperyVecNormal, m_UpperzVecNormal;
			VECTOR_ref											m_UpperyVec, m_UpperzVec, m_UpperPos;
			float												m_UpperAnim{ 0.f };
			//
			switchs												m_LaserSwitch;
			VECTOR_ref											LaserStartPos;
			VECTOR_ref											LaserEndPos;
			VECTOR_ref											LaserEndPos2D;
			//
			float												m_ADSPer = 0.f;
			//
			VECTOR_ref											m_PrevPos;
			VECTOR_ref											m_WalkSwingRad;
			VECTOR_ref											m_WalkSwing;
			VECTOR_ref											m_WalkSwing_p;
			VECTOR_ref											m_WalkSwing_t;

			float												m_GunShakePer{ 0.f };
			VECTOR_ref											m_GunShake;
			VECTOR_ref											m_GunShake_r;

			float												m_AmmoHandR{ 0.f };
			float												m_AmmoHand{ 0.f };


			VECTOR_ref											m_EyeVecR, m_CamEyeVec;

			int													m_LeanSoundReq{ 0 };
			float												m_LeanRatePer{ 0.f };
			float												m_SquatPer{ 0.f };
			float												m_RunPer{ 0.f };

			bool												m_SquatSoundReq{ false };


			VECTOR_ref											m_HitAxis{ VECTOR_ref::front() };
			float												m_HitPower{ 0.f };
			float												m_HitPowerR{ 0.f };
		private:
			std::shared_ptr<BackGroundClassBase>		m_BackGround;				//BG
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept { this->m_BackGround = backGround; }
		public://ゲッター(ラッパー)
			const auto		GetBottomStandAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_WalkBack : CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_LeftStep : CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_RightStep : CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_Turn : CharaAnimeID::Bottom_Stand_Turn; }
			const auto		GetBottomRunAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_Run; }
			const auto		GetFrame(CharaFrame frame) const noexcept { return m_Frames[(int)frame].first; }
		public://ゲッター
			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->GetObj().get_anime((int)anim); }
			auto&			GetGunPtrNow(void) noexcept { return this->m_Gun_Ptr; }
			const auto&		GetGunPtrNow_Const(void) const noexcept { return this->m_Gun_Ptr; }
			const auto		GetReticleRad(void) const noexcept { return this->m_LateLeanRad + (KeyControl::GetRad().y() - this->m_yrad_Bottom)*0.15f; }
			const auto&		GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd; }
			const auto&		GetSendCamShake(void) const noexcept { return this->m_SendCamShake; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto&		GetDamageSwitch(void) const noexcept { return this->m_DamageSwitch; }
			const auto&		GetDamageSwitchRec(void) const noexcept { return this->m_DamageSwitchRec; }
			const auto&		GetSpeed(void) const noexcept { return this->m_Speed; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }

			const auto		GetLaserActive(void) const noexcept { return this->m_LaserSwitch.on(); }
			const auto		GetIsADS(void) const noexcept { return this->m_ReadyTimer == 0.f; }
			const auto		GetIsAim(void) const noexcept { return !(this->m_ReadyTimer == UpperTimerLimit); }
		private:
			void			SetReady(void) noexcept { this->m_ReadyTimer = UpperTimerLimit; }
			void			SetAim(void) noexcept { this->m_ReadyTimer = 0.1f; }
			void			SetADS(void) noexcept { this->m_ReadyTimer = 0.f; }
		public://セッター
			void			SetCamEyeVec(const VECTOR_ref& value) noexcept { this->m_CamEyeVec = value; }
			void			SetDamageSwitchRec(unsigned long long value) noexcept { this->m_DamageSwitchRec = value; }
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { GetObj().frame_Reset(GetFrame(frame)); }
			bool			SetDamageEvent(const DamageEvent& value) noexcept {
				if (this->m_MyID == value.ID && this->m_objType == value.CharaType) {
					SubHP(value.Damage, value.rad);
					return true;
				}
				return false;
			}
			void			SetAnimLoop(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimLoop((int)ID, speed); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(GetFrame(frame), value * this->m_Frames[(int)frame].second); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { this->m_Shapes[(int)pShape].second = Per; }
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			SetGunPtr(const std::shared_ptr<GunClass>& pGunPtr0) noexcept { this->m_Gun_Ptr = pGunPtr0; }
			void			GunSetUp() noexcept {
				if (GetGunPtrNow() != nullptr) {
					this->m_CharaAnimeSel = GetGunPtrNow()->GetHumanAnimType();
					GetGunPtrNow()->SetPlayerID(this->m_MyID);
				}
			}
		private:
			const auto		GetEyeVecMat(void) const noexcept {
				auto tmpSwingMatrix =
					MATRIX_ref::RotZ(deg2rad(m_WalkSwing.z()*m_WalkSwingRad.z()))*
					MATRIX_ref::RotX(deg2rad(m_WalkSwing.x()*m_WalkSwingRad.x()));
				auto tmpUpperMatrix =
					//MATRIX_ref::RotZ(this->m_LeanRad) *
					MATRIX_ref::RotX(KeyControl::GetRad().x()) *
					MATRIX_ref::RotY(KeyControl::GetRad().y() - this->m_yrad_Bottom);
				return tmpSwingMatrix * tmpUpperMatrix * this->m_move.mat;
			}
		public://ゲッター
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return GetFrameLocalMatrix(GetFrame(frame)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return GetFrameWorldMatrix(GetFrame(frame)); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return GetParentFrameWorldMatrix(GetFrame(frame)); }
			const auto		GetCharaDir(void) const noexcept { return this->m_UpperMatrix * this->m_move.mat; }
			const auto		GetCharaVecX(void) const noexcept { return GetCharaDir().xvec(); }
			const auto		GetCharaVecY(void) const noexcept { return GetCharaDir().yvec(); }
			const auto		GetCharaVector(void) const noexcept { return GetCharaDir().zvec() * -1.f; }
			const auto		GetCanshot(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetCanShot() : false; }
			const auto		GetAmmoNum(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetAmmoNum() : 0; }
			const auto		GetAmmoAll(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetAmmoAll() : 0; }
			const auto		GetShoting(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetShoting() : false; }
			const auto&		GetADSPer(void) const noexcept { return this->m_ADSPer; }
			const auto		GetEyeVecY(void) const noexcept { return GetEyeVecMat().yvec(); }
			const auto		GetEyeVector(void) const noexcept { return GetEyeVecMat().zvec() * -1.f; }
			const auto		GetEyePosition(void) const noexcept {
				auto EyePosition = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f
					+ MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f,0.f,-0.5f), GetEyeVecMat()) + this->m_MoveEyePos;
				if (GetGunPtrNow_Const()) {
					return Lerp(EyePosition, GetGunPtrNow_Const()->GetEyePos() + m_GunShake_r * 0.3f, this->m_ADSPer);
				}
				else {
					return EyePosition;
				}
			}
		private:
			//被弾チェック
			const auto		CheckLineHited(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				bool is_Hit = false;
				for (auto& h : this->m_HitBox) {
					is_Hit |= h.Colcheck(StartPos, pEndPos);
				}
				return is_Hit;
			}
		public:
			const auto		CheckLineHit(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				if (GetMinLenSegmentToPoint(StartPos, *pEndPos, m_move.pos) <= 2.0f*Scale_Rate) {
					if (this->CheckLineHited(StartPos, pEndPos)) {									//とりあえず当たったかどうか探す
						return true;
					}
				}
				return false;
			}
			const auto		CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos, const VECTOR_ref& pShooterPos) noexcept {
				if (!CheckLineHit(StartPos, pEndPos)) { return false; }
				//被弾処理
				for (auto& h : this->m_HitBox) {
					if (h.Colcheck(StartPos, pEndPos)) {
						h.GetColType();
						pAmmo->Penetrate();	//貫通
						//ダメージ計算
						{
							auto v1 = GetCharaVector();
							auto v2 = (pShooterPos - this->m_move.pos).Norm(); v2.y(0);
							this->m_DamageEvent.SetEvent(this->m_MyID, m_objType, pAmmo->GetDamage(), atan2f(v1.cross(v2).y(), v1.dot(v2)));
							++this->m_DamageSwitch;// %= 255;//
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
							if (h.GetColType() == HitType::Leg) {
								KeyControl::SetIsSquat(true);
							}
						}
						//todo : ヒットした部分に近い頂点を赤くする
						return true;
					}
				}
				return false;
			}
			void			move_RightArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			void			move_LeftArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			void			HitDamage(const VECTOR_ref& ShooterPos, HitPoint Damage) {
				//ダメージ計算
				auto v1 = GetCharaVector();
				auto v2 = (ShooterPos - this->m_move.pos).Norm(); v2.y(0);
				this->m_DamageEvent.SetEvent(this->m_MyID, m_objType, Damage, atan2f(v1.cross(v2).y(), v1.dot(v2)));
				++this->m_DamageSwitch;// %= 255;//
			}
			//
			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos, PlayerID pID) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
			void			SetEyeVec() noexcept;
		public://レーザーサイト
			const auto&		GetAimPoint(void) const noexcept { return this->LaserEndPos2D; }
			void			SetLaser2D(const VECTOR_ref& value) noexcept {
				this->LaserEndPos2D = value;
				if (!(0.f < this->LaserEndPos2D.z() && this->LaserEndPos2D.z() < 1.f)) {
					auto* DrawParts = DXDraw::Instance();
					this->LaserEndPos2D.Set((float)DrawParts->m_DispXSize / 2, (float)DrawParts->m_DispYSize / 2, -1.f);
				}
			}

			const auto		GetLaserStartPos(void) const noexcept { return this->GetGunPtrNow_Const()->GetMuzzleMatrix().pos() + this->GetGunPtrNow_Const()->GetMuzzleMatrix().yvec()*-0.05f*Scale_Rate; }
			void			SetLaserPos(const VECTOR_ref& value) noexcept { LaserEndPos = value; }

			void			DrawLaser() noexcept {
				SetUseLighting(FALSE);
				DrawSphere_3D(LaserEndPos, 0.01f*Scale_Rate, GetColor(255, 24, 24), GetColor(0, 0, 0));
				DrawCapsule_3D(GetLaserStartPos(), LaserEndPos, 0.0015f*Scale_Rate, GetColor(255, 24, 24), GetColor(0, 0, 0));
				SetUseLighting(TRUE);
			}
		private: //更新関連
			void			ExecuteSavePrev(void) noexcept;			//以前の状態保持														//
			void			ExecuteInput(void) noexcept;			//操作																	//0.01ms
			void			ExecuteUpperMatrix(void) noexcept;		//上半身回転															//0.06ms
			void			ExecuteAnim(void) noexcept;				//SetMat指示															//0.03ms
			void			ExecuteMatrix(void) noexcept;			//SetMat指示更新														//0.03ms
			void			ExecuteShape(void) noexcept;			//顔																	//0.01ms
			void			ExecuteHeartRate(void) noexcept;		//心拍数																//0.00ms
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				this->m_IsDraw = true;
			}
			~CharacterClass(void) noexcept {}
		public: //継承
			//
			void			Init(void) noexcept override;
			void			FirstExecute(void) noexcept override;
			void			CheckDraw(void) noexcept override {
				this->m_IsDraw = false;
				this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetCameraPosition()).size();
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-2.5f*Scale_Rate, -0.f*Scale_Rate, -2.5f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(2.5f*Scale_Rate, 2.5f*Scale_Rate, 2.5f*Scale_Rate)).get()) == FALSE
					) {
					this->m_IsDraw |= true;
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			DrawShadow(void) noexcept override;
			//
			void			Dispose(void) noexcept override;
		};
	};
};
