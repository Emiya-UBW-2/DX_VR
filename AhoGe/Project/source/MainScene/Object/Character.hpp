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

			struct BlurParts {
				Vector3DX	Pos;
				float		Time{};
				float		TimeMax{};
			public:
				const auto IsActive() const noexcept { return Time > 0.f; }
				const auto GetPer() const noexcept { return Time / TimeMax; }
			};
			std::array<BlurParts, 60>	m_Blur{};
			int							m_BlurNow{0};
		private:
			void AddBlur(float Blur) noexcept {
				m_Blur.at(m_BlurNow).Pos = GetPos();
				m_Blur.at(m_BlurNow).TimeMax = Blur;
				m_Blur.at(m_BlurNow).Time = m_Blur.at(m_BlurNow).TimeMax;
				++m_BlurNow %= (int)m_Blur.size();
			}
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
			const auto GetSpeedPer() const noexcept { return m_Speed / m_SpeedLimit; }
		public:
			void Init_Sub() noexcept override;
			void Execute_Sub() noexcept override;
			void Draw_Sub() noexcept override;
			void Dispose_Sub() noexcept override;
		};
	};
};
