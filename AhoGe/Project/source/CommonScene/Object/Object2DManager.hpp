#pragma once
#include	"../../Header.hpp"
#include	"../../sub.hpp"

#include	"Base2DObject.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		typedef std::shared_ptr<Base2DObject> SharedObj2D;

		class Object2DManager : public SingletonBase<Object2DManager> {
		private:
			friend class SingletonBase<Object2DManager>;
		private:
			std::vector<SharedObj2D>		m_Object;
			int								m_LastUniqueID{ 0 };
		private:
			Object2DManager(void) noexcept {
				m_Object.reserve(256);
			}
			~Object2DManager(void) noexcept {
				DeleteAll();
				m_Object.shrink_to_fit();
			}
		public:
			void			AddObject(const SharedObj2D& NewObj) noexcept ;
			SharedObj2D* 	GetObj(int ModelType, int num) noexcept ;
			void			DelObj(const SharedObj2D& ptr) noexcept ;

		public:
			void CheckColObject(Base2DObject* ptr) noexcept ;

		public:
			void			ExecuteObject(void) noexcept ;
			void			DrawShadow(void) noexcept ;
			void			Draw(void) noexcept ;
			void			DeleteAll(void) noexcept ;
		};
	};
};