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
			const Impl* GetParam() const noexcept { return m_Param; }
		public:
			CharacterObject();
			~CharacterObject();
		public:
			const Vector3DX& GetPos() const noexcept;
		public:
			void Init(PlayerID MyCharaID) noexcept;
			void ExecuteInput(InputControl* MyInput) noexcept;
			void Execute() noexcept;
			void Draw() noexcept;
			void Dispose() noexcept;
		};
	};
};
