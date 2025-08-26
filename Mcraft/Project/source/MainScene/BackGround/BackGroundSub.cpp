#include	"BackGroundSub.hpp"

namespace FPS_n2 {
	namespace BackGround {
		void		VoxelControl::AddPlaneXPlus(vert32* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			if (useTexture) {
				zscale = (Voxel2.z - Voxel1.z + 1) * cellx.ScaleRate;
				Xofs = Voxel1.z % 2 == 0;
				Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 1;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b101).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + zscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b100).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b111).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + zscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b110).get();
				Vert.norm = Vector3DX::right().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneXMinus(vert32* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			if (useTexture) {
				zscale = (Voxel2.z - Voxel1.z + 1) * cellx.ScaleRate;
				Xofs = Voxel1.z % 2 == 0;
				Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 1;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b011).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + zscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b010).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b001).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + zscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b000).get();
				Vert.norm = Vector3DX::left().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneYPlus(vert32* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			if (useTexture) {
				zscale = (Voxel2.z - Voxel1.z + 1) * cellx.ScaleRate;
				Xofs = Voxel1.z % 2 == 0;
				Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 0;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b111).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + zscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b110).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b011).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + zscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b010).get();
				Vert.norm = Vector3DX::up().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneYMinus(vert32* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int zscale{};
			int Xofs{};
			int Yofs{};
			if (useTexture) {
				zscale = (Voxel2.z - Voxel1.z + 1) * cellx.ScaleRate;
				Xofs = Voxel1.z % 2 == 0;
				Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 2;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b001).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + zscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b000).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b101).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + zscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b100).get();
				Vert.norm = Vector3DX::down().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneZPlus(vert32* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			if (useTexture) {
				xscale = (Voxel2.x - Voxel1.x + 1) * cellx.ScaleRate;
				Xofs = Voxel2.x % 2 == 0;
				Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 1;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b001).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + xscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b101).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b011).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + xscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b111).get();
				Vert.norm = Vector3DX::forward().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
		}
		void		VoxelControl::AddPlaneZMinus(vert32* pTarget, size_t id, const Vector3Int& Voxel1, const Vector3Int& Voxel2, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			pTarget->AllocatePlane();

			int xscale{};
			int Xofs{};
			int Yofs{};
			if (useTexture) {
				xscale = (Voxel2.x - Voxel1.x + 1) * cellx.ScaleRate;
				Xofs = Voxel2.x % 2 == 0;
				Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 1;
			}
			size_t ZERO = pTarget->GetInNum() * 4 - 4;
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 0];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b010).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + xscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 1];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b110).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 0);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 2];
				Vert.pos = cellx.GetWorldPosOffset(Voxel1, 0b000).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + xscale);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
			{
				VERTEX3D& Vert = pTarget->SetInVert()[ZERO + 3];
				Vert.pos = cellx.GetWorldPosOffset(Voxel2, 0b100).get();
				Vert.norm = Vector3DX::back().get();
				Vert.dif = GetColorU8(128, 128, 128, 255);
				Vert.spc = GetColorU8(64, 64, 64, 255);
				if (useTexture) {
					Vert.u = TexTileU * static_cast<float>(Xofs + 0);
					Vert.v = TexTileV * static_cast<float>(Yofs + 1);
				}
			}
		}

		void		VoxelControl::AddPlanesXY(vert32* pTarget,
			bool isDrawXPlus, bool isDrawYPlus,
			size_t id,
			const Vector3Int& VCenter, const Vector3Int& Vofs, int MaxminT, int MaxmaxT, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			int Min = 0;
			int Max = 0;
			bool PrevPut = false;
			int8_t PutBlockID = s_EmptyBlick;
			bool CanDrawXYZ[6] = { false, false, false, false, false, false };
			Vector3Int Vofset = Vofs;
			for (Vofset.z = MaxminT; Vofset.z <= MaxmaxT; ++Vofset.z) {
				const auto& CellBuff = cellx.GetCellBuf(VCenter + Vofset);
				bool CheckInside = false;
				if (!cellx.isReferenceCell()) {
					bool CheckInsideX = (DrawMinXMinus < Vofset.x) && (Vofset.x < DrawMinXPlus);
					bool CheckInsideY = (DrawMinYMinus < Vofset.y) && (Vofset.y < DrawMinYPlus);
					bool CheckInsideZ = (DrawMinZMinus < Vofset.z) && (Vofset.z < DrawMinZPlus);
					CheckInside = CheckInsideX && CheckInsideY && CheckInsideZ;
				}
				bool CheckBlockID = useTexture && (PutBlockID != CellBuff.GetID());

				bool IsPutPoint = !(
					(Vofset.z == MaxmaxT)//操作の端っこについた
					|| CheckInside//描画してはならない地点に入った
					|| (PrevPut && CheckBlockID)//置けるところだがテクスチャが変わった
					|| !CellBuff.CanDraw()//描画してはいけないブロックの地点に入った
					);
				if (IsPutPoint) {
					if (!PrevPut) {
						//ブロックが置ける部分なので観測開始
						Min = Vofset.z;
						PutBlockID = CellBuff.GetID();
						CanDrawXYZ[0] = false;
						CanDrawXYZ[1] = false;
						CanDrawXYZ[2] = false;
						CanDrawXYZ[3] = false;
					}
					//置ける地点中は全ブロックが隠れていないか確認
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
				else {
					if (PrevPut) {
						Max = Vofset.z - 1;
						//置けない部分なので今まで置けていた分をまとめてポリゴン化
						Vector3Int V1 = Vofset; V1.z = Min; V1 += VCenter;
						Vector3Int V2 = Vofset; V2.z = Max; V2 += VCenter;
						bool CheckFill = false;//埋まっている判定であろうと絶対埋めないといけないフラグ
						if (!cellx.isReferenceCell()) {
							bool CheckFillX = (DrawMinXMinus <= Vofset.x) && (Vofset.x <= DrawMinXPlus);
							bool CheckFillY = (DrawMinYMinus <= Vofset.y) && (Vofset.y <= DrawMinYPlus);
							bool CheckFillZ = Min <= DrawMinZPlus && DrawMinZMinus <= Max;
							CheckFill = CheckFillX && CheckFillY && CheckFillZ;
						}
						if (isDrawXPlus) {
							if (CheckFill || CanDrawXYZ[0]) {
								AddPlaneXPlus(pTarget, id, V1, V2, useTexture);
							}
						}
						else {
							if (CheckFill || CanDrawXYZ[1]) {
								AddPlaneXMinus(pTarget, id, V1, V2, useTexture);
							}
						}
						if (isDrawYPlus) {
							if (CheckFill || CanDrawXYZ[2]) {
								AddPlaneYPlus(pTarget, id, V1, V2, useTexture);
							}
						}
						else {
							if (CheckFill || CanDrawXYZ[3]) {
								AddPlaneYMinus(pTarget, id, V1, V2, useTexture);
							}
						}
						//テクスチャ変化の場合だけもう一回判定させるドン
						if (CheckBlockID) {
							--Vofset.z;
						}
					}
				}
				PrevPut = IsPutPoint;
			}
		}
		void		VoxelControl::AddPlanesZ(vert32* pTarget,
			bool isDrawZPlus,
			size_t id,
			const Vector3Int& VCenter, const Vector3Int& Vofs, int MaxminT, int MaxmaxT, bool useTexture) noexcept {
			CellsData& cellx = this->m_CellxN[id];
			int Min = 0;
			int Max = 0;
			bool PrevPut = false;
			int8_t PutBlockID = s_EmptyBlick;
			bool CanDrawXYZ[6] = { false, false, false, false, false, false };
			Vector3Int Vofset = Vofs;
			for (Vofset.x = MaxminT; Vofset.x <= MaxmaxT; ++Vofset.x) {
				const auto& CellBuff = cellx.GetCellBuf(VCenter + Vofset);
				bool CheckInside = false;
				if (!cellx.isReferenceCell()) {
					bool CheckInsideX = (DrawMinXMinus < Vofset.x) && (Vofset.x < DrawMinXPlus);
					bool CheckInsideY = (DrawMinYMinus < Vofset.y) && (Vofset.y < DrawMinYPlus);
					bool CheckInsideZ = (DrawMinZMinus < Vofset.z) && (Vofset.z < DrawMinZPlus);
					CheckInside = CheckInsideX && CheckInsideY && CheckInsideZ;
				}
				bool CheckBlockID = useTexture && (PutBlockID != CellBuff.GetID());

				bool IsPutPoint = !(
					(Vofset.x == MaxmaxT)//操作の端っこについたので置けない
					|| CheckInside//描画してはならない地点に入ったので置けない
					|| (PrevPut && CheckBlockID)//置けるところだがテクスチャが変わったので置けない
					|| !CellBuff.CanDraw()//描画してはいけないブロックの地点に入ったので置けない
					);
				if (IsPutPoint) {
					if (!PrevPut) {
						//ブロックが置ける部分なので観測開始
						Min = Vofset.x;
						PutBlockID = CellBuff.GetID();
						CanDrawXYZ[4] = false;
						CanDrawXYZ[5] = false;
					}
					//置ける地点中は全ブロックが隠れていないか確認
					if (!CanDrawXYZ[4] && !CellBuff.IsOcclusion(4)) {
						CanDrawXYZ[4] = true;
					}
					if (!CanDrawXYZ[5] && !CellBuff.IsOcclusion(5)) {
						CanDrawXYZ[5] = true;
					}
				}
				else {
					if (PrevPut) {
						//置けない部分なので今まで置けていた分をまとめてポリゴン化
						Max = Vofset.x - 1;
						bool CheckFill = false;//埋まっている判定であろうと絶対埋めないといけないフラグ
						if (!cellx.isReferenceCell()) {
							bool CheckFillX = Min <= DrawMinXPlus && DrawMinXMinus <= Max;
							bool CheckFillY = (DrawMinYMinus <= Vofset.y) && (Vofset.y <= DrawMinYPlus);
							bool CheckFillZ = (DrawMinZMinus <= Vofset.z) && (Vofset.z <= DrawMinZPlus);
							CheckFill = CheckFillX && CheckFillY && CheckFillZ;
						}
						Vector3Int V1 = Vofset; V1.x = Min; V1 += VCenter;
						Vector3Int V2 = Vofset; V2.x = Max; V2 += VCenter;
						if (isDrawZPlus) {
							if (CheckFill || CanDrawXYZ[4]) {
								AddPlaneZPlus(pTarget, id, V1, V2, useTexture);
							}
						}
						else {
							if (CheckFill || CanDrawXYZ[5]) {
								AddPlaneZMinus(pTarget, id, V1, V2, useTexture);
							}
						}
						//テクスチャ変化の場合だけもう一回判定させるドン
						if (CheckBlockID) {
							--Vofset.x;
						}
					}
				}
				PrevPut = IsPutPoint;
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

			float CamDotMinX = 0.f;
			float CamDotMaxX = 0.f;
			float CamDotMinZ = 0.f;
			float CamDotMaxZ = 0.f;
			if (UseFrustumCulling) {
				CamDotMinX = Draws.GetCamVec().x * (static_cast<float>(DrawMaxXMinus) + 0.5f);
				CamDotMaxX = Draws.GetCamVec().x * (static_cast<float>(DrawMaxXPlus) + 0.5f);
				CamDotMinZ = Draws.GetCamVec().z * (static_cast<float>(DrawMaxZMinus) + 0.5f);
				CamDotMaxZ = Draws.GetCamVec().z * (static_cast<float>(DrawMaxZPlus) + 0.5f);
			}
			Vector3Int Vofs{};
			for (Vofs.y = DrawMaxYMinus; Vofs.y <= DrawMaxYPlus; ++Vofs.y) {
				if (!cellx.isInside(VCenter.y + Vofs.y)) { continue; }
				float CamVecDotY = Draws.GetCamVec().y * (static_cast<float>(Vofs.y) + 0.5f);
				if (UseFrustumCulling) {
					//矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
					if (
						((CamDotMinX + CamVecDotY + CamDotMinZ) <= 0.0f) &&//Dot
						((CamDotMaxX + CamVecDotY + CamDotMaxZ) <= 0.0f) &&//Dot
						((CamDotMaxX + CamVecDotY + CamDotMinZ) <= 0.0f) &&//Dot
						((CamDotMinX + CamVecDotY + CamDotMaxZ) <= 0.0f)//Dot
						) {
						continue;
					}
				}
				//X
				for (Vofs.z = DrawMaxZMinus; Vofs.z <= DrawMaxZPlus; ++Vofs.z) {
					int xMaxminT = DrawMaxXMinus;
					int xMaxmaxT = DrawMaxXPlus;
					if (UseFrustumCulling) {
						float CamVecDotZ = Draws.GetCamVec().z * (static_cast<float>(Vofs.z) + 0.5f);
						if (!CullingLine(&xMaxminT, &xMaxmaxT, CamDotMinX + CamVecDotY + CamVecDotZ, CamDotMaxX + CamVecDotY + CamVecDotZ)) {
							continue;
						}
					}
					AddPlanesZ(&Draws.m_Vert32,
						UseFrustumCulling ? (Vofs.z < 0) : (Draws.GetCamVec().z < 0.0f),
						id,
						VCenter, Vofs, xMaxminT, xMaxmaxT, useTexture);
				}
				//Z
				for (Vofs.x = DrawMaxXMinus; Vofs.x <= DrawMaxXPlus; ++Vofs.x) {
					int zMaxminT = DrawMaxZMinus;
					int zMaxmaxT = DrawMaxZPlus;
					if (UseFrustumCulling) {
						float CamVecDotX = Draws.GetCamVec().x * (static_cast<float>(Vofs.x) + 0.5f);
						if (!CullingLine(&zMaxminT, &zMaxmaxT, CamVecDotX + CamVecDotY + CamDotMinZ, CamVecDotX + CamVecDotY + CamDotMaxZ)) {
							continue;
						}
					}
					AddPlanesXY(&Draws.m_Vert32,
						UseFrustumCulling ? (Vofs.x < 0) : (Draws.GetCamVec().x < 0.0f),
						UseFrustumCulling ? (Vofs.y < 0) : (Draws.GetCamVec().y < 0.0f),
						id, VCenter, Vofs, zMaxminT, zMaxmaxT, useTexture);
				}
			}
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
							[&](const Vector3Int& Voxel) {
								if (!GetReferenceCells().isInside(Voxel.y)) { return false; }
								if (!GetReferenceCells().GetCellBuf(Voxel).CanDraw()) { return false; }
								Vector3DX MinPos = GetReferenceCells().GetWorldPosOffset(Voxel, 0b000) + Vector3DX::one() * -0.1f * Scale3DRate;
								Vector3DX MaxPos = GetReferenceCells().GetWorldPosOffset(Voxel, 0b111) + Vector3DX::one() * 0.1f * Scale3DRate;
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
							[&](const Vector3Int& Voxel) {
								if (!GetReferenceCells().isInside(Voxel.y)) { return false; }
								const auto& CellBuff = GetReferenceCells().GetCellBuf(Voxel);
								if (!CellBuff.CanDraw()) { return false; }
								Vector3DX MinPos = GetReferenceCells().GetWorldPosOffset(Voxel, 0b000);
								Vector3DX MaxPos = GetReferenceCells().GetWorldPosOffset(Voxel, 0b111);

								MV1_COLL_RESULT_POLY tmp{};
								// Left
								if (!CellBuff.IsOcclusion(1)) {
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
								}
								// Right
								if (!CellBuff.IsOcclusion(0)) {
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
								}
								// Back
								if (!CellBuff.IsOcclusion(5)) {
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
								}
								// Forward
								if (!CellBuff.IsOcclusion(4)) {
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
								}
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
			std::vector<int8_t>				SaveCellIDList{};
			std::ifstream fin{};
			fin.open(Path, std::ios::in | std::ios::binary);
			SaveCellIDList.resize(static_cast<size_t>(256 * 256 * 256));
			fin.read((char*)SaveCellIDList.data(), static_cast<size_t>(sizeof(SaveCellIDList[0])) * 256 * 256 * 256);
			fin.close();
			for (int Xvoxel = 0; Xvoxel < GetReferenceCells().All; ++Xvoxel) {
				for (int Yvoxel = 0; Yvoxel < GetReferenceCells().All; ++Yvoxel) {
					for (int Zvoxel = 0; Zvoxel < GetReferenceCells().All; ++Zvoxel) {
						SetBlick(Xvoxel, Yvoxel, Zvoxel, SaveCellIDList[GetReferenceCells().GetCellIndex(Xvoxel, Yvoxel, Zvoxel)], false);
					}
				}
			}
		}
		void		VoxelControl::SaveCellsFile(std::string_view Path) const noexcept {
			std::vector<int8_t>				SaveCellIDList{};
			SaveCellIDList.resize(static_cast<size_t>(256 * 256 * 256));
			for (int Xvoxel = 0; Xvoxel < GetReferenceCells().All; ++Xvoxel) {
				for (int Yvoxel = 0; Yvoxel < GetReferenceCells().All; ++Yvoxel) {
					for (int Zvoxel = 0; Zvoxel < GetReferenceCells().All; ++Zvoxel) {
						SaveCellIDList[GetReferenceCells().GetCellIndex(Xvoxel, Yvoxel, Zvoxel)] = GetReferenceCells().GetCellBuf(Xvoxel, Yvoxel, Zvoxel).GetID();
					}
				}
			}

			std::ofstream fout{};
			fout.open(Path, std::ios::out | std::ios::binary | std::ios::trunc);
			fout.write((char*)SaveCellIDList.data(), static_cast<size_t>(sizeof(SaveCellIDList[0])) * 256 * 256 * 256);
			fout.close(); //ファイルを閉じる
		}
		void		VoxelControl::LoadCellsClip(int Xvoxel, int Yvoxel, int Zvoxel, std::string_view Path) noexcept {
			std::vector<int8_t>				SaveCellIDList{};
			int XTotal = 1;
			int YTotal = 1;
			int ZTotal = 1;
			std::ifstream fin{};
			fin.open(Path, std::ios::in | std::ios::binary);
			fin.read((char*)&XTotal, sizeof(XTotal));
			fin.read((char*)&YTotal, sizeof(YTotal));
			fin.read((char*)&ZTotal, sizeof(ZTotal));
			SaveCellIDList.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
			fin.read((char*)SaveCellIDList.data(), static_cast<size_t>(sizeof(SaveCellIDList[0])) * XTotal * YTotal * ZTotal);
			fin.close(); //ファイルを閉じる

			for (int Xofs = 0; Xofs < XTotal; ++Xofs) {
				for (int Yofs = 0; Yofs < YTotal; ++Yofs) {
					for (int Zofs = 0; Zofs < ZTotal; ++Zofs) {
						SetBlick(Xofs + Xvoxel, Yofs + Yvoxel, Zofs + Zvoxel, SaveCellIDList[static_cast<size_t>(Xofs * YTotal * ZTotal + Yofs * ZTotal + Zofs)], false);
					}
				}
			}
		}
		void		VoxelControl::SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax, std::string_view Path) const noexcept {
			std::vector<int8_t>				SaveCellIDList{};
			int XTotal = (XMax - XMin + 1);
			int YTotal = (YMax - YMin + 1);
			int ZTotal = (ZMax - ZMin + 1);

			SaveCellIDList.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
			for (int xm = 0; xm < XTotal; ++xm) {
				for (int ym = 0; ym < YTotal; ++ym) {
					for (int zm = 0; zm < ZTotal; ++zm) {
						SaveCellIDList[static_cast<size_t>(xm * YTotal * ZTotal + ym * ZTotal + zm)] = GetReferenceCells().GetCellBuf(XMin + xm, YMin + ym, ZMin + zm).GetID();
					}
				}
			}

			std::ofstream fout{};
			fout.open(Path, std::ios::out | std::ios::binary | std::ios::trunc);
			fout.write((char*)&XTotal, sizeof(XTotal));
			fout.write((char*)&YTotal, sizeof(YTotal));
			fout.write((char*)&ZTotal, sizeof(ZTotal));
			fout.write((char*)SaveCellIDList.data(), static_cast<size_t>(sizeof(SaveCellIDList[0]) * XTotal * YTotal * ZTotal));
			fout.close(); //ファイルを閉じる
		}
		//
		void		VoxelControl::SettingChange(void) noexcept {
			this->m_ShadowMaxDrawLOD = 0;//表示
			auto* OptionParts = OptionManager::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) == 0) {
				this->m_ShadowMaxDrawLOD = -1;//非表示
			}
			this->m_MaxDrawLOD = 1;//2段目まで表示
		}
		void		VoxelControl::SetBlick(int Xvoxel, int Yvoxel, int Zvoxel, int8_t ID, bool CalcOther) noexcept {
			this->m_CellxN[ReferenceCell].SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetID(ID);
			if (ID == s_EmptyBlick) {
				this->m_CellxN[ReferenceCell].SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetLife(0);
			}
			if (CalcOther) {
				//簡易版を更新
				for (int loop = 1; loop < TotalCellLayer; ++loop) {
					CellsData& cellx = this->m_CellxN[loop];

					int xm = Xvoxel / cellx.ScaleRate;
					int ym = Yvoxel / cellx.ScaleRate;
					int zm = Zvoxel / cellx.ScaleRate;

					cellx.SetCellBuf(xm, ym, zm).SetID(GetReferenceCells().isFill(xm, ym, zm, cellx.ScaleRate));
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
						if (GetReferenceCells().GetCellBuf(Xvoxel, Yvoxel, Zvoxel).GetID() != s_EmptyBlick) {
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
							cellx.SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetID(GetReferenceCells().isFill(Xvoxel, Yvoxel, Zvoxel, cellx.ScaleRate));
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
			this->m_DrawThreadDatas[0].m_Jobs.Init(
				[this]() { AddCubes(0, 0, true, true); },
				[this]() { this->m_DrawThreadDatas[0].EndRegist(this->m_DrawCenterPos, this->m_CamVec); }, false);
			this->m_DrawThreadDatas[1].m_Jobs.Init(
				[this]() { AddCubes(1, 1, true, true); },
				[this]() { this->m_DrawThreadDatas[1].EndRegist(this->m_DrawCenterPos, this->m_CamVec); }, false);
			this->m_DrawThreadDatas[2].m_Jobs.Init(
				[this]() { AddCubes(2, 2, true, true); },
				[this]() { this->m_DrawThreadDatas[2].EndRegist(this->m_DrawCenterPos, this->m_CamVec); }, false);
			this->m_DrawThreadDatas[3].m_Jobs.Init(
				[this]() { AddCubes(3, 3, true, true); },
				[this]() { this->m_DrawThreadDatas[3].EndRegist(this->m_DrawCenterPos, this->m_CamVec); }, false);
			this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 0)].m_Jobs.Init(
				[this]() { AddCubes(0, static_cast<size_t>(TotalCellLayer + 0), false, false); },
				[this]() {
					this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 0)].EndRegist(this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
					SceneControl::Instance()->SetIsUpdateFarShadowActive();//影のデプスバッファを更新するフラグ
				}, true);
			this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 1)].m_Jobs.Init(
				[this]() { AddCubes(1, static_cast<size_t>(TotalCellLayer + 1), false, false); },
				[this]() {
					this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 1)].EndRegist(this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
					SceneControl::Instance()->SetIsUpdateFarShadowActive();//影のデプスバッファを更新するフラグ
				}, true);
			this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 2)].m_Jobs.Init(
				[this]() { AddCubes(2, static_cast<size_t>(TotalCellLayer + 2), false, false); },
				[this]() {
					this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 2)].EndRegist(this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
					SceneControl::Instance()->SetIsUpdateFarShadowActive();//影のデプスバッファを更新するフラグ
				}, true);
			this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 3)].m_Jobs.Init(
				[this]() { AddCubes(3, static_cast<size_t>(TotalCellLayer + 3), false, false); },
				[this]() {
					this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 3)].EndRegist(this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
					SceneControl::Instance()->SetIsUpdateFarShadowActive();//影のデプスバッファを更新するフラグ
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
			this->m_ShadowDrawCenterPos = Vector3DX::vget(0.0f, -25.0f * Scale3DRate, 0.0f);
			this->m_ShadowCamVec = PostPassParts->GetShadowDir();
			if (this->m_isChangeBlock) {
				this->m_isChangeBlock = false;
				//ジョブを強制開始させる
				for (int loop = 0; loop < TotalCellLayer; ++loop) {
					this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + loop)].m_Jobs.JobStart();
				}
			}
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				if ((loop != 0) && (loop != this->m_ThreadCounter)) { continue; }
				CellsData& cellx = this->m_CellxN[loop];
				//ポリゴンアップデート
				this->m_DrawThreadDatas[loop].Update(this->m_MaxDrawLOD >= cellx.GetLODRate());
				//影ポリゴンアップデート
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + loop)].Update(this->m_ShadowMaxDrawLOD >= cellx.GetLODRate());
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
				this->m_DrawThreadDatas[threadID].Draw(this->m_tex);
			}
		}
		void		VoxelControl::SetShadow_Draw_Rigid(void) const noexcept {
			constexpr int MinLimit = std::min(std::min(DrawMinXPlus, DrawMinZPlus), DrawMinYPlus) * 3 / 4;
			constexpr int MaxLimit = std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus) * 5 / 4;
			float Near = GetCameraNear() / MaxLimit;
			float Far = GetCameraFar() / MinLimit;

			SetUseTextureToShader(0, this->m_tex.get());
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				const CellsData& cellx = this->m_CellxN[loop];
				if (cellx.isReferenceCell() && !(cellx.Scale < Far)) { continue; }
				if (!cellx.isReferenceCell() && !(Near < cellx.Scale && cellx.Scale < Far)) { continue; }
				this->m_DrawThreadDatas[loop].DrawByShader();
			}
			SetUseTextureToShader(0, InvalidID);
		}
		void		VoxelControl::Draw(void) const noexcept {
			constexpr int MinLimit = std::min(std::min(DrawMinXPlus, DrawMinZPlus), DrawMinYPlus) * 3 / 4;
			constexpr int MaxLimit = std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus) * 5 / 4;
			float Near = GetCameraNear() / MaxLimit;
			float Far = GetCameraFar() / MinLimit;

			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				const CellsData& cellx = this->m_CellxN[loop];
				if (cellx.isReferenceCell() && !(cellx.Scale < Far)) { continue; }
				if (!cellx.isReferenceCell() && !(Near < cellx.Scale && cellx.Scale < Far)) { continue; }
				this->m_DrawThreadDatas[loop].Draw(this->m_tex);
			}
		}
		void		VoxelControl::Dispose(void) noexcept {
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
