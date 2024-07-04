#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

#include "Base2DObject.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class Base2DObject;

		typedef std::shared_ptr<Base2DObject> SharedObj2D;

		class Object2DManager : public SingletonBase<Object2DManager> {
		private:
			friend class SingletonBase<Object2DManager>;
		private:
			std::vector<SharedObj2D>		m_Object;
		private:
			Object2DManager() {
				m_Object.reserve(256);
			}
			~Object2DManager() {
				DeleteAll();
				m_Object.shrink_to_fit();
			}
		public:
			void			AddObject(const SharedObj2D& NewObj) noexcept;
			SharedObj2D*	GetObj(int ModelType, int num) noexcept;
			void			DelObj(const SharedObj2D& ptr) noexcept;
		public:
			void			ExecuteObject(void) noexcept;
			void			Draw() noexcept;
			void			DeleteAll(void) noexcept;
		};
	};
};