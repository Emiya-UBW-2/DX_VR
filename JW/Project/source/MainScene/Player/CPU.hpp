#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../sub.hpp"
#include "../../ObjectManager.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ENUM_AI_PHASE {
			Normal,
			Shot,
			Dead,
		};

		class AIControl {
		private:
			class Impl;
			Impl *m_Param;
			Impl* GetParam() noexcept { return m_Param; }
		public:
			AIControl();
			~AIControl();
		public:
			void Init(const std::shared_ptr<BackGroundClassMain>& BackBround_t, PlayerID MyCharaID) noexcept;
			void Execute(InputControl* MyInput) noexcept;
			void Draw() noexcept {}
			void Dispose() noexcept {}
		};
	};
};
