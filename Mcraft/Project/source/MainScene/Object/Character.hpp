#pragma once
#include	"../../Header.hpp"
#include "CharacterEnum.hpp"
#include "Character_before.hpp"
#include "CharaAnimData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		class CharacterClass : public ObjectBaseClass, public EffectControl {
		private:
			KeyControl				m_KeyControl;
			LaserSightClass			m_LaserSightClass;
			HitBoxControl			m_HitBoxControl;
			WalkSwingControl		m_WalkSwingControl;
			EyeSwingControl			m_EyeSwingControl;
			HitReactionControl		m_HitReactionControl;
			RagDollControl			m_RagDollControl;
			GunPtrControl			m_GunPtrControl;
			AutoAimControl			m_AutoAimControl;

			EnumGunReadySeq										m_GunReadySeq{ EnumGunReadySeq::LowReady };
			float												m_ADSPer{ 0.f };
			PointControl<HitPoint, 100>							m_HP{};
			PointControl<ArmerPoint, 100>						m_AP{};
			DamageEventControl									m_Damage;
			bool												m_ActionFirstFrame{ false };
			CharaActionID										m_CharaAction{ CharaActionID::Ready };
			std::array<ArmMovePerClass, (int)EnumGunAnimType::Max>	m_Arm;
			bool												m_MagHand{ false };
			ArmMovePerClass										m_MagArm;
			float												m_StuckGunTimer{ 0.f };
			bool												m_IsStuckGun{ false };
			bool												m_IsChanging{ false };
			bool												m_IsChange{ false };
			float												m_HPRec{ 0.f };
			bool												m_ArmBreak{ false };
			switchs												m_SightChange;
			int													m_CharaSound{ -1 };			//サウンド
			Vector3DX											m_RecoilRadAdd;
			Pendulum2D											m_SlingArmZrad;
			float												m_ArmBreakPer{};
			Pendulum2D											m_SlingZrad;
			Matrix4x4DX											m_MagMiss{}, m_MagSuccess{};
			FallControl											m_Grenade;
			std::shared_ptr<AmmoDataClass>						m_ChamberAmmoData{ nullptr };		//
			bool												m_MoveOverRideFlag{ false };
			moves												m_OverRideInfo;
			PlayerID											m_MyID{ 0 };
			CharaTypeID											m_CharaType{};
		private:
			Matrix3x3DX											m_CharaRotationCache{};
			Matrix3x3DX											m_EyeRotationCache{};
			Vector3DX											m_EyePositionCache{};
		public:
			bool												CanLookTarget{ true };
			Vector3DX											m_CameraPos;
			float												m_Length{};
		public:
			//外にあぶれていた項目
			auto&			GetRagDoll(void) noexcept { return m_RagDollControl.GetRagDoll(); }
			const auto&		GetLeanRad(void) const noexcept { return m_KeyControl.GetLeanRad(); }
			const auto&		GetGunPtrNow(void) const noexcept { return m_GunPtrControl.GetGunPtrNow(); }
			auto&			GetGunPtr(int ID) noexcept { return m_GunPtrControl.GetGunPtr(ID); }
			const auto		GetSightZoomSize() const noexcept { return m_GunPtrControl.GetSightZoomSize(); }
			const auto&		GetAutoAimID(void) const noexcept { return m_AutoAimControl.GetAutoAimID(); }
			const auto		GetAutoAimActive(void) const noexcept { return m_AutoAimControl.GetAutoAimActive(); }
			const auto		GetIsADS(void) const noexcept { return this->m_GunReadySeq == EnumGunReadySeq::ADS; }
			const auto&		GetADSPer(void) const noexcept { return this->m_ADSPer; }
			void			SetAim(void) noexcept { this->m_GunReadySeq = EnumGunReadySeq::Aim; }

			void			LoadCharaGun(const std::string& FolderName, int Sel) noexcept;
			static void		LoadChara(const std::string& FolderName, PlayerID ID) noexcept;
		public://ゲッター
			const auto		IsAlive(void) const noexcept { return m_HP.IsNotZero(); }
			const auto		IsLowHP(void) const noexcept { return this->m_HP.GetPoint() < (this->m_HP.GetMax() * 35 / 100); }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			auto			GetFrameWorldMat(CharaFrame frame) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(frame))); }
			auto			GetFrameLocalMat(CharaFrame frame) const noexcept { return GetObj_const().GetFrameLocalMatrix(GetFrame(static_cast<int>(frame))); }
			const auto&		GetDamageEvent(void) const noexcept { return m_Damage; }
			const Matrix3x3DX GetEyeRotation(void) const noexcept;
			const Vector3DX GetEyePosition(void) const noexcept;
			const auto&		GetEyeRotationCache(void) const noexcept { return this->m_EyeRotationCache; }
			const auto&		GetEyePositionCache(void) const noexcept { return this->m_EyePositionCache; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetCharaAction(void) const noexcept { return this->m_CharaAction; }
			const auto		GetRecoilRadAdd(void) const noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				return this->m_RecoilRadAdd * (60.f * DXLib_refParts->GetDeltaTime());
			}
		public://セッター
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
			void			SetMoveOverRide(const moves& o) noexcept {
				this->m_MoveOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
			void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept { m_Damage.AddDamageEvent(pRet); }
			bool			SetDamageEvent(const DamageEvent& value) noexcept;
			void			Heal(HitPoint value, bool SwitchOn) noexcept {
				m_Damage.Add(GetMyPlayerID(), GetMyPlayerID(), -value, -value, Vector3DX::up());
				if (SwitchOn) {
					m_ArmBreak = false;
				}
			}
			const bool		CheckDamageRay(HitPoint* Damage, ArmerPoint* ArmerDamage, bool CheckBodyParts, PlayerID AttackID, const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept;
		private:
			const auto		GetCharaDir(void) const noexcept { return m_KeyControl.GetCharaLocalDir() * this->GetMove().GetMat(); }
			void			Shot_Start() noexcept;
			void			Reload_Start() noexcept;
		private: //更新関連
			void			ExecuteInput(void) noexcept;
			void			ExecuteSound(void) noexcept;
			void			ExecuteMatrix(void) noexcept;
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = (int)ObjType::Human;
				this->m_IsDraw = true;
			}
			~CharacterClass(void) noexcept {}
		public:
			void			ValueSet(PlayerID pID, CharaTypeID value) noexcept;
			void			MovePoint(float pxRad, float pyRad, const Vector3DX& pPos, int GunSel) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
			void			SetupRagDoll(void) noexcept {
				MV1::SetAnime(&m_RagDollControl.GetRagDoll(), GetObj());
				m_RagDollControl.Init(GetObj());
			}
		private:
			int				GetFrameNum() noexcept override { return (int)CharaFrame::Max; }
			const char*		GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }

			int				GetShapeNum() noexcept override { return (int)CharaShape::Max; }
			const char*		GetShapeStr(int id) noexcept override { return CharaShapeName[id]; }
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
				EffectControl::Dispose();
				m_GunPtrControl.DisposeGunPtr();
				m_ChamberAmmoData.reset();
			}
		};
	};
};
