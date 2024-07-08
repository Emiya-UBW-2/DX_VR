#include	"Object2DManager.hpp"

const FPS_n2::Sceneclass::Object2DManager* SingletonBase<FPS_n2::Sceneclass::Object2DManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {

		void			Object2DManager::AddObject(const SharedObj2D& NewObj) noexcept {
			this->m_Object.resize(this->m_Object.size() + 1);
			this->m_Object.back() = NewObj;
			this->m_Object.back()->Init();
		}
		SharedObj2D* Object2DManager::GetObj(int ModelType, int num) noexcept {
			int cnt = 0;
			for (auto& o : this->m_Object) {
				if (o->GetObjType() == ModelType) {
					if (cnt == num) {
						return &o;
					}
					cnt++;
				}
			}
			return nullptr;
		}
		void			Object2DManager::DelObj(const SharedObj2D& ptr) noexcept {
			for (auto& o : this->m_Object) {
				if (o == ptr) {
					// 順番の維持のためここはerase
					o->Dispose();
					this->m_Object.erase(this->m_Object.begin() + (&o - &this->m_Object.front()));
					break;
				}
			}
		}

		void 	Object2DManager::CheckColObject(Base2DObject* ptr) noexcept {
			float Len = Get2DSize(0.5f + 0.5f);
			for (auto& o : this->m_Object) {
				if (o.get() == ptr) { continue; }
				// 自分が当たったら押し出し
				Vector3DX vec = o->GetPos() - ptr->GetPos();
				if (vec.sqrMagnitude() < (Len * Len)) {
					Vector3DX vec = o->GetPos() - ptr->GetPos();
					o->SetPos(o->GetPos() - vec.normalized() * (vec.magnitude() - Len) / 2.f);
					ptr->SetPos(ptr->GetPos() + vec.normalized() * (vec.magnitude() - Len) / 2.f);
				}
			}
		};

		void			Object2DManager::ExecuteObject(void) noexcept {
			// アップデート
			for (auto& o : this->m_Object) {
				if (!o->GetIsDelete()) {
					o->Execute();
				}
			}
			// アップデート
			for (auto& o : this->m_Object) {
				if (!o->GetIsDelete()) {
					o->ExecuteAfter();
				}
			}
			// オブジェクトの排除チェック
			for (int i = 0; i < this->m_Object.size(); i++) {
				auto& o = this->m_Object.at(i);
				if (o->GetIsDelete()) {
					// 順番の維持のためここはerase
					o->Dispose();
					this->m_Object.erase(this->m_Object.begin() + i);
					i--;
				}
			}
		}
		void			Object2DManager::Draw() noexcept {
			for (auto& o : this->m_Object) {
				if (o->GetIsDraw()) {
					o->Draw();
				}
			}
		}
		void			Object2DManager::DeleteAll(void) noexcept {
			for (auto& o : this->m_Object) {
				if (o) {
					o->Dispose();
				}
			}
			this->m_Object.clear();
		}
	};
};