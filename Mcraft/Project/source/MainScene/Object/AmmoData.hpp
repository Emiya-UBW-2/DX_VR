﻿#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Objects {
		class AmmoData {
			std::string		m_path;
			std::string		m_name;
			float			m_caliber{ 0.0f };
			float			m_speed{ 100.0f };				//弾速
			float			m_penetration{ 10.0f };			//貫通
			HitPoint		m_damage{ 10 };					//ダメージ
			int				m_Pellet{ 1 };
			float			m_Accuracy{ 0.0f };
			int				m_FallSound{ 0 };
			int				m_EffectID{ -1 };
		public://getter
			const auto& GetPath(void) const noexcept { return this->m_path; }
			const auto& GetName(void) const noexcept { return this->m_name; }
			const auto& GetCaliber(void) const noexcept { return this->m_caliber; }
			const auto& GetSpeed(void) const noexcept { return this->m_speed; }
			const auto& GetPenetration(void) const noexcept { return this->m_penetration; }
			const auto& GetDamage(void) const noexcept { return this->m_damage; }
			const auto& GetPellet(void) const noexcept { return this->m_Pellet; }
			const auto& GetAccuracy(void) const noexcept { return this->m_Accuracy; }
			const auto& GetFallSound(void) const noexcept { return this->m_FallSound; }
			const auto& GetEffectID(void) const noexcept { return this->m_EffectID; }
		public:
			AmmoData(std::string path_) noexcept { Set(path_); }
		private:
			void			Set(std::string path_) {
				this->m_path = path_;

				//Load_Sub(this->m_path);

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
						else if (LEFT == "ammo_cal(mm)") {
							this->m_caliber = std::stof(RIGHT) * 0.001f;		//口径
						}
						else if (LEFT == "ammo_speed(m/s)") {
							this->m_speed = std::stof(RIGHT);				//弾速
						}
						else if (LEFT == "ammo_pene(mm)") {
							this->m_penetration = std::stof(RIGHT);			//貫通
						}
						else if (LEFT == "ammo_damage") {
							this->m_damage = (HitPoint)std::stoi(RIGHT);		//ダメージ
						}
						else if (LEFT == "Pellets") {
							this->m_Pellet = std::stoi(RIGHT);				//ペレットの数
						}
						else if (LEFT == "Accuracy") {
							this->m_Accuracy = std::stof(RIGHT);				//ペレットの数
						}
						else if (LEFT == "FallSound") {
							this->m_FallSound = std::stoi(RIGHT);				//ペレットの数
						}
						else if (LEFT == "UseEffect") {
							this->m_EffectID = std::stoi(RIGHT);
						}
					}
				}
			}
		};

		class AmmoDataManager : public SingletonBase<AmmoDataManager> {
		private:
			friend class SingletonBase<AmmoDataManager>;
		private:
			std::vector<std::unique_ptr<AmmoData>>	m_Data;
		public:
			std::vector<std::string>					m_AmmoList;
		private:
			AmmoDataManager(void) noexcept {
				this->m_AmmoList.clear();
				std::vector<WIN32_FIND_DATA> pData;
				GetFileNamesInDirectory("data/ammo/*", &pData);
				for (auto& data : pData) {
					this->m_AmmoList.emplace_back(data.cFileName);
				}
			}
			virtual ~AmmoDataManager(void) noexcept {
				for (auto& obj : this->m_Data) {
					obj.reset();
				}
				this->m_Data.clear();
			}
		public:
			const auto& Get(int index) const noexcept { return this->m_Data[index]; }
			const int Add(const std::string& filepath) noexcept {
				auto Find = std::find_if(this->m_Data.begin(), this->m_Data.end(), [&](const std::unique_ptr<AmmoData>& tgt) {return tgt->GetPath() == filepath; });
				if (Find != this->m_Data.end()) {
					int index = static_cast<int>(Find - this->m_Data.begin());
					return index;
				}
				this->m_Data.emplace_back(std::make_unique<AmmoData>(filepath));
				return static_cast<int>(this->m_Data.size() - 1);
			}
		public:
			void Init() noexcept {
				for (auto& name : this->m_AmmoList) {
					std::string ChildPath = "data/ammo/";
					ChildPath += name;
					ChildPath += "/";
					this->Add(ChildPath);
				}
			}
		};
	}
}
