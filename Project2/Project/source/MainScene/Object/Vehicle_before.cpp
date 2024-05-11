#include	"Vehicle_before.hpp"
#include "../../MainScene/Player/Player.hpp"
#include "../../MainScene/Inventory.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		bool		Guns::Update(bool key, PlayerID ID, const Vector3DX& pos_t, const Vector3DX& nomal_t) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();

			this->m_Recoil = std::max(this->m_Recoil - 1.f / DrawParts->GetFps(), 0.f);
			this->m_React = std::max(this->m_React - 1.f / DrawParts->GetFps(), 0.f);
			//ËŒ‚
			if (this->m_loadtimer == 0) {
				//ƒCƒ“ƒxƒ“ƒgƒŠ‚©‚ç’e‚ğ’T‚·
				const std::shared_ptr<CellItem>* PtrBuf = nullptr;
				float timeMul = 1.f;
				for (int i = 0; i < (int)this->GetData()->GetAmmoSpec().size(); i++) {
					const auto* Ptr = PlayerMngr->GetPlayer(ID).GetInventory(0, [&](const std::shared_ptr<CellItem>& tgt) {
						return tgt->GetItemData() == this->GetData()->GetAmmoSpec().at(i);
																			 });
					if (Ptr) {
						PtrBuf = Ptr;
						timeMul = 1.f;
						break;
					}
				}
				if (!PtrBuf) {
					for (int i = 0; i < (int)this->GetData()->GetAmmoSpec().size(); i++) {
						const auto* Ptr = PlayerMngr->GetPlayer(ID).GetInventory(1, [&](const std::shared_ptr<CellItem>& tgt) {
							return tgt->GetItemData() == this->GetData()->GetAmmoSpec().at(i);
																				 });
						if (Ptr) {
							PtrBuf = Ptr;
							timeMul = 2.5f;
							break;
						}
					}
				}

				if (PtrBuf && key) {
					this->m_loadtimer = GetTotalloadtime() * timeMul;
					this->m_Recoil = 1.f;
					this->m_React = std::clamp(this->m_React + GetCaliberSize() * 10.f, 0.f, 3.f);
					this->m_reloadSEFlag = true;

					//’e¶¬
					auto LastAmmo = std::make_shared<AmmoClass>();
					ObjMngr->AddObject(LastAmmo);
					LastAmmo->Init();
					LastAmmo->Put(reinterpret_cast<const std::shared_ptr<AmmoData>&>((*PtrBuf)->GetItemData()), pos_t, nomal_t, ID);
					//
					HitPoint AmmoC = 1;
					if ((*PtrBuf)->Sub(&AmmoC)) {
						PlayerMngr->GetPlayer(ID).DeleteInventory(*PtrBuf);
					}
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
