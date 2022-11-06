#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {


		//îÌíeÉ`ÉFÉbÉN
		const auto		CharacterClass::CheckAmmoHited(const AmmoClass& pAmmo) noexcept {
			bool is_Hit = false;
			for (auto& h : m_HitBox) {
				is_Hit |= h.Colcheck(pAmmo);
			}
			return is_Hit;
		}
		//îÌíeèàóù
		const auto		CharacterClass::CalcAmmoHited(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept {
			auto SE = SoundPool::Instance();
			bool isDamage = false;
			return isDamage;
		}
		const std::pair<bool, bool>		CharacterClass::CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept {
			std::pair<bool, bool> isDamaged{ false,false };
			if (GetMinLenSegmentToPoint(pAmmo->GetMove().repos, pAmmo->GetMove().pos, this->m_move.pos) <= 2.0f*Scale_Rate) {
				if (this->CheckAmmoHited(*pAmmo)) {									//Ç∆ÇËÇ†Ç¶Ç∏ìñÇΩÇ¡ÇΩÇ©Ç«Ç§Ç©íTÇ∑
					isDamaged.first = true;
					if (this->CalcAmmoHited(pAmmo, pShooterPos)) {
						isDamaged.second = true;
					}
				}
			}
			return isDamaged;
		}
	};
};
