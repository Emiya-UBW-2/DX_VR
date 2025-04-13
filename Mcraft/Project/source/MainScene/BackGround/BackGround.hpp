#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"

#define CHECKTHREADTIME (false)

namespace FPS_n2 {
	namespace BackGround {
		class MazeControl {
			enum class MAZETYPE :int {
				WALL,
				PATH,
			};
		private:
			int		m_Width{ 1 };
			int		m_Height{ 1 };
			std::vector<std::vector<MAZETYPE>>		m_Maze;
		private:
			//穴掘り
			void dig(int x, int y) {
				//指定した部分を掘っておく
				this->m_Maze[x][y] = MAZETYPE::PATH;
				// どの方向を掘ろうとしたかを覚えておく変数
				int ok = 0;
				// 全方向試すまでループ
				while (ok != 0b1111) {
					int Dir = GetRand(3) % 4;
					ok |= (1 << Dir);
					float rad = deg2rad(Dir * 90);
					int next_x = x + static_cast<int>(sin(rad) * 2.f);//0 2 0 -2
					int next_y = y + static_cast<int>(cos(rad) * 2.f);//2 0 -2 0
					if ((0 <= next_x && next_x < this->m_Width) && (0 <= next_y && next_y < this->m_Height)) {
						if (this->m_Maze[next_x][next_y] == MAZETYPE::WALL) {
							this->m_Maze[static_cast<size_t>((next_x + x) / 2)][static_cast<size_t>((next_y + y) / 2)] = MAZETYPE::PATH;
							//その場から次の穴掘り
							dig(next_x, next_y);
						}
					}
				}
			}
		public:
			//該当座標が通路かどうか
			const auto PosIsPath(int x, int y) {
				if ((0 <= x && x < this->m_Width) && (0 <= y && y < this->m_Height)) {
					return this->m_Maze[x][y] == MAZETYPE::PATH;
				}
				else {
					return false;
				}
			}
			//通路の総数を取得
			const auto GetPachCount(void) noexcept {
				int OneSize = 0;
				for (int y = 0; y < this->m_Height; ++y) {
					for (int x = 0; x < this->m_Width; ++x) {
						if (PosIsPath(x, y)) {
							++OneSize;
						}
					}
				}
				return OneSize;
			}
		public:
			// 迷路を作成する
			void createMaze(int width, int height) {
				this->m_Width = width;
				this->m_Height = height;

				this->m_Maze.resize(this->m_Width);
				for (auto& mazeX : this->m_Maze) {
					mazeX.resize(this->m_Height);
					for (auto& cell : mazeX) {
						cell = MAZETYPE::WALL; // 全マス壁にする
					}
				}

				// 開始点をランダム（奇数座標）に決定する
				dig(std::clamp(2 * GetRand(this->m_Width / 2) + 1, 0, this->m_Width - 1), std::clamp(2 * GetRand(this->m_Height / 2) + 1, 0, this->m_Height - 1));
				//追加で穴あけ
				for (int y = 1; y < this->m_Height - 1; ++y) {
					for (int x = 1; x < this->m_Width - 1; ++x) {
						bool isHit = false;
						if (x == 1 || (x == this->m_Width - 1 - 1)) {
							isHit = true;
						}
						if (y == 1 || (y == this->m_Height - 1 - 1)) {
							isHit = true;
						}
						if (!isHit) { continue; }
						this->m_Maze[x][y] = MAZETYPE::PATH;
					}
				}
				//追加で穴あけ
				for (int y = 1; y < this->m_Height - 1; ++y) {
					for (int x = 1; x < this->m_Width - 1; ++x) {
						bool isHit = false;
						if ((y % 6) == 0) {
							if ((x % 6) == 0) { isHit = true; }
						}
						if (!isHit) { continue; }
						this->m_Maze[x][y] = MAZETYPE::PATH;
					}
				}
			}
			void Reset(void) noexcept {
				for (auto& mx : this->m_Maze) {
					mx.clear();
				}
				this->m_Maze.clear();
				this->m_Width = 1;
				this->m_Height = 1;
			}
		};



		static constexpr int8_t s_EmptyBlick = 0;
		static constexpr int TotalCellLayer = 4;
		static constexpr int MulPer = 2;
		static constexpr float CellScale = Scale3DRate / 2.f / 2.f;

