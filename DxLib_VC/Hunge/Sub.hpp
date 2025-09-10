#pragma once
#define NOMINMAX

#include "DxLib.h"

#include <array>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

constexpr float BaseFrameRate = 60.f;

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
public://シングルトン
private:
	static const DXDraw* m_Singleton;
public:
	static void Create() noexcept {
		m_Singleton = new DXDraw();
	}
	static DXDraw* Instance(void) noexcept {
		if (m_Singleton == nullptr) {
			MessageBox(NULL, "Failed Instance Create", "", MB_OK);
			exit(-1);
		}
		// if (m_Singleton == nullptr) { m_Singleton = new DXDraw(); }
		return (DXDraw*)m_Singleton;
	}
	static void Release(void) noexcept {
		delete m_Singleton;
	}
private:
	//コンストラクタ
	DXDraw(void) noexcept {
		int DispXSize = GetSystemMetrics(SM_CXSCREEN);
		int DispYSize = GetSystemMetrics(SM_CYSCREEN);

		//DispXSize = 1280;
		//DispYSize = 720;

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


class Rule {
public://ゲッター
	//タイルごとのデータポインタを取得
	int GetPlayerAll() const noexcept { return 8; }
	int GetPlayablePlayer() const noexcept { return 2; }
public://シングルトン
private:
	static const Rule* m_Singleton;
public:
	static void Create() noexcept {
		m_Singleton = new Rule();
	}
	static Rule* Instance(void) noexcept {
		if (m_Singleton == nullptr) {
			MessageBox(NULL, "Failed Instance Create", "", MB_OK);
			exit(-1);
		}
		// if (m_Singleton == nullptr) { m_Singleton = new Rule(); }
		return (Rule*)m_Singleton;
	}
	static void Release(void) noexcept {
		delete m_Singleton;
	}
private:
	//コンストラクタ
	Rule() noexcept {
	}
	//デストラクタ
	~Rule(void) noexcept {
	}
public:
};


enum class SoundEnum {
	Walk,
	Shot,
	Hit,

	Start,
	End,

	Env,

	Max,
};

static const char* SoundPath[static_cast<int>(SoundEnum::Max)] = {
	"data/Sound/runfoot.wav",
	"data/Sound/shot.wav",
	"data/Sound/hit.wav",

	"data/Sound/start.wav",
	"data/Sound/end.wav",

	"data/Sound/Envi.wav",
};

class SoundPool {
private://シングルトン
	static const SoundPool* m_Singleton;
public:
	static void Create(void) noexcept {
		m_Singleton = new SoundPool();
	}
	static SoundPool* Instance(void) noexcept {
		if (m_Singleton == nullptr) {
			MessageBox(NULL, "Failed Instance Create", "", MB_OK);
			exit(-1);
		}
		// if (m_Singleton == nullptr) { m_Singleton = new SoundPool(); }
		return (SoundPool*)m_Singleton;
	}
	static void Release(void) noexcept {
		delete m_Singleton;
	}
private:
	struct Pool {
		int m_DupricateID{ -1 };
	public:
		void Init() {
			if (m_DupricateID != -1) {
				DeleteSoundMem(m_DupricateID);
			}
			m_DupricateID = -1;
		}
	};
private:
	std::array<int, static_cast<int>(SoundEnum::Max)> m_Base;
	std::vector<Pool> m_Pool;
private:
	//コンストラクタ
	SoundPool(void) noexcept {
		for (auto& b : m_Base) {
			int index = static_cast<int>(&b - &m_Base.front());
			b = LoadSoundMem(SoundPath[index]);
		}
		m_Pool.resize(32);
		for (auto& p : m_Pool) {
			p.Init();
		}
	}
	//デストラクタ
	~SoundPool(void) noexcept {
		for (auto& b : m_Base) {
			DeleteSoundMem(b);
		}
		for (auto& p : m_Pool) {
			p.Init();
		}
		m_Pool.clear();
	}
public:
	void PlayLoop(SoundEnum ID, int Pan = -256, int Size = -1) noexcept {
		for (auto& p : m_Pool) {
			if (p.m_DupricateID != -1) {
				if (CheckSoundMem(p.m_DupricateID) == 1) { continue; }
				p.Init();
			}
			p.m_DupricateID = DuplicateSoundMem(m_Base[static_cast<int>(ID)]);

			ChangePanSoundMem((Pan == -256) ? 0 : Pan, p.m_DupricateID);
			ChangeVolumeSoundMem((Size == -1) ? 255 : Size, p.m_DupricateID);
			PlaySoundMem(p.m_DupricateID, DX_PLAYTYPE_LOOP, TRUE);
			return;
		}
		m_Pool.emplace_back();
		auto& p = m_Pool.back();
		p.m_DupricateID = DuplicateSoundMem(m_Base[static_cast<int>(ID)]);

		ChangePanSoundMem((Pan == -256) ? 0 : Pan, p.m_DupricateID);
		ChangeVolumeSoundMem((Size == -1) ? 255 : Size, p.m_DupricateID);
		PlaySoundMem(p.m_DupricateID, DX_PLAYTYPE_LOOP, TRUE);
	}
	void Play(SoundEnum ID, int Pan = -256, int Size = -1) noexcept {
		for (auto& p : m_Pool) {
			if (p.m_DupricateID != -1) {
				if (CheckSoundMem(p.m_DupricateID) == 1) { continue; }
				p.Init();
			}
			p.m_DupricateID = DuplicateSoundMem(m_Base[static_cast<int>(ID)]);

			ChangePanSoundMem((Pan == -256) ? 0 : Pan, p.m_DupricateID);
			ChangeVolumeSoundMem((Size == -1) ? 255 : Size, p.m_DupricateID);
			PlaySoundMem(p.m_DupricateID, DX_PLAYTYPE_BACK, TRUE);
			return;
		}
		m_Pool.emplace_back();
		auto& p = m_Pool.back();
		p.m_DupricateID = DuplicateSoundMem(m_Base[static_cast<int>(ID)]);

		ChangePanSoundMem((Pan == -256) ? 0 : Pan, p.m_DupricateID);
		ChangeVolumeSoundMem((Size == -1) ? 255 : Size, p.m_DupricateID);
		PlaySoundMem(p.m_DupricateID, DX_PLAYTYPE_BACK, TRUE);
	}
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

//キャラクター
enum class CharaType {
	Team,
	TeamNPC,
	Enemy,
};

class CharacterBase {
private:
	CharaType	m_CharaType{};
	int			m_GraphHandle{};
	int			m_WatchHandle{};

	bool		m_isDraw{ true };
	bool		m_isIntercept{ false };

	bool		m_isMoving{ false };
	float		m_DrawAlpha{ 1.f };

	size_t		m_PointLightID{ SIZE_MAX };
	VECTOR2D	m_Pos{};
	VECTOR2D	m_DrawPos{};
	VECTOR2D	m_MoveAimPos{};
	VECTOR2D	m_AimPos{};
	VECTOR2D	m_TargetPos{};
	float		m_Rad{};
	float		m_DrawRad{};
	bool		m_isTargetChanged = false;

	int										TargetPathPlanningIndex{ -1 };		// 次の中間地点となる経路上のポリゴンの経路探索情報が格納されているメモリアドレスを格納する変数
	PathChecker								m_PathChecker{};

	VECTOR2D	m_FogMoveRandom{};
	float		m_MoveTimer = 0.f;
	float		m_MoveTimerWave = 0.f;

	float		m_ShotTimer = 0.f;
	bool		m_IsShot = false;

	bool		m_IsAlive = true;
public:
	std::array<int, 10>	m_WayPointList{};
public:
	const auto GetCharaType() const noexcept { return m_CharaType; }
	const auto IsEnemy() const noexcept { return m_CharaType == CharaType::Enemy; }
	const auto GetPos() const noexcept { return m_DrawPos; }
	const auto GetRad() const noexcept { return m_DrawRad; }
	const auto IsDraw() const noexcept { return m_isDraw; }
	const auto IsMoving() const noexcept { return m_isMoving; }
	const auto IsIntercept() const noexcept { return m_isIntercept; }
	const auto IsShot() const noexcept { return m_IsShot; }
	const auto IsAlive() const noexcept { return m_IsAlive; }

	void SetShotReset() noexcept {
		m_ShotTimer = 0.f;
		m_IsShot = false;
	}

	void Death(int killID) noexcept {
		m_IsAlive = false;
	}

	void SetDraw(bool isDraw) noexcept {
		m_isDraw = isDraw;
	}
	void SetIntercept(bool isDraw) noexcept {
		m_isIntercept = isDraw;
	}
	void SetMoveAimPos(VECTOR2D Pos) noexcept {
		m_MoveAimPos = Pos;
	}
	void SetAimPos(VECTOR2D Pos) noexcept {
		m_AimPos = Pos;
	}
	void SetTargetPos(VECTOR2D Pos) noexcept {
		m_TargetPos = Pos;
		m_isTargetChanged = true;
	}
public:
	void Init(VECTOR2D Pos, float Yrad, CharaType Type) noexcept {
		auto* drawcontrol = DrawControl::Instance();
		m_Pos = Pos;
		m_DrawPos = Pos;

		m_Rad = Yrad;
		m_DrawRad = Yrad;
		SetMoveAimPos(Pos);
		SetAimPos(Pos);
		SetTargetPos(Pos);
		m_CharaType = Type;
		switch (m_CharaType) {
		case CharaType::Team:
			m_PointLightID = drawcontrol->AddPointLight(VGet(Pos.x, Pos.y, 0.f));
			m_GraphHandle = LoadGraph("data/TeamChara.bmp");
			break;
		case CharaType::TeamNPC:
			m_GraphHandle = LoadGraph("data/TeamCharaNPC.bmp");
			break;
		case CharaType::Enemy:
			m_GraphHandle = LoadGraph("data/EnemyChara.bmp");
			break;
		default:
			break;
		}
		m_WatchHandle = LoadGraph("data/Watch.png");

		for (auto& w : m_WayPointList) {
			w = -1;
		}
		m_isMoving = false;
		m_IsAlive = true;
	}
	void Update() noexcept {
		auto* drawcontrol = DrawControl::Instance();

		m_DrawAlpha = std::clamp(m_DrawAlpha + ((m_isDraw || !m_IsAlive) ? 1.f / 0.1f : -1.f) / BaseFrameRate, 0.f, 1.f);
		m_MoveTimer = std::max(m_MoveTimer - 1.f / BaseFrameRate, 0.f);
		m_MoveTimerWave = std::max(m_MoveTimerWave - 1.f / BaseFrameRate, 0.f);
		if (!m_IsAlive) { return; }

		if (m_isTargetChanged) {
			m_isTargetChanged = false;
			//経路探索しなおす
			m_PathChecker.Dispose();
			if (m_PathChecker.Init(m_Pos, m_TargetPos)) {	// 指定の２点の経路情報を探索する
				TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetPolyIndex();	// 移動開始時点の移動中間地点の経路探索情報もスタート地点にあるポリゴンの情報
			}
		}
		//経路に沿って移動
		{
			VECTOR2D Pos = m_PathChecker.GetNextPoint(m_Pos, &TargetPathPlanningIndex);
			VECTOR2D Vec(Pos.x - m_Pos.x, Pos.y - m_Pos.y);
			float Length = std::hypotf(Vec.x, Vec.y);
			if (Length > 0.5f) {
				float speed = 0.1f;
				Vec.x *= speed / Length;
				Vec.y *= speed / Length;
				m_Pos.x += Vec.x;
				m_Pos.y += Vec.y;

				m_Rad = std::atan2f(Vec.x, -Vec.y);

				m_isMoving = true;
			}
			else {
				m_Pos = Pos;
				m_isMoving = false;
			}
			//
			{
				VECTOR2D Pos;
				if (IsIntercept()) {
					Pos = m_MoveAimPos;
				}
				else {
					Pos = m_AimPos;
				}

				VECTOR2D AimVec(Pos.x - m_Pos.x, Pos.y - m_Pos.y);
				float AimLength = std::hypotf(AimVec.x, AimVec.y);
				if (AimLength > 0.5f) {
					m_Rad = std::atan2f(AimVec.x, -AimVec.y);
				}
			}
		}
		//近似
		{
			float Per = (1.f - 0.9f);
			m_DrawPos.x = m_DrawPos.x + (m_Pos.x - m_DrawPos.x) * Per;
			m_DrawPos.y = m_DrawPos.y + (m_Pos.y - m_DrawPos.y) * Per;

			//m_isMoving = std::hypotf(m_Pos.x - m_DrawPos.x, m_Pos.y - m_DrawPos.y) >= 0.1f;

			float Diff = m_DrawRad - m_Rad;
			if (Diff > DX_PI_F) {
				Diff -= DX_PI_F * 2;
			}
			if (Diff < -DX_PI_F) {
				Diff += DX_PI_F * 2;
			}
			if (Diff > 1.f * DX_PI_F / 180.f) {
				m_DrawRad -= 3.f * DX_PI_F / 180.f;
			}
			else if (Diff < -1.f * DX_PI_F / 180.f) {
				m_DrawRad += 3.f * DX_PI_F / 180.f;
			}

			if (m_DrawRad > DX_PI_F) {
				m_DrawRad -= DX_PI_F * 2;
			}
			if (m_DrawRad < -DX_PI_F) {
				m_DrawRad += DX_PI_F * 2;
			}
		}
		//
		if (m_PointLightID != SIZE_MAX) {
			VECTOR LightPos = drawcontrol->GetPointLight(m_PointLightID);
			LightPos.x = m_DrawPos.x;
			LightPos.y = m_DrawPos.y;
			drawcontrol->SetPointLight(m_PointLightID, LightPos);
		}
		if (m_MoveTimer == 0.f) {
			if (IsMoving()) {
				m_MoveTimer = 0.5f;
				auto* maincontrol = DXDraw::Instance();
				auto* soundPool = SoundPool::Instance();
				VECTOR MP = drawcontrol->Get2DPos(m_DrawPos.x, m_DrawPos.y, 0.f);
				int Pan = 255 * (static_cast<int>(MP.x) - (1920 / 2)) / (1920 / 2);
				int Scale = 255 - std::abs(255 * (static_cast<int>(std::hypotf(MP.x - static_cast<float>(1920 / 2), MP.y - static_cast<float> (1080 / 2)))) / (1920 / 2)) / 2;
				soundPool->Play(SoundEnum::Walk, Pan, Scale);
			}
		}
		//ランダム形成
		if (m_MoveTimerWave == 0.f) {
			if (!m_isDraw && IsMoving()) {
				m_MoveTimerWave = 0.5f;
				m_FogMoveRandom.x = m_DrawPos.x + static_cast<float>(GetRand(200) - 100) / 100.f * 3.f;
				m_FogMoveRandom.y = m_DrawPos.y + static_cast<float>(GetRand(200) - 100) / 100.f * 3.f;

				m_FogMoveRandom.x = std::clamp(m_FogMoveRandom.x, 0.f, static_cast<float>(drawcontrol->GetMapXsize()));
				m_FogMoveRandom.y = std::clamp(m_FogMoveRandom.y, 0.f, static_cast<float>(drawcontrol->GetMapYsize()));
			}
		}
		//
		m_IsShot = false;
		m_ShotTimer += 1.f / BaseFrameRate;
		if (m_ShotTimer > 0.1f) {
			m_ShotTimer -= 0.1f;
			m_IsShot = true;
		}
	}
	void Draw() const noexcept {
		auto* drawcontrol = DrawControl::Instance();
		VECTOR EyeTargetPos = drawcontrol->GetCameraPos();
		VECTOR Pos = drawcontrol->Get2DPos(m_DrawPos.x, m_DrawPos.y, 0.f);

		if (m_IsAlive) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(128 * m_DrawAlpha));
			SetDrawBright(150, 200, 255);
			DrawCircleGauge((int)Pos.x, (int)Pos.y,
				static_cast<double>(m_DrawRad * 100.f / (DX_PI_F * 2.f) + 70.f * 100.f / 360.f),
				m_WatchHandle,
				static_cast<double>(m_DrawRad * 100.f / (DX_PI_F * 2.f) - 70.f * 100.f / 360.f),
				static_cast<double>(EyeTargetPos.z * 100.f / 128.f)
			);
			SetDrawBright(255, 255, 255);
		}
		if (!m_IsAlive) {
			SetDrawBright(128, 128, 128);
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * m_DrawAlpha));
		DrawRotaGraph((int)Pos.x, (int)Pos.y, static_cast<double>(EyeTargetPos.z), static_cast<double>(m_DrawRad), m_GraphHandle, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		if (!m_IsAlive) {
			SetDrawBright(255, 255, 255);
		}
		if (m_MoveTimerWave > 0.f) {
			VECTOR Pos2 = drawcontrol->Get2DPos(m_FogMoveRandom.x, m_FogMoveRandom.y, 0.f);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(128 * m_MoveTimerWave / 0.5f));
			DrawCircle((int)Pos2.x, (int)Pos2.y, (int)(32.f * (0.5f - m_MoveTimerWave) / 0.5f), GetColor(192, 192, 192), FALSE, 2);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
	void Dispose() noexcept {
		DeleteGraph(m_GraphHandle);
		DeleteGraph(m_WatchHandle);
		m_PathChecker.Dispose();
	}
};


class AmmoEffectPool {
	struct Ammo {
		VECTOR2D m_Repos{};
		VECTOR2D m_Pos{};
		VECTOR2D m_Vec{};
		float m_Timer{};
		float m_HitFlash{};
	public:
		bool IsActive() const noexcept { return m_Timer > 0.f; }
	};
private:
	std::vector<Ammo> m_Ammo;
public:
	void Add(VECTOR2D Pos, VECTOR2D Vec) noexcept {
		//正規化
		float Length = std::hypotf(Vec.x, Vec.y);
		if (Length != 0.f) {
			Vec.x = Vec.x / Length;
			Vec.y = Vec.y / Length;
		}
		else {
			Vec.x = 0.f;
			Vec.y = 1.f;
		}
		for (auto& a : m_Ammo) {
			if (a.IsActive() || a.m_HitFlash > 0.f) { continue; }
			a.m_Pos = Pos;
			a.m_Repos = Pos;
			a.m_Vec = Vec;
			a.m_Timer = 1.f;
			return;
		}
		m_Ammo.emplace_back();
		auto& a = m_Ammo.back();
		a.m_Pos = Pos;
		a.m_Repos = Pos;
		a.m_Vec = Vec;
		a.m_Timer = 1.f;
	}
public:
	void Init() noexcept {
		m_Ammo.resize(32);
		for (auto& a : m_Ammo) {
			a.m_Timer = 0.f;
			a.m_HitFlash = 0.f;
		}
	}
	void Update() noexcept {
		auto* drawcontrol = DrawControl::Instance();
		for (auto& a : m_Ammo) {
			if (a.m_Timer > 0.f) {
				a.m_Timer = std::max(a.m_Timer - 1.f / BaseFrameRate, 0.f);
				a.m_Repos = a.m_Pos;
				a.m_Pos.x += a.m_Vec.x * (50.f / BaseFrameRate);
				a.m_Pos.y += a.m_Vec.y * (50.f / BaseFrameRate);
				/*
				if (!drawcontrol->CheckPolyMove(a.m_Repos, a.m_Pos)) {
					a.m_Timer = 0.f;
				}
				//*/
				if (a.m_Timer == 0.f) {
					a.m_HitFlash = 0.3f;
				}
			}
			if (a.m_HitFlash > 0.f) {
				a.m_HitFlash = std::max(a.m_HitFlash - 1.f / BaseFrameRate, 0.f);
			}
		}
	}
	void Draw() noexcept {
		auto* drawcontrol = DrawControl::Instance();
		for (auto& a : m_Ammo) {
			if (a.m_Timer > 0.f) {
				VECTOR MP = drawcontrol->Get2DPos(a.m_Repos.x, a.m_Repos.y, 0.f);
				VECTOR MP2 = drawcontrol->Get2DPos(a.m_Pos.x, a.m_Pos.y, 0.f);
				DrawLine((int)MP.x, (int)MP.y, (int)MP2.x, (int)MP2.y, GetColor(192, 192, 0), 3);
			}
			if (a.m_HitFlash > 0.f) {
				VECTOR MP = drawcontrol->Get2DPos(a.m_Pos.x, a.m_Pos.y, 0.f);
				VECTOR MP2 = drawcontrol->Get2DPos(a.m_Pos.x, a.m_Pos.y + 1.f, 0.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f * a.m_HitFlash / 0.3f));
				DrawCircle((int)MP.x, (int)MP.y, (int)((MP2.y - MP.y) * (0.3f - a.m_HitFlash) / 0.3f), GetColor(192, 192, 0), false, 3);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	}
	void Dispose() noexcept {
		m_Ammo.clear();
	}
};


class SideLog {
private://シングルトン
	static const SideLog* m_Singleton;
public:
	static void Create(void) noexcept {
		m_Singleton = new SideLog();
	}
	static SideLog* Instance(void) noexcept {
		if (m_Singleton == nullptr) {
			MessageBox(NULL, "Failed Instance Create", "", MB_OK);
			exit(-1);
		}
		// if (m_Singleton == nullptr) { m_Singleton = new SideLog(); }
		return (SideLog*)m_Singleton;
	}
	static void Release(void) noexcept {
		delete m_Singleton;
	}
private:
	struct Pool {
		int kill = -1;
		int death = -1;
		float m_Timer{ -1.f };
	public:
		void Init() {
			m_Timer = -1.f;
		}
	};
private:
	std::vector<Pool> m_Pool;
private:
	//コンストラクタ
	SideLog(void) noexcept {
		m_Pool.reserve(32);
	}
	//デストラクタ
	~SideLog(void) noexcept {
		m_Pool.clear();
	}
public:
	void Add(int KillID, int DeathID) noexcept {
		m_Pool.emplace_back();
		auto& p = m_Pool.back();
		p.m_Timer = 3.f;
		p.kill = KillID;
		p.death = DeathID;
	}
public:
	void Update() noexcept {
		for (int loop = 0; loop < m_Pool.size(); ++loop) {
			auto& p = m_Pool.at(loop);
			p.m_Timer = std::max(p.m_Timer - 1.f / BaseFrameRate, 0.f);
			if (p.m_Timer == 0.f) {
				m_Pool.erase(m_Pool.begin() + loop);
				--loop;
			}
		}
	}
	void Draw(int X, int Y) noexcept {
		auto* rule = Rule::Instance();

		int Max = static_cast<int>(m_Pool.size());

		int Xpos = X;
		int Ypos = Y;

		int Xsize = 320;
		int Ysize = 32;

		Ypos -= (Ysize + 6) * Max;

		for (auto& p : m_Pool) {
			bool TeamWin = (p.kill < rule->GetPlayerAll() / 2);
			Xpos = X;
			float Per = 0.f;
			if (p.m_Timer > 2.75f) {
				Per = (p.m_Timer - 2.75f) / (3.f - 2.75f);
			}
			if (p.m_Timer < 0.25f) {
				Per = 1.f - (p.m_Timer - 0.f) / (0.25f - 0.f);
			}
			Xpos -= static_cast<int>((Per * Per) * Xsize);

			DrawBox(Xpos, Ypos - 1, Xpos + Xsize, Ypos + Ysize + 1, GetColor(255, 255, 255), TRUE);
			DrawBox(Xpos, Ypos, Xpos + Xsize / 2, Ypos + Ysize, TeamWin ? GetColor(0, 255, 0) : GetColor(255, 0, 0), TRUE);
			DrawBox(Xpos + Xsize / 2, Ypos, Xpos + Xsize, Ypos + Ysize, TeamWin ? GetColor(255, 0, 0) : GetColor(0, 255, 0), TRUE);
			char Text[64] = "";
			if (TeamWin) {
				sprintfDx(Text, "Team%d", p.kill);
			}
			else {
				sprintfDx(Text, "Enemy%d", p.kill);
			}
			DrawString(Xpos + Xsize / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) - 8, Ypos + Ysize / 2 - 18 / 2, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));

			if (TeamWin) {
				sprintfDx(Text, "Enemy%d", p.death);
			}
			else {
				sprintfDx(Text, "Team%d", p.death);
			}
			DrawString(Xpos + Xsize / 2 + 8, Ypos + Ysize / 2 - 18 / 2, Text, GetColor(255, 255, 255), GetColor(0, 0, 0));
			Ypos += Ysize + 6;
		}
	}
};

