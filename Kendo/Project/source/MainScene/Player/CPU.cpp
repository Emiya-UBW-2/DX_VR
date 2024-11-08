#include	"CPU.hpp"
#pragma warning(disable:4464)
#include "../../MainScene/Object/Character.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Player {
		void AIControl::Execute(InputControl* MyInput, bool IsTutorial) noexcept
		{
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(m_MyID)->GetChara();
			auto& Target = (std::shared_ptr<CharacterObject::CharacterClass>&)PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();

			bool W_key{ false };
			bool A_key{ false };
			bool S_key{ false };
			bool D_key{ false };
			bool shotMain_Key{ false };
			bool shotSub_Key{ false };
			bool Guard_Key{ false };
			float pp_x{ 0.f }, pp_y{ 0.f };

			bool IsOutArea = false;
			{
				Vector3DX Vec = Chara->GetMove().GetPos() - Vector3DX::zero();
				float Len = 11.f / 2.f * Scale3DRate;
				if ((Vec.x < -Len || Len < Vec.x) ||
					(Vec.z < -Len || Len < Vec.z)) {
					IsOutArea = true;
				}
			}

			bool IsNearPlayer = false;
			bool IsFarPlayer = false;
			{
				Vector3DX Vec = Target->GetMove().GetPos() - Chara->GetMove().GetPos(); Vec.y = 0.f;
				float LenF = 3.5f * Scale3DRate;
				if (Vec.sqrMagnitude() > LenF * LenF) {
					IsFarPlayer = true;
				}
				float LenN = 1.0f * Scale3DRate;
				if (Vec.sqrMagnitude() < LenN * LenN) {
					IsNearPlayer = true;
				}
			}

			if (IsTutorial) {
				m_Counter01 += DXLib_refParts->GetDeltaTime();
				if (m_Counter01 > 5.f) {
					m_Counter01 -= 5.f;
				}
				if (m_Counter01 > 3.f) {
					Guard_Key = true;
				}
			}
			else {
				m_Counter11 += DXLib_refParts->GetDeltaTime();
				if (m_Counter11 > 2.5f) {
					m_Counter11 -= 2.5f;
					m_LMR = GetRand(100);
				}
				m_Counter12 += DXLib_refParts->GetDeltaTime();
				if (m_Counter12 > 0.2f) {
					m_Counter12 -= 0.2f;
					if (IsFarPlayer) {
						m_FMB = 0;
					}
					else if (IsNearPlayer) {
						m_FMB = 100;
					}
					else {
						m_FMB = 34 + GetRand(66);
					}
				}

				if (Target->GetCharaAction() != EnumArmAnimType::Ready && Target->GetCharaAction() != EnumArmAnimType::Run) {
					m_LMR = 50;
				}

				if (0 <= m_LMR && m_LMR <= 25) {
					A_key = true;
					D_key = false;
				}
				if (26 <= m_LMR && m_LMR <= 75) {
					A_key = false;
					D_key = false;
				}
				if (76 <= m_LMR && m_LMR <= 100) {
					A_key = false;
					D_key = true;
				}
				if (0 <= m_FMB && m_FMB <= 33) {
					W_key = true;
					S_key = false;
				}
				if (34 <= m_FMB && m_FMB <= 66) {
					W_key = false;
					S_key = false;
				}
				if (67 <= m_FMB && m_FMB <= 100) {
					W_key = false;
					S_key = true;
				}
			}

			if (IsOutArea) {
				W_key = true;
			}

			if (Chara->GetBambooVec().magnitude() > deg2rad(1)) {
				pp_x = std::clamp(-1000.f * Chara->GetBambooVec().x / deg2rad(150.f), -3.f, 3.f);
				pp_y = std::clamp(-1000.f * Chara->GetBambooVec().y / deg2rad(150.f), -3.f, 3.f);
			}
			//AI
			MyInput->SetInputStart(pp_x, pp_y);
			MyInput->SetInputPADS(PADS::MOVE_W, W_key);
			MyInput->SetInputPADS(PADS::MOVE_S, S_key);
			MyInput->SetInputPADS(PADS::MOVE_A, A_key);
			MyInput->SetInputPADS(PADS::MOVE_D, D_key);
			MyInput->SetInputPADS(PADS::SHOT, shotMain_Key);
			MyInput->SetInputPADS(PADS::ULT, shotSub_Key);
			MyInput->SetInputPADS(PADS::AIM, Guard_Key);
			MyInput->SetInputPADS(PADS::JUMP, false);
		}
	}
};
