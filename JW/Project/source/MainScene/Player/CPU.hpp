#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../sub.hpp"
#include "../../ObjectManager.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ENUM_AI_PHASE {
			Normal,
		};
		//AI—p
		class AI {
		public:
		public:
			std::shared_ptr<BackGroundClassBase>	m_BackGround;				//BG
			PlayerID								m_MyCharaID{ 0 };
			ENUM_AI_PHASE							m_Phase{ ENUM_AI_PHASE::Normal };
			std::array<int, 6>						m_WayPoints;
			PlayerID								m_TargetCharaID{ 0 };
		public:
			AI(void) noexcept { }
			~AI(void) noexcept { }
		public:
			void SetBackGround(const std::shared_ptr<BackGroundClassBase>& BackBround_t) { m_BackGround = BackBround_t; }

			void FillWayPoints(int now) {
				for (auto& w : this->m_WayPoints) { w = now; }
			}
			void PushFrontWayPoint(int now) {
				for (size_t i = (this->m_WayPoints.size() - 1); i >= 1; i--) { this->m_WayPoints[i] = this->m_WayPoints[i - 1]; }
				this->m_WayPoints[0] = now;
			}
		public:
			VECTOR_ref		GetNowWaypointPos();
			int		GetNextWaypoint(const VECTOR_ref&);
			void	SetNextWaypoint(const VECTOR_ref& vec_z) {
				int now = this->GetNextWaypoint(vec_z);
				if (now != -1) {
					this->PushFrontWayPoint(now);
				}
			}

			void Set_wayp_return(void) noexcept {
				auto ppp = this->m_WayPoints[1];
				this->FillWayPoints(this->m_WayPoints[0]);
				this->m_WayPoints[0] = ppp;
			}
		public:
			void Init() {
				this->m_Phase = ENUM_AI_PHASE::Normal;
				int now = GetNextWaypoint(VECTOR_ref::zero());
				this->FillWayPoints((now != -1) ? now : 0);
			}
		};

		class AIControl {
			const float										SearchTotal{ 1.f };
		private:
			AI												m_AI;						//AI—p
		private:
		public:
			void Init(const std::shared_ptr<BackGroundClassBase>& BackBround_t, PlayerID MyCharaID) noexcept;
			void Execute(InputControl* MyInput) noexcept;
			void Draw() noexcept;
			void Dispose() noexcept {}
		};
	};
};
