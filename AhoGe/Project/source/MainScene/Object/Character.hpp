#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"
#include "../../CommonScene/Object/Base2DObject.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterObject : public Base2DObject {
			Vector3DX m_InputVec{};

		public:
			CharacterObject();
			~CharacterObject();
		public:
			void		ExecuteInput(InputControl* MyInput) noexcept;
		public:
			void Execute_Sub() noexcept override;
			void Draw_Sub() noexcept override;
			void Dispose_Sub() noexcept override;
		};
	};
};
