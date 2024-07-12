#include	"Object2DManager.hpp"

const FPS_n2::Sceneclass::Object2DManager* SingletonBase<FPS_n2::Sceneclass::Object2DManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {

		void			Object2DManager::AddObject(const SharedObj2D& NewObj) noexcept {
			this->m_Object.resize(this->m_Object.size() + 1);
			this->m_Object.back() = NewObj;
			this->m_Object.back()->SetObjectID(this->m_LastUniqueID);
			this->m_Object.back()->Init();
			this->m_LastUniqueID = 0;
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
			for (auto& o : this->m_Object) {
				if (o.get() == ptr) { continue; }
				// 自分が当たったら押し出し
				SEGMENT_SEGMENT_RESULT Result;
				GetSegmenttoSegment(ptr->GetPrevPos(), ptr->GetPos(), o->GetPrevPos(), o->GetPos(), &Result);

				float Mag = std::sqrt(Result.SegA_SegB_MinDist_Square);
				float Len = Get2DSize(ptr->GetSize() / 2.f + o->GetSize() / 2.f);
				if (Mag < Len) {
					bool ptr_p = ptr->GetHitTarget() == HitTarget::Physical;
					bool o_p = o->GetHitTarget() == HitTarget::Physical;
					if (ptr_p || o_p) {
						Vector3DX pos;
						pos += Result.SegA_MinDist_Pos;
						pos += Result.SegB_MinDist_Pos;
						pos /= 2.f;
						Vector3DX vec = o->GetPos() - pos; vec.z = 0.f;
						float Per = 0.5f;
						if (ptr_p && !o_p) {
							Per = 0.f;
						}
						else if (!ptr_p && o_p) {
							Per = 1.f;
						}
						o->SetPos(o->GetPos() - vec.normalized() * (Mag - Len) * Per);
						ptr->SetPos(ptr->GetPos() + vec.normalized() * (Mag - Len) * (1.f - Per));
					}
					if (!ptr_p) {
						ptr->SetHitObjectID(o->GetObjectID());
					}
					if (!o_p) {
						o->SetHitObjectID(ptr->GetObjectID());
					}
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
			for (int i = 0, Max = static_cast<int>(this->m_Object.size()); i < Max; i++) {
				auto& o = this->m_Object.at(static_cast<size_t>(i));
				if (o->GetIsDelete()) {
					// 順番の維持のためここはerase
					o->Dispose();
					this->m_Object.erase(this->m_Object.begin() + i);
					i--;
					Max--;
				}
			}
		}
		void			Object2DManager::DrawShadow(void) noexcept {
			for (auto& o : this->m_Object) {
				if (o->GetIsDraw()) {
					o->DrawShadow();
				}
			}
		}
		void			Object2DManager::Draw(void) noexcept {
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
			this->m_LastUniqueID = 0;//一旦ユニークIDもリセット
		}
	};
};