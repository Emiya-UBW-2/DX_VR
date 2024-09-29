#include "CubeEditer.hpp"

void		CubeEditer::Bresenham3D(const int x1, const int y1, const int z1, const int x2, const int y2, const int z2, const std::function<bool(int, int, int)>& OutPutLine) noexcept {

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

void		CubeEditer::AddCube(int x, int y, int z) noexcept {
	if (
		((x < -m_CellsData.Xall / 2) || (x > m_CellsData.Xall / 2 - 1)) ||
		((y < -m_CellsData.Yall / 2) || (y > m_CellsData.Yall / 2 - 1)) ||
		((z < -m_CellsData.Zall / 2) || (z > m_CellsData.Zall / 2 - 1))
		) {
		return;
	}
	if (m_CellsData.SetCell(x, y, z).selset != INVALID_ID) { return; }

	m_CellsData.SetCell(x, y, z).selset = m_vert32.size() / (6 * 4);
	VECTOR Pos = VGet(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	VECTOR PosMin = Pos;
	VECTOR PosMax = VAdd(Pos, VGet(1.f, 1.f, 1.f));
	auto GetPlane = [&](const VECTOR& Pos1, const VECTOR& Pos2, const VECTOR& Pos3, const VECTOR& Pos4, const VECTOR& Normal) {

		size_t Now = m_vert32.size();
		size_t Nowi = m_index32.size();
		m_vert32.resize(m_vert32.size() + 4);
		m_index32.resize(m_index32.size() + 6);

		m_index32.at(Nowi) = (uint32_t)(Now);
		m_index32.at(Nowi + 1) = (uint32_t)(Now + 1);
		m_index32.at(Nowi + 2) = (uint32_t)(Now + 2);
		m_index32.at(Nowi + 3) = (uint32_t)(Now + 3);
		m_index32.at(Nowi + 4) = (uint32_t)(Now + 2);
		m_index32.at(Nowi + 5) = (uint32_t)(Now + 1);

		m_vert32.at(Now).pos = Pos1;
		m_vert32.at(Now).u = 1.0f;
		m_vert32.at(Now).v = 0.0f;
		m_vert32.at(Now + 1).pos = Pos2;
		m_vert32.at(Now + 1).u = 0.0f;
		m_vert32.at(Now + 1).v = 0.0f;
		m_vert32.at(Now + 2).pos = Pos3;
		m_vert32.at(Now + 2).u = 1.0f;
		m_vert32.at(Now + 2).v = 1.0f;
		m_vert32.at(Now + 3).pos = Pos4;
		m_vert32.at(Now + 3).u = 0.0f;
		m_vert32.at(Now + 3).v = 1.0f;
		for (size_t i = 0; i < 4; i++) {
			m_vert32.at(Now + i).norm = Normal;
			m_vert32.at(Now + i).dif = GetColorU8(255, 0, 0, 255);
			m_vert32.at(Now + i).spc = GetColorU8(255, 255, 255, 255);
			m_vert32.at(Now + i).su = m_vert32.at(Now + i).u;
			m_vert32.at(Now + i).sv = m_vert32.at(Now + i).v;
			/*
			m_vert32.at(Now + i).spos.x = m_vert32.at(Now + i).pos.x;
			m_vert32.at(Now + i).spos.y = m_vert32.at(Now + i).pos.y;
			m_vert32.at(Now + i).spos.z = m_vert32.at(Now + i).pos.z;
			m_vert32.at(Now + i).spos.x = 1.0f;

			m_vert32.at(Now + i).binorm = Normal;
			m_vert32.at(Now + i).tan = Normal;
			//*/
		}
		};
	// 頂点データの作成
	GetPlane(VGet(PosMin.x, PosMax.y, PosMin.z), VGet(PosMax.x, PosMax.y, PosMin.z), PosMin, VGet(PosMax.x, PosMin.y, PosMin.z), VGet(0, 0, -1));
	GetPlane(VGet(PosMin.x, PosMin.y, PosMax.z), VGet(PosMax.x, PosMin.y, PosMax.z), VGet(PosMin.x, PosMax.y, PosMax.z), PosMax, VGet(0, 0, 1));
	GetPlane(VGet(PosMin.x, PosMax.y, PosMax.z), VGet(PosMin.x, PosMax.y, PosMin.z), VGet(PosMin.x, PosMin.y, PosMax.z), PosMin, VGet(-1, 0, 0));
	GetPlane(VGet(PosMax.x, PosMax.y, PosMin.z), PosMax, VGet(PosMax.x, PosMin.y, PosMin.z), VGet(PosMax.x, PosMin.y, PosMax.z), VGet(1, 0, 0));
	GetPlane(VGet(PosMin.x, PosMax.y, PosMax.z), PosMax, VGet(PosMin.x, PosMax.y, PosMin.z), VGet(PosMax.x, PosMax.y, PosMin.z), VGet(0, 1, 0));
	GetPlane(PosMin, VGet(PosMax.x, PosMin.y, PosMin.z), VGet(PosMin.x, PosMin.y, PosMax.z), VGet(PosMax.x, PosMin.y, PosMax.z), VGet(0, -1, 0));
}
void		CubeEditer::DelCube(int x, int y, int z) noexcept {
	if (
		((x < -m_CellsData.Xall / 2) || (x > m_CellsData.Xall / 2 - 1)) ||
		((y < -m_CellsData.Yall / 2) || (y > m_CellsData.Yall / 2 - 1)) ||
		((z < -m_CellsData.Zall / 2) || (z > m_CellsData.Zall / 2 - 1))
		) {
		return;
	}
	int sel = m_CellsData.GetCell(x, y, z).selset;
	if (sel == INVALID_ID) { return; }
	for (int i = 0; i < (6 * 4); i++) {
		m_vert32.erase(m_vert32.begin() + (sel * (6 * 4)));
	}
	for (int i = 0; i < (6 * 6); i++) {
		m_index32.erase(m_index32.begin() + (sel * (6 * 6)));
	}
	for (int i = (sel * (6 * 6)); i < m_index32.size(); i++) {
		m_index32.at(i) -= (6 * 4);
	}

	m_CellsData.SetCell(x, y, z).selset = INVALID_ID;
	for (int xt = -m_CellsData.Xall / 2; xt < m_CellsData.Xall / 2; xt++) {
		for (int zt = -m_CellsData.Zall / 2; zt < m_CellsData.Zall / 2; zt++) {
			for (int yt = -m_CellsData.Yall / 2; yt < m_CellsData.Yall / 2; yt++) {
				if (m_CellsData.GetCell(xt, yt, zt).selset == INVALID_ID) { continue; }
				if (sel < m_CellsData.GetCell(xt, yt, zt).selset) {
					m_CellsData.SetCell(xt, yt, zt).selset--;
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------
void		CubeEditer::Init() noexcept {
	m_CellsData.Xall = 100;
	m_CellsData.Yall = 100;
	m_CellsData.Zall = 100;
	m_CellsData.m_Cell.resize((size_t)(m_CellsData.Xall * m_CellsData.Yall * m_CellsData.Zall));
	for (int x = -m_CellsData.Xall / 2; x < m_CellsData.Xall / 2; x++) {
		for (int z = -m_CellsData.Zall / 2; z < m_CellsData.Zall / 2; z++) {
			for (int y = -m_CellsData.Yall / 2; y < m_CellsData.Yall / 2; y++) {
				m_CellsData.SetCell(x, y, z).selset = INVALID_ID;
			}
		}
	}

	MATERIALPARAM Param;
	Param.Diffuse = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);						// ディフューズカラー
	Param.Ambient = GetColorF(0.15f, 0.15f, 0.15f, 1.0f);						// アンビエントカラー
	Param.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// スペキュラカラー
	Param.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// エミッシブカラー
	Param.Power = 100.f;							// スペキュラハイライトの鮮明度
	SetMaterialParam(Param);
}
void		CubeEditer::DrawBG() noexcept {
	for (int x = -50; x <= 50; x++) {
		DrawLine3D(Vector3DX::vget((float)x * 1.f, 0.f, -50.f).get(), Vector3DX::vget((float)x * 1.f, 0.f, 50.f).get(), (std::abs(x) % 3 == 0) ? White : Gray50);
	}
	for (int z = -50; z <= 50; z++) {
		DrawLine3D(Vector3DX::vget(-50.f, 0.f, (float)z * 1.f).get(), Vector3DX::vget(50.f, 0.f, (float)z * 1.f).get(), (std::abs(z) % 3 == 0) ? White : Gray50);
	}
	DrawLine3D(Vector3DX::vget(0.f, 0.f, 0.f).get(), Vector3DX::vget(10000.f, 0.f, 0.f).get(), Red);
	DrawLine3D(Vector3DX::vget(0.f, 0.f, 0.f).get(), Vector3DX::vget(0.f, 10000.f, 0.f).get(), Green);
	DrawLine3D(Vector3DX::vget(0.f, 0.f, 0.f).get(), Vector3DX::vget(0.f, 0.f, 10000.f).get(), Blue);
}
void		CubeEditer::Draw() noexcept {
	if (m_vert32.size() > 0 && m_index32.size() > 0) {
		DrawPolygon32bitIndexed3D(m_vert32.data(), static_cast<int>(m_vert32.size()), m_index32.data(), static_cast<int>(m_index32.size() / 3), DX_NONE_GRAPH, FALSE);
	}
}
