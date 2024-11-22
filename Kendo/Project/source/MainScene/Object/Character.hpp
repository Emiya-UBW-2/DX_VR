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
		class CharacterClass : public ObjectBaseClass {
		private:
#ifdef _USE_EFFEKSEER_
			EffectControl										m_Effect;
#endif
			int													m_CharaSound{ -1 };			//サウンド
			std::shared_ptr<WeaponObject::WeaponClass>			m_Weapon_Ptr{ nullptr };		//銃
			bool												m_ConcussionSwitch{ false };
			HitBoxControl										m_HitBoxControl;
			CharaMove											m_CharaMove;
			DamageEventControl									m_Damage;
			float												m_DamageCoolTime{ 0.f };
			float												m_TsubaSoundCoolTime{ 0.f };
			float												m_GuardHit{ 0.f };
			Vector2DX											m_BambooVecBase;
			Vector2DX											m_BambooVec;
			bool												m_MoveOverRideFlag{ false };
			moves												m_OverRideInfo;
			PlayerID											m_MyID{ 0 };
			PlayerID											m_ViewID{ 0 };
		public:
			bool												CanLookTarget{ true };
		public://ゲッター
			const auto&		GetCharaAction(void) const noexcept { return m_CharaMove.GetCharaAction(); }
			const auto&		GetEyePosition(void) const noexcept { return this->m_CharaMove.GetEyePosition(); }
			const auto		GetEyeMatrix(void) const noexcept { return m_CharaMove.GetEyeMatrix(); }
			const auto&		GetWazaType(void) const noexcept { return m_CharaMove.m_WazaType; }
			const auto&		GetHeartRate(void) const noexcept { return m_CharaMove.GetHeartRate(); }
			const auto&		GetHeartRatePow(void) const noexcept { return m_CharaMove.GetHeartRatePow(); }
			auto			GetStaminaPer(void) const noexcept { return m_CharaMove.GetStaminaPer(); }
			auto			GetYaTimerPer(void) const noexcept { return m_CharaMove.GetYaTimerPer(); }
			auto			GetGuardCoolDownTimerPer(void) const noexcept { return m_CharaMove.GetGuardCoolDownTimerPer(); }

			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_Damage; }
			const auto&		GetBambooVec(void) const noexcept { return m_BambooVec; }
			const auto&		GetWeaponPtr(void) const noexcept { return m_Weapon_Ptr; }
			auto			PopConcussionSwitch(void) noexcept {
				auto Prev = m_ConcussionSwitch;
				m_ConcussionSwitch = false;
				return Prev;
			}
			const auto		IsOutArea(void) const noexcept {
				Vector3DX Vec = this->GetMove().GetPos() - Vector3DX::zero();
				float Len = 11.f / 2.f * Scale3DRate;
				return ((Vec.x < -Len || Len < Vec.x) ||
					(Vec.z < -Len || Len < Vec.z));
			}
			auto			GetGuardOn(void) const noexcept { return m_GuardHit > 0.f; }//ガードで竹刀を抑えた
			Vector3DX		GetFramePosition(CharaFrame frame) const noexcept { return GetObj_const().GetFramePosition(GetFrame(static_cast<int>(frame))); }
			//Vector3DX		GetFramePosition(CharaFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))).pos(); }
			
		public://セッター
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
				m_CharaMove.SetActionOverRide(o);
				OverrideAction();
			}
		public: //更新関連
			bool			CheckDamageRay(HitPoint Damage, PlayerID AttackID, float Kihaku, WazaType pWazaType, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;
			void			MovePoint(float pyRad, const Vector3DX& pPos) noexcept;
			void			SetInput(const InputControl& pInput) noexcept;
		private:
			bool			CheckTsuba(void) const noexcept;
			void			SetFumikomi(void) noexcept;
			void			OverrideAction(void) noexcept;
			void			OverrideReady(void) noexcept { m_CharaMove.OverrideReady(); }
			void			OverrideTsuba(void) noexcept { m_CharaMove.OverrideTsuba(); }
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteAnim(void) noexcept;
			void			ExecuteSound(void) noexcept;
			void			ExecuteMatrix(void) noexcept;
		private: //継承
			int				GetFrameNum(void) noexcept override { return static_cast<int>(CharaFrame::Max); }
			const char*		GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }
			int				GetShapeNum(void) noexcept override { return static_cast<int>(CharaShape::Max); }
			const char*		GetShapeStr(int id) noexcept override { return CharaShapeName[id]; }
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
			void			CheckDraw(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			DrawShadow(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
		};
	}
};
