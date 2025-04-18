#include	"Ammo.hpp"

#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../MainScene/Object/Character.hpp"
#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Objects {
		void		AmmoObj::FirstExecute(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGroundParts = BackGround::BackGroundControl::Instance();
			auto* SE = SoundPool::Instance();

			//移動確定
			this->m_repos = this->m_pos;
			this->m_pos = this->m_pos + (this->m_vec * (this->m_speed * DXLib_refParts->GetDeltaTime())) + Vector3DX::up() * this->m_yAdd;
			this->m_yAdd += (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));

			//AmmoObj
			Vector3DX repos_tmp = this->m_repos;
			Vector3DX pos_tmp = this->m_pos;
			Vector3DX norm_tmp = Vector3DX::zero();

			bool is_HitAll = false;

			//地面との判定
			auto ColResGround = BackGroundParts->CheckLinetoMap(repos_tmp, &pos_tmp, &norm_tmp);
			//ヘリとの判定
			//TODO PlayerMngr->GetHelicopter()
			//戦車との判定
			//if (this->m_ShootCheraID != -1 && PlayerMngr->GetVehicle()->CheckAmmoHit(repos_tmp, &pos_tmp)) {
			//	SetDelete();
			//	is_HitAll = true;
			//}
			//キャラとの判定
			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				if (PlayerMngr->GetPlayer(loop)->GetChara()->CheckDamageRay((*this->m_AmmoData)->GetDamage(), (PlayerID)this->m_ShootCheraID, repos_tmp, &pos_tmp)) {
					SetDelete();
					is_HitAll = true;
				}
			}
			//キャラに当たらず接地したら
			if (ColResGround && !is_HitAll) {
				SetDelete();
				//壁破壊
				{
					int								xput = 4;
					int								yput = 1;
					int								zput = 4;
					if ((*this->m_AmmoData)->GetCaliber() > 0.0127f) {
						xput = 6;
						yput = 3;
						zput = 6;
					}
					if ((*this->m_AmmoData)->GetCaliber() > 0.050f) {
						xput = 8;
						yput = 5;
						zput = 8;
					}
					auto Put = BackGroundParts->GetPoint(pos_tmp);
					bool IsChanged = false;
					for (int xp = -xput / 2; xp <= xput / 2; ++xp) {
						for (int yp = -yput / 2; yp <= yput / 2; ++yp) {
							for (int zp = -zput / 2; zp <= zput / 2; ++zp) {
								int xx = (Put.x + xp);
								int yy = (Put.y + yp);
								int zz = (Put.z + zp);
								auto& cell = BackGroundParts->GetCellBuf(xx, yy, zz);
								switch (cell.GetCell()) {
								case 2:
									BackGroundParts->SetBlick(xx, yy, zz, 3);
									break;
								case 3:
									BackGroundParts->SetBlick(xx, yy, zz, BackGround::s_EmptyBlick);
									IsChanged = true;
									break;
								default:
									break;
								}
							}
						}
					}
					if (IsChanged) {
						BackGroundParts->UpdateOnce();
					}
				}
				//エフェクト
				EffectSingleton::Instance()->SetOnce_Any(Effect::ef_gndsmoke, pos_tmp, norm_tmp, std::min(0.0127f,(*this->m_AmmoData)->GetCaliber()) / 0.02f * Scale3DRate);//0.00762f
				//サウンド
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGround0) + GetRand(5 - 1))->Play3D(pos_tmp, Scale3DRate * 10.f);
			}
			//消す(スピードが0以下、貫通が0以下、5回反射する)
			if (this->m_speed <= 0.f || this->m_penetration <= 0.f || this->m_RicochetCnt > 5 || this->m_Timer > 5.f) {
				SetDelete();
			}
			this->m_Timer += DXLib_refParts->GetDeltaTime();
		}
	}
}
