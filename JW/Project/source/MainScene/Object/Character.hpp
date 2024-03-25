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
#include "Morphine.hpp"


namespace FPS_n2 {
	namespace Sceneclass {

		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public LifeControl,
			public KeyControl,
			public OverrideControl,
			public EffectControl,
			public LaserSightClass,
			public HitBoxControl,
			public WalkSwingControl,
			public EyeSwingControl,
			public GunSwingControl,
			public StackLeftHandControl,
			public MagStockControl,
			public HitReactionControl,
			public RagDollControl,
			public ULTControl,
			public CharaTypeControl,
			public GunReadyControl {
		private:
			CharaAnimeID										m_BottomAnimSelect;
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{0};
			std::array<ArmMovePerClass, (int)EnumGunAnimType::Max>	m_Arm;
			float												m_LeanRad{0.f};
			float												m_LateLeanRad{0.f};
			float												m_yrad_Upper{0.f}, m_yrad_Bottom{0.f};
			float												m_yrad_UpperChange{0.f}, m_yrad_BottomChange{0.f};
			bool												m_TurnBody{false};
			bool												m_MagHand{false};
			ArmMovePerClass										m_MagArm;
			float												m_StuckGunTimer{0.f};
			bool												m_IsStuckGun{false};
			float												m_AmmoHandR{0.f};
			float												m_AmmoHand{0.f};
			//銃
			int													m_ReadyAnimPhase{0};
			float												m_ReadyAnim{-1.f};
			int													m_GunSelect{0};
			std::array<std::shared_ptr<GunClass>, 2>			m_Gun_Ptr{nullptr , nullptr};			//銃
			bool												m_IsChanging{false};
			bool												m_HeadShotSwitch{false};
			float												m_HeadShotPer{0.f};
			//
			std::shared_ptr<ArmerClass>							m_Armer_Ptr{nullptr};
			ArmMovePerClass										m_Wear_Armer;
			bool												m_CanWearArmer{false};
			bool												m_IsWearArmer{false};
			bool												m_WearArmerSwitch{false};
			bool												m_IsWearingArmer{false};
			bool												m_WearArmer{false};

			std::shared_ptr<MorphineClass>						m_Morphine_Ptr{nullptr};
			ArmMovePerClass										m_Wear_Morphine;
			float												m_Wear_MorphineFrame{0.f};
			float												m_Wear_MorphinePer{0.f};
			bool												m_CanWearMorphine{false};
			bool												m_IsWearMorphine{false};
			bool												m_MorphineSwitch{false};
			bool												m_IsWearingMorphine{false};
			bool												m_WearMorphine{false};

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

			std::shared_ptr<BackGroundClassBase>				m_BackGround;				//BG
			bool												m_IsMainGame{false};
		public:
			bool												CanLookTarget{true};
		private:
			const MATRIX_ref GetEyeVecMat(void) const noexcept;
			const float		GetSpeedPer(void) const noexcept;
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
			const auto		CanWearMorphine() const noexcept { return this->m_CanWearMorphine && (LifeControl::GetHP() < LifeControl::GetHPMax()); }
			const auto		GetWearArmerSwitch() const noexcept { return this->m_WearArmerSwitch; }
			const auto		GetMorphineSwitch() const noexcept { return this->m_MorphineSwitch; }
			const auto		IsULTSelect() const noexcept { return this->m_GunSelect == 1; }

			const auto		PopHeadShotSwitch() noexcept {
				auto ret = this->m_HeadShotSwitch;
				this->m_HeadShotSwitch = false;
				return ret;
			}

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
			void			SetMorphine(const std::shared_ptr<MorphineClass>& pMorphine) noexcept { this->m_Morphine_Ptr = pMorphine; }
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
			const auto		GetShotPhase(void) const noexcept { return (GetGunPtrNow()) ? GetGunPtrNow()->GetShotPhase() : GunAnimeID::Base; }
			const auto		GetShootReady(void) const noexcept { return (GetGunPtrNow()) ? GetGunPtrNow()->GetShootReady() : false; }
			const auto		GetMeleeSwitch(void) const noexcept { return m_MeleeCoolDown == 1.f; }
			const auto		GetCharaPosition(void) const noexcept { return this->m_move.pos; }
			const auto		GetCharaDir(void) const noexcept {
				auto tmpUpperMatrix =
					MATRIX_ref::RotZ(this->m_LeanRad) *
					MATRIX_ref::RotX(KeyControl::GetRad().x()) *
					MATRIX_ref::RotY(KeyControl::GetRun() ? 0.f : this->m_yrad_BottomChange);
				return tmpUpperMatrix * this->m_move.mat;
			}
			const auto		GetEyeVecY(void) const noexcept { return GetEyeVecMat().yvec(); }
			const auto		GetEyeVector(void) const noexcept { return GetEyeVecMat().zvec() * -1.f; }
			const auto		GetEyePosition(void) const noexcept {
				auto EyePosition = (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f
					+ MATRIX_ref::Vtrans(VECTOR_ref::front()*-0.5f, GetEyeVecMat()) + EyeSwingControl::GetEyeSwingPos();
				if (GetGunPtrNow()) {
					return Lerp(EyePosition, GetGunPtrNow()->GetEyePos(), GunReadyControl::GetADSPer());
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
			void			SetGunPtr(int ID, const std::shared_ptr<GunClass>& pGunPtr0) noexcept { this->m_Gun_Ptr[ID] = pGunPtr0; }
			void			Heal(HitPoint value, bool SwitchOn) noexcept {
				LifeControl::SetHealEvent(this->m_MyID, this->m_MyID, value, 0);
				if (SwitchOn) {
					m_ArmBreak = false;
				}
			}
			bool			GetArmer() noexcept {
				bool prev = this->m_CanWearArmer;
				this->m_CanWearArmer = true;
				return this->m_CanWearArmer != prev;
			}
			bool			GetMorphine() noexcept {
				bool prev = this->m_CanWearMorphine;
				this->m_CanWearMorphine = true;
				return this->m_CanWearMorphine != prev;
			}

			const bool		CheckHit(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept;
			const bool		CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept;
			const bool		CheckMeleeHit(PlayerID MeleeID, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept;
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround, bool IsMainGame) noexcept;
			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos, PlayerID pID, int GunSel) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteUpperMatrix(void) noexcept;
			void			ExecuteAnim(void) noexcept;
			void			ExecuteMatrix(void) noexcept;
			void			ExecuteShape(void) noexcept {
				//SetShapePer(CharaShape::None, 0.f);
			}
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
