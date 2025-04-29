#include	"Ammo.hpp"

#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../MainScene/Object/Character.hpp"
#include	"../../MainScene/Player/Player.hpp"

const FPS_n2::Objects::AmmoPool* SingletonBase<FPS_n2::Objects::AmmoPool>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Objects {
		void		AmmoObj::FirstUpdate(void) noexcept {
			if (!IsActive()) {
				this->m_IsDrawLine = false;
				return;
			}
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
			if (this->m_ShootCheraID != -1 && PlayerMngr->GetHelicopter()->CheckAmmoHit(repos_tmp, &pos_tmp)) {
				SetActive(false);
				is_HitAll = true;
				if (this->m_ShootCheraID == PlayerMngr->GetWatchPlayerID()) {//撃ったキャラ
					HitMarkerPool::Instance()->AddMarker(pos_tmp, 0, (*this->m_AmmoData)->GetDamage());
				}
			}
			/*
			//戦車との判定
			if (this->m_ShootCheraID != -1 && PlayerMngr->GetVehicle()->CheckAmmoHit(repos_tmp, &pos_tmp)) {
				SetActive(false);
				is_HitAll = true;
			}
			//*/
			//キャラとの判定
			for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
				if (PlayerMngr->GetPlayer(loop)->GetChara()->CheckDamageRay((*this->m_AmmoData)->GetDamage(), (PlayerID)this->m_ShootCheraID, repos_tmp, &pos_tmp)) {
					SetActive(false);
					is_HitAll = true;
				}
			}
			//キャラに当たらず接地したら
			if (ColResGround && !is_HitAll) {
				SetActive(false);
				//壁破壊
				{
					int								xput = 4;
					int								yput = 2;
					int								zput = 4;
					if ((*this->m_AmmoData)->GetCaliber() >= 0.0127f) {
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
								if (BackGroundParts->DamageCell(Put.x + xp, Put.y + yp, Put.z + zp, 50)) {
									IsChanged = true;
								}
							}
						}
					}
					if (IsChanged) {
						BackGroundParts->UpdateOnce();
					}
				}
				//エフェクト
				EffectSingleton::Instance()->SetOnce_Any(Effect::ef_gndsmoke, pos_tmp, norm_tmp, std::min(0.0127f,(*this->m_AmmoData)->GetCaliber()) / 0.02f * Scale3DRate);
				//サウンド
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGround0) + GetRand(5 - 1))->Play3D(pos_tmp, Scale3DRate * 10.0f);
			}
			//消す(スピードが0以下、貫通が0以下、5回反射する)
			if (this->m_speed <= 0.0f || this->m_penetration <= 0.0f || this->m_RicochetCnt > 5 || this->m_Timer > 5.0f) {
				SetActive(false);
			}
			this->m_Timer += DXLib_refParts->GetDeltaTime();
			this->m_pos = pos_tmp;
		}
	}
}
