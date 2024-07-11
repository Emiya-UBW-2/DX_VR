#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include	"../../CommonScene/Object/Base2DObject.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterObject : public Base2DObject {
			const float					m_SpeedLimit = 128.f;
		private:
			PlayerID					m_PlayerID{0};
			Vector3DX					m_InputVec{};

			float						m_NormalSpeedLimit{1.f};
			float						m_Speed{1.f};

			float						m_Alpha{1.f};

			float						m_Rad{0.f};
			float						m_Rad_R{0.f};
			float						m_DodgeCoolTime{ 0.f };

			float						m_ShotCoolTime{ 0.f };
			float						m_GraphTimer{ 0.f };

			float						m_RunFootPer{0.f};
		public:
			CharacterObject(void) noexcept;
			~CharacterObject(void) noexcept;
		public:
			void		ExecuteInput(InputControl* MyInput) noexcept ;
			void		SetPlayerID(PlayerID value) noexcept { this->m_PlayerID = value; }
			void		SetViewRad(float value) noexcept { this->m_Rad = value; }
			void		SetGraphTimer(float value) noexcept { this->m_GraphTimer = value; }
		public:
			const auto CanLookPlayer0(void) const noexcept { return m_Alpha > 0.5f; }
			const auto& GetBodyRad(void) const noexcept { return m_Rad; }
			const auto& GetViewRad(void) const noexcept { return m_Rad_R; }
			const auto GetSpeedPer(void) const noexcept { return m_Speed / m_SpeedLimit; }
			const auto& GetGraphTimer(void) const noexcept { return m_GraphTimer; }
		public:
			void Init_Sub(void) noexcept override;
			void Execute_Sub(void) noexcept override;
			void DrawShadow_Sub(void) noexcept override;
			void Draw_Sub(void) noexcept override;
			void Dispose_Sub(void) noexcept override;
		};
	};
};
