#include	"Mod.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			ModClass::Init(void) noexcept {
			ObjectBaseClass::Init();
			//�f�[�^
			InitModSlotControl(this->m_FilePath, true);
			Init_Mod();
		}
	};
};
