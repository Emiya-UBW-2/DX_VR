#include	"Character.hpp"
#include "../../MainScene/BackGround/BackGround.hpp"
#include "../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		CharacterObject::CharacterObject() {
			m_InputVec.Set(0.f, 0.f, 0.f);
		}
		CharacterObject::~CharacterObject() {}
		//
		void		CharacterObject::ExecuteInput(InputControl* MyInput) noexcept {
			auto* DrawParts = DXDraw::Instance();

			//移動速度
			float Speed = 48.f;
			if (MyInput->GetPADSPress(PADS::RUN)) {
				Speed = 96.f;
			}
			if (MyInput->GetPADSPress(PADS::WALK)) {
				Speed = 16.f;
			}
			Easing(&m_SpeedLimit, Speed, 0.9f, EasingType::OutExpo);

			bool WKey = MyInput->GetPADSPress(PADS::MOVE_W);
			bool SKey = MyInput->GetPADSPress(PADS::MOVE_S);
			bool AKey = MyInput->GetPADSPress(PADS::MOVE_A);
			bool DKey = MyInput->GetPADSPress(PADS::MOVE_D);
			m_Speed = 0.f;
			//移動演算
			if (DKey && !AKey) {
				m_InputVec.x = std::clamp(m_InputVec.x + 256.f / DrawParts->GetFps(), -m_SpeedLimit, m_SpeedLimit);
			}
			else if (AKey && !DKey) {
				m_InputVec.x = std::clamp(m_InputVec.x - 256.f / DrawParts->GetFps(), -m_SpeedLimit, m_SpeedLimit);
			}
			else {
				Easing(&m_InputVec.x, 0.f, 0.95f, EasingType::OutExpo);
			}
			if (WKey && !SKey) {
				m_InputVec.y = std::clamp(m_InputVec.y + 256.f / DrawParts->GetFps(), -m_SpeedLimit, m_SpeedLimit);
			}
			else if (SKey && !WKey) {
				m_InputVec.y = std::clamp(m_InputVec.y - 256.f / DrawParts->GetFps(), -m_SpeedLimit, m_SpeedLimit);
			}
			else {
				Easing(&m_InputVec.y, 0.f, 0.95f, EasingType::OutExpo);
			}
			m_Speed = std::max(std::abs(m_InputVec.x), std::abs(m_InputVec.y));
			//ローリング(移動押下中にスペース)
			if (m_DodgeCoolTime == 0.f) {
				if ((DKey || AKey || WKey || SKey) && MyInput->GetPADSPress(PADS::JUMP)) {
					m_DodgeCoolTime = 1.f;
				}
			}
			else {
				m_DodgeCoolTime = std::max(m_DodgeCoolTime - 1.f / DrawParts->GetFps(), 0.f);
				if (m_DodgeCoolTime > 0.75f) {
					m_Speed = 128.f;
				}
				else {
					m_Speed = 24.f;
				}
			}
			//
			SetVec(m_InputVec.normalized() * m_Speed);
			//
			for (auto& b : m_Blur) {
				b.Per = std::max(b.Per - 1.f / DrawParts->GetFps(), 0.f);
			}
			{
				float Blur = 0.f;
				if (m_DodgeCoolTime > 0.75f) {
					Blur = 0.3f;
				}
				if (MyInput->GetPADSPress(PADS::RUN)) {
					Blur = 0.1f;
				}
				if (Blur > 0.f) {
					m_Blur.at(m_BlurNow).Pos = GetPos();
					m_Blur.at(m_BlurNow).PerMax = Blur;
					m_Blur.at(m_BlurNow).Per = m_Blur.at(m_BlurNow).PerMax;
					++m_BlurNow %= (int)m_Blur.size();
				}
			}
		}
		//
		void CharacterObject::Init_Sub() noexcept {}
		void CharacterObject::Execute_Sub() noexcept {
			if (m_PlayerID != 0) {
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer(0).GetChara();
				float HalfLen = 0.5f * ((64.f * 64.f) / (1080 / 2));
				float ViewLimit = 30.f * ((64.f * 64.f) / (1080 / 2));
				if ((Chara->GetPos() - GetPos()).magnitude() < ViewLimit) {
					Easing(&m_Alpha, BackGroundClassBase::Instance()->CheckHideShadow(Chara->GetPos(), GetPos(), HalfLen), 0.5f, EasingType::OutExpo);
				}
				else {
					Easing(&m_Alpha, 0.f, 0.5f, EasingType::OutExpo);
				}
			}
			else {
				m_Alpha = 1.f;
			}

			//狙い
			{
				Vector3DX Vec;Vec.Set(std::sin(m_Rad_R), std::cos(m_Rad_R), 0.f);
				Vector3DX vec_a;vec_a.Set(std::sin(m_Rad), std::cos(m_Rad), 0.f);
				float cost = Vector3DX::Cross(vec_a, Vec).z;
				float sint = sqrtf(std::abs(1.f - cost * cost));
				m_Rad_R += (std::atan2f(cost, sint)) / 50.f;

				if (m_Rad_R < 0.f) { m_Rad_R += DX_PI_F * 2.f; }
				if (m_Rad_R > DX_PI_F * 2.f) { m_Rad_R -= DX_PI_F * 2.f; }
			}
		}
		void CharacterObject::Draw_Sub() noexcept {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			int Radius = y_r(64.f * 64.f / CamPos.z) / 2;

			for (auto& b : m_Blur) {
				if (b.Per > 0.f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(16.f * m_Alpha * (b.Per / b.PerMax)), 0, 255));
					auto DispPos = Convert2DtoDisp(b.Pos);
					DrawCircle((int)DispPos.x, (int)DispPos.y, (int)(Radius * std::pow(b.Per / b.PerMax, 0.5f)), (m_PlayerID == 0) ? GetColor(37, 68, 141) : GetColor(92, 84, 50));
				}
			}
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * m_Alpha), 0, 255));
			auto DispPos = Convert2DtoDisp(GetPos());
			DrawCircle((int)DispPos.x, (int)DispPos.y, Radius, (m_PlayerID == 0) ? GetColor(37, 68, 141) : GetColor(92, 84, 50));

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		void CharacterObject::Dispose_Sub() noexcept {}
	};
};
