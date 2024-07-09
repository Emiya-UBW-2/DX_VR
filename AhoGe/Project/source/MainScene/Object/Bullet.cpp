#include	"Bullet.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../MainScene/Player/Player.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		BulletObject::BulletObject() {
		}
		BulletObject::~BulletObject() {}
		//
		void BulletObject::Execute_OnHitObject() noexcept {
			SetDelete();
		}
		void BulletObject::Execute_OnHitWall() noexcept {
			SetDelete();
		}
		// 
		void BulletObject::Init_Sub() noexcept {
			SetColTarget(ColTarget::All);
			SetHitTarget(HitTarget::HitOnly);
		}
		void BulletObject::Execute_Sub() noexcept {
			// ブラー
			AddBlur(0.5f);
		}
		void BulletObject::DrawShadow_Sub() noexcept {
			float Radius = (float)GetDispSize(GetSize() / 2.f);
			//影
			auto* BackGround = BackGroundClassBase::Instance();
			auto DispPos = Convert2DtoDisp(GetPos()) + BackGround->GetAmbientLightVec() * 0.25f;
			DrawCircle((int)DispPos.x, (int)DispPos.y, (int)Radius, Black);
		}
		void BulletObject::Draw_Sub() noexcept {
			float Radius = (float)GetDispSize(GetSize() / 2.f);
			//ブラー
			for (auto& b : GetBlur()) {
				if (b.IsActive()) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(16.f * b.GetPer()), 0, 255));
					auto DispPos = Convert2DtoDisp(b.Pos);
					DrawCircle((int)DispPos.x, (int)DispPos.y, (int)(Radius * std::pow(b.GetPer(), 0.5f)), (m_ShotPlayerID == 0) ? Yellow : Green);
				}
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			//本体
			auto DispPos = Convert2DtoDisp(GetPos());
			DrawCircle((int)DispPos.x, (int)DispPos.y, (int)Radius, (m_ShotPlayerID == 0) ? Yellow : Green);
		}
		void BulletObject::Dispose_Sub() noexcept {}
	};
};
