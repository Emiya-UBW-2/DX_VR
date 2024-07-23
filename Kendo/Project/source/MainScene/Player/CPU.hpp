#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//AI—p
		class AIControl {
			PlayerID m_MyID{ 0 };
		public:
			void Init(PlayerID MyID) noexcept {
				m_MyID = MyID;
			}
			void Execute(InputControl* MyInput) noexcept;
			void Draw(void) noexcept {
			}
			void Dispose(void) noexcept {
			}
		};
	};
};
