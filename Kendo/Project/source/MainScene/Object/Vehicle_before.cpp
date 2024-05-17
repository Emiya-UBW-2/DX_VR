#include	"Vehicle_before.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		bool		Guns::Update(bool key, PlayerID ID, const Vector3DX& pos_t, const Vector3DX& nomal_t) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();

			this->m_Recoil = std::max(this->m_Recoil - 1.f / DrawParts->GetFps(), 0.f);
			this->m_React = std::max(this->m_React - 1.f / DrawParts->GetFps(), 0.f);
			//ËŒ‚
			if (this->m_loadtimer == 0) {
				//ƒCƒ“ƒxƒ“ƒgƒŠ‚©‚ç’e‚ğ’T‚·
				const std::shared_ptr<AmmoData>& PtrBuf = this->GetData()->GetAmmoSpec().at(0);
				float timeMul = 1.f;
				if (PtrBuf && key) {
					this->m_loadtimer = GetTotalloadtime() * timeMul;
					this->m_Recoil = 1.f;
					this->m_React = std::clamp(this->m_React + GetCaliberSize() * 10.f, 0.f, 3.f);
					this->m_reloadSEFlag = true;

					//’e¶¬
					auto LastAmmo = std::make_shared<AmmoClass>();
					ObjMngr->AddObject(LastAmmo);
					LastAmmo->Init();
					LastAmmo->Put(PtrBuf, pos_t, nomal_t, ID);
					//todo:’e”ŠÇ—
					//
					if (ID == 0) {
						SE->Get((int)SoundEnum::Tank_Eject).Play(this->GetData()->GetEjectSound());
					}
					return true;
				}
			}
			else {
				if (this->m_reloadSEFlag && this->m_loadtimer < 1.f) {
					this->m_reloadSEFlag = false;
					if (ID == 0) {
						SE->Get((int)SoundEnum::Tank_Reload).Play(GetData()->GetReloadSound());
					}
				}
				this->m_loadtimer = std::max(this->m_loadtimer - 1.f / DrawParts->GetFps(), 0.f);
			}
			return false;
		}
	};
};
