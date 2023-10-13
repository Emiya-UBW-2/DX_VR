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

		//人間のフレーム、アニメ管理
		class HumanControl {
		private:
			//体のフレーム情報
			class frame_body {
			public:
				//頭
				frames head_f;
				//胴体
				frames bodyg_f;
				frames bodyc_f;
				frames bodyb_f;
				frames body_f;
				//右手座標系
				frames RIGHThand2_f;
				frames RIGHThand_f;
				frames RIGHTarm2_f;
				frames RIGHTarm1_f;
				//左手座標系
				frames LEFThand2_f;
				frames LEFThand_f;
				frames LEFTarm2_f;
				frames LEFTarm1_f;

				//右手座標系
				frames RIGHTreg2_f;
				frames RIGHTreg_f;
				frames RIGHTfoot2_f;
				frames RIGHTfoot1_f;
				//左手座標系
				frames LEFTreg2_f;
				frames LEFTreg_f;
				frames LEFTfoot2_f;
				frames LEFTfoot1_f;
			public:
				//
				void Get_frame(MV1& obj_) noexcept {
					for (int i = 0; i < int(obj_.frame_num()); ++i) {
						std::string p = obj_.frame_name(i);
						if (p == std::string("グルーブ")) {
							this->bodyg_f.Set(i, obj_);
						}
						else if (p == std::string("下半身")) {
							this->bodyc_f.Set(i, obj_);
						}

						else if (p.find("左足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
							this->LEFTfoot1_f.Set(i, obj_);
						}
						else if (p.find("左ひざ") != std::string::npos) {
							this->LEFTfoot2_f.Set(i, obj_);
						}
						else if (p.find("左足首") != std::string::npos && p.find("先") == std::string::npos) {
							this->LEFTreg_f.Set(i, obj_);
						}
						else if (p.find("左足首先") != std::string::npos) {
							this->LEFTreg2_f.Set(i, obj_);
						}

						else if (p.find("右足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
							this->RIGHTfoot1_f.Set(i, obj_);
						}
						else if (p.find("右ひざ") != std::string::npos) {
							this->RIGHTfoot2_f.Set(i, obj_);
						}
						else if (p.find("右足首") != std::string::npos && p.find("先") == std::string::npos) {
							this->RIGHTreg_f.Set(i, obj_);
						}
						else if (p.find("右足首先") != std::string::npos) {
							this->RIGHTreg2_f.Set(i, obj_);
						}
						else if (p.find("上半身") != std::string::npos && p.find("上半身2") == std::string::npos) {
							this->bodyb_f.Set(i, obj_);
						}
						else if (p.find("上半身2") != std::string::npos) {
							this->body_f.Set(i, obj_);
						}
						else if (p.find("頭") != std::string::npos && p.find("先") == std::string::npos) {
							this->head_f.Set(i, obj_);
							//head_hight = obj_.frame(this->head_f.first).y();
						}

						else if (p.find("右腕") != std::string::npos && p.find("捩") == std::string::npos) {
							this->RIGHTarm1_f.Set(i, obj_);
						}
						else if (p.find("右ひじ") != std::string::npos) {
							this->RIGHTarm2_f.Set(i, obj_);
						}
						else if (p == std::string("右手首")) {
							this->RIGHThand_f.Set(i, obj_);
						}
						else if (p == std::string("右手先") || p == std::string("右手首先")) {
							this->RIGHThand2_f.Set(i, obj_);
						}

						else if (p.find("左腕") != std::string::npos && p.find("捩") == std::string::npos) {
							this->LEFTarm1_f.Set(i, obj_);
						}
						else if (p.find("左ひじ") != std::string::npos) {
							this->LEFTarm2_f.Set(i, obj_);
						}
						else if (p == std::string("左手首")) {
							this->LEFThand_f.Set(i, obj_);
						}
						else if (p == std::string("左手先") || p == std::string("左手首先")) {
							this->LEFThand2_f.Set(i, obj_);
						}
					}
				}
				//
				void copy_frame(MV1& mine, frame_body& frame_tgt_, MV1* tgt) noexcept {
					tgt->SetMatrix(mine.GetMatrix());
					//
					tgt->SetFrameLocalMatrix(frame_tgt_.head_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.head_f.GetFrameID()));
					//
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyg_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.bodyg_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyc_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.bodyc_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyb_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.bodyb_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.body_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.body_f.GetFrameID()));
					//右手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHThand_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.GetFrameID()));
					//左手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFThand2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFThand_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.GetFrameID()));
					//右手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.GetFrameID()));
					//左手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTreg_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.GetFrameID()));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.GetFrameID(), mine.GetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.GetFrameID()));
					for (int i = 0; i < tgt->get_anime().size(); ++i) {
						tgt->get_anime(i).per = mine.get_anime(i).per;
						tgt->get_anime(i).time = mine.get_anime(i).time;
					}
				}
				//
			};
		protected:
			//体のフレーム情報
			frame_body lagframe_;							//フレーム
			frame_body frame_s;								//フレーム
		protected:
			void Set_Body(MV1& obj_body_t, MV1& obj_lag_t) noexcept {
				//身体
				this->frame_s.Get_frame(obj_body_t);
				//ラグドール
				this->lagframe_.Get_frame(obj_lag_t);
			}
			void Frame_Copy_Lag(MV1& obj_body_t, MV1* obj_lag_t) { this->frame_s.copy_frame(obj_body_t, this->lagframe_, obj_lag_t); }
		};


		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public LifeControl,
			public KeyControl,
			public ShapeControl,
			public OverrideControl,
			public EffectControl,
			public LaserSightClass,
			public HitBoxControl,
			public HumanControl
		{
		private://キャラパラメーター
			const float											SpeedLimit{ 1.75f };
			const float											UpperTimerLimit = 10.f;
		private:
			CharaTypeID											m_CharaType;
			int													m_AimAnimeSel{ 0 };
			int													m_CheckAnimeSel{ 0 };
			VECTOR_ref											m_PosBuf;
			MATRIX_ref											m_UpperMatrix;
			float												m_LeanRad{ 0.f };
			float												m_LateLeanRad{ 0.f };
			float												m_Speed{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			bool												m_TurnBody{ false };
			bool												m_RunReady{ false };
			bool												m_Running{ false };
			CharaAnimeID										m_BottomAnimSelect;
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			float												m_ReadyTimer{ 0.f };
			std::array<ArmMovePerClass, (int)EnumGunAnimType::Max>	m_Arm;
			bool												m_MagHand{ false };
			ArmMovePerClass										m_MagArm;
			float												m_MoveEyePosTimer{ 0.f };
			VECTOR_ref											m_MoveEyePos;
			bool												m_IsStuckGun{ false };
			float												m_IsStuckLeftHandTimer{ 0.f };
			bool												m_IsStuckLeftHand{ false };
			ArmMovePerClass										m_StuckLeftHand;
			VECTOR_ref											m_StuckLeftHandPos;
			VECTOR_ref											m_StuckLeftHandNormal;
			VECTOR_ref											m_StuckLeftHandPos_R;
			VECTOR_ref											m_StuckLeftHandNormal_R;
			//入力
			bool												m_Press_Shot{ false };
			bool												m_Press_Reload{ false };
			bool												m_Press_Aim{ false };
			bool												m_Press_Check{ false };
			bool												m_Press_Watch{ false };
			//体力
			std::vector<DamageEvent>							m_DamageEvent;									//
			//サウンド
			int													m_CharaSound{ -1 };
			//
			bool												m_SendCamShake{ false };
			//
			VECTOR_ref											m_UpperPrevRad;
			VECTOR_ref											m_UpperRad;
			VECTOR_ref											m_UpperyVecNormal, m_UpperzVecNormal;
			VECTOR_ref											m_UpperyVec, m_UpperzVec, m_UpperPos;
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
			int													m_LeanSoundReq{ 0 };
			float												m_LeanRatePer{ 0.f };
			float												m_SquatPer{ 0.f };
			float												m_RunPer{ 0.f };
			bool												m_SquatSoundReq{ false };
			VECTOR_ref											m_HitAxis{ VECTOR_ref::front() };
			float												m_HitPower{ 0.f };
			float												m_HitPowerR{ 0.f };
			MV1													m_RagDoll;
			float												m_RagDollTimer{ 0.f };						//ラグドールの物理演算フラグ
			//銃
			std::shared_ptr<GunClass>							m_Gun_Ptr{ nullptr };			//銃
		private:
			std::shared_ptr<BackGroundClassBase>		m_BackGround;				//BG
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept {
				m_BackGround.reset();
				this->m_BackGround = backGround;
			}
		public://ゲッター(ラッパー)
			const auto		GetBottomStandAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_WalkBack : CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_LeftStep : CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_RightStep : CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_Turn : CharaAnimeID::Bottom_Stand_Turn; }
			const auto		GetBottomRunAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_Run; }
			const bool		HaveFrame(CharaFrame frame) const noexcept { return this->m_Frames[(int)frame].first != -1; }
			const auto		GetFrame(CharaFrame frame) const noexcept { return m_Frames[(int)frame].first; }
			const auto&		GetFrameBaseLocalMat(CharaFrame frame) const noexcept { return this->m_Frames[(int)frame].second; }
		public://ゲッター
			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->GetObj().get_anime((int)anim); }
			auto&			GetGunPtrNow(void) noexcept { return this->m_Gun_Ptr; }
			auto&			GetDamageEvent(void) noexcept { return this->m_DamageEvent; }
			const auto&		GetGunPtrNow_Const(void) const noexcept { return this->m_Gun_Ptr; }
			const auto		GetReticleRad(void) const noexcept { return this->m_LateLeanRad + (KeyControl::GetRad().y() - this->m_yrad_Bottom)*0.15f; }
			const auto&		GetSendCamShake(void) const noexcept { return this->m_SendCamShake; }
			const auto&		GetSpeed(void) const noexcept { return this->m_Speed; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }

			const auto		GetIsADS(void) const noexcept { return this->m_ReadyTimer == 0.f; }
			const auto		GetIsAim(void) const noexcept { return !(this->m_ReadyTimer == UpperTimerLimit); }
			const auto		GetShotPhase(void) const noexcept { return (GetGunPtrNow_Const()) ? GetGunPtrNow_Const()->GetShotPhase() : GunAnimeID::Base; }

			const auto		GetShootReady(void) const noexcept { return (GetGunPtrNow_Const()) ? GetGunPtrNow_Const()->GetShootReady() : false; }
		private:
			void			SetReady(void) noexcept { this->m_ReadyTimer = UpperTimerLimit; }
			void			SetAim(void) noexcept { this->m_ReadyTimer = 0.1f; }
			void			SetADS(void) noexcept { this->m_ReadyTimer = 0.f; }
		public://セッター
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { GetObj().frame_Reset(GetFrame(frame)); }
			bool			SetDamageEvent(const DamageEvent& value) noexcept {
				if (this->m_MyID == value.ID && this->m_objType == value.CharaType) {
					SubHP(value.Damage, value.rad);
					return true;
				}
				return false;
			}
			void			SetAnimLoop(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimLoop((int)ID, speed); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(GetFrame(frame), value * GetFrameBaseLocalMat(frame)); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { this->m_Shapes[(int)pShape].second = Per; }
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			SetGunPtr(const std::shared_ptr<GunClass>& pGunPtr0) noexcept { this->m_Gun_Ptr = pGunPtr0; }
			void			GunSetUp() noexcept {
				if (GetGunPtrNow() != nullptr) {
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
			const auto		GetAmmoNum(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetAmmoNum() : 0; }
			const auto		GetAmmoAll(void) const noexcept { return (GetGunPtrNow_Const() != nullptr) ? GetGunPtrNow_Const()->GetAmmoAll() : 0; }

			const auto&		GetADSPer(void) const noexcept { return this->m_ADSPer; }
			const auto		GetEyeVecY(void) const noexcept { return GetEyeVecMat().yvec(); }
			const auto		GetEyeVector(void) const noexcept { return GetEyeVecMat().zvec() * -1.f; }
			const auto		GetEyePosition(void) const noexcept {
				auto EyePosition = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f
					+ MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, 0.f, -0.5f), GetEyeVecMat()) + this->m_MoveEyePos;
				if (GetGunPtrNow_Const()) {
					return Lerp(EyePosition, GetGunPtrNow_Const()->GetEyePos() + m_GunShake_r * 0.3f, this->m_ADSPer);
				}
				else {
					return EyePosition;
				}
			}
		public:
			//とりあえず当たったかどうか探す
			const auto		CheckLineHit(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				if (!IsAlive()) { return false; }
				if (!(GetMinLenSegmentToPoint(StartPos, *pEndPos, m_move.pos) <= 2.0f*Scale_Rate)) { return false; }
				return HitBoxControl::CheckLineHited(StartPos, pEndPos);
			}
			const bool		CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept;
			void			move_RightArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			void			move_LeftArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			//
			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos, PlayerID pID) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
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
