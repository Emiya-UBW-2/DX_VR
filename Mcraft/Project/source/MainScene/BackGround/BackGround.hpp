#pragma once
#pragma warning(disable:4464)
#include "../../Header.hpp"
#include "../../MainScene/BackGround/BackGroundSub.hpp"
#include <random>

namespace FPS_n2 {
	namespace BackGround {
		static constexpr int8_t s_EmptyBlick = 0;
		static constexpr int total = 2;
		static constexpr int MulPer = 3;
		static constexpr float CellScale = Scale_Rate / 2.f / 2.f;
		static constexpr int DrawMax = 50;

		class BackGroundClass : public SingletonBase<BackGroundClass> {
		private:
			friend class SingletonBase<BackGroundClass>;
		private:
			MV1								m_ObjSky;

			std::vector<VERTEX3D>			m_vert32;
			std::vector<uint32_t>			m_index32;
			size_t							m_32Num{ 0 };
			size_t							m_32Size{ 0 };
			std::vector<VERTEX3D>			m_vert32SB;
			std::vector<VERTEX3DSHADER>		m_vert32S;
			std::vector<uint32_t>			m_index32S;
			size_t							m_S32Num{ 0 };
			size_t							m_S32Size{ 0 };

			GraphHandle						m_tex{};
			GraphHandle						m_norm{};

			struct CellsData {
				std::array<int8_t, 256 * 256 * 256> m_Cell{};
				std::array<int8_t, 256 * 256 * 256> m_FillInfo{};//周りの遮蔽データのbitフラグ
				int Xall = 256;
				int Yall = 256;
				int Zall = 256;
				int scaleRate = 1;
				//
				const auto& GetCell(int x, int y, int z) const noexcept { return m_Cell[static_cast<size_t>(x * Yall * Zall + y * Zall + z)]; }
				const bool IsActiveCell(int x, int y, int z) const noexcept { return GetCell(x, y, z) != s_EmptyBlick; }
				const bool isFill(int x, int y, int z) const noexcept {
					int FillCount = 0;
					int FillAll = 0;

					int xMaxmin = x * MulPer + MulPer - 1;
					int yMaxmin = y * MulPer + MulPer - 1;
					int zMaxmin = z * MulPer + MulPer - 1;

					for (int xt = xMaxmin; xt < std::min(xMaxmin + MulPer, Xall); ++xt) {
						for (int yt = yMaxmin; yt < std::min(yMaxmin + MulPer, Yall); ++yt) {
							for (int zt = zMaxmin; zt < std::min(zMaxmin + MulPer, Zall); ++zt) {
								++FillAll;
								if (!IsActiveCell(xt, yt, zt)) { continue; }
								++FillCount;
							}
						}
					}
					return (FillAll != 0) && (static_cast<float>(FillCount) / FillAll >= (1.f / 2.f));
				}
				const bool isInside(int x, int y, int z) const noexcept {
					return (
						((0 <= x) && (x < Xall)) &&
						((0 <= y) && (y < Yall)) &&
						((0 <= z) && (z < Zall))
						);
				}
				const Vector3DX GetPos(int x, int y, int z) const noexcept {
					return Vector3DX::vget(static_cast<float>(x - Xall / 2), static_cast<float>(y - Yall / 2), static_cast<float>(z - Zall / 2)) * (CellScale * scaleRate);
				}
				//
				void SetScale(int scale, int baseX, int baseY, int baseZ) noexcept {
					scaleRate = scale;
					Xall = baseX / scaleRate;
					Yall = baseY / scaleRate;
					Zall = baseZ / scaleRate;
				}
				auto& SetCell(int x, int y, int z) noexcept { return m_Cell[static_cast<size_t>(x * Yall * Zall + y * Zall + z)]; }
				//
				const auto& GetOcclusionInfo(int x, int y, int z) const noexcept { return m_FillInfo[static_cast<size_t>(x * Yall * Zall + y * Zall + z)]; }
				auto& SetOcclusionInfo(int x, int y, int z) noexcept { return m_FillInfo[static_cast<size_t>(x * Yall * Zall + y * Zall + z)]; }
				void CalcOcclusion(int x, int y, int z) noexcept {
					//端で端に面している面は隠す
					SetOcclusionInfo(x, y, z) = 0;
					SetOcclusionInfo(x, y, z) |= (1 << 0) * ((x == Xall - 1) ? 1 : IsActiveCell(x + 1, y, z));
					SetOcclusionInfo(x, y, z) |= (1 << 1) * ((x == 0) ? 1 : IsActiveCell(x - 1, y, z));
					SetOcclusionInfo(x, y, z) |= (1 << 2) * ((y == Yall - 1) ? 1 : IsActiveCell(x, y + 1, z));
					SetOcclusionInfo(x, y, z) |= (1 << 3) * ((y == 0) ? 1 : IsActiveCell(x, y - 1, z));
					SetOcclusionInfo(x, y, z) |= (1 << 4) * ((z == Zall - 1) ? 1 : IsActiveCell(x, y, z + 1));
					SetOcclusionInfo(x, y, z) |= (1 << 5) * ((z == 0) ? 1 : IsActiveCell(x, y, z - 1));
				}
			};
			std::array<CellsData, total>	m_CellxN;

