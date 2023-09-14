#include	"Gun.hpp"

#include "../../ObjectManager.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void GunClass::SetMagazine(const char* MagName) noexcept {
			if (MagName != nullptr) {
				std::string magName = MagName;
				auto* ObjMngr = ObjectManager::Instance();
				this->m_Mag_Ptr = (std::shared_ptr<MagazineClass>&)(*ObjMngr->AddObject(ObjType::Magazine, PHYSICS_SETUP::DISABLE, false, ("data/mag/" + magName + "/").c_str()));
				this->m_Mag_Ptr->SetAmmo(GetAmmoAll());
				this->m_in_chamber = false;
			}
			else {
				this->m_Mag_Ptr.reset();
			}
		}
		void GunClass::ExecuteCartInChamber(void) noexcept {
			auto Prev = this->m_IsChamberMove;//GetNowAnime
			this->m_IsChamberMove = false;
			bool IsEject = false;
			switch (GetShotType()) {
			case SHOTTYPE::FULL:
			case SHOTTYPE::SEMI:
				this->m_IsChamberMove |= ((this->m_ShotPhase == CharaAnimPhase::Shot) && (GetNowAnime().time >= 3.f));
				IsEject |= ((this->m_ShotPhase == CharaAnimPhase::Shot) && (GetNowAnime().time >= 1.f));

				this->m_IsChamberMove |= ((this->m_ShotPhase == CharaAnimPhase::Cocking) && (GetNowAnime().time >= 25.f));
				IsEject |= ((this->m_ShotPhase == CharaAnimPhase::Cocking) && (GetNowAnime().time >= 19.f));
				break;
			case SHOTTYPE::BOLT:
				this->m_IsChamberMove |= ((this->m_ShotPhase == CharaAnimPhase::LoadEnd) && (GetNowAnime().time >= 5.f));
				IsEject |= ((this->m_ShotPhase == CharaAnimPhase::Unload) && (GetNowAnime().time >= 19.f));

				this->m_IsChamberMove |= ((this->m_ShotPhase == CharaAnimPhase::Cocking) && (GetNowAnime().time >= 25.f));
				IsEject |= ((this->m_ShotPhase == CharaAnimPhase::Cocking) && (GetNowAnime().time >= 19.f));
				break;
			default:
				break;
			}

			if (this->m_IsChamberMove) {
				if (this->m_IsChamberMove != Prev) {
					this->m_AmmoData = this->m_Mag_Ptr->GetAmmoSpec();//マガジンの一番上の弾データを取る
					if (!this->GetIsMagEmpty()) {
						this->m_in_chamber = true;
					}
					this->m_Mag_Ptr->SubAmmo();//チャンバーインtodo
					this->m_ChamberMovePer = 0.f;
				}
			}
		}
		void GunClass::SetBullet(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();

			SE->Get((int)SoundEnum::Trigger).Play_3D(0, GetMatrix().pos(), Scale_Rate*5.f);
			SE->Get((int)m_GunSoundSet[this->m_SoundSel].m_Shot).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);

			this->m_in_chamber = false;

			this->m_IsShot = true;
			//
			auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->AddObject(ObjType::Ammo));
			auto mat = MATRIX_ref::RotZ(deg2rad(GetRandf(1.f))) * MATRIX_ref::RotX(deg2rad(GetRandf(1.f))) * GetMuzzleMatrix();
			LastAmmo->Put(this->m_AmmoData, mat.pos(), mat.zvec() * -1.f, m_MyID);
			this->m_AmmoData.reset();
			this->m_AmmoData = nullptr;
		}
	};
};
