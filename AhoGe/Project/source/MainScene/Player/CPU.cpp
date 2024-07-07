#include	"CPU.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ENUM_AI_PHASE {
			Normal,
			Caution,
			Alert,
			Dead,
		};
		//
		class PathChecker {
		public:
			// 経路探索処理用の１ポリゴンの情報
			class PATHPLANNING_UNIT {
				int PolyIndex;						// ポリゴン番号
				float TotalDistance;				// 経路探索でこのポリゴンに到達するまでに通過したポリゴン間の距離の合計
				PATHPLANNING_UNIT *PrevPolyUnit;		// 経路探索で確定した経路上の一つ前のポリゴン( 当ポリゴンが経路上に無い場合は nullptr )
				PATHPLANNING_UNIT *NextPolyUnit;		// 経路探索で確定した経路上の一つ先のポリゴン( 当ポリゴンが経路上に無い場合は nullptr )
			public:
				PATHPLANNING_UNIT *ActiveNextUnit;		// 経路探索処理対象になっている次のポリゴンのメモリアドレスを格納する変数
			public:
				const auto& GetPolyIndex() const noexcept { return this->PolyIndex; }
				const auto& GetNextPolyUnit() const noexcept { return this->NextPolyUnit; }
			public:
				bool SetPrevPolyUnit(PATHPLANNING_UNIT *PUnit, int tris) {
					auto* BackGround = BackGroundClassBase::Instance();
					// 隣接するポリゴンが既に経路探索処理が行われていて、且つより距離の長い経路となっている場合は何もしない
					auto& Unit = BackGround->GetBuildData(PUnit->PolyIndex);

					auto trisdistance = PUnit->TotalDistance +
						(BackGround->GetBuildData(Unit->GetLinkPolyIndex(tris))->m_Pos + Unit->m_Pos).magnitude();

					if (this->TotalDistance > trisdistance) {
						this->TotalDistance = trisdistance;		// 隣接するポリゴンにここに到達するまでの距離を代入する
					}
					else {
						if (this->PrevPolyUnit) { return false; }
					}
					this->PrevPolyUnit = PUnit;			// 隣接するポリゴンに経路情報となる自分のポリゴンの番号を代入する
					return true;
				}
				bool SearchThisUnit(PATHPLANNING_UNIT *SearchUnit) {
					// 次のループで行う経路探索処理対象に追加する、既に追加されていたら追加しない
					PATHPLANNING_UNIT *PUnitSub2 = SearchUnit;
					while (true) {
						if (PUnitSub2 == nullptr) { break; }
						if (PUnitSub2 == this) { return false; }//既に追加されとる
						PUnitSub2 = PUnitSub2->ActiveNextUnit;
					}
					return PUnitSub2;
				}
			public:
				// ゴール地点のポリゴンからスタート地点のポリゴンに辿って経路上のポリゴンに次に移動すべきポリゴンの番号を代入する
				static void SetNextIndex(PATHPLANNING_UNIT *pGoal, PATHPLANNING_UNIT *pStart) {
					PATHPLANNING_UNIT *PUnit = pGoal;
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
			Vector3DX GoalPosition;					// 目標位置
			std::vector<PATHPLANNING_UNIT>UnitArray;	// 経路探索処理で使用する全ポリゴンの情報配列が格納されたメモリ領域の先頭メモリアドレスを格納する変数
			PATHPLANNING_UNIT *StartUnit{nullptr};			// 経路のスタート地点にあるポリゴン情報へのメモリアドレスを格納する変数
			PATHPLANNING_UNIT *GoalUnit{nullptr};				// 経路のゴール地点にあるポリゴン情報へのメモリアドレスを格納する変数
		public:
			const auto& GetStartUnit() const noexcept { return this->StartUnit; }
		public:
			Vector3DX GetNextPoint(const Vector3DX& NowPosition, int *TargetPathPlanningIndex) const {
				auto* BackGround = BackGroundClassBase::Instance();
				int NowIndex = BackGround->GetNearestBuilds(NowPosition);
				if (NowIndex != this->GoalUnit->GetPolyIndex()) {																	// 現在乗っているポリゴンがゴール地点にあるポリゴンの場合は処理を分岐
					if (NowIndex == *TargetPathPlanningIndex) {													// 現在乗っているポリゴンが移動中間地点のポリゴンの場合は次の中間地点を決定する処理を行う
						const float COLLWIDTH = 0.1f * ((64.f * 64.f) / (1080 / 2));				// 当たり判定のサイズ
						while (true) {																				// 次の中間地点が決定するまでループし続ける
							if (!this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()) { break; }
							auto NextIndex = this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()->GetPolyIndex();
							if (!BackGround->CheckPolyMoveWidth(NowPosition, NextIndex, COLLWIDTH)) { break; }		// 経路上の次のポリゴンの中心座標に直線的に移動できない場合はループから抜ける
							(*TargetPathPlanningIndex) = NextIndex;													// チェック対象を経路上の更に一つ先のポリゴンに変更する
							if ((*TargetPathPlanningIndex) == this->GoalUnit->GetPolyIndex()) { break; }				// もしゴール地点のポリゴンだったらループを抜ける
						}
					}
					// 移動方向を決定する、移動方向は現在の座標から中間地点のポリゴンの中心座標に向かう方向
					return BackGround->GetBuildData(*TargetPathPlanningIndex)->m_Pos;
				}
				else {
					// 方向は目標座標
					return this->GoalPosition;
				}
			}
		public:
			bool Init(Vector3DX StartPos, Vector3DX GoalPos) {
				auto* BackGround = BackGroundClassBase::Instance();
				// 指定の２点の経路を探索する( 戻り値 true:経路構築成功 false:経路構築失敗( スタート地点とゴール地点を繋ぐ経路が無かった等 ) )
				this->GoalPosition = GoalPos;			// ゴール位置を保存

				this->UnitArray.resize(BackGround->GetBuildDataNum());			// 経路探索用のポリゴン情報を格納するメモリ領域を確保、初期化
				for (auto& p : this->UnitArray) {
					p.Init((int)(&p - &this->UnitArray.front()));
				}

				int StartIndex = BackGround->GetNearestBuilds(StartPos);	// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
				if (StartIndex == -1) { return false; }
				this->StartUnit = &this->UnitArray[StartIndex];					// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存

				int GoalIndex = BackGround->GetNearestBuilds(GoalPos);		// ゴール地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
				if (GoalIndex == -1) { return false; }
				this->GoalUnit = &this->UnitArray[GoalIndex];				// ゴール地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
				if (GoalIndex == StartIndex) { return false; }				// ゴール地点にあるポリゴンとスタート地点にあるポリゴンが同じだったら false を返す

				PATHPLANNING_UNIT *ActiveFirstUnit = this->StartUnit;		// 経路探索処理対象のポリゴンとしてスタート地点にあるポリゴンを登録する
				// 経路を探索してゴール地点のポリゴンにたどり着くまでループを繰り返す
				while (true) {
					bool Goal = false;
					// 経路探索処理対象になっているポリゴンをすべて処理
					PATHPLANNING_UNIT *PUnit = ActiveFirstUnit;
					ActiveFirstUnit = nullptr;
					while (true) {
						// ポリゴンの辺の数だけ繰り返し
						for (int K = 0; K < 4; K++) {
							int Index = BackGround->GetBuildData(PUnit->GetPolyIndex())->GetLinkPolyIndex(K);
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
		//AI用
		class AIControl::Impl {
		private:
			const PlayerID							m_TargetCharaID{0};
		public:
			int										TargetPathPlanningIndex{0};		// 次の中間地点となる経路上のポリゴンの経路探索情報が格納されているメモリアドレスを格納する変数
			PathChecker								m_PathChecker;
			float									m_PathUpdateTimer{0.f};
			//
			ENUM_AI_PHASE							m_Phase{ENUM_AI_PHASE::Normal};
			PlayerID								m_MyCharaID{0};
			InputControl							m_MyInput;
			float									m_Rad{0.f};
			Vector3DX								m_LastFindPos{};
			float									m_LostTimer{0.f};
			float									m_GraphTimer{0.f};
		public:
			Impl(void) noexcept {}
			~Impl(void) noexcept {}
		public:
			const auto& GetViewRad() const noexcept { return m_Rad; }
		private:
			const auto GetIsSeeTarget() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				if (Chara->CanLookPlayer0()) {
					float ViewLimit = 10.f * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < ViewLimit) {
						Vector3DX Vec;Vec.Set(std::sin(Chara->GetViewRad()), std::cos(Chara->GetViewRad()), 0.f);
						Vector3DX vec_a;vec_a = (Chara->GetPos() - Target->GetPos()).normalized();
						if (-Vector3DX::Dot(vec_a, Vec) > std::cos(deg2rad(45))) {
							m_LastFindPos = Target->GetPos();
							return true;
						}
					}
				}
				return false;
			}
			//
			void		ChangePoint() noexcept {
				auto* BackGround = BackGroundClassBase::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();

				auto TgtPos = Target->GetPos();
				/*
				std::vector<int> SelList = BackGround->GetNearBuildsList(TgtPos);
				if (SelList.size() > 0) {
					TgtPos = BackGround->GetBuildData(SelList.at(GetRand((int)SelList.size() - 1)))->m_Pos;
				}
				//*/
				m_PathChecker.Dispose();
				m_PathChecker.Init(Chara->GetPos(), TgtPos);	// 指定の２点の経路情報を探索する
				this->TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetPolyIndex();	// 移動開始時点の移動中間地点の経路探索情報もスタート地点にあるポリゴンの情報
			}
		public:
			void		Init(PlayerID MyCharaID) noexcept {
				m_MyCharaID = MyCharaID;
				this->m_PathUpdateTimer = 0.f;
			}
			//
			void		Execute_Before() noexcept {
				auto* DrawParts = DXDraw::Instance();
				//初期化
				m_MyInput.ResetAllInput();
				this->m_GraphTimer = std::max(this->m_GraphTimer - 1.f / DrawParts->GetFps(), 0.f);
				//
				m_PathUpdateTimer = std::max(m_PathUpdateTimer - 1.f / DrawParts->GetFps(), 0.f);
				if (m_PathUpdateTimer <= 0.f) {
					m_PathUpdateTimer += 1.f;
					this->ChangePoint();
				}
			}
			void		Execute_Normal() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				//探索
				{
					m_LastFindPos = m_PathChecker.GetNextPoint(Chara->GetPos(), &this->TargetPathPlanningIndex);

					float Len = 0.1f * ((64.f * 64.f) / (1080 / 2));
					auto Vec = m_LastFindPos - Chara->GetPos();
					if (Vec.magnitude() > Len) {
						m_MyInput.SetInputPADS(PADS::MOVE_W, Vec.y > 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_S, Vec.y < 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_A, Vec.x < 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_D, Vec.x > 0.f);
					}
				}
				//
				if (GetIsSeeTarget()) {
					float ViewLimit = 5.f * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < ViewLimit) {
						this->m_Phase = ENUM_AI_PHASE::Alert;
						this->m_GraphTimer = 2.f;
					}
					else {
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
				}
				else {
					float NearLimit = (1.5f + 1.f * Target->GetSpeed() / 128.f) * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < NearLimit) {
						m_LastFindPos = Target->GetPos();
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
				}
				//
				m_Rad = -GetRadVec2Vec(Convert2DtoDisp(Chara->GetPos()), Convert2DtoDisp(m_LastFindPos));

				this->m_Phase = ENUM_AI_PHASE::Normal;
			}
			void		Execute_Caution() noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				//
				m_Rad = -GetRadVec2Vec(Convert2DtoDisp(Chara->GetPos()), Convert2DtoDisp(m_LastFindPos));
				//
				if (GetIsSeeTarget()) {
					float ViewLimit = 5.f * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < ViewLimit) {
						this->m_Phase = ENUM_AI_PHASE::Alert;
						this->m_GraphTimer = 2.f;
					}
					m_LostTimer = 5.f;
				}
				else {
					m_LostTimer = std::max(m_LostTimer - 1.f / DrawParts->GetFps(), 0.f);
					if (m_LostTimer == 0.f) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
						this->m_GraphTimer = 2.f;
					}
				}
			}
			void		Execute_Alert() noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				//
				m_Rad = -GetRadVec2Vec(Convert2DtoDisp(Chara->GetPos()), Convert2DtoDisp(Target->GetPos()));
				//
				m_LastFindPos = Target->GetPos();
				//
				if (GetIsSeeTarget()) {
					m_LostTimer = 5.f;
				}
				else {
					m_LostTimer = std::max(m_LostTimer - 1.f / DrawParts->GetFps(), 0.f);
					if (m_LostTimer == 0.f) {
						m_LostTimer = 5.f;
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
				}
			}
		};
		//
		AIControl::AIControl() {
			m_Param = new Impl;
		}
		AIControl::~AIControl() {
			delete m_Param;
		}
		//
		const float & AIControl::GetViewRad() const noexcept { return this->GetParam()->GetViewRad(); }
		const float & AIControl::GetGraphTimer() const noexcept { return this->GetParam()->m_GraphTimer; }
		const bool AIControl::IsCaution() const noexcept { return this->GetParam()->m_Phase == ENUM_AI_PHASE::Caution; }
		const bool AIControl::IsAlert() const noexcept { return this->GetParam()->m_Phase == ENUM_AI_PHASE::Alert; }
		//
		void AIControl::Init(PlayerID MyCharaID) noexcept {
			this->GetParam()->Init(MyCharaID);
		}
		void AIControl::Execute(InputControl* MyInput) noexcept {
			//return;
			//AI
			this->GetParam()->Execute_Before();
			switch (this->GetParam()->m_Phase) {
				case ENUM_AI_PHASE::Normal:
					this->GetParam()->Execute_Normal();
					break;
				case ENUM_AI_PHASE::Caution:
					this->GetParam()->Execute_Caution();
					break;
				case ENUM_AI_PHASE::Alert:
					this->GetParam()->Execute_Alert();
					break;
				case ENUM_AI_PHASE::Dead:
					break;
				default:
					break;
			}
			*MyInput = this->GetParam()->m_MyInput;
		}
	};
};
