#include	"BackGroundSub.hpp"

namespace FPS_n2 {
	namespace BackGround {
		void		VoxelControl::AddPlaneXPlus(vert32* pTarget, size_t id, const Vector3Int& VCenter, const Vector3Int& Vofs, int zmin, int zmax, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (useTexture) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (VCenter.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmax, 0b101).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin, 0b100).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmax, 0b111).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin, 0b110).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneXMinus(vert32* pTarget, size_t id, const Vector3Int& VCenter, const Vector3Int& Vofs, int zmin, int zmax, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (useTexture) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (VCenter.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmax, 0b011).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin, 0b010).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmax, 0b001).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin, 0b000).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneYPlus(vert32* pTarget, size_t id, const Vector3Int& VCenter, const Vector3Int& Vofs, int zmin, int zmax, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (useTexture) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (VCenter.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin).GetCell() - 1) * 3) + 0;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmax, 0b111).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin, 0b110).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmax, 0b011).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin, 0b010).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneYMinus(vert32* pTarget, size_t id, const Vector3Int& VCenter, const Vector3Int& Vofs, int zmin, int zmax, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (useTexture) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (VCenter.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin).GetCell() - 1) * 3) + 2;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmax, 0b001).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin, 0b000).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmax, 0b101).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + Vofs.x, VCenter.y + Vofs.y, VCenter.z + zmin, 0b100).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneZPlus(vert32* pTarget, size_t id, const Vector3Int& VCenter, int xmin, int xmax, const Vector3Int& Vofs, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (useTexture) {
				xscale = (xmax - xmin + 1) * cellx.ScaleRate;
				Xofs = (VCenter.x + xmax) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(VCenter.x + xmin, VCenter.y + Vofs.y, VCenter.z + Vofs.z).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + xmin, VCenter.y + Vofs.y, VCenter.z + Vofs.z, 0b001).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + xmax, VCenter.y + Vofs.y, VCenter.z + Vofs.z, 0b101).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + xmin, VCenter.y + Vofs.y, VCenter.z + Vofs.z, 0b011).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + xmax, VCenter.y + Vofs.y, VCenter.z + Vofs.z, 0b111).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneZMinus(vert32* pTarget, size_t id, const Vector3Int& VCenter, int xmin, int xmax, const Vector3Int& Vofs, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (useTexture) {
				xscale = (xmax - xmin + 1) * cellx.ScaleRate;
				Xofs = (VCenter.x + xmax) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(VCenter.x + xmin, VCenter.y + Vofs.y, VCenter.z + Vofs.z).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + xmin, VCenter.y + Vofs.y, VCenter.z + Vofs.z, 0b010).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + xmax, VCenter.y + Vofs.y, VCenter.z + Vofs.z, 0b110).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + xmin, VCenter.y + Vofs.y, VCenter.z + Vofs.z, 0b000).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(VCenter.x + xmax, VCenter.y + Vofs.y, VCenter.z + Vofs.z, 0b100).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}

		void		VoxelControl::AddPlanesXY(vert32* pTarget, bool isDrawXPlus, bool isDrawYPlus, size_t id, const Vector3Int& VCenter, const Vector3Int& Vofs, int MaxminT, int MaxmaxT, int Minmin, int Minmax, bool CheckInsideXY, bool CheckFillXY, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			int Min = 0;
			int Max = 0;
			bool isHitmin = true;
			int8_t selectBlock = s_EmptyBlick;
			bool CanDrawXYZ[6] = { false, false, false, false, false, false };
			Vector3Int Vofset = Vofs;
			for (Vofset.z = MaxminT; Vofset.z <= MaxmaxT; ++Vofset.z) {
				auto& CellBuff = cellx.GetCellBuf(VCenter.x + Vofset.x, VCenter.y + Vofset.y, VCenter.z + Vofset.z);
				bool CheckInsideZ = (Minmin < Vofset.z) && (Vofset.z < Minmax);
				bool CheckBlockID = useTexture && (selectBlock != CellBuff.GetCell());
				if (
					(Vofset.z == MaxmaxT)
					|| (CheckInsideXY && CheckInsideZ)
					|| (!isHitmin && CheckBlockID)
					|| !CellBuff.CanDraw()
					) {
					//置けない部分なので今まで置けていた分をまとめてポリゴン化
					if (!isHitmin) {
						isHitmin = true;
						bool CheckFillXYZ = CheckFillXY && (Min <= Minmax && Minmin <= Max);
						if (isDrawXPlus) {
							if (CheckFillXYZ || CanDrawXYZ[0]) {
								AddPlaneXPlus(pTarget, id, VCenter, Vofset, Min, Max, useTexture);
							}
						}
						else {
							if (CheckFillXYZ || CanDrawXYZ[1]) {
								AddPlaneXMinus(pTarget, id, VCenter, Vofset, Min, Max, useTexture);
							}
						}
						if (isDrawYPlus) {
							if (CheckFillXYZ || CanDrawXYZ[2]) {
								AddPlaneYPlus(pTarget, id, VCenter, Vofset, Min, Max, useTexture);
							}
						}
						else {
							if (CheckFillXYZ || CanDrawXYZ[3]) {
								AddPlaneYMinus(pTarget, id, VCenter, Vofset, Min, Max, useTexture);
							}
						}
						//この場合だけもう一回判定させるドン
						if (CheckBlockID) {
							--Vofset.z;
						}
					}
				}
				else {
					//ブロックが置ける部分
					if (isHitmin) {
						isHitmin = false;
						Min = Vofset.z;
						selectBlock = CellBuff.GetCell();
						CanDrawXYZ[0] = false;
						CanDrawXYZ[1] = false;
						CanDrawXYZ[2] = false;
						CanDrawXYZ[3] = false;
					}
					Max = Vofset.z;
					if (!CanDrawXYZ[0] && !CellBuff.IsOcclusion(0)) {
						CanDrawXYZ[0] = true;
					}
					if (!CanDrawXYZ[1] && !CellBuff.IsOcclusion(1)) {
						CanDrawXYZ[1] = true;
					}
					if (!CanDrawXYZ[2] && !CellBuff.IsOcclusion(2)) {
						CanDrawXYZ[2] = true;
					}
					if (!CanDrawXYZ[3] && !CellBuff.IsOcclusion(3)) {
						CanDrawXYZ[3] = true;
					}
				}
			}
		}
		void		VoxelControl::AddPlanesZ(vert32* pTarget, bool isDrawZPlus, size_t id, const Vector3Int& VCenter, const Vector3Int& Vofs, int MaxminT, int MaxmaxT, int Minmin, int Minmax, bool CheckInsideYZ, bool CheckFillYZ, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			int Min = 0;
			int Max = 0;
			bool isHitmin = true;
			int8_t selectBlock = s_EmptyBlick;
			bool CanDrawXYZ[6] = { false, false, false, false, false, false };
			Vector3Int Vofset = Vofs;
			for (Vofset.x = MaxminT; Vofset.x <= MaxmaxT; ++Vofset.x) {
				auto& CellBuff = cellx.GetCellBuf(VCenter.x + Vofset.x, VCenter.y + Vofset.y, VCenter.z + Vofset.z);
				bool CheckInsideX = (Minmin < Vofset.x) && (Vofset.x < Minmax);
				bool CheckBlockID = useTexture && (selectBlock != CellBuff.GetCell());
				if (
					(Vofset.x == MaxmaxT)
					|| (CheckInsideYZ && CheckInsideX)
					|| (!isHitmin && CheckBlockID)
					|| !CellBuff.CanDraw()
					) {
					//置けない部分なので今まで置けていた分をまとめてポリゴン化
					if (!isHitmin) {
						isHitmin = true;
						bool CheckFillX = Min <= Minmax && Minmin <= Max;
						bool CheckFillXYZ = CheckFillYZ && CheckFillX;
						if (isDrawZPlus) {
							if (CheckFillXYZ || CanDrawXYZ[4]) {
								AddPlaneZPlus(pTarget, id, VCenter, Min, Max, Vofset, useTexture);
							}
						}
						else {
							if (CheckFillXYZ || CanDrawXYZ[5]) {
								AddPlaneZMinus(pTarget, id, VCenter, Min, Max, Vofset, useTexture);
							}
						}
						//この場合だけもう一回判定させるドン
						if (CheckBlockID) {
							--Vofset.x;
						}
					}
				}
				else {
					//ブロックが置ける部分
					if (isHitmin) {
						isHitmin = false;
						Min = Vofset.x;
						selectBlock = CellBuff.GetCell();
						CanDrawXYZ[4] = false;
						CanDrawXYZ[5] = false;
					}
					Max = Vofset.x;
					if (!CanDrawXYZ[4] && !CellBuff.IsOcclusion(4)) {
						CanDrawXYZ[4] = true;
					}
					if (!CanDrawXYZ[5] && !CellBuff.IsOcclusion(5)) {
						CanDrawXYZ[5] = true;
					}
				}
			}
		}

		static bool CullingLine(int* MaxminT, int* MaxmaxT, float dTa, float dTb) noexcept {
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

		void		VoxelControl::AddCubes(size_t id, size_t threadID, bool UseFrustumCulling, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			DrawThreadData& Draws = this->m_DrawThreadDatas[threadID];
			Draws.StartRegist();
			Vector3Int VCenter = cellx.GetVoxelPoint(Draws.GetDrawCenterPos());

			float CamXMinX = 0.f;
			float CamXMaxX = 0.f;
			float CamYMinY = 0.f;
			float CamYMaxY = 0.f;
			float CamZMinZ = 0.f;
			float CamZMaxZ = 0.f;
			if (UseFrustumCulling) {
				CamXMinX = Draws.GetCamVec().x * (static_cast<float>(DrawMaxXMinus) + 0.5f);
				CamXMaxX = Draws.GetCamVec().x * (static_cast<float>(DrawMaxXPlus) + 0.5f);
				CamYMinY = Draws.GetCamVec().y * (static_cast<float>(DrawMaxYMinus) + 0.5f);
				CamYMaxY = Draws.GetCamVec().y * (static_cast<float>(DrawMaxYPlus) + 0.5f);
				CamZMinZ = Draws.GetCamVec().z * (static_cast<float>(DrawMaxZMinus) + 0.5f);
				CamZMaxZ = Draws.GetCamVec().z * (static_cast<float>(DrawMaxZPlus) + 0.5f);
			}
			//X
			Vector3Int Vofs{};
			for (Vofs.z = DrawMaxZMinus; Vofs.z <= DrawMaxZPlus; ++Vofs.z) {
				float CamVecDotZ = Draws.GetCamVec().z * (static_cast<float>(Vofs.z) + 0.5f);
				bool CheckFillZ = false;
				if (UseFrustumCulling) {
					//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
					if (
						((CamXMinX + CamYMinY + CamVecDotZ) <= 0.0f) &&//Dot
						((CamXMaxX + CamYMaxY + CamVecDotZ) <= 0.0f) &&//Dot
						((CamXMaxX + CamYMinY + CamVecDotZ) <= 0.0f) &&//Dot
						((CamXMinX + CamYMaxY + CamVecDotZ) <= 0.0f)//Dot
						) { continue; }
					CheckFillZ = cellx.isFarCells() && ((DrawMinZMinus <= Vofs.z) && (Vofs.z <= DrawMinZPlus));
				}
				bool CheckInsideZ = cellx.isFarCells() && ((DrawMinZMinus < Vofs.z) && (Vofs.z < DrawMinZPlus));
				for (Vofs.y = DrawMaxYMinus; Vofs.y <= DrawMaxYPlus; ++Vofs.y) {
					if (!cellx.isInside(VCenter.y + Vofs.y)) { continue; }
					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					bool CheckFillY = false;
					if (UseFrustumCulling) {
						float CamVecDotY = Draws.GetCamVec().y * (static_cast<float>(Vofs.y) + 0.5f);
						if (!CullingLine(&xMaxminT, &xMaxmaxT, CamXMinX + CamVecDotY + CamVecDotZ, CamXMaxX + CamVecDotY + CamVecDotZ)) {
							continue;
						}
						CheckFillY = (DrawMinYMinus <= Vofs.y) && (Vofs.y <= DrawMinYPlus);
					}
					bool CheckInsideY = (DrawMinYMinus < Vofs.y) && (Vofs.y < DrawMinYPlus);
					AddPlanesZ(&Draws.Vert32,
						UseFrustumCulling ? (Vofs.z < 0.f) : (Draws.GetCamVec().z < 0.f),
						id, VCenter, Vofs, xMaxminT, xMaxmaxT, DrawMinXMinus, DrawMinXPlus, CheckInsideZ && CheckInsideY, CheckFillZ && CheckFillY, useTexture);
				}
			}
			//Z
			for (Vofs.x = DrawMaxXMinus; Vofs.x <= DrawMaxXPlus; ++Vofs.x) {
				float CamVecDotX = Draws.GetCamVec().x * (static_cast<float>(Vofs.x) + 0.5f);
				bool CheckFillX = false;
				if (UseFrustumCulling) {
					//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
					if (
						((CamVecDotX + CamYMinY + CamZMinZ) <= 0.0f) &&//Dot
						((CamVecDotX + CamYMaxY + CamZMaxZ) <= 0.0f) &&//Dot
						((CamVecDotX + CamYMinY + CamZMaxZ) <= 0.0f) &&//Dot
						((CamVecDotX + CamYMaxY + CamZMinZ) <= 0.0f)//Dot
						) { continue; }
					CheckFillX = cellx.isFarCells() && ((DrawMinXMinus <= Vofs.x) && (Vofs.x <= DrawMinXPlus));
				}
				bool CheckInsideX = cellx.isFarCells() && ((DrawMinXMinus < Vofs.x) && (Vofs.x < DrawMinXPlus));
				for (Vofs.y = DrawMaxYMinus; Vofs.y <= DrawMaxYPlus; ++Vofs.y) {
					if (!cellx.isInside(VCenter.y + Vofs.y)) { continue; }
					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					bool CheckFillXY = false;
					if (UseFrustumCulling) {
						float CamVecDotY = Draws.GetCamVec().y * (static_cast<float>(Vofs.y) + 0.5f);
						if (!CullingLine(&zMaxminT, &zMaxmaxT, CamVecDotX + CamVecDotY + CamZMinZ, CamVecDotX + CamVecDotY + CamZMaxZ)) {
							continue;
						}
						CheckFillXY = CheckFillX && ((DrawMinYMinus <= Vofs.y) && (Vofs.y <= DrawMinYPlus));
					}
					bool CheckInsideY = (DrawMinYMinus < Vofs.y) && (Vofs.y < DrawMinYPlus);
					AddPlanesXY(&Draws.Vert32,
						UseFrustumCulling ? (Vofs.x < 0) : (Draws.GetCamVec().x < 0.0f),
						UseFrustumCulling ? (Vofs.y < 0) : (Draws.GetCamVec().y < 0.0f),
						id, VCenter, Vofs, zMaxminT, zMaxmaxT, DrawMinZMinus, DrawMinZPlus, CheckInsideX && CheckInsideY, CheckFillXY, useTexture);
				}
			}
		}
		void		VoxelControl::FlipCubes(size_t threadID, const Vector3DX& camPos, const Vector3DX& camVec) noexcept {
			DrawThreadData& Draws = this->m_DrawThreadDatas[threadID];
			Draws.EndRegist();
			Draws.SetDrawCenterPos(camPos);
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

			for (int Xofs = -1; Xofs <= 1; ++Xofs) {
				for (int Yofs = -1; Yofs <= 1; ++Yofs) {
					for (int Zofs = -1; Zofs <= 1; ++Zofs) {
						bool isHit = false;
						Bresenham3D(
							Start.x + Xofs, Start.y + Yofs, Start.z + Zofs,
							End.x + Xofs, End.y + Yofs, End.z + Zofs,
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

			for (int Xofs = -3; Xofs <= 3; ++Xofs) {
				for (int Yofs = 3; Yofs <= 6; ++Yofs) {
					for (int Zofs = -3; Zofs <= 3; ++Zofs) {
						Bresenham3D(
							Start.x + Xofs, Start.y + Yofs, Start.z + Zofs,
							End.x + Xofs, End.y + Yofs, End.z + Zofs,
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
		//
		void		VoxelControl::LoadCellsFile(std::string_view Path) noexcept {
			std::ifstream fin{};
			fin.open(Path, std::ios::in | std::ios::binary);
			this->m_CellBase.resize(static_cast<size_t>(256 * 256 * 256));
			fin.read((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * 256 * 256 * 256);
			fin.close();
			for (int Xvoxel = 0; Xvoxel < GetReferenceCells().All; ++Xvoxel) {
				for (int Yvoxel = 0; Yvoxel < GetReferenceCells().All; ++Yvoxel) {
					for (int Zvoxel = 0; Zvoxel < GetReferenceCells().All; ++Zvoxel) {
						SetBlick(Xvoxel, Yvoxel, Zvoxel, this->m_CellBase[GetReferenceCells().GetCellNum(Xvoxel, Yvoxel, Zvoxel)], false);
					}
				}
			}
		}
		void		VoxelControl::SaveCellsFile(std::string_view Path) noexcept {
			this->m_CellBase.resize(static_cast<size_t>(256 * 256 * 256));
			for (int Xvoxel = 0; Xvoxel < GetReferenceCells().All; ++Xvoxel) {
				for (int Yvoxel = 0; Yvoxel < GetReferenceCells().All; ++Yvoxel) {
					for (int Zvoxel = 0; Zvoxel < GetReferenceCells().All; ++Zvoxel) {
						this->m_CellBase[GetReferenceCells().GetCellNum(Xvoxel, Yvoxel, Zvoxel)] = GetReferenceCells().GetCellBuf(Xvoxel, Yvoxel, Zvoxel).GetCell();
					}
				}
			}

			std::ofstream fout{};
			fout.open(Path, std::ios::out | std::ios::binary | std::ios::trunc);
			fout.write((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * 256 * 256 * 256);
			fout.close(); //ファイルを閉じる
		}
		void		VoxelControl::LoadCellsClip(int Xvoxel, int Yvoxel, int Zvoxel, std::string_view Path) noexcept {
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

			for (int Xofs = 0; Xofs < XTotal; ++Xofs) {
				for (int Yofs = 0; Yofs < YTotal; ++Yofs) {
					for (int Zofs = 0; Zofs < ZTotal; ++Zofs) {
						SetBlick(Xofs + Xvoxel, Yofs + Yvoxel, Zofs + Zvoxel, this->m_CellBase[static_cast<size_t>(Xofs * YTotal * ZTotal + Yofs * ZTotal + Zofs)], false);
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
		//
		void		VoxelControl::SettingChange(void) noexcept {
			this->m_ShadowRate = 0;
			auto* OptionParts = OptionManager::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) == 0) {
				this->m_ShadowRate = -1;
			}
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
				for (int Xvoxel = 0; Xvoxel < cellx.All; ++Xvoxel) {
					for (int  Yvoxel = 0;  Yvoxel < cellx.All; ++Yvoxel) {
						for (int Zvoxel = 0;  Zvoxel < cellx.All; ++Zvoxel) {
							cellx.SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetCell(GetReferenceCells().isFill(Xvoxel, Yvoxel, Zvoxel, cellx.ScaleRate));
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
				[this]() { FlipCubes(0, this->m_DrawCenterPos, this->m_CamVec); }, false);
			this->m_Jobs[1].Init(
				[this]() { AddCubes(1, 1, true, true); },
				[this]() { FlipCubes(1, this->m_DrawCenterPos, this->m_CamVec); }, false);
			this->m_Jobs[2].Init(
				[this]() { AddCubes(2, 2, true, true); },
				[this]() { FlipCubes(2, this->m_DrawCenterPos, this->m_CamVec); }, false);
			this->m_Jobs[3].Init(
				[this]() { AddCubes(3, 3, true, true); },
				[this]() { FlipCubes(3, this->m_DrawCenterPos, this->m_CamVec); }, false);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 0)].Init(
				[this]() { AddCubes(0, static_cast<size_t>(TotalCellLayer + 0), false, false); },
				[this]() {
					FlipCubes(static_cast<size_t>(TotalCellLayer + 0), this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
					SceneControl::Instance()->SetIsUpdateFarShadowActive();
				}, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 1)].Init(
				[this]() { AddCubes(1, static_cast<size_t>(TotalCellLayer + 1), false, false); },
				[this]() {
					FlipCubes(static_cast<size_t>(TotalCellLayer + 1), this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
					SceneControl::Instance()->SetIsUpdateFarShadowActive();
				}, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 2)].Init(
				[this]() { AddCubes(2, static_cast<size_t>(TotalCellLayer + 2), false, false); },
				[this]() {
					FlipCubes(static_cast<size_t>(TotalCellLayer + 2), this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
					SceneControl::Instance()->SetIsUpdateFarShadowActive();
				}, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 3)].Init(
				[this]() { AddCubes(3, static_cast<size_t>(TotalCellLayer + 3), false, false); },
				[this]() {
					FlipCubes(static_cast<size_t>(TotalCellLayer + 3), this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
					SceneControl::Instance()->SetIsUpdateFarShadowActive();
				}, true);
			this->m_ThreadCounter = 0;
			this->m_isChangeBlock = false;
			//
			SettingChange();
		}
		void		VoxelControl::Update(void) noexcept {
			auto* CameraParts = Camera3D::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			this->m_DrawCenterPos = CameraParts->GetMainCamera().GetCamPos();
			this->m_CamVec = (CameraParts->GetMainCamera().GetCamVec() - CameraParts->GetMainCamera().GetCamPos()).normalized();
			this->m_ShadowDrawCenterPos = Vector3DX::vget(0.0f, -25.0f, 0.0f) * Scale3DRate;
			this->m_ShadowCamVec = PostPassParts->GetShadowDir();
			if (this->m_isChangeBlock) {
				this->m_isChangeBlock = false;
				for (int loop = 0; loop < TotalCellLayer; ++loop) {
					this->m_Jobs[static_cast<size_t>(TotalCellLayer + loop)].JobStart();
				}
			}
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
				if ((this->m_ShadowRate < cellx.GetScale())) {
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
