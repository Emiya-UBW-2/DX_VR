#pragma once
#include "BackGroundSub.hpp"

namespace FPS_n2 {
	/*�j���*/
	void	BreakWall::WallObj::SetTri(int ID, bool isback, float offset) noexcept {
		//0����ɕʃI�u�W�F�����
		auto& vrt = this->m_WallVertex[ID];
		auto& ind = this->m_WallIndex[ID];
		vrt.resize(this->m_WallVertex[0].size());
		ind.resize(this->m_WallIndex[0].size());
		for (int i = 0; i < vrt.size(); i++) {
			vrt[i] = this->m_WallVertex[0][i];
			vrt[i].pos = ((Vector3DX)(vrt[i].pos) + (Vector3DX)(vrt[i].norm)*offset).get();
			if (isback) {
				vrt[i].norm = ((Vector3DX)(vrt[i].norm)*-1.f).get();
			}
		}
		for (size_t i = 0; i < ind.size() / (size_t)Triangle_Num; i++) {
			size_t index = i * (size_t)Triangle_Num;
			ind.at(index) = this->m_WallIndex[0].at(index);
			if (isback) {
				ind.at(index + 1) = this->m_WallIndex[0].at(index + 2);
				ind.at(index + 2) = this->m_WallIndex[0].at(index + 1);
			}
			else {
				ind.at(index + 1) = this->m_WallIndex[0].at(index + 1);
				ind.at(index + 2) = this->m_WallIndex[0].at(index + 2);
			}
		}
	}
	void	BreakWall::WallObj::SetFirst(const MV1_REF_POLYGONLIST& PolyList, const Vector3DX& pos, float YRad, float YScale) noexcept {
		this->m_Pos = pos;
		this->m_YScale = YScale;
		auto matrix = Matrix4x4DX::RotAxis(Vector3DX::up(), YRad)*Matrix4x4DX::Mtrans(pos);
		this->m_WallVertex[0].resize(PolyList.VertexNum);
		Vector3DX BasePos = PolyList.Vertexs[0].Position;
		if (BasePos.y > 0.f) { BasePos.y = (BasePos.y*m_YScale); }
		for (int i = 0; i < this->m_WallVertex[0].size(); i++) {
			Vector3DX Pos = PolyList.Vertexs[i].Position;
			if (Pos.y > 0.f) { Pos.y = (Pos.y*m_YScale); }
			this->m_WallVertex[0][i].pos = Matrix4x4DX::Vtrans(Pos, matrix).get();
			this->m_WallVertex[0][i].norm = Matrix4x4DX::Vtrans(PolyList.Vertexs[i].Normal, matrix.rotation()).normalized().get();
			this->m_WallVertex[0][i].dif = PolyList.Vertexs[i].DiffuseColor;
			this->m_WallVertex[0][i].spc = PolyList.Vertexs[i].SpecularColor;

			auto uv = Pos - BasePos;

			this->m_WallVertex[0][i].u = uv.x / Scale_Rate;
			this->m_WallVertex[0][i].v = -uv.y / Scale_Rate / m_YScale;
			this->m_WallVertex[0][i].su = uv.x / Scale_Rate;
			this->m_WallVertex[0][i].sv = -uv.y / Scale_Rate / m_YScale;
		}
		this->m_WallIndex[0].resize((size_t)PolyList.PolygonNum * (size_t)Triangle_Num);
		for (size_t i = 0; i < this->m_WallIndex[0].size() / (size_t)Triangle_Num; i++) {
			size_t index = i * (size_t)Triangle_Num;
			this->m_WallIndex[0][index + 0] = (WORD)PolyList.Polygons[i].VIndex[0];
			this->m_WallIndex[0][index + 1] = (WORD)PolyList.Polygons[i].VIndex[1];
			this->m_WallIndex[0][index + 2] = (WORD)PolyList.Polygons[i].VIndex[2];
		}
		//���]
		SetTri(1, true, 0.f);
		//���]
		SetTri(2, false, -0.7f);
		//���]
		SetTri(3, true, -0.7f);

		for (size_t i = 0; i < 2; i++) {
			size_t index = i * (size_t)Triangle_Num;
			this->m_WallBase[i][0] = this->m_WallVertex[0][this->m_WallIndex[0][index + 0]].pos;
			this->m_WallBase[i][1] = this->m_WallVertex[0][this->m_WallIndex[0][index + 1]].pos;
			this->m_WallBase[i][2] = this->m_WallVertex[0][this->m_WallIndex[0][index + 2]].pos;
		}
		this->m_Wallnorm = this->m_WallVertex[0][0].norm;
	}
	void	BreakWall::WallObj::SetNext() noexcept {
		this->m_WallVertex[0].clear();
		this->m_WallIndex[0].clear();
		for (auto& Side : this->m_Side) {
			size_t index = &Side - &this->m_Side.front();
			//�o�������̂����X�g�ɍēo�^
			for (auto& s : this->m_Tri2D[index]) {
				for (auto& p : s.Getpoints()) {
					Vector3DX p2; p2.Set(p.x, 0.f, p.y);
					this->m_WallVertex[0].resize(this->m_WallVertex[0].size() + 1);
					this->m_WallVertex[0].back().pos = (Matrix4x4DX::Vtrans(p2, Side[0]->m_Mat.inverse()) + this->m_BasePos).get();
					this->m_WallVertex[0].back().norm = Side[0]->m_Norm.get();
					this->m_WallVertex[0].back().dif = Side[0]->m_dif;
					this->m_WallVertex[0].back().spc = Side[0]->m_spc;
					this->m_WallVertex[0].back().u = p.x / Scale_Rate;
					this->m_WallVertex[0].back().v = -p.y / Scale_Rate / m_YScale;
					this->m_WallVertex[0].back().su = p.x / Scale_Rate;
					this->m_WallVertex[0].back().sv = -p.y / Scale_Rate / m_YScale;

					this->m_WallIndex[0].emplace_back((WORD)(this->m_WallVertex[0].size() - 1));
				}
			}
		}
		//���]
		SetTri(1, true, 0.f);
		//���]
		SetTri(2, false, -0.7f);
		//���]
		SetTri(3, true, -0.7f);
	}
	void	BreakWall::WallObj::SetSide(const Vector3DX* basepos) noexcept {
		if (basepos) { this->m_BasePos = *basepos; }
		for (auto& s : this->m_Side) {
			for (auto&s2 : s) { s2.reset(); }
			s.clear();
		}
		this->m_Side.clear();
		this->m_Side.resize(this->m_WallIndex[0].size() / Triangle_Num);
		this->m_Tri2D.resize(this->m_Side.size());
		for (auto& Side : this->m_Side) {
			size_t index = &Side - &this->m_Side.front();
			auto GetVertex = [&](int ID) {return &(this->m_WallVertex[0][this->m_WallIndex[0][index * (size_t)Triangle_Num + ID]]); };
			auto GetVertexPos = [&](int ID) {return &(GetVertex(ID)->pos); };
			Vector3DX TriPos0 = *GetVertexPos(0);
			Vector3DX TriPos1 = *GetVertexPos(1);
			Vector3DX TriPos2 = *GetVertexPos(2);
			Vector3DX TriNorm = (Vector3DX::Cross((TriPos1 - TriPos0), TriPos2 - TriPos0)).normalized();

			Side.resize(Side.size() + Triangle_Num);
			Side[0] = std::make_unique<SideControl>();
			Side[0]->Set(SideType::Triangle, *GetVertexPos(0), this->m_BasePos, TriNorm, GetVertex(0)->dif, GetVertex(0)->spc);

			Side[1] = std::make_unique<SideControl>();
			Side[1]->Set(SideType::Triangle, *GetVertexPos(1), this->m_BasePos, TriNorm, GetVertex(1)->dif, GetVertex(1)->spc);

			Side[2] = std::make_unique<SideControl>();
			Side[2]->Set(SideType::Triangle, *GetVertexPos(2), this->m_BasePos, TriNorm, GetVertex(2)->dif, GetVertex(2)->spc);
		}
	}
	bool	BreakWall::WallObj::GetSamePoint(const Vector3DX& LineA, const Vector3DX& LineB, const Vector3DX& pointA, const Vector3DX& pointB) noexcept {
		//�_AB�����̓������ɂ��邩�ǂ���
		auto vec1 = (LineB - LineA).normalized();
		return Vector3DX::Dot(Vector3DX::Cross(vec1, (pointA - LineA).normalized()), Vector3DX::Cross(vec1, (pointB - LineA).normalized())) > 0.f;
	};
	void	BreakWall::WallObj::CalcDelaunay(std::vector<Triangle>* Ans, const std::vector<std::unique_ptr<SideControl>>& points, const Triangle& ExternalTriangle, const std::vector<Vector3DX>& GonPoint2D) noexcept {
		//�h���l�[�O�p�`�������v�Z.��ԊO���̋���O�p�`�𐶐��A�����ł͉�ʓ��̓_����Ƃ��ĉ�ʃT�C�Y���܂ގO�p�`�����
		/*
		points �v�Z�Ώۂ̓_�Q
		ExternalTriangle �_������O�p�`
		 */
		std::vector<Triangle*> triangles;		// ���������O�p�`��ێ�����z��
		triangles.emplace_back(new Triangle);
		*triangles.back() = ExternalTriangle;
		Triangle* super_triangle = triangles.back();
		for (auto& p : points) {
			// �O�ډ~�ɓ_���܂܂��O�p�`��������
			std::vector<Triangle*> hit_triangles;
			for (auto& tris : triangles) {
				if (tris->getCircumscribedCircle(p->m_Pos2D)) {
					hit_triangles.emplace_back(tris);
				}
			}
			std::vector<Triangle::LineControl> line_stack;
			for (auto& tris : hit_triangles) {
				// ���������O�p�`�̕ӂ��X�^�b�N�ɐς�
				for (int i = 0; i < Triangle_Num; i++) {
					auto startp = tris->Getpoints()[i];
					auto endp = tris->Getpoints()[(i + 1) % Triangle_Num];
					line_stack.emplace_back(std::make_pair(startp, endp));
				}
				// ���������O�p�`��z�񂩂�폜
				triangles.erase(std::find(triangles.begin(), triangles.end(), tris));
				// ���������O�p�`���Y���̓_�ŕ������A�V����3�̎O�p�`�ɂ���
				Triangle new_tri1; new_tri1.Set(line_stack[0].first, line_stack[0].second, p->m_Pos2D);
				Triangle new_tri2; new_tri2.Set(line_stack[1].first, line_stack[1].second, p->m_Pos2D);
				Triangle new_tri3; new_tri3.Set(line_stack[2].first, line_stack[2].second, p->m_Pos2D);
				if (!new_tri1.IsOverlap()) {
					triangles.emplace_back(new Triangle);
					*triangles.back() = new_tri1;
				}
				if (!new_tri2.IsOverlap()) {
					triangles.emplace_back(new Triangle);
					*triangles.back() = new_tri2;
				}
				if (!new_tri3.IsOverlap()) {
					triangles.emplace_back(new Triangle);
					*triangles.back() = new_tri3;
				}
			}
			hit_triangles.clear();
			// �X�^�b�N����ɂȂ�܂ŌJ��Ԃ�
			for (int i = 0; i < line_stack.size(); i++) {
				auto& line = line_stack[i];
				// �ӂ����L����O�p�`��������
				std::vector<Triangle*> common_line_triangles;
				for (auto& tris : triangles) {
					if (tris->Hasline(line)) {
						common_line_triangles.emplace_back(tris);
					}
				}
				if (common_line_triangles.size() <= 1) { continue; }				// ���L����ӂ����O�p�`���ӂ�������Ȃ���΃X�L�b�v

				// ���L�Ӂi������AB�Ƃ���j���܂�2�̎O�p�`��ABC, ABD�Ƃ���
				// �����A�O�p�`ABC�̊O�ډ~�ɓ_D������ꍇ�́A���L�ӂ�flip���A��AD/DB/BC/CA���X�^�b�N��emplace_back����
				// �܂�A���������O�p�`�����X�g����폜���A�V�����Ӄ��X�g���X�^�b�N�ɐς�
				// ����ɁA�V�����ł����O�p�`�����X�g�ɉ�����
				auto* triangle_ABC = common_line_triangles[0];
				auto* triangle_ABD = common_line_triangles[1];

				// �I�΂ꂽ�O�p�`������̂��̂̏ꍇ�͂�����폜���Ď���
				if (triangle_ABC == triangle_ABD) {
					triangles.erase(std::find(triangles.begin(), triangles.end(), triangle_ABC));
					triangles.erase(std::find(triangles.begin(), triangles.end(), triangle_ABD));
					continue;
				}

				auto Cptr = triangle_ABC->noCommonPointByline(line);			// �O�p�`ABC�̒��_�̂����A���L�ӈȊO�̓_���擾�i�܂�_C�j
				auto Dptr = triangle_ABD->noCommonPointByline(line);			// �O�p�`ABD�̒��_�̂����A���L�ӈȊO�̓_���擾�i�܂�_D�j
				Vector3DX point_C; if (Cptr) { point_C = *Cptr; }
				Vector3DX point_D; if (Dptr) { point_D = *Dptr; }

				// ���_D���O�p�`ABC�̊O�ډ~�Ɋ܂܂�Ă邩����
				if (triangle_ABC->getCircumscribedCircle(point_D)) {
					// �O�p�`���X�g����O�p�`���폜
					triangles.erase(std::find(triangles.begin(), triangles.end(), triangle_ABC));
					triangles.erase(std::find(triangles.begin(), triangles.end(), triangle_ABD));

					// ���L�ӂ�flip���Ăł���O�p�`��V�����O�p�`�S�ɒǉ�
					Triangle tri_ACD; tri_ACD.Set(line.first, point_C, point_D);
					Triangle tri_BCD; tri_BCD.Set(line.second, point_C, point_D);
					if (!tri_ACD.IsOverlap()) {
						triangles.emplace_back(new Triangle);
						*triangles.back() = tri_ACD;
					}
					if (!tri_BCD.IsOverlap()) {
						triangles.emplace_back(new Triangle);
						*triangles.back() = tri_BCD;
					}

					// ��L�O�p�`�̕ӂ�line stack�ɒǉ�
					triangle_ABC->otherlineByline(line, &line_stack);
					triangle_ABD->otherlineByline(line, &line_stack);
				}
			}
			line_stack.clear();
		}

		// �Ō�ɁA����O�p�`�ƒ��_�����L���Ă���O�p�`�����X�g����폜
		for (auto& point : super_triangle->Getpoints()) {
			for (auto& tris : triangles) {
				if (tris && tris->HasPoint(point)) {
					delete tris; tris = nullptr;
				}
			}
		}
		//
		for (auto& tris : triangles) {
			//���r��
			if (tris) {
				for (auto& tris2 : triangles) {
					if (tris2 == tris) { continue; }
					if (tris2) {
						bool ok = false;
						int OK0[3]{ -1,-1,-1 };
						for (int Lp = 0; Lp < 3; Lp++) {
							for (int L0 = 0; L0 < 3; L0++) {
								if (OK0[0] == L0) { continue; }
								if (OK0[1] == L0) { continue; }
								if (tris->Getpoints()[Lp] == tris2->Getpoints()[L0]) {
									OK0[Lp] = L0;
									break;
								}
							}
							if (OK0[Lp] == -1) {
								ok = true;
								break;//����Ă��Ȃ�
							}
						}
						//��肪����̂�tris���폜
						if (!ok) {
							delete tris; tris = nullptr;
							break;
						}
					}
				}
			}
			//�ׂ�������̂��폜
			if (tris) {
				bool ok = true;
				Vector3DX Pos0 = tris->Getpoints()[0];
				Vector3DX Pos1 = tris->Getpoints()[1];
				Vector3DX Pos2 = tris->Getpoints()[2];
				while (true) {
					for (int loop2 = 0; loop2 < 3; loop2++) {
						if (GetMinLenSegmentToPoint(
							tris->Getpoints()[loop2],
							tris->Getpoints()[(loop2 + 1) % 3],
							tris->Getpoints()[(loop2 + 2) % 3]) <= 0.01f) {
							delete tris; tris = nullptr;
							ok = false;
							break;
						}
					}
					if (!ok) { break; }
					break;
				}
			}
			//�ӂ��l�p�̓_����̓������x�N�g����������r��
			if (tris) {
				bool ok = true;
				auto N_gon = GonPoint2D.size();
				if (N_gon >= 3) {
					for (int loop2 = 0; loop2 < 3; loop2++) {
						Vector3DX Pos1 = tris->Getpoints()[(loop2 + 0) % 3];
						Vector3DX Pos2 = tris->Getpoints()[(loop2 + 1) % 3];
						for (int loop3 = 0; loop3 < N_gon; loop3++) {
							Vector3DX PosZ = GonPoint2D.at(((size_t)loop3 + 0) % N_gon);
							Vector3DX PosA = GonPoint2D.at(((size_t)loop3 + 1) % N_gon);//�
							Vector3DX PosB = GonPoint2D.at(((size_t)loop3 + 2) % N_gon);
							if (Pos1 == PosA) {
								if ((Pos2 != PosB) && (Pos2 != PosZ)) {
									if (
										(Vector3DX::Cross((PosB - PosA).normalized(), (Pos2 - PosA).normalized()).z <= 0.f) &&
										(Vector3DX::Cross((PosZ - PosA).normalized(), (Pos2 - PosA).normalized()).z >= 0.f)
										) {
										ok = false;
										break;
									}
								}
							}
							if (!ok) { break; }
							if (Pos2 == PosA) {
								if ((Pos1 != PosB) && (Pos1 != PosZ)) {
									if (
										(Vector3DX::Cross((PosB - PosA).normalized(), (Pos1 - PosA).normalized()).z <= 0.f) &&
										(Vector3DX::Cross((PosZ - PosA).normalized(), (Pos1 - PosA).normalized()).z > 0.f)
										) {
										ok = false;
										break;
									}
								}
							}
							if (!ok) { break; }
						}
						if (!ok) { break; }
					}
					if (!ok) {
						delete tris; tris = nullptr;
					}
				}
			}
		}
		//
		Ans->clear();
		for (auto& tris : triangles) {
			if (tris) {
				Vector3DX Pos0 = tris->Getpoints()[0];
				Vector3DX Pos1 = tris->Getpoints()[1];
				Vector3DX Pos2 = tris->Getpoints()[2];
				Vector3DX cross = Vector3DX::Cross((Pos1 - Pos0), Pos2 - Pos0);
				if (cross.z >= 0.f) {
					tris->Set(Pos0, Pos2, Pos1);
				}
				Ans->emplace_back(*tris);
				delete tris; tris = nullptr;
			}
		}
		triangles.clear();
	}
	//
	void	BreakWall::WallObj::Init(const MV1& obj, const MV1& col, const Vector3DX& pos, float YRad, float YScale) noexcept {
		this->m_TexHandle1 = GraphHandle::Make(512, (int)(512.f* YScale));
		{
			auto Handle1 = GraphHandle::Load("data/model/wall/tex.dds");//tex.dds//tex.bmp

			this->m_TexHandle1.SetDraw_Screen(false);
			{
				Handle1.DrawExtendGraph(0, 0, 512, (int)(512.f* YScale), false);
			}
			Handle1.Dispose();
		}


		this->m_TexHandle2 = GraphHandle::Load("data/model/wall/plathome.dds");

		SetFirst(MV1GetReferenceMesh(obj.get(), 0, FALSE), pos, YRad, YScale);
		MV1TerminateReferenceMesh(obj.get(), 0, FALSE);

		Vector3DX basepos = this->m_WallVertex[0][this->m_WallIndex[0][0]].pos;
		SetSide(&basepos);

		MATERIALPARAM							Material_t;
		Material_t.Diffuse = GetLightDifColor();
		Material_t.Specular = GetLightSpcColor();
		Material_t.Ambient = GetLightAmbColor();
		Material_t.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material_t.Power = 20.0f;
		SetMaterialParam(Material_t);
		this->m_IsDraw = true;

		this->m_PlayerCol = col.Duplicate();
		this->m_PlayerCol.SetupCollInfo(2, 2, 2);
		this->m_PlayerCol.SetMatrix(Matrix4x4DX::RotAxis(Vector3DX::up(), YRad)*Matrix4x4DX::Mtrans(pos));
		this->m_PlayerCol.RefreshCollInfo();
	}
	void	BreakWall::WallObj::Execute(void) noexcept {
		if (this->m_WallCalc.get() != nullptr) {
			if (this->m_isThreadEnd) {
				//m_WallCalc->join();
				this->m_WallCalc->detach();
				this->m_WallCalc.reset();
				this->m_isThreadEnd = false;
			}
		}
		if ((this->m_Bigcount > 3) || (this->m_WallIndex[0].size() > 5000)) {
			this->m_BreakTimer = std::max(this->m_BreakTimer - 1.f / FPS / 2.f, 0.f);
		}
		else {
			this->m_BreakTimer = 1.f;
		}
		//printfDx("�X���b�h��:%d\n", std::thread::hardware_concurrency());

		//addtri += GetMouseWheelRotVolWithCheck();
	}
	bool	BreakWall::WallObj::Draw() noexcept {

		//m_PlayerCol.DrawModel();
		//return true;

		if (
			(
				CheckCameraViewClip_Box(
				(this->m_Pos + Vector3DX::vget(-6.25f, 0, -6.25f)).get(),
					(this->m_Pos + Vector3DX::vget(6.25f, 40, 6.25f)).get()) == FALSE
				)
			&& this->m_IsDraw
			) {

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*this->m_BreakTimer), 0, 255));

			auto vec_tmp = this->m_Pos - GetCameraPosition();
			int sel = (Vector3DX::Dot(this->m_Wallnorm, vec_tmp.normalized()*-1.f) > 0.f) ? 0 : 1;
			for (int i = 0; i < 4; i++) {
				if ((i % 2) == sel) {
					if (1 <= i && i <= 2) {
						if ((this->m_BreakTimer == 1.f) && (vec_tmp.magnitude() < 5.f*Scale_Rate)) {
							DrawPolygonIndexed3D(this->m_WallVertex[i].data(), (int)this->m_WallVertex[i].size(), this->m_WallIndex[i].data(), (int)(this->m_WallIndex[i].size()) / Triangle_Num, this->m_TexHandle2.get(), TRUE);
						}
					}
					else {
						DrawPolygonIndexed3D(this->m_WallVertex[i].data(), (int)this->m_WallVertex[i].size(), this->m_WallIndex[i].data(), (int)(this->m_WallIndex[i].size()) / Triangle_Num, this->m_TexHandle1.get(), TRUE);
					}
				}
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			return true;
		}
		return false;
