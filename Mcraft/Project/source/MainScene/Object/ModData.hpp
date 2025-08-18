#pragma once
#include	"../../Header.hpp"

#include	"GunEnum.hpp"
#include	"AmmoData.hpp"

namespace FPS_n2 {
	namespace Guns {
		class GunPartsData {
			struct SlotInfo {
				GunSlot						SlotType{ GunSlot::Gun };
				std::vector<int>			CanAttachItemsUniqueID;
				std::vector<std::string>	ConflictsItemsUniqueID;
				bool						IsNeed{ false };
			};
		private:
			std::string						m_path;
			std::string						m_name;
			int								m_UniqueID{ 0 };
			std::vector<std::unique_ptr<SlotInfo>>	m_SlotInfo;

			bool							m_IsRecoilPower{ false };
			bool							m_IsRecoilReturn{ false };
			bool							m_IsShotType{ false };

			bool							m_IsRecoilScaleRate{ false };
			int								m_RecoilScaleRate{ 5 };

			int								m_RecoilPower{ 120 };
			float							m_RecoilReturn{ 0.9f };
			SHOTTYPE						m_ShotType{ SHOTTYPE::SEMI };
			bool							m_IronSight{ false };
			int								m_IsPlayableWeapon{ InvalidID };
			int								m_UnlockScore{ 0 };

			STOCKTYPE						m_StockType{ STOCKTYPE::none };

			EnumGunSound					m_GunShootSound{ EnumGunSound::ShotNormal };

			GraphHandle						m_Icon;

			GraphHandle						m_Reitcle;
			float							m_ZoomSize{ 1.0f };
			int								m_HumanAnimType{ InvalidID };

			int								m_ShootRate_Diff{ 0 };
			int								m_Recoil_Diff{ 0 };

			int								m_ShotRate{ 300 };
			int								m_SoundSelect{ 0 };
			RELOADTYPE						m_ReloadType{ RELOADTYPE::MAG };
			bool							m_IsThrowWeapon{ false };
			bool							m_CanADS{ true };
			bool							m_CanSwitch{ true };
			bool							m_isHoldOpen{ true };

			bool							m_IsJungle{ false };

			int								m_Weight{ 0 };

			bool							m_IsCustomize{ false };

			float							m_ReloadSpeedAdd{ 0.f };
			float							m_AimSpeedAdd{ 0.f };
			float							m_AutoAimSpeedAdd{ 0.f };

			std::array<int, static_cast<int>(Charas::GunAnimeID::Max)>	m_AnimSelect{ InvalidID };

