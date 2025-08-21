#include	"Ammo.hpp"

#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../MainScene/Object/Character.hpp"
#include	"../../MainScene/Player/Player.hpp"

const FPS_n2::Objects::AmmoPool* SingletonBase<FPS_n2::Objects::AmmoPool>::m_Singleton = nullptr;
const FPS_n2::Objects::AmmoLinePool* SingletonBase<FPS_n2::Objects::AmmoLinePool>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Objects {
		void		AmmoObj::Put(const std::unique_ptr<AmmoData>* pAmmoData, const Vector3DX& pos, const Vector3DX& vec, PlayerID myID) noexcept {
			this->m_RicochetCnt = 0;
			this->m_pos = pos;
			this->m_repos = pos;
			this->m_vec = vec;
			this->m_AmmoData = pAmmoData;
			this->m_speed = (*this->m_AmmoData)->GetSpeed() * Scale3DRate;
			this->m_penetration = (*this->m_AmmoData)->GetPenetration();
			this->m_yAdd = 0.0f;
			this->m_Timer = 0.0f;
			this->m_ShootCheraID = myID;
			SetActive(true);
			this->m_IsDrawLine = true;
			this->m_EffectTimer = 1.f;
			this->m_HP = 2;

			if ((*this->m_AmmoData)->GetEffectID() != InvalidID) {
				auto* OptionParts = OptionManager::Instance();
				if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
					if (this->m_EffectUniqueID != InvalidID) {
						EffectSingleton::Instance()->StopEffectAny(Effect::ef_rocket, this->m_EffectUniqueID);
					}
					this->m_EffectUniqueID = EffectSingleton::Instance()->SetLoopAny(Effect::ef_rocket, this->m_pos);
				}
			}
		}
		void		AmmoObj::FirstUpdate(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();

			if (!IsActive()) {
				this->m_IsDrawLine = false;
				if (this->m_AmmoData && (this->m_EffectUniqueID != InvalidID)) {
					if (this->m_EffectTimer < 0.f) {
						if ((*this->m_AmmoData)->GetEffectID() != InvalidID) {
							auto* OptionParts = OptionManager::Instance();
							if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
								EffectSingleton::Instance()->StopEffectAny(Effect::ef_rocket, this->m_EffectUniqueID);
							}
							this->m_EffectUniqueID = InvalidID;
						}
					}
					else {
						this->m_EffectTimer -= DXLib_refParts->GetDeltaTime();
					}
				}
				return;
			}
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

			auto SetBreakWall = [&]() {
				//壁破壊
				{
					int								xput = 4;
					int								yput = 2;
					int								zput = 4;
					int8_t							damage = 21;
					if ((*this->m_AmmoData)->GetCaliber() <= 0.006f) {//AR,散弾
						damage = 15;
						xput = 4;
						yput = 2;
						zput = 4;
					}
					if ((*this->m_AmmoData)->GetCaliber() >= 0.0127f) {//機銃
						damage = 50;
						xput = 6;
						yput = 3;
						zput = 6;
					}
					if ((*this->m_AmmoData)->GetCaliber() >= 0.025f) {//機関砲
						damage = 100;
						xput = 9;
						yput = 9;
						zput = 9;
					}
					if ((*this->m_AmmoData)->GetCaliber() > 0.050f) {//ロケット
						damage = 34;
						xput = 8;
						yput = 5;
						zput = 8;
					}
					auto Put = BackGroundParts->GetPoint(pos_tmp);
					bool IsChanged = false;
					for (int xp = -xput / 2; xp <= xput / 2; ++xp) {
						for (int yp = -yput / 2; yp <= yput / 2; ++yp) {
							for (int zp = -zput / 2; zp <= zput / 2; ++zp) {
								if (BackGroundParts->DamageCell(Put.x + xp, Put.y + yp, Put.z + zp, damage)) {
									IsChanged = true;
								}
							}
						}
					}
					if (IsChanged) {
						auto* OptionParts = OptionManager::Instance();
						if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 2) {
							EffectSingleton::Instance()->SetOnce_Any(Effect::ef_break, BackGroundParts->GetPos(Put.x, Put.y, Put.z), Vector3DX::forward(), 3.0f, 3.0f);
						}
					}
				}
				//エフェクト
				auto* OptionParts = OptionManager::Instance();
				if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
					EffectSingleton::Instance()->SetOnce_Any(Effect::ef_gndsmoke, pos_tmp, norm_tmp, std::min(0.0127f, (*this->m_AmmoData)->GetCaliber()) / 0.02f * Scale3DRate);
				}
				};

			bool is_HitAll = false;

			//地面との判定
			bool ColResGround = false;

			auto Sub = BackGroundParts->CheckLinetoMap(repos_tmp, &pos_tmp, &norm_tmp);
			if (Sub != 0) {
				this->m_HP -= Sub;
				ColResGround = (this->m_HP <= 0);
				if (!ColResGround) {
					SetBreakWall();
					pos_tmp = this->m_pos;
				}
			}
			//ヘリとの判定
			if (this->m_ShootCheraID != -1 && PlayerMngr->GetHelicopter() && PlayerMngr->GetHelicopter()->CheckAmmoHit(this->m_ShootCheraID, repos_tmp, &pos_tmp)) {
				SetActive(false);
				is_HitAll = true;
				if (this->m_ShootCheraID == PlayerMngr->GetWatchPlayerID()) {//撃ったキャラ
					HitMarkerPool::Instance()->AddMarker(pos_tmp, 0, (*this->m_AmmoData)->GetDamage());
				}
			}
			if (this->m_ShootCheraID != -1 && PlayerMngr->GetTeamHelicopter()->CheckAmmoHit(this->m_ShootCheraID, repos_tmp, &pos_tmp)) {
				SetActive(false);
				is_HitAll = true;
				if (this->m_ShootCheraID == PlayerMngr->GetWatchPlayerID()) {//撃ったキャラ
					HitMarkerPool::Instance()->AddMarker(pos_tmp, 0, (*this->m_AmmoData)->GetDamage());
				}
			}
			/*
			//戦車との判定
			if (this->m_ShootCheraID != -1 && PlayerMngr->GetVehicle()->CheckAmmoHit(this->m_ShootCheraID, repos_tmp, &pos_tmp)) {
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
				SetBreakWall();
				//サウンド
				SE->Get(SoundType::SE, static_cast<int>(SoundEnum::HitGround0))->Play3D(pos_tmp, Scale3DRate * 10.0f);
			}
			//消す(スピードが0以下、貫通が0以下、5回反射する)
			if (this->m_speed <= 0.0f || this->m_penetration <= 0.0f || this->m_RicochetCnt > 5 || this->m_Timer > 5.0f) {
				SetActive(false);
			}
			this->m_Timer += DXLib_refParts->GetDeltaTime();
			this->m_pos = pos_tmp;
			if ((*this->m_AmmoData)->GetEffectID() != InvalidID) {
				auto* OptionParts = OptionManager::Instance();
				if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) >= 1) {
					EffectSingleton::Instance()->Update_LoopAnyEffect(Effect::ef_rocket, this->m_EffectUniqueID, this->m_pos, this->m_vec, 1.0f);
				}
			}
		}

		void		AmmoLine::Update() noexcept {
			if (!IsActive()) { return; }
			auto* DXLib_refParts = DXLib_ref::Instance();
			m_Time -= DXLib_refParts->GetDeltaTime();
			Easing(&m_Per, std::clamp(this->m_Time, 0.f, 1.f), 0.9f, EasingType::OutExpo);
		}
	}
}
