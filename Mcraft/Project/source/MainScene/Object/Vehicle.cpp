#pragma once
#include "Vehicle.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Objects {
		//----------------------------------------------------------
		//�������֘A
		//----------------------------------------------------------
		//
		void			VehicleObj::ValueInit(const VhehicleData* pVeh_data) noexcept {
			this->m_VecData = pVeh_data;
			GetObj().SetMaterialDrawAlphaTestAll(true, DX_CMP_GREATER, 128);								//�{��
			this->hitres.resize(GetCol().GetMeshNum());											//���W���[�����Ƃ̓����蔻�茋�ʂ��m��
			this->hitssort.resize(GetCol().GetMeshNum());											//���W���[�����Ƃ̓����蔻�菇�����m��
			for (auto& f : this->m_b2Foot) {
				f.Init(&f == &this->m_b2Foot.front(), this->m_VecData);
			}
			this->m_wheel_frameYpos.resize(this->m_VecData->Get_wheelframe().size());
			//�C
			this->m_Gun.resize(this->m_VecData->Get_gunframe().size());
			this->m_view_rad.resize(this->m_Gun.size());
			for (const auto& g : this->m_VecData->Get_gunframe()) {
				this->m_Gun[&g - &this->m_VecData->Get_gunframe().front()].Init(&g);
			}
		}
		//
		void			VehicleObj::ValueSet(float pxRad, float pyRad, const Vector3DX& pos_t) noexcept {
			SetMove().SetMat(Matrix3x3DX::RotAxis(Vector3DX::right(), pxRad) * Matrix3x3DX::RotAxis(Vector3DX::up(), pyRad));
			SetMove().SetPos(pos_t);
			SetMove().SetVec(Vector3DX::zero());
			for (auto& w : this->m_wheel_frameYpos) { w = 0.f; }
			//��ԃX�|�[��
			this->m_PosBufOverRideFlag = false;

			this->m_MouseVec = GetMove().GetMat().inverse();

			this->m_speed = 0.f;
			this->m_speed_add = 0.f;
			this->m_speed_sub = 0.f;
			this->m_wheel_Left = 0.f;
			this->m_wheel_Right = 0.f;
			this->m_Tilt = Vector3DX::zero();
			this->m_radAdd = Vector3DX::zero();
			this->m_yradadd_left = 0.f;
			this->m_yradadd_right = 0.f;
		}
		//
		void			VehicleObj::SetInput(const InputControl& pInput, bool pReady) noexcept {
			//�G�C��
			auto y_mouse = std::atan2f(this->m_MouseVec.zvec().x, this->m_MouseVec.zvec().z);
			auto x_mouse = std::atan2f(-this->m_MouseVec.zvec().y, std::hypotf(this->m_MouseVec.zvec().x, this->m_MouseVec.zvec().z));
			this->m_MouseVec = Matrix3x3DX::RotAxis(Vector3DX::right(), std::clamp(x_mouse + pInput.GetAddxRad(), -deg2rad(40.f), deg2rad(40.f))) * Matrix3x3DX::RotAxis(Vector3DX::up(), y_mouse + pInput.GetAddyRad());
			//
			if (this->m_view_override) {
				this->m_view_rad[0].x = pInput.GetxRad();
				this->m_view_rad[0].y = pInput.GetyRad();
			}
			if (pReady) {
				this->m_key[0] = pInput.GetPADSPress(Controls::PADS::SHOT);			//�ˌ�
				this->m_key[1] = pInput.GetPADSPress(Controls::PADS::ULT);			//�}�V���K��
				this->m_key[2] = pInput.GetPADSPress(Controls::PADS::MOVE_W);		//�O�i
				this->m_key[3] = pInput.GetPADSPress(Controls::PADS::MOVE_S);		//���
				this->m_key[4] = pInput.GetPADSPress(Controls::PADS::MOVE_D);		//�E
				this->m_key[5] = pInput.GetPADSPress(Controls::PADS::MOVE_A);		//��
				this->m_key[6] = pInput.GetPADSPress(Controls::PADS::AIM);			//��
			}
			else {
				for (auto& k : this->m_key) {
					k = false;
				}
			}
		}
		//�J�����ݒ�o��
		void			VehicleObj::Setcamera(Camera3DInfo& MainCamera_t, const float fov_base) noexcept {
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			auto fov_t = MainCamera_t.GetCamFov();
			auto near_t = MainCamera_t.GetCamNear();
			auto far_t = MainCamera_t.GetCamFar();

			Vector3DX eyepos = Get_EyePos_Base();
			auto OLD = this->m_range;

			if (is_ADS()) {
				this->m_ratio = std::clamp(this->m_ratio + float(PadControl::Instance()->GetMouseWheelRot()) * 2.0f, 0.0f, 30.f);
				if (this->m_ratio == 0.f) {
					this->m_range = 0.f + this->m_range_change;
					this->m_range_r = this->m_range;
				}
				else {
					Easing(&fov_t, fov_base / this->m_ratio, 0.9f, EasingType::OutExpo);
				}
			}
			else {
				eyepos += this->m_MouseVec.zvec() * this->m_range_r*Scale3DRate;
				Vector3DX eyepos2 = eyepos + this->m_MouseVec.zvec() * -1.f * (this->m_range_r * Scale3DRate);
				if (BackGroundParts->CheckLinetoMap(eyepos, &eyepos2)) { eyepos = eyepos2; }

				this->m_ratio = 2.0f;
				this->m_range = std::clamp(this->m_range - float(PadControl::Instance()->GetMouseWheelRot()) * this->m_range_change, 0.f, 4.f);
				Easing(&this->m_range_r, this->m_range, 0.8f, EasingType::OutExpo);

				Easing(&fov_t, fov_base, 0.9f, EasingType::OutExpo);
			}
			this->m_changeview = ((this->m_range != OLD) && (this->m_range == 0.f || OLD == 0.f));

			MainCamera_t.SetCamPos(
				eyepos,
				Get_EyePos_Base() + this->m_MouseVec.zvec() * -1.f * std::max(this->m_range_r * Scale3DRate, 1.f),
				Lerp(GetMove().GetMat().yvec(), Vector3DX::up(), std::clamp(this->m_range_r / 3.f, 0.f, 1.f))
			);

			Easing(&near_t, 1.f + 2.f*((is_ADS()) ? this->m_ratio : 2.f), 0.9f, EasingType::OutExpo);
			Easing(&far_t, std::max(this->m_AimingDistance, Scale3DRate * 50.f) + Scale3DRate * 50.f, 0.9f, EasingType::OutExpo);
			MainCamera_t.SetCamInfo(fov_t, near_t, far_t);
		}

		//----------------------------------------------------------
		//�X�V�֘A
		//----------------------------------------------------------

		//��e�`�F�b�N
		const auto		VehicleObj::CheckAmmoHited(const Objects::AmmoObj& pAmmo) noexcept {
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
			MV1_COLL_RESULT_POLY colres{}; colres.HitFlag = FALSE;
			Vector3DX EndPos = pAmmo.GetMove().GetPos();
			for (const auto& m : this->m_VecData->Get_armer_mesh()) { HitCheck_Tank(m.first, pAmmo.GetMove().GetRePos(), colres); }//�S���Z�b�g
			for (const auto& m : this->m_VecData->Get_armer_mesh()) {
				auto colres_t = GetColLine(pAmmo.GetMove().GetRePos(), EndPos, m.first);
				if (colres_t.HitFlag == TRUE) {
					t = m.first;
					colres = colres_t;
					EndPos = colres_t.HitPosition;
				}
			}
			//
			for (auto& m : this->m_VecData->Get_module_mesh()) {//���W���[��
				is_Hit |= HitCheck_Tank(m, pAmmo.GetMove().GetRePos(), GetColLine(pAmmo.GetMove().GetRePos(), EndPos, m));
			}
			for (auto& m : this->m_VecData->Get_space_mesh()) {//��ԑ��b
				is_Hit |= HitCheck_Tank(m, pAmmo.GetMove().GetRePos(), GetColLine(pAmmo.GetMove().GetRePos(), EndPos, m));
			}
			//
			if (t != -1) {
				is_Hit |= HitCheck_Tank(t, pAmmo.GetMove().GetRePos(), colres);
			}
			return is_Hit;
		}
		//��e����
		const auto		VehicleObj::CalcAmmoHited(float EffectSize) noexcept {
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

					for (auto& mesh : this->m_VecData->Get_space_mesh()) {
						if (tt.GetHitMesh() == mesh) {
							//��ԑ��b�ɓ��������̂Ń��W���[����30�_��
							//EffectSingleton::Instance()->SetOnce(Effect::ef_gndsmoke, HitPos, HitNormal, EffectSize);
						}
					}
					for (auto& mesh : this->m_VecData->Get_module_mesh()) {
						if (tt.GetHitMesh() == mesh) {
							//���W���[���ɓ��������̂Ń��W���[����30�_��
							//EffectSingleton::Instance()->SetOnce(Effect::ef_gndsmoke, HitPos, HitNormal, EffectSize);
						}
					}
					//�_���[�W�ʂɓ�������
					for (auto& mesh : this->m_VecData->Get_armer_mesh()) {
						if (tt.GetHitMesh() == mesh.first) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_Ricochet))->Play3D(HitPos, 100.f * Scale3DRate);
							//�G�t�F�N�g�Z�b�g
							//EffectSingleton::Instance()->SetOnce(Effect::ef_gndsmoke, HitPos, HitNormal, EffectSize * 10.f);
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
		void			VehicleObj::ExecuteSavePrev(void) noexcept {
		}
		//���̑�
		void			VehicleObj::ExecuteElse(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			//�G���W����
			if (this->m_engine_time <= 0.f) {

				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_engine))->Play3D(GetMove().GetPos(), 50.f * Scale3DRate);//, DX_PLAYTYPE_LOOP
				this->m_engine_time = 0.9f;
			}
			else {
				this->m_engine_time = std::max(this->m_engine_time - 1.f * DXLib_refParts->GetDeltaTime(), 0.f);
			}
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_move))->SetPosition(GetMove().GetPos());
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_move))->SetLocalVolume(
				std::clamp((int)(255.f * (std::abs(GetMove().GetVec().magnitude() / Scale3DRate) * 0.75f + std::abs(this->m_radAdd.y) * 8.f) * 10.f), 0, 255)
			);
		}
		//����
		void			VehicleObj::ExecuteInput(void) noexcept {
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			if (!this->m_view_override) {
				float	view_YradAdd{ 0.f };							//
				float	view_XradAdd{ 0.f };							//
				for (int i = 0; i < this->m_view_rad.size(); i++) {
					if (this->m_key[6]) { //�C�����b�N
						view_XradAdd = 0.f;
						view_YradAdd = 0.f;
					}
					else {
						//�_��
						Vector3DX MuzPos = GetGunMuzzlePos(i);
						Vector3DX EndPos = MuzPos + (this->m_MouseVec.zvec() * -1.f).normalized() * (500.f * Scale3DRate);
						BackGroundParts->CheckLinetoMap(MuzPos, &EndPos);

						Vector3DX BasePos = GetGunMuzzleBase(i);
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
						if (i == 0) {
							//printfDx("%.2f\n", rad2deg(view_XradAdd));
						}
					}

					float limit = this->m_VecData->GetMaxTurretRad() * DXLib_refParts->GetDeltaTime();
					if (i > 0) {
						limit *= 2.f;
					}
					this->m_view_rad[i].x = (std::clamp(this->m_view_rad[i].x + std::clamp(view_XradAdd, -limit / 5.f, limit / 5.f), -deg2rad(10.f), deg2rad(40.f)));
					this->m_view_rad[i].y += (std::clamp(view_YradAdd, -limit, limit));
				}
			}
			this->m_view_override = false;
		}
		//�t���[������
		void			VehicleObj::ExecuteFrame(void) noexcept {
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			//�C������
			for (auto& g : this->m_Gun) {
				auto ID = &g - &this->m_Gun.front();
				if (ID == 0) {
					g.ExecuteGunFrame(this->m_view_rad[ID].x, this->m_view_rad[ID].y, &SetObj(), &SetCol());
				}
				else {
					g.ExecuteGunFrame(this->m_view_rad[ID].x, this->m_view_rad[ID].y - this->m_view_rad[0].y, &SetObj(), &SetCol());
				}
			}
			//�]��
			auto y_vec = GetObj().GetMatrix().yvec();
			for (auto& f : this->m_VecData->Get_wheelframe()) {
				if (f.GetFrameID() >= 0) {
					auto ID = &f - &this->m_VecData->Get_wheelframe().front();
					GetObj().ResetFrameUserLocalMatrix(f.GetFrameID());

					auto startpos = GetObj().GetFramePosition(f.GetFrameID());
					auto pos_t1 = startpos + y_vec * ((-f.GetFrameWorldPosition().pos().y) + 2.f*Scale3DRate);
					auto pos_t2 = startpos + y_vec * ((-f.GetFrameWorldPosition().pos().y) - 0.3f*Scale3DRate);
					auto ColRes = BackGroundParts->CheckLinetoMap(pos_t1, &pos_t2);
					Easing(&this->m_wheel_frameYpos[ID], (ColRes) ? (pos_t2.y + y_vec.y * f.GetFrameWorldPosition().pos().y - startpos.y) : -0.3f*Scale3DRate, 0.9f, EasingType::OutExpo);
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
		//���실��
		void			VehicleObj::ExecuteMove(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			//�X���̎擾
			{
				float yradBody = 0.f;
				if (this->m_PosBufOverRideFlag) {
					yradBody = this->m_RadOverRide.y;
				}
				else {
					auto pp = (GetMove().GetMat() * Matrix3x3DX::RotVec2(Vector3DX::up(), this->m_BodyNormal).inverse()).zvec() * -1.f;
					yradBody = std::atan2f(pp.x, pp.z);
				}
				SetMove().SetMat(Matrix3x3DX::Axis1(this->m_BodyNormal, (Matrix3x3DX::RotAxis(Vector3DX::up(), yradBody) * Matrix3x3DX::RotVec2(Vector3DX::up(), this->m_BodyNormal)).zvec() * -1.f));
				SetMove().Update(0.f, 0.f);
				if (this->m_PosBufOverRideFlag) {
					//��U���̏�Œn�`����
					SetMove().SetPos(this->m_PosBufOverRide);
					SetMove().Update(0.f, 0.f);
					UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				}
				Easing(&this->m_BodyNormal,
					Vector3DX::Cross(
						GetObj().GetFramePosition(this->m_VecData->Get_square(0)) - GetObj().GetFramePosition(this->m_VecData->Get_square(3)),
						GetObj().GetFramePosition(this->m_VecData->Get_square(1)) - GetObj().GetFramePosition(this->m_VecData->Get_square(2))
					).normalized(),
					0.8f, EasingType::OutExpo);
			}
			//�n�ʔ���
			{
				auto pos_ytmp = GetMove().GetPos().y;
				float hight_t = 0.f;
				int cnt_t = 0;
				//����
				for (auto& f : this->m_b2Foot) {
					f.FirstExecute(&SetObj());
					for (const auto& t : f.Getdownsideframe()) {
						if (t.GetColResult_Y() != (std::numeric_limits<float>::max)()) {
							hight_t += t.GetColResult_Y();
							cnt_t++;
						}
					}
				}
				int cnt_sq = cnt_t;
				for (const auto& s : this->m_VecData->Get_square()) {
					auto p_t = GetObj().GetFramePosition(s);
					auto pos_t1 = p_t + (Vector3DX::up() * 3.f*Scale3DRate);
					auto pos_t2 = p_t + (Vector3DX::up() * -0.5f*Scale3DRate);
					if (BackGroundParts->CheckLinetoMap(pos_t1, &pos_t2)) {
						hight_t += pos_t2.y;
						cnt_t++;
					}
				}
				if (cnt_t > 0) {
					Easing(&pos_ytmp, (hight_t / cnt_t), 0.9f, EasingType::OutExpo);
				}
				Vector3DX Pos = GetMove().GetPos();
				Pos.y = pos_ytmp;
				SetMove().SetPos(Pos);
				//�n��or���ʂɂ��邩�ǂ���
				if ((cnt_t > 0)) {
					//�O�i���
					{
						const auto spdold = this->m_speed_add + this->m_speed_sub;
						this->m_speed_add = (this->m_key[2]) ? std::min(this->m_speed_add + (0.15f / 3.6f) * (60.f * DXLib_refParts->GetDeltaTime()), (this->m_VecData->GetMaxFrontSpeed() / 3.6f)) : std::max(this->m_speed_add - (0.7f / 3.6f) * (60.f * DXLib_refParts->GetDeltaTime()), 0.f);
						this->m_speed_sub = (this->m_key[3]) ? std::max(this->m_speed_sub - (0.15f / 3.6f) * (60.f * DXLib_refParts->GetDeltaTime()), (this->m_VecData->GetMaxBackSpeed() / 3.6f)) : std::min(this->m_speed_sub + (0.7f / 3.6f) * (60.f * DXLib_refParts->GetDeltaTime()), 0.f);
						this->m_speed = (spdold + ((this->m_speed_add + this->m_speed_sub) - spdold) * 0.1f) * DXLib_refParts->GetDeltaTime();
						auto vec = GetMove().GetMat().zvec() * -1.f * (this->m_speed*Scale3DRate);
						if ((cnt_t - cnt_sq) < 3) {
							vec.y = GetMove().GetVec().y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));
						}
						SetMove().SetVec(vec);
					}
					//����
					{
						const auto radold = this->m_radAdd;
						this->m_yradadd_left = (this->m_key[4]) ? std::max(this->m_yradadd_left - deg2rad(1.f * (60.f * DXLib_refParts->GetDeltaTime())), deg2rad(-this->m_VecData->GetMaxBodyRad())) : std::min(this->m_yradadd_left + deg2rad(2.f * (60.f * DXLib_refParts->GetDeltaTime())), 0.f);
						this->m_yradadd_right = (this->m_key[5]) ? std::min(this->m_yradadd_right + deg2rad(1.f * (60.f * DXLib_refParts->GetDeltaTime())), deg2rad(this->m_VecData->GetMaxBodyRad())) : std::max(this->m_yradadd_right - deg2rad(2.f * (60.f * DXLib_refParts->GetDeltaTime())), 0.f);
						this->m_radAdd.y = (this->m_yradadd_left + this->m_yradadd_right) * DXLib_refParts->GetDeltaTime();
						//����
						this->m_radAdd.x = (deg2rad(-(this->m_speed / (60.f * DXLib_refParts->GetDeltaTime())) / (0.1f / 3.6f) * 50.f));
						this->m_radAdd.z = (deg2rad(-this->m_radAdd.y / (deg2rad(5.f) * DXLib_refParts->GetDeltaTime()) * 5.f));
						Easing(&this->m_Tilt, Vector3DX::vget(std::clamp(this->m_radAdd.x - radold.x, deg2rad(-30.f), deg2rad(30.f)), 0.f, std::clamp(this->m_radAdd.z - radold.z, deg2rad(-15.f), deg2rad(15.f))), 0.95f, EasingType::OutExpo);
						SetMove().SetMat(GetMove().GetMat() * Matrix3x3DX::RotAxis(GetMove().GetMat().xvec(), -this->m_Tilt.x) * Matrix3x3DX::RotAxis(GetMove().GetMat().zvec(), this->m_Tilt.z) );
						SetMove().Update(0.f, 0.f);
					}
					//
				}
				else {
					auto vec = GetMove().GetMat().zvec() * -1.f * (this->m_speed * Scale3DRate);
					vec.y = GetMove().GetVec().y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));
					SetMove().SetVec(vec);
				}
			}
			//�ˌ�����
			for (auto& cg : this->m_Gun) {
				int index = static_cast<int>(&cg - &this->m_Gun.front());
				Matrix3x3DX Mat = GetMove().GetMat();
				cg.FireReaction(&Mat);
				SetMove().SetMat(Mat);
				//�ˌ�
				if (cg.Execute(this->m_key[(index == 0) ? 0 : 1])) {
					SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_Shot))->Play3D(GetMove().GetPos(), 250.f*Scale3DRate);													//�T�E���h
					//EffectSingleton::Instance()->SetOnce(Effect::ef_fire2, GetGunMuzzlePos(index), GetGunMuzzleVec(index), cg.GetCaliberSize() / 0.1f * Scale3DRate);	//�e���C�G�t�F�N�g�̃Z�b�g
					auto LastAmmo = std::make_shared<Objects::AmmoObj>();
					ObjectManager::Instance()->InitObject(LastAmmo);
					LastAmmo->Put(&Objects::AmmoDataManager::Instance()->Get(cg.GetAmmoSpec(0)), GetGunMuzzlePos(index), GetGunMuzzleVec(index), -1);
				}
			}
			//�ړ�
			if (this->m_PosBufOverRideFlag) {
				SetMove().SetPos(this->m_PosBufOverRide);
				SetMove().SetVec(this->m_VecBufOverRide);
			}
			else {
				SetMove().SetPos(GetMove().GetPos() + GetMove().GetVec());
			}
			//�]��
			this->m_wheel_Left += (-(this->m_speed * 2.f - this->m_radAdd.y * 5.f));
			this->m_wheel_Right += (-(this->m_speed * 2.f + this->m_radAdd.y * 5.f));

			//�]��
			for (auto& f : this->m_b2Foot) {
				f.LateExecute(
					&SetObj(),
					(GetMove().GetPos() - GetMove().GetRePos()).magnitude() * 60.f * DXLib_refParts->GetDeltaTime());
			}

			SetMove().SetMat(GetMove().GetMat()* Matrix3x3DX::RotAxis(Vector3DX::up(), -this->m_radAdd.y));

			SetMove().Update(0.f, 0.f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
			this->m_PosBufOverRideFlag = false;

			//�j��
			{
				auto* BackGroundParts = BackGround::BackGroundControl::Instance();
				int								xput = 24;
				int								yput = 8;
				int								zput = 24;
				auto Put = BackGroundParts->GetPoint(GetMove().GetPos());
				bool IsBreak = false;
				for (int xp = -xput / 2; xp < xput / 2; ++xp) {
					for (int yp = 0; yp < yput; ++yp) {
						for (int zp = -zput / 2; zp < zput / 2; ++zp) {
							auto& cell = BackGroundParts->GetCellBuf((Put.x + xp), (Put.y + yp), (Put.z + zp));
							if (cell.GetCell() == 0) {
								continue;
							}
							if (cell.GetCell() == 1) {
								continue;
							}
							BackGroundParts->SetBlick((Put.x + xp), (Put.y + yp), (Put.z + zp), BackGround::s_EmptyBlick);
							IsBreak = true;
						}
					}
				}
				if (IsBreak) {
					BackGroundParts->UpdateOnce();
				}
			}
		}
		//�`�拤��
		void			VehicleObj::DrawCommon(void) noexcept {
			if (!is_ADS()) {
				if (CheckCameraViewClip_Box(
					(this->GetMove().GetPos() + Vector3DX::vget(-5, -5, -5)*Scale3DRate).get(),
					(this->GetMove().GetPos() + Vector3DX::vget(5, 5, 5)*Scale3DRate).get()) == FALSE
					) {

					//GetCol().DrawModel();
					//return;

					if (true) {
						MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_wheel_Left * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
						GetObj().DrawMesh(0);
						MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_wheel_Right * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
						GetObj().DrawMesh(1);
						MV1ResetFrameTextureAddressTransform(GetObj().get(), 0);
						GetObj().DrawMesh(2);
						for (int i = 2; i < GetObj().GetMeshNum(); i++) {
							GetObj().DrawMesh(i);
						}
						//GetCol().DrawModel();
					}
					else {
						GetObj().DrawModel();
					}
				}
			}
		}
		//
		const bool		VehicleObj::CheckLine(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) noexcept {
			bool IsHit = false;
			if (this->RefreshCol(StartPos, *EndPos, 10.0f*Scale3DRate)) {
				//���b(��ԋ߂��ʒu�̂��̂Ɍ��肷��)
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
		const std::pair<bool, bool>		VehicleObj::CheckAmmoHit(Objects::AmmoObj* pAmmo) noexcept {
			std::pair<bool, bool> isDamaged{ false,false };
			if (this->RefreshCol(pAmmo->GetMove().GetRePos(), pAmmo->GetMove().GetPos(), 10.0f * Scale3DRate)) {
				if (this->CheckAmmoHited(*pAmmo)) {									//�Ƃ肠���������������ǂ����T��
					isDamaged.first = true;
					if (this->CalcAmmoHited(1.f)) {
						isDamaged.second = true;
					}
				}
			}
			return isDamaged;
		}
	};
};
