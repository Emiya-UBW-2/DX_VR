#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace AIs {
		enum class ENUM_AI_PHASE {
			Normal,
			Shot,
			Dead,
		};

		class AIControl {
			PlayerID								m_MyCharaID{ 0 };
			PlayerID								m_TargetCharaID{ 0 };

			float									m_RepopTimer{ 0.f };

			bool									m_IsLeftHeli{};
			float									m_RapeTimer{ 0.f };
		public:
			void Repop(void) noexcept;
		public:
			AIControl(PlayerID MyID) noexcept {
				Init(MyID);
			}
			virtual ~AIControl(void) noexcept {
				Dispose();
			}
		private:
			void Init(PlayerID MyID) noexcept;
			void Dispose(void) noexcept {}
		public:
			void Update(InputControl*) noexcept;
			void Draw(void) noexcept {}
		};
	};
};
