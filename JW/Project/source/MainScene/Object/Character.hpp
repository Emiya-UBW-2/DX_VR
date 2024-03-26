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
			public GunReadyControl,
			public GunPtrControl,
			public AutoAimControl,
			public ArmerPtrControl,
			public MorphinePtrControl
		{
		private:
			CharaAnimeID										m_BottomAnimSelect;
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{0};
			std::array<ArmMovePerClass, (int)EnumGunAnimType::Max>	m_Arm;
			float												m_LeanRad{0.f};
			float												m_LateLeanRad{0.f};
			float												m_yrad_Upper{0.f}, m_yrad_Bottom{0.f};
			float												m_yrad_UpperChange{0.f}, m_yrad_BottomChange{0.f}, m_yrad_BottomChangeRun{0.f};
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
			bool												m_IsChanging{false};
			bool												m_HeadShotSwitch{false};
			float												m_HeadShotPer{0.f};
			//
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

			//サウンド
			float												m_SoundPower{0.f};
			int													m_CharaSound{-1};
			//
			std::array<ItemFallControl, 2>						m_ItemFallControl;
			VECTOR_ref											m_RecoilRadAdd;
			//
			Pendulum2D											m_SlingZrad;
			std::array<float, 2>								m_SlingPer;
			std::array<MATRIX_ref, 2>							m_SlingMat;

			std::shared_ptr<BackGroundClassBase>				m_BackGround;				//BG
			bool												m_IsMainGame{false};

			int													m_ArmerStock{0};
			int													m_MorphineStock{0};
		public:
			bool												CanLookTarget{true};

			bool			GetArmer() noexcept {
				auto prev = this->m_ArmerStock;
				this->m_ArmerStock = 1;
				return this->m_ArmerStock != prev;
			}
		private:
			const float		GetSpeedPer(void) const noexcept;
			const auto		GetIsCheck() const noexcept {
				bool IsCheck = GetGunPtrNow()->GetIsMagFull();
				if (IsGun0Select() && GetGunPtrNow()->GetReloadType() == RELOADTYPE::MAG) {
					IsCheck = (GetGunPtrNow()->GetAmmoNum() >= MagStockControl::GetMag(0));
				}
				return IsCheck;
			}
			void			move_RightArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			void			move_LeftArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			const auto		GetCharaPosition(void) const noexcept { return this->m_move.pos; }

			const auto		IsAimPer(void) const noexcept { return (this->m_Arm[(int)EnumGunAnimType::Ready].Per() <= 0.1f); }
			const auto		IsLowReadyPer(void) const noexcept { return (this->m_Arm[(int)EnumGunAnimType::Ready].Per() >= 0.95f); }


			const auto		GetBottomStandAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_WalkBack : CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_LeftStep : CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_RightStep : CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return KeyControl::GetIsSquat() ? CharaAnimeID::Bottom_Squat_Turn : CharaAnimeID::Bottom_Stand_Turn; }
		public://ゲッター
			const MATRIX_ref GetEyeMatrix(void) const noexcept;
			auto&			GetSoundPower(void) noexcept { return this->m_SoundPower; }
			const auto		GetMeleeSwitch(void) const noexcept { return m_MeleeCoolDown == 1.f; }
			const auto		GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd*(60.f / FPS); }
			const auto		GetCharaDir(void) const noexcept {
				auto tmpUpperMatrix =
					MATRIX_ref::RotZ(this->m_LeanRad) *
					MATRIX_ref::RotX(KeyControl::GetRad().x()) *
					MATRIX_ref::RotY(this->m_yrad_BottomChangeRun);
				return tmpUpperMatrix * this->m_move.mat;
			}
			const auto		GetGunRadAdd(void) const noexcept { return this->m_LateLeanRad + this->m_yrad_BottomChange*0.15f; }
			const auto		PopHeadShotSwitch() noexcept {
				auto ret = this->m_HeadShotSwitch;
				this->m_HeadShotSwitch = false;
				return ret;
			}
		public://セッター
			bool			SetDamageEvent(const DamageEvent& value) noexcept;
			void			Heal(HitPoint value, bool SwitchOn) noexcept {
				LifeControl::SetHealEvent(this->m_MyID, this->m_MyID, value, 0);
				if (SwitchOn) {
					m_ArmBreak = false;
				}
			}
			const bool		CheckDamageRay(HitPoint* Damage, ArmerPoint* ArmerDamage, bool CheckBodyParts, PlayerID AttackID, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept;
			void			FlipAnime() noexcept {
				for (int i = 0; i < GetObj().get_anime().size(); i++) {
					this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i], 0.f, 1.f);
					this->GetObj().get_anime(i).per = this->m_AnimPerBuf[i];
				}
				GetObj().work_anime();
			}
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteUpperMatrix(void) noexcept;
			void			ExecuteAnim(void) noexcept;
			void			ExecuteSound(void) noexcept;
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
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround, bool IsMainGame) noexcept;
			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos, PlayerID pID, int GunSel) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
		public: //継承
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
			void			Dispose(void) noexcept override;
		};
	};
};
