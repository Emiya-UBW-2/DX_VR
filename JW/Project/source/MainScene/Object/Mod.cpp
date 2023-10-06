#include	"Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			ModClass::Init(void) noexcept {
			ObjectBaseClass::Init();
			//データ
			m_ModDataClass = ModDataManager::Instance()->LoadAction(this->m_FilePath);
			//
			if (m_ModDataClass->GetPartsSlot(GunSlot::UnderRail)) {
				int size = (int)m_ModDataClass->GetPartsSlot(GunSlot::UnderRail)->m_Items.size();
				int ID = GetRand(size);
				if (ID != size) {
					m_ModControl.SetParts(m_ModDataClass->GetPartsSlot(GunSlot::UnderRail)->m_Items.at(ID), ObjType::UnderRail, GetObj());
				}
			}
			if (m_ModDataClass->GetPartsSlot(GunSlot::Sight)) {
				int size = (int)m_ModDataClass->GetPartsSlot(GunSlot::Sight)->m_Items.size();
				int ID = GetRand(size);
				if (ID != size) {
					m_ModControl.SetParts(m_ModDataClass->GetPartsSlot(GunSlot::Sight)->m_Items.at(ID), ObjType::Sight, GetObj());
				}
			}
			if (m_ModDataClass->GetPartsSlot(GunSlot::MuzzleAdapter)) {
				int size = (int)m_ModDataClass->GetPartsSlot(GunSlot::MuzzleAdapter)->m_Items.size();
				int ID = GetRand(size);
				if (ID != size) {
					m_ModControl.SetParts(m_ModDataClass->GetPartsSlot(GunSlot::MuzzleAdapter)->m_Items.at(ID), ObjType::MuzzleAdapter, GetObj());
				}
			}
			//
			Init_Mod();
		}
		//マズル
		void			UpperClass::Init_Mod(void) noexcept {
			this->m_IsRecoilPower = false;
			this->m_IsRecoilReturn = false;
			this->m_IsShotType = false;
			//追加データ
			int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				auto ALL = getparams::Getstr(mdata);
				//コメントアウト
				if (ALL.find("//") != std::string::npos) {
					ALL = ALL.substr(0, ALL.find("//"));
				}
				//
				if (ALL == "") { continue; }
				auto LEFT = getparams::getleft(ALL);
				auto RIGHT = getparams::getright(ALL);
				//
				if (LEFT == "RecoilPower") {
					this->m_RecoilPower = std::stoi(RIGHT);
					this->m_IsRecoilPower = true;
				}
				else if (LEFT == "RecoilReturn") {
					this->m_RecoilReturn = std::stof(RIGHT);
					this->m_IsRecoilReturn = true;
				}
				else if (LEFT == "shottype") {
					for (int i = 0; i < (int)SHOTTYPE::Max; i++) {
						if (RIGHT == SHOTTYPEName[i]) {
							this->m_ShotType = (SHOTTYPE)i;
							this->m_IsShotType = true;
							break;
						}
					}
				}
			}
			FileRead_close(mdata);
		}
		//マガジン
		void			MagazineClass::Init_Mod(void) noexcept {
			this->m_AmmoSpec.clear();
			//追加データ
			int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				auto ALL = getparams::Getstr(mdata);
				//コメントアウト
				if (ALL.find("//") != std::string::npos) {
					ALL = ALL.substr(0, ALL.find("//"));
				}
				//
				if (ALL == "") { continue; }
				auto LEFT = getparams::getleft(ALL);
				auto RIGHT = getparams::getright(ALL);
				//
				if (LEFT == "capacity") {
					this->m_CapacityMax = (HitPoint)std::stoi(RIGHT);		//総弾数
				}
				if (LEFT.find("useammo" + std::to_string(this->m_AmmoSpec.size())) != std::string::npos) {
					this->m_AmmoSpec.emplace_back(AmmoDataManager::Instance()->LoadAction("data/ammo/" + RIGHT + "/"));
				}
			}
			FileRead_close(mdata);
		}
		//マズル
		void			MuzzleClass::Init_Mod(void) noexcept {
			//追加データ
			int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
			while (true) {
				if (FileRead_eof(mdata) != 0) { break; }
				auto ALL = getparams::Getstr(mdata);
				//コメントアウト
				if (ALL.find("//") != std::string::npos) {
					ALL = ALL.substr(0, ALL.find("//"));
				}
				//
				if (ALL == "") { continue; }
				auto LEFT = getparams::getleft(ALL);
				auto RIGHT = getparams::getright(ALL);
				//
				if (LEFT == "GunShootSound") {
					//GunShootSoundName[]
					for (int i = 0; i < (int)GunShootSound::Max; i++) {
						if (RIGHT == GunShootSoundName[i]) {
							this->m_GunShootSound = (GunShootSound)i;
							break;
						}
					}
					break;//これだけなのでブレイクしとく
				}
			}
			FileRead_close(mdata);
		}
	};
};
