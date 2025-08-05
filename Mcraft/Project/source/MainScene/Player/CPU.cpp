#include	"CPU.hpp"
#include	"../../MainScene/NetWork.hpp"
#include	"../../MainScene/Object/Character.hpp"
#include	"../../MainScene/Player/Player.hpp"
#include	"../../Scene/MainScene.hpp"

namespace FPS_n2 {
	namespace AIs {
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
					auto* BackGroundParts = BackGround::BackGroundControl::Instance();
					// 隣接するポリゴンが既に経路探索処理が行われていて、且つより距離の長い経路となっている場合は何もしない
					auto& Unit = BackGroundParts->GetBuildData().at(PUnit->PolyIndex);

					auto trisdistance = PUnit->TotalDistance +
						(BackGroundParts->GetBuildData().at(Unit.GetLinkPolyIndex(tris)).GetPos() + Unit.GetPos()).magnitude();

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
			Vector3DX GoalPosition;					// 目標位置
			std::vector<PATHPLANNING_UNIT>UnitArray;	// 経路探索処理で使用する全ポリゴンの情報配列が格納されたメモリ領域の先頭メモリアドレスを格納する変数
			PATHPLANNING_UNIT* StartUnit{ nullptr };			// 経路のスタート地点にあるポリゴン情報へのメモリアドレスを格納する変数
			PATHPLANNING_UNIT* GoalUnit{ nullptr };				// 経路のゴール地点にあるポリゴン情報へのメモリアドレスを格納する変数
		public:
			const auto& GetStartUnit() const noexcept { return this->StartUnit; }
		public:
			Vector3DX GetNextPoint(const Vector3DX& NowPosition, int* TargetPathPlanningIndex) const {
				auto* BackGroundParts = BackGround::BackGroundControl::Instance();
				int NowIndex = BackGroundParts->GetNearestBuilds(NowPosition);
				if (!((*TargetPathPlanningIndex != -1) && (this->GoalUnit))) {
					return BackGroundParts->GetBuildData().at(BackGroundParts->GetNearestBuilds2(NowPosition)).GetPos();
				}
				if (NowIndex != this->GoalUnit->GetPolyIndex()) {																	// 現在乗っているポリゴンがゴール地点にあるポリゴンの場合は処理を分岐
					if (NowIndex == *TargetPathPlanningIndex) {													// 現在乗っているポリゴンが移動中間地点のポリゴンの場合は次の中間地点を決定する処理を行う
						const float COLLWIDTH = 0.001f * Scale3DRate;												// 当たり判定のサイズ
						while (true) {																				// 次の中間地点が決定するまでループし続ける
							if (!this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()) { break; }
							auto NextIndex = this->UnitArray.at(*TargetPathPlanningIndex).GetNextPolyUnit()->GetPolyIndex();
							if (!BackGroundParts->CheckPolyMoveWidth(NowPosition, NextIndex, COLLWIDTH)) { break; }		// 経路上の次のポリゴンの中心座標に直線的に移動できない場合はループから抜ける
							(*TargetPathPlanningIndex) = NextIndex;													// チェック対象を経路上の更に一つ先のポリゴンに変更する
							if ((*TargetPathPlanningIndex) == this->GoalUnit->GetPolyIndex()) { break; }				// もしゴール地点のポリゴンだったらループを抜ける
						}
					}
					// 移動方向を決定する、移動方向は現在の座標から中間地点のポリゴンの中心座標に向かう方向
					return BackGroundParts->GetBuildData().at(*TargetPathPlanningIndex).GetPos();
				}
				else {
					// 方向は目標座標
					return this->GoalPosition;
				}
			}
		public:
			bool Init(Vector3DX StartPos, Vector3DX GoalPos) {
				auto* BackGroundParts = BackGround::BackGroundControl::Instance();
				// 指定の２点の経路を探索する( 戻り値 true:経路構築成功 false:経路構築失敗( スタート地点とゴール地点を繋ぐ経路が無かった等 ) )
				this->GoalPosition = GoalPos;			// ゴール位置を保存

				this->UnitArray.resize(BackGroundParts->GetBuildData().size());			// 経路探索用のポリゴン情報を格納するメモリ領域を確保、初期化
				for (auto& p : this->UnitArray) {
					p.Init(static_cast<int>(&p - &this->UnitArray.front()));
				}

				int StartIndex = BackGroundParts->GetNearestBuilds(StartPos);	// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
				if (StartIndex == -1) { return false; }
				this->StartUnit = &this->UnitArray[StartIndex];					// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存

				int GoalIndex = BackGroundParts->GetNearestBuilds(GoalPos);		// ゴール地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
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
							int Index = BackGroundParts->GetBuildData().at(PUnit->GetPolyIndex()).GetLinkPolyIndex(K);
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
		public:
			ENUM_AI_PHASE							m_Phase{ ENUM_AI_PHASE::Normal };
			PlayerID								m_MyCharaID{ 0 };
			InputControl							m_MyInput;
		private:
			const PlayerID							m_TargetCharaID{ 0 };

			int										TargetPathPlanningIndex{ 0 };		// 次の中間地点となる経路上のポリゴンの経路探索情報が格納されているメモリアドレスを格納する変数
			PathChecker								m_PathChecker;

			float									m_PathUpdateTimer{ 0.f };
			float									m_CheckAgain{ 0.f };
			float									m_ShotTimer{ 0.f };
			float									m_BackTimer{ 0.f };
			float									m_MoveFrontTimer{ 0.f };

			int										m_LeanLR{ 0 };
		public:
			Impl(void) noexcept {}
			~Impl(void) noexcept {}
		private:
			bool									IsGotLengthToTarget{ true };
			float									LengthToTarget{ 0.f };
			Vector3DX								VectorToTarget;
			const Vector3DX GetTargetPos() const noexcept {
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& TargetChara = PlayerMngr->GetPlayer(this->m_TargetCharaID)->GetChara();
				//return TargetChara->GetEyePositionCache();
				return
					Lerp(TargetChara->GetMove().GetPos(), TargetChara->GetEyePositionCache(), std::clamp(static_cast<float>(GetRand(100)) / 100.f, 0.f, 1.f)) +
					Vector3DX::vget(GetRandf(1.f), 0.f, GetRandf(1.f)) * Scale3DRate;
			}
			float					GetLengthToTarget() {
				if (IsGotLengthToTarget) {
					return LengthToTarget;
				}
				IsGotLengthToTarget = true;
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
				auto TgtPos = GetTargetPos();

				Vector3DX MyPos = MyChara->GetEyePositionCache();

				LengthToTarget = (TgtPos - MyPos).magnitude();
				VectorToTarget = (TgtPos - MyPos).normalized();

				return LengthToTarget;
			}
			const Vector3DX& GetVectorToTarget() {
				if (IsGotLengthToTarget) {
					return VectorToTarget;
				}
				IsGotLengthToTarget = true;
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
				auto TgtPos = GetTargetPos();

				Vector3DX MyPos = MyChara->GetEyePositionCache();

				LengthToTarget = (TgtPos - MyPos).magnitude();
				VectorToTarget = (TgtPos - MyPos).normalized();

				return VectorToTarget;
			}
		private:
			void		Reset() noexcept {
				this->m_Phase = ENUM_AI_PHASE::Normal;
				this->m_CheckAgain = 0.f;
				this->m_PathUpdateTimer = 1.f;
			}
			void		ChangePoint() noexcept {
				auto* BackGroundParts = BackGround::BackGroundControl::Instance();
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
				auto TgtPos = GetTargetPos();
				Vector3DX MyPos = MyChara->GetEyePositionCache();

				auto Target = TgtPos;
				std::vector<int> SelList;
				SelList.clear();
				auto GetRandomMyPos = [&]() {
					if (SelList.size() == 0) {
						for (auto& C : BackGroundParts->GetBuildData()) {
							auto Vec = C.GetPos() - MyPos; Vec.y = (0.f);
							if (Vec.magnitude() < 10.f * Scale3DRate) {
								SelList.emplace_back(static_cast<int>(&C - &BackGroundParts->GetBuildData().front()));
							}
						}
					}
					MyPos = BackGroundParts->GetBuildData().at(SelList.at(GetRand((int)SelList.size() - 1))).GetPos();
					};
				auto CheckPathToTarget = [&]() {
					m_PathChecker.Dispose();
					return m_PathChecker.Init(MyPos, Target);	// 指定の２点の経路情報を探索する
					};

				if (GetLengthToTarget() > 20.f * Scale3DRate) {
					if (SelList.size() == 0) {
						for (auto& C : BackGroundParts->GetBuildData()) {
							auto Vec = C.GetPos() - Target; Vec.y = (0.f);
							if (Vec.magnitude() < 10.f * Scale3DRate) {
								SelList.emplace_back(static_cast<int>(&C - &BackGroundParts->GetBuildData().front()));
							}
						}
					}
					Target = BackGroundParts->GetBuildData().at(SelList.at(GetRand((int)SelList.size() - 1))).GetPos();
				}
				this->TargetPathPlanningIndex = -1;
				for (int i = 0; i < 10; i++) {
					if (CheckPathToTarget()) {
						this->TargetPathPlanningIndex = m_PathChecker.GetStartUnit()->GetPolyIndex();	// 移動開始時点の移動中間地点の経路探索情報もスタート地点にあるポリゴンの情報
						break;
					}
					else {
						GetRandomMyPos();//再選定
					}
				}
			}
			void		Repop(bool CanRepop) noexcept {
				auto* BackGroundParts = BackGround::BackGroundControl::Instance();
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
				auto TgtPos = GetTargetPos();

				Vector3DX pos_t;
				while (true) {
					pos_t = BackGroundParts->GetBuildData().at(static_cast<size_t>(GetRand(static_cast<int>(BackGroundParts->GetBuildData().size()) - 1))).GetPos();

					Vector3DX EndPos = pos_t + Vector3DX::up() * 1.f * Scale3DRate;
					if (CanRepop) {
						if ((TgtPos - EndPos).magnitude() <= 10.f * Scale3DRate) { continue; }
					}
					else {
						if ((TgtPos - EndPos).magnitude() >= 15.f * Scale3DRate) { continue; }
					}
					if (BackGroundParts->CheckLinetoMap(TgtPos, &EndPos) != 0) { break; }
				}

				/*
				Vector3DX EndPos = pos_t + Vector3DX::up() * 10.f*Scale3DRate;
				if (BackGroundParts->CheckLinetoMap(pos_t + Vector3DX::up() * -10.f*Scale3DRate, &EndPos, false) != 0) {
					pos_t = EndPos;
				}
				//*/

				MyChara->Spawn(deg2rad(0.f), deg2rad(GetRandf(180.f)), pos_t, 0, true, 0.f);
				this->Reset();
			}
			void		AimDir(const Vector3DX& VEC) {
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();

				Vector3DX Vec = VEC; Vec.y = (0.f);

				auto Dir = MyChara->GetEyeRotationCache().zvec() * -1.f;
				auto Dir_XZ = Dir; Dir_XZ.y = (0.f);
				{
					Vector3DX DirHY; DirHY.Set(Dir_XZ.magnitude(), 0.f, Dir.y);
					Vector3DX VecHY; VecHY.Set(Vec.magnitude(), 0.f, VEC.y);
					auto IsFront = ((Vector3DX::Dot(DirHY.normalized(), VecHY.normalized())) > 0.f);
					auto cross = Vector3DX::Cross(DirHY.normalized(), VecHY.normalized()).y;
					m_MyInput.SetAddxRad(IsFront ? (-0.04f * cross) : 0.f);
				}
				{
					auto IsFront = ((Vector3DX::Dot(Dir_XZ.normalized(), Vec.normalized())) > 0.f);
					auto cross = Vector3DX::Cross(Dir_XZ.normalized(), Vec.normalized()).y;
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
						if (this->m_MyInput.GetAddyRad() > 0.07f) {
							this->m_LeanLR = 1;
						}
						if (this->m_MyInput.GetAddyRad() < -0.07f) {
							this->m_LeanLR = -1;
						}
					}
				}
			}
		public:
			void		SetMyCharaID(PlayerID MyCharaID) noexcept { this->m_MyCharaID = MyCharaID; }
		public:
			void		Init() noexcept {
				this->Reset();
				this->m_PathUpdateTimer = 0.f;
				this->m_MoveFrontTimer = static_cast<float>(GetRand(6));
			}
			//
			void		Execute_Before() noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				//初期化
				m_MyInput.ResetAllInput();
				IsGotLengthToTarget = false;
				//前準備
				m_PathUpdateTimer = std::max(this->m_PathUpdateTimer - DXLib_refParts->GetDeltaTime(), 0.f);
				if (this->m_PathUpdateTimer <= 0.f) {
					m_PathUpdateTimer += 1.f;
					this->ChangePoint();
				}

				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();

