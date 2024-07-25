#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"
#include "CharacterEnum.hpp"
#include "Character_before.hpp"
#include "CharaAnimData.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

#include "Weapon.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public KeyControl
		{
		private:
			EnumWeaponAnimType									m_CharaAction{ EnumWeaponAnimType::Ready };
			std::array<ArmMovePerClass, static_cast<int>(EnumWeaponAnimType::Max)>	m_Arm;
			//入力
			int													m_CharaSound{ -1 };			//サウンド
			CharaTypeID											m_CharaType{};
			std::shared_ptr<WeaponObject::WeaponClass>			m_Weapon_Ptr{ nullptr };		//銃

			float												m_YaTimer{ 0.f };

			float												m_RunTime{ 0.f };
			float												m_FrontAttackActionTime{ 0.f };
			float												m_BackAttackActionTime{ 0.f };
			float												m_TsubaCoolDown{ 0.f };

			Vector2DX											m_BambooVecBase;

			Vector2DX											m_BambooVec;
			Vector2DX											m_GuardVec, m_GuardVecR;

			float												m_HeartUp{ 0.f };
			float												m_HeartUpR{ 0.f };

			float												m_GuardStartTimer{ 0.f };
			float												m_GuardTimer{ 0.f };

			DamageEventControl									m_Damage;
			bool												m_MoveOverRideFlag{ false };
			moves												m_OverRideInfo;
			Vector3DX											m_EyePosition;

			PlayerID											m_MyID{ 0 };
			PlayerID											m_ViewID{ 0 };
		public:
			bool												CanLookTarget{ true };
		public://ゲッター
			const auto& GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			const auto& GetDamageEvent(void) const noexcept { return this->m_Damage; }
			const auto& GetEyePosition(void) const noexcept { return this->m_EyePosition; }
			const auto& GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto& GetCharaAction(void) const noexcept { return this->m_CharaAction; }
			const auto& GetBambooVec(void) const noexcept { return m_BambooVec; }
			const auto& GetGuardVec(void) const noexcept { return m_GuardVecR; }
			auto			IsGuardStarting(void) const noexcept { return m_CharaAction == EnumWeaponAnimType::GuardStart; }
			auto			GetGuardStartPer(void) const noexcept { return (IsGuardStarting()) ? (m_GuardStartTimer / 1.f) : 0.f; }
			Matrix3x3DX		GetEyeMatrix(void) const noexcept;
			Vector3DX		GetFramePosition(CharaFrame frame) const noexcept { return MV1GetFramePosition(GetObj_const().GetHandle(), GetFrame(static_cast<int>(frame))); }
		public://セッター
			void			ValueSet(PlayerID pID, CharaTypeID value) noexcept {
				this->m_CharaType = value;
				this->m_MyID = pID;
			}
			void			SetWeaponPtr(std::shared_ptr<WeaponObject::WeaponClass>& pWeaponPtr0) noexcept { this->m_Weapon_Ptr = pWeaponPtr0; }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
			void			SetViewID(PlayerID value) noexcept { this->m_ViewID = value; }
			bool			SetDamageEvent(const DamageEvent& value) noexcept;
			void			SetMoveOverRide(const moves& o) noexcept {
				this->m_MoveOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
			void			SetActionOverRide(EnumWeaponAnimType o) noexcept {
				this->m_CharaAction = o;
				OverrideAction();
			}
		public: //更新関連
			void			MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
		private:
			void			PlayVoice(void) const noexcept;
			void			StopVoice(void) const noexcept;

			void			CheckTsuba(void) noexcept;

			void			OverrideAction(void) noexcept;
			void			OverrideReady(void) noexcept;
			void			OverrideRun(void) noexcept;
			void			OverrideFrontAttack(void) noexcept;
			void			OverrideDo(void) noexcept;
			void			OverrideTsuba(void) noexcept;
			void			OverrideBackAttack(void) noexcept;
			void			OverrideGuardStart(void) noexcept;
			void			OverrideGuard(void) noexcept;
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteAction(void) noexcept;
			void			ExecuteAnim(void) noexcept;
			void			ExecuteSound(void) noexcept;
			void			ExecuteMatrix(void) noexcept;
		private: //継承
			int				GetFrameNum(void) noexcept override { return static_cast<int>(CharaFrame::Max); }
			const char* GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }

			int				GetShapeNum(void) noexcept override { return static_cast<int>(CharaShape::Max); }
			const char* GetShapeStr(int id) noexcept override { return CharaShapeName[id]; }
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
		private: //継承
			void			Init_Sub(void) noexcept override;
			void			FirstExecute(void) noexcept override;
			void			CheckDraw(void) noexcept override {
				this->m_IsDraw = false;
				this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetScreenPosition()).magnitude();
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f * Scale_Rate, -0.f * Scale_Rate, -2.5f * Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f * Scale_Rate, 2.5f * Scale_Rate, 2.5f * Scale_Rate)).get()) == FALSE
					) {
					this->m_IsDraw |= true;
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (GetMyPlayerID() == 0) {
						if (isDrawSemiTrans) {
							this->GetObj().DrawModel();
						}
					}
					else {
						for (int i = 0; i < this->GetObj().GetMeshNum(); i++) {
							if ((MV1GetMeshSemiTransState(this->GetObj().GetHandle(), i) == TRUE) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
					}
				}
			}
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					this->GetObj().DrawModel();
				}
			}
			void			Dispose_Sub(void) noexcept override {
				m_Weapon_Ptr.reset();
			}
		};
	}
};
