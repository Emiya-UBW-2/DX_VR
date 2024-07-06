#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include "../../CommonScene/Object/Base2DObject.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterObject : public Base2DObject {
			PlayerID m_PlayerID{0};
			Vector3DX m_InputVec{};
			float m_MoveSpeed{1.f};
			float m_Alpha{1.f};
			float m_Rad{0.f};
			float m_DodgeCoolTime{0.f};
			float m_Rad_R{0.f};
		public:
			CharacterObject();
			~CharacterObject();
		public:
			void		ExecuteInput(InputControl* MyInput) noexcept;
			void		SetPlayerID(PlayerID value) noexcept { this->m_PlayerID = value; }
			void		SetViewRad(float value) noexcept { this->m_Rad = value; }
		public:
			const auto CanLookPlayer0() const noexcept { return m_Alpha > 0.5f; }
			const auto& GetBodyRad() const noexcept { return m_Rad; }
			const auto& GetViewRad() const noexcept { return m_Rad_R; }
		public:
			void Init_Sub() noexcept override;
			void Execute_Sub() noexcept override;
			void Draw_Sub() noexcept override;
			void Dispose_Sub() noexcept override;
		};
	};
};
