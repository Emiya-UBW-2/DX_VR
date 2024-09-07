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
			int m_Width{1};
			int m_Height{1};
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
					int next_x = x + static_cast<int>(sin(rad)*2.f);//0 2 0 -2
					int next_y = y + static_cast<int>(cos(rad)*2.f);//2 0 -2 0
					if ((0 <= next_x && next_x < m_Width) && (0 <= next_y && next_y < m_Height)) {
						if (m_Maze[next_x][next_y] == MAZETYPE::WALL) {
							m_Maze[static_cast<size_t>((next_x + x) / 2)][static_cast<size_t>((next_y + y) / 2)] = MAZETYPE::PATH;
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
			int						m_mesh{-1};
			MV1						m_Obj;
			MV1						m_Col;
			Matrix4x4DX				m_mat;
			Vector3DX				m_MinPos;
			Vector3DX				m_MaxPos;

			bool checkDraw{true};

			int							MyIndex{0};
			int						LinkPolyIndex[4]{-1,-1,-1,-1};
		public:
			const int	GetIndex() const noexcept { return MyIndex; }
			const int	GetLinkPolyIndex(int ID) const noexcept { return LinkPolyIndex[ID]; }
			void		SetLink(int tris, int index) { LinkPolyIndex[tris] = index; }
		public:
			const auto&		GetMeshSel(void) const noexcept { return m_mesh; }
			const auto&		GetObj(void) const noexcept { return this->m_Obj; }
			const auto&		GetCol(void) const noexcept { return this->m_Col; }
			const auto&		GetMinPos(void) const noexcept { return m_MinPos; }
			const auto&		GetMaxPos(void) const noexcept { return m_MaxPos; }
			const auto&		GetMatrix(void) const noexcept { return m_mat; }
			const auto&		IsDraw(void) const noexcept { return checkDraw; }

			const auto		GetColLine(const Vector3DX& repos, const Vector3DX& pos) const noexcept { return this->m_Col.CollCheck_Line(repos, pos, m_mesh); }
		public:
			void		Set(int index, const MV1& ObjModel, const MV1& ColModel, int frame) {
				this->MyIndex = index;
				this->m_Obj = ObjModel.Duplicate();
				this->m_Col = ColModel.Duplicate();
				this->m_Col.SetupCollInfo(1, 1, 1, m_mesh);
				this->m_mesh = frame;
				for (int i = 0; i < 4; i++) {
					LinkPolyIndex[i] = -1;
				}
			}
			void		ChangeSel(int frame) {
				if (this->m_Col.IsActive()) {
					MV1TerminateCollInfo(this->m_Col.GetHandle(), m_mesh);
					this->m_Col.SetupCollInfo(1, 1, 1, m_mesh);
					this->m_mesh = frame;
				}
			}
			void		SetPosition(Vector3DX pos, float rad) {
				this->m_mat = Matrix4x4DX::RotAxis(Vector3DX::up(), rad)*Matrix4x4DX::Mtrans(pos);
				this->m_Obj.SetMatrix(this->m_mat);
				this->m_Col.SetMatrix(this->m_mat);
				this->m_Col.RefreshCollInfo(this->m_mesh);
				this->m_MinPos = this->m_mat.pos() + this->m_Col.GetMeshMinPosition(this->m_mesh);
				this->m_MaxPos = this->m_mat.pos() + this->m_Col.GetMeshMaxPosition(this->m_mesh);
			}

			void		EnableChackCam() {
				this->checkDraw = true;
			}
			void		DisableChackCam() {
				this->checkDraw = false;
			}
		};
		class LightControl {
		private:
			struct Lights {
				bool		m_isHit{false};

				float		m_SlingPower{0.1f};
				float		m_EraseTimer{3.f};

				Pendulum2D	m_SlingZrad;

				float		m_Yadd{0.f};
				Vector3DX	m_Pos;
				float		m_length{0.f};
				MV1			m_Obj;

				Builds*		m_NearPath{nullptr};
			};
		private:
			GraphHandle					Light_Graph;
			MV1							m_ObjLightBase;
			std::vector<Lights>			m_LightPoiont;
			std::array<int, 2>			m_LightHandle{-1,-1};
		public:
			const auto&		GetNearestLight(int No) const noexcept { return this->m_LightPoiont[No].m_Pos; }
		public:
			bool			HitLightCheck(const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept {
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
			void			Set(int No, const Vector3DX& BasePos, Builds* ptr) {
				this->m_LightPoiont[No].m_Pos = BasePos;
				this->m_LightPoiont[No].m_Obj = this->m_ObjLightBase.Duplicate();
				this->m_LightPoiont[No].m_SlingZrad.Init(0.05f*Scale_Rate, 3.f, deg2rad(0));
				this->m_LightPoiont[No].m_NearPath = ptr;
			}
		public:
			void			Load() {
				MV1::Load("data/model/map_parts/light.mv1", &this->m_ObjLightBase, DX_LOADMODEL_PHYSICS_DISABLE);
				Light_Graph = GraphHandle::Load("data/Picture/light.png");
			}
			void			Init(int count) {
				this->m_LightPoiont.resize(count);

				for (int i = 0; i < 2; i++) {
					m_LightHandle[i] = CreateSpotLightHandle(
						GetNearestLight(i).get(),
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
			}

			void			Update() {
				auto* DrawParts = DXDraw::Instance();
				for (int i = 0; i < this->m_LightPoiont.size(); i++) {
					auto& o = this->m_LightPoiont[i];

					o.m_SlingZrad.Update();

					o.m_SlingZrad.AddRad(1.f / DrawParts->GetFps() * o.m_SlingPower);
					o.m_SlingPower = 0.f;

					if (o.m_isHit) {
						o.m_Pos.y += (o.m_Yadd);
						o.m_Yadd += M_GR / (DrawParts->GetFps() * DrawParts->GetFps());
						o.m_EraseTimer -= 1.f / DrawParts->GetFps();
					}


					o.m_SlingZrad.GetRad();

					auto Vec = (o.m_Pos - DrawParts->GetMainCamera().GetCamPos());
					o.m_Obj.SetMatrix(Matrix4x4DX::RotAxis(Vec.normalized(), o.m_SlingZrad.GetRad()) * Matrix4x4DX::Mtrans(o.m_Pos));
					o.m_length = Vec.magnitude();

					if (o.m_EraseTimer <= 0.f) {
						std::swap(o, this->m_LightPoiont.back());
						this->m_LightPoiont.pop_back();
						i--;
					}
				}
				std::sort(this->m_LightPoiont.begin(), this->m_LightPoiont.end(), [&](const Lights& a, const Lights& b) {			return a.m_length < b.m_length && a.m_NearPath->IsDraw() && !a.m_isHit; 		});
			}
			void			SetLight() {
#if HIGH_FPS_ROM
				SetLightEnable(TRUE);
				return;
#endif
				auto* DrawParts = DXDraw::Instance();
				for (int i = 0; i < 2; i++) {
					SetLightEnableHandle(m_LightHandle[i], FALSE);
				}
				SetLightEnable(FALSE);

				int count = 0;
				for (auto& b : this->m_LightPoiont) {
					if (b.m_NearPath->IsDraw()) {
						auto Pos = b.m_Pos;

						auto vec1 = (DrawParts->GetMainCamera().GetCamVec() - DrawParts->GetMainCamera().GetCamPos()).normalized();
						auto vec2 = (Pos - DrawParts->GetMainCamera().GetCamPos()).normalized();
						auto vec2_XZ = vec2; vec2_XZ.y = (0.f);
						if (Vector3DX::Dot(vec1, vec2_XZ) > std::sin(deg2rad(-60))) {
							auto Vec = Matrix4x4DX::Vtrans(Vector3DX::up()*-1.f, Matrix4x4DX::RotAxis(vec2, b.m_SlingZrad.GetRad()));
							SetLightEnableHandle(m_LightHandle[count], TRUE);
							SetLightPositionHandle(m_LightHandle[count], Pos.get());
							SetLightDirectionHandle(m_LightHandle[count], Vec.get());
							count++;
							if (count >= 2) { break; }
						}
					}
				}
			}
			void			Draw() {
				for (auto& b : this->m_LightPoiont) {
					if (b.m_NearPath->IsDraw()) {
						if (CheckCameraViewClip_Box(
							(b.m_Pos + Vector3DX::vget(-1.5f*Scale_Rate, -0.5f*Scale_Rate, -1.5f*Scale_Rate)).get(),
							(b.m_Pos + Vector3DX::vget(1.5f*Scale_Rate, 3.f*Scale_Rate, 1.5f*Scale_Rate)).get()) == FALSE
							) {
							b.m_Obj.DrawModel();
						}
					}
				}
			}
			void			DrawFront() {
				auto* DrawParts = DXDraw::Instance();
				SetUseLighting(FALSE);
				float rad = -Vector3DX::Dot(Vector3DX::Cross((DrawParts->GetMainCamera().GetCamVec() - DrawParts->GetMainCamera().GetCamPos()), Vector3DX::up()), DrawParts->GetMainCamera().GetCamUp());
				int max = (int)this->m_LightPoiont.size();
				for (int i = max - 1; i >= 0; i--) {
					auto& b = this->m_LightPoiont[i];
					if (!b.m_isHit) {
						if (b.m_NearPath->IsDraw()) {
							if (CheckCameraViewClip_Box(
								(b.m_Pos + Vector3DX::vget(-1.5f*Scale_Rate, -0.5f*Scale_Rate, -1.5f*Scale_Rate)).get(),
								(b.m_Pos + Vector3DX::vget(1.5f*Scale_Rate, 3.f*Scale_Rate, 1.5f*Scale_Rate)).get()) == FALSE
								) {
								auto Vec = (b.m_Pos - DrawParts->GetMainCamera().GetCamPos()); Vec.y = (0.f);
								auto Len = Vec.magnitude();
								auto Min = 4.f*Scale_Rate;
								auto Max = 20.f*Scale_Rate;
								float per = 0.5f*std::clamp((Len - Min) / 30.f, 0.f, 1.f);
								if (Len > Max) {
									per = 0.5f*std::clamp(1.f - (Len - Max) / 30.f, 0.f, 1.f);
								}
								if (per > 1.f / 255.f) {
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f*per), 0, 255));
									DrawBillboard3D(b.m_Pos.get(), 0.5f, 1.f, 2.2f*Scale_Rate, rad + b.m_SlingZrad.GetRad(), Light_Graph.get(), TRUE);
								}
							}
						}
					}
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				SetUseLighting(TRUE);
			}
			void			Dispose() {
				for (auto& b : this->m_LightPoiont) {
					b.m_Obj.Dispose();
				}
				m_LightPoiont.clear();
				Light_Graph.Dispose();
			}
		};
		class Garbage {
			static const int Blocks{5};
			struct GarbageObjct {
				MV1 m_Obj;
				bool IsChanged{false};
				float m_Rad{0.f};
				float m_PrevRad{0.f};
				Vector3DX m_Pos;
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
			void		Execute(const Vector3DX& CamPos) {
				auto* OptionParts = OPTION::Instance();
				if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) == 0) {
					return;
				}
				int x = static_cast<int>(CamPos.x / 100.f*2.f);
				int z = static_cast<int>(CamPos.z / 100.f*2.f);
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
						int index = static_cast<int>(&o - &m_Obj.front());
						int xp = (x + (index % Blocks) - 2);
						int zp = (z + (index / Blocks) - 2);
						//前のフレームでの同じ場所を参照
						o.IsChanged = true;
						for (auto& o2 : m_Obj) {
							int index2 = static_cast<int>(&o2 - &m_Obj.front());
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
						o.m_Pos = Vector3DX::vget(static_cast<float>(xp * 100), 0.f, static_cast<float>(zp * 100));
					}
				}
				for (auto& o : m_Obj) {
					o.m_PrevRad = o.m_Rad;
				}
				xprev = x;
				zprev = z;
				for (auto& o : m_Obj) {
					o.m_Obj.SetMatrix(Matrix4x4DX::RotAxis(Vector3DX::up(), o.m_Rad) * Matrix4x4DX::Mtrans(o.m_Pos));
				}
			}
			void		ShadowDraw() {
				auto* OptionParts = OPTION::Instance();
				if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) == 0) {
					return;
				}
				m_Obj.at(Blocks * Blocks / 2).m_Obj.DrawModel();
			}
			void		Draw() {
				auto* OptionParts = OPTION::Instance();
				if (OptionParts->GetParamInt(EnumSaveParam::ObjLevel) == 0) {
					return;
				}
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
		class DrumControl {
		private:
			struct Drums {
				Vector3DX	m_Pos;
				MV1			m_Obj;
				MV1			m_Col;
				Builds*		m_NearPath{nullptr};
			};
		private:
			MV1							m_ObjDrumBase;
			MV1							m_ColBase;
			std::vector<Drums>			m_DrumPoiont;
		public:
			const auto&		GetDrum() const noexcept { return this->m_DrumPoiont; }
		public:
			void			Set(int No, const Vector3DX& BasePos, Builds* ptr) {
				this->m_DrumPoiont[No].m_Pos = BasePos;
				this->m_DrumPoiont[No].m_Obj = this->m_ObjDrumBase.Duplicate();
				this->m_DrumPoiont[No].m_Col = this->m_ColBase.Duplicate();
				this->m_DrumPoiont[No].m_Col.SetupCollInfo(1, 1, 1);
				this->m_DrumPoiont[No].m_NearPath = ptr;
			}
		public:
			void			Load() {
				MV1::Load("data/model/build/model.mv1", &this->m_ObjDrumBase, DX_LOADMODEL_PHYSICS_DISABLE);
				MV1::Load("data/model/build/col.mv1", &this->m_ColBase, DX_LOADMODEL_PHYSICS_DISABLE);
			}
			void			Init(int count) {
				this->m_DrumPoiont.resize(count);
			}

			void			Update() {
				for (int i = 0; i < this->m_DrumPoiont.size(); i++) {
					auto& o = this->m_DrumPoiont[i];
					o.m_Obj.SetMatrix(Matrix4x4DX::Mtrans(o.m_Pos));
					o.m_Col.SetMatrix(Matrix4x4DX::Mtrans(o.m_Pos));
					o.m_Col.RefreshCollInfo();
				}
			}
			void			Draw() {
				for (auto& b : this->m_DrumPoiont) {
					if (b.m_NearPath->IsDraw()) {
						if (CheckCameraViewClip_Box(
							(b.m_Pos + Vector3DX::vget(-1.5f*Scale_Rate, -0.5f*Scale_Rate, -1.5f*Scale_Rate)).get(),
							(b.m_Pos + Vector3DX::vget(1.5f*Scale_Rate, 3.f*Scale_Rate, 1.5f*Scale_Rate)).get()) == FALSE
							) {
							b.m_Obj.DrawModel();
						}
					}
				}
			}
			void			Dispose() {
				for (auto& b : this->m_DrumPoiont) {
					b.m_Obj.Dispose();
				}
				m_DrumPoiont.clear();
			}
		};
	private:
#if HIGH_FPS_ROM
		const int Size = 13;
#else
		const int Size = 15;
#endif
		const float tileSize = 30.f;
	private:
		GraphHandle					m_MapGraph;
		int m_MapGraphXSize{0}, m_MapGraphYSize{0};
	private:
		void	MakeMiniMap() noexcept;
		void	UpdateMiniMap() noexcept;
	private:
		MV1							m_ObjBuildBase;
		MV1							m_ColBuildBase;
		std::vector<Builds>			m_ObjBuilds;
		MazeControl					m_MazeControl;
		Garbage						m_Garbage;
		LightControl				m_LightControl;
		DrumControl					m_DrumControl;
	public:
		void			Load(void) noexcept;
		void			Init() noexcept;
		void			Execute(void) noexcept;
		void			ShadowDraw() noexcept;
		void			Draw() noexcept;
		void			DrawFront() noexcept;
		void			Dispose(void) noexcept;
	public:
		const auto&		GetMapGraph(void) const noexcept { return this->m_MapGraph; }
		const auto&		GetMapGraphXSize(void) const noexcept { return this->m_MapGraphXSize; }
		const auto&		GetMapGraphYSize(void) const noexcept { return this->m_MapGraphYSize; }
		const auto&		GetBuildDatas(void) const noexcept { return this->m_ObjBuilds; }
		const auto&		GetDrumControl(void) const noexcept { return this->m_DrumControl; }
		const auto&		GetNearestLight(int No) const noexcept { return m_LightControl.GetNearestLight(No); }

		const auto		HitLightCheck(const Vector3DX& StartPos, Vector3DX* pEndPos) noexcept { return m_LightControl.HitLightCheck(StartPos, pEndPos); }
		const int		GetNearestBuilds(const Vector3DX& NowPosition) const noexcept {
			for (auto& bu : this->GetBuildDatas()) {
				if (bu.GetMeshSel() < 0) { continue; }
				if (
					(bu.GetMaxPos().x > NowPosition.x && NowPosition.x > bu.GetMinPos().x) &&
					(bu.GetMaxPos().z > NowPosition.z && NowPosition.z > bu.GetMinPos().z)
					) {
					return static_cast<int>(&bu - &this->GetBuildDatas().front());
				}
			}
			return -1;
		}
		bool CheckPolyMoveWidth(Vector3DX StartPos, int TargetIndex, float Width) const {
			// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )( 幅指定版 )
			Vector3DX TargetPos = GetBuildDatas().at(TargetIndex).GetMatrix().pos();
			// 最初に開始座標から目標座標に直線的に移動できるかどうかをチェック
			if (CheckPolyMove(StartPos, TargetPos) == false) { return false; }

			Vector3DX Direction = TargetPos - StartPos;		// 開始座標から目標座標に向かうベクトルを算出
			Direction.y = (0.0f);		// y座標を 0.0f にして平面的なベクトルにする

			// 開始座標から目標座標に向かうベクトルに直角な正規化ベクトルを算出
			Vector3DX SideDirection = Vector3DX::Cross(Direction, Vector3DX::up()).normalized();
			{
				// 開始座標と目標座標を Width / 2.0f 分だけ垂直方向にずらして、再度直線的に移動できるかどうかをチェック
				Vector3DX TempVec = SideDirection * (Width / 2.0f);
				if (CheckPolyMove(StartPos + TempVec, TargetPos + TempVec) == false) {
					return false;
				}
			}
			{
				// 開始座標と目標座標を Width / 2.0f 分だけ一つ前とは逆方向の垂直方向にずらして、再度直線的に移動できるかどうかをチェック
				Vector3DX TempVec = SideDirection * (-Width / 2.0f);
				if (CheckPolyMove(StartPos + TempVec, TargetPos + TempVec) == false) {
					return false;
				}
			}
			return true;		// ここまできたら指定の幅があっても直線的に移動できるということなので true を返す
		}
	private:
		// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )
		bool CheckPolyMove(Vector3DX StartPos, Vector3DX TargetPos) const {
			int CheckPoly[4]{};
			int CheckPolyPrev[4]{};
			int NextCheckPoly[4]{};
			int NextCheckPolyPrev[4]{};

			// 開始座標と目標座標の y座標値を 0.0f にして、平面上の判定にする
			StartPos.y = (0.0f);
			TargetPos.y = (0.0f);

			// 開始座標と目標座標の直上、若しくは直下に存在するポリゴンを検索する
			int StartPoly = this->GetNearestBuilds(StartPos);
			int TargetPoly = this->GetNearestBuilds(TargetPos);

			// ポリゴンが存在しなかったら移動できないので false を返す
			if (StartPoly == -1 || TargetPoly == -1) { return false; }

			// 指定線分上にあるかどうかをチェックするポリゴンとして開始座標の直上、若しくは直下に存在するポリゴンを登録
			int CheckPolyNum = 1;
			CheckPoly[0] = StartPoly;
			int CheckPolyPrevNum = 0;
			CheckPolyPrev[0] = -1;

			// 結果が出るまで無条件で繰り返し
			while (true) {
				int NextCheckPolyNum = 0;			// 次のループでチェック対象になるポリゴンの数をリセットしておく
				int NextCheckPolyPrevNum = 0;			// 次のループでチェック対象から外すポリゴンの数をリセットしておく
				// チェック対象のポリゴンの数だけ繰り返し
				for (int i = 0; i < CheckPolyNum; i++) {
					int Index = CheckPoly[i];
					// チェック対象のポリゴンの３座標を取得 y座標を0.0にして、平面的な判定を行うようにする
					Vector3DX Pos = this->GetBuildDatas().at(Index).GetMatrix().pos(); Pos.y = (0.f);

					for (int K = 0; K < 4; K++) {
						int LinkIndex = this->GetBuildDatas().at(Index).GetLinkPolyIndex(K);

						;

						Vector3DX PolyPos = Pos;
						PolyPos.x += ((tileSize / 2.f)*((K == 0 || K == 1) ? 1.f : -1.f));
						PolyPos.z += ((tileSize / 2.f)*((K == 1 || K == 2) ? 1.f : -1.f));
						int K2 = (K + 1) % 4;
						Vector3DX PolyPos2 = Pos;
						PolyPos2.x += ((tileSize / 2.f)*((K2 == 0 || K2 == 1) ? 1.f : -1.f));
						PolyPos2.z += ((tileSize / 2.f)*((K2 == 1 || K2 == 2) ? 1.f : -1.f));
						// ポリゴンの頂点番号0と1の辺に隣接するポリゴンが存在する場合で、
						// 且つ辺の線分と移動開始点、終了点で形成する線分が接していたら if 文が真になる
						if (LinkIndex != -1 && GetMinLenSegmentToSegment(StartPos, TargetPos, PolyPos, PolyPos2) < 0.01f) {
							// もし辺と接しているポリゴンが目標座標上に存在するポリゴンだったら 開始座標から目標座標上まで途切れなくポリゴンが存在するということなので true を返す
							if (LinkIndex == TargetPoly) { return true; }

							// 辺と接しているポリゴンを次のチェック対象のポリゴンに加える

							// 既に登録されているポリゴンの場合は加えない
							int j = 0;
							for (j = 0; j < NextCheckPolyNum; j++) {
								if (NextCheckPoly[j] == LinkIndex) { break; }
							}
							if (j == NextCheckPolyNum) {
								// 次のループで除外するポリゴンの対象に加える

								// 既に登録されている除外ポリゴンの場合は加えない
								int j2 = 0;
								for (j2 = 0; j2 < NextCheckPolyPrevNum; j2++) {
									if (NextCheckPolyPrev[j2] == Index) { break; }
								}
								if (j2 == NextCheckPolyPrevNum) {
									NextCheckPolyPrev[NextCheckPolyPrevNum] = Index;
									NextCheckPolyPrevNum++;
								}

								// 一つ前のループでチェック対象になったポリゴンの場合も加えない
								int j3 = 0;
								for (j3 = 0; j3 < CheckPolyPrevNum; j3++) {
									if (CheckPolyPrev[j3] == LinkIndex) { break; }
								}
								if (j3 == CheckPolyPrevNum) {
									// ここまで来たら漸く次のチェック対象のポリゴンに加える
									NextCheckPoly[NextCheckPolyNum] = LinkIndex;
									NextCheckPolyNum++;
								}
							}
						}
					}
				}

				// 次のループでチェック対象になるポリゴンが一つもなかったということは
				// 移動開始点、終了点で形成する線分と接するチェック対象のポリゴンに隣接する
				// ポリゴンが一つもなかったということなので、直線的な移動はできないということで false を返す
				if (NextCheckPolyNum == 0) { return false; }

				// 次にチェック対象となるポリゴンの情報をコピーする
				for (int i = 0; i < NextCheckPolyNum; i++) {
					CheckPoly[i] = NextCheckPoly[i];
				}
				CheckPolyNum = NextCheckPolyNum;

				// 次にチェック対象外となるポリゴンの情報をコピーする
				for (int i = 0; i < NextCheckPolyPrevNum; i++) {
					CheckPolyPrev[i] = NextCheckPolyPrev[i];
				}
				CheckPolyPrevNum = NextCheckPolyPrevNum;
			}
		}

		const auto		GetPos(int x, int y) const noexcept {
			Vector3DX BasePos;
			BasePos.Set(tileSize * (float)x, 0.f, tileSize * (float)y);
			BasePos -= Vector3DX::vget(tileSize * (float)Size / 2.f, 0.f, tileSize * (float)Size / 2.f);
			return BasePos;
		}
	};
};
