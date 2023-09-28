#pragma once
#include	"../../Header.hpp"

#include "ObjectBase_before.hpp"
#include "GunEnum.hpp"
#include "ModData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunDataClass : public ModDataClass {
		private:
			SHOTTYPE						m_ShotType{ SHOTTYPE::SEMI };		//
			int								m_HumanAnimType{ 0 };				//
			int								m_SoundSel{ 0 };
			RELOADTYPE						m_ReloadType{ RELOADTYPE::MAG };	//
		public://ƒQƒbƒ^[
			const auto& GetShotType(void) const noexcept { return this->m_ShotType; }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_HumanAnimType; }
			const auto& GetSoundSel(void) const noexcept { return this->m_SoundSel; }
			const auto& GetReloadType(void) const noexcept { return this->m_ReloadType; }
		public://
			void		Set_Sub(const std::string& LEFT, const std::string&RIGHT) noexcept override {
				if (LEFT == "shottype") {
					if (RIGHT == "FULL") {
						this->m_ShotType = SHOTTYPE::FULL;
					}
					else if (RIGHT == "SEMI") {
						this->m_ShotType = SHOTTYPE::SEMI;
					}
					else if (RIGHT == "BOLT") {
						this->m_ShotType = SHOTTYPE::BOLT;
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
				else {
					ModDataClass::SetSlot(LEFT, RIGHT);
				}
			}
		};

		class GunDataManager : public SingletonBase<GunDataManager> {
		private:
			friend class SingletonBase<GunDataManager>;
		private:
			std::vector<std::shared_ptr<GunDataClass>>	m_Object;
		public:
			const auto&	LoadAction(const std::string& filepath) noexcept {
				for (auto& o : m_Object) {
					if (o->GetPath() == filepath) {
						return o;
					}
				}
				m_Object.resize(m_Object.size() + 1);
				m_Object.back() = std::make_shared<GunDataClass>();
				m_Object.back()->Set(filepath);
				return m_Object.back();
			}
		};
	};
};