#if false
		for (auto& s : this->m_Side) {
			for (int i = 0; i < s.size(); i++) {
				DrawLine3D(s[i]->Pos.get(), s[(i + 1) % s.size()]->Pos.get(), GetColor(255, 0, 0));
			}
			for (auto& s2 : s) {
				DrawSphere_3D(s2->Pos, Scale_Rate*0.05f, GetColor(255, 0, 0), GetColor(255, 255, 255));
			}
		}

		int x = 400;
		int y = 400;
		int xsize = 0;
		float scale = 20.f;
		x = 400;
		for (auto& s : this->m_Tri2D) {
			for (int loop = 0; loop < (int)s.size(); loop++) {
				int xadd = loop * addtri;
				int yadd = 0;
				{
					int x1 = x + xadd + (int)(s[loop].Getpoints()[0].x*scale);
					int y1 = y + yadd - (int)(s[loop].Getpoints()[0].y*scale);
					int x2 = x + xadd + (int)(s[loop].Getpoints()[1].x*scale);
					int y2 = y + yadd - (int)(s[loop].Getpoints()[1].y*scale);
					int x3 = x + xadd + (int)(s[loop].Getpoints()[2].x*scale);
					int y3 = y + yadd - (int)(s[loop].Getpoints()[2].y*scale);
					DrawTriangle(x1, y1, x2, y2, x3, y3, GetColor(0, 255 * (loop + 1) / (int)s.size(), 0), TRUE);
				}
			}
			x += xsize;
		}
