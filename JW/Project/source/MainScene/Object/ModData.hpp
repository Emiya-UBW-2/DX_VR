#pragma once
#include	"../../Header.hpp"

#include "ObjectBase_before.hpp"
#include "GunEnum.hpp"
#include "AmmoData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ModDataClass : public ItemData {
		public://ゲッター
			struct PartsSlot {
				GunSlot						m_GunSlot{GunSlot::Gun};
				std::vector<int>			m_ItemsUniqueID;
				std::vector<std::string>	m_Conflicts;
				bool						m_IsNeed{false};

				const auto					ItemMaxCount(void) const noexcept { return  this->m_IsNeed ? ((int)this->m_ItemsUniqueID.size() - 1) : (int)this->m_ItemsUniqueID.size(); }
			};
		private:
			int								m_UniqueID{0};
			std::vector<PartsSlot>			m_PartsSlot;						//
		private:
			bool							m_IsRecoilPower{false};
			bool							m_IsRecoilReturn{false};
			bool							m_IsShotType{false};		//

			int								m_RecoilPower{120};
			float							m_RecoilReturn{0.9f};
			SHOTTYPE						m_ShotType{SHOTTYPE::SEMI};		//
			bool							m_IronSight{false};

			GunShootSound					m_GunShootSound{GunShootSound::Normal};

			GraphHandle						m_Reitcle;
			float							m_ZoomSize{1.f};
			int								m_HumanAnimType{-1};				//

			int								m_ShootRate_Diff{0};
			int								m_ReloadRate_Diff{0};


			std::vector<std::shared_ptr<AmmoDataClass>>	m_AmmoSpec;
			std::vector<std::string>					m_Info;
			int											m_CapacityMax{0};
		public://ゲッター
			//性能
			const auto& GetIsRecoilPower(void) const noexcept { return this->m_IsRecoilPower; }
			const auto& GetIsRecoilReturn(void) const noexcept { return this->m_IsRecoilReturn; }
			const auto& GetIsShotType(void) const noexcept { return this->m_IsShotType; }
			const auto& GetRecoilPower(void) const noexcept { return this->m_RecoilPower; }
			const auto& GetRecoilReturn(void) const noexcept { return this->m_RecoilReturn; }
			const auto& GetShotType(void) const noexcept { return this->m_ShotType; }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_HumanAnimType; }
			const auto& GetIronSight(void) const noexcept { return this->m_IronSight; }
			const auto&		GetInfo(void) const noexcept { return this->m_Info; }
			//銃声
			const auto&	GetGunShootSound(void) const noexcept { return this->m_GunShootSound; }
			//スコープ
			const auto&		GetReitcleGraph(void) const noexcept { return this->m_Reitcle; }
			const auto&		GetZoomSize(void) const noexcept { return this->m_ZoomSize; }
			//マガジン
			const auto&		GetAmmoAll(void) const noexcept { return this->m_CapacityMax; }
			const auto&		GetAmmoSpecMagTop(void) const noexcept { return this->m_AmmoSpec[0]; }
			//性能周り
			const auto&		GetShootRate_Diff(void) const noexcept { return this->m_ShootRate_Diff; }
			const auto&		GetReloadRate_Diff(void) const noexcept { return this->m_ReloadRate_Diff; }
		protected:
			void				SetSlot(const std::string& LEFT, const std::string&RIGHT) noexcept;
			virtual void		SetMod_Sub(const std::string&, const std::string&) noexcept {}
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
			void		Load_Sub(const std::string& Path) noexcept override {
				FILEINFO FileInfo;
				if (FileRead_findFirst((Path + "reticle_0.png").c_str(), &FileInfo) != (DWORD_PTR)-1) {
					m_Reitcle = GraphHandle::Load(Path + "reticle_0.png");
				}

				this->m_AmmoSpec.clear();
				this->m_Info.clear();
			}
			void		Set_Sub(const std::string& LEFT, const std::string&RIGHT) noexcept override {
				SetSlot(LEFT, RIGHT);
				SetMod_Sub(LEFT, RIGHT);
			}
		};

		class ModDataManager : public SingletonBase<ModDataManager> {
		private:
			friend class SingletonBase<ModDataManager>;
		private:
			std::list<std::shared_ptr<ModDataClass>>	m_Object;
			int											m_LastUniqueID{0};
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
