#pragma once
#include "../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class HoukiClass : public ObjectBaseClass {
		public:
			HoukiClass(void) noexcept { this->m_objType = ObjType::Houki; }
			~HoukiClass(void) noexcept {}
		public:
			void Init(void) noexcept override {
			}
			void Execute(void) noexcept override {
			}
		};
	};
};
