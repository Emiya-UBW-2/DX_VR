#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include	"../../CommonScene/Object/Base2DObject.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MetalObject : public Base2DObject {
			const float					m_SpeedLimit = 128.f;
		private:
			PlayerID					m_ShotPlayerID{ 0 };
			PlayerID					m_HitPlayerID{ 0 };
			float						m_Speed{ 1.f };
			float						m_Rad{ 0.f };
			float						m_Rad_R{0.f};
			float						m_RadL{0.f};
			float						m_RadR{0.f};

			float						m_Foot{0.f};
			bool						m_FootDown{false};
			Vector3DX					m_BasePos{};
			Vector3DX					m_PosL{};
			Vector3DX					m_PosR{};

			float						m_NormalSpeedLimit{1.f};
			Vector3DX					m_InputVec{};

			float						m_ShotCoolTime{0.f};

			std::array<int, 8>			m_MissileID{};
			int							m_MissileIDNum{};

			bool						m_CanMove{false};

			HitPoint					m_HitPoint{0};
		public:
			MetalObject(void) noexcept;
			MetalObject(const MetalObject&) = delete;
			MetalObject(MetalObject&& o) = delete;
			MetalObject& operator=(const MetalObject&) = delete;
			MetalObject& operator=(MetalObject&& o) = delete;

			virtual ~MetalObject(void) noexcept;
		public:
			void		SetPlayerID(PlayerID value) noexcept { this->m_ShotPlayerID = value; }
			void		SetCanMove(bool value) noexcept { this->m_CanMove = value; }
			
			const auto& GetHitPoint(void) const noexcept { return this->m_HitPoint; }
			auto GetHitPointMax(void) const noexcept { return 1000; }
		public:
			void Execute_OnHitObject(void) noexcept override;
			void Execute_OnHitWall(void) noexcept override;
		public:
			void Init_Sub(void) noexcept override;
			void Execute_Sub(void) noexcept override;
			void DrawShadow_Sub(void) noexcept override;
			void Draw_Sub(void) noexcept override;
			void Dispose_Sub(void) noexcept override;
		};
	};
};
