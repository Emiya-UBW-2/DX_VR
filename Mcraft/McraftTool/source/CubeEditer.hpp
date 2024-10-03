#define NOMINMAX

#include "DXLib_ref/DXLib_ref.h"
using namespace DXLibRef;

static const int8_t s_EmptyBlick = 0;

struct CellsData {
	std::vector<int8_t> m_Cell;
	int16_t Xall = 500;
	int16_t Yall = 160;
	int16_t Zall = 500;
	int8_t scaleRate = 1;

	const auto& GetCell(int x, int y, int z) const noexcept {
		return m_Cell.at((size_t)((x + Xall / 2) + Xall * (y + Yall / 2) + Xall * Yall * (z + Zall / 2)));
	}
	auto& SetCell(int x, int y, int z) noexcept {
		return m_Cell.at((size_t)((x + Xall / 2) + Xall * (y + Yall / 2) + Xall * Yall * (z + Zall / 2)));
	}

	void SetCellOnBlick(int x, int y, int z, bool value) noexcept {
		SetCell(x, y, z) = (int8_t)(value ? -std::abs(GetCell(x, y, z)) : std::abs(GetCell(x, y, z)));
	}
	const bool IsActiveCell(int x, int y, int z) const noexcept {
		return GetCell(x, y, z) != s_EmptyBlick;
	}

	const bool IsDrawCell(int x, int y, int z) const noexcept {
		return GetCell(x, y, z) > s_EmptyBlick;
	}

	std::vector<int> m_number;
	const auto& GetCellNum(int x, int y, int z) const noexcept {
		return m_number.at((size_t)((x + Xall / 2) + Xall * (y + Yall / 2) + Xall * Yall * (z + Zall / 2)));
	}
	auto& SetCellNum(int x, int y, int z) noexcept {
		return m_number.at((size_t)((x + Xall / 2) + Xall * (y + Yall / 2) + Xall * Yall * (z + Zall / 2)));
	}
};


struct SavePointData {
	int x{};
	int y{};
	int z{};
	int prevselect = INVALID_ID;
	int select = INVALID_ID;
};


class CubeEditer {
	std::vector<VERTEX3D>		m_vert32;
	std::vector<uint32_t>		m_index32;
	CellsData					m_CellsData;

	std::vector<std::vector<SavePointData>> m_SavePointData{};
	int m_SavePointDataNow = 0;
public:
	const auto GetSavePointDataMax() const noexcept { return m_SavePointData.size() - 1; }
	const auto GetSavePointDataNow() const noexcept { return m_SavePointDataNow; }
public:
	static void		Bresenham3D(const int x1, const int y1, const int z1, const int x2, const int y2, const int z2, const std::function<bool(int, int, int)>& OutPutLine) noexcept;

	void		AddCube(int x, int y, int z, int select, bool DrawCheckPoint = true) noexcept;
	void		DelCube(int x, int y, int z, bool DrawCheckPoint = true) noexcept;

	void		CheckPoint() noexcept;
	void		UnDo() noexcept;
	void		ReDo() noexcept;

	void		LoadCellsFile() noexcept;
	void		SaveCellsFile() noexcept;
public:
	void		Init() noexcept;
	void		DrawBG() noexcept;
	void		Draw() noexcept;
};

