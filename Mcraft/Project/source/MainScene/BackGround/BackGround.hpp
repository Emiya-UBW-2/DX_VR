#pragma once
#pragma warning(disable:4464)
#include "../../Header.hpp"
#include "../../MainScene/BackGround/BackGroundSub.hpp"

#define EDITBLICK (TRUE)

namespace FPS_n2 {
	namespace BackGround {
		static constexpr int8_t s_EmptyBlick = 0;
		static constexpr int total = 4;
		static constexpr int MulPer = 2;
		static constexpr float CellScale = Scale3DRate / 2.f / 2.f;

		static constexpr int DrawMax = 25;//65

		static constexpr int DrawMaxXPlus = DrawMax;
		static constexpr int DrawMaxXMinus = -DrawMax;

		static constexpr int DrawMaxZPlus = DrawMax;
		static constexpr int DrawMaxZMinus = -DrawMax;

		static constexpr int DrawMaxYPlus = 15 * 2 / 5;//30 * 6 / 5
		static constexpr int DrawMaxYMinus = -15 * 8 / 5;//-30 * 4 / 5


		static constexpr int DrawMinXPlus = DrawMaxXPlus / MulPer;
		static constexpr int DrawMinXMinus = DrawMaxXMinus / MulPer;

		static constexpr int DrawMinZPlus = DrawMaxZPlus / MulPer;
		static constexpr int DrawMinZMinus = DrawMaxZMinus / MulPer;

		static constexpr int DrawMinYPlus = DrawMaxYPlus / MulPer;
		static constexpr int DrawMinYMinus = DrawMaxYMinus / MulPer;

		class BackGroundClass : public SingletonBase<BackGroundClass> {
		private:
			friend class SingletonBase<BackGroundClass>;
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
			struct CellBuffer
			{
				int8_t m_Cell{};
				int8_t m_FillInfo{};//周りの遮蔽データのbitフラグ
				std::array<uint8_t,8> m_DifColorPow{};
			};
			struct CellsData {
				std::vector<CellBuffer> m_CellBuffer;
				int scaleRate = 1;
				int All = 256 / scaleRate;
				//算術補助系
				int Half = All / 2;
				int AllPow2 = All * All;
				float Scale = (CellScale * scaleRate);
				//
				const int	GetIndex(int t) const noexcept { return (t % All + All) % All; }
				const size_t	GetCellNum(int x, int y, int z) const noexcept { return static_cast<size_t>(GetIndex(x) * AllPow2 + y * All + GetIndex(z)); }
				//
				const auto&		GetCellBuf(int x, int y, int z) const noexcept { return m_CellBuffer[GetCellNum(x, y, z)]; }
				auto&			SetCellBuf(int x, int y, int z) noexcept { return m_CellBuffer[GetCellNum(x, y, z)]; }
				//
				const auto GetPosBuffer(int x, int y, int z,int ID) const noexcept {
					return GetPos(x + ((ID >> 2) & 1), y + ((ID >> 1) & 1), z + (ID & 1));
				}
				//
				const int8_t	isFill(int x, int y, int z, int mul) const noexcept {
					mul /= scaleRate;
					int FillCount = 0;
					int FillAll = 0;

					int xMaxmin = x * mul + mul - 1;
					int yMaxmin = y * mul + mul - 1;
					int zMaxmin = z * mul + mul - 1;
					std::vector<int> IDCount;
					for (int xt = xMaxmin; xt < xMaxmin + mul; ++xt) {
						for (int yt = yMaxmin; yt < std::min(yMaxmin + mul, All); ++yt) {
							for (int zt = zMaxmin; zt < zMaxmin + mul; ++zt) {
								++FillAll;
								if (GetCellBuf(xt, yt, zt).m_Cell == s_EmptyBlick) { continue; }
								++FillCount;
								auto cell = GetCellBuf(xt, yt, zt).m_Cell + 1;
								if (cell > IDCount.size()) {
									IDCount.resize(cell);
								}
								IDCount.at(static_cast<size_t>(cell - 1))++;
							}
						}
					}
					if ((FillAll != 0) && (static_cast<float>(FillCount) / FillAll >= (1.f / 4.f))) {
						int max = -1;
						int8_t id = 1;
						for (int8_t index = 0; auto & i : IDCount) {
							if (max < i) {
								max = i;
								id = index;
							}
							index++;
						}
						return id;
					}
					else {
						return s_EmptyBlick;
					}
				}
				const bool		isInside(int y) const noexcept { return ((0 <= y) && (y < All)); }
				const Vector3DX	GetPos(int x, int y, int z) const noexcept {
					return Vector3DX::vget(static_cast<float>(x - Half), static_cast<float>(y - Half), static_cast<float>(z - Half)) * Scale;
				}
				const Vector3Int GetPoint(const Vector3DX& pos) const noexcept {
					Vector3DX Start = pos / Scale;
					return Vector3Int(static_cast<int>(Start.x) + Half, static_cast<int>(Start.y) + Half, static_cast<int>(Start.z) + Half);
				}

