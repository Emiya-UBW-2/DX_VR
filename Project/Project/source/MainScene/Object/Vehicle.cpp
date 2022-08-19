#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//操作共通
		void			VehicleClass::ExecuteMove(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			bool isfloat = this->m_VecData->GetIsFloat() && (this->m_move.pos.y() == -this->m_VecData->GetDownInWater());
			//傾きの取得
			{
				const auto bNormal = (isfloat) ? VECTOR_ref::up() : this->m_BodyNormal;
				float yradBody = 0.f;
				if (this->m_PosBufOverRideFlag) {
					yradBody = this->m_yRadOverRide;
				}
				else {
					auto pp = (this->m_move.mat * MATRIX_ref::RotVec2(VECTOR_ref::up(), bNormal).Inverse()).zvec() * -1.f;
					yradBody = atan2f(pp.x(), pp.z());
				}
				this->m_move.mat = MATRIX_ref::Axis1_YZ(bNormal, (MATRIX_ref::RotY(yradBody)*MATRIX_ref::RotVec2(VECTOR_ref::up(), bNormal)).zvec() * -1.f);
				if (this->m_PosBufOverRideFlag) {
					//一旦その場で地形判定
					this->m_move.pos = this->m_PosBufOverRide;
					GetObj().SetMatrix(this->m_move.MatIn());
				}
				Easing(&this->m_BodyNormal, ((GetObj().frame(this->m_VecData->Get_square(0)) - GetObj().frame(this->m_VecData->Get_square(3))).cross(GetObj().frame(this->m_VecData->Get_square(1)) - GetObj().frame(this->m_VecData->Get_square(2)))).Norm(), 0.8f, EasingType::OutExpo);
			}
			//地面判定
			{
				auto pos_ytmp = this->m_move.pos.y();
				float hight_t = 0.f;
				int cnt_t = 0;
				//履帯
				for (auto& f : this->m_b2Foot) {
					f.FirstExecute(&GetObj(), this->m_MapCol);
					for (const auto& t : f.Getb2downsideframe()) {
						if (t.colres.HitFlag == TRUE) {
							hight_t += t.colres.HitPosition.y;
							cnt_t++;
						}
					}
				}
				int cnt_sq = cnt_t;
				for (const auto& s : this->m_VecData->Get_square()) {
					auto p_t = GetObj().frame(s);
					auto ColResGround = this->m_MapCol->CollCheck_Line(p_t + (VECTOR_ref::up() * 3.f*Scale_Rate), p_t + (VECTOR_ref::up() * -0.5f*Scale_Rate));
					if (ColResGround.HitFlag == TRUE) {
						hight_t += ColResGround.HitPosition.y;
						cnt_t++;
					}
				}
				if (cnt_t > 0) {
					Easing(&pos_ytmp, (hight_t / cnt_t), 0.9f, EasingType::OutExpo);
				}
				this->m_move.pos.y(pos_ytmp);
				//地面or水面にいるかどうか
				if ((cnt_t > 0) || isfloat) {
					//前進後退
					{
						const auto spdold = this->m_speed_add + this->m_speed_sub;
						this->m_speed_add = (this->m_key[2]) ? std::min(this->m_speed_add + (0.15f / 3.6f) * (60.f / FPS), (this->m_VecData->GetMaxFrontSpeed() / 3.6f)) : std::max(this->m_speed_add - (0.7f / 3.6f) * (60.f / FPS), 0.f);
						this->m_speed_sub = (this->m_key[3]) ? std::max(this->m_speed_sub - (0.15f / 3.6f) * (60.f / FPS), (this->m_VecData->GetMaxBackSpeed() / 3.6f)) : std::min(this->m_speed_sub + (0.7f / 3.6f) * (60.f / FPS), 0.f);
						this->m_speed = (spdold + ((this->m_speed_add + this->m_speed_sub) - spdold) * 0.1f) / FPS;
						auto vec = this->m_move.mat.zvec() * -1.f * (this->m_speed*Scale_Rate);
						this->m_move.vec.x(vec.x());
						if ((cnt_t - cnt_sq) >= 3) {
							this->m_move.vec.y(vec.y());
						}
						else {
							this->m_move.vec.yadd(M_GR / (FPS * FPS));
						}
						this->m_move.vec.z(vec.z());
					}
					//旋回
					{
						const auto radold = this->m_radAdd;
						this->m_yradadd_left = (this->m_key[4]) ? std::max(this->m_yradadd_left - deg2rad(3.5f * (60.f / FPS)), deg2rad(-this->m_VecData->GetMaxBodyRad())) : std::min(this->m_yradadd_left + deg2rad(2.1f * (60.f / FPS)), 0.f);
						this->m_yradadd_right = (this->m_key[5]) ? std::min(this->m_yradadd_right + deg2rad(3.5f * (60.f / FPS)), deg2rad(this->m_VecData->GetMaxBodyRad())) : std::max(this->m_yradadd_right - deg2rad(2.1f * (60.f / FPS)), 0.f);
						this->m_radAdd.y((this->m_yradadd_left + this->m_yradadd_right) / FPS);
						//慣性
						this->m_radAdd.x(deg2rad(-(this->m_speed / (60.f / FPS)) / (0.1f / 3.6f) * 50.f));
						this->m_radAdd.z(deg2rad(-this->m_radAdd.y() / (deg2rad(5.f) / FPS) * 5.f));
						Easing(&this->m_Tilt, VECTOR_ref::vget(std::clamp(this->m_radAdd.x() - radold.x(), deg2rad(-30.f), deg2rad(30.f)), 0.f, std::clamp(this->m_radAdd.z() - radold.z(), deg2rad(-15.f), deg2rad(15.f))), 0.95f, EasingType::OutExpo);
						this->m_move.mat *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), -this->m_Tilt.x()) * MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_Tilt.z());
					}
					//
				}
				else {
					this->m_move.vec.yadd(M_GR / (FPS * FPS));
				}
			}
			//射撃反動
			for (auto& cg : this->m_Gun) {
				cg.FireReaction(&this->m_move.mat);
				//射撃
				if (cg.Execute(this->m_key[(&cg == &this->m_Gun.front()) ? 0 : 1], this->m_CharaType == CharaTypeID::Mine)) {
					SE->Get((int)SoundEnum::Tank_Shot).Play_3D(cg.GetShotSound(), this->m_move.pos, 250.f*Scale_Rate);							//サウンド
					Effect_UseControl::Set_Effect(Effect::ef_fire, GetObj().frame(cg.GetGunMuzzleFrameID()), GetObj().GetFrameLocalWorldMatrix(cg.GetGunTrunnionFrameID()).zvec() * -1.f, cg.GetCaliberSize() / 0.1f * Scale_Rate);				//銃発砲エフェクトのセット

					auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->AddObject(ObjType::Ammo));
					LastAmmo->Put(&cg.GetAmmoSpec(), GetObj().frame(cg.GetGunMuzzleFrameID()), GetObj().GetFrameLocalWorldMatrix(cg.GetGunTrunnionFrameID()).zvec() * -1.f, this->m_MyID);
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
					this->m_move.pos.y(std::max(this->m_move.pos.y(), -this->m_VecData->GetDownInWater()));
				}
			}
			//転輪
			this->m_wheel_Left += (-(this->m_speed * 2.f - this->m_radAdd.y() * 5.f));
			this->m_wheel_Right += (-(this->m_speed * 2.f + this->m_radAdd.y() * 5.f));
		}
	};
};