			//スレッド用
			std::thread						m_Job;
			bool							m_JobEnd{};
			std::vector<VERTEX3D>			m_vert32Out;
			std::vector<uint32_t>			m_index32Out;
			size_t							m_32NumOut{ 0 };
			std::vector<VERTEX3D>			m_vert32SBOut;
			std::vector<VERTEX3DSHADER>		m_vert32SOut;
			std::vector<uint32_t>			m_index32SOut;
			size_t							m_S32NumOut{ 0 };
			int BaseRate = 100;
			int ShadowRate = 100;
			int ShadowMax = DrawMax;
			int lightX{};
			int lightY{};
			int lightZ{};
			Vector3DX CamPos;
			Vector3DX CamVec;

			//Edit
			float LenMouse = 2.f;
			int xput = 3, yput = 3, zput = 3;
			int8_t blicksel = -1;
		public:
			BackGroundClass(void) noexcept {}
			BackGroundClass(const BackGroundClass&) = delete;
			BackGroundClass(BackGroundClass&& o) = delete;
			BackGroundClass& operator=(const BackGroundClass&) = delete;
			BackGroundClass& operator=(BackGroundClass&& o) = delete;

			virtual ~BackGroundClass(void) noexcept {}
		private:
			inline static bool		CalcIntersectionPoint(const Vector3DX& pointA, const Vector3DX& pointB, const Vector3DX& planePos, const Vector3DX& planenormal, Vector3DX* pointIntersection, bool* pSameVecNormalToA, bool* pOnFront) noexcept {
				// 線分に当たらない
				float dTa = Vector3DX::Dot(planenormal, (pointA - planePos));
				float dTb = Vector3DX::Dot(planenormal, (pointB - planePos));
				*pOnFront = !(dTa < 0.f && dTb < 0.f);
				if ((dTa >= 0.f && dTb >= 0.f) || !*pOnFront) {
					return false;
				}

				*pSameVecNormalToA = (dTa >= 0.f);
				*pointIntersection = pointA + (pointB - pointA) * (std::abs(dTa) / (std::abs(dTa) + std::abs(dTb)));
				return true;
			}
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
		private:
			void			AddPlane(const CellsData& cellx, int x, int y, int z, COLOR_U8 DifColor, COLOR_U8 SpcColor, int xscale,
				int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int xN, int yN, int zN) noexcept;
			void			AddCubeZ(const CellsData& cellx, int x, int y, int zmin, int zmax, int centerX, int centerY, int centerZ) noexcept;
			void			AddCubes(const CellsData& cellx, int centerX, int centerY, int centerZ) noexcept;
			const bool CalcYZActive(const CellsData& cellx, int x, int yMaxmin, int yMaxmax, int zMaxmin, int zMaxmax) const noexcept {
				float scale = CellScale * cellx.scaleRate;
				Vector3DX mid = Vector3DX::one() * (0.5f * scale);
				//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
				Vector3DX YZPos0 = cellx.GetPos(x, yMaxmin, zMaxmin) + mid;
				Vector3DX YZPos1 = cellx.GetPos(x, yMaxmax, zMaxmax) + mid;
				Vector3DX YZPos2 = cellx.GetPos(x, yMaxmin, zMaxmax) + mid;
				Vector3DX YZPos3 = cellx.GetPos(x, yMaxmax, zMaxmin) + mid;
				bool IsHit = false;
				while (true) {
					if (Vector3DX::Dot(CamVec, (YZPos0 - CamPos)) > 0.f) { IsHit = true; break; }
					if (Vector3DX::Dot(CamVec, (YZPos1 - CamPos)) > 0.f) { IsHit = true; break; }
					if (Vector3DX::Dot(CamVec, (YZPos2 - CamPos)) > 0.f) { IsHit = true; break; }
					if (Vector3DX::Dot(CamVec, (YZPos3 - CamPos)) > 0.f) { IsHit = true; break; }
					break;
				}
				return IsHit;
			}
			const bool CalcZMinMax(const CellsData& cellx, int x, int y, int* zMaxmin, int* zMaxmax) const noexcept {
				bool pSameVecNormalToA{};
				bool OnFront{};
				//*
				float scale = CellScale * cellx.scaleRate;
				Vector3DX mid = Vector3DX::one() * (0.5f * scale);
				Vector3DX ZMinPos = cellx.GetPos(x, y, *zMaxmin) + mid;
				Vector3DX ZMaxPos = cellx.GetPos(x, y, *zMaxmax) + mid;
				Vector3DX HitPos;
				if (CalcIntersectionPoint(ZMinPos, ZMaxPos, CamPos, CamVec, &HitPos, &pSameVecNormalToA, &OnFront)) {
					if (!pSameVecNormalToA) {
						*zMaxmin = std::max(static_cast<int>((HitPos.z - 0.5) / scale) + cellx.Zall / 2, *zMaxmin);
					}
					else {
						*zMaxmax = std::min(static_cast<int>((HitPos.z - 0.5) / scale) + cellx.Zall / 2 + 1, *zMaxmax);
					}
					return true;
				}
				return OnFront;
			}

			void			AddShadowPlane(const CellsData& cellx, int x, int y, int z, int xscale,
				int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int xN, int yN, int zN) noexcept;
			void			AddShadowCubeZ(const CellsData& cellx, int x, int y, int zmin, int zmax, int centerX, int centerY, int centerZ) noexcept;
			void			AddShadowCubes(const CellsData& cellx, int centerX, int centerY, int centerZ) noexcept;
		public:
			bool			CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) const noexcept;
			bool			CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept;

			void			LoadCellsFile() noexcept;
			void			SaveCellsFile() noexcept;

			void			SetBlick(int x, int y, int z, int8_t select) noexcept;
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
