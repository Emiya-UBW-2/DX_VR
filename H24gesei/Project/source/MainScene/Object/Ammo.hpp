#pragma once
#include	"../../Header.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace CharacterObject {
		class AmmoClass : public ObjectBaseClass {
		private:
			HitPoint		m_Damage{ 0 };
			float			m_Caliber{ 0.f };
			float			m_speed{ 0.f };

			bool			m_IsHit{ false };
			bool			m_IsDrawHitUI{ false };
			float			m_HitTimer{ 0.f };
			int				m_RicochetCnt{ 0 };
			float			m_Hit_alpha{ 0.f };
			Vector3DX		m_Hit_DispPos;
			PlayerID		m_ShootCheraID{ -1 };
			std::array<Vector3DX, 60> m_Line;
			int				m_LineSel = 0;
			float			m_yAdd{ 0.f };
			float			m_Timer{ 0.f };

		private:
			PlayerID											m_MyID{ 0 };
		public:
			const auto& GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
		public://getter
			const auto& GetShootedID(void) const noexcept { return this->m_ShootCheraID; }
			const auto& GetDamage(void) const noexcept { return this->m_Damage; }
			const auto& GetCaliberSize(void) const noexcept { return this->m_Caliber; }
			const auto		GetEffectSize(void) const noexcept { return ((GetCaliberSize() >= 0.020f) ? GetCaliberSize() : 0.025f) / 0.1f; }
		public:
			void			Put(HitPoint Damage, float Cal, const Vector3DX& pos_t, const Vector3DX& pVec, PlayerID pMyID) {
				this->m_Damage = Damage;
				this->m_Caliber = Cal;//0.00762f
				this->m_speed = 600.f * Scale_Rate;

				this->m_move.Init(pos_t, pVec, Matrix3x3DX::identity());
				this->m_ShootCheraID = pMyID;
				for (auto& l : this->m_Line) { l = this->m_move.GetPos(); }

				SetActive(true);
				this->m_RicochetCnt = 0;
				this->m_IsHit = false;
				this->m_IsDrawHitUI = false;
				this->m_HitTimer = 0.f;
				this->m_yAdd = 0.f;
				this->m_Timer = 0.f;
			}
			void			Draw_Hit_UI(GraphHandle& Hit_Graph) {
				auto* DrawParts = DXDraw::Instance();
				if (this->m_IsDrawHitUI) {
					if ((this->m_Hit_alpha >= 10.f / 255.f) && (this->m_Hit_DispPos.z >= 0.f && this->m_Hit_DispPos.z <= 1.f)) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f * this->m_Hit_alpha));
						Hit_Graph.DrawRotaGraph((int)this->m_Hit_DispPos.x, (int)this->m_Hit_DispPos.y, (float)DrawParts->GetUIY(static_cast<int>(this->m_Hit_alpha * 0.5f * 100.0f)) / 100.f, 0.f, true);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
			}
			void			Penetrate(void) noexcept {
				SetActive(false);
				this->m_IsHit = true;
				this->m_IsDrawHitUI = true;
			}
			void			HitGround(const Vector3DX& position) {
				SetActive(false);
				this->m_move.SetPos(position);
				this->m_IsHit = true;
				this->m_IsDrawHitUI = false;
			}
		public: //コンストラクタ、デストラクタ
			AmmoClass(void) noexcept { this->m_objType = (int)ObjType::Ammo; }
			~AmmoClass(void) noexcept {}
		public: //継承
			void			Init_Sub() noexcept override {
				SetActive(false);
			}
			//
			void			FirstExecute(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				if (this->m_IsHit) {
					this->m_IsHit = false;
					this->m_HitTimer = 0.25f;
					this->m_Hit_alpha = 1.f;
				}
				this->m_HitTimer = std::clamp(this->m_HitTimer - 1.f / DrawParts->GetFps(), 0.f, 0.25f);
				if (this->m_Hit_alpha > 0.f) {
					Easing(&this->m_Hit_alpha, (this->m_HitTimer > 0.f) ? 2.f : 0.f, 0.95f, EasingType::OutExpo);
					if (this->m_Hit_alpha <= 0.01f) {
						this->m_Hit_alpha = 0;
						SetDelete();
					}
				}
				if (IsActive()) {
					//移動確定
					this->m_move.SetPos(this->m_move.GetPos() + (this->m_move.GetVec() * (this->m_speed / DrawParts->GetFps())) + Vector3DX::up() * this->m_yAdd);
					this->m_move.Update(0.f, 0.f);
					//this->m_yAdd += (M_GR / (DrawParts->GetFps()*DrawParts->GetFps()));

					this->m_Line[this->m_LineSel] = this->m_move.GetPos() + Vector3DX::vget(GetRandf(Scale_Rate * 0.1f * this->m_Timer), GetRandf(Scale_Rate * 0.1f * this->m_Timer), GetRandf(Scale_Rate * 0.1f * this->m_Timer));
					++this->m_LineSel %= this->m_Line.size();

					//消す(スピードが0以下、貫通が0以下、5回反射する)
					if (this->m_speed <= 0.f || this->m_Timer > 3.f) {
						SetActive(false);
					}
					//this->m_speed -= 5.f / DrawParts->GetFps();
					//this->m_penetration -= 5.f / DrawParts->GetFps();
				}
				this->m_Timer += 1.f / DrawParts->GetFps();
			}
			//
			void			DrawShadow(void) noexcept override {}
			void			CheckDraw(void) noexcept override {
				auto tmp = ConvWorldPosToScreenPos(this->m_move.GetPos().get());
				if (tmp.z >= 0.f && tmp.z <= 1.f) {
					this->m_Hit_DispPos = tmp;
					auto* DrawParts = DXDraw::Instance();
					this->m_Hit_DispPos *= static_cast<float>(DrawParts->GetUIY(1080)) / static_cast<float>(DrawParts->GetScreenY(1080));
				}
				if (!IsActive()) {
					return;
				}
				this->m_IsDraw = false;
				if (CheckCameraViewClip_Box(this->m_move.GetRePos().get(), this->m_move.GetPos().get()) == FALSE) {
					this->m_IsDraw |= true;
				}
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (!isDrawSemiTrans) { return; }
				auto* DrawParts = DXDraw::Instance();

				SetUseLightAngleAttenuation(FALSE);
				if (IsActive() && this->m_IsDraw) {
					{
						float per = 0.5f;
						if (!IsActive()) {
							per = std::clamp(this->m_Hit_alpha, 0.f, 0.5f);
						}
						int max = (int)this->m_Line.size();
						for (int i = 1; i < max; i++) {
							int LS = (i + this->m_LineSel);
							auto p1 = (LS - 1) % max;
							auto p2 = LS % max;
							float DistanceToCam = (this->m_Line[p2] - DrawParts->GetMainCamera().GetCamPos()).magnitude();
							if (DistanceToCam < 300.f * Scale_Rate) {
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * per * ((float)(i) / max)), 0, 255));
								DrawCapsule3D(this->m_Line[p1].get(), this->m_Line[p2].get(), GetCaliberSize() * Scale_Rate * 2.f * ((float)(i) / max), 3, GetColor(192, 192, 192), GetColor(192, 192, 192), TRUE);
							}
						}
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}
					DrawCapsule_3D(this->m_move.GetPos(), this->m_move.GetRePos(), ((GetCaliberSize() - 0.00762f) * 0.1f + 0.00762f) * Scale_Rate, GetColor(255, 255, 172), GetColor(255, 255, 255));
				}
				SetUseLightAngleAttenuation(TRUE);
			}
		};
	};
};
