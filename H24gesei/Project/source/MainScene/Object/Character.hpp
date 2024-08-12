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

			HitPoint											m_HP{ 1000 };

			struct TurretData
			{
				frames Rotate;
				frames Elevate;
				float MinusLimit{ -50.f };
				float Rad{ 0.f };
				float PlusLimit{ 50.f };

				float RotateRate{ 1.f };

				float AmmoSize{ 12.7f };
				float LoadTime{ 1.f };

				float NowRad{ 0.f };
				float NowRadR{ 0.f };
				float NowLoadTime{ 0.f };

				bool CanShot{ false };
				bool HitLimit{ false };
			};

			std::vector<TurretData>								m_TurretData{};
			float												m_ShotCoolDown{ 0.f };

			MV1													m_WaveObj{};
			float												m_WaveScr{};
			float												m_WaveAlpha{};
			float												m_WaveFront{};

			float												m_AimDistance{};
		public:
			bool												CanLookTarget{ true };
		public://ゲッター
			const auto& GetTurretData(void) const noexcept { return this->m_TurretData; }
			const auto& GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			const auto& GetDamageEvent(void) const noexcept { return this->m_Damage; }
			const auto& GetEyePosition(void) const noexcept { return this->m_EyePosition; }
			const auto& GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto& GetBambooVec(void) const noexcept { return m_BambooVec; }
			const auto& GetGuardVec(void) const noexcept { return m_GuardVecR; }
			const auto& GetGuardCoolDownTimer(void) const noexcept { return m_GuardCoolDownTimer; }
			const auto GetGuardCoolDownTimerMax(void) const noexcept { return 2.f; }
			auto			PopConcussionSwitch(void) noexcept {
				auto Prev = m_ConcussionSwitch;
				m_ConcussionSwitch = false;
				return Prev;
			}
			Vector3DX		GetFramePosition(CharaFrame frame) const noexcept { return GetObj_const().GetFramePosition(GetFrame(static_cast<int>(frame))); }
			const auto&		GetHP(void) const noexcept { return m_HP; }
			HitPoint		GetHPMax(void) noexcept {
				return 1000;
			}
			auto			GetCamTarget() noexcept {
				return (GetMove().GetPos() +Vector3DX::up()*(10.f*Scale_Rate) - CharaMove::GetGunMatrix().zvec() * (m_AimDistance * Scale_Rate));
			}
		public://セッター
			void			ValueSet(PlayerID pID, CharaTypeID value) noexcept {
				this->m_CharaType = value;
				this->m_MyID = pID;
				m_HP = GetHPMax();
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

			int				GetMaterialNum(void) noexcept override { return static_cast<int>(CharaMaterial::Max); }
			const char* GetMaterialStr(int id) noexcept override { return CharaMaterialName[id]; }

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
				if (this->GetMove().GetPos().y < -50.f * Scale_Rate) { return; }
				this->m_IsDraw = false;
				this->m_DistanceToCam = (this->GetMove().GetPos() - GetScreenPosition()).magnitude();
				if (CheckCameraViewClip_Box(
					(this->GetMove().GetPos() + Vector3DX::vget(-800.f * Scale_Rate, -0.f * Scale_Rate, -800.f * Scale_Rate)).get(),
					(this->GetMove().GetPos() + Vector3DX::vget(800.f * Scale_Rate, 370.f * Scale_Rate, 800.f * Scale_Rate)).get()) == FALSE
					) {
					this->m_IsDraw |= true;
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				float DistanceToCam = (this->GetMove().GetPos() - DrawParts->GetMainCamera().GetCamPos()).magnitude();
				if (this->GetMove().GetPos().y < -50.f * Scale_Rate) { return; }
				if (IsActive() && this->m_IsDraw) {
					if (m_MyID == m_ViewID) {
						if (isDrawSemiTrans) {
							this->m_WaveObj.DrawModel();
							for (int i = 0, num = this->GetObj().GetMaterialNum(); i < num; ++i) {
								if (i != GetMaterial(static_cast<int>(CharaMaterial::Body))) {
									if (m_HP == 0) {
										continue;
									}
									if (DistanceToCam > 150.f * Scale_Rate) {
										continue;
									}
								}
								this->GetObj().DrawMesh(i);
							}

							SetUseLighting(FALSE);
							for (auto& t : m_TurretData) {
								Vector3DX Pos = GetObj().GetFramePosition(t.Elevate.GetFrameID());
								Vector3DX Vec = GetObj().GetFrameLocalWorldMatrix(t.Rotate.GetFrameID()).zvec() * (-30.f * Scale_Rate);
								Vec = Matrix3x3DX::Vtrans(Vec, Matrix3x3DX::RotAxis(Vector3DX::up(), t.Rad));
								unsigned int Color = GetColor(255, 255, 0);
								if (t.CanShot) {
									Color = GetColor(0, 255, 0);
								}
								else if (t.HitLimit) {
									Color = GetColor(255, 0, 0);
								}
								if (t.NowLoadTime > 0.f) {
									Color = GetColor(255, 0, 0);
								}

								DrawCapsule3D((Pos).get(), (Pos + Vec).get(), 0.2f * Scale_Rate, 8, Color, Color, TRUE);
							}
							SetUseLighting(TRUE);

							//DrawCapsule3D(
							//	(GetMove().GetPos() + Matrix3x3DX::Vtrans(Vector3DX::vget(0.f, 10.0f * Scale_Rate, -100.0f * Scale_Rate), GetBaseRotMatrix())).get(),
							//	(GetMove().GetPos() + Matrix3x3DX::Vtrans(Vector3DX::vget(0.f, 10.0f * Scale_Rate, 100.0f * Scale_Rate), GetBaseRotMatrix())).get(), 10.f * Scale_Rate, 8, White, White, TRUE);
				
						}
					}
					else {
						if (!isDrawSemiTrans) {
							this->m_WaveObj.DrawModel();
							for (int i = 0, num = this->GetObj().GetMaterialNum(); i < num; ++i) {
								if (i != GetMaterial(static_cast<int>(CharaMaterial::Body))) {
									if (m_HP == 0) {
										continue;
									}
									if (DistanceToCam > 150.f * Scale_Rate) {
										continue;
									}
								}
								this->GetObj().DrawMesh(i);
							}
						}
					}
				}
			}
			void			DrawShadow(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				float DistanceToCam = (this->GetMove().GetPos() - DrawParts->GetMainCamera().GetCamPos()).magnitude();
				if (this->GetMove().GetPos().y < -50.f * Scale_Rate) { return; }
				if (IsActive()) {
					for (int i = 0, num = this->GetObj().GetMaterialNum(); i < num; ++i) {
						if (i != GetMaterial(static_cast<int>(CharaMaterial::Body))) {
							if (m_HP == 0) {
								continue;
							}
							if (DistanceToCam > 150.f * Scale_Rate) {
								continue;
							}
						}
						this->GetObj().DrawMesh(i);
					}
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
