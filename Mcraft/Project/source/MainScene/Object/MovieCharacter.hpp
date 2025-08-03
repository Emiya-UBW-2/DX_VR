#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Charas {

		class MovieCharacter : public BaseObject {
		public: //コンストラクタ、デストラクタ
			MovieCharacter(void) noexcept { this->m_objType = static_cast<int>(ObjType::MovieObj); }
			virtual ~MovieCharacter(void) noexcept {}
		private:
			//int				GetFrameNum(void) noexcept override { return static_cast<int>(CharaFrame::Max); }
			//const char*		GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }

			//int				GetShapeNum(void) noexcept override { return static_cast<int>(CharaShape::Max); }
			//const char*		GetShapeStr(int id) noexcept override { return CharaShapeName[id]; }
		private: //継承
			void			Init_Sub(void) noexcept override;
			void			FirstUpdate(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
		};

		enum class MovieHeliFrame {
			Center,
			Max,
		};
		static const char* MovieHeliName[static_cast<int>(MovieHeliFrame::Max)] = {
			"センター",
		};

		class MovieObject : public BaseObject {
		public: //コンストラクタ、デストラクタ
			MovieObject(void) noexcept { this->m_objType = static_cast<int>(ObjType::MovieObj); }
			virtual ~MovieObject(void) noexcept {}
		private:
			int				GetFrameNum(void) noexcept override { return static_cast<int>(MovieHeliFrame::Max); }
			const char*		GetFrameStr(int id) noexcept override { return MovieHeliName[id]; }

			//int				GetShapeNum(void) noexcept override { return static_cast<int>(CharaShape::Max); }
			//const char*		GetShapeStr(int id) noexcept override { return CharaShapeName[id]; }
		private: //継承
			void			Init_Sub(void) noexcept override;
			void			FirstUpdate(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;

			void			DrawShadow(void) noexcept override {
				if (!IsActive()) { return; }
				GetObj().DrawModel();
			}
			void			Draw(bool, int) noexcept override {
				if (!IsActive()) { return; }
				GetObj().DrawModel();
			}
		public:
			void			UpdateLocal(void) noexcept;
		};
	}
}
