#pragma once
#include	"../../Header.hpp"
#include "CharacterEnum.hpp"
#include "Character_before.hpp"
#include "CharaAnimData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public LifeControl,
			public KeyControl,
			public EffectControl,
			public LaserSightClass,
			public HitBoxControl,
			public WalkSwingControl,
			public EyeSwingControl,
			public StackLeftHandControl,
			public MagStockControl,
			public HitReactionControl,
			public RagDollControl,
			public ULTControl,
			public GunReadyControl,
			public GunPtrControl,
			public AutoAimControl,
			public ArmerPtrControl,
			public MorphinePtrControl,
			public ItemPopControl
		{
		private:
			bool												m_ActionFirstFrame{false};
			CharaActionID										m_CharaAction{CharaActionID::Ready};
			std::array<ArmMovePerClass, (int)EnumGunAnimType::Max>	m_Arm;
			bool												m_MagHand{false};
			ArmMovePerClass										m_MagArm;
			float												m_StuckGunTimer{0.f};
			bool												m_IsStuckGun{false};
			float												m_AmmoHandR{0.f};
			float												m_AmmoHand{0.f};
			int													m_ReadyAnimPhase{0};
			float												m_ReadyAnim{-1.f};
			bool												m_IsChanging{false};
			bool												m_HeadShotSwitch{false};
			float												m_ULTUp{0.f};
			bool												m_ULTActive{false};
			float												m_HPRec{0.f};
			//入力
			float												m_MeleeCoolDown{0.f};
			bool												m_ArmBreak{false};
			switchs												m_SightChange;
			float												m_SoundPower{0.f};			//サウンド
			int													m_CharaSound{-1};			//サウンド
			VECTOR_ref											m_RecoilRadAdd;
			Pendulum2D											m_SlingZrad;
			std::array<float, 2>								m_SlingPer;
			std::array<MATRIX_ref, 2>							m_SlingMat;
			ArmMovePerClass										m_ULTBar;
			std::shared_ptr<BackGroundClassBase>				m_BackGround;				//BG
			CharaTypeID											m_CharaType;
			bool												m_IsMainGame{false};
			int													m_ArmerStock{0};
			int													m_MorphineStock{0};
			bool												m_AmmoLoadStart{false};
			bool												m_AmmoLoadSwitch{false};
			int													m_AmmoLoadCount{0};

			bool											m_IsHardMode{false};
		public:
			bool												CanLookTarget{true};

			bool			GetArmer() noexcept {
				auto prev = this->m_ArmerStock;
				this->m_ArmerStock = 1;
				return this->m_ArmerStock != prev;
			}
			void			SetPlayMode(bool value) noexcept {
				m_IsHardMode = value;
			}
		private:
			void			move_RightArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			void			move_LeftArm(const VECTOR_ref& GunPos, const VECTOR_ref& Gunyvec, const VECTOR_ref& Gunzvec) noexcept;
			const MATRIX_ref GetCharaDir(void) const noexcept;

			const auto		GetCharaPosition(void) const noexcept { return this->m_move.posbuf; }
			const auto		IsAimPer(void) const noexcept { return (this->m_Arm[(int)EnumGunAnimType::Ready].Per() <= 0.1f); }
			const auto		IsLowReadyPer(void) const noexcept { return (this->m_Arm[(int)EnumGunAnimType::Ready].Per() >= 0.95f); }
		private:
			void			Shot_Start() noexcept;
			void			Reload_Start() noexcept;
		public://ゲッター
			const MATRIX_ref GetEyeMatrix(void) const noexcept;
			const auto&		GetGunSelPer(void) const noexcept { return this->m_ULTBar.Per(); }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetCharaAction(void) const noexcept { return this->m_CharaAction; }
			auto&			GetSoundPower(void) noexcept { return this->m_SoundPower; }
			const auto		GetMeleeSwitch(void) const noexcept { return m_MeleeCoolDown == 1.f; }
			const auto		GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd*(60.f / FPS); }
			const auto		PopHeadShotSwitch() noexcept {
				auto ret = this->m_HeadShotSwitch;
				this->m_HeadShotSwitch = false;
				return ret;
			}
			const auto&		GetArmerStock(void) const noexcept { return this->m_ArmerStock; }
			const auto&		GetMorphineStock(void) const noexcept { return this->m_MorphineStock; }
		public://セッター
			bool			SetDamageEvent(const DamageEvent& value) noexcept;
			void			Heal(HitPoint value, bool SwitchOn) noexcept {
				LifeControl::SetHealEvent(this->m_MyID, this->m_MyID, value, 0);
				if (SwitchOn) {
					m_ArmBreak = false;
				}
			}
			const bool		CheckDamageRay(HitPoint* Damage, ArmerPoint* ArmerDamage, bool CheckBodyParts, PlayerID AttackID, const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept;
			void			LoadExtends() noexcept;
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteSound(void) noexcept;
			void			ExecuteMatrix(void) noexcept;
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				this->m_IsDraw = true;
			}
			~CharacterClass(void) noexcept {}
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept { this->m_BackGround = backGround; }
			void			ValueSet(PlayerID pID, bool IsMainGame, CharaTypeID value) noexcept;
			void			MovePoint(float pxRad, float pyRad, const VECTOR_ref& pPos, int GunSel) noexcept;
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
