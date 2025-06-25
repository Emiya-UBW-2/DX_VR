#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"


namespace FPS_n2 {
	namespace BackGround {
		static constexpr int8_t s_EmptyBlick = 0;
		static constexpr int TotalCellLayer = 4;
		static constexpr int MulPer = 2;
		static constexpr float CellScale = Scale3DRate / 2.0f / 2.0f;

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
				int8_t					Life{};
				int8_t					Cell{};
				int8_t					FillInfo{};//周りの遮蔽データのbitフラグ
			public:
				const auto& GetCell(void) const noexcept { return this->Cell; }
				bool IsEmpty(void) const noexcept { return GetCell() == s_EmptyBlick; }
				bool IsOcclusion(void) const noexcept { return this->FillInfo == 0b111111; }
				bool IsOcclusion(int id) const noexcept { return (this->FillInfo & (1 << id)) != 0; }

				bool CanDraw(void) const noexcept { return !IsEmpty() && !IsOcclusion(); }
			public:
				void ResetOcclusion(void) noexcept { FillInfo = 0; }
				void SetOcclusion(int id) noexcept { FillInfo |= (1 << id); }
			};
			struct CellsData {
				std::vector<CellBuffer>	CellBuffer;
				int						ScaleInt = 0;
				//算術補助系
				int						ScaleRate = static_cast<int>(std::pow(MulPer, this->ScaleInt));
				int						All = 256 / this->ScaleRate;
				int						Half = this->All / 2;
				int						AllPow2 = this->All * this->All;
				float					Scale = (CellScale * this->ScaleRate);
			public:
				const bool		isFarCells(void) const noexcept { return this->ScaleInt != 0; }
				//
				const int		GetIndex(int pos) const noexcept { return (pos % this->All + this->All) % this->All; }
				const size_t	GetCellNum(int xpos, int ypos, int zpos) const noexcept { return static_cast<size_t>(GetIndex(xpos) * this->AllPow2 + ypos * this->All + GetIndex(zpos)); }
				//
				const auto&		GetCellBuf(int xpos, int ypos, int zpos) const noexcept { return this->CellBuffer[GetCellNum(xpos, ypos, zpos)]; }
				auto&			SetCellBuf(int xpos, int ypos, int zpos) noexcept { return this->CellBuffer[GetCellNum(xpos, ypos, zpos)]; }
				//
				const auto		GetPosBuffer(int xpos, int ypos, int zpos, int ID) const noexcept { return GetPos(xpos + ((ID >> 2) & 1), ypos + ((ID >> 1) & 1), zpos + (ID & 1)); }
				//
				const int8_t	isFill(int xpos, int ypos, int zpos, int mul) const noexcept {
					mul /= this->ScaleRate;
					int FillCount = 0;
					int FillAll = 0;

					int xMaxmin = xpos * mul + mul - 1;
					int yMaxmin = ypos * mul + mul - 1;
					int zMaxmin = zpos * mul + mul - 1;
					std::vector<int> IDCount;
					for (int xt = xMaxmin; xt < xMaxmin + mul; ++xt) {
						for (int yt = yMaxmin; yt < std::min(yMaxmin + mul, this->All); ++yt) {
							for (int zt = zMaxmin; zt < zMaxmin + mul; ++zt) {
								++FillAll;
								if (GetCellBuf(xt, yt, zt).IsEmpty()) { continue; }
								++FillCount;
								auto cell = static_cast<size_t>(GetCellBuf(xt, yt, zt).GetCell() + 1);
								if (cell > IDCount.size()) {
									IDCount.resize(cell);
								}
								++IDCount[cell - 1];
							}
						}
					}
					if ((FillAll != 0) && (static_cast<float>(FillCount) / FillAll >= (1.0f / 4.0f))) {
						int max = -1;
						int8_t id = 1;
						for (auto& idc : IDCount) {
							if (max < idc) {
								max = idc;
								id = static_cast<int8_t>(&idc - &IDCount.front());
							}
						}
						return id;
					}
					else {
						return s_EmptyBlick;
					}
				}
				const bool		isInside(int ypos) const noexcept { return ((0 <= ypos) && (ypos < this->All)); }
				const Vector3DX	GetPos(int xpos, int ypos, int zpos) const noexcept {
					return Vector3DX::vget(static_cast<float>(xpos - this->Half), static_cast<float>(ypos - this->Half), static_cast<float>(zpos - this->Half)) * this->Scale;
				}
				const Vector3Int	GetPoint(const Vector3DX& pos) const noexcept {
					Vector3DX Start = pos / this->Scale;
					return Vector3Int(static_cast<int>(Start.x) + this->Half, static_cast<int>(Start.y) + this->Half, static_cast<int>(Start.z) + this->Half);
				}

