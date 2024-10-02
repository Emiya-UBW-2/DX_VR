#include	"BackGround.hpp"

const FPS_n2::BackGround::BackGroundClass* SingletonBase<FPS_n2::BackGround::BackGroundClass>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace BackGround {
		bool		BackGroundClass::CalcIntersectionPoint(const Vector3DX& pointA, const Vector3DX& pointB, const Vector3DX& planePos, const Vector3DX& planenormal, Vector3DX* pointIntersection, bool* pSameVecNormalToA, bool* pOnFront) noexcept {
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
		void		BackGroundClass::Bresenham3D(const int x1, const int y1, const int z1, const int x2, const int y2, const int z2, const std::function<bool(int, int, int)>& OutPutLine) noexcept {

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
				for (int i = 0; i < l; i++) {
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
				for (int i = 0; i < m; i++) {
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
				for (int i = 0; i < n; i++) {
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
		void		BackGroundClass::AddCube(const CellsData& cell, int x, int y, int z, bool CheckFill, COLOR_U8 DifColor, COLOR_U8 SpcColor) noexcept {
			auto* DrawParts = DXDraw::Instance();
			Vector3DX Pos = Vector3DX::vget(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
			Vector3DX PosMin = Pos * (CellScale * cell.scaleRate);
			Vector3DX PosMid = (Pos + Vector3DX::one() * 0.5f) * (CellScale * cell.scaleRate);
			Vector3DX PosMax = (Pos + Vector3DX::one()) * (CellScale * cell.scaleRate);

			Vector3DX PosCamVec = PosMid - DrawParts->GetMainCamera().GetCamPos();




			auto GetPlane = [&](const Vector3DX& Pos1, const Vector3DX& Pos2, const Vector3DX& Pos3, const Vector3DX& Pos4, const Vector3DX& Normal) {
				//PosCamVec.x * Normal.x + PosCamVec.y * Normal.y + PosCamVec.z * Normal.z;
				if (Normal.x != 0) {
					if ((PosCamVec.x > 0.f && Normal.x > 0.f) || (PosCamVec.x < 0.f && Normal.x < 0.f)) { return; }
				}
				if (Normal.y != 0) {
					if ((PosCamVec.y > 0.f && Normal.y > 0.f) || (PosCamVec.y < 0.f && Normal.y < 0.f)) { return; }
				}
				if (Normal.z != 0) {
					if ((PosCamVec.z > 0.f && Normal.z > 0.f) || (PosCamVec.z < 0.f && Normal.z < 0.f)) { return; }
				}
				//if (Vector3DX::Dot(PosCamVec, Normal) >= 0.f) { return; }

				if ((!CheckFill || cell.GetCell(x + static_cast<int>(Normal.x), y + static_cast<int>(Normal.y), z + static_cast<int>(Normal.z)) == 0)) {

					size_t Now = m_vert32Num;
					size_t Nowi = m_index32Num;
					m_vert32Num += 4;
					m_index32Num += 6;
					if (m_vert32Num > m_vert32.size()) {
						m_vert32.resize(m_vert32Num);
					}
					if (m_index32Num > m_index32.size()) {
						m_index32.resize(m_index32Num);
					}

					m_index32.at(Nowi) = (uint32_t)(Now);
					m_index32.at(Nowi + 1) = (uint32_t)(Now + 1);
					m_index32.at(Nowi + 2) = (uint32_t)(Now + 2);
					m_index32.at(Nowi + 3) = (uint32_t)(Now + 3);
					m_index32.at(Nowi + 4) = (uint32_t)(Now + 2);
					m_index32.at(Nowi + 5) = (uint32_t)(Now + 1);

					float Xofs = 1.0f - Normal.y;
					float uMin = (0.f + Xofs) / 8.f;
					float vMin = 0.f / 8.f;
					float uMax = uMin + 1.f / 8.f;
					float vMax = vMin + 1.f / 8.f;

					m_vert32.at(Now).pos = Pos1.get();
					m_vert32.at(Now).u = uMax;
					m_vert32.at(Now).v = vMin;
					m_vert32.at(Now + 1).pos = Pos2.get();
					m_vert32.at(Now + 1).u = uMin;
					m_vert32.at(Now + 1).v = vMin;
					m_vert32.at(Now + 2).pos = Pos3.get();
					m_vert32.at(Now + 2).u = uMax;
					m_vert32.at(Now + 2).v = vMax;
					m_vert32.at(Now + 3).pos = Pos4.get();
					m_vert32.at(Now + 3).u = uMin;
					m_vert32.at(Now + 3).v = vMax;
					for (size_t i = 0; i < 4; i++) {
						m_vert32.at(Now + i).norm = Normal.get();
						m_vert32.at(Now + i).dif = DifColor;
						m_vert32.at(Now + i).spc = SpcColor;
						m_vert32.at(Now + i).su = m_vert32.at(Now + i).u;
						m_vert32.at(Now + i).sv = m_vert32.at(Now + i).v;
					}
				}
				};
			// 頂点データの作成
			GetPlane(
				Vector3DX::vget(PosMin.x, PosMax.y, PosMin.z), Vector3DX::vget(PosMax.x, PosMax.y, PosMin.z), PosMin, Vector3DX::vget(PosMax.x, PosMin.y, PosMin.z),
				Vector3DX::back());

			GetPlane(
				Vector3DX::vget(PosMin.x, PosMin.y, PosMax.z), Vector3DX::vget(PosMax.x, PosMin.y, PosMax.z), Vector3DX::vget(PosMin.x, PosMax.y, PosMax.z), PosMax,
				Vector3DX::forward());

			GetPlane(
				Vector3DX::vget(PosMin.x, PosMax.y, PosMax.z), Vector3DX::vget(PosMin.x, PosMax.y, PosMin.z), Vector3DX::vget(PosMin.x, PosMin.y, PosMax.z), PosMin,
				Vector3DX::left());

			GetPlane(
				Vector3DX::vget(PosMax.x, PosMax.y, PosMin.z), PosMax, Vector3DX::vget(PosMax.x, PosMin.y, PosMin.z), Vector3DX::vget(PosMax.x, PosMin.y, PosMax.z),
				Vector3DX::right());

			GetPlane(
				Vector3DX::vget(PosMin.x, PosMax.y, PosMax.z), PosMax, Vector3DX::vget(PosMin.x, PosMax.y, PosMin.z), Vector3DX::vget(PosMax.x, PosMax.y, PosMin.z),
				Vector3DX::up());

			GetPlane(
				PosMin, Vector3DX::vget(PosMax.x, PosMin.y, PosMin.z), Vector3DX::vget(PosMin.x, PosMin.y, PosMax.z), Vector3DX::vget(PosMax.x, PosMin.y, PosMax.z),
				Vector3DX::down());
		}

		void BackGroundClass::AddShadowCube(const CellsData& cell, int x, int y, int z, COLOR_U8 DifColor, COLOR_U8 SpcColor) noexcept {
			Vector3DX Pos = Vector3DX::vget(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
			Vector3DX PosMin = Pos * (CellScale * cell.scaleRate);
			Vector3DX PosMid = (Pos + Vector3DX::one() * 0.5f) * (CellScale * cell.scaleRate);
			Vector3DX PosMax = (Pos + Vector3DX::one()) * (CellScale * cell.scaleRate);

			auto GetPlane = [&](const Vector3DX& Pos1, const Vector3DX& Pos2, const Vector3DX& Pos3, const Vector3DX& Pos4, const Vector3DX& Normal) {
				if (cell.GetCell(x + static_cast<int>(Normal.x), y + static_cast<int>(Normal.y), z + static_cast<int>(Normal.z)) == 0) {
					size_t NowS = m_vert32SNum;
					size_t NowSi = m_index32SNum;
					m_vert32SNum += 4;
					m_index32SNum += 6;
					if (m_vert32SNum > m_vert32S.size()) {
						m_vert32S.resize(m_vert32SNum);
						m_vert32SB.resize(m_vert32SNum);
					}
					if (m_index32SNum > m_index32S.size()) {
						m_index32S.resize(m_index32SNum);
					}

					m_vert32S.at(NowS).pos = Pos1.get();
					m_vert32S.at(NowS + 1).pos = Pos2.get();
					m_vert32S.at(NowS + 2).pos = Pos3.get();
					m_vert32S.at(NowS + 3).pos = Pos4.get();

					m_vert32SB.at(NowS).pos = Pos1.get();
					m_vert32SB.at(NowS + 1).pos = Pos2.get();
					m_vert32SB.at(NowS + 2).pos = Pos3.get();
					m_vert32SB.at(NowS + 3).pos = Pos4.get();
					for (size_t i = 0; i < 4; i++) {
						m_vert32SB.at(NowS + i).norm = Normal.get();
						m_vert32SB.at(NowS + i).dif = DifColor;
						m_vert32SB.at(NowS + i).spc = SpcColor;
					}

					m_index32S.at(NowSi) = (uint32_t)(NowS);
					m_index32S.at(NowSi + 1) = (uint32_t)(NowS + 1);
					m_index32S.at(NowSi + 2) = (uint32_t)(NowS + 2);
					m_index32S.at(NowSi + 3) = (uint32_t)(NowS + 3);
					m_index32S.at(NowSi + 4) = (uint32_t)(NowS + 2);
					m_index32S.at(NowSi + 5) = (uint32_t)(NowS + 1);
				}
				};
			// 頂点データの作成
			GetPlane(
				Vector3DX::vget(PosMin.x, PosMax.y, PosMin.z), Vector3DX::vget(PosMax.x, PosMax.y, PosMin.z), PosMin, Vector3DX::vget(PosMax.x, PosMin.y, PosMin.z),
				Vector3DX::back());

			GetPlane(
				Vector3DX::vget(PosMin.x, PosMin.y, PosMax.z), Vector3DX::vget(PosMax.x, PosMin.y, PosMax.z), Vector3DX::vget(PosMin.x, PosMax.y, PosMax.z), PosMax,
				Vector3DX::forward());

			GetPlane(
				Vector3DX::vget(PosMin.x, PosMax.y, PosMax.z), Vector3DX::vget(PosMin.x, PosMax.y, PosMin.z), Vector3DX::vget(PosMin.x, PosMin.y, PosMax.z), PosMin,
				Vector3DX::left());

			GetPlane(
				Vector3DX::vget(PosMax.x, PosMax.y, PosMin.z), PosMax, Vector3DX::vget(PosMax.x, PosMin.y, PosMin.z), Vector3DX::vget(PosMax.x, PosMin.y, PosMax.z),
				Vector3DX::right());

			GetPlane(
				Vector3DX::vget(PosMin.x, PosMax.y, PosMax.z), PosMax, Vector3DX::vget(PosMin.x, PosMax.y, PosMin.z), Vector3DX::vget(PosMax.x, PosMax.y, PosMin.z),
				Vector3DX::up());

			GetPlane(
				PosMin, Vector3DX::vget(PosMax.x, PosMin.y, PosMin.z), Vector3DX::vget(PosMin.x, PosMin.y, PosMax.z), Vector3DX::vget(PosMax.x, PosMin.y, PosMax.z),
				Vector3DX::down());
		}

		bool		BackGroundClass::ColRayBox(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AABBMinPos, const Vector3DX& AABBMaxPos, Vector3DX* Normal, int* NormalNum) const noexcept {
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
							if (NormalNum) {
								*NormalNum = Vec.x > 0.f ? 0 : 1;
							}
							break;
						case 1:
							if (Normal) {
								*Normal = Vec.y > 0.f ? Vector3DX::down() : Vector3DX::up();
							}
							if (NormalNum) {
								*NormalNum = Vec.y > 0.f ? 2 : 3;
							}
							break;
						case 2:
							if (Normal) {
								*Normal = Vec.z > 0.f ? Vector3DX::back() : Vector3DX::forward();
							}
							if (NormalNum) {
								*NormalNum = Vec.z > 0.f ? 4 : 5;
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
		bool		BackGroundClass::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) const noexcept {
			auto& cell = m_CellxN.back();
			Vector3DX Start = StartPos / (CellScale * cell.scaleRate);
			Vector3DX End = StartPos / (CellScale * cell.scaleRate);

			for (int xm = -1; xm <= 1; xm++) {
				for (int ym = -1; ym <= 1; ym++) {
					for (int zm = -1; zm <= 1; zm++) {
						bool isHit = false;
						Bresenham3D(static_cast<int>(Start.x + xm), static_cast<int>(Start.y + ym), static_cast<int>(Start.z + zm), static_cast<int>(End.x + xm), static_cast<int>(End.y + ym), static_cast<int>(End.z + zm), [&](int x, int y, int z) {
							if (
								((x <= -cell.Xall / 2) || (cell.Xall / 2 - 1 < x)) ||
								((y <= -cell.Yall / 2) || (cell.Yall / 2 - 1 < y)) ||
								((z <= -cell.Zall / 2) || (cell.Zall / 2 - 1 < z))
								) {
								return false;
							}

							const auto& Cell = cell.GetCell(x, y, z);
							if (Cell <= 0) { return false; }
							Vector3DX MinPos = (Vector3DX::vget(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) + (Vector3DX::one() * 0.0f)) * (CellScale * cell.scaleRate);
							Vector3DX MaxPos = (Vector3DX::vget(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) + (Vector3DX::one() * 1.0f)) * (CellScale * cell.scaleRate);
							if (ColRayBox(StartPos, EndPos, MinPos, MaxPos, Normal)) {
								isHit = true;
								return true;
							}
							return false;
							});
						if (isHit) { return true; }
					}
				}
			}
			return false;
		}

		bool		BackGroundClass::CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept {
			auto MoveVector = *EndPos - StartPos;
			// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
			std::vector<MV1_COLL_RESULT_POLY> kabes;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			auto& cell = m_CellxN.back();
			Vector3DX Start = StartPos / (CellScale * cell.scaleRate);
			Vector3DX End = StartPos / (CellScale * cell.scaleRate);

			for (int xm = -2; xm <= 2; xm++) {
				for (int ym = 2; ym <= 2; ym++) {
					for (int zm = -2; zm <= 2; zm++) {
						Bresenham3D(static_cast<int>(Start.x + xm), static_cast<int>(Start.y + ym), static_cast<int>(Start.z + zm), static_cast<int>(End.x + xm), static_cast<int>(End.y + ym), static_cast<int>(End.z + zm), [&](int x, int y, int z) {
							if (
								((x <= -cell.Xall / 2) || (cell.Xall / 2 - 1 < x)) ||
								((y <= -cell.Yall / 2) || (cell.Yall / 2 - 1 < y)) ||
								((z <= -cell.Zall / 2) || (cell.Zall / 2 - 1 < z))
								) {
								return false;
							}

							const auto& Cell = cell.GetCell(x, y, z);
							if (Cell <= 0) { return false; }
							Vector3DX MinPos = (Vector3DX::vget(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) + (Vector3DX::one() * -0.1f)) * (CellScale * cell.scaleRate);
							Vector3DX MaxPos = (Vector3DX::vget(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) + (Vector3DX::one() * 1.1f)) * (CellScale * cell.scaleRate);

							MV1_COLL_RESULT_POLY tmp{};
							// Left
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::left().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::left().get();
							kabes.emplace_back(tmp);
							// Right
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::right().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::right().get();
							kabes.emplace_back(tmp);
							// Back
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::back().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MinPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MinPos.z).get();
							tmp.Normal = Vector3DX::back().get();
							kabes.emplace_back(tmp);
							// Forward
							tmp.Position[0] = Vector3DX::vget(MinPos.x, MinPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::forward().get();
							kabes.emplace_back(tmp);
							tmp.Position[0] = Vector3DX::vget(MaxPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[1] = Vector3DX::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
							tmp.Position[2] = Vector3DX::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
							tmp.Normal = Vector3DX::forward().get();
							kabes.emplace_back(tmp);
							return true;
							});
					}
				}
			}
			bool HitFlag = false;
			// 壁ポリゴンとの当たり判定処理
			if (kabes.size() > 0) {
				HitFlag = false;
				for (auto& kabe : kabes) {
					// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
						HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
						if (MoveVector.magnitude() >= 0.001f) {	// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定
							// 壁に当たったら壁に遮られない移動成分分だけ移動する
							*EndPos = Vector3DX::Cross(kabe.Normal, Vector3DX::Cross(MoveVector, kabe.Normal)) + StartPos;
							//EndPos->y(StartPos.y);
							bool j = false;
							for (auto& kabe2 : kabes) {
								if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
									j = true;
									break;// 当たっていたらループから抜ける
								}
							}
							if (!j) {
								HitFlag = false;
								break;//どのポリゴンとも当たらなかったということなので壁に当たったフラグを倒した上でループから抜ける
							}
						}
						else {
							break;
						}
					}
				}
				if (HitFlag) {		// 壁に当たっていたら壁から押し出す処理を行う
					for (int k = 0; k < 16; ++k) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
						bool HitF = false;
						for (auto& kabe : kabes) {
							// プレイヤーと当たっているかを判定
							if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
								*EndPos += Vector3DX(kabe.Normal) * (0.015f * Scale_Rate);					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
								bool j = false;
								for (auto& kabe2 : kabes) {
									// 当たっていたらループを抜ける
									if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
										j = true;
										break;
									}
								}
								if (!j) {// 全てのポリゴンと当たっていなかったらここでループ終了
									break;
								}
								HitF = true;
							}
						}
						if (!HitF) {//全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
							break;
						}
					}
				}
				kabes.clear();
			}
			return HitFlag;
		}


		void		BackGroundClass::LoadCellsFile() noexcept {
			std::ifstream fin;
			fin.open("data/Map.txt", std::ios::in | std::ios::binary);
			fin.read((char*)&m_CellxN.back().Xall, sizeof(m_CellxN.back().Xall));
			fin.read((char*)&m_CellxN.back().Yall, sizeof(m_CellxN.back().Yall));
			fin.read((char*)&m_CellxN.back().Zall, sizeof(m_CellxN.back().Zall));
			m_CellxN.back().scaleRate = 1;
			m_CellxN.back().m_Cell.resize((size_t)(m_CellxN.back().Xall * m_CellxN.back().Yall * m_CellxN.back().Zall));
			fin.read((char*)&m_CellxN.back().m_Cell.at(0), sizeof(m_CellxN.back().m_Cell.at(0)) * m_CellxN.back().m_Cell.size());
			fin.close();
		}
		void		BackGroundClass::SaveCellsFile() noexcept {
			std::ofstream fout;
			fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
			fout.write((char*)&m_CellxN.back().Xall, sizeof(m_CellxN.back().Xall));
			fout.write((char*)&m_CellxN.back().Yall, sizeof(m_CellxN.back().Yall));
			fout.write((char*)&m_CellxN.back().Zall, sizeof(m_CellxN.back().Zall));
			fout.write((char*)&m_CellxN.back().m_Cell.at(0), sizeof(m_CellxN.back().m_Cell.at(0)) * m_CellxN.back().m_Cell.size());
			fout.close();  //ファイルを閉じる
		}

		//簡略版を制作
		void		BackGroundClass::MakeSimple() noexcept {
			for (int sel = total - 1 - 1; sel >= 0; sel--) {
				auto& cell = m_CellxN.at(sel);
				auto& cell2 = m_CellxN.at((size_t)(sel + 1));

				cell.scaleRate = (int8_t)pow(MulPer, total - 1 - sel);
				cell.Xall = m_CellxN.back().Xall / cell.scaleRate;
				cell.Yall = m_CellxN.back().Yall / cell.scaleRate;
				cell.Zall = m_CellxN.back().Zall / cell.scaleRate;
				cell.m_Cell.resize((size_t)(cell.Xall * cell.Yall * cell.Zall));

				for (int xm = -cell.Xall / 2; xm < cell.Xall / 2; xm++) {
					for (int ym = -cell.Yall / 2; ym < cell.Yall / 2; ym++) {
						for (int zm = -cell.Zall / 2; zm < cell.Zall / 2; zm++) {
							int FillCount = 0;
							int FillAll = 0;


							int xMaxmin = std::max(xm * MulPer, -cell2.Xall / 2);
							int xMaxmax = std::min((xm + 1) * MulPer, cell2.Xall / 2);
							int yMaxmin = std::max(ym * MulPer, -cell2.Yall / 2);
							int yMaxmax = std::min((ym + 1) * MulPer, cell2.Yall / 2);
							int zMaxmin = std::max(zm * MulPer, -cell2.Zall / 2);
							int zMaxmax = std::min((zm + 1) * MulPer, cell2.Zall / 2);

							for (int x = xMaxmin; x < xMaxmax; x++) {
								for (int y = yMaxmin; y < yMaxmax; y++) {
									for (int z = zMaxmin; z < zMaxmax; z++) {
										const auto& Cell = cell2.GetCell(x, y, z);
										FillAll++;
										if (Cell == 0) { continue; }
										FillCount++;
									}
								}
							}
							cell.SetCell(xm, ym, zm) = ((FillAll != 0) && (static_cast<float>(FillCount) / FillAll >= (1.f / 2.f))) ? 1 : 0;
						}
					}
				}
			}
		}
		//遮蔽検索
		void		BackGroundClass::CalcOcclusion() noexcept {
			for (auto& cell : m_CellxN) {
				for (int x = -cell.Xall / 2; x < cell.Xall / 2; x++) {
					for (int y = -cell.Yall / 2; y < cell.Yall / 2; y++) {
						for (int z = -cell.Zall / 2; z < cell.Zall / 2; z++) {
							if (cell.GetCell(x, y, z) == 0) { continue; }
							//端は全て隠す
							if (
								((x == -cell.Xall / 2) || (x == cell.Xall / 2 - 1)) ||
								((y == -cell.Yall / 2) || (y == cell.Yall / 2 - 1)) ||
								((z == -cell.Zall / 2) || (z == cell.Zall / 2 - 1))
								) {
								cell.SetCell(x, y, z) = (int8_t)-std::abs(cell.SetCell(x, y, z));
								continue;
							}
							cell.SetCell(x, y, z) = (int8_t)((
								(cell.GetCell(x + 1, y, z) != 0) &&
								(cell.GetCell(x - 1, y, z) != 0) &&
								(cell.GetCell(x, y + 1, z) != 0) &&
								(cell.GetCell(x, y - 1, z) != 0) &&
								(cell.GetCell(x, y, z + 1) != 0) &&
								(cell.GetCell(x, y, z - 1) != 0)
								) ? -std::abs(cell.SetCell(x, y, z)) : std::abs(cell.SetCell(x, y, z)));
						}
					}
				}
			}
		}
		//
		void BackGroundClass::SetBlick(int x, int y, int z, int8_t sel) noexcept {
			if (
				((x < -m_CellxN.back().Xall / 2) || (x > m_CellxN.back().Xall / 2 - 1)) ||
				((y < -m_CellxN.back().Yall / 2) || (y > m_CellxN.back().Yall / 2 - 1)) ||
				((z < -m_CellxN.back().Zall / 2) || (z > m_CellxN.back().Zall / 2 - 1))
				) {
				return;
			}
			m_CellxN.back().SetCell(x, y, z) = sel;
			//簡易版を更新
			for (int i = total - 1 - 1; i >= 0; i--) {
				auto& cell = m_CellxN.at(i);
				auto& cell2 = m_CellxN.at((size_t)(i + 1));

				int FillCount = 0;
				int FillAll = 0;

				int xm = x / cell.scaleRate;
				int ym = y / cell.scaleRate;
				int zm = z / cell.scaleRate;

				int xMaxmin = std::max(xm * MulPer, -cell2.Xall / 2);
				int xMaxmax = std::min((xm + 1) * MulPer, cell2.Xall / 2);
				int yMaxmin = std::max(ym * MulPer, -cell2.Yall / 2);
				int yMaxmax = std::min((ym + 1) * MulPer, cell2.Yall / 2);
				int zMaxmin = std::max(zm * MulPer, -cell2.Zall / 2);
				int zMaxmax = std::min((zm + 1) * MulPer, cell2.Zall / 2);

				for (int xt = xMaxmin; xt < xMaxmax; xt++) {
					for (int yt = yMaxmin; yt < yMaxmax; yt++) {
						for (int zt = zMaxmin; zt < zMaxmax; zt++) {
							const auto& Cell = cell2.GetCell(xt, yt, zt);
							FillAll++;
							if (Cell == 0) { continue; }
							FillCount++;
						}
					}
				}
				cell.SetCell(xm, ym, zm) = ((FillAll != 0) && (static_cast<float>(FillCount) / FillAll >= (1.f / 2.f))) ? 1 : 0;
			}
			for (auto& cell : m_CellxN) {
				int xm = x / cell.scaleRate;
				int ym = y / cell.scaleRate;
				int zm = z / cell.scaleRate;
				auto CheckCell = [&](int xp, int yp, int zp) {
					if (
						((xp < -cell.Xall / 2) || (xp > cell.Xall / 2 - 1)) ||
						((yp < -cell.Yall / 2) || (yp > cell.Yall / 2 - 1)) ||
						((zp < -cell.Zall / 2) || (zp > cell.Zall / 2 - 1))
						) {
						return;
					}
					if (
						((xp == -cell.Xall / 2) || (xp == cell.Xall / 2 - 1)) ||
						((yp == -cell.Yall / 2) || (yp == cell.Yall / 2 - 1)) ||
						((zp == -cell.Zall / 2) || (zp == cell.Zall / 2 - 1))
						) {
						cell.SetCell(xp, yp, zp) = (int8_t)-std::abs(cell.SetCell(x, y, z));
						return;
					}
					if (cell.GetCell(xm, ym, zm) == 0) {
						cell.SetCell(xp, yp, zp) = (int8_t)std::abs(cell.SetCell(xp, yp, zp));
					}
					else if(cell.SetCell(xp, yp, zp) < 0){
						cell.SetCell(xp, yp, zp) = (int8_t)((
							(cell.GetCell(xp + 1, yp, zp) != 0) &&
							(cell.GetCell(xp - 1, yp, zp) != 0) &&
							(cell.GetCell(xp, yp + 1, zp) != 0) &&
							(cell.GetCell(xp, yp - 1, zp) != 0) &&
							(cell.GetCell(xp, yp, zp + 1) != 0) &&
							(cell.GetCell(xp, yp, zp - 1) != 0)
							) ? -std::abs(cell.SetCell(xp, yp, zp)) : std::abs(cell.SetCell(xp, yp, zp)));
					}
					};
				CheckCell(xm + 1, ym, zm);
				CheckCell(xm - 1, ym, zm);
				CheckCell(xm, ym + 1, zm);
				CheckCell(xm, ym - 1, zm);
				CheckCell(xm, ym, zm + 1);
				CheckCell(xm, ym, zm - 1);
			}
		}
	}
}
