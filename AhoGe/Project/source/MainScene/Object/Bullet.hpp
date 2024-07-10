#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include	"../../CommonScene/Object/Base2DObject.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BulletObject : public Base2DObject {
			const float					m_SpeedLimit = 128.f;
		private:
			PlayerID					m_ShotPlayerID{ 0 };
			PlayerID					m_HitPlayerID{ 0 };
			float						m_Speed{ 1.f };
			float						m_Rad{ 0.f };
		public:
			BulletObject(void) noexcept;
			~BulletObject(void) noexcept;
		public:
			void		SetPlayerID(PlayerID value) noexcept { this->m_ShotPlayerID = value; }
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
