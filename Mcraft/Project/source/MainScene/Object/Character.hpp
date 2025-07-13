#pragma once
#include	"../../Header.hpp"
#include	"CharacterEnum.hpp"
#include	"Character_before.hpp"
#include	"CharaAnimData.hpp"

namespace FPS_n2 {
	namespace Charas {

		class CharacterObj : public BaseObject {
			HitBoxControl										m_HitBoxControl;
			WalkSwingControl									m_WalkSwingControl;
			HitReactionControl									m_HitReactionControl;
			RagDollControl										m_RagDollControl;
			GunPtrControl										m_GunPtrControl;
			LeanControl											m_LeanControl{};
			MoveControl											m_MoveControl{};
			RotateControl										m_RotateControl{};

			bool												m_IsRappelling{};
			bool												m_IsRappellingEnd{};
			InputControl										m_Input;
			bool												m_ReserveReload{ false };
			bool												m_IsSquat{ false };
			std::array<float, static_cast<int>(CharaAnimeID::AnimeIDMax)>	m_AnimPerBuf{ 0 };
			CharaAnimeID										m_BottomAnimSelect{};
			PointControl<HitPoint, 100>							m_HP{};
			PointControl<ArmerPoint, 160>						m_BodyPoint{};
			PointControl<ArmerPoint, 100>						m_HeadPoint{};
			DamageEventControl									m_Damage;
			float												m_StuckGunTimer{ 0.0f };
			bool												m_IsStuckGun{ false };
			float												m_HPRec{ 0.0f };
			bool												m_ArmBreak{ false };
			int													m_CharaSound{ InvalidID };			//サウンド
			Pendulum2D											m_SlingArmZrad;
			float												m_ArmBreakPer{};
			Pendulum2D											m_SlingZrad;
			bool												m_MoveOverRideFlag{ false };
			NetWork::MoveInfo									m_OverRideInfo;
			PlayerID											m_MyID{ 0 };
			CharaTypeID											m_CharaType{};
			bool												m_CanLookTarget{ true };
			float												m_Length{};
			bool												m_ConcussionSwitch{};
			float												m_Concussion{ 0.0f };
			float												m_ConcussionPer{ 0.0f };
			bool												m_CanWatch{ true };
			bool												m_HeadShotSwitch{ false };
			float												m_GunyAdd{ 0.0f };
			bool												m_GunFallActive{ true };

			int													m_GrenadeCount{ 3 };
			std::vector<int> m_ItemList_EnableSpawnBySoldier;

			float												m_WearArmorTime{ 0.f };
			float												m_WearArmorPer{ 0.f };
			int													m_WearArmorFlag{ InvalidID };
		private://キャッシュ
			Matrix3x3DX											m_EyeRotationCache{};
			Vector3DX											m_EyePositionCache{};
			bool												m_IsActiveCameraPos{ false };
			ScreenPosition										m_CameraPos;
		private:
			const auto		GetSpeed(void) const noexcept {
				if (this->m_IsSquat) {
					if (this->m_Input.GetPADSPress(Controls::PADS::WALK)) {
						return 0.15f * GetDebuff();
					}
					return 0.475f * GetDebuff();
				}
				else {
					if (this->m_Input.GetPADSPress(Controls::PADS::WALK)) {
						return 0.2f * GetDebuff();
					}
					return 0.625f * GetDebuff();
				}
			}
			float			GetDebuff(void) const noexcept;

