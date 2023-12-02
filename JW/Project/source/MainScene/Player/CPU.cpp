#include	"CPU.hpp"
#include "../../NetWork.hpp"
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"
#include "../../Scene/MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//AI用
		class AIControl::Impl {
		public:
			ENUM_AI_PHASE							m_Phase{ ENUM_AI_PHASE::Normal };
			PlayerID								m_MyCharaID{ 0 };
			PlayerID								m_TargetCharaID{ 0 };
		private:
			std::shared_ptr<BackGroundClassBase>	m_BackGround{ nullptr };				//BG
			std::array<int, 6>						m_WayPoints{ 0,0,0,0,0,0 };
			float									m_CheckAgain{ 0.f };
			float									m_ShotTimer{ 0.f };
			float									m_BackTimer{ 0.f };
			int										m_LeanLR{ 0 };
		public:
			BOOL CanLookTarget{ true };
		private:
			bool W_key{ false };
			bool S_key{ false };
			bool A_key{ false };
			bool D_key{ false };
			bool Run_key{ false };
			bool Q_key{ false };
			bool E_key{ false };
			bool R_key{ false };
			bool F_key{ false };
			bool C_key{ false };
			bool shotMain_Key{ false };
			int32_t x_m{ 0 }, y_m{ 0 };
		public:
			Impl(void) noexcept { }
			~Impl(void) noexcept { }
		private:
			void		SetBackGround(const std::shared_ptr<BackGroundClassBase>& BackBround_t) noexcept { m_BackGround = BackBround_t; }

			void		FillWayPoints(int now) noexcept {
				for (auto& w : this->m_WayPoints) { w = now; }
			}
			void		PushFrontWayPoint(int now) noexcept {
				for (size_t i = (this->m_WayPoints.size() - 1); i >= 1; i--) { this->m_WayPoints[i] = this->m_WayPoints[i - 1]; }
				this->m_WayPoints[0] = now;
			}
		private:
			VECTOR_ref	GetNowWaypointPos() noexcept {
				auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);
				auto& C = MainGB->GetBuildDatas().at(this->m_WayPoints[0]);
				return C.GetMatrix().pos();
			}
			int			GetNextWaypoint(const VECTOR_ref& Dir, int Rank) noexcept {//todo:何番目に近いポイント
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(m_MyCharaID).GetChara();
				auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);

				std::vector<std::pair<float, int>> DirChecks;
				bool CheckDir = (Dir.Length() > 0.f);
				for (auto& C : MainGB->GetBuildDatas()) {
					if (C.GetMeshSel() < 0) { continue; }
					int index = (int)(&C - &MainGB->GetBuildDatas().front());
					if (std::find(m_WayPoints.begin(), m_WayPoints.end(), index) != m_WayPoints.end()) {
						continue;
					}
					auto Vec = C.GetMatrix().pos() - MyChara->GetMove().pos;
					auto Len = Vec.Length();
					if (Len < 5.f*Scale_Rate) {
						if (CheckDir) {
							auto Dir_XZ = Dir; Dir_XZ.y(0.f);
							auto Vec_XZ = Vec; Vec_XZ.y(0.f);
							Vec_XZ = Vec_XZ.Norm();
							auto Dot = Vec_XZ.dot(Dir_XZ.Norm());
							if (Dot > 0.f) {
								auto StartPos = MyChara->GetMove().pos + VECTOR_ref::up()*1.f*Scale_Rate;
								auto EndPos = StartPos + Vec_XZ * (5.f*Scale_Rate);
								if (GetMinLenSegmentToPoint(StartPos, EndPos, C.GetMatrix().pos()) >= 5.f*Scale_Rate) { continue; }
								auto ret = C.GetColLine(StartPos, EndPos);
								if (ret.HitFlag == FALSE) {
									DirChecks.emplace_back(std::make_pair(Dot, index));
								}
							}
						}
						else {
							DirChecks.emplace_back(std::make_pair(Len, index));
						}
					}
				}
				if (DirChecks.size() <= Rank) {
					return -1;
				}
				if (CheckDir) {
					std::sort(DirChecks.begin(), DirChecks.end(), [&](const std::pair<float, int>& A, const std::pair<float, int>& B) {return A.first > B.first; });
				}
				else {
					std::sort(DirChecks.begin(), DirChecks.end(), [&](const std::pair<float, int>& A, const std::pair<float, int>& B) {return A.first < B.first; });
				}
				return DirChecks.at(Rank).second;
			}
			void		SetNextWaypoint(const VECTOR_ref& vec_z) noexcept {
				int now = this->GetNextWaypoint(vec_z, 0);
				if (now != -1) {
					this->PushFrontWayPoint(now);
				}
			}

			void		Set_wayp_return(void) noexcept {
				auto ppp = this->m_WayPoints[1];
				this->FillWayPoints(this->m_WayPoints[0]);
				this->m_WayPoints[0] = ppp;
			}
		public:
			void		Reset() noexcept {
				this->m_Phase = ENUM_AI_PHASE::Normal;
				int now = GetNextWaypoint(VECTOR_ref::zero(), 0);
				this->FillWayPoints((now != -1) ? now : 0);
				this->m_CheckAgain = 0.f;
			}
		public:
			void		Init(const std::shared_ptr<BackGroundClassBase>& BackBround_t, PlayerID MyCharaID) noexcept {
				this->m_MyCharaID = MyCharaID;
				this->SetBackGround(BackBround_t);
				this->Reset();
			}
			void		Execute_Before() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();

				W_key = false;
				S_key = false;
				A_key = false;
				D_key = false;
				Run_key = false;
				Q_key = false;
				E_key = false;
				R_key = false;
				F_key = false;
				C_key = false;
				shotMain_Key = false;
				x_m = 0;
				y_m = 0;

				{
					auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);

					auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
					auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();
					auto MyPos = MyChara->GetEyePosition();
					//
					CanLookTarget = true;
					auto Dir_t = TgtPos - MyPos;
					if (Dir_t.Length() < 15.f*Scale_Rate) {
						for (auto& C : MainGB->GetBuildDatas()) {
							if (C.GetMeshSel() < 0) { continue; }
							auto StartPos = MyPos;
							auto EndPos = TgtPos;
							if (GetMinLenSegmentToPoint(StartPos, EndPos, C.GetMatrix().pos()) >= 5.f*Scale_Rate) { continue; }
							auto ret = C.GetColLine(StartPos, EndPos);
							if (ret.HitFlag == TRUE) {
								CanLookTarget = false;
								break;
							}
						}
					}
					else {
						CanLookTarget = false;
					}
				}

				if (MyChara->GetIsSquat()) {
					this->C_key = GetRand(100) < 1;
				}
			}
			void		Execute_Normal() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);

				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();
				auto TgtPos_XZ = TgtPos; TgtPos_XZ.y(0.f);

				auto Dir = MyChara->GetEyeVector();
				auto Dir_XZ = Dir; Dir_XZ.y(0.f);

				auto MyPos = MyChara->GetMove().pos;
				auto MyPos_XZ = MyPos; MyPos_XZ.y(0.f);

				//
				Dir_XZ = Dir_XZ.Norm();
				auto GonePoint = MainGB->GetBuildDatas().at(this->m_WayPoints[0]).GetMatrix().pos();
				GonePoint.y(0.f);

				auto Vec = GonePoint - MyPos_XZ;

				bool LookFront = true;
				bool LookLR = true;

				int next = this->GetNextWaypoint(Dir, 1);
				if (next != -1) {
					auto NextPoint = MainGB->GetBuildDatas().at(next).GetMatrix().pos();
					NextPoint.y(0.f);
					auto Vec2_XZ = NextPoint - MyPos_XZ;

					auto cross = Vec2_XZ.Norm().cross(Vec.Norm()).y();
					if (abs(cross) > sin(deg2rad(30.f))) {
						LookFront = false;
						LookLR = (cross > 0);
						Dir_XZ = MATRIX_ref::Vtrans(Dir_XZ, MATRIX_ref::RotY(deg2rad(LookLR ? 90 : -90)));
					}
				}
				if (LookFront) {
					A_key = GetRand(100) > 50;
					D_key = GetRand(100) > 50;
				}
				auto dot = Dir_XZ.dot(Vec.Norm());
				if (dot > 0.f) {
					auto cross = Dir_XZ.cross(Vec.Norm()).y();
					if (abs(cross) < sin(deg2rad(40.f))) {
						if (LookFront) {
							W_key = true;

							if (abs(cross) < sin(deg2rad(10.f))) {
								Run_key = true;
							}
						}
						else {
							if (LookLR) {
								D_key = true;
							}
							else {
								A_key = true;
							}
						}
					}
					y_m = (int32_t)(700.f*cross);
				}
				else {
					y_m = (int32_t)(LookLR ? 1000.f : -1000.f);
				}
				if (Vec.Length() < 1.5f*Scale_Rate) {
					auto Dir_t = TgtPos - MyPos;
					if (Dir_t.Length() < 10.f*Scale_Rate) {
						Dir = Dir_t.Norm();
					}
					int now = this->GetNextWaypoint(Dir, 0);
					if (now != -1) {
						this->PushFrontWayPoint(now);
					}
					else {
						this->Set_wayp_return();
					}
					this->m_CheckAgain = 0.f;
				}
				else {
					this->m_CheckAgain += 1.f / FPS;
					if (this->m_CheckAgain > 3.f) {
						this->Reset();
					}
				}
				if (CanLookTarget) {
					auto Dir_t = TgtPos - MyPos;
					if (Dir_t.Length() < 20.f*Scale_Rate) {
						this->m_Phase = ENUM_AI_PHASE::Shot;
					}
				}
			}
			void		Execute_Shot() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID).GetChara();
				//auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);

				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();
				auto TgtPos_XZ = TgtPos; TgtPos_XZ.y(0.f);

				auto Dir = MyChara->GetEyeVector();
				auto Dir_XZ = Dir; Dir_XZ.y(0.f);

				auto MyPos = MyChara->GetMove().pos;
				auto MyPos_XZ = MyPos; MyPos_XZ.y(0.f);

				//
				auto VecT = TgtPos - MyChara->GetEyePosition();
				VecT = MATRIX_ref::Vtrans(VecT.Norm(), MATRIX_ref::RotX(deg2rad(GetRandf(25.f))) * MATRIX_ref::RotY(deg2rad(GetRandf(25.f))));
				auto Vec = VecT; Vec.y(0.f);
				{
					auto dot = Dir_XZ.Norm().dot(Vec.Norm());
					auto cross = Dir_XZ.Norm().cross(Vec.Norm()).y();
					if (dot > 0.f) {
						if (abs(cross) < 0.4f) {
							y_m = (int32_t)(700.f*cross);
						}
						else {
							y_m = (int32_t)(1400.f*cross);
						}
					}
					else {
						y_m = (int32_t)((cross > 0) ? 1000.f : -1000.f);
					}

					if (!((dot > 0.f) && (abs(cross) < 0.4f))) {
						if (y_m > 700) {
							this->m_LeanLR = 1;
						}
						if (y_m < -700) {
							this->m_LeanLR = -1;
						}
					}
				}
				{
					VECTOR_ref DirHY; DirHY.Set(Dir_XZ.Length(), 0.f, Dir.y()); DirHY = DirHY.Norm();
					VECTOR_ref VecHY; VecHY.Set(Vec.Length(), 0.f, VecT.y()); VecHY = VecHY.Norm();
					auto dot = DirHY.dot(VecHY);
					auto cross = DirHY.cross(VecHY).y();
					if (dot > 0.f) {
						x_m = -(int32_t)(400.f*cross);
					}
				}
				//リーン
				if (this->m_LeanLR == 1) {
					Q_key = true;
				}
				else if (this->m_LeanLR == -1) {
					E_key = true;
				}
				//
				if (this->m_ShotTimer == 0.f) {
					shotMain_Key = GetRand(100) < 10;
					if (shotMain_Key) {
						if (CanLookTarget) {
							this->m_ShotTimer = (float)(50 + GetRand(200)) / 100.f;
						}
						else {
							this->m_ShotTimer = (float)(10 + GetRand(100)) / 100.f;
						}
					}
				}
				else {
					this->m_ShotTimer = std::max(this->m_ShotTimer - 1.f / FPS, 0.f);
				}

				if (!CanLookTarget) {
					auto Dir_t = TgtPos - MyPos;
					if (Dir_t.Length() > 20.f*Scale_Rate) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
					}
					if (this->m_CheckAgain == 0.f) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
					}
					this->m_CheckAgain = std::max(this->m_CheckAgain - 1.f / FPS, 0.f);
					this->m_BackTimer = 3.f + GetRandf(2.f);
				}
				else {
					this->m_CheckAgain = 5.f;

					this->m_BackTimer = std::max(this->m_BackTimer - 1.f / FPS, 0.f);
					if (this->m_BackTimer == 0.f) {
						this->m_BackTimer = 3.f + GetRandf(2.f);
					}
					if (this->m_BackTimer <= 1.f) {
						S_key = true;
					}
				}
			}
			void		Execute_After(InputControl* MyInput) noexcept {
				MyInput->SetInput(
					(float)this->x_m / 10000.f,
					(float)this->y_m / 10000.f,
					this->W_key, this->S_key, this->A_key, this->D_key,
					this->Run_key,
					this->Q_key, this->E_key,//QE
					false, false, false, false,
					false,//1
					this->R_key,
					this->F_key,
					this->C_key,
					this->shotMain_Key,
					false
				);
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
		void AIControl::Init(const std::shared_ptr<BackGroundClassBase>& BackBround_t, PlayerID MyCharaID) noexcept {
			this->GetParam()->Init(BackBround_t, MyCharaID);
		}
		void AIControl::Execute(InputControl* MyInput) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->GetParam()->m_MyCharaID).GetChara();
			//AI
			this->GetParam()->Execute_Before();
			if (MyChara->IsAlive()) {
				switch (this->GetParam()->m_Phase) {
				case ENUM_AI_PHASE::Normal:
					this->GetParam()->Execute_Normal();
					break;
				case ENUM_AI_PHASE::Shot:
					this->GetParam()->Execute_Shot();
					break;
				default:
					break;
				}
			}
			else {
				if (!this->GetParam()->CanLookTarget) {
					auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
					auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();
					auto MyPos = MyChara->GetEyePosition();

					auto Dir_t = TgtPos - MyPos;
					if (Dir_t.Length() > 20.f*Scale_Rate) {
						MyChara->Heal(100);
						this->GetParam()->Reset();
					}
				}
			}
			this->GetParam()->Execute_After(MyInput);
		}
		void AIControl::Draw() noexcept {
#ifdef DEBUG
			if (this->GetParam()->m_Phase != ENUM_AI_PHASE::Normal) {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->GetParam()->m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->GetParam()->m_TargetCharaID).GetChara();
				auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();
				auto MyPos = MyChara->GetEyePosition();
				DrawCapsule_3D(MyPos, TgtPos, 0.1f*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
			}
#endif
		}
	};
};
