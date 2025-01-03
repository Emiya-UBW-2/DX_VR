#include	"CPU.hpp"
#pragma warning(disable:4464)
#include	"../../MainScene/Object/Character.hpp"
#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Player {
		void AIControl::Execute(InputControl* MyInput, bool IsTutorial) noexcept {
			auto* DXLib_refParts = DXLib_ref::Instance();
			auto* PlayerMngr = Player::PlayerManager::Instance();
			auto& Chara = PlayerMngr->GetPlayer(m_MyID)->GetChara();
			auto& Target = PlayerMngr->GetPlayer(1 - this->m_MyID)->GetChara();

			bool W_key{ false };
			bool A_key{ false };
			bool S_key{ false };
			bool D_key{ false };
			bool shotMain_Key{ false };
			bool shotSub_Key{ false };
			bool Guard_Key{ false };
			bool Ya_Key{ false };
			float pp_x{ 0.f }, pp_y{ 0.f };

			Vector3DX Vec = Target->GetMove().GetPos() - Chara->GetMove().GetPos(); Vec.y = 0.f;
			float Length = Vec.magnitude();

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
				//
				m_Counter11 += DXLib_refParts->GetDeltaTime();
				if (m_Counter11 > 2.5f) {
					m_Counter11 -= 2.5f;
					m_LMR = GetRand(100);
				}
				if (Target->GetCharaAction() != EnumArmAnimType::Ready && Target->GetCharaAction() != EnumArmAnimType::Run) {
					m_LMR = 50;
				}
				//
				m_Counter12 += DXLib_refParts->GetDeltaTime();
				if (m_Counter12 > 0.2f) {
					m_Counter12 -= 0.2f;
					if (Length > 2.65f * Scale3DRate) {
						m_FMB = 0;
					}
					else if (Length < 1.0f * Scale3DRate) {
						m_FMB = 100;
					}
					else {
						m_FMB = 34 + GetRand(66);
					}
				}
				m_Counter13 += DXLib_refParts->GetDeltaTime();
				if (m_Counter13 > 1.f) {
					m_Counter13 -= 1.f;
					if ((1.0f * Scale3DRate <= Length && Length <= 2.65f * Scale3DRate) || (Target->GetYaTimerPer() >= 0.9f)) {
						Ya_Key = true;
					}
				}

				m_Counter14 += DXLib_refParts->GetDeltaTime();
				if (m_Counter14 > 0.2f) {
					m_Counter14 -= 0.2f;
					if ((1.f * Scale3DRate < Length) && (Length < 2.4f * Scale3DRate)) {
						//受け
						{
							//防御
							if ((Chara->GetCharaAction() == EnumArmAnimType::Ready) && (Target->GetCharaAction() == EnumArmAnimType::Men)) {
								Guard_Key = (GetRand(100) < 70);
							}
						}
						//仕掛け
						if (
							(Chara->GetYaTimerPer() <= 0.99f) &&
							((Chara->GetYaTimerPer() >= 0.75f) || (GetRand(100) < 10))
							) {
							//面
							if ((Chara->GetCharaAction() == EnumArmAnimType::Ready) && (Target->GetCharaAction() == EnumArmAnimType::Ready)) {
								shotMain_Key = true;
							}
							//小手
							if ((Chara->GetCharaAction() == EnumArmAnimType::Ready) && (Target->GetCharaAction() == EnumArmAnimType::GuardSuriage)) {
								shotSub_Key = true;
							}
						}
					}
					else {
						if ((Chara->GetCharaAction() == EnumArmAnimType::Tsuba) && (Target->GetCharaAction() == EnumArmAnimType::Tsuba)) {
							shotMain_Key = (GetRand(100) < 20);
						}
					}
				}
				//
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

				if (Chara->IsOutArea()) {
					W_key = true;
				}
			}

			if (Chara->GetBambooVec().magnitude() > deg2rad(1)) {
				pp_x = std::clamp(-10000.f * Chara->GetBambooVec().x / deg2rad(150.f), -9.f, 9.f);
				pp_y = std::clamp(-10000.f * Chara->GetBambooVec().y / deg2rad(150.f), -9.f, 9.f);
			}
			//AI
			MyInput->ResetAllInput();
			MyInput->SetAddxRad(pp_x);
			MyInput->SetAddyRad(pp_y);
			MyInput->SetInputPADS(Controls::PADS::MOVE_W, W_key);
			MyInput->SetInputPADS(Controls::PADS::MOVE_S, S_key);
			MyInput->SetInputPADS(Controls::PADS::MOVE_A, A_key);
			MyInput->SetInputPADS(Controls::PADS::MOVE_D, D_key);
			MyInput->SetInputPADS(Controls::PADS::SHOT, shotMain_Key);
			MyInput->SetInputPADS(Controls::PADS::ULT, shotSub_Key);
			MyInput->SetInputPADS(Controls::PADS::AIM, Guard_Key);
			MyInput->SetInputPADS(Controls::PADS::JUMP, Ya_Key);
		}
	}
}
