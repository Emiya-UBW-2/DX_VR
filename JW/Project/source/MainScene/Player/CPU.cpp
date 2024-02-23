#include	"CPU.hpp"
#include "../../NetWork.hpp"
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"
#include "../../Scene/MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PathChecker {
			std::shared_ptr<BackGroundClassMain>	m_BackGround{ nullptr };				//BG
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
				bool SetPrevPolyUnit(PATHPLANNING_UNIT *PUnit, int tris, const std::shared_ptr<BackGroundClassMain>& m_BackGround) {
					// 隣接するポリゴンが既に経路探索処理が行われていて、且つより距離の長い経路となっている場合は何もしない
					auto& Unit = m_BackGround->GetBuildData().at(PUnit->PolyIndex);

					auto trisdistance = PUnit->TotalDistance +
						(m_BackGround->GetBuildData().at(Unit.GetLinkPolyIndex(tris)).GetMatrix().pos() + Unit.GetMatrix().pos()).Length();

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
			VECTOR_ref GoalPosition;					// 目標位置
			std::vector<PATHPLANNING_UNIT>UnitArray;	// 経路探索処理で使用する全ポリゴンの情報配列が格納されたメモリ領域の先頭メモリアドレスを格納する変数
			PATHPLANNING_UNIT *StartUnit{ nullptr };			// 経路のスタート地点にあるポリゴン情報へのメモリアドレスを格納する変数
			PATHPLANNING_UNIT *GoalUnit{ nullptr };				// 経路のゴール地点にあるポリゴン情報へのメモリアドレスを格納する変数
		public:
			void		SetBackGround(const std::shared_ptr<BackGroundClassMain>& BackBround_t) noexcept { m_BackGround = BackBround_t; }
			const auto& GetStartUnit() const noexcept { return this->StartUnit; }
		public:
			VECTOR_ref GetNextPoint(const VECTOR_ref& NowPosition, int *TargetPathPlanningIndex) const {
				int NowIndex = m_BackGround->GetNearestBuilds(NowPosition);
				if (NowIndex != this->GoalUnit->GetPolyIndex()) {																	// 現在乗っているポリゴンがゴール地点にあるポリゴンの場合は処理を分岐
					if (NowIndex == *TargetPathPlanningIndex) {													// 現在乗っているポリゴンが移動中間地点のポリゴンの場合は次の中間地点を決定する処理を行う
						const float COLLWIDTH = 0.1f*Scale_Rate;				// 当たり判定のサイズ
						while (true) {																				// 次の中間地点が決定するまでループし続ける
							if (!this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()) { break; }
							auto NextIndex = this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()->GetPolyIndex();
							if (!m_BackGround->CheckPolyMoveWidth(NowPosition, NextIndex, COLLWIDTH)) { break; }		// 経路上の次のポリゴンの中心座標に直線的に移動できない場合はループから抜ける
							(*TargetPathPlanningIndex) = NextIndex;													// チェック対象を経路上の更に一つ先のポリゴンに変更する
							if ((*TargetPathPlanningIndex) == this->GoalUnit->GetPolyIndex()) { break; }				// もしゴール地点のポリゴンだったらループを抜ける
						}
					}
					// 移動方向を決定する、移動方向は現在の座標から中間地点のポリゴンの中心座標に向かう方向
					return m_BackGround->GetBuildData().at(*TargetPathPlanningIndex).GetMatrix().pos();
				}
				else {
					// 方向は目標座標
					return  this->GoalPosition;
				}
			}
		public:
			bool Init(VECTOR_ref StartPos, VECTOR_ref GoalPos) {
				// 指定の２点の経路を探索する( 戻り値  true:経路構築成功  false:経路構築失敗( スタート地点とゴール地点を繋ぐ経路が無かった等 ) )
				this->GoalPosition = GoalPos;			// ゴール位置を保存

				this->UnitArray.resize(m_BackGround->GetBuildData().size());			// 経路探索用のポリゴン情報を格納するメモリ領域を確保、初期化
				for (auto& p : this->UnitArray) {
					p.Init((int)(&p - &this->UnitArray.front()));
				}

				int StartIndex = m_BackGround->GetNearestBuilds(StartPos);	// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
				if (StartIndex == -1) { return false; }
				this->StartUnit = &this->UnitArray[StartIndex];					// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存

				int GoalIndex = m_BackGround->GetNearestBuilds(GoalPos);		// ゴール地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
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
							int Index = m_BackGround->GetBuildData().at(PUnit->GetPolyIndex()).GetLinkPolyIndex(K);
							if (Index == -1) { continue; }											// 辺に隣接するポリゴンが無い場合は何もしない
							if (Index == this->StartUnit->GetPolyIndex()) { continue; }				//スタート地点のポリゴンだった場合は何もしない
							auto& NowUnit = this->UnitArray[Index];
							if (!NowUnit.SetPrevPolyUnit(PUnit, K, m_BackGround)) {
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
		public:
			ENUM_AI_PHASE							m_Phase{ ENUM_AI_PHASE::Normal };
			PlayerID								m_MyCharaID{ 0 };
			InputControl							m_MyInput;
		private:
			const PlayerID							m_TargetCharaID{ 0 };

			int										TargetPathPlanningIndex{ 0 };		// 次の中間地点となる経路上のポリゴンの経路探索情報が格納されているメモリアドレスを格納する変数
			PathChecker								m_PathChecker;

			std::shared_ptr<BackGroundClassMain>	m_BackGround{ nullptr };				//BG

			float									m_PathUpdateTimer{ 0.f };
			float									m_CheckAgain{ 0.f };
			float									m_ShotTimer{ 0.f };
			float									m_BackTimer{ 0.f };
			float									m_RepopTimer{ 0.f };
			float									m_MoveFrontTimer{ 0.f };

			int										m_LeanLR{ 0 };
		public:
			Impl(void) noexcept { }
			~Impl(void) noexcept { }
		private:
			bool									IsGotLengthToTarget{ true };
			float									LengthToTarget{ 0.f };
			VECTOR_ref								VectorToTarget;
			float					GetLengthToTarget() {
				if (IsGotLengthToTarget) {
					return LengthToTarget;
				}
				IsGotLengthToTarget = true;
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();

				auto MyPos = MyChara->GetEyePosition();

				LengthToTarget = (TgtPos - MyPos).Length();
				VectorToTarget = (TgtPos - MyPos).Norm();

				return LengthToTarget;
			}
			const VECTOR_ref&		GetVectorToTarget() {
				if (IsGotLengthToTarget) {
					return VectorToTarget;
				}
				IsGotLengthToTarget = true;
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();

				auto MyPos = MyChara->GetEyePosition();

				LengthToTarget = (TgtPos - MyPos).Length();
				VectorToTarget = (TgtPos - MyPos).Norm();

				return VectorToTarget;
			}
		private:
			void		Reset() noexcept {
				this->m_Phase = ENUM_AI_PHASE::Normal;
				this->m_CheckAgain = 0.f;
				this->m_PathUpdateTimer = 1.f;
			}
			void		ChangePoint() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();
				auto MyPos = MyChara->GetEyePosition();

				auto Target = TgtPos;
				if (GetLengthToTarget() > 10.f*Scale_Rate) {
					std::vector<int> SelList;
					for (auto& C : this->m_BackGround->GetBuildData()) {
						if (C.GetMeshSel() < 0) { continue; }
						auto Vec = C.GetMatrix().pos() - Target; Vec.y(0.f);
						if (Vec.Length() < 10.f*Scale_Rate) {
							SelList.emplace_back((int)(&C - &this->m_BackGround->GetBuildData().front()));
						}
					}
					Target = this->m_BackGround->GetBuildData().at(SelList.at(GetRand((int)SelList.size() - 1))).GetMatrix().pos();
				}
				m_PathChecker.Dispose();
				auto MyPos_XZ = MyPos; MyPos_XZ.y(0.f);
				Target.y(0.f);
				m_PathChecker.Init(MyPos_XZ, Target);	// 指定の２点の経路情報を探索する
				this->TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetPolyIndex();	// 移動開始時点の移動中間地点の経路探索情報もスタート地点にあるポリゴンの情報
			}
			void		Repop() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();

				VECTOR_ref pos_t;
				while (true) {
					pos_t = this->m_BackGround->GetBuildData().at(GetRand((int)(this->m_BackGround->GetBuildData().size()) - 1)).GetMatrix().pos();

					VECTOR_ref StartPos = TgtPos;
					VECTOR_ref EndPos = pos_t + VECTOR_ref::up() * 1.f*Scale_Rate;
					if (this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, false)) {
						break;
					}
				}

				auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -10.f*Scale_Rate, pos_t + VECTOR_ref::up() * 10.f*Scale_Rate);
				if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }


				MyChara->ValueSet(deg2rad(0.f), deg2rad(GetRandf(180.f)), pos_t, this->m_MyCharaID);
				MyChara->Heal(100);
				this->Reset();
			}
			void		AimDir(const VECTOR_ref& VEC) {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();

				auto Vec = VEC; Vec.y(0.f);

				auto Dir = MyChara->GetEyeVector();
				auto Dir_XZ = Dir; Dir_XZ.y(0.f);
				{
					VECTOR_ref DirHY; DirHY.Set(Dir_XZ.Length(), 0.f, Dir.y());
					VECTOR_ref VecHY; VecHY.Set(Vec.Length(), 0.f, VEC.y());
					auto IsFront = ((DirHY.Norm().dot(VecHY.Norm())) > 0.f);
					auto cross = DirHY.Norm().cross(VecHY.Norm()).y();
					m_MyInput.SetAddxRad(IsFront ? (-0.04f*cross) : 0.f);
				}
				{
					auto IsFront = ((Dir_XZ.Norm().dot(Vec.Norm())) > 0.f);
					auto cross = Dir_XZ.Norm().cross(Vec.Norm()).y();
					if (IsFront) {
						if (abs(cross) < 0.4f) {
							m_MyInput.SetAddyRad(cross * 0.07f);
						}
						else {
							m_MyInput.SetAddyRad(cross * 0.14f);
						}
					}
					else {
						m_MyInput.SetAddyRad((cross > 0) ? 0.1f : -0.1f);
					}
					if (!(IsFront && (abs(cross) < 0.4f))) {
						if (m_MyInput.GetAddyRad() > 0.07f) {
							this->m_LeanLR = 1;
						}
						if (m_MyInput.GetAddyRad() < -0.07f) {
							this->m_LeanLR = -1;
						}
					}
				}
			}
		public:
			void		SetupBackGround(const std::shared_ptr<BackGroundClassMain>& BackBround_t) noexcept {
				m_BackGround = BackBround_t;
				m_PathChecker.SetBackGround(this->m_BackGround);
			}
			void		SetMyCharaID(PlayerID MyCharaID) noexcept { this->m_MyCharaID = MyCharaID; }
		public:
			void		Init() noexcept {
				this->Reset();
				this->m_PathUpdateTimer = 0.f;
				this->m_MoveFrontTimer = (float)(GetRand(6));
			}
			//
			void		Execute_Before() noexcept {
				//初期化
				m_MyInput.SetInputStart(0, 0, VECTOR_ref::zero());
				IsGotLengthToTarget = false;
				//前準備
				m_PathUpdateTimer = std::max(m_PathUpdateTimer - 1.f / FPS, 0.f);
				if(m_PathUpdateTimer<=0.f){
					m_PathUpdateTimer += 1.f;
					this->ChangePoint();
				}

				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();

				m_MyInput.SetInputPADS(PADS::SQUAT, MyChara->GetIsSquat() && (GetRand(100) < 1));

				if (!MyChara->CanLookTarget && (GetLengthToTarget() > 15.f*Scale_Rate)) {
					this->m_RepopTimer += 1.f / FPS;
					if (this->m_RepopTimer > 10.f) {
						this->m_RepopTimer -= 10.f;
						Repop();
					}
				}
				else {
					this->m_RepopTimer = 0.f;
				}
				if (!MyChara->IsAlive()) {
					this->m_Phase = ENUM_AI_PHASE::Dead;
				}
			}
			void		Execute_Normal() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto MyPos = MyChara->GetEyePosition();
				//
				if (this->m_MoveFrontTimer>6.f) {
					this->m_MoveFrontTimer -= 6.f;
				}
				this->m_MoveFrontTimer += 1.f / FPS;
				m_MyInput.SetInputPADS(PADS::MOVE_W, this->m_MoveFrontTimer > 4.f);
				m_MyInput.SetInputPADS(PADS::MOVE_A, GetRand(100) > 50);
				m_MyInput.SetInputPADS(PADS::MOVE_D, GetRand(100) > 50);
				//エイム
				VECTOR_ref Vec = m_PathChecker.GetNextPoint(MyPos, &this->TargetPathPlanningIndex) - MyPos; Vec.y(0.f);
				AimDir(MATRIX_ref::Vtrans(Vec, MATRIX_ref::RotX(deg2rad(GetRandf(15.f))) * MATRIX_ref::RotY(deg2rad(GetRandf(15.f)))));
				//
				if (MyChara->CanLookTarget) {
					this->m_CheckAgain = 0.f;
					this->m_Phase = ENUM_AI_PHASE::Shot;
					if (GetLengthToTarget() < 5.f*Scale_Rate) {
						auto SE = SoundPool::Instance();
						if (GetRand(1) == 0) {
							SE->Get((int)SoundEnum::Man_contact).Play_3D(0, MyPos, Scale_Rate * 10.f);
						}
						else {
							SE->Get((int)SoundEnum::Man_openfire).Play_3D(0, MyPos, Scale_Rate * 10.f);
						}
					}
				}
			}
			void		Execute_Shot() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				//エイム
				AimDir(MATRIX_ref::Vtrans(GetVectorToTarget(), MATRIX_ref::RotX(deg2rad(GetRandf(15.f))) * MATRIX_ref::RotY(deg2rad(GetRandf(15.f)))));
				//リーン
				m_MyInput.SetInputPADS(PADS::LEAN_L, (this->m_LeanLR == 1));
				m_MyInput.SetInputPADS(PADS::LEAN_R, (this->m_LeanLR == -1));
				//後退
				if (MyChara->CanLookTarget) {
					this->m_BackTimer = std::max(this->m_BackTimer - 1.f / FPS, 0.f);
					if (this->m_BackTimer == 0.f) {
						this->m_BackTimer = 3.f + GetRandf(2.f);
					}
					if (this->m_BackTimer <= 1.f) {
						m_MyInput.SetInputPADS(PADS::MOVE_S, true);
					}
				}
				else {
					this->m_BackTimer = 3.f + GetRandf(2.f);
				}
				//
				this->m_ShotTimer = std::max(this->m_ShotTimer - 1.f / FPS, 0.f);
				if (this->m_ShotTimer == 0.f) {
					m_MyInput.SetInputPADS(PADS::SHOT, true);
					this->m_ShotTimer = MyChara->CanLookTarget ? ((float)(10 + GetRand(100)) / 100.f) : ((float)(50 + GetRand(400)) / 100.f);
				}
				//
				if (!MyChara->CanLookTarget) {
					if (
						(GetLengthToTarget() > 20.f*Scale_Rate) ||
						(this->m_CheckAgain == 0.f)
						) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
					}
					this->m_CheckAgain = std::max(this->m_CheckAgain - 1.f / FPS, 0.f);
				}
				else {
					this->m_CheckAgain = 5.f;
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
		void AIControl::Init(const std::shared_ptr<BackGroundClassMain>& BackBround_t, PlayerID MyCharaID) noexcept {
			this->GetParam()->SetupBackGround(BackBround_t);
			this->GetParam()->SetMyCharaID(MyCharaID);
			this->GetParam()->Init();
		}
		void AIControl::Execute(InputControl* MyInput) noexcept {
			//AI
			this->GetParam()->Execute_Before();
			switch (this->GetParam()->m_Phase) {
			case ENUM_AI_PHASE::Normal:
				this->GetParam()->Execute_Normal();
				break;
			case ENUM_AI_PHASE::Shot:
				this->GetParam()->Execute_Shot();
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
