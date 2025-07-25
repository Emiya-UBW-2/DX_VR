﻿#include	"ModData.hpp"

const FPS_n2::Guns::GunPartsDataManager* SingletonBase<FPS_n2::Guns::GunPartsDataManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Guns {

		void		GunPartsData::SetSlot(const std::string& LEFT, const std::string& RIGHT) noexcept {
			if (LEFT.find("Slot") != std::string::npos) {
				if (LEFT.find("Type") != std::string::npos) {
					for (int loop = 0; loop < static_cast<int>(GunSlot::Max); ++loop) {
						if (RIGHT == GunSlotName[loop]) {
							this->m_SlotInfo.emplace_back(std::make_unique< SlotInfo>());
							this->m_SlotInfo.back()->SlotType = (GunSlot)loop;
							break;
						}
					}
				}
				else if (LEFT.find("Item") != std::string::npos) {
					std::string Path;
					Path = "data/Mods/";
					Path += GunSlotName[static_cast<int>(this->m_SlotInfo.back()->SlotType)];
					Path += "/" + RIGHT + "/";
					this->m_SlotInfo.back()->CanAttachItemsUniqueID.emplace_back(GunPartsDataManager::Instance()->Add(Path));
				}
				else if (LEFT.find("Conflict") != std::string::npos) {
					this->m_SlotInfo.back()->ConflictsItemsUniqueID.emplace_back(RIGHT);
				}
				else if (LEFT.find("IsNeed") != std::string::npos) {
					this->m_SlotInfo.back()->IsNeed = (RIGHT == "true");
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
			else if (LEFT == "RecoilScaleRate") {
				this->m_RecoilScaleRate = std::stoi(RIGHT);
				this->m_IsRecoilScaleRate = true;
			}
			else if (LEFT == "shottype") {
				for (int loop = 0; loop < static_cast<int>(SHOTTYPE::Max); ++loop) {
					if (RIGHT == SHOTTYPEName[loop]) {
						this->m_ShotType = (SHOTTYPE)loop;
						this->m_IsShotType = true;
						break;
					}
				}
			}

			else if (LEFT == "stockType") {
				for (int loop = 0; loop < static_cast<int>(STOCKTYPE::Max); ++loop) {
					if (RIGHT == STOCKTYPEName[loop]) {
						this->m_StockType = (STOCKTYPE)loop;
						break;
					}
				}
			}

			else if (LEFT == "GunShootSound") {
				for (int loop = 0; loop < static_cast<int>(EnumGunSound::Max); ++loop) {
					if (RIGHT == EnumGunSoundName[loop]) {
						this->m_GunShootSound = (EnumGunSound)loop;
						break;
					}
				}
			}

			else if (LEFT == "zoom") {
				this->m_ZoomSize = std::stof(RIGHT);
			}

			else if (LEFT == "usehumanAnim") {
				this->m_HumanAnimType = std::stoi(RIGHT);
			}

			else if (LEFT == "capacity") {
				this->m_CapacityMax = (HitPoint)std::stoi(RIGHT);		//総弾数
			}
			else if (LEFT.find("useammo" + std::to_string(this->m_AmmoSpecID.size())) != std::string::npos) {
				this->m_AmmoSpecID.emplace_back(Objects::AmmoDataManager::Instance()->Add("data/ammo/" + RIGHT + "/"));
			}

			else if (LEFT == "ShootRate_Diff") {
				this->m_ShootRate_Diff = std::stoi(RIGHT);
			}
			else if (LEFT == "Recoil_Diff") {
				this->m_Recoil_Diff = std::stoi(RIGHT);
			}
			else if (LEFT == "IronSight") {
				this->m_IronSight = true;
			}
			else if (LEFT == "Info") {
				this->m_Info.emplace_back(RIGHT);
			}
			else if (LEFT == "Info_Eng") {
				this->m_InfoEng.emplace_back(RIGHT);
			}

			else if (LEFT == "ShotRate") {
				this->m_ShotRate = std::stoi(RIGHT);
			}
			else if (LEFT == "soundselect") {
				this->m_SoundSelect = std::stoi(RIGHT);
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
			else if (LEFT == "CanADS") {
				this->m_CanADS = RIGHT == "True";
			}
			else if (LEFT == "CanSwitch") {
				this->m_CanSwitch = RIGHT == "True";
			}
			else if (LEFT == "isHoldOpen") {
				this->m_isHoldOpen = RIGHT == "True";
			}
			else if (LEFT == "IsJungle") {
				this->m_IsJungle = RIGHT == "True";
			}
			else if (LEFT == "Weight") {
				this->m_Weight = std::stoi(RIGHT);
			}
			else if (LEFT == "ReloadSpeedAdd") {
				this->m_ReloadSpeedAdd = std::stof(RIGHT);
			}
			else if (LEFT == "AimSpeedAdd") {
				this->m_AimSpeedAdd = std::stof(RIGHT);
			}
			else if (LEFT == "AutoAimSpeedAdd") {
				this->m_AutoAimSpeedAdd = std::stof(RIGHT);
				}
			else {
				for (int loop = 0; loop < static_cast<int>(Charas::GunAnimeID::Max); ++loop) {
					if (LEFT == Charas::GunAnimeIDName[loop]) {
						this->m_AnimSelect[loop] = std::stoi(RIGHT);
						break;
					}
				}
			}
		}

		const int	GunPartsDataManager::Add(const std::string& filepath) noexcept {
			auto Find = std::find_if(this->m_Data.begin(), this->m_Data.end(), [&](const  std::unique_ptr<GunPartsData>& tgt) {return tgt->GetPath() == filepath; });
			if (Find != this->m_Data.end()) {
				return (*Find)->GetUniqueID();
			}
			this->m_Data.emplace_back(std::make_unique<GunPartsData>());
			auto UniqueID = this->m_LastUniqueID;
			this->m_Data.back()->SetUniqueID(UniqueID);
			++this->m_LastUniqueID;
			this->m_Data.back()->Set(filepath);
			return UniqueID;
		}
	}
}
