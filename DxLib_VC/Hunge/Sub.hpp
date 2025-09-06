#define NOMINMAX

#include "DxLib.h"

#include <array>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>


static const int tile_size = 64;		//表示上のタイルサイズ
static const int tile_pic_size = 32;	//画像のタイルサイズ
//ベクトルのxy間の角度を取得
static const auto GetRadVec2Vec(const VECTOR& vec1, const VECTOR& vec2) { return std::atan2f(vec1.x - vec2.x, vec1.y - vec2.y); }
//角度をラジアンに変換
static const auto Deg2Rad(float value) { return value * DX_PI_F / 180.f; }

//いろいろまとめるクラス
class DXDraw {
	static const int		BaseScreenWidth{ 1920 };						/*UI描画などの基準となる解像度*/
	static const int		BaseScreenHeight{ 1080 };						/*UI描画などの基準となる解像度*/
	static const int		BaseDPI = 96;
	static const int GetDPI() noexcept {
		int DPI = BaseDPI;
		GetMonitorDpi(NULL, &DPI);
		if (DPI == 0) {
			DPI = BaseDPI;
		}
		return DPI;
	}
private:
	int draw_x = 1280;			//ウィンドウサイズX
	int draw_y = 720;			//ウィンドウサイズY
public:
	const auto GetDispX() const noexcept { return draw_x; }
	const auto GetDispY() const noexcept { return draw_y; }
public:
	//コンストラクタ
	DXDraw(void) noexcept {
		int DispXSize = GetSystemMetrics(SM_CXSCREEN);
		int DispYSize = GetSystemMetrics(SM_CYSCREEN);

		SetOutApplicationLogValidFlag(false);           /*log*/
		SetMainWindowText("game title");                /*タイトル*/
		ChangeWindowMode(true);                         /*窓表示*/
		SetUseDirect3DVersion(DX_DIRECT3D_11);          /*directX ver*/
		SetGraphMode(DispXSize, DispYSize, 32);         /*解像度*/
		SetUseDirectInputFlag(true);                    /*DirectInput使用*/
		SetDirectInputMouseMode(true);                  /*DirectInputマウス使用*/
		SetWindowSizeChangeEnableFlag(false, false);    /*ウインドウサイズを手動変更不可、ウインドウサイズに合わせて拡大もしないようにする*/
		SetUsePixelLighting(true);                      /*ピクセルライティングの使用*/
		SetFullSceneAntiAliasingMode(4, 2);             /*アンチエイリアス*/
		SetWaitVSyncFlag(true);                         /*垂直同期*/
		DxLib_Init();                                   /*ＤＸライブラリ初期化処理*/
		SetSysCommandOffFlag(true);                     /*タスクスイッチを有効にするかどうかを設定する*/
		SetAlwaysRunFlag(true);                         /*background*/

		// 解像度指定
		if (DispYSize >= (DispXSize * BaseScreenHeight / BaseScreenWidth)) {// 4:3
			DispYSize = (DispXSize * BaseScreenHeight / BaseScreenWidth);
		}
		else {// 16:9より横長
			DispXSize = (DispYSize * BaseScreenWidth / BaseScreenHeight);
		}
		draw_x = DispXSize - 72 * 16 / 9;
		draw_y = DispYSize - 72;
		draw_x = draw_x * GetDPI() / BaseDPI;
		draw_y = draw_y * GetDPI() / BaseDPI;

		SetWindowStyleMode(0);
		SetWindowPosition((DispXSize - draw_x) / 2, 0);
		SetWindowSize(draw_x, draw_y);
	}
	//デストラクタ
	~DXDraw(void) noexcept {
		DxLib_End();
	}
public:

};

// ベクトルデータ型
struct VECTOR2D
{
	float					x, y;
};


