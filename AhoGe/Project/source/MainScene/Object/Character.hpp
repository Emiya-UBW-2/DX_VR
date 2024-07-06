#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include "../../CommonScene/Object/Base2DObject.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterObject : public Base2DObject {
			Vector3DX m_InputVec{};
			float m_MoveSpeed{1.f};
			float m_Alpha{1.f};

			PlayerID m_PlayerID{0};
		public:
			CharacterObject();
			~CharacterObject();
		public:
			void		ExecuteInput(InputControl* MyInput) noexcept;

			void		SetPlayerID(PlayerID value) noexcept { this->m_PlayerID = value; }
		public:
			void Execute_Sub() noexcept override;
			void Draw_Sub() noexcept override;
			void Dispose_Sub() noexcept override;
		};
	};
};
