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
			public ItemPopControl {
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
			bool												m_IsChange{false};
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
			Vector3DX											m_RecoilRadAdd;
			Pendulum2D											m_SlingZrad;
			std::array<float, 2>								m_SlingPer;
			std::array<Matrix4x4DX, 2>							m_SlingMat;
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
		private:
			PlayerID											m_MyID{0};
		public:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
		public:
			bool												CanLookTarget{true};

			bool			GetArmer() noexcept {
				auto prev = this->m_ArmerStock;
				this->m_ArmerStock = 1;
				return this->m_ArmerStock != prev;
			}
			void			SetPlayMode(bool value) noexcept { m_IsHardMode = value; }
		private:
			void			move_RightArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept;
			void			move_LeftArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept;
			const Matrix4x4DX GetCharaDir(void) const noexcept;

			const auto		GetCharaPosition(void) const noexcept { return this->m_move.posbuf; }
			const auto		IsAimPer(void) const noexcept { return (this->m_Arm[(int)EnumGunAnimType::Ready].Per() <= 0.1f); }
			const auto		IsLowReadyPer(void) const noexcept { return (this->m_Arm[(int)EnumGunAnimType::Ready].Per() >= 0.95f); }
		private:
			void			Shot_Start() noexcept;
			void			Reload_Start() noexcept;
		public://ゲッター
			const Matrix4x4DX GetEyeMatrix(void) const noexcept;
			const auto&		GetGunSelPer(void) const noexcept { return this->m_ULTBar.Per(); }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetCharaAction(void) const noexcept { return this->m_CharaAction; }
			auto&			GetSoundPower(void) noexcept { return this->m_SoundPower; }
			const auto		GetMeleeSwitch(void) const noexcept { return m_MeleeCoolDown == 1.f; }
			const auto		GetRecoilRadAdd(void) const noexcept {
				auto* DrawParts = DXDraw::Instance();
				return this->m_RecoilRadAdd*(60.f / DrawParts->GetFps());
			}
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
			const bool		CheckDamageRay(HitPoint* Damage, ArmerPoint* ArmerDamage, bool CheckBodyParts, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;
			void			LoadExtends() noexcept;
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteSound(void) noexcept;
			void			ExecuteMatrix(void) noexcept;
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = (int)ObjType::Human;
				this->m_IsDraw = true;
			}
			~CharacterClass(void) noexcept {}
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept { this->m_BackGround = backGround; }
			void			ValueSet(PlayerID pID, bool IsMainGame, CharaTypeID value) noexcept;
			void			MovePoint(float pxRad, float pyRad, const Vector3DX& pPos, int GunSel) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
		private:
			int	GetFrameNum() noexcept override { return (int)CharaFrame::Max; }
			const char*	GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }

			int	GetShapeNum() noexcept override { return (int)CharaShape::Max; }
			const char*	GetShapeStr(int id) noexcept override { return CharaShapeName[id]; }
		private: //継承
			void			Init_Sub(void) noexcept override;
			void			FirstExecute(void) noexcept override;
			void			CheckDraw(void) noexcept override {
				this->m_IsDraw = false;
				this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetScreenPosition()).magnitude();
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f*Scale_Rate, -0.f*Scale_Rate, -2.5f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f*Scale_Rate, 2.5f*Scale_Rate, 2.5f*Scale_Rate)).get()) == FALSE
					) {
					this->m_IsDraw |= true;
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			DrawShadow(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
		};
	};
};