//タイルデータ
struct TileData {
	int Height{ 0 };
	int FloorID{ 0 };
	int WayPointRad{ -1 };
private:
	struct LinkBuffer {
		bool isActive{ false };
		VECTOR2D Pos{};
		int ID{};
	};
private:
	int							m_mesh{ -1 };
	VECTOR2D					m_MinPos{};
	VECTOR2D					m_Pos{};
	VECTOR2D					m_MaxPos{};
	int							MyIndex{ 0 };
public:
	std::array<LinkBuffer, 4>	m_LinkPosBuffer{};
public:
	const int	GetIndex() const noexcept { return MyIndex; }
	const int	GetLinkPolyIndex(int ID) const noexcept { return m_LinkPosBuffer[ID].ID; }
	void		SetLinkBuffer(int tris, int X, int Y) noexcept {
		m_LinkPosBuffer[tris].isActive = true;
		m_LinkPosBuffer[tris].Pos.x = (float)X;
		m_LinkPosBuffer[tris].Pos.y = (float)Y;
	}
public:
	const VECTOR2D& GetMinPos(void) const noexcept { return m_MinPos; }
	const VECTOR2D& GetPos(void) const noexcept { return m_Pos; }
	const VECTOR2D& GetMaxPos(void) const noexcept { return m_MaxPos; }
public:
	void		Set(int index) noexcept {
		this->MyIndex = index;
		for (auto& L : this->m_LinkPosBuffer) {
			L.isActive = false;
			L.ID = -1;
		}
	}
	void		SetPosition(VECTOR2D MinPos, VECTOR2D MaxPos) noexcept {
		this->m_MinPos = MinPos;
		this->m_MaxPos = MaxPos;
		this->m_Pos.x = (this->m_MinPos.x + this->m_MaxPos.x) / 2.f;
		this->m_Pos.y = (this->m_MinPos.y + this->m_MaxPos.y) / 2.f;
	}
};

//描画クラス
class DrawControl {
private:
	int m_draw_x = 1920;			//ウィンドウサイズX
	int m_draw_y = 1080;			//ウィンドウサイズY

	VECTOR					m_EyePos{};

	int						LastDrawBuffer{ -1 };			//最終描画結果を描画するスクリーンバッファ

	int						m_BlackScreen{ -1 };
	int						m_WallBuffer{ -1 };

	int* m_FloorHandlePtr{ nullptr };

	int						m_PointBuffer{ -1 };				//影のスクリーンバッファ
	int						m_PointBuffer2{ -1 };				//影のスクリーンバッファ
	std::vector<int>		m_PointLightGraph{};
	std::vector<VECTOR>		m_PointLightPos{};

	std::vector<TileData>	m_TileData{};

	int						m_MapXsize{ 0 };
	int						m_MapYsize{ 0 };
private:
	//タイルごとのデータを取得
	const auto GetTileData(int x, int y) const noexcept { return m_TileData.at(static_cast<size_t>(std::clamp(x, 0, m_MapXsize - 1) * m_MapYsize + std::clamp(y, 0, m_MapYsize - 1))); }
public:
	const auto GetMapXsize() const noexcept { return m_MapXsize; }
	const auto GetMapYsize() const noexcept { return m_MapYsize; }


