#pragma once
#include	"../../Header.hpp"
#include "../../MainScene/Object/VehicleData.hpp"
#include "../../MainScene/Object/Vehicle_before.hpp"
#include "../../MainScene/Object/Ammo.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//戦車
		class VehicleClass : public ObjectBaseClass, public EffectControl,
			public KeyControl,
			public LifeControl
		{
		private:
			MV1													m_hit_pic;						//弾痕  
			//操作
			float												m_AimingDistance{ 100.f*Scale_Rate };			//
			//プレイヤー周り
			CharaTypeID											m_CharaType{ CharaTypeID::Enemy };				//
			//車体
			int													m_VehDataID{0};
			std::vector<Guns>									m_Gun;											//

			int													DIV{5};

			//
			float												m_speedBase{0.f};								//移動速度
			float												m_speed{ 0.f };									//移動速度
			float												m_speed_add{ 0.f };								//
			float												m_speed_sub{ 0.f };								//
			Vector3DX											m_radAdd;										//
			float												m_yradadd_left{ 0.f };							//
			float												m_yradadd_right{ 0.f };							//
			Vector3DX											m_BodyNormal{Vector3DX::up()};				//
			Vector3DX											m_Tilt;											//

			float												m_engine_time{ 0.f };							//エンジン音声
			bool												m_engine_start{ false };						//エンジン音声
			int													m_enginemoveID{ -1 };							//エンジン音声
			HIT_ACTIVE											m_Hit_active;									//
			std::vector<HitSortInfo> hitssort;																	//フレームに当たった順番
			std::vector<MV1_COLL_RESULT_POLY> hitres;															//確保
			//転輪
			float												m_wheel_Left{ 0.f };							//転輪回転
			float												m_wheel_Right{ 0.f };							//転輪回転
			std::array<std::vector<wheel_frame>, 2>				m_Wheel;							//転輪のY方向保持
			std::array<std::vector<cat_frame>, 2>				m_CatFoot;										//履帯BOX2D
			std::array<FootWorld, 2>							m_b2Foot;										//履帯BOX2D
			std::shared_ptr<BackGroundClass>					m_BackGround;				//BG
			//ダメージ
			float												m_Fuel{ 100.f };
			std::vector<HitPoint>								m_HP_parts;
			//box2d
			b2Pats												m_b2mine;										//BOX2D
		private:
			PlayerID											m_MyID{0};
		public:
			bool												CanLookTarget{true};
		public:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
		public:
			void			SetMapCol(const std::shared_ptr<BackGroundClass>& backGround) { this->m_BackGround = backGround; }
		public:			//getter
			const auto&		GetData(void) const noexcept { return VehDataControl::Instance()->GetVehData()[this->m_VehDataID]; }

			const auto&		Get_HP_parts(void) const noexcept { return this->m_HP_parts; }
			const auto&		GetFuel(void) const noexcept { return this->m_Fuel; }
			const auto&		GetFuelMax(void) const noexcept { return GetData().GetMaxFuel(); }

			const auto&		GetSpeed(void) const noexcept { return this->m_speed; }

			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto		Get_body_yrad(void) const noexcept { auto pp = this->m_move.mat.zvec()*-1.f; return std::atan2f(pp.x, pp.z); }
			const auto&		GetRadAdd(void) const noexcept { return this->m_radAdd; }

			const auto&		Gunloadtime(size_t id_t) const noexcept { return this->m_Gun[id_t].Getloadtime(); }
			const auto&		GetTotalloadtime(size_t id_t) const noexcept { return this->m_Gun[id_t].GetTotalloadtime(); }
			const auto&		GetGun(void) const noexcept { return this->m_Gun; }
			const auto		Get_Gunsize(void) const noexcept { return this->m_Gun.size(); }
			const auto		GetGunMuzzleMatrix(int ID) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(this->m_Gun[ID].GetGunTrunnion().GetFrameID()).rotation(); }
			const auto		GetGunMuzzleBase(int ID) const noexcept { return GetObj_const().frame(this->m_Gun[ID].GetGunTrunnion().GetFrameID()); }
			const auto		GetGunMuzzlePos(int ID) const noexcept { return GetObj_const().frame(this->m_Gun[ID].GetGunMuzzle().GetFrameID()); }
			const auto		GetGunMuzzleVec(int ID) const noexcept { return (GetGunMuzzlePos(ID) - GetGunMuzzleBase(ID)).normalized(); }
			const auto		Get_EyePos_Base(void) const noexcept { return this->m_move.pos + this->m_move.mat.yvec() * (3.f * Scale_Rate); }
			const auto&		GetViewRad(void) const noexcept { return this->m_Gun[0].GetGunViewVec(); }
			const auto		GetAimPoint(void) const noexcept { return GetGunMuzzlePos(0) + GetGunMuzzleVec(0) * this->m_AimingDistance; }
			const auto&		GetAimingDistance(void) const noexcept { return this->m_AimingDistance; }
		public:			//setter
			const bool		SetDamageEvent(const DamageEvent& value) noexcept;
			void			ClashParts(int ID) noexcept { this->m_HP_parts[ID] = 0; }
			void			RepairParts(int ID) noexcept { this->m_HP_parts[ID] = GetHPMax() / 2; }
			void			SetAimingDistance(float value) noexcept { this->m_AimingDistance = value; }
			void			SubHP_Parts(HitPoint damage_t, int parts_Set_t) noexcept;
		public:
			void			ValueInit(int VhehID, const std::shared_ptr<b2World>& pB2World, PlayerID pID) noexcept;
			void			MovePoint(float pxRad, float pyRad, const Vector3DX& pos_t) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady, bool isOverrideView = false) noexcept;													//
			const std::pair<bool, bool>		CheckAmmoHit(AmmoClass* pAmmo) noexcept;
			void			HitGround(const Vector3DX& pos_t, const Vector3DX& pNorm, const Vector3DX& pVec) noexcept;
			void			DrawModuleView(int xp, int yp, int size) noexcept;																					//被弾チェック

			const bool		CheckLine(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) noexcept;

		private://更新関連
			const auto		CheckAmmoHited(const AmmoClass& pAmmo) noexcept;																					//被弾チェック
			const auto		CalcAmmoHited(AmmoClass* pAmmo) noexcept;															//被弾処理

			void			ExecuteSavePrev(void) noexcept;																										//以前の状態保持
			void			ExecuteElse(void) noexcept;																											//その他
			void			ExecuteInput(void) noexcept;																										//操作
			void			ExecuteFrame(void) noexcept;																										//フレーム操作
			void			ExecuteMove(void) noexcept;																											//移動操作
			void			ExecuteMatrix(void) noexcept;																										//SetMat指示更新
		public: //コンストラクタ、デストラクタ
			VehicleClass(void) noexcept { this->m_objType = (int)ObjType::Vehicle; }
			~VehicleClass(void) noexcept {}
		public: //継承
			void			Init_Sub(void) noexcept override {
			}
			void			FirstExecute(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				auto* SE = SoundPool::Instance();
				//初回のみ更新する内容
				if (this->m_IsFirstLoop) {
					this->m_engine_start = true;
					this->m_engine_time = (float)GetRand(100) / 100.f;
					this->m_enginemoveID = SE->Get((int)SoundEnum::Tank_move).Play_3D(0, this->m_move.pos, 30.f*Scale_Rate, 255, DX_PLAYTYPE_LOOP);
				}
				ExecuteSavePrev();			//以前の状態保持
				ExecuteElse();				//その他
				ExecuteInput();				//操作
				ExecuteFrame();				//フレーム操作
				ExecuteMove();				//移動操作
				if (IsAlive()) {
					this->m_Fuel -= 1.f / DrawParts->GetFps() * (0.6f + (std::abs(this->m_move.vec.magnitude() / Scale_Rate) * 0.75f + std::abs(this->m_radAdd.y) * 8.f)*3.5f);
				}
			}
			void			LateExecute(void) noexcept override {
				ExecuteMatrix();			//SetMat指示//0.03ms
				EffectControl::Execute();
			}
			void			DrawShadow(void) noexcept override {
				GetObj().DrawModel();
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				if (!(this->m_IsActive && this->m_IsDraw)) {
					if (GetMyPlayerID() == 0) {
						for (int i = 3; i < GetObj().mesh_num(); i++) {
							GetObj().DrawMesh(i);
						}
					}
					return;
				}
				if (isDrawSemiTrans) { return; }
				if ((CheckCameraViewClip_Box(
					(this->GetMove().pos + Vector3DX::vget(-5, 0, -5)*Scale_Rate).get(),
					(this->GetMove().pos + Vector3DX::vget(5, 5, 5)*Scale_Rate).get()) == FALSE) ||
					 CanLookTarget
					) {
					int fog_enable;
					int fog_mode;
					int fog_r, fog_g, fog_b;
					float fog_start, fog_end;
					float fog_density;

					fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
					fog_mode = GetFogMode();														// フォグモードを取得する
					GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
					GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
					fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )

					SetFogEnable(TRUE);
					SetFogColor(64, 64, 64);
					SetFogStartEnd(Scale_Rate*50.f, Scale_Rate*300.f);

					//this->m_col.DrawModel();
					//this->m_Hit_active.Draw();
					if (true) {
						if (this->m_HP_parts[GetData().Get_module_mesh()[0]] > 0) {
							MV1SetFrameTextureAddressTransform(GetObj().get(), 0, 0.f, -this->m_wheel_Left * 0.1f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
							GetObj().DrawMesh(0);
						}
						if (this->m_HP_parts[GetData().Get_module_mesh()[1]] > 0) {
							MV1SetFrameTextureAddressTransform(GetObj().get(), 0, 0.f, -this->m_wheel_Right * 0.1f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
							GetObj().DrawMesh(1);
						}
						MV1ResetFrameTextureAddressTransform(GetObj().get(), 0);
						GetObj().DrawMesh(2);
						if (GetMyPlayerID() == 0) {
							for (int i = 2; i < GetObj().mesh_num(); i++) {
								GetObj().DrawMesh(i);
							}
						}
						else {
							GetObj().DrawMesh(2);
							GetObj().DrawMesh(3);
						}
						//this->m_col.DrawModel();
						this->m_Hit_active.Draw();
					}
					else {
						GetObj().DrawModel();
					}

					SetFogEnable(fog_enable);
					SetFogMode(fog_mode);
					SetFogColor(fog_r, fog_g, fog_b);
					SetFogStartEnd(fog_start, fog_end);
					SetFogDensity(fog_density);
				}
			}
			void			Dispose_Sub(void) noexcept override {
				EffectControl::Dispose();
				for (auto& f : this->m_CatFoot) {
					for (auto& t : f) {
						t.Dispose();
					}
					f.clear();
				}
				for (auto& f : this->m_b2Foot) {
					f.Dispose();
				}
				//this->m_b2mine.Dispose();
				for (auto& f : this->m_Wheel) {
					f.clear();
				}
				for (auto& cg : this->m_Gun) {
					cg.Dispose();
				}
				this->m_Gun.clear();
				this->m_Hit_active.Dispose();
				this->m_HP_parts.clear();
				this->hitres.clear();
				this->hitssort.clear();
			}
		};
	};
};