		static constexpr int DrawMax = 50;//65

		static constexpr int DrawMaxXPlus = DrawMax;
		static constexpr int DrawMaxXMinus = -DrawMax;

		static constexpr int DrawMaxZPlus = DrawMax;
		static constexpr int DrawMaxZMinus = -DrawMax;

		static constexpr int DrawMaxYPlus = 40;
		static constexpr int DrawMaxYMinus = -30;


		static constexpr int DrawMinXPlus = DrawMaxXPlus / MulPer;
		static constexpr int DrawMinXMinus = DrawMaxXMinus / MulPer;

		static constexpr int DrawMinZPlus = DrawMaxZPlus / MulPer;
		static constexpr int DrawMinZMinus = DrawMaxZMinus / MulPer;

		static constexpr int DrawMinYPlus = DrawMaxYPlus / MulPer;
		static constexpr int DrawMinYMinus = DrawMaxYMinus / MulPer;

		class BackGroundControl : public SingletonBase<BackGroundControl> {
		private:
			friend class SingletonBase<BackGroundControl>;
		private:
			struct Vector3Int {
				int x{};
				int y{};
				int z{};

			public:
				Vector3Int(int X, int Y, int Z)noexcept {
					this->x = X;
					this->y = Y;
					this->z = Z;
				}
			};
			struct CellBuffer {
				int8_t					m_Cell{};
				int8_t					m_FillInfo{};//周りの遮蔽データのbitフラグ
			public:
				bool IsEmpty(void) const noexcept { return this->m_Cell == s_EmptyBlick; }
				bool IsOcclusion(void) const noexcept { return this->m_FillInfo == 0b111111; }
			};
			struct CellsData {
				std::vector<CellBuffer>	m_CellBuffer;
				int		m_scaleRate = 1;
				int		m_All = 256 / this->m_scaleRate;
				//算術補助系
				int		m_Half = this->m_All / 2;
				int		m_AllPow2 = this->m_All * this->m_All;
				float	m_Scale = (CellScale * this->m_scaleRate);
			public:
				//
				const int	GetIndex(int t) const noexcept { return (t % this->m_All + this->m_All) % this->m_All; }
				const size_t	GetCellNum(int x, int y, int z) const noexcept { return static_cast<size_t>(GetIndex(x) * this->m_AllPow2 + y * this->m_All + GetIndex(z)); }
				//
				const auto&		GetCellBuf(int x, int y, int z) const noexcept { return this->m_CellBuffer[GetCellNum(x, y, z)]; }
				auto&			SetCellBuf(int x, int y, int z) noexcept { return this->m_CellBuffer[GetCellNum(x, y, z)]; }
				//
				const auto GetPosBuffer(int x, int y, int z,int ID) const noexcept {
					return GetPos(x + ((ID >> 2) & 1), y + ((ID >> 1) & 1), z + (ID & 1));
				}
				//
				const int8_t	isFill(int x, int y, int z, int mul) const noexcept {
					mul /= this->m_scaleRate;
					int FillCount = 0;
					int FillAll = 0;

					int xMaxmin = x * mul + mul - 1;
					int yMaxmin = y * mul + mul - 1;
					int zMaxmin = z * mul + mul - 1;
					std::vector<int> IDCount;
					for (int xt = xMaxmin; xt < xMaxmin + mul; ++xt) {
						for (int yt = yMaxmin; yt < std::min(yMaxmin + mul, this->m_All); ++yt) {
							for (int zt = zMaxmin; zt < zMaxmin + mul; ++zt) {
								++FillAll;
								if (GetCellBuf(xt, yt, zt).IsEmpty()) { continue; }
								++FillCount;
								auto cell = GetCellBuf(xt, yt, zt).m_Cell + 1;
								if (cell > IDCount.size()) {
									IDCount.resize(cell);
								}
								++IDCount.at(static_cast<size_t>(cell - 1));
							}
						}
					}
					if ((FillAll != 0) && (static_cast<float>(FillCount) / FillAll >= (1.f / 4.f))) {
						int max = -1;
						int8_t id = 1;
						for (int8_t loop = 0; auto & idc : IDCount) {
							if (max < idc) {
								max = idc;
								id = loop;
							}
							++loop;
						}
						return id;
					}
					else {
						return s_EmptyBlick;
					}
				}
				const bool		isInside(int y) const noexcept { return ((0 <= y) && (y < this->m_All)); }
				const Vector3DX	GetPos(int x, int y, int z) const noexcept {
					return Vector3DX::vget(static_cast<float>(x - this->m_Half), static_cast<float>(y - this->m_Half), static_cast<float>(z - this->m_Half)) * this->m_Scale;
				}
				const Vector3Int GetPoint(const Vector3DX& pos) const noexcept {
					Vector3DX Start = pos / this->m_Scale;
					return Vector3Int(static_cast<int>(Start.x) + this->m_Half, static_cast<int>(Start.y) + this->m_Half, static_cast<int>(Start.z) + this->m_Half);
				}

