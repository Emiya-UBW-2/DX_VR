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

			float									m_RepopTimer{ 0.0f };

			bool									m_IsLeftHeli{};
			float									m_RapeTimer{ 0.0f };
		private:
			class Impl;
			Impl* m_Param;
			Impl* GetParam() noexcept { return m_Param; }
		public:
			void Repop(void) noexcept;
		public:
			AIControl(PlayerID MyID) noexcept;
			virtual ~AIControl(void) noexcept;
		private:
			void Init(PlayerID MyID) noexcept;
			void Dispose(void) noexcept {}
		public:
			const InputControl Update(void) noexcept;
			void Draw(void) noexcept;
		};
	}
}
