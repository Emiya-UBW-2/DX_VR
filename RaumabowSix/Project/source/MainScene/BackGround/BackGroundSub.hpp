#pragma once
#include	"../../Header.hpp"


namespace FPS_n2 {
	class Builds {
		int						m_mesh{ -1 };
		MV1						m_Col;
		MV1						m_LockPoint;
		MV1						m_WallLine;
		std::vector<std::pair<int, std::vector<int>>>	m_LockPointPos;
		MATRIX_ref				m_mat;
		VECTOR_ref				m_MinPos;
		VECTOR_ref				m_MaxPos;
	public:
		const auto&		GetMeshSel(void) const noexcept { return m_mesh; }
		const auto&		GetCol(void) const noexcept { return this->m_Col; }
		const auto&		GetMinPos(void) const noexcept { return m_MinPos; }
		const auto&		GetMaxPos(void) const noexcept { return m_MaxPos; }
		const auto&		GetMatrix(void) const noexcept { return m_mat; }
		const auto&		GetLockPoint(void) const noexcept { return this->m_LockPoint; }
		const auto&		GetLockPointPos(void) const noexcept { return this->m_LockPointPos; }
		const auto&		GetWallLine(void) const noexcept { return this->m_WallLine; }
		const auto		GetColLine(const VECTOR_ref& repos, const VECTOR_ref& pos) const noexcept { return this->m_Col.CollCheck_Line(repos, pos, m_mesh); }
	public:
		void		Set(const MV1& ColModel, const MV1& LockPointModel, const MV1& WallLineModel, int frame) {
			this->m_Col = ColModel.Duplicate();
			this->m_mesh = frame;
			this->m_Col.SetupCollInfo(1, 1, 1, m_mesh);

			this->m_LockPoint = LockPointModel.Duplicate();
			this->m_WallLine = WallLineModel.Duplicate();
			{
				for (auto& l : this->m_LockPointPos) { l.second.clear(); }
				this->m_LockPointPos.clear();
				int sel = 0;
				for (int i = 1; i < this->m_LockPoint.frame_num(); i++) {
					if (this->m_LockPoint.frame_parent(i) == -2) {
						if (sel == this->m_mesh) {
							for (int loop = 0; loop < this->m_LockPoint.frame_child_num(i); loop++) {
								int child = (int)this->m_LockPoint.frame_child(i, loop);
								m_LockPointPos.resize(m_LockPointPos.size() + 1);
								m_LockPointPos.back().first = child;
								for (int loop2 = 0; loop2 < this->m_LockPoint.frame_child_num(child); loop2++) {
									m_LockPointPos.back().second.emplace_back((int)this->m_LockPoint.frame_child(child, loop2));
								}
							}


							break;
						}
						sel++;
					}
				}
			}
		}
		void		ChangeSel(int frame) {
			if (this->m_Col.IsActive()) {
				MV1TerminateCollInfo(this->m_Col.get(), m_mesh);
				this->m_mesh = frame;
				this->m_Col.SetupCollInfo(1, 1, 1, m_mesh);

				{
					for (auto& l : this->m_LockPointPos) { l.second.clear(); }
					this->m_LockPointPos.clear();
					int sel = 0;
					for (int i = 1; i < this->m_LockPoint.frame_num(); i++) {
						if (this->m_LockPoint.frame_parent(i) == -2) {
							if (sel == this->m_mesh) {
								m_LockPointPos.resize(m_LockPointPos.size() + 1);
								m_LockPointPos.back().first = i;
								for (int loop = 0; loop < this->m_LockPoint.frame_child_num(i); loop++) {
									m_LockPointPos.back().second.emplace_back((int)this->m_LockPoint.frame_child(i, loop));
								}
								break;
							}
							sel++;
						}
					}
				}
			}
		}
		void		SetPosition(const MV1& colModel, VECTOR_ref pos, float rad, bool isTilt) {
			auto ofset = pos.y();
			pos.y(0.f);
			this->m_mat.clear();
			auto res = colModel.CollCheck_Line(pos + VECTOR_ref::vget(0.f, 100.f*Scale_Rate, 0.f), pos + VECTOR_ref::vget(0.f, -100.f*Scale_Rate, 0.f));
			if (res.HitFlag == TRUE) {
				pos = res.HitPosition;
				pos += VECTOR_ref::up()*(0.1f*Scale_Rate + ofset);
				if (isTilt) {
					this->m_mat = MATRIX_ref::RotVec2(VECTOR_ref::up(), res.Normal);
				}
			}
			this->m_mat = MATRIX_ref::RotY(rad)*m_mat*MATRIX_ref::Mtrans(pos);
			this->m_Col.SetMatrix(this->m_mat);
			this->m_Col.RefreshCollInfo(this->m_mesh);
			this->m_LockPoint.SetMatrix(this->m_mat);
			this->m_WallLine.SetMatrix(this->m_mat);
			this->m_MinPos = this->m_mat.pos() + this->m_Col.mesh_minpos(this->m_mesh) + VECTOR_ref::up()*(-1.f*Scale_Rate);
			this->m_MaxPos = this->m_mat.pos() + this->m_Col.mesh_maxpos(this->m_mesh) + VECTOR_ref::up()*(1.f*Scale_Rate);

			//this->m_MinPos = this->m_mat.pos() + VECTOR_ref::vget(-30.f*Scale_Rate, -10.f*Scale_Rate, -30.f*Scale_Rate);
			//this->m_MaxPos = this->m_mat.pos() + VECTOR_ref::vget(30.f*Scale_Rate, 30.f*Scale_Rate, 30.f*Scale_Rate);
		}
	};
	class BuildControl {
		MV1							m_ObjBuildBase;
		MV1							m_ColBuildBase;
		MV1							m_lockpointBuildBase;
		MV1							m_wallLine;

