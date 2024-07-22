#pragma once
#pragma warning(disable:4464)
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
			public EyeSwingControl
		{
		private:
			bool												m_ActionFirstFrame{false};
			EnumWeaponAnimType									m_CharaAction{EnumWeaponAnimType::Ready};
			std::array<ArmMovePerClass, static_cast<int>(EnumWeaponAnimType::Max)>	m_Arm;
			//入力
			int													m_CharaSound{-1};			//サウンド
			CharaTypeID											m_CharaType{};
			std::shared_ptr<WeaponClass>						m_Weapon_Ptr{nullptr};		//銃

			float												m_YaTimer{0.f};

			float												m_RunTime{0.f};
			float												m_NormalActionTime{0.f};

			Vector3DX											m_MouseVecR;

			Vector3DX											m_BambooVec;
			Vector3DX											m_GuardVec, m_GuardVecR;

			float												m_HeartUp{0.f};
			float												m_HeartUpR{0.f};

			float												m_GuardTimer{0.f};


			float												m_D1{1.f};
			float												m_D2{0.f};
			float												m_D3{0.1f};
			float												m_D4{0.f};

			DamageEventControl									m_Damage;
			bool												m_MoveOverRideFlag{ false };
			moves												m_OverRideInfo;
			Matrix3x3DX											m_BaseMatrix;
		public:
			auto&			GetWeaponPtrNow(void) noexcept { return this->m_Weapon_Ptr; }
			void			SetWeaponPtr(std::shared_ptr<WeaponClass>& pWeaponPtr0) noexcept {
				this->m_Weapon_Ptr = pWeaponPtr0;
			}
		private:
			PlayerID											m_MyID{ 0 };
			PlayerID											m_ViewID{ 0 };
		public:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_Damage; }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
			void			SetViewID(PlayerID value) noexcept { this->m_ViewID = value; }
		public:
			bool												CanLookTarget{true};
		private:
			void			move_RightArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept ;
			void			move_LeftArm(const Vector3DX& GunPos, const Vector3DX& Gunyvec, const Vector3DX& Gunzvec) noexcept ;
			const Matrix3x3DX GetCharaDir(void) const noexcept ;

			auto		GetCharaPosition(void) const noexcept { return this->m_move.GetPos(); }
			auto		IsAimPer(void) const noexcept { return (this->m_Arm[static_cast<size_t>(EnumWeaponAnimType::Ready)].Per() <= 0.1f); }
			auto		IsLowReadyPer(void) const noexcept { return (this->m_Arm[static_cast<size_t>(EnumWeaponAnimType::Ready)].Per() >= 0.95f); }
		private:
			void			OverrideReady() noexcept ;

			void			FrontAttack_Start() noexcept ;
			void			FrontAttack_End() noexcept ;

			void			Do_Start() noexcept ;
			void			Do_End() noexcept ;

			void			Tsuba_Start() noexcept ;
			void			Tsuba_End() noexcept ;

			void			OverrideTsuba() noexcept ;

			void			BackAttack_Start() noexcept ;
			void			BackAttack_End() noexcept ;
		public://ゲッター
			const Matrix4x4DX GetEyeMatrix(void) const noexcept ;
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetCharaAction(void) const noexcept { return this->m_CharaAction; }
			const auto&		GetBambooVec(void) const noexcept { return m_BambooVec; }
			const auto&		GetGuardVec(void) const noexcept { return m_GuardVecR; }
			auto		IsGuardStarting(void) const noexcept { return m_CharaAction == EnumWeaponAnimType::GuardStart; }
			auto		GetGuardStartPer(void) const noexcept { return (IsGuardStarting()) ? (m_GuardTimer / 1.f) : 0.f; }

			Vector3DX		GetFramePosition(CharaFrame frame) const noexcept { return MV1GetFramePosition(GetObj_const().GetHandle(), GetFrame(static_cast<int>(frame))); }
		public://セッター
			bool			SetDamageEvent(const DamageEvent& value) noexcept ;
			void			SetMoveOverRide(const moves& o) noexcept {
				this->m_MoveOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
		private: //更新関連
			void			ExecuteInput(void) noexcept ;
			void			ExecuteAction(void) noexcept ;
			void			ExecuteAnim(void) noexcept ;
			void			ExecuteSound(void) noexcept ;
			void			ExecuteMatrix(void) noexcept ;
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = static_cast<int>(ObjType::Human);
				this->m_IsDraw = true;
			}
			CharacterClass(const CharacterClass&) = delete;
			CharacterClass(CharacterClass&& o) = delete;
			CharacterClass& operator=(const CharacterClass&) = delete;
			CharacterClass& operator=(CharacterClass&& o) = delete;

			virtual ~CharacterClass(void) noexcept {}
		public:
			void			ValueSet(PlayerID pID, bool IsMainGame, CharaTypeID value) noexcept ;
			void			MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept ;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept ;
		private:
			int	GetFrameNum() noexcept override { return static_cast<int>(CharaFrame::Max); }
			const char*	GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }

			int	GetShapeNum() noexcept override { return static_cast<int>(CharaShape::Max); }
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
