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
			bool												m_TsubaSoundSwitch{ false };
			float												m_GuardHit{ 0.f };
			//
			Vector2DX											m_BambooVecBase;
			Vector2DX											m_BambooVec;
			//
			bool												m_MoveOverRideFlag{ false };
			moves												m_OverRideInfo;
			PlayerID											m_MyID{ 0 };
			PlayerID											m_ViewID{ 0 };
		public://ゲッター
			const auto&		GetCharaAction(void) const noexcept { return m_CharaMove.GetCharaAction(); }
			const auto&		GetEyePosition(void) const noexcept { return m_CharaMove.GetEyePosition(); }
			const auto		GetEyeMatrix(void) const noexcept { return m_CharaMove.GetEyeMatrix(); }
			const auto&		GetHeartRate(void) const noexcept { return m_CharaMove.GetHeartRate(); }
			const auto&		GetHeartRatePow(void) const noexcept { return m_CharaMove.GetHeartRatePow(); }
			const auto&		GetInputControl(void) const noexcept { return m_CharaMove.GetInputControl(); }
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
			Vector3DX		GetFramePosition(CharaFrame frame) const noexcept { return GetObj_const().GetFramePosition(GetFrame(static_cast<int>(frame))); }
			//Vector3DX		GetFramePosition(CharaFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))).pos(); }
			const auto		IsPlayerChara(void) const noexcept { return this->m_MyID == this->m_ViewID; }
		public://セッター
			void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept { this->m_Damage.AddDamageEvent(pRet); }
			void			SetWeaponPtr(std::shared_ptr<WeaponObject::WeaponClass>& pWeaponPtr0) noexcept { this->m_Weapon_Ptr = pWeaponPtr0; }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
			void			SetViewID(PlayerID value) noexcept { this->m_ViewID = value; }
			void			SetMoveOverRide(const moves& o) noexcept {
				this->m_MoveOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
			void			SetActionOverRide(EnumArmAnimType o) noexcept {
				if (m_CharaMove.GetCharaAction() == EnumArmAnimType::Run) { return; }
				ChangeAction(o);
			}
			void			MovePoint(float pyRad, const Vector3DX& pPos) noexcept {
				m_CharaMove.Init(pyRad);
				Matrix3x3DX Mat; Mat.SetRadian(0.f, pyRad, 0.f);
				ResetMove(Mat, pPos);
				this->m_CharaSound = -1;
			}
		public: //更新関連
			bool			SetDamageEvent(const DamageEvent& value, bool IsTraining) noexcept;
			void			SetInput(const InputControl& pInput) noexcept;
			void			UpdateBambooAttack(void) noexcept;
		private:
			bool			CheckDamageRay(CharacterClass* pAttacker, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;
			bool			CheckTsuba(void) const noexcept;
			void			ChangeAction(EnumArmAnimType o) noexcept;
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteAnim(void) noexcept;
			void			ExecuteSound(void) noexcept;
			void			ExecuteMatrix(void) noexcept;
			void			ExecuteWeapon(void) noexcept;
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
