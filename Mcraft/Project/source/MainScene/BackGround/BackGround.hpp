#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"
#include	"BackGroundSub.hpp"

namespace FPS_n2 {
	namespace BackGround {

		class WayPoint {
		public:
			class Builds {
				struct LinkBuffer {
					bool isActive{ false };
					Vector3DX Pos{};
					int ID;
				};
			private:
				int							m_mesh{ -1 };
				Vector3DX					m_MinPos;
				Vector3DX					m_Pos;
				Vector3DX					m_MaxPos;
				int							MyIndex{ 0 };
			public:
				std::array<LinkBuffer, 4>	m_LinkPosBuffer{};
			public:
				const int	GetIndex() const noexcept { return MyIndex; }
				const int	GetLinkPolyIndex(int ID) const noexcept { return m_LinkPosBuffer[ID].ID; }
				void		SetLinkBuffer(int tris, const Vector3DX& pos) noexcept {
					m_LinkPosBuffer[tris].isActive = true;
					m_LinkPosBuffer[tris].Pos = pos;
				}
			public:
				const Vector3DX& GetMinPos(void) const noexcept { return m_MinPos; }
				const Vector3DX& GetPos(void) const noexcept { return m_Pos; }
				const Vector3DX& GetMaxPos(void) const noexcept { return m_MaxPos; }
			public:
				void		Set(int index) noexcept {
					this->MyIndex = index;
					for (auto& L : this->m_LinkPosBuffer) {
						L.isActive = false;
						L.ID = -1;
					}
				}
				void		SetPosition(Vector3DX MinPos, Vector3DX MaxPos) noexcept {
					this->m_MinPos = MinPos;
					this->m_MaxPos = MaxPos;
					this->m_Pos = (MinPos + MaxPos) / 2;
				}
			};
		private:
			std::vector<Builds>				m_WayPoints;
			int								m_SeekPoint{ 0 };
		public:
			std::vector<Builds>& SetWayPoints(void) noexcept { return this->m_WayPoints; }
			const std::vector<Builds>& GetWayPoints(void) const noexcept { return this->m_WayPoints; }
		public:
			//所定の位置からNm未満かNm以上の座標を取得する
			Vector3DX		GetRandomPoint(const Vector3DX& Pos, float RangeSmaller = InvalidID, float RangeOver = InvalidID) const noexcept {
				std::vector<int> SelList;
				SelList.clear();
				for (auto& C : this->m_WayPoints) {
					int index = static_cast<int>(&C - &this->m_WayPoints.front());
					Vector3DX Vec = C.GetPos() - Pos; Vec.y = (0.f);
					if (RangeSmaller != InvalidID && RangeOver != InvalidID) {
						if (Vec.IsRangeSmaller(RangeSmaller) && !Vec.IsRangeSmaller(RangeOver)) {
							SelList.emplace_back(index);
						}
					}
					else {
						if (RangeSmaller != InvalidID && Vec.IsRangeSmaller(RangeSmaller)) {
							SelList.emplace_back(index);
						}
						if (RangeOver != InvalidID && !Vec.IsRangeSmaller(RangeOver)) {
							SelList.emplace_back(index);
						}
					}
				}
				Vector3DX Answer;
				if (SelList.size() == 0) {
					Answer = Pos;
				}
				return this->m_WayPoints.at(SelList.at(GetRand((int)SelList.size() - 1))).GetPos();
			}

