#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MorphineClass :
			public ObjectBaseClass {
		private:
		public:
			MorphineClass(void) noexcept { this->m_objType = (int)ObjType::Morphine; }
			~MorphineClass(void) noexcept {}
		public:
			void			FirstExecute(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override;
		};
	};
};
