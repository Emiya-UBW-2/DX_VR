#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClassBase : public SingletonBase<BackGroundClassBase> {
		private:
			friend class SingletonBase<BackGroundClassBase>;
		private:
			struct Blick {
				Vector3DX m_Pos;
				bool IsWall{false};

				bool HIT[4]{};
			public:
				Blick(int x, int y) noexcept {
					m_Pos.x = (float)x;
					m_Pos.y = (float)y;
				}
			};
		private:
			std::vector<std::vector<std::shared_ptr<Blick>>> m_Blick;
		public://
			const bool		CheckLinetoMap(const Vector3DX& StartPos, Vector3DX* EndPos, float Radius, Vector3DX* Normal = nullptr) const noexcept;
		public://
			//
			void			Init(const char* MapPath) noexcept;
			//
			void			Execute(void) noexcept;
			//
			void			BG_Draw(void) noexcept;
			void			Draw(void) noexcept;
			void			DrawFront(void) noexcept;
			//
			void			Dispose(void) noexcept;
		};
	};
};
