#pragma once
#include	"Vehicle.hpp"
#include "../../MainScene/Player/Player.hpp"
#include "../../MainScene/Object/Ammo.hpp"
namespace FPS_n2 {
	namespace Sceneclass {
		//----------------------------------------------------------
		//�������֘A
		//----------------------------------------------------------
		void			VehicleClass::SubHP_Parts(HitPoint damage_t, int parts_Set_t) noexcept {
			this->m_HP_parts[parts_Set_t] = std::max<HitPoint>(this->m_HP_parts[parts_Set_t] - damage_t, 0);
		}
		const bool		VehicleClass::SetDamageEvent(const DamageEvent& value) noexcept {
			if (this->m_MyID == value.DamageID) {
				SubHP(value.Damage);
				if (!IsAlive()) {
					EffectControl::SetOnce(EffectResource::Effect::ef_greexp2, this->m_move.pos, this->m_move.mat.zvec(), Scale_Rate*2.f);
				}
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
			//�V�F�C�v���{�f�B�ɒǉ�
			b2PolygonShape dynamicBox = GetData().GetDynamicBox();
			this->m_b2mine.Set(
				CreateB2Body((std::shared_ptr<b2World>&)pB2World, b2_dynamicBody, 0.f, 0.f, 0.f),
				&dynamicBox
			);
			//
			for (auto& f : this->m_CatFoot) {
				const auto& w = GetData().Get_b2downsideframe()[(((&f == &this->m_CatFoot.front()) != 0) ? 0 : 1)];
				f.resize(w.size());
				for (auto& t : f) {
					t.Init(w[&t - &f.front()]);
				}
			}
			for (auto& f : this->m_b2Foot) {
				f.Init(&f == &this->m_b2Foot.front(), &GetData(), &GetObj());
			}
			for (int i = 0;i < 2;i++) {
				int cnt = 0;
				for (auto& f : GetData().Get_wheelframe()) {
					auto localmatBase = f.GetFrameWorldPosition().pos();
					bool IsLeft = (localmatBase.x >= 0);
					if (i == 0) {
						if (!IsLeft) { continue; }
					}
					else {
						if (IsLeft) { continue; }
					}
					this->m_Wheel[i].resize(this->m_Wheel[i].size() + 1);
					this->m_Wheel[i].back().Init(f);
					cnt++;
				}
			}
			//�C
			this->m_Gun.resize(GetData().Get_gunframe().size());
			for (const auto& g : GetData().Get_gunframe()) {
				int index = (int)(&g - &GetData().Get_gunframe().front());
				this->m_Gun[index].Init(this->m_VehDataID, index);
			}
			KeyControl::InitOverride();
		}
		//
		void			VehicleClass::MovePoint(float pxRad, float pyRad, const Vector3DX& pos_t) noexcept {
			this->m_move.mat = Matrix4x4DX::RotAxis(Vector3DX::right(), pxRad) * Matrix4x4DX::RotAxis(Vector3DX::up(), pyRad);
			this->m_move.pos = pos_t;
			this->m_move.repos = pos_t;
			this->m_move.vec.Set(0, 0, 0);
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

			KeyControl::InitKey(this->m_move.mat.inverse().zvec());

			this->m_speedBase = 0.f;
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
			KeyControl::InputKey(pInput, pReady);
			if (isOverrideView) {
				//this->m_view_rad[0].x = (pInput.GetxRad());
				//this->m_view_rad[0].y = (pInput.GetyRad());
			}
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
			auto* DrawParts = DXDraw::Instance();
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
								if (GetMyPlayerID() == 0) {
									DrawParts->SetCamShake(0.5f, 1.f*Scale_Rate);
								}
							}
							else {
								pAmmo->Ricochet(HitPos, HitNormal);	//���e
								if (GetMyPlayerID() == 0) {
									SE->Get((int)SoundEnum::Tank_Ricochet).Play_3D(GetRand(16), HitPos, 100.f*Scale_Rate, 216);
									DrawParts->SetCamShake(0.3f, 0.1f*pAmmo->GetCaliberSize() / 0.10f*Scale_Rate);
								}
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
			this->m_ColActive = false;
			this->m_move.repos = this->m_move.pos;
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
			auto* SE = SoundPool::Instance();
			for (auto& cg : this->m_Gun) {
				auto ID = (int)(&cg - &this->m_Gun.front());
				auto Pos = GetGunMuzzlePos(ID);
				auto Vec = GetGunMuzzleVec(ID);
				float	view_YradAdd{0.f};							//
				float	view_XradAdd{0.f};							//
				if (this->IsAlive()) {
					if (GetInputControl().GetPADSPress(PADS::AIM)) { //�C�����b�N
						view_XradAdd = 0.f;
						view_YradAdd = 0.f;
					}
					else {
						//�_��
						Vector3DX EndPos = Pos - this->GetRadBuf() * (500.f*Scale_Rate);
						this->m_BackGround->CheckLinetoMap(Pos, &EndPos, true, false);
						auto vec_a = (EndPos - GetGunMuzzleBase(ID)).normalized();
						float a_hyp = std::hypotf(vec_a.x, vec_a.z);
						float z_hyp = std::hypotf(Vec.x, Vec.z);
						{
							float cost = Vector3DX::Cross(Vec, vec_a).y / z_hyp;
							float sint = sqrtf(std::abs(1.f - cost * cost));
							view_YradAdd = (std::atan2f(cost, sint)) / 5.f;
						}
						view_XradAdd = (std::atan2f(vec_a.y, a_hyp) - std::atan2f(Vec.y, z_hyp)) / 1.f;
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
				cg.UpdateGunVec(
					std::clamp(view_XradAdd, -limit / 5.f, limit / 5.f)*60.f / DrawParts->GetFps(),
					std::clamp(view_YradAdd, -limit, limit)*60.f / DrawParts->GetFps()
				);
				//�ˌ�
				if (cg.Update(
					(ID == 0) ? GetInputControl().GetPADSPress(PADS::SHOT) : GetInputControl().GetPADSPress(PADS::JUMP)
					, this->m_MyID, Pos, Vec)) {
					SE->Get((int)SoundEnum::Tank_Shot).Play_3D(cg.GetData()->GetShotSound(), this->m_move.pos, 250.f*Scale_Rate);//�T�E���h
					EffectControl::SetOnce(EffectResource::Effect::ef_fire, Pos, Vec, cg.GetCaliberSize() / 0.1f * Scale_Rate);//�e���C�G�t�F�N�g�̃Z�b�g
				}
			}
		}
		//�t���[������
		void			VehicleClass::ExecuteFrame(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			//�C������
			for (auto& g : this->m_Gun) {
				if (g.GetGunBase().GetFrameID() >= 0) {
					auto Mat = Matrix4x4DX::RotAxis(Vector3DX::up(), g.GetGunViewVec().y) *
						GetObj().GetFrameLocalMatrix(GetObj().frame_parent(g.GetGunBase().GetFrameID())).rotation().inverse() *
						Matrix4x4DX::Mtrans(g.GetGunBase().GetFrameLocalPosition().pos());
					GetObj().SetFrameLocalMatrix(g.GetGunBase().GetFrameID(), Mat);
					this->m_col.SetFrameLocalMatrix(g.GetGunBase().GetFrameID(), Mat);
				}
				if (g.GetGunTrunnion().GetFrameID() >= 0) {
					auto Mat = Matrix4x4DX::RotAxis(Vector3DX::right(), g.GetGunViewVec().x) * Matrix4x4DX::Mtrans(g.GetGunTrunnion().GetFrameLocalPosition().pos());
					GetObj().SetFrameLocalMatrix(g.GetGunTrunnion().GetFrameID(), Mat);
					this->m_col.SetFrameLocalMatrix(g.GetGunTrunnion().GetFrameID(), Mat);
				}
				if (g.GetGunMuzzle().GetFrameID() >= 0) {
					auto Mat = Matrix4x4DX::Mtrans(Vector3DX::forward() * (g.GetRecoilPer() * 0.5f * Scale_Rate)) * Matrix4x4DX::Mtrans(g.GetGunMuzzle().GetFrameLocalPosition().pos());
					GetObj().SetFrameLocalMatrix(g.GetGunMuzzle().GetFrameID(), Mat);
					this->m_col.SetFrameLocalMatrix(g.GetGunMuzzle().GetFrameID(), Mat);
				}
				if (g.GetGunBase().GetFrameID() >= 0) {
					auto v1 = Vector3DX::forward();
					auto v2 = GetObj().GetFrameLocalMatrix(g.GetGunBase().GetFrameID()).rotation().zvec();v2.y = 0.f;
					g.UpdateFireReaction_(std::atan2f(Vector3DX::Cross(v1, v2).y, Vector3DX::Dot(v1, v2)));
				}
			}
			//�]��
			{
				if (CanLookTarget) {
					float Len = (DrawParts->GetMainCamera().GetCamPos() - this->m_move.pos).magnitude();
					if (Len < 10.f*Scale_Rate) {
						DIV = 1;
					}
					else if (Len < 20.f*Scale_Rate) {
						DIV = 3;
					}
					else if (Len < 30.f*Scale_Rate) {
						DIV = 5;
					}
					else if (Len < 40.f*Scale_Rate) {
						DIV = 7;
					}
					else {
						DIV = 9;
					}
				}
				else {
					DIV = 11;
				}
				auto y_vec = GetObj().GetMatrix().yvec();
				for (auto& f : this->m_Wheel) {
					float Res_y = 0.f;
					bool IsHit = false;
					int max = (int)f.size();
					for (auto& t : f) {
						auto ID = (int)(&t - &f.front());

						auto localmatBase = t.GetFrame().GetFrameWorldPosition().pos();
						GetObj().frame_Reset(t.GetFrame().GetFrameID());

						auto startpos = GetObj().frame(t.GetFrame().GetFrameID());
						if ((ID % DIV == 0) || (ID == (max - 1))) {
							auto pos_t = startpos - y_vec * localmatBase.y;
							auto pos_t1 = pos_t + y_vec * (2.f*Scale_Rate);
							auto pos_t2 = pos_t + y_vec * (-0.3f*Scale_Rate);
							IsHit = this->m_BackGround->CheckLinetoMap(pos_t1, &pos_t2, true, this->m_MyID != 0);
							if (IsHit) {
								Res_y = pos_t2.y - pos_t.y;
							}
							else {
								Res_y = -0.3f*Scale_Rate;
							}
							t.SetYpos(Res_y);
						}
						t.Update();
					}
				}
				for (auto& f : this->m_Wheel) {
					int max = (int)f.size();
					for (auto& t : f) {
						auto localmatBase = t.GetFrame().GetFrameWorldPosition().pos();

						auto ID = (int)(&t - &f.front());

						GetObj().frame_Reset(t.GetFrame().GetFrameID());
						bool IsLeft = (localmatBase.x >= 0);

						auto startpos = GetObj().frame(t.GetFrame().GetFrameID());
						if (ID != (max - 1)) {
							int Now = (ID / DIV) * DIV;
							int Next = Now + DIV;
							if (Next >= max) {
								Next = max - 1;
							}
							if (Next > Now) {
								t.SetYpos(Lerp(f.at(Now).GetYPos(), f.at(Next).GetYPos(), (float)(ID - Now) / (float)(Next - Now)));
							}
						}
						GetObj().SetFrameLocalMatrix(t.GetFrame().GetFrameID(),
													 Matrix4x4DX::RotAxis(Vector3DX::right(), IsLeft ? this->m_wheel_Left : this->m_wheel_Right) *
													 Matrix4x4DX::Mtrans(localmatBase + Vector3DX::up() * t.GetYPosR())
						);
					}
				}
			}
			for (const auto& f : GetData().Get_wheelframe_nospring()) {
				if (f.GetFrameID() < 0) { continue; }
				GetObj().SetFrameLocalMatrix(f.GetFrameID(), Matrix4x4DX::RotAxis(Vector3DX::right(), (f.GetFrameWorldPosition().pos().x >= 0) ? this->m_wheel_Left : this->m_wheel_Right) * Matrix4x4DX::Mtrans(f.GetFrameWorldPosition().pos()));
			}
		}
		//���실��
		void			VehicleClass::ExecuteMove(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			bool isfloat = GetData().GetIsFloat() && (this->m_move.pos.y == -GetData().GetDownInWater());
			bool isOverride = PutOverride();
			//�X���̎擾
			{
				const auto bNormal = (isfloat) ? Vector3DX::up() : this->m_BodyNormal;
				float yradBody = 0.f;
				if (isOverride) {
					yradBody = GetOverRideInfo().rad.y;
				}
				else {
					auto pp = (this->m_move.mat * Matrix4x4DX::RotVec2(Vector3DX::up(), bNormal).inverse()).zvec() * -1.f;
					yradBody = std::atan2f(pp.x, pp.z);
				}
				this->m_move.mat = Matrix4x4DX::Axis1(bNormal, (Matrix4x4DX::RotAxis(Vector3DX::up(), yradBody)*Matrix4x4DX::RotVec2(Vector3DX::up(), bNormal)).zvec() * -1.f);
				if (isOverride) {
					//��U���̏�Œn�`����
					this->m_move.pos = GetOverRideInfo().pos;
				}
				Easing(&this->m_BodyNormal, (Vector3DX::Cross((GetObj().frame(GetData().Get_square(0)) - GetObj().frame(GetData().Get_square(3))), GetObj().frame(GetData().Get_square(1)) - GetObj().frame(GetData().Get_square(2)))).normalized(), 0.8f, EasingType::OutExpo);
			}
			//�n�ʔ���
			{
				auto pos_ytmp = this->m_move.pos.y;
				float hight_t = 0.f;
				int cnt_t = 0;
				//����
				auto y_vec = GetObj().GetMatrix().yvec();
				for (auto& f : this->m_CatFoot) {
					float Res_y = 0.f;
					bool IsHit = false;
					int max = (int)f.size();
					for (auto& t : f) {
						auto ID = (int)(&t - &f.front());

						if ((ID % DIV == 0) || (ID == (max - 1))) {
							GetObj().frame_Reset(t.GetFrame().GetFrameID());
							auto startpos = GetObj().frame(t.GetFrame().GetFrameID());
							auto localmatBase = t.GetFrame().GetFrameWorldPosition().pos();
							auto pos_t = startpos - y_vec * localmatBase.y;
							auto pos_t1 = pos_t + y_vec * (2.f*Scale_Rate);
							auto pos_t2 = pos_t + y_vec * (-0.3f*Scale_Rate);
							IsHit = m_BackGround->CheckLinetoMap(pos_t1, &pos_t2, true, (this->m_MyID != 0));
							if (IsHit) {
								Res_y = pos_t2.y - pos_t.y;
								hight_t += pos_t2.y;
								cnt_t++;
							}
							else {
								Res_y = -0.4f*Scale_Rate;
							}
							t.SetYpos(Res_y);
						}
						t.Update(&GetObj(), (IsHit) ? this->m_speed : 0.f);
					}
				}
				for (auto& f : this->m_CatFoot) {
					int max = (int)f.size();
					for (auto& t : f) {
						auto localmatBase = t.GetFrame().GetFrameWorldPosition().pos();

						auto ID = (int)(&t - &f.front());
						if (ID != (max - 1)) {
							int Now = (ID / DIV) * DIV;
							int Next = Now + DIV;
							if (Next >= max) {
								Next = max - 1;
							}
							if (Next > Now) {
								t.SetYpos(Lerp(f.at(Now).GetYPos(), f.at(Next).GetYPos(), (float)(ID - Now) / (float)(Next - Now)));
							}
						}
						GetObj().SetFrameLocalMatrix(t.GetFrame().GetFrameID(), Matrix4x4DX::Mtrans(localmatBase + Vector3DX::up() * t.GetYPosR()));
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
					bool CanMove = (this->m_HP_parts[GetData().Get_module_mesh()[0]] > 0) && (this->m_HP_parts[GetData().Get_module_mesh()[1]] > 0);
					bool CanTurn = (this->m_HP_parts[GetData().Get_module_mesh()[0]] > 0) || (this->m_HP_parts[GetData().Get_module_mesh()[1]] > 0);
					//�O�i���
					{
						this->m_speed_add = std::clamp(
							this->m_speed_add + ((GetInputControl().GetPADSPress(PADS::MOVE_W) && CanMove) ? 0.05f : -0.3f) / DrawParts->GetFps(),
							0.f, GetData().GetMaxFrontSpeed());

						this->m_speed_sub = std::clamp(
							this->m_speed_sub + ((GetInputControl().GetPADSPress(PADS::MOVE_S) && CanMove) ? -0.15f : 0.3f) / DrawParts->GetFps(),
							GetData().GetMaxBackSpeed(), 0.f);

						Easing(&this->m_speedBase, this->m_speed_add + this->m_speed_sub, 0.9f, EasingType::OutExpo);
						auto vec = this->m_move.mat.zvec() * (-1.f * this->m_speedBase*Scale_Rate);
						this->m_move.vec.x = vec.x;
						if ((cnt_t - cnt_sq) >= 3) {
							this->m_move.vec.y = vec.y;
						}
						else {
							this->m_move.vec.y += (M_GR / (DrawParts->GetFps() * DrawParts->GetFps()));
						}
						this->m_move.vec.z = vec.z;
					}
					//����
					{
						const auto radold = this->m_radAdd;
						this->m_yradadd_left = (GetInputControl().GetPADSPress(PADS::MOVE_D) && CanTurn) ? std::max(this->m_yradadd_left - deg2rad(1.f * (60.f / DrawParts->GetFps())), deg2rad(-GetData().GetMaxBodyRad())) : std::min(this->m_yradadd_left + deg2rad(2.f * (60.f / DrawParts->GetFps())), 0.f);
						this->m_yradadd_right = (GetInputControl().GetPADSPress(PADS::MOVE_A) && CanTurn) ? std::min(this->m_yradadd_right + deg2rad(1.f * (60.f / DrawParts->GetFps())), deg2rad(GetData().GetMaxBodyRad())) : std::max(this->m_yradadd_right - deg2rad(2.f * (60.f / DrawParts->GetFps())), 0.f);
						this->m_radAdd.y = ((this->m_yradadd_left + this->m_yradadd_right) / DrawParts->GetFps());
						//����
						this->m_radAdd.x = (deg2rad(-this->m_speed * 1800.f));
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
				this->m_move.mat *=
					Matrix4x4DX::RotAxis(this->m_move.mat.xvec(), cg.GetFireReactionVec().x) *
					Matrix4x4DX::RotAxis(this->m_move.mat.zvec(), cg.GetFireReactionVec().z);
			}
			//�ړ�
			if (isOverride) {
				this->m_move.pos = GetOverRideInfo().pos;
				this->m_move.vec = GetOverRideInfo().vec;
			}
			else {
				this->m_move.pos += this->m_move.vec;
				//����
				if (GetData().GetIsFloat()) {
					this->m_move.pos.y = (std::max(this->m_move.pos.y, -GetData().GetDownInWater()));
				}
			}
			//�]��
			this->m_wheel_Left += (-(this->m_speedBase * 2.f - this->m_radAdd.y * 5.f));
			this->m_wheel_Right += (-(this->m_speedBase * 2.f + this->m_radAdd.y * 5.f));
			//��ԕǔ���
			if (isOverride) {
				this->m_b2mine.SetTransform(b2Vec2(this->m_move.pos.x, this->m_move.pos.z), Get_body_yrad());
			}
			else {
				this->m_b2mine.Execute(this->m_move.vec, this->m_radAdd.y);
			}
		}
		//SetMat�w���X�V
		void			VehicleClass::ExecuteMatrix(void) noexcept {
			//��ԍ��W���f
			this->m_move.mat *= Matrix4x4DX::RotAxis(Vector3DX::up(), -this->m_b2mine.Rad() - Get_body_yrad());
			this->m_move.pos = Vector3DX::vget(this->m_b2mine.Pos().x, this->m_move.pos.y, this->m_b2mine.Pos().y);
			UpdateMove();
			this->m_speed = (this->m_move.pos - this->m_move.repos).magnitude() / Scale_Rate;
			if (this->m_speedBase < 0.f) {
				this->m_speed *= -1.f;
			}
			//�]��
			b2Vec2 Gravity2D = b2Vec2(
				(-9.8f / 60.f) * (Vector3DX::Dot(this->m_move.mat.zvec(), Vector3DX::up())),
				(-9.8f / 60.f) * (Vector3DX::Dot(this->m_move.mat.yvec(), Vector3DX::up())));

			if (CanLookTarget) {
				for (auto& f : this->m_b2Foot) {
					f.LateExecute(&f == &this->m_b2Foot.front(), &GetData(), &GetObj(), Gravity2D);
				}
			}
			this->m_Hit_active.Execute(GetObj());
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
			auto base = KeyControl::GetRadBuf()*-1.f;
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
