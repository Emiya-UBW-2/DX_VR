#include	"MovieCharacter.hpp"

namespace FPS_n2 {
	namespace Charas {
		//
		void			MovieCharacter::Init_Sub(void) noexcept {
			SetMinAABB(Vector3DX::vget(2.0f, 0.0f, 2.0f) * -Scale3DRate);
			SetMaxAABB(Vector3DX::vget(2.0f, 2.0f, 2.0f) * Scale3DRate);
		}
		void			MovieCharacter::FirstUpdate(void) noexcept {
			SetMove().Update(0.8f, 0.0f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());

			SetAnimLoop(static_cast<int>(0), 1.0f);
			SetObj().SetAnim(static_cast<int>(0)).SetPer(1.0f);

			SetObj().UpdateAnimAll();
		}
		void MovieCharacter::Dispose_Sub(void) noexcept {
		}
		//
		void			MovieObject::Init_Sub(void) noexcept {
			SetMinAABB(Vector3DX::vget(2.0f, 0.0f, 2.0f) * -Scale3DRate);
			SetMaxAABB(Vector3DX::vget(2.0f, 2.0f, 2.0f) * Scale3DRate);
		}
		void			MovieObject::FirstUpdate(void) noexcept {}
		void MovieObject::Dispose_Sub(void) noexcept {}
		void MovieObject::UpdateLocal(void) noexcept {
			SetMove().Update(0.8f, 0.0f);
			UpdateObjMatrix(GetMove().GetMat(), GetMove().GetPos());

			SetAnimLoop(static_cast<int>(0), 1.0f);
			SetObj().SetAnim(static_cast<int>(0)).SetPer(1.0f);

			SetAnimOnce(static_cast<int>(1), 1.0f);
			SetObj().SetAnim(static_cast<int>(1)).SetPer(1.0f);

			SetObj().UpdateAnimAll();
		}
	}
}
