#include	"Object2DManager.hpp"

	const FPS_n2::Sceneclass::Object2DManager* SingletonBase<FPS_n2::Sceneclass::Object2DManager>::m_Singleton = nullptr;
	namespace FPS_n2 {
		namespace Sceneclass {

			void			Object2DManager::AddObject(const SharedObj2D& NewObj) noexcept {
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = NewObj;
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
						//順番の維持のためここはerase
						o->Dispose();
						this->m_Object.erase(this->m_Object.begin() + (&o - &this->m_Object.front()));
						break;
					}
				}
			}

			void 	Object2DManager::CheckColObject(const Base2DObject* ptr) noexcept {

				VECTOR SegmentPos1 = ptr->GetPos().get();SegmentPos1.z = 0.f;
				VECTOR SegmentPos2 = ptr->GetPos().get();SegmentPos2.z = 0.f;
				SEGMENT_POINT_RESULT Result;
				float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));
				for (auto& o : this->m_Object) {
					if (o.get() == ptr) { continue; }
					//自分が当たったら押し出し
					VECTOR PointPos = VGet(o->GetPos().x, o->GetPos().y, 0.f);PointPos.z = 0.f;
					Segment_Point_Analyse(&SegmentPos1, &SegmentPos2, &PointPos, &Result);
					if (Result.Seg_Point_MinDist_Square < ((HalfLen + HalfLen) * (HalfLen + HalfLen))) {
						Vector3DX vec = o->GetPos() - Result.Seg_MinDist_Pos;
						float Len = vec.magnitude();

						o->SetPos(o->GetPos() - vec.normalized() * (Len - (HalfLen + HalfLen)));
					}
				}
			};

			void			Object2DManager::ExecuteObject(void) noexcept {
				//アップデート
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (!o->GetIsDelete()) {
						o->Execute();
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