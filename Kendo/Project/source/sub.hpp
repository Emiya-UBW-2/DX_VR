#pragma once
#include	"Header.hpp"
//
namespace FPS_n2 {
	//ダメージイベント
	struct DamageEvent {
		PlayerID				ShotID{127};
		PlayerID				DamageID{127};
		HitPoint				Damage{ 0 };
		void SetEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage) noexcept {
			this->ShotID = ShotID_t;
			this->DamageID = DamageID_t;
			this->Damage = pDamage;
		}
	};

	class DamageEventControl {
		std::array<DamageEvent, 32>							m_Damage;
		uint8_t												m_DamageCount{0};
	public:
		const auto&		GetDamageEvent(void) const noexcept { return this->m_Damage; }
		const auto&		GetDamageEventCount(void) const noexcept { return this->m_DamageCount; }
	public:
		void			AddDamageEvent(std::vector<DamageEvent>* pRet)const noexcept {
			if (!pRet) { return; }
			for (auto& e : this->m_Damage) {
				if ((&e - &this->m_Damage.front()) >= this->m_DamageCount) { break; }
				pRet->emplace_back(e);
			}
		}
	public:
		void			Reset(void) noexcept {
			this->m_DamageCount = 0;
		}
		void			Add(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage) noexcept {
			this->m_Damage.at(this->m_DamageCount).SetEvent(ShotID_t, DamageID_t, pDamage);
			++this->m_DamageCount %= static_cast<uint8_t>(this->m_Damage.size());
		}
	};
	//
	namespace Sceneclass {
		class CommonBattleResource : public SingletonBase<CommonBattleResource> {
		private:
			friend class SingletonBase<CommonBattleResource>;
		private:
			CommonBattleResource(void) noexcept {}
			CommonBattleResource(const CommonBattleResource&) = delete;
			CommonBattleResource(CommonBattleResource&& o) = delete;
			CommonBattleResource& operator=(const CommonBattleResource&) = delete;
			CommonBattleResource& operator=(CommonBattleResource&& o) = delete;

			virtual ~CommonBattleResource(void) noexcept {}
		public:
			void			Load(void) noexcept ;
			void			Set(void) noexcept ;
			void			Dispose(void) noexcept ;
		public:
			void			LoadChara(const std::string&FolderName, PlayerID ID) noexcept ;
			void			LoadGun(const std::string&FolderName, PlayerID ID) noexcept ;
		};
	};
};
