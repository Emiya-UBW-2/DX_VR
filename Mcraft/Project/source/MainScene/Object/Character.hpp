#pragma once
#include	"../../Header.hpp"
#include	"CharacterEnum.hpp"
#include	"Character_before.hpp"
#include	"CharaAnimData.hpp"

namespace FPS_n2 {
	namespace Charas {

		class CharacterObj : public BaseObject {
		private:
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
			bool												m_IsSquat{ false };
			std::array<float, static_cast<int>(CharaAnimeID::AnimeIDMax)>	m_AnimPerBuf{ 0 };
			CharaAnimeID										m_BottomAnimSelect{};
			PointControl<HitPoint, 100>							m_HP{};
			PointControl<ArmerPoint, 100>						m_AP{};
			DamageEventControl									m_Damage;
			float												m_StuckGunTimer{ 0.f };
			bool												m_IsStuckGun{ false };
			float												m_HPRec{ 0.f };
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
			float												m_Concussion{ 0.f };
			float												m_ConcussionPer{ 0.f };
		private://キャッシュ
			Matrix3x3DX											m_EyeRotationCache{};
			Vector3DX											m_EyePositionCache{};
			bool												m_IsActiveCameraPos{ false };
			ScreenPosition										m_CameraPos;
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			const auto		GetSpeed(void) const noexcept {
				if (this->m_IsSquat) {
					if (this->m_Input.GetPADSPress(Controls::PADS::WALK)) {
						return 0.15f;
					}
					return 0.475f;
				}
				else {
					if (this->m_Input.GetPADSPress(Controls::PADS::WALK)) {
						return 0.2f;
					}
					return 0.625f;
				}
			}
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
		public://ゲッター
			const auto&		GetMoveEyePos(void) const noexcept { return this->m_WalkSwingControl.GetMoveEyePos(); }
			const auto&		GetRagDoll(void) const noexcept { return this->m_RagDollControl.GetRagDoll(); }
			const auto&		GetLeanRad(void) const noexcept { return this->m_LeanControl.GetRad(); }
			const auto&		GetRotateRad(void) const noexcept { return this->m_RotateControl.GetRad(); }
			const auto&		GetHitBoxList(void) const noexcept { return this->m_HitBoxControl.GetHitBoxPointList(); }
			const auto&		GetGunPtr(int ID) const noexcept { return this->m_GunPtrControl.GetGunPtr(ID); }
			const auto&		GetGunPtrNow(void) const noexcept { return GetGunPtr(this->m_GunPtrControl.GetNowGunSelect()); }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_Damage; }
			const auto&		GetEyeRotationCache(void) const noexcept { return this->m_EyeRotationCache; }
			const auto&		GetEyePositionCache(void) const noexcept { return this->m_EyePositionCache; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto		GetIsADS(void) const noexcept { return (!this->m_IsStuckGun && GetGunPtrNow() && GetGunPtrNow()->GetCanADS()) && this->m_Input.GetPADSPress(Controls::PADS::AIM); }
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
			void			SetDamage(PlayerID DamageID_t, HitPoint Damage, ArmerPoint ArmerDamage, int HitType, const Vector3DX& StartPos, const Vector3DX& EndPos) noexcept {
				this->m_Damage.Add(GetMyPlayerID(), DamageID_t, Damage, ArmerDamage, HitType, StartPos, EndPos);
			}
			void			Heal(HitPoint Point) noexcept { SetDamage(GetMyPlayerID(), -Point, -Point, static_cast<int>(HitType::Body), GetMove().GetPos(), GetMove().GetPos()); }
			auto&			SetRagDoll(void) noexcept { return this->m_RagDollControl.SetRagDoll(); }
			bool			SetDamageEvent(const DamageEvent& Event) noexcept;
			const bool		CheckDamageRay(HitPoint Damage, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;

			const auto&		GetAutoAimID() const noexcept { return GetGunPtrNow()->GetAutoAimID(); }
			const auto&		GetAutoAimPos() const noexcept { return GetGunPtrNow()->GetAutoAimPos(); }
			void			OverrideAutoAimID(PlayerID ID, int pos) const noexcept { GetGunPtrNow()->OverrideAutoAimID(ID, pos); }
		private: //更新関連
			void			UpdateInput(void) noexcept;
			void			UpdateMatrix(void) noexcept;
		public: //コンストラクタ、デストラクタ
			CharacterObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::Human); }
			virtual ~CharacterObj(void) noexcept {}
		public:
			static void		LoadChara(const std::string& FolderName, PlayerID ID) noexcept;
			void			LoadCharaGun(const std::string& FolderName, int Select) noexcept;
			void			SetupRagDoll(void) noexcept {
				MV1::SetAnime(&SetRagDoll(), GetObj());
				this->m_RagDollControl.Init(GetObj());
			}
			void			Spawn(float pxRad, float pyRad, const Vector3DX& pPos, int GunSelect, bool CheckGround) noexcept {
				this->m_HP.Init();
				this->m_AP.Init();
				Heal(100);
				this->m_ArmBreak = false;
				this->m_ArmBreakPer = 0.f;
				this->m_SlingArmZrad.Init(0.08f * Scale3DRate, 3.f, deg2rad(50));
				this->m_HPRec = 0.f;

				this->m_MoveOverRideFlag = false;
				this->m_Input.ResetAllInput();
				this->m_RotateControl.Init(pxRad, pyRad);
				this->m_MoveControl.Init();
				this->m_LeanControl.Init();
				for (auto& per : this->m_AnimPerBuf) { per = 0.f; }
				this->m_IsSquat = false;
				Vector3DX pos_t = pPos;
				if (CheckGround) {
					Vector3DX EndPos = pos_t - Vector3DX::up() * 200.f * Scale3DRate;
					if (BackGround::BackGroundControl::Instance()->CheckLinetoMap(pos_t + Vector3DX::up() * 200.f * Scale3DRate, &EndPos)) {
						pos_t = EndPos;
					}
				}
				SetMove().SetAll(pos_t, pos_t, pos_t, Vector3DX::zero(), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetRad().y), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetRad().y));
				//
				this->m_GunPtrControl.SelectGun(GunSelect);
				for (int loop = 0, max = this->m_GunPtrControl.GetGunNum(); loop < max; ++loop) {
					if (!this->m_GunPtrControl.GetGunPtr(loop)) { continue; }
					this->m_GunPtrControl.GetGunPtr(loop)->Spawn();
				}
				this->m_SlingZrad.Init(0.05f * Scale3DRate, 3.f, deg2rad(50));
			}

			const auto& GetIsRappelling() const noexcept { return this->m_IsRappelling && !this->m_IsRappellingEnd; }
			void			SetRappelling(void) noexcept {
				this->m_IsRappelling = true;
				this->m_IsRappellingEnd = false;
			}
			void			Input(const InputControl& pInput) noexcept { this->m_Input = pInput; }
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
	};
};