#endif
	}
	bool	BreakWall::WallObj::CheckHit(const Vector3DX& repos, Vector3DX* pos, Vector3DX* norm, float radius) noexcept {
		Vector3DX pos_t = *pos;
		bool ishit = false;
		if (this->m_BreakTimer != 1.f) {
			return false;
		}
		bool isback = (Vector3DX::Dot((pos_t - repos), this->m_Wallnorm) >= 0.f);
		Vector3DX vec = (isback) ? ((pos_t - repos)*-1.f) : (pos_t - repos);
		Vector3DX RePos = isback ? pos_t : repos;
		{
			float Radius = radius * Scale_Rate*1.8f*5.f;
			int N_gon = 5 + std::clamp((int)(radius / 0.015f), 0, 11);
			Vector3DX xaxis = Vector3DX::Cross(vec.normalized(), Vector3DX::up());
			Vector3DX yaxis = Vector3DX::Cross(vec.normalized(), xaxis);
			this->m_GonPoint[0].resize(N_gon);
			this->m_GonPoint[1].resize(N_gon);
			for (int gon = 0; gon < N_gon; gon++) {
				float rad = deg2rad(360.f * (0.5f + (float)gon) / (float)N_gon);
				this->m_GonPoint[0][gon] = RePos + (xaxis * sin(rad) + yaxis * cos(rad))*Radius;
				//���ʏ�̂���ʂ��|�C���g�擾
				Vector3DX TriPos0 = m_WallBase[0][0];
				Vector3DX TriPos1 = m_WallBase[0][1];
				Vector3DX TriPos2 = m_WallBase[0][2];
				Vector3DX TriNorm = (Vector3DX::Cross((TriPos1 - TriPos0), TriPos2 - TriPos0)).normalized();

				Vector3DX PosN = GetMinPosSegmentToPoint(TriPos0, TriNorm, m_GonPoint[0][gon]);
				float pAN = std::abs(Vector3DX::Dot((this->m_GonPoint[0][gon] - PosN), TriNorm));
				float pBN = std::abs(Vector3DX::Dot(((this->m_GonPoint[0][gon] + vec) - PosN), TriNorm));
				this->m_GonPoint[1][gon] = m_GonPoint[0][gon] + vec * (pAN / (pAN + pBN));
			}

			for (int gon = 0; gon < N_gon; gon++) {
				Vector3DX repos_tmp, pos_tmp;
				if (gon == N_gon) {
					repos_tmp = repos;
					pos_tmp = pos_t;
				}
				else {
					repos_tmp = m_GonPoint[0][gon];
					pos_tmp = m_GonPoint[0][gon] + vec;
				}
				for (int i = 0; i < 2; i++) {
					auto res1 = HitCheck_Line_Triangle(repos_tmp.get(), pos_tmp.get(), m_WallBase[i][0].get(), m_WallBase[i][1].get(), m_WallBase[i][2].get());
					if (res1.HitFlag == TRUE) {
						//����4�p�ɂē���������
						for (int index = 0; index < this->m_WallIndex[0].size() / Triangle_Num; index++) {
							auto GetVertexPos = [&](int ID) {return &(this->m_WallVertex[0][this->m_WallIndex[0].at(index * (size_t)Triangle_Num + ID)].pos); };
							auto res2 = HitCheck_Line_Triangle(repos_tmp.get(), pos_tmp.get(), *GetVertexPos(0), *GetVertexPos(1), *GetVertexPos(2));
							if (res2.HitFlag == TRUE) {
								ishit = true;
								*pos = repos + (*pos - repos).normalized()*(((Vector3DX)res1.Position - repos).magnitude());
								if (norm) {
									*norm = this->m_Wallnorm;
								}
								break;
							}
						}
						break;
					}
				}
				if (ishit) { break; }
			}
		}
		{
			Vector3DX repos_tmp = RePos, pos_tmp = RePos + vec;
			for (int i = 0; i < 2; i++) {
				auto res1 = HitCheck_Line_Triangle(repos_tmp.get(), pos_tmp.get(), m_WallBase[i][0].get(), m_WallBase[i][1].get(), m_WallBase[i][2].get());
				if (res1.HitFlag == TRUE) {
					//����4�p�ɂē���������
					for (int index = 0; index < this->m_WallIndex[0].size() / Triangle_Num; index++) {
						auto GetVertexPos = [&](int ID) {return &(this->m_WallVertex[0][this->m_WallIndex[0].at(index * (size_t)Triangle_Num + ID)].pos); };
						auto res2 = HitCheck_Line_Triangle(repos_tmp.get(), pos_tmp.get(), *GetVertexPos(0), *GetVertexPos(1), *GetVertexPos(2));
						if (res2.HitFlag == TRUE) {
							this->m_TexHandle1.SetDraw_Screen(false);
							{
								Vector3DX TriPos0 = m_WallBase[0][0];
								Vector3DX TriPos1 = m_WallBase[0][1];
								Vector3DX TriPos2 = m_WallBase[0][2];
								Vector3DX TriNorm = (Vector3DX::Cross((TriPos1 - TriPos0), TriPos2 - TriPos0)).normalized();

								Vector3DX Zvec = Vector3DX::up();// (Side[1]->Pos - this->m_BasePos).normalized();
								Matrix4x4DX Mat = Matrix4x4DX::Axis1(TriNorm, Zvec).inverse();

								Vector3DX PosN = GetMinPosSegmentToPoint(TriPos0, TriNorm, RePos);
								float pAN = std::abs(Vector3DX::Dot((RePos - PosN), TriNorm));
								float pBN = std::abs(Vector3DX::Dot(((RePos + vec) - PosN), TriNorm));
								Vector3DX Pos = RePos + vec * (pAN / (pAN + pBN));

								Vector3DX Pos2D = Matrix4x4DX::Vtrans((Pos - this->m_BasePos), Mat) / Scale_Rate;
								Pos2D.Set(Pos2D.x, -Pos2D.z, 0.f);

								DrawCircle((int)(Pos2D.x * 512), (int)(Pos2D.y * 512), (int)(512.f * radius*12.5f), GetColor(0, 0, 0));
							}
							break;
						}
					}
					break;
				}
			}
		}
		if (ishit) {
			if (this->m_WallCalc.get() == nullptr) {
				this->m_WallCalc = std::make_shared<std::thread>([&]() {
					this->m_isThreadEnd = false;
					LONGLONG OLDTime = GetNowHiPerformanceCount();
					auto GonPoint2 = m_GonPoint;//�ӂ̐�
					int N_gon = (int)GonPoint2[1].size();
					{
						for (auto& Side : this->m_Side) {
							int index = (int)(&Side - &this->m_Side.front());
							auto GetVertex = [&](int ID) {return &(this->m_WallVertex[0][this->m_WallIndex[0].at(index * (size_t)Triangle_Num + ID)]); };
							auto GetVertexPos = [&](int ID) {return &(GetVertex(ID)->pos); };
							Vector3DX TriPos0 = *GetVertexPos(0);
							Vector3DX TriPos1 = *GetVertexPos(1);
							Vector3DX TriPos2 = *GetVertexPos(2);
							Vector3DX TriNorm = (Vector3DX::Cross((TriPos1 - TriPos0), TriPos2 - TriPos0)).normalized();

							for (int gon = 0; gon < N_gon; gon++) {
								//���ɓ����Ă��镔��
								auto res2 = HitCheck_Line_Triangle(GonPoint2[0][gon].get(), (GonPoint2[0][gon] + (GonPoint2[1][gon] - GonPoint2[0][gon])*2.f).get(), TriPos0.get(), TriPos1.get(), TriPos2.get());
								if ((res2.HitFlag == TRUE)) {
									Side.resize(Side.size() + 1);
									Side.back() = std::make_unique<SideControl>();
									Side.back()->Set(SideType::Square, res2.Position, this->m_BasePos, TriNorm, GetVertex(0)->dif, GetVertex(0)->spc, gon);
								}
							}
							//n_side�̒��ɂ���_�̍폜(�O���Ƃ��Ă͕s�v�Ȃ��̂̂���)
							for (int s = 0; s < Side.size(); s++) {
								if (Side[s]->m_Type == SideType::Triangle) {
									bool isIn = true;
									for (int gon = 0; gon < N_gon; gon++) {
										if (!GetSamePoint(GonPoint2[1][gon], GonPoint2[1][(gon + 1) % N_gon], GonPoint2[1][(gon + 2) % N_gon], Side[s]->m_Pos)) {
											isIn = false;
											break;
										}
									}
									if (isIn) {
										Side[s].reset();
										Side.erase(Side.begin() + s);
										s--;
									}
								}
							}
							//�O�p�ƕӂ̌�_��ǉ�
							for (int gon = 0; gon < N_gon; gon++) {
								Vector3DX pos1 = GonPoint2[1][gon];
								Vector3DX pos2 = GonPoint2[1][(gon + 1) % N_gon];
								for (int tri = 0; tri < Triangle_Num; tri++) {
									Vector3DX TriPost0 = *GetVertexPos(tri);
									Vector3DX TriPost1 = *GetVertexPos((tri + 1) % Triangle_Num);
									Vector3DX TriPost2 = *GetVertexPos((tri + 2) % Triangle_Num);
									SEGMENT_SEGMENT_RESULT Res;
									if (GetSegmenttoSegment(pos1, pos2, TriPost0, TriPost1, &Res)) {
										//pos2���O�p�̕ӂ̂ǂ���ɂ��邩
										if (GetSamePoint(TriPost0, TriPost1, TriPost2, pos2)) {
											Side.resize(Side.size() + 1);
											Side.back() = std::make_unique<SideControl>();
											Side.back()->Set(SideType::Mix_OuttoIn, Res.SegA_MinDist_Pos, this->m_BasePos, TriNorm, GetVertex(tri)->dif, GetVertex(tri)->spc, gon);
										}
										else {
											Side.resize(Side.size() + 1);
											Side.back() = std::make_unique<SideControl>();
											Side.back()->Set(SideType::Mix_IntoOut, Res.SegA_MinDist_Pos, this->m_BasePos, TriNorm, GetVertex(tri)->dif, GetVertex(tri)->spc, (gon + 1) % N_gon);
										}
									}
								}
							}
							std::vector<Vector3DX>	Point2D;//�ӂ̐�
							{
								struct DATA {
									Vector3DX point;//�ӂ̐�
									int SquarePoint{ 0 };//�ӂ̐�
									SideType type{ SideType::None };//�ӂ̐�
								};
								std::vector<DATA> data;//�ӂ̐�
								for (int s = 0; s < Side.size(); s++) {
									if (Side[s]->m_SquarePoint != -1) {
										//IntoOut�Ɣԍ��������Ȃ�OuttoIn���D��
										data.resize(data.size() + 1);
										data.back().point = Side[s]->m_Pos2D;
										data.back().SquarePoint = Side[s]->m_SquarePoint;
										data.back().type = Side[s]->m_Type;
									}
								}
								std::sort(data.begin(), data.end(), [](const DATA& a, const DATA& b) {
									return (a.SquarePoint == b.SquarePoint) ? ((a.type == SideType::Mix_IntoOut) && (b.type == SideType::Mix_OuttoIn)) : (a.SquarePoint < b.SquarePoint);
								});
								for (int loop = 0; loop < data.size(); loop++) {
									Point2D.emplace_back(data[loop].point);
								}
							}
							// ��ԊO���̋���O�p�`�𐶐��A�����ł͉�ʓ��̓_����Ƃ��ĉ�ʃT�C�Y���܂ގO�p�`�����
							Vector3DX position; position.Set(0, 0, 0.f);
							Vector3DX Size; Size.Set(200.f, 200.f, 0.f);
							CalcDelaunay(&this->m_Tri2D[index], Side, Triangle::GetExternalTriangle(position, Size), Point2D);
							Point2D.clear();
						}
						this->m_IsDraw = false;
						SetNext();
						SetSide();
						this->m_IsDraw = true;
					}
					this->m_isThreadEnd = true;
					if ((GetNowHiPerformanceCount() - OLDTime) > 16 * 2 * 1000) {
						this->m_Bigcount++;
					}
				});
			}
		}
		return ishit;
	}
	//
	void	BreakWall::DrawCommon(void) noexcept {
		for (auto& w : this->m_Walls) {
			w.Draw();
		}
	}
	void	BreakWall::Init(void) noexcept {
		MV1::Load("data/model/map/wallpoint.mv1", &this->m_ObjGround_Wallpoint);
		MV1::Load("data/model/wall/model.mqoz", &this->m_objWall);
		MV1::Load("data/model/wallcol/model.mv1", &this->m_objWallCol);
		{
			float lim = 0.5f;
			MV1_REF_POLYGONLIST p = MV1GetReferenceMesh(this->m_ObjGround_Wallpoint.get(), 0, FALSE);
			for (int i = 0; i < p.PolygonNum; i++) {
				Vector3DX pos0 = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
				Vector3DX pos1 = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
				Vector3DX pos2 = p.Vertexs[p.Polygons[i].VIndex[2]].Position;

				auto p0 = (pos0 - pos1); auto py0 = p0.y; p0.y = (0);
				auto p1 = (pos1 - pos2); auto py1 = p1.y;  p1.y = (0);
				auto p2 = (pos2 - pos0); auto py2 = p2.y;  p2.y = (0);

				if (p0.magnitude() > lim && py0 <= lim) {
					this->m_WallParts.resize(this->m_WallParts.size() + 1);
					this->m_WallParts.back()[0] = pos0;
					this->m_WallParts.back()[1] = pos1;
					continue;
				}
				if (p1.magnitude() > lim && py1 <= lim) {
					this->m_WallParts.resize(this->m_WallParts.size() + 1);
					this->m_WallParts.back()[0] = pos1;
					this->m_WallParts.back()[1] = pos2;
					continue;
				}
				if (p2.magnitude() > lim && py2 <= lim) {
					this->m_WallParts.resize(this->m_WallParts.size() + 1);
					this->m_WallParts.back()[0] = pos2;
					this->m_WallParts.back()[1] = pos0;
					continue;
				}
			}
			MV1TerminateReferenceMesh(this->m_ObjGround_Wallpoint.get(), 0, FALSE);
			this->m_Walls.resize(this->m_WallParts.size());
			for (int i = 0; i < this->m_WallParts.size(); i++) {
				auto pos = (this->m_WallParts[i][0] + this->m_WallParts[i][1]) / 2.f;
				auto vec = (this->m_WallParts[i][0] - this->m_WallParts[i][1]); vec.y = (0);

				this->m_Walls[i].Init(this->m_objWall, this->m_objWallCol, pos, std::atan2f(vec.z, vec.x), 2.7f);
			}
			//this->m_Walls.clear();
		}
		SetUseBackCulling(TRUE);
		SetTextureAddressModeUV(DX_TEXADDRESS_WRAP, DX_TEXADDRESS_WRAP);
	}
	void	BreakWall::Execute(void) noexcept {
		for (auto& w : this->m_Walls) {
			w.Execute();
		}
		//�j��
		for (int i = 0; i < this->m_Walls.size(); i++) {
			if (this->m_Walls[i].GetIsBreak()) {
				std::swap(this->m_Walls.back(), this->m_Walls[i]);
				this->m_Walls.pop_back();
				i--;
			}
		}
	}
	void	BreakWall::Dispose(void) noexcept {
		this->m_Walls.clear();
	}
};
