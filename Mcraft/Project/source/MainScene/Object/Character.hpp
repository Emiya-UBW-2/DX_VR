#pragma once
#include	"../../Header.hpp"
#include "CharacterEnum.hpp"
#include "Character_before.hpp"
#include "CharaAnimData.hpp"

#include "Gun_before.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		class CharacterClass : public ObjectBaseClass {
		private:
			HitBoxControl										m_HitBoxControl;
			WalkSwingControl									m_WalkSwingControl;
			EyeSwingControl										m_EyeSwingControl;
			HitReactionControl									m_HitReactionControl;
			RagDollControl										m_RagDollControl;
			GunPtrControl										m_GunPtrControl;
			AutoAimControl										m_AutoAimControl;
			EffectControl										m_EffectControl;
			LeanControl											m_LeanControl{};
			MoveControl											m_MoveControl{};
			RotateControl										m_RotateControl{};

			InputControl										m_Input;
			bool												m_IsSquat{ false };
			std::array<float, static_cast<int>(CharaAnimeID::AnimeIDMax)>	m_AnimPerBuf{ 0 };
			CharaAnimeID										m_BottomAnimSelect{};
			float												m_SwitchPer{};
			bool												m_IsSwitchRight{};
			EnumGunReadySeq										m_GunReadySeq{ EnumGunReadySeq::Aim };
			float												m_ADSPer{ 0.f };
			PointControl<HitPoint, 100>							m_HP{};
			PointControl<ArmerPoint, 100>						m_AP{};
			DamageEventControl									m_Damage;
			std::array<bool, static_cast<int>(EnumGunAnimType::Max)>			m_IsActiveArmAnim{};
			bool												m_MagHand{ false };
			ArmMovePerClass										m_MagArm;
			float												m_StuckGunTimer{ 0.f };
			bool												m_IsStuckGun{ false };
			bool												m_IsChanging{ false };
			bool												m_IsChange{ false };
			float												m_HPRec{ 0.f };
			bool												m_ArmBreak{ false };
			switchs												m_SightChange;
			switchs												m_ULTKey;
			switchs												m_ThrowKey;
			switchs												m_Squat;
			int													m_CharaSound{ -1 };			//サウンド
			Vector3DX											m_RecoilRadAdd;
			Pendulum2D											m_SlingArmZrad;
			float												m_ArmBreakPer{};
			Pendulum2D											m_SlingZrad;
			FallControl											m_Grenade;
			std::shared_ptr<AmmoDataClass>						m_ChamberAmmoData{ nullptr };		//
			bool												m_MoveOverRideFlag{ false };
			moves												m_OverRideInfo;
			PlayerID											m_MyID{ 0 };
			CharaTypeID											m_CharaType{};
			bool												m_Cancel{ false };			//

			bool												m_IsGrenadeThrow{ false };			//
		private://キャッシュ
			Matrix3x3DX											m_CharaRotationCache{};
			Matrix3x3DX											m_EyeRotationCache{};
			Vector3DX											m_EyePositionCache{};
		public:
			bool												m_CanLookTarget{ true };
			Vector3DX											m_CameraPos;
			float												m_Length{};

		public://ゲッター
			//外にあぶれていた項目
			const auto		IsMoveFront(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_W) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_S); }
			const auto		IsMoveBack(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_S) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_W); }
			const auto		IsMoveLeft(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_A) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_D); }
			const auto		IsMoveRight(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_D) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_A); }

			const auto		GetCharaLocalDir(void) const noexcept {
				return
					Matrix3x3DX::RotAxis(Vector3DX::forward(), m_LeanControl.GetRad()) *
					Matrix3x3DX::RotAxis(Vector3DX::right(), m_RotateControl.GetRad().x) *
					Matrix3x3DX::RotAxis(Vector3DX::up(), m_RotateControl.GetYRadBottomChange())
					;
			}
			const auto		GetBottomStandAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_WalkBack : CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_LeftStep : CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_RightStep : CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_Turn : CharaAnimeID::Bottom_Stand_Turn; }
			const auto		GetSpeed(void) const noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (this->m_IsSquat) {
					if (this->m_Input.GetPADSPress(Controls::PADS::WALK)) {
						return 0.95f * 60.f * DXLib_refParts->GetDeltaTime();
					}
					return 1.85f * 60.f * DXLib_refParts->GetDeltaTime();
				}
				else {
					if (this->m_Input.GetPADSPress(Controls::PADS::WALK)) {
						return 1.25f * 60.f * DXLib_refParts->GetDeltaTime();
					}
					return 2.25f * 60.f * DXLib_refParts->GetDeltaTime();
				}
			}
			const auto		GetSpeedPer(void) const noexcept { return std::clamp(GetSpeed() / 0.65f, 0.5f, 1.f); }
			const auto		GetIsADS(void) const noexcept { return this->m_GunReadySeq == EnumGunReadySeq::ADS; }
			const auto		GetSightZoomSize() const noexcept { return GetIsADS() ? m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect()).GetSightZoomSize() : 1.f; }
			auto&			GetRagDoll(void) noexcept { return m_RagDollControl.GetRagDoll(); }
			const auto&		GetLeanRad(void) const noexcept { return m_LeanControl.GetRad(); }
			const auto&		GetGunPtrNow(void) const noexcept { return m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect()).m_Gun_Ptr; }
			const auto&		GetAutoAimID(void) const noexcept { return m_AutoAimControl.GetAutoAimID(); }
			const auto		GetAutoAimActive(void) const noexcept { return m_AutoAimControl.GetAutoAimActive(); }
			const auto&		GetHitBoxList(void) const noexcept { return m_HitBoxControl.GetHitBoxPointList(); }
			const auto		IsGunShotSwitch(void) const noexcept { return m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect()).m_Gun_Ptr && m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect()).m_Gun_Ptr->GetShotSwitch(); }
			const auto		GetAutoAimRadian() const noexcept {
				float Len = std::max(0.01f, std::hypotf((float)(m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect()).m_Gun_Ptr->GetAimXPos() - 1920 / 2), (float)(m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect()).m_Gun_Ptr->GetAimYPos() - 1080 / 2)));
				Len = std::clamp(100.f / Len, 0.f, 1.f);
				return deg2rad(5) * Len;
			}
			void			DrawReticle(void) const noexcept {
				if (m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect()).m_Gun_Ptr) {
					if ((this->m_ADSPer >= 0.8f) || (m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect()).GetSightZoomSize() == 1.f)) {
						m_GunPtrControl.GetParam(m_GunPtrControl.GetNowGunSelect()).m_Gun_Ptr->DrawReticle(GetLeanRad());
					}
				}
			}
		public://ゲッター
			const auto		IsAlive(void) const noexcept { return m_HP.IsNotZero(); }
			const auto		IsLowHP(void) const noexcept { return this->m_HP.GetPoint() < (this->m_HP.GetMax() * 35 / 100); }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			auto			GetFrameWorldMat(CharaFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			auto			GetFrameLocalMat(CharaFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(GetFrame(static_cast<int>(frame))); }
			const auto&		GetDamageEvent(void) const noexcept { return m_Damage; }
			const auto&		GetEyeRotationCache(void) const noexcept { return this->m_EyeRotationCache; }
			const auto&		GetEyePositionCache(void) const noexcept { return this->m_EyePositionCache; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto		GetRecoilRadAdd(void) const noexcept { return this->m_RecoilRadAdd * (60.f * DXLib_ref::Instance()->GetDeltaTime()); }
		public://セッター
			void			SetPlayerID(PlayerID value) noexcept {
				this->m_MyID = value;
				//銃のIDセットアップ
				for (int loop = 0; loop < 2; ++loop) {
					if (m_GunPtrControl.GetParam(loop).m_Gun_Ptr) {
						m_GunPtrControl.GetParam(loop).m_Gun_Ptr->SetPlayerID(GetMyPlayerID());
					}
				}
			}
			void			SetCharaTypeID(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			SetMoveOverRide(const moves& o) noexcept {
				this->m_MoveOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
			void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept { m_Damage.AddDamageEvent(pRet); }
			bool			SetDamageEvent(const DamageEvent& value) noexcept;
			void			Heal(HitPoint value) noexcept { m_Damage.Add(GetMyPlayerID(), GetMyPlayerID(), -value, -value, Vector3DX::up()); }
			const bool		CheckDamageRay(HitPoint* Damage, ArmerPoint* ArmerDamage, bool CheckBodyParts, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteMatrix(void) noexcept;
			void			ExecuteGrenade(void) noexcept;
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = static_cast<int>(ObjType::Human);
				this->m_IsDraw = true;
			}
			~CharacterClass(void) noexcept {}
		public:
			static void		LoadChara(const std::string& FolderName, PlayerID ID) noexcept;
			void			LoadCharaGun(const std::string& FolderName, int Sel, bool IsPreset) noexcept;
			void			SetupRagDoll(void) noexcept {
				MV1::SetAnime(&m_RagDollControl.GetRagDoll(), GetObj());
				m_RagDollControl.Init(GetObj());
			}
			void			Spawn(float pxRad, float pyRad, const Vector3DX& pPos, int GunSel) noexcept;
			void			Input(const InputControl& pInput) noexcept { this->m_Input = pInput; }
		private:
			int				GetFrameNum() noexcept override { return static_cast<int>(CharaFrame::Max); }
			const char*		GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }

			int				GetShapeNum() noexcept override { return static_cast<int>(CharaShape::Max); }
			const char*		GetShapeStr(int id) noexcept override { return CharaShapeName[id]; }
		private: //継承
			void			Init_Sub(void) noexcept override;
			void			FirstExecute(void) noexcept override;
			void			CheckDraw(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					if (IsAlive()) {
						this->GetObj().DrawModel();
					}
					else {
						m_RagDollControl.GetRagDoll().DrawModel();
					}
				}
			}
			void			Dispose_Sub(void) noexcept override {
				m_EffectControl.Dispose();
				m_GunPtrControl.Dispose();
				m_ChamberAmmoData.reset();
			}
		};
	};
};
