#pragma once
#include	"Header.hpp"
//
namespace FPS_n2 {
	//ダメージイベント
	struct DamageEvent {
		PlayerID				ShotID{127};
		PlayerID				DamageID{127};
		HitPoint				Damage{0};
		void SetEvent(PlayerID ShotID_t, PlayerID DamageID_t, HitPoint pDamage) noexcept {
			this->ShotID = ShotID_t;
			this->DamageID = DamageID_t;
			this->Damage = pDamage;
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