			const int		GetNearestBuilds(const Vector3DX& NowPosition) const noexcept {
				for (auto& bu : this->m_WayPoints) {
					if (
						(bu.GetMaxPos().x >= NowPosition.x && NowPosition.x >= bu.GetMinPos().x) &&
						(bu.GetMaxPos().z >= NowPosition.z && NowPosition.z >= bu.GetMinPos().z)
						) {
						return static_cast<int>(&bu - &this->m_WayPoints.front());
					}
				}
				return -1;
			}
			const int		GetNearestBuilds2(const Vector3DX& NowPosition) const noexcept {
				float Len = 1000000.f;
				int Answer = -1;
				for (auto& bu : this->m_WayPoints) {
					Vector3DX Length = bu.GetPos() - NowPosition;
					Length.y = 0.f;
					if (Len >= Length.sqrMagnitude()) {
						Len = Length.sqrMagnitude();
						Answer = static_cast<int>(&bu - &this->m_WayPoints.front());
					}
				}
				return Answer;
			}
			// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )
			bool			CheckPolyMove(Vector3DX StartPos, Vector3DX TargetPos) const {
				int Rate = 6;

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
						Vector3DX Pos = this->m_WayPoints.at(Index).GetPos(); Pos.y = (0.f);

						for (int K = 0; K < 4; K++) {
							int LinkIndex = this->m_WayPoints.at(Index).GetLinkPolyIndex(K);

							;

							Vector3DX PolyPos = Pos;
							PolyPos.x += ((Rate / 2.f) * ((K == 0 || K == 1) ? 1.f : -1.f));
							PolyPos.z += ((Rate / 2.f) * ((K == 1 || K == 2) ? 1.f : -1.f));
							int K2 = (K + 1) % 4;
							Vector3DX PolyPos2 = Pos;
							PolyPos2.x += ((Rate / 2.f) * ((K2 == 0 || K2 == 1) ? 1.f : -1.f));
							PolyPos2.z += ((Rate / 2.f) * ((K2 == 1 || K2 == 2) ? 1.f : -1.f));
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
			bool			CheckPolyMoveWidth(Vector3DX StartPos, int TargetIndex, float Width) const {
				// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )( 幅指定版 )
				Vector3DX TargetPos = this->m_WayPoints.at(TargetIndex).GetPos();
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
		public:
			void			Init(int Count) noexcept {
				this->m_WayPoints.resize(Count);
				this->m_SeekPoint = 0;
			}
			auto&			AddWayPoint(Vector3DX MinPos, Vector3DX MaxPos) noexcept {
				auto& w = this->m_WayPoints.at(this->m_SeekPoint);
				w.Set(this->m_SeekPoint);
				w.SetPosition(MinPos, MaxPos);
				++this->m_SeekPoint;
				return w;
			}
			void			Setup() noexcept {
				for (auto& w : this->m_WayPoints) {
					for (auto& L : w.m_LinkPosBuffer) {
						if (L.isActive) {
							L.ID = GetNearestBuilds(L.Pos);
						}
					}
				}
			}
		};

		class BackGroundControl : public SingletonBase<BackGroundControl> {
		private:
			friend class SingletonBase<BackGroundControl>;
		public:
			class Builds {
				int						m_mesh{ -1 };
				Vector3DX				m_MinPos;
				Vector3DX				m_Pos;
				Vector3DX				m_MaxPos;

				int							MyIndex{ 0 };
				int							LinkPolyIndex[4]{ -1,-1,-1,-1 };
			public:
				const int	GetIndex() const noexcept { return MyIndex; }
				const int	GetLinkPolyIndex(int ID) const noexcept { return LinkPolyIndex[ID]; }
				void		SetLink(int tris, int index) noexcept { LinkPolyIndex[tris] = index; }
			public:
				const Vector3DX& GetMinPos(void) const noexcept { return m_MinPos; }
				const Vector3DX& GetPos(void) const noexcept { return m_Pos; }
				const Vector3DX& GetMaxPos(void) const noexcept { return m_MaxPos; }
			public:
				void		Set(int index) noexcept {
					this->MyIndex = index;
					for (int i = 0; i < 4; i++) {
						LinkPolyIndex[i] = -1;
					}
				}
				void		SetPosition(Vector3DX MinPos, Vector3DX MaxPos) noexcept {
					this->m_MinPos = MinPos;
					this->m_MaxPos = MaxPos;
					this->m_Pos = (MinPos + MaxPos) / 2;
				}
			};
		private:
			std::unique_ptr<WayPoint>		m_WayPoint;
			std::unique_ptr<VoxelControl>	m_VoxelControl;
			MV1								m_ObjSky;
			bool							m_GrenadeBomb{ false };
			std::shared_ptr<BaseObject>		m_ItemContainerObj;
			std::vector<const MV1*>			m_AddonColObj;
		private:
			BackGroundControl(void) noexcept { Load(); }
			BackGroundControl(const BackGroundControl&) = delete;
			BackGroundControl(BackGroundControl&&) = delete;
			BackGroundControl& operator=(const BackGroundControl&) = delete;
			BackGroundControl& operator=(BackGroundControl&&) = delete;

			virtual ~BackGroundControl(void) noexcept { Dispose_Load(); }
		public:
			const auto&		GetWayPoint(void) const noexcept { return this->m_WayPoint; }

			void			SetGrenadeBomb() noexcept {
				m_GrenadeBomb = true;
			}
			bool			PopGrenadeBomb() noexcept {
				bool Prev = m_GrenadeBomb;
				m_GrenadeBomb = false;
				return Prev;
			}
		public:
			void			SetItemContainerObj(const std::shared_ptr<BaseObject>& pObj) noexcept { this->m_ItemContainerObj = pObj; }
			std::shared_ptr<BaseObject>& GetItemContainerObj(void) noexcept { return this->m_ItemContainerObj; }

			int				CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, Vector3DX* Normal = nullptr) const noexcept;
			int				CheckLinetoMap(const Vector3DX& StartPos, const Vector3DX& EndPos) const noexcept {
				Vector3DX pEndPos = EndPos;
				return CheckLinetoMap(StartPos, &pEndPos);
			}
			bool			CheckMapWall(const Vector3DX& StartPos, Vector3DX* EndPos, const Vector3DX& AddCapsuleMin, const Vector3DX& AddCapsuleMax, float Radius) const noexcept {
				return this->m_VoxelControl->CheckWall(StartPos, EndPos, AddCapsuleMin, AddCapsuleMax, Radius, m_AddonColObj);
			}
		public:
			const Vector3Int GetVoxelPoint(const Vector3DX& pos) const noexcept { return this->m_VoxelControl->GetReferenceCells().GetVoxelPoint(pos); }
			const Vector3DX GetWorldPos(const Vector3Int& VoxelPoint) const noexcept { return this->m_VoxelControl->GetReferenceCells().GetWorldPos(VoxelPoint); }
			//ブロックにダメージを与える
			bool			DamageCell(int Xvoxel, int Yvoxel, int Zvoxel, int8_t Damage) noexcept { return this->m_VoxelControl->DamageCell(Xvoxel, Yvoxel, Zvoxel, Damage); }
			void			SetCell(int Xvoxel, int Yvoxel, int Zvoxel, int8_t ID) noexcept { this->m_VoxelControl->SetBlick(Xvoxel, Yvoxel, Zvoxel, ID); }
			void			SettingChange(void) noexcept { this->m_VoxelControl->SettingChange(); }
			constexpr float GetDrawFarMax() const noexcept { return this->m_VoxelControl->GetDrawFarMax(); }
		public://
			void			Load(void) noexcept;
			//
			void			Init(bool IsTutorial) noexcept;
			void			Update(void) noexcept;
			//
			void			BG_Draw(void) const noexcept;
			void			Shadow_Draw(void) const noexcept;
			void			SetShadow_Draw_Rigid(void) const noexcept;
			void			Draw(void) const noexcept;
			//
			void			Dispose(void) noexcept;
			void			Dispose_Load(void) noexcept;
		};
		class BackGroundControlTitle : public SingletonBase<BackGroundControlTitle> {
		private:
			friend class SingletonBase<BackGroundControlTitle>;
		private:
			MV1										m_ObjSky{};
		private:
			BackGroundControlTitle(void) noexcept { Load(); }
			BackGroundControlTitle(const BackGroundControlTitle&) = delete;
			BackGroundControlTitle(BackGroundControlTitle&&) = delete;
			BackGroundControlTitle& operator=(const BackGroundControlTitle&) = delete;
			BackGroundControlTitle& operator=(BackGroundControlTitle&&) = delete;

			virtual ~BackGroundControlTitle(void) noexcept { Dispose_Load(); }
		public://
			void			Load(void) noexcept;

			void			Init(void) noexcept;
			void			BG_Draw(void) const noexcept;

			void			Dispose_Load(void) noexcept;
		};
	}
}
