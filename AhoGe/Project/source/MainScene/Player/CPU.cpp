#include	"CPU.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//AIóp
		class AIControl::Impl {
		public:
			ENUM_AI_PHASE							m_Phase{ENUM_AI_PHASE::Normal};
			PlayerID								m_MyCharaID{0};
			InputControl							m_MyInput;
		private:
			const PlayerID							m_TargetCharaID{0};
		public:
			Impl(void) noexcept {}
			~Impl(void) noexcept {}
		public:
			void		Init() noexcept {
			}
			//
			void		Execute_Before() noexcept {
				//èâä˙âª
				m_MyInput.ResetAllInput();
			}
			void		Execute_Normal() noexcept {
			}
		};
		//
		AIControl::AIControl() {
			m_Param = new Impl;
		}
		AIControl::~AIControl() {
			delete m_Param;
		}
		//
		void AIControl::Init(const std::shared_ptr<BackGroundClassMain>& BackBround_t, PlayerID MyCharaID) noexcept {
			this->GetParam()->Init();
		}
		void AIControl::Execute(InputControl* MyInput) noexcept {
			//return;
			//AI
			this->GetParam()->Execute_Before();
			switch (this->GetParam()->m_Phase) {
				case ENUM_AI_PHASE::Normal:
					this->GetParam()->Execute_Normal();
					break;
				case ENUM_AI_PHASE::Shot:
					break;
				case ENUM_AI_PHASE::Dead:
					break;
				default:
					break;
			}
			*MyInput = this->GetParam()->m_MyInput;
		}
	};
};
