#include	"Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//AI—p
		class CharacterObject::Impl {
		private:
			PlayerID m_MyCharaID;
		public:
			Impl(void) noexcept {}
			~Impl(void) noexcept {}
		public:
			void		Init(PlayerID MyCharaID) noexcept {
				m_MyCharaID = MyCharaID;
			}
			//
			void		Execute() noexcept {
			}
			void		Draw() noexcept {
				DrawCircle(32 * m_MyCharaID, 32 * m_MyCharaID, 5, Red);
			}
			void		Dispose() noexcept {
			}
		};
		//
		CharacterObject::CharacterObject() {
			m_Param = new Impl;
		}
		CharacterObject::~CharacterObject() {
			delete m_Param;
		}
		//
		void CharacterObject::Init(PlayerID MyCharaID) noexcept {
			this->GetParam()->Init(MyCharaID);
		}
		void CharacterObject::Execute() noexcept {
			this->GetParam()->Execute();
		}
		void CharacterObject::Draw() noexcept {
			this->GetParam()->Draw();
		}
		void CharacterObject::Dispose() noexcept {
			this->GetParam()->Dispose();
		}
	};
};
