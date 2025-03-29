#include	"ModData.hpp"

const FPS_n2::Sceneclass::ModDataManager* SingletonBase<FPS_n2::Sceneclass::ModDataManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {

		void		ModDataClass::SetSlot(const std::string& LEFT, const std::string& RIGHT) noexcept {
			if (LEFT.find("Slot") != std::string::npos) {
				if (LEFT.find("Type") != std::string::npos) {
					for (int i = 0; i < static_cast<int>(GunSlot::Max); i++) {
						if (RIGHT == GunSlotName[i]) {
							this->m_PartsSlot.resize(this->m_PartsSlot.size() + 1);
							this->m_PartsSlot.back().m_GunSlot = (GunSlot)i;
							break;
						}
					}
				}
				else if (LEFT.find("Item") != std::string::npos) {
					std::string Path;
					Path = "data/Mods/";
					Path += GunSlotName[static_cast<int>(this->m_PartsSlot.back().m_GunSlot)];
					Path += "/" + RIGHT + "/";
					this->m_PartsSlot.back().m_ItemsUniqueID.emplace_back((*ModDataManager::Instance()->AddData(Path))->GetUniqueID());
				}
				else if (LEFT.find("Conflict") != std::string::npos) {
					this->m_PartsSlot.back().m_Conflicts.emplace_back(RIGHT);
				}
				else if (LEFT.find("IsNeed") != std::string::npos) {
					this->m_PartsSlot.back().m_IsNeed = (RIGHT == "true");
				}
			}

			if (LEFT == "Name") {
				this->m_name = RIGHT;
			}
			else if (LEFT == "RecoilPower") {
				this->m_RecoilPower = std::stoi(RIGHT);
				this->m_IsRecoilPower = true;
			}
			else if (LEFT == "RecoilReturn") {
				this->m_RecoilReturn = std::stof(RIGHT);
				this->m_IsRecoilReturn = true;
			}
			else if (LEFT == "shottype") {
				for (int i = 0; i < static_cast<int>(SHOTTYPE::Max); i++) {
					if (RIGHT == SHOTTYPEName[i]) {
						this->m_ShotType = (SHOTTYPE)i;
						this->m_IsShotType = true;
						break;
					}
				}
			}

			else if (LEFT == "GunShootSound") {
				for (int i = 0; i < static_cast<int>(GunShootSound::Max); i++) {
					if (RIGHT == GunShootSoundName[i]) {
						this->m_GunShootSound = (GunShootSound)i;
						break;
					}
				}
			}

			else if (LEFT == "zoom") {
				m_ZoomSize = std::stof(RIGHT);
			}

			else if (LEFT == "usehumanAnim") {
				this->m_HumanAnimType = std::stoi(RIGHT);
			}

			else if (LEFT == "capacity") {
				this->m_CapacityMax = (HitPoint)std::stoi(RIGHT);		//‘’e”
			}
			else if (LEFT.find("useammo" + std::to_string(this->m_AmmoSpec.size())) != std::string::npos) {
				this->m_AmmoSpec.emplace_back(AmmoDataManager::Instance()->LoadAction("data/ammo/" + RIGHT + "/"));
			}

			else if (LEFT == "ShootRate_Diff") {
				m_ShootRate_Diff = std::stoi(RIGHT);
			}
			else if (LEFT == "Recoil_Diff") {
				m_Recoil_Diff = std::stoi(RIGHT);
			}
			else if (LEFT == "IronSight") {
				this->m_IronSight = true;
			}
			else if (LEFT == "Info") {
				m_Info.emplace_back(RIGHT);
			}
			else if (LEFT == "Info_Eng") {
				m_InfoEng.emplace_back(RIGHT);
			}

			else if (LEFT == "ShotRate") {
				this->m_ShotRate = std::stoi(RIGHT);
			}
			else if (LEFT == "soundsel") {
				this->m_SoundSel = std::stoi(RIGHT);
			}
			else if (LEFT == "reloadType") {
				if (RIGHT == "MAG") {
					this->m_ReloadType = RELOADTYPE::MAG;
				}
				else if (RIGHT == "AMMO") {
					this->m_ReloadType = RELOADTYPE::AMMO;
				}
			}
			else if (LEFT == "IsThrowWeapon") {
				this->m_IsThrowWeapon = RIGHT == "True";
			}
		}

		const std::shared_ptr<ModDataClass>* ModDataManager::AddData(const std::string& filepath) noexcept {
			for (auto& o : m_Object) {
				if (o->GetPath() == filepath) {
					return &o;
				}
			}
			auto tmp = std::make_shared<ModDataClass>();
			auto UniqueID = m_LastUniqueID;
			tmp->SetUniqueID(UniqueID);
			m_LastUniqueID++;
			tmp->Set(filepath);
			m_Object.emplace_back(tmp);
			return GetData(UniqueID);
		}
	};
};
