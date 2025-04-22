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

			MyChara->Spawn(deg2rad(0.f), deg2rad(GetRandf(180.f)), Vector3DX::vget(GetRandf(10.f), -20.f, GetRandf(10.f)) * Scale3DRate, 0);
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
			}
			else {
				this->m_RepopTimer += DXLib_refParts->GetDeltaTime();
				if (this->m_RepopTimer > 5.f) {
					Repop();
				}
			}
		}
	};
};
