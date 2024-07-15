#include	"Metal.hpp"
#include	"../BackGround/BackGround.hpp"
#include	"../Player/Player.hpp"
#include	"../../CommonScene/Object/Object2DManager.hpp"

#include	"Bullet.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		MetalObject::MetalObject(void) noexcept {
			this->m_InputVec.Set(0.f, 0.f, 0.f);
		}
		MetalObject::~MetalObject(void) noexcept {}
		//
		void MetalObject::Execute_OnHitObject(void) noexcept {
			auto* Obj2DParts = Object2DManager::Instance();
			auto* Obj = Obj2DParts->GetObj(GetHitObjectID());
			auto* SE = SoundPool::Instance();
			if (Obj) {
				Effect2DControl::Instance()->Set((*Obj)->GetPos(), EffectType::Guard, 1.f);
				//Effect2DControl::Instance()->Set((*Obj)->GetPos(), EffectType::Damage, 0.25f);
				if ((*Obj)->GetSize() >= 3.f) {
					m_HitPoint = 0;
					SE->Get(static_cast<int>(SoundEnum::Bomb)).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				else {
					m_HitPoint--;
					SE->Get(static_cast<int>(SoundEnum::Guard)).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				if (m_HitPoint == 0) {
					Effect2DControl::Instance()->Set(GetPos(), EffectType::WallHit, 2.f);
					SetDelete();
				}
			}
		}
		void MetalObject::Execute_OnHitWall(void) noexcept {
		}
		// 
		void MetalObject::Init_Sub(void) noexcept {
			SetColTarget(ColTarget::Object);
			SetHitTarget(HitTarget::HitOnly);
			m_Foot = 0.f;
			for (auto& g : m_MissileID) {
				g = 0;
			}
			m_MissileIDNum = 0;
			m_HitPoint = 1200;
		}
		void MetalObject::Execute_Sub(void) noexcept {
			if (m_IsFirstLoop) {
				m_BasePos = GetPos();
				m_PosL = GetPos();
				m_PosR = GetPos();
			}
			if (!m_CanMove) {
				SetVec(Vector3DX::zero());
				return;
			}
			auto* DrawParts = DXDraw::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* Obj2DParts = Object2DManager::Instance();

			auto& Chara = PlayerMngr->GetPlayer((PlayerID)0)->GetChara();
			if (!Chara) {
				SetVec(Vector3DX::zero());
				return;
			}
			float Length = (GetPos() - Chara->GetPos()).magnitude();

			this->m_Rad = DX_PI_F - GetRadVec2Vec(Chara->GetPos(), GetPos());
			{
				Vector3DX Vec;Vec.Set(std::sin(this->m_Rad_R), std::cos(this->m_Rad_R), 0.f);
				Vector3DX vec_a;vec_a.Set(std::sin(this->m_Rad), std::cos(this->m_Rad), 0.f);
				float cost = Vector3DX::Cross(vec_a, Vec).z;
				float sint = sqrtf(std::abs(1.f - cost * cost));
				this->m_Rad_R += (std::atan2f(cost, sint)) *0.6f / DrawParts->GetFps();

				if (this->m_Rad_R < 0.f) { this->m_Rad_R += DX_PI_F * 2.f; }
				if (this->m_Rad_R > DX_PI_F * 2.f) { this->m_Rad_R -= DX_PI_F * 2.f; }
			}
			{
				// à⁄ìÆë¨ìx
				float Speed = 0.5f;
				Easing(&this->m_NormalSpeedLimit, Speed, 0.9f, EasingType::OutExpo);

				bool WKey = false;
				bool SKey = false;
				bool AKey = false;
				bool DKey = false;

				{
					auto LastFindPos = Chara->GetPos();
					float Len = Get2DSize(0.1f);
					auto Vec = LastFindPos - GetPos();
					if (Length > Get2DSize(17.5f)) {
						WKey = (Vec.y > Len / 3.f);
						SKey = (Vec.y < -Len / 3.f);
						AKey = (Vec.x < -Len / 3.f);
						DKey = (Vec.x > Len / 3.f);
					}
					if (Length < Get2DSize(12.5f)) {
						WKey = (Vec.y < -Len / 3.f);
						SKey = (Vec.y > Len / 3.f);
						AKey = (Vec.x > Len / 3.f);
						DKey = (Vec.x < -Len / 3.f);
					}
				}
				//äOÇ©ÇÁñﬂÇÈ
				auto Vec = m_BasePos - GetPos();
				if (Vec.y < Get2DSize(-20)) {
					WKey = false;
					SKey = true;
				}
				if (Vec.y > Get2DSize(20)) {
					WKey = true;
					SKey = false;
				}
				if (Vec.x < Get2DSize(-20)) {
					AKey = true;
					DKey = false;
				}
				if (Vec.x > Get2DSize(20)) {
					AKey = false;
					DKey = true;
				}
				this->m_Speed = 0.f;
				// à⁄ìÆââéZ
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
				// 
				SetVec(this->m_InputVec.normalized() * this->m_Speed);
			}
			{
				m_Foot += 1.f / DrawParts->GetFps() / 2.f;
				if (m_Foot > 1.f) {
					m_Foot -= 1.f;
				}

				if (0.25f + 0.1f < m_Foot && m_Foot < 0.5f - 0.1f) {
					{
						Vector3DX Vec;Vec.Set(std::sin(this->m_RadL), std::cos(this->m_RadL), 0.f);
						Vector3DX vec_a;vec_a.Set(std::sin(this->m_Rad), std::cos(this->m_Rad), 0.f);
						float cost = Vector3DX::Cross(vec_a, Vec).z;
						float sint = sqrtf(std::abs(1.f - cost * cost));
						this->m_RadL += (std::atan2f(cost, sint)) *10.f / DrawParts->GetFps();

						if (this->m_RadL < 0.f) { this->m_RadL += DX_PI_F * 2.f; }
						if (this->m_RadL > DX_PI_F * 2.f) { this->m_RadL -= DX_PI_F * 2.f; }
					}
					Easing(&m_PosR, GetPos() + GetVec()*50.f, 0.5f, EasingType::OutExpo);
					m_FootDown = true;
				}
				else {
					if (m_FootDown) {
						if (Length < Get2DSize(30.f)) {
							CameraShake::Instance()->SetCamShake(0.1f, 50.f*Get2DSize(5.f) / std::max(Length, Get2DSize(1.f)));
						}
					}
					m_FootDown = false;
				}
				if (0.75f + 0.1f < m_Foot && m_Foot < 1.f - 0.1f) {
					{
						Vector3DX Vec;Vec.Set(std::sin(this->m_RadR), std::cos(this->m_RadR), 0.f);
						Vector3DX vec_a;vec_a.Set(std::sin(this->m_Rad), std::cos(this->m_Rad), 0.f);
						float cost = Vector3DX::Cross(vec_a, Vec).z;
						float sint = sqrtf(std::abs(1.f - cost * cost));
						this->m_RadR += (std::atan2f(cost, sint)) *10.f / DrawParts->GetFps();

						if (this->m_RadR < 0.f) { this->m_RadR += DX_PI_F * 2.f; }
						if (this->m_RadR > DX_PI_F * 2.f) { this->m_RadR -= DX_PI_F * 2.f; }
					}
					Easing(&m_PosL, GetPos() + GetVec()*50.f, 0.5f, EasingType::OutExpo);
					m_FootDown = true;
				}
				else {
					if (m_FootDown) {
						if (Length < Get2DSize(30.f)) {
							CameraShake::Instance()->SetCamShake(0.1f, 50.f*Get2DSize(5.f) / std::max(Length, Get2DSize(1.f)));
						}
					}
					m_FootDown = false;
				}
			}
			//
			{
				//íe
				if (this->m_ShotCoolTime == 0.f) {
					if (Get2DSize(10.f) < Length && Length < Get2DSize(20.f) && (GetRand(10) < 1)) {
						const auto& Obj = std::make_shared<BulletObject>();
						Obj2DParts->AddObject(Obj);
						Obj->SetObjType(static_cast<int>(Object2DType::Bullet));
						Obj->SetPlayerID(1);
						Vector3DX Vec; Vec.Set(std::sin(DX_PI_F - this->m_Rad), std::cos(DX_PI_F - this->m_Rad), 0.f);
						Obj->SetPos(GetPos() + Vec * Get2DSize(GetSize() / 2.f+0.75f));
						Obj->SetVec(Vec * 3.f);
						Obj->SetSize(0.5f);
						this->m_ShotCoolTime = 0.5f;
					}
					else if (Get2DSize(20.f) < Length && (GetRand(10) < 1)) {
						{
							const auto& Obj = std::make_shared<BulletObject>();
							Obj2DParts->AddObject(Obj);
							Obj->SetObjType(static_cast<int>(Object2DType::Bullet));
							Obj->SetPlayerID(1);
							Vector3DX Vec; Vec.Set(std::sin(DX_PI_F - this->m_Rad + DX_PI_F / 6), std::cos(DX_PI_F - this->m_Rad + DX_PI_F / 6), 0.f);
							Obj->SetPos(GetPos() + Vec * Get2DSize(GetSize() / 2.f + 0.75f));
							Obj->SetVec(Vec * 1.5f);
							Obj->SetSize(0.5f);

							m_MissileID.at(m_MissileIDNum) = Obj->GetObjectID();
							++m_MissileIDNum %= static_cast<int>(m_MissileID.size());
						}
						{
							const auto& Obj = std::make_shared<BulletObject>();
							Obj2DParts->AddObject(Obj);
							Obj->SetObjType(static_cast<int>(Object2DType::Bullet));
							Obj->SetPlayerID(1);
							Vector3DX Vec; Vec.Set(std::sin(DX_PI_F - this->m_Rad - DX_PI_F / 6), std::cos(DX_PI_F - this->m_Rad - DX_PI_F / 6), 0.f);
							Obj->SetPos(GetPos() + Vec * Get2DSize(GetSize() / 2.f + 0.75f));
							Obj->SetVec(Vec * 1.5f);
							Obj->SetSize(0.5f);

							m_MissileID.at(m_MissileIDNum) = Obj->GetObjectID();
							++m_MissileIDNum %= static_cast<int>(m_MissileID.size());
						}
						this->m_ShotCoolTime = 5.f;
					}
				}
				else {
					this->m_ShotCoolTime = std::max(this->m_ShotCoolTime - 1.f / DrawParts->GetFps(), 0.f);
				}
				for (auto& g : m_MissileID) {
					auto* Obj = Obj2DParts->GetObj(g);
					if (Obj) {
						Vector3DX Vec; Vec = (Chara->GetPos() - (*Obj)->GetPos()).normalized();
						Vector3DX Vec_R; Vec_R = (*Obj)->GetVec().normalized();
						if (Vector3DX::Dot(Vec, Vec_R) > 0.f) {
							Easing(&Vec_R, Vec, 0.975f, EasingType::OutExpo);
							(*Obj)->SetVec(Vec_R.normalized() * (*Obj)->GetVec().magnitude());
						}
					}
				}
				
			}
			//
		}
		void MetalObject::DrawShadow_Sub(void) noexcept {
			float Radius = static_cast<float>(GetDispSize(GetSize() / 2.f));
			//âe
			auto* BackGround = BackGroundClassBase::Instance();
			Vector3DX DispPos;
			Vector3DX DispPosB;
			Vector3DX AddPos;
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			//ãr
			AddPos.Set(Get2DSize(GetSize() / 2.f*1.2f), Get2DSize(-GetSize() / 2.f*1.2f), 0.f);
			Convert2DtoDisp(m_PosR + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_RadR)), &DispPos);
			DispPos += BackGround->GetAmbientLightVec() * 0.25f*CamPos.z;
			DispPosB = DispPos;

			AddPos.Set(Get2DSize(GetSize() / 2.f*0.8f), Get2DSize(-GetSize() / 2.f*0.2f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_RadR)), &DispPos);
			DispPos += BackGround->GetAmbientLightVec() * 0.25f*CamPos.z;
			DrawLine(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(DispPosB.x), static_cast<int>(DispPosB.y), Black, static_cast<int>(Radius*0.25f) * 2);

			AddPos.Set(-Get2DSize(GetSize() / 2.f*1.2f), Get2DSize(-GetSize() / 2.f*1.2f), 0.f);
			Convert2DtoDisp(m_PosL + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_RadL)), &DispPos);
			DispPos += BackGround->GetAmbientLightVec() * 0.25f*CamPos.z;
			DispPosB = DispPos;

			AddPos.Set(-Get2DSize(GetSize() / 2.f*0.8f), Get2DSize(-GetSize() / 2.f*0.2f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_RadL)), &DispPos);
			DispPos += BackGround->GetAmbientLightVec() * 0.25f*CamPos.z;
			DrawLine(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(DispPosB.x), static_cast<int>(DispPosB.y), Black, static_cast<int>(Radius*0.25f) * 2);
			//êgëÃ
			Convert2DtoDisp(GetPos(), &DispPos);
			DispPos += BackGround->GetAmbientLightVec() * 0.25f*CamPos.z;
			DrawCircle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(Radius), Black);
			//âEéË
			AddPos.Set(-Get2DSize(GetSize() / 2.f*0.9f), Get2DSize(-GetSize() / 2.f*2.5f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_Rad_R)), &DispPos);
			DispPos += BackGround->GetAmbientLightVec() * 0.35f*CamPos.z;
			DispPosB = DispPos;

			AddPos.Set(-Get2DSize(GetSize() / 2.f*0.9f), Get2DSize(GetSize() / 2.f*0.5f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_Rad_R)), &DispPos);
			DispPos += BackGround->GetAmbientLightVec() * 0.35f*CamPos.z;
			DrawLine(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(DispPosB.x), static_cast<int>(DispPosB.y), Black, static_cast<int>(Radius*0.15f) * 2);
			//ç∂éË
			AddPos.Set(-Get2DSize(-GetSize() / 2.f*1.2f), Get2DSize(GetSize() / 2.f*0.35f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_Rad_R)), &DispPos);
			DispPos += BackGround->GetAmbientLightVec() * 0.35f*CamPos.z;
			DispPosB = DispPos;

			AddPos.Set(-Get2DSize(-GetSize() / 2.f*1.2f), Get2DSize(-GetSize() / 2.f*0.35f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_Rad_R)), &DispPos);
			DispPos += BackGround->GetAmbientLightVec() * 0.35f*CamPos.z;
			DrawLine(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(DispPosB.x), static_cast<int>(DispPosB.y), Black, static_cast<int>(Radius*0.25f) * 2);
		}
		void MetalObject::Draw_Sub(void) noexcept {
			float Radius = static_cast<float>(GetDispSize(GetSize() / 2.f));
			Vector3DX DispPos;
			Vector3DX DispPosB;
			Vector3DX AddPos;
			//ãr
			AddPos.Set(Get2DSize(GetSize() / 2.f*1.2f), Get2DSize(-GetSize() / 2.f*1.2f), 0.f);
			Convert2DtoDisp(m_PosR + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_RadR)), &DispPos);
			DispPosB = DispPos;

			AddPos.Set(Get2DSize(GetSize() / 2.f*0.8f), Get2DSize(-GetSize() / 2.f*0.2f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_RadR)), &DispPos);
			DrawLine(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(DispPosB.x), static_cast<int>(DispPosB.y), GetColor(128, 0, 0), static_cast<int>(Radius*0.25f) * 2);

			AddPos.Set(-Get2DSize(GetSize() / 2.f*1.2f), Get2DSize(-GetSize() / 2.f*1.2f), 0.f);
			Convert2DtoDisp(m_PosL + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_RadL)), &DispPos);
			DispPosB = DispPos;

			AddPos.Set(-Get2DSize(GetSize() / 2.f*0.8f), Get2DSize(-GetSize() / 2.f*0.2f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_RadL)), &DispPos);
			DrawLine(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(DispPosB.x), static_cast<int>(DispPosB.y), GetColor(128, 0, 0), static_cast<int>(Radius*0.25f) * 2);
			//êgëÃ
			Convert2DtoDisp(GetPos(), &DispPos);
			DrawCircle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(Radius), GetColor(192, 0, 0));
			//âEéË
			AddPos.Set(-Get2DSize(GetSize() / 2.f*0.9f), Get2DSize(-GetSize() / 2.f*2.5f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_Rad_R)), &DispPos);
			DispPosB = DispPos;

			AddPos.Set(-Get2DSize(GetSize() / 2.f*0.9f), Get2DSize(GetSize() / 2.f*0.5f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_Rad_R)), &DispPos);
			DrawLine(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(DispPosB.x), static_cast<int>(DispPosB.y), GetColor(255, 0, 0), static_cast<int>(Radius*0.15f) * 2);
			//ç∂éË
			AddPos.Set(-Get2DSize(-GetSize() / 2.f*1.2f), Get2DSize(GetSize() / 2.f*0.35f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_Rad_R)), &DispPos);
			DispPosB = DispPos;

			AddPos.Set(-Get2DSize(-GetSize() / 2.f*1.2f), Get2DSize(-GetSize() / 2.f*0.35f), 0.f);
			Convert2DtoDisp(GetPos() + Matrix4x4DX::Vtrans(AddPos, Matrix4x4DX::RotAxis(Vector3DX::forward(), m_Rad_R)), &DispPos);
			DrawLine(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(DispPosB.x), static_cast<int>(DispPosB.y), GetColor(255, 0, 0), static_cast<int>(Radius*0.25f) * 2);
		}
		void MetalObject::Dispose_Sub(void) noexcept {}
	};
};
