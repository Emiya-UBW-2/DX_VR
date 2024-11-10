#pragma once
#pragma warning(disable:4464)
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Player {
		//AI用
		class AIControl {
			PlayerID m_MyID{ 0 };
			//チュートリアル用
			float m_Counter01{ 0 };
			//
			float m_Counter11{ 0 };
			float m_Counter12{ 0 };
			float m_Counter13{ 0 };
			float m_Counter14{ 0 };
			int m_LMR{ 50 };
			int m_FMB{ 50 };
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
