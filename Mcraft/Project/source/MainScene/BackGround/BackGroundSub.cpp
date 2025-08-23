#include	"BackGroundSub.hpp"

namespace FPS_n2 {
	namespace BackGround {
		void		VoxelControl::AddPlaneXPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int Xvoxel, int Yvoxel, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmax, 0b101).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin, 0b100).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmax, 0b111).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin, 0b110).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneXMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int Xvoxel, int Yvoxel, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmax, 0b011).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin, 0b010).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmax, 0b001).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin, 0b000).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneYPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int Xvoxel, int Yvoxel, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin).GetCell() - 1) * 3) + 0;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmax, 0b111).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin, 0b110).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmax, 0b011).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin, 0b010).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneYMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int Xvoxel, int Yvoxel, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin).GetCell() - 1) * 3) + 2;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmax, 0b001).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin, 0b000).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmax, 0b101).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(center.x + Xvoxel, center.y + Yvoxel, center.z + zmin, 0b100).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneZPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int Yvoxel, int Zvoxel, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				xscale = (xmax - xmin + 1) * cellx.ScaleRate;
				Xofs = (center.x + xmax) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xmin, center.y + Yvoxel, center.z + Zvoxel).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(center.x + xmin, center.y + Yvoxel, center.z + Zvoxel, 0b001).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(center.x + xmax, center.y + Yvoxel, center.z + Zvoxel, 0b101).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(center.x + xmin, center.y + Yvoxel, center.z + Zvoxel, 0b011).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(center.x + xmax, center.y + Yvoxel, center.z + Zvoxel, 0b111).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneZMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int Yvoxel, int Zvoxel, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				xscale = (xmax - xmin + 1) * cellx.ScaleRate;
				Xofs = (center.x + xmax) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xmin, center.y + Yvoxel, center.z + Zvoxel).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(center.x + xmin, center.y + Yvoxel, center.z + Zvoxel, 0b010).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(center.x + xmax, center.y + Yvoxel, center.z + Zvoxel, 0b110).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(center.x + xmin, center.y + Yvoxel, center.z + Zvoxel, 0b000).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(center.x + xmax, center.y + Yvoxel, center.z + Zvoxel, 0b100).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}

		void		VoxelControl::AddPlanesXY(vert32* pTarget, float camVecX, float camVecY, const CellsData& cellx, const Vector3Int& center, int Xvoxel, int Yvoxel, int zMaxminT, int zMaxmaxT, bool CheckInsideXY, bool CheckFillXY, bool IsCalcUV) noexcept {
			int zmin = 0;
			int zmax = 0;
			bool CheckFillXYZ = false;
			bool isHitmin = true;
			int8_t selectBlock = s_EmptyBlick;
			bool CanDrawXPlus = false;
			bool CanDrawXMinus = false;
			bool CanDrawYPlus = false;
			bool CanDrawYMinus = false;
			for (int Zvoxel = zMaxminT; Zvoxel <= zMaxmaxT; ++Zvoxel) {
				auto& CellBuff = cellx.GetCellBuf(center.x + Xvoxel, center.y + Yvoxel, center.z + Zvoxel);
				bool CheckInsideXYZ = CheckInsideXY && ((DrawMinZMinus < Zvoxel) && (Zvoxel < DrawMinZPlus));
				bool CheckBlockID = IsCalcUV && (selectBlock != CellBuff.GetCell());
				if (
					(Zvoxel == zMaxmaxT)
					|| CheckInsideXYZ
					|| (!isHitmin && CheckBlockID)
					|| !CellBuff.CanDraw()
					) {
					//置けない部分なので今まで置けていた分をまとめてポリゴン化
					if (!isHitmin) {
						isHitmin = true;
						if (camVecX < 0.0f) {
							if (CheckFillXYZ || CanDrawXPlus) {
								AddPlaneXPlus(pTarget, cellx, center, Xvoxel, Yvoxel, zmin, zmax, IsCalcUV);
							}
						}
						else {
							if (CheckFillXYZ || CanDrawXMinus) {
								AddPlaneXMinus(pTarget, cellx, center, Xvoxel, Yvoxel, zmin, zmax, IsCalcUV);
							}
						}
						if (camVecY < 0.0f) {
							if (CheckFillXYZ || CanDrawYPlus) {
								AddPlaneYPlus(pTarget, cellx, center, Xvoxel, Yvoxel, zmin, zmax, IsCalcUV);
							}
						}
						else {
							if (CheckFillXYZ || CanDrawYMinus) {
								AddPlaneYMinus(pTarget, cellx, center, Xvoxel, Yvoxel, zmin, zmax, IsCalcUV);
							}
						}
						//この場合だけもう一回判定させるドン
						if (CheckBlockID) {
							--Zvoxel;
						}
					}
				}
				else {
					//ブロックが置ける部分
					if (isHitmin) {
						isHitmin = false;
						zmin = Zvoxel;
						selectBlock = CellBuff.GetCell();
						CanDrawXPlus = false;
						CanDrawXMinus = false;
						CanDrawYPlus = false;
						CanDrawYMinus = false;
					}
					zmax = Zvoxel;
					CheckFillXYZ = CheckFillXY && (zmin <= DrawMinZPlus && DrawMinZMinus <= zmax);
					if (!CanDrawXPlus) {
						if (!CellBuff.IsOcclusion(0)) {
							CanDrawXPlus = true;
						}
					}
					if (!CanDrawXMinus) {
						if (!CellBuff.IsOcclusion(1)) {
							CanDrawXMinus = true;
						}
					}
					if (!CanDrawYPlus) {
						if (!CellBuff.IsOcclusion(2)) {
							CanDrawYPlus = true;
						}
					}
					if (!CanDrawYMinus) {
						if (!CellBuff.IsOcclusion(3)) {
							CanDrawYMinus = true;
						}
					}
				}
			}
		}
		void		VoxelControl::AddPlanesZ(vert32* pTarget, float camVecZ, const CellsData& cellx, const Vector3Int& center, int xMaxminT, int xMaxmaxT, int Yvoxel, int Zvoxel, bool CheckInsideYZ, bool CheckFillYZ, bool IsCalcUV) noexcept {
			int xmin = 0;
			int xmax = 0;
			bool CheckFillXYZ = false;
			bool isHitmin = true;
			int8_t selectBlock = s_EmptyBlick;
			bool CanDrawZPlus = false;
			bool CanDrawZMinus = false;
			for (int Xvoxel = xMaxminT; Xvoxel <= xMaxmaxT; ++Xvoxel) {
				auto& CellBuff = cellx.GetCellBuf(center.x + Xvoxel, center.y + Yvoxel, center.z + Zvoxel);
				bool CheckInsideXYZ = CheckInsideYZ && ((DrawMinXMinus < Xvoxel) && (Xvoxel < DrawMinXPlus));
				bool CheckBlockID = IsCalcUV && (selectBlock != CellBuff.GetCell());
				if (
					(Xvoxel == xMaxmaxT)
					|| CheckInsideXYZ
					|| (!isHitmin && CheckBlockID)
					|| !CellBuff.CanDraw()
					) {
					//置けない部分なので今まで置けていた分をまとめてポリゴン化
					if (!isHitmin) {
						isHitmin = true;
						if (camVecZ < 0) {
							if (CheckFillXYZ || CanDrawZPlus) {
								AddPlaneZPlus(pTarget, cellx, center, xmin, xmax, Yvoxel, Zvoxel, IsCalcUV);
							}
						}
						else {
							if (CheckFillXYZ || CanDrawZMinus) {
								AddPlaneZMinus(pTarget, cellx, center, xmin, xmax, Yvoxel, Zvoxel, IsCalcUV);
							}
						}
						//この場合だけもう一回判定させるドン
						if (CheckBlockID) {
							--Xvoxel;
						}
					}
				}
				else {
					//ブロックが置ける部分
					if (isHitmin) {
						isHitmin = false;
						xmin = Xvoxel;
						selectBlock = CellBuff.GetCell();
						CanDrawZPlus = false;
						CanDrawZMinus = false;
					}
					xmax = Xvoxel;
					CheckFillXYZ = CheckFillYZ && (xmin <= DrawMinXPlus && DrawMinXMinus <= xmax);
					if (!CanDrawZPlus) {
						if (!CellBuff.IsOcclusion(4)) {
							CanDrawZPlus = true;
						}
					}
					if (!CanDrawZMinus) {
						if (!CellBuff.IsOcclusion(5)) {
							CanDrawZMinus = true;
						}
					}
				}
			}
		}

		bool CullingLine(int* MaxminT, int* MaxmaxT, float dTa, float dTb) {
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

		void		VoxelControl::AddCubes(size_t id, size_t threadID, bool CheckCamPosition, bool IsCalcUV) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			DrawThreadData& Draws = this->m_DrawThreadDatas[threadID];
			Draws.StartRegist();
			Vector3Int center = cellx.GetVoxelPoint(Draws.GetCamPos());

			float CamXMinX = Draws.GetCamVec().x * (static_cast<float>(DrawMaxXMinus) + 0.5f);
			float CamXMaxX = Draws.GetCamVec().x * (static_cast<float>(DrawMaxXPlus) + 0.5f);
			float CamYMinY = Draws.GetCamVec().y * (static_cast<float>(DrawMaxYMinus) + 0.5f);
			float CamYMaxY = Draws.GetCamVec().y * (static_cast<float>(DrawMaxYPlus) + 0.5f);
			float CamZMinZ = Draws.GetCamVec().z * (static_cast<float>(DrawMaxZMinus) + 0.5f);
			float CamZMaxZ = Draws.GetCamVec().z * (static_cast<float>(DrawMaxZPlus) + 0.5f);
			//X
			for (int Zvoxel = DrawMaxZMinus; Zvoxel <= DrawMaxZPlus; ++Zvoxel) {
				float CamZZ = Draws.GetCamVec().z * (static_cast<float>(Zvoxel) + 0.5f);
				bool CheckFillZ = false;
				if (CheckCamPosition) {
					//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
					if (
						((CamXMinX + CamYMinY + CamZZ) <= 0.0f) &&//Dot
						((CamXMaxX + CamYMaxY + CamZZ) <= 0.0f) &&//Dot
						((CamXMaxX + CamYMinY + CamZZ) <= 0.0f) &&//Dot
						((CamXMinX + CamYMaxY + CamZZ) <= 0.0f)//Dot
						) { continue; }
					CheckFillZ = cellx.isFarCells() && ((DrawMinZMinus <= Zvoxel) && (Zvoxel <= DrawMinZPlus));
				}
				bool CheckInsideZ = cellx.isFarCells() && ((DrawMinZMinus < Zvoxel) && (Zvoxel < DrawMinZPlus));
				for (int Yvoxel = DrawMaxYMinus; Yvoxel <= DrawMaxYPlus; ++Yvoxel) {
					if (!cellx.isInside(center.y + Yvoxel)) { continue; }
					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					bool CheckFillYZ = false;
					if (CheckCamPosition) {
						float CamYY = Draws.GetCamVec().y * (static_cast<float>(Yvoxel) + 0.5f);
						float dTa = (CamXMinX + CamYY + CamZZ);//Dot
						float dTb = (CamXMaxX + CamYY + CamZZ);//Dot
						if (!CullingLine(&xMaxminT, &xMaxmaxT, dTa, dTb)) {
							continue;
						}
						CheckFillYZ = CheckFillZ && ((DrawMinYMinus <= Yvoxel) && (Yvoxel <= DrawMinYPlus));
					}
					bool CheckInsideYZ = CheckInsideZ && ((DrawMinYMinus < Yvoxel) && (Yvoxel < DrawMinYPlus));
					AddPlanesZ(&Draws.Vert32,
						CheckCamPosition ? static_cast<float>(Zvoxel) : Draws.GetCamVec().z,
						cellx, center, xMaxminT, xMaxmaxT, Yvoxel, Zvoxel, CheckInsideYZ, CheckFillYZ, IsCalcUV);
				}
			}
			//Z
			for (int Xvoxel = DrawMaxXMinus; Xvoxel <= DrawMaxXPlus; ++Xvoxel) {
				float CamXX = Draws.GetCamVec().x * (static_cast<float>(Xvoxel) + 0.5f);
				bool CheckFillX = false;
				if (CheckCamPosition) {
					//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
					if (
						((CamXX + CamYMinY + CamZMinZ) <= 0.0f) &&//Dot
						((CamXX + CamYMaxY + CamZMaxZ) <= 0.0f) &&//Dot
						((CamXX + CamYMinY + CamZMaxZ) <= 0.0f) &&//Dot
						((CamXX + CamYMaxY + CamZMinZ) <= 0.0f)//Dot
						) { continue; }
					CheckFillX = cellx.isFarCells() && ((DrawMinXMinus <= Xvoxel) && (Xvoxel <= DrawMinXPlus));
				}
				bool CheckInsideX = cellx.isFarCells() && ((DrawMinXMinus < Xvoxel) && (Xvoxel < DrawMinXPlus));
				for (int Yvoxel = DrawMaxYMinus; Yvoxel <= DrawMaxYPlus; ++Yvoxel) {
					if (!cellx.isInside(center.y + Yvoxel)) { continue; }
					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					bool CheckFillXY = false;
					if (CheckCamPosition) {
						float CamYY = Draws.GetCamVec().y * (static_cast<float>(Yvoxel) + 0.5f);
						float dTa = (CamXX + CamYY + CamZMinZ);//Dot
						float dTb = (CamXX + CamYY + CamZMaxZ);//Dot
						if (!CullingLine(&zMaxminT, &zMaxmaxT, dTa, dTb)) {
							continue;
						}
						CheckFillXY = CheckFillX && ((DrawMinYMinus <= Yvoxel) && (Yvoxel <= DrawMinYPlus));
					}
					bool CheckInsideXY = CheckInsideX && ((DrawMinYMinus < Yvoxel) && (Yvoxel < DrawMinYPlus));
					AddPlanesXY(&Draws.Vert32,
						CheckCamPosition ? static_cast<float>(Xvoxel) : Draws.GetCamVec().x,
						CheckCamPosition ? static_cast<float>(Yvoxel) : Draws.GetCamVec().y,
						cellx, center, Xvoxel, Yvoxel, zMaxminT, zMaxmaxT, CheckInsideXY, CheckFillXY, IsCalcUV);
				}
			}
		}
		void		VoxelControl::FlipCubes(size_t threadID, const Vector3DX& camPos, const Vector3DX& camVec) noexcept {
			DrawThreadData& Draws = this->m_DrawThreadDatas[threadID];
			Draws.EndRegist();
			Draws.SetCamPos(camPos);
			Draws.SetCamVec(camVec);
		}
		//
		int			VoxelControl::CheckLine(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) const noexcept {
			int HitCount = 0;

			if (isnan<float>(StartPos.x) || isnan<float>(StartPos.y) || isnan<float>(StartPos.z)) {
				return HitCount;
			}
			if (isnan<float>((*EndPos).x) || isnan<float>((*EndPos).y) || isnan<float>((*EndPos).z)) {
				return HitCount;
			}
			float scale = 200.f * Scale3DRate;
			Vector3DX SP = StartPos; SP.y = 0.f;
			Vector3DX EP = (*EndPos); SP.y = 0.f;
			if (!SP.IsRangeSmaller(scale) || !EP.IsRangeSmaller(scale)) {
				return HitCount;
			}


			Vector3Int Start = GetReferenceCells().GetVoxelPoint(StartPos);
			Vector3Int End = GetReferenceCells().GetVoxelPoint(*EndPos);// *EndPos

			for (int xm = -1; xm <= 1; ++xm) {
				for (int ym = -1; ym <= 1; ++ym) {
					for (int zm = -1; zm <= 1; ++zm) {
						bool isHit = false;
						Bresenham3D(
							Start.x + xm, Start.y + ym, Start.z + zm,
							End.x + xm, End.y + ym, End.z + zm,
							[&](int Xvoxel, int Yvoxel, int Zvoxel) {
								if (!GetReferenceCells().isInside(Yvoxel)) { return false; }
								if (!GetReferenceCells().GetCellBuf(Xvoxel, Yvoxel, Zvoxel).CanDraw()) { return false; }
								Vector3DX MinPos = GetReferenceCells().GetWorldPosOffset(Xvoxel, Yvoxel, Zvoxel, 0b000) + Vector3DX::one() * -0.1f * Scale3DRate;
								Vector3DX MaxPos = GetReferenceCells().GetWorldPosOffset(Xvoxel, Yvoxel, Zvoxel, 0b111) + Vector3DX::one() * 0.1f * Scale3DRate;
								Vector3DX tmpEndPos = *EndPos;
								Vector3DX tmpNormal;
								if (ColRayBox(StartPos, &tmpEndPos, MinPos, MaxPos, &tmpNormal)) {
									if ((tmpEndPos - StartPos).sqrMagnitude() < (*EndPos - StartPos).sqrMagnitude()) {
										*EndPos = tmpEndPos;
										if (Normal) {
											*Normal = tmpNormal;
										}
									}
									isHit = true;
									++HitCount;
								}
								return false;
							});
						if (isHit) {
							return HitCount;
						}
					}
				}
			}
			return HitCount;
		}
		bool		VoxelControl::CheckWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius, const std::vector<const MV1*>& addonColObj) const noexcept {
			Vector3DX MoveVector = *EndPos - StartPos;
			// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
			std::vector<MV1_COLL_RESULT_POLY> kabes;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			Vector3Int Start = GetReferenceCells().GetVoxelPoint(StartPos);
			Vector3Int End = GetReferenceCells().GetVoxelPoint(*EndPos);// *EndPos

			for (int xm = -3; xm <= 3; ++xm) {
				for (int ym = 3; ym <= 6; ++ym) {
					for (int zm = -3; zm <= 3; ++zm) {
						Bresenham3D(
							Start.x + xm, Start.y + ym, Start.z + zm,
							End.x + xm, End.y + ym, End.z + zm,
							[&](int Xvoxel, int Yvoxel, int Zvoxel) {
								if (!GetReferenceCells().isInside(Yvoxel)) { return false; }
								if (!GetReferenceCells().GetCellBuf(Xvoxel, Yvoxel, Zvoxel).CanDraw()) { return false; }
								Vector3DX MinPos = GetReferenceCells().GetWorldPosOffset(Xvoxel, Yvoxel, Zvoxel, 0b000);
								Vector3DX MaxPos = GetReferenceCells().GetWorldPosOffset(Xvoxel, Yvoxel, Zvoxel, 0b111);

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
			//*
			for (auto& ad : addonColObj) {
				MV1_COLL_RESULT_POLY_DIM HitDim = ad->CollCheck_Sphere(StartPos, AddCapsuleMax.magnitude() + MoveVector.magnitude());
				// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
				for (int i = 0; i < HitDim.HitNum; ++i) {
					kabes.emplace_back(HitDim.Dim[i]);// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
				}
				MV1CollResultPolyDimTerminate(HitDim);	// 検出したプレイヤーの周囲のポリゴン情報を開放する
			}
			//*/

			bool HitFlag = false;
			// 壁ポリゴンとの当たり判定処理
			if (kabes.size() > 0) {
				HitFlag = false;
				for (auto& kabe : kabes) {
					// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
						HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
						if (!MoveVector.IsRangeSmaller(0.001f)) {	// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定
							// 壁に当たったら壁に遮られない移動成分分だけ移動する
							*EndPos = Vector3DX::Cross(kabe.Normal, Vector3DX::Cross(MoveVector, kabe.Normal)) + StartPos;
							//EndPos->y(StartPos.y);
							bool isHit = false;
							for (auto& kabe2 : kabes) {
								if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
									isHit = true;
									break;// 当たっていたらループから抜ける
								}
							}
							if (!isHit) {
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
					for (int loop = 0; loop < 32; ++loop) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
						bool HitF = false;
						for (auto& kabe : kabes) {
							// プレイヤーと当たっているかを判定
							if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
								*EndPos += Vector3DX(kabe.Normal) * (0.015f * Scale3DRate);					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
								bool isHit = false;
								for (auto& kabe2 : kabes) {
									// 当たっていたらループを抜ける
									if (GetHitCapsuleToTriangle(*EndPos + AddCapsuleMin, *EndPos + AddCapsuleMax, Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
										isHit = true;
										break;
									}
								}
								if (!isHit) {// 全てのポリゴンと当たっていなかったらここでループ終了
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
		void		VoxelControl::LoadCellsFile(std::string_view Path) noexcept {
			std::ifstream fin{};
			fin.open(Path, std::ios::in | std::ios::binary);
			this->m_CellBase.resize(static_cast<size_t>(256 * 256 * 256));
			fin.read((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * 256 * 256 * 256);
			fin.close();
			for (int xm = 0; xm < GetReferenceCells().All; ++xm) {
				for (int ym = 0; ym < GetReferenceCells().All; ++ym) {
					for (int zm = 0; zm < GetReferenceCells().All; ++zm) {
						SetBlick(xm, ym, zm, this->m_CellBase[GetReferenceCells().GetCellNum(xm, ym, zm)], false);
					}
				}
			}
		}
		void		VoxelControl::SaveCellsFile(std::string_view Path) noexcept {
			this->m_CellBase.resize(static_cast<size_t>(256 * 256 * 256));
			for (int xm = 0; xm < GetReferenceCells().All; ++xm) {
				for (int ym = 0; ym < GetReferenceCells().All; ++ym) {
					for (int zm = 0; zm < GetReferenceCells().All; ++zm) {
						this->m_CellBase[GetReferenceCells().GetCellNum(xm, ym, zm)] = GetReferenceCells().GetCellBuf(xm, ym, zm).GetCell();
					}
				}
			}

			std::ofstream fout{};
			fout.open(Path, std::ios::out | std::ios::binary | std::ios::trunc);
			fout.write((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * 256 * 256 * 256);
			fout.close(); //ファイルを閉じる
		}
		void		VoxelControl::LoadCellsClip(int xbasepos, int ybasepos, int zbasepos, std::string_view Path) noexcept {
			int XTotal = 1;
			int YTotal = 1;
			int ZTotal = 1;
			std::ifstream fin{};
			fin.open(Path, std::ios::in | std::ios::binary);
			fin.read((char*)&XTotal, sizeof(XTotal));
			fin.read((char*)&YTotal, sizeof(YTotal));
			fin.read((char*)&ZTotal, sizeof(ZTotal));
			this->m_CellBase.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
			fin.read((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * XTotal * YTotal * ZTotal);
			fin.close(); //ファイルを閉じる

			for (int xm = 0; xm < XTotal; ++xm) {
				for (int ym = 0; ym < YTotal; ++ym) {
					for (int zm = 0; zm < ZTotal; ++zm) {
						SetBlick(xm + xbasepos, ym + ybasepos, zm + zbasepos, this->m_CellBase[static_cast<size_t>(xm * YTotal * ZTotal + ym * ZTotal + zm)], false);
					}
				}
			}
		}
		void		VoxelControl::SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax, std::string_view Path) noexcept {
			int XTotal = (XMax - XMin + 1);
			int YTotal = (YMax - YMin + 1);
			int ZTotal = (ZMax - ZMin + 1);

			this->m_CellBase.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
			for (int xm = 0; xm < XTotal; ++xm) {
				for (int ym = 0; ym < YTotal; ++ym) {
					for (int zm = 0; zm < ZTotal; ++zm) {
						this->m_CellBase[static_cast<size_t>(xm * YTotal * ZTotal + ym * ZTotal + zm)] = GetReferenceCells().GetCellBuf(XMin + xm, YMin + ym, ZMin + zm).GetCell();
					}
				}
			}

			std::ofstream fout{};
			fout.open(Path, std::ios::out | std::ios::binary | std::ios::trunc);
			fout.write((char*)&XTotal, sizeof(XTotal));
			fout.write((char*)&YTotal, sizeof(YTotal));
			fout.write((char*)&ZTotal, sizeof(ZTotal));
			fout.write((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0]) * XTotal * YTotal * ZTotal));
			fout.close(); //ファイルを閉じる
		}
		void		VoxelControl::SettingChange(void) noexcept {
			this->m_ShadowRate = 0;
			this->m_BaseRate = 1;
		}
		void		VoxelControl::SetBlick(int Xvoxel, int Yvoxel, int Zvoxel, int8_t select, bool CalcOther) noexcept {
			this->m_CellxN[ReferenceCell].SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetCell(select);
			if (select == s_EmptyBlick) {
				this->m_CellxN[ReferenceCell].SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetLife(0);
			}
			if (CalcOther) {
				//簡易版を更新
				for (int loop = 1; loop < TotalCellLayer; ++loop) {
					CellsData& cellx = this->m_CellxN[loop];

					int xm = Xvoxel / cellx.ScaleRate;
					int ym = Yvoxel / cellx.ScaleRate;
					int zm = Zvoxel / cellx.ScaleRate;

					cellx.SetCellBuf(xm, ym, zm).SetCell(GetReferenceCells().isFill(xm, ym, zm, cellx.ScaleRate));
				}
				//遮蔽検索
				for (auto& cellx : this->m_CellxN) {
					int xm = Xvoxel / cellx.ScaleRate;
					int ym = Yvoxel / cellx.ScaleRate;
					int zm = Zvoxel / cellx.ScaleRate;

					cellx.CalcOcclusion(xm + 1, ym, zm);
					cellx.CalcOcclusion(xm - 1, ym, zm);
					cellx.CalcOcclusion(xm, ym + 1, zm);
					cellx.CalcOcclusion(xm, ym - 1, zm);
					cellx.CalcOcclusion(xm, ym, zm + 1);
					cellx.CalcOcclusion(xm, ym, zm - 1);
				}
			}
		}
		//
		void		VoxelControl::Load(void) noexcept {
			this->m_tex.Load("data/tex.png");
		}
		void		VoxelControl::Init(void) noexcept {
			//初期化
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				CellsData& cellx = this->m_CellxN[loop];
				cellx.Init(loop);
			}
			for (int Xvoxel = 0; Xvoxel < GetReferenceCells().All; ++Xvoxel) {
				for (int Zvoxel = 0; Zvoxel < GetReferenceCells().All; ++Zvoxel) {
					for (int Yvoxel = 0; Yvoxel < GetReferenceCells().All; ++Yvoxel) {
						SetBlick(Xvoxel, Yvoxel, Zvoxel, s_EmptyBlick, false);
					}
				}
			}
		}
		void		VoxelControl::Setup(void) noexcept {
			//HPを設定
			for (int Xvoxel = 0; Xvoxel < GetReferenceCells().All; ++Xvoxel) {
				for (int Zvoxel = 0; Zvoxel < GetReferenceCells().All; ++Zvoxel) {
					for (int Yvoxel = 0; Yvoxel < GetReferenceCells().All; ++Yvoxel) {
						if (GetReferenceCells().GetCellBuf(Xvoxel, Yvoxel, Zvoxel).GetCell() != s_EmptyBlick) {
							this->m_CellxN[ReferenceCell].SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetLife(100);
						}
					}
				}
			}
			//簡略版を制作
			for (int loop = 1; loop < TotalCellLayer; ++loop) {
				CellsData& cellx = this->m_CellxN[loop];
				for (int xm = 0; xm < cellx.All; ++xm) {
					for (int ym = 0; ym < cellx.All; ++ym) {
						for (int zm = 0; zm < cellx.All; ++zm) {
							cellx.SetCellBuf(xm, ym, zm).SetCell(GetReferenceCells().isFill(xm, ym, zm, cellx.ScaleRate));
						}
					}
				}
			}
			//遮蔽検索
			for (auto& cellx : this->m_CellxN) {
				for (int Xvoxel = 0; Xvoxel < cellx.All; ++Xvoxel) {
					for (int Yvoxel = 0; Yvoxel < cellx.All; ++Yvoxel) {
						for (int Zvoxel = 0; Zvoxel < cellx.All; ++Zvoxel) {
							cellx.CalcOcclusion(Xvoxel, Yvoxel, Zvoxel);
						}
					}
				}
			}
			//描画ポリゴンを生成するスレッドを作成
			constexpr size_t size = (DrawMaxXPlus - DrawMaxXMinus) * (DrawMaxYPlus - DrawMaxYMinus) * (DrawMaxZPlus - DrawMaxZMinus) / 2 * 3 / 100;
			for (auto& Vert : this->m_DrawThreadDatas) {
				Vert.Init(size);
			}
			this->m_Jobs[0].Init(
				[this]() { AddCubes(0, 0, true, true); },
				[this]() {
					auto* CameraParts = Camera3D::Instance();
					FlipCubes(0, CameraParts->GetMainCamera().GetCamPos(), (CameraParts->GetMainCamera().GetCamVec() - CameraParts->GetMainCamera().GetCamPos()).normalized());
				}, false);
			this->m_Jobs[1].Init(
				[this]() { AddCubes(1, 1, true, true); },
				[this]() {
					auto* CameraParts = Camera3D::Instance();
					FlipCubes(1, CameraParts->GetMainCamera().GetCamPos(), (CameraParts->GetMainCamera().GetCamVec() - CameraParts->GetMainCamera().GetCamPos()).normalized());
				}, false);
			this->m_Jobs[2].Init(
				[this]() { AddCubes(2, 2, true, true); },
				[this]() {
					auto* CameraParts = Camera3D::Instance();
					FlipCubes(2, CameraParts->GetMainCamera().GetCamPos(), (CameraParts->GetMainCamera().GetCamVec() - CameraParts->GetMainCamera().GetCamPos()).normalized());
				}, false);
			this->m_Jobs[3].Init(
				[this]() { AddCubes(3, 3, true, true); },
				[this]() {
					auto* CameraParts = Camera3D::Instance();
					FlipCubes(3, CameraParts->GetMainCamera().GetCamPos(), (CameraParts->GetMainCamera().GetCamVec() - CameraParts->GetMainCamera().GetCamPos()).normalized());
				}, false);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 0)].Init(
				[this]() { AddCubes(0, TotalCellLayer + 0, false, false); },
				[this]() {
					auto* PostPassParts = PostPassEffect::Instance();
					auto* SceneParts = SceneControl::Instance();
					FlipCubes(TotalCellLayer + 0, Vector3DX::vget(0.0f, -25.0f, 0.0f) * Scale3DRate, PostPassParts->GetShadowDir());
					SceneParts->SetIsUpdateFarShadowActive();
				}, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 1)].Init(
				[this]() { AddCubes(1, TotalCellLayer + 1, false, false); },
				[this]() {
					auto* PostPassParts = PostPassEffect::Instance();
					auto* SceneParts = SceneControl::Instance();
					FlipCubes(TotalCellLayer + 1, Vector3DX::vget(0.0f, -25.0f, 0.0f) * Scale3DRate, PostPassParts->GetShadowDir());
					SceneParts->SetIsUpdateFarShadowActive();
				}, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 2)].Init(
				[this]() { AddCubes(2, TotalCellLayer + 2, false, false); },
				[this]() {
					auto* PostPassParts = PostPassEffect::Instance();
					auto* SceneParts = SceneControl::Instance();
					FlipCubes(TotalCellLayer + 2, Vector3DX::vget(0.0f, -25.0f, 0.0f) * Scale3DRate, PostPassParts->GetShadowDir());
					SceneParts->SetIsUpdateFarShadowActive();
				}, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 3)].Init(
				[this]() { AddCubes(3, TotalCellLayer + 3, false, false); },
				[this]() {
					auto* PostPassParts = PostPassEffect::Instance();
					auto* SceneParts = SceneControl::Instance();
					FlipCubes(TotalCellLayer + 3, Vector3DX::vget(0.0f, -25.0f, 0.0f) * Scale3DRate, PostPassParts->GetShadowDir());
					SceneParts->SetIsUpdateFarShadowActive();
				}, true);
			this->m_ThreadCounter = 0;
			this->m_isChangeBlock = false;
			//
			SettingChange();
		}
		void		VoxelControl::Update(void) noexcept {
			if (this->m_isChangeBlock) {
				this->m_isChangeBlock = false;
				//UpdateOnce
				for (int loop = 0; loop < TotalCellLayer; ++loop) {
					this->m_Jobs[static_cast<size_t>(TotalCellLayer + loop)].JobStart();
				}
			}
			auto* OptionParts = OptionManager::Instance();
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				if ((loop != 0) && (loop != this->m_ThreadCounter)) { continue; }
				CellsData& cellx = this->m_CellxN[loop];
				//
				if (this->m_BaseRate < cellx.GetScale()) {
					this->m_DrawThreadDatas[loop].Reset();
				}
				else {
					this->m_Jobs[loop].Update();
				}
				//
				int threadID = TotalCellLayer + loop;
				if ((OptionParts->GetParamInt(EnumSaveParam::shadow) == 0) || (this->m_ShadowRate < cellx.GetScale())) {
					this->m_DrawThreadDatas[threadID].Reset();
					this->m_Jobs[threadID].UpdateDisable();
				}
				else {
					this->m_Jobs[threadID].Update();
				}
			}
			++this->m_ThreadCounter %= TotalCellLayer;

#if defined(DEBUG) && FALSE
			{
				for (int loop = 0; loop < TotalCellLayer; ++loop) {
					const vert32& Vert = this->m_DrawThreadDatas[loop].GetVert32();
					const CellsData& cellx = this->m_CellxN[loop];
					if (Vert.GetOutNum() == 0) { continue; }
					printfDx("Vertex[%d]\n", Vert.GetOutNum() * 4);
				}
			}
#endif
		}
		void		VoxelControl::Shadow_Draw(void) const noexcept {
			for (int threadID = TotalCellLayer; threadID < TotalCellLayer + TotalCellLayer; ++threadID) {
				const vert32& Vert = this->m_DrawThreadDatas[threadID].GetVert32();
				Vert.Draw(this->m_tex);
			}
		}
		void		VoxelControl::SetShadow_Draw_Rigid(void) const noexcept {
			constexpr int MinLimit = std::min(std::min(DrawMinXPlus, DrawMinZPlus), DrawMinYPlus) * 3 / 4;
			constexpr int MaxLimit = std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus) * 5 / 4;
			float Near = GetCameraNear() / MaxLimit;
			float Far = GetCameraFar() / MinLimit;

			SetUseTextureToShader(0, this->m_tex.get());
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				const vert32& Vert = this->m_DrawThreadDatas[loop].GetVert32();
				const CellsData& cellx = this->m_CellxN[loop];
				if (Vert.GetOutNum() == 0) { continue; }
				if (!cellx.isFarCells() && !(cellx.Scale < Far)) { continue; }
				if (cellx.isFarCells() && !(Near < cellx.Scale && cellx.Scale < Far)) { continue; }
				Vert.DrawByShader();
			}
			SetUseTextureToShader(0, InvalidID);
		}
		void		VoxelControl::Draw(void) const noexcept {
			constexpr int MinLimit = std::min(std::min(DrawMinXPlus, DrawMinZPlus), DrawMinYPlus) * 3 / 4;
			constexpr int MaxLimit = std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus) * 5 / 4;
			float Near = GetCameraNear() / MaxLimit;
			float Far = GetCameraFar() / MinLimit;

			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				const vert32& Vert = this->m_DrawThreadDatas[loop].GetVert32();
				const CellsData& cellx = this->m_CellxN[loop];
				if (Vert.GetOutNum() == 0) { continue; }
				if (!cellx.isFarCells() && !(cellx.Scale < Far)) { continue; }
				if (cellx.isFarCells() && !(Near < cellx.Scale && cellx.Scale < Far)) { continue; }
				Vert.Draw(this->m_tex);
			}
		}
		void		VoxelControl::Dispose(void) noexcept {
			for (auto& job : m_Jobs) {
				job.Dispose();
			}
			for (auto& Vert : this->m_DrawThreadDatas) {
				Vert.Dispose();
			}
			for (auto& cellx : this->m_CellxN) {
				cellx.Dispose();
			}
		}
		void		VoxelControl::Dispose_Load(void) noexcept {
			this->m_tex.Dispose();
		}
	}
};
