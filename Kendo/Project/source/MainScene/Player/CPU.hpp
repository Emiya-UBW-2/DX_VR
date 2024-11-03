#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Player {
		//AI—p
		class AIControl {
			PlayerID m_MyID{ 0 };

			float m_Counter{ 0 };
		public:
			void Init(PlayerID MyID) noexcept {
				m_MyID = MyID;
			}
			void Execute(InputControl* MyInput, bool IsTutorial) noexcept;
			void Draw(void) noexcept {
			}
			void Dispose(void) noexcept {
			}
		};
	}
}
