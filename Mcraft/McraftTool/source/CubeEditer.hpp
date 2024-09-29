#define NOMINMAX

#include "DXLib_ref/DXLib_ref.h"
using namespace DXLibRef;

struct CellData {
	int8_t select = INVALID_ID;
	int8_t inRock = false;
};
struct CellsData {
	std::vector<int> m_number;
	std::vector<CellData> m_Cell;
	int16_t Xall = 500;
	int16_t Yall = 160;
	int16_t Zall = 500;
	const auto& GetCell(int x, int y, int z) const noexcept {
		return m_Cell.at((size_t)((x + Xall / 2) + Xall * (y + Yall / 2) + Xall * Yall * (z + Zall / 2)));
	}
	auto& SetCell(int x, int y, int z) noexcept {
		return m_Cell.at((size_t)((x + Xall / 2) + Xall * (y + Yall / 2) + Xall * Yall * (z + Zall / 2)));
	}
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

