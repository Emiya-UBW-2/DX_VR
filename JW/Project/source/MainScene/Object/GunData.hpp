#pragma once
#include	"../../Header.hpp"

#include "ObjectBase_before.hpp"
#include "GunEnum.hpp"
#include "ModData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunDataClass : public ModDataClass {
		private:
			int								m_ShotRate{300};
			int								m_HumanAnimType{0};				//
			int								m_SoundSel{0};
			RELOADTYPE						m_ReloadType{RELOADTYPE::MAG};	//
			bool							m_IronSight{false};
		public://ƒQƒbƒ^[
			const auto& GetShotRate(void) const noexcept { return this->m_ShotRate; }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_HumanAnimType; }
			const auto& GetSoundSel(void) const noexcept { return this->m_SoundSel; }
			const auto& GetReloadType(void) const noexcept { return this->m_ReloadType; }
			const auto& GetIronSight(void) const noexcept { return this->m_IronSight; }
		public://
			void		SetMod_Sub(const std::string& LEFT, const std::string&RIGHT) noexcept override {
				if (LEFT == "ShotRate") {
					this->m_ShotRate = std::stoi(RIGHT);
				}
				else if (LEFT == "usehumanAnim") {
					this->m_HumanAnimType = std::stoi(RIGHT);
				}
				else if (LEFT == "soundsel") {
					this->m_SoundSel = std::stoi(RIGHT);
				}
				else if (LEFT == "IronSight") {
					this->m_IronSight = true;
				}
				else if (LEFT == "reloadType") {
					if (RIGHT == "MAG") {
						this->m_ReloadType = RELOADTYPE::MAG;
					}
					else if (RIGHT == "AMMO") {
						this->m_ReloadType = RELOADTYPE::AMMO;
					}
				}
			}
		};
	};
};
