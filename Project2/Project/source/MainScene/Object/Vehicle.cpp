#pragma once
#include	"Vehicle.hpp"
#include "../../MainScene/Player/Player.hpp"
#include "../../MainScene/Object/Item.hpp"

#include "../../MainScene/Object/Ammo.hpp"
namespace FPS_n2 {
	namespace Sceneclass {
		//----------------------------------------------------------
		//�������֘A
		//----------------------------------------------------------
		void			VehicleClass::SubHP_Parts(HitPoint damage_t, int parts_Set_t) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();//todo:GetMyPlayerID()
			if (GetData().Get_module_mesh()[0] == parts_Set_t) {
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
			else if (GetData().Get_module_mesh()[1] == parts_Set_t) {
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
			auto* DrawParts = DXDraw::Instance();
			if (this->m_MyID == value.DamageID) {
				SubHP(value.Damage);
				if (!IsAlive()) {
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
										Vec*25.f / DrawParts->GetFps() * (1.0f*GetRandf(0.5f)));
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
		void			VehicleClass::ValueInit(int VhehID, const std::shared_ptr<b2World>& pB2World, PlayerID pID) noexcept {
			MV1::Load("data/model/hit/model.mv1", &this->m_hit_pic);
			this->m_MyID = pID;
			this->m_VehDataID = VhehID;
			this->m_Hit_active.Init(this->m_hit_pic);														//�e��

			GetObj().material_AlphaTestAll(true, DX_CMP_GREATER, 128);								//�{��
			this->hitres.resize(this->m_col.mesh_num());											//���W���[�����Ƃ̓����蔻�茋�ʂ��m��
			this->hitssort.resize(this->m_col.mesh_num());											//���W���[�����Ƃ̓����蔻�菇�����m��
			b2PolygonShape dynamicBox = GetData().GetDynamicBox();
			this->m_b2mine.Set(CreateB2Body((std::shared_ptr<b2World>&)pB2World, b2_dynamicBody, 0.f, 0.f, 0.f), &dynamicBox);	//�V�F�C�v���{�f�B�ɒǉ�
			for (auto& f : this->m_b2Foot) {
				f.Init(&f == &this->m_b2Foot.front(), &GetData(), &GetObj());
			}
			this->m_wheel_frameYpos.resize(GetData().Get_wheelframe().size());
			//�C
			this->m_Gun.resize(GetData().Get_gunframe().size());
			for (const auto& g : GetData().Get_gunframe()) {
				int index = (int)(&g - &GetData().Get_gunframe().front());
				this->m_Gun[index].Init(this->m_VehDataID, index);
			}
		}
		//
		void			VehicleClass::ValueSet(float pxRad, float pyRad, const Vector3DX& pos_t) noexcept {
			this->m_move.mat = Matrix4x4DX::RotAxis(Vector3DX::right(), pxRad) * Matrix4x4DX::RotAxis(Vector3DX::up(), pyRad);
			this->m_move.pos = pos_t;
			this->m_move.vec.Set(0, 0, 0);
			for (auto& w : this->m_wheel_frameYpos) { w = 0.f; }
			//�C
			//�q�b�g�|�C���g
			{
				InitLife(GetData().GetMaxHP());
				this->m_HP_parts.resize(this->m_col.mesh_num());
				for (auto& h : this->m_HP_parts) { h = GetHPMax(); }//���W���[���ϋv
			}
			this->m_Fuel = (float)GetData().GetMaxFuel();

			//��ԃX�|�[��
			this->m_b2mine.SetTransform(b2Vec2(this->m_move.pos.x, this->m_move.pos.z), Get_body_yrad());
			this->m_PosBufOverRideFlag = false;

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
			//�G�C��
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
			this->m_key[0] = pInput.GetPADSPress(PADS::SHOT) && pReady && this->IsAlive();			//�ˌ�
			this->m_key[1] = pInput.GetPADSPress(PADS::JUMP) && pReady && this->IsAlive();			//�}�V���K��
			this->m_key[2] = pInput.GetPADSPress(PADS::MOVE_W) && pReady && this->IsAlive() && (this->m_HP_parts[GetData().Get_module_mesh()[0]] > 0) && (this->m_HP_parts[GetData().Get_module_mesh()[1]] > 0);		//�O�i
			this->m_key[3] = pInput.GetPADSPress(PADS::MOVE_S) && pReady && this->IsAlive() && (this->m_HP_parts[GetData().Get_module_mesh()[0]] > 0) && (this->m_HP_parts[GetData().Get_module_mesh()[1]] > 0);		//���
			this->m_key[4] = pInput.GetPADSPress(PADS::MOVE_D) && pReady && this->IsAlive() && ((this->m_HP_parts[GetData().Get_module_mesh()[0]] > 0) || (this->m_HP_parts[GetData().Get_module_mesh()[1]] > 0));		//�E
			this->m_key[5] = pInput.GetPADSPress(PADS::MOVE_A) && pReady && this->IsAlive() && ((this->m_HP_parts[GetData().Get_module_mesh()[0]] > 0) || (this->m_HP_parts[GetData().Get_module_mesh()[1]] > 0));		//��
			this->m_key[6] = pInput.GetPADSPress(PADS::AIM) && pReady && this->IsAlive();			//��

			//this->m_key[6] = true;
		}
		//�J�����ݒ�o��
		void			VehicleClass::Setcamera(Camera3DInfo& MainCamera_t, const float fov_base) noexcept {
			auto fov_t = MainCamera_t.GetCamFov();
			auto near_t = MainCamera_t.GetCamNear();
			auto far_t = MainCamera_t.GetCamFar();

			Vector3DX eyepos = Get_EyePos_Base();
			auto OLD = this->m_range;

			if (!this->IsAlive()) {
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
				if (this->IsAlive()) {
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
		//�X�V�֘A
		//----------------------------------------------------------

		//��e�`�F�b�N
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
			//���b(��ԋ߂��ʒu�̂��̂Ɍ��肷��)
			int t = -1;
			MV1_COLL_RESULT_POLY colres; colres.HitFlag = FALSE;
			Vector3DX EndPos = pAmmo.GetMove().pos;
			for (const auto& m : GetData().Get_armer_mesh()) { HitCheck_Tank(m.first, pAmmo.GetMove().repos, colres); }//�S���Z�b�g
			for (const auto& m : GetData().Get_armer_mesh()) {
				auto colres_t = GetColLine(pAmmo.GetMove().repos, EndPos, m.first);
				if (colres_t.HitFlag == TRUE) {
					t = m.first;
					colres = colres_t;
					EndPos = colres_t.HitPosition;
				}
			}
			//
			for (auto& m : GetData().Get_module_mesh()) {//���W���[��
				is_Hit |= HitCheck_Tank(m, pAmmo.GetMove().repos, GetColLine(pAmmo.GetMove().repos, EndPos, m));
			}
			for (auto& m : GetData().Get_space_mesh()) {//��ԑ��b
				is_Hit |= HitCheck_Tank(m, pAmmo.GetMove().repos, GetColLine(pAmmo.GetMove().repos, EndPos, m));
			}
			//
			if (t != -1) {
				is_Hit |= HitCheck_Tank(t, pAmmo.GetMove().repos, colres);
			}
			return is_Hit;
		}
		//��e����
		const auto		VehicleClass::CalcAmmoHited(AmmoClass* pAmmo) noexcept {
			auto* SE = SoundPool::Instance();
			std::sort(this->hitssort.begin(), this->hitssort.end(), [](const HitSortInfo& x, const HitSortInfo& y) { return x < y; });	//�����蔻����߂����Ƀ\�[�g
			bool isDamage = false;
			//�_���[�W�ʂɓ͂��܂Ŕ���
			for (const auto& tt : this->hitssort) {
				if (tt.IsHit()) {
					if (tt.GetHitMesh() >= this->hitres.size()) { continue; }
					Vector3DX HitPos = this->hitres[tt.GetHitMesh()].HitPosition;
					Vector3DX HitNormal = this->hitres[tt.GetHitMesh()].Normal;
					HitNormal = HitNormal.normalized();

					for (auto& mesh : GetData().Get_space_mesh()) {
						if (tt.GetHitMesh() == mesh) {
							//��ԑ��b�ɓ��������̂Ń��W���[����30�_��
							EffectControl::SetOnce(EffectResource::Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate);
							this->SubHP_Parts(pAmmo->GetDamage(), (HitPoint)tt.GetHitMesh());
						}
					}
					for (auto& mesh : GetData().Get_module_mesh()) {
						if (tt.GetHitMesh() == mesh) {
							//���W���[���ɓ��������̂Ń��W���[����30�_��
							EffectControl::SetOnce(EffectResource::Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate);
							this->SubHP_Parts(pAmmo->GetDamage(), (HitPoint)tt.GetHitMesh());
						}
					}
					//�_���[�W�ʂɓ�������
					for (auto& mesh : GetData().Get_armer_mesh()) {
						if (tt.GetHitMesh() == mesh.first) {
							//�e���_���[�W�w�ɓ͂���
							if (pAmmo->PenetrationCheck(mesh.second, HitNormal) && this->IsAlive()) {						//�_���[�W�ʂɓ����������ɑ��b�l�ɏ��Ă邩�ǂ���
								pAmmo->Penetrate();	//�ђ�
								SE->Get((int)SoundEnum::Tank_Damage).Play_3D(GetRand(1), HitPos, 100.f*Scale_Rate, 216);
								//�_���[�W�v�Z
								//auto v1 = Matrix4x4DX::RotAxis(Vector3DX::up(), Get_body_yrad()).zvec();
								//auto v2 = (pShooterPos - this->m_move.pos).normalized(); v2.y = (0);
								//std::atan2f(Vector3DX::Cross(v1, v2).y, Vector3DX::Dot(v1, v2))
								LifeControl::SetSubHPEvent(pAmmo->GetShootedID(), this->m_MyID, pAmmo->GetDamage());
								this->SubHP_Parts(pAmmo->GetDamage(), (HitPoint)tt.GetHitMesh());
								isDamage = true;
							}
							else {
								pAmmo->Ricochet(HitPos, HitNormal);	//���e
								SE->Get((int)SoundEnum::Tank_Ricochet).Play_3D(GetRand(16), HitPos, 100.f*Scale_Rate, 216);
								m_ShakePer = 0.1f*pAmmo->GetCaliberSize() / 0.10f;
							}
							//�G�t�F�N�g�Z�b�g
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

		//�ȑO�̏�ԕێ�
		void			VehicleClass::ExecuteSavePrev(void) noexcept {
			this->m_spd_rec = this->m_speed;
			this->m_ColActive = false;
		}
		//���̑�
		void			VehicleClass::ExecuteElse(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* SE = SoundPool::Instance();
			//�G���W����
			if (this->IsAlive()) {
				if (this->m_engine_time <= 0.f) {

					SE->Get((int)SoundEnum::Tank_engine).Play_3D(0, this->m_move.pos, 50.f*Scale_Rate, 64);//, DX_PLAYTYPE_LOOP
					this->m_engine_time = 0.9f;
					if (this->m_engine_start) {
						this->m_engine_start = false;
						SE->Get((int)SoundEnum::Tank_Start).Play_3D(0, this->m_move.pos, 50.f*Scale_Rate, 128);//, DX_PLAYTYPE_LOOP
					}
				}
				else {
					this->m_engine_time = std::max(this->m_engine_time - 1.f / DrawParts->GetFps(), 0.f);
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
		//����
		void			VehicleClass::ExecuteInput(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			if (!this->m_view_override) {
				float	view_YradAdd{ 0.f };							//
				float	view_XradAdd{ 0.f };							//
				for (auto& g : this->m_Gun) {
					auto ID = (int)(&g - &this->m_Gun.front());
					if (this->IsAlive()) {
						if (this->m_key[6]) { //�C�����b�N
							view_XradAdd = 0.f;
							view_YradAdd = 0.f;
						}
						else {
							//�_��
							Vector3DX MuzPos = GetGunMuzzlePos(ID);
							Vector3DX EndPos = MuzPos + (this->m_MouseVec.zvec() * -1.f).normalized() * (500.f*Scale_Rate);
							this->m_BackGround->CheckLinetoMap(MuzPos, &EndPos, true, false);

							Vector3DX BasePos = GetGunMuzzleBase(ID);
							//���f
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
						}
					}
					else {
						view_XradAdd = 0.1f / 60.f;
						view_YradAdd = 0.f;
					}

					float limit = GetData().GetMaxTurretRad() / 60.f;
					if (ID > 0) {
						limit *= 2.f;
					}
					g.UpdateGunVec(
						std::clamp(view_XradAdd, -limit / 5.f, limit / 5.f)*60.f / DrawParts->GetFps(),
						std::clamp(view_YradAdd, -limit, limit)*60.f / DrawParts->GetFps()
					);
				}
			}
			this->m_view_override = false;
		}
		//�t���[������
		void			VehicleClass::ExecuteFrame(void) noexcept {
			//�C������
			for (auto& g : this->m_Gun) {
				auto ID = &g - &this->m_Gun.front();
				float Xrad = g.GetGunViewVec().x;
				float Yrad = (ID == 0) ? g.GetGunViewVec().y : (g.GetGunViewVec().y - GetViewRad().y);

				float xrad = std::clamp(Xrad, deg2rad(g.GetData()->GetDownRadLimit()), deg2rad(g.GetData()->GetUpRadLimit()));

				if (g.GetGunBase().GetFrameID() >= 0) {
					auto Mat = Matrix4x4DX::RotAxis(Vector3DX::up(), Yrad) * Matrix4x4DX::Mtrans(g.GetGunBase().GetFrameLocalPosition().pos());
					GetObj().SetFrameLocalMatrix(g.GetGunBase().GetFrameID(), Mat);
					this->m_col.SetFrameLocalMatrix(g.GetGunBase().GetFrameID(), Mat);
				}
				if (g.GetGunTrunnion().GetFrameID() >= 0) {
					auto Mat = Matrix4x4DX::RotAxis(Vector3DX::right(), xrad) * Matrix4x4DX::Mtrans(g.GetGunTrunnion().GetFrameLocalPosition().pos());
					GetObj().SetFrameLocalMatrix(g.GetGunTrunnion().GetFrameID(), Mat);
					this->m_col.SetFrameLocalMatrix(g.GetGunTrunnion().GetFrameID(), Mat);
				}
				if (g.GetGunMuzzle().GetFrameID() >= 0) {
					auto Mat = Matrix4x4DX::Mtrans(Vector3DX::forward() * (g.GetRecoilPer() * 0.5f * Scale_Rate)) * Matrix4x4DX::Mtrans(g.GetGunMuzzle().GetFrameLocalPosition().pos());
					GetObj().SetFrameLocalMatrix(g.GetGunMuzzle().GetFrameID(), Mat);
					this->m_col.SetFrameLocalMatrix(g.GetGunMuzzle().GetFrameID(), Mat);
				}
				g.UpdateFireReaction_(Yrad);
			}
			//�]��
			auto y_vec = GetObj().GetMatrix().yvec();
			for (auto& f : GetData().Get_wheelframe()) {
				if (f.GetFrameID() >= 0) {
					auto ID = &f - &GetData().Get_wheelframe().front();
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
			for (const auto& f : GetData().Get_wheelframe_nospring()) {
				if (f.GetFrameID() >= 0) {
					GetObj().SetFrameLocalMatrix(f.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::right(), (f.GetFrameWorldPosition().pos().x >= 0) ? this->m_wheel_Left : this->m_wheel_Right) * Matrix4x4DX::Mtrans(f.GetFrameWorldPosition().pos()));
				}
			}
		}
		//���실��
		void			VehicleClass::ExecuteMove(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* PlayerMngr = PlayerManager::Instance();//todo:GetMyPlayerID()
			bool isfloat = GetData().GetIsFloat() && (this->m_move.pos.y == -GetData().GetDownInWater());
			//�X���̎擾
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
					//��U���̏�Œn�`����
					this->m_move.pos = this->m_PosBufOverRide;
					GetObj().SetMatrix(this->m_move.MatIn());
				}
				Easing(&this->m_BodyNormal, (Vector3DX::Cross((GetObj().frame(GetData().Get_square(0)) - GetObj().frame(GetData().Get_square(3))), GetObj().frame(GetData().Get_square(1)) - GetObj().frame(GetData().Get_square(2)))).normalized(), 0.8f, EasingType::OutExpo);
			}
			//�n�ʔ���
			{
				auto pos_ytmp = this->m_move.pos.y;
				float hight_t = 0.f;
				int cnt_t = 0;
				//����
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
				for (const auto& s : GetData().Get_square()) {
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
				//�n��or���ʂɂ��邩�ǂ���
				if ((cnt_t > 0) || isfloat) {
					//�O�i���
					{
						const auto spdold = this->m_speed_add + this->m_speed_sub;
						this->m_speed_add = (this->m_key[2]) ? std::min(this->m_speed_add + (0.15f / 3.6f) * (60.f / DrawParts->GetFps()), (GetData().GetMaxFrontSpeed() / 3.6f)) : std::max(this->m_speed_add - (0.7f / 3.6f) * (60.f / DrawParts->GetFps()), 0.f);
						this->m_speed_sub = (this->m_key[3]) ? std::max(this->m_speed_sub - (0.15f / 3.6f) * (60.f / DrawParts->GetFps()), (GetData().GetMaxBackSpeed() / 3.6f)) : std::min(this->m_speed_sub + (0.7f / 3.6f) * (60.f / DrawParts->GetFps()), 0.f);
						this->m_speed = (spdold + ((this->m_speed_add + this->m_speed_sub) - spdold) * 0.1f) / DrawParts->GetFps();
						auto vec = this->m_move.mat.zvec() * -1.f * (this->m_speed*Scale_Rate);
						this->m_move.vec.x = (vec.x);
						if ((cnt_t - cnt_sq) >= 3) {
							this->m_move.vec.y = (vec.y);
						}
						else {
							this->m_move.vec.y += (M_GR / (DrawParts->GetFps() * DrawParts->GetFps()));
						}
						this->m_move.vec.z = (vec.z);
					}
					//����
					{
						const auto radold = this->m_radAdd;
						this->m_yradadd_left = (this->m_key[4]) ? std::max(this->m_yradadd_left - deg2rad(1.f * (60.f / DrawParts->GetFps())), deg2rad(-GetData().GetMaxBodyRad())) : std::min(this->m_yradadd_left + deg2rad(2.f * (60.f / DrawParts->GetFps())), 0.f);
						this->m_yradadd_right = (this->m_key[5]) ? std::min(this->m_yradadd_right + deg2rad(1.f * (60.f / DrawParts->GetFps())), deg2rad(GetData().GetMaxBodyRad())) : std::max(this->m_yradadd_right - deg2rad(2.f * (60.f / DrawParts->GetFps())), 0.f);
						this->m_radAdd.y = ((this->m_yradadd_left + this->m_yradadd_right) / DrawParts->GetFps());
						//����
						this->m_radAdd.x = (deg2rad(-(this->m_speed / (60.f / DrawParts->GetFps())) / (0.1f / 3.6f) * 50.f));
						this->m_radAdd.z = (deg2rad(-this->m_radAdd.y / (deg2rad(5.f) / DrawParts->GetFps()) * 5.f));
						Easing(&this->m_Tilt, Vector3DX::vget(std::clamp(this->m_radAdd.x - radold.x, deg2rad(-30.f), deg2rad(30.f)), 0.f, std::clamp(this->m_radAdd.z - radold.z, deg2rad(-15.f), deg2rad(15.f))), 0.95f, EasingType::OutExpo);
						this->m_move.mat *= Matrix4x4DX::RotAxis(this->m_move.mat.xvec(), -this->m_Tilt.x) * Matrix4x4DX::RotAxis(this->m_move.mat.zvec(), this->m_Tilt.z);
					}
					//
				}
				else {
					this->m_move.vec.y += (M_GR / (DrawParts->GetFps() * DrawParts->GetFps()));
				}
			}
			//�ˌ�����
			for (auto& cg : this->m_Gun) {
				auto index = &cg - &this->m_Gun.front();
				this->m_move.mat *=
					Matrix4x4DX::RotAxis(this->m_move.mat.xvec(), cg.GetFireReactionVec().x) *
					Matrix4x4DX::RotAxis(this->m_move.mat.zvec(), cg.GetFireReactionVec().z);
				//�ˌ�
				const std::shared_ptr<CellItem>* PtrBuf = nullptr;
				int Check = -1;
				float Time = 1.f;
				for (int i = 0; i < (int)cg.GetData()->GetAmmoSpec().size(); i++) {
					const auto* Ptr = PlayerMngr->GetPlayer(this->GetMyPlayerID()).GetInventory(0, [&](const std::shared_ptr<CellItem>& tgt) {
						return tgt->GetItemData() == cg.GetData()->GetAmmoSpec().at(i);
																								});
					if (Ptr) {
						PtrBuf = Ptr;
						Check = i;
						Time = 1.f;
						break;
					}
				}
				if (!PtrBuf) {
					for (int i = 0; i < (int)cg.GetData()->GetAmmoSpec().size(); i++) {
						const auto* Ptr = PlayerMngr->GetPlayer(this->GetMyPlayerID()).GetInventory(1, [&](const std::shared_ptr<CellItem>& tgt) {
							return tgt->GetItemData() == cg.GetData()->GetAmmoSpec().at(i);
																									});
						if (Ptr) {
							PtrBuf = Ptr;
							Check = i;
							Time = 2.5f;
							break;
						}
					}
				}

				if (cg.Execute_(this->m_key[(index == 0) ? 0 : 1], (Check >= 0), Time, this->m_MyID == 0)) {
					HitPoint AmmoC = 1;
					if ((*PtrBuf)->Sub(&AmmoC)) {
						PlayerMngr->GetPlayer(this->GetMyPlayerID()).DeleteInventory(*PtrBuf);
					}
					SE->Get((int)SoundEnum::Tank_Shot).Play_3D(cg.GetData()->GetShotSound(), this->m_move.pos, 250.f*Scale_Rate);													//�T�E���h

					EffectControl::SetOnce(EffectResource::Effect::ef_fire, GetGunMuzzlePos((int)index), GetGunMuzzleVec((int)index), cg.GetCaliberSize() / 0.1f * Scale_Rate);	//�e���C�G�t�F�N�g�̃Z�b�g

					auto LastAmmo = std::make_shared<AmmoClass>();
					ObjMngr->AddObject(LastAmmo);

					LastAmmo->SetMapCol(this->m_BackGround);
					LastAmmo->Init();

					LastAmmo->Put(cg.GetData()->GetAmmoSpec().at(Check), GetGunMuzzlePos((int)index), GetGunMuzzleVec((int)index), this->m_MyID);
					if (this->m_MyID == 0) {
						SE->Get((int)SoundEnum::Tank_Eject).Play(cg.GetData()->GetEjectSound());
					}
				}
			}
			//�ړ�
			if (this->m_PosBufOverRideFlag) {
				this->m_move.pos = this->m_PosBufOverRide;
				this->m_move.vec = this->m_VecBufOverRide;
			}
			else {
				this->m_move.pos += this->m_move.vec;
				//����
				if (GetData().GetIsFloat()) {
					this->m_move.pos.y = (std::max(this->m_move.pos.y, -GetData().GetDownInWater()));
				}
			}
			//�]��
			this->m_wheel_Left += (-(this->m_speed * 2.f - this->m_radAdd.y * 5.f));
			this->m_wheel_Right += (-(this->m_speed * 2.f + this->m_radAdd.y * 5.f));
			//��ԕǔ���
			if (this->m_PosBufOverRideFlag) {
				this->m_b2mine.SetTransform(b2Vec2(this->m_move.pos.x, this->m_move.pos.z), Get_body_yrad());
			}
			else {
				this->m_b2mine.Execute(this->m_move.vec, this->m_radAdd.y);
			}
		}
		//SetMat�w���X�V
		void			VehicleClass::ExecuteMatrix(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto OldPos = this->m_move.pos;
			//��ԍ��W���f
			this->m_move.mat *= Matrix4x4DX::RotAxis(Vector3DX::up(), -this->m_b2mine.Rad() - Get_body_yrad());
			this->m_move.pos = Vector3DX::vget(this->m_b2mine.Pos().x, this->m_move.pos.y, this->m_b2mine.Pos().y);
			float spdrec = this->m_spd_buf;
			Easing(&this->m_spd_buf, this->m_b2mine.Speed() * ((this->m_spd_buf > 0) ? 1.f : -1.f), 0.99f, EasingType::OutExpo);
			this->m_speed = this->m_spd_buf - spdrec;

			//�]��
			b2Vec2 Gravity2D = b2Vec2(
				(M_GR / DrawParts->GetFps() / 2.f) * (Vector3DX::Dot(this->m_move.mat.zvec(), Vector3DX::up())),
				(M_GR / DrawParts->GetFps() / 2.f) * (Vector3DX::Dot(this->m_move.mat.yvec(), Vector3DX::up())));
			for (auto& f : this->m_b2Foot) {
				f.LateExecute(
					&f == &this->m_b2Foot.front(), &GetData(), &GetObj(),
					Gravity2D, (&f == &this->m_b2Foot.front()) ? this->m_wheel_Left : this->m_wheel_Right,
					(this->m_move.pos - this->m_move.repos).magnitude() * 60.f / DrawParts->GetFps());
			}
			UpdateMove();
			this->m_add_vec_real = this->m_move.pos - this->m_move.repos;
			this->m_Hit_active.Execute(GetObj());
			this->m_move.repos = this->m_move.pos;
		}
		//�`�拤��
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
						if (this->m_HP_parts[GetData().Get_module_mesh()[0]] > 0) {
							MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_wheel_Left * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
							GetObj().DrawMesh(0);
						}
						if (this->m_HP_parts[GetData().Get_module_mesh()[1]] > 0) {
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
				//���b(��ԋ߂��ʒu�̂��̂Ɍ��肷��)
				for (const auto& m : GetData().Get_armer_mesh()) {
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
		const std::pair<bool, bool>		VehicleClass::CheckAmmoHit(AmmoClass* pAmmo) noexcept {
			std::pair<bool, bool> isDamaged{ false,false };
			if (this->RefreshCol(pAmmo->GetMove().repos, pAmmo->GetMove().pos, 10.0f*Scale_Rate)) {
				if (this->CheckAmmoHited(*pAmmo)) {									//�Ƃ肠���������������ǂ����T��
					isDamaged.first = true;
					if (this->CalcAmmoHited(pAmmo)) {
						isDamaged.second = true;
					}
				}
			}
			return isDamaged;
		}
		//
		void			VehicleClass::HitGround(const Vector3DX& pos_t, const Vector3DX& pNorm, const Vector3DX& /*pVec*/) noexcept {
			EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_t, pNorm, 0.05f / 0.1f * Scale_Rate);
			//hit_obj_p.Set(a.GetCaliberSize() * Scale_Rate, pos_t, pNorm, pVec);	//�e��
		}
		//
		void			VehicleClass::DrawModuleView(int xp, int yp, int size) noexcept {
			auto base = GetLookVec().zvec()*-1.f;
			base.y = (0.f);
			base = base.normalized();

			auto vec = Vector3DX::forward();
			auto rad = std::atan2f(Vector3DX::Cross(base, vec).y, Vector3DX::Dot(base, vec));


			for (auto&m : GetData().Get_module_view()[0]) {
				if (0 <= m.second && m.second < this->m_HP_parts.size()) {
					if (this->m_HP_parts[GetData().Get_module_mesh()[m.second]] > 0) {
						auto radp = deg2rad(90)*(float)this->m_HP_parts[GetData().Get_module_mesh()[m.second]] / (float)(GetHPMax());
						SetDrawBright(std::clamp((int)(255.f*cos(radp)*2.f), 0, 255), std::clamp((int)(255.f*sin(radp)*2.f), 0, 255), 0);
					}
					else {
						SetDrawBright(0, 0, 0);
					}
				}
				else {
					auto radp = deg2rad(90)*(float)this->GetHP() / (float)(GetHPMax());
					SetDrawBright(std::clamp((int)(255.f*cos(radp)*2.f), 0, 255), std::clamp((int)(255.f*sin(radp)*2.f), 0, 255), 0);
				}
				m.first->DrawRotaGraph(xp, yp, (float)size / 200, rad + this->Get_body_yrad(), true);
			}
			for (auto&m : GetData().Get_module_view()[1]) {
				if (0 <= m.second && m.second < this->m_HP_parts.size()) {
					if (this->m_HP_parts[GetData().Get_module_mesh()[m.second]] > 0) {
						auto radp = deg2rad(90)*(float)this->m_HP_parts[GetData().Get_module_mesh()[m.second]] / (float)(GetHPMax());
						SetDrawBright(std::clamp((int)(255.f*cos(radp)*2.f), 0, 255), std::clamp((int)(255.f*sin(radp)*2.f), 0, 255), 0);
					}
					else {
						SetDrawBright(0, 0, 0);
					}
				}
				else {
					auto radp = deg2rad(90)*(float)this->GetHP() / (float)(GetHPMax());
					SetDrawBright(std::clamp((int)(255.f*cos(radp)*2.f), 0, 255), std::clamp((int)(255.f*sin(radp)*2.f), 0, 255), 0);
				}
				m.first->DrawRotaGraph(xp, yp, (float)size / 200, rad + this->Get_body_yrad() + GetViewRad().y, true);
			}
			SetDrawBright(255, 255, 255);
		}
	};
};
