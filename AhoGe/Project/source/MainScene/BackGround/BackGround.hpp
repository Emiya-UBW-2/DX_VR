#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClassBase : public SingletonBase<BackGroundClassBase> {
		private:
			friend class SingletonBase<BackGroundClassBase>;
		private:

			static const int tile_pic_size = 32;	// 画像のタイルサイズ

		private:

			struct Blick {
				Vector3DX					m_Pos;
				int							palletNum{0};
				bool						IsWall{false};
				float						ZRad{0};
				std::array<int, 4>			AddpalletNum{};
				std::array<float, 4>		AddZRad{0};
				std::array<int, 4>			LinkIndex{-1,-1,-1,-1};
				std::array < Vector3DX, 4>	BoxSide;
			public:
				Blick(int x, int y) noexcept {
					m_Pos.x = Get2DSize((float)x);
					m_Pos.y = Get2DSize((float)y);
					for (int i = 0; i < 4; i++) {
						LinkIndex.at(i) = -1;
					}
					float HalfLen = Get2DSize(0.5f);
					BoxSide.at(0).Set(m_Pos.x - HalfLen, m_Pos.y - HalfLen, 0.f);
					BoxSide.at(1).Set(m_Pos.x + HalfLen, m_Pos.y - HalfLen, 0.f);
					BoxSide.at(2).Set(m_Pos.x + HalfLen, m_Pos.y + HalfLen, 0.f);
					BoxSide.at(3).Set(m_Pos.x - HalfLen, m_Pos.y + HalfLen, 0.f);
				}
			public:
				bool CheckPointHit(const Vector3DX& Pos) {
					float HalfLen = Get2DSize(0.5f);
					return HitPointToRectangle((int)Pos.x, (int)Pos.y, (int)(this->m_Pos.x - HalfLen), (int)(this->m_Pos.y - HalfLen), (int)(this->m_Pos.x + HalfLen), (int)(this->m_Pos.y + HalfLen));
				}
				bool CheckRectangleHit(const Vector3DX& Min, const Vector3DX& Max) {
					float HalfLen = Get2DSize(0.5f);
					return ((int)Min.x < this->m_Pos.x + HalfLen && this->m_Pos.x - HalfLen < (int)Max.x) && ((int)Min.y < this->m_Pos.y + HalfLen && this->m_Pos.y - HalfLen < (int)Max.y);
				}

			};

			// プレイヤーの周囲にあるステージ壁を取得する( 検出する範囲は移動距離も考慮する )
			struct CheckLines {
				std::array<Vector3DX,3> Position{ };
				Vector3DX Normal{};
				bool canuse{true};
			public:
				void CalcNormal() {
					Vector3DX StartP = this->Position.at(0);
					Vector3DX EndP = this->Position.at(1);
					Vector3DX In = this->Position.at(2);
					Vector3DX VecP = EndP - StartP;
					this->Normal = Vector3DX::Cross(VecP, Vector3DX::Cross(VecP, In - StartP)).normalized();
				}
			};
			// 
			// 視点から影になっている部分の四角形
			struct ShadowBoxes {
				std::array<Vector3DX,4> Position{ };
			};
		private:
			std::vector<CheckLines> WallList;// 壁と判断された構造体のアドレスを保存しておく
			std::vector<ShadowBoxes> m_ShadowBoxes;
			std::vector<std::vector<std::shared_ptr<Blick>>> m_Blick;
			std::vector<GraphHandle>	m_MapChip;
			std::vector<GraphHandle>	m_WallChip;

			float m_AmbientShadowLength{36.f};
			float m_AmbientLightRad = deg2rad(45);
			Vector3DX m_PointLightPos;

			float ShadowPer = 0.5f;
			GraphHandle m_PointShadowHandle;
			GraphHandle m_AmbientShadowHandle;
		public:// 
			const auto&		GetShadowGraph(void) const noexcept { return this->m_PointShadowHandle; }
			const auto		GetXSize(void) const noexcept { return this->m_Blick.size(); }
			const auto		GetYSize(void) const noexcept { return this->m_Blick.back().size(); }
			const auto		GetXYToNum(int x, int y) const noexcept { return x * (int)GetXSize() + y; }
			const auto		GetNumToXY(int num) const noexcept { return std::make_pair<int,int>(num / (int)GetXSize(), num % (int)GetXSize()); }

			const auto&		GetFloorData(int x, int y) const noexcept { return this->m_Blick.at(x).at(y); }
			const auto&		GetFloorData(int num) const noexcept { auto XY = GetNumToXY(num); return GetFloorData(XY.first, XY.second); }
			const auto		GetNearestFloors(const Vector3DX& Pos) const noexcept {
				for (auto& b : m_Blick) {
					for (auto& B : b) {
						if (B->IsWall) { continue; }
						if (!B->CheckPointHit(Pos)) { continue; }
						int x = (int)(&b - &m_Blick.front());
						int y = (int)(&B - &b.front());
						return GetXYToNum(x, y);
					}
				}
				return -1;
			}
			const auto		GetNearFloorsList(const Vector3DX& Pos) const noexcept {
				std::vector<int> SelList;
				float ViewLimit = Get2DSize(5.f);
				for (auto& b : m_Blick) {
					for (auto& B : b) {
						if (B->IsWall) { continue; }
						if ((B->m_Pos - Pos).sqrMagnitude() < ViewLimit* ViewLimit) {
							int x = (int)(&b - &m_Blick.front());
							int y = (int)(&B - &b.front());
							SelList.emplace_back(GetXYToNum(x, y));
						}
					}
				}
				return SelList;
			}
			bool CheckNextFloor(int Start, int Target) const {
				// 地点が存在しなかったら移動できないので false を返す
				if (Start == -1 || Target == -1) {
					return false;
				}
				for (int i = 0; i < 4; i++) {
					// もし辺と接している地点が目標座標上に存在する地点だったら 開始座標から目標座標上まで途切れなく地点が存在する
					if (GetFloorData(Start)->LinkIndex.at(i) == Target) {
						return true;
					}
				}
				return false;
			}
			// 
			const float		CheckHideShadow(const Vector3DX& PosA, const Vector3DX& PosB, float Radius) noexcept;
			const bool		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) noexcept;
			void DrawAmbientShadow(int x, int y) const noexcept;
			void DrawPointShadow(int x, int y) noexcept;
		public:// 
			void		SetAmbientLight(float ShadowLen, float Rad) noexcept {
				m_AmbientShadowLength = ShadowLen;
				m_AmbientLightRad = Rad;
			}
			void		SetPointLight(const Vector3DX& Pos) noexcept {
				m_PointLightPos = Convert2DtoDisp(Pos);
			}
		public:// 
			// 
			void			Init(const char* MapPath) noexcept;
			// 
			void			Execute(void) noexcept;
			// 
			void			SetupShadow(void) noexcept;
			// 
			void			Draw(void) noexcept;
			void			DrawFront(void) noexcept;
			// 
			void			Dispose(void) noexcept;
		};
	};
};
