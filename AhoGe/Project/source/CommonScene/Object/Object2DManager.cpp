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
						//���Ԃ̈ێ��̂��߂�����erase
						o->Dispose();
						this->m_Object.erase(this->m_Object.begin() + (&o - &this->m_Object.front()));
						break;
					}
				}
			}

			void			Object2DManager::ExecuteObject(void) noexcept {
				//�A�b�v�f�[�g
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (!o->GetIsDelete()) {
						o->Execute();
					}
				}
				//�I�u�W�F�N�g�̔r���`�F�b�N
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetIsDelete()) {
						//���Ԃ̈ێ��̂��߂�����erase
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