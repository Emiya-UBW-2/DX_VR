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
			float scale = CellScale * cell.scaleRate;
			Vector3DX camPos = DrawParts->GetMainCamera().GetCamPos() / scale;


			auto GetPlane = [&](int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int xN, int yN, int zN) {
				if (CheckFill && cell.IsActiveCell(x + xN, y + yN, z + zN)) { return; }

				if (m_32Num + 1 > m_32Size) {
					m_32Size = m_32Num + 1;
					m_vert32.resize(m_32Size * 4);
					m_index32.resize(m_32Size * 6);
				}

				float Xofs = 1.0f - static_cast<float>(yN);
				float uvAdd = 1.f / 8.f;
				float uMin = Xofs * uvAdd;
				float vMin = 0.f * uvAdd;

				auto AddPointPos = [&](int xt, int yt, int u, int v, int sel) {
					Vector3DX Pos;
					if (xN != 0) {
						Pos = Vector3DX::vget(static_cast<float>(x + (xN + 1) / 2), static_cast<float>(y + xt), static_cast<float>(z + yt)) * scale;
					}
					else if (yN != 0) {
						Pos = Vector3DX::vget(static_cast<float>(x + xt), static_cast<float>(y + (yN + 1) / 2), static_cast<float>(z + yt)) * scale;
					}
					else if (zN != 0) {
						Pos = Vector3DX::vget(static_cast<float>(x + xt), static_cast<float>(y + yt), static_cast<float>(z + (zN + 1) / 2)) * scale;
					}

					auto& V = m_vert32.at(m_32Num * 4 + sel);
					V.pos = Pos.get();
					V.u = uMin + uvAdd * static_cast<float>(u);
					V.v = vMin + uvAdd * static_cast<float>(v);
					V.norm = VGet(static_cast<float>(xN), static_cast<float>(yN), static_cast<float>(zN));
					V.dif = DifColor;
					V.spc = SpcColor;
					};

				AddPointPos(x1, y1, 1, 0, 0);
				AddPointPos(x2, y2, 0, 0, 1);
				AddPointPos(x3, y3, 1, 1, 2);
				AddPointPos(x4, y4, 0, 1, 3);
				m_index32.at(m_32Num * 6) = (uint32_t)(m_32Num * 4);
				m_index32.at(m_32Num * 6 + 1) = (uint32_t)(m_32Num * 4 + 1);
				m_index32.at(m_32Num * 6 + 2) = (uint32_t)(m_32Num * 4 + 2);
				m_index32.at(m_32Num * 6 + 3) = (uint32_t)(m_32Num * 4 + 3);
				m_index32.at(m_32Num * 6 + 4) = (uint32_t)(m_32Num * 4 + 2);
				m_index32.at(m_32Num * 6 + 5) = (uint32_t)(m_32Num * 4 + 1);
				m_32Num++;
				};
			// 頂点データの作成
			if (z >= camPos.z) {
				GetPlane(
					0, 1,
					1, 1,
					0, 0,
					1, 0,
					0, 0, -1);
			}

			if (z + 1 <= camPos.z) {
				GetPlane(
					0, 0,
					1, 0,
					0, 1,
					1, 1,
					0, 0, 1);
			}

			if (x >= camPos.x) {
				GetPlane(
					1, 1,
					1, 0,
					0, 1,
					0, 0,
					-1, 0, 0);
			}
			if (x + 1 <= camPos.x) {
				GetPlane(
					1, 0,
					1, 1,
					0, 0,
					0, 1,
					1, 0, 0);
			}

			if (y + 1 <= camPos.y) {
				GetPlane(
					0, 1,
					1, 1,
					0, 0,
					1, 0,
					0, 1, 0);
			}

			if (y >= camPos.y) {
				GetPlane(
					0, 0,
					1, 0,
					0, 1,
					1, 1,
					0, -1, 0);
			}
		}

		void BackGroundClass::AddShadowCube(const CellsData& cell, int x, int y, int z, COLOR_U8 DifColor, COLOR_U8 SpcColor) noexcept {
			Vector3DX Pos = Vector3DX::vget(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
			Vector3DX PosMin = Pos * (CellScale * cell.scaleRate);
			Vector3DX PosMax = (Pos + Vector3DX::one()) * (CellScale * cell.scaleRate);

			auto GetPlane = [&](const Vector3DX& Pos1, const Vector3DX& Pos2, const Vector3DX& Pos3, const Vector3DX& Pos4, const Vector3DX& Normal) {
				if (cell.IsActiveCell(x + static_cast<int>(Normal.x), y + static_cast<int>(Normal.y), z + static_cast<int>(Normal.z))) {
					return;
				}

				size_t NowS = m_S32Num;
				m_S32Num++;
				if (m_S32Num > m_S32Size) {
					m_vert32S.resize(m_S32Num * 4);
					m_vert32SB.resize(m_S32Num * 4);
					m_index32S.resize(m_S32Num * 6);
					m_S32Size = m_S32Num;
				}

				m_vert32S.at(NowS * 4).pos = Pos1.get();
				m_vert32S.at(NowS * 4 + 1).pos = Pos2.get();
				m_vert32S.at(NowS * 4 + 2).pos = Pos3.get();
				m_vert32S.at(NowS * 4 + 3).pos = Pos4.get();

				m_vert32SB.at(NowS * 4).pos = Pos1.get();
				m_vert32SB.at(NowS * 4 + 1).pos = Pos2.get();
				m_vert32SB.at(NowS * 4 + 2).pos = Pos3.get();
				m_vert32SB.at(NowS * 4 + 3).pos = Pos4.get();
				for (size_t i = 0; i < 4; i++) {
					m_vert32SB.at(NowS * 4 + i).norm = Normal.get();
					m_vert32SB.at(NowS * 4 + i).dif = DifColor;
					m_vert32SB.at(NowS * 4 + i).spc = SpcColor;
				}

				m_index32S.at(NowS * 6) = (uint32_t)(NowS * 4);
				m_index32S.at(NowS * 6 + 1) = (uint32_t)(NowS * 4 + 1);
				m_index32S.at(NowS * 6 + 2) = (uint32_t)(NowS * 4 + 2);
				m_index32S.at(NowS * 6 + 3) = (uint32_t)(NowS * 4 + 3);
				m_index32S.at(NowS * 6 + 4) = (uint32_t)(NowS * 4 + 2);
				m_index32S.at(NowS * 6 + 5) = (uint32_t)(NowS * 4 + 1);
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
								((x <= -cell.Xharf) || (cell.Xharf - 1 < x)) ||
								((y <= -cell.Yharf) || (cell.Yharf - 1 < y)) ||
								((z <= -cell.Zharf) || (cell.Zharf - 1 < z))
								) {
								return false;
							}
							if (!cell.IsDrawCell(x, y, z)) { return false; }
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
								((x <= -cell.Xharf) || (cell.Xharf - 1 < x)) ||
								((y <= -cell.Yharf) || (cell.Yharf - 1 < y)) ||
								((z <= -cell.Zharf) || (cell.Zharf - 1 < z))
								) {
								return false;
							}
							if (!cell.IsDrawCell(x, y, z)) { return false; }
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
			auto& cell = m_CellxN.back();
			fin.read((char*)&cell.Xall, sizeof(cell.Xall));
			fin.read((char*)&cell.Yall, sizeof(cell.Yall));
			fin.read((char*)&cell.Zall, sizeof(cell.Zall));
			cell.Xharf = cell.Xall / 2;
			cell.Yharf = cell.Yall / 2;
			cell.Zharf = cell.Zall / 2;
			cell.scaleRate = 1;
			cell.m_Cell.resize((size_t)(cell.Xall * cell.Yall * cell.Zall));
			fin.read((char*)&cell.m_Cell.at(0), sizeof(cell.m_Cell.at(0)) * cell.m_Cell.size());
			/*
			{
				auto& cell = m_CellxN.back();
				cell.Xall = 256;
				cell.Yall = 256;
				cell.Zall = 256;
				cell.Xharf = cell.Xall / 2;
				cell.Yharf = cell.Yall / 2;
				cell.Zharf = cell.Zall / 2;
				cell.scaleRate = 1;
				cell.m_Cell.resize((size_t)(cell.Xall * cell.Yall * cell.Zall));

				for (int x = -cell.Xharf; x < cell.Xharf; x++) {
					for (int y = -cell.Yharf; y < cell.Yharf; y++) {
						for (int z = -cell.Zharf; z < cell.Zharf; z++) {
							cell.SetCell(x, y, z) = s_EmptyBlick;
							if (
								(-m_CellxN2.Xharf <= x && x < m_CellxN2.Xharf) &&
								(-m_CellxN2.Yharf <= y && y < m_CellxN2.Yharf) &&
								(-m_CellxN2.Zharf <= z && z < m_CellxN2.Zharf)
								) {
								cell.SetCell(x, y, z) = m_CellxN2.GetCell(x, y, z);
							}
						}
					}
				}
			}
			//*/
			fin.close();
		}
		void		BackGroundClass::SaveCellsFile() noexcept {
			std::ofstream fout;
			fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
			auto& cell = m_CellxN.back();
			fout.write((char*)&cell.Xall, sizeof(cell.Xall));
			fout.write((char*)&cell.Yall, sizeof(cell.Yall));
			fout.write((char*)&cell.Zall, sizeof(cell.Zall));
			fout.write((char*)&cell.m_Cell.at(0), sizeof(cell.m_Cell.at(0)) * cell.m_Cell.size());
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
				cell.Xharf = cell.Xall / 2;
				cell.Yharf = cell.Yall / 2;
				cell.Zharf = cell.Zall / 2;
				cell.m_Cell.resize((size_t)(cell.Xall * cell.Yall * cell.Zall));

				for (int xm = -cell.Xharf; xm < cell.Xharf; xm++) {
					for (int ym = -cell.Yharf; ym < cell.Yharf; ym++) {
						for (int zm = -cell.Zharf; zm < cell.Zharf; zm++) {
							int FillCount = 0;
							int FillAll = 0;


							int xMaxmin = std::max(xm * MulPer, -cell2.Xharf);
							int xMaxmax = std::min((xm + 1) * MulPer, cell2.Xharf);
							int yMaxmin = std::max(ym * MulPer, -cell2.Yharf);
							int yMaxmax = std::min((ym + 1) * MulPer, cell2.Yharf);
							int zMaxmin = std::max(zm * MulPer, -cell2.Zharf);
							int zMaxmax = std::min((zm + 1) * MulPer, cell2.Zharf);

							for (int x = xMaxmin; x < xMaxmax; x++) {
								for (int y = yMaxmin; y < yMaxmax; y++) {
									for (int z = zMaxmin; z < zMaxmax; z++) {
										FillAll++;
										if (!cell2.IsActiveCell(x, y, z)) { continue; }
										FillCount++;
									}
								}
							}
							cell.SetCell(xm, ym, zm) = ((FillAll != 0) && (static_cast<float>(FillCount) / FillAll >= (1.f / 2.f))) ? 1 : s_EmptyBlick;
						}
					}
				}
			}
		}
		//遮蔽検索
		void		BackGroundClass::CalcOcclusion() noexcept {
			for (auto& cell : m_CellxN) {
				for (int x = -cell.Xharf; x < cell.Xharf; x++) {
					for (int y = -cell.Yharf; y < cell.Yharf; y++) {
						for (int z = -cell.Zharf; z < cell.Zharf; z++) {
							if (!cell.IsActiveCell(x, y, z)) { continue; }
							//端は全て隠す
							if (
								((x == -cell.Xharf) || (x == cell.Xharf - 1)) ||
								((y == -cell.Yharf) || (y == cell.Yharf - 1)) ||
								((z == -cell.Zharf) || (z == cell.Zharf - 1))
								) {
								cell.SetCellOnBlick(x, y, z, true);
								continue;
							}
							cell.SetCellOnBlick(x, y, z,
								cell.IsActiveCell(x + 1, y, z) &&
								cell.IsActiveCell(x - 1, y, z) &&
								cell.IsActiveCell(x, y + 1, z) &&
								cell.IsActiveCell(x, y - 1, z) &&
								cell.IsActiveCell(x, y, z + 1) &&
								cell.IsActiveCell(x, y, z - 1)
							);
						}
					}
				}
			}
		}
		//
		void BackGroundClass::SetBlick(int x, int y, int z, int8_t sel) noexcept {
			{
				auto& cell = m_CellxN.back();
				if (
					((x < -cell.Xharf) || (x > cell.Xharf - 1)) ||
					((y < -cell.Yharf) || (y > cell.Yharf - 1)) ||
					((z < -cell.Zharf) || (z > cell.Zharf - 1))
					) {
					return;
				}
				cell.SetCell(x, y, z) = sel;
			}
			//簡易版を更新
			for (int i = total - 1 - 1; i >= 0; i--) {
				auto& cell = m_CellxN.at(i);
				auto& cell2 = m_CellxN.at((size_t)(i + 1));

				int FillCount = 0;
				int FillAll = 0;

				int xm = x / cell.scaleRate;
				int ym = y / cell.scaleRate;
				int zm = z / cell.scaleRate;

				int xMaxmin = std::max(xm * MulPer, -cell2.Xharf);
				int xMaxmax = std::min((xm + 1) * MulPer, cell2.Xharf);
				int yMaxmin = std::max(ym * MulPer, -cell2.Yharf);
				int yMaxmax = std::min((ym + 1) * MulPer, cell2.Yharf);
				int zMaxmin = std::max(zm * MulPer, -cell2.Zharf);
				int zMaxmax = std::min((zm + 1) * MulPer, cell2.Zharf);

				for (int xt = xMaxmin; xt < xMaxmax; xt++) {
					for (int yt = yMaxmin; yt < yMaxmax; yt++) {
						for (int zt = zMaxmin; zt < zMaxmax; zt++) {
							FillAll++;
							if (!cell2.IsActiveCell(xt, yt, zt)) { continue; }
							FillCount++;
						}
					}
				}
				cell.SetCell(xm, ym, zm) = ((FillAll != 0) && (static_cast<float>(FillCount) / FillAll >= (1.f / 2.f))) ? 1 : s_EmptyBlick;
			}
			for (auto& cell : m_CellxN) {
				int xm = x / cell.scaleRate;
				int ym = y / cell.scaleRate;
				int zm = z / cell.scaleRate;
				auto CheckCell = [&](int xp, int yp, int zp) {
					if (
						((xp < -cell.Xharf) || (xp > cell.Xharf - 1)) ||
						((yp < -cell.Yharf) || (yp > cell.Yharf - 1)) ||
						((zp < -cell.Zharf) || (zp > cell.Zharf - 1))
						) {
						return;
					}
					if (
						((xp == -cell.Xharf) || (xp == cell.Xharf - 1)) ||
						((yp == -cell.Yharf) || (yp == cell.Yharf - 1)) ||
						((zp == -cell.Zharf) || (zp == cell.Zharf - 1))
						) {
						cell.SetCellOnBlick(xp, yp, zp, true);
						return;
					}
					if (!cell.IsActiveCell(xm, ym, zm)) {
						cell.SetCellOnBlick(xp, yp, zp, false);
					}
					else if (cell.GetCell(xp, yp, zp) < s_EmptyBlick) {
						cell.SetCellOnBlick(xp, yp, zp,
							cell.IsActiveCell(xp + 1, yp, zp) &&
							cell.IsActiveCell(xp - 1, yp, zp) &&
							cell.IsActiveCell(xp, yp + 1, zp) &&
							cell.IsActiveCell(xp, yp - 1, zp) &&
							cell.IsActiveCell(xp, yp, zp + 1) &&
							cell.IsActiveCell(xp, yp, zp - 1)
						);
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
