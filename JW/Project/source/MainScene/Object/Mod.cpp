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
				m_ModControl.SetParts(m_ModDataClass->GetPartsSlot(GunSlot::UnderRail)->m_Items.at(GetRand(size - 1)), ObjType::UnderRail, GetObj());
			}
			if (m_ModDataClass->GetPartsSlot(GunSlot::Sight)) {
				int size = (int)m_ModDataClass->GetPartsSlot(GunSlot::Sight)->m_Items.size();
				m_ModControl.SetParts(m_ModDataClass->GetPartsSlot(GunSlot::Sight)->m_Items.at(GetRand(size - 1)), ObjType::Sight, GetObj());
			}
			//
			Init_Mod();
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
	};
};
