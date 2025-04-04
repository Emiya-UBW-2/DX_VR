#include	"CPU.hpp"
#pragma warning(disable:4464)
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Player {
		void AIControl::Execute(InputControl* MyInput) noexcept
		{
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& MyPlayer = (m_MyID >= PlayerMngr->GetPlayerNum()) ? PlayerMngr->GetNPC(m_MyID - PlayerMngr->GetPlayerNum()) : PlayerMngr->GetPlayer(m_MyID);
			auto& Chara = (std::shared_ptr<CharacterObject::CharacterClass>&)MyPlayer->GetChara();
			auto& Target = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(0)->GetChara();

			bool W_key{ false };
			bool A_key{ false };
			bool S_key{ false };
			bool D_key{ false };

			bool shotMain_Key{ false };
			bool shotSub_Key{ false };
			float pp_x{ 0.f }, pp_y{ 0.f };

			m_Counter += 1.f / DrawParts->GetFps();
			if (m_Counter > 3.f) {
				m_Counter -= 3.f;
			}
			if (m_Counter > 2.f) {
				shotMain_Key = true;
			}
			Vector3DX MyVec = Chara->GetMove().GetMat().zvec();
			Vector3DX TgtVec = (Target->GetMove().GetPos() - Chara->GetMove().GetPos()).normalized();

			Vector3DX Vec = MyVec; Vec.y = 0.f;
			Vector3DX vec_a = TgtVec; vec_a.y = 0.f;
			if (Vector3DX::Cross(vec_a, Vec).y > 0.f) {
				if (m_Counter > 1.f) {
					D_key = true;
				}
			}
			else {
				if (m_Counter > 1.f) {
					A_key = true;
				}
			}

			if (Chara->GetBambooVec().magnitude() > deg2rad(1)) {
				//pp_x = std::clamp(-100.f * Chara->GetBambooVec().x / deg2rad(150.f), -3.f, 3.f);
			}
			//AI
			MyInput->SetInputStart(pp_x, pp_y);
			MyInput->SetInputPADS(PADS::MOVE_W, W_key);
			MyInput->SetInputPADS(PADS::MOVE_S, S_key);
			MyInput->SetInputPADS(PADS::MOVE_A, A_key);
			MyInput->SetInputPADS(PADS::MOVE_D, D_key);
			MyInput->SetInputPADS(PADS::SHOT, shotMain_Key);
			MyInput->SetInputPADS(PADS::JUMP, shotSub_Key);
		}
	}
};
