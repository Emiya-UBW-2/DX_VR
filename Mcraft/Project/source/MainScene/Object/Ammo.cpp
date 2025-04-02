#include "Ammo.hpp"

#include "../../MainScene/BackGround/BackGround.hpp"
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"

void FPS_n2::Sceneclass::AmmoClass::FirstExecute(void) noexcept {
	auto* DXLib_refParts = DXLib_ref::Instance();
	auto* PlayerMngr = Player::PlayerManager::Instance();
	auto* BackGround = BackGround::BackGroundClass::Instance();
	auto* SE = SoundPool::Instance();

	//移動確定
	this->m_repos = this->m_pos;
	this->m_pos = this->m_pos + (this->m_vec * (this->m_speed * DXLib_refParts->GetDeltaTime())) + Vector3DX::up() * this->m_yAdd;
	this->m_yAdd += (GravityRate / (DXLib_refParts->GetFps() * DXLib_refParts->GetFps()));

	//AmmoClass
	Vector3DX repos_tmp = this->m_repos;
	Vector3DX pos_tmp = this->m_pos;
	Vector3DX norm_tmp = Vector3DX::zero();

	bool is_HitAll = false;

	//地面との判定
	auto ColResGround = BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, &norm_tmp);
	//キャラとの判定
	for (int index = 0; index < PlayerMngr->GetPlayerNum(); index++) {
		auto& tgt = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(index)->GetChara();
		if (tgt->CheckDamageRay(this->m_AmmoData->GetDamage(), (PlayerID)this->m_ShootCheraID, repos_tmp, &pos_tmp)) {
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
			auto Put = BackGround->GetPoint(pos_tmp);
			for (int xp = -xput / 2; xp <= xput / 2; xp++) {
				for (int yp = -yput / 2; yp <= yput / 2; yp++) {
					for (int zp = -zput / 2; zp <= zput / 2; zp++) {
						int xx = (Put.x + xp);
						int yy = (Put.y + yp);
						int zz = (Put.z + zp);
						auto& cell = BackGround->GetCellBuf(xx, yy, zz);
						switch (cell.m_Cell) {
						case 2:
							BackGround->SetBlick(xx, yy, zz, 3);
							break;
						case 3:
							BackGround->SetBlick(xx, yy, zz, FPS_n2::BackGround::s_EmptyBlick);
							break;
						default:
							break;
						}
					}
				}
			}
		}
		//エフェクト
		EffectSingleton::Instance()->SetOnce_Any(Sceneclass::Effect::ef_gndsmoke, pos_tmp, norm_tmp, this->m_AmmoData->GetCaliber() / 0.02f * Scale3DRate);//0.00762f
		//サウンド
		SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGround0) + GetRand(5 - 1))->Play3D(pos_tmp, Scale3DRate * 10.f);
	}
	//消す(スピードが0以下、貫通が0以下、5回反射する)
	if (this->m_speed <= 0.f || this->m_penetration <= 0.f || this->m_RicochetCnt > 5 || this->m_Timer > 5.f) {
		SetDelete();
	}
	this->m_Timer += DXLib_refParts->GetDeltaTime();
}