		std::vector<Builds>			m_ObjBuilds;

		std::vector<Model_Instance>	m_Inst;
	public:
		const auto&		GetBuildCol(void) const noexcept { return this->m_ObjBuilds; }
		const auto&		GetBuildCol(void) noexcept { return this->m_ObjBuilds; }
	public:
		void			Load(void) noexcept {
			MV1::Load("data/model/build/model.mv1", &this->m_ObjBuildBase);
			MV1::Load("data/model/build/col.mv1", &this->m_ColBuildBase);
			MV1::Load("data/model/build/lockpoint.mv1", &this->m_lockpointBuildBase);
			MV1::Load("data/model/build/wallLine.mv1", &this->m_wallLine);
		}
		void			Init(const MV1* MapCol) noexcept {
			int OneSize = 50;

			this->m_ObjBuilds.resize(OneSize);
			this->m_Inst.resize(8);
			{
				for (int loop = 0; loop < OneSize; loop++) {
					int ID = GetRand((int)this->m_Inst.size() - 1);
					if (4 <= ID && ID <= 6) {
						auto per = GetRand(100);
						if (per < 15) {
							ID = GetRand(4 - 1);
						}
						else if (per < 15 + 25) {
							ID = 7;// +GetRand(2);
						}
					}

					VECTOR_ref BasePos;
					while (true) {
						BasePos.Set(GetRandf(25.f*Scale_Rate), 0.f, GetRandf(25.f*Scale_Rate));
						if ((BasePos - VECTOR_ref::vget(25.f*Scale_Rate, 0.f, 25.f*Scale_Rate)).Length() <= 15.f*Scale_Rate) {
							continue;
						}
						if ((BasePos - VECTOR_ref::vget(-25.f*Scale_Rate, 0.f, -25.f*Scale_Rate)).Length() <= 15.f*Scale_Rate) {
							continue;
						}

						bool isHit = false;
						for (int loop2 = 0; loop2 < loop; loop2++) {
							auto Len = this->m_ObjBuilds[loop2].GetMatrix().pos() - BasePos;
							Len.y(0);
							if (Len.Length() <= 2.f*Scale_Rate) {
								isHit = true;
								break;
							}
						}
						if (!isHit) { break; }
					}
					{
						bool xzturn = false;
						//
						if (0 <= ID && ID <= 3) {
							BasePos.y(-0.2f*Scale_Rate);
						}
						if (4 <= ID && ID <= 6) {
							BasePos.y(-0.4f*Scale_Rate);
						}
						if (7 <= ID && ID <= 7) {
							BasePos.y(-0.05f*Scale_Rate);
							xzturn = true;
						}

						this->m_ObjBuilds[loop].Set(this->m_ColBuildBase, this->m_lockpointBuildBase, this->m_wallLine, ID);
						this->m_ObjBuilds[loop].SetPosition(*MapCol, BasePos, deg2rad(GetRandf(180.f)), xzturn);
					}
				}
			}
			for (int i = 0; i < (int)this->m_Inst.size(); i++) {
				int total = 0;
				for (auto& b : m_ObjBuilds) {
					if (b.GetMeshSel() == i) {
						total++;
					}
				}
				this->m_Inst[i].Init(this->m_ObjBuildBase, i);
				this->m_Inst[i].Reset();
				this->m_Inst[i].Set_start(total);
				for (auto& b : m_ObjBuilds) {
					if (b.GetMeshSel() == i) {
						this->m_Inst[i].Set_one(b.GetMatrix());
					}
				}
				this->m_Inst[i].Execute();
			}

			MATERIALPARAM							Material_t;
			Material_t.Diffuse = GetLightDifColor();
			Material_t.Specular = GetLightSpcColor();
			Material_t.Ambient = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);;
			Material_t.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
			Material_t.Power = 20.0f;
			SetMaterialParam(Material_t);
		}
		void			Draw() noexcept {
			for (auto& b : this->m_Inst) {
				b.Draw();
			}
		}
		void			Dispose(void) noexcept {
			this->m_ObjBuildBase.Dispose();
			this->m_ColBuildBase.Dispose();
			for (auto& b : this->m_Inst) {
				b.Dispose();
			}
			this->m_Inst.clear();
		}

	};

	//Box2D•Ç
	class WallLines {
	private:
		std::vector<std::array<VECTOR_ref, 2>>	m_wallParts;	//•Ç‚ðƒZƒbƒg
	public://getter
		//auto&			GetBox2Dworld(void) noexcept { return this->m_b2world; }
	public:
		void			Add(const MV1_REF_POLYGONLIST &p) {
			for (int i = 0; i < p.PolygonNum; i++) {
				{
					this->m_wallParts.resize(this->m_wallParts.size() + 1);
					this->m_wallParts.back()[0] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
					this->m_wallParts.back()[1] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
					auto Buf = this->m_wallParts.back()[0] - this->m_wallParts.back()[1]; Buf.y(0.f);
					if (Buf.Length() <= 0.005f) {
						this->m_wallParts.pop_back();
					}
				}
				{
					this->m_wallParts.resize(this->m_wallParts.size() + 1);
					this->m_wallParts.back()[0] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
					this->m_wallParts.back()[1] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
					auto Buf = this->m_wallParts.back()[0] - this->m_wallParts.back()[1]; Buf.y(0.f);
					if (Buf.Length() <= 0.005f) {
						this->m_wallParts.pop_back();
					}
				}
				{
					this->m_wallParts.resize(this->m_wallParts.size() + 1);
					this->m_wallParts.back()[0] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
					this->m_wallParts.back()[1] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
					auto Buf = this->m_wallParts.back()[0] - this->m_wallParts.back()[1]; Buf.y(0.f);
					if (Buf.Length() <= 0.005f) {
						this->m_wallParts.pop_back();
					}
				}
			}
		}
		void			Init(void) noexcept {
			this->m_wallParts.resize(this->m_wallParts.size() + 1);
			this->m_wallParts.back()[0].Set(25.f*Scale_Rate, 0.f, 25.f*Scale_Rate);
			this->m_wallParts.back()[1].Set(-25.f*Scale_Rate, 0.f, 25.f*Scale_Rate);

			this->m_wallParts.resize(this->m_wallParts.size() + 1);
			this->m_wallParts.back()[0].Set(-25.f*Scale_Rate, 0.f, 25.f*Scale_Rate);
			this->m_wallParts.back()[1].Set(-25.f*Scale_Rate, 0.f, -25.f*Scale_Rate);

			this->m_wallParts.resize(this->m_wallParts.size() + 1);
			this->m_wallParts.back()[0].Set(-25.f*Scale_Rate, 0.f, -25.f*Scale_Rate);
			this->m_wallParts.back()[1].Set(25.f*Scale_Rate, 0.f, -25.f*Scale_Rate);

			this->m_wallParts.resize(this->m_wallParts.size() + 1);
			this->m_wallParts.back()[0].Set(25.f*Scale_Rate, 0.f, -25.f*Scale_Rate);
			this->m_wallParts.back()[1].Set(25.f*Scale_Rate, 0.f, 25.f*Scale_Rate);
		}
		void			Draw(int x, int y, int size, float scale, const VECTOR_ref& Pos,float Yrad) noexcept {
			//auto* DrawParts = DXDraw::Instance();

			float Len = y_r(20) / Scale_Rate * scale;

			int Mx1 = x;
			int My1 = y + size / 2;

			int Bx1 = x - size;
			int By1 = y - size;
			int Bx2 = x + size;
			int By2 = y + size;

			DrawBox(Bx1, By1, Bx2, By2, GetColor(64, 64, 64), TRUE);
			for (auto& w : this->m_wallParts) {
				auto W1 = MATRIX_ref::Vtrans(w[0] - Pos, MATRIX_ref::RotY(-Yrad));
				auto W2 = MATRIX_ref::Vtrans(w[1] - Pos, MATRIX_ref::RotY(-Yrad));
				int x1, y1, x2, y2;
				x1 = Mx1 - (int)(W1.x()*Len);
				y1 = My1 + (int)(W1.z()*Len);
				x2 = Mx1 - (int)(W2.x()*Len);
				y2 = My1 + (int)(W2.z()*Len);
				if (std::min(x1, x2) <= Bx2 &&
					Bx1 <= std::max(x1, x2) &&
					std::min(y1, y2) <= By2 &&
					By1 <= std::max(y1, y2)) {
					DrawLine_2D(x1, y1, x2, y2, GetColor(255, 0, 0), std::max(1, (int)((float)y_r(6)*scale)));
				}
			}
			DxLib::Paint(Mx1, My1, GetColor(92, 92, 92));

			DrawCircle(Mx1, My1, std::max(1, (int)((float)y_r(10)*scale)), GetColor(0, 255, 0), TRUE);
			DrawBoxLine_2D(Bx1, By1, Bx2, By2, GetColor(192, 192, 192), 5);
		}
		void			Dispose(void) noexcept {
			this->m_wallParts.clear();
		}
	};
};
