#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectManager {
			std::vector<std::shared_ptr<ObjectBaseClass>> m_Object;
			switchs m_ResetP;
			const MV1* m_MapCol = nullptr;
		public:
			void AddObject(ObjType ModelType, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") {
				switch (ModelType) {
				case ObjType::Human:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<CharacterClass>();
					break;
				case ObjType::Houki:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<HoukiClass>();
					break;
				case ObjType::Item:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<ItemClass>();
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

				this->m_Object.back()->Init();
				this->m_Object.back()->SetFrameNum();
				this->m_Object.back()->SetMapCol(this->m_MapCol);
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
			void DelObj(ObjType ModelType, int num) {
				int cnt = 0;
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetobjType() == ModelType) {
						if (cnt == num) {
							//順番の維持のためここはerase
							this->m_Object.erase(this->m_Object.begin() + i);
							break;
						}
						cnt++;
					}
				}
			}
			//Delobj予定
		public:
			void Init(const MV1* MapCol) {
				this->m_MapCol = MapCol;
			}
			void ExecuteObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Execute();
				}
				//物理アップデート
				this->m_ResetP.GetInput(CheckHitKeyWithCheck(KEY_INPUT_P) != 0);

				for (auto& o : this->m_Object) {
					if (this->m_ResetP.trigger()) { o->SetResetP(true); }
					o->ExecuteCommon();
				}
			}
			void DrawDepthObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Depth_Draw();
				}
			}
			void DrawObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->CheckDraw();
					o->Draw();
				}
			}
			void DrawObject_Shadow(void) noexcept {
				for (auto& o : this->m_Object) {
					o->DrawShadow();
				}
			}
			void DisposeObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Dispose();
				}
			}
			//
		};
	};
};