	const auto GetTileData(int index) const noexcept { return m_TileData.at(index); }
	const auto GetTileSize() const noexcept { return m_TileData.size(); }
	const int		GetNearestBuilds(const VECTOR2D& NowPosition) const noexcept {
		for (auto& bu : this->m_TileData) {
			if (
				(bu.GetMaxPos().x >= NowPosition.x && NowPosition.x >= bu.GetMinPos().x) &&
				(bu.GetMaxPos().y >= NowPosition.y && NowPosition.y >= bu.GetMinPos().y)
				) {
				return static_cast<int>(&bu - &this->m_TileData.front());
			}
		}
		return -1;
	}
	const int		GetNearestBuilds2(const VECTOR2D& NowPosition) const noexcept {
		float Len = 1000000.f;
		int Answer = -1;
		for (auto& bu : this->m_TileData) {
			VECTOR2D Length{};
			Length.x = bu.GetPos().x - NowPosition.x;
			Length.y = bu.GetPos().y - NowPosition.y;
			if (Len >= std::hypotf(Length.x, Length.y)) {
				Len = std::hypotf(Length.x, Length.y);
				Answer = static_cast<int>(&bu - &this->m_TileData.front());
			}
		}
		return Answer;
	}
	// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )
	bool			CheckPolyMove(VECTOR2D StartPos, VECTOR2D TargetPos) const {
		int Rate = 1;

		int CheckPoly[4]{};
		int CheckPolyPrev[4]{};
		int NextCheckPoly[4]{};
		int NextCheckPolyPrev[4]{};

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
				// チェック対象のポリゴンの３座標を取得
				VECTOR2D Pos = this->m_TileData.at(Index).GetPos();

				for (int K = 0; K < 4; K++) {
					int LinkIndex = this->m_TileData.at(Index).GetLinkPolyIndex(K);

					;

					VECTOR2D PolyPos = Pos;
					PolyPos.x += ((Rate / 2.f) * ((K == 0 || K == 1) ? 1.f : -1.f));
					PolyPos.y += ((Rate / 2.f) * ((K == 1 || K == 2) ? 1.f : -1.f));
					int K2 = (K + 1) % 4;
					VECTOR2D PolyPos2 = Pos;
					PolyPos2.x += ((Rate / 2.f) * ((K2 == 0 || K2 == 1) ? 1.f : -1.f));
					PolyPos2.y += ((Rate / 2.f) * ((K2 == 1 || K2 == 2) ? 1.f : -1.f));
					// ポリゴンの頂点番号0と1の辺に隣接するポリゴンが存在する場合で、
					// 且つ辺の線分と移動開始点、終了点で形成する線分が接していたら if 文が真になる
					auto P = Segment_Segment_MinLength(VGet(StartPos.x, StartPos.y, 0.f), VGet(TargetPos.x, TargetPos.y, 0.f), VGet(PolyPos.x, PolyPos.y, 0.f), VGet(PolyPos2.x, PolyPos2.y, 0.f));
					if (LinkIndex != -1 && P < 0.01f) {
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
	bool			CheckPolyMoveWidth(VECTOR2D StartPos, VECTOR2D TargetPos, float Width) const {
		// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )( 幅指定版 )
		// 最初に開始座標から目標座標に直線的に移動できるかどうかをチェック
		if (CheckPolyMove(StartPos, TargetPos) == false) { return false; }

		VECTOR2D Direction{};//開始座標から目標座標に向かうベクトルを算出
		Direction.x = TargetPos.x - StartPos.x;
		Direction.y = TargetPos.y - StartPos.y;

		// 開始座標から目標座標に向かうベクトルに直角な正規化ベクトルを算出
		auto Tmp = VNorm(VCross(VGet(Direction.x, Direction.y, 0.f), VGet(0.f, 0.f, 1.f)));
		VECTOR2D SideDirection(Tmp.x, Tmp.y);
		{
			// 開始座標と目標座標を Width / 2.0f 分だけ垂直方向にずらして、再度直線的に移動できるかどうかをチェック
			VECTOR2D TempVec{};// = VScale(SideDirection, Width / 2.0f);
			TempVec.x = SideDirection.x * Width / 2.0f;
			TempVec.y = SideDirection.y * Width / 2.0f;
			VECTOR2D A{}, B{};
			A.x = StartPos.x + TempVec.x;
			A.y = StartPos.y + TempVec.y;
			B.x = TargetPos.x + TempVec.x;
			B.y = TargetPos.y + TempVec.y;
			if (CheckPolyMove(A, B) == false) {
				return false;
			}
		}
		{
			// 開始座標と目標座標を Width / 2.0f 分だけ一つ前とは逆方向の垂直方向にずらして、再度直線的に移動できるかどうかをチェック
			VECTOR2D TempVec{};// = VScale(SideDirection, -Width / 2.0f);
			TempVec.x = SideDirection.x * -Width / 2.0f;
			TempVec.y = SideDirection.y * -Width / 2.0f;
			VECTOR2D A{}, B{};
			A.x = StartPos.x + TempVec.x;
			A.y = StartPos.y + TempVec.y;
			B.x = TargetPos.x + TempVec.x;
			B.y = TargetPos.y + TempVec.y;
			if (CheckPolyMove(A, B) == false) {
				return false;
			}
		}
		return true;		// ここまできたら指定の幅があっても直線的に移動できるということなので true を返す
	}

	void			SetupLink() noexcept {
		int						SeekPoint{ 0 };
		for (int x = 0; x < m_MapXsize; x++) {
			for (int y = 0; y < m_MapYsize; y++) {
				auto& bu = *SetTileData(x, y);
				bu.Set(SeekPoint);
				++SeekPoint;
				if (GetTileData(x, y).Height == 0) {
					bu.SetPosition(VECTOR2D((float)x - 0.5f, (float)y - 0.5f), VECTOR2D((float)(x + 0.5f), (float)(y + 0.5f)));

					if (GetTileData(x + 1, y).Height == 0) {
						bu.SetLinkBuffer(0, x + 1, y);
					}
					if (GetTileData(x, y + 1).Height == 0) {
						bu.SetLinkBuffer(1, x, y + 1);
					}
					if (GetTileData(x - 1, y).Height == 0) {
						bu.SetLinkBuffer(2, x - 1, y);
					}
					if (GetTileData(x, y - 1).Height == 0) {
						bu.SetLinkBuffer(3, x, y - 1);
					}
				}
			}
		}
		//リンクを確立
		for (auto& w : this->m_TileData) {
			for (auto& L : w.m_LinkPosBuffer) {
				if (L.isActive) {
					L.ID = GetNearestBuilds(L.Pos);
				}
			}
		}
	}

	const float GetDrawTileSize() const noexcept {
		return m_EyePos.z * tile_size;
	}
	//座標変換
	const auto Get2DPos(float x, float y, float z) const noexcept {
		float tile_scale = (12.f / (float)GetDrawTileSize()) * (float)GetDrawTileSize() / ((12.f / (float)GetDrawTileSize()) * (float)GetDrawTileSize() - z) * (float)GetDrawTileSize();//タイルの表示倍率
		return VGet((float)(m_draw_x / 2) + (x - m_EyePos.x) * tile_scale, (float)(m_draw_y / 2) + (y - m_EyePos.y) * tile_scale, tile_scale);
	}
	const auto GetPos2D(float x2D, float y2D) const noexcept {
		float tile_scale = (12.f / (float)GetDrawTileSize()) * (float)GetDrawTileSize() / ((12.f / (float)GetDrawTileSize()) * (float)GetDrawTileSize() - 0) * (float)GetDrawTileSize();//タイルの表示倍率
		return VGet(
			(x2D - (float)(m_draw_x / 2)) / tile_scale + m_EyePos.x,
			(y2D - (float)(m_draw_y / 2)) / tile_scale + m_EyePos.y,
			0.f);
	}
private:
	//タイルの最小最大座標を取得
	const auto GetTileMin(const VECTOR& value) const noexcept { return VGet(value.x - value.z / 2.f, value.y - value.z / 2.f, 0.f); }
	const auto GetTileMax(const VECTOR& value) const noexcept { return VGet(value.x + value.z / 2.f, value.y + value.z / 2.f, 0.f); }
	//タイルごとの座標
	const auto Gettile2DPos(int x, int y, bool ismnus) const noexcept { return Get2DPos((float)x, (float)y, (float)(GetTileData(x, y).Height * (ismnus ? -1 : 1))); }
	//ポイント影を描画
	void DrawPointShadow(int x, int y, const VECTOR& light2D) const noexcept {
		auto Draw = [&](const VECTOR& sidePos, const VECTOR& postmp1, const VECTOR& postmp2, const VECTOR& lightpos, float radsub) {
			auto centerZ = (Gettile2DPos(x, y, false).z - sidePos.z) / (float)GetDrawTileSize();
			if (centerZ > 0 && std::cos(GetRadVec2Vec(sidePos, lightpos) - radsub) > 0.f) {
				auto length = 480.f * centerZ;
				float lightrad1 = GetRadVec2Vec(postmp1, lightpos);
				float lightrad2 = GetRadVec2Vec(postmp2, lightpos);
				DrawModiGraph(
					(int)(postmp1.x + std::sin(lightrad1) * length), (int)(postmp1.y + std::cos(lightrad1) * length),
					(int)(postmp2.x + std::sin(lightrad2) * length), (int)(postmp2.y + std::cos(lightrad2) * length),
					(int)postmp2.x, (int)postmp2.y,
					(int)postmp1.x, (int)postmp1.y,
					m_BlackScreen, false);
			}
			};
		//下
		{
			auto sidePos = Gettile2DPos(x, y + 1, false);
			Draw(sidePos, VGet(GetTileMin(sidePos).x, GetTileMin(sidePos).y, 0.f), VGet(GetTileMax(sidePos).x, GetTileMin(sidePos).y, 0.f), Get2DPos(light2D.x, light2D.y, (float)GetTileData(x, y + 1).Height), Deg2Rad(0.f));
		}
		//左
		{
			auto sidePos = Gettile2DPos(x - 1, y, false);
			Draw(sidePos, VGet(GetTileMax(sidePos).x, GetTileMin(sidePos).y, 0.f), VGet(GetTileMax(sidePos).x, GetTileMax(sidePos).y, 0.f), Get2DPos(light2D.x, light2D.y, (float)GetTileData(x - 1, y).Height), Deg2Rad(270.f));
		}
		//上
		{
			auto sidePos = Gettile2DPos(x, y - 1, false);
			Draw(sidePos, VGet(GetTileMax(sidePos).x, GetTileMax(sidePos).y, 0.f), VGet(GetTileMin(sidePos).x, GetTileMax(sidePos).y, 0.f), Get2DPos(light2D.x, light2D.y, (float)GetTileData(x, y - 1).Height), Deg2Rad(180.f));
		}
		//右
		{
			auto sidePos = Gettile2DPos(x + 1, y, false);
			Draw(sidePos, VGet(GetTileMin(sidePos).x, GetTileMax(sidePos).y, 0.f), VGet(GetTileMin(sidePos).x, GetTileMin(sidePos).y, 0.f), Get2DPos(light2D.x, light2D.y, (float)GetTileData(x + 1, y).Height), Deg2Rad(90.f));
		}
	}
	//床を描画
	void DrawFloor(int x, int y) const noexcept {
		auto centerPos = Gettile2DPos(x, y, false);
		if (!(centerPos.z <= (float)GetDrawTileSize())) { return; }
		//影をブレンド
		GraphBlendRectBlt2(
			m_FloorHandlePtr[GetTileData(x, y).FloorID], m_PointBuffer2, m_WallBuffer,
			0, 0,
			tile_pic_size, tile_pic_size,
			std::max(0, (int)GetTileMin(centerPos).x), std::max(0, (int)GetTileMin(centerPos).y),
			std::min(m_draw_x, (int)GetTileMax(centerPos).x), std::min(m_draw_y, (int)GetTileMax(centerPos).y),
			0, 0,
			(centerPos.z <= (float)GetDrawTileSize()) ? 128 : 0, DX_GRAPH_BLEND_NORMAL);

		//最終描画
		DrawRotaGraph((int)centerPos.x, (int)centerPos.y, (double)(centerPos.z / tile_pic_size), 0.0, m_WallBuffer, false);
	}
	//描画順を考慮しつつ描画
	void DrawAll(std::function<void(int, int)> Func) const noexcept {
		for (int x = 0; x < m_MapXsize; x++) {
			if ((float)x - m_EyePos.x > -1.f / 2.f) { continue; }
			//左上
			for (int y = 0; y < m_MapYsize; y++) {
				if ((float)y - m_EyePos.y > -1.f / 2.f) { continue; }
				Func(x, y);
			}
			//左下
			for (int y = m_MapYsize - 1; y >= 0; y--) {
				if ((float)y - m_EyePos.y < 1.f / 2.f) { continue; }
				Func(x, y);
			}
		}
		for (int x = m_MapXsize - 1; x >= 0; x--) {
			if ((float)x - m_EyePos.x < 1.f / 2.f) { continue; }
			//右上
			for (int y = 0; y < m_MapYsize; y++) {
				if ((float)y - m_EyePos.y > -1.f / 2.f) { continue; }
				Func(x, y);
			}
			//右下
			for (int y = m_MapYsize - 1; y >= 0; y--) {
				if ((float)y - m_EyePos.y < 1.f / 2.f) { continue; }
				Func(x, y);
			}
		}
		//被っているところ
		for (int x = 0; x < m_MapXsize; x++) {
			float x_f = (float)x - m_EyePos.x;
			if (x_f <= -1.f / 2.f || 1.f / 2.f <= x_f) { continue; }
			//上
			for (int y = 0; y < m_MapYsize; y++) {
				if ((float)y - m_EyePos.y > -1.f / 2.f) { continue; }
				Func(x, y);
			}
			//下
			for (int y = m_MapYsize - 1; y >= 0; y--) {
				if ((float)y - m_EyePos.y < 1.f / 2.f) { continue; }
				Func(x, y);
			}
		}
		for (int y = 0; y < m_MapYsize; y++) {
			float y_f = (float)y - m_EyePos.y;
			if (y_f <= -1.f / 2.f || 1.f / 2.f <= y_f) { continue; }
			//上
			for (int x = 0; x < m_MapXsize; x++) {
				if ((float)x - m_EyePos.x > 0) { continue; }
				Func(x, y);
			}
			//下
			for (int x = m_MapXsize - 1; x >= 0; x--) {
				if ((float)x - m_EyePos.x < 0) { continue; }
				Func(x, y);
			}
		}
	}
	//画像ハンドルをリセット
	void InitGraphHandle(void) noexcept {
		if (m_FloorHandlePtr) {
			DeleteSharingGraph(m_FloorHandlePtr[0]);
			delete[] m_FloorHandlePtr;
			m_FloorHandlePtr = nullptr;
		}
	}
public://ゲッター
	//カメラの座標指定
	void SetCameraPos(const VECTOR& EyePos) noexcept {
		m_EyePos = VGet(EyePos.x, EyePos.y, EyePos.z);
	}
	//カメラの座標取得
	const auto GetCameraPos(void) const noexcept {
		return VGet(m_EyePos.x, m_EyePos.y, m_EyePos.z);
	}
	//ポイントライトの追加
	const auto AddPointLight(const VECTOR& LightPos) noexcept {
		m_PointLightGraph.emplace_back(MakeScreen(m_draw_x, m_draw_y, false));
		m_PointLightPos.emplace_back(LightPos);
		return m_PointLightPos.size() - 1;
	}
	//該当IDのポイントライト座標を指定
	void SetPointLight(size_t ID, const VECTOR& LightPos) noexcept { m_PointLightPos.at(ID) = LightPos; }
	//該当IDのポイントライト座標を取得
	const auto& GetPointLight(size_t ID) const noexcept { return m_PointLightPos.at(ID); }
	//画像ハンドルを設定
	void SetPicture(const char* floorTexName) noexcept {
		InitGraphHandle();
		m_FloorHandlePtr = new int[4];
		LoadDivGraph(floorTexName, 4, 2, 2, tile_pic_size, tile_pic_size, m_FloorHandlePtr);//床テクスチャ
	}
	//マップデータのサイズを設定
	void SetMapSize(int x, int y) noexcept {
		m_MapXsize = x;
		m_MapYsize = y;
		m_TileData.resize(static_cast<size_t>(m_MapXsize * m_MapYsize));
	}
	//タイルごとのデータポインタを取得
	TileData* SetTileData(int x, int y) noexcept { return &m_TileData.at(static_cast<size_t>(std::clamp(x, 0, m_MapXsize - 1) * m_MapYsize + std::clamp(y, 0, m_MapYsize - 1))); }
public://シングルトン
private:
	static const DrawControl* m_Singleton;
public:
	static void Create(int draw_x, int draw_y) noexcept {
		m_Singleton = new DrawControl(draw_x, draw_y);
	}
	static DrawControl* Instance(void) noexcept {
		if (m_Singleton == nullptr) {
			MessageBox(NULL, "Failed Instance Create", "", MB_OK);
			exit(-1);
		}
		// if (m_Singleton == nullptr) { m_Singleton = new DrawControl(); }
		return (DrawControl*)m_Singleton;
	}
	static void Release(void) noexcept {
		delete m_Singleton;
	}
private:
	//コンストラクタ
	DrawControl(int draw_x, int draw_y) noexcept {
		m_draw_x = draw_x;
		m_draw_y = draw_y;
		m_BlackScreen = MakeScreen(tile_pic_size, tile_pic_size);
		m_WallBuffer = MakeScreen(tile_pic_size, tile_pic_size);
		SetDrawScreen(m_BlackScreen);
		{
			DrawBox(0, 0, tile_pic_size, tile_pic_size, GetColor(0, 0, 0), true);
		}
		m_PointBuffer = MakeScreen(m_draw_x, m_draw_y, false);
		m_PointBuffer2 = MakeScreen(m_draw_x, m_draw_y, true);
		LastDrawBuffer = MakeScreen(m_draw_x, m_draw_y, true);
	}
	//デストラクタ
	~DrawControl(void) noexcept {
		//画像ハンドルの削除
		for (auto& p : m_PointLightGraph) {
			DeleteGraph(p);
		}
		DeleteGraph(m_PointBuffer);
		DeleteGraph(m_PointBuffer2);
		DeleteGraph(LastDrawBuffer);
		DeleteGraph(m_BlackScreen);
		DeleteGraph(m_WallBuffer);
		//タイルデータのクリア
		m_TileData.clear();
		//画像ハンドルの消去
		InitGraphHandle();
	}
public:
	//更新
	void Execute(void) noexcept {
		//シャドウバッファに描画
		for (auto& p : m_PointLightGraph) {
			int index = static_cast<int>(&p - &m_PointLightGraph.front());
			SetDrawScreen(p);
			ClearDrawScreen();
			{
				DrawBox(0, 0, m_draw_x, m_draw_y, GetColor(255, 255, 255), true);
				for (int x = 0; x < m_MapXsize; x++) {
					for (int y = 0; y < m_MapYsize; y++) {
						DrawPointShadow(x, y, m_PointLightPos.at(index));		//ポイント影
					}
				}
			}
		}


		SetDrawScreen(m_PointBuffer);
		ClearDrawScreen();
		{
			SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			for (auto& p : m_PointLightGraph) {
				SetDrawScreen(m_PointBuffer);
				DrawGraph(0, 0, p, true);
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		GraphFilter(m_PointBuffer, DX_GRAPH_FILTER_INVERT);

		SetDrawScreen(m_PointBuffer2);
		ClearDrawScreen();

		GraphBlend(m_PointBuffer2, m_PointBuffer, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
			DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);


		//最終描画
		SetDrawScreen(LastDrawBuffer);
		ClearDrawScreen();
		{
			DrawAll([&](int x, int y) {
				DrawFloor(x, y);							//床
				});
		}
	}
	//描画
	void Draw(int draw_x, int draw_y) const noexcept {
		auto prev = GetDrawMode();
		DxLib::SetDrawMode(DX_DRAWMODE_NEAREST);
		DrawExtendGraph(0, 0, draw_x, draw_y, LastDrawBuffer, true);
		DxLib::SetDrawMode(prev);
	}
};

class PathChecker {
public:
	// 経路探索処理用の１ポリゴンの情報
	class PATHPLANNING_UNIT {
		int PolyIndex;						// ポリゴン番号
		float TotalDistance;				// 経路探索でこのポリゴンに到達するまでに通過したポリゴン間の距離の合計
		PATHPLANNING_UNIT* PrevPolyUnit;		// 経路探索で確定した経路上の一つ前のポリゴン( 当ポリゴンが経路上に無い場合は nullptr )
		PATHPLANNING_UNIT* NextPolyUnit;		// 経路探索で確定した経路上の一つ先のポリゴン( 当ポリゴンが経路上に無い場合は nullptr )
	public:
		PATHPLANNING_UNIT* ActiveNextUnit;		// 経路探索処理対象になっている次のポリゴンのメモリアドレスを格納する変数
	public:
		const auto& GetPolyIndex() const noexcept { return this->PolyIndex; }
		const auto& GetNextPolyUnit() const noexcept { return this->NextPolyUnit; }
	public:
		bool SetPrevPolyUnit(PATHPLANNING_UNIT* PUnit, int tris) {
			auto* drawcontrol = DrawControl::Instance();
			// 隣接するポリゴンが既に経路探索処理が行われていて、且つより距離の長い経路となっている場合は何もしない
			auto& Unit = drawcontrol->GetTileData(PUnit->PolyIndex);

			VECTOR2D A = drawcontrol->GetTileData(Unit.GetLinkPolyIndex(tris)).GetPos();
			VECTOR2D B = Unit.GetPos();
			auto trisdistance = PUnit->TotalDistance +
				VSize(VAdd(VGet(A.x, A.y, 0.f), VGet(B.x, B.y, 0.f)));

			if (this->TotalDistance > trisdistance) {
				this->TotalDistance = trisdistance;		// 隣接するポリゴンにここに到達するまでの距離を代入する
			}
			else {
				if (this->PrevPolyUnit) { return false; }
			}
			this->PrevPolyUnit = PUnit;			// 隣接するポリゴンに経路情報となる自分のポリゴンの番号を代入する
			return true;
		}
		bool SearchThisUnit(PATHPLANNING_UNIT* SearchUnit) {
			// 次のループで行う経路探索処理対象に追加する、既に追加されていたら追加しない
			PATHPLANNING_UNIT* PUnitSub2 = SearchUnit;
			while (true) {
				if (PUnitSub2 == nullptr) { break; }
				if (PUnitSub2 == this) { return false; }//既に追加されとる
				PUnitSub2 = PUnitSub2->ActiveNextUnit;
			}
			return PUnitSub2;
		}
	public:
		// ゴール地点のポリゴンからスタート地点のポリゴンに辿って経路上のポリゴンに次に移動すべきポリゴンの番号を代入する
		static void SetNextIndex(PATHPLANNING_UNIT* pGoal, PATHPLANNING_UNIT* pStart) {
			PATHPLANNING_UNIT* PUnit = pGoal;
			while (true) {
				auto* PrevPUnitIndex = PUnit;
				PUnit = PUnit->PrevPolyUnit;
				PUnit->NextPolyUnit = PrevPUnitIndex;
				if (PUnit == pStart) { break; }
			}
		}
	public:
		void Init(int index) {
			this->PolyIndex = index;
			this->TotalDistance = 0.0f;
			this->PrevPolyUnit = nullptr;
			this->NextPolyUnit = nullptr;
			this->ActiveNextUnit = nullptr;
		}
	};
private:
	VECTOR2D GoalPosition{};							// 目標位置
	std::vector<PATHPLANNING_UNIT>UnitArray{};			// 経路探索処理で使用する全ポリゴンの情報配列が格納されたメモリ領域の先頭メモリアドレスを格納する変数
	PATHPLANNING_UNIT* StartUnit{ nullptr };			// 経路のスタート地点にあるポリゴン情報へのメモリアドレスを格納する変数
	PATHPLANNING_UNIT* GoalUnit{ nullptr };				// 経路のゴール地点にあるポリゴン情報へのメモリアドレスを格納する変数
public:
	const auto& GetStartUnit() const noexcept { return this->StartUnit; }
public:
	VECTOR2D GetNextPoint(const VECTOR2D& NowPosition, int* TargetPathPlanningIndex) const {
		auto* drawcontrol = DrawControl::Instance();
		int NowIndex = drawcontrol->GetNearestBuilds(NowPosition);
		if (!((*TargetPathPlanningIndex != -1) && (this->GoalUnit))) {
			return drawcontrol->GetTileData(drawcontrol->GetNearestBuilds2(NowPosition)).GetPos();
		}
		if (NowIndex != this->GoalUnit->GetPolyIndex()) {																	// 現在乗っているポリゴンがゴール地点にあるポリゴンの場合は処理を分岐
			if (NowIndex == *TargetPathPlanningIndex) {													// 現在乗っているポリゴンが移動中間地点のポリゴンの場合は次の中間地点を決定する処理を行う
				while (true) {																				// 次の中間地点が決定するまでループし続ける
					if (!this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()) { break; }
					auto NextIndex = this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()->GetPolyIndex();
					if (!drawcontrol->CheckPolyMoveWidth(NowPosition, drawcontrol->GetTileData(NextIndex).GetPos(), 0.5f)) { break; }		// 経路上の次のポリゴンの中心座標に直線的に移動できない場合はループから抜ける
					(*TargetPathPlanningIndex) = NextIndex;													// チェック対象を経路上の更に一つ先のポリゴンに変更する
					if ((*TargetPathPlanningIndex) == this->GoalUnit->GetPolyIndex()) { break; }				// もしゴール地点のポリゴンだったらループを抜ける
				}
			}
			// 移動方向を決定する、移動方向は現在の座標から中間地点のポリゴンの中心座標に向かう方向
			return drawcontrol->GetTileData(*TargetPathPlanningIndex).GetPos();
		}
		else {
			// 方向は目標座標
			return this->GoalPosition;
		}
	}
public:
	bool Init(VECTOR2D StartPos, VECTOR2D GoalPos) {
		auto* drawcontrol = DrawControl::Instance();
		// 指定の２点の経路を探索する( 戻り値 true:経路構築成功 false:経路構築失敗( スタート地点とゴール地点を繋ぐ経路が無かった等 ) )
		this->GoalPosition = GoalPos;			// ゴール位置を保存

		this->UnitArray.resize(drawcontrol->GetTileSize());			// 経路探索用のポリゴン情報を格納するメモリ領域を確保、初期化
		for (auto& p : this->UnitArray) {
			p.Init(static_cast<int>(&p - &this->UnitArray.front()));
		}

		int StartIndex = drawcontrol->GetNearestBuilds(StartPos);	// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
		if (StartIndex == -1) { return false; }
		this->StartUnit = &this->UnitArray[StartIndex];					// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存

		int GoalIndex = drawcontrol->GetNearestBuilds(GoalPos);		// ゴール地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
		if (GoalIndex == -1) { return false; }
		this->GoalUnit = &this->UnitArray[GoalIndex];				// ゴール地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
		if (GoalIndex == StartIndex) { return false; }				// ゴール地点にあるポリゴンとスタート地点にあるポリゴンが同じだったら false を返す

		PATHPLANNING_UNIT* ActiveFirstUnit = this->StartUnit;		// 経路探索処理対象のポリゴンとしてスタート地点にあるポリゴンを登録する
		// 経路を探索してゴール地点のポリゴンにたどり着くまでループを繰り返す
		while (true) {
			bool Goal = false;
			// 経路探索処理対象になっているポリゴンをすべて処理
			PATHPLANNING_UNIT* PUnit = ActiveFirstUnit;
			ActiveFirstUnit = nullptr;
			while (true) {
				// ポリゴンの辺の数だけ繰り返し
				for (int K = 0; K < 4; K++) {
					int Index = drawcontrol->GetTileData(PUnit->GetPolyIndex()).GetLinkPolyIndex(K);
					if (Index == -1) { continue; }											// 辺に隣接するポリゴンが無い場合は何もしない
					if (Index == this->StartUnit->GetPolyIndex()) { continue; }				//スタート地点のポリゴンだった場合は何もしない
					auto& NowUnit = this->UnitArray[Index];
					if (!NowUnit.SetPrevPolyUnit(PUnit, K)) {
						continue;
					}
					// 次のループで行う経路探索処理対象に追加する、既に追加されていたら追加しない
					if (!NowUnit.SearchThisUnit(ActiveFirstUnit)) {
						NowUnit.ActiveNextUnit = ActiveFirstUnit;
						ActiveFirstUnit = &NowUnit;
					}
					// 隣接するポリゴンがゴール地点にあるポリゴンだったらゴールに辿り着いたフラグを立てる
					if (Index == this->GoalUnit->GetPolyIndex()) {
						Goal = true;
					}
				}
				PUnit = PUnit->ActiveNextUnit;
				if (PUnit == nullptr) { break; }
			}

			if (!ActiveFirstUnit) { return false; }			// スタート地点にあるポリゴンからゴール地点にあるポリゴンに辿り着けないということなので false を返す
			if (Goal) { break; }
		}
		PATHPLANNING_UNIT::SetNextIndex(this->GoalUnit, this->StartUnit);		// ゴール地点のポリゴンからスタート地点のポリゴンに辿って経路上のポリゴンに次に移動すべきポリゴンの番号を代入する
		return true;										// ここにきたらスタート地点からゴール地点までの経路が探索できたということなので true を返す
	}
	void Dispose(void) {
		this->UnitArray.clear();
	}
};