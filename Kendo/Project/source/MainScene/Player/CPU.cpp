#include	"CPU.hpp"
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void AIControl::Execute(InputControl* MyInput) noexcept
		{
			bool W_key{ false };
			bool A_key{ false };
			bool S_key{ false };
			bool D_key{ false };

			bool shotMain_Key{ false };
			bool shotSub_Key{ false };
			float pp_x{ 0.f }, pp_y{ 0.f };
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(m_MyID)->GetChara();

			if (Chara->GetBambooVec().magnitude() > deg2rad(1)) {
				pp_x = std::clamp(-100.f * Chara->GetBambooVec().x / deg2rad(60.f), -3.f, 3.f);
				pp_y = std::clamp(-100.f * Chara->GetBambooVec().y / deg2rad(60.f), -3.f, 3.f);
			}
#ifdef DEBUG
			printfDx("[%5.2f,%5.2f]\n", pp_x, pp_y);
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
	};
};
