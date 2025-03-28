#include	"Character_before.hpp"
#include	"Character.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void HitBoxControl::Update(const ObjectBaseClass* ptr, float SizeRate) noexcept {
			auto* Ptr = (CharacterClass*)ptr;
			int ID = 0;
			m_HitBox[ID].Update(Ptr->GetEyePositionCache(), 0.13f * Scale3DRate * SizeRate, HitType::Head); ID++;
			m_HitBox[ID].Update((Ptr->GetEyePositionCache() + Ptr->GetFrameWorldMat(CharaFrame::Upper).pos()) / 2.f, 0.16f * Scale3DRate * SizeRate, HitType::Body); ID++;
			m_HitBox[ID].Update(Ptr->GetFrameWorldMat(CharaFrame::Upper).pos(), 0.13f * Scale3DRate * SizeRate, HitType::Body); ID++;

			m_HitBox[ID].Update((Ptr->GetFrameWorldMat(CharaFrame::Upper).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightFoot1)).pos()) / 2.f, 0.13f * Scale3DRate * SizeRate, HitType::Body); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat(CharaFrame::Upper).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot1)).pos()) / 2.f, 0.13f * Scale3DRate * SizeRate, HitType::Body); ID++;

			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightArm)).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightArm2)).pos()) / 2.f, 0.06f * Scale3DRate * SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::RightArm2)).pos(), 0.06f * Scale3DRate * SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightWrist)).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightArm2)).pos()) / 2.f, 0.06f * Scale3DRate * SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::RightWrist)).pos(), 0.06f * Scale3DRate * SizeRate, HitType::Arm); ID++;

			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftArm)).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftArm2)).pos()) / 2.f, 0.06f * Scale3DRate * SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::LeftArm2)).pos(), 0.06f * Scale3DRate * SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftWrist)).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftArm2)).pos()) / 2.f, 0.06f * Scale3DRate * SizeRate, HitType::Arm); ID++;
			m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::LeftWrist)).pos(), 0.06f * Scale3DRate * SizeRate, HitType::Arm); ID++;

			m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::RightFoot1)).pos(), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightFoot1)).pos() + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos()) / 2.f, 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos(), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos() * 0.25f + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos() * 0.75f), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos() * 0.5f + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos() * 0.5f), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos() * 0.75f + Ptr->GetFrameWorldMat((CharaFrame::RightFoot2)).pos() * 0.25f), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::RightFoot)).pos(), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;

			m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::LeftFoot1)).pos(), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot1)).pos() + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos()) / 2.f, 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos(), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos() * 0.25f + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos() * 0.75f), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos() * 0.5f + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos() * 0.5f), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update((Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos() * 0.75f + Ptr->GetFrameWorldMat((CharaFrame::LeftFoot2)).pos() * 0.25f), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
			m_HitBox[ID].Update(Ptr->GetFrameWorldMat((CharaFrame::LeftFoot)).pos(), 0.095f * Scale3DRate * SizeRate, HitType::Leg); ID++;
		}
		//
		void AutoAimControl::Update(bool isActive, PlayerID MyPlayerID, const Vector3DX& EyePos, const Vector3DX& AimVector, float Radian) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto* BackGround = BackGround::BackGroundClass::Instance();
			if (isActive) {
				for (int i = 0; i < PlayerMngr->GetPlayerNum(); i++) {
					if (i == MyPlayerID) { continue; }
					auto& c = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(i)->GetChara();
					if (!c->IsAlive()) { continue; }
					if (!c->m_CanLookTarget) { continue; }
					int pos = -1;
					float CosMax = -1.f;
					for (const auto& h : c->GetHitBoxList()) {
						auto EndPost = h.GetPos();
						if (BackGround->CheckLinetoMap(EyePos, &EndPost)) { continue; }
						switch (h.GetColType()) {
						case FPS_n2::Sceneclass::HitType::Head:
						case FPS_n2::Sceneclass::HitType::Body:
							break;
						default:
							continue;
							break;
						}
						float Cos = Vector3DX::Dot(AimVector, (h.GetPos() - EyePos).normalized());
						if (Cos > cos(Radian)) {
							if (CosMax < Cos) {
								CosMax = Cos;
								pos = static_cast<int>(&h - &c->GetHitBoxList().front());
							}
						}
					}
					if (pos != -1) {
						m_AutoAimTimer = 1.f;
						m_AutoAim = i;
						m_AutoAimPoint = pos;
						break;
					}
				}
			}

			auto prev = m_AutoAimTimer;
			m_AutoAimTimer = std::max(m_AutoAimTimer - DXLib_refParts->GetDeltaTime(), 0.f);
			if (prev > 0.f && m_AutoAimTimer == 0.f) {
				m_AutoAim = -1;
			}
			if (m_AutoAim != -1) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(m_AutoAim)->GetChara();
				if (!Chara->IsAlive()) {
					m_AutoAim = -1;
				}
			}
			m_AutoAimActive = (isActive && this->m_AutoAim != -1);
			Easing(&m_AutoAimPer, m_AutoAimActive ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

			if (this->m_AutoAim != -1) {
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(this->m_AutoAim)->GetChara();
				Easing(&m_AutoAimVec, (Chara->GetHitBoxList().at(this->m_AutoAimPoint).GetPos() - EyePos).normalized(), 0.8f, EasingType::OutExpo);
			}
			else {
				Easing(&m_AutoAimVec, AimVector, 0.95f, EasingType::OutExpo);
			}
		}
	};
};
