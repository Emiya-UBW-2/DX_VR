#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ENUM_AI_PHASE {
			Normal,
			Shot,
			Dead,
		};

		class AIControl {
			PlayerID								m_MyCharaID{ 0 };
			PlayerID								m_TargetCharaID{ 0 };

			float									m_RepopTimer{ 0.f };
		public:
			void Repop();
		public:
			AIControl();
			~AIControl();
		public:
			void Init(PlayerID MyID) noexcept;
			void Execute(InputControl*) noexcept;
			void Draw() noexcept {}
			void Dispose() noexcept {}
		};
	};
};
