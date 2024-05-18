#pragma once
#include	"../../Header.hpp"
#include "CharacterEnum.hpp"
#include "Character_before.hpp"
#include "CharaAnimData.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

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
			public HitReactionControl
		{
		private:
			bool												m_ActionFirstFrame{false};
			CharaActionID										m_CharaAction{CharaActionID::Ready};
			std::array<ArmMovePerClass, (int)EnumGunAnimType::Max>	m_Arm;
			float												m_AmmoHandR{0.f};
			float												m_AmmoHand{0.f};
			float												m_HPRec{0.f};
			//入力
			float												m_MeleeCoolDown{0.f};
			float												m_SoundPower{0.f};			//サウンド
			int													m_CharaSound{-1};			//サウンド
			std::shared_ptr<BackGroundClass>				m_BackGround;				//BG
			CharaTypeID											m_CharaType;
			int													m_ArmerStock{0};
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
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetCharaAction(void) const noexcept { return this->m_CharaAction; }
			auto&			GetSoundPower(void) noexcept { return this->m_SoundPower; }
			const auto		GetMeleeSwitch(void) const noexcept { return m_MeleeCoolDown == 1.f; }
			const auto&		GetArmerStock(void) const noexcept { return this->m_ArmerStock; }
		public://セッター
			bool			SetDamageEvent(const DamageEvent& value) noexcept;
			void			Heal(HitPoint value, bool) noexcept {
				LifeControl::SetHealEvent(this->m_MyID, this->m_MyID, value);
			}
			const bool		CheckDamageRay(HitPoint* Damage, bool CheckBodyParts, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;
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
			void			SetMapCol(const std::shared_ptr<BackGroundClass>& backGround) noexcept { this->m_BackGround = backGround; }
			void			ValueSet(PlayerID pID, bool IsMainGame, CharaTypeID value) noexcept;
			void			MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept;
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
