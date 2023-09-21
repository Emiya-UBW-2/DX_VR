#include	"ObjectManager.hpp"
#include "MainScene/Object/Ammo.hpp"
#include "MainScene/Object/Gun.hpp"
#include "MainScene/Object/Mod.hpp"
#include "MainScene/Object/Character.hpp"
#include "MainScene/Object/Cart.hpp"

const FPS_n2::Sceneclass::ObjectManager* SingletonBase<FPS_n2::Sceneclass::ObjectManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {
		std::shared_ptr<ObjectBaseClass>* ObjectManager::MakeObject(ObjType ModelType) noexcept {
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

		std::shared_ptr<ObjectBaseClass>*  ObjectManager::AddObject(ObjType ModelType, PHYSICS_SETUP TYPE, bool UseToonWhenCreateFile, const char* filepath, const char* objfilename, const char* colfilename) noexcept {
			auto* Ptr = MakeObject(ModelType);

			LoadModel(TYPE, UseToonWhenCreateFile, (*Ptr).get(), filepath, objfilename, colfilename);

			(*Ptr)->SetMapCol(this->m_BackGround);
			(*Ptr)->Init();
			return Ptr;
		}
		std::shared_ptr<ObjectBaseClass>* ObjectManager::AddObject(ObjType ModelType) noexcept {
			auto* Ptr = MakeObject(ModelType);

			(*Ptr)->SetMapCol(this->m_BackGround);
			(*Ptr)->Init();
			return Ptr;
		}
	};
};
