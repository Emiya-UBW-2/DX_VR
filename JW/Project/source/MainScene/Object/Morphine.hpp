#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MorphineClass :
			public ObjectBaseClass {
		private:
		public:
			MorphineClass(void) noexcept { this->m_objType = ObjType::Morphine; }
			~MorphineClass(void) noexcept {}
		public:
			void			Init(void) noexcept override;
			void			FirstExecute(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			Dispose(void) noexcept override;
		};
	};
};
