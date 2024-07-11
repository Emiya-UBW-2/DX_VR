#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class EventType {
			Entry,
			CutScene,
			Max,
		};
		static const char* g_EventStr[(int)EventType::Max] = {
			"Entry",
			"CutScene",
		};


		class BackGroundClassBase : public SingletonBase<BackGroundClassBase> {
		private:
			friend class SingletonBase<BackGroundClassBase>;
		private:
			static const int tile_pic_size = 32;	// 画像のタイルサイズ
		private:
			//
			class Blick {
			private:
				class BlickPalette {
					int							m_paletteNum{ -1 };
					float						m_ZRad{ 0.f };
				public:
					void Set(int pal, float ZRad) noexcept {
						m_paletteNum = pal;
						m_ZRad = ZRad;
					}
					void Reset() noexcept {
						m_paletteNum = -1;
						m_ZRad = 0.f;
					}
				public:
					const auto IsActive(void) const noexcept { return this->m_paletteNum != -1; }
					const auto& GetpaletteNum(void) const noexcept { return this->m_paletteNum; }
					const auto& GetZRad(void) const noexcept { return this->m_ZRad; }
				};
			private:
				Vector3DX					m_Pos;
				bool						m_IsWall{ false };
				bool						m_IsCheckWall{ false };//壁で尚且つチェック対象かどうか
				std::array<BlickPalette, 5>	m_Palette{};
				std::array<Vector3DX, 4>	m_BoxSide;
				std::array<int, 8>			m_LinkIndex{};//CPUが斜め方向も探索するようにしたいなら8に指定
			public:
				const auto& GetPos(void) const noexcept { return this->m_Pos; }
				const auto& GetIsWall(void) const noexcept { return this->m_IsWall; }
				const auto& GetIsCheckWall(void) const noexcept { return this->m_IsCheckWall; }
				const auto& GetPalette(void) const noexcept { return this->m_Palette; }
				const auto& GetBoxSide(int num) const noexcept { return this->m_BoxSide.at(num); }
				const auto& GetLinkIndex(void) const noexcept { return this->m_LinkIndex; }
			public://コンストラクタ
				Blick(int x, int y) noexcept {
					m_Pos.x = Get2DSize((float)x);
					m_Pos.y = Get2DSize((float)y);
					for (auto& index : m_LinkIndex) {
						index = -1;
					}
					float HalfLen = Get2DSize(0.5f);
					m_BoxSide.at(0).Set(m_Pos.x - HalfLen, m_Pos.y - HalfLen, 0.f);
					m_BoxSide.at(1).Set(m_Pos.x + HalfLen, m_Pos.y - HalfLen, 0.f);
					m_BoxSide.at(2).Set(m_Pos.x + HalfLen, m_Pos.y + HalfLen, 0.f);
					m_BoxSide.at(3).Set(m_Pos.x - HalfLen, m_Pos.y + HalfLen, 0.f);
				}
			public:
				void SetBasePalette(int pal, bool IsWall) noexcept {
					this->m_Palette.at(0).Set(pal, 0.f);
					for (int i = 0; i < 4; i++) {
						this->m_Palette.at(i + 1).Reset();
					}
					this->m_IsWall = IsWall;
					this->m_IsCheckWall = this->m_IsWall;
				}
				void SetWallPallet(std::array<bool, 8> LURN) noexcept {
					if (!this->m_IsWall) { return; }
					int WayCount = 0;
					for (int i = 0; i < 4; i++) { if (LURN.at(i)) { WayCount++; } }
					switch (WayCount) {
					case 0:
						this->m_Palette.at(0).Set(0, 0.f);
						break;
					case 1:
						for (int i = 0; i < 4; i++) {
							if (LURN.at(i)) {
								this->m_Palette.at(0).Set(1, deg2rad(-90 * i + 90));
								break;
							}
						}
						break;
					case 2:
					{
						bool IsHit = false;
						for (int i = 0; i < 4; i++) {
							if (!LURN.at(i) && !LURN.at((i + 1) % 4)) {
								this->m_Palette.at(0).Set(2 + 5, deg2rad(-90 * i));
								IsHit = true;
								break;
							}
						}
						if (!IsHit) {
							for (int i = 0; i < 4; i++) {
								if (!LURN.at(i)) {
									this->m_Palette.at(0).Set(2, deg2rad(90 * i - 90));
									break;
								}
							}
						}
					}
					break;
					case 3:
						for (int i = 0; i < 4; i++) {
							if (!LURN.at(i)) {
								this->m_Palette.at(0).Set(3, deg2rad(-90 * i + 90));
								break;
							}
						}
						break;
					case 4:
						this->m_Palette.at(0).Set(4, 0.f);
						this->m_IsCheckWall = false;
						break;
					default:
						break;
					}

					for (int i = 0; i < 4; i++) {
						if (LURN.at(i) && LURN.at((i + 1) % 4) && !LURN.at((i + 4) % 8)) {
							this->m_Palette.at(i + 1).Set(9, deg2rad(-90 * i));
						}
					}
				}
				void SetLinkIndex(int num, int index) noexcept { this->m_LinkIndex.at(num) = index; }
			public://当たり判定
				bool CheckPointHit(const Vector3DX& Pos) {
					return HitPointToRectangle(
						(int)Pos.x, (int)Pos.y, 
						(int)(this->GetBoxSide(0).x), (int)(this->GetBoxSide(0).y),
						(int)(this->GetBoxSide(2).x), (int)(this->GetBoxSide(2).y));
				}
				bool CheckRectangleHit(const Vector3DX& Min, const Vector3DX& Max) {
					return 
						((int)Min.x < this->GetBoxSide(2).x && this->GetBoxSide(0).x < (int)Max.x) &&
						((int)Min.y < this->GetBoxSide(2).y && this->GetBoxSide(0).y < (int)Max.y);
				}
				bool CheckCapsuleHit(const Vector3DX& Pos1, const Vector3DX& Pos2, float Radius) {
					for (int i = 0; i < 4; i++) {
						if (GetHitCheckToCapsule(Pos1, Pos2, Radius, this->GetBoxSide(i), this->GetBoxSide((i + 1) % 4), 0.f)) {
							return true;
						}
					}
					return false;
				}
				bool CheckLineHit(const Vector3DX& Pos1, const Vector3DX& Pos2) {
					return CheckCapsuleHit(Pos1, Pos2, 0.001f);
				}
			public:
				// 環境影を描画
				void			DrawAmbientShadow(const Vector3DX& AmbientLightVec, float AmbientLightRad, const GraphHandle& ShadowChip) const noexcept ;
				// ポイント影を描画
				void			DrawPointShadow(const Vector3DX& PointLightPos, const GraphHandle& ShadowChip) noexcept ;
			};
			// プレイヤーの周囲にあるステージ壁を取得する( 検出する範囲は移動距離も考慮する )
			class CheckLines {
				std::array<Vector3DX,3>		m_Position{ };
				Vector3DX					m_Normal{};
				bool						m_Active{true};
			public:
				void			SetActive(bool value) noexcept { m_Active = value; }
			public:
				const auto&		GetPos(int num) const noexcept { return this->m_Position.at(num); }
				const auto&		GetNormal(void) const noexcept { return this->m_Normal; }
				const auto&		IsActive(void) const noexcept { return this->m_Active; }
			public:
				void			CalcNormal(void) noexcept {
					Vector3DX StartP = this->m_Position.at(0);
					Vector3DX EndP = this->m_Position.at(1);
					Vector3DX In = this->m_Position.at(2);
					Vector3DX VecP = EndP - StartP;
					this->m_Normal = Vector3DX::Cross(VecP, Vector3DX::Cross(VecP, In - StartP)).normalized();
				}
			public:
				void			SetUp(const Vector3DX& Pos0, const Vector3DX& Pos1, const Vector3DX& Pos2) noexcept {
					this->m_Active = true;
					this->m_Position.at(0) = Pos0;
					this->m_Position.at(1) = Pos1;
					this->m_Position.at(2) = Pos2;
					this->CalcNormal();
				}
			};
			//CPUに巡回させるルート
			struct PlayerPatrol {
				int					m_index{};
				std::vector<int>	m_Patrol{};
			};

			//イベントチップ
			struct EventChip {
				int					m_index{};
				int					m_EventID{ 0 };
				EventType			m_EventType{};
				//遷移用設定
				std::string			m_MapName;//行先マップ
				int					m_EntryID{};//行先マップのイベントID
				//カットシーン用設定
				int					m_CutSceneID{};//カットシーンID
			};
		private:
			int									m_Xsize{};
			int									m_Ysize{};
			std::vector<std::shared_ptr<Blick>>	m_Blick;
			std::vector<PlayerPatrol>			m_PlayerSpawn;
			std::vector<EventChip>				m_EventChip;
			std::vector<GraphHandle>			m_MapChip;
			std::vector<GraphHandle>			m_WallChip;
			float								m_AmbientShadowLength{36.f};
			float								m_AmbientLightRad = deg2rad(45);
			Vector3DX							m_AmbientLightVec;
			Vector3DX							m_PointLightPos;
			GraphHandle							m_PointShadowHandle;
			GraphHandle							m_AmbientShadowHandle;
			int									m_GetMapTextID{};
		public:
			const auto&		GetMapTextID(void) const noexcept { return this->m_GetMapTextID; }
			const auto&		GetShadowGraph(void) const noexcept { return this->m_PointShadowHandle; }
			const auto&		GetAmbientLightVec(void) const noexcept { return this->m_AmbientLightVec; }
			const auto&		GetPlayerSpawn(void) const noexcept { return this->m_PlayerSpawn; }
			const auto&		GetEventChip(void) const noexcept { return this->m_EventChip; }
			const auto		GetXSize(void) const noexcept { return this->m_Xsize; }
			const auto		GetYSize(void) const noexcept { return this->m_Ysize; }
			const auto		GetXYToNum(int x, int y) const noexcept { return std::min(x, this->m_Xsize - 1) * this->m_Ysize + std::min(y, this->m_Ysize - 1); }
			const auto		GetNumToXY(int num) const noexcept { return std::make_pair<int,int>(num / this->m_Ysize, num % this->m_Ysize); }
			const auto&		GetFloorData(int num) const noexcept { return this->m_Blick.at(num); }
			const auto&		GetFloorData(int x, int y) const noexcept { return GetFloorData(GetXYToNum(x, y)); }
			const auto		GetNearestFloors(const Vector3DX& Pos) const noexcept {
				for (auto& B : m_Blick) {
					if (B->GetIsWall()) { continue; }
					if (!B->CheckPointHit(Pos)) { continue; }
					return (int)(&B - &m_Blick.front());
				}
				return -1;
			}
			const auto		GetNearFloorsList(const Vector3DX& Pos) const noexcept {
				std::vector<int> SelList;
				float ViewLimit = Get2DSize(5.f);
				for (auto& B : m_Blick) {
					if (B->GetIsWall()) { continue; }
					if ((B->GetPos() - Pos).sqrMagnitude() < ViewLimit * ViewLimit) {
						SelList.emplace_back((int)(&B - &m_Blick.front()));
					}
				}
				return SelList;
			}
			const float		CheckHideShadow(const Vector3DX& PosA, const Vector3DX& PosB, float Radius) noexcept ;
			const bool		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius, bool IsPhysical) const noexcept ;
		public:
			void			SetAmbientLight(float ShadowLen, float Rad) noexcept {
				m_AmbientShadowLength = ShadowLen;
				m_AmbientLightRad = Rad;
				float Radius = (float)y_r(m_AmbientShadowLength);
				m_AmbientLightVec.Set(std::sin(m_AmbientLightRad) * Radius, std::cos(m_AmbientLightRad) * Radius, 0.f);
			}
			void			SetPointLight(const Vector3DX& Pos) noexcept {
				m_PointLightPos = Convert2DtoDisp(Pos);
			}
		public:
			void			Init(const std::string& MapPath) noexcept ;
			void			Execute(void) noexcept {}
			void			SetupShadow(std::function<void()> AddAmbShadow) noexcept ;
			void			Draw(void) noexcept ;
			void			DrawFront(void) noexcept ;
			void			Dispose(void) noexcept ;
		};
	};
};
