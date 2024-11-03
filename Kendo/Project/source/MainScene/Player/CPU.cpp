#include	"CPU.hpp"
#pragma warning(disable:4464)
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Player {
		void AIControl::Execute(InputControl* MyInput, bool IsTutorial) noexcept
		{
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(m_MyID)->GetChara();

			bool W_key{ false };
			bool A_key{ false };
			bool S_key{ false };
			bool D_key{ false };

			bool IsOutArea = false;
			{
				Vector3DX Vec = Chara->GetMove().GetPos() - Vector3DX::zero();
				float Len = 11.f / 2.f * Scale_Rate;
				if ((Vec.x < -Len || Len < Vec.x) ||
					(Vec.z < -Len || Len < Vec.z)) {
					IsOutArea = true;
				}
			}

			if (IsOutArea) {
				W_key = true;
			}
			if (IsTutorial) {

			}

			bool shotMain_Key{ false };
			bool shotSub_Key{ false };
			float pp_x{ 0.f }, pp_y{ 0.f };

			m_Counter += DrawParts->GetDeltaTime();
			if (m_Counter > 5.f) {
				m_Counter -= 5.f;
			}
			if (m_Counter > 3.f) {
				//shotMain_Key = true;
			}

			if (Chara->GetBambooVec().magnitude() > deg2rad(1)) {
				pp_x = std::clamp(-100.f * Chara->GetBambooVec().x / deg2rad(150.f), -3.f, 3.f);
				pp_y = std::clamp(-100.f * Chara->GetBambooVec().y / deg2rad(150.f), -3.f, 3.f);
			}
#ifdef DEBUG
			//printfDx("[%5.2f,%5.2f]\n", pp_x, pp_y);
#endif
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
