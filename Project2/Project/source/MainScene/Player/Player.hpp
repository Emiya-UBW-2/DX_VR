#pragma once
#include	"../../Header.hpp"
#include "../../MainScene/Object/ItemData.hpp"
#include "../../MainScene/NetWorks.hpp"

#include "../../MainScene/Object/Vehicle.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		class CellItem {
			int							m_Count{1};
			std::shared_ptr<ItemData>	Data{nullptr};
			bool						Is90{false};
			int							SlotID{0};
		public:
			const auto& GetItemData(void) const noexcept { return this->Data; }
			const auto&	GetXsize(void) const noexcept { return this->Is90 ? this->Data->GetYsize() : this->Data->GetXsize(); }
			const auto&	GetYsize(void) const noexcept { return this->Is90 ? this->Data->GetXsize() : this->Data->GetYsize(); }
			const auto&	GetCount(void) const noexcept { return this->m_Count; }
			const auto& GetIs90(void) const noexcept { return this->Is90; }
			const auto& GetSlotID(void) const noexcept { return this->SlotID; }
			void Rotate() { this->Is90 ^= 1; }
		public:
			void Set(const std::shared_ptr<ItemData>& data, int cap, int Slot) {
				this->Data = data;
				if (cap < 0) {
					this->m_Count = this->Data->GetCapacity();
				}
				else {
					this->m_Count = cap;
				}
				SlotID = Slot;
			}
			bool Sub(HitPoint* value) {
				auto prev = this->m_Count;
				this->m_Count = std::max(this->m_Count - *value, 0);
				*value -= (HitPoint)(prev - this->m_Count);
				return (prev != this->m_Count && this->m_Count == 0);//0になったとき
			}

			void Dispose() {
				this->Data.reset();
			}
			void Draw(int xp, int yp) {
				auto* Fonts = FontPool::Instance();

				this->Data->GetSlotPic().DrawRotaGraph(xp + GetXsize()*y_r(64) / 2, yp + GetYsize()*y_r(64) / 2, (float)y_r(64) / 64.f, deg2rad(this->Is90 ? 90.f : 0.f), false);
				Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(12), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM,
																	   xp + GetXsize()*y_r(64), yp + GetYsize()*y_r(64),
																	   ((100 * this->m_Count / this->Data->GetCapacity()) > 34) ? White : Red, Black, "%d/%d", this->m_Count, this->Data->GetCapacity());
			}
		};

		class PlayerControl {
		private:
			std::shared_ptr<VehicleClass>										m_Vehicle{ nullptr };
			std::vector<std::vector<std::vector<std::shared_ptr<CellItem>>>>	m_Inventorys;
			float							m_Score{ 0.f };							//スコア
		public:
			PlayerControl(void) noexcept {
				this->m_Score = 0.f;
			}
			~PlayerControl(void) noexcept {
				this->Dispose();
			}
		public:
			void		SetVehicle(const std::shared_ptr<VehicleClass>& pVehicle) { m_Vehicle = pVehicle; }
			void		AddScore(float value) { this->m_Score += value; }
			void		SubScore(float value) { this->m_Score -= value; }
			void		SetScore(float value) { this->m_Score = value; }
			auto&		GetVehicle(void) noexcept { return m_Vehicle; }
			const auto&	GetScore(void) const noexcept { return this->m_Score; }
			const auto	IsRide(void) const noexcept { return (bool)m_Vehicle; }
		public:
			auto&			GetInventorys(void) noexcept { return this->m_Inventorys; }
			const auto		GetInventoryXSize(int ID) const noexcept { return (int)this->m_Inventorys[ID].size(); }
			const auto		GetInventoryYSize(int ID) const noexcept { return (int)this->m_Inventorys[ID][0].size(); }
		public:
			void SetInventory(int ID, int x, int y) noexcept {
				this->m_Inventorys[ID].resize(x);
				for (auto& xp : this->m_Inventorys[ID]) { xp.resize(y); }
			}
			bool CanPutInventory(int ID, int xp, int yp, int xsize, int ysize, const std::shared_ptr<ItemData>* Drag, const std::shared_ptr<CellItem>* DragIn = nullptr) noexcept {
				//自機の履帯以外は乗らない
				if (Drag) {
					if (ID == 2 || ID == 3) {
						if ((*Drag) != m_Vehicle->GetTrackPtr()) {
							return false;
						}
					}
				}
				//
				if (DragIn && *DragIn == this->m_Inventorys[ID][xp][yp]) {}//該当部分がInで埋まっている
				else if (!this->m_Inventorys[ID][xp][yp].get()) {}//該当部分が空
				else { return false; }//該当部分が何かで埋まってる
				//該当部の1マスが空だとして、その上にアイテムがないか
				for (int x = 0; x <= xp; x++) {
					for (int y = 0; y <= yp; y++) {
						auto& yo = this->m_Inventorys.at(ID).at(x).at(y);
						if (yo.get() && (&yo != DragIn) && !(xp == x && yp == y)) {
							if (yo->GetXsize() > (xp - x) && yo->GetYsize() > (yp - y)) {
								return false;
							}
						}
					}
				}
				//指示サイズのものが置けるかチェック
				if (xsize > 1 || ysize > 1) {
					if (xp + xsize > GetInventoryXSize(ID) || yp + ysize > GetInventoryYSize(ID)) {
						return false;//マス越え
					}
					for (int x = xp; x < xp + xsize; x++) {
						for (int y = yp; y < yp + ysize; y++) {
							if (!CanPutInventory(ID, x, y, 1, 1, Drag, DragIn)) {
								return false;
							}
						}
					}
				}
				return true;
			}
			void PutInventory(int ID, int x, int y, const std::shared_ptr<ItemData>& data, int cap, bool Is90) noexcept {
				this->m_Inventorys.at(ID).at(x).at(y) = std::make_shared<CellItem>();
				this->m_Inventorys.at(ID).at(x).at(y)->Set(data, cap, ID);
				if (Is90) {
					this->m_Inventorys.at(ID).at(x).at(y)->Rotate();
				}
			}
			void DeleteInventory(int ID, int x, int y) noexcept {
				this->m_Inventorys.at(ID).at(x).at(y).reset();
			}
			void DeleteInventory(const std::shared_ptr<CellItem>& tgt) noexcept {
				for (auto& I : this->m_Inventorys) {
					for (auto& xo : I) {
						for (auto& yo : xo) {
							if (yo == tgt) {
								yo.reset();
								return;
							}
						}
					}
				}
			}
			const std::shared_ptr<CellItem>* GetInventory(int ID, int xp, int yp) const noexcept {
				if (this->m_Inventorys[ID][xp][yp].get() == nullptr) {
					for (int x = 0; x <= xp; x++) {
						for (int y = 0; y <= yp; y++) {
							auto& yo = this->m_Inventorys[ID][x][y];
							if (yo.get() && !(xp == x && yp == y)) {
								if (yo->GetXsize() > (xp - x) && yo->GetYsize() > (yp - y)) {
									return &yo;
								}
							}
						}
					}
					return nullptr;
				}
				else {
					return &this->m_Inventorys[ID][xp][yp];
				}
			}
			const std::shared_ptr<CellItem>* GetInventory(int ID, std::function<bool(const std::shared_ptr<CellItem>&)> Check) const noexcept {
				for (auto& xo : this->m_Inventorys[ID]) {
					for (auto& yo : xo) {
						if (yo.get()) {
							if (Check(yo)) {
								return &yo;
							}
						}
					}
				}
				return nullptr;
			}
			void FillInventory(int ID, const std::shared_ptr<ItemData>& data, int xStart, int yStart, int xEnd, int yEnd,int MaxCount=-1) noexcept {
				int xp = xStart;
				int yp = yStart;
				int cnt = 0;
				while (true) {
					if (MaxCount >= 0) {
						if (cnt >= MaxCount) { break; }
					}
					PutInventory(ID, xp, yp, data, -1, false);
					xp += data->GetXsize();
					if (xp >= std::min(xEnd, GetInventoryXSize(ID))) {
						xp = xStart;
						yp += data->GetYsize();
						if (yp >= std::min(yEnd, GetInventoryYSize(ID))) {
							break;
						}
					}
					cnt++;
				}
			};
		public:
			void Init(void) noexcept {
				this->m_Vehicle = nullptr;
				this->m_Inventorys.resize(5);
			}

			void Dispose(void) noexcept {
				this->m_Vehicle = nullptr;
			}
		};

		class PlayerManager :public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		private:
			std::vector<PlayerControl> m_Player;
		public:
			auto&		GetPlayer(int ID) { return m_Player[ID]; }
		public:
			void Init(int playerNum) {
				this->m_Player.resize(playerNum);
				for (auto& p : this->m_Player) {
					p.Init();
				}
			}
			void Dispose(void) noexcept {
				for (auto& p : this->m_Player) {
					p.Dispose();
				}
				this->m_Player.clear();
			}
		};

	};
};
