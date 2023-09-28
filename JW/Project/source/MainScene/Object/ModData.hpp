#pragma once
#include	"../../Header.hpp"

#include "ObjectBase_before.hpp"
#include "GunEnum.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ModDataClass : public ItemData {
			struct PartsSlot {
				GunSlot						m_GunSlot;
				std::vector<std::string>	m_Items;
				std::vector<std::string>	m_Conflicts;
			};
		private:
			std::vector<PartsSlot>			m_PartsSlot;						//
		protected:
			void		SetSlot(const std::string& LEFT, const std::string&RIGHT) noexcept {
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
						this->m_PartsSlot.back().m_Items.emplace_back(RIGHT);
					}
					else if (LEFT.find("Conflict") != std::string::npos) {
						this->m_PartsSlot.back().m_Conflicts.emplace_back(RIGHT);
					}
				}
			}
		public://ƒQƒbƒ^[
			const PartsSlot* GetPartsSlot(GunSlot sel) const noexcept {
				for (const auto& s : this->m_PartsSlot) {
					if (s.m_GunSlot == sel) {
						return &s;
					}
				}
				return nullptr;
			}
		public://
			void		Set_Sub(const std::string& LEFT, const std::string&RIGHT) noexcept override {
				SetSlot(LEFT, RIGHT);
			}
		};

		class ModDataManager : public SingletonBase<ModDataManager> {
		private:
			friend class SingletonBase<ModDataManager>;
		private:
			std::vector<std::shared_ptr<ModDataClass>>	m_Object;
		public:
			const auto&	LoadAction(const std::string& filepath) noexcept {
				for (auto& o : m_Object) {
					if (o->GetPath() == filepath) {
						return o;
					}
				}
				m_Object.resize(m_Object.size() + 1);
				m_Object.back() = std::make_shared<ModDataClass>();
				m_Object.back()->Set(filepath);
				return m_Object.back();
			}
		};
	};
};
