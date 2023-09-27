#include	"Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//ƒ}ƒKƒWƒ“
		void			MagazineClass::Init_Mod(void) noexcept {
			int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
			this->m_CapacityMax = (HitPoint)getparams::_int(mdata);		//‘’e”

			this->m_AmmoSpec.clear();
			while (true) {
				auto stp = getparams::Getstr(mdata);
				if (stp.find("useammo" + std::to_string(this->m_AmmoSpec.size())) == std::string::npos) {
					break;
				}
				this->m_AmmoSpec.emplace_back(AmmoDataManager::Instance()->LoadAction("data/ammo/" + getparams::getright(stp) + "/"));
			}
			FileRead_close(mdata);
		}
		void			MagazineClass::FirstExecute_Mod(void) noexcept {
			switch (m_ReloadTypeBuf) {
			case RELOADTYPE::MAG:
				if (this->HandPer > 0.f) {
					SetMove(MATRIX_ref::RotX(deg2rad(-30.f*this->HandPer))*GetMove().mat.GetRot(), Lerp(GetMove().pos, this->HandMatrix.pos(), this->HandPer));
				}
				break;
			case RELOADTYPE::AMMO:
				break;
			default:
				break;
			}
		}
		void			MagazineClass::Dispose_Mod(void) noexcept {
			for (auto& A : this->m_AmmoSpec) {
				A.reset();
			}
			this->m_AmmoSpec.clear();
		}
	};
};