enum class Match {
	Ready,
	Game,
	TeamWin,
	EnemyWin,
	Draw,
};
class MainScene {
	int Width = 24;
	int Height = 16;

	AmmoEffectPool AmmoEffect;
	int BGGraphHandle{ -1 };
	int BufferHandle{ -1 };
	VECTOR EyeTargetPos{};
	VECTOR EyePos{};
	std::vector<CharacterBase> Chara{};
	int m_NowMoveCharacter = 0;
	bool m_MovingCharacter = false;
	float m_MovingTimer = 0.1f;

	int SpeedRate = 1;
	bool PrevPressUp = false;
	bool PrevPressDown = false;

	bool PrevMouseLeftPress{};
	VECTOR2D MouseLeftHoldPos{};
	bool LeftPressCancel{};

	int TeamAlive = 0;
	int EnemyAlive = 0;

	float TeamAliveR = 0.f;
	float EnemyAliveR = 0.f;

	Match m_MatchNow = Match::Ready;

	float ReadyTimer = 4.f;
	float EndTimer = 0.f;

	float TotalTimer = 90.f;
	float Timer = TotalTimer;

	int MX = 0;
	int MY = 0;
	VECTOR2D MousePos{};
public:
	void Init() noexcept {
		DrawControl::Create(1920, 1080);
		SideLog::Create();
		Rule::Create();
		auto* drawcontrol = DrawControl::Instance();
		auto* soundPool = SoundPool::Instance();
		auto* sideLog = SideLog::Instance();
		auto* rule = Rule::Instance();

		AmmoEffect.Init();
		//設定
		//マップデータ作成
		{
			int SoftImageHandle = -1;

			SoftImageHandle = LoadSoftImage("data/map.bmp");

			GetSoftImageSize(SoftImageHandle, &Width, &Height);
			Width /= 2;
			Height /= 2;

			drawcontrol->SetPicture("data/Tex3.bmp");
			drawcontrol->SetMapSize(Width, Height);
			for (int x = 0; x < Width; x++) {
				for (int y = 0; y < Height; y++) {
					int r, g, b;
					GetPixelSoftImage(SoftImageHandle, x * 2, y * 2, &r, &g, &b, NULL);
					if (r == 0 && g == 0 && b == 0) {
						drawcontrol->SetTileData(x, y)->Height = 8;
					}
					else {
						drawcontrol->SetTileData(x, y)->Height = 0;
					}
					if (r == 0 && g == 255 && b == 0) {
						drawcontrol->SetTileData(x, y)->FloorID = 1;
					}
					else if (r == 255 && g == 0 && b == 0) {
						drawcontrol->SetTileData(x, y)->FloorID = 2;
					}
					else {
						drawcontrol->SetTileData(x, y)->FloorID = (GetRand(100) > 50) ? 3 : 0;
					}
					drawcontrol->SetTileData(x, y)->WayPointRad = -1;
					if ((r == 128 && g == 128 && b == 128) || (r == 192 && g == 192 && b == 192)) {
						drawcontrol->SetTileData(x, y)->WayPointRad = 0;
						int Color[2][2] = {};
						for (int xl = 0; xl < 2; xl++) {
							for (int yl = 0; yl < 2; yl++) {
								GetPixelSoftImage(SoftImageHandle, x * 2 + xl, y * 2 + yl, &r, &g, &b, NULL);
								if (r == 192 && g == 192 && b == 192) {
									Color[xl][yl] = 0;
								}
								if (r == 128 && g == 128 && b == 128) {
									Color[xl][yl] = 1;
								}
							}
						}
						if (Color[0][0] == 0 && Color[1][0] == 0) {
							drawcontrol->SetTileData(x, y)->WayPointRad = 180;
						}
						if (Color[1][0] == 0 && Color[1][1] == 0) {
							drawcontrol->SetTileData(x, y)->WayPointRad = 90;
						}
						if (Color[1][1] == 0 && Color[0][1] == 0) {
							drawcontrol->SetTileData(x, y)->WayPointRad = 0;
						}
						if (Color[0][1] == 0 && Color[0][0] == 0) {
							drawcontrol->SetTileData(x, y)->WayPointRad = -90;
						}
					}
				}
			}
			DeleteSoftImage(SoftImageHandle);


			//経路探索
			drawcontrol->SetupLink();
		}

		BGGraphHandle = LoadGraph("data/bg.png");

		BufferHandle = MakeScreen(1920, 1080, false);

		Chara.resize(rule->GetPlayerAll());
		for (int loop = 0; loop < rule->GetPlayerAll(); ++loop) {
			if (loop < rule->GetPlayerAll() / 2 - rule->GetPlayablePlayer()) {
				int Index = loop;
				Chara.at(loop).Init(VECTOR2D(static_cast<float>(Index), 0.f), DX_PI_F, CharaType::TeamNPC);
			}
			else if (loop < rule->GetPlayerAll() / 2) {
				int Index = loop;
				Chara.at(loop).Init(VECTOR2D(static_cast<float>(Index), 0.f), DX_PI_F, CharaType::Team);
			}
			else {
				int Index = loop - (rule->GetPlayerAll() / 2);
				Chara.at(loop).Init(VECTOR2D(static_cast<float>(Width - 1 - Index), static_cast<float>(Height - 1)), 0.f, CharaType::Enemy);
			}
		}


		m_NowMoveCharacter = 0;
		m_MovingCharacter = false;
		m_MovingTimer = 0.1f;

		SpeedRate = 1;
		PrevPressUp = false;
		PrevPressDown = false;

		drawcontrol->SetCameraPos(VGet(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y, 1.f));					//視点の場所(x,y,高さ)
		EyeTargetPos = drawcontrol->GetCameraPos();
		EyePos = EyeTargetPos;

		soundPool->PlayLoop(SoundEnum::Env);

		LeftPressCancel = false;

		TeamAlive = 0;
		EnemyAlive = 0;

		TeamAliveR = 0.f;
		EnemyAliveR = 0.f;


		m_MatchNow = Match::Ready;

		ReadyTimer = 4.f;
		EndTimer = 0.f;

		TotalTimer = 90.f;
		Timer = TotalTimer;
	}
	bool Update() noexcept {
		auto* maincontrol = DXDraw::Instance();
		auto* drawcontrol = DrawControl::Instance();
		auto* soundPool = SoundPool::Instance();
		auto* sideLog = SideLog::Instance();
		auto* rule = Rule::Instance();
		//カメラの位置を設定
		{
			float speed = 0.08f;
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_W) != 0) {
				EyeTargetPos.y += -speed;
			}
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_S) != 0) {
				EyeTargetPos.y += speed;
			}
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_A) != 0) {
				EyeTargetPos.x += -speed;
			}
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_D) != 0) {
				EyeTargetPos.x += speed;
			}
			if (GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
				if (!Chara.at(m_NowMoveCharacter).IsEnemy()) {
					EyeTargetPos.x = Chara.at(m_NowMoveCharacter).GetPos().x;
					EyeTargetPos.y = Chara.at(m_NowMoveCharacter).GetPos().y;
				}
			}

			EyeTargetPos.x = std::clamp(EyeTargetPos.x, 0.f, static_cast<float>(Width));
			EyeTargetPos.y = std::clamp(EyeTargetPos.y, 0.f, static_cast<float>(Height));

			//カメラ拡大縮小
			int MouseWheelRot = 0;
			if (GetWindowActiveFlag()) {
				MouseWheelRot = GetMouseWheelRotVol();
			}
			else {
				GetMouseWheelRotVol();//前回との差分が入る仕組みなのでアクティブでない場合も通してはおくように
			}
			EyeTargetPos.z = std::clamp(EyeTargetPos.z + static_cast<float>(MouseWheelRot) * 0.1f, 0.5f, 1.f);


			float Per = (1.f - 0.9f);
			EyePos.x = EyePos.x + (EyeTargetPos.x - EyePos.x) * Per;
			EyePos.y = EyePos.y + (EyeTargetPos.y - EyePos.y) * Per;
			EyePos.z = EyePos.z + (EyeTargetPos.z - EyePos.z) * Per;
			drawcontrol->SetCameraPos(EyePos);
		}
		//マウス座標を取得しておく
		GetMousePoint(&MX, &MY);
		MX = MX * 1920 / maincontrol->GetDispX();
		MY = MY * 1080 / maincontrol->GetDispY();

		VECTOR MPos = drawcontrol->GetPos2D(static_cast<float>(MX), static_cast<float>(MY));
		MX = std::clamp(static_cast<int>(MPos.x + 0.5f), 0, Width);
		MY = std::clamp(static_cast<int>(MPos.y + 0.5f), 0, Height);
		MousePos = VECTOR2D(static_cast<float>(MX), static_cast<float>(MY));
		for (int speedScale = 0; speedScale < SpeedRate; ++speedScale) {
			{
				TeamAlive = 0;
				EnemyAlive = 0;
				for (auto& c : Chara) {
					if (!c.IsAlive()) { continue; }
					if (!c.IsEnemy()) {
						TeamAlive++;
					}
					else {
						EnemyAlive++;
					}
				}
			}
			if (m_MatchNow != Match::Ready) {
				//
				for (auto& c : Chara) {
					c.Update();
				}
				if (m_MatchNow == Match::Game) {
					if (m_MovingCharacter) {
						Timer = std::max(Timer - 1.f / BaseFrameRate, 0.f);
					}
					{
						if (Timer == 0.f) {
							m_MatchNow = Match::Draw;
							soundPool->Play(SoundEnum::End);
						}
						if (TeamAlive != 0 && EnemyAlive == 0) {
							m_MatchNow = Match::TeamWin;
							soundPool->Play(SoundEnum::End);
						}
						if (TeamAlive == 0 && EnemyAlive != 0) {
							m_MatchNow = Match::EnemyWin;
							soundPool->Play(SoundEnum::End);
						}
					}
					for (auto& c : Chara) {
						c.SetIntercept(false);
					}
					//
					if (!Chara.at(m_NowMoveCharacter).IsAlive()) {
						for (int loop = 0; loop < static_cast<int>(Chara.size()); ++loop) {
							++m_NowMoveCharacter %= static_cast<int>(Chara.size());
							if (Chara.at(m_NowMoveCharacter).IsAlive()) {
								break;
							}
						}
					}
					if (m_MovingCharacter) {
						m_MovingTimer = std::max(m_MovingTimer - 1.f / BaseFrameRate, 0.f);
						if (m_MovingTimer == 0.f && !Chara.at(m_NowMoveCharacter).IsMoving()) {
							for (int loop = 0; loop < static_cast<int>(Chara.size()); ++loop) {
								++m_NowMoveCharacter %= static_cast<int>(Chara.size());
								if (Chara.at(m_NowMoveCharacter).IsAlive()) {
									break;
								}
							}
							m_MovingCharacter = false;
						}
					}
					//射撃判定ついでに視認判定
					if (m_MovingCharacter) {
						for (auto& c : Chara) {
							if (!c.IsAlive()) { continue; }
							if (!c.IsEnemy()) { continue; }
							c.SetDraw(false);
							//c.SetDraw(true);
						}
						for (auto& c : Chara) {
							if (!c.IsAlive()) { continue; }
							int index = static_cast<int>(&c - &Chara.front());
							bool IsPeeker = (m_NowMoveCharacter == static_cast<int>(&c - &Chara.front()));//移動中のキャラである
							float Rad = 45.f * DX_PI_F / 180.f;
							int TargetID = -1;
							//見えている敵の中で自身が視線を通していて一番角度差が小さい敵を探す
							for (auto& tgt : Chara) {
								if (!tgt.IsAlive()) { continue; }
								if (c.IsEnemy() == tgt.IsEnemy()) { continue; }
								int indextgt = static_cast<int>(&tgt - &Chara.front());
								if (!drawcontrol->CheckPolyMoveWidth(tgt.GetPos(), c.GetPos(), 0.1f)) { continue; }
								VECTOR2D AimVec(tgt.GetPos().x - c.GetPos().x, tgt.GetPos().y - c.GetPos().y);
								float AimLength = std::hypotf(AimVec.x, AimVec.y);
								if (AimLength > 3.f) {
									float Diff = c.GetRad() - std::atan2f(AimVec.x, -AimVec.y);
									if (Diff > DX_PI_F) {
										Diff -= DX_PI_F * 2;
									}
									if (Diff < -DX_PI_F) {
										Diff += DX_PI_F * 2;
									}
									if (!(std::fabsf(Diff) <= 45.f * DX_PI_F / 180.f)) { continue; }
									if (Rad > Diff) {
										Rad = Diff;
										TargetID = indextgt;
									}
									tgt.SetDraw(true);
								}
								else {
									Rad = 0.f;
									TargetID = indextgt;
									tgt.SetDraw(true);
								}
							}
							if (TargetID == -1) {
								c.SetShotReset();
								continue;
							}
							//その敵に向く
							auto& tgt = Chara.at(TargetID);
							c.SetMoveAimPos(tgt.GetPos());
							c.SetIntercept(true);
							//shot
							if (c.IsShot()) {
								//射撃エフェクトを出す
								VECTOR2D Vec(tgt.GetPos().x - c.GetPos().x, tgt.GetPos().y - c.GetPos().y);
								AmmoEffect.Add(c.GetPos(), Vec);
								VECTOR MP = drawcontrol->Get2DPos(c.GetPos().x, c.GetPos().y, 0.f);
								int Pan = 255 * (static_cast<int>(MP.x) - (1920 / 2)) / (1920 / 2);
								int Scale = 255 - std::abs(255 * (static_cast<int>(std::hypotf(MP.x - static_cast<float>(1920 / 2), MP.y - static_cast<float> (1080 / 2)))) / (1920 / 2)) / 2;
								soundPool->Play(SoundEnum::Shot, Pan, Scale);
								//判定する
								if (GetRand(100) < (IsPeeker ? 55 : 30)) {
									//
									tgt.Death(index);
									soundPool->Play(SoundEnum::Hit);

									sideLog->Add(index, TargetID);
								}
							}
						}
					}
					//味方は強制表示
					for (auto& c : Chara) {
						if (!c.IsAlive()) { continue; }
						if (c.IsEnemy()) { continue; }
						c.SetDraw(true);
					}
					//敵行動
					for (auto& en : Chara) {
						if (!en.IsAlive()) { continue; }
						if (en.GetCharaType() == CharaType::Team) { continue; }
						bool CheckTarget = false;
						/*
						if (en.m_WayPointList.at(0) != -1) {
							auto& Target = drawcontrol->GetTileData(en.m_WayPointList.at(0)).GetPos();
							float Length = std::hypotf(Target.x - en.GetPos().x, Target.y - en.GetPos().y);
							if (Length < 0.5f) {
								CheckTarget = true;
							}
						}
						else {
							CheckTarget = true;
						}
						//*/
						if (!m_MovingCharacter) {
							if (m_NowMoveCharacter == static_cast<int>(&en - &Chara.front())) {
								m_MovingCharacter = true;
								CheckTarget = true;
								m_MovingTimer = 0.4f;
							}
						}
						if (!CheckTarget) { continue; }
						float LengthBase = 5.f;
						while (true) {
							int X{}, Y{};
							float LengthReq = LengthBase;
							for (int x = 0; x < Width; x++) {
								for (int y = 0; y < Height; y++) {
									auto* pNow = drawcontrol->SetTileData(x, y);
									if ((pNow->Height == 0) && (pNow->WayPointRad != -1)) {
										{
											bool isHitWay = false;
											//違うルートを通る
											if (LengthReq <= 7.5f) {
												for (auto& w : en.m_WayPointList) {
													if (pNow->GetIndex() == w) {
														isHitWay = true;
														break;
													}
												}
											}
											if (isHitWay) { continue; }
											//敵と違うルートを通る
											for (auto& en2 : Chara) {
												if (!en2.IsAlive()) { continue; }
												if (&en == &en2) { continue; }
												if (pNow->GetIndex() == en2.m_WayPointList.at(0)) {
													isHitWay = true;
													break;
												}
											}
											if (isHitWay) { continue; }
										}

										float Rad = en.GetRad();
										if ((en.IsEnemy() && TeamAlive <= 2) ||
											(!en.IsEnemy() && EnemyAlive <= 2)
											) {
											//一番近い敵の方向
											for (auto& tgt : Chara) {
												if (!tgt.IsAlive()) { continue; }
												if (tgt.IsEnemy() != en.IsEnemy()) { continue; }

												VECTOR2D Vec(tgt.GetPos().x - en.GetPos().x, tgt.GetPos().y - en.GetPos().y);
												float Length = std::hypotf(Vec.x, Vec.y);
												if (Length > 0.f) {
													Rad = std::atan2f(Vec.x, -Vec.y);
												}
												break;
											}
										}
										{
											auto& Target = pNow->GetPos();
											VECTOR2D AimVec(Target.x - en.GetPos().x, Target.y - en.GetPos().y);
											float Diff = Rad - std::atan2f(AimVec.x, -AimVec.y);
											if (Diff > DX_PI_F) {
												Diff -= DX_PI_F * 2;
											}
											if (Diff < -DX_PI_F) {
												Diff += DX_PI_F * 2;
											}
											if (!(std::fabsf(Diff) <= 70.f * DX_PI_F / 180.f)) {
												continue;
											}
										}
										//*
										{
											float NowRad =
												(en.m_WayPointList.at(0) == -1) ?
												Rad :
												static_cast<float>(drawcontrol->GetTileData(en.m_WayPointList.at(0)).WayPointRad) * DX_PI_F / 180.f;

											float rad = static_cast<float>(pNow->WayPointRad) * DX_PI_F / 180.f;
											float Diff = NowRad - rad;
											if (Diff > DX_PI_F) {
												Diff -= DX_PI_F * 2;
											}
											if (Diff < -DX_PI_F) {
												Diff += DX_PI_F * 2;
											}
											if (!(std::fabsf(Diff) <= 120.f * DX_PI_F / 180.f)) {
												continue;
											}
										}
										//*/
										float Length = std::hypotf(static_cast<float>(x) - en.GetPos().x, static_cast<float>(y) - en.GetPos().y);
										if (LengthReq > Length) {
											LengthReq = Length;
											X = x;
											Y = y;
										}
									}
								}
							}
							if (LengthReq != LengthBase) {
								for (int loop = static_cast<int>(en.m_WayPointList.size()) - 1; loop >= 1; --loop) {
									en.m_WayPointList.at(loop) = en.m_WayPointList.at(static_cast<size_t>(loop - 1));
								}

								en.m_WayPointList.at(0) = drawcontrol->SetTileData(X, Y)->GetIndex();
								en.SetTargetPos(VECTOR2D(static_cast<float>(X), static_cast<float>(Y)));
								float rad = static_cast<float>(drawcontrol->SetTileData(X, Y)->WayPointRad) * DX_PI_F / 180.f;
								VECTOR2D Vec(std::sin(rad), std::cos(rad));
								en.SetAimPos(VECTOR2D(static_cast<float>(X) + Vec.x, static_cast<float>(Y) + Vec.y));
								en.SetMoveAimPos(VECTOR2D(static_cast<float>(X) + Vec.x, static_cast<float>(Y) + Vec.y));
								break;
							}
							LengthBase += 0.5f;
							if (LengthBase > 10.f) {
								break;
							}
						}
					}
					//操作
					if (!m_MovingCharacter) {
						if (Chara.at(m_NowMoveCharacter).GetCharaType() == CharaType::Team) {
							float Len = std::hypotf(MousePos.x - Chara.at(m_NowMoveCharacter).GetPos().x, MousePos.y - Chara.at(m_NowMoveCharacter).GetPos().y);
							bool IsActivePlace = (drawcontrol->SetTileData(MX, MY)->Height == 0);

							if (GetWindowActiveFlag() && (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) {
								LeftPressCancel = true;
							}
							bool LMP = (GetWindowActiveFlag() && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0);
							if (LMP && !LeftPressCancel) {
								if (!PrevMouseLeftPress) {
									if (!IsActivePlace || Len > 9.f) {
										LeftPressCancel = true;
									}
									MouseLeftHoldPos = MousePos;
								}
							}
							else {
								if (PrevMouseLeftPress) {
									if (!LeftPressCancel) {
										if (IsActivePlace) {
											Chara.at(m_NowMoveCharacter).SetAimPos(MousePos);
											Chara.at(m_NowMoveCharacter).SetMoveAimPos(MousePos);
											Chara.at(m_NowMoveCharacter).SetTargetPos(MouseLeftHoldPos);
											m_MovingCharacter = true;
											m_MovingTimer = 0.4f;
										}
									}
								}
								if (!LMP) {
									LeftPressCancel = false;
								}
							}
							PrevMouseLeftPress = LMP;
						}
						else {
							PrevMouseLeftPress = false;
						}
					}
				}
				//
				AmmoEffect.Update();
			}
			{
				TeamAliveR = TeamAliveR + (static_cast<float>(TeamAlive) - TeamAliveR) * 0.1f;
				EnemyAliveR = EnemyAliveR + (static_cast<float>(EnemyAlive) - EnemyAliveR) * 0.1f;
			}
			ReadyTimer = std::max(ReadyTimer - 1.f / BaseFrameRate, 0.f);

			switch (m_MatchNow) {
			case Match::Ready:
				if (ReadyTimer <= 1.f) {
					m_MatchNow = Match::Game;
					soundPool->Play(SoundEnum::Start);
				}
				break;
			case Match::Draw:
			case Match::TeamWin:
			case Match::EnemyWin:
				EndTimer = std::min(EndTimer + 1.f / BaseFrameRate, 2.f);
				break;
			default:
				break;
			}
			sideLog->Update();
		}
		{
			bool PressUp = GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_UP) != 0;
			bool PressDown = GetWindowActiveFlag() && CheckHitKey(KEY_INPUT_DOWN) != 0;

			if (PressUp && !PrevPressUp) {
				switch (SpeedRate) {
				case 0: SpeedRate = 1; break;
				case 1: SpeedRate = 2; break;
				case 2: SpeedRate = 4; break;
				case 4: SpeedRate = 4; break;
				default: break;
				}
			}
			if (PressDown && !PrevPressDown) {
				switch (SpeedRate) {
				case 0: SpeedRate = 0; break;
				case 1: SpeedRate = 0; break;
				case 2: SpeedRate = 1; break;
				case 4: SpeedRate = 2; break;
				default: break;
				}
			}
			PrevPressUp = PressUp;
			PrevPressDown = PressDown;
		}
		drawcontrol->Execute();
		if (EndTimer >= 2.f) {
			return false;
		}
		return true;
	}
	void Draw() noexcept {
		auto* maincontrol = DXDraw::Instance();
		auto* drawcontrol = DrawControl::Instance();
		auto* sideLog = SideLog::Instance();
		int PrevScreen = GetDrawScreen();
		{
			//描画
			SetDrawScreen(BufferHandle);
			ClearDrawScreen();
			{
				{
					//背景
					drawcontrol->Draw(1920, 1080);
					//弾
					AmmoEffect.Draw();
					//キャラ
					for (auto& c : Chara) {
						if (c.IsAlive()) { continue; }
						c.Draw();
					}
					for (auto& c : Chara) {
						if (!c.IsAlive()) { continue; }
						int index = static_cast<int>(&c - &Chara.front());
						if (m_NowMoveCharacter == index) { continue; }
						c.Draw();
					}
					if (Chara.at(m_NowMoveCharacter).IsAlive()) {
						Chara.at(m_NowMoveCharacter).Draw();
					}
					/*
					for (int x = 0; x < Width; x++) {
						for (int y = 0; y < Height; y++) {
							if (drawcontrol->SetTileData(x, y)->WayPointRad != -1) {
								VECTOR MP = drawcontrol->Get2DPos(x, y, 0.f);
								DrawCircle((int)MP.x, (int)MP.y, 16, GetColor(0, 255, 0), false, 3);

								float rad = static_cast<float>(drawcontrol->SetTileData(x, y)->WayPointRad) * DX_PI_F / 180.f;
								VECTOR MP2{};
								VECTOR2D Vec(std::sin(rad), std::cos(rad));
								float Length = std::hypotf(Vec.x, Vec.y);
								if (Length > 0.5f) {
									MP2.x = MP.x + Vec.x / Length * 64.f;
									MP2.y = MP.y + Vec.y / Length * 64.f;
								}
								DrawLine((int)MP.x, (int)MP.y, (int)MP2.x, (int)MP2.y, GetColor(0, 255, 0), 3);
							}
						}
					}
					//*/
					if (ReadyTimer > 1.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * (ReadyTimer - 1.f) / 3.f));
						DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					if (EndTimer > 1.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * (EndTimer - 1.f)));
						DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					if (m_MatchNow == Match::Game) {
						//カーソル
						if (!m_MovingCharacter) {
							if (Chara.at(m_NowMoveCharacter).GetCharaType() == CharaType::Team) {
								float Len = std::hypotf(MousePos.x - Chara.at(m_NowMoveCharacter).GetPos().x, MousePos.y - Chara.at(m_NowMoveCharacter).GetPos().y);


								if (PrevMouseLeftPress && !LeftPressCancel) {
									bool IsActivePlace = (drawcontrol->SetTileData(MX, MY)->Height == 0);
									VECTOR MP = drawcontrol->Get2DPos(MouseLeftHoldPos.x, MouseLeftHoldPos.y, 0.f);
									DrawCircle((int)MP.x, (int)MP.y, 16, IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), false, 3);

									VECTOR MP2 = drawcontrol->Get2DPos(MousePos.x, MousePos.y, 0.f);

									if (IsActivePlace) {
										DrawCircle((int)MP2.x, (int)MP2.y, 8, IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), false, 3);
									}

									VECTOR2D Vec(MP2.x - MP.x, MP2.y - MP.y);
									float Length = std::hypotf(Vec.x, Vec.y);
									if (Length > 0.5f) {
										MP2.x = MP.x + Vec.x / Length * 64.f;
										MP2.y = MP.y + Vec.y / Length * 64.f;
									}
									DrawLine((int)MP.x, (int)MP.y, (int)MP2.x, (int)MP2.y, IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), 3);
								}
								else {
									bool IsActivePlace = (drawcontrol->SetTileData(MX, MY)->Height == 0 && Len < 9.f);
									if (IsActivePlace) {
										VECTOR MP = drawcontrol->Get2DPos(MousePos.x, MousePos.y, 0.f);
										DrawCircle((int)MP.x, (int)MP.y, 16, IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), false, 3);
									}
									{
										VECTOR MP = drawcontrol->Get2DPos(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y, 0.f);
										VECTOR MP2 = drawcontrol->Get2DPos(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y + 9.f, 0.f);
										DrawCircle((int)MP.x, (int)MP.y, (int)(MP2.y - MP.y), IsActivePlace ? GetColor(0, 255, 0) : GetColor(255, 0, 0), false, 3);
									}
								}
							}
						}
						//操作キャラ
						if (Chara.at(m_NowMoveCharacter).IsDraw()) {
							VECTOR MP = drawcontrol->Get2DPos(Chara.at(m_NowMoveCharacter).GetPos().x, Chara.at(m_NowMoveCharacter).GetPos().y, 0.f);
							DrawCircle((int)MP.x, (int)MP.y, 32, GetColor(255, 2550, 0), false, 3);
						}
					}
					//
					//Timer
					{
						unsigned int Color = 0;
						bool IsDraw = true;

						Color = GetColor(0, 255, 0);
						if (Timer < TotalTimer * 1.f / 3.f || Timer == 0.f) {
							Color = GetColor(255, 0, 0);
						}
						if (m_MovingCharacter) {
							IsDraw = (static_cast<int>(Timer * 10.f) % 10 > 5);
							Color = GetColor(255, 255, 0);
						}
						if (IsDraw) {
							char Text[64] = "";
							sprintfDx(Text, "%d:%05.2f", static_cast<int>(Timer / 60.f), Timer - static_cast<int>(Timer / 60.f) * 60.f);
							DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 92 + 32, Text, Color, GetColor(0, 0, 0));
						}
						{
							char Text[64] = "Stop";
							if (SpeedRate > 0) {
								sprintfDx(Text, "X %d", SpeedRate);
							}
							DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 92 + 32 + 24, Text, GetColor(0, 255, 0), GetColor(0, 0, 0));
						}
					}
					//turn
					switch (Chara.at(m_NowMoveCharacter).GetCharaType()) {
					case CharaType::Team:
						if (!m_MovingCharacter) {
							DrawString(128, 128, "あなたの操作フェーズです", GetColor(128, 255, 0), GetColor(0, 0, 0));
						}
						break;
					case CharaType::TeamNPC:
						DrawString(128, 128, "味方の操作フェーズです", GetColor(100, 100, 255), GetColor(0, 0, 0));
						break;
					case CharaType::Enemy:
						DrawString(128, 128, "敵の操作フェーズです", GetColor(255, 0, 0), GetColor(0, 0, 0));
						break;
					default:
						break;
					}
					//
					{
						{
							int X = static_cast<int>((TeamAliveR / 4.f) * static_cast<float>(1920 / 4));
							DrawBox(1920 / 2 - X - 1, 92 - 1, 1920 / 2, 92 + 18 + 1, GetColor(255, 255, 255), TRUE);
							DrawBox(1920 / 2 - X, 92, 1920 / 2, 92 + 18, GetColor(0, 255, 0), TRUE);
						}
						{
							int X = static_cast<int>((EnemyAliveR / 4.f) * static_cast<float>(1920 / 4));
							DrawBox(1920 / 2, 92 - 1, 1920 / 2 + X + 1, 92 + 18 + 1, GetColor(255, 255, 255), TRUE);
							DrawBox(1920 / 2, 92, 1920 / 2 + X, 92 + 18, GetColor(255, 0, 0), TRUE);
						}
					}
					//
					sideLog->Draw(128, 1080 / 2 - 64);

					DrawExtendGraph(0, 0, 1920, 1080, BGGraphHandle, true);
				}
				//操作説明
				DrawString(32, 1080 - 32 - 64, "移動操作 左クリック:移動指示 左クリック中に右クリック:キャンセル", GetColor(255, 255, 0), GetColor(0, 0, 0));
				DrawString(32, 1080 - 32 - 32, "カメラ操作 左Shift:操作キャラをフォーカス　WASD:移動 ↑↓:速度切替", GetColor(255, 255, 0), GetColor(0, 0, 0));
				//
				if (ReadyTimer > 0.f) {
					if (ReadyTimer - 1.f > 0.f) {
						int X = static_cast<int>(((ReadyTimer - 1.f) / 3.f) * static_cast<float>(1920 / 2));
						DrawBox(1920 / 2 - X, 1080 / 2 - 32 - 1, 1920 / 2 + X, 1080 / 2 + 32 + 1, GetColor(255, 255, 255), TRUE);
						DrawBox(1920 / 2 - X, 1080 / 2 - 32, 1920 / 2 + X, 1080 / 2 + 32, GetColor(64, 64, 64), TRUE);
					}
					if ((static_cast<int>(ReadyTimer * 20.f) % 10 > 5)) {
						char Text[64] = "Go !";
						if (ReadyTimer - 1.f > 0.f) {
							sprintfDx(Text, "Ready...");
						}
						DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 1080 / 2 - 18 / 2, Text, GetColor(0, 255, 0), GetColor(0, 0, 0));
					}
				}
				//
				if (m_MatchNow == Match::Draw || m_MatchNow == Match::TeamWin || m_MatchNow == Match::EnemyWin) {
					int X = static_cast<int>((1.f - std::powf(1.f - std::clamp(EndTimer / 0.5f, 0.f, 1.f), 2.f)) * static_cast<float>(1920 / 2));
					DrawBox(1920 / 2 - X, 1080 / 2 - 32 - 1, 1920 / 2 + X, 1080 / 2 + 32 + 1, GetColor(255, 255, 255), TRUE);
					DrawBox(1920 / 2 - X, 1080 / 2 - 32, 1920 / 2 + X, 1080 / 2 + 32, GetColor(64, 64, 64), TRUE);
					switch (m_MatchNow) {
					case Match::Draw:
					{
						const char Text[] = "Draw";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 1080 / 2 - 18 / 2, Text, GetColor(0, 255, 0), GetColor(0, 0, 0));
					}
					break;
					case Match::TeamWin:
					{
						const char Text[] = "Team Win";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 1080 / 2 - 18 / 2, Text, GetColor(0, 255, 0), GetColor(0, 0, 0));
					}
					break;
					case Match::EnemyWin:
					{
						const char Text[] = "Enemy Win";
						DrawString(1920 / 2 - GetDrawStringWidth(Text, GetStringLength(Text)) / 2, 1080 / 2 - 18 / 2, Text, GetColor(255, 0, 0), GetColor(0, 0, 0));
					}
					break;
					default:
						break;
					}
				}
			}
		}
		SetDrawScreen(PrevScreen);
		{
			int OffsetX = 0;
			int OffsetY = 0;
			if (ReadyTimer > 3.5f) {
				float Per = (ReadyTimer - 3.5f) / (4.f - 3.5f);
				OffsetY = static_cast<int>(static_cast<float>(maincontrol->GetDispY()) * (Per * Per));
			}
			DrawExtendGraph(OffsetX, OffsetY, maincontrol->GetDispX() + OffsetX, maincontrol->GetDispY() + OffsetY, BufferHandle, FALSE);
		}
	}
	void Dispose() noexcept {
		DeleteGraph(BGGraphHandle);
		DeleteGraph(BufferHandle);
		AmmoEffect.Dispose();
		for (auto& c : Chara) {
			c.Dispose();
		}
		Rule::Release();
		SideLog::Release();
		DrawControl::Release();
	}
};