				m_MyInput.SetInputPADS(Controls::PADS::SQUAT, MyChara->GetIsSquat() && (GetRand(100) < 1));

				//CanRepop
				if (!MyChara->IsAlive()) {
					this->m_Phase = ENUM_AI_PHASE::Dead;
				}
			}
			void		Execute_Normal() noexcept {
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
				Vector3DX MyPos = MyChara->GetEyePositionCache();
				//
				//エイム
				Vector3DX Vec = m_PathChecker.GetNextPoint(MyChara->GetFrameWorldMat(Charas::CharaFrame::Upper2).pos(), &this->TargetPathPlanningIndex) - MyPos; Vec.y = (0.f);
				AimDir(Matrix4x4DX::Vtrans(Vec, Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(GetRandf(15.f))) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(15.f)))));
				//
				m_MyInput.SetInputPADS(Controls::PADS::MOVE_W, true);
				m_MyInput.SetInputPADS(Controls::PADS::MOVE_A, GetRand(100) > 50);
				m_MyInput.SetInputPADS(Controls::PADS::MOVE_D, GetRand(100) > 50);
				//
				if (MyChara->GetCanLookByPlayer()) {
					this->m_CheckAgain = 0.f;
					this->m_Phase = ENUM_AI_PHASE::Shot;
					if (PlayerMngr->m_FindCount <= 0.f) {
						PlayerMngr->m_FindCount = 10.f;
						if (GetLengthToTarget() < 10.f * Scale3DRate) {
							auto SE = SoundPool::Instance();
							if (GetRand(1) == 0) {
								SE->Get(SoundType::SE, (int)SoundEnum::Man_contact)->Play3D(MyPos, Scale3DRate * 10.f);
							}
							else {
								SE->Get(SoundType::SE, (int)SoundEnum::Man_openfire)->Play3D(MyPos, Scale3DRate * 10.f);
							}
						}
					}
				}
			}
			void		Execute_Shot() noexcept {
				auto* DXLib_refParts = DXLib_ref::Instance();
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
				Vector3DX MyPos = MyChara->GetEyePositionCache();
				//エイム
				AimDir(Matrix4x4DX::Vtrans(GetVectorToTarget(), Matrix4x4DX::RotAxis(Vector3DX::right(), deg2rad(GetRandf(15.f))) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(GetRandf(15.f)))));
				//リーン
				m_MyInput.SetInputPADS(Controls::PADS::LEAN_L, (this->m_LeanLR == 1));
				m_MyInput.SetInputPADS(Controls::PADS::LEAN_R, (this->m_LeanLR == -1));
				//後退
				if (MyChara->GetCanLookByPlayer()) {
					this->m_BackTimer = std::max(this->m_BackTimer - DXLib_refParts->GetDeltaTime(), 0.f);
					if (this->m_BackTimer == 0.f) {
						this->m_BackTimer = 3.f + GetRandf(2.f);
					}
					if (this->m_BackTimer <= 1.f) {
						m_MyInput.SetInputPADS(Controls::PADS::MOVE_S, true);
					}
				}
				else {
					this->m_BackTimer = 3.f + GetRandf(2.f);
				}

				//近い他人から離れる
				{
					auto Dir = MyChara->GetEyeRotationCache().zvec() * -1.f;
					auto Dir_XZ = Dir; Dir_XZ.y = (0.f);
					for (int loop = 0; loop < PlayerMngr->GetPlayerNum(); ++loop) {
						if (this->m_MyCharaID == loop) { continue; }
						Vector3DX Vec = PlayerMngr->GetPlayer(loop)->GetChara()->GetEyePositionCache() - MyPos; Vec.y = 0.f;
						if (Vec.sqrMagnitude() < (2.f * Scale3DRate) * (2.f * Scale3DRate)) {

							auto IsFront = ((Vector3DX::Dot(Dir_XZ.normalized(), Vec.normalized())) > 0.f);
							auto cross = Vector3DX::Cross(Dir_XZ.normalized(), Vec.normalized()).y;
							if (IsFront) {
								m_MyInput.SetInputPADS(Controls::PADS::MOVE_S, true);
							}
							else {
								m_MyInput.SetInputPADS(Controls::PADS::MOVE_W, true);
							}
							if (!(IsFront && (abs(cross) < 0.4f))) {
								if (this->m_MyInput.GetAddyRad() > 0.07f) {
									m_MyInput.SetInputPADS(Controls::PADS::MOVE_D, true);
								}
								if (this->m_MyInput.GetAddyRad() < -0.07f) {
									m_MyInput.SetInputPADS(Controls::PADS::MOVE_A, true);
								}
							}

							break;
						}
					}
				}
				//
				m_MyInput.SetInputPADS(Controls::PADS::AIM, true);
				//
				this->m_ShotTimer = std::max(this->m_ShotTimer - DXLib_refParts->GetDeltaTime(), 0.f);
				if (this->m_ShotTimer == 0.f) {
					this->m_ShotTimer = static_cast<float>(50 + GetRand(100)) / 100.f;
					Vector3DX Vec = GetVectorToTarget(); Vec.y = 0.f;
					Vector3DX Vec2 = MyChara->GetEyeRotationCache().zvec() * -1.f; Vec2.y = 0.f;
					m_MyInput.SetInputPADS(Controls::PADS::SHOT, (MyChara->GetCanLookByPlayer() && (Vector3DX::Dot(Vec, Vec2) > cos(deg2rad(30)))));

				}
				//
				if (!MyChara->GetCanLookByPlayer()) {
					if (
						(GetLengthToTarget() > 20.f * Scale3DRate) ||
						(this->m_CheckAgain == 0.f)
						) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
					}
					this->m_CheckAgain = std::max(this->m_CheckAgain - DXLib_refParts->GetDeltaTime(), 0.f);
				}
				else {
					this->m_CheckAgain = 5.f;
				}
			}
			void		Execute_Dead() noexcept {
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
				if (MyChara->IsAlive()) {
					this->m_Phase = ENUM_AI_PHASE::Normal;
				}
			}


			void		Draw() noexcept {
				return;
				auto* PlayerMngr = Player::PlayerManager::Instance();
				auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();

				Vector3DX MyPos = MyChara->GetEyePositionCache();
				int tmp = this->TargetPathPlanningIndex;
				Vector3DX Pos = m_PathChecker.GetNextPoint(MyPos, &tmp);
				Pos.y = MyPos.y;
				DrawCapsule_3D(MyPos, Pos, 1.f, GetColor(255, 0, 0), GetColor(255, 0, 0));
			}
		};
		//
		void		AIControl::Repop(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();

			Vector3DX ZVec{};
			Vector3DX Pos{};
			if (this->m_IsLeftHeli) {
				ZVec = PlayerMngr->GetHelicopter()->GetMove().GetMat().xvec() * -1.0f;
				Pos = PlayerMngr->GetHelicopter()->GetObj().GetFrameLocalWorldMatrix(PlayerMngr->GetHelicopter()->GetFrame(static_cast<int>(Objects::HeliFrame::Rappelling2))).pos();
			}
			else {
				ZVec = PlayerMngr->GetHelicopter()->GetMove().GetMat().xvec();
				Pos = PlayerMngr->GetHelicopter()->GetObj().GetFrameLocalWorldMatrix(PlayerMngr->GetHelicopter()->GetFrame(static_cast<int>(Objects::HeliFrame::Rappelling1))).pos();
			}
			MyChara->Spawn(deg2rad(0.0f), std::atan2(ZVec.x, ZVec.z) + deg2rad(GetRandf(10.0f)), Pos, 2, false, 0.f);
			MyChara->SetRappelling();
			this->m_RapeTimer = 0.0f;
		}
		//
		AIControl::AIControl(PlayerID MyID) noexcept {
			m_Param = new Impl;
			Init(MyID);
		}
		AIControl::~AIControl(void) noexcept {
			Dispose();
			delete m_Param;
		}
		//
		void AIControl::Init(PlayerID MyID) noexcept {
			this->m_MyCharaID = MyID;
			this->GetParam()->SetMyCharaID(MyID);
			this->GetParam()->Init();
		}
		const InputControl AIControl::Update(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();

			if (MyChara->IsAlive()) {
				this->m_RepopTimer = 0.0f;

				if (MyChara->GetIsRappelling()) {
					this->m_RapeTimer += DXLib_refParts->GetDeltaTime();

					Vector3DX Pos{};
					if (this->m_IsLeftHeli) {
						Pos = PlayerMngr->GetHelicopter()->GetObj().GetFrameLocalWorldMatrix(PlayerMngr->GetHelicopter()->GetFrame(static_cast<int>(Objects::HeliFrame::Rappelling2))).pos();
					}
					else {
						Pos = PlayerMngr->GetHelicopter()->GetObj().GetFrameLocalWorldMatrix(PlayerMngr->GetHelicopter()->GetFrame(static_cast<int>(Objects::HeliFrame::Rappelling1))).pos();
					}
					NetWork::MoveInfo MoveInfoData;
					MoveInfoData.repos = MyChara->GetMove().GetRePos();
					MoveInfoData.pos = Pos - PlayerMngr->GetHelicopter()->GetMove().GetMat().yvec()*(60.0f * this->m_RapeTimer);
					MoveInfoData.vec = Vector3DX::zero();
					MoveInfoData.mat = MyChara->GetMove().GetMat();
					MoveInfoData.WatchRad = MyChara->GetRotateRad();
					MyChara->SetMoveOverRide(MoveInfoData);
				}
			}
			else {
				this->m_RepopTimer += DXLib_refParts->GetDeltaTime();
				if (this->m_RepopTimer > 5.0f) {
					this->m_RepopTimer -= 5.0f;
					if (PlayerMngr->GetHelicopter()->GetIsActiveRappelling()) {
						this->m_IsLeftHeli = (PlayerMngr->GetHelicopter()->PopSpawnPoint() % 2) == 0;
						Repop();
					}
				}
			}
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
				this->GetParam()->Execute_Dead();
				break;
			default:
				break;
			}
			if (MyChara->IsAlive() && !MyChara->GetIsRappelling()) {
				return this->GetParam()->m_MyInput;
			}
			else {
				return InputControl();
			}
		}

		void AIControl::Draw() noexcept {
			this->GetParam()->Draw();
		}
	}
}