			const auto		IsMoveFront(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_W) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_S); }
			const auto		IsMoveBack(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_S) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_W); }
			const auto		IsMoveLeft(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_A) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_D); }
			const auto		IsMoveRight(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_D) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_A); }
			const auto		GetBottomStandAnimSelect(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSelect(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSelect(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_WalkBack : CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSelect(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_LeftStep : CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSelect(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_RightStep : CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSelect(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_Turn : CharaAnimeID::Bottom_Stand_Turn; }
		public://プレイヤーキャラから見た際の情報
			const auto&		GetCanLookByPlayer(void) const noexcept { return this->m_CanLookTarget; }
			const auto&		GetLengthToPlayer(void) const noexcept { return this->m_Length; }
			const auto&		GetIsActiveCameraPosToPlayer(void) const noexcept { return this->m_IsActiveCameraPos; }
			const auto&		GetCameraPosToPlayer(void) const noexcept { return this->m_CameraPos; }
			const auto&		GetIsSquat(void) const noexcept { return this->m_IsSquat; }
		public://ゲッター
			int				GetWeight_gram(void) const noexcept;
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			const auto&		GetMoveEyePos(void) const noexcept { return this->m_WalkSwingControl.GetMoveEyePos(); }
			const auto&		GetLeanRad(void) const noexcept { return this->m_LeanControl.GetRad(); }
			const auto&		GetRotateRad(void) const noexcept { return this->m_RotateControl.GetRad(); }
			const auto&		GetHitBoxList(void) const noexcept { return this->m_HitBoxControl.GetHitBoxPointList(); }
			const auto		GetGunPtr(int ID) const noexcept { return this->m_GunPtrControl.GetGunPtr(ID); }
			const auto		GetGunPtrNowSel(void) const noexcept { return this->m_GunPtrControl.GetNowGunSelect(); }
			const auto		GetGunPtrNow(void) const noexcept { return GetGunPtr(this->m_GunPtrControl.GetNowGunSelect()); }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_Damage; }
			const auto&		GetEyeRotationCache(void) const noexcept { return this->m_EyeRotationCache; }
			const auto&		GetEyePositionCache(void) const noexcept { return this->m_EyePositionCache; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto		GetIsADS(void) const noexcept { return (!this->m_IsStuckGun && GetGunPtrNow() && GetGunPtrNow()->GetCanADS()) && this->m_Input.GetPADSPress(Controls::PADS::AIM); }
			const auto&		GetHP(void) const noexcept { return this->m_HP; }
			const auto&		GetBodyAP(void) const noexcept { return this->m_BodyPoint; }
			const auto&		GetHeadAP(void) const noexcept { return this->m_HeadPoint; }
			const auto		IsAlive(void) const noexcept { return this->m_HP.IsNotZero(); }
			const auto		IsLowHP(void) const noexcept { return this->m_HP.GetPoint() < (this->m_HP.GetMax() * 35 / 100); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			const auto		GetCameraPosition(void) const noexcept {
				if (GetGunPtrNow()) {
					return Lerp<Vector3DX>(GetEyePositionCache(), GetGunPtrNow()->GetPartsFrameMatParent(Guns::GunFrame::Eyepos).pos(), GetGunPtrNow()->GetGunAnimBlendPer(GunAnimeID::ADS));
				}
				return GetEyePositionCache();
			}
		public://セッター
			void			SetPlayerID(PlayerID ID) noexcept {
				this->m_MyID = ID;
				//銃のIDセットアップ
				for (int loop = 0, max = this->m_GunPtrControl.GetGunNum(); loop < max; ++loop) {
					if (this->m_GunPtrControl.GetGunPtr(loop)) {
						this->m_GunPtrControl.GetGunPtr(loop)->SetPlayerID(GetMyPlayerID());
					}
				}
			}
			void			SetCharaTypeID(CharaTypeID Type) noexcept { this->m_CharaType = Type; }
			void			SetMoveOverRide(const NetWork::MoveInfo& overrideInfo) noexcept {
				this->m_MoveOverRideFlag = true;
				this->m_OverRideInfo = overrideInfo;
			}
			void			SetDamageEventReset(void) noexcept { this->m_Damage.Reset(); }
			void			PopDamageEvent(std::vector<DamageEvent>* pRet) noexcept { this->m_Damage.Pop(pRet); }
			//自分がダメージを与えたと通知
			void			SetDamage(PlayerID damageID, HitPoint damage, int hitType, const Vector3DX& startPos, const Vector3DX& endPos) noexcept {
				this->m_Damage.Add(GetMyPlayerID(), damageID, damage, hitType, startPos, endPos);
			}
			void			Heal(HitPoint Point) noexcept { SetDamage(GetMyPlayerID(), -Point, static_cast<int>(HitType::Body), GetMove().GetPos(), GetMove().GetPos()); }

			void			SetWearArmorFlag(int ID) noexcept {
				this->m_WearArmorFlag = ID;
			}
			void			HealHelmet() noexcept {
				SetDamage(GetMyPlayerID(), -1, static_cast<int>(HitType::Helmet), GetMove().GetPos(), GetMove().GetPos());
				SetWearArmorFlag(1);
			}
			void			HealArmor() noexcept {
				SetDamage(GetMyPlayerID(), -1, static_cast<int>(HitType::Armor), GetMove().GetPos(), GetMove().GetPos());
				SetWearArmorFlag(0);
			}

			bool			SetDamageEvent(const DamageEvent& Event) noexcept;
			const bool		CheckDamageRay(HitPoint Damage, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;

			const auto&		GetAutoAimID() const noexcept { return GetGunPtrNow()->GetAutoAimID(); }
			const auto&		GetAutoAimPos() const noexcept { return GetGunPtrNow()->GetAutoAimPos(); }
			void			OverrideAutoAimID(PlayerID ID, int pos) const noexcept { GetGunPtrNow()->OverrideAutoAimID(ID, pos); }

			void			SetHeadShot(void) noexcept { this->m_LeanControl.SetHeadShot(); }
			const auto		PopHeadShotSwitch() noexcept {
				auto ret = this->m_HeadShotSwitch;
				this->m_HeadShotSwitch = false;
				return ret;
			}
			bool			IsWearingArmor() const noexcept { return this->m_WearArmorFlag != InvalidID; }
		private: //更新関連
			void			UpdateInput(void) noexcept;
			void			UpdateMatrix(void) noexcept;
		public: //コンストラクタ、デストラクタ
			CharacterObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Human); }
			virtual ~CharacterObj(void) noexcept {}
		public:
			static void		LoadChara(const std::string& FolderName, PlayerID ID) noexcept;
			void			LoadCharaGun(const std::string& FolderName, int Select) noexcept;
			void			SetupRagDoll(const MV1& baseModel) noexcept {
				this->m_RagDollControl.SetRagDoll().Duplicate(baseModel);
				MV1::SetAnime(&this->m_RagDollControl.SetRagDoll(), GetObj());
				this->m_RagDollControl.Init(GetObj());
			}
			void			Spawn(float pxRad, float pyRad, const Vector3DX& pPos, int GunSelect, bool CheckGround) noexcept;

			const auto		GetIsRappelling() const noexcept { return this->m_IsRappelling && !this->m_IsRappellingEnd; }
			void			SetRappelling(void) noexcept {
				this->m_IsRappelling = true;
				this->m_IsRappellingEnd = false;
			}
			void			Input(const InputControl& pInput) noexcept {
				this->m_Input = pInput;
				float debuff = GetDebuff();
				this->m_Input.SetAddxRad(this->m_Input.GetAddxRad() * debuff);
				this->m_Input.SetAddyRad(this->m_Input.GetAddyRad() * debuff);
			}
		private:
			int				GetFrameNum(void) noexcept override { return static_cast<int>(CharaFrame::Max); }
			const char*		GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }

			int				GetShapeNum(void) noexcept override { return static_cast<int>(CharaShape::Max); }
			const char*		GetShapeStr(int id) noexcept override { return CharaShapeName[id]; }
		private: //継承
			void			Init_Sub(void) noexcept override;
			void			FirstUpdate(void) noexcept override;
			void			CheckDraw_Sub(int) noexcept override;
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override;
			void			DrawShadow(void) noexcept override;
			void			Dispose_Sub(void) noexcept override {
				this->m_RagDollControl.Dispose();
				this->m_GunPtrControl.Dispose();
			}
		};
	}
}
