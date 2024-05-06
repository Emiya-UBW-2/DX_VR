#pragma once
#include	"Vehicle.hpp"
#include "../../MainScene/Player/Player.hpp"
#include "../../MainScene/Object/Item.hpp"

#include "../../MainScene/Object/Ammo.hpp"
namespace FPS_n2 {
	namespace Sceneclass {
		//----------------------------------------------------------
		//初期化関連
		//----------------------------------------------------------
		void			VehicleClass::SubHP_Parts(HitPoint damage_t, int parts_Set_t) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();//todo:GetMyPlayerID()
			if (this->m_VecData->Get_module_mesh()[0] == parts_Set_t) {
				while (true) {
					const auto* Ptr = PlayerMngr->GetPlayer(this->GetMyPlayerID()).GetInventory(2, [&](const std::shared_ptr<CellItem>& tgt) { return tgt.get(); });
					if (Ptr) {
						if ((*Ptr)->Sub(&damage_t)) {
							PlayerMngr->GetPlayer(this->GetMyPlayerID()).DeleteInventory(*Ptr);
						}
						RepairParts(parts_Set_t);
					}
					else {
						ClashParts(parts_Set_t);
						break;
					}
					if (damage_t == 0) {
						break;
					}
				}
			}
			else if (this->m_VecData->Get_module_mesh()[1] == parts_Set_t) {
				while (true) {
					const auto* Ptr = PlayerMngr->GetPlayer(this->GetMyPlayerID()).GetInventory(3, [&](const std::shared_ptr<CellItem>& tgt) { return tgt.get(); });
					if (Ptr) {
						if ((*Ptr)->Sub(&damage_t)) {
							PlayerMngr->GetPlayer(this->GetMyPlayerID()).DeleteInventory(*Ptr);
						}
						RepairParts(parts_Set_t);
					}
					else {
						ClashParts(parts_Set_t);
						break;
					}
					if (damage_t == 0) {
						break;
					}
				}
			}
			else {
				this->m_HP_parts[parts_Set_t] = std::max<HitPoint>(this->m_HP_parts[parts_Set_t] - damage_t, 0);
			}
		}
		const bool		VehicleClass::SetDamageEvent(const DamageEvent& value) noexcept {
			if (this->m_MyID == value.ID && this->m_objType == (int)value.CharaType) {
				SubHP(value.Damage, value.rad);
				if (this->m_HP == 0) {
					EffectControl::SetOnce(EffectResource::Effect::ef_greexp2, this->m_move.pos, this->m_move.mat.zvec(), Scale_Rate*2.f);
					auto* ObjMngr = ObjectManager::Instance();
					auto* PlayerMngr = PlayerManager::Instance();//todo:GetMyPlayerID()

					for (auto& I : PlayerMngr->GetPlayer(this->m_MyID).GetInventorys()) {
						for (auto& xo : I) {
							for (auto& yo : xo) {
								if (yo.get() && (GetRand(100) < 33)) {
									auto Ptr = std::make_shared<ItemClass>();
									ObjMngr->AddObject(Ptr);
									ObjMngr->LoadModel(Ptr, Ptr, "data/model/item/");

									Ptr->SetMapCol(this->m_BackGround);
									Ptr->Init();

									auto Vec = (Matrix4x4DX::RotAxis(Vector3DX::right(), GetRandf(deg2rad(90)))*Matrix4x4DX::RotAxis(Vector3DX::up(), GetRandf(deg2rad(360)))).yvec()*Scale_Rate;
									Ptr->SetMove(
										Matrix4x4DX::RotVec2(Vector3DX::up(), Vec.normalized()),
										this->m_move.pos + Vec * 6.f,
										Vec*25.f / FPS * (1.0f*GetRandf(0.5f)));
									Ptr->SetData(yo->GetItemData(), yo->GetCount());
									PlayerMngr->GetPlayer(this->m_MyID).DeleteInventory(yo);
								}
							}
						}
					}

				}
				m_ShakePer = 1.f;
				return true;
			}
			return false;
		}
		//
		void			VehicleClass::ValueInit(const VehDataControl::VhehicleData* pVeh_data, const MV1& hit_pic, const std::shared_ptr<b2World>& pB2World, PlayerID pID) noexcept {
			this->m_MyID = pID;
			this->m_VecData = pVeh_data;
			this->m_Hit_active.Init(hit_pic);														//弾痕

			GetObj().material_AlphaTestAll(true, DX_CMP_GREATER, 128);								//本体
			this->hitres.resize(this->m_col.mesh_num());											//モジュールごとの当たり判定結果を確保
			this->hitssort.resize(this->m_col.mesh_num());											//モジュールごとの当たり判定順序を確保
			b2PolygonShape dynamicBox = this->m_VecData->GetDynamicBox();
			this->m_b2mine.Set(CreateB2Body((std::shared_ptr<b2World>&)pB2World, b2_dynamicBody, 0.f, 0.f, 0.f), &dynamicBox);	//シェイプをボディに追加
			for (auto& f : this->m_b2Foot) {
				f.Init(&f == &this->m_b2Foot.front(), this->m_VecData, &GetObj());
			}
			this->m_wheel_frameYpos.resize(this->m_VecData->Get_wheelframe().size());
			//砲
			this->m_Gun.resize(this->m_VecData->Get_gunframe().size());
			this->m_view_rad.resize(this->m_Gun.size());
			for (const auto& g : this->m_VecData->Get_gunframe()) {
				this->m_Gun[&g - &this->m_VecData->Get_gunframe().front()].Init(&g);
			}
		}
		//
		void			VehicleClass::ValueSet(float pxRad, float pyRad, const Vector3DX& pos_t) noexcept {
			this->m_move.mat = Matrix4x4DX::RotAxis(Vector3DX::right(), pxRad) * Matrix4x4DX::RotAxis(Vector3DX::up(), pyRad);
			this->m_move.pos = pos_t;
			this->m_move.vec.Set(0, 0, 0);
			for (auto& w : this->m_wheel_frameYpos) { w = 0.f; }
			//砲
			//ヒットポイント
			{
				this->m_HP = this->m_VecData->GetMaxHP();
				this->m_HP_parts.resize(this->m_col.mesh_num());
				for (auto& h : this->m_HP_parts) { h = this->m_VecData->GetMaxHP(); }//モジュール耐久
			}
			this->m_Fuel = (float)this->m_VecData->GetMaxFuel();

			//戦車スポーン
			this->m_b2mine.SetTransform(b2Vec2(this->m_move.pos.x, this->m_move.pos.z), Get_body_yrad());
			this->m_PosBufOverRideFlag = false;
			this->m_DamageSwitch = 0;
			this->m_DamageSwitchRec = this->m_DamageSwitch;

			this->m_MouseVec = this->m_move.mat.inverse();

			this->m_speed = 0.f;
			this->m_speed_add = 0.f;
			this->m_speed_sub = 0.f;
			this->m_wheel_Left = 0.f;
			this->m_wheel_Right = 0.f;
			this->m_Tilt.Set(0, 0, 0);
			this->m_radAdd.Set(0, 0, 0);
			this->m_yradadd_left = 0.f;
			this->m_yradadd_right = 0.f;
		}
		//
		void			VehicleClass::SetInput(const InputControl& pInput, bool pReady, bool isOverrideView) noexcept {
			this->m_ReadySwitch = (this->m_KeyActive != pReady);
			this->m_KeyActive = pReady;
			//エイム
			auto y_mouse = std::atan2f(this->m_MouseVec.zvec().x, this->m_MouseVec.zvec().z);
			auto x_mouse = std::atan2f(-this->m_MouseVec.zvec().y, std::hypotf(this->m_MouseVec.zvec().x, this->m_MouseVec.zvec().z));

			//y_mouse += 0.01f;
			this->m_MouseVec = Matrix4x4DX::RotAxis(Vector3DX::right(), std::clamp(x_mouse + pInput.GetAddxRad(), -deg2rad(40.f), deg2rad(40.f))) * Matrix4x4DX::RotAxis(Vector3DX::up(), y_mouse + pInput.GetAddyRad());

			Easing(&this->m_ShakePos, Vector3DX::vget(GetRandf(10.f*Scale_Rate*m_ShakePer), GetRandf(10.f*Scale_Rate*m_ShakePer), GetRandf(10.f*Scale_Rate*m_ShakePer)), 0.9f, EasingType::OutExpo);
			Easing(&this->m_ShakePer, 0.f, 0.9f, EasingType::OutExpo);
			//
			if (isOverrideView) {
				this->m_view_override = true;
				//this->m_view_rad[0].x = (pInput.GetxRad());
				//this->m_view_rad[0].y = (pInput.GetyRad());
			}
			this->m_key[0] = pInput.GetPADSPress(PADS::SHOT) && pReady && this->Get_alive();			//射撃
			this->m_key[1] = pInput.GetPADSPress(PADS::JUMP) && pReady && this->Get_alive();			//マシンガン
			this->m_key[2] = pInput.GetPADSPress(PADS::MOVE_W) && pReady && this->Get_alive() && (this->m_HP_parts[this->m_VecData->Get_module_mesh()[0]] > 0) && (this->m_HP_parts[this->m_VecData->Get_module_mesh()[1]] > 0);		//前進
			this->m_key[3] = pInput.GetPADSPress(PADS::MOVE_S) && pReady && this->Get_alive() && (this->m_HP_parts[this->m_VecData->Get_module_mesh()[0]] > 0) && (this->m_HP_parts[this->m_VecData->Get_module_mesh()[1]] > 0);		//後退
			this->m_key[4] = pInput.GetPADSPress(PADS::MOVE_D) && pReady && this->Get_alive() && ((this->m_HP_parts[this->m_VecData->Get_module_mesh()[0]] > 0) || (this->m_HP_parts[this->m_VecData->Get_module_mesh()[1]] > 0));		//右
			this->m_key[5] = pInput.GetPADSPress(PADS::MOVE_A) && pReady && this->Get_alive() && ((this->m_HP_parts[this->m_VecData->Get_module_mesh()[0]] > 0) || (this->m_HP_parts[this->m_VecData->Get_module_mesh()[1]] > 0));		//左
			this->m_key[6] = pInput.GetPADSPress(PADS::AIM) && pReady && this->Get_alive();			//左

			//this->m_key[6] = true;
		}
		//カメラ設定出力
		void			VehicleClass::Setcamera(Camera3DInfo& MainCamera_t, const float fov_base) noexcept {
			auto fov_t = MainCamera_t.GetCamFov();
			auto near_t = MainCamera_t.GetCamNear();
			auto far_t = MainCamera_t.GetCamFar();

			Vector3DX eyepos = Get_EyePos_Base();
			auto OLD = this->m_range;

			if (!this->Get_alive()) {
				ReSet_range();
			}

			if (is_ADS()) {
				this->m_ratio = std::clamp(this->m_ratio + float(GetMouseWheelRotVolWithCheck()) * 2.0f, 0.0f, 30.f);
				if (this->m_ratio == 0.f) {
					this->m_range = 0.f + this->m_range_change;
					this->m_range_r = this->m_range;
				}
				else {
					Easing(&fov_t, fov_base / this->m_ratio, 0.9f, EasingType::OutExpo);
				}
			}
			else {
				eyepos += this->m_MouseVec.zvec() * this->m_range_r*Scale_Rate;
				Vector3DX eyepos2 = eyepos + this->m_MouseVec.zvec() * -1.f * (this->m_range_r * Scale_Rate);
				if (this->m_BackGround->CheckLinetoMap(eyepos, &eyepos2, true, false)) { eyepos = eyepos2; }

				this->m_ratio = 2.0f;
				if (this->Get_alive()) {
					this->m_range = std::clamp(this->m_range - float(GetMouseWheelRotVolWithCheck()) * this->m_range_change, 0.f, 8.f);
				}
				Easing(&this->m_range_r, this->m_range, 0.8f, EasingType::OutExpo);

				Easing(&fov_t, fov_base, 0.9f, EasingType::OutExpo);
			}
			Vector3DX eyetgt = Get_EyePos_Base() + this->m_MouseVec.zvec() * -1.f * std::max(this->m_range_r*Scale_Rate, 1.f);

			this->m_changeview = ((this->m_range != OLD) && (this->m_range == 0.f || OLD == 0.f));

			MainCamera_t.SetCamPos(
				eyepos + m_ShakePos * (is_ADS() ? 0.05f : 1.f),
				eyetgt + m_ShakePos * (is_ADS() ? 0.f : 2.f),
				Lerp(this->m_move.mat.yvec(), Vector3DX::up(), std::clamp(this->m_range_r / 3.f, 0.f, 1.f))
			);

			Easing(&near_t, 1.f + 2.f*((is_ADS()) ? this->m_ratio : 2.f), 0.9f, EasingType::OutExpo);
			Easing(&far_t, std::max(this->m_AimingDistance, Scale_Rate * 50.f) + Scale_Rate * 50.f, 0.9f, EasingType::OutExpo);
			MainCamera_t.SetCamInfo(fov_t, near_t, far_t);
		}