			std::vector<int>	m_AmmoSpecID;
			std::vector<std::string>					m_Info;
			std::vector<std::string>					m_InfoEng;
			int											m_CapacityMax{ 0 };
		public:
			GunPartsData(void) noexcept {}
			~GunPartsData(void) noexcept {
				this->m_AmmoSpecID.clear();
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
			const auto& GetisHoldOpen(void) const noexcept { return this->m_isHoldOpen; }
			const auto& GetIsJungle(void) const noexcept { return this->m_IsJungle; }
			const auto& GetIsCustomize(void) const noexcept { return this->m_IsCustomize; }
			const auto& GetAnimSelectList(void) const noexcept { return this->m_AnimSelect; }
			const auto& GetIsRecoilPower(void) const noexcept { return this->m_IsRecoilPower; }
			const auto& GetIsRecoilReturn(void) const noexcept { return this->m_IsRecoilReturn; }

			const auto& GetIsRecoilScaleRate(void) const noexcept { return this->m_IsRecoilScaleRate; }
			const auto& GetRecoilScaleRate(void) const noexcept { return this->m_RecoilScaleRate; }

			const auto& GetIsShotType(void) const noexcept { return this->m_IsShotType; }
			const auto& GetRecoilPower(void) const noexcept { return this->m_RecoilPower; }
			const auto& GetRecoilReturn(void) const noexcept { return this->m_RecoilReturn; }
			const auto& GetShotType(void) const noexcept { return this->m_ShotType; }
			const auto& GetHumanAnimType(void) const noexcept { return this->m_HumanAnimType; }
			const auto& IsIronSight(void) const noexcept { return this->m_IronSight; }
			const auto& IsPlayableWeapon(void) const noexcept { return this->m_IsPlayableWeapon; }
			const auto& GetUnlockScore(void) const noexcept { return this->m_UnlockScore; }
			const auto& GetInfo(void) const noexcept {
				auto* OptionParts = OptionManager::Instance();
				switch ((LanguageType)OptionParts->GetParamInt(EnumSaveParam::Language)) {
				case LanguageType::Eng:
					return this->m_InfoEng;
				case LanguageType::Jpn:
					return this->m_Info;
				default:
					return this->m_Info;
				}
			}

			const auto& GetStockType(void) const noexcept { return this->m_StockType; }
			const auto& GetIconGraph(void) const noexcept { return this->m_Icon; }

			const auto& GetWeight_gram(void) const noexcept { return this->m_Weight; }

			const auto& GetReloadSpeedAdd(void) const noexcept { return this->m_ReloadSpeedAdd; }
			const auto& GetAimSpeedAdd(void) const noexcept { return this->m_AimSpeedAdd; }
			const auto& GetAutoAimSpeedAdd(void) const noexcept { return this->m_AutoAimSpeedAdd; }
			//銃声
			const auto& GetGunShootSound(void) const noexcept { return this->m_GunShootSound; }
			//スコープ
			const auto& GetReitcleGraph(void) const noexcept { return this->m_Reitcle; }
			const auto& GetSightZoomSize(void) const noexcept { return this->m_ZoomSize; }
			//マガジン
			const auto& GetAmmoAll(void) const noexcept { return this->m_CapacityMax; }
			const auto& GetAmmoSpecID(int index) const noexcept { return this->m_AmmoSpecID[index]; }
			//性能周り
			const auto& GetShootRate_Diff(void) const noexcept { return this->m_ShootRate_Diff; }
			const auto& GetRecoil_Diff(void) const noexcept { return this->m_Recoil_Diff; }

			const SlotInfo* GetSlotInfo(GunSlot select) const noexcept {
				for (const auto& slot : this->m_SlotInfo) {
					if (slot->SlotType == select) {
						return slot.get();
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
				if (FileRead_findFirst((this->m_path + "Gauge.bmp").c_str(), &FileInfo) != (DWORD_PTR)InvalidID) {
					this->m_Icon.Load(this->m_path + "Gauge.bmp");
				}
				if (FileRead_findFirst((this->m_path + "reticle_0.png").c_str(), &FileInfo) != (DWORD_PTR)InvalidID) {
					this->m_Reitcle.Load(this->m_path + "reticle_0.png");
				}

				this->m_AmmoSpecID.clear();
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
		public:
		private:
			std::list<std::unique_ptr<GunPartsData>>	m_Data;
			int											m_LastUniqueID{ 0 };
			std::vector<std::string>					m_GunList;
			std::vector<std::string>					m_GunPathList;
			std::vector<std::string>					m_ModPathList;
		private:
			GunPartsDataManager(void) noexcept {
				//銃オブジェ文字列リストの生成
				{
					this->m_GunList.clear();
					this->m_GunPathList.clear();
					std::string Path = "data/gun/";
					std::vector<WIN32_FIND_DATA> pData;
					GetFileNamesInDirectory((Path + "*").c_str(), &pData);
					for (auto& data : pData) {
						this->m_GunList.emplace_back(data.cFileName);
						this->m_GunPathList.emplace_back(Path + data.cFileName + "/");
					}
				}
				{
					this->m_ModPathList.clear();
					std::string Path = "data/Mods/";
					std::vector<WIN32_FIND_DATA> pData;
					GetFileNamesInDirectory((Path + "*").c_str(), &pData);
					for (auto& data : pData) {
						std::string Path2 = Path + data.cFileName + "/";
						std::vector<WIN32_FIND_DATA> pData2;
						GetFileNamesInDirectory((Path2 + "*").c_str(), &pData2);
						for (auto& data2 : pData2) {
							this->m_ModPathList.emplace_back(Path2 + data2.cFileName + "/");
						}
					}
				}
			}
			virtual ~GunPartsDataManager(void) noexcept {
				for (auto& obj : this->m_Data) {
					obj.reset();
				}
				this->m_Data.clear();
			}
		public:
			const std::unique_ptr<GunPartsData>* GetData(int uniqueID) noexcept {
				auto Find = std::find_if(this->m_Data.begin(), this->m_Data.end(), [&](const  std::unique_ptr<GunPartsData>& tgt) {return tgt->GetUniqueID() == uniqueID; });
				if (Find != this->m_Data.end()) {
					return &*Find;
				}
				return nullptr;
			}
			const auto& GetGunNameList(void) const noexcept { return this->m_GunList; }
			const auto& GetGunPathList(void) const noexcept { return this->m_GunPathList; }
			const auto& GetModPathList(void) const noexcept { return this->m_ModPathList; }
			const int Add(const std::string& filepath) noexcept;
		public:
			void Init() noexcept {
				//銃オブジェ文字列リストの生成
				for (auto& Path : this->m_ModPathList) {
					this->Add(Path);
				}
				for (auto& Path : this->m_GunPathList) {
					this->Add(Path);
				}
			}
		};
	}
}
