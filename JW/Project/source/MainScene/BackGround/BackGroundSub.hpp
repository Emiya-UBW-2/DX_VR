#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	class MazeControl {
		enum class MAZETYPE :int {
			WALL,
			PATH,
		};
	private:
		int m_Width{ 1 };
		int m_Height{ 1 };
		std::vector<std::vector<MAZETYPE>> m_Maze;
	private:
		//穴掘り
		void dig(int x, int y) {
			//指定した部分を掘っておく
			m_Maze[x][y] = MAZETYPE::PATH;
			// どの方向を掘ろうとしたかを覚えておく変数
			int ok = 0;
			// 全方向試すまでループ
			while (ok != 0b1111) {
				int Dir = GetRand(3) % 4;
				ok |= (1 << Dir);
				float rad = deg2rad(Dir * 90);
				int next_x = x + (int)(sin(rad)*2.f);//0 2  0 -2
				int next_y = y + (int)(cos(rad)*2.f);//2 0 -2  0
				if ((0 <= next_x && next_x < m_Width) && (0 <= next_y && next_y < m_Height)) {
					if (m_Maze[next_x][next_y] == MAZETYPE::WALL) {
						m_Maze[(next_x + x) / 2][(next_y + y) / 2] = MAZETYPE::PATH;
						//その場から次の穴掘り
						dig(next_x, next_y);
					}
				}
			}
		}
	public:
		//該当座標が通路かどうか
		const auto PosIsPath(int x, int y) {
			if ((0 <= x && x < m_Width) && (0 <= y && y < m_Height)) {
				return m_Maze[x][y] == MAZETYPE::PATH;
			}
			else {
				return false;
			}
		}
	public:
		// 迷路を作成する
		void createMaze(int w, int h) {
			m_Width = w;
			m_Height = h;

			m_Maze.resize(m_Width);
			for (auto& mx : m_Maze) {
				mx.resize(m_Height);
				for (auto& m : mx) {
					m = MAZETYPE::WALL; // 全マス壁にする
				}
			}
			// 開始点をランダム（奇数座標）に決定する
			dig(std::clamp(2 * GetRand(m_Width / 2) + 1, 0, m_Width - 1), std::clamp(2 * GetRand(m_Height / 2) + 1, 0, m_Height - 1));
			//追加で穴あけ
			for (int y = 1; y < m_Height - 1; y++) {
				for (int x = 1; x < m_Width - 1; x++) {
					bool isHit = false;
					if ((y % 4) == 0) {
						if ((x % 4) == 0) { isHit = true; }
					}
					if (!isHit) { continue; }
					m_Maze[x][y] = MAZETYPE::PATH;
				}
			}
		}
		void Reset() {
			for (auto& mx : m_Maze) {
				mx.clear();
			}
			m_Maze.clear();
			m_Width = 1;
			m_Height = 1;
		}
	};

	class Builds {
		int						m_mesh{ -1 };
		MV1						m_Col;
		MATRIX_ref				m_mat;
		VECTOR_ref				m_MinPos;
		VECTOR_ref				m_MaxPos;

	public:
		const auto&		GetMeshSel(void) const noexcept { return m_mesh; }
		const auto&		GetCol(void) const noexcept { return this->m_Col; }
		const auto&		GetMinPos(void) const noexcept { return m_MinPos; }
		const auto&		GetMaxPos(void) const noexcept { return m_MaxPos; }
		const auto&		GetMatrix(void) const noexcept { return m_mat; }
		const auto		GetColLine(const VECTOR_ref& repos, const VECTOR_ref& pos) const noexcept { return this->m_Col.CollCheck_Line(repos, pos, m_mesh); }
	public:
		void		Set(const MV1& ColModel, int frame) {
			this->m_Col = ColModel.Duplicate();
			this->m_Col.SetupCollInfo(1, 1, 1, m_mesh);
			this->m_mesh = frame;
		}
		void		ChangeSel(int frame) {
			if (this->m_Col.IsActive()) {
				MV1TerminateCollInfo(this->m_Col.get(), m_mesh);
				this->m_Col.SetupCollInfo(1, 1, 1, m_mesh);
				this->m_mesh = frame;
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
			this->m_MinPos = this->m_mat.pos() + this->m_Col.mesh_minpos(this->m_mesh) + VECTOR_ref::up()*(-1.f*Scale_Rate);
			this->m_MaxPos = this->m_mat.pos() + this->m_Col.mesh_maxpos(this->m_mesh) + VECTOR_ref::up()*(1.f*Scale_Rate);
		}
	};
	class BuildControl {
		MV1							m_ObjBuildBase;
		MV1							m_ColBuildBase;
		std::vector<Builds>			m_ObjBuilds;
		std::vector<Model_Instance>	m_Inst;
		std::vector<std::pair<MV1, int>>	m_Obj;

		MazeControl					m_MazeControl;


		GraphHandle					m_MapGraph;
		int m_MapGraphXSize{ 0 }, m_MapGraphYSize{ 0 };

		MV1											m_ObjLightBase;
		GraphHandle									Light_Graph;
		struct Lights {
			bool		m_isHit{ false };

			float		m_SlingPower{ 0.1f };
			float		m_EraseTimer{ 3.f };

			Pendulum2D	m_SlingZrad;

			float		m_Yadd{ 0.f };
			VECTOR_ref	m_Pos;
			float		m_length{ 0.f };
			MV1			m_Obj;
		};
		std::vector<Lights>							m_LightPoiont;
		std::array<int, 2>							m_LightHandle;

		Camera3DInfo								m_CamPos;
	public:
		void	SetCamPos(const Camera3DInfo& value) noexcept { m_CamPos = value; }
		const auto&		GetNearestLight(int No) noexcept { return this->m_LightPoiont[No].m_Pos; }
	public:
		const auto&		GetBuildCol(void) const noexcept { return this->m_ObjBuilds; }
		const auto&		GetBuildCol(void) noexcept { return this->m_ObjBuilds; }
		const auto&		GetMapGraph(void) noexcept { return this->m_MapGraph; }
		const auto&		GetMapGraphXSize(void) noexcept { return this->m_MapGraphXSize; }
		const auto&		GetMapGraphYSize(void) noexcept { return this->m_MapGraphYSize; }

		bool			HitLightCheck(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) noexcept {
			for (int i = 0; i < this->m_LightPoiont.size(); i++) {
				auto& o = this->m_LightPoiont[i];

				if (GetMinLenSegmentToPoint(StartPos, *pEndPos, o.m_Pos) <= 2.f*Scale_Rate) {
					o.m_SlingPower = GetRandf(3.f);
				}
				if (GetMinLenSegmentToPoint(StartPos, *pEndPos, o.m_Pos) <= 0.3f*Scale_Rate) {
					o.m_isHit = true;
					o.m_EraseTimer = 3.f;
					*pEndPos = o.m_Pos;
					return true;
				}
			}
			return false;
		}
	public:
		void			Load(void) noexcept {
			MV1::Load("data/model/map_parts/model.mv1", &this->m_ObjBuildBase);
			MV1::Load("data/model/map_parts/col.mv1", &this->m_ColBuildBase);
			MV1::Load("data/model/map_parts/light.mv1", &this->m_ObjLightBase);
		}
		void			Init(const MV1* MapCol) noexcept {
			int Size = 19;
			float tileSize = 30.f;
			/* 迷路の作成 */
			m_MazeControl.createMaze(Size, Size);
			{
				m_MapGraph = GraphHandle::Make(Size * (int)tileSize, Size * (int)tileSize, true);
				m_MapGraph.SetDraw_Screen();
				{
					for (int y = 0; y < Size; y++) {
						for (int x = 0; x < Size; x++) {
							if (m_MazeControl.PosIsPath(x, y)) {
								int count = 0;

								bool XP = m_MazeControl.PosIsPath(x + 1, y);
								bool XM = m_MazeControl.PosIsPath(x - 1, y);
								bool ZP = m_MazeControl.PosIsPath(x, y + 1);
								bool ZM = m_MazeControl.PosIsPath(x, y - 1);

								if (XP) { count++; }
								if (XM) { count++; }
								if (ZP) { count++; }
								if (ZM) { count++; }

								int color = 255;// *count / 4;

								int Wall = 3;

								DrawBox(
									x * 30 + (!XM ? Wall : 0),
									y * 30 + (!ZM ? Wall : 0),
									(x + 1) * 30 - (!XP ? Wall : 0),
									(y + 1) * 30 - (!ZP ? Wall : 0),
									GetColor(color, color, color), TRUE);
							}
						}
					}
				}
				m_MapGraph.GetSize(&m_MapGraphXSize, &m_MapGraphYSize);
			}
			{
				int OneSize = 0;
				for (int y = 0; y < Size; y++) {
					for (int x = 0; x < Size; x++) {
						if (m_MazeControl.PosIsPath(x, y)) {
							OneSize++;
						}
					}
				}
				//
				this->m_Obj.resize(OneSize);
				this->m_ObjBuilds.resize(OneSize);
				this->m_Inst.resize(5);
				{
					VECTOR_ref BasePos;
					float deg = 0.f;
					int loop = 0;
					for (int y = 0; y < Size; y++) {
						for (int x = 0; x < Size; x++) {
							if (m_MazeControl.PosIsPath(x, y)) {
								int count = 0;

								bool XP = m_MazeControl.PosIsPath(x + 1, y);
								bool XM = m_MazeControl.PosIsPath(x - 1, y);
								bool ZP = m_MazeControl.PosIsPath(x, y + 1);
								bool ZM = m_MazeControl.PosIsPath(x, y - 1);

								if (XP) { count++; }
								if (XM) { count++; }
								if (ZP) { count++; }
								if (ZM) { count++; }

								int ID = 0;
								switch (count) {
								case 1:
									ID = 0;
									if (ZM) { deg = 0.f; }
									if (XM) { deg = 90.f; }
									if (ZP) { deg = 180.f; }
									if (XP) { deg = 270.f; }
									break;
								case 2:
									if (ZP && ZM) { ID = 1; deg = 0.f; }
									if (XP && XM) { ID = 1; deg = 90.f; }

									if (XP && ZP) { ID = 4; deg = 270.f; }
									if (XM && ZP) { ID = 4; deg = 180.f; }
									if (XM && ZM) { ID = 4; deg = 90.f; }
									if (XP && ZM) { ID = 4; deg = 0.f; }
									break;
								case 3:
									ID = 2;
									if (ZP && XP && ZM) { deg = 0.f; }
									if (ZP && XM && ZM) { deg = 180.f; }
									if (XP && ZP && XM) { deg = 270.f; }
									if (XP && ZM && XM) { deg = 90.f; }
									break;
								case 4:
									ID = 3;
									break;
								default:
									break;
								}

								BasePos.Set(tileSize * (float)x, 0.f, tileSize * (float)y);
								BasePos -= VECTOR_ref::vget(tileSize * (float)Size / 2.f, 0.f, tileSize * (float)Size / 2.f);

								this->m_ObjBuilds[loop].Set(this->m_ColBuildBase, ID);
								this->m_ObjBuilds[loop].SetPosition(*MapCol, BasePos, deg2rad(deg), false);

								this->m_Obj[loop].first = this->m_ObjBuildBase.Duplicate();
								this->m_Obj[loop].first.SetMatrix(MATRIX_ref::RotY(deg2rad(deg))*MATRIX_ref::Mtrans(BasePos));
								this->m_Obj[loop].second = ID;

								loop++;
							}
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
			}
			MATERIALPARAM Material_t;
			Material_t.Diffuse = GetLightDifColor();
			Material_t.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
			Material_t.Ambient = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);;
			Material_t.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
			Material_t.Power = 20.0f;
			SetMaterialParam(Material_t);
			{
				Light_Graph = GraphHandle::Load("data/Picture/light.png");
				int OneSize = 0;
				for (int y = 0; y < Size; y++) {
					for (int x = 0; x < Size; x++) {
						if (m_MazeControl.PosIsPath(x, y)) {
							bool isHit = false;
							if ((y % 4) == 0) {
								if ((x % 3) == 0) { isHit = true; }
							}
							if ((y % 4) == 2) {
								if ((x % 3) == 2) { isHit = true; }
							}
							if ((y % 2) == 1) {
								if ((x % 3) == 1) { isHit = true; }
							}
							if (!isHit) { continue; }

							OneSize++;
						}
					}
				}
				this->m_LightPoiont.resize(OneSize);
				VECTOR_ref BasePos;
				int loop = 0;
				for (int y = 0; y < Size; y++) {
					for (int x = 0; x < Size; x++) {
						if (m_MazeControl.PosIsPath(x, y)) {
							bool isHit = false;
							if ((y % 4) == 0) {
								if ((x % 3) == 0) { isHit = true; }
							}
							if ((y % 4) == 2) {
								if ((x % 3) == 2) { isHit = true; }
							}
							if ((y % 2) == 1) {
								if ((x % 3) == 1) { isHit = true; }
							}
							if (!isHit) { continue; }

							BasePos.Set(tileSize * (float)x, 0.f, tileSize * (float)y);
							BasePos -= VECTOR_ref::vget(tileSize * (float)(Size) / 2.f, 0.f, tileSize * (float)(Size) / 2.f);
							BasePos.yadd(2.65f*Scale_Rate);

							this->m_LightPoiont[loop].m_Pos = BasePos;
							this->m_LightPoiont[loop].m_Obj = this->m_ObjLightBase.Duplicate();
							this->m_LightPoiont[loop].m_SlingZrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(0));
							loop++;
						}
					}
				}
				for (int i = 0; i < 2; i++) {
					m_LightHandle[i] = CreateSpotLightHandle(
						this->m_LightPoiont[i].m_Pos.get(),
						VGet(0.0f, -1.0f, 0.0f),
						DX_PI_F / 4.0f,
						DX_PI_F / 6.0f,
						5.0f*Scale_Rate,
						0.0f,
						0.006f,
						0.0f);
					SetLightDifColorHandle(m_LightHandle[i], GetColorF(1.f, 1.f, 1.f, 1.f));
					SetLightSpcColorHandle(m_LightHandle[i], GetColorF(0.01f, 0.01f, 0.01f, 0.f));
					SetLightAmbColorHandle(m_LightHandle[i], GetColorF(0.1f, 0.1f, 0.1f, 1.f));
				}
				ChangeLightTypeSpot(this->m_LightPoiont[3].m_Pos.get(),
					VGet(0.0f, -1.0f, 0.0f),
					DX_PI_F / 4.0f,
					DX_PI_F / 6.0f,
					5.0f*Scale_Rate,
					0.0f,
					0.006f,
					0.0f);
				SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
				SetLightSpcColor(GetColorF(0.01f, 0.01f, 0.01f, 0.f));
				SetLightAmbColor(GetColorF(0.1f, 0.1f, 0.1f, 1.f));
			}
		}
		void			Execute(void) noexcept {
			for (int i = 0; i < this->m_LightPoiont.size(); i++) {
				auto& o = this->m_LightPoiont[i];

				o.m_SlingZrad.Execute();

				o.m_SlingZrad.AddRad(1.f / FPS * o.m_SlingPower);
				o.m_SlingPower = 0.f;

				if (o.m_isHit) {
					o.m_Pos.yadd(o.m_Yadd);
					o.m_Yadd += M_GR / (FPS * FPS);
					o.m_EraseTimer -= 1.f / FPS;
				}


				o.m_SlingZrad.GetRad();

				auto Vec = (o.m_Pos - m_CamPos.GetCamPos());
				o.m_Obj.SetMatrix(MATRIX_ref::RotAxis(Vec.Norm(), o.m_SlingZrad.GetRad()) * MATRIX_ref::Mtrans(o.m_Pos));
				o.m_length = Vec.Length();

				if (o.m_EraseTimer <= 0.f) {
					std::swap(o, this->m_LightPoiont.back());
					this->m_LightPoiont.pop_back();
					i--;
				}
			}
			std::sort(this->m_LightPoiont.begin(), this->m_LightPoiont.end(), [&](const Lights& a, const Lights& b) { return a.m_length < b.m_length && !a.m_isHit; });

			int count = 0;
			for (auto& o : this->m_LightPoiont) {
				auto vec1 = (m_CamPos.GetCamVec() - m_CamPos.GetCamPos());
				auto vec2 = (o.m_Pos - m_CamPos.GetCamPos()); vec2.y(0.f);
				float dot = vec1.Norm().dot(vec2.Norm());
				if (dot > std::sin(deg2rad(-60))) {
					auto Vec = MATRIX_ref::Vtrans(VECTOR_ref::up()*-1.f, MATRIX_ref::RotAxis((o.m_Pos - m_CamPos.GetCamPos()).Norm(), o.m_SlingZrad.GetRad()));
					if (count <= 1) {
						SetLightPositionHandle(m_LightHandle[count], o.m_Pos.get());
						SetLightDirectionHandle(m_LightHandle[count], Vec.get());
					}
					else {
						SetLightPosition(o.m_Pos.get());
						SetLightDirection(Vec.get());
					}
					count++;
					if (count >= 2) { break; }
				}
			}
		}
		void			Draw() noexcept {
			int fog_enable = 0;
			int fog_mode = 0;
			int fog_r = 0, fog_g = 0, fog_b = 0;
			float fog_start = 0.f, fog_end = 0.f;
			float fog_density = 0.f;

			fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
			fog_mode = GetFogMode();														// フォグモードを取得する
			GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
			GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
			fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )

			SetFogEnable(TRUE);
			SetFogMode(DX_FOGMODE_EXP2);
			SetFogStartEnd(Scale_Rate*6.f, Scale_Rate*10.f);
			SetFogColor(26, 29, 20);
			SetFogDensity(0.01f);
#if FALSE
			for (auto& b : this->m_Inst) {
				b.Draw();
			}
#else
			for (auto& b : this->m_Obj) {
				auto Pos = b.first.GetMatrix().pos();
				auto Vec = (Pos - m_CamPos.GetCamPos()); Vec.y(0.f);
				auto Len = Vec.Length();
				if (Len <= 15.f*Scale_Rate) {
					if (CheckCameraViewClip_Box(
						(Pos + VECTOR_ref::vget(-1.5f*Scale_Rate, -0.5f*Scale_Rate, -1.5f*Scale_Rate)).get(),
						(Pos + VECTOR_ref::vget(1.5f*Scale_Rate, 3.f*Scale_Rate, 1.5f*Scale_Rate)).get()) == FALSE
						) {
						b.first.DrawMesh(b.second);
					}
				}
			}
#endif
			for (auto& b : this->m_LightPoiont) {
				auto Pos = b.m_Pos;
				auto Vec = (Pos - m_CamPos.GetCamPos()); Vec.y(0.f);
				auto Len = Vec.Length();
				if (Len <= 15.f*Scale_Rate) {
					if (CheckCameraViewClip_Box(
						(Pos + VECTOR_ref::vget(-1.5f*Scale_Rate, -0.5f*Scale_Rate, -1.5f*Scale_Rate)).get(),
						(Pos + VECTOR_ref::vget(1.5f*Scale_Rate, 3.f*Scale_Rate, 1.5f*Scale_Rate)).get()) == FALSE
						) {
						b.m_Obj.DrawModel();
					}
				}
			}

			SetFogEnable(fog_enable);
			SetFogMode(fog_mode);
			SetFogColor(fog_r, fog_g, fog_b);
			SetFogStartEnd(fog_start, fog_end);
			SetFogDensity(fog_density);
			//
			SetUseLighting(FALSE);
			float rad = -(m_CamPos.GetCamVec() - m_CamPos.GetCamPos()).cross(VECTOR_ref::up()).dot(m_CamPos.GetCamUp());
			int max = (int)this->m_LightPoiont.size();
			for (int i = max - 1; i >= 0; i--) {
				auto& o = this->m_LightPoiont[i];
				if (!o.m_isHit) {
					auto Pos = o.m_Pos;
					auto Vec = (Pos - m_CamPos.GetCamPos()); Vec.y(0.f);
					auto Len = Vec.Length();
					if (Len <= 15.f*Scale_Rate) {
						if (CheckCameraViewClip_Box(
							(Pos + VECTOR_ref::vget(-1.5f*Scale_Rate, -0.5f*Scale_Rate, -1.5f*Scale_Rate)).get(),
							(Pos + VECTOR_ref::vget(1.5f*Scale_Rate, 3.f*Scale_Rate, 1.5f*Scale_Rate)).get()) == FALSE
							) {
							float per = 0.5f*std::clamp((Len - 50.f) / 30.f, 0.f, 1.f);
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
							DrawBillboard3D(o.m_Pos.get(), 0.5f, 1.f, 2.2f*Scale_Rate, rad + o.m_SlingZrad.GetRad(), Light_Graph.get(), TRUE);
						}
					}
				}
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			SetUseLighting(TRUE);
		}
		void			Dispose(void) noexcept {
			this->m_ObjBuildBase.Dispose();
			this->m_ColBuildBase.Dispose();
			for (auto& b : this->m_Inst) {
				b.Dispose();
			}
			this->m_Inst.clear();

			m_MazeControl.Reset();

			for (auto& b : this->m_LightPoiont) {
				b.m_Obj.Dispose();
			}
			m_LightPoiont.clear();
			Light_Graph.Dispose();
		}

	};
	//kusa
	class Grass {
		class grass_t;
		struct GrassPos {
			int X_PosMin = 0;
			int Y_PosMin = 0;
			int X_PosMax = 0;
			int Y_PosMax = 0;
		};
	private:
		class Impl;
		Impl* m_Impl{ nullptr };
	public:
		Grass();
		~Grass();
	public:
		void Init(const MV1* MapCol, int softimage, const char* Graph, const char* Model, float MAPX, float MAPZ, int count);
		void Dispose(void) noexcept;
		void DrawShadow(void) noexcept {}
		void Draw(void) noexcept;
	};
	//
	struct treePats {
		MV1 obj, obj_far;
		MATRIX_ref mat;
		VECTOR_ref pos;
	};
	class TreeControl {
		MV1 tree_model, tree_far;

		std::vector<treePats>		tree;
	public:
		void	Load() noexcept {
			MV1::Load("data/model/tree/model.mv1", &tree_model, true); //木
			MV1::Load("data/model/tree/model2.mv1", &tree_far, true); //木
		}
		void	Init(const MV1* MapCol) noexcept {
			tree.resize(1300);
			for (auto& t : tree) {
				auto scale = 15.f / 10.f*Scale_Rate;
				t.mat = MATRIX_ref::GetScale(VECTOR_ref::vget(scale, scale, scale));
				t.pos = VECTOR_ref::vget(
					GetRandf(150)*10.f*Scale_Rate,
					0.f,
					GetRandf(150)*10.f*Scale_Rate);


				auto res = MapCol->CollCheck_Line(t.pos + VECTOR_ref::up()*300.f*Scale_Rate, t.pos + VECTOR_ref::up()*-300.f*Scale_Rate);
				if (res.HitFlag == TRUE) {
					t.pos = res.HitPosition;
				}
				t.obj = tree_model.Duplicate();
				t.obj_far = tree_far.Duplicate();
				t.obj_far.material_AlphaTestAll(true, DX_CMP_GREATER, 128);

				t.obj.SetMatrix(t.mat * MATRIX_ref::Mtrans(t.pos));
			}
		}
		void	Execute(void) noexcept {
			//木セット
			for (auto& t : tree) {
				t.obj.SetMatrix(t.mat * MATRIX_ref::Mtrans(t.pos));
			}
		}
		void	Dispose(void) noexcept {
			tree_model.Dispose(); //木
			tree_far.Dispose(); //木
			for (auto&t : tree) {
				t.obj.Dispose();
				t.obj_far.Dispose();
			}
			tree.clear();
		}
		void	DrawFar() noexcept {
			for (auto& t : tree) {
				t.obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
				t.obj.DrawModel();
			}
		}
		void	Draw(bool isSetFog) noexcept {
			int fog_enable = 0;
			int fog_mode = 0;
			int fog_r = 0, fog_g = 0, fog_b = 0;
			float fog_start = 0.f, fog_end = 0.f;
			float fog_density = 0.f;
			if (isSetFog) {
				fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
				fog_mode = GetFogMode();														// フォグモードを取得する
				GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
				GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
				fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )

				SetFogEnable(TRUE);
				SetFogColor(4, 16, 0);
				SetFogStartEnd(Scale_Rate*5.f, Scale_Rate*50.f);
			}
			auto Farlimit = 360.f*Scale_Rate;
			auto Farlimit2 = 240.f*Scale_Rate;
			for (auto& t : tree) {
				auto LengthtoCam = (t.pos - GetCameraPosition());

				if (CheckCameraViewClip_Box(
					(t.pos + VECTOR_ref::vget(-20, 0, -20)*Scale_Rate).get(),
					(t.pos + VECTOR_ref::vget(20, 20, 20)*Scale_Rate).get()) == FALSE
					) {
					if (LengthtoCam.Length() > Farlimit && isSetFog) {
						SetUseLighting(FALSE);
						LengthtoCam.y(0.f); LengthtoCam = LengthtoCam.Norm();
						float rad = std::atan2f(VECTOR_ref::front().cross(LengthtoCam).y(), VECTOR_ref::front().dot(LengthtoCam));
						t.obj_far.SetMatrix(MATRIX_ref::RotY(rad) * t.mat * MATRIX_ref::Mtrans(t.pos));
						t.obj_far.DrawModel();
						SetUseLighting(TRUE);
					}
					else {
						if (isSetFog) {
							t.obj.material_AlphaTestAll(true, DX_CMP_GREATER, (int)(128.f + 127.f*(Farlimit2 - LengthtoCam.Length()) / Farlimit2));
						}
						else {
							t.obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
						}
						t.obj.DrawModel();
					}
				}

			}
			if (isSetFog) {
				SetFogEnable(fog_enable);
				SetFogMode(fog_mode);
				SetFogColor(fog_r, fog_g, fog_b);
				SetFogStartEnd(fog_start, fog_end);
				SetFogDensity(fog_density);
			}
		}
	};
};
