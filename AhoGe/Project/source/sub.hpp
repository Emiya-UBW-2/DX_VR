#pragma once
#include	"Header.hpp"

//
namespace FPS_n2 {
	namespace Sceneclass {
		class CommonBattleResource : public SingletonBase<CommonBattleResource> {
		private:
			friend class SingletonBase<CommonBattleResource>;
		private:
		public:
			void			Load(void) noexcept;
			void			Set(void) noexcept;
			void			Dispose(void) noexcept;
		public:
		};

		class Cam2DControl : public SingletonBase<Cam2DControl> {
		private:
			friend class SingletonBase<Cam2DControl>;
		private:
			Vector3DX m_Pos{};
		public:
			const auto& GetCamPos() const noexcept { return m_Pos; }

			void			SetCamPos(const Vector3DX& Pos) noexcept{
				m_Pos = Pos;
			}
			void			SetCamAim(const Vector3DX& Pos) noexcept {
				float z = m_Pos.z;
				Easing(&m_Pos, Pos, 0.9f, EasingType::OutExpo);
				m_Pos.z = z;
			}
		public:
			void			Set(void) noexcept;
			void			Update(void) noexcept;
			void			Dispose(void) noexcept;
		public:
		};

		extern const Vector3DX Convert2DtoDisp(const Vector3DX& Pos2D);
	};
};
