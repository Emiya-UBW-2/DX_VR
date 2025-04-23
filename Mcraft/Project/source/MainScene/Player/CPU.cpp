#include	"CPU.hpp"
#include	"../../MainScene/NetWork.hpp"
#include	"../../MainScene/Object/Character.hpp"
#include	"../../MainScene/Player/Player.hpp"
#include	"../../Scene/MainScene.hpp"

namespace FPS_n2 {
	namespace AIs {
		void		AIControl::Repop(void) noexcept {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
			//auto& TargetChara = PlayerMngr->GetPlayer(this->m_TargetCharaID)->GetChara();

			Vector3DX ZVec = PlayerMngr->GetHelicopter()->GetMove().GetMat().xvec();
			float rot = std::atan2(ZVec.x, ZVec.z);
			MyChara->Spawn(deg2rad(0.f), rot + deg2rad(GetRandf(10.f)),
				PlayerMngr->GetHelicopter()->GetObj().GetFrameLocalWorldMatrix(PlayerMngr->GetHelicopter()->GetFrame(static_cast<int>(Objects::HeliFrame::Rappelling1))).pos(),
				0, false);
			MyChara->SetRappelling();
		}
		//
		void AIControl::Init(PlayerID MyID) noexcept {
			this->m_MyCharaID = MyID;
		}
		void AIControl::Update(InputControl*) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
			//auto& TargetChara = PlayerMngr->GetPlayer(this->m_TargetCharaID)->GetChara();
			if (MyChara->IsAlive()) {
				this->m_RepopTimer = 0.f;

				if (MyChara->GetIsRappelling()) {
					Vector3DX Vec = Vector3DX::down() * (10.f * 60.f * DXLib_refParts->GetDeltaTime());
					NetWork::MoveInfo MoveInfoData;
					MoveInfoData.repos = MyChara->GetMove().GetRePos();
					MoveInfoData.pos = MyChara->GetMove().GetPos() + Vec;
					MoveInfoData.vec = Vec;
					MoveInfoData.mat = MyChara->GetMove().GetMat();
					MoveInfoData.WatchRad = MyChara->GetRotateRad();
					MyChara->SetMoveOverRide(MoveInfoData);
				}
			}
			else {
				this->m_RepopTimer += DXLib_refParts->GetDeltaTime();
				if (this->m_RepopTimer > 5.f) {
					this->m_RepopTimer -= 5.f;
					if (PlayerMngr->GetHelicopter()->GetIsActiveRappelling()) {
						Repop();
					}
				}
			}
		}
	};
};
