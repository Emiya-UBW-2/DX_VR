#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"
#include	"CharacterEnum.hpp"
#include	"Character_before.hpp"
#include	"CharaAnimData.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

#include	"Weapon.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public HitBoxControl,
			public CharaMove
#ifdef _USE_EFFEKSEER_
			, public EffectControl
#endif
		{
		private:
			EnumArmAnimType									m_CharaAction{ EnumArmAnimType::Ready };
			std::array<ArmMovePerClass, static_cast<int>(EnumArmAnimType::Max)>	m_Arm;
			//入力
			int													m_CharaSound{ -1 };			//サウンド
			CharaTypeID											m_CharaType{};
			std::shared_ptr<WeaponObject::WeaponClass>			m_Weapon_Ptr{ nullptr };		//銃
			bool												m_ConcussionSwitch{ false };
			float												m_YaTimer{ 0.f };

			float												m_DamageCoolTime{ 0.f };
			float												m_TsubaSoundCoolTime{ 0.f };
			float												m_GuardHit{ 0.f };

			float												m_RunTime{ 0.f };
			float												m_FrontAttackActionTime{ 0.f };
			float												m_BackAttackActionTime{ 0.f };
			float												m_TsubaCoolDown{ 0.f };

			Vector2DX											m_BambooVecBase;

			Vector2DX											m_BambooVec;
			Vector2DX											m_GuardVec, m_GuardVecR;

			float												m_HeartUp{ 0.f };
			float												m_HeartUpR{ 0.f };

			float												m_GuardTimer{ 0.f };
			float												m_GuardCoolDownTimer{ 0.f };

			DamageEventControl									m_Damage;
			bool												m_MoveOverRideFlag{ false };
			moves												m_OverRideInfo;
			Vector3DX											m_EyePosition;

			PlayerID											m_MyID{ 0 };
			PlayerID											m_ViewID{ 0 };

			WazaType											m_WazaType{ WazaType::Men };
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
			const auto& GetYaTimer(void) const noexcept { return m_YaTimer; }
			const auto& GetGuardCoolDownTimer(void) const noexcept { return m_GuardCoolDownTimer; }
			const auto GetGuardCoolDownTimerMax(void) const noexcept { return 4.f; }
			const auto& GetWeaponPtr(void) const noexcept { return m_Weapon_Ptr; }
			const auto& GetWazaType(void) const noexcept { return m_WazaType; }
			auto			PopConcussionSwitch(void) noexcept {
				auto Prev = m_ConcussionSwitch;
				m_ConcussionSwitch = false;
				return Prev;
			}
			auto			GetGuardOn(void) const noexcept { return m_GuardHit>0.f; }//ガードで竹刀を抑えた
			auto			GetYaTimerMax(void) const noexcept { return 10.f; }
			auto			IsGuardStarting(void) const noexcept { return false; }
			auto			IsAttackAction(EnumArmAnimType value) const noexcept {
				switch (value) {
				case EnumArmAnimType::Men:
				case EnumArmAnimType::Kote:
				case EnumArmAnimType::Dou:
				case EnumArmAnimType::Tsuki:
				case EnumArmAnimType::HikiMen:
				case EnumArmAnimType::HikiKote:
				case EnumArmAnimType::HikiDou:
					return true;
				case EnumArmAnimType::Ready:
				case EnumArmAnimType::Run:
				case EnumArmAnimType::Tsuba:
				case EnumArmAnimType::GuardSuriage:
				case EnumArmAnimType::Max:
				default:
					return false;
				}
			}
			auto			IsGuardAction(EnumArmAnimType value) const noexcept {
				switch (value) {
				case EnumArmAnimType::GuardSuriage:
					return true;
				case EnumArmAnimType::Ready:
				case EnumArmAnimType::Run:
				case EnumArmAnimType::Men:
				case EnumArmAnimType::Kote:
				case EnumArmAnimType::Dou:
				case EnumArmAnimType::Tsuki:
				case EnumArmAnimType::Tsuba:
				case EnumArmAnimType::HikiMen:
				case EnumArmAnimType::HikiKote:
				case EnumArmAnimType::HikiDou:
				case EnumArmAnimType::Max:
				default:
					return false;
				}
			}
			auto			IsAttacking(void) const noexcept { return IsAttackAction(m_CharaAction); }
			auto			GetGuardStartPer(void) const noexcept { return 0.f; }
			Vector3DX		GetFramePosition(CharaFrame frame) const noexcept { return GetObj_const().GetFramePosition(GetFrame(static_cast<int>(frame))); }
		public://セッター
			void			ValueSet(PlayerID pID, CharaTypeID value) noexcept {
				this->m_CharaType = value;
				this->m_MyID = pID;
			}
			void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept { this->m_Damage.AddDamageEvent(pRet); }
			void			SetWeaponPtr(std::shared_ptr<WeaponObject::WeaponClass>& pWeaponPtr0) noexcept { this->m_Weapon_Ptr = pWeaponPtr0; }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
			void			SetViewID(PlayerID value) noexcept { this->m_ViewID = value; }
			bool			SetDamageEvent(const DamageEvent& value, bool IsTraining) noexcept;
			void			SetMoveOverRide(const moves& o) noexcept {
				this->m_MoveOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
			void			SetActionOverRide(EnumArmAnimType o) noexcept {
				this->m_CharaAction = o;
				OverrideAction();
			}
		public: //更新関連
			bool			CheckDamageRay(HitPoint* Damage, PlayerID AttackID, float Kihaku, WazaType pWazaType, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;
			void			MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
		private:
			void			PlayVoice(void) const noexcept;
			void			StopVoice(void) const noexcept;

			void			CheckTsuba(void) noexcept;
			void			SetFumikomi(void) noexcept;

			void			OverrideAction(void) noexcept;
			void			OverrideReady(void) noexcept;
			void			OverrideRun(void) noexcept;
			void			OverrideFrontAttack(void) noexcept;
			void			OverrideDo(void) noexcept;
			void			OverrideTsuba(void) noexcept;
			void			OverrideBackAttack(void) noexcept;
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
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-2.5f * Scale3DRate, -0.f * Scale3DRate, -2.5f * Scale3DRate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(2.5f * Scale3DRate, 2.5f * Scale3DRate, 2.5f * Scale3DRate)).get()) == FALSE
					) {
					this->m_IsDraw |= true;
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (m_MyID == m_ViewID) {
						if (isDrawSemiTrans) {
							this->GetObj().DrawModel();
						}
					}
					else {
						for (int i = 0, Num = this->GetObj_const().GetMeshNum(); i < Num; ++i) {
							if (this->GetObj_const().GetMeshSemiTransState(i) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
						//hitbox描画
						//HitBoxControl::DrawHitBox();
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
#ifdef _USE_EFFEKSEER_
				EffectControl::Dispose();
#endif
			}
		};
	}
};
