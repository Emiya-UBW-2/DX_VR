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
			int								m_RecoilPower{120};
			float							m_RecoilReturn{0.9f};
			SHOTTYPE						m_ShotType{SHOTTYPE::SEMI};		//
			int								m_HumanAnimType{0};				//
			int								m_SoundSel{0};
			RELOADTYPE						m_ReloadType{RELOADTYPE::MAG};	//
			GunShootSound					m_GunShootSound{GunShootSound::Normal};
		public://ƒQƒbƒ^[
			const auto& GetShotRate(void) const noexcept { return this->m_ShotRate; }
			const auto& GetRecoilPower(void) const noexcept { return this->m_RecoilPower; }
			const auto& GetRecoilReturn(void) const noexcept { return this->m_RecoilReturn; }
			const auto& GetShotType(void) const noexcept { return this->m_ShotType; }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_HumanAnimType; }
			const auto& GetSoundSel(void) const noexcept { return this->m_SoundSel; }
			const auto& GetReloadType(void) const noexcept { return this->m_ReloadType; }
			const auto& GetGunShootSound(void) const noexcept { return this->m_GunShootSound; }
		public://
			void		SetMod(const std::string& LEFT, const std::string&RIGHT) noexcept override {

				if (LEFT == "ShotRate") {
					this->m_ShotRate = std::stoi(RIGHT);
				}
				else if (LEFT == "RecoilPower") {
					this->m_RecoilPower = std::stoi(RIGHT);
				}
				else if (LEFT == "RecoilReturn") {
					this->m_RecoilReturn = std::stof(RIGHT);
				}
				else if (LEFT == "shottype") {
					for (int i = 0; i < (int)SHOTTYPE::Max; i++) {
						if (RIGHT == SHOTTYPEName[i]) {
							this->m_ShotType = (SHOTTYPE)i;
							break;
						}
					}
				}
				else if (LEFT == "usehumanAnim") {
					this->m_HumanAnimType = std::stoi(RIGHT);
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
				else if (LEFT == "GunShootSound") {
					//GunShootSoundName[]
					for (int i = 0; i < (int)GunShootSound::Max; i++) {
						if (RIGHT == GunShootSoundName[i]) {
							this->m_GunShootSound = (GunShootSound)i;
							break;
						}
					}
				}
				else {
					ModDataClass::SetSlot(LEFT, RIGHT);
				}
			}
		};
	};
};
