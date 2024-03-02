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
#include "Armer.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

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
			public WalkSwingControl,
			public GunSwingControl,
			public StackLeftHandControl,
			public MagStockControl,
			public HitReactionControl,
			public RagDollControl,
			public ULTControl {
		private://キャラパラメーター
			const float											UpperTimerLimit = 10.f;
		private:
			CharaTypeID											m_CharaType;
			VECTOR_ref											m_PosBuf;
			CharaAnimeID										m_BottomAnimSelect;
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{0};
			std::array<ArmMovePerClass, (int)EnumGunAnimType::Max>	m_Arm;
			float												m_ReadyTimer{0.f};
			float												m_LeanRad{0.f};
			float												m_LateLeanRad{0.f};
			float												m_MoverPer{0.f};
			float												m_Speed{0.f};
			float												m_yrad_Upper{0.f}, m_yrad_Bottom{0.f};
			float												m_yrad_UpperChange{0.f}, m_yrad_BottomChange{0.f};
			bool												m_TurnBody{false};
			bool												m_MagHand{false};
			ArmMovePerClass										m_MagArm;
			float												m_MoveEyePosTimer{0.f};
			VECTOR_ref											m_MoveEyePos;
			float												m_StuckGunTimer{0.f};
			bool												m_IsStuckGun{false};
			float												m_ADSPer{0.f};
			float												m_AmmoHandR{0.f};
			float												m_AmmoHand{0.f};
			//銃
			int													m_ReadyAnimPhase{0};
			float												m_ReadyAnim{-1.f};
			int													m_GunSelect{0};
			std::array<std::shared_ptr<GunClass>, 2>			m_Gun_Ptr{nullptr , nullptr};			//銃
			bool												m_IsChanging{false};
			//
			std::shared_ptr<ArmerClass>							m_Armer_Ptr{nullptr};
			ArmMovePerClass										m_Wear_Armer;
			bool												m_CanWearArmer{false};
			bool												m_IsWearArmer{false};
			bool												m_IsWearingArmer{false};
			bool												m_WearArmer{false};

			float												m_ULTUp{0.f};
			bool												m_ULTActive{false};
			float												m_HPRec{0.f};
			//入力
			bool												m_Press_Shot{false};
			bool												m_Press_Reload{false};
			bool												m_Press_Aim{false};
			float												m_MeleeCoolDown{0.f};
			bool												m_ArmBreak{false};
			switchs												m_SightChange;

			float												m_AutoAimTimer{0.f};
			int													m_AutoAim{-1};
			float												m_AutoAimOn{0.f};

			//サウンド
			float												m_SoundPower{0.f};
			int													m_CharaSound{-1};
			int													m_LeanSoundReq{0};
			bool												m_SquatSoundReq{false};
			//
			std::array<ItemFallControl, 2>						m_ItemFallControl;
			VECTOR_ref											m_RecoilRadAdd;
			//
			Pendulum2D											m_SlingZrad;
			Pendulum2D											m_SlingYrad;
			std::array<float, 2>								m_SlingPer;
			std::array<MATRIX_ref, 2>							m_SlingMat;
		public:
			bool												CanLookTarget{true};
		private:
			std::shared_ptr<BackGroundClassBase>				m_BackGround;				//BG
			bool												m_IsMainGame{false};
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround, bool IsMainGame) noexcept {
				m_BackGround.reset();
				m_ItemFallControl.at(0).Dispose();
				m_ItemFallControl.at(1).Dispose();
				this->m_BackGround = backGround;
				this->m_IsMainGame = IsMainGame;
				m_ItemFallControl.at(0).Init(this->m_BackGround, "data/model/AmmoBox/", ItemType::AMMO);
				m_ItemFallControl.at(1).Init(this->m_BackGround, "data/model/Armer/", ItemType::ARMER);
				this->m_Gun_Ptr[0]->SetMapCol(this->m_BackGround);
				if (this->m_Gun_Ptr[1]) {
				this->m_Gun_Ptr[1]->SetMapCol(this->m_BackGround);
				}
			}
		private:
			void			SetReady(void) noexcept { this->m_ReadyTimer = UpperTimerLimit; }
			void			SetAim(void) noexcept { this->m_ReadyTimer = 0.1f; }
			void			SetADS(void) noexcept { this->m_ReadyTimer = 0.f; }
			const auto		GetEyeVecMat(void) const noexcept {
				auto tmpUpperMatrix =
					MATRIX_ref::RotZ(this->m_LeanRad / 5.f) *
					MATRIX_ref::RotX(KeyControl::GetRad().x()) *
					MATRIX_ref::RotY(this->m_yrad_BottomChange);
				return WalkSwingControl::GetWalkSwingMat() * tmpUpperMatrix * this->m_move.mat;
			}
			const auto		GetSpeedPer(void) const noexcept {
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
			void			move_RightArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			void			move_LeftArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
		public://ゲッター(ラッパー)
			const auto		GetBottomStandAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_WalkBack : CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_LeftStep : CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_RightStep : CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_Turn : CharaAnimeID::Bottom_Stand_Turn; }
			const auto		GetBottomRunAnimSel(void) const noexcept { return CharaAnimeID::Bottom_Stand_Run; }
			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->GetObj().get_anime((int)anim); }
			const bool		HaveFrame(CharaFrame frame) const noexcept { return this->m_Frames[(int)frame].first != -1; }
			const auto		GetFrame(CharaFrame frame) const noexcept { return m_Frames[(int)frame].first; }
			const auto&		GetFrameBaseLocalMat(CharaFrame frame) const noexcept { return this->m_Frames[(int)frame].second; }
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return GetFrameLocalMatrix(GetFrame(frame)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return GetFrameWorldMatrix(GetFrame(frame)); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return GetParentFrameWorldMatrix(GetFrame(frame)); }
			const auto		CanWearArmer() const noexcept { return this->m_CanWearArmer && (LifeControl::GetAP() < LifeControl::GetAPMax()); }
			
		public://セッター(ラッパー)
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { GetObj().frame_Reset(GetFrame(frame)); }
			void			SetAnimLoop(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimLoop((int)ID, speed); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(GetFrame(frame), value * GetFrameBaseLocalMat(frame)); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { this->m_Shapes[(int)pShape].second = Per; }
			void			FlipAnime() noexcept {
				//反映
				for (int i = 0; i < GetObj().get_anime().size(); i++) {
					GetAnimeBuf((CharaAnimeID)i) = std::clamp(GetAnimeBuf((CharaAnimeID)i), 0.f, 1.f);
					GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i);
				}
			}
			void			SetArmer(const std::shared_ptr<ArmerClass>& pArmer) noexcept { this->m_Armer_Ptr = pArmer; }
		public://ゲッター
			const auto		GetAutoAimPer(void) const noexcept { return this->m_AutoAimTimer / 1.f; }
			const auto&		GetAutoAimID(void) const noexcept { return this->m_AutoAim; }
			const auto&		GetAutoAimOn(void) const noexcept { return this->m_AutoAimOn; }

			const auto		GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd*(60.f / FPS); }
			auto&			GetSoundPower(void) noexcept { return this->m_SoundPower; }
			auto&			GetGunPtr(int ID) noexcept { return this->m_Gun_Ptr[ID]; }
			auto&			GetGunPtrNow(void) noexcept { return this->m_Gun_Ptr[m_GunSelect]; }
			const auto&		GetGunPtrNow(void) const noexcept { return this->m_Gun_Ptr[m_GunSelect]; }
			const auto		GetGunRadAdd(void) const noexcept { return this->m_LateLeanRad + this->m_yrad_BottomChange*0.15f; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }

			const auto		GetIsADS(void) const noexcept { return this->m_ReadyTimer == 0.f; }
			const auto		GetIsAim(void) const noexcept { return !(this->m_ReadyTimer == UpperTimerLimit); }
			const auto		GetShotPhase(void) const noexcept { return (GetGunPtrNow()) ? GetGunPtrNow()->GetShotPhase() : GunAnimeID::Base; }
			const auto		GetShootReady(void) const noexcept { return (GetGunPtrNow()) ? GetGunPtrNow()->GetShootReady() : false; }
			const auto		GetMeleeSwitch(void) const noexcept { return m_MeleeCoolDown == 1.f; }

			const auto		GetCharaDir(void) const noexcept {
				auto tmpUpperMatrix =
					MATRIX_ref::RotZ(this->m_LeanRad) *
					MATRIX_ref::RotX(KeyControl::GetRad().x()) *
					MATRIX_ref::RotY(KeyControl::GetRun() ? 0.f : this->m_yrad_BottomChange);
				return tmpUpperMatrix * this->m_move.mat;
			}
			const auto&		GetADSPer(void) const noexcept { return this->m_ADSPer; }
			const auto		GetEyeVecY(void) const noexcept { return GetEyeVecMat().yvec(); }
			const auto		GetEyeVector(void) const noexcept { return GetEyeVecMat().zvec() * -1.f; }
			const auto		GetEyePosition(void) const noexcept {
				auto EyePosition = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f
					+ MATRIX_ref::Vtrans(VECTOR_ref::front()*-0.5f, GetEyeVecMat()) + this->m_MoveEyePos;
				if (GetGunPtrNow()) {
					return Lerp(EyePosition, GetGunPtrNow()->GetEyePos(), this->m_ADSPer);
				}
				else {
					return EyePosition;
				}
			}
			const auto		GetIsCheck() const noexcept {
				bool IsCheck = GetGunPtrNow()->GetIsMagFull();
				if (m_GunSelect == 0 && GetGunPtrNow()->GetReloadType() == RELOADTYPE::MAG) {
					IsCheck = (GetGunPtrNow()->GetAmmoNum() >= MagStockControl::GetMag(0));
				}
				return IsCheck;
			}
			const auto		GetAmmoLoading() const noexcept { return GetGunPtrNow()->IsAmmoLoading(); }
			const auto		IsSightActive() const noexcept { return GetGunPtrNow()->IsSightActive(); }
			const auto		IsSightPtrActive() const noexcept { return GetGunPtrNow()->IsSightPtrActive(); }
			const auto		IsAutoAimActive() const noexcept { return GetGunPtrNow()->IsAutoAimActive(); }

			const auto&		GetSightReitcleGraphPtr() const noexcept { return GetGunPtrNow()->GetSightPtr()->GetModData()->GetReitcleGraph(); }
			const auto		GetSightZoomSize() const noexcept {
				if (!IsSightPtrActive()) { return 1.f; }
				return GetGunPtrNow()->GetSightPtr()->GetModData()->GetZoomSize();
			}
		public://セッター
			bool			SetDamageEvent(const DamageEvent& value) noexcept;
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			SetGunPtr(int ID, const std::shared_ptr<GunClass>& pGunPtr0) noexcept { this->m_Gun_Ptr[ID] = pGunPtr0; }
			void			Heal(HitPoint value) noexcept {
				LifeControl::SetHealEvent(this->m_MyID, this->m_MyID, value, 0);
				m_ArmBreak = false;
			}
			bool			GetArmer() noexcept {
				bool prev = this->m_CanWearArmer;
				this->m_CanWearArmer = true;
				return this->m_CanWearArmer != prev;
			}
			const bool		CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept;
			const bool		CheckMeleeHit(PlayerID MeleeID, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept;
		public:
			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos, PlayerID pID, int GunSel) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
		private: //更新関連
			void			ExecuteInput(void) noexcept;			//操作																	//0.01ms
			void			ExecuteUpperMatrix(void) noexcept;		//上半身回転															//0.06ms
			void			ExecuteAnim(void) noexcept;				//SetMat指示															//0.03ms
			void			ExecuteMatrix(void) noexcept;			//SetMat指示更新														//0.03ms
			void			ExecuteShape(void) noexcept;			//顔																	//0.01ms
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
