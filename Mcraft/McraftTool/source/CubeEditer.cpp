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

void		CubeEditer::AddCube(int x, int y, int z, int select, bool DrawCheckPoint) noexcept {
	if (
		((x < -m_CellsData.Xall / 2) || (x > m_CellsData.Xall / 2 - 1)) ||
		((y < -m_CellsData.Yall / 2) || (y > m_CellsData.Yall / 2 - 1)) ||
		((z < -m_CellsData.Zall / 2) || (z > m_CellsData.Zall / 2 - 1))
		) {
		return;
	}
	if (m_CellsData.SetCellNum(x, y, z) != INVALID_ID) { return; }

	m_CellsData.SetCellNum(x, y, z) = (int)(m_vert32.size()) / (6 * 4);
	m_CellsData.SetCell(x, y, z).select = select;
	VECTOR Pos = VGet(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	VECTOR PosMin = VAdd(Pos, VGet(0.005f, 0.005f, 0.005f));
	VECTOR PosMax = VAdd(Pos, VGet(0.995f, 0.995f, 0.995f));
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
			m_vert32.at(Now + i).dif = GetColorU8(std::clamp(255 - y * 25, 0, 255), 0, 0, 255);
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
	if (DrawCheckPoint) {
		if (m_SavePointDataNow != (int)m_SavePointData.size() - 1) {
			CheckPoint();
		}
		SavePointData tmp;
		tmp.x = x;
		tmp.y = y;
		tmp.z = z;
		tmp.prevselect = INVALID_ID;
		tmp.select = select;
		m_SavePointData.back().emplace_back(tmp);
	}
}
void		CubeEditer::DelCube(int x, int y, int z, bool DrawCheckPoint) noexcept {
	if (
		((x < -m_CellsData.Xall / 2) || (x > m_CellsData.Xall / 2 - 1)) ||
		((y < -m_CellsData.Yall / 2) || (y > m_CellsData.Yall / 2 - 1)) ||
		((z < -m_CellsData.Zall / 2) || (z > m_CellsData.Zall / 2 - 1))
		) {
		return;
	}
	if (m_CellsData.GetCell(x, y, z).select == INVALID_ID) { return; }
	int num = m_CellsData.GetCellNum(x, y, z);
	for (int i = 0; i < (6 * 4); i++) {
		m_vert32.erase(m_vert32.begin() + (size_t)(num * (6 * 4)));
	}
	for (int i = 0; i < (6 * 6); i++) {
		m_index32.erase(m_index32.begin() + (size_t)(num * (6 * 6)));
	}
	for (int i = (num * (6 * 6)); i < m_index32.size(); i++) {
		m_index32.at(i) -= (6 * 4);
	}
	int sel = m_CellsData.GetCell(x, y, z).select;
	m_CellsData.SetCell(x, y, z).select = INVALID_ID;
	m_CellsData.SetCellNum(x, y, z) = INVALID_ID;
	for (int xt = -m_CellsData.Xall / 2; xt < m_CellsData.Xall / 2; xt++) {
		for (int zt = -m_CellsData.Zall / 2; zt < m_CellsData.Zall / 2; zt++) {
			for (int yt = -m_CellsData.Yall / 2; yt < m_CellsData.Yall / 2; yt++) {
				if (m_CellsData.GetCell(xt, yt, zt).select == INVALID_ID) { continue; }
				if (num < m_CellsData.GetCellNum(xt, yt, zt)) {
					m_CellsData.SetCellNum(xt, yt, zt)--;
				}
			}
		}
	}
	if (DrawCheckPoint) {
		if (m_SavePointDataNow != (int)m_SavePointData.size() - 1) {
			CheckPoint();
		}
		SavePointData tmp;
		tmp.x = x;
		tmp.y = y;
		tmp.z = z;
		tmp.prevselect = sel;
		tmp.select = INVALID_ID;
		m_SavePointData.back().emplace_back(tmp);
	}
}
void CubeEditer::CheckPoint() noexcept {
	//そもそもセーブできるデータがない
	if (m_SavePointDataNow!=0 && m_SavePointData.at(m_SavePointDataNow).size()==0) {
		return;
	}
	if (m_SavePointDataNow != (int)m_SavePointData.size() - 1) {
		for (int i = m_SavePointDataNow, Max = (int)m_SavePointData.size(); i < Max; i++) {
			m_SavePointData.pop_back();
		}
	}
	std::vector<SavePointData> tmp;
	m_SavePointData.emplace_back(tmp);
	m_SavePointDataNow = (int)m_SavePointData.size() - 1;
}
void CubeEditer::UnDo() noexcept {
	if (m_SavePointDataNow == 0) { return; }
	m_SavePointDataNow--;
	for (auto i = m_SavePointData.at(m_SavePointDataNow).rbegin(), e = m_SavePointData.at(m_SavePointDataNow).rend(); i != e; ++i) {
		if (i->prevselect == INVALID_ID && i->select != INVALID_ID) {
			DelCube(i->x, i->y, i->z, false);
		}
		else if (i->prevselect != INVALID_ID && i->select == INVALID_ID) {
			AddCube(i->x, i->y, i->z, i->prevselect, false);
		}
	}
}
void CubeEditer::ReDo() noexcept {
	if (m_SavePointDataNow == (int)m_SavePointData.size() - 1) { return; }
	for (auto& i : m_SavePointData.at(m_SavePointDataNow)) {
		if (i.prevselect == INVALID_ID && i.select != INVALID_ID) {
			AddCube(i.x, i.y, i.z, i.select, false);
		}
		else if (i.prevselect != INVALID_ID && i.select == INVALID_ID) {
			DelCube(i.x, i.y, i.z, false);
		}
	}
	m_SavePointDataNow++;
}

void		CubeEditer::LoadCellsFile() noexcept {
	if (!std::filesystem::is_regular_file("Save/Map.txt")) {
		return;
	}
	m_vert32.clear();
	m_index32.clear();
	std::ifstream fin;
	fin.open("Save/Map.txt", std::ios::in | std::ios::binary);
	fin.read((char*)&m_CellsData.Xall, sizeof(m_CellsData.Xall));
	fin.read((char*)&m_CellsData.Yall, sizeof(m_CellsData.Yall));
	fin.read((char*)&m_CellsData.Zall, sizeof(m_CellsData.Zall));
	m_CellsData.m_number.resize((size_t)(m_CellsData.Xall * m_CellsData.Yall * m_CellsData.Zall));
	m_CellsData.m_Cell.resize((size_t)(m_CellsData.Xall * m_CellsData.Yall * m_CellsData.Zall));
	fin.read((char*)&m_CellsData.m_Cell.at(0), sizeof(m_CellsData.m_Cell.at(0)) * m_CellsData.m_Cell.size());
	fin.close();
	for (int x = -m_CellsData.Xall / 2; x < m_CellsData.Xall / 2; x++) {
		for (int z = -m_CellsData.Zall / 2; z < m_CellsData.Zall / 2; z++) {
			for (int y = -m_CellsData.Yall / 2; y < m_CellsData.Yall / 2; y++) {
				if (m_CellsData.SetCell(x, y, z).select != INVALID_ID) {
					AddCube(x, y, z, m_CellsData.SetCell(x, y, z).select, false);
				}
			}
		}
	}
}
void		CubeEditer::SaveCellsFile() noexcept {
	std::ofstream fout;
	fout.open("Save/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
	fout.write((char*)&m_CellsData.Xall, sizeof(m_CellsData.Xall));
	fout.write((char*)&m_CellsData.Yall, sizeof(m_CellsData.Yall));
	fout.write((char*)&m_CellsData.Zall, sizeof(m_CellsData.Zall));
	fout.write((char*)&m_CellsData.m_Cell.at(0), sizeof(m_CellsData.m_Cell.at(0)) * m_CellsData.m_Cell.size());
	fout.close();  //ファイルを閉じる
}

//-------------------------------------------------------------------------------------
void		CubeEditer::Init() noexcept {
	m_CellsData.Xall = 100;
	m_CellsData.Yall = 100;
	m_CellsData.Zall = 100;
	m_CellsData.m_number.resize((size_t)(m_CellsData.Xall * m_CellsData.Yall * m_CellsData.Zall));
	m_CellsData.m_Cell.resize((size_t)(m_CellsData.Xall * m_CellsData.Yall * m_CellsData.Zall));
	for (int x = -m_CellsData.Xall / 2; x < m_CellsData.Xall / 2; x++) {
		for (int z = -m_CellsData.Zall / 2; z < m_CellsData.Zall / 2; z++) {
			for (int y = -m_CellsData.Yall / 2; y < m_CellsData.Yall / 2; y++) {
				m_CellsData.SetCell(x, y, z).select = INVALID_ID;
				m_CellsData.SetCellNum(x, y, z) = INVALID_ID;
			}
		}
	}

	MATERIALPARAM Param{};
	Param.Diffuse = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);						// ディフューズカラー
	Param.Ambient = GetColorF(0.15f, 0.15f, 0.15f, 1.0f);						// アンビエントカラー
	Param.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// スペキュラカラー
	Param.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// エミッシブカラー
	Param.Power = 100.f;							// スペキュラハイライトの鮮明度
	SetMaterialParam(Param);
	
	CheckPoint();
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