		//----------------------------------------------------------
		//更新関連
		//----------------------------------------------------------

		//被弾チェック
		const auto		VehicleClass::CheckAmmoHited(const AmmoClass& pAmmo) noexcept {
			auto HitCheck_Tank = [&](int m, const Vector3DX& ray_repos, const MV1_COLL_RESULT_POLY& hitres) {
				this->hitres[m] = hitres;
				if (this->hitres[m].HitFlag == TRUE) {
					this->hitssort[m].Set(m, (ray_repos - this->hitres[m].HitPosition).magnitude());
					return true;
				}
				else {
					this->hitssort[m].Set(m);
					return false;
				}
			};
			bool is_Hit = false;
			//装甲(一番近い位置のものに限定する)
			int t = -1;
			MV1_COLL_RESULT_POLY colres; colres.HitFlag = FALSE;
			Vector3DX EndPos = pAmmo.GetMove().pos;
			for (const auto& m : this->m_VecData->Get_armer_mesh()) { HitCheck_Tank(m.first, pAmmo.GetMove().repos, colres); }//全リセット
			for (const auto& m : this->m_VecData->Get_armer_mesh()) {
				auto colres_t = GetColLine(pAmmo.GetMove().repos, EndPos, m.first);
				if (colres_t.HitFlag == TRUE) {
					t = m.first;
					colres = colres_t;
					EndPos = colres_t.HitPosition;
				}
			}
			//
			for (auto& m : this->m_VecData->Get_module_mesh()) {//モジュール
				is_Hit |= HitCheck_Tank(m, pAmmo.GetMove().repos, GetColLine(pAmmo.GetMove().repos, EndPos, m));
			}
			for (auto& m : this->m_VecData->Get_space_mesh()) {//空間装甲
				is_Hit |= HitCheck_Tank(m, pAmmo.GetMove().repos, GetColLine(pAmmo.GetMove().repos, EndPos, m));
			}
			//
			if (t != -1) {
				is_Hit |= HitCheck_Tank(t, pAmmo.GetMove().repos, colres);
			}
			return is_Hit;
		}
		//被弾処理
		const auto		VehicleClass::CalcAmmoHited(AmmoClass* pAmmo, const Vector3DX& pShooterPos) noexcept {
			auto* SE = SoundPool::Instance();
			std::sort(this->hitssort.begin(), this->hitssort.end(), [](const HitSortInfo& x, const HitSortInfo& y) { return x < y; });	//当たり判定を近い順にソート
			bool isDamage = false;
			//ダメージ面に届くまで判定
			for (const auto& tt : this->hitssort) {
				if (tt.IsHit()) {
					if (tt.GetHitMesh() >= this->hitres.size()) { continue; }
					Vector3DX HitPos = this->hitres[tt.GetHitMesh()].HitPosition;
					Vector3DX HitNormal = this->hitres[tt.GetHitMesh()].Normal;
					HitNormal = HitNormal.normalized();

					for (auto& mesh : this->m_VecData->Get_space_mesh()) {
						if (tt.GetHitMesh() == mesh) {
							//空間装甲に当たったのでモジュールに30ダメ
							EffectControl::SetOnce(EffectResource::Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate);
							this->SubHP_Parts(pAmmo->GetDamage(), (HitPoint)tt.GetHitMesh());
						}
					}
					for (auto& mesh : this->m_VecData->Get_module_mesh()) {
						if (tt.GetHitMesh() == mesh) {
							//モジュールに当たったのでモジュールに30ダメ
							EffectControl::SetOnce(EffectResource::Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate);
							this->SubHP_Parts(pAmmo->GetDamage(), (HitPoint)tt.GetHitMesh());
						}
					}
					//ダメージ面に当たった
					for (auto& mesh : this->m_VecData->Get_armer_mesh()) {
						if (tt.GetHitMesh() == mesh.first) {
							//弾がダメージ層に届いた
							if (pAmmo->PenetrationCheck(mesh.second, HitNormal) && this->Get_alive()) {						//ダメージ面に当たった時に装甲値に勝てるかどうか
								pAmmo->Penetrate();	//貫通
								SE->Get((int)SoundEnum::Tank_Damage).Play_3D(GetRand(1), HitPos, 100.f*Scale_Rate, 216);
								//ダメージ計算
								auto v1 = Matrix4x4DX::RotAxis(Vector3DX::up(), Get_body_yrad()).zvec();
								auto v2 = (pShooterPos - this->m_move.pos).normalized(); v2.y = (0);
								this->m_DamageEvent.SetEvent(this->m_MyID, (ObjType)this->m_objType, pAmmo->GetDamage(), std::atan2f(Vector3DX::Cross(v1, v2).y, Vector3DX::Dot(v1, v2)));
								++this->m_DamageSwitch;// %= 255;//
								this->SubHP_Parts(pAmmo->GetDamage(), (HitPoint)tt.GetHitMesh());
								isDamage = true;
							}
							else {
								pAmmo->Ricochet(HitPos, HitNormal);	//跳弾
								SE->Get((int)SoundEnum::Tank_Ricochet).Play_3D(GetRand(16), HitPos, 100.f*Scale_Rate, 216);
								m_ShakePer = 0.1f*pAmmo->GetCaliberSize() / 0.10f;
							}
							//エフェクトセット
							EffectControl::SetOnce(EffectResource::Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate*10.f);
							auto Frame0 = this->hitres[tt.GetHitMesh()].PosMaxWeightFrameIndex[0];
							this->m_Hit_active.Set(Frame0, GetObj(), HitPos, HitNormal, pAmmo->GetMove().vec.normalized(), pAmmo->GetCaliberSize()*Scale_Rate, !pAmmo->IsActive());
							break;
						}
					}
				}
				else {
					break;
				}
			}
			return isDamage;
		}

