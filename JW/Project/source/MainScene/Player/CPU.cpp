#include	"CPU.hpp"
#include "../../NetWork.hpp"
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"
#include "../../Scene/MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		VECTOR_ref		AI::GetNowWaypointPos() {
			auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);
			auto& C = MainGB->GetBuildDatas().at(this->m_WayPoints[0]);
			return C.GetMatrix().pos();
		}

		int AI::GetNextWaypoint(const VECTOR_ref& Dir) {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(m_MyCharaID).GetChara();
			auto& MainGB = (std::shared_ptr<BackGroundClassMain>&)(this->m_BackGround);

			int now = -1;
			float Dist = 100.f*Scale_Rate;

			bool CheckDir = (Dir.Length() > 0.f);
			float CheckDirPer = -1.f;
			for (auto& C : MainGB->GetBuildDatas()) {
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
							auto Endpos = StartPos + Vec_XZ * (5.f*Scale_Rate);
							auto ret = C.GetColLine(StartPos, Endpos);
							if (ret.HitFlag == FALSE) {
								if (CheckDirPer < Dot) {
									CheckDirPer = Dot;
									now = index;
								}
							}
						}
					}
					else {
						if (Dist > Len) {
							Dist = Len;
							now = index;
						}
					}
				}
			}
			return now;
		}
		//
		void AIControl::Init(const std::shared_ptr<BackGroundClassBase>& BackBround_t, PlayerID MyCharaID) noexcept {
			this->m_AI.m_MyCharaID = MyCharaID;
			this->m_AI.SetBackGround(BackBround_t);
			this->m_AI.Init();
		}
		void AIControl::Execute(InputControl* MyInput) noexcept {
			auto* PlayerMngr = PlayerManager::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(m_AI.m_MyCharaID).GetChara();

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
			//AI
			if (MyChara->IsAlive()) {
				auto Dir = MyChara->GetMove().mat.zvec()*-1.f;
				auto Vec = this->m_AI.GetNowWaypointPos() - MyChara->GetMove().pos;


				auto Dir_XZ = Dir; Dir_XZ.y(0.f);
				auto Vec_XZ = Vec; Vec_XZ.y(0.f);
				auto cross = Dir_XZ.Norm().cross(Vec_XZ.Norm()).y();
				auto dot = Dir_XZ.Norm().dot(Vec_XZ.Norm());

				if (dot > 0.f) {
					if (abs(cross) < sin(deg2rad(40.f))) {
						W_key = true;

						if (abs(cross) < sin(deg2rad(10.f))) {
							Run_key = true;
						}
					}
				}
				else {
					if (abs(cross) < sin(deg2rad(60.f))) {
						//S_key = true;
					}
					//S_key = true;
				}

				A_key = GetRand(100) > 50;
				D_key = GetRand(100) > 50;

				if (dot > 0.f) {
					y_m = (int32_t)(6.f*cross);
				}
				else {
					y_m = (int32_t)(-10.f);
				}

				if (Vec_XZ.Length() < 1.5f*Scale_Rate) {
					auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(0).GetChara();
					{
						auto Dir_t = TargetChara->GetMove().pos - MyChara->GetMove().pos;
						auto Len = Dir_t.Length();
						Dir_t = Dir_t.Norm();
						if (Len < 10.f*Scale_Rate) {
							Dir = Dir_t;
						}
					}
					int now = this->m_AI.GetNextWaypoint(Dir);
					if (now != -1) {
						this->m_AI.PushFrontWayPoint(now);
					}
					else {
						this->m_AI.Set_wayp_return();
					}
				}
			}
			if (MyChara->GetIsSquat()) {
				C_key = GetRand(100) < 1;
			}
			//shotMain_Key = GetRand(100) < 50;
			/*
			//y_m = 0;
			W_key = false;
			S_key = false;
			A_key = false;
			D_key = false;
			Q_key = false;
			E_key = false;
			shotMain_Key = false;
			//*/
			MyInput->SetInput(
				(float)x_m / 100.f,
				(float)y_m / 100.f,
				W_key, S_key, A_key, D_key,
				Run_key,
				Q_key, E_key,//QE
				false, false, false, false,
				false,//1
				R_key,
				F_key,
				C_key,
				shotMain_Key,
				false
			);
		}
		void AIControl::Draw() noexcept {
#ifdef DEBUG
			if (this->m_AI.m_Phase != ENUM_AI_PHASE::Normal) {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_AI.m_MyCharaID).GetChara();
				auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_AI.m_TargetCharaID).GetChara();
				VECTOR_ref StartPos = MyChara->GetEyePosition() + VECTOR_ref::vget(0.f, 5.f*Scale_Rate, 0.f);
				VECTOR_ref EndPos = TargetChara->GetMove().pos + VECTOR_ref::vget(0.f, 5.f*Scale_Rate, 0.f);
				DrawCapsule_3D(StartPos, EndPos, 0.1f*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
			}
#endif
		}
	};
};
