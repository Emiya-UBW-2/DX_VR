#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void GunClass::SetBullet(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();

			auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->AddObject(ObjType::Ammo));
			LastAmmo->Put(&GetAmmoSpec(), GetMuzzleMatrix().pos(), GetMuzzleMatrix().GetRot().zvec() * -1.f, this->m_MyID);

			SE->Get((int)SoundEnum::Trigger).Play_3D(0, GetMatrix().pos(), 12.5f*5.f);
			SE->Get((int)SoundEnum::Shot_Gun).Play_3D(0, GetMatrix().pos(), 12.5f*50.f);
			if (!this->m_in_chamber) {
				this->m_Mag_Ptr->SubAmmo();//チャンバーインtodo
			}
			this->m_in_chamber = false;

			this->m_IsShot = true;
			this->m_CartFlag = true;
		}
	};
};
