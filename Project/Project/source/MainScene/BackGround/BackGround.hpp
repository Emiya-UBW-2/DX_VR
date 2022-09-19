#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	const int Triangle_Num = 3;

	enum class SideType : char {
		None,
		Triangle,
		Square,
		Mix_OuttoIn,
		Mix_IntoOut,
	};
	struct SideControl {
	//private:
		SideType					Type;
		int							SquarePoint{ -1 };//�G��Ă���l�p�̕�
		VECTOR_ref					Pos;
		VECTOR_ref					Pos2D;
		MATRIX_ref					Mat;
		VECTOR_ref					Norm;							// �@��
		COLOR_U8					dif;							// �f�B�t���[�Y�J���[
		COLOR_U8					spc;							// �X�y�L�����J���[
	public:
		void Set(SideType type, const VECTOR_ref& pos, const VECTOR_ref& basepos, const VECTOR_ref& normal, 
				COLOR_U8					difcolor,
				COLOR_U8					spccolor,
			
			int squarepoint = -1) {
			Type = type;
			SquarePoint = squarepoint;//�G��Ă���l�p�̕�
			Pos = pos;

			VECTOR_ref Zvec = VECTOR_ref::up();// (Side[1]->Pos - this->m_BasePos).Norm();
			Mat = MATRIX_ref::Axis1_YZ(normal, Zvec).Inverse();

			Pos2D = MATRIX_ref::Vtrans((Pos - basepos), Mat);
			Pos2D.Set(Pos2D.x(), Pos2D.z(), 0.f);


			Norm = normal;
			dif = difcolor;
			spc = spccolor;
		}
	};

	class LineControl {
		VECTOR_ref m_start;
		VECTOR_ref m_end;
	public:
		void Set(const VECTOR_ref& start, const VECTOR_ref& end) {
			this->m_start = start;
			this->m_end = end;
		}
		const auto& Getstart() const { return this->m_start; }
		const auto& Getend() const { return this->m_end; }
	public:
		/**
		 * �^����ꂽ�_���܂�ł��邩
		 * @param {VECTOR_ref} point �����Ώۂ̓_
		 * @return {bool} �ӂ��^���ꂽ�_���܂�ł�����true
		 */
		bool HasPoint(const VECTOR_ref& point) const { return (this->m_start == point) || (this->m_end == point); }

		bool operator==(const LineControl& tgt) const {
			return (
				((this->m_start == tgt.m_start) && (this->m_end == tgt.m_end)) ||
				((this->m_start == tgt.m_end) && (this->m_end == tgt.m_start))
				);
		}
	};

	class Triangle {
		std::array<VECTOR_ref, Triangle_Num> m_points;
		std::array<LineControl, Triangle_Num> m_lines;
	public:
		void Set(const VECTOR_ref& point0, const VECTOR_ref& point1, const VECTOR_ref& point2) {
			this->m_points[0] = point0;
			this->m_points[1] = point1;
			this->m_points[2] = point2;
			this->m_lines[0].Set(this->m_points[0], this->m_points[1]);
			this->m_lines[1].Set(this->m_points[1], this->m_points[2]);
			this->m_lines[2].Set(this->m_points[2], this->m_points[0]);
		}
		const auto& Getpoints() const { return this->m_points; }
		const auto& Getlines() const { return this->m_lines; }
	public:
		/**
		 * �^����ꂽ�ӂ��܂܂Ȃ��_��Ԃ�
		 * @param {LineControl} line �����Ώۂ̕�
		 * @return {VECTOR_ref} �^����ꂽ�ӂɊ܂܂�Ȃ��_
		 */
		const VECTOR_ref* noCommonPointByline(const LineControl& line) const {
			for (int loop = 0; loop < this->m_points.size(); loop++) {
				if (!line.HasPoint(this->m_points[loop])) {
					return &(this->m_points[loop]);
				}
			}
			return nullptr;
		}

		/**
		 * �^����ꂽ�ӈȊO�̕ӂ�Ԃ�
		 * @param {LineControl} line �����Ώۂ̕�
		 * @return {Array.<LineControl>} �Y���̕ӈȊO�̕ӂ̔z��
		 */
		std::vector<LineControl> otherlineByline(const LineControl& line) const {
			std::vector<LineControl> result;
			for (int loop = 0; loop < this->m_lines.size(); loop++) {
				if (!(this->m_lines[loop] == line)) {
					result.emplace_back(this->m_lines[loop]);
				}
			}
			return result;
		}

		/**
		 * �^����ꂽ�ӂ��܂�ł��邩�`�F�b�N
		 * @param {LineControl} line �����Ώۂ̕�
		 * @return {bool} �^����ꂽ�ӂ��܂�ł�����true
		 */
		bool Hasline(const LineControl& line) const {
			for (int loop = 0; loop < this->m_lines.size(); loop++) {
				if ((this->m_lines[loop] == line)) {
					return true;
				}
			}
			return false;
		}

		/**
		 * �^����ꂽ�_�̒��_�����邩�m�F
		 * @param {Pointl} point �����Ώۂ̓_
		 * @return {bool} �Ώۂ̓_�����_�ɂ�������true
		 */
		bool HasPoint(const VECTOR_ref& point) const {
			for (int loop = 0; loop < this->m_points.size(); loop++) {
				if ((this->m_points[loop] == point)) {
					return true;
				}
			}
			return false;
		}

		bool operator==(const Triangle& triangle) const {
			for (int loop = 0; loop < triangle.m_points.size(); loop++) {
				if (!HasPoint(triangle.m_points[loop])) {
					return false;
				}
			}
			return true;
		}

		//�O�ډ~��point�������Ă��邩
		bool getCircumscribedCircle(const VECTOR_ref& point) {
			VECTOR_ref point1;

			VECTOR_ref p1 = this->m_points[0];
			VECTOR_ref p2 = this->m_points[1];
			VECTOR_ref p3 = this->m_points[2];

			auto x1_p = p1.x() * p1.x();
			auto x2_p = p2.x() * p2.x();
			auto x3_p = p3.x() * p3.x();
			auto y1_p = p1.y() * p1.y();
			auto y2_p = p2.y() * p2.y();
			auto y3_p = p3.y() * p3.y();

			// �O�ډ~�̒��S���W���v�Z
			auto c = 2 * ((p2.x() - p1.x()) * (p3.y() - p1.y()) - (p2.y() - p1.y()) * (p3.x() - p1.x()));
			point1.Set(
				((p3.y() - p1.y()) * (x2_p - x1_p + y2_p - y1_p) + (p1.y() - p2.y()) * (x3_p - x1_p + y3_p - y1_p)) / c,
				((p1.x() - p3.x()) * (x2_p - x1_p + y2_p - y1_p) + (p2.x() - p1.x()) * (x3_p - x1_p + y3_p - y1_p)) / c,
				0.f
			);
			return (point - point1).Length() < (p1 - point1).Length();
		}

		//�ӂɔ�肪�Ȃ����ǂ���
		bool IsOverlap() const {
			return (
				(this->m_points[0] == this->m_points[1]) ||
				(this->m_points[1] == this->m_points[2]) ||
				(this->m_points[2] == this->m_points[0])
				);
		}
	};

	Triangle GetExternalTriangle(const VECTOR_ref& position, const VECTOR_ref& size);
	//�_AB�����̓������ɂ��邩�ǂ���
	bool GetSamePoint(const VECTOR_ref& LineA, const VECTOR_ref& LineB, const VECTOR_ref& pointA, const VECTOR_ref& pointB);
	/**
	 * �h���l�[�O�p�`�������v�Z
	 *  points �v�Z�Ώۂ̓_�Q
	 *  ExternalTriangle �_������O�p�`
	 */
	void CalcDelaunay(std::vector<Triangle>* Ans, const std::vector<std::unique_ptr<SideControl>>& points, const Triangle& ExternalTriangle, const std::vector<VECTOR_ref>& GonPoint2D);
};


