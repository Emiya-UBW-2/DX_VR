#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace BackGround {
		//設定パラメーター
		static constexpr int TotalCellLayer = 4;//LOD段階
		static constexpr int MulPer = 2;//LODに伴う粒度指定
		static constexpr float CellScale = 0.25f * Scale3DRate;//1個のブロックの実際のサイズ設定

		//ブロックの最大描画範囲
		static constexpr int DrawMaxXPlus = 70;
		static constexpr int DrawMaxXMinus = -70;
		static constexpr int DrawMaxZPlus = 70;
		static constexpr int DrawMaxZMinus = -70;
		static constexpr int DrawMaxYPlus = 20;
		static constexpr int DrawMaxYMinus = -30;

		//内部計算用
		static constexpr int8_t s_EmptyBlick = 0;//ブロックIDがない=空であることをしめす
		static constexpr int ReferenceCell = 0;//基準になるボクセルタイルリストID
		//LODに準じて描画しない範囲
		static constexpr int DrawMinXPlus = DrawMaxXPlus / MulPer - 1;
		static constexpr int DrawMinXMinus = DrawMaxXMinus / MulPer;
		static constexpr int DrawMinZPlus = DrawMaxZPlus / MulPer - 1;
		static constexpr int DrawMinZMinus = DrawMaxZMinus / MulPer;
		static constexpr int DrawMinYPlus = DrawMaxYPlus / MulPer - 1;
		static constexpr int DrawMinYMinus = DrawMaxYMinus / MulPer;
		//テクスチャタイリングする際のUV分割
		static constexpr float TexTileU = 1.0f / 2.0f;
		static constexpr float TexTileV = 1.0f / 16.0f;

		//三次元整数座標系
		struct Vector3Int {
			int x{};
			int y{};
			int z{};
		public:
			Vector3Int()noexcept {
				this->x = 0;
				this->y = 0;
				this->z = 0;
			}
			Vector3Int(int X, int Y, int Z)noexcept {
				this->x = X;
				this->y = Y;
				this->z = Z;
			}
		public:
			void operator+=(const Vector3Int& o) noexcept {
				this->x += o.x;
				this->y += o.y;
				this->z += o.z;
			}
			const Vector3Int operator+(const Vector3Int& o) const noexcept {
				return Vector3Int(this->x + o.x, this->y + o.y, this->z + o.z);
			}
		};

		//ボクセル描画、コリジョン生成クラス
		class VoxelControl {
			class CellsData {
				//セル一つ一つに含まれる情報
				class CellBuffer {
					int8_t					m_Life{};//耐久
					int8_t					m_ID{};//ID
					int8_t					m_FillInfo{};//周りの遮蔽データのbitフラグ
				public:
					//壊れるまでの耐久を設定/取得
					void SetLife(int8_t id) noexcept { this->m_Life = id; }
					const int8_t& GetLife(void) const noexcept { return this->m_Life; }
					//ブロックIDを1～で指定/取得
					void SetID(int8_t id) noexcept { this->m_ID = id; }
					const int8_t& GetID(void) const noexcept { return this->m_ID; }
					const int8_t GetCellTexID(void) const noexcept { return ((GetID() - 1) * 3); }
					//ブロック間の遮蔽を設定/取得
					void ResetOcclusion(void) noexcept { this->m_FillInfo = 0; }
					void SetOcclusion(int id) noexcept { this->m_FillInfo |= (1 << id); }
				public:
					const bool IsEmpty(void) const noexcept { return GetID() == s_EmptyBlick; }
					const bool IsHidden(void) const noexcept { return this->m_FillInfo == 0b111111; }
					const bool IsOcclusion(int id) const noexcept { return (this->m_FillInfo & (1 << id)) != 0; }
					const bool CanDraw(void) const noexcept { return !IsEmpty() && !IsHidden(); }
				};
			private:
				std::vector<CellBuffer>	m_CellBuffer;
				int						m_LODRate = 0;
			public:
				//算術補助系
				int						ScaleRate = static_cast<int>(std::pow(MulPer, GetLODRate()));
				int						All = 256 / this->ScaleRate;//XYZに描画するセルの1軸の数
				int						Half = this->All / 2;//Allの半分
				int						AllPow2 = this->All * this->All;//Allの2乗
				float					Scale = (CellScale * this->ScaleRate);//描画座標に変換する際にかける数
			private:
				//特定軸のループするインデックスを取得
				const int			GetLoopIndex(int axis) const noexcept { return (axis % this->All + this->All) % this->All; }
			public:
				const CellBuffer&	GetCellBuf(int Xvoxel, int Yvoxel, int Zvoxel) const noexcept { return this->m_CellBuffer[GetCellIndex(Xvoxel, Yvoxel, Zvoxel)]; }
				const CellBuffer&	GetCellBuf(const Vector3Int& VoxelPoint) const noexcept { return GetCellBuf(VoxelPoint.x, VoxelPoint.y, VoxelPoint.z); }
				CellBuffer&			SetCellBuf(int Xvoxel, int Yvoxel, int Zvoxel) noexcept { return this->m_CellBuffer[GetCellIndex(Xvoxel, Yvoxel, Zvoxel)]; }
				const int&			GetLODRate(void) const noexcept { return this->m_LODRate; }
			public:
				const bool			isReferenceCell(void) const noexcept { return GetLODRate() == ReferenceCell; }
				const size_t		GetCellIndex(int Xvoxel, int Yvoxel, int Zvoxel) const noexcept { return static_cast<size_t>(GetLoopIndex(Xvoxel) * this->AllPow2 + Yvoxel * this->All + GetLoopIndex(Zvoxel)); }
				const int8_t		isFill(int Xvoxel, int Yvoxel, int Zvoxel, int mul) const noexcept {
					mul /= this->ScaleRate;
					int FillCount = 0;
					int FillAll = 0;

					int xMaxmin = Xvoxel * mul + mul - 1;
					int yMaxmin = Yvoxel * mul + mul - 1;
					int zMaxmin = Zvoxel * mul + mul - 1;
					std::vector<int> IDCount;
					for (int xt = xMaxmin; xt < xMaxmin + mul; ++xt) {
						for (int yt = yMaxmin; yt < std::min(yMaxmin + mul, this->All); ++yt) {
							for (int zt = zMaxmin; zt < zMaxmin + mul; ++zt) {
								++FillAll;
								if (GetCellBuf(xt, yt, zt).IsEmpty()) { continue; }
								++FillCount;
								size_t cell = static_cast<size_t>(GetCellBuf(xt, yt, zt).GetID() + 1);
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
				const bool			isInside(int Yvoxel) const noexcept { return ((0 <= Yvoxel) && (Yvoxel < this->All)); }
				//ボクセル座標から描画座標の最小範囲に変換
				const Vector3DX		GetWorldPos(int Xvoxel, int Yvoxel, int Zvoxel) const noexcept {
					return Vector3DX::vget(
						static_cast<float>(Xvoxel - this->Half),
						static_cast<float>(Yvoxel - this->Half),
						static_cast<float>(Zvoxel - this->Half)
					) * this->Scale;
				}
				const Vector3DX		GetWorldPos(const Vector3Int& VoxelPoint) const noexcept {
					return GetWorldPos(VoxelPoint.x, VoxelPoint.y, VoxelPoint.z);
				}
				//ボクセル座標から描画座標のXYZ各範囲に変換
				const Vector3DX		GetWorldPosOffset(const Vector3Int& VoxelPoint, int Xofs, int Yofs, int Zofs) const noexcept {
					return GetWorldPos(VoxelPoint.x + Xofs, VoxelPoint.y + Yofs, VoxelPoint.z + Zofs);
				}
				//指定の描画座標が入っている範囲のボクセル座標に変換
				const Vector3Int	GetVoxelPoint(const Vector3DX& pos) const noexcept {
					Vector3DX Point = pos / this->Scale;
					return Vector3Int(
						static_cast<int>(Point.x) + this->Half,
						static_cast<int>(Point.y) + this->Half,
						static_cast<int>(Point.z) + this->Half
					);
				}
				void				CalcOcclusion(int Xvoxel, int Yvoxel, int Zvoxel) noexcept {
					if (!isInside(Yvoxel)) { return; }
					if (GetCellBuf(Xvoxel, Yvoxel, Zvoxel).IsEmpty()) { return; }

					SetCellBuf(Xvoxel, Yvoxel, Zvoxel).ResetOcclusion();
					if (!GetCellBuf(Xvoxel + 1, Yvoxel, Zvoxel).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(0); }

					if (!GetCellBuf(Xvoxel - 1, Yvoxel, Zvoxel).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(1); }
					if ((Yvoxel == this->All - 1) ? true : !GetCellBuf(Xvoxel, Yvoxel + 1, Zvoxel).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(2); }
					if ((Yvoxel == 0) ? true : !GetCellBuf(Xvoxel, Yvoxel - 1, Zvoxel).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(3); }
					if (!GetCellBuf(Xvoxel, Yvoxel, Zvoxel + 1).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(4); }
					if (!GetCellBuf(Xvoxel, Yvoxel, Zvoxel - 1).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(5); }
				}
			public:
				void				Init(int scale) noexcept {
					this->m_LODRate = scale;
					//算術補助系の更新
					this->ScaleRate = static_cast<int>(std::pow(MulPer, GetLODRate()));
					this->All = 256 / this->ScaleRate;
					this->Half = this->All / 2;
					this->AllPow2 = this->All * this->All;
					this->Scale = (CellScale * this->ScaleRate);
					this->m_CellBuffer.resize(static_cast<size_t>(this->All * this->All * this->All));
				}
				void				Dispose(void) noexcept {
					this->m_CellBuffer.clear();
				}
			};
			//スレッド実行処理
			class ThreadJobs {
				std::thread						m_Job;//作業スレッド
				bool							m_JobEnd{};//ジョブ終了通知
				bool							m_IsDoOnce{};//1回だけ実行するフラグ
				bool							m_isDoing{ false };//
				bool							m_IsDoEnd{};//ジョブ終了後作業終了通知
#if defined(DEBUG) && CHECKTHREADTIME
				LONGLONG						m_StartTime{};
				LONGLONG						m_TotalTime{};
#endif
				std::function<void()>			m_Doing{ nullptr };//ジョブ
				std::function<void()>			m_EndDoing{ nullptr };//ジョブ終了後作業
			public:
				void JobStart(void) noexcept {
					if (!this->m_isDoing) {
						this->m_isDoing = true;
						this->m_JobEnd = true;
						this->m_IsDoEnd = false;
					}
				}
			public:
				void Init(std::function<void()> Doing, std::function<void()> EndDoing, bool IsDoOnce) noexcept {
					this->m_Doing = Doing;
					this->m_EndDoing = EndDoing;
					this->m_IsDoOnce = IsDoOnce;

					//JobStart();
					this->m_isDoing = true;
					this->m_JobEnd = true;
					this->m_IsDoEnd = true;
				}
				void Update(bool isActive) noexcept {
					if (isActive) {
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
							//1回だけの実行をしない場合
							if (!this->m_IsDoOnce) {
								this->m_isDoing = true;
							}
							if (this->m_isDoing) {
								if (this->m_Job.joinable()) {
									this->m_Job.detach();
								}
								std::thread tmp([&]() {
									if (this->m_Doing) {
										this->m_Doing();
									}
									this->m_JobEnd = true;
									this->m_isDoing = false;
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
					else {
						//1回だけ実行する場合はオンにしといて次回実行に備える
						if (this->m_IsDoOnce) {
							this->m_isDoing = true;
						}
					}
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
			class VERTEX3DData {
				int												m_Now{ 0 };
				size_t											m_32Size{ 0 };
				std::array<std::vector<VERTEX3D>, 2>			m_vert32;
				std::array<std::vector<uint32_t>, 2>			m_index32;
				std::array<size_t, 2>							m_32Num{ 0 };
			private:
				const size_t& GetInNum(void) const noexcept { return this->m_32Num[this->m_Now]; }
				const size_t& GetOutNum(void) const noexcept { return this->m_32Num[static_cast<size_t>(1 - this->m_Now)]; }
				const std::vector<VERTEX3D>& GetOutVert(void) const noexcept { return this->m_vert32[static_cast<size_t>(1 - this->m_Now)]; }
				const std::vector<uint32_t>& GetOutindex(void) const noexcept { return this->m_index32[static_cast<size_t>(1 - this->m_Now)]; }
			public:
				//元データをリセットせず表示状だけ破棄する
				void		Reset(void) noexcept {
					for (int loop = 0; loop < 2; ++loop) {
						this->m_32Num[loop] = 0;
					}
				}
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
					Reset();
					//データも破棄する
					for (int loop = 0; loop < 2; ++loop) {
						this->m_vert32[loop].clear();
						this->m_index32[loop].clear();
					}
					this->m_32Size = 0;
				}
				//登録数をリセット
				void		StartRegist(void) noexcept {
					this->m_32Num[this->m_Now] = 0;
				}
				//1枚確保
				size_t		RegistPlane(void) noexcept {
					++this->m_32Num[this->m_Now];
					if (GetInNum() > this->m_32Size) {
						this->m_32Size = GetInNum();
						for (int loop = 0; loop < 2; ++loop) {
							this->m_vert32[loop].resize(this->m_32Size * 4);
							this->m_index32[loop].resize(this->m_32Size * 6);
						}
					}
					size_t Now = GetInNum() - 1;
					uint32_t ZERO = static_cast<uint32_t>(Now * 4);
					this->m_index32[this->m_Now][Now * 6 + 0] = ZERO;
					this->m_index32[this->m_Now][Now * 6 + 1] = ZERO + 1;
					this->m_index32[this->m_Now][Now * 6 + 2] = ZERO + 2;
					this->m_index32[this->m_Now][Now * 6 + 3] = ZERO + 3;
					this->m_index32[this->m_Now][Now * 6 + 4] = ZERO + 2;
					this->m_index32[this->m_Now][Now * 6 + 5] = ZERO + 1;
					return Now;
				}
				//確保済の1枚について1角の情報を書き込む
				void		SetParamToPlane(int index, const Vector3DX& pos, const Vector3DX& norm, int Uoffset, int Voffset) noexcept {
					VERTEX3D& Vert = this->m_vert32[this->m_Now][index];
					Vert.pos = pos.get();
					Vert.norm = norm.get();
					Vert.dif = GetColorU8(128, 128, 128, 255);
					Vert.spc = GetColorU8(64, 64, 64, 255);
					Vert.u = TexTileU * static_cast<float>(Uoffset);
					Vert.v = TexTileV * static_cast<float>(Voffset);
				}
				//登録を完了し出力する
				void		EndRegist(void) noexcept {
					this->m_Now = 1 - this->m_Now;
				}
				//描画する
				void		DrawByShader(void) const noexcept {
					if (GetOutNum() == 0) { return; }
					DrawPolygon32bitIndexed3DToShader2(GetOutVert().data(), static_cast<int>(GetOutNum() * 4), GetOutindex().data(), static_cast<int>(GetOutNum() * 6 / 3));
				}
				void		Draw(const GraphHandle& GrHandle) const noexcept {
					if (GetOutNum() == 0) { return; }
					DrawPolygon32bitIndexed3D(GetOutVert().data(), static_cast<int>(GetOutNum() * 4), GetOutindex().data(), static_cast<int>(GetOutNum() * 6 / 3), GrHandle.get(), true);
				}
			};
			//スレッドが変更し描画で参照するデータ
			class DrawThreadData {
				Vector3DX			m_DrawCenterPos;
				Vector3DX			m_CamVec;
			public:
				VERTEX3DData		m_Vert32;
				ThreadJobs			m_Jobs;
			public:
				const Vector3DX&	GetDrawCenterPos(void) const noexcept { return this->m_DrawCenterPos; }
				const Vector3DX&	GetCamVec(void) const noexcept { return this->m_CamVec; }
#if defined(DEBUG) && FALSE
			public:
				const VERTEX3DData& GetVert32(void) const noexcept { return this->m_Vert32; }
#endif
			public:
				void		Init(size_t size) noexcept {
					this->m_Vert32.Init(size);
				}
				void		StartRegist() noexcept {
					this->m_Vert32.StartRegist();
				}
				void		EndRegist(const Vector3DX& camPos, const Vector3DX& camVec) noexcept {
					this->m_Vert32.EndRegist();
					this->m_DrawCenterPos = camPos;
					this->m_CamVec = camVec;
				}
				void		Update(bool value) noexcept {
					if (!value) {
						this->m_Vert32.Reset();//生成データ自体を破棄
					}
					this->m_Jobs.Update(value);
				}
				void		DrawByShader(void) const noexcept {
					this->m_Vert32.DrawByShader();
				}
				void		Draw(const GraphHandle& GrHandle) const noexcept {
					this->m_Vert32.Draw(GrHandle);
				}
				void		Dispose() noexcept {
					this->m_Vert32.Dispose();
					this->m_Jobs.Dispose();
				}
			};
		private:
			std::array<CellsData, TotalCellLayer>	m_CellxN;
			int								m_MaxDrawLOD = 1;
			int								m_ShadowMaxDrawLOD = 1;
			bool							m_isChangeBlock{ false };
			GraphHandle						m_tex{};
			std::array<DrawThreadData, TotalCellLayer + TotalCellLayer>	m_DrawThreadDatas;			//0~TotalCellLayer-1 = 表示ポリゴンスレッド用 / TotalCellLayer~ = 影スレッド用
			int								m_ThreadCounter = 0;
			Vector3DX						m_DrawCenterPos;
			Vector3DX						m_CamVec;
			Vector3DX						m_ShadowDrawCenterPos;
			Vector3DX						m_ShadowCamVec;
		private:
			//ブレゼンハムアルゴリズムで線分上にボクセルを走査
			inline static void		Bresenham3D(int x1, int y1, int z1, int x2, int y2, int z2, const std::function<bool(const Vector3Int&)>& OutPutLine) noexcept {
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
						if (OutPutLine(Vector3Int(point[0], point[1], point[2]))) { return; }
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
						if (OutPutLine(Vector3Int(point[0], point[1], point[2]))) { return; }
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
						if (OutPutLine(Vector3Int(point[0], point[1], point[2]))) { return; }
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
				OutPutLine(Vector3Int(point[0], point[1], point[2]));
			}
			//AABBと線分の当たり判定
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
			//線分をDotaDotbの結果から縮める
			inline static bool		CullingLine(int* MaxminT, int* MaxmaxT, float dTa, float dTb) noexcept {
				bool OnFront = (dTa >= 0.0f && dTb >= 0.0f);
				bool Onbehind = (dTa < 0.0f && dTb < 0.0f);
				if (Onbehind && !OnFront) {
					return false;
				}
				if (!OnFront && !Onbehind) {
					if (dTa < 0.0f) {
						*MaxminT = std::max(*MaxminT + static_cast<int>((*MaxmaxT - *MaxminT) * (dTa / (dTa - dTb))) - 1, *MaxminT);
					}
					else {
						*MaxmaxT = std::min(*MaxminT + static_cast<int>((*MaxmaxT - *MaxminT) * (dTa / (dTa - dTb))) + 1, *MaxmaxT);
					}
				}
				return true;
			};
			//各方向に向いているポリゴンの追加
			void			AddPlaneXPlus(VERTEX3DData* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept;
			void			AddPlaneXMinus(VERTEX3DData* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept;
			void			AddPlaneYPlus(VERTEX3DData* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept;
			void			AddPlaneYMinus(VERTEX3DData* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept;
			void			AddPlaneZPlus(VERTEX3DData* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept;
			void			AddPlaneZMinus(VERTEX3DData* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept;
			//XZ方向に走査してポリゴンをなるべく少ないポリゴン数で表示する
			void			AddPlanesXY(VERTEX3DData* pTarget, bool isDrawXPlus, bool isDrawYPlus, size_t id, const Vector3Int& VCenter, const Vector3Int& Vofs, int MaxminT, int MaxmaxT, bool useTexture) noexcept;
			void			AddPlanesZ(VERTEX3DData* pTarget, bool isDrawZPlus, size_t id, const Vector3Int& VCenter, const Vector3Int& Vofs, int MaxminT, int MaxmaxT, bool useTexture) noexcept;
			//視界から見て映るものだけをテクスチャ関係込みで更新
			void			AddCubes(size_t id, size_t threadID, bool UseFrustumCulling, bool useTexture) noexcept;
		public:
			const CellsData& GetReferenceCells(void) const noexcept { return this->m_CellxN[ReferenceCell]; }
			//ブロックを直接指定
			void			SetBlick(int Xvoxel, int Yvoxel, int Zvoxel, int8_t ID, bool CalcOther = true) noexcept;
			//ブロックにダメージを与える
			bool			DamageCell(int Xvoxel, int Yvoxel, int Zvoxel, int8_t Damage) noexcept {
				if (!GetReferenceCells().isInside(Yvoxel)) {
					return false;
				}
				const auto& CellBuff = GetReferenceCells().GetCellBuf(Xvoxel, Yvoxel, Zvoxel);
				if (CellBuff.IsEmpty()) { return false; }
				if (CellBuff.GetID() == 1) { return false; }
				if (CellBuff.GetID() == 4) { return false; }
				this->m_CellxN[ReferenceCell].SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetLife(CellBuff.GetLife() - Damage);
				if (CellBuff.GetLife() <= 0) {
					SetBlick(Xvoxel, Yvoxel, Zvoxel, s_EmptyBlick);
					m_isChangeBlock = true;
					return true;
				}
				else if (CellBuff.GetLife() <= 50) {
					if (CellBuff.GetID() == 2) {
						SetBlick(Xvoxel, Yvoxel, Zvoxel, 3);
					}
				}
				return false;
			}
			//ボクセルとの線での当たり判定を取る
			int				CheckLine(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) const noexcept;
			//対人を想定した壁判定を行う
			bool			CheckWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius, const std::vector<const MV1*>& addonColObj) const noexcept;
			//ボクセルデータをロードする
			void			LoadCellsFile(std::string_view Path) noexcept;
			//ボクセルデータをセーブする
			void			SaveCellsFile(std::string_view Path) const noexcept;
			//所定の座標にボクセルデータをロードする
			void			LoadCellsClip(int Xvoxel, int Yvoxel, int Zvoxel, std::string_view Path) noexcept;
			//所定の範囲のボクセルデータをセーブする
			void			SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax, std::string_view Path) const noexcept;

			void			SettingChange(void) noexcept;

			constexpr float GetDrawFarMax() const noexcept {
				return std::min(std::min(BackGround::DrawMaxXPlus, BackGround::DrawMaxZPlus), BackGround::DrawMaxYPlus) * BackGround::CellScale;
			}
		public:
			VoxelControl(void) noexcept {}
			VoxelControl(const VoxelControl&) = delete;
			VoxelControl(VoxelControl&&) = delete;
			VoxelControl& operator=(const VoxelControl&) = delete;
			VoxelControl& operator=(VoxelControl&&) = delete;

			virtual ~VoxelControl(void) noexcept {}
		public://
			void			Load(void) noexcept;
			void			Init(void) noexcept;
			void			Setup(void) noexcept;
			void			Update(void) noexcept;
			void			DrawShadow(void) const noexcept;
			void			DrawByShader(void) const noexcept;
			void			Draw(void) const noexcept;
			void			Dispose(void) noexcept;
			void			Dispose_Load(void) noexcept;
		};
	}
};
