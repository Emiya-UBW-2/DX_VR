#include	"CPU.hpp"
#include "../../MainScene/NetWork.hpp"
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"
#include "../../Scene/MainScene.hpp"

#define NOTUSEPATHCHECKER (FALSE)

namespace FPS_n2 {
	namespace Sceneclass {
		void		AIControl::Repop() {
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
			auto& TargetChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_TargetCharaID)->GetChara();
			auto TgtPos = TargetChara->GetFrameWorldMat(CharaFrame::Upper2).pos();

			Vector3DX pos_t;
			pos_t.Set(GetRandf(10.f), -20.f, GetRandf(10.f));
			pos_t *= Scale3DRate;

			Vector3DX EndPos = pos_t - Vector3DX::up() * 200.f * Scale3DRate;
			if (BackGround->CheckLinetoMap(pos_t + Vector3DX::up() * 0.f * Scale3DRate, &EndPos)) {
				pos_t = EndPos;
			}

			MyChara->MovePoint(deg2rad(0.f), deg2rad(GetRandf(180.f)), pos_t, 0);
			MyChara->Heal(100, true);
		}
		//
		AIControl::AIControl() {
		}
		AIControl::~AIControl() {
		}
		//
		void AIControl::Init(PlayerID MyID) noexcept {
			m_MyCharaID = MyID;
		}
		void AIControl::Execute(InputControl*) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_MyCharaID)->GetChara();
			if (MyChara->IsAlive()) {
				m_RepopTimer = 0.f;
			}
			else {
				m_RepopTimer += DXLib_refParts->GetDeltaTime();
				if (m_RepopTimer > 5.f) {
					Repop();
				}
			}
		}
	};
};
