#pragma once
#include	"../../Header.hpp"
#include "CharacterEnum.hpp"
#include "Character_before.hpp"
#include "CharaAnimData.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

#include "Weapon.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public KeyControl,
			public WalkSwingControl,
			public EyeSwingControl
		{
		private:
			bool												m_ActionFirstFrame{false};
			EnumWeaponAnimType									m_CharaAction{EnumWeaponAnimType::Ready};
			std::array<ArmMovePerClass, (int)EnumWeaponAnimType::Max>	m_Arm;
			//入力
			float												m_MeleeCoolDown{0.f};
			int													m_CharaSound{-1};			//サウンド
			std::shared_ptr<BackGroundClass>					m_BackGround;				//BG
			CharaTypeID											m_CharaType;
			std::shared_ptr<WeaponClass>						m_Weapon_Ptr{nullptr};			//銃

			float												m_YaTimer{0.f};

			float												m_RunTime{0.f};
			float												m_NormalActionTime{0.f};

			Vector3DX											m_MouseVecR;

			Vector3DX											m_BambooVec;
		public:
			auto&			GetWeaponPtrNow(void) noexcept { return this->m_Weapon_Ptr; }
			void			SetWeaponPtr(std::shared_ptr<WeaponClass>& pWeaponPtr0) noexcept {
				this->m_Weapon_Ptr = pWeaponPtr0;
			}
		private:
			PlayerID											m_MyID{0};
		public:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
		public:
			bool												CanLookTarget{true};

			bool												ResetMouse{true};
		private:
			void			move_RightArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept;
			void			move_LeftArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept;
			const Matrix4x4DX GetCharaDir(void) const noexcept;

			const auto		GetCharaPosition(void) const noexcept { return this->m_move.posbuf; }
			const auto		IsAimPer(void) const noexcept { return (this->m_Arm[(int)EnumWeaponAnimType::Ready].Per() <= 0.1f); }
			const auto		IsLowReadyPer(void) const noexcept { return (this->m_Arm[(int)EnumWeaponAnimType::Ready].Per() >= 0.95f); }
		private:
			void			OverrideReady() noexcept;

			void			FrontAttack_Start() noexcept;
			void			FrontAttack_End() noexcept;

			void			Do_Start() noexcept;
			void			Do_End() noexcept;

			void			Tsuba_Start() noexcept;
			void			Tsuba_End() noexcept;

			void			OverrideTsuba() noexcept;

			void			BackAttack_Start() noexcept;
			void			BackAttack_End() noexcept;
		public://ゲッター
			const Matrix4x4DX GetEyeMatrix(void) const noexcept;
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetCharaAction(void) const noexcept { return this->m_CharaAction; }
			const auto		GetMeleeSwitch(void) const noexcept { return m_MeleeCoolDown == 1.f; }
		public://セッター
			bool			SetDamageEvent(const DamageEvent& value) noexcept;
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteAction(void) noexcept;
			void			ExecuteAnim(void) noexcept;
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
