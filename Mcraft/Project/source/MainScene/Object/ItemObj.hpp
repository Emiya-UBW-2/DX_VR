#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Objects {

		enum class ItemType {
			Item,
			Armor,
			Helmet,
			Max
		};
		static const char* ItemTypeStr[static_cast<int>(ItemType::Max)] = {
			"Item",
			"Armor",
			"Helmet",
		};

		class ItemObjData {
			std::string		m_path;
			std::string		m_name;
			ItemType		m_ItemType;
			GraphHandle		m_Icon;
			bool			m_EnableSpawnBySoldier{ false };
			int				m_Weight{};
		public://getter
			const auto& GetPath(void) const noexcept { return this->m_path; }
			const auto& GetName(void) const noexcept { return this->m_name; }
			const auto& GetItemType(void) const noexcept { return this->m_ItemType; }
			const auto& GetIconGraph(void) const noexcept { return this->m_Icon; }
			const auto& EnableSpawnBySoldier(void) const noexcept { return this->m_EnableSpawnBySoldier; }
			const auto& GetWeight_gram(void) const noexcept { return this->m_Weight; }
		public:
			ItemObjData(std::string path_) noexcept { Set(path_); }
		private:
			void			Set(std::string path_) {
				this->m_path = path_;

				FILEINFO FileInfo;
				if (FileRead_findFirst((this->m_path + "icon.bmp").c_str(), &FileInfo) != (DWORD_PTR)InvalidID) {
					this->m_Icon.Load(this->m_path + "icon.bmp");
				}

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
					{
						if (LEFT == "Name") {
							this->m_name = RIGHT;
						}
						else if (LEFT == "Type") {
							for (int loop = 0; loop < static_cast<int>(ItemType::Max); ++loop) {
								if (RIGHT == ItemTypeStr[loop]) {
									m_ItemType = static_cast<ItemType>(loop);
									break;
								}
							}
						}
						else if (LEFT == "EnableSpawnBySoldier") {
							this->m_EnableSpawnBySoldier = (RIGHT == "TRUE");
						}
						else if (LEFT == "Weight") {
							m_Weight = std::stoi(RIGHT);
						}
					}
				}
			}
		};

		class ItemObjDataManager : public SingletonBase<ItemObjDataManager> {
		private:
			friend class SingletonBase<ItemObjDataManager>;
		private:
			std::vector<std::unique_ptr<ItemObjData>>	m_Data;
		private:
			ItemObjDataManager(void) noexcept {}
			virtual ~ItemObjDataManager(void) noexcept {
				for (auto& obj : this->m_Data) {
					obj.reset();
				}
				this->m_Data.clear();
			}
		public:
			const auto& GetList(void) const noexcept { return this->m_Data; }
			const auto& Get(int index) const noexcept { return this->m_Data[index]; }
			const int Add(const std::string& filepath) noexcept {
				auto Find = std::find_if(this->m_Data.begin(), this->m_Data.end(), [&](const std::unique_ptr<ItemObjData>& tgt) {return tgt->GetPath() == filepath; });
				if (Find != this->m_Data.end()) {
					int index = static_cast<int>(Find - this->m_Data.begin());
					return index;
				}
				this->m_Data.emplace_back(std::make_unique<ItemObjData>(filepath));
				return static_cast<int>(this->m_Data.size() - 1);
			}
		};



		class ItemObj : public BaseObject {
			int			m_ItemObjDataID{};
			float		m_yAdd{ 0.0f };
			Pendulum2D	m_Zrotate;
			float		m_Yrad{};
			Matrix3x3DX	m_Rot;
			Vector3DX	m_Pos;
			Vector3DX	m_Repos;
			bool		m_IsLR{};
			float		m_Timer{};
		public:
			ItemObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::ItemObj); }
			virtual ~ItemObj(void) noexcept {}
		public:
			void				SetUniqueID(int ID) noexcept { this->m_ItemObjDataID = ID; }
			const auto& GetUniqueID(void) const noexcept { return this->m_ItemObjDataID; }

			const auto			CanPick(void) const noexcept { return this->m_Timer > 3.f; }
			//接地
			void				Put(const Vector3DX& pos, const Vector3DX& vec) noexcept {
				SetActive(true);
				m_Repos = pos;
				m_Pos = pos;
				SetMove().SetPos(pos);
				SetMove().SetVec(vec);
				SetMove().Update(0.0f, 0.0f);
				m_Zrotate.Init(0.08f * Scale3DRate, 3.0f, deg2rad(50));
				this->m_Yrad = deg2rad(GetRandf(360.f));
				this->m_Timer = 0.f;
			}
		public:
			void				Init_Sub(void) noexcept override {
				SetActive(true);
				SetMinAABB(Vector3DX::vget(-1.f, 0.f, -1.f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(1.f, 1.f, 1.f) * Scale3DRate);
			}
			void				FirstUpdate(void) noexcept override;
			void				DrawShadow(void) noexcept override {
				if (!IsActive()) { return; }
				if ((GetMove().GetPos() - Camera3D::Instance()->GetMainCamera().GetCamPos()).sqrMagnitude() > (10.0f * Scale3DRate * 10.0f * Scale3DRate)) { return; }
				GetObj().DrawModel();
			}
			void			Draw(bool isDrawSemiTrans, int Range) noexcept override {
				if (!IsActive()) { return; }
				if (!IsDraw(Range)) { return; }
				if (isDrawSemiTrans) { return; }
				//if ((GetMove().GetPos() - Camera3D::Instance()->GetMainCamera().GetCamPos()).sqrMagnitude() > (10.0f * Scale3DRate * 10.0f * Scale3DRate)) { return; }
				if (!CanPick() && (static_cast<int>(this->m_Timer * 60.f) % 10 < 5)) { return; }
				GetObj().DrawModel();
			}
			void			Dispose_Sub(void) noexcept override {}
		};


		class ItemObjPool : public SingletonBase<ItemObjPool> {
		private:
			friend class SingletonBase<ItemObjPool>;
		private:
			std::vector<std::shared_ptr<Objects::ItemObj>> m_ItemObjList;
		private:
			ItemObjPool(void) noexcept {
				this->m_ItemObjList.clear();
			}
			virtual ~ItemObjPool(void) noexcept {
				for (auto& ammo : this->m_ItemObjList) {
					ObjectManager::Instance()->DelObj(ammo);
					ammo.reset();
				}
				this->m_ItemObjList.clear();
			}
		public:
			auto			GetList(void) noexcept { return this->m_ItemObjList; }

			void Put(int pItemObjDataID, const Vector3DX& pos, const Vector3DX& vec) noexcept {
				for (auto& ammo : this->m_ItemObjList) {
					if (!ammo->IsActive() && (ammo->GetUniqueID() == pItemObjDataID)) {
						ammo->Put(pos, vec);
						return;
					}
				}
				auto& ItemData = Objects::ItemObjDataManager::Instance()->Get(pItemObjDataID);

				this->m_ItemObjList.emplace_back(std::make_shared<Objects::ItemObj>());
				ObjectManager::Instance()->InitObject(this->m_ItemObjList.back(), ItemData->GetPath());
				this->m_ItemObjList.back()->SetUniqueID(pItemObjDataID);
				this->m_ItemObjList.back()->Put(pos, vec);
			}
		};


		class ItemContainerObj : public BaseObject {
		public:
			ItemContainerObj(void) noexcept { this->m_objType = static_cast<int>(ObjType::ItemContainerObj); }
			virtual ~ItemContainerObj(void) noexcept {}
		public:
			//接地
			void				Put(const Vector3DX& pos, const Matrix3x3DX& rot) noexcept {
				SetActive(true);
				SetMove().SetPos(pos);
				SetMove().SetMat(rot);
				SetMove().Update(0.0f, 0.0f);
				UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());
				GetCol().RefreshCollInfo();
			}
		public:
			void				Init_Sub(void) noexcept override {
				SetActive(true);
				SetMinAABB(Vector3DX::vget(-5.f, 0.f, -5.f) * Scale3DRate);
				SetMaxAABB(Vector3DX::vget(5.f, 5.f, 5.f) * Scale3DRate);
			}
			void				FirstUpdate(void) noexcept override {}
			void			Dispose_Sub(void) noexcept override {}
		};
	}
}
