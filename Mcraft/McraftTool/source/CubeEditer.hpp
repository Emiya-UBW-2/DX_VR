#define NOMINMAX

#include "DXLib_ref/DXLib_ref.h"
using namespace DXLibRef;

struct CellData {
	int selset = INVALID_ID;
};
struct CellsData {
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
};


class CubeEditer {
	std::vector<VERTEX3D>		m_vert32;
	std::vector<uint32_t>		m_index32;
	CellsData					m_CellsData;
public:
	static void		Bresenham3D(const int x1, const int y1, const int z1, const int x2, const int y2, const int z2, const std::function<bool(int, int, int)>& OutPutLine) noexcept;

	void		AddCube(int x, int y, int z) noexcept;
	void		DelCube(int x, int y, int z) noexcept;
public:
	void		Init() noexcept;
	void		DrawBG() noexcept;
	void		Draw() noexcept;
};

