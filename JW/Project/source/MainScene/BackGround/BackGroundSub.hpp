#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	class BuildControl {
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
			//通路の総数を取得
			const auto GetPachCount() noexcept {
				int OneSize = 0;
				for (int y = 0; y < m_Height; y++) {
					for (int x = 0; x < m_Width; x++) {
						if (PosIsPath(x, y)) {
							OneSize++;
						}
					}
				}
				return OneSize;
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
						if (x == 1 || (x == m_Width - 1 - 1)) {
							isHit = true;
						}
						if (y == 1 || (y == m_Height - 1 - 1)) {
							isHit = true;
						}
						if (!isHit) { continue; }
						m_Maze[x][y] = MAZETYPE::PATH;
					}
				}
				//追加で穴あけ
				for (int y = 1; y < m_Height - 1; y++) {
					for (int x = 1; x < m_Width - 1; x++) {
						bool isHit = false;
						if ((y % 6) == 0) {
							if ((x % 6) == 0) { isHit = true; }
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
			MV1						m_Obj;
			MV1						m_Col;
			MATRIX_ref				m_mat;
			VECTOR_ref				m_MinPos;
			VECTOR_ref				m_MaxPos;

			bool checkDraw{ true };
		public:
			const auto&		GetMeshSel(void) const noexcept { return m_mesh; }
			const auto&		GetObj(void) const noexcept { return this->m_Obj; }
			const auto&		GetCol(void) const noexcept { return this->m_Col; }
			const auto&		GetMinPos(void) const noexcept { return m_MinPos; }
			const auto&		GetMaxPos(void) const noexcept { return m_MaxPos; }
			const auto&		GetMatrix(void) const noexcept { return m_mat; }

			const auto&		IsDraw(void) const noexcept { return checkDraw; }

			const auto		GetColLine(const VECTOR_ref& repos, const VECTOR_ref& pos) const noexcept { return this->m_Col.CollCheck_Line(repos, pos, m_mesh); }
		public:
			void		Set(const MV1& ObjModel, const MV1& ColModel, int frame) {
				this->m_Obj = ObjModel.Duplicate();
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
			void		SetPosition(VECTOR_ref pos, float rad) {
				this->m_mat = MATRIX_ref::RotY(rad)*MATRIX_ref::Mtrans(pos);
				this->m_Obj.SetMatrix(this->m_mat);
				this->m_Col.SetMatrix(this->m_mat);
				this->m_Col.RefreshCollInfo(this->m_mesh);
				this->m_MinPos = this->m_mat.pos() + this->m_Col.mesh_minpos(this->m_mesh);
				this->m_MaxPos = this->m_mat.pos() + this->m_Col.mesh_maxpos(this->m_mesh);
			}

			void		EnableChackCam() {
				this->checkDraw = true;
			}
			void		DisableChackCam() {
				this->checkDraw = false;
			}
		};
		struct Lights {
			bool		m_isHit{ false };

			float		m_SlingPower{ 0.1f };
			float		m_EraseTimer{ 3.f };

			Pendulum2D	m_SlingZrad;

			float		m_Yadd{ 0.f };
			VECTOR_ref	m_Pos;
			float		m_length{ 0.f };
			MV1			m_Obj;

			Builds*		m_NearPath{ nullptr };
		};
		class Garbage {
			static const int Blocks{ 5 };
			struct GarbageObjct
			{
				MV1 m_Obj;
				bool IsChanged{ false };
				float m_Rad{ 0.f };
				float m_PrevRad{ 0.f };
				VECTOR_ref m_Pos;
			};
		private:
			MV1									m_Base;
			std::array<GarbageObjct, Blocks * Blocks>		m_Obj;
			int xprev = -1000;
			int zprev = -1000;
		public:
			void		Set() {
				MV1::Load("data/model/map_gareki/model.mv1", &this->m_Base, DX_LOADMODEL_PHYSICS_DISABLE);
				for (auto& o : m_Obj) {
					o.m_Obj = m_Base.Duplicate();
				}
			}
			void		Execute(const VECTOR_ref& CamPos) {
				int x = (int)(CamPos.x() / 100.f*2.f);
				int z = (int)(CamPos.z() / 100.f*2.f);
				{
					int abx = abs(x);
					if (abx > 0) {
						if (abx % 2 == 1) {
							abx += 1;
						}
						x = (x > 0) ? abx : -abx;
					}
					x /= 2;
				}
				{
					int abz = abs(z);
					if (abz > 0) {
						if (abz % 2 == 1) {
							abz += 1;
						}
						z = (z > 0) ? abz : -abz;
					}
					z /= 2;
				}
				for (auto& o : m_Obj) {
					if (xprev != x || zprev != z) {
						int index = (int)(&o - &m_Obj.front());
						int xp = (x + (index % Blocks) - 2);
						int zp = (z + (index / Blocks) - 2);
						//前のフレームでの同じ場所を参照
						o.IsChanged = true;
						for (auto& o2 : m_Obj) {
							int index2 = (int)(&o2 - &m_Obj.front());
							int xp2 = (xprev + (index2 % Blocks) - 2);
							int zp2 = (zprev + (index2 / Blocks) - 2);
							if (xp == xp2 && zp == zp2) {
								o.m_Rad = o2.m_PrevRad;
								o.IsChanged = false;
								break;
							}
						}
						if (o.IsChanged) {
							o.IsChanged = false;
							o.m_Rad = deg2rad(GetRandf(180.f));
						}
						o.m_Pos = VECTOR_ref::vget((float)(xp * 100), 0.f, (float)(zp * 100));
					}
				}
				for (auto& o : m_Obj) {
					o.m_PrevRad = o.m_Rad;
				}
				xprev = x;
				zprev = z;
				for (auto& o : m_Obj) {
					o.m_Obj.SetMatrix(MATRIX_ref::RotY(o.m_Rad) * MATRIX_ref::Mtrans(o.m_Pos));
				}
			}
			void		ShadowDraw() {
				m_Obj.at(Blocks * Blocks / 2).m_Obj.DrawModel();
			}
			void		Draw() {
				for (auto& o : m_Obj) {
					o.m_Obj.DrawModel();
				}
			}
			void		Dispose() {
				m_Base.Dispose();
				for (auto& o : m_Obj) {
					o.m_Obj.Dispose();
				}
			}
		};
	private:
		const int Size = 29;
		const float tileSize = 30.f;
	private:
		GraphHandle					m_MapGraph;
		int m_MapGraphXSize{ 0 }, m_MapGraphYSize{ 0 };
	private:
		void	MakeMiniMap() noexcept;
		void	UpdateMiniMap() noexcept;
	public:
		const auto&		GetMapGraph(void) noexcept { return this->m_MapGraph; }
		const auto&		GetMapGraphXSize(void) noexcept { return this->m_MapGraphXSize; }
		const auto&		GetMapGraphYSize(void) noexcept { return this->m_MapGraphYSize; }
	private:
		MV1							m_ObjBuildBase;
		MV1							m_ColBuildBase;
		std::vector<Builds>			m_ObjBuilds;
		MazeControl					m_MazeControl;
		Garbage						m_Garbage;
	public:
		const auto&		GetBuildDatas(void) const noexcept { return this->m_ObjBuilds; }
	private:
		const auto		GetPos(int x, int y) const noexcept {
			VECTOR_ref BasePos;
			BasePos.Set(tileSize * (float)x, 0.f, tileSize * (float)y);
			BasePos -= VECTOR_ref::vget(tileSize * (float)Size / 2.f, 0.f, tileSize * (float)Size / 2.f);
			return BasePos;
		}
	private:
		MV1							m_ObjLightBase;
		GraphHandle					Light_Graph;
		std::vector<Lights>			m_LightPoiont;
		std::array<int, 2>			m_LightHandle{ -1,-1 };
	private:
		void	InitLight() noexcept;
		void	UpdateLight() noexcept;
		void	DrawLight() noexcept;
		void	DisposeLight() noexcept;
	public:
		const auto&		GetNearestLight(int No) const noexcept { return this->m_LightPoiont[No].m_Pos; }
	public:
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
		void			Load(void) noexcept;
		void			Init() noexcept;
		void			Execute(void) noexcept;
		void			ShadowDraw() noexcept;
		void			Draw() noexcept;
		void			Dispose(void) noexcept;
	};
};
