#include	"CPU.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ENUM_AI_PHASE {
			Normal,
			Caution,
			Alert,
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

			float m_GraphTimer{0.f};
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
						float NearLimit = 1.5f * ((64.f * 64.f) / (1080 / 2));
						if ((Chara->GetPos() - Target->GetPos()).magnitude() < NearLimit) {
							m_LastFindPos = Target->GetPos();
							return true;
						}
						Vector3DX Vec;Vec.Set(std::sin(Chara->GetViewRad()), std::cos(Chara->GetViewRad()), 0.f);
						Vector3DX vec_a;vec_a = (Chara->GetPos() - Target->GetPos()).normalized();
						if (-Vector3DX::Dot(vec_a, Vec) > std::cos(deg2rad(45))) {
							m_LastFindPos = Target->GetPos();
							return true;
						}
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
				auto* DrawParts = DXDraw::Instance();
				//‰Šú‰»
				m_MyInput.ResetAllInput();
				this->m_GraphTimer = std::max(this->m_GraphTimer - 1.f / DrawParts->GetFps(), 0.f);
			}
			void		Execute_Normal() noexcept {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				//
				if (GetIsSeeTarget()) {
					float ViewLimit = 5.f * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < ViewLimit) {
						this->m_Phase = ENUM_AI_PHASE::Alert;
						this->m_GraphTimer = 2.f;
					}
					else {
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
					}
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
					float ViewLimit = 5.f * ((64.f * 64.f) / (1080 / 2));
					if ((Chara->GetPos() - Target->GetPos()).magnitude() < ViewLimit) {
						this->m_Phase = ENUM_AI_PHASE::Alert;
						this->m_GraphTimer = 2.f;
					}
					m_LostTimer = 5.f;
				}
				else {
					m_LostTimer = std::max(m_LostTimer - 1.f / DrawParts->GetFps(), 0.f);
					if (m_LostTimer == 0.f) {
						this->m_Phase = ENUM_AI_PHASE::Normal;
						this->m_GraphTimer = 2.f;
					}
				}
			}
			void		Execute_Alert() noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer((PlayerID)m_MyCharaID).GetChara();
				//auto& Target = PlayerMngr->GetPlayer((PlayerID)m_TargetCharaID).GetChara();
				//
				m_Rad = -GetRadVec2Vec(Convert2DtoDisp(Chara->GetPos()), Convert2DtoDisp(m_LastFindPos));
				//
				if (GetIsSeeTarget()) {
					m_LostTimer = 5.f;
				}
				else {
					m_LostTimer = std::max(m_LostTimer - 1.f / DrawParts->GetFps(), 0.f);
					if (m_LostTimer == 0.f) {
						m_LostTimer = 5.f;
						this->m_Phase = ENUM_AI_PHASE::Caution;
						this->m_GraphTimer = 2.f;
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
		const float & AIControl::GetGraphTimer() const noexcept {
			return this->GetParam()->m_GraphTimer;
		}
		const bool AIControl::IsCaution() const noexcept {
			return this->GetParam()->m_Phase == ENUM_AI_PHASE::Caution;
		}
		const bool AIControl::IsAlert() const noexcept {
			return this->GetParam()->m_Phase == ENUM_AI_PHASE::Alert;
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
				case ENUM_AI_PHASE::Alert:
					this->GetParam()->Execute_Alert();
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