				//
				void			SetScale(int scale) noexcept {
					this->ScaleInt = scale;
					this->ScaleRate = static_cast<int>(std::pow(MulPer, this->ScaleInt));
					this->All = 256 / this->ScaleRate;
					//算術補助系
					this->Half = this->All / 2;
					this->AllPow2 = this->All * this->All;
					this->Scale = (CellScale * this->ScaleRate);
					this->CellBuffer.resize(static_cast<size_t>(this->All * this->All * this->All));
				}
				//
				void			CalcOcclusion(int xpos, int ypos, int zpos) noexcept {
					if (!isInside(ypos)) { return; }
					if (GetCellBuf(xpos, ypos, zpos).IsEmpty()) { return; }

					SetCellBuf(xpos, ypos, zpos).ResetOcclusion();
					if (!GetCellBuf(xpos + 1, ypos, zpos).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(0); }

					if (!GetCellBuf(xpos - 1, ypos, zpos).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(1); }
					if ((ypos == this->All - 1) ? true : !GetCellBuf(xpos, ypos + 1, zpos).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(2); }
					if ((ypos == 0) ? true : !GetCellBuf(xpos, ypos - 1, zpos).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(3); }
					if (!GetCellBuf(xpos, ypos, zpos + 1).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(4); }
					if (!GetCellBuf(xpos, ypos, zpos - 1).IsEmpty()) { SetCellBuf(xpos, ypos, zpos).SetOcclusion(5); }
				}
			};
			//
			class ThreadJobs {
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

				void UpdateDisable(void) noexcept {
					this->m_isEnd = false;
				}

				void Update(void) noexcept {
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
					printfDx("%5.2fms \n", static_cast<float>(this->m_TotalTime) / 1000.0f);
#endif
				}

