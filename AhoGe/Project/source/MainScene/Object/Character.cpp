#include	"Character.hpp"
#include	"../BackGround/BackGround.hpp"
#include	"../Player/Player.hpp"

#include	"../../CommonScene/Object/Object2DManager.hpp"

#include	"Bullet.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		CharacterObject::CharacterObject(void) noexcept {
			this->m_InputVec.Set(0.f, 0.f, 0.f);
		}
		CharacterObject::~CharacterObject(void) noexcept {}
		// 
		void		CharacterObject::ExecuteInput(InputControl* MyInput) noexcept {
			auto* DrawParts = DXDraw::Instance();

			// 移動速度
			float Speed = 0.75f;
			if (this->m_PlayerID != 0) {
				Speed = 0.5f;
			}
			if (MyInput->GetPADSPress(PADS::RUN)) {
				Speed = 0.95f;
			}
			if (MyInput->GetPADSPress(PADS::WALK)) {
				Speed = 0.25f;
			}
			Easing(&this->m_NormalSpeedLimit, Speed, 0.9f, EasingType::OutExpo);

			bool WKey = MyInput->GetPADSPress(PADS::MOVE_W);
			bool SKey = MyInput->GetPADSPress(PADS::MOVE_S);
			bool AKey = MyInput->GetPADSPress(PADS::MOVE_A);
			bool DKey = MyInput->GetPADSPress(PADS::MOVE_D);
			this->m_Speed = 0.f;
			// 移動演算
			if (DKey && !AKey) {
				this->m_InputVec.x = std::clamp(this->m_InputVec.x + 4.f / DrawParts->GetFps(), -this->m_NormalSpeedLimit, this->m_NormalSpeedLimit);
			}
			else if (AKey && !DKey) {
				this->m_InputVec.x = std::clamp(this->m_InputVec.x - 4.f / DrawParts->GetFps(), -this->m_NormalSpeedLimit, this->m_NormalSpeedLimit);
			}
			else {
				Easing(&this->m_InputVec.x, 0.f, 0.95f, EasingType::OutExpo);
			}
			if (WKey && !SKey) {
				this->m_InputVec.y = std::clamp(this->m_InputVec.y + 4.f / DrawParts->GetFps(), -this->m_NormalSpeedLimit, this->m_NormalSpeedLimit);
			}
			else if (SKey && !WKey) {
				this->m_InputVec.y = std::clamp(this->m_InputVec.y - 4.f / DrawParts->GetFps(), -this->m_NormalSpeedLimit, this->m_NormalSpeedLimit);
			}
			else {
				Easing(&this->m_InputVec.y, 0.f, 0.95f, EasingType::OutExpo);
			}
			this->m_Speed = std::max(std::abs(this->m_InputVec.x), std::abs(this->m_InputVec.y));
			// ローリング(移動押下中にスペース)
			if (this->m_DodgeCoolTime == 0.f) {
				if ((DKey || AKey || WKey || SKey) && MyInput->GetPADSPress(PADS::JUMP)) {
					this->m_DodgeCoolTime = 1.f;
				}
			}
			else {
				this->m_DodgeCoolTime = std::max(this->m_DodgeCoolTime - 1.f / DrawParts->GetFps(), 0.f);
				if (this->m_DodgeCoolTime > 0.75f) {
					this->m_Speed = 2.f;
				}
				else {
					this->m_Speed = 0.375f;
				}
			}
			// 
			SetVec(this->m_InputVec.normalized() * this->m_Speed);
			// ブラー
			if (this->m_DodgeCoolTime > 0.75f) {
				AddBlur(0.3f);
			}
			else if (MyInput->GetPADSPress(PADS::RUN)) {
				AddBlur(0.1f);
			}
			//弾
			if (this->m_ShotCoolTime == 0.f) {
				if (MyInput->GetPADSPress(PADS::SHOT)) {
					auto* Obj2DParts = Object2DManager::Instance();
					const auto& Obj = std::make_shared<BulletObject>();
					Obj2DParts->AddObject(Obj);
					Obj->SetObjType(static_cast<int>(Object2DType::Bullet));
					Obj->SetPlayerID(this->m_PlayerID);
					Vector3DX Vec; Vec.Set(std::sin(this->m_Rad_R), std::cos(this->m_Rad_R), 0.f);
					Obj->SetPos(GetPos() + Vec*Get2DSize(1.2f));
					Obj->SetVec(Vec * 3.f);
					Obj->SetSize(0.5f);
					this->m_ShotCoolTime = 0.1f;
				}
			}
			else {
				this->m_ShotCoolTime = std::max(this->m_ShotCoolTime - 1.f / DrawParts->GetFps(), 0.f);
			}
		}
		// 
		void CharacterObject::Init_Sub(void) noexcept {
			this->m_RunFootPer = 0.f;
		}
		void CharacterObject::Execute_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			auto* SE = SoundPool::Instance();
			auto* Cam2D = Cam2DControl::Instance();

			if (this->m_PlayerID != 0) {
				auto& Chara = PlayerMngr->GetPlayer(0)->GetChara();
				float ViewLimit = Get2DSize(40.f);
				if ((Chara->GetPos() - GetPos()).sqrMagnitude() < ViewLimit* ViewLimit) {
					Easing(&this->m_Alpha, BackGround->CheckHideShadow(Chara->GetPos(), GetPos(), Get2DSize(GetSize() / 2.f)), 0.5f, EasingType::OutExpo);
				}
				else {
					Easing(&this->m_Alpha, 0.f, 0.5f, EasingType::OutExpo);
				}
			}
			else {
				this->m_Alpha = 1.f;
			}
			//
			this->m_GraphTimer = std::max(this->m_GraphTimer - 1.f / DrawParts->GetFps(), 0.f);
			// 狙い
			{
				Vector3DX Vec;Vec.Set(std::sin(this->m_Rad_R), std::cos(this->m_Rad_R), 0.f);
				Vector3DX vec_a;vec_a.Set(std::sin(this->m_Rad), std::cos(this->m_Rad), 0.f);
				float cost = Vector3DX::Cross(vec_a, Vec).z;
				float sint = sqrtf(std::abs(1.f - cost * cost));
				this->m_Rad_R += (std::atan2f(cost, sint)) *10.f / DrawParts->GetFps();

				if (this->m_Rad_R < 0.f) { this->m_Rad_R += DX_PI_F * 2.f; }
				if (this->m_Rad_R > DX_PI_F * 2.f) { this->m_Rad_R -= DX_PI_F * 2.f; }
			}
			//サウンド
			{
				this->m_RunFootPer += GetSpeedPer() * 500.f / DrawParts->GetFps();
				if (this->m_RunFootPer > 1.f) {
					this->m_RunFootPer -= 1.f;
					float Len = Get2DSize(30.f);
					auto Length = Cam2D->GetCamPos() - GetPos(); Length.z = 0.f;
					if (Length.sqrMagnitude() < Len*Len) {
						SE->Get(static_cast<int>(SoundEnum::RunFoot)).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
				}
			}
		}
		void CharacterObject::DrawShadow_Sub(void) noexcept {
			float Radius = static_cast<float>(GetDispSize(GetSize() / 2.f));
			auto* BackGround = BackGroundClassBase::Instance();
			if (this->m_Alpha > 1.f / 255.f) {
				Vector3DX DispPos;
				Convert2DtoDisp(GetPos(), &DispPos);
				DispPos += BackGround->GetAmbientLightVec() * 0.25f;
				DrawCircle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(Radius), Black);
			}
		}
		void CharacterObject::Draw_Sub(void) noexcept {
			float Radius = static_cast<float>(GetDispSize(GetSize() / 2.f));
			//ブラー
			for (auto& b : GetBlur()) {
				if (b.IsActive()) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(16.f * this->m_Alpha * b.GetPer()), 0, 255));
					Vector3DX DispPos;
					Convert2DtoDisp(b.GetPos(), &DispPos);
					DrawCircle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(Radius * std::pow(b.GetPer(), 0.5f)), (this->m_PlayerID == 0) ? GetColor(37, 68, 141) : GetColor(92, 84, 50));
				}
			}
			//本体
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_Alpha), 0, 255));
			Vector3DX DispPos;
			Convert2DtoDisp(GetPos(), &DispPos);
			DrawCircle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(Radius), (this->m_PlayerID == 0) ? GetColor(37, 68, 141) : GetColor(92, 84, 50));

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		void CharacterObject::Dispose_Sub(void) noexcept {}
	};
};
