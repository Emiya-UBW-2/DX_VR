#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"


namespace FPS_n2 {
	namespace BackGround {
		static constexpr int8_t s_EmptyBlick = 0;
		static constexpr int TotalCellLayer = 4;
		static constexpr int MulPer = 2;
		static constexpr float CellScale = Scale3DRate / 2.f / 2.f;

		static constexpr int DrawMax = 70;//65

		static constexpr int DrawMaxXPlus = DrawMax;
		static constexpr int DrawMaxXMinus = -DrawMax;

		static constexpr int DrawMaxZPlus = DrawMax;
		static constexpr int DrawMaxZMinus = -DrawMax;

		static constexpr int DrawMaxYPlus = 20;
		static constexpr int DrawMaxYMinus = -30;


		static constexpr int DrawMinXPlus = DrawMaxXPlus / MulPer - 1;
		static constexpr int DrawMinXMinus = DrawMaxXMinus / MulPer;

		static constexpr int DrawMinZPlus = DrawMaxZPlus / MulPer - 1;
		static constexpr int DrawMinZMinus = DrawMaxZMinus / MulPer;

		static constexpr int DrawMinYPlus = DrawMaxYPlus / MulPer - 1;
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
				const auto& GetCell(void) const noexcept { return this->m_Cell; }
				bool IsEmpty(void) const noexcept { return GetCell() == s_EmptyBlick; }
				bool IsOcclusion(void) const noexcept { return this->m_FillInfo == 0b111111; }
				bool IsOcclusion(int id) const noexcept { return (this->m_FillInfo & (1 << id)) != 0; }
			public:
				void SetOcclusion(int id) noexcept { m_FillInfo |= (1 << id); }
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
				const bool		isFarCells(void) const noexcept { return this->m_scaleRate != 1; }
				//
				const int		GetIndex(int t) const noexcept { return (t % this->m_All + this->m_All) % this->m_All; }
				const size_t	GetCellNum(int xpos, int ypos, int zpos) const noexcept { return static_cast<size_t>(GetIndex(xpos) * this->m_AllPow2 + ypos * this->m_All + GetIndex(zpos)); }
				//
				const auto&		GetCellBuf(int xpos, int ypos, int zpos) const noexcept { return this->m_CellBuffer[GetCellNum(xpos, ypos, zpos)]; }
				auto&			SetCellBuf(int xpos, int ypos, int zpos) noexcept { return this->m_CellBuffer[GetCellNum(xpos, ypos, zpos)]; }
				//
				const auto		GetPosBuffer(int xpos, int ypos, int zpos, int ID) const noexcept { return GetPos(xpos + ((ID >> 2) & 1), ypos + ((ID >> 1) & 1), zpos + (ID & 1)); }
				//
				const int8_t	isFill(int xpos, int ypos, int zpos, int mul) const noexcept {
					mul /= this->m_scaleRate;
					int FillCount = 0;
					int FillAll = 0;

					int xMaxmin = xpos * mul + mul - 1;
					int yMaxmin = ypos * mul + mul - 1;
					int zMaxmin = zpos * mul + mul - 1;
					std::vector<int> IDCount;
					for (int xt = xMaxmin; xt < xMaxmin + mul; ++xt) {
						for (int yt = yMaxmin; yt < std::min(yMaxmin + mul, this->m_All); ++yt) {
							for (int zt = zMaxmin; zt < zMaxmin + mul; ++zt) {
								++FillAll;
								if (GetCellBuf(xt, yt, zt).IsEmpty()) { continue; }
								++FillCount;
								auto cell = GetCellBuf(xt, yt, zt).GetCell() + 1;
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
				const bool		isInside(int ypos) const noexcept { return ((0 <= ypos) && (ypos < this->m_All)); }
				const Vector3DX	GetPos(int xpos, int ypos, int zpos) const noexcept {
					return Vector3DX::vget(static_cast<float>(xpos - this->m_Half), static_cast<float>(ypos - this->m_Half), static_cast<float>(zpos - this->m_Half)) * this->m_Scale;
				}
				const Vector3Int	GetPoint(const Vector3DX& pos) const noexcept {
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
				void			CalcOcclusion(int xpos, int ypos, int zpos) noexcept {
					SetCellBuf(xpos, ypos, zpos).m_FillInfo = 0;
					if (!GetCellBuf(xpos + 1, ypos, zpos).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(0); }

					if (!GetCellBuf(xpos - 1, ypos, zpos).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(1); }
					if ((ypos == this->m_All - 1) ? true : !GetCellBuf(xpos, ypos + 1, zpos).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(2); }
					if ((ypos == 0) ? true : !GetCellBuf(xpos, ypos - 1, zpos).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(3); }
					if (!GetCellBuf(xpos, ypos, zpos + 1).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(4); }
					if (!GetCellBuf(xpos, ypos, zpos - 1).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(5); }
				}
			};
			//
			struct ThreadJobs {
				std::thread						m_Job;
				bool							m_JobEnd{};
				bool							m_IsDoOnce{};
				bool							m_isEnd{};//
				bool							m_IsDoEnd{};
#if defined(DEBUG) && CHECKTHREADTIME
				LONGLONG						m_StartTime{};
				LONGLONG						m_TotalTime{};
#endif
				std::function<void()>			m_Doing{ nullptr };
				std::function<void()>			m_EndDoing{ nullptr };
			public:
				void JobStart(void) noexcept {
					if (this->m_isEnd) {
						this->m_JobEnd = true;
						this->m_isEnd = false;
						this->m_IsDoEnd = false;
					}
				}
				void Init(std::function<void()> Doing, std::function<void()> EndDoing, bool IsDoOnce) noexcept {
					this->m_IsDoOnce = IsDoOnce;
					this->m_Doing = Doing;
					this->m_EndDoing = EndDoing;
					this->m_isEnd = true;
					JobStart();
					this->m_IsDoEnd = true;
				}

				void Execute(void) noexcept {
					if (this->m_JobEnd) {
						this->m_JobEnd = false;
#if defined(DEBUG) && CHECKTHREADTIME
						this->m_TotalTime = GetNowHiPerformanceCount() - this->m_StartTime;
						this->m_StartTime = GetNowHiPerformanceCount();
#endif
						if (this->m_IsDoEnd) {
							this->m_IsDoEnd = false;
							if (this->m_EndDoing) {
								this->m_EndDoing();
							}
						}
						//
						if (!this->m_IsDoOnce) {
							this->m_isEnd = false;
						}
						if(!this->m_isEnd) {
							if (this->m_Job.joinable()) {
								this->m_Job.detach();
							}
							std::thread tmp([&]() {
								if (this->m_Doing) {
									this->m_Doing();
								}
								this->m_JobEnd = true;
								this->m_isEnd = true;
								this->m_IsDoEnd = true;
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
			class vert32 {
				int												m_Now{ 0 };

				size_t											m_32Size{ 0 };
				std::array<std::vector<VERTEX3D>, 2>			m_vert32;
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
						DrawPolygon32bitIndexed3DToShader2(GetOutVert().data(), static_cast<int>(GetOutNum() * 4), GetOutindex().data(), static_cast<int>(GetOutNum() * 6 / 3));
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
			std::array<ThreadJobs, TotalCellLayer + TotalCellLayer>	m_Jobs;
			//
			int								m_BaseRate = 100;
			int								m_ShadowRate = 100;
			int								m_ThreadCounter = 0;
			struct Draw {
				vert32				m_vert32;
				Vector3DX			m_CamPos;
				Vector3DX			m_CamVec;
				Vector3DX			m_light;
			};
			//表示ポリゴンスレッド用
			std::array<Draw, TotalCellLayer>	m_Draws;
			//影スレッド用
			std::array<Draw, TotalCellLayer>	m_DrawsSB;
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
			inline static bool		CalcYZActive(const Vector3DX& Vec, int xpos, int yMaxmin, int yMaxmax, int zMaxmin, int zMaxmax) noexcept {
				//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xpos) + 0.5f, static_cast<float>(yMaxmin) + 0.5f, static_cast<float>(zMaxmin) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xpos) + 0.5f, static_cast<float>(yMaxmax) + 0.5f, static_cast<float>(zMaxmax) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xpos) + 0.5f, static_cast<float>(yMaxmin) + 0.5f, static_cast<float>(zMaxmax) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xpos) + 0.5f, static_cast<float>(yMaxmax) + 0.5f, static_cast<float>(zMaxmin) + 0.5f)) > 0.f) { return true; }
				return false;
			}
			inline static bool		CalcZMinMax(const Vector3DX& Vec, int xpos, int ypos, int* zMaxmin, int* zMaxmax) noexcept {
				Vector3DX Vec1 = Vector3DX::vget(static_cast<float>(xpos) + 0.5f, static_cast<float>(ypos) + 0.5f, static_cast<float>(*zMaxmin) + 0.5f);
				Vector3DX Vec2 = Vector3DX::vget(static_cast<float>(xpos) + 0.5f, static_cast<float>(ypos) + 0.5f, static_cast<float>(*zMaxmax) + 0.5f);

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
			inline static bool		CalcXYActive(const Vector3DX& Vec, int xMaxmin, int xMaxmax, int yMaxmin, int yMaxmax, int zpos) noexcept {
				//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xMaxmin) + 0.5f, static_cast<float>(yMaxmin) + 0.5f, static_cast<float>(zpos) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xMaxmax) + 0.5f, static_cast<float>(yMaxmax) + 0.5f, static_cast<float>(zpos) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xMaxmax) + 0.5f, static_cast<float>(yMaxmin) + 0.5f, static_cast<float>(zpos) + 0.5f)) > 0.f) { return true; }
				if (Vector3DX::Dot(Vec, Vector3DX::vget(static_cast<float>(xMaxmin) + 0.5f, static_cast<float>(yMaxmax) + 0.5f, static_cast<float>(zpos) + 0.5f)) > 0.f) { return true; }
				return false;
			}
			inline static bool		CalcXMinMax(const Vector3DX& Vec, int* xMaxmin, int* xMaxmax, int ypos, int zpos) noexcept {
				Vector3DX Vec1 = Vector3DX::vget(static_cast<float>(*xMaxmin) + 0.5f, static_cast<float>(ypos) + 0.5f, static_cast<float>(zpos) + 0.5f);
				Vector3DX Vec2 = Vector3DX::vget(static_cast<float>(*xMaxmax) + 0.5f, static_cast<float>(ypos) + 0.5f, static_cast<float>(zpos) + 0.5f);

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
			bool			AddCubeX_CanAddPlane(const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int cy, int cz, int id) noexcept;
			bool			AddCubeZ_CanAddPlane(const CellsData& cellx, const Vector3Int& center, int cx, int cy, int zmin, int zmax, int id) noexcept;
			//
			void			AddPlaneXPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneXMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneYPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneYMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneZPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos, bool IsCalcUV) noexcept;
			void			AddPlaneZMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos, bool IsCalcUV) noexcept;
			//視界から見て映るものだけをテクスチャ関係込みで更新
			void			AddCubes(size_t id) noexcept;
			//ライトから見て映るものだけを更新
			void			AddShadowCubes(size_t id) noexcept;
		public:
			bool			CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) const noexcept;
			bool			CheckLinetoMap(const Vector3DX& StartPos, const Vector3DX& EndPos) const noexcept {
				Vector3DX pEndPos = EndPos;
				return CheckLinetoMap(StartPos, &pEndPos);
			}
			bool			CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept;

			void			LoadCellsFile(void) noexcept;
			void			SaveCellsFile(void) noexcept;

			void			LoadCellsClip(int xbasepos, int ybasepos,int zbasepos) noexcept;
			void			SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax) noexcept;

			void			SettingChange(void) noexcept;

			void			SetBlick(int xpos, int ypos, int zpos, int8_t select) noexcept;
			const Vector3Int GetPoint(const Vector3DX& pos) const noexcept { return this->m_CellxN.front().GetPoint(pos); }
			const Vector3DX GetPos(int xpos, int ypos, int zpos) const noexcept { return this->m_CellxN.front().GetPos(xpos, ypos, zpos); }

			const auto& GetCellBuf(int xpos, int ypos, int zpos) const noexcept {
				auto& cell = this->m_CellxN.front();
				//if (!cell.isInside(ypos)) { return; }
				return cell.GetCellBuf(xpos, ypos, zpos);
			}
		public://
			void			Load(void) noexcept;
			//
			void			Init(void) noexcept;
			//
			void			UpdateOnce(void) noexcept;

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
