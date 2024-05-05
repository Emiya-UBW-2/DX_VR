#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"
#include "ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ArmerClass :
			public ObjectBaseClass {
		private:
		public:
		public:
		public://ƒQƒbƒ^[
			const auto GetLeftHandMat() const noexcept { return this->GetFrameWorldMat(2); }
			const auto GetRightHandMat() const noexcept { return this->GetFrameWorldMat(3); }
		public:
			ArmerClass(void) noexcept { this->m_objType = ObjType::Armer; }
			~ArmerClass(void) noexcept {}
		public:
			void			Init_Sub(void) noexcept override;
			void			FirstExecute(void) noexcept override;
			void			Draw(bool isDrawSemiTrans) noexcept override;
			void			Dispose_Sub(void) noexcept override;
		};
	};
};
