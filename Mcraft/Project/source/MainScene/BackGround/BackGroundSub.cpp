#include	"BackGroundSub.hpp"

namespace FPS_n2 {
	namespace BackGround {
		void		VoxelControl::AddPlaneXPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b101).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b100).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b111).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b110).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneXMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b011).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b010).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b001).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b000).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneYPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 0;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b111).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b110).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b011).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b010).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneYMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xpos, int ypos, int zmin, int zmax, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				zscale = (zmax - zmin + 1) * cellx.ScaleRate;
				Xofs = (center.z + zmin) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xpos, center.y + ypos, center.z + zmin).GetCell() - 1) * 3) + 2;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b001).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b000).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmax, 0b101).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + zscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xpos, center.y + ypos, center.z + zmin, 0b100).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneZPlus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				xscale = (xmax - xmin + 1) * cellx.ScaleRate;
				Xofs = (center.x + xmax) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xmin, center.y + ypos, center.z + zpos).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b001).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b101).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b011).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b111).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneZMinus(vert32* pTarget, const CellsData& cellx, const Vector3Int& center, int xmin, int xmax, int ypos, int zpos, bool IsCalcUV) noexcept {
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			float uAdd{};
			float vAdd{};
			if (IsCalcUV) {
				xscale = (xmax - xmin + 1) * cellx.ScaleRate;
				Xofs = (center.x + xmax) % 2 == 0;
				Yofs = ((cellx.GetCellBuf(center.x + xmin, center.y + ypos, center.z + zpos).GetCell() - 1) * 3) + 1;
				uAdd = 1.0f / 2.0f;
				vAdd = 1.0f / 16.0f;
			}
			auto ZERO = pTarget->GetInNum() * 4 - 4;
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b010).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b110).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 0);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetPosBuffer(center.x + xmin, center.y + ypos, center.z + zpos, 0b000).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + xscale);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
			{
				auto& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetPosBuffer(center.x + xmax, center.y + ypos, center.z + zpos, 0b100).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (IsCalcUV) {
					Vert.u = uAdd * static_cast<float>(Xofs + 0);
					Vert.v = vAdd * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddCubes(size_t id) noexcept {
			auto& cellx = this->m_CellxN[id];
			auto& Draws = this->m_Draws[id];
			Draws.Vert32.ResetNum();
			auto center = cellx.GetPoint(Draws.CamPos);

			float CamXMinX = Draws.CamVec.x * (static_cast<float>(DrawMaxXMinus) + 0.5f);
			float CamXMaxX = Draws.CamVec.x * (static_cast<float>(DrawMaxXPlus) + 0.5f);
			float CamYMinY = Draws.CamVec.y * (static_cast<float>(DrawMaxYMinus) + 0.5f);
			float CamYMaxY = Draws.CamVec.y * (static_cast<float>(DrawMaxYPlus) + 0.5f);
			float CamZMinZ = Draws.CamVec.z * (static_cast<float>(DrawMaxZMinus) + 0.5f);
			float CamZMaxZ = Draws.CamVec.z * (static_cast<float>(DrawMaxZPlus) + 0.5f);
			//X
			for (int zpos = DrawMaxZMinus; zpos <= DrawMaxZPlus; ++zpos) {
				float CamZZ = Draws.CamVec.z * (static_cast<float>(zpos) + 0.5f);
				//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
				if (
					((CamXMinX + CamYMinY + CamZZ) <= 0.0f) &&//Dot
					((CamXMaxX + CamYMaxY + CamZZ) <= 0.0f) &&//Dot
					((CamXMaxX + CamYMinY + CamZZ) <= 0.0f) &&//Dot
					((CamXMinX + CamYMaxY + CamZZ) <= 0.0f)//Dot
					) {
					continue;
				}

				bool CheckFillZ = cellx.isFarCells() && ((DrawMinZMinus <= zpos) && (zpos <= DrawMinZPlus));
				bool CheckInsideZ = cellx.isFarCells() && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus));

				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }
					float CamYY = Draws.CamVec.y * (static_cast<float>(ypos) + 0.5f);

					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					{
						float dTa = (CamXMinX + CamYY + CamZZ);//Dot
						float dTb = (CamXMaxX + CamYY + CamZZ);//Dot
						bool OnFront = (dTa >= 0.0f && dTb >= 0.0f);
						bool Onbehind = (dTa < 0.0f && dTb < 0.0f);
						if (Onbehind && !OnFront) {
							continue;
						}
						if (!OnFront && !Onbehind) {
							if (dTa < 0.0f) {
								xMaxminT = std::max(DrawMaxXMinus + static_cast<int>((DrawMaxXPlus - DrawMaxXMinus) * (dTa / (dTa - dTb))) - 1, DrawMaxXMinus);
							}
							else {
								xMaxmaxT = std::min(DrawMaxXMinus + static_cast<int>((DrawMaxXPlus - DrawMaxXMinus) * (dTa / (dTa - dTb))) + 1, DrawMaxXPlus);
							}
						}
					}

					bool CheckFillYZ = CheckFillZ && ((DrawMinYMinus <= ypos) && (ypos <= DrawMinYPlus));
					bool CheckInsideYZ = CheckInsideZ && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));
					AddPlanesZ(&Draws.Vert32, static_cast<float>(zpos), cellx, center, xMaxminT, xMaxmaxT, ypos, zpos, CheckInsideYZ, CheckFillYZ, true);
				}
			}
			//Z
			for (int xpos = DrawMaxXMinus; xpos <= DrawMaxXPlus; ++xpos) {
				float CamXX = Draws.CamVec.x * (static_cast<float>(xpos) + 0.5f);
				//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
				if (
					((CamXX + CamYMinY + CamZMinZ) <= 0.0f) &&//Dot
					((CamXX + CamYMaxY + CamZMaxZ) <= 0.0f) &&//Dot
					((CamXX + CamYMinY + CamZMaxZ) <= 0.0f) &&//Dot
					((CamXX + CamYMaxY + CamZMinZ) <= 0.0f)//Dot
					) {
					continue;
				}

				bool CheckFillX = cellx.isFarCells() && ((DrawMinXMinus <= xpos) && (xpos <= DrawMinXPlus));
				bool CheckInsideX = cellx.isFarCells() && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus));

				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }
					float CamYY = Draws.CamVec.y * (static_cast<float>(ypos) + 0.5f);

					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					{
						float dTa = (CamXX + CamYY + CamZMinZ);//Dot
						float dTb = (CamXX + CamYY + CamZMaxZ);//Dot
						bool OnFront = (dTa >= 0.0f && dTb >= 0.0f);
						bool Onbehind = (dTa < 0.0f && dTb < 0.0f);
						if (Onbehind && !OnFront) {
							continue;
						}
						if (!OnFront && !Onbehind) {
							if (dTa < 0.0f) {
								zMaxminT = std::max(DrawMaxZMinus + static_cast<int>((DrawMaxZPlus - DrawMaxZMinus) * (dTa / (dTa - dTb))) - 1, DrawMaxZMinus);
							}
							else {
								zMaxmaxT = std::min(DrawMaxZMinus + static_cast<int>((DrawMaxZPlus - DrawMaxZMinus) * (dTa / (dTa - dTb))) + 1, DrawMaxZPlus);
							}
						}
					}
					bool CheckFillXY = CheckFillX && ((DrawMinYMinus <= ypos) && (ypos <= DrawMinYPlus));
					bool CheckInsideXY = CheckInsideX && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));
					AddPlanesXY(&Draws.Vert32, static_cast<float>(xpos), static_cast<float>(ypos), cellx, center, xpos, ypos, zMaxminT, zMaxmaxT, CheckInsideXY, CheckFillXY, true);
				}
			}
		}
		void		VoxelControl::FlipCubes(size_t id) noexcept {
			auto& Draws = this->m_Draws[id];
			Draws.Vert32.FlipVerts();
			auto* CameraParts = Camera3D::Instance();
			Draws.CamPos = CameraParts->GetMainCamera().GetCamPos();
			Draws.CamVec = (CameraParts->GetMainCamera().GetCamVec() - CameraParts->GetMainCamera().GetCamPos()).normalized();
		}
		void		VoxelControl::AddShadowCubes(size_t id) noexcept {
			size_t shadow = TotalCellLayer + id;
			auto& cellx = this->m_CellxN[id];
			auto& Draws = this->m_Draws[shadow];
			Draws.Vert32.ResetNum();
			auto center = cellx.GetPoint(Draws.CamPos);
			//X
			for (int zpos = DrawMaxZMinus; zpos <= DrawMaxZPlus; ++zpos) {
				bool CheckInsideZ = cellx.isFarCells() && ((DrawMinZMinus < zpos) && (zpos < DrawMinZPlus));
				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }
					bool CheckInsideYZ = CheckInsideZ && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));
					AddPlanesZ(&Draws.Vert32, Draws.CamVec.z, cellx, center, DrawMaxXMinus, DrawMaxXPlus, ypos, zpos, CheckInsideYZ, false, false);
				}
			}
			//Z
			for (int xpos = DrawMaxXMinus; xpos <= DrawMaxXPlus; ++xpos) {
				bool CheckInsideX = cellx.isFarCells() && ((DrawMinXMinus < xpos) && (xpos < DrawMinXPlus));
				for (int ypos = DrawMaxYMinus; ypos <= DrawMaxYPlus; ++ypos) {
					if (!cellx.isInside(center.y + ypos)) { continue; }
					bool CheckInsideXY = CheckInsideX && ((DrawMinYMinus < ypos) && (ypos < DrawMinYPlus));
					AddPlanesXY(&Draws.Vert32, Draws.CamVec.x, Draws.CamVec.y, cellx, center, xpos, ypos, DrawMaxZMinus, DrawMaxZPlus, CheckInsideXY, false, false);
				}
			}
		}
		void		VoxelControl::FlipShadowCubes(size_t id) noexcept {
			size_t shadow = TotalCellLayer + id;
			auto* PostPassParts = PostPassEffect::Instance();
			auto* SceneParts = SceneControl::Instance();
			auto& Draws = this->m_Draws[shadow];
			Draws.Vert32.FlipVerts();
			//Draws.CamPos =  Camera3D::Instance()->GetMainCamera().GetCamPos();
			Draws.CamPos = Vector3DX::vget(0.0f, -25.0f, 0.0f) * Scale3DRate;
			Draws.CamVec = PostPassParts->GetShadowDir();
			SceneParts->SetIsUpdateFarShadowActive();
		}
		int			VoxelControl::CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal) const noexcept {
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


			auto Start = GetReferenceCells().GetPoint(StartPos);
			auto End = GetReferenceCells().GetPoint(*EndPos);// *EndPos

			for (int xm = -1; xm <= 1; ++xm) {
				for (int ym = -1; ym <= 1; ++ym) {
					for (int zm = -1; zm <= 1; ++zm) {
						bool isHit = false;
						Bresenham3D(
							Start.x + xm, Start.y + ym, Start.z + zm,
							End.x + xm, End.y + ym, End.z + zm,
							[&](int xpos, int ypos, int zpos) {
								if (!GetReferenceCells().isInside(ypos)) { return false; }
								if (!GetReferenceCells().GetCellBuf(xpos, ypos, zpos).CanDraw()) { return false; }
								Vector3DX MinPos = GetPos(xpos, ypos, zpos) + Vector3DX::one() * -0.1f * Scale3DRate;
								Vector3DX MaxPos = GetPos(xpos + 1, ypos + 1, zpos + 1) + Vector3DX::one() * 0.1f * Scale3DRate;
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
		bool		VoxelControl::CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius, const std::vector<const MV1*>& addonColObj) const noexcept {
			auto MoveVector = *EndPos - StartPos;
			// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
			std::vector<MV1_COLL_RESULT_POLY> kabes;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			auto Start = GetReferenceCells().GetPoint(StartPos);
			auto End = GetReferenceCells().GetPoint(*EndPos);// *EndPos

			for (int xm = -3; xm <= 3; ++xm) {
				for (int ym = 3; ym <= 6; ++ym) {
					for (int zm = -3; zm <= 3; ++zm) {
						Bresenham3D(
							Start.x + xm, Start.y + ym, Start.z + zm,
							End.x + xm, End.y + ym, End.z + zm,
							[&](int xpos, int ypos, int zpos) {
								if (!GetReferenceCells().isInside(ypos)) { return false; }
								if (!GetReferenceCells().GetCellBuf(xpos, ypos, zpos).CanDraw()) { return false; }
								Vector3DX MinPos = GetPos(xpos, ypos, zpos);
								Vector3DX MaxPos = GetPos(xpos + 1, ypos + 1, zpos + 1);

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
				auto HitDim = ad->CollCheck_Sphere(StartPos, AddCapsuleMax.magnitude() + MoveVector.magnitude());
				// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
				for (int i = 0; i < HitDim.HitNum; ++i) {
					auto& h_d = HitDim.Dim[i];
					kabes.emplace_back(h_d);// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
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
		void		VoxelControl::LoadCellsFile(void) noexcept {
			{
				SetReferenceCells().SetScale(0);

				std::ifstream fin{};
				fin.open("data/Map.txt", std::ios::in | std::ios::binary);
				this->m_CellBase.resize(static_cast<size_t>(256 * 256 * 256));
				fin.read((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * 256 * 256 * 256);
				fin.close();
				for (int xm = 0; xm < GetReferenceCells().All; ++xm) {
					for (int ym = 0; ym < GetReferenceCells().All; ++ym) {
						for (int zm = 0; zm < GetReferenceCells().All; ++zm) {
							SetReferenceCells().SetCellBuf(xm, ym, zm).Cell = this->m_CellBase[GetReferenceCells().GetCellNum(xm, ym, zm)];
						}
					}
				}
			}
			//一部をペースト
			//LoadCellsClip(0, 128, 0);
			/*
			for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
				if (xpos - GetReferenceCells().Half > -72) {
					for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
						for (int ypos = GetReferenceCells().All; ypos >=130+15 ; --ypos) {
							SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = GetReferenceCells().GetCellBuf(xpos, ypos - 15, zpos).GetCell();
						}
					}
				}
			}
			//*/
			/*
			for (int xpos = 0; xpos < GetReferenceCells().All; ++xpos) {
				for (int zpos = 0; zpos < GetReferenceCells().All; ++zpos) {
					for (int ypos = GetReferenceCells().All; ypos >= 133; --ypos) {
						SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = 0;
					}
				}
			}
			//*/
		}
		void		VoxelControl::SaveCellsFile(void) noexcept {
			{
				this->m_CellBase.resize(static_cast<size_t>(256 * 256 * 256));
				for (int xm = 0; xm < GetReferenceCells().All; ++xm) {
					for (int ym = 0; ym < GetReferenceCells().All; ++ym) {
						for (int zm = 0; zm < GetReferenceCells().All; ++zm) {
							this->m_CellBase[GetReferenceCells().GetCellNum(xm, ym, zm)] = GetReferenceCells().GetCellBuf(xm, ym, zm).GetCell();
						}
					}
				}

				std::ofstream fout{};
				fout.open("data/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
				fout.write((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * 256 * 256 * 256);
				fout.close(); //ファイルを閉じる
			}
			//一部を切り取って保存
			/*
			int XMin = 37;
			int XMax = 144;
			int YMin = 128;
			int YMax = 205;
			int ZMin = 91;
			int ZMax = 153;
			SaveCellsClip(XMin, XMax, YMin, YMax, ZMin, ZMax);
			//*/
		}
		void		VoxelControl::LoadCellsClip(int xbasepos, int ybasepos, int zbasepos) noexcept {
			int XTotal = 1;
			int YTotal = 1;
			int ZTotal = 1;
			std::ifstream fin{};
			fin.open("data/Build/Map.txt", std::ios::in | std::ios::binary);
			fin.read((char*)&XTotal, sizeof(XTotal));
			fin.read((char*)&YTotal, sizeof(YTotal));
			fin.read((char*)&ZTotal, sizeof(ZTotal));
			this->m_CellBase.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
			fin.read((char*)this->m_CellBase.data(), static_cast<size_t>(sizeof(this->m_CellBase[0])) * XTotal * YTotal * ZTotal);
			fin.close(); //ファイルを閉じる

			for (int xm = 0; xm < XTotal; ++xm) {
				for (int ym = 0; ym < YTotal; ++ym) {
					for (int zm = 0; zm < ZTotal; ++zm) {
						SetReferenceCells().SetCellBuf(xm + xbasepos, ym + ybasepos, zm + zbasepos).Cell =
							this->m_CellBase[static_cast<size_t>(xm * YTotal * ZTotal + ym * ZTotal + zm)];
					}
				}
			}
		}
		void		VoxelControl::SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax) noexcept {
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
			fout.open("data/Build/Map.txt", std::ios::out | std::ios::binary | std::ios::trunc);
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
		void		VoxelControl::SetBlick(int xpos, int ypos, int zpos, int8_t select) noexcept {
			if (!GetReferenceCells().isInside(ypos)) { return; }
			//テクスチャだけ変える
			/*
			if (GetReferenceCells().GetCellBuf(xpos, ypos, zpos).IsEmpty()) { return; }
			SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = select;
			return;
			//*/
			//
			SetReferenceCells().SetCellBuf(xpos, ypos, zpos).Cell = select;
			//簡易版を更新
			for (int loop = 1; loop < TotalCellLayer; ++loop) {
				auto& cell1 = this->m_CellxN[loop];

				int xm = xpos / cell1.ScaleRate;
				int ym = ypos / cell1.ScaleRate;
				int zm = zpos / cell1.ScaleRate;

				cell1.SetCellBuf(xm, ym, zm).Cell = GetReferenceCells().isFill(xm, ym, zm, cell1.ScaleRate);
			}
			//遮蔽検索
			for (auto& cellx : this->m_CellxN) {
				int xm = xpos / cellx.ScaleRate;
				int ym = ypos / cellx.ScaleRate;
				int zm = zpos / cellx.ScaleRate;
				cellx.CalcOcclusion(xm + 1, ym, zm);
				cellx.CalcOcclusion(xm - 1, ym, zm);
				cellx.CalcOcclusion(xm, ym + 1, zm);
				cellx.CalcOcclusion(xm, ym - 1, zm);
				cellx.CalcOcclusion(xm, ym, zm + 1);
				cellx.CalcOcclusion(xm, ym, zm - 1);
			}
		}
		//
		void		VoxelControl::Load(void) noexcept {
			this->m_tex.Load("data/tex.png");
		}
		void		VoxelControl::Init(void) noexcept {
			//簡略版を制作
			for (int loop = 1; loop < TotalCellLayer; ++loop) {
				auto& cell1 = this->m_CellxN[loop];
				cell1.SetScale(loop);
				for (int xm = 0; xm < cell1.All; ++xm) {
					for (int ym = 0; ym < cell1.All; ++ym) {
						for (int zm = 0; zm < cell1.All; ++zm) {
							cell1.SetCellBuf(xm, ym, zm).Cell = GetReferenceCells().isFill(xm, ym, zm, cell1.ScaleRate);
						}
					}
				}
			}
			//遮蔽検索
			for (auto& cellx : this->m_CellxN) {
				for (int xpos = 0; xpos < cellx.All; ++xpos) {
					for (int ypos = 0; ypos < cellx.All; ++ypos) {
						for (int zpos = 0; zpos < cellx.All; ++zpos) {
							cellx.CalcOcclusion(xpos, ypos, zpos);
						}
					}
				}
			}

			//
			constexpr size_t size = (DrawMaxXPlus - DrawMaxXMinus) * (DrawMaxYPlus - DrawMaxYMinus) * (DrawMaxZPlus - DrawMaxZMinus) / 2 * 3 / 100;
			for (auto& Vert : this->m_Draws) {
				Vert.Vert32.Init(size);
			}
			//
			this->m_Jobs[0].Init(
				[&]() { AddCubes(0); },
				[&]() { FlipCubes(0); }, false);
			this->m_Jobs[1].Init(
				[&]() { AddCubes(1); },
				[&]() { FlipCubes(1); }, false);
			this->m_Jobs[2].Init(
				[&]() { AddCubes(2); },
				[&]() { FlipCubes(2); }, false);
			this->m_Jobs[3].Init(
				[&]() { AddCubes(3); },
				[&]() { FlipCubes(3); }, false);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 0)].Init(
				[&]() { AddShadowCubes(0); },
				[&]() { FlipShadowCubes(0); }, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 1)].Init(
				[&]() { AddShadowCubes(1); },
				[&]() { FlipShadowCubes(1); }, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 2)].Init(
				[&]() { AddShadowCubes(2); },
				[&]() { FlipShadowCubes(2); }, true);
			this->m_Jobs[static_cast<size_t>(TotalCellLayer + 3)].Init(
				[&]() { AddShadowCubes(3); },
				[&]() { FlipShadowCubes(3); }, true);
			SettingChange();
			this->m_ThreadCounter = 0;
			this->m_isChangeBlock = false;
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
				auto& cell1 = this->m_CellxN[loop];
				//
				if (this->m_BaseRate < cell1.ScaleInt) {
					this->m_Draws[loop].Vert32.Disable();
				}
				else {
					this->m_Jobs[loop].Update();
				}
				//
				int shadow = TotalCellLayer + loop;
				if ((OptionParts->GetParamInt(EnumSaveParam::shadow) == 0) || (this->m_ShadowRate < cell1.ScaleInt)) {
					this->m_Draws[shadow].Vert32.Disable();
					this->m_Jobs[shadow].UpdateDisable();
				}
				else {
					this->m_Jobs[shadow].Update();
				}
			}
			++this->m_ThreadCounter %= TotalCellLayer;

#if defined(DEBUG) && FALSE
			{
				for (int loop = 0; loop < TotalCellLayer; ++loop) {
					auto& Vert = this->m_Draws[loop].Vert32;
					auto& cell1 = this->m_CellxN[loop];
					if (Vert.GetOutNum() == 0) { continue; }
					printfDx("Vertex[%d]\n", Vert.GetOutNum() * 4);
				}
			}
#endif
		}
		void		VoxelControl::Shadow_Draw(void) const noexcept {
			for (int shadow = TotalCellLayer; shadow < TotalCellLayer + TotalCellLayer; ++shadow) {
				auto& Vert = this->m_Draws[shadow].Vert32;
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
				auto& Vert = this->m_Draws[loop].Vert32;
				auto& cell1 = this->m_CellxN[loop];
				if (Vert.GetOutNum() == 0) { continue; }
				if (!cell1.isFarCells() && !(cell1.Scale < Far)) { continue; }
				if (cell1.isFarCells() && !(Near < cell1.Scale && cell1.Scale < Far)) { continue; }
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
				auto& Vert = this->m_Draws[loop].Vert32;
				auto& cell1 = this->m_CellxN[loop];
				if (Vert.GetOutNum() == 0) { continue; }
				if (!cell1.isFarCells() && !(cell1.Scale < Far)) { continue; }
				if (cell1.isFarCells() && !(Near < cell1.Scale && cell1.Scale < Far)) { continue; }
				Vert.Draw(this->m_tex);
			}

			/*
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& ViewPlayer = PlayerMngr->GetWatchPlayer();
			auto& ViewChara = ViewPlayer->GetChara();
			float Height = ViewChara->GetEyePositionCache().y;
			for (auto& b : this->m_ObjBuilds) {
				Vector3DX Pos = b.GetPos();
				Pos.y = Height;
				DrawCube3D((Pos - Vector3DX::vget(1.f, 1.f, 1.f)).get(), (Pos + Vector3DX::vget(1.f, 1.f, 1.f)).get(), GetColor(0, 255, 0), GetColor(0, 255, 0), true);
			}
			//*/
		}
		void		VoxelControl::Dispose(void) noexcept {
			for (auto& job : m_Jobs) {
				job.Dispose();
			}
			for (auto& Vert : this->m_Draws) {
				Vert.Vert32.Dispose();
			}
			for (auto& cellx : this->m_CellxN) {
				cellx.m_CellBuffer.clear();
			}
		}
		void		VoxelControl::Dispose_Load(void) noexcept {
			this->m_tex.Dispose();
		}
	}
};
