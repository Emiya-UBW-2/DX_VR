#pragma once
#include	"../../Header.hpp"

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
				/*視界外か否かを判断*/
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
			const int grasss = 30 * 30;						/*grassの数*/
			std::array<grass_t, grassDiv>grass__;
			std::array<VECTOR_ref, grassDiv>grassPosMin;
			std::array<VECTOR_ref, grassDiv>grassPosMax;
			int grasss2 = 10 * 10;							/*grassの数*/
			std::array<grass_t, grassDiv>grass2__;
			int grasss3 = 4 * 4;							/*grassの数*/
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
			//y方向に操作する前提
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
					//ポジション決定
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

		class BackGroundClass {
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;
			MV1							m_ObjGroundCol_Box2D;
			std::shared_ptr<b2World>	m_b2world;
			std::vector<std::pair<b2Pats, std::array<VECTOR_ref, 2>>>	m_b2wallParts;	//壁をセット
			Grass						m_grass;
		public://getter
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
			auto&			GetBox2Dworld(void) noexcept { return this->m_b2world; }
		private:
			void			DrawCommon(void) noexcept {
				this->m_ObjGround.DrawModel();
			}
		public://
			//
			void			Init(void) noexcept {
				//地形
				MV1::Load("data/model/map/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				MV1::Load("data/model/map/col_box2D.mv1", &this->m_ObjGroundCol_Box2D);
				//空
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				//壁
				this->m_b2world = std::make_shared<b2World>(b2Vec2(0.0f, 0.0f)); // 剛体を保持およびシミュレートするワールドオブジェクトを構築
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
				for (auto& w : this->m_b2wallParts) {
					std::array<b2Vec2, 2> vs;								//
					vs[0].Set(w.second[0].x(), w.second[0].z());			//
					vs[1].Set(w.second[1].x(), w.second[1].z());			//
					b2ChainShape chain;										// This a chain shape with isolated vertices
					chain.CreateChain(&vs[0], 2);							//
					b2FixtureDef fixtureDef;								//動的ボディフィクスチャを定義します
					fixtureDef.shape = &chain;								//
					fixtureDef.density = 1.0f;								//ボックス密度をゼロ以外に設定すると、動的になります
					fixtureDef.friction = 0.3f;								//デフォルトの摩擦をオーバーライドします
					b2BodyDef bodyDef;										//ダイナミックボディを定義します。その位置を設定し、ボディファクトリを呼び出します
					bodyDef.type = b2_staticBody;							//
					bodyDef.position.Set(0, 0);								//
					bodyDef.angle = 0.f;									//
					w.first.Set(this->m_b2world->CreateBody(&bodyDef), &chain);	//
				}
				//
				this->m_grass.Init(&this->m_ObjGroundCol);
			}
			//
			void			Execute(void) noexcept {}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_NearFar(void) noexcept {
				DrawCommon();
			}
			void			Shadow_Draw(void) noexcept {
				DrawCommon();
			}
			void			Draw(void) noexcept {
				DrawCommon();
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