#pragma once
#include	"../../Header.hpp"

#include	"GunEnum.hpp"
#include	"AmmoData.hpp"

namespace FPS_n2 {
	namespace Guns {
		class GunPartsData {
			struct SlotInfo {
				GunSlot						m_SlotType{ GunSlot::Gun };
				std::vector<int>			m_CanAttachItemsUniqueID;
				std::vector<std::string>	m_ConflictsItemsUniqueID;
				bool						m_IsNeed{ false };
			};
		private:
			std::string						m_path;
			std::string						m_name;
			int								m_UniqueID{ 0 };
			std::vector<SlotInfo>			m_SlotInfo;

			bool							m_IsRecoilPower{ false };
			bool							m_IsRecoilReturn{ false };
			bool							m_IsShotType{ false };

			int								m_RecoilPower{ 120 };
			float							m_RecoilReturn{ 0.9f };
			SHOTTYPE						m_ShotType{ SHOTTYPE::SEMI };
			bool							m_IronSight{ false };

			EnumGunSound					m_GunShootSound{ EnumGunSound::ShotNormal };

			GraphHandle						m_Reitcle;
			float							m_ZoomSize{ 1.f };
			int								m_HumanAnimType{ InvalidID };

			int								m_ShootRate_Diff{ 0 };
			int								m_Recoil_Diff{ 0 };

			int								m_ShotRate{ 300 };
			int								m_SoundSelect{ 0 };
			RELOADTYPE						m_ReloadType{ RELOADTYPE::MAG };
			bool							m_IsThrowWeapon{ false };
			bool							m_CanADS{ true };
			bool							m_CanSwitch{ true };


			std::array<int, static_cast<int>(Charas::GunAnimeID::Max)>	m_AnimSelect{ InvalidID };

			std::vector<std::shared_ptr<Objects::AmmoData>>	m_AmmoSpec;
			std::vector<std::string>					m_Info;
			std::vector<std::string>					m_InfoEng;
			int											m_CapacityMax{ 0 };
		public:
			GunPartsData(void) noexcept {}
			virtual ~GunPartsData(void) noexcept {
				for (auto& ammo : this->m_AmmoSpec) {
					ammo.reset();
				}
				this->m_AmmoSpec.clear();
			}
		public://ゲッター
			const auto& GetPath(void) const noexcept { return this->m_path; }
			const auto& GetName(void) const noexcept { return this->m_name; }
			//
			const auto& GetShotRate(void) const noexcept { return this->m_ShotRate; }
			const auto& GetSoundSelect(void) const noexcept { return this->m_SoundSelect; }
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

			const SlotInfo* GetSlotInfo(GunSlot select) const noexcept {
				for (const auto& slot : this->m_SlotInfo) {
					if (slot.m_SlotType == select) {
						return &slot;
					}
				}
				return nullptr;
			}
			const auto& GetUniqueID(void) const noexcept { return this->m_UniqueID; }
		public://
			void			SetUniqueID(int ID) noexcept { this->m_UniqueID = ID; }
		public://
			void		Set(std::string path_) {
				for (auto& anim : this->m_AnimSelect) {
					anim = InvalidID;
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

		class GunPartsDataManager : public SingletonBase<GunPartsDataManager> {
		private:
			friend class SingletonBase<GunPartsDataManager>;
		private:
			std::list<std::shared_ptr<GunPartsData>>	m_Object;
			int											m_LastUniqueID{ 0 };
		private:
			GunPartsDataManager(void) noexcept {}
			virtual ~GunPartsDataManager(void) noexcept {
				for (auto& obj : this->m_Object) {
					obj.reset();
				}
				this->m_Object.clear();
			}
		public:
			const std::shared_ptr<GunPartsData>* GetData(int uniqueID) noexcept {
				for (auto& obj : this->m_Object) {
					if (obj->GetUniqueID() == uniqueID) {
						return &obj;
					}
				}
				return nullptr;
			}
		public:
			const std::shared_ptr<GunPartsData>* AddData(const std::string& filepath) noexcept;
		};
	};
};