		//以前の状態保持
		void			VehicleClass::ExecuteSavePrev(void) noexcept {
			this->m_spd_rec = this->m_speed;
			this->m_ColActive = false;
		}
		//その他
		void			VehicleClass::ExecuteElse(void) noexcept {
			auto* SE = SoundPool::Instance();
			//エンジン音
			if (this->Get_alive()) {
				if (this->m_engine_time <= 0.f) {

					SE->Get((int)SoundEnum::Tank_engine).Play_3D(0, this->m_move.pos, 50.f*Scale_Rate, 64);//, DX_PLAYTYPE_LOOP
					this->m_engine_time = 0.9f;
					if (this->m_engine_start) {
						this->m_engine_start = false;
						SE->Get((int)SoundEnum::Tank_Start).Play_3D(0, this->m_move.pos, 50.f*Scale_Rate, 128);//, DX_PLAYTYPE_LOOP
					}
				}
				else {
					this->m_engine_time = std::max(this->m_engine_time - 1.f / FPS, 0.f);
				}
			}
			else {
				this->m_engine_start = true;
				this->m_engine_time = 0.f;
			}
			if (this->m_enginemoveID >= 0) {
				SE->Get((int)SoundEnum::Tank_move).SetPos(0, this->m_enginemoveID, this->m_move.pos);
				SE->Get((int)SoundEnum::Tank_move).SetVol_Local(0, this->m_enginemoveID,
					std::clamp((int)(255.f*(std::abs(this->m_move.vec.magnitude() / Scale_Rate) * 0.75f + std::abs(this->m_radAdd.y) * 8.f)*10.f), 0, 255)
				);
			}
		}
		//操作
		void			VehicleClass::ExecuteInput(void) noexcept {
			if (!this->m_view_override) {
				float	view_YradAdd{ 0.f };							//
				float	view_XradAdd{ 0.f };							//
				for (int i = 0; i < this->m_view_rad.size(); i++) {
					if (this->Get_alive()) {
						if (this->m_key[6]) { //砲塔ロック
							view_XradAdd = 0.f;
							view_YradAdd = 0.f;
						}
						else {
							//狙い
							Vector3DX MuzPos = GetGunMuzzlePos(i);
							Vector3DX EndPos = MuzPos + (this->m_MouseVec.zvec() * -1.f).normalized() * (500.f*Scale_Rate);
							this->m_BackGround->CheckLinetoMap(MuzPos, &EndPos, true, false);

							Vector3DX BasePos = GetGunMuzzleBase(i);
							//反映
							auto vec_a = (EndPos - BasePos).normalized();
							auto vec_z = (MuzPos - BasePos).normalized();
							float a_hyp = std::hypotf(vec_a.x, vec_a.z);
							float z_hyp = std::hypotf(vec_z.x, vec_z.z);
							{
								float cost = Vector3DX::Cross(vec_z, vec_a).y / z_hyp;
								float sint = sqrtf(std::abs(1.f - cost * cost));
								view_YradAdd = (std::atan2f(cost, sint)) / 5.f;
							}
							view_XradAdd = (std::atan2f(vec_a.y, a_hyp) - std::atan2f(vec_z.y, z_hyp)) / 1.f;
							if (i == 0) {
								//printfDx("%.2f\n", rad2deg(view_XradAdd));
							}
						}
					}
					else {
						view_XradAdd = 0.1f / FPS;
						view_YradAdd = 0.f;
					}

					float limit = this->m_VecData->GetMaxTurretRad() / FPS;
					if (i > 0) {
						limit *= 2.f;
					}
					this->m_view_rad[i].x = (std::clamp(this->m_view_rad[i].x + std::clamp(view_XradAdd, -limit / 5.f, limit / 5.f), -deg2rad(10.f), deg2rad(40.f)));
					this->m_view_rad[i].y += (std::clamp(view_YradAdd, -limit, limit));
				}
			}
			this->m_view_override = false;
		}
		//フレーム操作
		void			VehicleClass::ExecuteFrame(void) noexcept {
			//砲塔旋回
			for (auto& g : this->m_Gun) {
				auto ID = &g - &this->m_Gun.front();
				if (ID == 0) {
					g.ExecuteGunFrame(this->m_view_rad[ID].x, this->m_view_rad[ID].y, &GetObj(), &this->m_col);
				}
				else {
					g.ExecuteGunFrame(this->m_view_rad[ID].x, this->m_view_rad[ID].y - this->m_view_rad[0].y, &GetObj(), &this->m_col);
				}
			}
			//転輪
			auto y_vec = GetObj().GetMatrix().yvec();
			for (auto& f : this->m_VecData->Get_wheelframe()) {
				if (f.GetFrameID() >= 0) {
					auto ID = &f - &this->m_VecData->Get_wheelframe().front();
					GetObj().frame_Reset(f.GetFrameID());

					auto startpos = GetObj().frame(f.GetFrameID());
					auto pos_t1 = startpos + y_vec * ((-f.GetFrameWorldPosition().pos().y) + 2.f*Scale_Rate);
					auto pos_t2 = startpos + y_vec * ((-f.GetFrameWorldPosition().pos().y) - 0.3f*Scale_Rate);
					auto ColRes = this->m_BackGround->CheckLinetoMap(pos_t1, &pos_t2, true, !(this->m_MyID == 0));//CheckOnlyGround
					Easing(&this->m_wheel_frameYpos[ID], (ColRes) ? (pos_t2.y + y_vec.y * f.GetFrameWorldPosition().pos().y - startpos.y) : -0.3f*Scale_Rate, 0.9f, EasingType::OutExpo);
					GetObj().SetFrameLocalMatrix(f.GetFrameID(),
						Matrix4x4DX::RotAxis(Vector3DX::right(), (f.GetFrameWorldPosition().pos().x >= 0) ? this->m_wheel_Left : this->m_wheel_Right) *
						Matrix4x4DX::Mtrans(Vector3DX::up() * this->m_wheel_frameYpos[ID]) *
						Matrix4x4DX::Mtrans(f.GetFrameWorldPosition().pos())
					);
				}
			}
			for (const auto& f : this->m_VecData->Get_wheelframe_nospring()) {
				if (f.GetFrameID() >= 0) {
					GetObj().SetFrameLocalMatrix(f.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::right(), (f.GetFrameWorldPosition().pos().x >= 0) ? this->m_wheel_Left : this->m_wheel_Right) * Matrix4x4DX::Mtrans(f.GetFrameWorldPosition().pos()));
				}
			}
		}
		//操作共通
		void			VehicleClass::ExecuteMove(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();//todo:GetMyPlayerID()
			bool isfloat = this->m_VecData->GetIsFloat() && (this->m_move.pos.y == -this->m_VecData->GetDownInWater());
			//傾きの取得
			{
				const auto bNormal = (isfloat) ? Vector3DX::up() : this->m_BodyNormal;
				float yradBody = 0.f;
				if (this->m_PosBufOverRideFlag) {
					yradBody = this->m_RadOverRide.y;
				}
				else {
					auto pp = (this->m_move.mat * Matrix4x4DX::RotVec2(Vector3DX::up(), bNormal).inverse()).zvec() * -1.f;
					yradBody = std::atan2f(pp.x, pp.z);
				}
				this->m_move.mat = Matrix4x4DX::Axis1(bNormal, (Matrix4x4DX::RotAxis(Vector3DX::up(), yradBody)*Matrix4x4DX::RotVec2(Vector3DX::up(), bNormal)).zvec() * -1.f);
				if (this->m_PosBufOverRideFlag) {
					//一旦その場で地形判定
					this->m_move.pos = this->m_PosBufOverRide;
					GetObj().SetMatrix(this->m_move.MatIn());
				}
				Easing(&this->m_BodyNormal, (Vector3DX::Cross((GetObj().frame(this->m_VecData->Get_square(0)) - GetObj().frame(this->m_VecData->Get_square(3))), GetObj().frame(this->m_VecData->Get_square(1)) - GetObj().frame(this->m_VecData->Get_square(2)))).normalized(), 0.8f, EasingType::OutExpo);
			}
			//地面判定
			{
				auto pos_ytmp = this->m_move.pos.y;
				float hight_t = 0.f;
				int cnt_t = 0;
				//履帯
				for (auto& f : this->m_b2Foot) {
					f.FirstExecute(&GetObj(), m_BackGround, !(this->m_MyID == 0));//CheckOnlyGround
					for (const auto& t : f.Getdownsideframe()) {
						if (t.GetColResult_Y() != (std::numeric_limits<float>::max)()) {
							hight_t += t.GetColResult_Y();
							cnt_t++;
						}
					}
				}
				int cnt_sq = cnt_t;
				for (const auto& s : this->m_VecData->Get_square()) {
					auto p_t = GetObj().frame(s);
					auto pos_t1 = p_t + (Vector3DX::up() * 3.f*Scale_Rate);
					auto pos_t2 = p_t + (Vector3DX::up() * -0.5f*Scale_Rate);
					if (this->m_BackGround->CheckLinetoMap(pos_t1, &pos_t2, true, false)) {
						hight_t += pos_t2.y;
						cnt_t++;
					}
				}
				if (cnt_t > 0) {
					Easing(&pos_ytmp, (hight_t / cnt_t), 0.9f, EasingType::OutExpo);
				}
				this->m_move.pos.y = (pos_ytmp);
				//地面or水面にいるかどうか
				if ((cnt_t > 0) || isfloat) {
					//前進後退
					{
						const auto spdold = this->m_speed_add + this->m_speed_sub;
						this->m_speed_add = (this->m_key[2]) ? std::min(this->m_speed_add + (0.15f / 3.6f) * (60.f / FPS), (this->m_VecData->GetMaxFrontSpeed() / 3.6f)) : std::max(this->m_speed_add - (0.7f / 3.6f) * (60.f / FPS), 0.f);
						this->m_speed_sub = (this->m_key[3]) ? std::max(this->m_speed_sub - (0.15f / 3.6f) * (60.f / FPS), (this->m_VecData->GetMaxBackSpeed() / 3.6f)) : std::min(this->m_speed_sub + (0.7f / 3.6f) * (60.f / FPS), 0.f);
						this->m_speed = (spdold + ((this->m_speed_add + this->m_speed_sub) - spdold) * 0.1f) / FPS;
						auto vec = this->m_move.mat.zvec() * -1.f * (this->m_speed*Scale_Rate);
						this->m_move.vec.x = (vec.x);
						if ((cnt_t - cnt_sq) >= 3) {
							this->m_move.vec.y = (vec.y);
						}
						else {
							this->m_move.vec.y += (M_GR / (FPS * FPS));
						}
						this->m_move.vec.z = (vec.z);
					}
					//旋回
					{
						const auto radold = this->m_radAdd;
						this->m_yradadd_left = (this->m_key[4]) ? std::max(this->m_yradadd_left - deg2rad(1.f * (60.f / FPS)), deg2rad(-this->m_VecData->GetMaxBodyRad())) : std::min(this->m_yradadd_left + deg2rad(2.f * (60.f / FPS)), 0.f);
						this->m_yradadd_right = (this->m_key[5]) ? std::min(this->m_yradadd_right + deg2rad(1.f * (60.f / FPS)), deg2rad(this->m_VecData->GetMaxBodyRad())) : std::max(this->m_yradadd_right - deg2rad(2.f * (60.f / FPS)), 0.f);
						this->m_radAdd.y = ((this->m_yradadd_left + this->m_yradadd_right) / FPS);
						//慣性
						this->m_radAdd.x = (deg2rad(-(this->m_speed / (60.f / FPS)) / (0.1f / 3.6f) * 50.f));
						this->m_radAdd.z = (deg2rad(-this->m_radAdd.y / (deg2rad(5.f) / FPS) * 5.f));
						Easing(&this->m_Tilt, Vector3DX::vget(std::clamp(this->m_radAdd.x - radold.x, deg2rad(-30.f), deg2rad(30.f)), 0.f, std::clamp(this->m_radAdd.z - radold.z, deg2rad(-15.f), deg2rad(15.f))), 0.95f, EasingType::OutExpo);
						this->m_move.mat *= Matrix4x4DX::RotAxis(this->m_move.mat.xvec(), -this->m_Tilt.x) * Matrix4x4DX::RotAxis(this->m_move.mat.zvec(), this->m_Tilt.z);
					}
					//
				}
				else {
					this->m_move.vec.y += (M_GR / (FPS * FPS));
				}
			}
			//射撃反動
			for (auto& cg : this->m_Gun) {
				auto index = &cg - &this->m_Gun.front();
				cg.FireReaction(&this->m_move.mat);
				//射撃
				const std::shared_ptr<CellItem>* PtrBuf = nullptr;
				int Check = -1;
				float Time = 1.f;
				for (int i = 0; i < cg.GetAmmoSpecNum(); i++) {
					const auto* Ptr = PlayerMngr->GetPlayer(this->GetMyPlayerID()).GetInventory(0, [&](const std::shared_ptr<CellItem>& tgt) {
						return tgt->GetItemData() == cg.GetAmmoSpec(i);
					});
					if (Ptr) {
						PtrBuf = Ptr;
						Check = i;
						Time = 1.f;
						break;
					}
				}
				if (!PtrBuf) {
					for (int i = 0; i < cg.GetAmmoSpecNum(); i++) {
						const auto* Ptr = PlayerMngr->GetPlayer(this->GetMyPlayerID()).GetInventory(1, [&](const std::shared_ptr<CellItem>& tgt) {
							return tgt->GetItemData() == cg.GetAmmoSpec(i);
						});
						if (Ptr) {
							PtrBuf = Ptr;
							Check = i;
							Time = 2.5f;
							break;
						}
					}
				}

				if (cg.Execute(this->m_key[(index == 0) ? 0 : 1], (Check >= 0), Time, this->m_CharaType == CharaTypeID::Mine)) {
					HitPoint AmmoC = 1;
					if ((*PtrBuf)->Sub(&AmmoC)) {
						PlayerMngr->GetPlayer(this->GetMyPlayerID()).DeleteInventory(*PtrBuf);
					}
					SE->Get((int)SoundEnum::Tank_Shot).Play_3D(cg.GetShotSound(), this->m_move.pos, 250.f*Scale_Rate);													//サウンド
					EffectControl::SetOnce(EffectResource::Effect::ef_fire, GetGunMuzzlePos((int)index), GetGunMuzzleVec((int)index), cg.GetCaliberSize() / 0.1f * Scale_Rate);	//銃発砲エフェクトのセット

					auto LastAmmo = std::make_shared<AmmoClass>();
					ObjMngr->AddObject(LastAmmo);

					LastAmmo->SetMapCol(this->m_BackGround);
					LastAmmo->Init();

					LastAmmo->Put(cg.GetAmmoSpec(Check), GetGunMuzzlePos((int)index), GetGunMuzzleVec((int)index), this->m_MyID);
				}
			}
			//移動
			if (this->m_PosBufOverRideFlag) {
				this->m_move.pos = this->m_PosBufOverRide;
				this->m_move.vec = this->m_VecBufOverRide;
			}
			else {
				this->m_move.pos += this->m_move.vec;
				//浮く
				if (this->m_VecData->GetIsFloat()) {
					this->m_move.pos.y = (std::max(this->m_move.pos.y, -this->m_VecData->GetDownInWater()));
				}
			}
			//転輪
			this->m_wheel_Left += (-(this->m_speed * 2.f - this->m_radAdd.y * 5.f));
			this->m_wheel_Right += (-(this->m_speed * 2.f + this->m_radAdd.y * 5.f));
			//戦車壁判定
			if (this->m_PosBufOverRideFlag) {
				this->m_b2mine.SetTransform(b2Vec2(this->m_move.pos.x, this->m_move.pos.z), Get_body_yrad());
			}
			else {
				this->m_b2mine.Execute(this->m_move.vec, this->m_radAdd.y);
			}
		}
		//SetMat指示更新
		void			VehicleClass::ExecuteMatrix(void) noexcept {
			auto OldPos = this->m_move.pos;
			//戦車座標反映
			this->m_move.mat *= Matrix4x4DX::RotAxis(Vector3DX::up(), -this->m_b2mine.Rad() - Get_body_yrad());
			this->m_move.pos = Vector3DX::vget(this->m_b2mine.Pos().x, this->m_move.pos.y, this->m_b2mine.Pos().y);
			float spdrec = this->m_spd_buf;
			Easing(&this->m_spd_buf, this->m_b2mine.Speed() * ((this->m_spd_buf > 0) ? 1.f : -1.f), 0.99f, EasingType::OutExpo);
			this->m_speed = this->m_spd_buf - spdrec;

			//転輪
			b2Vec2 Gravity2D = b2Vec2(
				(M_GR / FPS / 2.f) * (Vector3DX::Dot(this->m_move.mat.zvec(), Vector3DX::up())),
				(M_GR / FPS / 2.f) * (Vector3DX::Dot(this->m_move.mat.yvec(), Vector3DX::up())));
			for (auto& f : this->m_b2Foot) {
				f.LateExecute(
					&f == &this->m_b2Foot.front(), this->m_VecData, &GetObj(),
					Gravity2D, (&f == &this->m_b2Foot.front()) ? this->m_wheel_Left : this->m_wheel_Right,
					(this->m_move.pos - this->m_move.repos).magnitude() * 60.f / FPS);
			}
			UpdateMove();
			this->m_add_vec_real = this->m_move.pos - this->m_move.repos;
			this->m_Hit_active.Execute(GetObj());
			this->m_move.repos = this->m_move.pos;
		}
		//描画共通
		void			VehicleClass::DrawCommon(void) noexcept {
			if (!is_ADS()) {
				if (CheckCameraViewClip_Box(
					(this->GetMove().pos + Vector3DX::vget(-5, -5, -5)*Scale_Rate).get(),
					(this->GetMove().pos + Vector3DX::vget(5, 5, 5)*Scale_Rate).get()) == FALSE
					) {

					//this->m_col.DrawModel();
					//this->m_Hit_active.Draw();
					//return;

					if (true) {
						if (this->m_HP_parts[this->m_VecData->Get_module_mesh()[0]] > 0) {
							MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_wheel_Left * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
							GetObj().DrawMesh(0);
						}
						if (this->m_HP_parts[this->m_VecData->Get_module_mesh()[1]] > 0) {
							MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_wheel_Right * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
							GetObj().DrawMesh(1);
						}
						MV1ResetFrameTextureAddressTransform(GetObj().get(), 0);
						GetObj().DrawMesh(2);
						for (int i = 2; i < GetObj().mesh_num(); i++) {
							GetObj().DrawMesh(i);
						}
						//this->m_col.DrawModel();
						this->m_Hit_active.Draw();
					}
					else {
						GetObj().DrawModel();
					}
				}
			}
		}
		//
		const bool		VehicleClass::CheckLine(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) noexcept {
			bool IsHit = false;
			if (this->RefreshCol(StartPos, *EndPos, 10.0f*Scale_Rate)) {
				//装甲(一番近い位置のものに限定する)
				for (const auto& m : this->m_VecData->Get_armer_mesh()) {
					auto Res = GetColLine(StartPos, *EndPos, m.first);
					if (Res.HitFlag) {
						IsHit = true;
						*EndPos = Res.HitPosition;
						if (Normal) {
							*Normal = Res.Normal;
						}
					}
				}
			}
			return IsHit;
		}
		//
		const std::pair<bool, bool>		VehicleClass::CheckAmmoHit(AmmoClass* pAmmo, const Vector3DX& pShooterPos) noexcept {
			std::pair<bool, bool> isDamaged{ false,false };
			if (this->RefreshCol(pAmmo->GetMove().repos, pAmmo->GetMove().pos, 10.0f*Scale_Rate)) {
				if (this->CheckAmmoHited(*pAmmo)) {									//とりあえず当たったかどうか探す
					isDamaged.first = true;
					if (this->CalcAmmoHited(pAmmo, pShooterPos)) {
						isDamaged.second = true;
					}
				}
			}
			return isDamaged;
		}
		//
		void			VehicleClass::HitGround(const Vector3DX& pos_t, const Vector3DX& pNorm, const Vector3DX& /*pVec*/) noexcept {
			EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_t, pNorm, 0.05f / 0.1f * Scale_Rate);
			//hit_obj_p.Set(a.GetCaliberSize() * Scale_Rate, pos_t, pNorm, pVec);	//弾痕
		}
		//
		void			VehicleClass::DrawModuleView(int xp, int yp, int size) noexcept {
			auto base = GetLookVec().zvec()*-1.f;
			base.y = (0.f);
			base = base.normalized();

			auto vec = Vector3DX::forward();
			auto rad = std::atan2f(Vector3DX::Cross(base, vec).y, Vector3DX::Dot(base, vec));


			for (auto&m : this->m_VecData->Get_module_view()[0]) {
				if (0 <= m.second && m.second < this->m_HP_parts.size()) {
					if (this->m_HP_parts[this->m_VecData->Get_module_mesh()[m.second]] > 0) {
						auto radp = deg2rad(90)*(float)this->m_HP_parts[this->m_VecData->Get_module_mesh()[m.second]] / (float)(this->m_VecData->GetMaxHP());
						SetDrawBright(std::clamp((int)(255.f*cos(radp)*2.f), 0, 255), std::clamp((int)(255.f*sin(radp)*2.f), 0, 255), 0);
					}
					else {
						SetDrawBright(0, 0, 0);
					}
				}
				else {
					auto radp = deg2rad(90)*(float)this->GetHP() / (float)(this->m_VecData->GetMaxHP());
					SetDrawBright(std::clamp((int)(255.f*cos(radp)*2.f), 0, 255), std::clamp((int)(255.f*sin(radp)*2.f), 0, 255), 0);
				}
				m.first->DrawRotaGraph(xp, yp, (float)size / 200, rad + this->Get_body_yrad(), true);
			}
			for (auto&m : this->m_VecData->Get_module_view()[1]) {
				if (0 <= m.second && m.second < this->m_HP_parts.size()) {
					if (this->m_HP_parts[this->m_VecData->Get_module_mesh()[m.second]] > 0) {
						auto radp = deg2rad(90)*(float)this->m_HP_parts[this->m_VecData->Get_module_mesh()[m.second]] / (float)(this->m_VecData->GetMaxHP());
						SetDrawBright(std::clamp((int)(255.f*cos(radp)*2.f), 0, 255), std::clamp((int)(255.f*sin(radp)*2.f), 0, 255), 0);
					}
					else {
						SetDrawBright(0, 0, 0);
					}
				}
				else {
					auto radp = deg2rad(90)*(float)this->GetHP() / (float)(this->m_VecData->GetMaxHP());
					SetDrawBright(std::clamp((int)(255.f*cos(radp)*2.f), 0, 255), std::clamp((int)(255.f*sin(radp)*2.f), 0, 255), 0);
				}
				m.first->DrawRotaGraph(xp, yp, (float)size / 200, rad + this->Get_body_yrad() + this->m_view_rad[0].y, true);
			}
			SetDrawBright(255, 255, 255);
		}
	};
};
