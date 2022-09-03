#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void GunClass::SetMagazine(const char* MagName) noexcept {
			if (MagName != nullptr) {
				std::string magName = MagName;
				auto* ObjMngr = ObjectManager::Instance();
				m_Mag_Ptr = (std::shared_ptr<MagazineClass>&)(*ObjMngr->AddObject(ObjType::Magazine, ("data/mag/" + magName + "/").c_str()));
			}
			else {
				m_Mag_Ptr.reset();
			}
		}

		void GunClass::SetBullet(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();

			SE->Get((int)SoundEnum::Trigger).Play_3D(0, GetMatrix().pos(), Scale_Rate*5.f);
			SE->Get((int)SoundEnum::Shot_Gun).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);
			if (!this->m_in_chamber) {
				this->m_Mag_Ptr->SubAmmo();//チャンバーインtodo
			}
			this->m_NowAmmo = this->m_Mag_Ptr->GetAmmoSpec();
			this->m_in_chamber = false;
			this->m_IsShot = true;
			//
			auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->AddObject(ObjType::Ammo));
			LastAmmo->Put(this->m_NowAmmo, GetMuzzleMatrix().pos(), GetMuzzleMatrix().GetRot().zvec() * -1.f, this->m_MyID);
			//
			this->m_CartPtr = (std::shared_ptr<CartClass>&)(*ObjMngr->AddObject(ObjType::Cart, this->m_NowAmmo->GetPath().c_str(), "ammo"));
			this->m_CartPtr->SetInMag(true);
			this->m_CartPtr->SetIsEmpty(true);
		}
		void GunClass::SetCart(void) noexcept {
			if (GetAnime(GunAnimeID::Cocking).time >= 19.f || GetAnime(GunAnimeID::ReloadStart).time >= 19.f) {
				if (this->m_CartPtr.get() != nullptr) {
					float Spd = Scale_Rate *2.5f / 60.f;
					this->m_CartPtr->SetInMag(false);
					this->m_CartPtr->Set(GetCartMat().pos(), GetMuzzleMatrix().GetRot(), GetCartVec()*Spd);
					this->m_CartPtr.reset();
				}
			}
		}
	};
};
