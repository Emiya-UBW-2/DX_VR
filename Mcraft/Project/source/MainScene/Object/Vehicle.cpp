#pragma once
#include "Vehicle.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Objects {
		//被弾処理
		const bool		VehicleObj::CheckAmmoHit(const Vector3DX& StartPos, Vector3DX* EndPos) noexcept {
			auto* SE = SoundPool::Instance();
			//とりあえず当たったかどうか探す
			if (!RefreshCol(StartPos, *EndPos, 10.0f * Scale3DRate)) {
				return false;
			}
			{
				auto HitCheck_Tank = [&](int mesh, const Vector3DX& ray_repos, const MV1_COLL_RESULT_POLY& hitres) {
					this->m_hitres[mesh] = hitres;
					if (this->m_hitres[mesh].HitFlag == TRUE) {
						this->m_hitssort[mesh].Set(mesh, (ray_repos - this->m_hitres[mesh].HitPosition).magnitude());
						return true;
					}
					else {
						this->m_hitssort[mesh].Set(mesh);
						return false;
					}
					};
				bool is_Hit = false;
				//装甲(一番近い位置のものに限定する)
				int answer = InvalidID;
				MV1_COLL_RESULT_POLY colres{}; colres.HitFlag = FALSE;
				for (const auto& mesh : this->m_VecData->GetArmerMeshIDList()) { HitCheck_Tank(mesh.first, StartPos, colres); }//全リセット
				for (const auto& mesh : this->m_VecData->GetArmerMeshIDList()) {
					auto colresBuf = GetColLine(StartPos, *EndPos, mesh.first);
					if (colresBuf.HitFlag == TRUE) {
						answer = mesh.first;
						colres = colresBuf;
						*EndPos = colresBuf.HitPosition;
					}
				}
				//
				for (auto& mesh : this->m_VecData->GetModuleMeshIDList()) {//モジュール
					is_Hit |= HitCheck_Tank(mesh, StartPos, GetColLine(StartPos, *EndPos, mesh));
				}
				for (auto& mesh : this->m_VecData->GetSpaceArmerMeshIDList()) {//空間装甲
					is_Hit |= HitCheck_Tank(mesh, StartPos, GetColLine(StartPos, *EndPos, mesh));
				}
				//
				if (answer != InvalidID) {
					is_Hit |= HitCheck_Tank(answer, StartPos, colres);
				}
				if (!is_Hit) {
					return false;
				}
			}
			//当たり判定を近い順にソート
			std::sort(this->m_hitssort.begin(), this->m_hitssort.end(), [](const HitSortInfo& x, const HitSortInfo& y) { return x < y; });
			//ダメージ面に届くまで判定
			for (const auto& tt : this->m_hitssort) {
				if (tt.IsHit()) {
					if (tt.GetHitMesh() >= this->m_hitres.size()) { continue; }
					Vector3DX HitPos = this->m_hitres[tt.GetHitMesh()].HitPosition;
					Vector3DX HitNormal = this->m_hitres[tt.GetHitMesh()].Normal;
					HitNormal = HitNormal.normalized();

					for (auto& mesh : this->m_VecData->GetSpaceArmerMeshIDList()) {
						if (tt.GetHitMesh() == mesh) {
							EffectSingleton::Instance()->SetOnce(Effect::ef_gndsmoke, HitPos, HitNormal, 1.0f);
						}
					}
					for (auto& mesh : this->m_VecData->GetModuleMeshIDList()) {
						if (tt.GetHitMesh() == mesh) {
							EffectSingleton::Instance()->SetOnce(Effect::ef_gndsmoke, HitPos, HitNormal, 1.0f);
						}
					}
					//ダメージ面に当たった
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
		//更新関連
		//----------------------------------------------------------
		void			VehicleObj::Init_Sub(void) noexcept {
			this->m_VecData = std::make_unique<VhehicleData>(GetObj(), GetCol(), GetFilePath().c_str());
			for (auto& LR : this->m_CrawlerFrame) {
				const auto& crawlerFrame = this->m_VecData->GetCrawlerFrameList()[&LR - &this->m_CrawlerFrame.front()];
				LR.resize(crawlerFrame.size());
				for (auto& frame : LR) {
					frame.Init(crawlerFrame[&frame - &LR.front()]);
				}
			}
			this->m_WheelHeight.resize(this->m_VecData->GetWheelFrameList().size());
			//砲
			this->m_Gun.resize(this->m_VecData->GetGunData().size());
			for (auto& gun : this->m_Gun) {
				gun.Init(&this->m_VecData->GetGunData()[&gun - &this->m_Gun.front()]);
			}

			SetActive(true);
			SetMinAABB(Vector3DX::vget(-10.0f, -1.0f, -10.0f) * Scale3DRate);
			SetMaxAABB(Vector3DX::vget(10.0f, 10.0f, 10.0f) * Scale3DRate);
			GetObj().SetMaterialDrawAlphaTestAll(true, DX_CMP_GREATER, 128);
			this->m_hitres.resize(GetCol().GetMeshNum());											//モジュールごとの当たり判定結果を確保
			this->m_hitssort.resize(GetCol().GetMeshNum());											//モジュールごとの当たり判定順序を確保
		}
		void			VehicleObj::FirstUpdate(void) noexcept {
			auto* SE = SoundPool::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			//初回のみ更新する内容
			if (IsFirstLoop()) {
				this->m_engine_time = static_cast<float>(GetRand(100)) / 100.0f;
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_move))->Play3D(GetMove().GetPos(), 30.0f * Scale3DRate, DX_PLAYTYPE_LOOP);
			}
			//エンジン音
			if (this->m_engine_time <= 0.0f) {
				this->m_engine_time += 0.9f;
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_engine))->Play3D(GetMove().GetPos(), 30.0f * Scale3DRate);//, DX_PLAYTYPE_LOOP
			}
			this->m_engine_time = std::max(this->m_engine_time - DXLib_refParts->GetDeltaTime(), 0.0f);

			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_move))->SetPosition(GetMove().GetPos());
			SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_move))->SetLocalVolume(
				std::clamp(static_cast<int>(128.0f * (std::abs(GetMove().GetVec().magnitude() / DXLib_refParts->GetDeltaTime()) * 0.005f + std::abs(this->m_radAdd.y) * 0.1f) * 10.0f), 0, 128)
			);
			//狙い
			for (auto& gun : this->m_Gun) {
				Vector3DX StartPos = GetObj().GetFramePosition(gun.GetGunMuzzleFrame().GetFrameID());
				Vector3DX EndPos = StartPos + (this->m_MouseVec.zvec2()).normalized() * (500.0f * Scale3DRate);//狙う場所
				Vector3DX BasePos = GetObj().GetFramePosition(gun.GetGunTrunnionFrame().GetFrameID());
				gun.UpdateAim((StartPos - BasePos).normalized(), (EndPos - BasePos).normalized(), this->m_VecData);
			}
			//砲塔旋回
			for (auto& gun : this->m_Gun) {
				frames id;
				id = gun.GetGunTurretFrame();
				if (id.GetFrameID() >= 0) {
					auto RotateY = Matrix4x4DX::RotAxis(Vector3DX::up(), gun.GetShotTargetRad().y);
					SetObj().SetFrameLocalMatrix(id.GetFrameID(), RotateY * id.GetFrameLocalPosition());
					SetCol().SetFrameLocalMatrix(id.GetFrameID(), RotateY * id.GetFrameLocalPosition());
				}
				id = gun.GetGunTrunnionFrame();
				if (id.GetFrameID() >= 0) {
					auto RotateX = Matrix4x4DX::RotAxis(Vector3DX::right(), gun.GetShotTargetRad().x);
					SetObj().SetFrameLocalMatrix(id.GetFrameID(), RotateX * id.GetFrameLocalPosition());
					SetCol().SetFrameLocalMatrix(id.GetFrameID(), RotateX * id.GetFrameLocalPosition());
				}
				id = gun.GetGunMuzzleFrame();
				if (id.GetFrameID() >= 0) {
					auto Recoil = Matrix4x4DX::Mtrans(Vector3DX::vget(0.0f, 0.0f, gun.GetRecoil() * 0.5f * Scale3DRate));
					SetObj().SetFrameLocalMatrix(id.GetFrameID(), Recoil * id.GetFrameLocalPosition());
					SetCol().SetFrameLocalMatrix(id.GetFrameID(), Recoil * id.GetFrameLocalPosition());
				}
			}
			//転輪
			for (auto& height : this->m_WheelHeight) {
				auto index = static_cast<int>(&height - &this->m_WheelHeight.front());
				auto& frame = this->m_VecData->GetWheelFrameList()[index];
				if (frame.GetFrameID() >= 0) {
					SetObj().ResetFrameUserLocalMatrix(frame.GetFrameID());
					auto startpos = GetObj().GetFramePosition(frame.GetFrameID()) - GetMove().GetMat().yvec() * frame.GetFrameWorldPosition().pos().y;
					auto pos1 = startpos + GetMove().GetMat().yvec() * (1.0f * Scale3DRate);
					auto pos2 = startpos + GetMove().GetMat().yvec() * (-0.1f * Scale3DRate);
					auto ColRes = BackGroundParts->CheckLinetoMap(pos1, &pos2);
					Easing(&height, (ColRes) ? (pos2.y - startpos.y) : -0.1f * Scale3DRate, 0.9f, EasingType::OutExpo);
					SetObj().SetFrameLocalMatrix(frame.GetFrameID(),
						Matrix4x4DX::RotAxis(Vector3DX::right(), (frame.GetFrameWorldPosition().pos().x >= 0) ? this->m_CrawlerRotateLeft : this->m_CrawlerRotateRight) *
						Matrix4x4DX::Mtrans(Vector3DX::up() * height) * frame.GetFrameWorldPosition()
					);
				}
			}
			//上部支持輪などサスペンションがいらないもの
			for (const auto& frame : this->m_VecData->GetNoSpringWheelFrameList()) {
				if (frame.GetFrameID() >= 0) {
					SetObj().SetFrameLocalMatrix(frame.GetFrameID(),
						Matrix4x4DX::RotAxis(Vector3DX::right(), (frame.GetFrameWorldPosition().pos().x >= 0) ? this->m_CrawlerRotateLeft : this->m_CrawlerRotateRight) *
						frame.GetFrameWorldPosition());
				}
			}
			//履帯
			for (auto& LR : this->m_CrawlerFrame) {
				for (auto& frame : LR) {
					if (frame.GetFrame().GetFrameID() >= 0) {
						SetObj().ResetFrameUserLocalMatrix(frame.GetFrame().GetFrameID());
						auto startpos = GetObj().GetFramePosition(frame.GetFrame().GetFrameID()) - GetMove().GetMat().yvec() * frame.GetFrame().GetFrameWorldPosition().pos().y;
						auto pos1 = startpos + GetMove().GetMat().yvec() * (1.0f * Scale3DRate);
						auto pos2 = startpos + GetMove().GetMat().yvec() * (-0.2f * Scale3DRate);
						auto ColRes = BackGroundParts->CheckLinetoMap(pos1, &pos2);
						frame.Update(ColRes, pos2.y);
						float height = -0.2f * Scale3DRate;
						if (ColRes) {
							height = (pos2.y - startpos.y);
						}
						SetObj().SetFrameLocalMatrix(frame.GetFrame().GetFrameID(),
							Matrix4x4DX::Mtrans(Vector3DX::up() * height) * frame.GetFrame().GetFrameWorldPosition()
						);
					}
				}
			}
			//移動演算
			Vector3DX MoveVec = GetMove().GetVec();
			Matrix3x3DX MoveMat = GetMove().GetMat();
			//傾きの取得
			{
				Easing(&this->m_BodyNormal,
					Vector3DX::Cross(
						GetObj().GetFramePosition(this->m_VecData->GetSquareFrameID(0)) - GetObj().GetFramePosition(this->m_VecData->GetSquareFrameID(3)),
						GetObj().GetFramePosition(this->m_VecData->GetSquareFrameID(1)) - GetObj().GetFramePosition(this->m_VecData->GetSquareFrameID(2))
					).normalized(),
					0.95f, EasingType::OutExpo);
				//傾きをもとに最初の座標確定
				auto pp = (GetMove().GetMat() * Matrix3x3DX::RotVec2(Vector3DX::up(), this->m_BodyNormal).inverse()).zvec2();
				float yradBody = std::atan2f(pp.x, pp.z);
				MoveMat = Matrix3x3DX::Axis1(this->m_BodyNormal, (Matrix3x3DX::RotAxis(Vector3DX::up(), yradBody) * Matrix3x3DX::RotVec2(Vector3DX::up(), this->m_BodyNormal)).zvec2());
			}
			//地面判定
			float OnGroundHeight = 0.0f;
			int OnGroundCount = 0;
			{
				//履帯の接地判定
				for (const auto& LR : this->m_CrawlerFrame) {
					for (const auto& frame : LR) {
						if (frame.OnGround()) {
							OnGroundHeight += frame.GetHitHeight();
							++OnGroundCount;
						}
					}
				}
				int OnGroundCountSquare = 0;
				for (const auto& frame : this->m_VecData->GetSquareFrameList()) {
					auto pos = GetObj().GetFramePosition(frame);
					auto pos1 = pos + (Vector3DX::up() * 1.0f * Scale3DRate);
					auto pos2 = pos + (Vector3DX::up() * -1.0f * Scale3DRate);
					if (BackGroundParts->CheckLinetoMap(pos1, &pos2)) {
						OnGroundHeight += pos2.y;
						++OnGroundCount;
						++OnGroundCountSquare;
					}
				}
				//地面or水面にいるかどうか
				if ((OnGroundCount > 0)) {
					//前進後退
					this->m_speed_add = std::clamp(this->m_speed_add +
						(this->m_Input.GetPADSPress(Controls::PADS::MOVE_W) ? 3.0f : -12.0f) * DXLib_refParts->GetDeltaTime(),
						0.0f, this->m_VecData->GetMaxFrontSpeed());
					this->m_speed_sub = std::clamp(this->m_speed_sub -
						(this->m_Input.GetPADSPress(Controls::PADS::MOVE_S) ? 3.0f : -12.0f) * DXLib_refParts->GetDeltaTime(),
						this->m_VecData->GetMaxBackSpeed(), 0.0f);
					//旋回
					this->m_yradadd_right = std::clamp(this->m_yradadd_right +
						(this->m_Input.GetPADSPress(Controls::PADS::MOVE_A) ? 60.0f : -120.0f) * DXLib_refParts->GetDeltaTime(),
						0.0f, this->m_VecData->GetMaxBodyRad());
					this->m_yradadd_left = std::clamp(this->m_yradadd_left -
						(this->m_Input.GetPADSPress(Controls::PADS::MOVE_D) ? 60.0f : -120.0f) * DXLib_refParts->GetDeltaTime(),
						-this->m_VecData->GetMaxBodyRad(), 0.0f);
				}
				Easing(&this->m_speed, (this->m_speed_add + this->m_speed_sub) * 20.0f, 0.9f, EasingType::OutExpo);
				Easing(&this->m_radAdd.y, deg2rad(this->m_yradadd_left + this->m_yradadd_right), 0.9f, EasingType::OutExpo);
				//慣性
				const auto radold = this->m_radAdd;
				this->m_radAdd.x = deg2rad(this->m_speed * -1.0f);
				this->m_radAdd.z = deg2rad(this->m_radAdd.y / deg2rad(5.0f) * -5.0f);
				Easing(&this->m_Tilt, Vector2DX::vget(
					std::clamp(radold.x - this->m_radAdd.x, deg2rad(-30.0f), deg2rad(30.0f)),
					std::clamp(radold.z - this->m_radAdd.z, deg2rad(-15.0f), deg2rad(15.0f))), 0.95f, EasingType::OutExpo);

				MoveMat *= Matrix3x3DX::RotAxis(MoveMat.xvec(), this->m_Tilt.x) * Matrix3x3DX::RotAxis(MoveMat.zvec2(), this->m_Tilt.y);

				MoveVec = MoveMat.zvec2() * (this->m_speed / 3.6f * Scale3DRate *DXLib_refParts->GetDeltaTime());
				if (OnGroundCountSquare < 3 || (OnGroundCount == 0)) {
					MoveVec.y = GetMove().GetVec().y + (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));
				}
			}
			//射撃反動
			for (auto& cg : this->m_Gun) {
				int index = static_cast<int>(&cg - &this->m_Gun.front());
				cg.Update();
				//反動
				MoveMat *= Matrix3x3DX::RotAxis(MoveMat.xvec(), cg.GetShotRadAdd().x) * Matrix3x3DX::RotAxis(MoveMat.zvec(), cg.GetShotRadAdd().z);
				//射撃
				if ((index == 0) ? this->m_Input.GetPADSPress(Controls::PADS::SHOT) : this->m_Input.GetPADSPress(Controls::PADS::ULT)) {
					if (cg.CanShot()) {
						cg.Shot();
						Vector3DX MuzzlePos = GetObj().GetFramePosition(cg.GetGunMuzzleFrame().GetFrameID());
						Vector3DX MuzzleVec = (MuzzlePos - GetObj().GetFramePosition(cg.GetGunTrunnionFrame().GetFrameID())).normalized();
						SE->Get(SoundType::SE, static_cast<int>(SoundEnum::Tank_Shot))->Play3D(GetMove().GetPos(), 100.0f * Scale3DRate);													//サウンド
						EffectSingleton::Instance()->SetOnce_Any(Effect::ef_fire2, MuzzlePos, MuzzleVec, cg.GetAmmoSpec()->GetCaliber() * 10.0f * Scale3DRate, 2.0f);	//銃発砲エフェクトのセット
						Objects::AmmoPool::Instance()->Put(&cg.GetAmmoSpec(), MuzzlePos, MuzzleVec, this->m_MyPlayerID);
					}
				}
			}
			//転輪
			this->m_CrawlerRotateLeft += -this->m_speed / 3.6f * 0.0015f + this->m_radAdd.y * 0.1f;
			this->m_CrawlerRotateRight += -this->m_speed / 3.6f * 0.0015f - this->m_radAdd.y * 0.1f;

			MoveMat = MoveMat * Matrix3x3DX::RotAxis(Vector3DX::up(), -this->m_radAdd.y * DXLib_refParts->GetDeltaTime());
			//移動
			SetMove().SetVec(MoveVec);

			auto Pos = GetMove().GetPos() + GetMove().GetVec();

			if (OnGroundCount > 0) {
				auto pos_ytmp = GetMove().GetPos().y;
				Easing(&pos_ytmp, (OnGroundHeight / OnGroundCount), 0.9f, EasingType::OutExpo);
				Pos.y = pos_ytmp;
			}


			Pos.x = std::clamp(Pos.x, -15.0f * Scale3DRate, 15.0f * Scale3DRate);
			Pos.z = std::clamp(Pos.z, -15.0f * Scale3DRate, 15.0f * Scale3DRate);
			SetMove().SetPos(Pos);

			SetMove().SetMat(MoveMat);
			SetMove().Update(0.8f, 0.0f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());

			//破壊
			{
				int								xput = 24;
				int								yput = 12;
				int								zput = 24;
				auto Put = BackGroundParts->GetPoint(GetMove().GetPos());
				int IsBreak = 0;
				for (int xp = -xput / 2; xp < xput / 2; ++xp) {
					for (int yp = 0 - 4; yp < yput; ++yp) {
						for (int zp = -zput / 2; zp < zput / 2; ++zp) {
							if (BackGroundParts->DamageCell(Put.x + xp, Put.y + yp, Put.z + zp, 100)) {
								if ((xp % 3 == 0) || (zp % 3 == 0)) {
									//EffectSingleton::Instance()->SetOnce_Any(Effect::ef_break, BackGroundParts->GetPos(xx, yy, zz),
									//	Matrix3x3DX::Vtrans(Vector3DX::forward(), Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(180.0f)))), 3.0f + GetRandf(2.0f), 3.0f);
								}
								++IsBreak;
							}
						}
					}
				}
				if (IsBreak>0) {
					BackGroundParts->UpdateOnce();
					if (IsBreak > 20) {
						//this->m_speed_add *= 0.5f;
						//this->m_speed_sub *= 0.5f;
					}
					Camera3D::Instance()->SetCamShake(0.1f, 0.1f);
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
			for (int loop = 2; loop < GetObj().GetMeshNum(); loop++) {
				GetObj().DrawMesh(loop);
			}
			//GetCol().DrawModel();
		}
	}
}
