#pragma once
#include	"Header.hpp"

#include "MainScene/Object/CharacterEnum.hpp"
//
namespace FPS_n2 {

	//ダメージイベント
	struct DamageEvent {
		PlayerID				ShotID{ 127 };
		PlayerID				DamageID{ 127 };
		HitPoint				Damage{ 0 };
		ArmerPoint				ArmerDamage{ 0 };
		Vector3DX				m_Pos;
	public:
		void SetEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage, ArmerPoint pArmerDamage, const Vector3DX& pPos) noexcept {
			this->ShotID = ShotID_t;
			this->DamageID = DamageID_t;
			this->Damage = pDamage;
			this->ArmerDamage = pArmerDamage;
			this->m_Pos = pPos;
		}
	};
	class DamageEventControl {
		std::array<DamageEvent, 16>							m_Damage;
		uint8_t												m_DamageCount{ 0 };
	public:
		const auto& GetDamageEvent(void) const noexcept { return this->m_Damage; }
		const auto& GetDamageEventCount(void) const noexcept { return this->m_DamageCount; }
	public:
		void			AddDamageEvent(std::vector<DamageEvent>* pRet) noexcept {
			if (!pRet) { return; }
			for (auto& e : this->m_Damage) {
				if ((&e - &this->m_Damage.front()) >= this->m_DamageCount) { break; }
				pRet->emplace_back(e);
			}
			this->m_DamageCount = 0;
		}
	public:
		void			Reset(void) noexcept {
			this->m_DamageCount = 0;
		}
		void			Add(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage, ArmerPoint pArmerDamage, const Vector3DX& pPos) noexcept {
			this->m_Damage.at(this->m_DamageCount).SetEvent(ShotID_t, DamageID_t, pDamage, pArmerDamage, pPos);
			++this->m_DamageCount %= static_cast<uint8_t>(this->m_Damage.size());
		}
	};
	//
	namespace Sceneclass {
		class CommonBattleResource : public SingletonBase<CommonBattleResource> {
		private:
			friend class SingletonBase<CommonBattleResource>;
		private:
		public:
			void			Load(void) noexcept;
			void			Set(void) noexcept;
			void			Dispose(void) noexcept;
		};
	};


	class FadeControl {
		bool						m_IsBlackOut{ false };//カットシーン中フラグ
		float						m_BlackOutAlpha{ 0.f };
	public:
		auto		IsClear(void) const noexcept { return this->m_BlackOutAlpha == 0.f; }
		auto		IsAll(void) const noexcept { return this->m_BlackOutAlpha >= 1.f; }
	public:
		void			SetBlackOut(bool value) noexcept { this->m_IsBlackOut = value; }
	public:
		void			Init(void) noexcept;
		void			Update(void) noexcept;
		void			Draw(void) const noexcept;
	};
};
