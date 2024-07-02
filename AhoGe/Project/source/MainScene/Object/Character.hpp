#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterObject {
		private:
			class Impl;
			Impl *m_Param;
			Impl* GetParam() noexcept { return m_Param; }
		public:
			CharacterObject();
			~CharacterObject();
		public:
			void Init(PlayerID MyCharaID) noexcept;
			void Execute() noexcept;
			void Draw() noexcept;
			void Dispose() noexcept;
		};
	};
};
