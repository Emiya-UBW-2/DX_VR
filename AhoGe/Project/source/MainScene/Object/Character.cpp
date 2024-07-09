#include	"Character.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../MainScene/Player/Player.hpp"

#include	"../../CommonScene/Object/Object2DManager.hpp"

#include	"Bullet.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		CharacterObject::CharacterObject() {
			m_InputVec.Set(0.f, 0.f, 0.f);
		}
		CharacterObject::~CharacterObject() {}
		// 
		void		CharacterObject::ExecuteInput(InputControl* MyInput) noexcept {
			auto* DrawParts = DXDraw::Instance();

			// 移動速度
			float Speed = 0.75f;
			if (m_PlayerID != 0) {
				Speed = 0.5f;
			}
			if (MyInput->GetPADSPress(PADS::RUN)) {
				Speed = 1.25f;
			}
			if (MyInput->GetPADSPress(PADS::WALK)) {
				Speed = 0.25f;
			}
			Easing(&m_NormalSpeedLimit, Speed, 0.9f, EasingType::OutExpo);

			bool WKey = MyInput->GetPADSPress(PADS::MOVE_W);
			bool SKey = MyInput->GetPADSPress(PADS::MOVE_S);
			bool AKey = MyInput->GetPADSPress(PADS::MOVE_A);
			bool DKey = MyInput->GetPADSPress(PADS::MOVE_D);
			m_Speed = 0.f;
			// 移動演算
			if (DKey && !AKey) {
				m_InputVec.x = std::clamp(m_InputVec.x + 4.f / DrawParts->GetFps(), -m_NormalSpeedLimit, m_NormalSpeedLimit);
			}
			else if (AKey && !DKey) {
				m_InputVec.x = std::clamp(m_InputVec.x - 4.f / DrawParts->GetFps(), -m_NormalSpeedLimit, m_NormalSpeedLimit);
			}
			else {
				Easing(&m_InputVec.x, 0.f, 0.95f, EasingType::OutExpo);
			}
			if (WKey && !SKey) {
				m_InputVec.y = std::clamp(m_InputVec.y + 4.f / DrawParts->GetFps(), -m_NormalSpeedLimit, m_NormalSpeedLimit);
			}
			else if (SKey && !WKey) {
				m_InputVec.y = std::clamp(m_InputVec.y - 4.f / DrawParts->GetFps(), -m_NormalSpeedLimit, m_NormalSpeedLimit);
			}
			else {
				Easing(&m_InputVec.y, 0.f, 0.95f, EasingType::OutExpo);
			}
			m_Speed = std::max(std::abs(m_InputVec.x), std::abs(m_InputVec.y));
			// ローリング(移動押下中にスペース)
			if (m_DodgeCoolTime == 0.f) {
				if ((DKey || AKey || WKey || SKey) && MyInput->GetPADSPress(PADS::JUMP)) {
					m_DodgeCoolTime = 1.f;
				}
			}
			else {
				m_DodgeCoolTime = std::max(m_DodgeCoolTime - 1.f / DrawParts->GetFps(), 0.f);
				if (m_DodgeCoolTime > 0.75f) {
					m_Speed = 2.f;
				}
				else {
					m_Speed = 0.375f;
				}
			}
			// 
			SetVec(m_InputVec.normalized() * m_Speed);
			// ブラー
			if (m_DodgeCoolTime > 0.75f) {
				AddBlur(0.3f);
			}
			else if (MyInput->GetPADSPress(PADS::RUN)) {
				AddBlur(0.1f);
			}
			//弾
			if (m_ShotCoolTime == 0.f) {
				if (MyInput->GetPADSPress(PADS::SHOT)) {
					auto* Obj2DParts = Object2DManager::Instance();
					const auto& Obj = std::make_shared<BulletObject>();
					Obj2DParts->AddObject(Obj);
					Obj->SetObjType((int)Object2DType::Bullet);
					Obj->SetPlayerID(m_PlayerID);
					Vector3DX Vec; Vec.Set(std::sin(m_Rad_R), std::cos(m_Rad_R), 0.f);
					Obj->SetPos(GetPos() + Vec*Get2DSize(1.2f));
					Obj->SetVec(Vec * 3.f);
					Obj->SetSize(0.5f);
					m_ShotCoolTime = 0.1f;
				}
			}
			else {
				m_ShotCoolTime = std::max(m_ShotCoolTime - 1.f / DrawParts->GetFps(), 0.f);
			}
		}
		// 
		void CharacterObject::Init_Sub() noexcept {}
		void CharacterObject::Execute_Sub() noexcept {
			if (m_PlayerID != 0) {
				auto* PlayerMngr = PlayerManager::Instance();
				auto* BackGround = BackGroundClassBase::Instance();
				auto& Chara = PlayerMngr->GetPlayer(0).GetChara();
				float ViewLimit = Get2DSize(20.f);
				if ((Chara->GetPos() - GetPos()).sqrMagnitude() < ViewLimit* ViewLimit) {
					Easing(&m_Alpha, BackGround->CheckHideShadow(Chara->GetPos(), GetPos(), Get2DSize(GetSize() / 2.f)), 0.5f, EasingType::OutExpo);
				}
				else {
					Easing(&m_Alpha, 0.f, 0.5f, EasingType::OutExpo);
				}
			}
			else {
				m_Alpha = 1.f;
			}

			// 狙い
			{
				auto* DrawParts = DXDraw::Instance();

				Vector3DX Vec;Vec.Set(std::sin(m_Rad_R), std::cos(m_Rad_R), 0.f);
				Vector3DX vec_a;vec_a.Set(std::sin(m_Rad), std::cos(m_Rad), 0.f);
				float cost = Vector3DX::Cross(vec_a, Vec).z;
				float sint = sqrtf(std::abs(1.f - cost * cost));
				m_Rad_R += (std::atan2f(cost, sint)) *10.f / DrawParts->GetFps();

				if (m_Rad_R < 0.f) { m_Rad_R += DX_PI_F * 2.f; }
				if (m_Rad_R > DX_PI_F * 2.f) { m_Rad_R -= DX_PI_F * 2.f; }
			}
		}
		void CharacterObject::DrawShadow_Sub() noexcept {
			float Radius = (float)GetDispSize(GetSize() / 2.f);
			auto* BackGround = BackGroundClassBase::Instance();
			if (m_Alpha > 0.f) {
				auto DispPos = Convert2DtoDisp(GetPos()) + BackGround->GetAmbientLightVec() * 0.25f;
				DrawCircle((int)DispPos.x, (int)DispPos.y, (int)Radius, Black);
			}
		}
		void CharacterObject::Draw_Sub() noexcept {
			float Radius = (float)GetDispSize(GetSize() / 2.f);
			//ブラー
			for (auto& b : GetBlur()) {
				if (b.IsActive()) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(16.f * m_Alpha * b.GetPer()), 0, 255));
					auto DispPos = Convert2DtoDisp(b.Pos);
					DrawCircle((int)DispPos.x, (int)DispPos.y, (int)(Radius * std::pow(b.GetPer(), 0.5f)), (m_PlayerID == 0) ? GetColor(37, 68, 141) : GetColor(92, 84, 50));
				}
			}
			//本体
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * m_Alpha), 0, 255));
			auto DispPos = Convert2DtoDisp(GetPos());
			DrawCircle((int)DispPos.x, (int)DispPos.y, (int)Radius, (m_PlayerID == 0) ? GetColor(37, 68, 141) : GetColor(92, 84, 50));

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		void CharacterObject::Dispose_Sub() noexcept {}
	};
};