				//
				void			SetScale(int scale) noexcept {
					scaleRate = scale;
					All = 256 / scaleRate;
					//算術補助系
					Half = All / 2;
					AllPow2 = All * All;
					Scale = (CellScale * scaleRate);
					m_CellBuffer.resize((size_t)(All * All * All));
				}
				//
				void			CalcOcclusion(int x, int y, int z) noexcept {
					SetCellBuf(x, y, z).m_FillInfo = 0;
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 0) * (GetCellBuf(x + 1, y, z).m_Cell != s_EmptyBlick);
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 1) * (GetCellBuf(x - 1, y, z).m_Cell != s_EmptyBlick);
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 2) * ((y == All - 1) ? 1 : (GetCellBuf(x, y + 1, z).m_Cell != s_EmptyBlick));
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 3) * ((y == 0) ? 1 : (GetCellBuf(x, y - 1, z).m_Cell != s_EmptyBlick));
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 4) * (GetCellBuf(x, y, z + 1).m_Cell != s_EmptyBlick);
					SetCellBuf(x, y, z).m_FillInfo |= (1 << 5) * (GetCellBuf(x, y, z - 1).m_Cell != s_EmptyBlick);
				}
			};
			//
			struct ThreadJobs {
				std::thread						m_Job;
				bool							m_JobEnd{};
				LONGLONG						m_StartTime{};
				LONGLONG						m_TotalTime{};
				std::function<void()>			m_Doing{ nullptr };
				std::function<void()>			m_EndDoing{ nullptr };
			public:
				void Init(std::function<void()> Doing, std::function<void()> EndDoing) noexcept {
					m_JobEnd = true;
					m_Doing = Doing;
					m_EndDoing = EndDoing;
				}

				void Execute(void) noexcept {
					if (m_JobEnd) {
						m_JobEnd = false;
						m_TotalTime = GetNowHiPerformanceCount() - m_StartTime;
						m_StartTime = GetNowHiPerformanceCount();
						if (m_Job.joinable()) {
							m_Job.detach();
						}
						//
						if (m_EndDoing) {
							m_EndDoing();
						}
						//
						{
							std::thread tmp([&]() {
								if (m_Doing) {
									m_Doing();
								}
								m_JobEnd = true;
								});
							m_Job.swap(tmp);
							//強制待機
							//m_Job.join();
						}
					}
#if defined(DEBUG)
					printfDx("%5.2fms \n", (float)(m_TotalTime) / 1000.f);
#endif
				}

				void Dispose(void) noexcept {
					if (m_Job.joinable()) {
						m_Job.detach();
					}
					m_Doing = nullptr;
					m_EndDoing = nullptr;
				}
			};
			//
			template<class T>
			struct vert32 {
				std::vector<T>					m_vert32;
				std::vector<uint32_t>			m_index32;
				size_t							m_32Num{ 0 };
				size_t							m_32Size{ 0 };
				std::vector<T>					m_vert32Out;
				std::vector<uint32_t>			m_index32Out;
				size_t							m_32NumOut{ 0 };
			public:
				void		Init(size_t size) noexcept {
					this->m_vert32.resize(size * 4);
					this->m_index32.resize(size * 6);
					this->m_32Num = 0;
					this->m_32Size = size;
				}
				void		ResetNum(void) noexcept {
					this->m_32Num = 0;
				}
				void		AllocatePlane(void) noexcept {
					++this->m_32Num;
					if (this->m_32Num > this->m_32Size) {
						this->m_32Size = this->m_32Num;
						this->m_vert32.resize(this->m_32Size * 4);
						this->m_index32.resize(this->m_32Size * 6);
					}
					auto ZERO = (uint32_t)(this->m_32Num * 4 - 4);
					this->m_index32[this->m_32Num * 6 - 6] = ZERO;
					this->m_index32[this->m_32Num * 6 - 5] = ZERO + 1;
					this->m_index32[this->m_32Num * 6 - 4] = ZERO + 2;
					this->m_index32[this->m_32Num * 6 - 3] = ZERO + 3;
					this->m_index32[this->m_32Num * 6 - 2] = ZERO + 2;
					this->m_index32[this->m_32Num * 6 - 1] = ZERO + 1;
				}
				void		FlipVerts(void) noexcept {
					this->m_vert32Out = this->m_vert32;
					this->m_index32Out = this->m_index32;
					this->m_32NumOut = this->m_32Num;
				}
				void		Disable(void) noexcept {
					this->m_32NumOut = 0;
				}
				void		Draw(const GraphHandle& GrHandle) const noexcept {
					if (this->m_32NumOut > 0) {
						DrawPolygon32bitIndexed3D(this->m_vert32Out.data(), static_cast<int>(this->m_32NumOut * 4), this->m_index32Out.data(), static_cast<int>(this->m_32NumOut * 6 / 3), GrHandle.get(), TRUE);
					}
				}
			};
			//
			MV1								m_ObjSky;
			GraphHandle						m_tex{};
			GraphHandle						m_norm{};
			std::vector<int8_t>				m_CellBase{};
			std::array<CellsData, total>	m_CellxN;
			std::array<ThreadJobs, total + total + total>	m_Jobs;
			//
			int								BaseRate = 100;
			int								ShadowRate = 100;
			int								ThreadCounter = 0;
			//表示ポリゴンスレッド用
			std::array<vert32<VERTEX3D>, total>	m_vert32s;
			std::array<Vector3DX, total>	CamPos;
			std::array<Vector3DX, total>	CamVec;
			//影スレッド用
			std::array<vert32<VERTEX3D>, total>	m_vert32sSB;
			std::array<Vector3DX, total>	CamPosSB;
			std::array<Vector3DX, total>	light;

			std::array<vert32<VERTEX3DSHADER>, total>	m_vert32sS;
			std::array<Vector3DX, total>	CamPosS;
			std::array<Vector3DX, total>	CamVecS;
			//
