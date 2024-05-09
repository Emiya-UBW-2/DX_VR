#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class HindDClass : public ObjectBaseClass {
		private:
		public://ƒQƒbƒ^[
		public:
			HindDClass(void) noexcept { this->m_objType = (int)ObjType::HindD; }
			~HindDClass(void) noexcept { }
		public:
			void			FirstExecute(void) noexcept override {}
		};
	};
};
