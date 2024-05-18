#pragma once
#include	"../../Header.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//AI—p
		class AIControl {
		public:
			//AI‘€ì
			void SetNextWaypoint(const Vector3DX&) {
			}
			void AI_move(InputControl* MyInput) noexcept {
				bool W_key{ false };
				bool A_key{ false };
				bool S_key{ false };
				bool D_key{ false };

				bool shotMain_Key{ false };
				bool shotSub_Key{ false };
				int32_t x_m{ 0 }, y_m{ 0 };
				//AI
				MyInput->SetInputStart((float)x_m / 100.f, (float)y_m / 100.f);
				MyInput->SetInputPADS(PADS::MOVE_W, W_key);
				MyInput->SetInputPADS(PADS::MOVE_S, S_key);
				MyInput->SetInputPADS(PADS::MOVE_A, A_key);
				MyInput->SetInputPADS(PADS::MOVE_D, D_key);
				MyInput->SetInputPADS(PADS::SHOT, shotMain_Key);
				MyInput->SetInputPADS(PADS::JUMP, shotSub_Key);
			}
		public:
			void Init(std::shared_ptr<BackGroundClass>&, PlayerID) noexcept {
			}
			void Execute() noexcept {
			}
			void Draw() noexcept {
			}
		};
	};
};