#if defined(DEBUG) & EDITBLICK
			//Edit
			float							LenMouse = 2.f;
			int								xput = 5;
			int								yput = 2;
			int								zput = 5;
			int8_t							blicksel = -1;
			Vector3DX						PutPos;
#endif
		public:
			BackGroundClass(void) noexcept {}
			BackGroundClass(const BackGroundClass&) = delete;
			BackGroundClass(BackGroundClass&& o) = delete;
			BackGroundClass& operator=(const BackGroundClass&) = delete;
			BackGroundClass& operator=(BackGroundClass&& o) = delete;

			virtual ~BackGroundClass(void) noexcept {}
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
				const int l = std::abs(dx);
				const int y_inc = (dy < 0) ? -1 : 1;
				const int m = std::abs(dy);
				const int z_inc = (dz < 0) ? -1 : 1;
				const int n = std::abs(dz);
				const int dx2 = l << 1;
				const int dy2 = m << 1;
				const int dz2 = n << 1;

				if ((l >= m) && (l >= n)) {
					err_1 = dy2 - l;
					err_2 = dz2 - l;
					for (int i = 0; i < l; ++i) {
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
				else if ((m >= l) && (m >= n)) {
					err_1 = dx2 - m;
					err_2 = dz2 - m;
					for (int i = 0; i < m; ++i) {
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
					for (int i = 0; i < n; ++i) {
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
				float p[3]{}, d[3]{}, min[3]{}, max[3]{};
				p[0] = StartPos.x;
				p[1] = StartPos.y;
				p[2] = StartPos.z;
				d[0] = Vec.x;
				d[1] = Vec.y;
				d[2] = Vec.z;

				min[0] = AABBMinPos.x;
				min[1] = AABBMinPos.y;
				min[2] = AABBMinPos.z;

				max[0] = AABBMaxPos.x;
				max[1] = AABBMaxPos.y;
				max[2] = AABBMaxPos.z;

				float t = -FLT_MAX;
				float t_max = FLT_MAX;

				for (int i = 0; i < 3; ++i) {
					if (abs(d[i]) < FLT_EPSILON) {
						if (p[i] < min[i] || p[i] > max[i])
							return false; // 交差していない
					}
					else {
						// スラブとの距離を算出
						// t1が近スラブ、t2が遠スラブとの距離
						float odd = 1.0f / d[i];
						float t1 = (min[i] - p[i]) * odd;
						float t2 = (max[i] - p[i]) * odd;
						if (t1 > t2) {
							float tmp = t1;
							t1 = t2;
							t2 = tmp;
						}

						if (t1 > t) {
							t = t1;
							//どの向き？
							switch (i) {
							case 0:
								if (Normal) {
									*Normal = Vec.x > 0.f ? Vector3DX::left() : Vector3DX::right();
								}
								break;
							case 1:
								if (Normal) {
									*Normal = Vec.y > 0.f ? Vector3DX::down() : Vector3DX::up();
								}
								break;
							case 2:
								if (Normal) {
									*Normal = Vec.z > 0.f ? Vector3DX::back() : Vector3DX::forward();
								}
								break;
							default:
								break;
							}
						}
						if (t2 < t_max) {
							t_max = t2;
						}

						// スラブ交差チェック
						if (t >= t_max) {
							return false;
						}
					}
				}

				// 交差している
				if (EndPos) {
					*EndPos = StartPos + Vec * t;
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
			void			AddPlaneXPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddPlaneXMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddPlaneYPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddPlaneYMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddPlaneZPlus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept;
			void			AddPlaneZMinus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept;

			void			AddCubesX(size_t id) noexcept;
			void			AddCubesZ(size_t id) noexcept;
			//
			void			AddShadowPlaneXPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddShadowPlaneXMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddShadowPlaneYPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddShadowPlaneYMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddShadowPlaneZPlus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept;
			void			AddShadowPlaneZMinus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept;

			void			AddShadowCubesX(size_t id) noexcept;
			void			AddShadowCubesZ(size_t id) noexcept;
			//
			void			AddSetShadowPlaneXPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddSetShadowPlaneXMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddSetShadowPlaneYPlus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddSetShadowPlaneYMinus(size_t id, const CellsData& cellx, int x, int y, int zmin, int zmax) noexcept;
			void			AddSetShadowPlaneZPlus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept;
			void			AddSetShadowPlaneZMinus(size_t id, const CellsData& cellx, int xmin, int xmax, int y, int z) noexcept;

			void			AddSetShadowCubesX(size_t id) noexcept;
			void			AddSetShadowCubesZ(size_t id) noexcept;
		public:
			bool			CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) const noexcept;
			bool			CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept;

			void			LoadCellsFile() noexcept;
			void			SaveCellsFile() noexcept;

			void			LoadCellsClip(int xpos, int ypos,int zpos) noexcept;
			void			SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax) noexcept;

			void			SettingChange() noexcept;

#if defined(DEBUG) & EDITBLICK
			void			SetBlick(int x, int y, int z, int8_t select) noexcept;
#endif
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
