#pragma once
#include	"../../Header.hpp"
#include "CharacterEnum.hpp"
#include "Character_before.hpp"
#include "CharaAnimData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		class CharacterClass : public ObjectBaseClass {
		private:
			HitBoxControl										m_HitBoxControl;
			WalkSwingControl									m_WalkSwingControl;
			HitReactionControl									m_HitReactionControl;
			RagDollControl										m_RagDollControl;
			GunPtrControl										m_GunPtrControl;
			LeanControl											m_LeanControl{};
			MoveControl											m_MoveControl{};
			RotateControl										m_RotateControl{};

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
			int													m_CharaSound{ -1 };			//サウンド
			Pendulum2D											m_SlingArmZrad;
			float												m_ArmBreakPer{};
			Pendulum2D											m_SlingZrad;
			bool												m_MoveOverRideFlag{ false };
			moves												m_OverRideInfo;
			PlayerID											m_MyID{ 0 };
			CharaTypeID											m_CharaType{};
			bool												m_CanLookTarget{ true };
			float												m_Length{};
			bool												m_ConcussionSwitch{};
		private://キャッシュ
			Matrix3x3DX											m_EyeRotationCache{};
			Vector3DX											m_EyePositionCache{};
			bool												m_IsActiveCameraPos{ false };
			ScreenPosition										m_CameraPos;
		public:
		public://プレイヤーキャラから見た際の情報
			const auto&		GetCanLookByPlayer(void) const noexcept { return this->m_CanLookTarget; }
			const auto&		GetLengthToPlayer(void) const noexcept { return this->m_Length; }
			const auto&		GetIsActiveCameraPosToPlayer(void) const noexcept { return this->m_IsActiveCameraPos; }
			const auto&		GetCameraPosToPlayer(void) const noexcept { return this->m_CameraPos; }
		public://ゲッター
			const auto&		GetMoveEyePos(void) const noexcept { return m_WalkSwingControl.GetMoveEyePos(); }
			const auto&		GetRagDoll(void) const noexcept { return this->m_RagDollControl.GetRagDoll(); }
			const auto&		GetLeanRad(void) const noexcept { return this->m_LeanControl.GetRad(); }
			const auto&		GetRotRad(void) const noexcept { return this->m_RotateControl.GetRad(); }
			const auto&		GetHitBoxList(void) const noexcept { return this->m_HitBoxControl.GetHitBoxPointList(); }
			const auto&		GetGunPtrNow(void) const noexcept { return this->m_GunPtrControl.GetGunPtr(this->m_GunPtrControl.GetNowGunSelect()); }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_Damage; }
			const auto&		GetEyeRotationCache(void) const noexcept { return this->m_EyeRotationCache; }
			const auto&		GetEyePositionCache(void) const noexcept { return this->m_EyePositionCache; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto		GetSpeed(void) const noexcept {
				if (this->m_IsSquat) {
					if (this->m_Input.GetPADSPress(Controls::PADS::WALK)) {
						return 0.65f;
					}
					return 0.95f;
				}
				else {
					if (this->m_Input.GetPADSPress(Controls::PADS::WALK)) {
						return 0.95f;
					}
					return 1.25f;
				}
			}
			const auto		IsMoveFront(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_W) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_S); }
			const auto		IsMoveBack(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_S) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_W); }
			const auto		IsMoveLeft(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_A) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_D); }
			const auto		IsMoveRight(void) const noexcept { return this->m_Input.GetPADSPress(Controls::PADS::MOVE_D) && !this->m_Input.GetPADSPress(Controls::PADS::MOVE_A); }
			const auto		GetBottomStandAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat : CharaAnimeID::Bottom_Stand; }
			const auto		GetBottomWalkAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_Walk : CharaAnimeID::Bottom_Stand_Walk; }
			const auto		GetBottomWalkBackAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_WalkBack : CharaAnimeID::Bottom_Stand_WalkBack; }
			const auto		GetBottomLeftStepAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_LeftStep : CharaAnimeID::Bottom_Stand_LeftStep; }
			const auto		GetBottomRightStepAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_RightStep : CharaAnimeID::Bottom_Stand_RightStep; }
			const auto		GetBottomTurnAnimSel(void) const noexcept { return this->m_IsSquat ? CharaAnimeID::Bottom_Squat_Turn : CharaAnimeID::Bottom_Stand_Turn; }
			const auto		GetIsADS(void) const noexcept { return (!this->m_IsStuckGun && GetGunPtrNow() && GetGunPtrNow()->GetCanADS()) && this->m_Input.GetPADSPress(Controls::PADS::AIM); }
			const auto		IsAlive(void) const noexcept { return this->m_HP.IsNotZero(); }
			const auto		IsLowHP(void) const noexcept { return this->m_HP.GetPoint() < (this->m_HP.GetMax() * 35 / 100); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return GetObj().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return GetObj().GetFrameLocalMatrix(GetFrame(static_cast<int>(frame))); }
		public://セッター
			void			SetPlayerID(PlayerID value) noexcept {
				this->m_MyID = value;
				//銃のIDセットアップ
				for (int loop = 0; loop < this->m_GunPtrControl.GetGunNum(); ++loop) {
					if (this->m_GunPtrControl.GetGunPtr(loop)) {
						this->m_GunPtrControl.GetGunPtr(loop)->SetPlayerID(GetMyPlayerID());
					}
				}
			}
			void			SetConcussionSwitch() noexcept {
				this->m_ConcussionSwitch = true;
			}
			bool			PopConcussionSwitch() noexcept {
				if (this->m_ConcussionSwitch) {
					this->m_ConcussionSwitch = false;
					return true;
				}
				return false;
			}
			void			SetCharaTypeID(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			SetMoveOverRide(const moves& o) noexcept {
				this->m_MoveOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
			void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept { this->m_Damage.AddDamageEvent(pRet); }
			void			Heal(HitPoint value) noexcept { this->m_Damage.Add(GetMyPlayerID(), GetMyPlayerID(), -value, -value,static_cast<int>(HitType::Body), GetMove().GetPos(), GetMove().GetPos()); }
			auto&			SetRagDoll(void) noexcept { return this->m_RagDollControl.SetRagDoll(); }
			bool			SetDamageEvent(const DamageEvent& value) noexcept;
			const bool		CheckDamageRay(HitPoint Damage, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteMatrix(void) noexcept;
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = static_cast<int>(ObjType::Human);
			}
			~CharacterClass(void) noexcept {}
		public:
			static void		LoadChara(const std::string& FolderName, PlayerID ID) noexcept;
			void			LoadCharaGun(const std::string& FolderName, int Sel) noexcept;
			void			SetupRagDoll(void) noexcept {
				MV1::SetAnime(&SetRagDoll(), GetObj());
				this->m_RagDollControl.Init(GetObj());
			}
			void			Spawn(float pxRad, float pyRad, const Vector3DX& pPos, int GunSel) noexcept {
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
				for (auto& a : this->m_AnimPerBuf) { a = 0.f; }
				this->m_IsSquat = false;
				SetMove().SetAll(pPos, pPos, pPos, Vector3DX::zero(), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetRad().y), Matrix3x3DX::RotAxis(Vector3DX::up(), this->m_RotateControl.GetRad().y));
				//
				this->m_GunPtrControl.SelectGun(GunSel);
				for (int loop = 0; loop < this->m_GunPtrControl.GetGunNum(); ++loop) {
					if (!this->m_GunPtrControl.GetGunPtr(loop)) { continue; }
					this->m_GunPtrControl.GetGunPtr(loop)->Spawn();
				}
				this->m_SlingZrad.Init(0.05f * Scale3DRate, 3.f, deg2rad(50));
			}
			void			Input(const InputControl& pInput) noexcept { this->m_Input = pInput; }
		private:
			int				GetFrameNum() noexcept override { return static_cast<int>(CharaFrame::Max); }
			const char*		GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }

			int				GetShapeNum() noexcept override { return static_cast<int>(CharaShape::Max); }
			const char*		GetShapeStr(int id) noexcept override { return CharaShapeName[id]; }
		private: //継承
			void			Init_Sub(void) noexcept override;
			void			FirstExecute(void) noexcept override;
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