namespace FPS_n2 {
	namespace Sceneclass {
		//kusa
		class Grass {
			class grass_t {
			public:
				bool			canlook = true;
				Model_Instance	m_Inst;
			public:
				void Init(int total, int sel) {
					switch (sel) {
					case 0:
						this->m_Inst.Init("data/model/grass/grass.png", "data/model/grass/model.mv1");
						break;
					case 1:
						this->m_Inst.Init("data/model/grass/grass2.png", "data/model/grass/model.mv1");
						break;
					case 2:
						this->m_Inst.Init("data/model/grass/grass3.png", "data/model/grass/model.mv1");
						break;
					}
					this->m_Inst.Reset();
					this->m_Inst.Set_start(total);
				}
				void Set_one(const MATRIX_ref& mat) noexcept {
					this->m_Inst.Set_one(mat);
				}
				void put(void) noexcept {
					canlook = true;
					this->m_Inst.Execute();
				}
				void Dispose(void) noexcept {
					this->m_Inst.Dispose();
				}
				/*���E�O���ۂ��𔻒f*/
				void Check_CameraViewClip(const VECTOR_ref& min, const VECTOR_ref& max) noexcept {
					this->canlook = true;
					if (CheckCameraViewClip_Box(min.get(), max.get())) {
						this->canlook = false;
						return;
					}
				}
				void Draw(void) noexcept {
					if (this->canlook) {
						this->m_Inst.Draw();
					}
				}
			};
			struct GrassPos {
				int X_PosMin = 0;
				int Y_PosMin = 0;
				int X_PosMax = 0;
				int Y_PosMax = 0;
			};
		public:
			static const int grassDiv{ 12 };//6;
			const float size{ 60.f };
		private:
			const int grasss = 30 * 30;						/*grass�̐�*/
			std::array<grass_t, grassDiv>grass__;
			std::array<VECTOR_ref, grassDiv>grassPosMin;
			std::array<VECTOR_ref, grassDiv>grassPosMax;
			int grasss2 = 10 * 10;							/*grass�̐�*/
			std::array<grass_t, grassDiv>grass2__;
			int grasss3 = 4 * 4;							/*grass�̐�*/
			std::array<grass_t, grassDiv>grass3__;
			int Flag = 0;
			std::array<GrassPos, grassDiv> grassPos;
		private:
			int GetColorSoftImage(int softimage, int x_, int y_) {
				int _r_, _g_, _b_;
				int CCC = 0;
				GetPixelSoftImage(softimage, x_, y_, &_r_, &_g_, &_b_, nullptr);
				if (_r_ <= 64) {}
				else if (_r_ <= 192) { CCC |= (1 << 1); }
				else if (_r_ <= 256) { CCC |= (1 << 2); }
				if (_g_ <= 64) {}
				else if (_g_ <= 192) { CCC |= (1 << 4); }
				else if (_g_ <= 256) { CCC |= (1 << 5); }
				if (_b_ <= 64) {}
				else if (_b_ <= 192) { CCC |= (1 << 7); }
				else if (_b_ <= 256) { CCC |= (1 << 8); }
				return CCC;
			}
			//y�����ɑ��삷��O��
			void SetMinMax(int CCC, int ID, int softimage, int x_t, int y_t, int sizex, int sizey) {
				int BufC = -1;
				if ((Flag & (1 << ID)) == 0) {
					Flag |= (1 << ID);
					//xmin
					grassPos[ID].X_PosMin = x_t;
					y_t;
					//ymin
					BufC = -1;
					for (int y_ = 0; y_ < sizey; y_++) {
						for (int x_ = grassPos[ID].X_PosMin; x_ < sizex; x_++) {
							BufC = GetColorSoftImage(softimage, x_, y_);
							if (BufC == CCC) {
								grassPos[ID].Y_PosMin = y_;
								break;
							}
							else {
								BufC = -1;
							}
						}
						if (BufC >= 0) { break; }
					}
					//xmax
					BufC = -1;
					for (int x_ = sizex - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
						for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
							BufC = GetColorSoftImage(softimage, x_, y_);
							if (BufC == CCC) {
								grassPos[ID].X_PosMax = x_ + 1;
								break;
							}
							else {
								BufC = -1;
							}
						}
						if (BufC >= 0) { break; }
					}
					//ymax
					BufC = -1;
					for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
						for (int x_ = grassPos[ID].X_PosMax - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
							BufC = GetColorSoftImage(softimage, x_, y_);
							if (BufC == CCC) {
								grassPos[ID].Y_PosMax = y_ + 1;
								break;
							}
							else {
								BufC = -1;
							}
						}
						if (BufC >= 0) { break; }
					}
					//ok
				}
			}
		public:
			void Init(const MV1* MapCol) noexcept {
				float MAPX = 300.f*Scale_Rate;
				float MAPZ = 300.f*Scale_Rate;
				float PosX = 0.f;
				float PosZ = 0.f;

				float MINX = -MAPX / 2.f + PosX;
				float MINZ = -MAPZ / 2.f + PosZ;

				float MAXX = MAPX / 2.f + PosX;
				float MAXZ = MAPZ / 2.f + PosZ;

				auto softimage = LoadSoftImage("data/grass.png");

				int sizex = 0, sizey = 0;
				GetSoftImageSize(softimage, &sizex, &sizey);

				Flag = 0;
				for (int x_ = 0; x_ < sizex; x_++) {
					for (int y_ = 0; y_ < sizey; y_++) {
						int CCC = GetColorSoftImage(softimage, x_, y_);
						//255,0,0
						if (CCC == (1 << 2)) {
							SetMinMax(CCC, 0, softimage, x_, y_, sizex, sizey);
						}
						//255,128,0
						else if (CCC == ((1 << 2) | (1 << 4))) {
							SetMinMax(CCC, 1, softimage, x_, y_, sizex, sizey);
						}
						//255,255,0
						else if (CCC == ((1 << 2) | (1 << 5))) {
							SetMinMax(CCC, 2, softimage, x_, y_, sizex, sizey);
						}
						//128,255,0
						else if (CCC == ((1 << 1) | (1 << 5))) {
							SetMinMax(CCC, 3, softimage, x_, y_, sizex, sizey);
						}
						//0,255,0
						else if (CCC == (1 << 5)) {
							SetMinMax(CCC, 4, softimage, x_, y_, sizex, sizey);
						}
						//0,255,128
						else if (CCC == ((1 << 5) | (1 << 7))) {
							SetMinMax(CCC, 5, softimage, x_, y_, sizex, sizey);
						}
						//0,255,255
						else if (CCC == ((1 << 5) | (1 << 8))) {
							SetMinMax(CCC, 6, softimage, x_, y_, sizex, sizey);
						}
						//0,128,255
						else if (CCC == ((1 << 4) | (1 << 8))) {
							SetMinMax(CCC, 7, softimage, x_, y_, sizex, sizey);
						}
						//0,0,255
						else if (CCC == (1 << 8)) {
							SetMinMax(CCC, 8, softimage, x_, y_, sizex, sizey);
						}
						//128,0,255
						else if (CCC == ((1 << 1) | (1 << 8))) {
							SetMinMax(CCC, 9, softimage, x_, y_, sizex, sizey);
						}
						//255,0,255
						else if (CCC == ((1 << 2) | (1 << 8))) {
							SetMinMax(CCC, 10, softimage, x_, y_, sizex, sizey);
						}
						//255,0,128
						else if (CCC == ((1 << 2) | (1 << 7))) {
							SetMinMax(CCC, 11, softimage, x_, y_, sizex, sizey);
						}

						//MINX + (MAXX - MINX) * x_ / sizex = x_t 
						//MINZ + (MAXZ - MINZ) * y_ / sizey = z_t 
					}
				}
				for (int ID = 0; ID < grassDiv; ID++) {
					//�|�W�V��������
					float xp = MINX + (MAXX - MINX) * grassPos[ID].X_PosMin / sizex;
					float zp = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMin / sizey;
					float xp2 = MINX + (MAXX - MINX) * grassPos[ID].X_PosMax / sizex;
					float zp2 = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMax / sizey;
					float xsize = xp2 - xp;
					float zsize = zp2 - zp;
					//
					{
						grassPosMin[ID] = VECTOR_ref::vget(xp, 0.2f, zp);
						grassPosMax[ID] = grassPosMin[ID] + VECTOR_ref::vget(xsize, 1.f, zsize);
						float xmid = xsize / 2.f;
						float zmid = zsize / 2.f;
						if (grasss != 0) {
							auto& tgt_g = grass__[ID];
							tgt_g.Init(grasss, 0);
							for (int i = 0; i < grasss; ++i) {
								float x1 = xmid + GetRandf(xmid);
								float z1 = zmid + GetRandf(zmid);
								while (true) {
									int CCC = GetColorSoftImage(softimage,
										(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
										(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
									);
									if (CCC != 0) {
										break;
									}
									x1 = xmid + GetRandf(xmid);
									z1 = zmid + GetRandf(zmid);
								}

								auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.2f, z1);
								auto tmpscale = VECTOR_ref::vget(size*1.f, 6.0f + GetRandf(5.5f), size*1.f);
								auto res = MapCol->CollCheck_Line(tmpvect + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), tmpvect + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
								if (res.HitFlag == TRUE) { tmpvect = res.HitPosition; }
								tgt_g.Set_one(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale) * MATRIX_ref::RotVec2(VECTOR_ref::up(), ((VECTOR_ref)(res.Normal)).Norm())* MATRIX_ref::Mtrans(tmpvect));
							}
							tgt_g.put();
						}
						if (grasss2 != 0) {
							auto& tgt_g = grass2__[ID];
							tgt_g.Init(grasss2, 1);
							for (int i = 0; i < grasss2; ++i) {
								float x1 = xmid + GetRandf(xmid);
								float z1 = zmid + GetRandf(zmid);
								while (true) {
									int CCC = GetColorSoftImage(softimage,
										(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
										(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
									);
									if (CCC != 0) {
										break;
									}
									x1 = xmid + GetRandf(xmid);
									z1 = zmid + GetRandf(zmid);
								}
								auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.f, z1);
								auto tmpscale = VECTOR_ref::vget(size*1.f, 6.0f + GetRandf(5.5f), size*1.f);
								auto res = MapCol->CollCheck_Line(tmpvect + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), tmpvect + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
								if (res.HitFlag == TRUE) { tmpvect = res.HitPosition; }
								tgt_g.Set_one(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale) * MATRIX_ref::RotVec2(VECTOR_ref::up(), ((VECTOR_ref)(res.Normal)).Norm())* MATRIX_ref::Mtrans(tmpvect));
							}
							tgt_g.put();
						}
						if (grasss3 != 0) {
							auto& tgt_g = grass3__[ID];
							tgt_g.Init(grasss3, 2);
							for (int i = 0; i < grasss3; ++i) {
								float x1 = xmid + GetRandf(xmid);
								float z1 = zmid + GetRandf(zmid);
								while (true) {
									int CCC = GetColorSoftImage(softimage,
										(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
										(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
									);
									if (CCC != 0) {
										break;
									}
									x1 = xmid + GetRandf(xmid);
									z1 = zmid + GetRandf(zmid);
								}
								auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.f, z1);
								auto tmpscale = VECTOR_ref::vget(size*1.f, 6.0f + GetRandf(5.5f), size*1.f);
								auto res = MapCol->CollCheck_Line(tmpvect + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), tmpvect + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
								if (res.HitFlag == TRUE) { tmpvect = res.HitPosition; }
								tgt_g.Set_one(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale) * MATRIX_ref::RotVec2(VECTOR_ref::up(), ((VECTOR_ref)(res.Normal)).Norm())* MATRIX_ref::Mtrans(tmpvect));
							}
							tgt_g.put();
						}

						{
							auto res = MapCol->CollCheck_Line(grassPosMin[ID] + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), grassPosMin[ID] + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
							if (res.HitFlag == TRUE) { grassPosMin[ID] = res.HitPosition; }
							res = MapCol->CollCheck_Line(grassPosMax[ID] + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), grassPosMax[ID] + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
							if (res.HitFlag == TRUE) { grassPosMax[ID] = res.HitPosition; }
						}
					}
					//
				}
				DeleteSoftImage(softimage);
			}
			void Dispose(void) noexcept {
				for (int ID = 0; ID < grassDiv; ID++) {
					if (grasss != 0) {
						grass__[ID].Dispose();
					}
					if (grasss2 != 0) {
						grass2__[ID].Dispose();
					}
					if (grasss3 != 0) {
						grass3__[ID].Dispose();
					}
				}
			}
			void Draw(void) noexcept {
				SetFogEnable(TRUE);
				SetFogColor(184, 187, 118);
				SetDrawAlphaTest(DX_CMP_GREATER, 128);
				//SetUseLighting(FALSE);
				SetUseLightAngleAttenuation(FALSE);
				//auto dir=GetLightDirection();
				//VECTOR_ref vec = (VECTOR_ref)GetCameraPosition() - GetCameraTarget();
				//SetLightDirection(vec.Norm().get());

				for (int ID = 0; ID < grassDiv; ID++) {
#ifdef DEBUG
					DrawCube3D(grassPosMin[ID].get(), grassPosMax[ID].get(), GetColor(0, 0, 0), GetColor(0, 0, 0), FALSE);
#endif
					if (grasss != 0) {
						this->grass__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
						grass__[ID].Draw();
					}
					if (grasss2 != 0) {
						this->grass2__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
						grass2__[ID].Draw();
					}
					if (grasss3 != 0) {
						this->grass3__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
						grass3__[ID].Draw();
					}
				}
				//SetLightDirection(dir);

				SetUseLightAngleAttenuation(TRUE);
				//SetUseLighting(TRUE);
				SetDrawAlphaTest(-1, 0);
				SetFogEnable(FALSE);
			}
		};

		class WallObj {
		public:
		private:
			MV1										m_obj;
			std::vector<VERTEX3D>					m_WallVertex;		//�ǂ��Z�b�g
			VERTEX3D*								m_WallVertexPtr;
			std::vector<WORD>						m_WallIndex;		//�ǂ��Z�b�g
			WORD*									m_WallIndexPtr;

			int										m_TexHandle;

			std::vector<std::vector<std::unique_ptr<SideControl>>>	m_Side;
			std::vector<std::vector<Triangle>>		m_Tri2D;
			VECTOR_ref								m_BasePos;

			//int addtri = 0;
		public://getter
		private:
			void			SetFirst(const MV1_REF_POLYGONLIST& PolyList, const VECTOR_ref& pos, float YRad, float YScale) noexcept {
				auto matrix = MATRIX_ref::RotY(YRad)*MATRIX_ref::Mtrans(pos);
				this->m_WallVertex.resize(PolyList.VertexNum);
				for (int i = 0; i < this->m_WallVertex.size(); i++) {
					VECTOR_ref Pos = PolyList.Vertexs[i].Position;
					if (Pos.y() > 0.f) { Pos.y(Pos.y()*YScale); }
					this->m_WallVertex[i].pos = MATRIX_ref::Vtrans(Pos, matrix).get();
					this->m_WallVertex[i].norm = MATRIX_ref::Vtrans(PolyList.Vertexs[i].Normal, matrix.GetRot()).get();
					this->m_WallVertex[i].dif = PolyList.Vertexs[i].DiffuseColor;
					this->m_WallVertex[i].spc = PolyList.Vertexs[i].SpecularColor;
					this->m_WallVertex[i].u = PolyList.Vertexs[i].TexCoord[0].u;
					this->m_WallVertex[i].v = Lerp(1.f, PolyList.Vertexs[i].TexCoord[0].v, YScale);
					this->m_WallVertex[i].su = PolyList.Vertexs[i].TexCoord[1].u;
					this->m_WallVertex[i].sv = PolyList.Vertexs[i].TexCoord[1].v;
				}
				this->m_WallIndex.resize(PolyList.PolygonNum * Triangle_Num);
				for (int i = 0; i < this->m_WallIndex.size() / Triangle_Num; i++) {
					this->m_WallIndex[i * Triangle_Num + 0] = (WORD)PolyList.Polygons[i].VIndex[0];
					this->m_WallIndex[i * Triangle_Num + 1] = (WORD)PolyList.Polygons[i].VIndex[1];
					this->m_WallIndex[i * Triangle_Num + 2] = (WORD)PolyList.Polygons[i].VIndex[2];
				}
				this->m_WallVertexPtr = this->m_WallVertex.data();
				this->m_WallIndexPtr = this->m_WallIndex.data();
			}
			void			SetNext() {
				this->m_WallVertex.clear();
				this->m_WallIndex.clear();
				for (auto& Side : this->m_Side) {
					size_t index = &Side - &this->m_Side.front();
					//�o�������̂����X�g�ɍēo�^
					for (auto& s : this->m_Tri2D[index]) {
						for (auto& p : s.Getpoints()) {
							VECTOR_ref p2; p2.Set(p.x(), 0.f, p.y());
							this->m_WallVertex.resize(this->m_WallVertex.size() + 1);
							this->m_WallVertex.back().pos = (MATRIX_ref::Vtrans(p2, Side[0]->Mat.Inverse()) + this->m_BasePos).get();
							this->m_WallVertex.back().norm = Side[0]->Norm.get();
							this->m_WallVertex.back().dif = Side[0]->dif;
							this->m_WallVertex.back().spc = Side[0]->spc;
							this->m_WallVertex.back().u = -p.x() / Scale_Rate;
							this->m_WallVertex.back().v = p.y() / Scale_Rate;
							this->m_WallVertex.back().su = -p.x() / Scale_Rate;
							this->m_WallVertex.back().sv = p.y() / Scale_Rate;
							this->m_WallIndex.emplace_back((WORD)(this->m_WallVertex.size() - 1));
						}
					}
					//
				}
				this->m_WallVertexPtr = this->m_WallVertex.data();
				this->m_WallIndexPtr = this->m_WallIndex.data();
			}
			void			SetSide(const VECTOR_ref* basepos = nullptr) noexcept {
				if (basepos) { this->m_BasePos = *basepos; }
				for (auto& s : this->m_Side) {
					for (auto&s2 : s) { s2.reset(); }
					s.clear();
				}
				this->m_Side.clear();
				this->m_Side.resize(this->m_WallIndex.size() / Triangle_Num);
				this->m_Tri2D.resize(m_Side.size());
				for (auto& Side : this->m_Side) {
					size_t index = &Side - &this->m_Side.front();
					auto GetVertex = [&](int ID) {return &(this->m_WallVertex[this->m_WallIndex[(int)index * Triangle_Num + ID]]); };
					auto GetVertexPos = [&](int ID) {return &(GetVertex(ID)->pos); };
					VECTOR_ref TriPos0 = *GetVertexPos(0);
					VECTOR_ref TriPos1 = *GetVertexPos(1);
					VECTOR_ref TriPos2 = *GetVertexPos(2);
					VECTOR_ref TriNorm = ((TriPos1 - TriPos0).cross(TriPos2 - TriPos0)).Norm();

					Side.resize(Side.size() + Triangle_Num);
					Side[0] = std::make_unique<SideControl>();
					Side[0]->Set(SideType::Triangle, *GetVertexPos(0), this->m_BasePos, TriNorm, GetVertex(0)->dif, GetVertex(0)->spc);

					Side[1] = std::make_unique<SideControl>();
					Side[1]->Set(SideType::Triangle, *GetVertexPos(1), this->m_BasePos, TriNorm, GetVertex(1)->dif, GetVertex(1)->spc);

					Side[2] = std::make_unique<SideControl>();
					Side[2]->Set(SideType::Triangle, *GetVertexPos(2), this->m_BasePos, TriNorm, GetVertex(2)->dif, GetVertex(2)->spc);
				}
			}
		public:
			void			Init(const MV1& obj, const VECTOR_ref& pos, float YRad, float YScale) noexcept {
				this->m_TexHandle = MV1GetTextureGraphHandle(obj.get(), 0);

				SetFirst(MV1GetReferenceMesh(obj.get(), 0, FALSE), pos, YRad, YScale);
				MV1TerminateReferenceMesh(obj.get(), 0, FALSE);

				VECTOR_ref basepos = this->m_WallVertex[this->m_WallIndex[0]].pos;
				SetSide(&basepos);

				MATERIALPARAM							m_Material;
				m_Material.Diffuse = GetLightDifColor();
				m_Material.Specular = GetLightSpcColor();
				m_Material.Ambient = GetLightAmbColor();
				m_Material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
				m_Material.Power = 20.0f;
				SetMaterialParam(m_Material);
			}

			void			Execute(void) noexcept {
				//addtri += GetMouseWheelRotVolWithCheck();
			}

			void			Draw(bool IsCalling) noexcept {
				SetUseBackCulling(IsCalling ? TRUE : FALSE);
				SetTextureAddressModeUV(DX_TEXADDRESS_WRAP, DX_TEXADDRESS_WRAP);
				DrawPolygonIndexed3D(this->m_WallVertexPtr, (int)this->m_WallVertex.size(), this->m_WallIndexPtr, (int)(this->m_WallIndex.size()) / Triangle_Num, this->m_TexHandle, TRUE);
				SetUseBackCulling(FALSE);

				return;
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
							int x1 = x + xadd + (int)(s[loop].Getpoints()[0].x()*scale);
							int y1 = y + yadd - (int)(s[loop].Getpoints()[0].y()*scale);
							int x2 = x + xadd + (int)(s[loop].Getpoints()[1].x()*scale);
							int y2 = y + yadd - (int)(s[loop].Getpoints()[1].y()*scale);
							int x3 = x + xadd + (int)(s[loop].Getpoints()[2].x()*scale);
							int y3 = y + yadd - (int)(s[loop].Getpoints()[2].y()*scale);
							DrawTriangle(x1, y1, x2, y2, x3, y3, GetColor(0, 255 * (loop + 1) / (int)s.size(), 0), TRUE);
						}
					}
					x += xsize;
				}
