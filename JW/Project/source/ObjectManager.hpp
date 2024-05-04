#pragma once
#include	"Header.hpp"
#include "MainScene/Object/ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		typedef std::shared_ptr<ObjectBaseClass> SharedObj;

		class ObjectManager : public SingletonBase<ObjectManager> {
		private:
			friend class SingletonBase<ObjectManager>;
		private:
			std::vector<SharedObj>	m_Object;
			switchs					m_ResetP;
		private:
			ObjectManager() {
				m_Object.reserve(256);
			}
			~ObjectManager() {
				DeleteAll();
				m_Object.shrink_to_fit();
			}
		public:
			void			AddObject(const SharedObj& NewObj) noexcept;
			void			LoadModel(const SharedObj& pObj, const SharedObj& pAnim, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") const noexcept;
			SharedObj*		GetObj(ObjType ModelType, int num) noexcept;
			void			DelObj(SharedObj* ptr) noexcept;
			void			DelObj(ObjType ModelType, int num) noexcept { DelObj(GetObj(ModelType, num)); }
		public:
			void			ExecuteObject(void) noexcept;
			void			LateExecuteObject(void) noexcept;

			void			Draw_Depth(void) noexcept;
			void			Draw() noexcept;
			void			Draw_Shadow(void) noexcept;

			void			DeleteAll(void) noexcept;
		};
	};
};
