#pragma once
#include	"../../Header.hpp"
#include "../../MainScene/Object/ItemEnum.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ItemData {
		private:
			std::string		m_path;
			std::string		m_name;
			ItemType		m_ItemType{ ItemType::Track };					//ダメージ
			std::string		m_Info;
			GraphHandle		m_SlotPic;
			int				m_xsize{ 1 };
			int				m_ysize{ 1 };

			int				m_Capacity{ 1 };
		public://getter
			const auto&		GetPath(void) const noexcept { return this->m_path; }
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		GetItemType(void) const noexcept { return this->m_ItemType; }
			const auto&		GetInfo(void) const noexcept { return this->m_Info; }
			const auto&		GetSlotPic(void) const noexcept { return this->m_SlotPic; }
			const auto&		GetXsize(void) const noexcept { return this->m_xsize; }
			const auto&		GetYsize(void) const noexcept { return this->m_ysize; }
			const auto&		GetCapacity(void) const noexcept { return this->m_Capacity; }
		protected:
			virtual void	Set_Sub(const std::string&, const std::string&) noexcept {}
		public:
			void			Set(std::string path_) {
				this->m_path = path_;
				m_SlotPic = GraphHandle::Load(this->m_path + "pic.png");
				m_SlotPic.GetSize(&m_xsize, &m_ysize);
				m_xsize /= 64;
				m_ysize /= 64;

				int mdata = FileRead_open((this->m_path + "data.txt").c_str(), FALSE);
				while (true) {
					if (FileRead_eof(mdata) != 0) { break; }
					auto ALL = getparams::Getstr(mdata);
					//コメントアウト
					if (ALL.find("//") != std::string::npos) {
						ALL = ALL.substr(0, ALL.find("//"));
					}
					//
					if (ALL == "") { continue; }
					auto LEFT = getparams::getleft(ALL);
					auto RIGHT = getparams::getright(ALL);
					//アイテムデータ読みとり
					if (LEFT == "Name") {
						this->m_name = RIGHT;
					}
					if (LEFT == "ItemType") {
						for (int i = 0; i < (int)ItemType::Max; i++) {
							if (RIGHT == ItemtypeName[i]) {
								m_ItemType = (ItemType)i;
							}
						}
					}
					if (LEFT == "ItemInfo") {
						m_Info = RIGHT;
					}
					if (LEFT == "ItemCapacity") {
						m_Capacity = std::stoi(RIGHT);
					}
					Set_Sub(LEFT, RIGHT);
				}
				FileRead_close(mdata);
			}
		};
		class AmmoData : public ItemData {
		private:
			float			m_caliber{0.f};
			float			m_speed{100.f};				//弾速
			float			m_penetration{10.f};			//貫通
			HitPoint		m_damage{10};					//ダメージ
		public://getter
			const auto&		GetCaliber(void) const noexcept { return this->m_caliber; }
			const auto&		GetSpeed(void) const noexcept { return this->m_speed; }
			const auto&		GetPenetration(void) const noexcept { return this->m_penetration; }
			const auto&		GetDamage(void) const noexcept { return this->m_damage; }
		protected:
			void		Set_Sub(const std::string& LEFT, const std::string&RIGHT) noexcept override {
				if (LEFT == "ammo_cal(mm)") {
					this->m_caliber = std::stof(RIGHT)* 0.001f;	//口径
				}
				if (LEFT == "ammo_speed(m/s)") {
					this->m_speed = std::stof(RIGHT);				//弾速
				}
				if (LEFT == "ammo_pene(mm)") {
					this->m_penetration = std::stof(RIGHT);			//貫通
				}
				if (LEFT == "ammo_damage") {
					this->m_damage = (HitPoint)std::stoi(RIGHT);		//ダメージ
				}
			}
		public:
		};

		class ItemDataControl : public SingletonBase<ItemDataControl> {
		private:
			friend class SingletonBase<ItemDataControl>;
		private:
		private:
			std::vector<std::shared_ptr<ItemData>>		m_ItemData;						//アイテムデータ
		public:
			const auto&		GetData(void) const noexcept { return this->m_ItemData; }
		public:
			void	Load() noexcept {
				{
					std::vector<WIN32_FIND_DATA> data_t;
					std::string Path = "data/item/";
					GetFileNamesInDirectory("data/item/*", &data_t);
					for (auto& d : data_t) {
						if (d.cFileName[0] != '.') {
							m_ItemData.emplace_back(std::make_shared<ItemData>());
							m_ItemData.back()->Set(Path + d.cFileName + "/");
						}
					}
					data_t.clear();
				}
				{
					std::vector<WIN32_FIND_DATA> data_t;
					std::string Path = "data/ammo/";
					GetFileNamesInDirectory("data/ammo/*", &data_t);
					for (auto& d : data_t) {
						if (d.cFileName[0] != '.') {
							m_ItemData.emplace_back(std::make_shared<AmmoData>());
							m_ItemData.back()->Set(Path + d.cFileName + "/");
						}
					}
					data_t.clear();
				}
			}
			void	Dispose() noexcept {
				this->m_ItemData.clear();
			}
		};
	};
};
