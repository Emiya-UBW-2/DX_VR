#pragma once
#include	"../../Header.hpp"

#include "GunEnum.hpp"
#include "AmmoData.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ModDataClass {
			struct PartsSlot {
				GunSlot						m_GunSlot{ GunSlot::Gun };
				std::vector<int>			m_ItemsUniqueID;
				std::vector<std::string>	m_Conflicts;
				bool						m_IsNeed{ false };

				const auto					ItemMaxCount(void) const noexcept { return  this->m_IsNeed ? (static_cast<int>(this->m_ItemsUniqueID.size()) - 1) : static_cast<int>(this->m_ItemsUniqueID.size()); }
			};
		private:
			std::string		m_path;
			std::string		m_name;
			int								m_UniqueID{ 0 };
			std::vector<PartsSlot>			m_PartsSlot;						//

			bool							m_IsRecoilPower{ false };
			bool							m_IsRecoilReturn{ false };
			bool							m_IsShotType{ false };		//

			int								m_RecoilPower{ 120 };
			float							m_RecoilReturn{ 0.9f };
			SHOTTYPE						m_ShotType{ SHOTTYPE::SEMI };		//
			bool							m_IronSight{ false };

			GunShootSound					m_GunShootSound{ GunShootSound::Normal };

			GraphHandle						m_Reitcle;
			float							m_ZoomSize{ 1.f };
			int								m_HumanAnimType{ -1 };				//

			int								m_ShootRate_Diff{ 0 };
			int								m_Recoil_Diff{ 0 };

			int								m_ShotRate{ 300 };
			int								m_SoundSel{ 0 };
			RELOADTYPE						m_ReloadType{ RELOADTYPE::MAG };	//
			bool							m_IsThrowWeapon{ false };
			bool							m_CanADS{ true };
			bool							m_CanSwitch{ true };


			std::array<int, static_cast<int>(GunAnimeID::Max)>	m_AnimSelect{ -1 };

			std::vector<std::shared_ptr<AmmoDataClass>>	m_AmmoSpec;
			std::vector<std::string>					m_Info;
			std::vector<std::string>					m_InfoEng;
			int											m_CapacityMax{ 0 };
		public://ゲッター
			const auto& GetPath(void) const noexcept { return this->m_path; }
			const auto& GetName(void) const noexcept { return this->m_name; }
			//
			const auto& GetShotRate(void) const noexcept { return this->m_ShotRate; }
			const auto& GetSoundSel(void) const noexcept { return this->m_SoundSel; }
			const auto& GetReloadType(void) const noexcept { return this->m_ReloadType; }
			//性能
			const auto& GetIsThrowWeapon(void) const noexcept { return this->m_IsThrowWeapon; }
			const auto& GetCanADS(void) const noexcept { return this->m_CanADS; }
			const auto& GetCanSwitch(void) const noexcept { return this->m_CanSwitch; }
			const auto& GetAnimSelectList(void) const noexcept { return this->m_AnimSelect; }
			const auto& GetIsRecoilPower(void) const noexcept { return this->m_IsRecoilPower; }
			const auto& GetIsRecoilReturn(void) const noexcept { return this->m_IsRecoilReturn; }
			const auto& GetIsShotType(void) const noexcept { return this->m_IsShotType; }
			const auto& GetRecoilPower(void) const noexcept { return this->m_RecoilPower; }
			const auto& GetRecoilReturn(void) const noexcept { return this->m_RecoilReturn; }
			const auto& GetShotType(void) const noexcept { return this->m_ShotType; }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_HumanAnimType; }
			const auto& IsIronSight(void) const noexcept { return this->m_IronSight; }
			const auto& GetInfo(void) const noexcept { return this->m_Info; }
			const auto& GetInfoEng(void) const noexcept { return this->m_InfoEng; }

			//銃声
			const auto& GetGunShootSound(void) const noexcept { return this->m_GunShootSound; }
			//スコープ
			const auto& GetReitcleGraph(void) const noexcept { return this->m_Reitcle; }
			const auto& GetSightZoomSize(void) const noexcept { return this->m_ZoomSize; }
			//マガジン
			const auto& GetAmmoAll(void) const noexcept { return this->m_CapacityMax; }
			const auto& GetAmmoSpecMagTop(void) const noexcept { return this->m_AmmoSpec[0]; }
			//性能周り
			const auto& GetShootRate_Diff(void) const noexcept { return this->m_ShootRate_Diff; }
			const auto& GetRecoil_Diff(void) const noexcept { return this->m_Recoil_Diff; }

			const PartsSlot* GetPartsSlot(GunSlot sel) const noexcept {
				for (const auto& s : this->m_PartsSlot) {
					if (s.m_GunSlot == sel) {
						return &s;
					}
				}
				return nullptr;
			}
			const auto& GetUniqueID(void) const noexcept { return this->m_UniqueID; }
		public://
			void			SetUniqueID(int value) noexcept { this->m_UniqueID = value; }
		public://
			void		Set(std::string path_) {
				for (auto& s : this->m_AnimSelect) {
					s = -1;
				}

				this->m_path = path_;

				FILEINFO FileInfo;
				if (FileRead_findFirst((this->m_path + "reticle_0.png").c_str(), &FileInfo) != (DWORD_PTR)-1) {
					this->m_Reitcle.Load(this->m_path + "reticle_0.png");
				}

				this->m_AmmoSpec.clear();
				this->m_Info.clear();
				this->m_InfoEng.clear();

				FileStreamDX FileStream((this->m_path + "data.txt").c_str());
				while (true) {
					if (FileStream.ComeEof()) { break; }
					auto ALL = FileStream.SeekLineAndGetStr();
					//コメントアウト
					if (ALL.find("//") != std::string::npos) {
						ALL = ALL.substr(0, ALL.find("//"));
					}
					//
					if (ALL == "") { continue; }
					auto LEFT = FileStreamDX::getleft(ALL);
					auto RIGHT = FileStreamDX::getright(ALL);
					//アイテムデータ読みとり
					SetSlot(LEFT, RIGHT);
				}
			}
		private:
			void				SetSlot(const std::string& LEFT, const std::string& RIGHT) noexcept;
		};

		class ModDataManager : public SingletonBase<ModDataManager> {
		private:
			friend class SingletonBase<ModDataManager>;
		private:
			std::list<std::shared_ptr<ModDataClass>>	m_Object;
			int											m_LastUniqueID{ 0 };
		private:
			ModDataManager(void) noexcept {}
			~ModDataManager(void) noexcept {}
		public:
			const std::shared_ptr<ModDataClass>* GetData(int uniqueID) noexcept {
				for (auto& o : this->m_Object) {
					if (o->GetUniqueID() == uniqueID) {
						return &o;
					}
				}
				return nullptr;
			}
		public:
			const std::shared_ptr<ModDataClass>* AddData(const std::string& filepath) noexcept;
		};
	};
};