				void Dispose(void) noexcept {
					if (this->m_Job.joinable()) {
						this->m_Job.join();
						//this->m_Job.detach();
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
				const auto& GetInNum(void) const noexcept { return this->m_32Num[this->m_Now]; }
				auto& SetInVert(void) noexcept { return this->m_vert32[this->m_Now]; }
			public:
				const auto& GetOutNum(void) const noexcept { return this->m_32Num[static_cast<size_t>(1 - this->m_Now)]; }
				const auto& GetOutVert(void) const noexcept { return this->m_vert32[static_cast<size_t>(1 - this->m_Now)]; }
				const auto& GetOutindex(void) const noexcept { return this->m_index32[static_cast<size_t>(1 - this->m_Now)]; }
			public:
				void		Init(size_t size) noexcept {
					for (int loop = 0; loop < 2; ++loop) {
						this->m_vert32[loop].resize(size * 4);
						this->m_index32[loop].resize(size * 6);
						this->m_32Num[loop] = 0;
					}
					this->m_Now = 0;
					this->m_32Size = size;
				}
				void		Dispose(void) noexcept {
					for (int loop = 0; loop < 2; ++loop) {
						this->m_vert32[loop].clear();
						this->m_index32[loop].clear();
						this->m_32Num[loop] = 0;
					}
					this->m_32Size = 0;
				}
				void		ResetNum(void) noexcept {
					this->m_32Num[this->m_Now] = 0;
				}
				void		AllocatePlane(void) noexcept {
					++this->m_32Num[this->m_Now];
					if (GetInNum() > this->m_32Size) {
						this->m_32Size = GetInNum();
						for (int loop = 0; loop < 2; ++loop) {
							this->m_vert32[loop].resize(this->m_32Size * 4);
							this->m_index32[loop].resize(this->m_32Size * 6);
						}
					}
					auto ZERO = static_cast<uint32_t>(GetInNum() * 4 - 4);
					this->m_index32[this->m_Now][GetInNum() * 6 - 6] = ZERO;
					this->m_index32[this->m_Now][GetInNum() * 6 - 5] = ZERO + 1;
					this->m_index32[this->m_Now][GetInNum() * 6 - 4] = ZERO + 2;
					this->m_index32[this->m_Now][GetInNum() * 6 - 3] = ZERO + 3;
					this->m_index32[this->m_Now][GetInNum() * 6 - 2] = ZERO + 2;
					this->m_index32[this->m_Now][GetInNum() * 6 - 1] = ZERO + 1;
				}
				void		FlipVerts(void) noexcept {
					this->m_Now = 1 - this->m_Now;
				}
				void		Disable(void) noexcept {
					for (int loop = 0; loop < 2; ++loop) {
						this->m_32Num[loop] = 0;
					}
				}
				void		DrawByShader(void) const noexcept {
					if (GetOutNum() == 0) { return; }
					DrawPolygon32bitIndexed3DToShader2(GetOutVert().data(), static_cast<int>(GetOutNum() * 4), GetOutindex().data(), static_cast<int>(GetOutNum() * 6 / 3));
				}
				void		Draw(const GraphHandle& GrHandle) const noexcept {
					if (GetOutNum() == 0) { return; }
					DrawPolygon32bitIndexed3D(GetOutVert().data(), static_cast<int>(GetOutNum() * 4), GetOutindex().data(), static_cast<int>(GetOutNum() * 6 / 3), GrHandle.get(), true);
				}
			};
			//
			struct Draw {
				vert32				Vert32;
				Vector3DX			CamPos;
				Vector3DX			CamVec;
			};
		private:
			MV1								m_ObjSky;
			GraphHandle						m_tex{};
			std::vector<int8_t>				m_CellBase{};
			std::array<CellsData, TotalCellLayer>	m_CellxN;
			std::array<ThreadJobs, TotalCellLayer + TotalCellLayer>	m_Jobs;
			//
			int								m_BaseRate = 1;
			int								m_ShadowRate = 1;
			int								m_ThreadCounter = 0;
			//0~TotalCellLayer-1 : 表示ポリゴンスレッド用
			//TotalCellLayer~:影スレッド用
			std::array<Draw, TotalCellLayer + TotalCellLayer>	m_Draws;
			bool							m_isChangeBlock{ false };
		private:
			BackGroundControl(void) noexcept { Load(); }
			BackGroundControl(const BackGroundControl&) = delete;
			BackGroundControl(BackGroundControl&&) = delete;
			BackGroundControl& operator=(const BackGroundControl&) = delete;
			BackGroundControl& operator=(BackGroundControl&&) = delete;

			virtual ~BackGroundControl(void) noexcept { Dispose_Load(); }
		private:
			//
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
				const int dyAbs = std::abs(dy);
				const int z_inc = (dz < 0) ? -1 : 1;
				const int dzAbs = std::abs(dz);
				const int dx2 = Length << 1;
				const int dy2 = dyAbs << 1;
				const int dz2 = dzAbs << 1;

				if ((Length >= dyAbs) && (Length >= dzAbs)) {
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
				else if ((dyAbs >= Length) && (dyAbs >= dzAbs)) {
					err_1 = dx2 - dyAbs;
					err_2 = dz2 - dyAbs;
					for (int loop = 0; loop < dyAbs; ++loop) {
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
					err_1 = dy2 - dzAbs;
					err_2 = dx2 - dzAbs;
					for (int loop = 0; loop < dzAbs; ++loop) {
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
			//各方向に向いているポリゴンの追加
			void			AddPlaneXPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneXMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneYPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneYMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept;
			void			AddPlaneZPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos, bool IsCalcUV) noexcept;
			void			AddPlaneZMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos, bool IsCalcUV) noexcept;
			//XZ方向に走査してポリゴンをなるべく少ないポリゴン数で表示する
			void			AddPlanesXY(vert32* pTarget, float camVecX, float camVecY, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zMaxminT, int zMaxmaxT, bool CheckInsideXY, bool CheckFillXY, bool IsCalcUV) {
				int zmin = 0;
				int zmax = 0;
				bool CheckFillXYZ = false;
				bool isHitmin = true;
				int8_t selectBlock = s_EmptyBlick;
				bool CanDrawXPlus = false;
				bool CanDrawXMinus = false;
				bool CanDrawYPlus = false;
				bool CanDrawYMinus = false;
				for (int zpos = zMaxminT; zpos <= zMaxmaxT; ++zpos) {
					auto& CellBuff = cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zpos);
					bool CheckInsideXYZ = CheckInsideXY && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus));
					bool CheckBlockID = IsCalcUV && (selectBlock != CellBuff.GetCell());
					if (
						(zpos == zMaxmaxT)
						|| CheckInsideXYZ
						|| (!isHitmin && CheckBlockID)
						|| !CellBuff.CanDraw()
						) {
						//置けない部分なので今まで置けていた分をまとめてポリゴン化
						if (!isHitmin) {
							isHitmin = true;
							if (camVecX < 0.0f) {
								if (CheckFillXYZ || CanDrawXPlus) {
									AddPlaneXPlus(pTarget, cellx, center, xpos, ypos, zmin, zmax, IsCalcUV);
								}
							}
							else {
								if (CheckFillXYZ || CanDrawXMinus) {
									AddPlaneXMinus(pTarget, cellx, center, xpos, ypos, zmin, zmax, IsCalcUV);
								}
							}
							if (camVecY < 0.0f) {
								if (CheckFillXYZ || CanDrawYPlus) {
									AddPlaneYPlus(pTarget, cellx, center, xpos, ypos, zmin, zmax, IsCalcUV);
								}
							}
							else {
								if (CheckFillXYZ || CanDrawYMinus) {
									AddPlaneYMinus(pTarget, cellx, center, xpos, ypos, zmin, zmax, IsCalcUV);
								}
							}
							//この場合だけもう一回判定させるドン
							if (CheckBlockID) {
								--zpos;
							}
						}
					}
					else {
						//ブロックが置ける部分
						if (isHitmin) {
							isHitmin = false;
							zmin = zpos;
							selectBlock = CellBuff.GetCell();
							CanDrawXPlus = false;
							CanDrawXMinus = false;
							CanDrawYPlus = false;
							CanDrawYMinus = false;
						}
						zmax = zpos;
						CheckFillXYZ = CheckFillXY && (zmin <= DrawMinZPlus && DrawMinZMinus <= zmax);
						if (!CanDrawXPlus) {
							if (!CellBuff.IsOcclusion(0)) {
								CanDrawXPlus = true;
							}
						}
						if (!CanDrawXMinus) {
							if (!CellBuff.IsOcclusion(1)) {
								CanDrawXMinus = true;
							}
						}
						if (!CanDrawYPlus) {
							if (!CellBuff.IsOcclusion(2)) {
								CanDrawYPlus = true;
							}
						}
						if (!CanDrawYMinus) {
							if (!CellBuff.IsOcclusion(3)) {
								CanDrawYMinus = true;
							}
						}
					}
				}
			}
			void			AddPlanesZ(vert32* pTarget, float camVecZ, const CellsData& cellx, const Vector3Int& center, int xMaxminT, int xMaxmaxT, int ypos, int zpos, bool CheckInsideYZ, bool CheckFillYZ, bool IsCalcUV) {
				int xmin = 0;
				int xmax = 0;
				bool CheckFillXYZ = false;
				bool isHitmin = true;
				int8_t selectBlock = s_EmptyBlick;
				bool CanDrawZPlus = false;
				bool CanDrawZMinus = false;
				for (int xpos = xMaxminT; xpos <= xMaxmaxT; ++xpos) {
					auto& CellBuff = cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zpos);
					bool CheckInsideXYZ = CheckInsideYZ && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus));
					bool CheckBlockID = IsCalcUV && (selectBlock != CellBuff.GetCell());
					if (
						(xpos == xMaxmaxT)
						|| CheckInsideXYZ
						|| (!isHitmin && CheckBlockID)
						|| !CellBuff.CanDraw()
						) {
						//置けない部分なので今まで置けていた分をまとめてポリゴン化
						if (!isHitmin) {
							isHitmin = true;
							if (camVecZ < 0) {
								if (CheckFillXYZ || CanDrawZPlus) {
									AddPlaneZPlus(pTarget, cellx, center, xmin, xmax, ypos, zpos, IsCalcUV);
								}
							}
							else {
								if (CheckFillXYZ || CanDrawZMinus) {
									AddPlaneZMinus(pTarget, cellx, center, xmin, xmax, ypos, zpos, IsCalcUV);
								}
							}
							//この場合だけもう一回判定させるドン
							if (CheckBlockID) {
								--xpos;
							}
						}
					}
					else {
						//ブロックが置ける部分
						if (isHitmin) {
							isHitmin = false;
							xmin = xpos;
							selectBlock = CellBuff.GetCell();
							CanDrawZPlus = false;
							CanDrawZMinus = false;
						}
						xmax = xpos;
						CheckFillXYZ = CheckFillYZ && (xmin <= DrawMinXPlus && DrawMinXMinus <= xmax);
						if (!CanDrawZPlus) {
							if (!CellBuff.IsOcclusion(4)) {
								CanDrawZPlus = true;
							}
						}
						if (!CanDrawZMinus) {
							if (!CellBuff.IsOcclusion(5)) {
								CanDrawZMinus = true;
							}
						}
					}
				}
			}
			//視界から見て映るものだけをテクスチャ関係込みで更新
			void			AddCubes(size_t id) noexcept;
			void			FlipCubes(size_t id) noexcept;
			//ライトから見て映るものだけを更新
			void			AddShadowCubes(size_t id) noexcept;
			void			FlipShadowCubes(size_t id) noexcept;
			//
			const auto&		GetReferenceCells(void) const noexcept { return this->m_CellxN[0]; }
			auto&			SetReferenceCells(void) noexcept { return this->m_CellxN[0]; }
		public:
			class Builds {
				int						m_mesh{ -1 };
				Vector3DX				m_MinPos;
				Vector3DX				m_Pos;
				Vector3DX				m_MaxPos;

				int							MyIndex{ 0 };
				int							LinkPolyIndex[4]{ -1,-1,-1,-1 };
			public:
				const int	GetIndex() const noexcept { return MyIndex; }
				const int	GetLinkPolyIndex(int ID) const noexcept { return LinkPolyIndex[ID]; }
				void		SetLink(int tris, int index) { LinkPolyIndex[tris] = index; }
			public:
				const auto& GetMinPos(void) const noexcept { return m_MinPos; }
				const auto& GetPos(void) const noexcept { return m_Pos; }
				const auto& GetMaxPos(void) const noexcept { return m_MaxPos; }
			public:
				void		Set(int index) {
					this->MyIndex = index;
					for (int i = 0; i < 4; i++) {
						LinkPolyIndex[i] = -1;
					}
				}
				void		SetPosition(Vector3DX MinPos, Vector3DX MaxPos) {
					this->m_MinPos = MinPos;
					this->m_MaxPos = MaxPos;
					this->m_Pos = (MinPos + MaxPos) / 2;
				}
			};
			std::vector<Builds>			m_ObjBuilds;
		public:
			const auto&		GetBuildData(void) noexcept { return this->m_ObjBuilds; }

			const int		GetNearestBuilds(const Vector3DX& NowPosition) const noexcept {
				for (auto& bu : this->m_ObjBuilds) {
					if (
						(bu.GetMaxPos().x >= NowPosition.x && NowPosition.x >= bu.GetMinPos().x) &&
						(bu.GetMaxPos().z >= NowPosition.z && NowPosition.z >= bu.GetMinPos().z)
						) {
						return static_cast<int>(&bu - &this->m_ObjBuilds.front());
					}
				}
				return -1;
			}
			const int		GetNearestBuilds2(const Vector3DX& NowPosition) const noexcept {
				float Len = 1000000.f;
				int Answer = -1;
				for (auto& bu : this->m_ObjBuilds) {
					Vector3DX Length = bu.GetPos() - NowPosition;
					Length.y = 0.f;
					if (Len >= Length.sqrMagnitude()) {
						Len = Length.sqrMagnitude();
						Answer = static_cast<int>(&bu - &this->m_ObjBuilds.front());
					}
				}
				return Answer;
			}
			// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )
			bool CheckPolyMove(Vector3DX StartPos, Vector3DX TargetPos) const {
				int Rate = 6;

				int CheckPoly[4]{};
				int CheckPolyPrev[4]{};
				int NextCheckPoly[4]{};
				int NextCheckPolyPrev[4]{};

				// 開始座標と目標座標の y座標値を 0.0f にして、平面上の判定にする
				StartPos.y = (0.0f);
				TargetPos.y = (0.0f);

				// 開始座標と目標座標の直上、若しくは直下に存在するポリゴンを検索する
				int StartPoly = this->GetNearestBuilds(StartPos);
				int TargetPoly = this->GetNearestBuilds(TargetPos);

				// ポリゴンが存在しなかったら移動できないので false を返す
				if (StartPoly == -1 || TargetPoly == -1) { return false; }

				// 指定線分上にあるかどうかをチェックするポリゴンとして開始座標の直上、若しくは直下に存在するポリゴンを登録
				int CheckPolyNum = 1;
				CheckPoly[0] = StartPoly;
				int CheckPolyPrevNum = 0;
				CheckPolyPrev[0] = -1;

				// 結果が出るまで無条件で繰り返し
				while (true) {
					int NextCheckPolyNum = 0;			// 次のループでチェック対象になるポリゴンの数をリセットしておく
					int NextCheckPolyPrevNum = 0;			// 次のループでチェック対象から外すポリゴンの数をリセットしておく
					// チェック対象のポリゴンの数だけ繰り返し
					for (int i = 0; i < CheckPolyNum; i++) {
						int Index = CheckPoly[i];
						// チェック対象のポリゴンの３座標を取得 y座標を0.0にして、平面的な判定を行うようにする
						Vector3DX Pos = this->m_ObjBuilds.at(Index).GetPos(); Pos.y = (0.f);

						for (int K = 0; K < 4; K++) {
							int LinkIndex = this->m_ObjBuilds.at(Index).GetLinkPolyIndex(K);

							;

							Vector3DX PolyPos = Pos;
							PolyPos.x += ((Rate / 2.f) * ((K == 0 || K == 1) ? 1.f : -1.f));
							PolyPos.z += ((Rate / 2.f) * ((K == 1 || K == 2) ? 1.f : -1.f));
							int K2 = (K + 1) % 4;
							Vector3DX PolyPos2 = Pos;
							PolyPos2.x += ((Rate / 2.f) * ((K2 == 0 || K2 == 1) ? 1.f : -1.f));
							PolyPos2.z += ((Rate / 2.f) * ((K2 == 1 || K2 == 2) ? 1.f : -1.f));
							// ポリゴンの頂点番号0と1の辺に隣接するポリゴンが存在する場合で、
							// 且つ辺の線分と移動開始点、終了点で形成する線分が接していたら if 文が真になる
							if (LinkIndex != -1 && GetMinLenSegmentToSegment(StartPos, TargetPos, PolyPos, PolyPos2) < 0.01f) {
								// もし辺と接しているポリゴンが目標座標上に存在するポリゴンだったら 開始座標から目標座標上まで途切れなくポリゴンが存在するということなので true を返す
								if (LinkIndex == TargetPoly) { return true; }

								// 辺と接しているポリゴンを次のチェック対象のポリゴンに加える

								// 既に登録されているポリゴンの場合は加えない
								int j = 0;
								for (j = 0; j < NextCheckPolyNum; j++) {
									if (NextCheckPoly[j] == LinkIndex) { break; }
								}
								if (j == NextCheckPolyNum) {
									// 次のループで除外するポリゴンの対象に加える

									// 既に登録されている除外ポリゴンの場合は加えない
									int j2 = 0;
									for (j2 = 0; j2 < NextCheckPolyPrevNum; j2++) {
										if (NextCheckPolyPrev[j2] == Index) { break; }
									}
									if (j2 == NextCheckPolyPrevNum) {
										NextCheckPolyPrev[NextCheckPolyPrevNum] = Index;
										NextCheckPolyPrevNum++;
									}

									// 一つ前のループでチェック対象になったポリゴンの場合も加えない
									int j3 = 0;
									for (j3 = 0; j3 < CheckPolyPrevNum; j3++) {
										if (CheckPolyPrev[j3] == LinkIndex) { break; }
									}
									if (j3 == CheckPolyPrevNum) {
										// ここまで来たら漸く次のチェック対象のポリゴンに加える
										NextCheckPoly[NextCheckPolyNum] = LinkIndex;
										NextCheckPolyNum++;
									}
								}
							}
						}
					}

					// 次のループでチェック対象になるポリゴンが一つもなかったということは
					// 移動開始点、終了点で形成する線分と接するチェック対象のポリゴンに隣接する
					// ポリゴンが一つもなかったということなので、直線的な移動はできないということで false を返す
					if (NextCheckPolyNum == 0) { return false; }

					// 次にチェック対象となるポリゴンの情報をコピーする
					for (int i = 0; i < NextCheckPolyNum; i++) {
						CheckPoly[i] = NextCheckPoly[i];
					}
					CheckPolyNum = NextCheckPolyNum;

					// 次にチェック対象外となるポリゴンの情報をコピーする
					for (int i = 0; i < NextCheckPolyPrevNum; i++) {
						CheckPolyPrev[i] = NextCheckPolyPrev[i];
					}
					CheckPolyPrevNum = NextCheckPolyPrevNum;
				}
			}
			bool CheckPolyMoveWidth(Vector3DX StartPos, int TargetIndex, float Width) const {
				// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )( 幅指定版 )
				Vector3DX TargetPos = this->m_ObjBuilds.at(TargetIndex).GetPos();
				// 最初に開始座標から目標座標に直線的に移動できるかどうかをチェック
				if (CheckPolyMove(StartPos, TargetPos) == false) { return false; }

				Vector3DX Direction = TargetPos - StartPos;		// 開始座標から目標座標に向かうベクトルを算出
				Direction.y = (0.0f);		// y座標を 0.0f にして平面的なベクトルにする

				// 開始座標から目標座標に向かうベクトルに直角な正規化ベクトルを算出
				Vector3DX SideDirection = Vector3DX::Cross(Direction, Vector3DX::up()).normalized();
				{
					// 開始座標と目標座標を Width / 2.0f 分だけ垂直方向にずらして、再度直線的に移動できるかどうかをチェック
					Vector3DX TempVec = SideDirection * (Width / 2.0f);
					if (CheckPolyMove(StartPos + TempVec, TargetPos + TempVec) == false) {
						return false;
					}
				}
				{
					// 開始座標と目標座標を Width / 2.0f 分だけ一つ前とは逆方向の垂直方向にずらして、再度直線的に移動できるかどうかをチェック
					Vector3DX TempVec = SideDirection * (-Width / 2.0f);
					if (CheckPolyMove(StartPos + TempVec, TargetPos + TempVec) == false) {
						return false;
					}
				}
				return true;		// ここまできたら指定の幅があっても直線的に移動できるということなので true を返す
			}
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
			const Vector3Int GetPoint(const Vector3DX& pos) const noexcept { return GetReferenceCells().GetPoint(pos); }
			const Vector3DX GetPos(int xpos, int ypos, int zpos) const noexcept { return GetReferenceCells().GetPos(xpos, ypos, zpos); }
			//ブロックにダメージを与える
			bool			DamageCell(int xpos, int ypos, int zpos, int8_t Damage) noexcept {
				auto& cell = GetReferenceCells().GetCellBuf(xpos, ypos, zpos);
				if (cell.IsEmpty()) { return false; }
				if (cell.GetCell() == 1) { return false; }
				if (cell.GetCell() == 4) { return false; }
				SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Life -= Damage;
				if (cell.Life <= 0) {
					SetBlick(xpos, ypos, zpos, BackGround::s_EmptyBlick);
					m_isChangeBlock = true;
					return true;
				}
				else if (cell.Life <= 50) {
					if (cell.GetCell() == 2) {
						SetBlick(xpos, ypos, zpos, 3);
					}
				}
				return false;
			}
		public://
			void			Load(void) noexcept;
			//
			void			Init(void) noexcept;
			void			Update(void) noexcept;
			//
			void			BG_Draw(void) const noexcept;
			void			Shadow_Draw(void) const noexcept;
			void			SetShadow_Draw_Rigid(void) const noexcept;
			void			Draw(void) const noexcept;
			//
			void			Dispose(void) noexcept;
			void			Dispose_Load(void) noexcept;
		};
	}
}
