#pragma once
#include	"../../Header.hpp"

#include "../../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ArmerClass :
			public ObjectBaseClass {
		private:
		public:
		public:
		public://�Q�b�^�[
			const auto GetLeftHandMat() const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix(2); }
			const auto GetRightHandMat() const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix(3); }
		public:
			ArmerClass(void) noexcept { this->m_objType = (int)ObjType::Armer; }
			~ArmerClass(void) noexcept {}
		public:
			void			FirstExecute(void) noexcept override{}
			void			Draw(bool isDrawSemiTrans) noexcept override;
		};
	};
};
