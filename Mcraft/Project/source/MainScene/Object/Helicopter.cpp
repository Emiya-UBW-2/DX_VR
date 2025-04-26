#include	"Helicopter.hpp"

#include	"../Player/Player.hpp"

namespace FPS_n2 {
	namespace Objects {
		const bool HelicopterObj::CheckAmmoHit(const Vector3DX& StartPos, Vector3DX* EndPos) noexcept {
			auto* SE = SoundPool::Instance();
			//とりあえず当たったかどうか探す
			if (!RefreshCol(StartPos, *EndPos, 10.f * Scale3DRate)) {
				return false;
			}
			auto colres_t = GetColLine(StartPos, *EndPos, 0);
			if (colres_t.HitFlag == TRUE) {
				*EndPos = colres_t.HitPosition;
				EffectSingleton::Instance()->SetOnce_Any(Effect::ef_gndsmoke, *EndPos, colres_t.Normal, 2.0f);
				return true;
			}
			return false;
		}
		void HelicopterObj::FirstUpdate(void) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();

			m_RopePer = std::clamp(m_RopePer + (m_Rope ? 10.f : -5.f) * DXLib_refParts->GetDeltaTime(), 0.f, 10.f);
			Easing(&m_OpenPer, (m_Open) ? 1.f : 0.f, 0.95f, EasingType::OutExpo);

			SetAnimLoop(static_cast<int>(0), 1.f);
			SetObj().SetAnim(static_cast<int>(0)).SetPer(1.f);
			SetAnimLoop(static_cast<int>(1), 1.f);
			SetObj().SetAnim(static_cast<int>(1)).SetPer(m_OpenPer);
			SetObj().UpdateAnimAll();

			m_Timer += DXLib_refParts->GetDeltaTime();

			switch (this->m_HelicopterMove) {
			case HelicopterMove::Random:
				if (m_Timer > 10.f) {
					SetAction(HelicopterMove::Random);

					this->m_SpawnPoint = PlayerMngr->GetPlayerNum() - 1;
					SetAction(HelicopterMove::Rappelling);
				}
				m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.f, 0.f, 1.f));


				break;
			case HelicopterMove::Rappelling:
				if (m_Timer > 8.f) {
					m_Open = true;
				}
				if (m_Timer > 13.f) {
					m_Rope = true;
				}
				if (m_Timer > 33.f) {
					m_Rope = false;
				}
				if (m_Timer > 38.f) {
					SetAction(HelicopterMove::Random);
					m_Open = false;
				}
				m_NowPos = Lerp(m_PrevPos, m_TargetPos, std::clamp(m_Timer / 8.f, 0.f, 1.f));
				break;
			default:
				break;
			}



			auto PrevPos = m_Pos;

			Easing(&m_PosR, m_NowPos + (Vector3DX::vget(0.f, -20.f, 0.f) + Vector3DX::vget(GetRandf(1.f), GetRandf(1.f), GetRandf(1.f))) * Scale3DRate, 0.95f, EasingType::OutExpo);
			Easing(&m_Pos, m_PosR, 0.95f, EasingType::OutExpo);

			auto Vec = Matrix3x3DX::Vtrans((m_Pos - PrevPos), GetMove().GetMat().inverse()) / (60.f * DXLib_refParts->GetDeltaTime());
			Easing(&m_flontSpeedPer, std::clamp(-Vec.z, -1.f, 1.f), 0.975f, EasingType::OutExpo);
			Easing(&m_SideSpeedPer, std::clamp(-Vec.x, -1.f, 1.f), 0.975f, EasingType::OutExpo);

			auto PrevYrad = m_Yrad;
			Easing(&m_YradR, m_YradRT, 0.985f, EasingType::OutExpo);
			Easing(&m_Yrad, m_YradR, 0.985f, EasingType::OutExpo);

			Easing(&m_ZradR, GetRandf(30) + (m_Yrad - PrevYrad) * 50.f, 0.95f, EasingType::OutExpo);
			Easing(&m_Zrad, m_ZradR, 0.95f, EasingType::OutExpo);

			SetMat(m_Pos,
				Matrix3x3DX::RotAxis(Vector3DX::forward(), deg2rad(m_Zrad + 20.f * m_SideSpeedPer)) *
				Matrix3x3DX::RotAxis(Vector3DX::right(), deg2rad(-30.f * m_flontSpeedPer)) *
				Matrix3x3DX::RotAxis(Vector3DX::up(), deg2rad(m_Yrad)));

		}
	};
};
