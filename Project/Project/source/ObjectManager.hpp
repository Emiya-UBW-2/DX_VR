#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectManager {
			std::vector<std::shared_ptr<ObjectBaseClass>> m_Object;
			switchs m_ResetP;
			const MV1* m_MapCol = nullptr;
			//�V���O���g����
#if true

		private:
			static inline  ObjectManager* m_Singleton = nullptr;
		public:
			static void Create() {
				if (m_Singleton == nullptr) {
					m_Singleton = new ObjectManager();
				}
			}
			static ObjectManager* Instance(void) {
				if (m_Singleton == nullptr) {
					m_Singleton = new ObjectManager();
				}
				return m_Singleton;
			}
		private:

#endif
		public:
			std::shared_ptr<ObjectBaseClass>* AddObject(ObjType ModelType, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") {
				switch (ModelType) {
				case ObjType::Vehicle:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<VehicleClass>();
					break;
				case ObjType::Ammo:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<AmmoClass>();
					break;
				default:
					break;
				}
				{
					bool iscopy = false;
					for (auto& o : this->m_Object) {
						if (o->GetIsBaseModel(filepath, objfilename, colfilename)) {
							this->m_Object.back()->CopyModel(o);
							iscopy = true;
							break;
						}
					}
					if (!iscopy) {
						this->m_Object.back()->LoadModel(filepath, objfilename, colfilename);
					}
				}

				this->m_Object.back()->SetMapCol(this->m_MapCol);
				this->m_Object.back()->Init();
				this->m_Object.back()->SetFrameNum();

				return &(this->m_Object[this->m_Object.size() - 1]);
			}
			std::shared_ptr<ObjectBaseClass>* AddObject(ObjType ModelType) {
				switch (ModelType) {
				case ObjType::Vehicle:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<VehicleClass>();
					break;
				case ObjType::Ammo:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<AmmoClass>();
					break;
				default:
					break;
				}
				this->m_Object.back()->SetMapCol(this->m_MapCol);
				this->m_Object.back()->Init();
				this->m_Object.back()->SetFrameNum();

				return &(this->m_Object[this->m_Object.size() - 1]);
			}
			std::shared_ptr<ObjectBaseClass>* GetObj(ObjType ModelType, int num) {
				int cnt = 0;
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetobjType() == ModelType) {
						if (cnt == num) {
							return &this->m_Object[i];
						}
						cnt++;
					}
				}
				return nullptr;
			}
			void			DelObj(ObjType ModelType, int num) {
				int cnt = 0;
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetobjType() == ModelType) {
						if (cnt == num) {
							//���Ԃ̈ێ��̂��߂�����erase
							this->m_Object.erase(this->m_Object.begin() + i);
							break;
						}
						cnt++;
					}
				}
			}
			//Delobj�\��
		public:
			void			Init(const MV1* MapCol) {
				this->m_MapCol = MapCol;
			}
			void			ExecuteObject(void) noexcept {
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					o->FirstExecute();
				}
				//�����A�b�v�f�[�g
				this->m_ResetP.GetInput(CheckHitKeyWithCheck(KEY_INPUT_P) != 0);

				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (this->m_ResetP.trigger()) { o->SetResetP(true); }
					o->ExecuteCommon();
				}
			}
			void			LateExecuteObject(void) noexcept {
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					o->LateExecute();
				}
			}
			void			DrawDepthObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Depth_Draw();
				}
			}
			void			DrawObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->CheckDraw();
					o->Draw();
				}
			}
			void			DrawObject_Shadow(void) noexcept {
				for (auto& o : this->m_Object) {
					o->DrawShadow();
				}
			}
			void			DisposeObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Dispose();
				}
			}
			//
		};
	};
};
