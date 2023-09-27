#include	"ObjectManager.hpp"
#include "MainScene/Object/Ammo.hpp"
#include "MainScene/Object/Gun.hpp"
#include "MainScene/Object/Mod.hpp"
#include "MainScene/Object/Character.hpp"
#include "MainScene/Object/Cart.hpp"

const FPS_n2::Sceneclass::ObjectManager* SingletonBase<FPS_n2::Sceneclass::ObjectManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {
		SharedObj*		ObjectManager::MakeObject(ObjType ModelType) noexcept {
			switch (ModelType) {
			case ObjType::Human:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<CharacterClass>();
				break;
			case ObjType::Ammo:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<AmmoClass>();
				break;
			case ObjType::Cart:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<CartClass>();
				break;
			case ObjType::Gun:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<GunClass>();
				break;
			case ObjType::Magazine:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<MagazineClass>();
				break;
			case ObjType::Lower:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<LowerClass>();
				break;
			case ObjType::Upper:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<UpperClass>();
				break;
			case ObjType::Barrel:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<BarrelClass>();
				break;
			default:
				break;
			}
			return &(this->m_Object[this->m_Object.size() - 1]);
		}
		void			ObjectManager::LoadModel(PHYSICS_SETUP TYPE, bool UseToonWhenCreateFile, ObjectBaseClass* pObj, const char* filepath, const char* objfilename, const char* colfilename) const noexcept {
			bool iscopy = false;
			for (auto& o : this->m_Object) {
				if (o->GetIsBaseModel(filepath, objfilename, colfilename)) {
					pObj->CopyModel(o);
					iscopy = true;
					break;
				}
			}
			if (!iscopy) {
				pObj->LoadModel(TYPE, UseToonWhenCreateFile, filepath, objfilename, colfilename);
			}
			pObj->SetFrameNum();
		}
		//
		SharedObj*		ObjectManager::GetObj(ObjType ModelType, int num) noexcept {
			int cnt = 0;
			for (auto&o : this->m_Object) {
				if (o->GetobjType() == ModelType) {
					if (cnt == num) {
						return &o;
					}
					cnt++;
				}
			}
			return nullptr;
		}
		//
		void			ObjectManager::DelObj(ObjType ModelType, int num) noexcept {
			int cnt = 0;
			for (auto&o : this->m_Object) {
				if (o->GetobjType() == ModelType) {
					if (cnt == num) {
						//‡”Ô‚ÌˆÛŽ‚Ì‚½‚ß‚±‚±‚Íerase
						o->Dispose();
						this->m_Object.erase(this->m_Object.begin() + (&o - &this->m_Object.front()));
						break;
					}
					cnt++;
				}
			}
		}
	};
};
