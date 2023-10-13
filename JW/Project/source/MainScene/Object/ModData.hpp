#pragma once
#include	"../../Header.hpp"

#include "ObjectBase_before.hpp"
#include "GunEnum.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ModDataClass : public ItemData {
		public://ゲッター
			struct PartsSlot {
				GunSlot						m_GunSlot;
				std::vector<int>			m_ItemsUniqueID;
				std::vector<std::string>	m_Conflicts;
				bool						m_IsNeed{ false };
			};
		private:
			int								m_UniqueID{ 0 };
			std::vector<PartsSlot>			m_PartsSlot;						//
		protected:
			void				SetSlot(const std::string& LEFT, const std::string&RIGHT) noexcept;
			virtual void		SetMod(const std::string& LEFT, const std::string&RIGHT) noexcept {
				SetSlot(LEFT, RIGHT);
			}
		public://ゲッター
			const PartsSlot* GetPartsSlot(GunSlot sel) const noexcept {
				for (const auto& s : this->m_PartsSlot) {
					if (s.m_GunSlot == sel) {
						return &s;
					}
				}
				return nullptr;
			}
			const auto&		GetUniqueID(void) const noexcept { return this->m_UniqueID; }
		public://
			void			SetUniqueID(int value) noexcept { m_UniqueID = value; }
		public://
			void		Set_Sub(const std::string& LEFT, const std::string&RIGHT) noexcept override {
				SetMod(LEFT, RIGHT);
			}
		};

		class ModDataManager : public SingletonBase<ModDataManager> {
		private:
			friend class SingletonBase<ModDataManager>;
		private:
			std::list<std::shared_ptr<ModDataClass>>	m_Object;
			int											m_LastUniqueID{ 0 };
		private:
			ModDataManager() {}
			~ModDataManager() {}
		public:
			const std::shared_ptr<ModDataClass>*	GetData(int uniqueID) noexcept {
				for (auto& o : m_Object) {
					if (o->GetUniqueID() == uniqueID) {
						return &o;
					}
				}
				return nullptr;
			}
		public:
			const std::shared_ptr<ModDataClass>*	AddData(const std::string& filepath, bool isMod) noexcept;
		};
	};
};
