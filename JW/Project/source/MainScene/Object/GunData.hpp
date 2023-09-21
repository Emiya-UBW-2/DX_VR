#pragma once
#include	"../../Header.hpp"

#include "GunEnum.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		struct PartsSlot {
			GunSlot						m_GunSlot;
			std::vector<std::string>	m_Items;
			std::vector<std::string>	m_Conflicts;
		};

		class GunDataClass {
		private:
			std::string						m_Path;
			std::string						m_Name;
			SHOTTYPE						m_ShotType;						//
			int								m_HumanAnimType{ 0 };			//
			int								m_SoundSel{ 0 };
			RELOADTYPE						m_ReloadType;					//

			std::vector<PartsSlot>			m_PartsSlot;					//
		public://ƒQƒbƒ^[
			const auto& GetPath(void) const noexcept { return this->m_Path; }
			const auto& GetName(void) const noexcept { return this->m_Name; }
			const auto& GetShotType(void) const noexcept { return this->m_ShotType; }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_HumanAnimType; }
			const auto& GetSoundSel(void) const noexcept { return this->m_SoundSel; }
			const auto& GetReloadType(void) const noexcept { return this->m_ReloadType; }
			const PartsSlot* GetPartsSlot(GunSlot sel) const noexcept {
				for (const auto& s : this->m_PartsSlot) {
					if (s.m_GunSlot == sel) {
						return &s;
					}
				}
				return nullptr;
			}
		public://
			void SetData(const std::string& Path) {
				m_Path = Path;
				//
				enum class Load : int {
					Name,
					Shottype,
					HumanAnimeSel,
					SoundSel,
					ReloadType,
					Slot,
				};
				int mdata = FileRead_open((m_Path + "data.txt").c_str(), FALSE);
				int LoadNow = (int)Load::Name;
				while (true) {
					if (FileRead_eof(mdata) != 0) { break; }
					auto STR = getparams::Getstr(mdata);
					if (STR == "") { continue; }
					auto RIGHT = getparams::getright(STR);

					bool isBreak = false;
					switch ((Load)LoadNow) {
					case Load::Name:
						this->m_Name = RIGHT;
						LoadNow++;
						break;
					case Load::Shottype:
						if (RIGHT == "FULL") {
							this->m_ShotType = SHOTTYPE::FULL;
						}
						else if (RIGHT == "SEMI") {
							this->m_ShotType = SHOTTYPE::SEMI;
						}
						else if (RIGHT == "BOLT") {
							this->m_ShotType = SHOTTYPE::BOLT;
						}
						LoadNow++;
						break;
					case Load::HumanAnimeSel:
						this->m_HumanAnimType = std::stoi(RIGHT);
						LoadNow++;
						break;
					case Load::SoundSel:
						this->m_SoundSel = std::stoi(RIGHT);
						LoadNow++;
						break;
					case Load::ReloadType:
						if (RIGHT == "MAG") {
							this->m_ReloadType = RELOADTYPE::MAG;
						}
						else if (RIGHT == "AMMO") {
							this->m_ReloadType = RELOADTYPE::AMMO;
						}
						LoadNow++;
						break;
					case Load::Slot:
						if (STR.find("Slot") != std::string::npos) {
							if (STR.find("Type") != std::string::npos) {
								for (int i = 0; i < (int)GunSlot::Max; i++) {
									if (RIGHT == GunSlotName[i]) {
										this->m_PartsSlot.resize(this->m_PartsSlot.size() + 1);
										this->m_PartsSlot.back().m_GunSlot = (GunSlot)i;
										break;
									}
								}
							}
							else if (STR.find("Item") != std::string::npos) {
								this->m_PartsSlot.back().m_Items.emplace_back(RIGHT);
							}
							else if (STR.find("Conflict") != std::string::npos) {
								this->m_PartsSlot.back().m_Conflicts.emplace_back(RIGHT);
							}
							break;
						}
						LoadNow++;
						break;
					default:
						isBreak = true;
						break;
					}
					if (isBreak) {
						break;
					}
				}
				FileRead_close(mdata);
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
				m_Object.back()->SetData(filepath);
				return m_Object.back();
			}
		};
	};
};
