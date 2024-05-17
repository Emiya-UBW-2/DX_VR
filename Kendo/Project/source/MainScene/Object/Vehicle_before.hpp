#pragma once
#include	"../../Header.hpp"
#include "../../sub.hpp"

#include "VehicleData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		class LifeControl {
		private://キャラパラメーター
			HitPoint											m_HPMax = 100;
			HitPoint											m_HP{0};							//スコア
			std::vector<DamageEvent>							m_DamageEvent;						//ダメージイベント
		protected:
			void			SetHealEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint value) noexcept {
				this->m_DamageEvent.resize(this->m_DamageEvent.size() + 1);
				this->m_DamageEvent.back().SetEvent(ShotID_t, DamageID_t, -value);
			}
			void			SetSubHPEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint value) noexcept {
				this->m_DamageEvent.resize(this->m_DamageEvent.size() + 1);
				this->m_DamageEvent.back().SetEvent(ShotID_t, DamageID_t, value);
			}
		public://ゲッター
			const auto		IsAlive(void) const noexcept { return this->m_HP != 0; }
			const auto&		GetHP(void) const noexcept { return this->m_HP; }
			const auto&		GetHPMax(void) const noexcept { return this->m_HPMax; }
			void			SubHP(HitPoint damage_t) noexcept { this->m_HP = std::clamp<HitPoint>(this->m_HP - damage_t, 0, GetHPMax()); }

			const auto		IsLowHP(void) const noexcept { return GetHP() < (GetHPMax() * 35 / 100); }

			auto&			GetDamageEvent(void) noexcept { return this->m_DamageEvent; }
		public:
			void		InitLife(HitPoint Max) {
				this->m_HPMax = Max;
				this->m_HP = GetHPMax();
			}
		};

		//キャラ入力
		class KeyControl {
		private://キャラパラメーター
			InputControl										m_Input;
			Vector3DX											m_rad_Buf;

			bool												m_PosBufOverRideFlag{false};
			moves												m_OverRideInfo;
		public://ゲッター
			const auto		GetRadBuf(void) const noexcept { return this->m_rad_Buf; }
			const auto		GetOverRideInfo(void) const noexcept { return this->m_OverRideInfo; }
		protected:
			const auto		GetInputControl(void) const noexcept { return this->m_Input; }
		public:
			void		InitOverride() { this->m_PosBufOverRideFlag = false; }
			void		InitKey(const Vector3DX& pRad) {
				this->m_Input.ResetAllInput();
				this->m_rad_Buf = pRad;
			}
			//
			void		SetPosBufOverRide(const moves& o) noexcept {
				this->m_PosBufOverRideFlag = true;
				this->m_OverRideInfo = o;
			}
			bool		PutOverride() {
				if (this->m_PosBufOverRideFlag) {
					this->m_PosBufOverRideFlag = false;
					return true;
				}
				return false;
			}
			void		InputKey(const InputControl& pInput, bool pReady) {
				this->m_Input = pInput;
				if (!pReady) {
					this->m_Input.ResetKeyInput();
				}
				//回転
				{
					auto y_mouse = std::atan2f(this->m_rad_Buf.x, this->m_rad_Buf.z) + pInput.GetAddyRad();
					auto x_mouse = std::clamp(std::atan2f(-this->m_rad_Buf.y, std::hypotf(this->m_rad_Buf.x, this->m_rad_Buf.z)) + pInput.GetAddxRad(), -deg2rad(40.f), deg2rad(40.f));
					this->m_rad_Buf = (Matrix4x4DX::RotAxis(Vector3DX::right(), x_mouse) * Matrix4x4DX::RotAxis(Vector3DX::up(), y_mouse)).zvec();
				}
			}
		};

		//砲
		class Guns {
		private:
			float							m_loadtimer{0};			//装てんカウンター
			bool							m_reloadSEFlag{true};		//
			float							m_Recoil{0.f};			//駐退
			float							m_React{0.f};				//反動
			int													m_VehDataID{0};
			int													m_GunID{0};
			Vector3DX						m_ShotRadAdd;				//
			Vector3DX						m_view_rad;										//
		public:			//getter
			const auto*	GetData(void) const noexcept {
				return &VehDataControl::Instance()->GetVehData()[this->m_VehDataID].Get_gunframe().at(this->m_GunID);
			}
			const auto&	Getloadtime(void) const noexcept { return m_loadtimer; }
			const auto&	GetGunViewVec(void) const noexcept { return m_view_rad; }
			const auto&	GetFireReactionVec(void) const noexcept { return m_ShotRadAdd; }
			const auto&	GetCaliberSize(void) const noexcept { return GetData()->GetAmmoSpec()[0]->GetCaliber(); }
			const auto&	GetTotalloadtime(void) const noexcept { return GetData()->GetLoadTime(); }
			const auto&	GetGunBase(void) const noexcept { return GetData()->Get_frame(0); }
			const auto&	GetGunTrunnion(void) const noexcept { return GetData()->Get_frame(1); }
			const auto&	GetGunMuzzle(void) const noexcept { return GetData()->Get_frame(2); }
			const auto&	GetRecoilPer(void) const noexcept { return this->m_Recoil; }
		public:
		public: //コンストラクタ、デストラクタ
			Guns(void) noexcept {}
			Guns(const Guns&) noexcept {}
		public:
			void		Init(int VehID,int GunID) noexcept {
				this->m_VehDataID = VehID;
				this->m_GunID = GunID;
				this->m_ShotRadAdd.Set(0, 0, 0);
			}
			//銃反動
			void		UpdateGunVec(float pGunXradAdd, float pGunYradAdd) noexcept {
				this->m_view_rad.x = std::clamp(this->m_view_rad.x + pGunXradAdd,
												deg2rad(this->GetData()->GetDownRadLimit()),
												deg2rad(this->GetData()->GetUpRadLimit()));
				this->m_view_rad.y += pGunYradAdd;
			}
			bool		Update(bool key, PlayerID ID, const Vector3DX& pos_t, const Vector3DX& nomal_t) noexcept;
			void		UpdateFireReaction_(float pGunYrad) noexcept {
				Easing(&this->m_ShotRadAdd, Matrix4x4DX::RotAxis(Vector3DX::up(), pGunYrad).xvec() * -1.f * deg2rad(-this->m_React * GetCaliberSize() * 50.f), 0.85f, EasingType::OutExpo);
			}
			void		Dispose(void) noexcept {
				this->m_loadtimer = 0.f;
				this->m_reloadSEFlag = true;
				this->m_Recoil = 0.f;
				this->m_React = 0.f;
				this->m_ShotRadAdd.Set(0, 0, 0);
			}
		};
		//弾痕
		class HIT_ACTIVE {
			struct Hit {
				bool		m_IsActive{false};	//弾痕フラグ
				int			m_UseFrame{0};		//使用フレーム
				MV1			m_obj;					//弾痕モデル
				moves		m_move;					//座標
				int			m_FrameID{0};			//影響を受ける車輛側のフレーム
			};
			std::array<Hit, 24> hit_obj;			//弾痕
			int hitbuf = 0;							//使用弾痕
		public:
			void			Set(int FrameNum, const MV1& pObj, const  Vector3DX& Put_pos, const  Vector3DX& Put_normal, const Vector3DX& ammo_nomal/*this->move.vec.normalized()*/, const float&caliber, bool isPene) noexcept {
				auto AmmoSize = 200.f*caliber;
				auto YVec = Matrix4x4DX::Vtrans(Put_normal, pObj.GetMatrix().rotation().inverse());
				auto ZVec = Matrix4x4DX::Vtrans(Vector3DX::Cross(Put_normal, ammo_nomal).normalized(), pObj.GetMatrix().rotation().inverse());
				auto scale = Vector3DX::vget(AmmoSize / std::abs(Vector3DX::Dot(ammo_nomal, Put_normal)), AmmoSize, AmmoSize);

				auto matrix_tmp = pObj.GetFrameLocalWorldMatrix(FrameNum);
				auto rot_tmp = matrix_tmp.rotation();
				auto pos_tmp = matrix_tmp.pos();

				this->hit_obj[this->hitbuf].m_UseFrame = (isPene) ? 0 : 1;				//弾痕
				this->hit_obj[this->hitbuf].m_move.mat = Matrix4x4DX::GetScale(scale) * Matrix4x4DX::Axis1(YVec, ZVec);
				this->hit_obj[this->hitbuf].m_move.pos = Matrix4x4DX::Vtrans((Put_pos + YVec * 0.05f) - pos_tmp, rot_tmp.inverse());
				this->hit_obj[this->hitbuf].m_IsActive = true;
				this->hit_obj[this->hitbuf].m_FrameID = FrameNum;
				++this->hitbuf %= this->hit_obj.size();
			}
		public:
			void			Init(const MV1& hit_pic) noexcept {
				for (auto& h : this->hit_obj) {
					h.m_IsActive = false;
					h.m_obj = hit_pic.Duplicate();
				}
			}
			void			Execute(const MV1& pObj) noexcept {
				//弾痕
				for (auto& h : this->hit_obj) {
					if (h.m_IsActive) {
						auto matrixL_tmp = pObj.GetFrameLocalMatrix(h.m_FrameID);
						auto matrix_tmp = pObj.GetFrameLocalWorldMatrix(h.m_FrameID);
						auto rot_tmp = matrix_tmp.rotation();
						auto pos_tmp = matrix_tmp.pos();
						h.m_obj.SetMatrix(h.m_move.mat * matrixL_tmp.rotation() * rot_tmp * Matrix4x4DX::Mtrans(pos_tmp + Matrix4x4DX::Vtrans(h.m_move.pos, rot_tmp)));
					}
				}
			}
			void			Draw(void) noexcept {
				for (auto& h : this->hit_obj) {
					if (h.m_IsActive) {
						h.m_obj.DrawFrame(h.m_UseFrame);
					}
				}
			}
			void			Dispose(void) noexcept {
				for (auto& h : this->hit_obj) {
					h.m_IsActive = false;
					h.m_obj.Dispose();
					h.m_UseFrame = 0;
					h.m_move.mat = Matrix4x4DX::identity();
					h.m_move.pos.Set(0, 0, 0);
				}
			}
		};
		//命中関連
		class HitSortInfo {
			size_t					m_hitmesh{SIZE_MAX};
			float					m_hitDistance{-1};
		public:
			const auto		operator<(const HitSortInfo& tgt) const noexcept { return this->m_hitDistance < tgt.m_hitDistance; }
			void			Set(size_t f_t, float s_t = (std::numeric_limits<float>::max)())noexcept {
				this->m_hitmesh = f_t;
				this->m_hitDistance = s_t;
			}
			const auto		GetHitMesh(void) const noexcept { return this->m_hitmesh; }
			const auto		IsHit(void) const noexcept { return (this->m_hitDistance != (std::numeric_limits<float>::max)()); }
		};

		class cat_frame {
		private:
			frames					m_frame;
			float					m_Ypos{0};
			float					m_YposR{0};
			EffectControl::EffectS	m_gndsmkeffcs;
			float					m_gndsmksize{1.f};
		public:
			cat_frame(void) noexcept {}
			//~cat_frame(void) noexcept {}
		public://getter
			const auto&		GetFrame(void) const noexcept { return this->m_frame; }
			const auto&		GetYPosR(void) const noexcept { return this->m_YposR; }
			const auto&		GetYPos(void) const noexcept { return this->m_Ypos; }
			void			SetYpos(float pYpos) noexcept { this->m_Ypos = pYpos; }
		public:
			void			Init(const frames& pFrame) noexcept {
				this->m_frame = pFrame;
				this->m_Ypos = 0.f;
				this->m_YposR = 0.f;
				this->m_gndsmkeffcs.SetLoop(EffectResource::Instance()->effsorce.back(), Vector3DX::vget(0, -1, 0), Vector3DX::vget(0, 0, 1), 0.1f*Scale_Rate);
				this->m_gndsmksize = 0.1f;
			}
			//
			void			Update(MV1* pTargetObj, float pSpd) noexcept {
				Easing(&this->m_YposR, m_Ypos, 0.9f, EasingType::OutExpo);
				Easing(&this->m_gndsmksize, 0.2f + std::abs(pSpd) * 5.5f, 0.975f, EasingType::OutExpo);
				this->m_gndsmkeffcs.SetEffectPos(pTargetObj->frame(this->m_frame.GetFrameID()) + pTargetObj->GetMatrix().yvec() * (-this->m_frame.GetFrameWorldPosition().pos().y));
				this->m_gndsmkeffcs.SetEffectScale(std::clamp(this->m_gndsmksize, 0.2f, 1.5f)*Scale_Rate);
			}
			//
			void			Dispose(void) noexcept {
				this->m_gndsmkeffcs.Dispose();
			}
		};
		class wheel_frame {
		private:
			frames					m_frame;
			float					m_Ypos{0};
			float					m_YposR{0};
		public:
			wheel_frame(void) noexcept {}
			//~wheel_frame(void) noexcept {}
		public://getter
			const auto&		GetFrame(void) const noexcept { return this->m_frame; }
			const auto&		GetYPosR(void) const noexcept { return this->m_YposR; }
			const auto&		GetYPos(void) const noexcept { return this->m_Ypos; }
			void			SetYpos(float pYpos) noexcept { this->m_Ypos = pYpos; }
		public:
			void			Init(const frames& pFrame) noexcept {
				this->m_frame = pFrame;
				this->m_Ypos = 0.f;
				this->m_YposR = 0.f;
			}
			void			Update() noexcept {
				Easing(&this->m_YposR, m_Ypos, 0.9f, EasingType::OutExpo);
			}
		};
		//履帯BOX2D
		class FootWorld {
		private:
		private:
			std::shared_ptr<b2World>	m_FootWorld{nullptr};		//足world
			std::vector<b2Pats>			m_Foot, m_Wheel, m_Yudo;	//
		public:
			void			Init(bool IsLeft, const VehDataControl::VhehicleData* pUseVeh, MV1* pTargetObj) noexcept {
				this->m_FootWorld = std::make_unique<b2World>(b2Vec2(0.0f, 0.f));
				float LorR = ((IsLeft) ? 1.f : -1.f);
				//履帯
				this->m_Foot.clear();
				{
					b2BodyDef bd;
					b2Body* BodyB = this->m_FootWorld->CreateBody(&bd);
					b2EdgeShape shape;
					shape.Set(b2Vec2(-40.0f, -10.0f), b2Vec2(40.0f, -10.0f));
					BodyB->CreateFixture(&shape, 0.0f);
					b2Body* BodyA = BodyB;
					b2Vec2 anchor;
					for (const auto& w : pUseVeh->Get_b2upsideframe(IsLeft ? 0 : 1)) {
						anchor = b2Vec2(w.GetFrameWorldPosition().pos().z, w.GetFrameWorldPosition().pos().y);
						if (w.GetFrameWorldPosition().pos().x * LorR > 0) {
							this->m_Foot.resize(this->m_Foot.size() + 1);
							b2PolygonShape f_dynamicBox; //ダイナミックボディに別のボックスシェイプを定義
							f_dynamicBox.SetAsBox(0.2f*Scale_Rate, 0.05f*Scale_Rate);
							this->m_Foot.back().Set(CreateB2Body(this->m_FootWorld, b2_dynamicBody, anchor.x, anchor.y), &f_dynamicBox);
							b2RevoluteJointDef m_jointDef; m_jointDef.Initialize(BodyA, this->m_Foot.back().Get(), anchor);
							this->m_FootWorld->CreateJoint(&m_jointDef);
							BodyA = this->m_Foot.back().Get();
						}
					}
					if (!this->m_Foot.empty()) {
						b2RevoluteJointDef m_jointDef; m_jointDef.Initialize(BodyA, BodyB, anchor);
						this->m_FootWorld->CreateJoint(&m_jointDef);
					}
				}
				//転輪
				this->m_Wheel.clear();
				this->m_Yudo.clear();
				if (!this->m_Foot.empty()) {
					//転輪(動く)
					for (const auto& w : pUseVeh->Get_wheelframe()) {
						Vector3DX vects = pTargetObj->GetFrameLocalMatrix(w.GetFrameID()).pos();
						if (vects.x * LorR > 0) {
							this->m_Wheel.resize(this->m_Wheel.size() + 1);
							b2CircleShape shape;
							shape.m_radius = vects.y - 0.1f;
							this->m_Wheel.back().Set(CreateB2Body(this->m_FootWorld, b2_kinematicBody, vects.z, vects.y), &shape);
						}
					}
					//誘導輪(動かない)
					if (pUseVeh->Get_wheelframe_nospring().size() > 4) {
						for (const auto& w : pUseVeh->Get_wheelframe_nospring()) {
							Vector3DX vects = pTargetObj->GetFrameLocalMatrix(w.GetFrameID()).pos();
							if (vects.x * LorR > 0) {
								this->m_Yudo.resize(this->m_Yudo.size() + 1);
								b2CircleShape shape;
								shape.m_radius = 0.05f*Scale_Rate;
								this->m_Yudo.back().Set(CreateB2Body(this->m_FootWorld, b2_kinematicBody, vects.z, vects.y), &shape);
							}
						}
					}
				}
			}
			//
			void			LateExecute(bool IsLeft, const VehDataControl::VhehicleData* pUseVeh, MV1* pTargetObj, const b2Vec2& pGravity) noexcept {
				int LR_t = (IsLeft ? 1 : -1);
				if (this->m_Foot.size() != 0) {
					auto Wheel_t = this->m_Wheel.begin();
					for (const auto& w : pUseVeh->Get_wheelframe()) {
						Vector3DX vects = pTargetObj->GetFrameLocalMatrix(w.GetFrameID()).pos();
						if (vects.x * LR_t > 0) {
							Wheel_t->SetTransform(b2Vec2(vects.z, vects.y), 0.f);
							Wheel_t++;
						}
					}
					size_t i = 0;
					if (pUseVeh->Get_wheelframe_nospring().size() > 4) {
						for (const auto& w : pUseVeh->Get_wheelframe_nospring()) {
							Vector3DX vects = pTargetObj->GetFrameLocalMatrix(w.GetFrameID()).pos();
							if (vects.x * LR_t > 0) {
								this->m_Yudo[i++].SetTransform(b2Vec2(vects.z, vects.y), 0.f);
							}
						}
					}
					for (auto& t : this->m_Foot) {
						t.SetLinearVelocity(pGravity); //
					}

					this->m_FootWorld->Step(0.1f, 1, 1);

					for (auto& t : this->m_Foot) {
						t.pos = Vector3DX::vget(t.pos.x, t.Pos().y, t.Pos().x);
					}
				}
				{
					size_t i = 0;
					for (const auto& w : pUseVeh->Get_b2upsideframe((IsLeft) ? 0 : 1)) {
						if (w.GetFrameWorldPosition().pos().x * LR_t > 0) {
							auto& t = this->m_Foot[i++];
							t.pos = Vector3DX::vget(w.GetFrameWorldPosition().pos().x, t.pos.y, t.pos.z);
							pTargetObj->SetFrameLocalMatrix(w.GetFrameID(), Matrix4x4DX::Mtrans(t.pos));
						}
					}
				}
			}
			//
			void			Dispose(void) noexcept {
				for (auto& t : this->m_Foot) { t.Dispose(); }
				this->m_Foot.clear();
				for (auto& t : this->m_Wheel) { t.Dispose(); }
				this->m_Wheel.clear();
				for (auto& t : this->m_Yudo) { t.Dispose(); }
				this->m_Yudo.clear();
				this->m_FootWorld.reset();
			}
		};
	};
};
