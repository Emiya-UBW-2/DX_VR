#include	"BackGround.hpp"

const FPS_n2::BackGround::BackGroundClass* SingletonBase<FPS_n2::BackGround::BackGroundClass>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace BackGround {
		bool		BackGroundClass::CalcIntersectionPoint(const Vector3DX& pointA, const Vector3DX& pointB, const Vector3DX& planePos, const Vector3DX& planenormal, Vector3DX* pointIntersection, bool* pSameVecNormalToA, bool* pOnFront) noexcept {
			// �����ɓ�����Ȃ�
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




			auto GetPlane = [&](const Vector3DX& Pos1, const Vector3DX& Pos2, const Vector3DX& Pos3, const Vector3DX& Pos4, const VECTOR& Normal) {
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
				if (CheckFill && cell.IsActiveCell(x + static_cast<int>(Normal.x), y + static_cast<int>(Normal.y), z + static_cast<int>(Normal.z))) {
					return;
				}

				size_t Now = m_32Num;
				m_32Num++;
				if (m_32Num > m_32Size) {
					m_vert32.resize(m_32Num * 4);
					m_index32.resize(m_32Num * 6);
					m_32Size = m_32Num;
				}

				float Xofs = 1.0f - Normal.y;
				float uMin = (0.f + Xofs) / 8.f;
				float vMin = 0.f / 8.f;
				float uMax = uMin + 1.f / 8.f;
				float vMax = vMin + 1.f / 8.f;
				{
					auto& v = m_vert32.at(Now * 4);
					v.pos = Pos1.get();
					v.u = uMax;
					v.v = vMin;
					v.norm = Normal;
					v.dif = DifColor;
					v.spc = SpcColor;
				}
				{
					auto& v = m_vert32.at(Now * 4 + 1);
					v.pos = Pos2.get();
					v.u = uMin;
					v.v = vMin;
					v.norm = Normal;
					v.dif = DifColor;
					v.spc = SpcColor;
				}
				{
					auto& v = m_vert32.at(Now * 4 + 2);
					v.pos = Pos3.get();
					v.u = uMax;
					v.v = vMax;
					v.norm = Normal;
					v.dif = DifColor;
					v.spc = SpcColor;
				}
				{
					auto& v = m_vert32.at(Now * 4 + 3);
					v.pos = Pos4.get();
					v.u = uMin;
					v.v = vMax;
					v.norm = Normal;
					v.dif = DifColor;
					v.spc = SpcColor;
				}
				m_index32.at(Now * 6) = (uint32_t)(Now * 4);
				m_index32.at(Now * 6 + 1) = (uint32_t)(Now * 4 + 1);
				m_index32.at(Now * 6 + 2) = (uint32_t)(Now * 4 + 2);
				m_index32.at(Now * 6 + 3) = (uint32_t)(Now * 4 + 3);
				m_index32.at(Now * 6 + 4) = (uint32_t)(Now * 4 + 2);
				m_index32.at(Now * 6 + 5) = (uint32_t)(Now * 4 + 1);
				};
			// ���_�f�[�^�̍쐬
			GetPlane(
				Vector3DX::vget(PosMin.x, PosMax.y, PosMin.z), Vector3DX::vget(PosMax.x, PosMax.y, PosMin.z), PosMin, Vector3DX::vget(PosMax.x, PosMin.y, PosMin.z),
				Vector3DX::back().get());

			GetPlane(
				Vector3DX::vget(PosMin.x, PosMin.y, PosMax.z), Vector3DX::vget(PosMax.x, PosMin.y, PosMax.z), Vector3DX::vget(PosMin.x, PosMax.y, PosMax.z), PosMax,
				Vector3DX::forward().get());

			GetPlane(
				Vector3DX::vget(PosMin.x, PosMax.y, PosMax.z), Vector3DX::vget(PosMin.x, PosMax.y, PosMin.z), Vector3DX::vget(PosMin.x, PosMin.y, PosMax.z), PosMin,
				Vector3DX::left().get());

			GetPlane(
				Vector3DX::vget(PosMax.x, PosMax.y, PosMin.z), PosMax, Vector3DX::vget(PosMax.x, PosMin.y, PosMin.z), Vector3DX::vget(PosMax.x, PosMin.y, PosMax.z),
				Vector3DX::right().get());

			GetPlane(
				Vector3DX::vget(PosMin.x, PosMax.y, PosMax.z), PosMax, Vector3DX::vget(PosMin.x, PosMax.y, PosMin.z), Vector3DX::vget(PosMax.x, PosMax.y, PosMin.z),
				Vector3DX::up().get());

			GetPlane(
				PosMin, Vector3DX::vget(PosMax.x, PosMin.y, PosMin.z), Vector3DX::vget(PosMin.x, PosMin.y, PosMax.z), Vector3DX::vget(PosMax.x, PosMin.y, PosMax.z),
				Vector3DX::down().get());
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
			// ���_�f�[�^�̍쐬
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
			// ��������
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
						return false; // �������Ă��Ȃ�
				}
				else {
					// �X���u�Ƃ̋������Z�o
					// t1���߃X���u�At2�����X���u�Ƃ̋���
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
						//�ǂ̌����H
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

					// �X���u�����`�F�b�N
					if (t >= t_max) {
						return false;
					}
				}
			}

			// �������Ă���
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
			// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
			std::vector<MV1_COLL_RESULT_POLY> kabes;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
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
			// �ǃ|���S���Ƃ̓����蔻�菈��
			if (kabes.size() > 0) {
				HitFlag = false;
				for (auto& kabe : kabes) {
					// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
						HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
						if (MoveVector.magnitude() >= 0.001f) {	// x����z�������� 0.001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
							// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
							*EndPos = Vector3DX::Cross(kabe.Normal, Vector3DX::Cross(MoveVector, kabe.Normal)) + StartPos;
							//EndPos->y(StartPos.y);
							bool j = false;
							for (auto& kabe2 : kabes) {
								if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
									j = true;
									break;// �������Ă����烋�[�v���甲����
								}
							}
							if (!j) {
								HitFlag = false;
								break;//�ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂ŕǂɓ��������t���O��|������Ń��[�v���甲����
							}
						}
						else {
							break;
						}
					}
				}
				if (HitFlag) {		// �ǂɓ������Ă�����ǂ��牟���o���������s��
					for (int k = 0; k < 16; ++k) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
						bool HitF = false;
						for (auto& kabe : kabes) {
							// �v���C���[�Ɠ������Ă��邩�𔻒�
							if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
								*EndPos += Vector3DX(kabe.Normal) * (0.015f * Scale_Rate);					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
								bool j = false;
								for (auto& kabe2 : kabes) {
									// �������Ă����烋�[�v�𔲂���
									if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
										j = true;
										break;
									}
								}
								if (!j) {// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
									break;
								}
								HitF = true;
							}
						}
						if (!HitF) {//�S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
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
			fout.close();  //�t�@�C�������
		}

		//�ȗ��ł𐧍�
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
		//�Օ�����
		void		BackGroundClass::CalcOcclusion() noexcept {
			for (auto& cell : m_CellxN) {
				for (int x = -cell.Xall / 2; x < cell.Xall / 2; x++) {
					for (int y = -cell.Yall / 2; y < cell.Yall / 2; y++) {
						for (int z = -cell.Zall / 2; z < cell.Zall / 2; z++) {
							if (!cell.IsActiveCell(x, y, z)) { continue; }
							//�[�͑S�ĉB��
							if (
								((x == -cell.Xall / 2) || (x == cell.Xall / 2 - 1)) ||
								((y == -cell.Yall / 2) || (y == cell.Yall / 2 - 1)) ||
								((z == -cell.Zall / 2) || (z == cell.Zall / 2 - 1))
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
			if (
				((x < -m_CellxN.back().Xall / 2) || (x > m_CellxN.back().Xall / 2 - 1)) ||
				((y < -m_CellxN.back().Yall / 2) || (y > m_CellxN.back().Yall / 2 - 1)) ||
				((z < -m_CellxN.back().Zall / 2) || (z > m_CellxN.back().Zall / 2 - 1))
				) {
				return;
			}
			m_CellxN.back().SetCell(x, y, z) = sel;
			//�ȈՔł��X�V
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