				//
				void			SetScale(int scale) noexcept {
					this->m_scaleRate = scale;
					this->m_All = 256 / this->m_scaleRate;
					//算術補助系
					this->m_Half = this->m_All / 2;
					this->m_AllPow2 = this->m_All * this->m_All;
					this->m_Scale = (CellScale * this->m_scaleRate);
					this->m_CellBuffer.resize(static_cast<size_t>(this->m_All * this->m_All * this->m_All));
				}
				//
				void			CalcOcclusion(int x, int y, int z) noexcept {
					SetCellBuf(x, y, z).m_FillInfo = 0;
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 0) * (!GetCellBuf(x + 1, y, z).IsEmpty());
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 1) * (!GetCellBuf(x - 1, y, z).IsEmpty());
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 2) * ((y == this->m_All - 1) ? true : !GetCellBuf(x, y + 1, z).IsEmpty());
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 3) * ((y == 0) ? true : !GetCellBuf(x, y - 1, z).IsEmpty());
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 4) * (!GetCellBuf(x, y, z + 1).IsEmpty());
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 5) * (!GetCellBuf(x, y, z - 1).IsEmpty());
				}
			};
			//
			struct ThreadJobs {
				std::thread						m_Job;
				bool							m_JobEnd{};
				bool							m_IsDoOnce{};
				bool							m_isEnd{};//
#if defined(DEBUG) && CHECKTHREADTIME
				LONGLONG						m_StartTime{};
				LONGLONG						m_TotalTime{};
#endif
				std::function<void()>			m_Doing{ nullptr };
				std::function<void()>			m_EndDoing{ nullptr };
			public:
				void Init(std::function<void()> Doing, std::function<void()> EndDoing, bool IsDoOnce) noexcept {
					this->m_JobEnd = true;
					this->m_isEnd = false;
					this->m_IsDoOnce = IsDoOnce;
					this->m_Doing = Doing;
					this->m_EndDoing = EndDoing;
				}

				void Execute(void) noexcept {
					if (this->m_JobEnd) {
						this->m_JobEnd = false;
#if defined(DEBUG) && CHECKTHREADTIME
						this->m_TotalTime = GetNowHiPerformanceCount() - this->m_StartTime;
						this->m_StartTime = GetNowHiPerformanceCount();
#endif
						if (this->m_Job.joinable()) {
							this->m_Job.detach();
						}
						//
						if (this->m_EndDoing) {
							this->m_EndDoing();
						}
						//
						if (!this->m_IsDoOnce) {
							this->m_isEnd = false;
						}
						if(!this->m_isEnd) {
							std::thread tmp([&]() {
								if (this->m_Doing) {
									this->m_Doing();
								}
								this->m_JobEnd = true;
								this->m_isEnd = true;
								});
							this->m_Job.swap(tmp);
							//強制待機
							//this->m_Job.join();
						}
					}
#if defined(DEBUG) && CHECKTHREADTIME
					printfDx("%5.2fms \n", static_cast<float>(this->m_TotalTime) / 1000.f);
#endif
				}

				void Dispose(void) noexcept {
					if (this->m_Job.joinable()) {
						this->m_Job.detach();
					}
					this->m_Doing = nullptr;
					this->m_EndDoing = nullptr;
				}
			};
			//
			template<class T>
			class vert32 {
				int												m_Now{ 0 };

				size_t											m_32Size{ 0 };
				std::array<std::vector<T>, 2>					m_vert32;
				std::array<std::vector<uint32_t>, 2>			m_index32;
				std::array<size_t, 2>							m_32Num{ 0 };
			public:
				const auto& GetInNum(void) const noexcept { return this->m_32Num.at(this->m_Now); }
				auto& SetInVert(void) noexcept { return this->m_vert32.at(this->m_Now); }
				auto& SetInIndex(void) noexcept { return this->m_index32.at(this->m_Now); }
			public:
				const auto& GetOutNum(void) const noexcept { return this->m_32Num.at(static_cast<size_t>(1 - this->m_Now)); }
				const auto& GetOutVert(void) const noexcept { return this->m_vert32.at(static_cast<size_t>(1 - this->m_Now)); }
				const auto& GetOutindex(void) const noexcept { return this->m_index32.at(static_cast<size_t>(1 - this->m_Now)); }
			public:
				void		Init(size_t size) noexcept {
					for (int loop = 0; loop < 2; ++loop) {
						this->m_vert32.at(loop).resize(size * 4);
						this->m_index32.at(loop).resize(size * 6);
					}
					this->m_32Num.at(this->m_Now) = 0;
					this->m_32Size = size;
				}
				void		ResetNum(void) noexcept {
					this->m_32Num.at(this->m_Now) = 0;
				}
				void		AllocatePlane(void) noexcept {
					++this->m_32Num.at(this->m_Now);
					if (GetInNum() > this->m_32Size) {
						this->m_32Size = GetInNum();
						for (int loop = 0; loop < 2; ++loop) {
							this->m_vert32.at(loop).resize(this->m_32Size * 4);
							this->m_index32.at(loop).resize(this->m_32Size * 6);
						}
					}
					auto ZERO = (uint32_t)(GetInNum() * 4 - 4);
					SetInIndex()[GetInNum() * 6 - 6] = ZERO;
					SetInIndex()[GetInNum() * 6 - 5] = ZERO + 1;
					SetInIndex()[GetInNum() * 6 - 4] = ZERO + 2;
					SetInIndex()[GetInNum() * 6 - 3] = ZERO + 3;
					SetInIndex()[GetInNum() * 6 - 2] = ZERO + 2;
					SetInIndex()[GetInNum() * 6 - 1] = ZERO + 1;
				}
				void		FlipVerts(void) noexcept {
					this->m_Now = 1 - this->m_Now;
				}
				void		Disable(void) noexcept {
					ResetNum();
					this->m_32Num.at(static_cast<size_t>(1 - this->m_Now)) = 0;
				}
				void		DrawByShader(void) const noexcept {
					if (GetOutNum() > 0) {
						DrawPolygon32bitIndexed3DToShader(GetOutVert().data(), static_cast<int>(GetOutNum() * 4), GetOutindex().data(), static_cast<int>(GetOutNum() * 6 / 3));
					}
				}
				void		Draw(const GraphHandle& GrHandle) const noexcept {
					if (GetOutNum() > 0) {
						DrawPolygon32bitIndexed3D(GetOutVert().data(), static_cast<int>(GetOutNum() * 4), GetOutindex().data(), static_cast<int>(GetOutNum() * 6 / 3), GrHandle.get(), true);
					}
				}
			};
			//
			MV1								m_ObjSky;
			GraphHandle						m_tex{};
			GraphHandle						m_norm{};
			std::vector<int8_t>				m_CellBase{};
			std::array<CellsData, TotalCellLayer>	m_CellxN;
			std::array<ThreadJobs, TotalCellLayer + TotalCellLayer + TotalCellLayer>	m_Jobs;
			//
			int								m_BaseRate = 100;
			int								m_ShadowRate = 100;
			int								m_ThreadCounter = 0;
			struct Draw {
				vert32<VERTEX3D>	m_vert32;
				Vector3DX			m_CamPos;
				Vector3DX			m_CamVec;
			};
			struct DrawSB {
				vert32<VERTEX3D>	m_vert32;
				Vector3DX			m_CamPos;
				Vector3DX			m_light;
			};
			struct DrawSS {
				vert32<VERTEX3DSHADER>	m_vert32;
				Vector3DX			m_CamPos;
				Vector3DX			m_CamVec;
			};
			//表示ポリゴンスレッド用
			std::array<Draw, TotalCellLayer>	m_Draws;
			//影スレッド用
			std::array<DrawSB, TotalCellLayer>	m_DrawsSB;
			std::array<DrawSS, TotalCellLayer>	m_DrawsSS;

			MazeControl						m_MazeControl;
			//