#endif
			}

			bool			CheckHit(const VECTOR_ref& repos, VECTOR_ref* pos) {
				bool ishit = false;
				float length = (*pos - repos).size();
				for (int index = 0; index < this->m_WallIndex.size() / Triangle_Num; index++) {
					auto GetVertex = [&](int ID) {return &(this->m_WallVertex[this->m_WallIndex[(int)index * Triangle_Num + ID]]); };
					auto GetVertexPos = [&](int ID) {return &(GetVertex(ID)->pos); };
					auto res = HitCheck_Line_Triangle(repos.get(), pos->get(), *GetVertexPos(0), *GetVertexPos(1), *GetVertexPos(2));
					if (res.HitFlag == TRUE) {
						ishit = true;
						auto lentmp = ((VECTOR_ref)res.Position - repos).size();
						if (length > lentmp) {
							length = lentmp;
						}
					}
				}
				if (ishit) {
					CalcPoint(repos, *pos);
				}
				*pos = repos + (*pos - repos).Norm()*(length);
				return ishit;
			}

			void			CalcPoint(const VECTOR_ref& repos, const VECTOR_ref& pos) {
				VECTOR_ref vec = (pos - repos);
				VECTOR_ref xaxis = vec.Norm().cross(VECTOR_ref::up());
				VECTOR_ref yaxis = vec.Norm().cross(xaxis);
				const int N_gon = 5;
				const float Radius = 0.0762f*Scale_Rate;
				std::array<VECTOR_ref, N_gon> GonPoint;//�ӂ̐�
				for (auto& Side : this->m_Side) {
					size_t index = &Side - &this->m_Side.front();
					auto GetVertex = [&](int ID) {return &(this->m_WallVertex[this->m_WallIndex[(int)index * Triangle_Num + ID]]); };
					auto GetVertexPos = [&](int ID) {return &(GetVertex(ID)->pos); };
					VECTOR_ref TriPos0 = *GetVertexPos(0);
					VECTOR_ref TriPos1 = *GetVertexPos(1);
					VECTOR_ref TriPos2 = *GetVertexPos(2);
					VECTOR_ref TriNorm = ((TriPos1 - TriPos0).cross(TriPos2 - TriPos0)).Norm();

					for (int gon = 0; gon < N_gon; gon++) {
						float rad = deg2rad(360.f * (0.5f + (float)gon) / (float)N_gon);
						VECTOR_ref BasePos = repos + (xaxis * sin(rad) + yaxis * cos(rad))*Radius;
						//���ʏ�̂���ʂ��|�C���g�擾
						{
							VECTOR_ref PosN = Plane_Point_MinLength_Position(TriPos0.get(), TriNorm.get(), BasePos.get());
							float pAN = std::abs((BasePos - PosN).dot(TriNorm));
							float pBN = std::abs(((BasePos + vec) - PosN).dot(TriNorm));
							GonPoint[gon] = BasePos + vec * (pAN / (pAN + pBN));
						}
						//���ɓ����Ă��镔��
						{
							auto res2 = HitCheck_Line_Triangle(BasePos.get(), (BasePos + vec).get(), TriPos0.get(), TriPos1.get(), TriPos2.get());
							if ((res2.HitFlag == TRUE)) {
								Side.resize(Side.size() + 1);
								Side.back() = std::make_unique<SideControl>();
								Side.back()->Set(SideType::Square, res2.Position, this->m_BasePos, TriNorm, GetVertex(0)->dif, GetVertex(0)->spc, gon);
							}
						}
					}
					//n_side�̒��ɂ���_�̍폜(�O���Ƃ��Ă͕s�v�Ȃ��̂̂���)
					for (int s = 0; s < Side.size(); s++) {
						if (Side[s]->Type == SideType::Triangle) {
							bool isIn = true;
							for (int gon = 0; gon < N_gon; gon++) {
								if (!GetSamePoint(GonPoint[gon], GonPoint[(gon + 1) % N_gon], GonPoint[(gon + 2) % N_gon], Side[s]->Pos)) {
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
						VECTOR_ref pos1 = GonPoint[gon];
						VECTOR_ref pos2 = GonPoint[(gon + 1) % N_gon];
						for (int tri = 0; tri < Triangle_Num; tri++) {
							VECTOR_ref TriPost0 = *GetVertexPos(tri);
							VECTOR_ref TriPost1 = *GetVertexPos((tri + 1) % Triangle_Num);
							VECTOR_ref TriPost2 = *GetVertexPos((tri + 2) % Triangle_Num);
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
					std::vector<VECTOR_ref>	Point2D;//�ӂ̐�
					{
						struct DATA {
							VECTOR_ref point;//�ӂ̐�
							int SquarePoint;//�ӂ̐�
							SideType type;//�ӂ̐�
						};
						std::vector<DATA> data;//�ӂ̐�
						for (int s = 0; s < Side.size(); s++) {
							if (Side[s]->SquarePoint != -1) {
								//IntoOut�Ɣԍ��������Ȃ�OuttoIn���D��
								data.resize(data.size() + 1);
								data.back().point = Side[s]->Pos2D;
								data.back().SquarePoint = Side[s]->SquarePoint;
								data.back().type = Side[s]->Type;
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
					VECTOR_ref position; position.Set(0, 0, 0.f);
					VECTOR_ref Size; Size.Set(200.f, 200.f, 0.f);
					CalcDelaunay(&this->m_Tri2D[index], Side, GetExternalTriangle(position, Size), Point2D);
					Point2D.clear();
				}
				SetNext();
				SetSide();
			}
		};

		class BackGroundClass {
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;
			MV1							m_ObjGroundCol_Box2D;
			MV1							m_objWall;
			std::shared_ptr<b2World>	m_b2world;
			std::vector<std::pair<b2Pats, std::array<VECTOR_ref, 2>>>	m_b2wallParts;	//�ǂ��Z�b�g
			Grass						m_grass;

			std::vector<WallObj>		m_Walls;
		public://getter
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
			auto&			GetBox2Dworld(void) noexcept { return this->m_b2world; }

			const auto		GetWallCol(const VECTOR_ref& repos, VECTOR_ref* pos) noexcept {
				for (auto& w : this->m_Walls) {
					if (w.CheckHit(repos, pos)) {
						return true;
					}
				}
				return false;
			}

		private:
			void			DrawCommon(bool IsCalling) noexcept {
				this->m_ObjGround.DrawModel();
				for (auto& w : this->m_Walls) {
					w.Draw(IsCalling);
				}
			}
		public://
			//
			void			Init(void) noexcept {
				//�n�`
				MV1::Load("data/model/map/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				MV1::Load("data/model/map/col_box2D.mv1", &this->m_ObjGroundCol_Box2D);
				//��
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				//��
				this->m_b2world = std::make_shared<b2World>(b2Vec2(0.0f, 0.0f)); // ���̂�ێ�����уV�~�����[�g���郏�[���h�I�u�W�F�N�g���\�z
				MV1_REF_POLYGONLIST p = MV1GetReferenceMesh(this->m_ObjGroundCol_Box2D.get(), 0, FALSE);
				for (int i = 0; i < p.PolygonNum; i++) {
					this->m_b2wallParts.resize(this->m_b2wallParts.size() + 1);
					this->m_b2wallParts.back().second[0] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
					this->m_b2wallParts.back().second[1] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
					if (b2DistanceSquared(b2Vec2(this->m_b2wallParts.back().second[0].x(), this->m_b2wallParts.back().second[0].z()), b2Vec2(this->m_b2wallParts.back().second[1].x(), this->m_b2wallParts.back().second[1].z())) <= 0.005f * 0.005f) {
						this->m_b2wallParts.pop_back();
					}
					this->m_b2wallParts.resize(this->m_b2wallParts.size() + 1);
					this->m_b2wallParts.back().second[0] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
					this->m_b2wallParts.back().second[1] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
					if (b2DistanceSquared(b2Vec2(this->m_b2wallParts.back().second[0].x(), this->m_b2wallParts.back().second[0].z()), b2Vec2(this->m_b2wallParts.back().second[1].x(), this->m_b2wallParts.back().second[1].z())) <= 0.005f * 0.005f) {
						this->m_b2wallParts.pop_back();
					}
					this->m_b2wallParts.resize(this->m_b2wallParts.size() + 1);
					this->m_b2wallParts.back().second[0] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
					this->m_b2wallParts.back().second[1] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
					if (b2DistanceSquared(b2Vec2(this->m_b2wallParts.back().second[0].x(), this->m_b2wallParts.back().second[0].z()), b2Vec2(this->m_b2wallParts.back().second[1].x(), this->m_b2wallParts.back().second[1].z())) <= 0.005f * 0.005f) {
						this->m_b2wallParts.pop_back();
					}
				}
				MV1TerminateReferenceMesh(this->m_ObjGroundCol_Box2D.get(), 0, FALSE);
				for (auto& w : this->m_b2wallParts) {
					std::array<b2Vec2, 2> vs;								//
					vs[0].Set(w.second[0].x(), w.second[0].z());			//
					vs[1].Set(w.second[1].x(), w.second[1].z());			//
					b2ChainShape chain;										// This a chain shape with isolated vertices
					chain.CreateChain(&vs[0], 2);							//
					b2FixtureDef fixtureDef;								//���I�{�f�B�t�B�N�X�`�����`���܂�
					fixtureDef.shape = &chain;								//
					fixtureDef.density = 1.0f;								//�{�b�N�X���x���[���ȊO�ɐݒ肷��ƁA���I�ɂȂ�܂�
					fixtureDef.friction = 0.3f;								//�f�t�H���g�̖��C���I�[�o�[���C�h���܂�
					b2BodyDef bodyDef;										//�_�C�i�~�b�N�{�f�B���`���܂��B���̈ʒu��ݒ肵�A�{�f�B�t�@�N�g�����Ăяo���܂�
					bodyDef.type = b2_staticBody;							//
					bodyDef.position.Set(0, 0);								//
					bodyDef.angle = 0.f;									//
					w.first.Set(this->m_b2world->CreateBody(&bodyDef), &chain);	//
				}
				//
				this->m_grass.Init(&this->m_ObjGroundCol);
				
				MV1::Load("data/model/wall/model.mqoz", &this->m_objWall);
				this->m_Walls.resize(20);
				int i = 0;
				for (auto& w : this->m_Walls) {
					w.Init(this->m_objWall, VECTOR_ref::vget(12.5f*(i % 10), 12.5f*6.f, -0.1f*12.5f*(i / 10)), deg2rad(180.f), 3.f); i++;
				}
			}
			//
			void			Execute(void) noexcept {
				for (auto& w : this->m_Walls) {
					w.Execute();
				}
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_NearFar(void) noexcept {
				DrawCommon(false);
			}
			void			Shadow_Draw(void) noexcept {
				DrawCommon(false);
			}
			void			Draw(void) noexcept {
				DrawCommon(true);
				this->m_grass.Draw();
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjGroundCol.Dispose();
				this->m_ObjGroundCol_Box2D.Dispose();
				this->m_grass.Dispose();
				for (auto& w : this->m_b2wallParts) {
					w.first.Dispose();
					w.second[0].clear();
					w.second[1].clear();
				}
				this->m_b2wallParts.clear();
				this->m_b2world.reset();
			}
		};
	};
};
