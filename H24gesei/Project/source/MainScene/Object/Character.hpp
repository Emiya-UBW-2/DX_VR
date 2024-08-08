#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"
#include "CharacterEnum.hpp"
#include "Character_before.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		class CharacterClass :
			public ObjectBaseClass,
			public CharaMove
#ifdef _USE_EFFEKSEER_
			, public EffectControl
#endif
		{
		private:
			//入力
			int													m_CharaSound{ -1 };			//サウンド
			CharaTypeID											m_CharaType{};
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

			float												m_GuardTimer{ 0.f };
			float												m_GuardCoolDownTimer{ 0.f };

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
			const auto& GetBambooVec(void) const noexcept { return m_BambooVec; }
			const auto& GetGuardVec(void) const noexcept { return m_GuardVecR; }
			const auto& GetYaTimer(void) const noexcept { return m_YaTimer; }
			const auto& GetGuardCoolDownTimer(void) const noexcept { return m_GuardCoolDownTimer; }
			const auto GetGuardCoolDownTimerMax(void) const noexcept { return 2.f; }
			auto			PopConcussionSwitch(void) noexcept {
				auto Prev = m_ConcussionSwitch;
				m_ConcussionSwitch = false;
				return Prev;
			}
			Vector3DX		GetFramePosition(CharaFrame frame) const noexcept { return GetObj_const().GetFramePosition(GetFrame(static_cast<int>(frame))); }
		public://セッター
			void			ValueSet(PlayerID pID, CharaTypeID value) noexcept {
				this->m_CharaType = value;
				this->m_MyID = pID;
			}
			void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept { this->m_Damage.AddDamageEvent(pRet); }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
			void			SetViewID(PlayerID value) noexcept { this->m_ViewID = value; }
			bool			SetDamageEvent(const DamageEvent& value) noexcept;
			void			SetMoveOverRide(const moves& o) noexcept {
				this->m_MoveOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
		public: //更新関連
			bool			CheckDamageRay(HitPoint* Damage, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;
			void			MovePoint(float pxRad, float pyRad, const Vector3DX& pPos) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
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
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-800.f * Scale_Rate, -0.f * Scale_Rate, -800.f * Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + Vector3DX::vget(800.f * Scale_Rate, 370.f * Scale_Rate, 800.f * Scale_Rate)).get()) == FALSE
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
						if (!isDrawSemiTrans) {
							this->GetObj().DrawModel();
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
#ifdef _USE_EFFEKSEER_
				EffectControl::Dispose();
#endif
			}
		};
	}
};