#if defined(DEBUG) & EDITBLICK
			//Edit
			float							LenMouse = 2.f;
			int								xput = 5;
			int								yput = 2;
			int								zput = 5;
			int8_t							blickselect = -1;
			Vector3DX						PutPos;
#endif
		public:
			BackGroundControl(void) noexcept { Load(); }
			BackGroundControl(const BackGroundControl&) = delete;
			BackGroundControl(BackGroundControl&&) = delete;
			BackGroundControl& operator=(const BackGroundControl&) = delete;
			BackGroundControl& operator=(BackGroundControl&&) = delete;

			virtual ~BackGroundControl(void) noexcept { Dispose(); }
		private:
			inline static void		Bresenham3D(int x1, int y1, int z1, int x2, int y2, int z2, const std::function<bool(int, int, int)>& OutPutLine) noexcept {
				int err_1{}, err_2{};
				int point[3]{};

				point[0] = x1;
				point[1] = y1;
				point[2] = z1;
				const int dx = x2 - x1;
				const int dy = y2 - y1;
				const int dz = z2 - z1;
				const int x_inc = (dx < 0) ? -1 : 1;
				const int Length = std::abs(dx);
				const int y_inc = (dy < 0) ? -1 : 1;
				const int m = std::abs(dy);
				const int z_inc = (dz < 0) ? -1 : 1;
				const int n = std::abs(dz);
				const int dx2 = Length << 1;
				const int dy2 = m << 1;
				const int dz2 = n << 1;

				if ((Length >= m) && (Length >= n)) {
					err_1 = dy2 - Length;
					err_2 = dz2 - Length;
					for (int loop = 0; loop < Length; ++loop) {
						if (OutPutLine(point[0], point[1], point[2])) { return; }
						if (err_1 > 0) {
							point[1] += y_inc;
							err_1 -= dx2;
						}
						if (err_2 > 0) {
							point[2] += z_inc;
							err_2 -= dx2;
						}
						err_1 += dy2;
						err_2 += dz2;
						point[0] += x_inc;
					}
				}
				else if ((m >= Length) && (m >= n)) {
					err_1 = dx2 - m;
					err_2 = dz2 - m;
					for (int loop = 0; loop < m; ++loop) {
						if (OutPutLine(point[0], point[1], point[2])) { return; }
						if (err_1 > 0) {
							point[0] += x_inc;
							err_1 -= dy2;
						}
						if (err_2 > 0) {
							point[2] += z_inc;
							err_2 -= dy2;
						}
						err_1 += dx2;
						err_2 += dz2;
						point[1] += y_inc;
					}
				}
				else {
					err_1 = dy2 - n;
					err_2 = dx2 - n;
					for (int loop = 0; loop < n; ++loop) {
						if (OutPutLine(point[0], point[1], point[2])) { return; }
						if (err_1 > 0) {
							point[1] += y_inc;
							err_1 -= dz2;
						}
						if (err_2 > 0) {
							point[0] += x_inc;
							err_2 -= dz2;
						}
						err_1 += dy2;
						err_2 += dx2;
						point[2] += z_inc;
					}
				}
				OutPutLine(point[0], point[1], point[2]);
			}
			inline static bool		ColRayBox(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AABBMinPos, const Vector3DX& AABBMaxPos, Vector3DX* Normal = nullptr) noexcept {
				Vector3DX Vec = (*EndPos - StartPos);
				// 交差判定
				float start[3]{}, dir[3]{}, min[3]{}, max[3]{};
				start[0] = StartPos.x;
				start[1] = StartPos.y;
				start[2] = StartPos.z;
				dir[0] = Vec.x;
				dir[1] = Vec.y;
				dir[2] = Vec.z;

				min[0] = AABBMinPos.x;
				min[1] = AABBMinPos.y;
				min[2] = AABBMinPos.z;

				max[0] = AABBMaxPos.x;
				max[1] = AABBMaxPos.y;
				max[2] = AABBMaxPos.z;

				float t_min = -FLT_MAX;
				float t_max = FLT_MAX;

				for (int loop = 0; loop < 3; ++loop) {
					/*
					if (abs(dir[loop]) < FLT_EPSILON) {
						if (start[loop] < min[loop] || start[loop] > max[loop])
							return false; // 交差していない
					}
					else 
					//*/
					{
						// スラブとの距離を算出
						// t1が近スラブ、t2が遠スラブとの距離
						float odd = 1.0f / dir[loop];
						float t1 = (min[loop] - start[loop]) * odd;
						float t2 = (max[loop] - start[loop]) * odd;
						if (t1 > t2) {
							float tmp = t1;
							t1 = t2;
							t2 = tmp;
						}

						if (t1 > t_min) {
							t_min = t1;
						}
						if (t2 < t_max) {
							t_max = t2;
						}

						// スラブ交差チェック
						if (t_min >= t_max) {
							return false;
						}
					}
				}

				// 交差している
				if (EndPos) {
					*EndPos = StartPos + Vec * t_min;
				}
				float ret[3] = { EndPos->x, EndPos->y, EndPos->z };
				//どの向き？
				if (Normal) {
					for (int loop = 0; loop < 3; ++loop) {
						if (std::abs(ret[loop] - min[loop]) < 0.00001f) {
							switch (loop) {
							case 0:
								*Normal = Vector3DX::left();
								break;
							case 1:
								*Normal = Vector3DX::down();
								break;
							case 2:
								*Normal = Vector3DX::back();
								break;
							default:
								break;
							}
						}
						if (std::abs(ret[loop] - max[loop]) < 0.00001f) {
							switch (loop) {
							case 0:
								*Normal = Vector3DX::right();
								break;
							case 1:
								*Normal = Vector3DX::up();
								break;
							case 2:
								*Normal = Vector3DX::forward();
								break;
							default:
								break;
							}
						}
					}
				}

				return true;
			}
			inline static bool		CalcYZActive(const Vector3DX& Vec, int x, int yMaxmin, int yMaxmax, int zMaxmin, int zMaxmax) noexcept {
				//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(x) + 0.5f, static_cast<float>(yMaxmin) + 0.5f, static_cast<float>(zMaxmin) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(x) + 0.5f, static_cast<float>(yMaxmax) + 0.5f, static_cast<float>(zMaxmax) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(x) + 0.5f, static_cast<float>(yMaxmin) + 0.5f, static_cast<float>(zMaxmax) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(x) + 0.5f, static_cast<float>(yMaxmax) + 0.5f, static_cast<float>(zMaxmin) + 0.5f)) > 0.f) { return true; }
				return false;
			}
			inline static bool		CalcZMinMax(const Vector3DX& Vec, int x, int y, int* zMaxmin, int* zMaxmax) noexcept {
				Vector3DX Vec1 = Vector3DX::vget(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f, static_cast<float>(*zMaxmin) + 0.5f);
				Vector3DX Vec2 = Vector3DX::vget(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f, static_cast<float>(*zMaxmax) + 0.5f);

				float dTa = Vector3DX::Dot(Vec, Vec1);
				float dTb = Vector3DX::Dot(Vec, Vec2);
				bool OnFront = (dTa >= 0.f && dTb >= 0.f);
				if (OnFront || (dTa < 0.f && dTb < 0.f)) {
					return OnFront;
				}
				int HitPosZ = *zMaxmin + static_cast<int>((*zMaxmax - *zMaxmin) * std::clamp(std::abs(dTa) / (std::abs(dTa) + std::abs(dTb)), 0.f, 1.f));
				if (dTa < 0.f) {
					*zMaxmin = std::max(HitPosZ - 1, *zMaxmin);
				}
				else {
					*zMaxmax = std::min(HitPosZ + 1, *zMaxmax);
				}
				return true;
			}
			inline static bool		CalcXYActive(const Vector3DX& Vec, int xMaxmin, int xMaxmax, int yMaxmin, int yMaxmax, int z) noexcept {
				//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xMaxmin) + 0.5f, static_cast<float>(yMaxmin) + 0.5f, static_cast<float>(z) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xMaxmax) + 0.5f, static_cast<float>(yMaxmax) + 0.5f, static_cast<float>(z) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xMaxmax) + 0.5f, static_cast<float>(yMaxmin) + 0.5f, static_cast<float>(z) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xMaxmin) + 0.5f, static_cast<float>(yMaxmax) + 0.5f, static_cast<float>(z) + 0.5f)) > 0.f) { return true; }
				return false;
			}
			inline static bool		CalcXMinMax(const Vector3DX& Vec, int* xMaxmin, int* xMaxmax, int y, int z) noexcept {
				Vector3DX Vec1 = Vector3DX::vget(static_cast<float>(*xMaxmin) + 0.5f, static_cast<float>(y) + 0.5f, static_cast<float>(z) + 0.5f);
				Vector3DX Vec2 = Vector3DX::vget(static_cast<float>(*xMaxmax) + 0.5f, static_cast<float>(y) + 0.5f, static_cast<float>(z) + 0.5f);

				float dTa = Vector3DX::Dot(Vec, Vec1);
				float dTb = Vector3DX::Dot(Vec, Vec2);
				bool OnFront = (dTa >= 0.f && dTb >= 0.f);
				if (OnFront || (dTa < 0.f && dTb < 0.f)) {
					return OnFront;
				}
				int HitPosX = *xMaxmin + static_cast<int>((*xMaxmax - *xMaxmin) * std::clamp(std::abs(dTa) / (std::abs(dTa) + std::abs(dTb)), 0.f, 1.f));
				if (dTa < 0.f) {
					*xMaxmin = std::max(HitPosX - 1, *xMaxmin);
				}
				else {
					*xMaxmax = std::min(HitPosX + 1, *xMaxmax);
				}
				return true;
			}
		private:
			bool			AddCubeX_CanAddPlane(const CellsData& cellx, int xmin, int xmax, int cy, int cz, int id) noexcept;
			bool			AddCubeZ_CanAddPlane(const CellsData& cellx, int cx, int cy, int zmin, int zmax, int id) noexcept;
			//
			void			AddPlaneXPlus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneXMinus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneYPlus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneYMinus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneZPlus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int xmin, int xmax, int y, int z, bool IsCalcUV) noexcept;
			void			AddPlaneZMinus(vert32<VERTEX3D>* pTarget, const CellsData& cellx, int xmin, int xmax, int y, int z, bool IsCalcUV) noexcept;

			void			AddCubes(size_t id) noexcept;
			void			AddShadowCubes(size_t id) noexcept;
			//
			void			AddSetShadowPlaneXPlus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddSetShadowPlaneXMinus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddSetShadowPlaneYPlus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddSetShadowPlaneYMinus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddSetShadowPlaneZPlus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept;
			void			AddSetShadowPlaneZMinus(vert32<VERTEX3DSHADER>* pTarget, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept;

			void			AddSetShadowCubes(size_t id) noexcept;
		public:
			bool			CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) const noexcept;
			bool			CheckLinetoMap(const Vector3DX& StartPos, const Vector3DX& EndPos) const noexcept {
				Vector3DX pEndPos = EndPos;
				return CheckLinetoMap(StartPos, &pEndPos);
			}
			bool			CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept;

			void			LoadCellsFile(void) noexcept;
			void			SaveCellsFile(void) noexcept;

			void			LoadCellsClip(int xpos, int ypos,int zpos) noexcept;
			void			SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax) noexcept;

			void			SettingChange(void) noexcept;

			void			SetBlick(int x, int y, int z, int8_t select) noexcept;
			const Vector3Int GetPoint(const Vector3DX& pos) const noexcept { return this->m_CellxN.front().GetPoint(pos); }
			const auto& GetCellBuf(int x, int y, int z) const noexcept {
				auto& cell = this->m_CellxN.front();
				//if (!cell.isInside(y)) { return; }
				return cell.GetCellBuf(x, y, z);
			}
		public://
			void			Load(void) noexcept;
			//
			void			Init(void) noexcept;
			//
			void			Execute(void) noexcept;
			//
			void			BG_Draw(void) const noexcept;
			void			Shadow_Draw(void) const noexcept;
			void			SetShadow_Draw_Rigid(void) const noexcept;
			void			Draw(void) const noexcept;
			//
			void			Dispose(void) noexcept;
		};
	}
}
