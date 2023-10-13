#include	"ModData.hpp"
#include	"GunData.hpp"

const FPS_n2::Sceneclass::ModDataManager* SingletonBase<FPS_n2::Sceneclass::ModDataManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {
		void		ModDataClass::SetSlot(const std::string& LEFT, const std::string&RIGHT) noexcept {
			if (LEFT.find("Slot") != std::string::npos) {
				if (LEFT.find("Type") != std::string::npos) {
					for (int i = 0; i < (int)GunSlot::Max; i++) {
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
					Path += GunSlotName[(int)this->m_PartsSlot.back().m_GunSlot];
					Path += "/" + RIGHT + "/";
					this->m_PartsSlot.back().m_ItemsUniqueID.emplace_back((*ModDataManager::Instance()->AddData(Path, true))->GetUniqueID());
				}
				else if (LEFT.find("Conflict") != std::string::npos) {
					this->m_PartsSlot.back().m_Conflicts.emplace_back(RIGHT);
				}
				else if (LEFT.find("IsNeed") != std::string::npos) {
					this->m_PartsSlot.back().m_IsNeed = (RIGHT == "true");
				}
			}
		}

		const std::shared_ptr<ModDataClass>*	ModDataManager::AddData(const std::string& filepath, bool isMod) noexcept {
			for (auto& o : m_Object) {
				if (o->GetPath() == filepath) {
					return &o;
				}
			}
			std::shared_ptr<ModDataClass> tmp;
			if (isMod) {
				tmp = std::make_shared<ModDataClass>();
			}
			else {
				tmp = std::make_shared<GunDataClass>();
			}
			auto UniqueID = m_LastUniqueID;
			tmp->SetUniqueID(UniqueID);
			m_LastUniqueID++;
			tmp->Set(filepath);
			m_Object.emplace_back(tmp);
			return GetData(UniqueID);
		}
	};
};
