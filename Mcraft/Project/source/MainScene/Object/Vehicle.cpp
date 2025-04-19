#pragma once
#include "Vehicle.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Objects {
		//��e����
		const bool		VehicleObj::CheckAmmoHit(const Vector3DX& StartPos, Vector3DX* EndPos) noexcept {
			auto* SE = SoundPool::Instance();
			//�Ƃ肠���������������ǂ����T��
			if (!RefreshCol(StartPos, *EndPos, 10.0f * Scale3DRate)) {
				return false;
			}
			{
				auto HitCheck_Tank = [&](int m, const Vector3DX& ray_repos, const MV1_COLL_RESULT_POLY& hitres) {
					this->m_hitres[m] = hitres;
					if (this->m_hitres[m].HitFlag == TRUE) {
						this->m_hitssort[m].Set(m, (ray_repos - this->m_hitres[m].HitPosition).magnitude());
						return true;
					}
					else {
						this->m_hitssort[m].Set(m);
						return false;
					}
					};
				bool is_Hit = false;
				//���b(��ԋ߂��ʒu�̂��̂Ɍ��肷��)
				int t = InvalidID;
				MV1_COLL_RESULT_POLY colres{}; colres.HitFlag = FALSE;
				for (const auto& m : this->m_VecData->GetArmerMeshIDList()) { HitCheck_Tank(m.first, StartPos, colres); }//�S���Z�b�g
				for (const auto& m : this->m_VecData->GetArmerMeshIDList()) {
					auto colres_t = GetColLine(StartPos, *EndPos, m.first);
					if (colres_t.HitFlag == TRUE) {
						t = m.first;
						colres = colres_t;
						*EndPos = colres_t.HitPosition;
					}
				}
				//
				for (auto& m : this->m_VecData->GetModuleMeshIDList()) {//���W���[��
					is_Hit |= HitCheck_Tank(m, StartPos, GetColLine(StartPos, *EndPos, m));
				}
				for (auto& m : this->m_VecData->GetSpaceArmerMeshIDList()) {//��ԑ��b
					is_Hit |= HitCheck_Tank(m, StartPos, GetColLine(StartPos, *EndPos, m));
				}
				//
				if (t != InvalidID) {
					is_Hit |= HitCheck_Tank(t, StartPos, colres);
				}
				if (!is_Hit) {
					return false;
				}
			}
			//�����蔻����߂����Ƀ\�[�g
			std::sort(this->m_hitssort.begin(), this->m_hitssort.end(), [](const HitSortInfo& x, const HitSortInfo& y) { return x < y; });
			//�_���[�W�ʂɓ͂��܂Ŕ���
			for (const auto& tt : this->m_hitssort) {
				if (tt.IsHit()) {
					if (tt.GetHitMesh() >= this->m_hitres.size()) { continue; }
					Vector3DX HitPos = this->m_hitres[tt.GetHitMesh()].HitPosition;
					Vector3DX HitNormal = this->m_hitres[tt.GetHitMesh()].Normal;
					HitNormal = HitNormal.normalized();

					for (auto& mesh : this->m_VecData->GetSpaceArmerMeshIDList()) {
						if (tt.GetHitMesh() == mesh) {
							EffectSingleton::Instance()->SetOnce(Effect::ef_gndsmoke, HitPos, HitNormal, 1.f);
						}
					}
					for (auto& mesh : this->m_VecData->GetModuleMeshIDList()) {
						if (tt.GetHitMesh() == mesh) {
							EffectSingleton::Instance()->SetOnce(Effect::ef_gndsmoke, HitPos, HitNormal, 1.f);
						}
					}
					//�_���[�W�ʂɓ�������
					for (auto& mesh : this->m_VecData->GetArmerMeshIDList()) {
						if (tt.GetHitMesh() == mesh.first) {
							SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_Ricochet))->Play3D(HitPos, 100.0f * Scale3DRate);
							EffectSingleton::Instance()->SetOnce(Effect::ef_gndsmoke, HitPos, HitNormal, 2.0f);
							break;
						}
					}
				}
				else {
					break;
				}
			}
			return true;
		}
		//----------------------------------------------------------
		//�X�V�֘A
		//----------------------------------------------------------
		void			VehicleObj::Init_Sub(void) noexcept {
			this->m_VecData = std::make_unique<VhehicleData>(GetObj(), GetCol(), GetFilePath().c_str());
			for (auto& LR : this->m_CrawlerFrame) {
				const auto& w = this->m_VecData->GetCrawlerFrameList().at(&LR - &this->m_CrawlerFrame.front());
				LR.resize(w.size());
				for (auto& t : LR) {
					t.Init(w.at(&t - &LR.front()));
				}
			}
			this->m_WheelHeight.resize(this->m_VecData->GetWheelFrameList().size());
			//�C
			this->m_Gun.resize(this->m_VecData->GetGunData().size());
			for (auto& g : this->m_Gun) {
				g.Init(&this->m_VecData->GetGunData().at(&g - &this->m_Gun.front()));
			}

			SetActive(true);
			SetMinAABB(Vector3DX::vget(-10.0f, -1.0f, -10.0f) * Scale3DRate);
			SetMaxAABB(Vector3DX::vget(10.0f, 10.0f, 10.0f) * Scale3DRate);
			GetObj().SetMaterialDrawAlphaTestAll(true, DX_CMP_GREATER, 128);
			this->m_hitres.resize(GetCol().GetMeshNum());											//���W���[�����Ƃ̓����蔻�茋�ʂ��m��
			this->m_hitssort.resize(GetCol().GetMeshNum());											//���W���[�����Ƃ̓����蔻�菇�����m��
		}
		void			VehicleObj::FirstExecute(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			//����̂ݍX�V������e
			if (IsFirstLoop()) {
				this->m_engine_time = (float)GetRand(100) / 100.0f;
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_move))->Play3D(GetMove().GetPos(), 30.0f * Scale3DRate, DX_PLAYTYPE_LOOP);
			}
			//�G���W����
			if (this->m_engine_time <= 0.0f) {
				this->m_engine_time += 0.9f;
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_engine))->Play3D(GetMove().GetPos(), 30.0f * Scale3DRate);//, DX_PLAYTYPE_LOOP
			}
			this->m_engine_time = std::max(this->m_engine_time - DXLib_refParts->GetDeltaTime(), 0.0f);

			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_move))->SetPosition(GetMove().GetPos());
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_move))->SetLocalVolume(
				std::clamp((int)(128.0f * (std::abs(GetMove().GetVec().magnitude() / DXLib_refParts->GetDeltaTime()) * 0.005f + std::abs(this->m_radAdd.y) * 0.1f) * 10.0f), 0, 128)
			);
			//�_��
			for (auto& g : this->m_Gun) {
				Vector3DX StartPos = GetObj().GetFramePosition(g.GetGunMuzzleFrame().GetFrameID());
				Vector3DX EndPos = StartPos + (this->m_MouseVec.zvec() * -1.0f).normalized() * (500.0f * Scale3DRate);//�_���ꏊ
				Vector3DX BasePos = GetObj().GetFramePosition(g.GetGunTrunnionFrame().GetFrameID());
				g.UpdateAim((StartPos - BasePos).normalized(), (EndPos - BasePos).normalized(), this->m_VecData);
			}
			//�C������
			for (auto& g : this->m_Gun) {
				frames id;
				id = g.GetGunTurretFrame();
				if (id.GetFrameID() >= 0) {
					auto RotateY = Matrix4x4DX::RotAxis(Vector3DX::up(), g.GetShotTargetRad().y);
					SetObj().SetFrameLocalMatrix(id.GetFrameID(), RotateY * id.GetFrameLocalPosition());
					SetCol().SetFrameLocalMatrix(id.GetFrameID(), RotateY * id.GetFrameLocalPosition());
				}
				id = g.GetGunTrunnionFrame();
				if (id.GetFrameID() >= 0) {
					auto RotateX = Matrix4x4DX::RotAxis(Vector3DX::right(), g.GetShotTargetRad().x);
					SetObj().SetFrameLocalMatrix(id.GetFrameID(), RotateX * id.GetFrameLocalPosition());
					SetCol().SetFrameLocalMatrix(id.GetFrameID(), RotateX * id.GetFrameLocalPosition());
				}
				id = g.GetGunMuzzleFrame();
				if (id.GetFrameID() >= 0) {
					auto Recoil = Matrix4x4DX::Mtrans(Vector3DX::vget(0.f, 0.f, g.GetRecoil() * 0.5f * Scale3DRate));
					SetObj().SetFrameLocalMatrix(id.GetFrameID(), Recoil * id.GetFrameLocalPosition());
					SetCol().SetFrameLocalMatrix(id.GetFrameID(), Recoil * id.GetFrameLocalPosition());
				}
			}
			//�]��
			for (auto& Y : this->m_WheelHeight) {
				auto index = static_cast<int>(&Y - &this->m_WheelHeight.front());
				auto& f = this->m_VecData->GetWheelFrameList().at(index);
				if (f.GetFrameID() >= 0) {
					SetObj().ResetFrameUserLocalMatrix(f.GetFrameID());
					auto startpos = GetObj().GetFramePosition(f.GetFrameID()) - GetMove().GetMat().yvec() * f.GetFrameWorldPosition().pos().y;
					auto pos_t1 = startpos + GetMove().GetMat().yvec() * (1.0f * Scale3DRate);
					auto pos_t2 = startpos + GetMove().GetMat().yvec() * (-0.1f * Scale3DRate);
					auto ColRes = BackGroundParts->CheckLinetoMap(pos_t1, &pos_t2);
					Easing(&Y, (ColRes) ? (pos_t2.y - startpos.y) : -0.1f * Scale3DRate, 0.9f, EasingType::OutExpo);
					SetObj().SetFrameLocalMatrix(f.GetFrameID(),
						Matrix4x4DX::RotAxis(Vector3DX::right(), (f.GetFrameWorldPosition().pos().x >= 0) ? this->m_CrawlerRotateLeft : this->m_CrawlerRotateRight) *
						Matrix4x4DX::Mtrans(Vector3DX::up() * Y) * f.GetFrameWorldPosition()
					);
				}
			}
			//�㕔�x���ւȂǃT�X�y���V����������Ȃ�����
			for (const auto& f : this->m_VecData->GetNoSpringWheelFrameList()) {
				if (f.GetFrameID() >= 0) {
					SetObj().SetFrameLocalMatrix(f.GetFrameID(),
						Matrix4x4DX::RotAxis(Vector3DX::right(), (f.GetFrameWorldPosition().pos().x >= 0) ? this->m_CrawlerRotateLeft : this->m_CrawlerRotateRight) *
						f.GetFrameWorldPosition());
				}
			}
			//����
			for (auto& LR : this->m_CrawlerFrame) {
				for (auto& f : LR) {
					if (f.GetFrame().GetFrameID() >= 0) {
						SetObj().ResetFrameUserLocalMatrix(f.GetFrame().GetFrameID());
						auto startpos = GetObj().GetFramePosition(f.GetFrame().GetFrameID()) - GetMove().GetMat().yvec() * f.GetFrame().GetFrameWorldPosition().pos().y;
						auto pos_t1 = startpos + GetMove().GetMat().yvec() * (1.0f * Scale3DRate);
						auto pos_t2 = startpos + GetMove().GetMat().yvec() * (-0.2f * Scale3DRate);
						auto ColRes = BackGroundParts->CheckLinetoMap(pos_t1, &pos_t2);
						f.Update(ColRes, pos_t2.y);
						float Y = -0.2f * Scale3DRate;
						if (ColRes) {
							Y = (pos_t2.y - startpos.y);
						}
						SetObj().SetFrameLocalMatrix(f.GetFrame().GetFrameID(),
							Matrix4x4DX::Mtrans(Vector3DX::up() * Y) * f.GetFrame().GetFrameWorldPosition()
						);
					}
				}
			}
			//�ړ����Z
			Vector3DX MoveVec = GetMove().GetVec();
			Matrix3x3DX MoveMat = GetMove().GetMat();
			//�X���̎擾
			{
				Easing(&this->m_BodyNormal,
					Vector3DX::Cross(
						GetObj().GetFramePosition(this->m_VecData->GetSquareFrameID(0)) - GetObj().GetFramePosition(this->m_VecData->GetSquareFrameID(3)),
						GetObj().GetFramePosition(this->m_VecData->GetSquareFrameID(1)) - GetObj().GetFramePosition(this->m_VecData->GetSquareFrameID(2))
					).normalized(),
					0.95f, EasingType::OutExpo);
				//�X�������Ƃɍŏ��̍��W�m��
				auto pp = (GetMove().GetMat() * Matrix3x3DX::RotVec2(Vector3DX::up(), this->m_BodyNormal).inverse()).zvec() * -1.f;
				float yradBody = std::atan2f(pp.x, pp.z);
				MoveMat = Matrix3x3DX::Axis1(this->m_BodyNormal, (Matrix3x3DX::RotAxis(Vector3DX::up(), yradBody) * Matrix3x3DX::RotVec2(Vector3DX::up(), this->m_BodyNormal)).zvec() * -1.0f);
			}
			//�n�ʔ���
			float OnGroundHeight = 0.0f;
			int OnGroundCount = 0;
			{
				//���т̐ڒn����
				for (const auto& LR : this->m_CrawlerFrame) {
					for (const auto& f : LR) {
						if (f.OnGround()) {
							OnGroundHeight += f.GetHitHeight();
							++OnGroundCount;
						}
					}
				}
				int OnGroundCountSquare = 0;
				for (const auto& s : this->m_VecData->GetSquareFrameList()) {
					auto p_t = GetObj().GetFramePosition(s);
					auto pos_t1 = p_t + (Vector3DX::up() * 1.0f * Scale3DRate);
					auto pos_t2 = p_t + (Vector3DX::up() * -1.0f * Scale3DRate);
					if (BackGroundParts->CheckLinetoMap(pos_t1, &pos_t2)) {
						OnGroundHeight += pos_t2.y;
						++OnGroundCount;
						++OnGroundCountSquare;
					}
				}
				//�n��or���ʂɂ��邩�ǂ���
				if ((OnGroundCount > 0)) {
					//�O�i���
					this->m_speed_add = std::clamp(this->m_speed_add +
						(this->m_Input.GetPADSPress(Controls::PADS::MOVE_W) ? 3.0f : -1.0f) * DXLib_refParts->GetDeltaTime(),
						0.0f, this->m_VecData->GetMaxFrontSpeed());
					this->m_speed_sub = std::clamp(this->m_speed_sub -
						(this->m_Input.GetPADSPress(Controls::PADS::MOVE_S) ? 3.0f : -1.0f) * DXLib_refParts->GetDeltaTime(),
						this->m_VecData->GetMaxBackSpeed(), 0.f);
					//����
					this->m_yradadd_right = std::clamp(this->m_yradadd_right +
						(this->m_Input.GetPADSPress(Controls::PADS::MOVE_A) ? 60.f : -120.f) * DXLib_refParts->GetDeltaTime(),
						0.0f, this->m_VecData->GetMaxBodyRad());
					this->m_yradadd_left = std::clamp(this->m_yradadd_left -
						(this->m_Input.GetPADSPress(Controls::PADS::MOVE_D) ? 60.f : -120.f) * DXLib_refParts->GetDeltaTime(),
						-this->m_VecData->GetMaxBodyRad(), 0.0f);
				}
				Easing(&this->m_speed, (this->m_speed_add + this->m_speed_sub) * 20.f, 0.9f, EasingType::OutExpo);
				Easing(&this->m_radAdd.y, deg2rad(this->m_yradadd_left + this->m_yradadd_right), 0.9f, EasingType::OutExpo);
				//����
				const auto radold = this->m_radAdd;
				this->m_radAdd.x = deg2rad(this->m_speed * -1.0f);
				this->m_radAdd.z = deg2rad(this->m_radAdd.y / deg2rad(5.0f) * -5.0f);
				Easing(&this->m_Tilt, Vector2DX::vget(
					std::clamp(this->m_radAdd.x - radold.x, deg2rad(-30.0f), deg2rad(30.0f)),
					std::clamp(this->m_radAdd.z - radold.z, deg2rad(-15.0f), deg2rad(15.0f))), 0.95f, EasingType::OutExpo);

				MoveMat *= Matrix3x3DX::RotAxis(MoveMat.xvec(), -this->m_Tilt.x) * Matrix3x3DX::RotAxis(MoveMat.zvec(), this->m_Tilt.y);

				MoveVec = MoveMat.zvec() * -(this->m_speed / 3.6f * Scale3DRate *DXLib_refParts->GetDeltaTime());
				if (OnGroundCountSquare < 3 || (OnGroundCount == 0)) {
					MoveVec.y = GetMove().GetVec().y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));
				}
			}
			//�ˌ�����
			for (auto& cg : this->m_Gun) {
				int index = static_cast<int>(&cg - &this->m_Gun.front());
				cg.Update();
				//����
				MoveMat *= Matrix3x3DX::RotAxis(MoveMat.xvec(), cg.GetShotRadAdd().x) * Matrix3x3DX::RotAxis(MoveMat.zvec(), cg.GetShotRadAdd().z);
				//�ˌ�
				if ((index == 0) ? this->m_Input.GetPADSPress(Controls::PADS::SHOT) : this->m_Input.GetPADSPress(Controls::PADS::ULT)) {
					if (cg.CanShot()) {
						cg.Shot();
						Vector3DX MuzzlePos = GetObj().GetFramePosition(cg.GetGunMuzzleFrame().GetFrameID());
						Vector3DX MuzzleVec = (MuzzlePos - GetObj().GetFramePosition(cg.GetGunTrunnionFrame().GetFrameID())).normalized();
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_Shot))->Play3D(GetMove().GetPos(), 100.0f * Scale3DRate);													//�T�E���h
						EffectSingleton::Instance()->SetOnce_Any(Effect::ef_fire2, MuzzlePos, MuzzleVec, cg.GetAmmoSpec()->GetCaliber() * 10.0f * Scale3DRate, 2.f);	//�e���C�G�t�F�N�g�̃Z�b�g
						auto LastAmmo = std::make_shared<Objects::AmmoObj>();
						ObjectManager::Instance()->InitObject(LastAmmo);
						LastAmmo->Put(&cg.GetAmmoSpec(), MuzzlePos, MuzzleVec, this->m_MyPlayerID);
					}
				}
			}
			//�]��
			this->m_CrawlerRotateLeft += -this->m_speed / 3.6f * 0.0015f + this->m_radAdd.y * 0.1f;
			this->m_CrawlerRotateRight += -this->m_speed / 3.6f * 0.0015f - this->m_radAdd.y * 0.1f;

			MoveMat = MoveMat * Matrix3x3DX::RotAxis(Vector3DX::up(), -this->m_radAdd.y * DXLib_refParts->GetDeltaTime());
			//�ړ�
			SetMove().SetVec(MoveVec);

			auto Pos = GetMove().GetPos() + GetMove().GetVec();

			if (OnGroundCount > 0) {
				auto pos_ytmp = GetMove().GetPos().y;
				Easing(&pos_ytmp, (OnGroundHeight / OnGroundCount), 0.9f, EasingType::OutExpo);
				Pos.y = pos_ytmp;
			}


			Pos.x = std::clamp(Pos.x, -15.f * Scale3DRate, 15.f * Scale3DRate);
			Pos.z = std::clamp(Pos.z, -15.f * Scale3DRate, 15.f * Scale3DRate);
			SetMove().SetPos(Pos);

			SetMove().SetMat(MoveMat);
			SetMove().Update(0.8f, 0.0f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());

			//�j��
			{
				int								xput = 24;
				int								yput = 12;
				int								zput = 24;
				auto Put = BackGroundParts->GetPoint(GetMove().GetPos());
				int IsBreak = 0;
				for (int xp = -xput / 2; xp < xput / 2; ++xp) {
					for (int yp = 0 - 4; yp < yput; ++yp) {
						for (int zp = -zput / 2; zp < zput / 2; ++zp) {
							int xx = (Put.x + xp);
							int yy = (Put.y + yp);
							int zz = (Put.z + zp);
							auto& cell = BackGroundParts->GetCellBuf(xx, yy, zz);
							if (cell.GetCell() == BackGround::s_EmptyBlick) {
								continue;
							}
							if (cell.GetCell() == 1) {
								continue;
							}
							if (cell.GetCell() == 4) {
								continue;
							}
							BackGroundParts->SetBlick(xx, yy, zz, BackGround::s_EmptyBlick);
							if ((xp % 3 == 0) || (zp % 3 == 0)) {
								//EffectSingleton::Instance()->SetOnce_Any(Effect::ef_break, BackGroundParts->GetPos(xx, yy, zz),
								//	Matrix3x3DX::Vtrans(Vector3DX::forward(), Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180.f)))), 3.f + GetRandf(2.f), 3.f);
							}
							++IsBreak;
						}
					}
				}
				if (IsBreak>0) {
					BackGroundParts->UpdateOnce();
					if (IsBreak > 20) {
						//this->m_speed_add *= 0.5f;
						//this->m_speed_sub *= 0.5f;
					}
				}
			}
		}
		void			VehicleObj::Draw(bool isDrawSemiTrans, int Range) noexcept {
			if (!IsActive()) { return; }
			if (!IsDraw(Range)) { return; }
			if (isDrawSemiTrans) { return; }
			//GetCol().DrawModel();
			//return;

			MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_CrawlerRotateLeft * 0.1f, 0.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.0f);
			GetObj().DrawMesh(0);
			MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_CrawlerRotateRight * 0.1f, 0.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.0f);
			GetObj().DrawMesh(1);
			MV1ResetFrameTextureAddressTransform(GetObj().get(), 0);
			GetObj().DrawMesh(2);
			for (int i = 2; i < GetObj().GetMeshNum(); i++) {
				GetObj().DrawMesh(i);
			}
			//GetCol().DrawModel();
		}
	};
};
