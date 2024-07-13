#include	"Base2DObject.hpp"
#include	"Object2DManager.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void Base2DObject::BlurParts::Update(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			this->m_Time = std::max(this->m_Time - 1.f / DrawParts->GetFps(), 0.f);
		}
		void Base2DObject::AddBlur(float Blur) noexcept {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			auto* DrawParts = DXDraw::Instance();
			int Max = static_cast<int>(std::max(1.f, 300.f / std::max(30.f, DrawParts->GetFps())));
			Vector3DX Goal = this->m_Pos - this->m_Vec * ((Tile_DispSize*CamPos.z) / DrawParts->GetFps());
			for (int i = 0; i < Max; i++) {
				this->m_Blur.at(static_cast<size_t>(this->m_BlurNow)).Set(Lerp(Goal, this->m_Pos, (static_cast<float>(i) / static_cast<float>(Max))), Blur);
				++this->m_BlurNow %= static_cast<int>(this->m_Blur.size());
			}
		}
		void Base2DObject::Execute(void) noexcept {
			auto* Obj2DParts = Object2DManager::Instance();
			// ブラー
			for (auto& b : this->m_Blur) {
				b.Update();
			}
			//
			Execute_Sub();
			// オブジェクト判定
			switch (this->m_ColTarget) {
			case ColTarget::All:
			case ColTarget::Object:
				Obj2DParts->CheckColObject(this);
				break;
			case ColTarget::Wall:
			case ColTarget::None:
			default:
				break;
			}
			//何かに当たった
			if (this->m_HitObjectID != INVALID_ID) {
				Execute_OnHitObject();
				this->m_HitObjectID = INVALID_ID;
			}
			m_IsFirstLoop = false;
		}
		void Base2DObject::ExecuteAfter(void) noexcept {
			auto& CamPos = Cam2DControl::Instance()->GetCamPos();
			auto* DrawParts = DXDraw::Instance();
			auto* BackGround = BackGroundClassBase::Instance();
			// 衝突込みの演算
			Vector3DX Vec = this->m_Vec * ((Tile_DispSize*CamPos.z) / DrawParts->GetFps());
			// 壁判定
			switch (this->m_ColTarget) {
			case ColTarget::All:
			case ColTarget::Wall:
			{
				bool IsHit = false;
				if (this->m_HitTarget == HitTarget::Physical) {
					int Max = static_cast<int>(std::max(1.f, 60.f / std::max(30.f, DrawParts->GetFps())));
					for (int i = 0; i < Max; i++) {
						this->m_Pos += Vec * (1.f / static_cast<float>(Max));
						this->m_Pos.z = 0.f;
						IsHit |= BackGround->CheckLinetoMap(this->m_PrevPos, &this->m_Pos, Get2DSize(GetSize() / 2.f), true);
						this->m_PrevPos = this->m_Pos;
					}
				}
				else {
					this->m_Pos += Vec;
					this->m_Pos.z = 0.f;
					IsHit |= BackGround->CheckLinetoMap(this->m_PrevPos, &this->m_Pos, Get2DSize(GetSize() / 2.f), false);
					this->m_PrevPos = this->m_Pos;
				}
				if (IsHit) {
					Execute_OnHitWall();
				}
			}
				break;
			case ColTarget::Object:
			case ColTarget::None:
			default:
				this->m_Pos += Vec;
				this->m_Pos.z = 0.f;
				this->m_PrevPos = this->m_Pos;
				break;
			}
		}
	};
};
