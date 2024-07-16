#include	"Bullet.hpp"
#include	"../BackGround/BackGround.hpp"
#include	"../Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		BulletObject::BulletObject(void) noexcept {}
		BulletObject::~BulletObject(void) noexcept {}
		//
		void BulletObject::Execute_OnHitObject(void) noexcept {
			SetDelete();
		}
		void BulletObject::Execute_OnHitWall(void) noexcept {
			SetDelete();
			Effect2DControl::Instance()->Set(GetPos(), EffectType::WallHit, 0.5f);
		}
		// 
		void BulletObject::Init_Sub(void) noexcept {
			SetColTarget(ColTarget::All);
			SetHitTarget(HitTarget::HitOnly);
		}
		void BulletObject::Execute_Sub(void) noexcept {
			// ブラー
			AddBlur(0.5f);
		}
		void BulletObject::DrawShadow_Sub(void) noexcept {
			int Radius = GetDispSize(GetSize() / 2.f);
			if (!Is2DPositionInDisp(GetPos(), Radius)) { return; }
			//影
			auto* BackGround = BackGroundClassBase::Instance();
			Vector3DX DispPos;
			Convert2DtoDisp(GetPos(), &DispPos);
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();

			DispPos += BackGround->GetAmbientLightVec() * 0.25f*CamPos.z;
			DrawCircle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), Radius, Black);
		}
		void BulletObject::Draw_Sub(void) noexcept {
			int Radius = GetDispSize(GetSize() / 2.f);
			if (!Is2DPositionInDisp(GetPos(), Radius)) { return; }
			//ブラー
			for (auto& b : GetBlur()) {
				if (b.IsActive()) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(16.f * b.GetPer()), 0, 255));
					Vector3DX DispPos;
					Convert2DtoDisp(b.GetPos(), &DispPos);
					DrawCircle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), static_cast<int>(static_cast<float>(Radius) * std::pow(b.GetPer(), 0.5f)), (this->m_ShotPlayerID == 0) ? Yellow : Green);
				}
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			//本体
			Vector3DX DispPos;
			Convert2DtoDisp(GetPos(), &DispPos);
			DrawCircle(static_cast<int>(DispPos.x), static_cast<int>(DispPos.y), Radius, (this->m_ShotPlayerID == 0) ? Yellow : Green);
		}
		void BulletObject::Dispose_Sub(void) noexcept {}
	};
};
