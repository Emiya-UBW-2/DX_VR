#include	"ObjectManager.hpp"

const FPS_n2::Sceneclass::ObjectManager* SingletonBase<FPS_n2::Sceneclass::ObjectManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {
		void			ObjectManager::AddObject(const SharedObj& NewObj) noexcept {
			this->m_Object.resize(this->m_Object.size() + 1);
			this->m_Object.back() = NewObj;
		}
		void			ObjectManager::LoadModel(const SharedObj& pObj, const SharedObj& pAnim, const char* filepath, const char* objfilename, const char* colfilename) const noexcept {
			for (auto& o : this->m_Object) {
				if (!o->GetIsBaseModel()) { continue; }
				if (!o->GetPathCompare(filepath, objfilename, colfilename)) { continue; }
				pObj->CopyModel(o);
				if (pAnim) {
					MV1::SetAnime(&pObj->GetObj(), pAnim->GetObj());
				}
				return;
			}
			pObj->LoadModel(PHYSICS_SETUP::DISABLE, filepath, objfilename, colfilename);
			pObj->SaveModel(false);
			if (pAnim) {
				MV1::SetAnime(&pObj->GetObj(), pAnim->GetObj());
			}
		}
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
		void			ObjectManager::DelObj(SharedObj* ptr) noexcept {
			for (auto&o : this->m_Object) {
				if (o == *ptr) {
					//順番の維持のためここはerase
					o->Dispose();
					this->m_Object.erase(this->m_Object.begin() + (&o - &this->m_Object.front()));
					break;
				}
			}
		}

		void			ObjectManager::ExecuteObject(void) noexcept {
			//オブジェクトが増えた場合に備えて範囲forは使わない
			for (int i = 0; i < this->m_Object.size(); i++) {
				auto& o = this->m_Object[i];
				if (!o->GetIsDelete()) {
					/*
	#ifdef DEBUG
							printfDx("OBJ:[%s]\n", ObjTypeName[(int)o->GetobjType()]);
	#endif // DEBUG
					//*/
					o->FirstExecute();
				}
			}
			//物理アップデート
			this->m_ResetP.Execute(CheckHitKeyWithCheck(KEY_INPUT_P) != 0);
			for (int i = 0; i < this->m_Object.size(); i++) {
				auto& o = this->m_Object[i];
				if (!o->GetIsDelete()) {
					if (this->m_ResetP.trigger()) { o->SetResetP(true); }
					o->ExecuteCommon();
				}
			}
			//オブジェクトの排除チェック
			for (int i = 0; i < this->m_Object.size(); i++) {
				auto& o = this->m_Object[i];
				if (o->GetIsDelete()) {
					//順番の維持のためここはerase
					o->Dispose();
					this->m_Object.erase(this->m_Object.begin() + i);
					i--;
				}
			}
		}
		void			ObjectManager::LateExecuteObject(void) noexcept {
			for (auto&o : this->m_Object) {
				o->LateExecute();
				Vector3DX campos; campos.z = (-1.f);
				o->SetScreenPosition(campos, -1.f);
			}
		}

		void			ObjectManager::Draw_Depth(void) noexcept {
			for (auto& o : this->m_Object) {
				o->Depth_Draw();
			}
		}
		void			ObjectManager::Draw() noexcept {
			for (auto& o : this->m_Object) {
				o->CheckDraw();
				o->Draw(false);
			}
			for (auto& o : this->m_Object) {
				o->Draw(true);
			}
		}
		void			ObjectManager::Draw_Shadow(void) noexcept {
			for (auto& o : this->m_Object) {
				o->DrawShadow();
			}
		}

		void			ObjectManager::DeleteAll(void) noexcept {
			for (auto& o : this->m_Object) {
				if (o) {
					o->Dispose();
				}
			}
			this->m_Object.clear();
		}
	};
};
