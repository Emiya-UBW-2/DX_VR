#include	"Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			ModClass::Init(void) noexcept {
			ObjectBaseClass::Init();
			//ƒf[ƒ^
			InitModSlotControl(this->m_FilePath, true);
			Init_Mod();
		}
	};
};
