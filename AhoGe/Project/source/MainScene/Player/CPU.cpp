#include	"CPU.hpp"
#include	"../../MainScene/Player/Player.hpp"

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
			// 経路探索処理用の１地点の情報
			class PATHPLANNING_UNIT {
				int Index{ 0 };						// 番号
				float TotalDistance{ 0.f };				// 経路探索でこの地点に到達するまでに通過した地点間の距離の合計
				PATHPLANNING_UNIT* PrevUnit{ nullptr };		// 経路探索で確定した経路上の一つ前の地点( 当地点が経路上に無い場合は nullptr )
				PATHPLANNING_UNIT* NextUnit{ nullptr };		// 経路探索で確定した経路上の一つ先の地点( 当地点が経路上に無い場合は nullptr )
			public:
				PATHPLANNING_UNIT* ActiveNextUnit{ nullptr };		// 経路探索処理対象になっている次の地点のメモリアドレスを格納する変数
			public:
				const auto& GetIndex() const noexcept { return this->Index; }
				const auto& GetNextUnit() const noexcept { return this->NextUnit; }
			public:
				bool SetPrevUnit(PATHPLANNING_UNIT *PUnit, int tris) {
					auto* BackGround = BackGroundClassBase::Instance();
					// 隣接する地点が既に経路探索処理が行われていて、且つより距離の長い経路となっている場合は何もしない
					auto& Unit = BackGround->GetFloorData(PUnit->Index);

					auto trisdistance = PUnit->TotalDistance + (BackGround->GetFloorData(Unit->LinkIndex.at(tris))->m_Pos + Unit->m_Pos).magnitude();

					if (this->TotalDistance > trisdistance) {
						this->TotalDistance = trisdistance;		// 隣接する地点にここに到達するまでの距離を代入する
					}
					else {
						if (this->PrevUnit) { return false; }
					}
					this->PrevUnit = PUnit;			// 隣接する地点に経路情報となる自分の地点の番号を代入する
					return true;
				}
			public:
				static bool SetStartToGoal(const std::vector<PATHPLANNING_UNIT>& UnitArray, PATHPLANNING_UNIT* pStart, PATHPLANNING_UNIT* pGoal) {
					auto* BackGround = BackGroundClassBase::Instance();
					PATHPLANNING_UNIT* ActiveFirstUnit = pStart;		// 経路探索処理対象の地点としてスタート地点にある地点を登録する
					// 経路を探索してゴール地点の地点にたどり着くまでループを繰り返す
					while (true) {
						bool Goal = false;
						// 経路探索処理対象になっている地点をすべて処理
						PATHPLANNING_UNIT* PUnit = ActiveFirstUnit;
						ActiveFirstUnit = nullptr;
						while (true) {
							// 地点の辺の数だけ繰り返し
							for (int K = 0; K < 4; K++) {
								int Index = BackGround->GetFloorData(PUnit->GetIndex())->LinkIndex.at(K);
								if (Index == -1) { continue; }											// 辺に隣接する地点が無い場合は何もしない
								if (Index == pStart->GetIndex()) { continue; }				// スタート地点の地点だった場合は何もしない
								PATHPLANNING_UNIT* NowUnit = (PATHPLANNING_UNIT*) & UnitArray.at(Index);
								if (!NowUnit->SetPrevUnit(PUnit, K)) {
									continue;
								}
								// 次のループで行う経路探索処理対象に追加する、既に追加されていたら追加しない
								auto SearchThisUnit = [&]() {
									PATHPLANNING_UNIT* PUnitSub2 = ActiveFirstUnit;
									while (true) {
										if (!PUnitSub2) { break; }
										if (PUnitSub2 == NowUnit) { return true; }// 既に追加されとる
										PUnitSub2 = PUnitSub2->ActiveNextUnit;
									}
									return PUnitSub2 != nullptr;
									};
								if (!SearchThisUnit()) {
									NowUnit->ActiveNextUnit = ActiveFirstUnit;
									ActiveFirstUnit = NowUnit;
								}
								// 隣接する地点がゴール地点にある地点だったらゴールに辿り着いたフラグを立てる
								if (Index == pGoal->GetIndex()) {
									Goal = true;
								}
							}
							PUnit = PUnit->ActiveNextUnit;
							if (!PUnit) { break; }
						}

						if (!ActiveFirstUnit) { return false; }			// スタート地点にある地点からゴール地点にある地点に辿り着けないということなので false を返す
						if (Goal) { break; }
					}
					// ゴール地点の地点からスタート地点の地点に辿って経路上の地点に次に移動すべき地点の番号を代入する
					PATHPLANNING_UNIT* PUnit = pGoal;
					while (true) {
						auto* PrevPUnitIndex = PUnit;
						PUnit = PUnit->PrevUnit;
						PUnit->NextUnit = PrevPUnitIndex;
						if (PUnit == pStart) { break; }
					}
					return true;
				}
			public:
				void Init(int index) {
					this->Index = index;
					this->TotalDistance = 0.0f;
					this->PrevUnit = nullptr;
					this->NextUnit = nullptr;
					this->ActiveNextUnit = nullptr;
				}
			};
		private:
			Vector3DX GoalPosition;					// 目標位置
			std::vector<PATHPLANNING_UNIT>UnitArray;	// 経路探索処理で使用する全地点の情報配列が格納されたメモリ領域の先頭メモリアドレスを格納する変数
			PATHPLANNING_UNIT *StartUnit{nullptr};			// 経路のスタート地点にある地点情報へのメモリアドレスを格納する変数
			PATHPLANNING_UNIT *GoalUnit{nullptr};				// 経路のゴール地点にある地点情報へのメモリアドレスを格納する変数
		public:
			const auto& GetStartUnit() const noexcept { return this->StartUnit; }
		public:
			Vector3DX GetNextPoint(const Vector3DX& NowPosition, int *TargetPathPlanningIndex) const {
				auto* BackGround = BackGroundClassBase::Instance();
				int NowIndex = BackGround->GetNearestFloors(NowPosition);
				if (NowIndex != this->GoalUnit->GetIndex()) {												// 現在乗っている地点がゴール地点にある地点の場合は処理を分岐
					if (NowIndex == *TargetPathPlanningIndex) {												// 現在乗っている地点が移動中間地点の地点の場合は次の中間地点を決定する処理を行う
						while (true) {																		// 次の中間地点が決定するまでループし続ける
							auto* NextPtr = this->UnitArray.at(NowIndex).GetNextUnit();
							if (!NextPtr) { break; }
							if (!BackGround->CheckNextFloor(NowIndex, NextPtr->GetIndex())) { break; }		// 経路上の次の地点の中心座標に直線的に移動できない場合はループから抜ける
							NowIndex = NextPtr->GetIndex();													// チェック対象を経路上の更に一つ先の地点に変更する
							if (NowIndex == this->GoalUnit->GetIndex()) { break; }							// もしゴール地点の地点だったらループを抜ける
						}
						*TargetPathPlanningIndex = NowIndex;
					}
					// 移動方向を決定する、移動方向は現在の座標から中間地点の地点の中心座標に向かう方向
					return BackGround->GetFloorData(*TargetPathPlanningIndex)->m_Pos;
				}
				else {
					// 方向は目標座標
					return this->GoalPosition;
				}
			}
		public:
			// 指定の２点の経路を探索する( 戻り値 true:経路構築成功 false:経路構築失敗( スタート地点とゴール地点を繋ぐ経路が無かった等 ) )
			bool UpdatePath(const Vector3DX& StartPos, const Vector3DX& GoalPos) {
				auto* BackGround = BackGroundClassBase::Instance();
				this->GoalPosition = GoalPos;			// ゴール位置を保存
				// 経路探索用の地点情報を格納するメモリ領域を確保、初期化
				for (auto& p : this->UnitArray) {
					p.Init((int)(&p - &this->UnitArray.front()));
				}
				//スタート地点を決定
				int StartIndex = BackGround->GetNearestFloors(StartPos);
				if (StartIndex == -1) { return false; }
				this->StartUnit = &this->UnitArray.at(StartIndex);
				//ゴール地点を決定
				int GoalIndex = BackGround->GetNearestFloors(this->GoalPosition);
				if (GoalIndex == -1) { return false; }
				this->GoalUnit = &this->UnitArray.at(GoalIndex);
				// ゴール地点にある地点とスタート地点にある地点が同じだったら false を返す
				if (GoalIndex == StartIndex) { return false; }
				// 経路を探索
				return PATHPLANNING_UNIT::SetStartToGoal(UnitArray, this->StartUnit, this->GoalUnit);
			}
			void Init(void) {
				auto* BackGround = BackGroundClassBase::Instance();
				this->UnitArray.resize(BackGround->GetXSize() * BackGround->GetYSize());
			}
			void Dispose(void) {
				this->UnitArray.clear();
			}
		};
		// AI用
		class AIControl::Impl {
		private:
			const PlayerID							m_TargetCharaID{0};
		public:
			int										TargetPathPlanningIndex{0};		// 次の中間地点となる経路上の地点の経路探索情報が格納されているメモリアドレスを格納する変数
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
					float ViewLimit = Get2DSize(10.f);
					if ((Chara->GetPos() - Target->GetPos()).sqrMagnitude() < ViewLimit * ViewLimit) {
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
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();

				auto TgtPos = Target->GetPos();
				//*
				auto* BackGround = BackGroundClassBase::Instance();
				std::vector<int> SelList = BackGround->GetNearFloorsList(TgtPos);
				if (SelList.size() > 0) {
					TgtPos = BackGround->GetFloorData(SelList.at(GetRand((int)SelList.size() - 1)))->m_Pos;
				}
				// */
				m_PathChecker.UpdatePath(Chara->GetPos(), TgtPos);	// 指定の２点の経路情報を探索する
				this->TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetIndex();	// 移動開始時点の移動中間地点の経路探索情報もスタート地点にある地点の情報
			}
		public:
			void		Init() noexcept {
				this->m_PathUpdateTimer = -(float)m_MyCharaID / (float)Player_Num*5.f;
				m_PathChecker.Init();
			}
			// 
			void		Execute_Before() noexcept {
				auto* DrawParts = DXDraw::Instance();
				// 初期化
				m_MyInput.ResetAllInput();
				this->m_GraphTimer = std::max(this->m_GraphTimer - 1.f / DrawParts->GetFps(), 0.f);
				// 
				//printfDx("%f\n", m_PathUpdateTimer);
				m_PathUpdateTimer -= 1.f / DrawParts->GetFps();
				if (m_PathUpdateTimer <= 0.f) {
					m_PathUpdateTimer += 5.f;
					this->ChangePoint();
				}
			}
			void		Execute_Normal() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				// 探索
				{
					m_LastFindPos = m_PathChecker.GetNextPoint(Chara->GetPos(), &this->TargetPathPlanningIndex);

					float Len = Get2DSize(0.1f);
					auto Vec = m_LastFindPos - Chara->GetPos();
					if (Vec.sqrMagnitude() > Len * Len) {
						m_MyInput.SetInputPADS(PADS::MOVE_W, Vec.y > 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_S, Vec.y < 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_A, Vec.x < 0.f);
						m_MyInput.SetInputPADS(PADS::MOVE_D, Vec.x > 0.f);
						m_MyInput.SetInputPADS(PADS::WALK, true);
					}
				}
				// 
				if (GetIsSeeTarget()) {
					float ViewLimit = Get2DSize(5.f);
					if ((Chara->GetPos() - Target->GetPos()).sqrMagnitude() < ViewLimit * ViewLimit) {
						this->m_Phase = ENUM_AI_PHASE::Alert;
						this->m_GraphTimer = 2.f;
					}
					else {
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
				}
				else {
					float NearLimit = Get2DSize(1.5f + 1.f * Target->GetSpeedPer());
					if ((Chara->GetPos() - Target->GetPos()).sqrMagnitude() < NearLimit * NearLimit) {
						m_LastFindPos = Target->GetPos();
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
				}
				// 
				m_Rad = -GetRadVec2Vec(Convert2DtoDisp(Chara->GetPos()), Convert2DtoDisp(m_LastFindPos));

				//this->m_Phase = ENUM_AI_PHASE::Normal;
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
					float ViewLimit = Get2DSize(5.f);
					if ((Chara->GetPos() - Target->GetPos()).sqrMagnitude() < ViewLimit * ViewLimit) {
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
			//
			void		Dispose() noexcept {
				m_PathChecker.Dispose();
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
		void AIControl::SetPlayerID(PlayerID MyCharaID) noexcept { this->GetParam()->m_MyCharaID = MyCharaID; }
		// 
		void AIControl::Init() noexcept {
			this->GetParam()->Init();
		}
		void AIControl::Execute(InputControl* MyInput) noexcept {
			// return;
			// AI
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
		void AIControl::Draw() noexcept
		{
			//経路を示す
			/*
			float Radius = (float)GetDispSize(0.5f);

			{
				auto* BackGround = BackGroundClassBase::Instance();
				auto* PUnit = this->GetParam()->m_PathChecker.GetStartUnit();
				while (true) {
					auto DispPos = Convert2DtoDisp(BackGround->GetFloorData(PUnit->GetIndex())->m_Pos);
					DrawCircle((int)DispPos.x, (int)DispPos.y, Radius, Red);
					PUnit = PUnit->GetNextUnit();
					if (!PUnit) { break; }
				}
			}
			{
				auto DispPos = Convert2DtoDisp(this->GetParam()->m_LastFindPos);
				DrawCircle((int)DispPos.x, (int)DispPos.y, Radius, Red);
			}
			// */
		}
		void AIControl::Dispose() noexcept {
			this->GetParam()->Dispose();
		}
	};
};
