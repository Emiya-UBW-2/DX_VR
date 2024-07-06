#include	"CPU.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ENUM_AI_PHASE {
			Normal,
			Caution,
			Dead,
		};
		//AI—p
		class AIControl::Impl {
		private:
			const PlayerID							m_TargetCharaID{0};
		public:
			ENUM_AI_PHASE							m_Phase{ENUM_AI_PHASE::Normal};
			PlayerID								m_MyCharaID{0};
			InputControl							m_MyInput;
			float		m_Rad{0.f};
			Vector3DX	m_LastFindPos{};

			float m_LostTimer{0.f};
		public:
			Impl(void) noexcept {}
			~Impl(void) noexcept {}
		public:
			const auto& GetViewRad() const noexcept { return m_Rad; }
		private:
			const auto GetIsSeeTarget() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				if (Chara->CanLookPlayer0()) {
					float ViewLimit = 10.f * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < ViewLimit) {
						m_LastFindPos = Target->GetPos();
						return true;
					}
				}
				return false;
			}
		public:
			void		Init(PlayerID MyCharaID) noexcept {
				m_MyCharaID = MyCharaID;
			}
			//
			void		Execute_Before() noexcept {
				//‰Šú‰»
				m_MyInput.ResetAllInput();
			}
			void		Execute_Normal() noexcept {
				if (GetIsSeeTarget()) {
					this->m_Phase = ENUM_AI_PHASE::Caution;
				}
			}
			void		Execute_Caution() noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				//
				m_Rad = -GetRadVec2Vec(Convert2DtoDisp(Chara->GetPos()), Convert2DtoDisp(m_LastFindPos));
				//
				if (GetIsSeeTarget()) {
					m_LostTimer = 5.f;
				}
				else {
					m_LostTimer = std::max(m_LostTimer - 1.f / DrawParts->GetFps(), 0.f);
					if (m_LostTimer == 0.f) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
					}
				}
			}
		};
		//
		AIControl::AIControl() {
			m_Param = new Impl;
		}
		AIControl::~AIControl() {
			delete m_Param;
		}
		const float & AIControl::GetViewRad() const noexcept {
			return this->GetParam()->GetViewRad();
		}
		const bool AIControl::IsCaution() const noexcept {
			return this->GetParam()->m_Phase == ENUM_AI_PHASE::Caution;
		}
		const bool AIControl::IsAlert() const noexcept {
			return false;
		}
		//
		void AIControl::Init(PlayerID MyCharaID) noexcept {
			this->GetParam()->Init(MyCharaID);
		}
		void AIControl::Execute(InputControl* MyInput) noexcept {
			//return;
			//AI
			this->GetParam()->Execute_Before();
			switch (this->GetParam()->m_Phase) {
				case ENUM_AI_PHASE::Normal:
					this->GetParam()->Execute_Normal();
					break;
				case ENUM_AI_PHASE::Caution:
					this->GetParam()->Execute_Caution();
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
