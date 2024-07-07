#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClassBase : public SingletonBase<BackGroundClassBase> {
		private:
			friend class SingletonBase<BackGroundClassBase>;
		private:

			static const int tile_pic_size = 32;	//画像のタイルサイズ

		private:

			struct Blick {
				Vector3DX				m_Pos;
				Vector3DX				m_MinPos;
				Vector3DX				m_MaxPos;
				int						palletNum{0};
				bool					IsWall{false};
				float					ZRad{0};
				int						AddpalletNum[4]{};
				float					AddZRad[4]{0};
				int						LinkPolyIndex[4]{-1,-1,-1,-1};
				Vector3DX				BoxSide[4];
			public:
				Blick(int x, int y) noexcept {
					m_Pos.x = (float)x * ((64.f * 64.f) / (1080 / 2));
					m_Pos.y = (float)y * ((64.f * 64.f) / (1080 / 2));
					for (int i = 0; i < 4; i++) {
						LinkPolyIndex[i] = -1;
					}
					float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));
					m_MinPos.x = m_Pos.x - HalfLen;
					m_MinPos.y = m_Pos.y + HalfLen;

					m_MaxPos.x = m_Pos.x + HalfLen;
					m_MaxPos.y = m_Pos.y - HalfLen;

					BoxSide[0].Set(m_MinPos.x, m_MaxPos.y, 0.f);
					BoxSide[1].Set(m_MaxPos.x, m_MaxPos.y, 0.f);
					BoxSide[2].Set(m_MaxPos.x, m_MinPos.y, 0.f);
					BoxSide[3].Set(m_MinPos.x, m_MinPos.y, 0.f);
				}
			public:
				const int	GetLinkPolyIndex(int ID) const noexcept { return LinkPolyIndex[ID]; }
				void		SetLink(int tris, int index) { LinkPolyIndex[tris] = index; }
			};

			// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
			struct CheckLines {
				Vector3DX Position[3]{ };
				Vector3DX Normal{};
				bool canuse{true};
			public:
				void CalcNormal() {
					Vector3DX StartP = this->Position[0];
					Vector3DX EndP = this->Position[1];
					Vector3DX In = this->Position[2];
					Vector3DX VecP = EndP - StartP;
					this->Normal = Vector3DX::Cross(VecP, Vector3DX::Cross(VecP, In - StartP)).normalized();
				}
			};
			//
			// 視点から影になっている部分の四角形
			struct ShadowBoxes {
				Vector3DX Position[4]{ };
			};
		private:
			std::vector<CheckLines> WallList;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
			std::vector<ShadowBoxes> m_ShadowBoxes;
			std::vector<std::vector<std::shared_ptr<Blick>>> m_Blick;
			std::vector<GraphHandle>	m_MapChip;
			std::vector<GraphHandle>	m_WallChip;

			float m_AmbientShadowLength{36.f};
			float m_AmbientLightRad = deg2rad(45);
			Vector3DX m_PointLightPos;

			float ShadowPer = 0.5f;
			GraphHandle m_ShadowHandle;
		public://
			const auto&		GetBuildDataNum(void) noexcept { return this->m_Blick.size() * this->m_Blick.back().size(); }
			const auto		GetXYToNum(int x, int y) const noexcept { return x * (int)this->m_Blick.size() + y; }
			const auto		GetNumToXY(int num) const noexcept { return std::make_pair<int,int>(num / (int)this->m_Blick.size(), num % (int)this->m_Blick.size()); }
			const auto&		GetBuildData(int num) const noexcept { auto XY = GetNumToXY(num); return this->m_Blick.at(XY.first).at(XY.second); }
			const auto		GetNearestBuilds(const Vector3DX& Pos) const noexcept {
				for (auto& b : m_Blick) {
					for (auto& y : b) {
						//壁ではない
						if (!y->IsWall) {
							if ((y->m_MaxPos.x > Pos.x && Pos.x > y->m_MinPos.x) && (y->m_MinPos.y > Pos.y && Pos.y > y->m_MaxPos.y)) {
								return GetXYToNum((int)(&b - &m_Blick.front()), (int)(&y - &b.front()));
							}
						}
					}
				}
				return -1;
			}
			const auto		GetNearBuildsList(const Vector3DX& Pos) const noexcept {
				std::vector<int> SelList;
				int count = 0;
				float ViewLimit = 5.f * ((64.f * 64.f) / (1080 / 2));
				for (auto& b : m_Blick) {
					for (auto& y : b) {
						//壁ではない
						if (!y->IsWall) {
							auto Vec = y->m_Pos - Pos;
							if (Vec.magnitude() < ViewLimit) {
								SelList.emplace_back(count);
							}
						}
						count++;
					}
				}
				return SelList;
			}
			// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )
			bool CheckPolyMove(Vector3DX StartPos, Vector3DX TargetPos) const {
				int CheckPoly[4];
				int CheckPolyPrev[4];
				int NextCheckPoly[4];
				int NextCheckPolyPrev[4];

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
					for (int P = 0; P < CheckPolyNum; P++) {
						int Index = CheckPoly[P];
						auto& y = GetBuildData(Index);
						for (int i = 0; i < 4; i++) {
							Vector3DX StartP = y->BoxSide[i];
							Vector3DX EndP = y->BoxSide[(i + 1) % 4];
							int LinkIndex = y->GetLinkPolyIndex(i);
							// ポリゴンの頂点番号0と1の辺に隣接するポリゴンが存在する場合で、
							// 且つ辺の線分と移動開始点、終了点で形成する線分が接していたら if 文が真になる
							if (LinkIndex != -1 && GetMinLenSegmentToSegment(StartPos, TargetPos, StartP, EndP) < 0.01f) {
								// もし辺と接しているポリゴンが目標座標上に存在するポリゴンだったら 開始座標から目標座標上まで途切れなくポリゴンが存在するということなので true を返す
								if (LinkIndex == TargetPoly) { return true; }

								// 辺と接しているポリゴンを次のチェック対象のポリゴンに加える

								// 既に登録されているポリゴンの場合は加えない
								int IsHit = false;
								for (int j = 0; j < NextCheckPolyNum; j++) { if (NextCheckPoly[j] == LinkIndex) { IsHit = true; break; } }
								if (!IsHit) {
									// 既に登録されている除外ポリゴンの場合は加えない
									IsHit = false;
									for (int j = 0; j < NextCheckPolyPrevNum; j++) { if (NextCheckPolyPrev[j] == Index) { IsHit = true; break; } }
									if (!IsHit) {
										NextCheckPolyPrev[NextCheckPolyPrevNum] = Index;
										NextCheckPolyPrevNum++;
									}

									// 一つ前のループでチェック対象になったポリゴンの場合も加えない
									IsHit = false;
									for (int j = 0; j < CheckPolyPrevNum; j++) { if (CheckPolyPrev[j] == LinkIndex) { IsHit = true; break; } }
									if (!IsHit) {
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
					for (int P = 0; P < NextCheckPolyNum; P++) { CheckPoly[P] = NextCheckPoly[P]; }
					CheckPolyNum = NextCheckPolyNum;
					// 次にチェック対象外となるポリゴンの情報をコピーする
					for (int P = 0; P < NextCheckPolyPrevNum; P++) { CheckPolyPrev[P] = NextCheckPolyPrev[P]; }
					CheckPolyPrevNum = NextCheckPolyPrevNum;
				}
			}
			bool CheckPolyMoveWidth(Vector3DX StartPos, int TargetIndex, float Width) const {
				// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )( 幅指定版 )
				Vector3DX TargetPos = GetBuildData(TargetIndex)->m_Pos;
				// 最初に開始座標から目標座標に直線的に移動できるかどうかをチェック
				if (CheckPolyMove(StartPos, TargetPos) == false) { return false; }
				// 開始座標から目標座標に向かうベクトルに直角なベクトルを算出
				Vector3DX TempVec = Vector3DX::Cross(TargetPos - StartPos, Vector3DX::forward()).normalized() * (Width / 2.0f);
				// 開始座標と目標座標を Width / 2.0f 分だけ垂直方向にずらして、再度直線的に移動できるかどうかをチェック
				if (CheckPolyMove(StartPos + TempVec, TargetPos + TempVec) == false) { return false; }
				// 開始座標と目標座標を Width / 2.0f 分だけ一つ前とは逆方向の垂直方向にずらして、再度直線的に移動できるかどうかをチェック
				if (CheckPolyMove(StartPos - TempVec, TargetPos - TempVec) == false) { return false; }
				return true;		// ここまできたら指定の幅があっても直線的に移動できるということなので true を返す
			}





			//
			const float		CheckHideShadow(const Vector3DX & PosA, const Vector3DX & PosB, float Radius) noexcept;
			const bool		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius) noexcept;
			const void		Get2DPos(int x, int y, Vector3DX* Min, Vector3DX* Center, Vector3DX* Max) const noexcept;
			void DrawAmbientShadow(int x, int y) const noexcept;
			void DrawPointShadow(int x, int y) noexcept;
		public://
			void		SetAmbientLight(float ShadowLen, float Rad) noexcept {
				m_AmbientShadowLength = ShadowLen;
				m_AmbientLightRad = Rad;
			}
			void		SetPointLight(const Vector3DX& Pos) noexcept {
				m_PointLightPos = Convert2DtoDisp(Pos);
			}
		public://
			//
			void			Init(const char* MapPath) noexcept;
			//
			void			Execute(void) noexcept;
			//
			void			SetupShadow(void) noexcept;
			//
			void			Draw(void) noexcept;
			void			DrawFront(void) noexcept;
			//
			void			Dispose(void) noexcept;
		};
	};
};
