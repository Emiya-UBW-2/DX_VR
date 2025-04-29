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
			MyChara->Spawn(deg2rad(0.0f), std::atan2(ZVec.x, ZVec.z) + deg2rad(GetRandf(10.0f)), Pos, 2, false);
			MyChara->SetRappelling();
			this->m_RapeTimer = 0.0f;
		}
		//
		void AIControl::Init(PlayerID MyID) noexcept {
			this->m_MyCharaID = MyID;
		}
		const InputControl AIControl::Update(void) noexcept {
			InputControl Input;
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& MyChara = PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
			//auto& TargetChara = PlayerMngr->GetPlayer(this->m_TargetCharaID)->GetChara();
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
			return Input;
		}
	}
}
